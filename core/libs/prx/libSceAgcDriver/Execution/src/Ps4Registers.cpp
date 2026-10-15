#include "prx/libSceAgcDriver/Execution/include/Ps4Registers.hpp"
#include <algorithm>

namespace AgcDriver {

namespace {

constexpr std::uint32_t ColorBase = 0x318;
constexpr std::uint32_t ColorStride = 0xf;
constexpr std::uint32_t ColorTargets = 8;

// The color swizzle mode of a PS4 tiling index: display or thin 1D or 2D (0x1c-0x1f), else linear.
std::uint32_t colorSwizzleMode(std::uint32_t tileIndex) {
    switch (tileIndex) {
        case 9: return 0x1cu;
        case 13: return 0x1du;
        case 10: return 0x1eu;
        case 14: return 0x1fu;
        default: return 0u;
    }
}

std::uint32_t read(const Registers& context, std::uint32_t offset) {
    const auto found = context.find(offset);
    return found == context.end() ? 0u : found->second;
}

}

void WritePs4ContextRegister(Registers& context, Registers& userConfig, std::uint32_t offset, std::uint32_t value) {
    if (offset == 0x016) {
        // DB_DEPTH_SIZE holds the pitch and height in 8-pixel tiles, minus one; DB_DEPTH_SIZE_XY the last x and y.
        const auto width = ((value & 0x7ffu) + 1u) * 8u;
        const auto height = (((value >> 11u) & 0x7ffu) + 1u) * 8u;
        context.insert_or_assign(0x007, (width - 1u) | ((height - 1u) << 16u));
    }
    // GFX7 keeps the base vertex (VGT_INDX_OFFSET) and the primitive restart enable (VGT_MULTI_PRIM_IB_RESET_EN) in
    // context registers; GFX10 moved them to GE_INDX_OFFSET and GE_MULTI_PRIM_IB_RESET_EN.
    if (offset == 0x102) userConfig.insert_or_assign(0x24a, value);
    if (offset == 0x2a5) userConfig.insert_or_assign(0x24b, value & 1u);
    if (offset < ColorBase || offset >= ColorBase + ColorTargets * ColorStride) {
        context.insert_or_assign(offset, value);
        return;
    }
    const auto slot = (offset - ColorBase) / ColorStride;
    const auto target = ColorBase + slot * ColorStride;
    switch (offset - target) {
        case 0:
            // A PS4 color address fits CB_COLOR_BASE; CB_COLOR_BASE_EXT is zero.
            context.insert_or_assign(0x390 + slot, 0u);
            break;
        case 4:
            // CB_COLOR_INFO without the linear-general and CMASK layout bits and the blend optimization hints.
            value &= ~0x07f80080u;
            break;
        case 5:
            // CB_COLOR_ATTRIB keeps its sample counts and alpha override; its tile mode index becomes the swizzle mode of
            // a 2D CB_COLOR_ATTRIB3 at resource level 1: linear, or a PS4 tiling (ColorTileMode::Ps4*).
            context.insert_or_assign(0x3b8 + slot, (1u << 27u) | (1u << 24u) | (colorSwizzleMode(value & 0x1fu) << 14u));
            value &= 0x3f000u;
            break;
        default:
            break;
    }
    context.insert_or_assign(offset, value);
    if (offset - target == 1 || offset - target == 2) {
        // CB_COLOR_PITCH and CB_COLOR_SLICE count 8-pixel rows and 64-pixel tiles, minus one; CB_COLOR_ATTRIB2 holds
        // the last row and column of mip 0.
        const auto pitch = ((read(context, target + 1) & 0x7ffu) + 1u) * 8u;
        const auto height = std::max((((read(context, target + 2) & 0x3fffffu) + 1u) * 64u) / pitch, 1u);
        context.insert_or_assign(0x3b0 + slot, (height - 1u) | ((pitch - 1u) << 14u));
    }
}

}
