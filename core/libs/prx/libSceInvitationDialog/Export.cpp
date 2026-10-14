#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

extern "C" {

int APS5_VABI sceInvitationDialogUpdateStatus(void) {
    NotImplemented_nid_no_patch("9+g9iOq+7kg");
    return 0;
}

int APS5_VABI sceInvitationDialogTerminate(void) {
    NotImplemented_nid_no_patch("B6HVJtDYxEE");
    return 0;
}

std::int32_t APS5_VABI sceInvitationDialogGetResultA(void* result) {
    static_cast<void>(result);
    NotImplemented_nid_no_patch("WuuUhuKOxwQ");
    return {};
}

int APS5_VABI sceInvitationDialogInitialize(void) {
    NotImplemented_nid_no_patch("XvA5KS56wcs");
    return 0;
}

int APS5_VABI sceInvitationDialogOpenA(void) {
    NotImplemented_nid_no_patch("sAxbHhAWMXM");
    return 0;
}

}
