#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

extern "C" {

std::int32_t APS5_VABI sceVrTrackerCpuProcess(const void* param) {
    static_cast<void>(param);
    NotImplemented_nid_no_patch("5IFOAYv-62g");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerGetResult(const void* param, void* result) {
    static_cast<void>(param);
    static_cast<void>(result);
    NotImplemented_nid_no_patch("76OBvrrQXUc");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerUpdateMotionSensorData(const void* param) {
    static_cast<void>(param);
    NotImplemented_nid_no_patch("9fvHMUbsom4");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerResetOrientationRelative(int device_type, std::int32_t handle) {
    static_cast<void>(device_type);
    static_cast<void>(handle);
    NotImplemented_nid_no_patch("E0P0sN-wy+4");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerRecalibrate(const void* param) {
    static_cast<void>(param);
    NotImplemented_nid_no_patch("EUCaQtXXYNI");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerTerm(void) {
    NotImplemented_nid_no_patch("IBv4P3q1pQ0");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerQueryMemory(const void* param, void* result) {
    static_cast<void>(param);
    static_cast<void>(result);
    NotImplemented_nid_no_patch("K7yhYrsIBPc");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerUnregisterDevice(std::int32_t handle) {
    static_cast<void>(handle);
    NotImplemented_nid_no_patch("Q8skQqEwn5c");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerInit(const void* param) {
    static_cast<void>(param);
    NotImplemented_nid_no_patch("QkRl7pART9M");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerGpuSubmit(const void* param) {
    static_cast<void>(param);
    NotImplemented_nid_no_patch("TVegDMLaBB8");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerNotifyEndOfCpuProcess(const void* param) {
    static_cast<void>(param);
    NotImplemented_nid_no_patch("VItTwN8DmS8");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerGetTime(std::uint64_t* time) {
    static_cast<void>(time);
    NotImplemented_nid_no_patch("XoeWzXlrnMw");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerGpuWait(const void* param) {
    static_cast<void>(param);
    NotImplemented_nid_no_patch("gkGuO9dd57M");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerRegisterDevice(int device_type, std::int32_t handle) {
    static_cast<void>(device_type);
    static_cast<void>(handle);
    NotImplemented_nid_no_patch("sIh8GwcevaQ");
    return {};
}

std::int32_t APS5_VABI sceVrTrackerGetPlayAreaWarningInfo(void* info) {
    static_cast<void>(info);
    NotImplemented_nid_no_patch("zvyKP0Z3UvU");
    return {};
}

}
