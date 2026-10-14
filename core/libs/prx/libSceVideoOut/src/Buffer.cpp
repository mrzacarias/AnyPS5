#include <array>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <stdexcept>
#include <string>

#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"
#include "prx/libSceVideoOut/include/VideoOutDriver.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libc/include/ApplicationHeap.hpp"

AgcDriver::DisplayBuffer DescribeVideoOutBuffer(const VideoOutBuffer& buffer, const BufferAttributeGroup& group) {
    const auto& attribute = group.attribute;
    if (attribute.reserved0 != 0 || attribute.pad0 != 0 || attribute.reserved1[0] != 0 || attribute.reserved1[1] != 0 || attribute.reserved1[2] != 0) throw std::runtime_error("VideoOut: reserved buffer attribute bits are set");
    if (attribute.tiling_mode > (ApplicationIsPs4_nid_no_patch() ? AgcDriver::DisplayTilingPs4Display2dThin : 1u) || (attribute.tiling_mode == 0 && attribute.pitch_in_pixel != 0) || attribute.aspect_ratio != 0 || (attribute.option != VIDEO_OUT_BUFFER_ATTRIBUTE_OPTION_NONE && attribute.option != VIDEO_OUT_BUFFER_ATTRIBUTE_OPTION_STRICT_COLORIMETRY)) throw std::runtime_error("VideoOut: unsupported tiling " + std::to_string(attribute.tiling_mode) + ", pitch " + std::to_string(attribute.pitch_in_pixel) + ", aspect ratio " + std::to_string(attribute.aspect_ratio) + " or buffer option " + std::to_string(attribute.option));
    const auto hex = [](std::uint64_t value) {
        char text[24];
        std::snprintf(text, sizeof(text), "0x%llx", static_cast<unsigned long long>(value));
        return std::string(text);
    };
    const bool compressed = group.category == VIDEO_OUT_BUFFER_ATTRIBUTE_CATEGORY_COMPRESSED;
    if (!compressed && group.category != VIDEO_OUT_BUFFER_ATTRIBUTE_CATEGORY_UNCOMPRESSED) throw std::runtime_error("VideoOut: unknown buffer category " + std::to_string(group.category));
    if (!compressed && (buffer.metadataAddress != 0 || attribute.dcc_control != 0 || attribute.dcc_cb_register_clear_color != 0)) throw std::runtime_error("VideoOut: uncompressed buffer with DCC metadata " + hex(buffer.metadataAddress) + ", dcc_control " + hex(attribute.dcc_control) + " or DCC clear color " + hex(attribute.dcc_cb_register_clear_color));
    if (compressed && buffer.metadataAddress == 0) throw std::runtime_error("VideoOut: DCC-compressed buffer " + hex(buffer.dataAddress) + " without DCC metadata");
    if (compressed && attribute.tiling_mode != 0) throw std::runtime_error("VideoOut: DCC-compressed buffer " + hex(buffer.dataAddress) + " with linear tiling");
    if (compressed && (attribute.dcc_control & ~VIDEO_OUT_DCC_CONTROL_BLOCK_LAYOUT) != 0) throw std::runtime_error("VideoOut: unsupported DCC control " + hex(attribute.dcc_control) + " (only the block size and independence fields " + hex(VIDEO_OUT_DCC_CONTROL_BLOCK_LAYOUT) + " are known)");
    const AgcDriver::DisplayBuffer result{buffer.dataAddress, attribute.pixel_format, attribute.width, attribute.height, attribute.tiling_mode, attribute.pitch_in_pixel, compressed ? buffer.metadataAddress : 0, compressed ? attribute.dcc_cb_register_clear_color : 0};
    static_cast<void>(AgcDriverDisplayBufferSize_nid_postfix(result));
    return result;
}

extern "C" {

void APS5_VABI sceVideoOutSetBufferAttribute2(VideoOutBufferAttribute2* attribute, uint64_t pixelFormat, uint32_t tilingMode, uint32_t width, uint32_t height, uint64_t option, uint32_t dccControl, uint64_t dccCbRegisterClearColor) {
    if (attribute == nullptr) {
        throw std::runtime_error("sceVideoOutSetBufferAttribute2: null attribute");
    }
    std::memset(attribute, 0, sizeof(VideoOutBufferAttribute2));
    attribute->tiling_mode = tilingMode;
    attribute->aspect_ratio = 0;
    attribute->width = width;
    attribute->height = height;
    attribute->pitch_in_pixel = 0;
    attribute->option = option;
    attribute->pixel_format = pixelFormat;
    attribute->dcc_cb_register_clear_color = dccCbRegisterClearColor;
    attribute->dcc_control = dccControl;
}

void APS5_VABI sceVideoOutAddBufferAttributeOption(VideoOutBufferAttribute2* attribute, uint64_t option) {
    if (attribute == nullptr) {
        throw std::runtime_error("sceVideoOutAddBufferAttributeOption: null attribute");
    }
    attribute->option |= option;
}

int APS5_VABI sceVideoOutRegisterBuffers2(int handle, int setIndex, int bufferIndexStart, const VideoOutBuffers* buffers, int bufferNum, const VideoOutBufferAttribute2* attribute, int category, void* option) try {
    auto cfg = VideoOutDriver::Get().GetConfig(handle);
    if (cfg == nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_HANDLE");
    }
    if (buffers == nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_ADDRESS");
    }
    if (attribute == nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_OPTION");
    }
    if (option != nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_OPTION");
    }
    if (setIndex < 0 || setIndex >= VIDEO_OUT_BUFFER_ATTRIBUTE_NUM_MAX || bufferIndexStart < 0 || bufferIndexStart >= VIDEO_OUT_BUFFER_NUM_MAX || bufferNum < 1 || bufferNum > VIDEO_OUT_BUFFER_NUM_MAX || bufferIndexStart + bufferNum > VIDEO_OUT_BUFFER_NUM_MAX) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_VALUE");
    }
    if (category != VIDEO_OUT_BUFFER_ATTRIBUTE_CATEGORY_UNCOMPRESSED && category != VIDEO_OUT_BUFFER_ATTRIBUTE_CATEGORY_COMPRESSED) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_CATEGORY");
    }
    AgcDriverCheckGuestMemory_nid_postfix(attribute, sizeof(*attribute), alignof(VideoOutBufferAttribute2));
    AgcDriverCheckGuestMemory_nid_postfix(buffers, static_cast<std::size_t>(bufferNum) * sizeof(*buffers), alignof(VideoOutBuffers));
    std::unique_lock lock(cfg->mutex);
    cfg->Check();
    if (cfg->closing) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_HANDLE");
    }
    if (cfg->groups[setIndex].occupied) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_INDEX");
    }
    for (int i = 0; i < bufferNum; i++) {
        if (cfg->buffers[bufferIndexStart + i].Occupied()) {
            throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_SLOT_OCCUPIED");
        }
    }
    BufferAttributeGroup group{};
    group.attribute = *attribute;
    group.category = category;
    group.occupied = true;
    std::array<VideoOutBuffer, VIDEO_OUT_BUFFER_NUM_MAX> registered{};
    for (int i = 0; i < bufferNum; ++i) {
        registered[i] = {setIndex, reinterpret_cast<uint64_t>(buffers[i].data), reinterpret_cast<uint64_t>(buffers[i].metadata)};
        const auto display = DescribeVideoOutBuffer(registered[i], group);
        AgcDriverCheckGuestMemory_nid_postfix(buffers[i].data, AgcDriverDisplayBufferSize_nid_postfix(display), AgcDriverDisplayBufferAlignment_nid_postfix(display));
    }
    cfg->groups[setIndex] = group;
    cfg->width = attribute->width;
    cfg->height = attribute->height;
    for (int i = 0; i < bufferNum; i++) {
        cfg->buffers[bufferIndexStart + i] = registered[i];
    }
    return 0;
} catch (const ProcessShutdown&) {
    LibcAwaitExit_nid_postfix();
}

void APS5_VABI sceVideoOutSetBufferAttribute(VideoOutBufferAttribute* attribute, uint32_t pixelFormat, uint32_t tilingMode, uint32_t aspectRatio, uint32_t width, uint32_t height, uint32_t pitchInPixel) {
    if (attribute == nullptr) {
        throw std::runtime_error("sceVideoOutSetBufferAttribute: null attribute");
    }
    std::memset(attribute, 0, sizeof(VideoOutBufferAttribute));
    attribute->pixel_format = pixelFormat;
    attribute->tiling_mode = tilingMode;
    attribute->aspect_ratio = aspectRatio;
    attribute->width = width;
    attribute->height = height;
    attribute->pitch_in_pixel = pitchInPixel;
}

int APS5_VABI sceVideoOutRegisterBuffers(int handle, int bufferIndexStart, void* const* addresses, int bufferNum, const VideoOutBufferAttribute* attribute) try {
    auto cfg = VideoOutDriver::Get().GetConfig(handle);
    if (cfg == nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_HANDLE");
    }
    if (addresses == nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_ADDRESS");
    }
    if (attribute == nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_OPTION");
    }
    if (bufferNum < 1 || bufferNum > VIDEO_OUT_BUFFER_NUM_MAX) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_VALUE");
    }
    AgcDriverCheckGuestMemory_nid_postfix(attribute, sizeof(*attribute), alignof(VideoOutBufferAttribute));
    AgcDriverCheckGuestMemory_nid_postfix(addresses, static_cast<std::size_t>(bufferNum) * sizeof(*addresses), alignof(void*));
    if (attribute->reserved0 != 0 || attribute->reserved1 != 0) throw std::runtime_error("VideoOut: reserved buffer attribute bits are set");
    VideoOutBufferAttribute2 converted{};
    converted.tiling_mode = attribute->tiling_mode == 0 ? AgcDriver::DisplayTilingPs4Display2dThin : attribute->tiling_mode;
    converted.aspect_ratio = attribute->aspect_ratio;
    converted.width = attribute->width;
    converted.height = attribute->height;
    converted.pitch_in_pixel = attribute->pitch_in_pixel;
    converted.option = attribute->option;
    converted.pixel_format = (static_cast<std::uint64_t>(attribute->pixel_format & 0xFFFF0000u) << 32) | (static_cast<std::uint64_t>(attribute->pixel_format & 0xFFFFu) << 16);
    std::array<VideoOutBuffers, VIDEO_OUT_BUFFER_NUM_MAX> buffers{};
    for (int i = 0; i < bufferNum; ++i) buffers[i].data = addresses[i];
    int setIndex = 0;
    {
        std::unique_lock lock(cfg->mutex);
        cfg->Check();
        while (setIndex < VIDEO_OUT_BUFFER_ATTRIBUTE_NUM_MAX && cfg->groups[setIndex].occupied) ++setIndex;
    }
    if (setIndex == VIDEO_OUT_BUFFER_ATTRIBUTE_NUM_MAX) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_INDEX");
    }
    const int result = sceVideoOutRegisterBuffers2(handle, setIndex, bufferIndexStart, buffers.data(), bufferNum, &converted, VIDEO_OUT_BUFFER_ATTRIBUTE_CATEGORY_UNCOMPRESSED, nullptr);
    return result < 0 ? result : setIndex;
} catch (const ProcessShutdown&) {
    LibcAwaitExit_nid_postfix();
}

int APS5_VABI sceVideoOutSubmitChangeBufferAttribute2(int handle, int setIndex, const VideoOutBufferAttribute2* attribute, void* option) try {
    auto cfg = VideoOutDriver::Get().GetConfig(handle);
    if (cfg == nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_HANDLE");
    }
    if (attribute == nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_OPTION");
    }
    if (setIndex < 0 || setIndex >= VIDEO_OUT_BUFFER_ATTRIBUTE_NUM_MAX) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_INDEX");
    }
    if (option != nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_OPTION");
    }
    std::unique_lock lock(cfg->mutex);
    cfg->Check();
    if (cfg->closing || !cfg->groups[setIndex].occupied) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_INDEX");
    }
    AgcDriverCheckGuestMemory_nid_postfix(attribute, sizeof(*attribute), alignof(VideoOutBufferAttribute2));
    auto group = cfg->groups[setIndex];
    group.attribute = *attribute;
    for (int i = 0; i < VIDEO_OUT_BUFFER_NUM_MAX; ++i) {
        if (cfg->buffers[i].groupIndex != setIndex) continue;
        const auto display = DescribeVideoOutBuffer(cfg->buffers[i], group);
        AgcDriverCheckGuestMemory_nid_postfix(reinterpret_cast<const void*>(display.address), AgcDriverDisplayBufferSize_nid_postfix(display), AgcDriverDisplayBufferAlignment_nid_postfix(display));
    }
    cfg->groups[setIndex] = group;
    return 0;
} catch (const ProcessShutdown&) {
    LibcAwaitExit_nid_postfix();
}

int APS5_VABI sceVideoOutUnregisterBuffers(int handle, int setIndex) try {
    auto cfg = VideoOutDriver::Get().GetConfig(handle);
    if (cfg == nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_HANDLE");
    }
    if (setIndex < 0 || setIndex >= VIDEO_OUT_BUFFER_ATTRIBUTE_NUM_MAX) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_INDEX");
    }
    std::unique_lock lock(cfg->mutex);
    cfg->Check();
    if (cfg->closing) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_HANDLE");
    }
    if (!cfg->groups[setIndex].occupied) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_INDEX");
    }
    for (int i = 0; i < VIDEO_OUT_BUFFER_NUM_MAX; ++i) {
        if (cfg->buffers[i].groupIndex == setIndex && cfg->bufferPending[i] != 0) throw std::runtime_error("VideoOut: cannot unregister a buffer with a pending flip");
    }
    cfg->groups[setIndex] = BufferAttributeGroup{};
    for (auto& buf : cfg->buffers) {
        if (buf.groupIndex == setIndex) {
            buf = VideoOutBuffer{};
        }
    }
    return 0;
} catch (const ProcessShutdown&) {
    LibcAwaitExit_nid_postfix();
}

int APS5_VABI sceVideoOutSubmitFlip(int handle, int index, int flipMode, int64_t flipArg) try {
    auto cfg = VideoOutDriver::Get().GetConfig(handle);
    if (cfg == nullptr) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_HANDLE");
    }
    // Every mode is presented at the next vsync; HSYNC/WINDOW variants only change tearing behaviour.
    if (flipMode < VIDEO_OUT_FLIP_MODE_VSYNC || flipMode > 6) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_VALUE");
    }
    if (index < VIDEO_OUT_BUFFER_INDEX_BLACK || index >= VIDEO_OUT_BUFFER_NUM_MAX) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_INDEX");
    }
    std::unique_lock lock(cfg->mutex);
    cfg->Check();
    if (cfg->closing) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_HANDLE");
    }
    const bool special = (index == VIDEO_OUT_BUFFER_INDEX_BLANK || index == VIDEO_OUT_BUFFER_INDEX_BLACK);
    if (!special && !cfg->buffers[index].Occupied()) {
        throw std::runtime_error(std::string(__func__) + ": VIDEO_OUT_ERROR_INVALID_INDEX");
    }
    lock.unlock();
    return VideoOutDriver::Get().SubmitFlip(handle, index, flipMode, flipArg);
} catch (const ProcessShutdown&) {
    LibcAwaitExit_nid_postfix();
}

}
