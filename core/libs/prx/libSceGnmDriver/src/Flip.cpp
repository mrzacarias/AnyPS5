#include "prx/libSceGnmDriver/include/Packets.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libSceAgcDriver/Execution/include/VideoOutput.hpp"

#include <cstdint>

extern "C" {

int APS5_VABI sceGnmInsertWaitFlipDone(std::uint32_t* command, std::uint32_t words, int videoOutHandle, std::uint32_t displayBufferIndex) {
    constexpr std::uint32_t WaitFlipDoneWords = 7;
    if (words != WaitFlipDoneWords) return -1;
    AgcDriverCheckGuestMemory_nid_postfix(command, WaitFlipDoneWords * sizeof(std::uint32_t), alignof(std::uint32_t), true);
    command[0] = AgcDriver::RenderingWaitPacketHeader;
    command[1] = static_cast<std::uint32_t>(videoOutHandle);
    command[2] = displayBufferIndex;
    command[3] = 0;
    GnmDriver::WriteNop(command + AgcDriver::RenderingWaitPacketWords, WaitFlipDoneWords - AgcDriver::RenderingWaitPacketWords);
    return 0;
}

}
