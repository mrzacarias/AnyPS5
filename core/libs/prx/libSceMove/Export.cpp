#include <atomic>
#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

static constexpr int SCE_MOVE_ERROR_NO_CONTROLLER_CONNECTED = 1;
static constexpr int SCE_MOVE_ERROR_NOT_INIT = static_cast<int>(0x80EE0001);
static constexpr int SCE_MOVE_ERROR_ALREADY_INIT = static_cast<int>(0x80EE0002);
static constexpr int SCE_MOVE_ERROR_INVALID_ARG = static_cast<int>(0x80EE0003);

static std::atomic<bool> g_initialized{false};
static std::atomic<int> g_lastHandle{0x30b0000};

extern "C" {

int APS5_VABI sceMoveInit(void) {
    return g_initialized.exchange(true) ? SCE_MOVE_ERROR_ALREADY_INIT : 0;
}

int APS5_VABI sceMoveTerm(void) {
    return g_initialized.exchange(false) ? 0 : SCE_MOVE_ERROR_NOT_INIT;
}

int APS5_VABI sceMoveOpen(int userId, int type, int index) {
    static_cast<void>(userId);
    static_cast<void>(type);
    static_cast<void>(index);
    if (!g_initialized) return SCE_MOVE_ERROR_NOT_INIT;
    return g_lastHandle += 0x100;
}

int APS5_VABI sceMoveGetDeviceInfo(int handle, void* info) {
    static_cast<void>(handle);
    if (!g_initialized) return SCE_MOVE_ERROR_NOT_INIT;
    if (info == nullptr) return SCE_MOVE_ERROR_INVALID_ARG;
    return SCE_MOVE_ERROR_NO_CONTROLLER_CONNECTED;
}

int APS5_VABI sceMoveReadStateRecent(int handle, std::int64_t timestamp, void* data, int* count) {
    static_cast<void>(handle);
    if (!g_initialized) return SCE_MOVE_ERROR_NOT_INIT;
    if (timestamp < 0 || data == nullptr || count == nullptr) return SCE_MOVE_ERROR_INVALID_ARG;
    return SCE_MOVE_ERROR_NO_CONTROLLER_CONNECTED;
}

int APS5_VABI sceMoveSetVibration(int handle, std::uint8_t intensity) {
    static_cast<void>(handle);
    static_cast<void>(intensity);
    return g_initialized ? SCE_MOVE_ERROR_NO_CONTROLLER_CONNECTED : SCE_MOVE_ERROR_NOT_INIT;
}

int APS5_VABI sceMoveSetLightSphere(int handle, std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
    static_cast<void>(handle);
    static_cast<void>(red);
    static_cast<void>(green);
    static_cast<void>(blue);
    return g_initialized ? SCE_MOVE_ERROR_NO_CONTROLLER_CONNECTED : SCE_MOVE_ERROR_NOT_INIT;
}

}
