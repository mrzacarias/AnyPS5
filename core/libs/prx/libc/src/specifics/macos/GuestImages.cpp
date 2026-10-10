#ifdef __APPLE__

#include "prx/libc/include/GuestAllocations.hpp"
#include "prx/libc/include/specifics/linux/ElfTypes.hpp"
#include <mach-o/dyld.h>
#include <mach-o/getsect.h>
#include <mach-o/loader.h>
#include <cstring>
#include <mutex>
#include <vector>

namespace {

constexpr std::uint64_t MetaVersion = 2;
constexpr std::size_t LoadBiasOffset = 0x08;
constexpr std::size_t HeaderCountOffset = 0x58;
constexpr std::size_t HeadersOffset = 0x60;

std::mutex registeredMutex;
std::vector<dl_phdr_info> registered;

}

extern "C" void Aps5RegisterGuestImage_nid_no_patch(const dl_phdr_info* image) {
    {
        std::lock_guard lock(registeredMutex);
        registered.push_back(*image);
    }
    GuestAllocations::Mutation mutation;
    for (std::uint16_t index = 0; index < image->dlpi_phnum; ++index) {
        const auto& header = image->dlpi_phdr[index];
        if (header.p_type != PT_LOAD || header.p_memsz == 0) continue;
        const auto begin = (image->dlpi_addr + header.p_vaddr) & ~std::uint64_t{0xfff};
        const auto end = (image->dlpi_addr + header.p_vaddr + header.p_memsz + 0xfff) & ~std::uint64_t{0xfff};
        const bool writable = (header.p_flags & PF_W) != 0;
        const bool readable = writable || (header.p_flags & PF_R) != 0;
        if (!mutation.Overlaps(reinterpret_cast<void*>(begin), static_cast<std::size_t>(end - begin))) {
            mutation.Add(reinterpret_cast<void*>(begin), static_cast<std::size_t>(end - begin), readable, writable);
            continue;
        }
        for (auto page = begin; page < end; page += 0x1000)
            if (!mutation.Overlaps(reinterpret_cast<void*>(page), 0x1000)) mutation.Add(reinterpret_cast<void*>(page), 0x1000, readable, writable);
    }
}

extern "C" int dl_iterate_phdr(int (*callback)(dl_phdr_info*, std::size_t, void*), void* data) {
    std::vector<dl_phdr_info> images;
    {
        std::lock_guard lock(registeredMutex);
        images = registered;
    }
    for (auto& image : images)
        if (const int result = callback(&image, sizeof(image), data); result != 0) return result;
    for (std::uint32_t index = 0; index < _dyld_image_count(); ++index) {
        const auto* header = reinterpret_cast<const mach_header_64*>(_dyld_get_image_header(index));
        if (header == nullptr || header->magic != MH_MAGIC_64 || (header->flags & MH_DYLIB_IN_CACHE) != 0) continue;
        unsigned long size = 0;
        const auto* meta = getsectiondata(header, "__APS5DATA", "__meta", &size);
        if (meta == nullptr || size < HeadersOffset) continue;
        std::uint64_t version;
        std::uint64_t loadBias;
        std::uint64_t count;
        std::memcpy(&version, meta, sizeof(version));
        std::memcpy(&loadBias, meta + LoadBiasOffset, sizeof(loadBias));
        std::memcpy(&count, meta + HeaderCountOffset, sizeof(count));
        if (version != MetaVersion || count > 0xffff || count > (size - HeadersOffset) / sizeof(Elf64_Phdr)) continue;
        dl_phdr_info info {};
        info.dlpi_addr = static_cast<std::uintptr_t>(loadBias);
        info.dlpi_name = header->filetype == MH_EXECUTE ? "" : _dyld_get_image_name(index);
        info.dlpi_phdr = reinterpret_cast<const Elf64_Phdr*>(meta + HeadersOffset);
        info.dlpi_phnum = static_cast<std::uint16_t>(count);
        if (const int result = callback(&info, sizeof(info), data); result != 0) return result;
    }
    return 0;
}

#endif
