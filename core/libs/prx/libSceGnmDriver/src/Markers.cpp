#include "prx/libSceGnmDriver/include/Packets.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"

#include <cstdint>
#include <cstring>

namespace {

constexpr std::uint32_t DebugMarkerPush = 0x68750001u;
constexpr std::uint32_t DebugMarkerPop = 0x68750002u;
constexpr std::uint32_t DebugSetMarker = 0x68750003u;
constexpr std::uint32_t DebugColorMarkerPush = 0x6875000eu;

int writeStringMarker(std::uint32_t* command, std::uint32_t words, const char* marker, std::uint32_t payload, bool colored, std::uint32_t color) {
    if (command == nullptr || marker == nullptr) return -1;
    const auto length = std::strlen(marker);
    const auto body = colored ? ((length + 0xc) >> 2) + ((length + 0x10) >> 3) * 2 : ((length + 8) >> 2) + ((length + 0xc) >> 3) * 2;
    if (body + 2 != words) return -1;
    AgcDriverCheckGuestMemory_nid_postfix(command, words * sizeof(std::uint32_t), alignof(std::uint32_t), true);
    command[0] = GnmDriver::Type3Header(GnmDriver::OpcodeNop, static_cast<std::uint32_t>(body) + 1);
    command[1] = payload;
    auto* bytes = reinterpret_cast<std::uint8_t*>(command + 2);
    std::memset(bytes, 0, body * sizeof(std::uint32_t));
    std::memcpy(bytes, marker, length + 1);
    if (colored) std::memcpy(bytes + length + 1 + 8, &color, sizeof(color));
    return 0;
}

}

extern "C" {

int APS5_VABI sceGnmInsertPushMarker(std::uint32_t* command, std::uint32_t words, const char* marker) {
    return writeStringMarker(command, words, marker, DebugMarkerPush, false, 0);
}

int APS5_VABI sceGnmInsertSetMarker(std::uint32_t* command, std::uint32_t words, const char* marker) {
    return writeStringMarker(command, words, marker, DebugSetMarker, false, 0);
}

int APS5_VABI sceGnmInsertPushColorMarker(std::uint32_t* command, std::uint32_t words, const char* marker, std::uint32_t color) {
    return writeStringMarker(command, words, marker, DebugColorMarkerPush, true, color);
}

int APS5_VABI sceGnmInsertPopMarker(std::uint32_t* command, std::uint32_t words) {
    if (command == nullptr || words != 6) return -1;
    AgcDriverCheckGuestMemory_nid_postfix(command, words * sizeof(std::uint32_t), alignof(std::uint32_t), true);
    command[0] = GnmDriver::Type3Header(GnmDriver::OpcodeNop, 5);
    command[1] = DebugMarkerPop;
    std::memset(command + 2, 0, 4 * sizeof(std::uint32_t));
    return 0;
}

int APS5_VABI sceGnmInsertDingDongMarker(std::uint32_t* command, std::uint32_t words) {
    if (command == nullptr || words != 4) return -1;
    AgcDriverCheckGuestMemory_nid_postfix(command, words * sizeof(std::uint32_t), alignof(std::uint32_t), true);
    command[0] = GnmDriver::Type3Header(GnmDriver::OpcodeNop, 3);
    std::memset(command + 1, 0, 3 * sizeof(std::uint32_t));
    return 0;
}

}
