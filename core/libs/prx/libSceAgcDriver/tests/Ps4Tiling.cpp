#include "prx/libSceAgcDriver/Graphics/include/Ps4Tiling.hpp"
#include <bit>
#include <cstdio>
#include <vector>

using namespace AgcDriver::Graphics;

namespace {

std::size_t offset(const Ps4TileBlock& block, std::uint32_t pitch, std::uint32_t x, std::uint32_t y) {
    std::uint32_t inBlock = 0;
    const auto coordinates = (x & 0xfffu) | ((y << 12u) & 0xfff000u);
    for (std::uint32_t bit = 0; bit < 16u; ++bit) inBlock |= (std::popcount(coordinates & block.equation[bit]) & 1u) << bit;
    return (static_cast<std::size_t>(y / block.height) * (pitch / block.width) + x / block.width) * block.bytes + inBlock;
}

// The 32 bpp display 2D layout VideoOut scans out of PS4 display buffers (DisplayBuffer.cpp), written out by hand from
// addrlib: micro tile x0 x1 y0 x2 y1 y2, pipes x3^y3^x4, x4^y4, x5^y5, banks of the 1x1_16 macro tile mode.
std::size_t display2dThin(std::uint32_t x, std::uint32_t y, std::uint32_t pitch) {
    const std::uint32_t element = (x & 1u) | ((x >> 1u) & 1u) << 1u | (y & 1u) << 2u | ((x >> 2u) & 1u) << 3u | ((y >> 1u) & 1u) << 4u | ((y >> 2u) & 1u) << 5u;
    const std::uint32_t pipe = (((x >> 3u) ^ (y >> 3u) ^ (x >> 4u)) & 1u) | (((x >> 4u) ^ (y >> 4u)) & 1u) << 1u | (((x >> 5u) ^ (y >> 5u)) & 1u) << 2u;
    const std::uint32_t bank = (((x >> 6u) ^ (y >> 6u)) & 1u) | (((x >> 7u) ^ (y >> 5u) ^ (y >> 6u)) & 1u) << 1u | (((x >> 8u) ^ (y >> 4u)) & 1u) << 2u | (((x >> 9u) ^ (y >> 3u)) & 1u) << 3u;
    const std::size_t macroTile = static_cast<std::size_t>(y / 64u) * (pitch / 128u) + x / 128u;
    return element * 4u | static_cast<std::size_t>(pipe) << 8u | static_cast<std::size_t>(bank) << 11u | macroTile << 15u;
}

}

int main() {
    int failures = 0;
    const auto display = Ps4Tiling(TextureTileMode::kPs4Display2D, 4u);
    if (display.bytes != 32768u || display.width != 128u || display.height != 64u) {
        std::fprintf(stderr, "32 bpp display 2D macro tile is not 128x64 and 32 KiB\n");
        ++failures;
    }
    for (std::uint32_t y = 0; y < 1088u && failures < 8; y += 7u) {
        for (std::uint32_t x = 0; x < 1920u && failures < 8; x += 5u) {
            if (offset(display, 1920u, x, y) != display2dThin(x, y, 1920u)) {
                std::fprintf(stderr, "display 2D offset of (%u, %u) is 0x%zx, the scanout layout's 0x%zx\n", x, y, offset(display, 1920u, x, y), display2dThin(x, y, 1920u));
                ++failures;
            }
        }
    }
    // Depth tiling indices 0-7 decode to the depth mode, read through the depth surface that holds it and sized as thin 2D.
    for (std::uint32_t index = 0; index < 8u; ++index) {
        if (Ps4TileMode(index) != TextureTileMode::kPs4Depth) {
            std::fprintf(stderr, "PS4 depth tiling index %u is not the depth mode\n", index);
            ++failures;
        }
    }
    const auto depth = Ps4Tiling(TextureTileMode::kPs4Depth, 4u);
    const auto thin2d = Ps4Tiling(TextureTileMode::kPs4Thin2D, 4u);
    if (depth.bytes != thin2d.bytes || depth.width != thin2d.width || depth.height != thin2d.height || !IsPs4MacroTileMode(TextureTileMode::kPs4Depth)) {
        std::fprintf(stderr, "PS4 depth tiling is not sized as thin 2D\n");
        ++failures;
    }
    // Thin 1D: 8x8 micro tiles in rows, elements x0 y0 x1 y1 x2 y2.
    const auto thin = Ps4Tiling(TextureTileMode::kPs4Thin1D, 4u);
    if (thin.bytes != 256u || offset(thin, 16u, 3u, 5u) != ((1u | 1u << 1u | 1u << 2u | 0u << 3u | 0u << 4u | 1u << 5u) * 4u) || offset(thin, 16u, 9u, 8u) != 3u * 256u + 4u) {
        std::fprintf(stderr, "thin 1D offsets are wrong\n");
        ++failures;
    }
    // Thick 1D: 8x8x4 micro tiles, 32 bpp elements x0 y0 x1 z0 y1 z1 x2 y2 (addrlib's thick element order).
    const auto thick = Ps4Tiling(TextureTileMode::kPs4Thick1D, 4u);
    const auto thickOffset = [&](std::uint32_t x, std::uint32_t y, std::uint32_t z) {
        std::uint32_t at = 0;
        const auto coordinates = (x & 0xfffu) | ((y << 12u) & 0xfff000u) | (z << 24u);
        for (std::uint32_t bit = 0; bit < 16u; ++bit) at |= (std::popcount(coordinates & thick.equation[bit]) & 1u) << bit;
        return at;
    };
    if (thick.bytes != 1024u || thick.width != 8u || thick.height != 8u || thickOffset(1u, 0u, 1u) != 9u * 4u || thickOffset(0u, 0u, 2u) != 32u * 4u || thickOffset(4u, 4u, 3u) != (64u + 128u + 8u + 32u) * 4u) {
        std::fprintf(stderr, "thick 1D offsets are wrong\n");
        ++failures;
    }
    // Every 2D macro tile is a permutation of its bytes' elements.
    for (std::uint32_t bytes = 1; bytes <= 16u; bytes <<= 1u) {
        for (const auto mode : {TextureTileMode::kPs4Display2D, TextureTileMode::kPs4Thin2D}) {
            const auto block = Ps4Tiling(mode, bytes);
            std::vector<bool> seen(block.bytes / bytes);
            for (std::uint32_t y = 0; y < block.height; ++y) {
                for (std::uint32_t x = 0; x < block.width; ++x) {
                    const auto at = offset(block, block.width, x, y) / bytes;
                    if (at >= seen.size() || seen[at]) {
                        std::fprintf(stderr, "macro tile of mode %d at %u bytes maps two elements to one\n", static_cast<int>(mode), bytes);
                        ++failures;
                        y = block.height;
                        break;
                    }
                    seen[at] = true;
                }
            }
        }
    }
    return failures == 0 ? 0 : 1;
}
