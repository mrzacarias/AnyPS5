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
- [sceGnmSetupMipStatsReport](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmLogicalCuIndexToPhysicalCuIndex](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmMapComputeQueue](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmFindResourcesPublic](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetResourceBaseAddressAndSizeInBytes](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmValidateGetDiagnostics](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmRequestFlipAndSubmitDoneForWorkload](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmMapComputeQueueWithPriority](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmUnmapComputeQueue](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmValidateDisableDiagnostics2](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmComputeWaitSemaphore](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetEqTimeStamp](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmDisableMipStatsReport](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmSetResourceUserData](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmLogicalTcaUnitToPhysical](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetNumTcaUnits](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmValidateResetState](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetResourceName](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmValidationRegisterMemoryCheckCallback](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetOwnerName](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmValidateGetDiagnosticInfo](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmValidateDisableDiagnostics](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetResourceShaderGuid](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetResourceUserData](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmSetWaveLimitMultipliers](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmInsertSetColorMarker](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmDingDong](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetResourceType](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmDingDongForWorkload](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmDrawIndirectCountMulti](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmDriverTriggerCapture](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetPhysicalCounterFromVirtualized](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmRequestMipStatsReportAndReset](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmComputeWaitOnAddress](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetProtectionFaultTimeStamp](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmRequestFlipAndSubmitDone](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmValidateDrawCommandBuffers](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetLastWaitedAddress](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmDrawIndexMultiInstanced](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmSetGsRingSizes](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetShaderStatus](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetShaderProgramBaseAddress](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetGpuBlockStatus](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmValidateDispatchCommandBuffers](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmDebugHardwareStatus](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmDrawOpaqueAuto](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmGetGpuInfoStatus](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmDrawIndexIndirectCountMulti](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceRazorCaptureImmediate](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
- [sceGnmDispatchIndirectOnMec](../../../core/libs/prx/libSceGnmDriver/Export.cpp) (libSceGnmDriver) - unknown signature
