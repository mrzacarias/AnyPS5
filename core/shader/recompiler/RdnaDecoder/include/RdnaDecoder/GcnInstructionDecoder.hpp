#ifndef CORE_SHADER_RECOMPILIER_RDNADECODER_INCLUDE_RDNADECODER_GCNINSTRUCTIONDECODER_HPP
#define CORE_SHADER_RECOMPILIER_RDNADECODER_INCLUDE_RDNADECODER_GCNINSTRUCTIONDECODER_HPP

#include "RdnaDecoder/RdnaInstruction.hpp"
#include <array>
#include <cstdint>
#include <span>

namespace ShaderRecompiler {

// PS4 shaders are GCN (GFX7) code. While set, DecodeRdnaInstruction and the program decoders built on it read GCN.
void SetGcnShaderCode(bool gcn);
[[nodiscard]] bool GcnShaderCode();

// Decodes a GCN instruction as the RDNA instruction with the same meaning, keeping its GCN program counter and length.
[[nodiscard]] RdnaInstruction DecodeGcnInstruction(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex);

// The RDNA buffer format (IrBufferFormat value) of a GCN data and number format pair.
[[nodiscard]] std::uint32_t GcnBufferFormat(std::uint32_t dataFormat, std::uint32_t numberFormat);
// The RDNA image format (IrBufferFormat value) of a GCN image data and number format pair.
[[nodiscard]] std::uint32_t GcnImageFormat(std::uint32_t dataFormat, std::uint32_t numberFormat);

// GCN buffer and image descriptors in the RDNA layout. An image is linear to RDNA; its word 7 holds bit 31 and the
// GCN tiling index.
[[nodiscard]] std::array<std::uint32_t, 4> GcnBufferDescriptor(const std::array<std::uint32_t, 4>& gcn);
[[nodiscard]] std::array<std::uint32_t, 8> GcnImageDescriptor(const std::array<std::uint32_t, 8>& gcn);

}

#endif
