#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

static constexpr int SCE_DISC_MAP_ERROR_NO_BITMAP_INFO = static_cast<int>(0x81100004);

extern "C" {

APS5_EXPORT("fJgP+wqifno", sceDiscMapUnknown00);
int APS5_VABI sceDiscMapUnknown00(const char* path, std::int64_t offset, std::int64_t bytes, int* flags, int* first, int* second) {
    if (path == nullptr || offset < 0 || bytes < 0 || flags == nullptr || first == nullptr || second == nullptr) throw std::runtime_error("sceDiscMapUnknown00: invalid argument");
    *flags = 0;
    *first = 0;
    *second = 0;
    return 0;
}

int APS5_VABI sceDiscMapIsRequestOnHDD(const char* path, std::int64_t offset, std::int64_t bytes, int* onHdd) {
    if (path == nullptr || offset < 0 || bytes < 0 || onHdd == nullptr) throw std::runtime_error("sceDiscMapIsRequestOnHDD: invalid argument");
    return SCE_DISC_MAP_ERROR_NO_BITMAP_INFO;
}

}
