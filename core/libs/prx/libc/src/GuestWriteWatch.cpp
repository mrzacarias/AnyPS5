#include "prx/libc/include/GuestWriteWatch.hpp"
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

#if defined(__linux__)
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/userfaultfd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <atomic>
#include <chrono>
#include <csignal>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <memory>
#include <sys/mman.h>
#include <thread>
#include <unistd.h>
#endif

#if (defined(__linux__) && defined(PAGEMAP_SCAN) && defined(UFFD_FEATURE_WP_ASYNC)) || defined(__APPLE__)
#define APS5_GUEST_WRITE_WATCH 1
#endif

namespace GuestWriteWatch {
namespace {

#if defined(__linux__) && defined(PAGEMAP_SCAN) && defined(UFFD_FEATURE_WP_ASYNC)
constexpr std::uintptr_t PageBytes = 4096;

class Watch {
public:
    static Watch& Get() {
        static Watch watch;
        return watch;
    }

    bool Available() const {
        return _pagemap >= 0;
    }

    void Register(std::uintptr_t begin, std::uintptr_t end) {
        if (!Available() || end <= begin) return;
        std::unique_lock lock(_lock);
        remove(_ranges, begin, end);
        remove(_fresh, begin, end);
        uffdio_register registration{};
        registration.range.start = begin;
        registration.range.len = end - begin;
        registration.mode = UFFDIO_REGISTER_MODE_WP;
        if (ioctl(_uffd, UFFDIO_REGISTER, &registration) != 0) {
            static bool reported = false;
            if (!reported) {
                reported = true;
                std::fprintf(stderr, "[memory] write watch: cannot register 0x%llx+0x%llx (%s); the range stays unwatched\n", static_cast<unsigned long long>(begin), static_cast<unsigned long long>(end - begin), std::strerror(errno));
            }
            return;
        }
        insert(_ranges, begin, end);
        insert(_fresh, begin, end);
    }

    bool Unregister(std::uintptr_t begin, std::uintptr_t end) {
        if (!Available() || end <= begin) return false;
        std::unique_lock lock(_lock);
        const bool watched = remove(_ranges, begin, end);
        remove(_fresh, begin, end);
        return watched;
    }

    bool Covers(std::uintptr_t begin, std::uintptr_t end) {
        if (!Available()) return false;
        std::shared_lock lock(_lock);
        return covers(begin, end);
    }

    bool Collect(std::uintptr_t begin, std::uintptr_t end, void (*written)(void*, std::uintptr_t, std::uintptr_t), void* context) {
        if (!Available()) return false;
        begin &= ~(PageBytes - 1);
        end = (end + PageBytes - 1) & ~(PageBytes - 1);
        if (end <= begin) return true;
        std::vector<std::pair<std::uintptr_t, std::uintptr_t>> fresh;
        {
            std::unique_lock lock(_lock);
            if (!covers(begin, end)) return false;
            auto it = _fresh.upper_bound(begin);
            if (it != _fresh.begin()) --it;
            for (; it != _fresh.end() && it->first < end; ++it) {
                const auto from = std::max(it->first, begin);
                const auto to = std::min(it->second, end);
                if (from < to) fresh.emplace_back(from, to);
            }
            if (!fresh.empty()) remove(_fresh, begin, end);
        }
        for (const auto& [from, to] : fresh) {
            written(context, from, to);
            if (protect(from, to)) continue;
            std::unique_lock lock(_lock);
            for (const auto& [left, right] : fresh) {
                if (!covers(left, right)) continue;
                remove(_fresh, left, right);
                insert(_fresh, left, right);
            }
            written(context, begin, end);
            return false;
        }
        std::array<page_region, 256> regions;
        auto cursor = begin;
        while (cursor < end) {
            pm_scan_arg scan{};
            scan.size = sizeof(scan);
            scan.flags = PM_SCAN_WP_MATCHING | PM_SCAN_CHECK_WPASYNC;
            scan.start = cursor;
            scan.end = end;
            scan.vec = reinterpret_cast<std::uintptr_t>(regions.data());
            scan.vec_len = regions.size();
            scan.category_mask = PAGE_IS_WRITTEN;
            scan.return_mask = PAGE_IS_WRITTEN;
            const auto count = ioctl(_pagemap, PAGEMAP_SCAN, &scan);
            if (count < 0) {
                written(context, cursor, end);
                return false;
            }
            for (long i = 0; i < count; ++i) written(context, regions[i].start, regions[i].end);
            if (scan.walk_end <= cursor || scan.walk_end >= end) break;
            cursor = scan.walk_end;
        }
        return true;
    }

private:
    Watch() {
        if (std::getenv("APS5_NO_WRITE_WATCH") == nullptr) open();
    }

    void open() {
        _uffd = static_cast<int>(syscall(SYS_userfaultfd, O_CLOEXEC | O_NONBLOCK));
        if (_uffd < 0 && errno == EPERM) _uffd = static_cast<int>(syscall(SYS_userfaultfd, O_CLOEXEC | O_NONBLOCK | UFFD_USER_MODE_ONLY));
        if (_uffd < 0) return unavailable("userfaultfd");
        uffdio_api api{};
        api.api = UFFD_API;
        api.features = UFFD_FEATURE_WP_ASYNC | UFFD_FEATURE_WP_UNPOPULATED;
        if (ioctl(_uffd, UFFDIO_API, &api) != 0 || (api.features & UFFD_FEATURE_WP_ASYNC) == 0 || (api.features & UFFD_FEATURE_WP_UNPOPULATED) == 0) return unavailable("asynchronous userfaultfd write protection");
        const int pagemap = ::open("/proc/self/pagemap", O_RDONLY | O_CLOEXEC);
        if (pagemap < 0) return unavailable("/proc/self/pagemap");
        if (!probe(pagemap)) {
            close(pagemap);
            return unavailable("PAGEMAP_SCAN");
        }
        _pagemap = pagemap;
    }

    void unavailable(const char* what) {
        const int error = errno;
        if (_uffd >= 0) close(_uffd);
        _uffd = -1;
        std::fprintf(stderr, "[memory] write watch unavailable: %s failed (%s); guest memory is compared instead\n", what, std::strerror(error));
    }

    bool protect(std::uintptr_t begin, std::uintptr_t end) const {
        uffdio_writeprotect protection{};
        protection.range.start = begin;
        protection.range.len = end - begin;
        protection.mode = UFFDIO_WRITEPROTECT_MODE_WP;
        return ioctl(_uffd, UFFDIO_WRITEPROTECT, &protection) == 0;
    }

    bool probe(int pagemap) {
        constexpr std::uintptr_t probeBytes = 4 * PageBytes;
        void* pages = mmap(nullptr, probeBytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (pages == MAP_FAILED) return false;
        const auto address = reinterpret_cast<std::uintptr_t>(pages);
        auto* bytes = static_cast<volatile char*>(pages);
        bytes[0] = 1;
        uffdio_register registration{};
        registration.range.start = address;
        registration.range.len = probeBytes;
        registration.mode = UFFDIO_REGISTER_MODE_WP;
        bool working = ioctl(_uffd, UFFDIO_REGISTER, &registration) == 0 && protect(address, address + probeBytes);
        std::array<page_region, 4> regions{};
        const auto scan = [&]() -> long {
            pm_scan_arg arguments{};
            arguments.size = sizeof(arguments);
            arguments.flags = PM_SCAN_WP_MATCHING | PM_SCAN_CHECK_WPASYNC;
            arguments.start = address;
            arguments.end = address + probeBytes;
            arguments.vec = reinterpret_cast<std::uintptr_t>(regions.data());
            arguments.vec_len = regions.size();
            arguments.category_mask = PAGE_IS_WRITTEN;
            arguments.return_mask = PAGE_IS_WRITTEN;
            const auto count = ioctl(pagemap, PAGEMAP_SCAN, &arguments);
            if (count < 0) return -1;
            long pagesWritten = 0;
            for (long i = 0; i < count; ++i) pagesWritten += static_cast<long>((regions[i].end - regions[i].start) / PageBytes);
            return pagesWritten;
        };
        working = working && scan() == 0;
        if (working) {
            bytes[0] = 2;
            bytes[2 * PageBytes] = 1;
            working = scan() == 2 && scan() == 0;
        }
        munmap(pages, probeBytes);
        return working;
    }

    bool covers(std::uintptr_t begin, std::uintptr_t end) const {
        auto next = _ranges.upper_bound(begin);
        if (next == _ranges.begin()) return false;
        return std::prev(next)->second >= end;
    }

    static void insert(std::map<std::uintptr_t, std::uintptr_t>& ranges, std::uintptr_t begin, std::uintptr_t end) {
        auto next = ranges.upper_bound(begin);
        if (next != ranges.begin() && std::prev(next)->second == begin) {
            begin = std::prev(next)->first;
            ranges.erase(std::prev(next));
        }
        if (next != ranges.end() && next->first == end) {
            end = next->second;
            ranges.erase(next);
        }
        ranges.emplace(begin, end);
    }

    static bool remove(std::map<std::uintptr_t, std::uintptr_t>& ranges, std::uintptr_t begin, std::uintptr_t end) {
        bool removed = false;
        auto it = ranges.upper_bound(begin);
        if (it != ranges.begin()) --it;
        while (it != ranges.end() && it->first < end) {
            const auto rangeBegin = it->first;
            const auto rangeEnd = it->second;
            if (rangeEnd <= begin) {
                ++it;
                continue;
            }
            removed = true;
            it = ranges.erase(it);
            if (rangeBegin < begin) ranges.emplace(rangeBegin, begin);
            if (rangeEnd > end) ranges.emplace(end, rangeEnd);
        }
        return removed;
    }

    int _uffd = -1;
    int _pagemap = -1;
    std::shared_mutex _lock;
    std::map<std::uintptr_t, std::uintptr_t> _ranges;
    std::map<std::uintptr_t, std::uintptr_t> _fresh;
};

const bool g_opened = (Watch::Get(), true);
#elif defined(__APPLE__)
// macOS has no asynchronous write protection: a collect write-protects the written pages it reports,
// and the first store to one faults into OnFault, which marks the page written and lifts the
// protection. Kernel stores fail on a protected page instead of faulting, so the host paths that let
// the kernel write guest memory open the pages first (BeginHostWrite). A page is tracked only when
// the host maps it read-write and the registered range covers it whole; other pages are reported
// written at every collect.
class Watch {
public:
    static Watch& Get() {
        static Watch watch;
        return watch;
    }

    bool Available() const {
        return _available;
    }

    void Register(std::uintptr_t begin, std::uintptr_t end) {
        if (!Available() || end <= begin) return;
        std::lock_guard lock(_mutex);
        const auto writable = readWrite(begin, end);
        for (auto page = begin / _pageBytes; page * _pageBytes < end; ++page) {
            auto* state = slot(page, true);
            if (state == nullptr) continue;
            const auto from = page * _pageBytes;
            const bool whole = from >= begin && from + _pageBytes <= end;
            const bool tracked = whole && std::any_of(writable.begin(), writable.end(), [&](const auto& run) { return run.first <= from && from + _pageBytes <= run.second; });
            const auto previous = replace(*state, tracked ? Dirty : Untracked);
            // A page this watch protected reads as read-only to the host: it stays tracked.
            if (!tracked && whole && previous == Clean) state->store(Clean, std::memory_order_release);
        }
    }

    bool Unregister(std::uintptr_t begin, std::uintptr_t end) {
        if (!Available() || end <= begin) return false;
        std::lock_guard lock(_mutex);
        bool watched = false;
        for (auto page = begin / _pageBytes; page * _pageBytes < end; ++page) {
            auto* state = slot(page, false);
            if (state == nullptr) continue;
            const auto previous = replace(*state, Unwatched);
            if (previous == Unwatched) continue;
            watched = true;
            if (previous == Clean || previous == Protecting) mprotect(reinterpret_cast<void*>(page * _pageBytes), _pageBytes, PROT_READ | PROT_WRITE);
        }
        return watched;
    }

    bool Covers(std::uintptr_t begin, std::uintptr_t end) {
        if (!Available()) return false;
        for (auto page = begin / _pageBytes; page * _pageBytes < end; ++page) {
            auto* state = slot(page, false);
            if (state == nullptr || state->load(std::memory_order_acquire) == Unwatched) return false;
        }
        return true;
    }

    bool Collect(std::uintptr_t begin, std::uintptr_t end, void (*written)(void*, std::uintptr_t, std::uintptr_t), void* context) {
        if (!Available()) return false;
        begin -= begin % _pageBytes;
        end = (end + _pageBytes - 1) / _pageBytes * _pageBytes;
        if (end <= begin) return true;
        if (!Covers(begin, end)) return false;
        std::uintptr_t reportBegin = 0, reportEnd = 0, protectBegin = 0, protectEnd = 0;
        bool complete = true;
        const auto report = [&] {
            if (reportEnd > reportBegin) written(context, reportBegin, reportEnd);
            reportBegin = reportEnd = 0;
        };
        const auto protect = [&] {
            if (protectEnd <= protectBegin) return;
            const bool done = mprotect(reinterpret_cast<void*>(protectBegin), protectEnd - protectBegin, PROT_READ) == 0;
            for (auto from = protectBegin; from < protectEnd; from += _pageBytes) {
                std::uint8_t expected = Protecting;
                slot(from / _pageBytes, false)->compare_exchange_strong(expected, done ? Clean : Dirty, std::memory_order_acq_rel);
            }
            complete = complete && done;
            _protected.fetch_add((protectEnd - protectBegin) / _pageBytes, std::memory_order_relaxed);
            protectBegin = protectEnd = 0;
        };
        for (auto from = begin; from < end; from += _pageBytes) {
            auto* state = slot(from / _pageBytes, false);
            auto current = state->load(std::memory_order_acquire);
            if (current == Clean) {
                report();
                protect();
                continue;
            }
            if (reportEnd != from) {
                report();
                reportBegin = from;
            }
            reportEnd = from + _pageBytes;
            if (current == Dirty && state->compare_exchange_strong(current, Protecting, std::memory_order_acq_rel)) {
                if (protectEnd != from) {
                    protect();
                    protectBegin = from;
                }
                protectEnd = from + _pageBytes;
            } else {
                protect();
            }
        }
        report();
        protect();
        if (!complete) written(context, begin, end);
        reportProfile();
        return complete;
    }

    void BeginHostWrite(std::uintptr_t begin, std::uintptr_t end) {
        if (!Available() || end <= begin) return;
        std::lock_guard lock(_mutex);
        for (auto page = begin / _pageBytes; page * _pageBytes < end; ++page) {
            auto* state = slot(page, false);
            if (state == nullptr || _hostWrites[page]++ != 0) continue;
            auto current = state->load(std::memory_order_acquire);
            for (;;) {
                if (current == Unwatched || current == Untracked) break;
                if (current == Protecting) {
                    std::this_thread::yield();
                    current = state->load(std::memory_order_acquire);
                    continue;
                }
                if (state->compare_exchange_weak(current, HostOpen, std::memory_order_acq_rel)) {
                    mprotect(reinterpret_cast<void*>(page * _pageBytes), _pageBytes, PROT_READ | PROT_WRITE);
                    break;
                }
            }
        }
        _hostOpened.fetch_add(1, std::memory_order_relaxed);
    }

    void EndHostWrite(std::uintptr_t begin, std::uintptr_t end) {
        if (!Available() || end <= begin) return;
        std::lock_guard lock(_mutex);
        for (auto page = begin / _pageBytes; page * _pageBytes < end; ++page) {
            const auto found = _hostWrites.find(page);
            if (found == _hostWrites.end() || --found->second != 0) continue;
            _hostWrites.erase(found);
            std::uint8_t expected = HostOpen;
            if (auto* state = slot(page, false)) state->compare_exchange_strong(expected, Dirty, std::memory_order_acq_rel);
        }
    }

    // After the guest changed the protection of [begin, end): a tracked page made read-write again
    // is write-protected again (its state stays), one made inaccessible or executable is no longer
    // tracked. Read-only pages keep their state.
    void Protect(std::uintptr_t begin, std::uintptr_t end, int protection) {
        if (!Available() || end <= begin) return;
        std::lock_guard lock(_mutex);
        for (auto page = begin / _pageBytes; page * _pageBytes < end; ++page) {
            auto* state = slot(page, false);
            if (state == nullptr) continue;
            auto current = state->load(std::memory_order_acquire);
            for (;;) {
                if (current == Unwatched || current == Untracked || current == HostOpen) break;
                if (current == Protecting) {
                    std::this_thread::yield();
                    current = state->load(std::memory_order_acquire);
                } else if (protection == (PROT_READ | PROT_WRITE)) {
                    if (current == Clean) mprotect(reinterpret_cast<void*>(page * _pageBytes), _pageBytes, PROT_READ);
                    break;
                } else if (protection == PROT_READ || state->compare_exchange_weak(current, Untracked, std::memory_order_acq_rel)) {
                    break;
                }
            }
        }
    }

    // The bytes of [begin, end) from `begin` on whose pages share whether this watch holds them
    // write-protected: such pages are read-write to the guest whatever the host reports.
    std::size_t ProtectedRun(std::uintptr_t begin, std::uintptr_t end, bool* watchProtected) {
        const auto held = [&](std::uintptr_t page) {
            auto* state = slot(page, false);
            const auto current = state != nullptr ? state->load(std::memory_order_acquire) : Unwatched;
            return current == Clean || current == Protecting;
        };
        *watchProtected = Available() && held(begin / _pageBytes);
        if (!Available()) return end - begin;
        auto page = begin / _pageBytes + 1;
        while (page * _pageBytes < end && held(page) == *watchProtected) ++page;
        return std::min<std::uintptr_t>(end, page * _pageBytes) - begin;
    }

private:
    enum State : std::uint8_t { Unwatched, Clean, Dirty, Protecting, HostOpen, Untracked };
    static constexpr std::uintptr_t LeafPages = std::uintptr_t{1} << 16;
    static constexpr std::uintptr_t AddressLimit = std::uintptr_t{1} << 47;
    using Leaf = std::array<std::atomic<std::uint8_t>, LeafPages>;

    Watch() : _pageBytes(static_cast<std::uintptr_t>(getpagesize())), _leafCount(AddressLimit / _pageBytes / LeafPages), _leaves(new std::atomic<Leaf*>[_leafCount]()) {
        if (std::getenv("APS5_NO_WRITE_WATCH") != nullptr) return;
        struct sigaction action {};
        action.sa_sigaction = OnFault;
        action.sa_flags = SA_SIGINFO;
        sigemptyset(&action.sa_mask);
        if (sigaction(SIGBUS, &action, &_previousBus) != 0 || sigaction(SIGSEGV, &action, &_previousSegv) != 0) {
            std::fprintf(stderr, "[memory] write watch unavailable: cannot install the fault handler; guest memory is compared instead\n");
            return;
        }
        _available = true;
    }

    // Replaces a page's state once no collect is protecting it, so a collect's protection never lands
    // on a page this watch has let go of; returns the state replaced.
    static std::uint8_t replace(std::atomic<std::uint8_t>& state, std::uint8_t next) {
        auto current = state.load(std::memory_order_acquire);
        for (;;) {
            if (current == Protecting) {
                std::this_thread::yield();
                current = state.load(std::memory_order_acquire);
            } else if (state.compare_exchange_weak(current, next, std::memory_order_acq_rel)) {
                return current;
            }
        }
    }

    std::atomic<std::uint8_t>* slot(std::uintptr_t page, bool create) {
        const auto index = page / LeafPages;
        if (index >= _leafCount) return nullptr;
        auto* leaf = _leaves[index].load(std::memory_order_acquire);
        if (leaf == nullptr && create) {
            leaf = new Leaf();
            _leaves[index].store(leaf, std::memory_order_release);
        }
        return leaf != nullptr ? &(*leaf)[page % LeafPages] : nullptr;
    }

    // The parts of [begin, end) the host maps exactly read-write.
    std::vector<std::pair<std::uintptr_t, std::uintptr_t>> readWrite(std::uintptr_t begin, std::uintptr_t end) const {
        std::vector<std::pair<std::uintptr_t, std::uintptr_t>> runs;
        mach_vm_address_t address = begin;
        while (address < end) {
            mach_vm_size_t size = 0;
            vm_region_basic_info_data_64_t info{};
            mach_msg_type_number_t count = VM_REGION_BASIC_INFO_COUNT_64;
            mach_port_t object = MACH_PORT_NULL;
            if (mach_vm_region(mach_task_self(), &address, &size, VM_REGION_BASIC_INFO_64, reinterpret_cast<vm_region_info_t>(&info), &count, &object) != KERN_SUCCESS || address >= end) break;
            if (info.protection == (VM_PROT_READ | VM_PROT_WRITE)) runs.emplace_back(std::max<std::uintptr_t>(address, begin), std::min<std::uintptr_t>(address + size, end));
            address += size;
        }
        return runs;
    }

    // Async-signal-safe: no allocation, no lock. The protection is lifted before the page is marked
    // written, so a collect that re-protects the page afterwards sees it written and protects it
    // again, and a collect that protected it first loses the page to this store.
    static void OnFault(int signal, siginfo_t* info, void* context) {
        auto& watch = Get();
        const auto address = reinterpret_cast<std::uintptr_t>(info->si_addr);
        if (auto* state = watch.slot(address / watch._pageBytes, false)) {
            auto current = state->load(std::memory_order_acquire);
            if (current != Unwatched && current != Untracked) {
                const auto page = address - address % watch._pageBytes;
                if (mprotect(reinterpret_cast<void*>(page), watch._pageBytes, PROT_READ | PROT_WRITE) == 0) {
                    while (current != HostOpen && current != Unwatched && current != Untracked && !state->compare_exchange_weak(current, Dirty, std::memory_order_acq_rel)) {}
                    watch._faults.fetch_add(1, std::memory_order_relaxed);
                    return;
                }
            }
        }
        const struct sigaction& previous = signal == SIGBUS ? watch._previousBus : watch._previousSegv;
        if ((previous.sa_flags & SA_SIGINFO) != 0 && previous.sa_sigaction != nullptr) return previous.sa_sigaction(signal, info, context);
        if ((previous.sa_flags & SA_SIGINFO) == 0 && previous.sa_handler != SIG_DFL && previous.sa_handler != SIG_IGN) return previous.sa_handler(signal);
        std::signal(signal, SIG_DFL);
    }

    // APS5_PROFILE_DRAW: the [writewatch] line, every 10 s.
    void reportProfile() {
        static const bool profile = std::getenv("APS5_PROFILE_DRAW") != nullptr;
        if (!profile) return;
        const auto now = std::chrono::steady_clock::now();
        auto last = _lastReport.load(std::memory_order_relaxed);
        if (now - last < std::chrono::seconds(10) || !_lastReport.compare_exchange_strong(last, now)) return;
        std::fprintf(stderr, "[writewatch] %llu faults, %llu pages protected, %llu host writes opened (cumulative)\n", static_cast<unsigned long long>(_faults.load()), static_cast<unsigned long long>(_protected.load()), static_cast<unsigned long long>(_hostOpened.load()));
    }

    const std::uintptr_t _pageBytes;
    const std::uintptr_t _leafCount;
    const std::unique_ptr<std::atomic<Leaf*>[]> _leaves;
    bool _available = false;
    struct sigaction _previousBus {}, _previousSegv {};
    std::mutex _mutex;
    std::map<std::uintptr_t, std::uint32_t> _hostWrites;
    std::atomic<std::uint64_t> _faults{0};
    std::atomic<std::uint64_t> _protected{0};
    std::atomic<std::uint64_t> _hostOpened{0};
    std::atomic<std::chrono::steady_clock::time_point> _lastReport{};
};

const bool g_opened = (Watch::Get(), true);
#endif

}

bool GuestWriteWatchAvailable_nid_postfix() {
#ifdef APS5_GUEST_WRITE_WATCH
    return Watch::Get().Available();
#else
    return false;
#endif
}

void GuestWriteWatchRegister_nid_postfix(const void* pointer, std::size_t bytes) {
#ifdef APS5_GUEST_WRITE_WATCH
    const auto begin = reinterpret_cast<std::uintptr_t>(pointer);
    Watch::Get().Register(begin, begin + bytes);
#else
    static_cast<void>(pointer);
    static_cast<void>(bytes);
#endif
}

bool GuestWriteWatchUnregister_nid_postfix(const void* pointer, std::size_t bytes) {
#ifdef APS5_GUEST_WRITE_WATCH
    const auto begin = reinterpret_cast<std::uintptr_t>(pointer);
    return Watch::Get().Unregister(begin, begin + bytes);
#else
    static_cast<void>(pointer);
    static_cast<void>(bytes);
    return false;
#endif
}

bool GuestWriteWatchCovers_nid_postfix(std::uintptr_t address, std::size_t bytes) {
#ifdef APS5_GUEST_WRITE_WATCH
    return bytes != 0 && address + bytes > address && Watch::Get().Covers(address, address + bytes);
#else
    static_cast<void>(address);
    static_cast<void>(bytes);
    return false;
#endif
}

bool GuestWriteWatchCollect_nid_postfix(std::uintptr_t address, std::size_t bytes, void (*written)(void* context, std::uintptr_t begin, std::uintptr_t end), void* context) {
#ifdef APS5_GUEST_WRITE_WATCH
    if (bytes == 0 || address + bytes < address) return false;
    return Watch::Get().Collect(address, address + bytes, written, context);
#else
    static_cast<void>(address);
    static_cast<void>(bytes);
    static_cast<void>(written);
    static_cast<void>(context);
    return false;
#endif
}

bool GuestWriteWatchBeginHostWrite_nid_postfix(void* pointer, std::size_t bytes) {
#if defined(__APPLE__)
    const auto begin = reinterpret_cast<std::uintptr_t>(pointer);
    Watch::Get().BeginHostWrite(begin, begin + bytes);
    return true;
#else
    static_cast<void>(pointer);
    static_cast<void>(bytes);
    return false;
#endif
}

void GuestWriteWatchEndHostWrite_nid_postfix(void* pointer, std::size_t bytes) {
#if defined(__APPLE__)
    const auto begin = reinterpret_cast<std::uintptr_t>(pointer);
    Watch::Get().EndHostWrite(begin, begin + bytes);
#else
    static_cast<void>(pointer);
    static_cast<void>(bytes);
#endif
}

void GuestWriteWatchProtect_nid_postfix(const void* pointer, std::size_t bytes, int protection) {
#if defined(__APPLE__)
    const auto begin = reinterpret_cast<std::uintptr_t>(pointer);
    Watch::Get().Protect(begin, begin + bytes, protection);
#else
    static_cast<void>(pointer);
    static_cast<void>(bytes);
    static_cast<void>(protection);
#endif
}

std::size_t GuestWriteWatchProtectedRun_nid_postfix(std::uintptr_t address, std::size_t bytes, bool* watchProtected) {
#if defined(__APPLE__)
    return Watch::Get().ProtectedRun(address, address + bytes, watchProtected);
#else
    static_cast<void>(address);
    *watchProtected = false;
    return bytes;
#endif
}

}
