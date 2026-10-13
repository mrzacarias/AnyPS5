#include <relinker/analysis/SceCodeExtent.hpp>
#include <algorithm>
#include <cstring>

namespace Relinker {

std::optional<VirtualAddress> FindSceCodeEnd(std::span<const std::uint8_t> segment, VirtualAddress segmentVAddr, const std::vector<UnusedNidFilter::StrictCodeRegion>& functions, VirtualAddress pltGot, ByteCount pltRelocationsSize) {
    VirtualAddress codeEnd = 0;
    for (const auto& function : functions)
        if (function.Begin >= segmentVAddr && function.Begin - segmentVAddr < segment.size())
            codeEnd = std::max(codeEnd, function.End);
    if (codeEnd == 0)
        return std::nullopt;
    if (codeEnd - segmentVAddr > segment.size())
        throw RelinkerException("Unwind function exceeds executable segment", codeEnd);
    if (pltRelocationsSize == 0)
        return codeEnd;
    const auto target = [&](const std::size_t at) {
        std::int32_t displacement = 0;
        std::memcpy(&displacement, segment.data() + at + 2, sizeof(displacement));
        return segmentVAddr + at + 6 + static_cast<VirtualAddress>(static_cast<std::int64_t>(displacement));
    };
    const std::size_t lastFunctionEnd = codeEnd - segmentVAddr;
    const auto pltSizeAt = [&](const std::size_t at) -> ByteCount {
        if (segment[at] == 0xff && segment[at + 1] == 0x35 && segment[at + 6] == 0xff && segment[at + 7] == 0x25
            && target(at) == pltGot + 8 && target(at + 6) == pltGot + 16)
            return 16 * (1 + pltRelocationsSize / 24);
        if (segment[at] == 0xff && segment[at + 1] == 0x25 && target(at) == pltGot + 24
            && std::all_of(segment.begin() + at + 6, segment.begin() + at + 16, [](const std::uint8_t byte) { return byte == 0xcc; }))
            return 16 * (pltRelocationsSize / 24);
        return 0;
    };
    std::optional<std::size_t> plt;
    ByteCount pltSize = 0;
    for (std::size_t at = lastFunctionEnd; !plt && segment.size() >= 16 && at <= segment.size() - 16; ++at)
        if ((pltSize = pltSizeAt(at)) != 0) plt = at;
    for (std::size_t at = 0; !plt && at < lastFunctionEnd && segment.size() >= 16 && at <= segment.size() - 16; ++at)
        if ((pltSize = pltSizeAt(at)) != 0) plt = at;
    if (!plt)
        throw RelinkerException("PLT not found after the last unwind function", codeEnd);
    if (pltSize > segment.size() - *plt)
        throw RelinkerException("PLT exceeds executable segment", segmentVAddr + *plt);
    return std::max(codeEnd, segmentVAddr + *plt + pltSize);
}

}
