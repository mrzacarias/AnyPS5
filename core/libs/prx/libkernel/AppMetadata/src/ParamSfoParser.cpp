#include "prx/libkernel/AppMetadata/include/ParamSfoParser.hpp"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr std::uint8_t SfoMagic[4] = {0x00, 0x50, 0x53, 0x46};
constexpr std::size_t SfoHeaderSize = 0x14;
constexpr std::size_t SfoEntrySize = 0x10;
constexpr std::uint16_t SfoFormatUtf8Special = 0x0004;
constexpr std::uint16_t SfoFormatUtf8 = 0x0204;
constexpr std::uint16_t SfoFormatInteger = 0x0404;

template <typename T>
T read(const std::vector<std::uint8_t>& bytes, std::size_t offset) {
    if (offset > bytes.size() || sizeof(T) > bytes.size() - offset) throw std::runtime_error("param.sfo is truncated");
    T value;
    std::memcpy(&value, bytes.data() + offset, sizeof(T));
    return value;
}

std::vector<std::uint8_t> readFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("failed to open param.sfo");
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

}

ParsedParamJson parseParamSfo(const std::filesystem::path& paramSfoPath) {
    const auto bytes = readFile(paramSfoPath);
    if (bytes.size() < SfoHeaderSize || std::memcmp(bytes.data(), SfoMagic, sizeof(SfoMagic)) != 0) throw std::runtime_error("param.sfo has no PSF header");
    const auto keyTable = read<std::uint32_t>(bytes, 0x08);
    const auto dataTable = read<std::uint32_t>(bytes, 0x0C);
    const auto count = read<std::uint32_t>(bytes, 0x10);
    if (count > (bytes.size() - SfoHeaderSize) / SfoEntrySize || keyTable < SfoHeaderSize + count * SfoEntrySize || keyTable > dataTable || dataTable > bytes.size()) throw std::runtime_error("param.sfo tables are out of bounds");
    ParsedParamJson result;
    for (std::uint32_t index = 0; index < count; ++index) {
        const std::size_t entry = SfoHeaderSize + index * SfoEntrySize;
        const std::size_t keyOffset = keyTable + read<std::uint16_t>(bytes, entry);
        const auto format = read<std::uint16_t>(bytes, entry + 2);
        const auto length = read<std::uint32_t>(bytes, entry + 4);
        const std::size_t dataOffset = static_cast<std::size_t>(dataTable) + read<std::uint32_t>(bytes, entry + 12);
        if (keyOffset >= dataTable || dataOffset > bytes.size() || length > bytes.size() - dataOffset) throw std::runtime_error("param.sfo entry is out of bounds");
        const auto* keyStart = reinterpret_cast<const char*>(bytes.data() + keyOffset);
        const std::string key(keyStart, strnlen(keyStart, dataTable - keyOffset));
        const auto* data = bytes.data() + dataOffset;
        if (format == SfoFormatUtf8 || format == SfoFormatUtf8Special) {
            std::string value(reinterpret_cast<const char*>(data), length);
            value.resize(strnlen(value.c_str(), value.size()));
            if (key == "TITLE") result.title = value;
            else if (key == "TITLE_ID") result.titleId = value;
        } else if (format == SfoFormatInteger) {
            if (length != 4) throw std::runtime_error("param.sfo integer " + key + " is not 4 bytes");
            std::int32_t value;
            std::memcpy(&value, data, sizeof(value));
            if (key == "DOWNLOAD_DATA_SIZE") result.downloadDataSizeMiB = static_cast<std::uint32_t>(value);
            for (int param = 0; param < 4; ++param) {
                if (key == "USER_DEFINED_PARAM_" + std::to_string(param + 1)) result.userDefinedParams[param] = value;
            }
        } else {
            throw std::runtime_error("param.sfo entry " + key + " has unknown format " + std::to_string(format));
        }
    }
    if (result.titleId.empty()) throw std::runtime_error("param.sfo is missing TITLE_ID");
    if (result.title.empty()) throw std::runtime_error("param.sfo is missing TITLE");
    return result;
}
