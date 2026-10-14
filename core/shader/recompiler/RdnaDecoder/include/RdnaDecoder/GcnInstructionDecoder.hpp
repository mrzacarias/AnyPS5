#ifndef CORE_SHADER_RECOMPILIER_RDNADECODER_INCLUDE_RDNADECODER_GCNINSTRUCTIONDECODER_HPP
#define CORE_SHADER_RECOMPILIER_RDNADECODER_INCLUDE_RDNADECODER_GCNINSTRUCTIONDECODER_HPP

#include "RdnaDecoder/RdnaInstruction.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <span>
#include <unordered_map>

namespace ShaderRecompiler {

// PS4 shaders are GCN (GFX7) code. While set, DecodeRdnaInstruction and the program decoders built on it read GCN.
void SetGcnShaderCode(bool gcn);
[[nodiscard]] bool GcnShaderCode();

// The guest memory reader GCN programs fall back on when a request's memory does not hold an address their scalar
// loads read (GcnImageDimensions); the driver sets it.
void SetGcnGuestMemoryReader(bool (*read)(std::uint64_t address, std::uint32_t* value));
[[nodiscard]] bool ReadGcnGuestMemory(std::uint64_t address, std::uint32_t& value);

// Decodes a GCN instruction as the RDNA instruction with the same meaning, keeping its GCN program counter and length.
[[nodiscard]] RdnaInstruction DecodeGcnInstruction(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex);

// GCN image instructions take their dimension from the T#, where RDNA ones name it (DIM). While a scope is set,
// DecodeGcnInstruction gives the image instruction at each program counter the scope maps that RDNA DIM value; the
// others read 2D images, or 2D arrays when they declare an array.
class GcnImageDimensionScope {
public:
    explicit GcnImageDimensionScope(const std::unordered_map<std::uint32_t, std::uint32_t>& dimensions);
    ~GcnImageDimensionScope();
    GcnImageDimensionScope(const GcnImageDimensionScope&) = delete;
    GcnImageDimensionScope& operator=(const GcnImageDimensionScope&) = delete;

private:
    const std::unordered_map<std::uint32_t, std::uint32_t>* previous;
};

// The RDNA DIM, by program counter, of the GCN image instructions whose T# the scalar code before them holds in a
// straight-line reading: user data from s[userDataBase], copied by s_mov or loaded by s_load and s_buffer_load from
// the addresses it holds, which `read` reads a dword at a time. An image instruction whose T# is unknown, or whose
// type does not match its array declaration, is left out.
[[nodiscard]] std::unordered_map<std::uint32_t, std::uint32_t> GcnImageDimensions(std::span<const std::uint32_t> code, std::span<const std::uint32_t> userData, std::uint32_t userDataBase, const std::function<bool(std::uint64_t address, std::uint32_t& value)>& read);

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
