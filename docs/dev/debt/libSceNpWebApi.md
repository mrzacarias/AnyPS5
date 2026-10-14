# libSceNpWebApi technical debt

### Silent stubs

- [libSceNpWebApi](../../../core/libs/prx/libSceNpWebApi/Export.cpp) (libSceNpWebApi) - the PS4 PSN web API has no service behind it, as in libSceNpWebApi2: library and user contexts, handles, requests, push event filters and callbacks get ids, deletions succeed, and every request answers `SCE_NP_WEBAPI_ERROR_NOT_SIGNED_IN` (0x80552907) when sent or read. A push event filter for a named NP service answers `SCE_NP_WEBAPI_ERROR_SIGNED_IN_USER_NOT_FOUND` (0x80552910), as [shadPS4](https://github.com/shadps4-emu/shadPS4/blob/9dada49956b53aa0fb8dcc9ca81db53ff817e122/src/core/libraries/np/np_web_api/np_web_api_internal.cpp) does offline. Signatures and error codes follow shadPS4

### Unknown function info

- [sceNpWebApiUtilityParseNpId](../../../core/libs/prx/libSceNpWebApi/Export.cpp) (libSceNpWebApi) - unknown signature
