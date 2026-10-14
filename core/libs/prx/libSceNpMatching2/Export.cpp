#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

extern "C" {

int APS5_VABI sceNpMatching2ContextStop(std::uint16_t ctxId) {
    static_cast<void>(ctxId);
    NotImplemented_nid_no_patch("-f6M4caNe8k");
    return {};
}

int APS5_VABI sceNpMatching2RegisterSignalingCallback(std::uint16_t ctxId, void* callback, void* userdata) {
    static_cast<void>(ctxId);
    static_cast<void>(callback);
    static_cast<void>(userdata);
    NotImplemented_nid_no_patch("0UMeWRGnZKA");
    return {};
}

int APS5_VABI sceNpMatching2Initialize(void* param) {
    static_cast<void>(param);
    NotImplemented_nid_no_patch("10t3e5+JPnU");
    return {};
}

int APS5_VABI sceNpMatching2GetRoomDataExternalList(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("26vWrPAWJfM");
    return {};
}

int APS5_VABI sceNpMatching2GetRoomMemberDataInternal(std::uint16_t ctxId, const void* request, const void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("5lhvOqheFBA");
    return {};
}

int APS5_VABI sceNpMatching2ContextStart(std::uint16_t ctxId, std::uint64_t timeout) {
    static_cast<void>(ctxId);
    static_cast<void>(timeout);
    NotImplemented_nid_no_patch("7vjNQ6Z1op0");
    return {};
}

int APS5_VABI sceNpMatching2GetSslMemoryInfo(void) {
    NotImplemented_nid_no_patch("8btynvj0KNA");
    return {};
}

int APS5_VABI sceNpMatching2KickoutRoomMember(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("AUVfU6byg3c");
    return {};
}

int APS5_VABI sceNpMatching2LeaveRoom(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("BD6kfx442Do");
    return {};
}

int APS5_VABI sceNpMatching2SetSignalingOptParam(void) {
    NotImplemented_nid_no_patch("ES3UMUWWj9U");
    return {};
}

int APS5_VABI sceNpMatching2GetUserInfoListA(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("GyI2f9yDUXM");
    return {};
}

int APS5_VABI sceNpMatching2SetRoomMemberDataInternal(std::uint16_t ctxId, const void* request, const void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("HoqTrkS9c5Q");
    return {};
}

int APS5_VABI sceNpMatching2SendRoomMessage(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("Iw2h0Jrrb5U");
    return {};
}

int APS5_VABI sceNpMatching2GetRoomDataInternal(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("Jraxifmoet4");
    return {};
}

int APS5_VABI sceNpMatching2GetRoomMemberIdListLocal(void) {
    NotImplemented_nid_no_patch("KC+GnHzrK2o");
    return {};
}

int APS5_VABI sceNpMatching2GetServerId(std::uint16_t ctxId, std::uint16_t* serverId) {
    static_cast<void>(ctxId);
    static_cast<void>(serverId);
    NotImplemented_nid_no_patch("LhCPctIICxQ");
    return {};
}

int APS5_VABI sceNpMatching2Terminate(void) {
    NotImplemented_nid_no_patch("Mqp3lJ+sjy4");
    return {};
}

int APS5_VABI sceNpMatching2DestroyContext(std::uint16_t ctxId) {
    static_cast<void>(ctxId);
    NotImplemented_nid_no_patch("Nz-ZE7ur32I");
    return {};
}

int APS5_VABI sceNpMatching2SetRoomDataInternal(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("S9D8JSYIrjE");
    return {};
}

int APS5_VABI sceNpMatching2CreateJoinRoomA(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("V6KSpKv9XJE");
    return {};
}

int APS5_VABI sceNpMatching2SearchRoom(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("VqZX7POg2Mk");
    return {};
}

int APS5_VABI sceNpMatching2CreateContextA(const void* param, std::uint16_t* ctxId) {
    static_cast<void>(param);
    static_cast<void>(ctxId);
    NotImplemented_nid_no_patch("ajvzc8e2upo");
    return {};
}

int APS5_VABI sceNpMatching2GetSignalingOptParamLocal(void) {
    NotImplemented_nid_no_patch("cgQhq3E0eGo");
    return {};
}

int APS5_VABI sceNpMatching2RegisterContextCallback(void* callback, void* userdata) {
    static_cast<void>(callback);
    static_cast<void>(userdata);
    NotImplemented_nid_no_patch("fQQfP87I7hs");
    return {};
}

int APS5_VABI sceNpMatching2JoinRoomA(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("gQ6cUriNpgs");
    return {};
}

int APS5_VABI sceNpMatching2GetMemoryInfo(void) {
    NotImplemented_nid_no_patch("gpSAvdheZ0Q");
    return {};
}

int APS5_VABI sceNpMatching2SetUserInfo(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("meEjIdbjAA0");
    return {};
}

int APS5_VABI sceNpMatching2SignalingGetConnectionInfoA(std::uint16_t ctxId, std::uint64_t roomId, std::uint16_t memberId, std::uint32_t infoType, void* connInfo) {
    static_cast<void>(ctxId);
    static_cast<void>(roomId);
    static_cast<void>(memberId);
    static_cast<void>(infoType);
    static_cast<void>(connInfo);
    NotImplemented_nid_no_patch("nNeC3F8-g+4");
    return {};
}

int APS5_VABI sceNpMatching2SendRoomChatMessage(std::uint16_t ctxId, void* reqParam, void* optParam, std::int32_t* reqId) {
    static_cast<void>(ctxId);
    static_cast<void>(reqParam);
    static_cast<void>(optParam);
    static_cast<void>(reqId);
    NotImplemented_nid_no_patch("opDpl74pi2E");
    return {};
}

int APS5_VABI sceNpMatching2RegisterRoomEventCallback(std::uint16_t ctxId, void* callback, void* userdata) {
    static_cast<void>(ctxId);
    static_cast<void>(callback);
    static_cast<void>(userdata);
    NotImplemented_nid_no_patch("p+2EnxmaAMM");
    return {};
}

int APS5_VABI sceNpMatching2SetRoomDataExternal(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("q7GK98-nYSE");
    return {};
}

int APS5_VABI sceNpMatching2GetWorldInfoList(std::uint16_t ctxId, void* request, void* requestOpt, std::uint32_t* requestId) {
    static_cast<void>(ctxId);
    static_cast<void>(request);
    static_cast<void>(requestOpt);
    static_cast<void>(requestId);
    NotImplemented_nid_no_patch("rJNPJqDCpiI");
    return {};
}

int APS5_VABI sceNpMatching2RegisterRoomMessageCallback(std::uint16_t ctxId, void* callback, void* userdata) {
    static_cast<void>(ctxId);
    static_cast<void>(callback);
    static_cast<void>(userdata);
    NotImplemented_nid_no_patch("uBESzz4CQws");
    return {};
}

int APS5_VABI sceNpMatching2GetRoomPasswordLocal(void) {
    NotImplemented_nid_no_patch("vbtWT3lZBOM");
    return {};
}

int APS5_VABI sceNpMatching2SignalingGetPingInfo(std::uint16_t ctxId, const void* reqParam, const void* optParam, std::int32_t* reqId) {
    static_cast<void>(ctxId);
    static_cast<void>(reqParam);
    static_cast<void>(optParam);
    static_cast<void>(reqId);
    NotImplemented_nid_no_patch("wUmwXZHaX1w");
    return {};
}

}
