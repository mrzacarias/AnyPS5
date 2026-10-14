# libSceGnmDriver technical debt

### Silent stubs

- [sceGnmIsUserPaEnabled, sceGnmDriverCaptureInProgress, sceGnmDriverTraceInProgress, sceRazorIsLoaded, sceGnmValidateOnSubmitEnabled, sceGnmValidateGetVersion](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - answer as retail firmware does in [shadPS4](https://github.com/shadps4-emu/shadPS4/blob/2319a36986265440bbc6394e599296d96e74e3f2/src/core/libraries/gnmdriver/gnmdriver.cpp): no user PA, capture, trace, Razor or submit validation (false, version 0). Their signatures are assumed
- [sceGnmGetGpuCoreClockFrequency, sceGnmGetOffChipTessellationBufferSize](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - return the base PS4 GPU clock (800 MHz) and an 8 MiB off-chip tessellation buffer, as shadPS4 does; Neo mode is not reported
- [sceGnmAreSubmitsAllowed](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - always 1: the system suspend that blocks submissions is not modelled
- [sceGnmSubmitDone](../../../core/libs/prx/libSceGnmDriver/src/Submit.cpp) (libSceGnmDriver) - returns 0 without waiting: the end-of-frame notification only matters to the system suspend, which is not modelled
- [sceGnmFlushGarlic](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - does nothing: guest memory written by the CPU is coherent with the host GPU path
- [sceGnmDestroyWorkloadStream](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - workload streams keep no state, so there is nothing to destroy

### Unknown function info

- [sceGnmAddEqEvent](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - the graphics end-of-pipe event type 0x40 fires on the end-of-pipe interrupts of AgcDriver queue 0, the queue Gnm submissions run on. The compute release-memory event types 0 to 6 are registered but never fire, as compute queues are not mapped yet (`sceGnmMapComputeQueue` throws); other types throw. `sceGnmGetEqEventType` returns the event ident, as the PS5 `sceAgcDriverGetEqEventType` does
- [sceGnmCreateWorkloadStream](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - workload streams only group submissions for profiling. `sceGnmCreateWorkloadStream`, `sceGnmDestroyWorkloadStream`, `sceGnmBeginWorkload` and `sceGnmEndWorkload` return what the decompiled firmware does in [shadPS4](https://github.com/shadps4-emu/shadPS4/blob/2319a36986265440bbc6394e599296d96e74e3f2/src/core/libraries/gnmdriver/gnmdriver.cpp) (one stream, id 1); the error code 3 and the workload encoding are unverified
- [sceGnmSetupMipStatsReport](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmLogicalCuIndexToPhysicalCuIndex](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmMapComputeQueue](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmFindResourcesPublic](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetResourceBaseAddressAndSizeInBytes](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmValidateGetDiagnostics](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmRequestFlipAndSubmitDoneForWorkload](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmMapComputeQueueWithPriority](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmUnmapComputeQueue](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmValidateDisableDiagnostics2](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmComputeWaitSemaphore](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetEqTimeStamp](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmDisableMipStatsReport](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmSetResourceUserData](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmLogicalTcaUnitToPhysical](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetNumTcaUnits](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmValidateResetState](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetResourceName](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmValidationRegisterMemoryCheckCallback](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetOwnerName](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmValidateGetDiagnosticInfo](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmValidateDisableDiagnostics](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetResourceShaderGuid](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetResourceUserData](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmSetWaveLimitMultipliers](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmInsertSetColorMarker](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmDingDong](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetResourceType](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmDingDongForWorkload](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmDrawIndirectCountMulti](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmDriverTriggerCapture](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetPhysicalCounterFromVirtualized](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmRequestMipStatsReportAndReset](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmComputeWaitOnAddress](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetProtectionFaultTimeStamp](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmRequestFlipAndSubmitDone](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmValidateDrawCommandBuffers](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetLastWaitedAddress](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmDrawIndexMultiInstanced](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmSetGsRingSizes](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetShaderStatus](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetShaderProgramBaseAddress](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetGpuBlockStatus](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmValidateDispatchCommandBuffers](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmDebugHardwareStatus](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmDrawOpaqueAuto](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmGetGpuInfoStatus](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmDrawIndexIndirectCountMulti](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceRazorCaptureImmediate](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
- [sceGnmDispatchIndirectOnMec](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - signature from shadPS4
