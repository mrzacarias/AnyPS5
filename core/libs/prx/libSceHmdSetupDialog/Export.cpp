#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

extern "C" {

std::int32_t APS5_VABI sceHmdSetupDialogTerminate(void) {
    NotImplemented_nid_no_patch("+z4OJmFreZc");
    return {};
}

std::int32_t APS5_VABI sceHmdSetupDialogGetResult(void* result) {
    static_cast<void>(result);
    NotImplemented_nid_no_patch("6lVRHMV5LY0");
    return {};
}

std::int32_t APS5_VABI sceHmdSetupDialogInitialize(void) {
    NotImplemented_nid_no_patch("NB1Y2kA2jCY");
    return {};
}

std::int32_t APS5_VABI sceHmdSetupDialogOpen(const void* param) {
    static_cast<void>(param);
    NotImplemented_nid_no_patch("NNgiV4T+akU");
    return {};
}

int APS5_VABI sceHmdSetupDialogUpdateStatus(void) {
    NotImplemented_nid_no_patch("Ud7j3+RDIBg");
    return 0;
}

std::int32_t APS5_VABI sceHmdSetupDialogClose(void) {
    NotImplemented_nid_no_patch("nmHzU4Gh0xs");
    return {};
}

}
