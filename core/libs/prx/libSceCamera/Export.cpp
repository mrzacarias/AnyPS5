#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

extern "C" {

std::int32_t APS5_VABI sceCameraStart(std::int32_t handle, void* param) {
    static_cast<void>(handle);
    static_cast<void>(param);
    NotImplemented_nid_no_patch("9EpRYMy7rHU");
    return {};
}

std::int32_t APS5_VABI sceCameraOpen(std::int32_t user_id, std::int32_t type, std::int32_t index, void* param) {
    static_cast<void>(user_id);
    static_cast<void>(type);
    static_cast<void>(index);
    static_cast<void>(param);
    NotImplemented_nid_no_patch("BHn83xrF92E");
    return {};
}

std::int32_t APS5_VABI sceCameraClose(std::int32_t handle) {
    static_cast<void>(handle);
    NotImplemented_nid_no_patch("OMS9LlcrvBo");
    return {};
}

std::int32_t APS5_VABI sceCameraIsValidFrameData(std::int32_t handle, void* frame_data) {
    static_cast<void>(handle);
    static_cast<void>(frame_data);
    NotImplemented_nid_no_patch("U3BVwQl2R5Q");
    return {};
}

std::int32_t APS5_VABI sceCameraSetConfig(std::int32_t handle, void* config) {
    static_cast<void>(handle);
    static_cast<void>(config);
    NotImplemented_nid_no_patch("VQ+5kAqsE2Q");
    return {};
}

std::int32_t APS5_VABI sceCameraGetFrameData(std::int32_t handle, void* frame_data) {
    static_cast<void>(handle);
    static_cast<void>(frame_data);
    NotImplemented_nid_no_patch("mxgMmR+1Kr0");
    return {};
}

std::int32_t APS5_VABI sceCameraIsAttached(std::int32_t index) {
    static_cast<void>(index);
    NotImplemented_nid_no_patch("p6n3Npi3YY4");
    return {};
}

std::int32_t APS5_VABI sceCameraSetVideoSync(std::int32_t handle, void* video_sync) {
    static_cast<void>(handle);
    static_cast<void>(video_sync);
    NotImplemented_nid_no_patch("wpeyFwJ+UEI");
    return {};
}

}
