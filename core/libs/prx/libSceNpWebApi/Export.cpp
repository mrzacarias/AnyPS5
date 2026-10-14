#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"
#include <atomic>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

static constexpr int SCE_NP_WEBAPI_ERROR_INVALID_ARGUMENT = static_cast<int>(0x80552902);
static constexpr int SCE_NP_WEBAPI_ERROR_INVALID_LIB_CONTEXT_ID = static_cast<int>(0x80552903);
static constexpr int SCE_NP_WEBAPI_ERROR_NOT_SIGNED_IN = static_cast<int>(0x80552907);
static constexpr int SCE_NP_WEBAPI_ERROR_SIGNED_IN_USER_NOT_FOUND = static_cast<int>(0x80552910);
static std::atomic<int> g_nextId{1};

struct NpWebApiMemoryPoolStats {
    std::uint64_t poolSize;
    std::uint64_t maxInuseSize;
    std::uint64_t currentInuseSize;
    std::int32_t reserved;
};

static std::mutex g_poolsMutex;
static std::unordered_map<int, std::uint64_t> g_pools;

static int nextId() {
    return g_nextId.fetch_add(1, std::memory_order_relaxed);
}

extern "C" {

int APS5_VABI sceNpWebApiInitialize(int libHttpCtxId, std::uint64_t poolSize) {
    static_cast<void>(libHttpCtxId);
    const int libCtxId = nextId();
    std::lock_guard lock(g_poolsMutex);
    g_pools[libCtxId] = poolSize;
    return libCtxId;
}

int APS5_VABI sceNpWebApiTerminate(int libCtxId) {
    std::lock_guard lock(g_poolsMutex);
    g_pools.erase(libCtxId);
    return 0;
}

int APS5_VABI sceNpWebApiGetMemoryPoolStats(int libCtxId, NpWebApiMemoryPoolStats* stats) {
    if (stats == nullptr) return SCE_NP_WEBAPI_ERROR_INVALID_ARGUMENT;
    std::lock_guard lock(g_poolsMutex);
    const auto pool = g_pools.find(libCtxId);
    if (pool == g_pools.end()) throw std::invalid_argument("sceNpWebApiGetMemoryPoolStats: unknown library context");
    *stats = {pool->second, 0, 0, 0};
    return 0;
}

int APS5_VABI sceNpWebApiCreateContextA(int libCtxId, int userId) {
    static_cast<void>(userId);
    if (libCtxId >= 0x8000) return SCE_NP_WEBAPI_ERROR_INVALID_LIB_CONTEXT_ID;
    return nextId();
}

int APS5_VABI sceNpWebApiDeleteContext(int titleUserCtxId) {
    static_cast<void>(titleUserCtxId);
    return 0;
}

int APS5_VABI sceNpWebApiCreateHandle(int libCtxId) {
    static_cast<void>(libCtxId);
    return nextId();
}

int APS5_VABI sceNpWebApiDeleteHandle(int libCtxId, int handleId) {
    static_cast<void>(libCtxId);
    static_cast<void>(handleId);
    return 0;
}

int APS5_VABI sceNpWebApiCreateRequest(int titleUserCtxId, const char* apiGroup, const char* path, int method, const void* contentParameter, std::int64_t* requestId) {
    static_cast<void>(titleUserCtxId);
    static_cast<void>(method);
    static_cast<void>(contentParameter);
    if (apiGroup == nullptr || path == nullptr || requestId == nullptr) return SCE_NP_WEBAPI_ERROR_INVALID_ARGUMENT;
    *requestId = nextId();
    return 0;
}

int APS5_VABI sceNpWebApiDeleteRequest(std::int64_t requestId) {
    static_cast<void>(requestId);
    return 0;
}

int APS5_VABI sceNpWebApiAbortRequest(std::int64_t requestId) {
    static_cast<void>(requestId);
    return 0;
}

int APS5_VABI sceNpWebApiAddHttpRequestHeader(std::int64_t requestId, const char* fieldName, const char* value) {
    static_cast<void>(requestId);
    if (fieldName == nullptr || value == nullptr) return SCE_NP_WEBAPI_ERROR_INVALID_ARGUMENT;
    return 0;
}

int APS5_VABI sceNpWebApiSendRequest2(std::int64_t requestId, const void* data, std::uint64_t dataSize, void* responseInformation) {
    static_cast<void>(requestId);
    static_cast<void>(data);
    static_cast<void>(dataSize);
    static_cast<void>(responseInformation);
    return SCE_NP_WEBAPI_ERROR_NOT_SIGNED_IN;
}

int APS5_VABI sceNpWebApiReadData(std::int64_t requestId, void* data, std::uint64_t size) {
    static_cast<void>(requestId);
    static_cast<void>(data);
    static_cast<void>(size);
    return SCE_NP_WEBAPI_ERROR_NOT_SIGNED_IN;
}

int APS5_VABI sceNpWebApiGetHttpResponseHeaderValue(std::int64_t requestId, const char* fieldName, char* value, std::uint64_t valueSize) {
    static_cast<void>(requestId);
    static_cast<void>(fieldName);
    static_cast<void>(value);
    static_cast<void>(valueSize);
    return SCE_NP_WEBAPI_ERROR_NOT_SIGNED_IN;
}

int APS5_VABI sceNpWebApiGetHttpResponseHeaderValueLength(std::int64_t requestId, const char* fieldName, std::uint64_t* valueLength) {
    static_cast<void>(requestId);
    static_cast<void>(fieldName);
    static_cast<void>(valueLength);
    return SCE_NP_WEBAPI_ERROR_NOT_SIGNED_IN;
}

int APS5_VABI sceNpWebApiCreateExtdPushEventFilter(int libCtxId, int handleId, const char* npServiceName, std::uint32_t npServiceLabel, const void* filterParameter, std::uint64_t filterParameterCount) {
    static_cast<void>(libCtxId);
    static_cast<void>(handleId);
    if ((npServiceName != nullptr && npServiceLabel == 0xffffffffu) || filterParameter == nullptr || filterParameterCount == 0) return SCE_NP_WEBAPI_ERROR_INVALID_ARGUMENT;
    if (npServiceName != nullptr) return SCE_NP_WEBAPI_ERROR_SIGNED_IN_USER_NOT_FOUND;
    return nextId();
}

int APS5_VABI sceNpWebApiDeleteExtdPushEventFilter(int libCtxId, int filterId) {
    static_cast<void>(libCtxId);
    static_cast<void>(filterId);
    return 0;
}

int APS5_VABI sceNpWebApiRegisterExtdPushEventCallbackA(int titleUserCtxId, int filterId, void* callback, void* userArgument) {
    static_cast<void>(titleUserCtxId);
    static_cast<void>(filterId);
    static_cast<void>(userArgument);
    if (callback == nullptr) return SCE_NP_WEBAPI_ERROR_INVALID_ARGUMENT;
    return nextId();
}

int APS5_VABI sceNpWebApiUnregisterExtdPushEventCallback(int titleUserCtxId, int callbackId) {
    static_cast<void>(titleUserCtxId);
    static_cast<void>(callbackId);
    return 0;
}

int APS5_VABI sceNpWebApiUtilityParseNpId(void) {
    NotImplemented_nid_no_patch("or0e885BlXo");
    return 0;
}

}
