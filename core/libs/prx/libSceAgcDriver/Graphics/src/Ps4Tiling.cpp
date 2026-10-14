#include "prx/libSceAgcDriver/Graphics/include/Ps4Tiling.hpp"
#include <algorithm>
#include <bit>
#include <stdexcept>
#include <string>
#include <vector>

namespace AgcDriver::Graphics {

namespace {

void require(bool condition, const char* reason) {
    if (!condition) throw std::runtime_error(reason);
}

constexpr std::uint32_t X(std::uint32_t bit) { return 1u << bit; }
constexpr std::uint32_t Y(std::uint32_t bit) { return 1u << (12u + bit); }

// The element order inside an 8x8 micro tile (addrlib ComputePixelIndexWithinMicroTile): display tiling depends on
// the element size, thin tiling interleaves x and y.
std::array<std::uint32_t, 6> microTileBits(TextureTileMode mode, std::uint32_t elementBytes) {
    if (mode == TextureTileMode::kPs4Thin1D || mode == TextureTileMode::kPs4Thin2D) return {X(0), Y(0), X(1), Y(1), X(2), Y(2)};
    switch (elementBytes) {
        case 1: return {X(0), X(1), X(2), Y(1), Y(0), Y(2)};
        case 2: return {X(0), X(1), X(2), Y(0), Y(1), Y(2)};
        case 4: return {X(0), X(1), Y(0), X(2), Y(1), Y(2)};
        case 8: return {X(0), Y(0), X(1), X(2), Y(1), Y(2)};
        default: return {Y(0), X(0), X(1), X(2), Y(1), Y(2)};
    }
}

}

TextureTileMode Ps4TileMode(std::uint32_t tileIndex) {
    switch (tileIndex) {
        case 8: case 31: return TextureTileMode::kLinear;  // DisplayLinearAligned, DisplayLinearGeneral
        case 9: return TextureTileMode::kPs4Display1D;
        case 10: return TextureTileMode::kPs4Display2D;
        case 13: return TextureTileMode::kPs4Thin1D;
        case 14: return TextureTileMode::kPs4Thin2D;
        default: throw std::runtime_error("AGC graphics: PS4 tiling index " + std::to_string(tileIndex) + " is not supported");
    }
}

Ps4TileBlock Ps4Tiling(TextureTileMode mode, std::uint32_t elementBytes) {
    require(IsPs4TileMode(mode) && std::has_single_bit(elementBytes) && elementBytes <= 16u, "AGC graphics: invalid PS4 tile mode or element size");
    const auto elementLog2 = static_cast<std::uint32_t>(std::countr_zero(elementBytes));
    // Byte-offset bits inside the pipe and bank interleave: the element's bytes, then its micro tile index.
    std::vector<std::uint32_t> offset(elementLog2, 0u);
    for (const auto bit : microTileBits(mode, elementBytes)) offset.push_back(bit);
    Ps4TileBlock block{};
    if (!IsPs4MacroTileMode(mode)) {
        block = {64u * elementBytes, 8u, 8u, {}};
        std::copy(offset.begin(), offset.end(), block.equation.begin());
        return block;
    }
    // One sample of a thin color surface: the micro tile is 64 * elementBytes bytes, so the macro tile mode index is
    // log2(elementBytes): 1x4_16, 1x2_16, 1x1_16, 1x1_16 and 1x1_8 (bank width 1, bank height, banks, aspect).
    constexpr std::uint32_t BankHeights[5] = {4, 2, 1, 1, 1};
    constexpr std::uint32_t BankCounts[5] = {16, 16, 16, 16, 8};
    constexpr std::uint32_t Aspects[5] = {4, 2, 2, 2, 1};
    constexpr std::uint32_t Pipes = 8;
    const auto bankHeight = BankHeights[elementLog2];
    const auto banks = BankCounts[elementLog2];
    const auto aspect = Aspects[elementLog2];
    // The micro tile rows of a bank (addrlib tileRowIndex) follow the element order.
    for (std::uint32_t bit = 0; (1u << bit) < bankHeight; ++bit) offset.push_back(Y(3u + bit));
    // Pipes (P8_32x32_16x16) and banks XOR coordinate bits, the banks those of the micro tile position past a pipe
    // row (x) and a bank column (y).
    const std::array<std::uint32_t, 3> pipe{X(3) | Y(3) | X(4), X(4) | Y(4), X(5) | Y(5)};
    const auto tx = [&](std::uint32_t bit) { return X(6u + bit); };
    const auto ty = [&](std::uint32_t bit) { return Y(3u + static_cast<std::uint32_t>(std::countr_zero(bankHeight)) + bit); };
    std::vector<std::uint32_t> bank;
    if (banks == 16u) bank = {tx(0) | ty(3), tx(1) | ty(2) | ty(3), tx(2) | ty(1), tx(3) | ty(0)};
    else bank = {tx(0) | ty(2), tx(1) | ty(1) | ty(2), tx(2) | ty(0)};
    std::vector<std::uint32_t> address(offset.begin(), offset.begin() + 8);
    address.insert(address.end(), pipe.begin(), pipe.end());
    address.insert(address.end(), bank.begin(), bank.end());
    address.insert(address.end(), offset.begin() + 8, offset.end());
    require(address.size() <= block.equation.size(), "AGC graphics: PS4 macro tile exceeds 64 KiB");
    block.bytes = 1u << address.size();
    block.width = 8u * Pipes * aspect;
    block.height = 8u * bankHeight * banks / aspect;
    std::copy(address.begin(), address.end(), block.equation.begin());
    require(block.bytes == block.width * block.height * elementBytes, "AGC graphics: PS4 macro tile geometry is inconsistent");
    return block;
}

}
