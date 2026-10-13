#include "prx/libSceAgcDriver/Execution/include/Driver/Driver.hpp"
#include "prx/libSceAgcDriver/Execution/include/ProfileOutput.hpp"
#include "prx/libSceAgcDriver/Execution/include/Pm4.hpp"
#include "prx/libSceAgcDriver/Execution/include/VideoOutput.hpp"
#include <algorithm>
#include <cstdlib>
#include <pthread.h>
#include <stdexcept>
#include <thread>

namespace AgcDriver::DriverDetail {

namespace {

// APS5_DRAW_PREFETCH=<threads>: helper threads walk each queue-0 submission ahead of the packet worker
// and build the draw-cache entries of its draws (the capture and materialize of a miss, not the commit),
// so the worker finds an entry to validate instead of capturing the draw itself. A helper prefetches
// every <threads>th draw, at most PrefetchLeadDraws draws ahead of the worker, and never one the worker
// has reached; a draw whose capture meets a pending write is left to the worker.
std::size_t prefetchThreads() {
    static const std::size_t threads = [] {
        const char* text = std::getenv("APS5_DRAW_PREFETCH");
        return text != nullptr ? std::min<std::size_t>(std::strtoull(text, nullptr, 10), 8) : std::size_t{0};
    }();
    return threads;
}

constexpr std::size_t PrefetchLeadDraws = 256;

struct PrefetchJob {
    std::uint64_t generation = 0;
    std::shared_ptr<const Submission> submission;
    std::shared_ptr<const QueueState> state;
};

struct PrefetchState {
    std::mutex mutex;
    std::condition_variable posted;
    PrefetchJob job;
    std::atomic<std::uint64_t> generation{0};
    std::atomic<std::size_t> workerCursor{0};
    bool started = false;
    std::atomic<std::uint64_t> built{0}, skipped{0}, aborted{0}, passed{0}, leadWaits{0};
    std::chrono::steady_clock::time_point reported = std::chrono::steady_clock::now();
};

PrefetchState& prefetchState() {
    static PrefetchState state;
    return state;
}

void report(PrefetchState& state) {
    static const bool profile = std::getenv("APS5_PROFILE_DRAW") != nullptr;
    if (!profile) return;
    const auto now = std::chrono::steady_clock::now();
    {
        std::lock_guard lock(state.mutex);
        if (now - state.reported < std::chrono::seconds(10)) return;
        state.reported = now;
    }
    AgcDriver::ProfilePrint_nid_no_patch("[draw-prefetch] (10 s, %zu threads): %llu entries built, %llu draws skipped (cached, indirect or nothing to draw), %llu aborted (a pending write or a failure), %llu reached by the worker first, %llu waits for the lead\n",
                                         prefetchThreads(), static_cast<unsigned long long>(state.built.exchange(0)), static_cast<unsigned long long>(state.skipped.exchange(0)), static_cast<unsigned long long>(state.aborted.exchange(0)),
                                         static_cast<unsigned long long>(state.passed.exchange(0)), static_cast<unsigned long long>(state.leadWaits.exchange(0)));
}

}

bool& Driver::prefetchingDraw() {
    static thread_local bool prefetching = false;
    return prefetching;
}

void Driver::prefetchDraws(const Submission& submission, const QueueState& queue) {
    const auto threads = prefetchThreads();
    if (threads == 0 || submission.queue != 0) return;
    auto& state = prefetchState();
    {
        std::lock_guard lock(state.mutex);
        state.job = {state.generation.load(std::memory_order_relaxed) + 1, std::make_shared<const Submission>(submission), std::make_shared<const QueueState>(queue)};
        state.workerCursor.store(0, std::memory_order_relaxed);
        state.generation.store(state.job.generation, std::memory_order_release);
        if (!state.started) {
            state.started = true;
            for (std::size_t helper = 0; helper < threads; ++helper) {
                struct Start {
                    Driver* driver;
                    std::size_t helper;
                };
                pthread_attr_t attributes;
                pthread_attr_init(&attributes);
                pthread_attr_setstacksize(&attributes, 64u << 20u);
                pthread_t thread;
                const int created = pthread_create(&thread, &attributes, [](void* argument) -> void* {
                    const auto start = *static_cast<Start*>(argument);
                    delete static_cast<Start*>(argument);
                    start.driver->runDrawPrefetch(start.helper);
                    return nullptr;
                }, new Start{this, helper});
                pthread_attr_destroy(&attributes);
                if (created != 0) throw std::runtime_error("draw prefetch: cannot start a helper thread");
                pthread_detach(thread);
            }
        }
    }
    state.posted.notify_all();
}

void Driver::notePrefetchProgress(std::size_t cursor) {
    if (prefetchThreads() == 0) return;
    prefetchState().workerCursor.store(cursor, std::memory_order_release);
}

void Driver::endPrefetch() {
    if (prefetchThreads() == 0) return;
    prefetchState().generation.fetch_add(1, std::memory_order_release);
}

void Driver::runDrawPrefetch(std::size_t helper) {
    auto& state = prefetchState();
    const auto threads = prefetchThreads();
    prefetchingDraw() = true;
    std::uint64_t done = 0;
    const auto stopRequested = [&] { return stopping.load(std::memory_order_acquire) || shutdownToken.stop_requested(); };
    for (;;) {
        PrefetchJob job;
        {
            std::unique_lock lock(state.mutex);
            while (!state.posted.wait_for(lock, std::chrono::milliseconds(100), [&] { return stopRequested() || (state.job.generation != done && state.job.generation == state.generation.load(std::memory_order_acquire)); })) {
            }
            if (stopRequested()) return;
            job = state.job;
        }
        done = job.generation;
        const auto current = [&] { return state.generation.load(std::memory_order_acquire) == job.generation; };
        const auto& submission = *job.submission;
        const auto& commands = submission.commands;
        QueueState scratch = *job.state;
        std::deque<std::size_t> ahead;
        std::size_t draws = 0;
        for (std::size_t cursor = 0; cursor < commands.size() && current() && !stopRequested();) {
            const auto header = commands[cursor];
            if (Pm4::FillerPacket(header)) {
                ++cursor;
                continue;
            }
            const auto count = Pm4::PacketWords(header);
            if (cursor + count > commands.size()) break;
            const auto packet = std::span(commands).subspan(cursor, count);
            const auto opcode = (header >> 8u) & 0xffu;
            if (header == FlipPacketHeader || header == RenderingWaitPacketHeader || opcode == 0x22 || (Pm4::Predicated(header) && scratch.predication.operation != 0)) break;
            if (Pm4::DrawOpcode(opcode)) {
                ahead.push_back(cursor);
                if (draws++ % threads == helper) {
                    for (;;) {
                        const auto worker = state.workerCursor.load(std::memory_order_acquire);
                        while (!ahead.empty() && ahead.front() <= worker) ahead.pop_front();
                        if (ahead.size() <= PrefetchLeadDraws || !current()) break;
                        ++state.leadWaits;
                        std::this_thread::sleep_for(std::chrono::microseconds(50));
                    }
                    if (!current()) break;
                    if (cursor <= state.workerCursor.load(std::memory_order_acquire)) {
                        ++state.passed;
                    } else {
                        try {
                            std::string rejected;
                            if (draw(scratch, packet, submission, rejected) == DrawVerdict::Drawn) ++state.built;
                            else ++state.skipped;
                        } catch (...) {
                            ++state.aborted;
                        }
                    }
                    if (helper == 0) report(state);
                }
            } else if (Pm4::IndirectRegisterOpcode(opcode)) {
                const auto pairs = submission.registerLists.find(cursor);
                if (pairs == submission.registerLists.end()) break;
                try {
                    Pm4::ExecuteIndirectRegisters(packet, pairs->second, scratch);
                } catch (const std::exception&) {
                    break;
                }
            } else if (opcode != 0x15 && opcode != 0x3f && opcode != 0x42 && opcode != 0x46 && opcode != 0x58 && !Pm4::AccessesMemory(header)) {
                try {
                    Pm4::Execute(packet, scratch);
                } catch (const std::exception&) {
                    break;
                }
            }
            cursor += count;
        }
    }
}

}
