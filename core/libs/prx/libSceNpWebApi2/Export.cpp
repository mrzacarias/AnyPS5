#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"
#include <atomic>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

// The PSN web API has no service behind it: handles can be created, but requests fail.
static constexpr int SCE_NP_WEBAPI2_ERROR_INVALID_ARGUMENT = static_cast<int>(0x80553402);
static constexpr int SCE_NP_WEBAPI2_ERROR_UNAVAILABLE = static_cast<int>(0x80553406);
static constexpr int SCE_NP_WEBAPI_ERROR_INVALID_ARGUMENT = static_cast<int>(0x80552902);
static constexpr int SCE_NP_WEBAPI_ERROR_INVALID_LIB_CONTEXT_ID = static_cast<int>(0x80552903);
static constexpr int SCE_NP_WEBAPI_ERROR_NOT_SIGNED_IN = static_cast<int>(0x80552907);
static constexpr int SCE_NP_WEBAPI_ERROR_SIGNED_IN_USER_NOT_FOUND = static_cast<int>(0x80552910);
static std::atomic<int> g_nextHandle{1};

struct NpWebApi2MemoryPoolStats {
    size_t pool_size;
    size_t max_inuse_size;
    size_t current_inuse_size;
    int32_t reserved;
};

static std::mutex g_poolsMutex;
static std::unordered_map<int, size_t> g_pools;

extern "C" {

int APS5_VABI sceNpWebApi2AbortRequest(int64_t request_id) {
    (void)request_id;
    return 0;
}

int APS5_VABI sceNpWebApi2AddHttpRequestHeader(int64_t request_id, const char* field_name, const char* value) {
    (void)request_id;
    (void)field_name;
    (void)value;
    return 0;
}

int APS5_VABI sceNpWebApi2AddWebTraceTag() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

void APS5_VABI sceNpWebApi2CheckTimeout(void) {
}

int APS5_VABI sceNpWebApi2CreateRequest(int user_context_id, const char* api_group, const char* path, const char* method, const void* content_parameter, int64_t* request_id) {
    (void)user_context_id;
    (void)api_group;
    (void)path;
    (void)method;
    (void)content_parameter;
    if (!request_id) return SCE_NP_WEBAPI2_ERROR_INVALID_ARGUMENT;
    *request_id = g_nextHandle.fetch_add(1, std::memory_order_relaxed);
    return 0;
}

int APS5_VABI sceNpWebApi2CreateUserContext(int lib_ctx_id, int user_id) {
    (void)lib_ctx_id;
    (void)user_id;
    return g_nextHandle.fetch_add(1, std::memory_order_relaxed);
}

int APS5_VABI sceNpWebApi2DeleteRequest(int64_t request_id) {
    (void)request_id;
    return 0;
}

int APS5_VABI sceNpWebApi2DeleteUserContext(int user_context_id) {
    (void)user_context_id;
    return 0;
}

int APS5_VABI sceNpWebApi2GetHttpResponseHeaderValue(int64_t request_id, const char* field_name, char* value, size_t value_size) {
    (void)request_id;
    (void)field_name;
    (void)value;
    (void)value_size;
    return SCE_NP_WEBAPI2_ERROR_UNAVAILABLE;
}

int APS5_VABI sceNpWebApi2GetHttpResponseHeaderValueLength(int64_t request_id, const char* field_name, size_t* value_length) {
    (void)request_id;
    (void)field_name;
    (void)value_length;
    return SCE_NP_WEBAPI2_ERROR_UNAVAILABLE;
}

int APS5_VABI sceNpWebApi2Initialize(int lib_http_ctx_id, size_t pool_size) {
    (void)lib_http_ctx_id;
    const int lib_ctx_id = g_nextHandle.fetch_add(1, std::memory_order_relaxed);
    std::lock_guard lock(g_poolsMutex);
    g_pools[lib_ctx_id] = pool_size;
    return lib_ctx_id;
}

int APS5_VABI sceNpWebApi2GetMemoryPoolStats(int lib_ctx_id, NpWebApi2MemoryPoolStats* stats) {
    if (stats == nullptr) APS5_INVALID_ARG_EX;
    std::lock_guard lock(g_poolsMutex);
    const auto pool = g_pools.find(lib_ctx_id);
    if (pool == g_pools.end()) throw std::invalid_argument("sceNpWebApi2GetMemoryPoolStats: unknown context");
    *stats = {pool->second, 0, 0, 0};
    return 0;
}

int APS5_VABI sceNpWebApi2PushEventCreateFilter(int lib_ctx_id, int handle_id, const char* np_service_name, uint32_t np_service_label, const void* filter_param, size_t filter_param_num) {
    (void)lib_ctx_id;
    (void)handle_id;
    (void)np_service_name;
    (void)np_service_label;
    (void)filter_param;
    (void)filter_param_num;
    return g_nextHandle.fetch_add(1, std::memory_order_relaxed);
}

int APS5_VABI sceNpWebApi2PushEventCreateHandle(int lib_ctx_id) {
    (void)lib_ctx_id;
    return g_nextHandle.fetch_add(1, std::memory_order_relaxed);
}

int APS5_VABI sceNpWebApi2PushEventAbortHandle(int libCtxId, int handleId) {
    (void)libCtxId;
    (void)handleId;
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceNpWebApi2PushEventDeleteHandle(int lib_ctx_id, int handle_id) {
    (void)lib_ctx_id;
    (void)handle_id;
    return 0;
}

int APS5_VABI sceNpWebApi2PushEventDeletePushContext(int user_context_id, const void* push_context_id) {
    (void)user_context_id;
    (void)push_context_id;
    return 0;
}

int APS5_VABI sceNpWebApi2PushEventRegisterCallback(int user_context_id, int filter_id, void* callback, void* user_arg) {
    (void)user_context_id;
    (void)filter_id;
    (void)callback;
    (void)user_arg;
    return g_nextHandle.fetch_add(1, std::memory_order_relaxed);
}

int APS5_VABI sceNpWebApi2ReadData(int64_t request_id, void* data, size_t size) {
    (void)request_id;
    (void)data;
    (void)size;
    return SCE_NP_WEBAPI2_ERROR_UNAVAILABLE;
}

int APS5_VABI sceNpWebApi2SendRequest(int64_t request_id, const void* data, size_t data_size, NpWebApi2ResponseInformationOption* response_info_option) {
    (void)request_id;
    (void)data;
    (void)data_size;
    (void)response_info_option;
    return SCE_NP_WEBAPI2_ERROR_UNAVAILABLE;
}

int APS5_VABI sceNpWebApi2Terminate(int lib_ctx_id) {
    std::lock_guard lock(g_poolsMutex);
    g_pools.erase(lib_ctx_id);
    return 0;
}

int APS5_VABI sceNpWebApi2PushEventCreatePushContext() {
    return SCE_NP_WEBAPI2_ERROR_UNAVAILABLE;
}

int APS5_VABI sceNpWebApi2PushEventDeleteFilter() {
    return 0;
}

int APS5_VABI sceNpWebApi2PushEventSetHandleTimeout(int lib_ctx_id, int handle_id, uint32_t timeout) {
    (void)handle_id;
    (void)timeout;
    std::lock_guard lock(g_poolsMutex);
    if (!g_pools.contains(lib_ctx_id)) throw std::invalid_argument("sceNpWebApi2PushEventSetHandleTimeout: unknown context");
    return 0;
}

int APS5_VABI sceNpWebApi2PushEventRegisterPushContextCallback() {
    return g_nextHandle.fetch_add(1, std::memory_order_relaxed);
}

int APS5_VABI sceNpWebApi2PushEventStartPushContextCallback() {
    return 0;
}

int APS5_VABI sceNpWebApi2PushEventUnregisterCallback() {
    return 0;
}

int APS5_VABI sceNpWebApi2PushEventUnregisterPushContextCallback() {
    return 0;
}

int APS5_VABI sceNpWebApi2SetRequestTimeout() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}


int APS5_VABI sceNpWebApiInitialize(int libHttpCtxId, std::uint64_t poolSize) {
    static_cast<void>(libHttpCtxId);
    const int libCtxId = g_nextHandle.fetch_add(1, std::memory_order_relaxed);
    std::lock_guard lock(g_poolsMutex);
    g_pools[libCtxId] = poolSize;
    return libCtxId;
}

int APS5_VABI sceNpWebApiTerminate(int libCtxId) {
    std::lock_guard lock(g_poolsMutex);
    g_pools.erase(libCtxId);
    return 0;
}

int APS5_VABI sceNpWebApiGetMemoryPoolStats(int libCtxId, NpWebApi2MemoryPoolStats* stats) {
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
    return g_nextHandle.fetch_add(1, std::memory_order_relaxed);
}

int APS5_VABI sceNpWebApiDeleteContext(int titleUserCtxId) {
    static_cast<void>(titleUserCtxId);
    return 0;
}

int APS5_VABI sceNpWebApiCreateHandle(int libCtxId) {
    static_cast<void>(libCtxId);
    return g_nextHandle.fetch_add(1, std::memory_order_relaxed);
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
    *requestId = g_nextHandle.fetch_add(1, std::memory_order_relaxed);
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
    return g_nextHandle.fetch_add(1, std::memory_order_relaxed);
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
    return g_nextHandle.fetch_add(1, std::memory_order_relaxed);
}

int APS5_VABI sceNpWebApiUnregisterExtdPushEventCallback(int titleUserCtxId, int callbackId) {
    static_cast<void>(titleUserCtxId);
    static_cast<void>(callbackId);
    return 0;
}

std::int32_t APS5_VABI sceNpWebApiUtilityParseNpId(const char* pJsonNpId, void* pNpId) {
    static_cast<void>(pJsonNpId);
    static_cast<void>(pNpId);
    NotImplemented_nid_no_patch("or0e885BlXo");
    return {};
}

}
