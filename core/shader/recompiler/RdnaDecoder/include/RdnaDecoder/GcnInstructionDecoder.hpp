#ifndef CORE_SHADER_RECOMPILIER_RDNADECODER_INCLUDE_RDNADECODER_GCNINSTRUCTIONDECODER_HPP
#define CORE_SHADER_RECOMPILIER_RDNADECODER_INCLUDE_RDNADECODER_GCNINSTRUCTIONDECODER_HPP

#include "RdnaDecoder/RdnaInstruction.hpp"
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

}

#endif
