#include "prx/libSceGnmDriver/include/Packets.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"

#include <array>
#include <cstdint>

namespace {

constexpr std::uint32_t OpcodeDispatchDirect = 0x15;
constexpr std::uint32_t OpcodeDispatchIndirect = 0x16;
constexpr std::uint32_t OpcodeDrawIndirect = 0x24;
constexpr std::uint32_t OpcodeDrawIndexIndirect = 0x25;
constexpr std::uint32_t OpcodeDrawIndex2 = 0x27;
constexpr std::uint32_t OpcodeDrawIndexAuto = 0x2d;
constexpr std::uint32_t OpcodeDrawIndexOffset2 = 0x35;
constexpr std::uint32_t AutoIndex = 2;
constexpr std::uint32_t IaMultiVgtParam = 0x2aa;
constexpr std::array<std::uint32_t, 7> IndirectUserDataBase{0u, 0u, 0x4cu, 0u, 0xccu, 0u, 0x14cu};

void checkCommand(std::uint32_t* command, std::uint32_t words) {
    AgcDriverCheckGuestMemory_nid_postfix(command, words * sizeof(std::uint32_t), alignof(std::uint32_t), true);
}

void padding(std::uint32_t* command) {
    GnmDriver::WriteNop(command, 4);
}

std::uint32_t userDataSlot(std::uint32_t slot, std::uint32_t stage) {
    return slot == 0 ? 0u : (slot & 0xffffu) + IndirectUserDataBase[stage];
}

int drawIndirect(std::uint32_t opcode, std::uint32_t* command, std::uint32_t words, std::uint32_t dataOffset, std::uint32_t stage, std::uint32_t vertexSlot, std::uint32_t instanceSlot, std::uint32_t flags, std::uint32_t initiator) {
    if (command == nullptr || words != 9 || stage >= IndirectUserDataBase.size() || vertexSlot >= 0x10 || instanceSlot >= 0x10) return -1;
    checkCommand(command, words);
    command[0] = GnmDriver::Type3Header(opcode, 4, false, (flags & 1u) != 0);
    command[1] = dataOffset;
    command[2] = userDataSlot(vertexSlot, stage);
    command[3] = userDataSlot(instanceSlot, stage);
    command[4] = initiator;
    padding(command + 5);
    return 0;
}

}

extern "C" {

int APS5_VABI sceGnmDispatchDirect(std::uint32_t* command, std::uint32_t words, std::uint32_t threadsX, std::uint32_t threadsY, std::uint32_t threadsZ, std::uint32_t flags) {
    if (command == nullptr || words != 9 || static_cast<std::int32_t>(threadsX | threadsY | threadsZ) < 0) return -1;
    checkCommand(command, words);
    command[0] = GnmDriver::Type3Header(OpcodeDispatchDirect, 4, true, (flags & 1u) != 0);
    command[1] = threadsX;
    command[2] = threadsY;
    command[3] = threadsZ;
    command[4] = (flags & 0x18u) + 1u;
    padding(command + 5);
    return 0;
}

int APS5_VABI sceGnmDispatchIndirect(std::uint32_t* command, std::uint32_t words, std::uint32_t dataOffset, std::uint32_t flags) {
    if (command == nullptr || words != 7) return -1;
    checkCommand(command, words);
    command[0] = GnmDriver::Type3Header(OpcodeDispatchIndirect, 2, true, (flags & 1u) != 0);
    command[1] = dataOffset;
    command[2] = (flags & 0x18u) + 1u;
    padding(command + 3);
    return 0;
}

int APS5_VABI sceGnmDrawIndex(std::uint32_t* command, std::uint32_t words, std::uint32_t indexCount, std::uintptr_t indexAddress, std::uint32_t flags, std::uint32_t type) {
    static_cast<void>(type);
    if (command == nullptr || words != 10 || indexAddress == 0 || (indexAddress & 1u) != 0 || (flags & 0x1ffffffeu) != 0) return -1;
    checkCommand(command, words);
    command[0] = GnmDriver::Type3Header(OpcodeDrawIndex2, 5);
    command[1] = indexCount;
    command[2] = static_cast<std::uint32_t>(indexAddress);
    command[3] = static_cast<std::uint32_t>(indexAddress >> 32u);
    command[4] = indexCount;
    command[5] = 0;
    padding(command + 6);
    return 0;
}

int APS5_VABI sceGnmDrawIndexAuto(std::uint32_t* command, std::uint32_t words, std::uint32_t indexCount, std::uint32_t flags) {
    if (command == nullptr || words != 7 || (flags & 0x1ffffffeu) != 0) return -1;
    checkCommand(command, words);
    command[0] = GnmDriver::Type3Header(OpcodeDrawIndexAuto, 2);
    command[1] = indexCount;
    command[2] = AutoIndex;
    padding(command + 3);
    return 0;
}

int APS5_VABI sceGnmDrawIndexOffset(std::uint32_t* command, std::uint32_t words, std::uint32_t indexOffset, std::uint32_t indexCount, std::uint32_t flags) {
    if (command == nullptr || words != 9) return -1;
    checkCommand(command, words);
    command[0] = GnmDriver::Type3Header(OpcodeDrawIndexOffset2, 4, false, (flags & 1u) != 0);
    command[1] = indexCount;
    command[2] = indexOffset;
    command[3] = indexCount;
    command[4] = 0;
    padding(command + 5);
    return 0;
}

int APS5_VABI sceGnmDrawIndirect(std::uint32_t* command, std::uint32_t words, std::uint32_t dataOffset, std::uint32_t stage, std::uint32_t vertexSlot, std::uint32_t instanceSlot, std::uint32_t flags) {
    return drawIndirect(OpcodeDrawIndirect, command, words, dataOffset, stage, vertexSlot, instanceSlot, flags, AutoIndex);
}

int APS5_VABI sceGnmDrawIndexIndirect(std::uint32_t* command, std::uint32_t words, std::uint32_t dataOffset, std::uint32_t stage, std::uint32_t vertexSlot, std::uint32_t instanceSlot, std::uint32_t flags) {
    return drawIndirect(OpcodeDrawIndexIndirect, command, words, dataOffset, stage, vertexSlot, instanceSlot, flags, 0);
}

int APS5_VABI sceGnmSetVgtControl(std::uint32_t* command, std::uint32_t words, std::uint32_t primitiveGroupSizeMinusOne, std::uint32_t partialVsWaveMode, std::uint32_t switchOnlyOnEndOfPacket) {
    if (command == nullptr || words != 3 || primitiveGroupSizeMinusOne >= 0x100 || (switchOnlyOnEndOfPacket | partialVsWaveMode) >= 2) return -1;
    checkCommand(command, words);
    GnmDriver::WriteContextRegisters(command, IaMultiVgtParam, (partialVsWaveMode & 1u) << 16u | primitiveGroupSizeMinusOne);
    return 0;
}

int APS5_VABI sceGnmResetVgtControl(std::uint32_t* command, std::uint32_t words) {
    if (command == nullptr || words != 3) return -1;
    checkCommand(command, words);
    GnmDriver::WriteContextRegisters(command, IaMultiVgtParam, 0xffu);
    return 0;
}

}
