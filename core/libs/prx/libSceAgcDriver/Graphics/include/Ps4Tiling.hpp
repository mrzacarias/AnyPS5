#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_PS4TILING_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_PS4TILING_HPP

#include "prx/libSceAgcDriver/Graphics/include/GuestTextureResource.hpp"
#include <array>
#include <cstdint>

namespace AgcDriver::Graphics {

// PS4 (GFX7) surface tiling, after AMD addrlib's GCN addressing (EgBasedLib) with the PS4's 8 pipes
// (P8_32x32_16x16), 256-byte pipe interleave and the macro tile modes of its GB_MACROTILE_MODE table.
constexpr bool IsPs4TileMode(TextureTileMode mode) {
    return mode == TextureTileMode::kPs4Display1D || mode == TextureTileMode::kPs4Display2D || mode == TextureTileMode::kPs4Thin1D || mode == TextureTileMode::kPs4Thin2D;
}

constexpr bool IsPs4MacroTileMode(TextureTileMode mode) {
    return mode == TextureTileMode::kPs4Display2D || mode == TextureTileMode::kPs4Thin2D;
}

// The 1D mode a 2D-tiled level falls back to when it is smaller than a macro tile.
constexpr TextureTileMode Ps4OneDimensionMode(TextureTileMode mode) {
    return mode == TextureTileMode::kPs4Display2D ? TextureTileMode::kPs4Display1D : mode == TextureTileMode::kPs4Thin2D ? TextureTileMode::kPs4Thin1D : mode;
}

// The tile mode of a PS4 tiling index (GB_TILE_MODE): linear, or one of the display and thin modes above.
TextureTileMode Ps4TileMode(std::uint32_t tileIndex);

// A tile block of a PS4 mode: a micro tile (1D) or a macro tile (2D) of `width` x `height` elements and `bytes`
// bytes, blocks laid out row by row, and the element-coordinate bits XORed into each byte-address bit of a
// block, in the TextureSwizzleEquation encoding (x bits 0-11, y bits 12-23).
struct Ps4TileBlock {
    std::uint32_t bytes;
    std::uint32_t width;
    std::uint32_t height;
    std::array<std::uint32_t, 16> equation;
};
Ps4TileBlock Ps4Tiling(TextureTileMode mode, std::uint32_t elementBytes);

}

#endif
