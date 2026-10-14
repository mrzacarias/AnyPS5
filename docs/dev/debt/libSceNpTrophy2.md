# libSceNpTrophy2 technical debt

### Silent stubs

- [libSceNpTrophy2](../../../core/libs/prx/libSceNpTrophy2/src/GameInfo.cpp): `sceNpTrophy2RegisterUnlockCallback` accepts a callback that is never called, as no trophy is ever unlocked, `sceNpTrophy2UnregisterUnlockCallback` only returns success, and `sceNpTrophy2ShowTrophyList` returns success without showing a list (there is no system trophy UI; seen on PPSA15552).
- [PS4 libSceNpTrophy functions](../../../core/libs/prx/libSceNpTrophy2/src/Ps4.cpp) (libSceNpTrophy2) - the PS4 trophy service is not modelled, as for libSceNpTrophy2: one context and one handle are handed out and always accepted, the unlock state reports no trophy (count 0, no flags), `sceNpTrophyUnlockTrophy` records nothing and reports no platinum (-1), and showing the trophy list does nothing. The error code `SCE_NP_TROPHY_ERROR_INVALID_ARGUMENT` (0x80551604) and the 128-bit flag array follow the PS4 SDK headers as shadPS4 uses them

### Unknown function info

- [sceNpTrophyGetGameIcon](../../../core/libs/prx/libSceNpTrophy2/src/Ps4.cpp) (libSceNpTrophy2) - signature from shadPS4
- [sceNpTrophyGetGameInfo](../../../core/libs/prx/libSceNpTrophy2/src/Ps4.cpp) (libSceNpTrophy2) - signature from shadPS4
- [sceNpTrophyCaptureScreenshot](../../../core/libs/prx/libSceNpTrophy2/src/Ps4.cpp) (libSceNpTrophy2) - signature from shadPS4
- [sceNpTrophyGetTrophyIcon](../../../core/libs/prx/libSceNpTrophy2/src/Ps4.cpp) (libSceNpTrophy2) - signature from shadPS4
- [sceNpTrophyGetTrophyInfo](../../../core/libs/prx/libSceNpTrophy2/src/Ps4.cpp) (libSceNpTrophy2) - signature from shadPS4
- [sceNpTrophyGetGroupIcon](../../../core/libs/prx/libSceNpTrophy2/src/Ps4.cpp) (libSceNpTrophy2) - signature from shadPS4
- [sceNpTrophyGetGroupInfo](../../../core/libs/prx/libSceNpTrophy2/src/Ps4.cpp) (libSceNpTrophy2) - signature from shadPS4
