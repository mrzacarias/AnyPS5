#include "RdnaDecoder/GcnInstructionDecoder.hpp"
#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include <cstdio>
#include <exception>
#include <string>
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

        const auto sample = decode({(0x3cu << 26u) | (0x20u << 18u) | (0xfu << 8u), 1u | (4u << 8u) | (2u << 16u) | (3u << 21u)});
        check(sample.op == RdnaOpcode::ImageSample && sample.imageDimension == RdnaImageDimension::Dim2D, "image_sample", "not a 2D sample");
        const auto arraySample = decode({(0x3cu << 26u) | (0x20u << 18u) | (1u << 14u) | (0xfu << 8u), 1u | (4u << 8u) | (2u << 16u) | (3u << 21u)});
        check(arraySample.imageDimension == RdnaImageDimension::Dim2DArray, "image_sample da", "not a 2D array sample");

        // GCN program counters: the one-word SMRD keeps the branch target at its GCN address.
        const std::vector<std::uint32_t> program{0xc2060900u, 0xbf820000u, EndProgram};
        const auto decoded = RdnaInstructionDecoder{}.Decode(program);
        check(decoded.instructions.size() == 3u && decoded.instructions[1].programCounter == 4u && decoded.instructions[1].branchTarget == 8u && decoded.instructions[2].programCounter == 8u, "program", "GCN program counters are not kept");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "decode failed: %s\n", error.what());
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
