#include "prx/libSceGnmDriver/include/Packets.hpp"
#include "prx/libSceAgcDriver/Execution/include/Driver.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libSceAgcDriver/Execution/include/VideoOutput.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr std::uint32_t PrepareFlipWords = 64;
constexpr std::uint32_t PrepareFlipHeader = GnmDriver::Type3Header(GnmDriver::OpcodeNop, PrepareFlipWords - 1);
constexpr std::uint32_t PrepareFlip = 0x68750777u;
constexpr std::uint32_t PrepareFlipLabel = 0x68750778u;
constexpr std::uint32_t OpcodeWriteData = 0x37;
constexpr std::uint32_t WriteDataToMemory = 0x500u;

constexpr std::uint32_t OpcodeWriteConstRam = 0x81;
constexpr std::uint32_t OpcodeDumpConstRam = 0x83;
constexpr std::uint32_t OpcodeIncrementCeCounter = 0x84;
constexpr std::uint32_t OpcodeIncrementDeCounter = 0x85;
constexpr std::uint32_t OpcodeWaitOnCeCounter = 0x86;
constexpr std::uint32_t OpcodeWaitOnDeCounterDiff = 0x88;

void checkBuffers(std::uint32_t count, std::uint32_t* const* dcbAddresses, const std::uint32_t* dcbBytes, std::uint32_t* const* ccbAddresses, const std::uint32_t* ccbBytes) {
    if (count == 0 || dcbAddresses == nullptr || dcbBytes == nullptr) throw std::runtime_error("Gnm submit: no draw command buffers");
    AgcDriverCheckGuestMemory_nid_postfix(dcbAddresses, count * sizeof(*dcbAddresses), alignof(std::uint32_t*));
    AgcDriverCheckGuestMemory_nid_postfix(dcbBytes, count * sizeof(*dcbBytes), alignof(std::uint32_t));
    for (std::uint32_t index = 0; index < count; ++index) {
        if (dcbAddresses[index] == nullptr || dcbBytes[index] == 0 || dcbBytes[index] % sizeof(std::uint32_t) != 0) throw std::runtime_error("Gnm submit: invalid draw command buffer " + std::to_string(index));
    }
    if (ccbBytes == nullptr) return;
    AgcDriverCheckGuestMemory_nid_postfix(ccbBytes, count * sizeof(*ccbBytes), alignof(std::uint32_t));
    for (std::uint32_t index = 0; index < count; ++index) {
        if (ccbBytes[index] == 0) continue;
        if (ccbAddresses == nullptr || ccbBytes[index] % sizeof(std::uint32_t) != 0) throw std::runtime_error("Gnm submit: invalid constant command buffer " + std::to_string(index));
        AgcDriverCheckGuestMemory_nid_postfix(ccbAddresses, count * sizeof(*ccbAddresses), alignof(std::uint32_t*));
        AgcDriverCheckGuestMemory_nid_postfix(ccbAddresses[index], ccbBytes[index], alignof(std::uint32_t));
    }
}

std::uint32_t packetWords(std::uint32_t header, std::uint32_t remaining, const char* buffer) {
    // A type 2 packet is a one-word filler; the others give their body length.
    const auto words = (header >> 30u) == 2u ? 1u : (header >> 30u) == 3u ? ((header >> 16u) & 0x3fffu) + 2u : 0u;
    if (words == 0 || words > remaining) throw std::runtime_error(std::string("Gnm submit: malformed packet in the ") + buffer);
    return words;
}

// The PS4 constant engine runs the constant command buffer beside the draw one: it fills constant RAM and dumps it to
// memory, and the two meet at counters (the draw engine's WAIT_ON_CE_COUNTER waits until the constant engine has
// counted past the draw engine's INCREMENT_DE_COUNTER count). In one stream each wait becomes the constant packets up to
// the increment it waits for; the draw engine runs the constant RAM packets itself, as on the PS5, and the constant
// engine is never ahead, so its WAIT_ON_DE_COUNTER_DIFF holds nothing.
std::vector<std::uint32_t> mergeConstantEngine(const std::uint32_t* dcb, std::uint32_t dcbWords, const std::uint32_t* ccb, std::uint32_t ccbWords) {
    std::vector<std::uint32_t> merged;
    merged.reserve(dcbWords + ccbWords);
    std::uint32_t constantCursor = 0;
    std::uint64_t constantCount = 0;
    std::uint64_t drawCount = 0;
    const auto runConstantEngine = [&](bool toEnd) {
        while (constantCursor < ccbWords && (toEnd || constantCount <= drawCount)) {
            const auto header = ccb[constantCursor];
            const auto words = packetWords(header, ccbWords - constantCursor, "constant command buffer");
            const auto opcode = (header >> 30u) == 3u ? (header >> 8u) & 0xffu : GnmDriver::OpcodeNop;
            if (opcode == OpcodeWriteConstRam || opcode == OpcodeDumpConstRam) merged.insert(merged.end(), ccb + constantCursor, ccb + constantCursor + words);
            else if (opcode == OpcodeIncrementCeCounter) ++constantCount;
            else if (opcode != GnmDriver::OpcodeNop && opcode != OpcodeWaitOnDeCounterDiff) {
                char message[96];
                std::snprintf(message, sizeof(message), "Gnm submit: constant engine packet 0x%02x is not implemented", opcode);
                throw std::runtime_error(message);
            }
            constantCursor += words;
        }
    };
    for (std::uint32_t cursor = 0; cursor < dcbWords;) {
        const auto header = dcb[cursor];
        const auto words = packetWords(header, dcbWords - cursor, "draw command buffer");
        const auto opcode = (header >> 30u) == 3u ? (header >> 8u) & 0xffu : GnmDriver::OpcodeNop;
        if (opcode == OpcodeWaitOnCeCounter) runConstantEngine(false);
        else if (opcode == OpcodeIncrementDeCounter) ++drawCount;
        else merged.insert(merged.end(), dcb + cursor, dcb + cursor + words);
        cursor += words;
    }
    runConstantEngine(true);
    return merged;
}

void submit(std::uint32_t count, std::uint32_t* const* dcbAddresses, const std::uint32_t* dcbBytes, std::uint32_t* const* ccbAddresses, const std::uint32_t* ccbBytes) {
    AgcDriverUseGcnShaders_nid_postfix();
    for (std::uint32_t index = 0; index < count; ++index) {
        const auto words = dcbBytes[index] / static_cast<std::uint32_t>(sizeof(std::uint32_t));
        if (ccbBytes != nullptr && ccbBytes[index] != 0) {
            auto merged = mergeConstantEngine(dcbAddresses[index], words, ccbAddresses[index], ccbBytes[index] / static_cast<std::uint32_t>(sizeof(std::uint32_t)));
            const Packet packet{merged.data(), static_cast<std::uint32_t>(merged.size()), 0, {}};
            AgcDriverSubmit_nid_postfix(&packet, 0);
            continue;
        }
        const Packet packet{dcbAddresses[index], words, 0, {}};
        AgcDriverSubmit_nid_postfix(&packet, 0);
    }
}

void patchFlip(std::uint32_t* dcb, std::uint32_t words, std::uint32_t videoOutHandle, std::uint32_t displayBufferIndex, std::uint32_t flipMode, std::int64_t flipArgument) {
    if (words < PrepareFlipWords) throw std::runtime_error("Gnm submit and flip: the last draw command buffer is shorter than its prepareFlip packet");
    auto* flip = dcb + words - PrepareFlipWords;
    AgcDriverCheckGuestMemory_nid_postfix(flip, PrepareFlipWords * sizeof(std::uint32_t), alignof(std::uint32_t), true);
    std::array<std::uint32_t, 5> prepared{};
    std::memcpy(prepared.data(), flip, sizeof(prepared));
    if (prepared[0] != PrepareFlipHeader || (prepared[1] != PrepareFlip && prepared[1] != PrepareFlipLabel)) throw std::runtime_error("Gnm submit and flip: no prepareFlip packet at the end of the last draw command buffer, or an unsupported one (interrupt variants are not implemented)");
    const auto argument = static_cast<std::uint64_t>(flipArgument);
    const std::array<std::uint32_t, AgcDriver::FlipPacketWords> packet{AgcDriver::FlipPacketHeader, videoOutHandle, displayBufferIndex, flipMode, static_cast<std::uint32_t>(argument), static_cast<std::uint32_t>(argument >> 32u)};
    std::memcpy(flip, packet.data(), sizeof(packet));
    auto* cursor = flip + packet.size();
    if (prepared[1] == PrepareFlipLabel) {
        const std::array<std::uint32_t, 5> label{GnmDriver::Type3Header(OpcodeWriteData, 4), WriteDataToMemory, prepared[2] & ~3u, prepared[3], prepared[4]};
        std::memcpy(cursor, label.data(), sizeof(label));
        cursor += label.size();
    }
    GnmDriver::WriteNop(cursor, static_cast<std::uint32_t>(flip + PrepareFlipWords - cursor));
}

int submitAndFlip(std::uint32_t count, std::uint32_t* const* dcbAddresses, const std::uint32_t* dcbBytes, std::uint32_t* const* ccbAddresses, const std::uint32_t* ccbBytes, std::uint32_t videoOutHandle, std::uint32_t displayBufferIndex, std::uint32_t flipMode, std::int64_t flipArgument) {
    checkBuffers(count, dcbAddresses, dcbBytes, ccbAddresses, ccbBytes);
    patchFlip(dcbAddresses[count - 1], dcbBytes[count - 1] / static_cast<std::uint32_t>(sizeof(std::uint32_t)), videoOutHandle, displayBufferIndex, flipMode, flipArgument);
    submit(count, dcbAddresses, dcbBytes, ccbAddresses, ccbBytes);
    return 0;
}

}

extern "C" {

int APS5_VABI sceGnmSubmitCommandBuffers(std::uint32_t count, std::uint32_t* const* dcbAddresses, const std::uint32_t* dcbBytes, std::uint32_t* const* ccbAddresses, const std::uint32_t* ccbBytes) {
    checkBuffers(count, dcbAddresses, dcbBytes, ccbAddresses, ccbBytes);
    submit(count, dcbAddresses, dcbBytes, ccbAddresses, ccbBytes);
    return 0;
}

int APS5_VABI sceGnmSubmitCommandBuffersForWorkload(std::uint64_t workload, std::uint32_t count, std::uint32_t* const* dcbAddresses, const std::uint32_t* dcbBytes, std::uint32_t* const* ccbAddresses, const std::uint32_t* ccbBytes) {
    static_cast<void>(workload);
    return sceGnmSubmitCommandBuffers(count, dcbAddresses, dcbBytes, ccbAddresses, ccbBytes);
}

int APS5_VABI sceGnmSubmitAndFlipCommandBuffers(std::uint32_t count, std::uint32_t* const* dcbAddresses, const std::uint32_t* dcbBytes, std::uint32_t* const* ccbAddresses, const std::uint32_t* ccbBytes, std::uint32_t videoOutHandle, std::uint32_t displayBufferIndex, std::uint32_t flipMode, std::int64_t flipArgument) {
    return submitAndFlip(count, dcbAddresses, dcbBytes, ccbAddresses, ccbBytes, videoOutHandle, displayBufferIndex, flipMode, flipArgument);
}

int APS5_VABI sceGnmSubmitAndFlipCommandBuffersForWorkload(std::uint64_t workload, std::uint32_t count, std::uint32_t* const* dcbAddresses, const std::uint32_t* dcbBytes, std::uint32_t* const* ccbAddresses, const std::uint32_t* ccbBytes, std::uint32_t videoOutHandle, std::uint32_t displayBufferIndex, std::uint32_t flipMode, std::int64_t flipArgument) {
    static_cast<void>(workload);
    return submitAndFlip(count, dcbAddresses, dcbBytes, ccbAddresses, ccbBytes, videoOutHandle, displayBufferIndex, flipMode, flipArgument);
}

int APS5_VABI sceGnmSubmitDone(void) {
    return 0;
}

}
