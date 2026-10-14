#ifndef CORE_LIBS_PRX_LIBSCEGNMDRIVER_INCLUDE_PACKETS_HPP
#define CORE_LIBS_PRX_LIBSCEGNMDRIVER_INCLUDE_PACKETS_HPP

#include <cstdint>

#include "prx/libc/include/General.hpp"

namespace GnmDriver {

constexpr std::uint32_t OpcodeNop = 0x10;
constexpr std::uint32_t OpcodeAcquireMem = 0x58;
constexpr std::uint32_t OpcodeSetShReg = 0x76;

constexpr std::uint32_t Type3Header(std::uint32_t opcode, std::uint32_t bodyWords, bool compute = false) {
    return 0xc0000000u | (bodyWords - 1u) << 16u | opcode << 8u | (compute ? 2u : 0u);
}

inline std::uint32_t* WriteNop(std::uint32_t* command, std::uint32_t totalWords) {
    command[0] = Type3Header(OpcodeNop, totalWords - 1u);
    command[1] = 0;
    return command + totalWords;
}

inline std::uint32_t* WriteShRegister(std::uint32_t* command, std::uint32_t offset, std::uint32_t value, bool compute = false) {
    command[0] = Type3Header(OpcodeSetShReg, 2, compute);
    command[1] = offset;
    command[2] = value;
    return command + 3;
}

}

#endif
