# libSceMove technical debt

### Unknown function info

- [libSceMove](../../../core/libs/prx/libSceMove/Export.cpp) (libSceMove) - no PlayStation Move controller is ever connected. The signatures, the error codes and the answers (`sceMoveOpen` returns distinct handles, which libSceVrTracker needs; every device query answers `SCE_MOVE_ERROR_NO_CONTROLLER_CONNECTED`, 1) follow [shadPS4](https://github.com/shadps4-emu/shadPS4/blob/f4531fd9272a98e6e3062fc330adf82b8582f10e/src/core/libraries/move/move.cpp); unverified on hardware
