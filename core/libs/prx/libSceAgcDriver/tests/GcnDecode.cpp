#include "ControlFlow/GraphBuilder.hpp"
#include "RdnaDecoder/GcnInstructionDecoder.hpp"
#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include <array>
#include <cstdio>
#include <exception>
#include <string>
#include <utility>
#include <vector>

using namespace ShaderRecompiler;

namespace {

constexpr std::uint32_t EndProgram = 0xbf810000u;

constexpr std::uint32_t smrd(std::uint32_t op, std::uint32_t sdst, std::uint32_t sbase, bool immediate, std::uint32_t offset) {
    return (0x18u << 27u) | (op << 22u) | (sdst << 15u) | ((sbase / 2u) << 9u) | ((immediate ? 1u : 0u) << 8u) | offset;
}

constexpr std::uint32_t vop2(std::uint32_t op, std::uint32_t vdst, std::uint32_t vsrc1, std::uint32_t src0) {
    return (op << 25u) | (vdst << 17u) | (vsrc1 << 9u) | src0;
}

constexpr std::uint32_t vopc(std::uint32_t op, std::uint32_t vsrc1, std::uint32_t src0) {
    return (0x3eu << 25u) | (op << 17u) | (vsrc1 << 9u) | src0;
}

constexpr std::uint32_t vop3(std::uint32_t op, std::uint32_t vdst, bool clamp) {
    return (0x34u << 26u) | (op << 17u) | ((clamp ? 1u : 0u) << 11u) | vdst;
}

constexpr std::uint32_t sources(std::uint32_t src0, std::uint32_t src1, std::uint32_t src2) {
    return src0 | (src1 << 9u) | (src2 << 18u);
}

constexpr std::uint32_t Vgpr = 256u;

int failures = 0;

void check(bool condition, const char* name, const std::string& what) {
    if (!condition) {
        std::fprintf(stderr, "%s: %s\n", name, what.c_str());
        ++failures;
    }
}

RdnaInstruction decode(std::vector<std::uint32_t> words) {
    words.push_back(EndProgram);
    return DecodeRdnaInstruction(0u, words, 0u);
}

bool scalar(const RdnaOperand& operand, std::uint32_t reg) {
    return operand.kind == RdnaOperandKind::ScalarRegister && operand.reg == reg;
}

bool vector(const RdnaOperand& operand, std::uint32_t reg) {
    return operand.kind == RdnaOperandKind::VectorRegister && operand.reg == reg;
}

}

int main() {
    SetGcnShaderCode(true);
    try {
        // The first word of Inscryption's first pixel shader: s_buffer_load_dword s12, s[8:11], 0x0.
        auto load = decode({0xc2060900u});
        check(load.op == RdnaOpcode::SBufferLoadDword && load.wordCount == 1u && scalar(load.destination, 12u) && scalar(load.source0, 8u) && load.memoryOffset == 0u, "s_buffer_load_dword", "wrong opcode, length or operands");
        load = decode({smrd(0x02u, 4u, 6u, true, 3u)});
        check(load.op == RdnaOpcode::SLoadDwordx4 && load.memoryOffset == 12u && load.source1.kind == RdnaOperandKind::Null, "s_load_dwordx4 immediate", "dword offset 3 is not byte offset 12");
        load = decode({smrd(0x08u, 4u, 6u, false, 2u)});
        check(load.memoryOffset == 0u && scalar(load.source1, 2u), "s_buffer_load_dword sgpr", "offset register s2 is lost");
        load = decode({smrd(0x09u, 4u, 6u, false, 255u), 0x10u});
        check(load.op == RdnaOpcode::SBufferLoadDwordx2 && load.wordCount == 2u && load.memoryOffset == 0x40u, "s_buffer_load_dwordx2 literal", "literal dword offset 0x10 is not two words and byte offset 0x40");

        const auto add = decode({vop2(0x25u, 1u, 3u, Vgpr + 2u)});
        check(add.op == RdnaOpcode::VAddI32 && add.wordCount == 1u && vector(add.destination, 1u) && add.destination2.kind == RdnaOperandKind::VccLo, "v_add_i32", "not v_add_co_u32 with its carry in vcc");
        const auto shift = decode({vop2(0x15u, 1u, 2u, 0u)});
        check(shift.op == RdnaOpcode::VLshrrevB32 && vector(shift.source0, 2u) && scalar(shift.source1, 0u), "v_lshr_b32", "not v_lshrrev_b32 with swapped operands");
        const auto select = decode({vop2(0x00u, 1u, 2u, Vgpr + 3u)});
        check(select.op == RdnaOpcode::VCndmaskB32, "v_cndmask_b32", "GCN VOP2 opcode 0 is not v_cndmask_b32");
        const auto madmk = decode({vop2(0x20u, 1u, 2u, Vgpr + 3u), 0x3f800000u});
        check(madmk.op == RdnaOpcode::VMadmkF32 && madmk.wordCount == 2u, "v_madmk_f32", "literal constant is not part of the instruction");
        const auto readlane = decode({vop2(0x01u, 5u, 4u, Vgpr + 3u)});
        check(readlane.op == RdnaOpcode::VReadlaneB32 && scalar(readlane.destination, 5u) && vector(readlane.source0, 3u) && scalar(readlane.source1, 4u), "v_readlane_b32", "lane select is not scalar s4");

        const auto mad = decode({vop3(0x141u, 4u, false), sources(Vgpr + 1u, Vgpr + 2u, Vgpr + 3u)});
        check(mad.op == RdnaOpcode::VMadF32 && mad.wordCount == 2u && vector(mad.source2, 3u), "v_mad_f32", "wrong opcode, length or third operand");
        const auto clamped = decode({vop3(0x103u, 4u, true), sources(Vgpr + 1u, Vgpr + 2u, 0u)});
        check(clamped.op == RdnaOpcode::VAddF32 && clamped.destination.clamp, "v_add_f32 clamp", "clamp bit is lost");
        const auto shift64 = decode({vop3(0x162u, 4u, false), sources(Vgpr + 2u, 129u, 0u)});
        check(shift64.op == RdnaOpcode::VLshrrevB64 && shift64.source0.kind == RdnaOperandKind::IntegerInlineConstant && vector(shift64.source1, 2u), "v_lshr_b64", "not v_lshrrev_b64 with swapped operands");
        const auto carry = decode({(0x34u << 26u) | (0x125u << 17u) | (20u << 8u) | 4u, sources(Vgpr + 1u, Vgpr + 2u, 0u)});
        check(carry.op == RdnaOpcode::VAddI32 && scalar(carry.destination2, 20u), "v_add_i32 vop3", "carry destination s[20:21] is lost");

        const auto kill = decode({vopc(0x14u, 2u, 128u)});
        check(kill.op == RdnaOpcode::VCmpxGtF32 && kill.destination.kind == RdnaOperandKind::ExecLo && kill.destination2.kind == RdnaOperandKind::VccLo, "v_cmpx_gt_f32", "GCN v_cmpx does not also write vcc");
        const auto killScalar = decode({vop3(0x14u, 10u, false), sources(128u, Vgpr + 2u, 0u)});
        check(killScalar.op == RdnaOpcode::VCmpxGtF32 && scalar(killScalar.destination2, 10u), "v_cmpx_gt_f32 vop3", "GCN v_cmpx does not also write s[10:11]");
        const auto signaling = decode({vopc(0x41u, 2u, Vgpr + 1u)});
        check(signaling.op == RdnaOpcode::VCmpLtF32 && signaling.destination2.kind == RdnaOperandKind::Unknown, "v_cmps_lt_f32", "not v_cmp_lt_f32");

        const auto typed = decode({(0x3au << 26u) | (7u << 23u) | (14u << 19u) | (3u << 16u), 1u | (2u << 8u) | (1u << 16u) | (128u << 24u)});
        check(typed.op == RdnaOpcode::TbufferLoadFormatXyzw && ((typed.numberFormat << 4u) | typed.dataFormat) == 77u, "tbuffer_load_format_xyzw", "32_32_32_32 float is not RDNA buffer format 77");
        check(GcnBufferFormat(6u, 7u) == 36u && GcnBufferFormat(10u, 0u) == 56u && GcnBufferFormat(9u, 4u) == 54u, "GcnBufferFormat", "wrong RDNA buffer formats");
        check(GcnImageFormat(10u, 9u) == 130u && GcnImageFormat(35u, 0u) == 169u && GcnImageFormat(37u, 9u) == 174u && GcnImageFormat(16u, 0u) == 133u, "GcnImageFormat", "wrong RDNA image formats");
        // Inscryption's 960x540 16_16_16_16 float target as a T#: the L2 memory type in word 1 is no address bit.
        const auto image = GcnImageDescriptor({0x03b39c00u, 0xdcc00040u, 0x7086c3bfu, 0x90e00facu, 0u, 0u, 0u, 0u});
        check(image == std::array<std::uint32_t, 8>{0x03b39c00u, 0xc4700000u, 0x8086c0efu, 0x90000facu, 0u, 0u, 0u, 0x8000000eu}, "GcnImageDescriptor", "960x540 T# has the wrong RDNA words or loses tiling index 14");
        const auto cube = GcnImageDescriptor({0x1000u, 0x02800000u, 0u, 0xb0000facu, 0u, 5u << 13u, 0u, 0u});
        check((cube[3] >> 28u) == 13u && (cube[4] & 0x1fffu) == 5u, "GcnImageDescriptor cube", "a cube is not a 2D array of six faces");
        // A V#: 40 address bits, stride 20, 32_32 float; word 1's padding and cache swizzle hint are no address bits.
        const auto buffer = GcnBufferDescriptor({0x56a2da1cu, 0x4014ff03u, 4u, 0x2005f22cu});
        check(buffer == std::array<std::uint32_t, 4>{0x56a2da1cu, 0x00140003u, 4u, 0x0104022cu | (64u << 12u)}, "GcnBufferDescriptor", "V# has the wrong RDNA words");

        const auto sample = decode({(0x3cu << 26u) | (0x20u << 18u) | (0xfu << 8u), 1u | (4u << 8u) | (2u << 16u) | (3u << 21u)});
        check(sample.op == RdnaOpcode::ImageSample && sample.imageDimension == RdnaImageDimension::Dim2D, "image_sample", "not a 2D sample");
        const auto arraySample = decode({(0x3cu << 26u) | (0x20u << 18u) | (1u << 14u) | (0xfu << 8u), 1u | (4u << 8u) | (2u << 16u) | (3u << 21u)});
        check(arraySample.imageDimension == RdnaImageDimension::Dim2DArray, "image_sample da", "not a 2D array sample");

        // GCN image instructions read the dimension of their T#: a 3D T# in user data s[8:15], a 2D array T# that
        // s_load_dwordx8 brings to s[16:23] from the table s[0:1] points at, and in s[24:31] one the scan cannot know.
        {
            const auto mimg = [](bool array, std::uint32_t resource) { return std::array<std::uint32_t, 2>{(0x3cu << 26u) | (0x20u << 18u) | ((array ? 1u : 0u) << 14u) | (0xfu << 8u), 1u | (4u << 8u) | ((resource / 4u) << 16u) | (3u << 21u)}; };
            std::vector<std::uint32_t> code{smrd(3u, 16u, 0u, true, 0u)};
            for (const auto& [array, resource] : {std::pair{false, 8u}, std::pair{true, 16u}, std::pair{false, 24u}}) {
                const auto words = mimg(array, resource);
                code.insert(code.end(), words.begin(), words.end());
            }
            code.push_back(EndProgram);
            std::vector<std::uint32_t> userData(16u, 0u);
            userData[0] = 0x1000u;
            userData[1] = 1u;
            userData[11] = 0xa0000facu;
            const auto read = [](std::uint64_t address, std::uint32_t& value) {
                if (address < 0x100001000ull || address >= 0x100001020ull) return false;
                value = address == 0x10000100cull ? 0xd0000facu : 0u;
                return true;
            };
            const auto dimensions = GcnImageDimensions(code, userData, 0u, read);
            check(dimensions.size() == 2u && dimensions.contains(4u) && dimensions.at(4u) == 2u && dimensions.contains(12u) && dimensions.at(12u) == 5u, "GcnImageDimensions", "the 3D and the loaded 2D array T# do not give their instructions their dimensions, or the unknown one does");
            const GcnImageDimensionScope scope(dimensions);
            check(DecodeRdnaInstruction(4u, code, 1u).imageDimension == RdnaImageDimension::Dim3D && DecodeRdnaInstruction(20u, code, 5u).imageDimension == RdnaImageDimension::Dim2D, "GcnImageDimensionScope", "the 3D sample is not 3D, or the unknown one is not 2D");
        }

        // The scan follows branches (Inscryption's probe-volume pixel shader): the 3D T# in user data s[0:7] reaches
        // the sample at 24 only through the branch that skips the s_mov making it 2D; at 32 the paths disagree.
        {
            const std::array<std::uint32_t, 2> sample{(0x3cu << 26u) | (0x20u << 18u) | (0xfu << 8u), 1u | (4u << 8u) | (3u << 21u)};
            const std::vector<std::uint32_t> code{sample[0], sample[1], 0xbf860003u, 0xbe8303ffu, 0x90000facu, 0xbf820002u, sample[0], sample[1], sample[0], sample[1], EndProgram};
            std::vector<std::uint32_t> userData(16u, 0u);
            userData[3] = 0xa0000facu;
            const auto dimensions = GcnImageDimensions(code, userData, 0u, [](std::uint64_t, std::uint32_t&) { return false; });
            check(dimensions.size() == 2u && dimensions.contains(0u) && dimensions.at(0u) == 2u && dimensions.contains(24u) && dimensions.at(24u) == 2u, "GcnImageDimensions branches", "a T# was not followed along the branch past the code that replaces it, or the join of disagreeing paths kept one");
        }

        // GCN program counters: the one-word SMRD keeps the branch target at its GCN address.
        const std::vector<std::uint32_t> program{0xc2060900u, 0xbf820000u, EndProgram};
        const auto decoded = RdnaInstructionDecoder{}.Decode(program);
        check(decoded.instructions.size() == 3u && decoded.instructions[1].programCounter == 4u && decoded.instructions[1].branchTarget == 8u && decoded.instructions[2].programCounter == 8u, "program", "GCN program counters are not kept");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "decode failed: %s\n", error.what());
        ++failures;
    }

    try {
        // Inscryption's vertex program entry: markers, then the fetch-shader call through user data s[0:1] that also
        // links into s[0:1], then a scalar load that overwrites the link.
        const std::vector<std::uint32_t> entry{0xbeeb03ffu, 0x22u, 0xbefc03ffu, 0xe8188d42u, 0xbf960000u, 0xbe802100u, 0xc2800508u, 0xbf8c007fu, EndProgram};
        const auto decoded = RdnaInstructionDecoder{}.Decode(entry);
        const SwappcInfo swappc{true, 0u, 16u};
        check(GraphBuilder{}.Build(decoded, &swappc).hasFetchCall, "PS4 fetch call", "the s_swappc_b64 is not a fetch-shader call");
        // Another entry rewrites only s0 after the call and never reads the pair.
        const std::vector<std::uint32_t> half{0xbeeb03ffu, 0x23u, 0xbefc03ffu, 0x93d1aa12u, 0xbf960000u, 0xbe802100u, 0x080012f2u, 0xc200050du, 0xbf8c007fu, EndProgram};
        check(GraphBuilder{}.Build(RdnaInstructionDecoder{}.Decode(half), &swappc).hasFetchCall, "PS4 fetch call, half rewritten", "the s_swappc_b64 is not a fetch-shader call");
        // The fetch shader returns by itself; the program must not return through the link.
        const std::vector<std::uint32_t> read{0xbe802100u, 0xbe802000u, EndProgram};
        bool refused = false;
        try {
            static_cast<void>(GraphBuilder{}.Build(RdnaInstructionDecoder{}.Decode(read), &swappc));
        } catch (const std::invalid_argument&) {
            refused = true;
        }
        check(refused, "PS4 fetch call, link return", "a return through the link register after the call is accepted");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "PS4 fetch call: %s\n", error.what());
        ++failures;
    }

    try {
        static_cast<void>(decode({(0x3fu << 25u) | (1u << 17u) | (0x29u << 9u) | (Vgpr + 2u)}));
        check(false, "v_rcp_legacy_f32", "decoded without an RDNA counterpart");
    } catch (const UnsupportedInstructionError& error) {
        check(std::string(error.what()).find("v_rcp_legacy_f32") != std::string::npos, "v_rcp_legacy_f32", std::string("refusal does not name it: ") + error.what());
    }

    SetGcnShaderCode(false);
    check(DecodeRdnaInstruction(0u, std::vector<std::uint32_t>{vop2(0x01u, 1u, 2u, Vgpr + 3u)}, 0u).op == RdnaOpcode::VCndmaskB32, "rdna", "RDNA VOP2 opcode 1 is not v_cndmask_b32 once GCN is off");
    return failures == 0 ? 0 : 1;
}
