#include "prx/libSceGnmDriver/include/Packets.hpp"
#include "prx/libSceAgcDriver/Execution/include/Driver.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libSceAgcDriver/Execution/include/VideoOutput.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

namespace {

constexpr std::uint32_t PrepareFlipWords = 64;
constexpr std::uint32_t PrepareFlipHeader = GnmDriver::Type3Header(GnmDriver::OpcodeNop, PrepareFlipWords - 1);
constexpr std::uint32_t PrepareFlip = 0x68750777u;
constexpr std::uint32_t PrepareFlipLabel = 0x68750778u;
constexpr std::uint32_t OpcodeWriteData = 0x37;
constexpr std::uint32_t WriteDataToMemory = 0x500u;

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
        if (ccbBytes[index] != 0) throw std::runtime_error("Gnm submit: constant command buffers are not implemented");
    }
    static_cast<void>(ccbAddresses);
}

void submit(std::uint32_t count, std::uint32_t* const* dcbAddresses, const std::uint32_t* dcbBytes) {
    AgcDriverUseGcnShaders_nid_postfix();
    for (std::uint32_t index = 0; index < count; ++index) {
        const Packet packet{dcbAddresses[index], dcbBytes[index] / static_cast<std::uint32_t>(sizeof(std::uint32_t)), 0, {}};
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
    submit(count, dcbAddresses, dcbBytes);
    return 0;
}

}

extern "C" {

int APS5_VABI sceGnmSubmitCommandBuffers(std::uint32_t count, std::uint32_t* const* dcbAddresses, const std::uint32_t* dcbBytes, std::uint32_t* const* ccbAddresses, const std::uint32_t* ccbBytes) {
    checkBuffers(count, dcbAddresses, dcbBytes, ccbAddresses, ccbBytes);
    submit(count, dcbAddresses, dcbBytes);
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
