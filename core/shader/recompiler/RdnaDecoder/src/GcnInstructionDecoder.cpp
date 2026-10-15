#include "RdnaDecoder/GcnInstructionDecoder.hpp"
#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <exception>
#include <optional>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <string_view>

namespace ShaderRecompiler {

namespace {

enum class GcnTarget : std::uint8_t {
    Same,         // the same encoding with the RDNA opcode
    Vop3,         // a VOP2, VOP1 or VOPC instruction RDNA only has in VOP3 encoding
    Vop3Swapped,  // a shift RDNA only has with reversed operands, in VOP3 encoding
    None,         // no RDNA instruction this decoder reads
};

struct GcnOpcode {
    std::uint32_t gcn;
    GcnTarget target;
    std::uint32_t rdna;
    const char* name;
};

#include "GcnOpcodeMap.inc"

constexpr std::uint32_t Vop3Encoding = 0x35u << 26u;
constexpr std::uint32_t SmemEncoding = 0x3du << 26u;
constexpr std::uint32_t NullScalar = 125u;
constexpr std::uint32_t VccLo = 106u;

std::atomic<bool> gcnShaderCode{false};
std::atomic<bool (*)(std::uint64_t, std::uint32_t*)> guestMemoryReader{nullptr};
thread_local const std::unordered_map<std::uint32_t, std::uint32_t>* imageDimensions = nullptr;

std::string hex(std::uint32_t value) {
    char text[11];
    std::snprintf(text, sizeof(text), "0x%08x", value);
    return text;
}

template <std::size_t Size>
const GcnOpcode& lookup(const GcnOpcode (&table)[Size], std::uint32_t opcode, const char* family, std::uint32_t programCounter) {
    const auto* entry = std::find_if(std::begin(table), std::end(table), [&](const GcnOpcode& item) { return item.gcn == opcode; });
    if (entry == std::end(table)) {
        throw std::invalid_argument(std::string("unknown GCN ") + family + " opcode " + hex(opcode) + " at program counter " + hex(programCounter));
    }
    if (entry->target == GcnTarget::None) {
        throw UnsupportedInstructionError(std::string("GCN ") + entry->name + " at program counter " + hex(programCounter) + " has no RDNA counterpart the decoder reads");
    }
    return *entry;
}

// RDNA VOP3 opcodes in the VOP3b encoding, which holds a scalar destination where VOP3a holds clamp and operand selects.
bool vop3b(std::uint32_t opcode) {
    switch (opcode) {
        case 0x128u: case 0x129u: case 0x12au:  // v_add_co_ci_u32, v_sub_co_ci_u32, v_subrev_co_ci_u32
        case 0x16du: case 0x16eu:               // v_div_scale_f32, v_div_scale_f64
        case 0x176u: case 0x177u:               // v_mad_u64_u32, v_mad_i64_i32
        case 0x30fu: case 0x310u: case 0x319u:  // v_add_co_u32, v_sub_co_u32, v_subrev_co_u32
            return true;
        default:
            return false;
    }
}

// A VOP2 instruction in RDNA VOP3 encoding. GCN v_add_i32 and its kin write their carry to VCC; v_readlane_b32 and
// v_writelane_b32 take a scalar lane select in the VSRC1 field.
std::array<std::uint32_t, 2> promoteVop2(std::uint32_t word, const GcnOpcode& entry) {
    const auto source0 = word & 0x1ffu;
    const auto vectorSource1 = (word >> 9u) & 0xffu;
    const auto destination = (word >> 17u) & 0xffu;
    const bool lane = std::string_view(entry.name).ends_with("lane_b32");
    const auto source1 = lane ? vectorSource1 : vectorSource1 + 256u;
    const auto carry = vop3b(entry.rdna) ? VccLo << 8u : 0u;
    if (entry.target == GcnTarget::Vop3Swapped) {
        return {Vop3Encoding | (entry.rdna << 16u) | destination, source1 | (source0 << 9u)};
    }
    return {Vop3Encoding | (entry.rdna << 16u) | carry | destination, source0 | (source1 << 9u)};
}

// GCN VOP3a keeps clamp at bit 11 and its opcode at bits 25:17; RDNA moves clamp to bit 15 and widens the opcode.
std::array<std::uint32_t, 2> reencodeVop3(std::uint32_t word0, std::uint32_t word1, const GcnOpcode& entry) {
    const auto fields = vop3b(entry.rdna) ? (word0 & 0x7fffu) : ((word0 & 0x7ffu) | (((word0 >> 11u) & 1u) << 15u));
    if (entry.target == GcnTarget::Vop3Swapped) {
        word1 = (word1 & ~0x3ffffu) | ((word1 & 0x1ffu) << 9u) | ((word1 >> 9u) & 0x1ffu);
    }
    return {Vop3Encoding | (entry.rdna << 16u) | fields, word1};
}

// GCN SMRD: offset 7:0, imm 8, sbase 14:9, sdst 21:15, opcode 26:22. Immediate offsets count dwords, and an offset
// field of 255 without imm takes a dword offset from the next word. RDNA SMEM takes byte offsets.
std::uint32_t smemOpcode(std::uint32_t opcode, std::uint32_t programCounter) {
    switch (opcode) {
        case 0x00u: case 0x01u: case 0x02u: case 0x03u: case 0x04u:  // s_load_dword{,x2,x4,x8,x16}
        case 0x08u: case 0x09u: case 0x0au: case 0x0bu: case 0x0cu:  // s_buffer_load_dword{,x2,x4,x8,x16}
            return opcode;
        case 0x1du: case 0x1fu: return 0x20u;  // s_dcache_inv_vol, s_dcache_inv
        case 0x1eu: return 0x24u;              // s_memtime
        default: throw std::invalid_argument("unknown GCN SMRD opcode " + hex(opcode) + " at program counter " + hex(programCounter));
    }
}

// The RDNA DIM of a GCN T# type (word 3 bits 31:28) for an image instruction that declares an array or not; a cube is
// read as the 2D array of its faces, as GCN programs address it.
std::optional<std::uint32_t> imageDimension(std::uint32_t type, bool array) {
    switch (type) {
        case 8: return array ? std::nullopt : std::optional<std::uint32_t>(0u);
        case 9: return array ? std::nullopt : std::optional<std::uint32_t>(1u);
        case 10: return array ? std::nullopt : std::optional<std::uint32_t>(2u);
        case 11: return array ? std::optional<std::uint32_t>(5u) : std::nullopt;
        case 12: return array ? std::optional<std::uint32_t>(4u) : std::nullopt;
        case 13: return array ? std::optional<std::uint32_t>(5u) : std::nullopt;
        case 14: return array ? std::nullopt : std::optional<std::uint32_t>(6u);
        case 15: return array ? std::optional<std::uint32_t>(7u) : std::nullopt;
        default: return std::nullopt;
    }
}

void requireWords(std::span<const std::uint32_t> code, std::uint32_t wordIndex, std::uint32_t count, std::uint32_t programCounter) {
    if (code.size() - wordIndex < count) {
        throw std::out_of_range("truncated GCN instruction at program counter " + hex(programCounter));
    }
}

}

void SetGcnShaderCode(bool gcn) {
    gcnShaderCode.store(gcn, std::memory_order_relaxed);
}

bool GcnShaderCode() {
    return gcnShaderCode.load(std::memory_order_relaxed);
}

void SetGcnGuestMemoryReader(bool (*read)(std::uint64_t address, std::uint32_t* value)) {
    guestMemoryReader.store(read, std::memory_order_relaxed);
}

bool ReadGcnGuestMemory(std::uint64_t address, std::uint32_t& value) {
    const auto read = guestMemoryReader.load(std::memory_order_relaxed);
    return read != nullptr && read(address, &value);
}

GcnImageDimensionScope::GcnImageDimensionScope(const std::unordered_map<std::uint32_t, std::uint32_t>& dimensions) : previous(imageDimensions) {
    imageDimensions = &dimensions;
}

GcnImageDimensionScope::~GcnImageDimensionScope() {
    imageDimensions = previous;
}

std::unordered_map<std::uint32_t, std::uint32_t> GcnImageDimensions(std::span<const std::uint32_t> code, std::span<const std::uint32_t> userData, std::uint32_t userDataBase, const std::function<bool(std::uint64_t address, std::uint32_t& value)>& read) {
    using Scalars = std::array<std::optional<std::uint32_t>, 104>;
    struct Decoded {
        std::uint32_t word;
        RdnaInstruction instruction;
    };
    std::vector<Decoded> program;
    std::unordered_map<std::uint32_t, std::size_t> indexOfWord;
    for (std::uint32_t word = 0; word < code.size();) {
        RdnaInstruction instruction;
        try {
            instruction = DecodeGcnInstruction(word * 4u, code, word);
        } catch (const std::exception&) {
            break;
        }
        indexOfWord.emplace(word, program.size());
        program.push_back({word, instruction});
        word += instruction.wordCount;
    }
    const auto scalarOf = [](const Scalars& scalars, const RdnaOperand& operand) -> std::optional<std::uint32_t> {
        switch (operand.kind) {
            case RdnaOperandKind::ScalarRegister: return operand.reg < scalars.size() ? scalars[operand.reg] : std::nullopt;
            case RdnaOperandKind::LiteralConstant: return operand.value;
            case RdnaOperandKind::IntegerInlineConstant: return static_cast<std::uint32_t>(operand.signedVal);
            case RdnaOperandKind::Null: return 0u;
            default: return std::nullopt;
        }
    };
    const auto forget = [](Scalars& scalars, const RdnaOperand& operand, std::uint32_t count) {
        if (operand.kind != RdnaOperandKind::ScalarRegister) return;
        for (auto reg = operand.reg; reg < operand.reg + count && reg < scalars.size(); ++reg) scalars[reg].reset();
    };
    // The registers an instruction leaves, from those it finds.
    const auto step = [&](Scalars& scalars, const RdnaInstruction& instruction) {
        const auto scalar = [&](const RdnaOperand& operand) { return scalarOf(scalars, operand); };
        const auto destination = instruction.destination;
        switch (instruction.op) {
            case RdnaOpcode::SLoadDword: case RdnaOpcode::SLoadDwordx2: case RdnaOpcode::SLoadDwordx4: case RdnaOpcode::SLoadDwordx8: case RdnaOpcode::SLoadDwordx16:
            case RdnaOpcode::SBufferLoadDword: case RdnaOpcode::SBufferLoadDwordx2: case RdnaOpcode::SBufferLoadDwordx4: case RdnaOpcode::SBufferLoadDwordx8: case RdnaOpcode::SBufferLoadDwordx16: {
                // s_load reads from the address in its register pair, s_buffer_load from the base of its V#.
                const auto base = instruction.source0.reg;
                const auto low = base < scalars.size() ? scalars[base] : std::nullopt;
                const auto high = base + 1u < scalars.size() ? scalars[base + 1u] : std::nullopt;
                const auto offset = scalar(instruction.source1);
                std::array<std::uint32_t, 16> values{};
                bool loaded = instruction.source0.kind == RdnaOperandKind::ScalarRegister && low && high && offset;
                const auto address = loaded ? (*low | (static_cast<std::uint64_t>(*high & 0xffffu) << 32u)) + instruction.memoryOffset + *offset : 0u;
                for (std::uint32_t index = 0; loaded && index < instruction.dataDwordCount; ++index) loaded = read(address + index * 4u, values[index]);
                forget(scalars, destination, instruction.dataDwordCount);
                if (loaded && destination.kind == RdnaOperandKind::ScalarRegister) {
                    for (std::uint32_t index = 0; index < instruction.dataDwordCount && destination.reg + index < scalars.size(); ++index) scalars[destination.reg + index] = values[index];
                }
                break;
            }
            case RdnaOpcode::SMovB32: {
                const auto value = scalar(instruction.source0);
                forget(scalars, destination, 1u);
                if (destination.kind == RdnaOperandKind::ScalarRegister && destination.reg < scalars.size()) scalars[destination.reg] = value;
                break;
            }
            case RdnaOpcode::SMovB64: {
                const auto low = scalar(instruction.source0);
                auto next = instruction.source0;
                ++next.reg;
                const auto high = instruction.source0.kind == RdnaOperandKind::ScalarRegister ? scalar(next) : instruction.source0.kind == RdnaOperandKind::IntegerInlineConstant && instruction.source0.signedVal < 0 ? std::optional<std::uint32_t>(0xffffffffu) : std::optional<std::uint32_t>(0u);
                forget(scalars, destination, 2u);
                if (destination.kind == RdnaOperandKind::ScalarRegister && destination.reg + 1u < scalars.size()) {
                    scalars[destination.reg] = low;
                    scalars[destination.reg + 1u] = low ? high : std::nullopt;
                }
                break;
            }
            case RdnaOpcode::SSwappcB64:
                // The fetch shader it calls writes registers this reading does not follow.
                scalars.fill(std::nullopt);
                break;
            default:
                forget(scalars, destination, instruction.is64Bit ? 2u : 1u);
                forget(scalars, instruction.destination2, 2u);
                break;
        }
    };
    // The registers are followed along the program's branches: an instruction starts from what every path to it
    // agrees on. s_branch and s_endpgm end their path; the conditional branches (SOPP 4-9) also fall through.
    std::vector<std::optional<Scalars>> entry(program.size());
    std::vector<std::size_t> pending;
    const auto reach = [&](std::size_t index, const Scalars& scalars) {
        auto& state = entry[index];
        bool changed = !state.has_value();
        if (changed) {
            state = scalars;
        } else {
            for (std::size_t reg = 0; reg < scalars.size(); ++reg) {
                if ((*state)[reg] && (!scalars[reg] || *scalars[reg] != *(*state)[reg])) {
                    (*state)[reg].reset();
                    changed = true;
                }
            }
        }
        if (changed) pending.push_back(index);
    };
    if (!program.empty()) {
        Scalars initial{};
        for (std::size_t index = 0; index < userData.size() && userDataBase + index < initial.size(); ++index) initial[userDataBase + index] = userData[index];
        reach(0u, initial);
    }
    while (!pending.empty()) {
        const auto index = pending.back();
        pending.pop_back();
        auto scalars = *entry[index];
        const auto& [word, instruction] = program[index];
        step(scalars, instruction);
        const auto raw = code[word];
        const bool sopp = (raw >> 23u) == 0x17fu;
        const auto opcode = (raw >> 16u) & 0x7fu;
        if (sopp && opcode >= 2u && opcode <= 9u && opcode != 3u) {
            const auto target = static_cast<std::int64_t>(word) + 1 + static_cast<std::int16_t>(raw & 0xffffu);
            if (const auto found = target >= 0 ? indexOfWord.find(static_cast<std::uint32_t>(target)) : indexOfWord.end(); found != indexOfWord.end()) reach(found->second, scalars);
        }
        const bool ends = sopp && (opcode == 1u || opcode == 2u);
        if (!ends && index + 1u < program.size()) reach(index + 1u, scalars);
    }
    std::unordered_map<std::uint32_t, std::uint32_t> dimensions;
    for (std::size_t index = 0; index < program.size(); ++index) {
        const auto word = program[index].word;
        if (!entry[index] || (code[word] >> 26u) != 0x3cu || word + 1u >= code.size()) continue;
        const auto& scalars = *entry[index];
        const auto resource = ((code[word + 1u] >> 16u) & 0x1fu) * 4u;
        const bool known = resource + 8u <= scalars.size() && std::all_of(scalars.begin() + resource, scalars.begin() + resource + 8u, [](const auto& value) { return value.has_value(); });
        if (known) {
            if (const auto dimension = imageDimension(*scalars[resource + 3u] >> 28u, ((code[word] >> 14u) & 1u) != 0u)) dimensions.emplace(word * 4u, *dimension);
        }
    }
    return dimensions;
}

std::uint32_t GcnBufferFormat(std::uint32_t dataFormat, std::uint32_t numberFormat) {
    // RDNA formats of each GCN data format, by GCN number format: unorm, snorm, uscaled, sscaled, uint, sint, -, float.
    static constexpr std::uint8_t formats[16][8] = {
        {},
        {1, 2, 3, 4, 5, 6, 0, 0},          // 8
        {7, 8, 9, 10, 11, 12, 0, 13},      // 16
        {14, 15, 16, 17, 18, 19, 0, 0},    // 8_8
        {0, 0, 0, 0, 20, 21, 0, 22},       // 32
        {23, 24, 25, 26, 27, 28, 0, 29},   // 16_16
        {30, 31, 32, 33, 34, 35, 0, 36},   // 10_11_11
        {37, 38, 39, 40, 41, 42, 0, 43},   // 11_11_10
        {44, 45, 46, 47, 48, 49, 0, 0},    // 10_10_10_2
        {50, 51, 52, 53, 54, 55, 0, 0},    // 2_10_10_10
        {56, 57, 58, 59, 60, 61, 0, 0},    // 8_8_8_8
        {0, 0, 0, 0, 62, 63, 0, 64},       // 32_32
        {65, 66, 67, 68, 69, 70, 0, 71},   // 16_16_16_16
        {0, 0, 0, 0, 72, 73, 0, 74},       // 32_32_32
        {0, 0, 0, 0, 75, 76, 0, 77},       // 32_32_32_32
        {},
    };
    const auto format = dataFormat < 16u && numberFormat < 8u ? formats[dataFormat][numberFormat] : 0u;
    if (format == 0u) {
        throw std::invalid_argument("GCN buffer data format " + std::to_string(dataFormat) + " with number format " + std::to_string(numberFormat) + " has no RDNA buffer format");
    }
    return format;
}

std::uint32_t GcnImageFormat(std::uint32_t dataFormat, std::uint32_t numberFormat) {
    constexpr std::uint32_t Unorm = 0, Snorm = 1, Float = 7, Srgb = 9;
    if (numberFormat == Srgb) {
        switch (dataFormat) {
            case 1: return 128u;   // 8
            case 3: return 129u;   // 8_8
            case 10: return 130u;  // 8_8_8_8
            case 35: return 170u;  // BC1
            case 36: return 172u;  // BC2
            case 37: return 174u;  // BC3
            case 41: return 182u;  // BC7
            default: break;
        }
    } else if (dataFormat >= 1u && dataFormat <= 14u) {
        return GcnBufferFormat(dataFormat, numberFormat);
    } else if (numberFormat == Unorm) {
        switch (dataFormat) {
            case 16: return 133u;  // 5_6_5
            case 17: return 134u;  // 1_5_5_5
            case 18: return 135u;  // 5_5_5_1
            case 19: return 136u;  // 4_4_4_4
            case 35: return 169u;  // BC1
            case 36: return 171u;  // BC2
            case 37: return 173u;  // BC3
            case 38: return 175u;  // BC4
            case 39: return 177u;  // BC5
            case 40: return 179u;  // BC6, unsigned
            case 41: return 181u;  // BC7
            default: break;
        }
    } else if (numberFormat == Snorm) {
        switch (dataFormat) {
            case 38: return 176u;  // BC4
            case 39: return 178u;  // BC5
            case 40: return 180u;  // BC6, signed
            default: break;
        }
    } else if (numberFormat == Float) {
        switch (dataFormat) {
            case 34: return 132u;  // 5_9_9_9 shared exponent
            case 40: return 179u;  // BC6
            default: break;
        }
    }
    throw std::invalid_argument("GCN image data format " + std::to_string(dataFormat) + " with number format " + std::to_string(numberFormat) + " has no RDNA image format");
}

std::array<std::uint32_t, 4> GcnBufferDescriptor(const std::array<std::uint32_t, 4>& gcn) {
    // V# word 1 keeps the address bits 39:32 (7:0) and the stride (29:16), not the cache swizzle hint (bit 30); word 3:
    // the data (18:15) and number (14:12) formats become one RDNA format (18:12), the GCN element size, ATC, hash, heap
    // and memory type bits go, and the resource level is 1.
    if ((gcn[1] >> 31u) != 0u) throw std::invalid_argument("swizzled GCN buffers are unsupported (V# " + hex(gcn[0]) + " " + hex(gcn[1]) + " " + hex(gcn[2]) + " " + hex(gcn[3]) + ")");
    const auto dataFormat = (gcn[3] >> 15u) & 0xfu;
    const auto format = dataFormat == 0u ? 0u : GcnBufferFormat(dataFormat, (gcn[3] >> 12u) & 7u);
    return {gcn[0], gcn[1] & 0x3fff00ffu, gcn[2], (gcn[3] & 0xc0e00fffu) | (format << 12u) | (1u << 24u)};
}

std::array<std::uint32_t, 8> GcnImageDescriptor(const std::array<std::uint32_t, 8>& gcn) {
    // GCN T#: address bits 45:40 and the L2 memory type (7:6) in word 1 below the minimum LOD (19:8), width and
    // height minus one in word 2, data (25:20) and number (29:26) formats in word 1, tiling index
    // in word 3, depth and pitch in word 4, base and last array slice in word 5. RDNA splits the width over words 1
    // and 2, puts the format at 28:20 of word 1, the swizzle mode where the tiling index was, the last slice and
    // base slice in word 4 and the last mip level in word 5. Word 7, the metadata address of a compressed RDNA image,
    // carries the GCN tiling index under bit 31 for the driver, which reads the image with the PS4 tiling.
    const auto width = gcn[2] & 0x3fffu;
    const auto height = (gcn[2] >> 14u) & 0x3fffu;
    const auto format = GcnImageFormat((gcn[1] >> 20u) & 0x3fu, (gcn[1] >> 26u) & 0xfu);
    // GCN programs turn cube coordinates into a face index themselves (v_cubeid), so a cube is a 2D array of its faces.
    const auto type = gcn[3] >> 28u == 11u ? 13u : gcn[3] >> 28u;
    const bool array = type == 12u || type == 13u;
    const auto depth = array ? (gcn[5] >> 13u) & 0x1fffu : gcn[4] & 0x1fffu;
    return {gcn[0], (gcn[1] & 0xfff3fu) | (format << 20u) | ((width & 3u) << 30u), (width >> 2u) | (height << 14u) | (1u << 31u), (gcn[3] & 0x000fffffu) | (type << 28u),
            depth | ((gcn[5] & 0x1fffu) << 16u), ((gcn[3] >> 16u) & 0xfu) << 4u, 0u, (1u << 31u) | ((gcn[3] >> 20u) & 0x1fu)};
}

RdnaInstruction DecodeGcnInstruction(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    if (wordIndex >= code.size()) {
        throw std::out_of_range("word index is out of the code span bounds");
    }
    const auto word = code[wordIndex];
    // The RDNA words, followed by the word after the GCN instruction for an RDNA decoder that reads a literal there.
    std::array<std::uint32_t, 4> rdna{word};
    std::uint32_t rdnaWords = 1;
    std::uint32_t gcnWords = 1;
    const GcnOpcode* compareExec = nullptr;
    std::uint32_t compareDestination = VccLo;

    if ((word & 0x80000000u) == 0u) {
        switch ((word >> 25u) & 0x3fu) {
            case 0x3eu: {
                const auto& entry = lookup(gcnVopcOpcodes, (word >> 17u) & 0xffu, "VOPC", programCounter);
                rdna[0] = (word & ~(0xffu << 17u)) | (entry.rdna << 17u);
                if (std::string_view(entry.name).starts_with("v_cmpx")) compareExec = &entry;
                break;
            }
            case 0x3fu: {
                rdna[0] = (word & ~(0xffu << 9u)) | (lookup(gcnVop1Opcodes, (word >> 9u) & 0xffu, "VOP1", programCounter).rdna << 9u);
                break;
            }
            default: {
                const auto& entry = lookup(gcnVop2Opcodes, (word >> 25u) & 0x3fu, "VOP2", programCounter);
                if (entry.target == GcnTarget::Same) {
                    rdna[0] = (word & ~(0x3fu << 25u)) | (entry.rdna << 25u);
                } else {
                    const auto promoted = promoteVop2(word, entry);
                    rdna = {promoted[0], promoted[1]};
                    rdnaWords = 2;
                }
                break;
            }
        }
    } else if ((word & 0xc0000000u) == 0x80000000u) {
        const auto opcode = (word >> 23u) & 0x7fu;
        if (opcode == 0x7du) {
            rdna[0] = (word & ~0xff00u) | (lookup(gcnSop1Opcodes, (word >> 8u) & 0xffu, "SOP1", programCounter).rdna << 8u);
        } else if (opcode == 0x7eu) {
            rdna[0] = (word & ~(0x7fu << 16u)) | (lookup(gcnSopcOpcodes, (word >> 16u) & 0x7fu, "SOPC", programCounter).rdna << 16u);
        } else if (opcode == 0x7fu) {
            rdna[0] = (word & ~(0x7fu << 16u)) | (lookup(gcnSoppOpcodes, (word >> 16u) & 0x7fu, "SOPP", programCounter).rdna << 16u);
        } else if (opcode >= 0x60u) {
            rdna[0] = (word & ~(0x1fu << 23u)) | (lookup(gcnSopkOpcodes, opcode & 0x1fu, "SOPK", programCounter).rdna << 23u);
        } else {
            rdna[0] = (word & ~(0x7fu << 23u)) | (lookup(gcnSop2Opcodes, opcode, "SOP2", programCounter).rdna << 23u);
        }
    } else if ((word >> 27u) == 0x18u) {
        const auto offset = word & 0xffu;
        const bool immediate = ((word >> 8u) & 1u) != 0u;
        const auto fields = (smemOpcode((word >> 22u) & 0x1fu, programCounter) << 18u) | (((word >> 15u) & 0x7fu) << 6u) | ((word >> 9u) & 0x3fu);
        std::uint32_t byteOffset = immediate ? offset * 4u : 0u;
        std::uint32_t scalarOffset = immediate ? NullScalar : offset;
        if (!immediate && offset == 255u) {
            requireWords(code, wordIndex, 2u, programCounter);
            if (code[wordIndex + 1u] >= 0x40000u) throw std::invalid_argument("GCN SMRD literal offset at program counter " + hex(programCounter) + " is out of RDNA SMEM range");
            byteOffset = code[wordIndex + 1u] * 4u;
            scalarOffset = NullScalar;
            gcnWords = 2;
        }
        rdna = {SmemEncoding | fields, byteOffset | (scalarOffset << 25u)};
        rdnaWords = 2;
    } else {
        switch (word >> 26u) {
            case 0x32u:  // VINTRP and EXP share the RDNA encodings
            case 0x3eu:
                break;
            case 0x34u: {
                requireWords(code, wordIndex, 2u, programCounter);
                const auto& entry = lookup(gcnVop3Opcodes, (word >> 17u) & 0x1ffu, "VOP3", programCounter);
                const auto reencoded = reencodeVop3(word, code[wordIndex + 1u], entry);
                rdna = {reencoded[0], reencoded[1]};
                rdnaWords = gcnWords = 2;
                if (entry.gcn <= 0xffu && std::string_view(entry.name).starts_with("v_cmpx")) {
                    compareExec = &entry;
                    compareDestination = word & 0xffu;
                }
                break;
            }
            case 0x36u: {
                requireWords(code, wordIndex, 2u, programCounter);
                rdna = {(word & ~(0xffu << 18u)) | (lookup(gcnDsOpcodes, (word >> 18u) & 0xffu, "DS", programCounter).rdna << 18u), code[wordIndex + 1u]};
                rdnaWords = gcnWords = 2;
                break;
            }
            case 0x38u:
            case 0x3au: {
                requireWords(code, wordIndex, 2u, programCounter);
                if (((word >> 15u) & 1u) != 0u) throw UnsupportedInstructionError("GCN buffer instruction with addr64 at program counter " + hex(programCounter));
                if ((word >> 26u) == 0x38u) {
                    rdna = {(word & ~(0x7fu << 18u)) | (lookup(gcnMubufOpcodes, (word >> 18u) & 0x7fu, "MUBUF", programCounter).rdna << 18u), code[wordIndex + 1u]};
                } else {
                    const auto opcode = lookup(gcnMtbufOpcodes, (word >> 16u) & 0x7u, "MTBUF", programCounter).rdna;
                    const auto format = GcnBufferFormat((word >> 19u) & 0xfu, (word >> 23u) & 0x7u);
                    rdna = {(word & ~(0x7fu << 19u) & ~(0x7u << 16u)) | (format << 19u) | ((opcode & 0x7u) << 16u), code[wordIndex + 1u] | (((opcode >> 3u) & 1u) << 21u)};
                }
                rdnaWords = gcnWords = 2;
                break;
            }
            case 0x3cu: {
                // GCN images take their dimension from the resource; the instruction only says whether it is an array
                // (GcnImageDimensionScope).
                requireWords(code, wordIndex, 2u, programCounter);
                const bool array = ((word >> 14u) & 1u) != 0u;
                auto dimension = array ? 5u : 1u;
                if (imageDimensions != nullptr) {
                    if (const auto found = imageDimensions->find(programCounter); found != imageDimensions->end()) dimension = found->second;
                }
                rdna = {(word & ~(1u << 14u)) | (dimension << 3u), code[wordIndex + 1u]};
                rdnaWords = gcnWords = 2;
                break;
            }
            default:
                throw std::invalid_argument("unknown GCN instruction family at program counter " + hex(programCounter) + " raw word " + hex(word));
        }
    }

    const auto base = std::min<std::size_t>(wordIndex + gcnWords, code.size());
    if (base < code.size()) rdna[rdnaWords] = code[base];
    const auto rdnaLength = rdnaWords + (base < code.size() ? 1u : 0u);
    auto instruction = DecodeRdnaEncoding(programCounter, std::span(rdna).first(rdnaLength), 0u);
    instruction.wordCount = gcnWords + (instruction.wordCount - std::min(instruction.wordCount, rdnaWords));
    SetRdnaRawWords(instruction, code, wordIndex, instruction.wordCount);
    // GCN v_cmpx writes the compare result to its scalar destination as well as to EXEC.
    if (compareExec != nullptr) {
        instruction.destination2 = DecodeRdnaScalarDestination(compareDestination, programCounter);
    }
    return instruction;
}

}
