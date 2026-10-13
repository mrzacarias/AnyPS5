# libSceDiscMap technical debt

### Unknown function info

- [fJgP+wqifno](../../../core/libs/prx/libSceDiscMap/Export.cpp) (libSceDiscMap) - unknown name. The signature `(path, offset, bytes, int* flags, int*, int*)` and the answer (all three outputs 0, return 0) follow [shadPS4](https://github.com/shadps4-emu/shadPS4/blob/f4531fd9272a98e6e3062fc330adf82b8582f10e/src/core/libraries/disc_map/disc_map.cpp) (`Func_7C980FFB0AA27E7A`); unverified on hardware. Invalid arguments throw, as their error codes are unknown
- [sceDiscMapIsRequestOnHDD](../../../core/libs/prx/libSceDiscMap/Export.cpp) (libSceDiscMap) - the signature `(path, offset, bytes, int*)` and the answer `SCE_DISC_MAP_ERROR_NO_BITMAP_INFO` (0x81100004) for every request follow [shadPS4](https://github.com/shadps4-emu/shadPS4/blob/f4531fd9272a98e6e3062fc330adf82b8582f10e/src/core/libraries/disc_map/disc_map.cpp): installed titles have no disc bitmap. The output is not written. Invalid arguments throw, as their error codes are unknown
