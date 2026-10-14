#include "prx/libSceAgcDriver/Execution/include/Ps4Registers.hpp"
#include <cstdio>

using namespace AgcDriver;

namespace {

int failures = 0;

std::uint32_t value(const Registers& context, std::uint32_t offset) {
    const auto found = context.find(offset);
    return found == context.end() ? 0xdeadbeefu : found->second;
}

void expect(const Registers& context, std::uint32_t offset, std::uint32_t expected, const char* name) {
    if (value(context, offset) == expected) return;
    std::fprintf(stderr, "%s: register 0x%x is 0x%08x, expected 0x%08x\n", name, offset, value(context, offset), expected);
    ++failures;
}

}

int main() {
    Registers context;
    Registers userConfig;
    // A 1920x1080 depth surface padded to 1088 rows.
    WritePs4ContextRegister(context, userConfig, 0x016, (1920u / 8u - 1u) | ((1088u / 8u - 1u) << 11u));
    expect(context, 0x007, 1919u | (1087u << 16u), "DB_DEPTH_SIZE_XY");

    // Color target 1, 1920 pixels wide and 1080 rows tall.
    constexpr std::uint32_t target = 0x318 + 0xf;
    WritePs4ContextRegister(context, userConfig, target, 0x12345u);
    expect(context, 0x391, 0u, "CB_COLOR1_BASE_EXT");
    WritePs4ContextRegister(context, userConfig, target + 1, 1920u / 8u - 1u);
    WritePs4ContextRegister(context, userConfig, target + 2, 1920u * 1080u / 64u - 1u);
    expect(context, 0x3b1, 1079u | (1919u << 14u), "CB_COLOR1_ATTRIB2");
    // 8_8_8_8 unorm with linear-general, CMASK-is-linear and both blend optimization hints.
    WritePs4ContextRegister(context, userConfig, target + 4, (10u << 2u) | 0x80u | (1u << 15u) | (1u << 19u) | (5u << 20u) | (3u << 23u));
    expect(context, target + 4, (10u << 2u) | (1u << 15u), "CB_COLOR1_INFO");
    // Tile mode index 13 (thin 1D) and FMASK tile mode index 14, one sample, alpha forced to one.
    WritePs4ContextRegister(context, userConfig, target + 5, 13u | (14u << 5u) | (1u << 17u));
    expect(context, target + 5, 1u << 17u, "CB_COLOR1_ATTRIB");
    expect(context, 0x3b9, (1u << 27u) | (1u << 24u) | (0x1du << 14u), "CB_COLOR1_ATTRIB3");

    // The base vertex and the primitive restart enable move to GE_INDX_OFFSET and GE_MULTI_PRIM_IB_RESET_EN.
    WritePs4ContextRegister(context, userConfig, 0x102, 37u);
    expect(userConfig, 0x24a, 37u, "GE_INDX_OFFSET");
    WritePs4ContextRegister(context, userConfig, 0x2a5, 1u);
    expect(userConfig, 0x24b, 1u, "GE_MULTI_PRIM_IB_RESET_EN");

    // A 32-bit depth surface with HTILE (TILE_SURFACE_ENABLE) and tile mode index 2: the GFX7 HTILE is not read.
    WritePs4ContextRegister(context, userConfig, 0x010, 3u | (2u << 20u) | (1u << 29u));
    expect(context, 0x010, 3u | (2u << 20u), "DB_Z_INFO");

    // Registers that need no translation keep their value.
    WritePs4ContextRegister(context, userConfig, 0x200, 0x7u);
    expect(context, 0x200, 0x7u, "DB_DEPTH_CONTROL");
    return failures == 0 ? 0 : 1;
}
