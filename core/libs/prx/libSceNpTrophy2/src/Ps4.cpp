#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libSceNpTrophy2/include/NpTrophy2.hpp"
#include "prx/libc/include/General.hpp"

static constexpr int SCE_NP_TROPHY_ERROR_INVALID_ARGUMENT = static_cast<int>(0x80551604);
static constexpr int NpTrophyInvalidTrophyId = -1;

struct NpTrophyFlagArray {
    std::uint32_t bits[4];
};

extern "C" {


int APS5_VABI sceNpTrophyUnlockTrophy(int context, int handle, int trophyId, int* platinumId) {
    static_cast<void>(context);
    static_cast<void>(handle);
    static_cast<void>(trophyId);
    if (platinumId == nullptr) return SCE_NP_TROPHY_ERROR_INVALID_ARGUMENT;
    *platinumId = NpTrophyInvalidTrophyId;
    return 0;
}

int APS5_VABI sceNpTrophyDestroyContext(int context) {
    static_cast<void>(context);
    return 0;
}

int APS5_VABI sceNpTrophyDestroyHandle(int handle) {
    static_cast<void>(handle);
    return 0;
}

int APS5_VABI sceNpTrophyGetGameIcon(void) {
    NotImplemented_nid_no_patch("HLwz1fRIycA");
    return 0;
}

int APS5_VABI sceNpTrophyGetTrophyUnlockState(int context, int handle, NpTrophyFlagArray* flags, std::uint32_t* count) {
    static_cast<void>(context);
    static_cast<void>(handle);
    if (flags == nullptr || count == nullptr) return SCE_NP_TROPHY_ERROR_INVALID_ARGUMENT;
    *flags = {};
    *count = 0;
    return 0;
}

int APS5_VABI sceNpTrophyRegisterContext(int context, int handle, std::uint64_t options) {
    static_cast<void>(context);
    static_cast<void>(handle);
    static_cast<void>(options);
    return 0;
}

int APS5_VABI sceNpTrophyCreateContext(int* context, int userId, std::uint32_t serviceLabel, std::uint64_t options) {
    static_cast<void>(userId);
    static_cast<void>(serviceLabel);
    static_cast<void>(options);
    if (context == nullptr) return SCE_NP_TROPHY_ERROR_INVALID_ARGUMENT;
    *context = NP_TROPHY2_CONTEXT_DEFAULT;
    return 0;
}

int APS5_VABI sceNpTrophyGetGameInfo(void) {
    NotImplemented_nid_no_patch("YYP3f2W09og");
    return 0;
}

int APS5_VABI sceNpTrophyAbortHandle(int handle) {
    static_cast<void>(handle);
    return 0;
}

int APS5_VABI sceNpTrophyCaptureScreenshot(void) {
    NotImplemented_nid_no_patch("cqGkYAN-gRw");
    return 0;
}

int APS5_VABI sceNpTrophyShowTrophyList(int context, int handle) {
    static_cast<void>(context);
    static_cast<void>(handle);
    return 0;
}

int APS5_VABI sceNpTrophyGetTrophyIcon(void) {
    NotImplemented_nid_no_patch("eBL+l6HG9xk");
    return 0;
}

int APS5_VABI sceNpTrophyCreateHandle(int* handle) {
    if (handle == nullptr) return SCE_NP_TROPHY_ERROR_INVALID_ARGUMENT;
    *handle = NP_TROPHY2_HANDLE_DEFAULT;
    return 0;
}

int APS5_VABI sceNpTrophyGetTrophyInfo(void) {
    NotImplemented_nid_no_patch("qqUVGDgQBm0");
    return 0;
}

int APS5_VABI sceNpTrophyGetGroupIcon(void) {
    NotImplemented_nid_no_patch("w4uMPmErD4I");
    return 0;
}

int APS5_VABI sceNpTrophyGetGroupInfo(void) {
    NotImplemented_nid_no_patch("wTUwGfspKic");
    return 0;
}

}
