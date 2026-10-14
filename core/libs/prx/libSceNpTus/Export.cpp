#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

extern "C" {

std::int32_t APS5_VABI sceNpTusGetDataForCrossSaveVUser(int reqId, const void* targetVirtualUserId, std::int32_t slotId, void* dataStatus, std::uint64_t dataStatusSize, void* data, std::uint64_t recvSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserId);
    static_cast<void>(slotId);
    static_cast<void>(dataStatus);
    static_cast<void>(dataStatusSize);
    static_cast<void>(data);
    static_cast<void>(recvSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("-LxFGYCJwww");
    return {};
}

int APS5_VABI sceNpTssGetData(void) {
    NotImplemented_nid_no_patch("-SUR+UoLS6c");
    return 0;
}

std::int32_t APS5_VABI sceNpTusTryAndSetVariableA(int reqId, std::uint64_t targetAccountId, std::int32_t slotId, std::int32_t opeType, std::int64_t variable, const std::int64_t* compareValue, const std::uint64_t* isLastChangedAuthor, const void* isLastChangedDate, void* resultVariable, std::uint64_t resultVariableSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountId);
    static_cast<void>(slotId);
    static_cast<void>(opeType);
    static_cast<void>(variable);
    static_cast<void>(compareValue);
    static_cast<void>(isLastChangedAuthor);
    static_cast<void>(isLastChangedDate);
    static_cast<void>(resultVariable);
    static_cast<void>(resultVariableSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("0up4MP1wNtc");
    return {};
}

std::int32_t APS5_VABI sceNpTusSetMultiSlotVariableVUser(int reqId, const void* targetVirtualUserId, std::int32_t* slotIds, std::int64_t* variables, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserId);
    static_cast<void>(slotIds);
    static_cast<void>(variables);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("1Cz0hTJFyh4");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetDataForCrossSave(int reqId, std::uint64_t targetAccountId, std::int32_t slotId, void* dataStatus, std::uint64_t dataStatusSize, void* data, std::uint64_t recvSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountId);
    static_cast<void>(slotId);
    static_cast<void>(dataStatus);
    static_cast<void>(dataStatusSize);
    static_cast<void>(data);
    static_cast<void>(recvSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("1TE3OvH61qo");
    return {};
}

int APS5_VABI sceNpTusCreateNpTitleCtxA(void) {
    NotImplemented_nid_no_patch("1n-dGukBgnY");
    return 0;
}

int APS5_VABI sceNpTusAbortRequest(void) {
    NotImplemented_nid_no_patch("2eq1bMwgZYo");
    return 0;
}

int APS5_VABI sceNpTusCreateRequest(void) {
    NotImplemented_nid_no_patch("3bh2aBvvmvM");
    return 0;
}

std::int32_t APS5_VABI sceNpTusAddAndGetVariableForCrossSaveVUser(int reqId, const void* targetVirtualUserId, std::int32_t slotId, std::int64_t inVariable, const std::uint64_t* isLastChangedAuthor, const void* isLastChangedDate, void* outVariable, std::uint64_t outVariableSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserId);
    static_cast<void>(slotId);
    static_cast<void>(inVariable);
    static_cast<void>(isLastChangedAuthor);
    static_cast<void>(isLastChangedDate);
    static_cast<void>(outVariable);
    static_cast<void>(outVariableSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("4VLlu7EIjzk");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetMultiSlotVariableAVUser(int reqId, const void* targetVirtualUserId, std::int32_t* slotIds, void* variables, std::uint64_t variablesSize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserId);
    static_cast<void>(slotIds);
    static_cast<void>(variables);
    static_cast<void>(variablesSize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("AsziNQ9X2uk");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetFriendsVariableA(int reqId, std::int32_t slotId, std::int32_t includeSelf, std::int32_t sortType, void* variableArray, std::uint64_t variableArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(slotId);
    static_cast<void>(includeSelf);
    static_cast<void>(sortType);
    static_cast<void>(variableArray);
    static_cast<void>(variableArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("C8TY-UnQoXg");
    return {};
}

int APS5_VABI sceNpTusDeleteRequest(void) {
    NotImplemented_nid_no_patch("CcIH40dYS88");
    return 0;
}

std::int32_t APS5_VABI sceNpTusGetMultiUserVariableForCrossSaveVUser(int reqId, const void* targetVirtualUserIdArray, std::int32_t slotId, void* variableArray, std::uint64_t variableArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserIdArray);
    static_cast<void>(slotId);
    static_cast<void>(variableArray);
    static_cast<void>(variableArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("Djuj2+1VNL0");
    return {};
}

std::int32_t APS5_VABI sceNpTusAddAndGetVariableForCrossSave(int reqId, std::uint64_t targetAccountId, std::int32_t slotId, std::int64_t inVariable, const std::uint64_t* isLastChangedAuthor, const void* isLastChangedDate, void* outVariable, std::uint64_t outVariableSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountId);
    static_cast<void>(slotId);
    static_cast<void>(inVariable);
    static_cast<void>(isLastChangedAuthor);
    static_cast<void>(isLastChangedDate);
    static_cast<void>(outVariable);
    static_cast<void>(outVariableSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("EPeq43CQKxY");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetMultiSlotVariableA(int reqId, std::uint64_t targetAccountId, std::int32_t* slotIds, void* variableArray, std::uint64_t variablesSize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountId);
    static_cast<void>(slotIds);
    static_cast<void>(variableArray);
    static_cast<void>(variablesSize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("GDXlRTxgd+M");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetMultiUserVariableA(int reqId, const std::uint64_t* targetAccountIdArray, std::int32_t slotId, void* variableArray, std::uint64_t variableArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountIdArray);
    static_cast<void>(slotId);
    static_cast<void>(variableArray);
    static_cast<void>(variableArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("Gjixv5hqRVY");
    return {};
}

int APS5_VABI sceNpTusDeleteNpTitleCtx(void) {
    NotImplemented_nid_no_patch("H3uq7x0sZOI");
    return 0;
}

std::int32_t APS5_VABI sceNpTusTryAndSetVariableForCrossSave(int reqId, std::uint64_t targetAccountId, std::int32_t slotId, std::int32_t opeType, std::int64_t variable, const std::int64_t* compareValue, const std::uint64_t* isLastChangedAuthor, const void* isLastChangedDate, void* resultVariable, std::uint64_t resultVariableSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountId);
    static_cast<void>(slotId);
    static_cast<void>(opeType);
    static_cast<void>(variable);
    static_cast<void>(compareValue);
    static_cast<void>(isLastChangedAuthor);
    static_cast<void>(isLastChangedDate);
    static_cast<void>(resultVariable);
    static_cast<void>(resultVariableSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("MGvSJEHwyL8");
    return {};
}

std::int32_t APS5_VABI sceNpTusAddAndGetVariableAVUser(int reqId, const void* targetVirtualUserId, std::int32_t slotId, std::int64_t inVariable, const std::uint64_t* isLastChangedAuthor, const void* isLastChangedDate, void* outVariable, std::uint64_t outVariableSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserId);
    static_cast<void>(slotId);
    static_cast<void>(inVariable);
    static_cast<void>(isLastChangedAuthor);
    static_cast<void>(isLastChangedDate);
    static_cast<void>(outVariable);
    static_cast<void>(outVariableSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("Nt1runsPVJc");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetMultiUserVariableForCrossSave(int reqId, const std::uint64_t* targetAccountIdArray, std::int32_t slotId, void* variableArray, std::uint64_t variableArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountIdArray);
    static_cast<void>(slotId);
    static_cast<void>(variableArray);
    static_cast<void>(variableArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("Q5uQeScvTPE");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetFriendsDataStatusForCrossSave(int reqId, std::int32_t slotId, std::int32_t includeSelf, std::int32_t sortType, void* statusArray, std::uint64_t statusArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(slotId);
    static_cast<void>(includeSelf);
    static_cast<void>(sortType);
    static_cast<void>(statusArray);
    static_cast<void>(statusArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("TDoqRD+CE+M");
    return {};
}

std::int32_t APS5_VABI sceNpTusSetDataA(int reqId, std::uint64_t targetAccountId, std::int32_t slotId, std::uint64_t totalSize, std::uint64_t sendSize, const void* data, const void* info, std::uint64_t infoStructSize, const std::uint64_t* isLastChangedAuthor, const void* isLastChangedDate, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountId);
    static_cast<void>(slotId);
    static_cast<void>(totalSize);
    static_cast<void>(sendSize);
    static_cast<void>(data);
    static_cast<void>(info);
    static_cast<void>(infoStructSize);
    static_cast<void>(isLastChangedAuthor);
    static_cast<void>(isLastChangedDate);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("VzxN3tOouj8");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetMultiUserDataStatusForCrossSave(int reqId, const std::uint64_t* targetAccountIdArray, std::int32_t slotId, void* statusArray, std::uint64_t statusArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountIdArray);
    static_cast<void>(slotId);
    static_cast<void>(statusArray);
    static_cast<void>(statusArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("cVeBif6zdZ4");
    return {};
}

std::int32_t APS5_VABI sceNpTusSetMultiSlotVariableA(int reqId, std::uint64_t targetAccountId, std::int32_t* slotIds, const std::int64_t* variables, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountId);
    static_cast<void>(slotIds);
    static_cast<void>(variables);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("cf-WMA0jYCc");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetMultiUserDataStatusAVUser(int reqId, const void* targetVirtualUserIdArray, std::int32_t slotId, void* statusArray, std::uint64_t statusArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserIdArray);
    static_cast<void>(slotId);
    static_cast<void>(statusArray);
    static_cast<void>(statusArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("fJU2TZId210");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetMultiUserVariableAVUser(int reqId, const void* targetVirtualUserIdArray, std::int32_t slotId, void* variableArray, std::uint64_t variableArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserIdArray);
    static_cast<void>(slotId);
    static_cast<void>(variableArray);
    static_cast<void>(variableArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("fVvocpq4mG4");
    return {};
}

std::int32_t APS5_VABI sceNpTusTryAndSetVariableForCrossSaveVUser(int reqId, const void* targetVirtualUserId, std::int32_t slotId, std::int32_t opeType, std::int64_t variable, const std::int64_t* compareValue, const std::uint64_t* isLastChangedAuthor, const void* isLastChangedDate, void* resultVariable, std::uint64_t resultVariableSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserId);
    static_cast<void>(slotId);
    static_cast<void>(opeType);
    static_cast<void>(variable);
    static_cast<void>(compareValue);
    static_cast<void>(isLastChangedAuthor);
    static_cast<void>(isLastChangedDate);
    static_cast<void>(resultVariable);
    static_cast<void>(resultVariableSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("fcCwKpi4CbU");
    return {};
}

std::int32_t APS5_VABI sceNpTusDeleteMultiSlotDataA(int reqId, std::uint64_t targetAccountId, std::int32_t* slotIds, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountId);
    static_cast<void>(slotIds);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("iXzUOM9sXU0");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetDataAVUser(int reqId, const void* targetVirtualUserId, std::int32_t slotId, void* dataStatus, std::uint64_t dataStatusSize, void* data, std::uint64_t recvSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserId);
    static_cast<void>(slotId);
    static_cast<void>(dataStatus);
    static_cast<void>(dataStatusSize);
    static_cast<void>(data);
    static_cast<void>(recvSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("iaH+Sxlw32k");
    return {};
}

std::int32_t APS5_VABI sceNpTusSetDataAVUser(int reqId, const void* targetVirtualUserId, std::int32_t slotId, std::uint64_t totalSize, std::uint64_t sendSize, const void* data, const void* info, std::uint64_t infoStructSize, const std::uint64_t* isLastChangedAuthor, const void* isLastChangedDate, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserId);
    static_cast<void>(slotId);
    static_cast<void>(totalSize);
    static_cast<void>(sendSize);
    static_cast<void>(data);
    static_cast<void>(info);
    static_cast<void>(infoStructSize);
    static_cast<void>(isLastChangedAuthor);
    static_cast<void>(isLastChangedDate);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("kbWqOt3QjKU");
    return {};
}

int APS5_VABI sceNpTssCreateNpTitleCtxA(void) {
    NotImplemented_nid_no_patch("lBtrk+7lk14");
    return 0;
}

std::int32_t APS5_VABI sceNpTusGetMultiUserDataStatusA(int reqId, const std::uint64_t* targetAccountIdArray, std::int32_t slotId, void* statusArray, std::uint64_t statusArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountIdArray);
    static_cast<void>(slotId);
    static_cast<void>(statusArray);
    static_cast<void>(statusArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("lxNDPDnWfMc");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetMultiSlotVariableForCrossSave(int reqId, std::uint64_t targetAccountId, std::int32_t* slotIds, void* variableArray, std::uint64_t variablesSize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountId);
    static_cast<void>(slotIds);
    static_cast<void>(variableArray);
    static_cast<void>(variablesSize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("m9XZnxw9AmE");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetFriendsVariableForCrossSave(int reqId, std::int32_t slotId, std::int32_t includeSelf, std::int32_t sortType, void* variableArray, std::uint64_t variableArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(slotId);
    static_cast<void>(includeSelf);
    static_cast<void>(sortType);
    static_cast<void>(variableArray);
    static_cast<void>(variableArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("mD6s8HtMdpk");
    return {};
}

std::int32_t APS5_VABI sceNpTusTryAndSetVariableAVUser(int reqId, const void* targetVirtualUserId, std::int32_t slotId, std::int32_t opeType, std::int64_t variable, const std::int64_t* compareValue, const std::uint64_t* isLastChangedAuthor, const void* isLastChangedDate, void* resultVariable, std::uint64_t resultVariableSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserId);
    static_cast<void>(slotId);
    static_cast<void>(opeType);
    static_cast<void>(variable);
    static_cast<void>(compareValue);
    static_cast<void>(isLastChangedAuthor);
    static_cast<void>(isLastChangedDate);
    static_cast<void>(resultVariable);
    static_cast<void>(resultVariableSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("oGIcxlUabSA");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetMultiUserDataStatusForCrossSaveVUser(int reqId, const void* targetVirtualUserIdArray, std::int32_t slotId, void* statusArray, std::uint64_t statusArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserIdArray);
    static_cast<void>(slotId);
    static_cast<void>(statusArray);
    static_cast<void>(statusArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("w-c7U0MW2KY");
    return {};
}

std::int32_t APS5_VABI sceNpTusAddAndGetVariableA(int reqId, std::uint64_t targetAccountId, std::int32_t slotId, std::int64_t inVariable, const std::uint64_t* isLastChangedAuthor, const void* isLastChangedDate, void* outVariable, std::uint64_t outVariableSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountId);
    static_cast<void>(slotId);
    static_cast<void>(inVariable);
    static_cast<void>(isLastChangedAuthor);
    static_cast<void>(isLastChangedDate);
    static_cast<void>(outVariable);
    static_cast<void>(outVariableSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("wPFah4-5Xec");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetMultiSlotVariableForCrossSaveVUser(int reqId, const void* virtualUserId, std::int32_t* slotIds, void* variableArray, std::uint64_t variablesSize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(virtualUserId);
    static_cast<void>(slotIds);
    static_cast<void>(variableArray);
    static_cast<void>(variablesSize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("wTuuw4-6HI8");
    return {};
}

std::int32_t APS5_VABI sceNpTusDeleteMultiSlotDataVUser(int reqId, const void* targetVirtualUserId, std::int32_t* slotIds, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetVirtualUserId);
    static_cast<void>(slotIds);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("xutwCvsydkk");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetDataA(int reqId, std::uint64_t targetAccountId, std::int32_t slotId, void* dataStatus, std::uint64_t dataStatusSize, void* data, std::uint64_t recvSize, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(targetAccountId);
    static_cast<void>(slotId);
    static_cast<void>(dataStatus);
    static_cast<void>(dataStatusSize);
    static_cast<void>(data);
    static_cast<void>(recvSize);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("yWEHUFkY1qI");
    return {};
}

std::int32_t APS5_VABI sceNpTusGetFriendsDataStatusA(int reqId, std::int32_t slotId, std::int32_t includeSelf, std::int32_t sortType, void* statusArray, std::uint64_t statusArraySize, int arrayLen, void* option) {
    static_cast<void>(reqId);
    static_cast<void>(slotId);
    static_cast<void>(includeSelf);
    static_cast<void>(sortType);
    static_cast<void>(statusArray);
    static_cast<void>(statusArraySize);
    static_cast<void>(arrayLen);
    static_cast<void>(option);
    NotImplemented_nid_no_patch("yixh7HDKWfk");
    return {};
}

}
