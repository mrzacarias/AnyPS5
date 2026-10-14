#include "prx/libSceGnmDriver/include/Packets.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"

#include <array>
#include <cstdint>
#include <cstring>

namespace {

constexpr std::uint32_t HardwareInitWords = 0x100;
constexpr std::uint32_t ContextInitWords = 0x100;

constexpr std::array<std::uint32_t, 12> ClearContextState{
    0xc0012800u, 0x80000000u, 0x80000000u, 0xc0001200u, 0u, 0xc0055800u,
    0x2ec47fc0u, 0xffffffffu, 0u, 0u, 0u, 10u,
};

constexpr std::array<std::uint32_t, 0x7c> InitSequence350{
    0xc0017600u, 0x216u, 0xffffffffu, 0xc0017600u, 0x217u, 0xffffffffu,
    0xc0017600u, 0x215u, 0u, 0xc0016900u, 0x2f9u, 0x2du,
    0xc0016900u, 0x282u, 8u, 0xc0016900u, 0x280u, 0x80008u,
    0xc0016900u, 0x281u, 0xffff0000u, 0xc0016900u, 0x204u, 0u,
    0xc0016900u, 0x206u, 0x43fu, 0xc0016900u, 0x83u, 0xffffu,
    0xc0016900u, 0x317u, 0x10u, 0xc0016900u, 0x2fau, 0x3f800000u,
    0xc0016900u, 0x2fcu, 0x3f800000u, 0xc0016900u, 0x2fbu, 0x3f800000u,
    0xc0016900u, 0x2fdu, 0x3f800000u, 0xc0016900u, 0x202u, 0xcc0010u,
    0xc0016900u, 0x30eu, 0xffffffffu, 0xc0016900u, 0x30fu, 0xffffffffu,
    0xc0002f00u, 1u, 0xc0017600u, 7u, 0x1701ffu, 0xc0017600u,
    0x46u, 0x1701fdu, 0xc0017600u, 0x87u, 0x1701ffu, 0xc0017600u,
    0xc7u, 0x1701fdu, 0xc0017600u, 0x107u, 0x17u, 0xc0017600u,
    0x147u, 0x1701fdu, 0xc0017600u, 0x47u, 0x1cu, 0xc0016900u,
    0x1b1u, 2u, 0xc0016900u, 0x101u, 0u, 0xc0016900u,
    0x100u, 0xffffffffu, 0xc0016900u, 0x103u, 0u, 0xc0016900u,
    0x284u, 0u, 0xc0016900u, 0x290u, 0u, 0xc0016900u,
    0x2aeu, 0u, 0xc0016900u, 0x102u, 0u, 0xc0016900u,
    0x292u, 0u, 0xc0016900u, 0x293u, 0x6020000u, 0xc0016900u,
    0x2f8u, 0u, 0xc0016900u, 0x2deu, 0x1e9u, 0xc0036900u,
    0x295u, 0x100u, 0x100u, 4u, 0xc0017900u, 0x200u,
    0xe0000000u, 0xc0016900u, 0x2aau, 0xffu,
};

constexpr std::array<std::uint32_t, 0x62> ContextInitSequence400{
    0xc0012800u, 0x80000000u, 0x80000000u, 0xc0001200u, 0u, 0xc0016900u,
    0x2f9u, 0x2du, 0xc0016900u, 0x282u, 8u, 0xc0016900u,
    0x280u, 0x80008u, 0xc0016900u, 0x281u, 0xffff0000u, 0xc0016900u,
    0x204u, 0u, 0xc0016900u, 0x206u, 0x43fu, 0xc0016900u,
    0x83u, 0xffffu, 0xc0016900u, 0x317u, 0x10u, 0xc0016900u,
    0x2fau, 0x3f800000u, 0xc0016900u, 0x2fcu, 0x3f800000u, 0xc0016900u,
    0x2fbu, 0x3f800000u, 0xc0016900u, 0x2fdu, 0x3f800000u, 0xc0016900u,
    0x202u, 0xcc0010u, 0xc0016900u, 0x30eu, 0xffffffffu, 0xc0016900u,
    0x30fu, 0xffffffffu, 0xc0002f00u, 1u, 0xc0016900u, 0x1b1u,
    2u, 0xc0016900u, 0x101u, 0u, 0xc0016900u, 0x100u,
    0xffffffffu, 0xc0016900u, 0x103u, 0u, 0xc0016900u, 0x284u,
    0u, 0xc0016900u, 0x290u, 0u, 0xc0016900u, 0x2aeu,
    0u, 0xc0016900u, 0x102u, 0u, 0xc0016900u, 0x292u,
    0u, 0xc0016900u, 0x293u, 0x6020000u, 0xc0016900u, 0x2f8u,
    0u, 0xc0016900u, 0x2deu, 0x1e9u, 0xc0036900u, 0x295u,
    0x100u, 0x100u, 4u, 0xc0016900u, 0x2aau, 0xffu,
    0xc09e1000u, 0u,
};

}

extern "C" {

std::uint32_t APS5_VABI sceGnmDrawInitDefaultHardwareState350(std::uint32_t* command, std::uint32_t words) {
    if (words < HardwareInitWords) return 0;
    AgcDriverCheckGuestMemory_nid_postfix(command, HardwareInitWords * sizeof(std::uint32_t), alignof(std::uint32_t), true);
    auto* cursor = command;
    std::memcpy(cursor, ClearContextState.data(), sizeof(ClearContextState));
    cursor += ClearContextState.size();
    std::memcpy(cursor, InitSequence350.data(), sizeof(InitSequence350));
    cursor += InitSequence350.size();
    GnmDriver::WriteNop(cursor, static_cast<std::uint32_t>(command + HardwareInitWords - cursor));
    return HardwareInitWords;
}

std::uint32_t APS5_VABI sceGnmDrawInitToDefaultContextState400(std::uint32_t* command, std::uint32_t words) {
    if (words != ContextInitWords) return 0;
    AgcDriverCheckGuestMemory_nid_postfix(command, ContextInitWords * sizeof(std::uint32_t), alignof(std::uint32_t), true);
    std::memcpy(command, ContextInitSequence400.data(), sizeof(ContextInitSequence400));
    return ContextInitWords;
}

std::uint32_t APS5_VABI sceGnmDispatchInitDefaultHardwareState(std::uint32_t* command, std::uint32_t words) {
    if (words < HardwareInitWords) return 0;
    AgcDriverCheckGuestMemory_nid_postfix(command, HardwareInitWords * sizeof(std::uint32_t), alignof(std::uint32_t), true);
    auto* cursor = command;
    cursor = GnmDriver::WriteShRegister(cursor, 0x216u, 0xffffffffu, true);
    cursor = GnmDriver::WriteShRegister(cursor, 0x217u, 0xffffffffu, true);
    cursor = GnmDriver::WriteShRegister(cursor, 0x215u, 0x170u, true);
    const std::array<std::uint32_t, 7> acquire{GnmDriver::Type3Header(GnmDriver::OpcodeAcquireMem, 6), 0x28000000u, 0u, 0u, 0u, 0u, 0xau};
    std::memcpy(cursor, acquire.data(), sizeof(acquire));
    cursor += acquire.size();
    GnmDriver::WriteNop(cursor, static_cast<std::uint32_t>(command + HardwareInitWords - cursor));
    return HardwareInitWords;
}

}
