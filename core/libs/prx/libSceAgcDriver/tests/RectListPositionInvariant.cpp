#include "Recompiler.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <map>
#include <set>
#include <span>
#include <spirv/unified1/spirv.hpp>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using namespace ShaderRecompiler;

void Check(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

bool PositionIsInvariant(std::span<const std::uint32_t> words) {
    Check(words.size() >= 5 && words[0] == spv::MagicNumber, "rect-list invariant: missing SPIR-V module");
    std::vector<std::pair<std::uint32_t, std::uint32_t>> positionMembers;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> invariantMembers;
    for (std::size_t offset = 5; offset < words.size();) {
        const auto count = words[offset] >> spv::WordCountShift;
        Check(count != 0 && count <= words.size() - offset, "rect-list invariant: malformed SPIR-V instruction");
        const auto opcode = words[offset] & spv::OpCodeMask;
        if (opcode == spv::OpMemberDecorate && count == 5 && words[offset + 3] == spv::DecorationBuiltIn && words[offset + 4] == spv::BuiltInPosition) {
            positionMembers.emplace_back(words[offset + 1], words[offset + 2]);
        }
        if (opcode == spv::OpMemberDecorate && count == 4 && words[offset + 3] == spv::DecorationInvariant) {
            invariantMembers.emplace_back(words[offset + 1], words[offset + 2]);
        }
        offset += count;
    }
    Check(!positionMembers.empty(), "rect-list invariant: shader has no gl_Position");
    for (const auto& member : positionMembers) {
        if (std::find(invariantMembers.begin(), invariantMembers.end(), member) == invariantMembers.end()) return false;
    }
    return true;
}

// The array lengths of the `storage` variable decorated with `builtin`, outermost first (empty without one).
std::vector<std::uint32_t> BuiltinArrayLengths(std::span<const std::uint32_t> words, spv::BuiltIn builtin, spv::StorageClass storage) {
    std::map<std::uint32_t, std::uint32_t> constants;
    std::map<std::uint32_t, std::pair<std::uint32_t, std::uint32_t>> arrays;
    std::map<std::uint32_t, std::uint32_t> pointers;
    std::map<std::uint32_t, std::uint32_t> variables;
    std::set<std::uint32_t> decorated;
    for (std::size_t offset = 5; offset < words.size();) {
        const auto count = words[offset] >> spv::WordCountShift;
        Check(count != 0 && count <= words.size() - offset, "rect-list distances: malformed SPIR-V instruction");
        const auto* word = words.data() + offset;
        const auto opcode = word[0] & spv::OpCodeMask;
        if (opcode == spv::OpDecorate && count == 4 && word[2] == spv::DecorationBuiltIn && word[3] == builtin) decorated.insert(word[1]);
        if (opcode == spv::OpConstant && count == 4) constants[word[2]] = word[3];
        if (opcode == spv::OpTypeArray && count == 4) arrays[word[1]] = {word[2], word[3]};
        if (opcode == spv::OpTypePointer && count == 4 && word[2] == storage) pointers[word[1]] = word[3];
        if (opcode == spv::OpVariable && count >= 4 && word[3] == storage) variables[word[2]] = word[1];
        offset += count;
    }
    std::vector<std::uint32_t> lengths;
    const auto variable = std::find_if(variables.begin(), variables.end(), [&](const auto& entry) { return decorated.contains(entry.first); });
    if (variable == variables.end() || !pointers.contains(variable->second)) return lengths;
    for (auto type = arrays.find(pointers.at(variable->second)); type != arrays.end(); type = arrays.find(type->second.first)) lengths.push_back(constants.at(type->second.second));
    return lengths;
}

// A vertex module with only `float gl_ClipDistance[2]` among its outputs: the rect-list builder reads no more of it.
std::vector<std::uint32_t> VertexWithClipDistances() {
    return {
        spv::MagicNumber, 0x00010400u, 0u, 7u, 0u,
        (4u << spv::WordCountShift) | spv::OpDecorate, 5u, spv::DecorationBuiltIn, spv::BuiltInClipDistance,
        (3u << spv::WordCountShift) | spv::OpTypeFloat, 1u, 32u,
        (4u << spv::WordCountShift) | spv::OpTypeInt, 2u, 32u, 0u,
        (4u << spv::WordCountShift) | spv::OpConstant, 2u, 3u, 2u,
        (4u << spv::WordCountShift) | spv::OpTypeArray, 4u, 1u, 3u,
        (4u << spv::WordCountShift) | spv::OpTypePointer, 6u, spv::StorageClassOutput, 4u,
        (4u << spv::WordCountShift) | spv::OpVariable, 6u, 5u, spv::StorageClassOutput,
    };
}

}

int main() {
    try {
        static constexpr std::array<std::uint32_t, 2> capabilities{spv::CapabilityShader, spv::CapabilityTessellation};
        SpirvTarget target{};
        target.vulkanVersion = 0x00401000u;
        target.spirvVersion = 0x00010400u;
        target.supportedCapabilities = capabilities;
        target.tessellation = TessellationTargetLimits{32, 128, 128, 120, 4096, 128, 128};
        RecompileResult vertex;
        RecompileResult fragment;
        const auto shaders = BuildRectListShaders(vertex, fragment, target);
        Check(PositionIsInvariant(shaders.control.spirv.Words()), "rect-list control gl_Position is not Invariant");
        Check(PositionIsInvariant(shaders.evaluation.spirv.Words()), "rect-list evaluation gl_Position is not Invariant");
        Check(BuiltinArrayLengths(shaders.control.spirv.Words(), spv::BuiltInClipDistance, spv::StorageClassInput).empty(), "rect-list control takes clip distances its vertex stage has not");
        // The stages take the vertex stage's clip distances (MoltenVK lays the control stage's inputs
        // out from every vertex output) and pass them on to the rasterizer as they pass the position.
        static constexpr std::array<std::uint32_t, 3> clipCapabilities{spv::CapabilityShader, spv::CapabilityTessellation, spv::CapabilityClipDistance};
        target.supportedCapabilities = clipCapabilities;
        vertex.spirv = VertexWithClipDistances();
        const auto clipped = BuildRectListShaders(vertex, fragment, target);
        using Lengths = std::vector<std::uint32_t>;
        Check(BuiltinArrayLengths(clipped.control.spirv.Words(), spv::BuiltInClipDistance, spv::StorageClassInput) == Lengths{3u, 2u}, "rect-list control does not take the vertex clip distances");
        Check(BuiltinArrayLengths(clipped.control.spirv.Words(), spv::BuiltInClipDistance, spv::StorageClassOutput) == Lengths{4u, 2u}, "rect-list control does not pass the clip distances on");
        Check(BuiltinArrayLengths(clipped.evaluation.spirv.Words(), spv::BuiltInClipDistance, spv::StorageClassInput) == Lengths{4u, 2u}, "rect-list evaluation does not take the clip distances");
        Check(BuiltinArrayLengths(clipped.evaluation.spirv.Words(), spv::BuiltInClipDistance, spv::StorageClassOutput) == Lengths{2u}, "rect-list evaluation does not clip with the clip distances");
        std::printf("rect-list position invariant tests passed\n");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
