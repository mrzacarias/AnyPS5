#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

extern "C" {

int APS5_VABI sceNpScoreAbortRequest(void) {
    NotImplemented_nid_no_patch("1i7kmKbX6hk");
    return 0;
}

int APS5_VABI sceNpScoreGetRankingByAccountIdForCrossSave(std::int32_t reqId, std::uint32_t boardId, const std::uint64_t* accountIdArray, std::uint64_t accountIdArraySize, void* rankArray, std::uint64_t rankArraySize, void* commentArray, std::uint64_t commentArraySize, void* infoArray, std::uint64_t infoArraySize, std::uint64_t arrayNum, void* lastSortDate, std::uint32_t* totalRecord, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(boardId);
    static_cast<void>(accountIdArray);
    static_cast<void>(accountIdArraySize);
    static_cast<void>(rankArray);
    static_cast<void>(rankArraySize);
    static_cast<void>(commentArray);
    static_cast<void>(commentArraySize);
    static_cast<void>(infoArray);
    static_cast<void>(infoArraySize);
    static_cast<void>(arrayNum);
    static_cast<void>(lastSortDate);
    static_cast<void>(totalRecord);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("3Ybj4E1qNtY");
    return {};
}

int APS5_VABI sceNpScoreGetFriendsRankingForCrossSave(std::int32_t reqId, std::uint32_t boardId, std::int32_t includeSelf, void* rankArray, std::uint64_t rankArraySize, void* commentArray, std::uint64_t commentArraySize, void* infoArray, std::uint64_t infoArraySize, std::uint64_t arrayNum, void* lastSortDate, std::uint32_t* totalRecord, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(boardId);
    static_cast<void>(includeSelf);
    static_cast<void>(rankArray);
    static_cast<void>(rankArraySize);
    static_cast<void>(commentArray);
    static_cast<void>(commentArraySize);
    static_cast<void>(infoArray);
    static_cast<void>(infoArraySize);
    static_cast<void>(arrayNum);
    static_cast<void>(lastSortDate);
    static_cast<void>(totalRecord);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("AgcxgceaH8k");
    return {};
}

int APS5_VABI sceNpScoreDeleteNpTitleCtx(void) {
    NotImplemented_nid_no_patch("G0pE+RNCwfk");
    return 0;
}

int APS5_VABI sceNpScoreCreateNpTitleCtxA(void) {
    NotImplemented_nid_no_patch("GWnWQNXZH5M");
    return 0;
}

std::int32_t APS5_VABI sceNpScoreGetRankingByAccountId(std::int32_t reqId, std::uint32_t boardId, const std::uint64_t* accountIdArray, std::uint64_t accountIdArraySize, void* rankArray, std::uint64_t rankArraySize, void* commentArray, std::uint64_t commentArraySize, void* infoArray, std::uint64_t infoArraySize, std::uint64_t arrayNum, void* lastSortDate, std::uint32_t* totalRecord, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(boardId);
    static_cast<void>(accountIdArray);
    static_cast<void>(accountIdArraySize);
    static_cast<void>(rankArray);
    static_cast<void>(rankArraySize);
    static_cast<void>(commentArray);
    static_cast<void>(commentArraySize);
    static_cast<void>(infoArray);
    static_cast<void>(infoArraySize);
    static_cast<void>(arrayNum);
    static_cast<void>(lastSortDate);
    static_cast<void>(totalRecord);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("K9tlODTQx3c");
    return {};
}

std::int32_t APS5_VABI sceNpScoreGetGameDataByAccountId(std::int32_t reqId, std::uint32_t boardId, std::uint64_t accountId, std::uint64_t* totalSize, std::uint64_t recvSize, void* data, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(boardId);
    static_cast<void>(accountId);
    static_cast<void>(totalSize);
    static_cast<void>(recvSize);
    static_cast<void>(data);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("Lmtc9GljeUA");
    return {};
}

std::int32_t APS5_VABI sceNpScoreGetBoardInfo(std::int32_t reqId, std::uint32_t boardId, void* boardInfo, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(boardId);
    static_cast<void>(boardInfo);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("LoVMVrijVOk");
    return {};
}

std::int32_t APS5_VABI sceNpScoreGetRankingByRangeA(std::int32_t reqId, std::uint32_t boardId, std::uint32_t startSerialRank, void* rankArray, std::uint64_t rankArraySize, void* commentArray, std::uint64_t commentArraySize, void* infoArray, std::uint64_t infoArraySize, std::uint64_t arrayNum, void* lastSortDate, std::uint32_t* totalRecord, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(boardId);
    static_cast<void>(startSerialRank);
    static_cast<void>(rankArray);
    static_cast<void>(rankArraySize);
    static_cast<void>(commentArray);
    static_cast<void>(commentArraySize);
    static_cast<void>(infoArray);
    static_cast<void>(infoArraySize);
    static_cast<void>(arrayNum);
    static_cast<void>(lastSortDate);
    static_cast<void>(totalRecord);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("MA9vSt7JImY");
    return {};
}

std::int32_t APS5_VABI sceNpScoreRecordGameData(std::int32_t reqId, std::uint32_t boardId, std::int64_t score, std::uint64_t totalSize, std::uint64_t sendSize, const void* data, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(boardId);
    static_cast<void>(score);
    static_cast<void>(totalSize);
    static_cast<void>(sendSize);
    static_cast<void>(data);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("bcoVwcBjQ9E");
    return {};
}

std::int32_t APS5_VABI sceNpScoreSetPlayerCharacterId(std::int32_t ctxId, std::int32_t pcId) {
    static_cast<void>(ctxId);
    static_cast<void>(pcId);
    NotImplemented_nid_no_patch("bygbKdHmjn4");
    return {};
}

int APS5_VABI sceNpScoreDeleteRequest(void) {
    NotImplemented_nid_no_patch("dK8-SgYf6r4");
    return 0;
}

std::int32_t APS5_VABI sceNpScoreGetFriendsRankingA(std::int32_t reqId, std::uint32_t boardId, std::int32_t includeSelf, void* rankArray, std::uint64_t rankArraySize, void* commentArray, std::uint64_t commentArraySize, void* infoArray, std::uint64_t infoArraySize, std::uint64_t arrayNum, void* lastSortDate, std::uint32_t* totalRecord, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(boardId);
    static_cast<void>(includeSelf);
    static_cast<void>(rankArray);
    static_cast<void>(rankArraySize);
    static_cast<void>(commentArray);
    static_cast<void>(commentArraySize);
    static_cast<void>(infoArray);
    static_cast<void>(infoArraySize);
    static_cast<void>(arrayNum);
    static_cast<void>(lastSortDate);
    static_cast<void>(totalRecord);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("gMbOn+-6eXA");
    return {};
}

int APS5_VABI sceNpScoreCreateRequest(void) {
    NotImplemented_nid_no_patch("gW8qyjYrUbk");
    return 0;
}

int APS5_VABI sceNpScoreGetRankingByRangeForCrossSave(std::int32_t reqId, std::uint32_t boardId, std::uint32_t startSerialRank, void* rankArray, std::uint64_t rankArraySize, void* commentArray, std::uint64_t commentArraySize, void* infoArray, std::uint64_t infoArraySize, std::uint64_t arrayNum, void* lastSortDate, std::uint32_t* totalRecord, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(boardId);
    static_cast<void>(startSerialRank);
    static_cast<void>(rankArray);
    static_cast<void>(rankArraySize);
    static_cast<void>(commentArray);
    static_cast<void>(commentArraySize);
    static_cast<void>(infoArray);
    static_cast<void>(infoArraySize);
    static_cast<void>(arrayNum);
    static_cast<void>(lastSortDate);
    static_cast<void>(totalRecord);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("nRoYV2yeUuw");
    return {};
}

int APS5_VABI sceNpScoreGetRankingByAccountIdPcIdForCrossSave(std::int32_t reqId, std::uint32_t boardId, const void* idArray, std::uint64_t idArraySize, void* rankArray, std::uint64_t rankArraySize, void* commentArray, std::uint64_t commentArraySize, void* infoArray, std::uint64_t infoArraySize, std::uint64_t arrayNum, void* lastSortDate, std::uint32_t* totalRecord, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(boardId);
    static_cast<void>(idArray);
    static_cast<void>(idArraySize);
    static_cast<void>(rankArray);
    static_cast<void>(rankArraySize);
    static_cast<void>(commentArray);
    static_cast<void>(commentArraySize);
    static_cast<void>(infoArray);
    static_cast<void>(infoArraySize);
    static_cast<void>(arrayNum);
    static_cast<void>(lastSortDate);
    static_cast<void>(totalRecord);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("oXjVieH6ZGQ");
    return {};
}

std::int32_t APS5_VABI sceNpScoreGetRankingByAccountIdPcId(std::int32_t reqId, std::uint32_t boardId, const void* idArray, std::uint64_t idArraySize, void* rankArray, std::uint64_t rankArraySize, void* commentArray, std::uint64_t commentArraySize, void* infoArray, std::uint64_t infoArraySize, std::uint64_t arrayNum, void* lastSortDate, std::uint32_t* totalRecord, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(boardId);
    static_cast<void>(idArray);
    static_cast<void>(idArraySize);
    static_cast<void>(rankArray);
    static_cast<void>(rankArraySize);
    static_cast<void>(commentArray);
    static_cast<void>(commentArraySize);
    static_cast<void>(infoArray);
    static_cast<void>(infoArraySize);
    static_cast<void>(arrayNum);
    static_cast<void>(lastSortDate);
    static_cast<void>(totalRecord);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("wJPWycVGzrs");
    return {};
}

int APS5_VABI sceNpScoreRecordScore(void) {
    NotImplemented_nid_no_patch("zT0XBtgtOSI");
    return 0;
}

}
