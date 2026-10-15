#include "SceTypes.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

extern "C" {
int APS5_VABI sceSaveDataInitialize(const void*);
int APS5_VABI sceSaveDataSetupSaveDataMemory(int, std::size_t, const SaveDataParam*);
int APS5_VABI sceSaveDataGetSaveDataMemory(int, void*, std::size_t, std::int64_t);
int APS5_VABI sceSaveDataSetSaveDataMemory(int, const void*, std::size_t, std::int64_t);
}

static void Check(bool value, int line) {
    if (!value) {
        std::fprintf(stderr, "PS4 save-data memory check failed at line %d\n", line);
        std::abort();
    }
}
#define Require(value) Check((value), __LINE__)

static std::vector<char> Read(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    Require(file.is_open());
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

int main() {
    const auto previous = std::filesystem::current_path();
    const auto root = std::filesystem::temp_directory_path() /
        ("anyps5-save-ps4-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    Require(std::filesystem::create_directory(root));
    std::filesystem::current_path(root);
    std::array<char, 4> buffer{};
    Require(sceSaveDataGetSaveDataMemory(7531, buffer.data(), buffer.size(), 0) == static_cast<int>(0x809F0001u));
    Require(sceSaveDataInitialize(nullptr) == 0);
    Require(sceSaveDataGetSaveDataMemory(7531, buffer.data(), buffer.size(), 0) == static_cast<int>(0x809F0012u));
    SaveDataParam param{};
    param.user_param = 42;
    Require(sceSaveDataSetupSaveDataMemory(7531, 16, &param) == 0);
    Require(Read("_sd_mem/u7531/slot0.bin") == std::vector<char>(16, '\0'));
    const auto* bytes = reinterpret_cast<const char*>(&param);
    Require(Read("_sd_mem/u7531/slot0.param") == std::vector<char>(bytes, bytes + sizeof(param)));
    const std::array<char, 4> saved{'l', 'i', 'm', 'b'};
    Require(sceSaveDataSetSaveDataMemory(7531, saved.data(), saved.size(), 12) == 0);
    Require(sceSaveDataSetSaveDataMemory(7531, saved.data(), saved.size(), 13) == static_cast<int>(0x809F0000u));
    Require(sceSaveDataGetSaveDataMemory(7531, buffer.data(), buffer.size(), 12) == 0);
    Require(buffer == saved);
    Require(sceSaveDataSetupSaveDataMemory(7531, 16, nullptr) == 0);
    Require(sceSaveDataGetSaveDataMemory(7531, buffer.data(), buffer.size(), 12) == 0);
    Require(buffer == saved);
    std::filesystem::current_path(previous);
    std::filesystem::remove_all(root);
    std::puts("PS4 save-data memory tests passed");
    return 0;
}
