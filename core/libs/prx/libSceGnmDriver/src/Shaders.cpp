#include "prx/libSceGnmDriver/include/Packets.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"

#include <array>
#include <cstdint>

namespace {

constexpr int SCE_GNM_ERROR_FAILURE = static_cast<int>(0x8EEE00FF);

void checkCommand(std::uint32_t* command, std::uint32_t words) {
    AgcDriverCheckGuestMemory_nid_postfix(command, words * sizeof(std::uint32_t), alignof(std::uint32_t), true);
}

void padding(std::uint32_t* command) {
    GnmDriver::WriteNop(command, 12);
}

alignas(256) constexpr std::array<std::uint32_t, 31> EmbeddedPs0{
    0xbeeb03ffu, 0x00000003u, 0x7e000280u, 0x5e000100u, 0xbf800000u, 0xf8001c0fu, 0x00000000u, 0xbf810000u,
    0x5362724fu, 0x07726468u, 0x00002043u, 0u, 0xb0a45b2bu, 0x1d39766du, 0x72044b7bu, 0x0000000fu,
    0x0fe000f0u, 0u, 0xc0000u, 4u, 0u, 4u, 2u, 2u, 0u, 0u, 0x10u, 0xfu, 0xcu, 0u, 0u,
};

alignas(256) constexpr std::array<std::uint32_t, 27> EmbeddedPs1{
    0xbeeb03ffu, 0x00000003u, 0x7e040280u, 0xf8001803u, 0x02020202u, 0xbf810000u,
    0x5362724fu, 0x07726468u, 0x00001841u, 0x04080002u, 0x98b9cb94u, 0u, 0x6f130734u, 0x0000000fu,
    0x0fe000f2u, 0u, 0x2000u, 0u, 0u, 2u, 2u, 2u, 0u, 0u, 0x10u, 3u, 0xcu,
};

alignas(256) constexpr std::array<std::uint32_t, 32> EmbeddedVs0{
    0xbeeb03ffu, 0x00000007u, 0x36020081u, 0x34020281u, 0x360000c2u, 0x4a0202c1u, 0x4a0000c1u, 0x7e020b01u,
    0x7e000b00u, 0x7e040280u, 0x7e0602f2u, 0xf80008cfu, 0x03020001u, 0xf800020fu, 0x03030303u, 0xbf810000u,
    0x5362724fu, 0x07726468u, 0x00004047u, 0u, 0x47f8c29fu, 0x9b2da5cfu, 0xff7c5b7du, 0x00000017u,
    0x0fe000f1u, 0u, 0x000c0000u, 4u, 0u, 4u, 0u, 7u,
};

std::uint32_t low(const void* code) {
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(code) >> 8u);
}

std::uint32_t high(const void* code) {
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(code) >> 40u);
}

std::uint32_t* writePixelShader(std::uint32_t* command, const std::uint32_t* registers, std::uint32_t high) {
    command = GnmDriver::WriteShRegisters(command, 8u, registers[0], high);
    command = GnmDriver::WriteShRegisters(command, 10u, registers[2], registers[3]);
    command = GnmDriver::WriteContextRegisters(command, 0x1c4u, registers[4], registers[5]);
    command = GnmDriver::WriteContextRegisters(command, 0x1b3u, registers[6], registers[7]);
    command = GnmDriver::WriteContextRegisters(command, 0x1b6u, registers[8]);
    command = GnmDriver::WriteContextRegisters(command, 0x1b8u, registers[9]);
    command = GnmDriver::WriteContextRegisters(command, 0x203u, registers[10]);
    return GnmDriver::WriteContextRegisters(command, 0x8fu, registers[11]);
}

std::uint32_t* writeVertexShader(std::uint32_t* command, const std::uint32_t* registers, std::uint32_t rsrc1, std::uint32_t high) {
    command = GnmDriver::WriteShRegisters(command, 0x48u, registers[0], high);
    command = GnmDriver::WriteShRegisters(command, 0x4au, rsrc1, registers[3]);
    command = GnmDriver::WriteContextRegisters(command, 0x207u, registers[6]);
    command = GnmDriver::WriteContextRegisters(command, 0x1b1u, registers[4]);
    return GnmDriver::WriteContextRegisters(command, 0x1c3u, registers[5]);
}

std::uint32_t modified(std::uint32_t rsrc1, std::uint32_t modifier, std::uint32_t mask) {
    return modifier == 0 ? rsrc1 : (rsrc1 & mask) | modifier;
}

}

extern "C" {

int APS5_VABI sceGnmSetCsShaderWithModifier(std::uint32_t* command, std::uint32_t words, const std::uint32_t* registers, std::uint32_t modifier) {
    if (command == nullptr || words <= 0x18 || registers == nullptr || (modifier & 0xfffffc3fu) != 0 || registers[1] != 0) return -1;
    checkCommand(command, 0x19);
    command = GnmDriver::WriteShRegisters(command, 0x20cu, registers[0], 0u);
    command = GnmDriver::WriteShRegisters(command, 0x212u, modified(registers[2], modifier, 0xfffffc3fu), registers[3]);
    command = GnmDriver::WriteShRegisters(command, 0x207u, registers[4], registers[5], registers[6]);
    padding(command);
    return 0;
}

int APS5_VABI sceGnmSetVsShader(std::uint32_t* command, std::uint32_t words, const std::uint32_t* registers, std::uint32_t modifier) {
    if (command == nullptr || words <= 0x1c || registers == nullptr || (modifier & 0xfcfffc3fu) != 0 || registers[1] != 0) return -1;
    checkCommand(command, 0x1d);
    padding(writeVertexShader(command, registers, modified(registers[2], modifier, 0xfcfffc3fu), 0u));
    return 0;
}

int APS5_VABI sceGnmUpdateVsShader(std::uint32_t* command, std::uint32_t words, const std::uint32_t* registers, std::uint32_t modifier) {
    if (command == nullptr || words <= 0x1c || registers == nullptr || (modifier & 0xfcfffc3fu) != 0 || registers[1] != 0) return -1;
    checkCommand(command, 0x1d);
    command = GnmDriver::WriteShRegisters(command, 0x48u, registers[0], 0u);
    command = GnmDriver::WriteShRegisters(command, 0x4au, modified(registers[2], modifier, 0xfcfffc3fu), registers[3]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e0207u, registers[6]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e01b1u, registers[4]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e01c3u, registers[5]);
    padding(command);
    return 0;
}

int APS5_VABI sceGnmSetPsShader350(std::uint32_t* command, std::uint32_t words, const std::uint32_t* registers) {
    if (command == nullptr || words <= 0x27) return -1;
    if (registers == nullptr) {
        checkCommand(command, 0x28);
        command = GnmDriver::WriteShRegisters(command, 8u, 0u, 0u);
        command = GnmDriver::WriteContextRegisters(command, 0x203u, 0u);
        command = GnmDriver::WriteContextRegisters(command, 0x8fu, 0xfu);
        GnmDriver::WriteNop(command, 0x1e);
        return 0;
    }
    if (registers[1] != 0) return -1;
    checkCommand(command, 0x28);
    padding(writePixelShader(command, registers, 0u));
    return 0;
}

int APS5_VABI sceGnmUpdatePsShader350(std::uint32_t* command, std::uint32_t words, const std::uint32_t* registers) {
    if (command == nullptr || words <= 0x27) return -1;
    if (registers == nullptr) {
        checkCommand(command, 0x28);
        command = GnmDriver::WriteShRegisters(command, 8u, 0u, 0u);
        command = GnmDriver::WriteNopPayload(command, 0xc01e0203u, 0u);
        command = GnmDriver::WriteContextRegisters(command, 0x8fu, 0xfu);
        GnmDriver::WriteNop(command, 0x1e);
        return 0;
    }
    if (registers[1] != 0) return -1;
    checkCommand(command, 0x28);
    command = GnmDriver::WriteShRegisters(command, 8u, registers[0], 0u);
    command = GnmDriver::WriteShRegisters(command, 10u, registers[2], registers[3]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e01c4u, registers[4], registers[5]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e01b3u, registers[6], registers[7]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e01b6u, registers[8]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e01b8u, registers[9]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e0203u, registers[10]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e008fu, registers[11]);
    padding(command);
    return 0;
}

int APS5_VABI sceGnmSetEsShader(std::uint32_t* command, std::uint32_t words, const std::uint32_t* registers, std::uint32_t modifier) {
    if (command == nullptr || words < 0x14 || registers == nullptr || (modifier & 0xfcfffc3fu) != 0 || registers[1] != 0) return -1;
    checkCommand(command, 0x14);
    command = GnmDriver::WriteShRegisters(command, 0xc8u, registers[0], 0u);
    command = GnmDriver::WriteShRegisters(command, 0xcau, modified(registers[2], modifier, 0xfcfffc3fu), registers[3]);
    padding(command);
    return 0;
}

int APS5_VABI sceGnmSetGsShader(std::uint32_t* command, std::uint32_t words, const std::uint32_t* registers) {
    if (command == nullptr || words < 0x1d || registers == nullptr || registers[1] != 0) return -1;
    checkCommand(command, 0x1d);
    command = GnmDriver::WriteShRegisters(command, 0x88u, registers[0], 0u);
    command = GnmDriver::WriteShRegisters(command, 0x8au, registers[2], registers[3]);
    command = GnmDriver::WriteContextRegisters(command, 0x2e5u, registers[4]);
    command = GnmDriver::WriteContextRegisters(command, 0x29bu, registers[5]);
    command = GnmDriver::WriteContextRegisters(command, 0x2e4u, registers[6]);
    padding(command);
    return 0;
}

int APS5_VABI sceGnmUpdateGsShader(std::uint32_t* command, std::uint32_t words, const std::uint32_t* registers) {
    if (command == nullptr || words < 0x1d || registers == nullptr || registers[1] != 0) return -1;
    checkCommand(command, 0x1d);
    command = GnmDriver::WriteShRegisters(command, 0x88u, registers[0], 0u);
    command = GnmDriver::WriteShRegisters(command, 0x8au, registers[2], registers[3]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e02e5u, registers[4]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e029bu, registers[5]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e02e4u, registers[6]);
    padding(command);
    return 0;
}

int APS5_VABI sceGnmSetHsShader(std::uint32_t* command, std::uint32_t words, const std::uint32_t* registers, std::uint32_t lsHsConfig) {
    if (command == nullptr || words < 0x1e || registers == nullptr || registers[1] != 0) return -1;
    checkCommand(command, 0x1e);
    command = GnmDriver::WriteShRegisters(command, 0x108u, registers[0], 0u);
    command = GnmDriver::WriteShRegisters(command, 0x10au, registers[2], registers[3]);
    command = GnmDriver::WriteContextRegisters(command, 0x286u, registers[5], registers[6]);
    command = GnmDriver::WriteContextRegisters(command, 0x2dbu, registers[4]);
    command = GnmDriver::WriteContextRegisters(command, 0x2d6u, lsHsConfig);
    padding(command);
    return 0;
}

int APS5_VABI sceGnmUpdateHsShader(std::uint32_t* command, std::uint32_t words, const std::uint32_t* registers, std::uint32_t lsHsConfig) {
    if (command == nullptr || words < 0x1e || registers == nullptr || registers[1] != 0) return -1;
    checkCommand(command, 0x1e);
    command = GnmDriver::WriteShRegisters(command, 0x108u, registers[0], 0u);
    command = GnmDriver::WriteShRegisters(command, 0x10au, registers[2], registers[3]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e0286u, registers[5], registers[6]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e02dbu, registers[4]);
    command = GnmDriver::WriteNopPayload(command, 0xc01e02d6u, lsHsConfig);
    padding(command);
    return 0;
}

int APS5_VABI sceGnmSetLsShader(std::uint32_t* command, std::uint32_t words, const std::uint32_t* registers, std::uint32_t modifier) {
    if (command == nullptr || words < 0x17 || registers == nullptr) return -1;
    const std::uint32_t mask = (modifier & 0xfffffc3fu) == 0 ? 0xfffffc3fu : 0xfcfffc3fu;
    if ((modifier & mask) != 0 || registers[1] != 0) return -1;
    checkCommand(command, 0x17);
    command = GnmDriver::WriteShRegisters(command, 0x148u, registers[0], 0u);
    command = GnmDriver::WriteShRegisters(command, 0x14bu, registers[3]);
    command = GnmDriver::WriteShRegisters(command, 0x14au, modified(registers[2], modifier, mask), registers[3]);
    padding(command);
    return 0;
}

int APS5_VABI sceGnmSetEmbeddedPsShader(std::uint32_t* command, std::uint32_t words, std::uint32_t shaderId, std::uint32_t modifier) {
    static_cast<void>(modifier);
    if (shaderId > 1) return SCE_GNM_ERROR_FAILURE;
    const std::uint32_t* code = shaderId == 0 ? EmbeddedPs0.data() : EmbeddedPs1.data();
    const std::array<std::uint32_t, 12> registers0{low(EmbeddedPs0.data()), 0u, 0xc0000u, 4u, 0u, 4u, 2u, 2u, 0u, 0u, 0x10u, 0xfu};
    const std::array<std::uint32_t, 12> registers1{low(EmbeddedPs1.data()), 0u, 0x2000u, 0u, 0u, 2u, 2u, 2u, 0u, 0u, 0x10u, 3u};
    if (command == nullptr || words <= 0x27) return -1;
    checkCommand(command, 0x28);
    padding(writePixelShader(command, shaderId == 0 ? registers0.data() : registers1.data(), high(code)));
    return 0;
}

int APS5_VABI sceGnmSetEmbeddedVsShader(std::uint32_t* command, std::uint32_t words, std::uint32_t shaderId, std::uint32_t modifier) {
    static_cast<void>(modifier);
    if (shaderId != 0) return SCE_GNM_ERROR_FAILURE;
    if (command == nullptr || words <= 0x1c) return -1;
    const std::array<std::uint32_t, 8> registers{low(EmbeddedVs0.data()), 0u, 0xc0000u, 4u, 0u, 4u, 0u, 7u};
    checkCommand(command, 0x1d);
    padding(writeVertexShader(command, registers.data(), registers[2], high(EmbeddedVs0.data())));
    return 0;
}

}
