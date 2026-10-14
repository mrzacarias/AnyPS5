#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"
#include "prx/libSceAgcDriver/State/include/Configuration.hpp"
#include "prx/libSceAgcDriver/Eq/include/Event.hpp"
#include <stdexcept>
#include <string>

static constexpr std::uint64_t SCE_GNM_EQ_EVENT_COMPUTE6_REL_MEM = 6;
static constexpr std::uint64_t SCE_GNM_EQ_EVENT_GFX_EOP = 0x40;
static constexpr std::uint32_t UnmappedComputeQueue = 0xFFFFFFFFu;
static constexpr int SCE_GNM_ERROR_FAILURE = static_cast<int>(0x8EEE00FF);

extern "C" {

int APS5_VABI sceGnmSetEmbeddedVsShader(void) {
    NotImplemented_nid_no_patch("+AFvOEXrKJk");
    return 0;
}

int APS5_VABI sceGnmSetupMipStatsReport(void) {
    NotImplemented_nid_no_patch("+xuDhxlWRPg");
    return 0;
}

int APS5_VABI sceGnmDispatchDirect(void) {
    NotImplemented_nid_no_patch("0BzLGljcwBo");
    return 0;
}

int APS5_VABI sceGnmInsertWaitFlipDone(void) {
    NotImplemented_nid_no_patch("1qXLHIpROPE");
    return 0;
}

int APS5_VABI sceGnmLogicalCuIndexToPhysicalCuIndex(void) {
    NotImplemented_nid_no_patch("26PM5Mzl8zc");
    return 0;
}

int APS5_VABI sceGnmMapComputeQueue(void) {
    NotImplemented_nid_no_patch("29oKvKXzEZo");
    return 0;
}

int APS5_VABI sceGnmFindResourcesPublic(void) {
    NotImplemented_nid_no_patch("4Mv9OXypBG8");
    return 0;
}

int APS5_VABI sceGnmGetResourceBaseAddressAndSizeInBytes(void) {
    NotImplemented_nid_no_patch("4PKnYXOhcx4");
    return 0;
}

int APS5_VABI sceGnmDrawIndirect(void) {
    NotImplemented_nid_no_patch("4v+otIIdjqg");
    return 0;
}

int APS5_VABI sceGnmValidateGetDiagnostics(void) {
    NotImplemented_nid_no_patch("5SHGNwLXBV4");
    return 0;
}

int APS5_VABI sceGnmSetPsShader350(void) {
    NotImplemented_nid_no_patch("5uFKckiJYRM");
    return 0;
}

int APS5_VABI sceGnmCreateWorkloadStream(std::uint64_t name, std::uint32_t* workloadStream) {
    if (name == 0 || workloadStream == nullptr) return 3;
    *workloadStream = 1;
    return 0;
}

int APS5_VABI sceGnmQueryResourceRegistrationUserMemoryRequirements(void) {
    return SCE_GNM_ERROR_FAILURE;
}

int APS5_VABI sceGnmRequestFlipAndSubmitDoneForWorkload(void) {
    NotImplemented_nid_no_patch("6YRHhh5mHCs");
    return 0;
}

int APS5_VABI sceGnmInsertPopMarker(void) {
    NotImplemented_nid_no_patch("7qZVNgEu+SY");
    return 0;
}

int APS5_VABI sceGnmRegisterGdsResource(void) {
    return SCE_GNM_ERROR_FAILURE;
}

int APS5_VABI sceGnmMapComputeQueueWithPriority(void) {
    NotImplemented_nid_no_patch("A+uGq+3KFtQ");
    return 0;
}

int APS5_VABI sceGnmUnmapComputeQueue(void) {
    NotImplemented_nid_no_patch("ArSg-TGinhk");
    return 0;
}

int APS5_VABI sceGnmValidateDisableDiagnostics2(void) {
    NotImplemented_nid_no_patch("BgM3t3LvcNk");
    return 0;
}

int APS5_VABI sceGnmDrawIndexIndirect(void) {
    NotImplemented_nid_no_patch("ED9-Fjr8Ta4");
    return 0;
}

int APS5_VABI sceGnmComputeWaitSemaphore(void) {
    NotImplemented_nid_no_patch("EJapNl2+pgU");
    return 0;
}

std::uint32_t APS5_VABI sceGnmGetOffChipTessellationBufferSize(void) {
    return 0x800000u;
}

int APS5_VABI sceGnmSetEsShader(void) {
    NotImplemented_nid_no_patch("FUHG8sQ3R58");
    return 0;
}

int APS5_VABI sceGnmEndWorkload(std::uint64_t workload) {
    if (workload == 0) return 2;
    return ((workload >> 56u) & 0xffu) > 0xf ? 2 : 0;
}

std::uint32_t APS5_VABI sceGnmGetGpuCoreClockFrequency(void) {
    return 800000000u;
}

int APS5_VABI sceGnmDrawIndexAuto(void) {
    NotImplemented_nid_no_patch("GGsn7jMTxw4");
    return 0;
}

int APS5_VABI sceGnmUpdateHsShader(void) {
    NotImplemented_nid_no_patch("GNlx+y7xPdE");
    return 0;
}

int APS5_VABI sceGnmSubmitAndFlipCommandBuffersForWorkload(void) {
    NotImplemented_nid_no_patch("Ga6r7H6Y0RI");
    return 0;
}

int APS5_VABI sceGnmGetEqTimeStamp(void) {
    NotImplemented_nid_no_patch("H7-fgvEutM0");
    return 0;
}

int APS5_VABI sceGnmDisableMipStatsReport(void) {
    NotImplemented_nid_no_patch("HHo1BAljZO8");
    return 0;
}

int APS5_VABI sceGnmDrawIndex(void) {
    NotImplemented_nid_no_patch("HlTPoZ-oY7Y");
    return 0;
}

int APS5_VABI sceGnmValidateGetVersion(void) {
    return 0;
}

int APS5_VABI sceGnmSetResourceUserData(void) {
    NotImplemented_nid_no_patch("K3BKBBYKUSE");
    return 0;
}

int APS5_VABI sceGnmLogicalTcaUnitToPhysical(void) {
    NotImplemented_nid_no_patch("Kl0Z3LH07QI");
    return 0;
}

int APS5_VABI sceGnmGetNumTcaUnits(void) {
    NotImplemented_nid_no_patch("KnldROUkWJY");
    return 0;
}

int APS5_VABI sceGnmSetCsShaderWithModifier(void) {
    NotImplemented_nid_no_patch("Kx-h-nWQJ8A");
    return 0;
}

int APS5_VABI sceGnmValidateResetState(void) {
    NotImplemented_nid_no_patch("MBMa6EFu4Ko");
    return 0;
}

int APS5_VABI sceGnmResetVgtControl(void) {
    NotImplemented_nid_no_patch("MYRtYhojKdA");
    return 0;
}

int APS5_VABI sceGnmInsertDingDongMarker(void) {
    NotImplemented_nid_no_patch("NfvOrNzy6sk");
    return 0;
}

int APS5_VABI sceGnmGetResourceName(void) {
    NotImplemented_nid_no_patch("O0S96YnD04U");
    return 0;
}

int APS5_VABI sceGnmDeleteEqEvent(KernelEqueue eq, std::uint64_t id) {
    if (id != SCE_GNM_EQ_EVENT_GFX_EOP && id > SCE_GNM_EQ_EVENT_COMPUTE6_REL_MEM) throw std::runtime_error("sceGnmDeleteEqEvent: unknown event type " + std::to_string(id));
    return AgcDriverDeleteEqEvent_nid_postfix(eq, static_cast<int>(id));
}

int APS5_VABI sceGnmValidationRegisterMemoryCheckCallback(void) {
    NotImplemented_nid_no_patch("Q7t4VEYLafI");
    return 0;
}

int APS5_VABI sceGnmGetOwnerName(void) {
    NotImplemented_nid_no_patch("QJjPjlmPAL0");
    return 0;
}

bool APS5_VABI sceGnmDriverTraceInProgress(void) {
    return false;
}

std::int32_t APS5_VABI sceGnmLogicalCuMaskToPhysicalCuMask(std::int64_t shaderEngine, std::int32_t logicalCuMask) {
    static_cast<void>(shaderEngine);
    return logicalCuMask;
}

int APS5_VABI sceGnmValidateGetDiagnosticInfo(void) {
    NotImplemented_nid_no_patch("RX7XCNSaL6I");
    return 0;
}

int APS5_VABI sceGnmValidateDisableDiagnostics(void) {
    NotImplemented_nid_no_patch("SXw4dZEkgpA");
    return 0;
}

bool APS5_VABI sceGnmDriverCaptureInProgress(void) {
    return false;
}

int APS5_VABI sceGnmGetResourceShaderGuid(void) {
    NotImplemented_nid_no_patch("UBv7FkVfzcQ");
    return 0;
}

int APS5_VABI sceGnmSetGsShader(void) {
    NotImplemented_nid_no_patch("UJwNuMBcUAk");
    return 0;
}

int APS5_VABI sceGnmGetResourceUserData(void) {
    NotImplemented_nid_no_patch("UoBuWAhKk7U");
    return 0;
}

int APS5_VABI sceGnmGetEqEventType(const KernelEvent* ev) {
    if (ev == nullptr || ev->filter != -14 || (ev->ident != SCE_GNM_EQ_EVENT_GFX_EOP && ev->ident > SCE_GNM_EQ_EVENT_COMPUTE6_REL_MEM)) throw std::runtime_error("sceGnmGetEqEventType: not a Gnm graphics event");
    return static_cast<int>(ev->ident);
}

int APS5_VABI sceGnmDestroyWorkloadStream(std::uint32_t workloadStream) {
    static_cast<void>(workloadStream);
    return 0;
}

int APS5_VABI sceGnmUpdateVsShader(void) {
    NotImplemented_nid_no_patch("V31V01UiScY");
    return 0;
}

int APS5_VABI sceGnmSetHsShader(void) {
    NotImplemented_nid_no_patch("VJNjFtqiF5w");
    return 0;
}

int APS5_VABI sceGnmInsertPushMarker(void) {
    NotImplemented_nid_no_patch("W1Etj-jlW7Y");
    return 0;
}

int APS5_VABI sceGnmSetEmbeddedPsShader(void) {
    NotImplemented_nid_no_patch("X9Omw9dwv5M");
    return 0;
}

int APS5_VABI sceGnmSetWaveLimitMultipliers(void) {
    NotImplemented_nid_no_patch("XiyzNZ9J4nQ");
    return 0;
}

int APS5_VABI sceGnmDispatchIndirect(void) {
    NotImplemented_nid_no_patch("Z43vKp5k7r0");
    return 0;
}

int APS5_VABI sceGnmRegisterOwner(void) {
    return SCE_GNM_ERROR_FAILURE;
}

int APS5_VABI sceGnmInsertPushColorMarker(void) {
    NotImplemented_nid_no_patch("aPIZJTXC+cU");
    return 0;
}

int APS5_VABI sceGnmInsertSetColorMarker(void) {
    NotImplemented_nid_no_patch("aj3L-iaFmyk");
    return 0;
}

int APS5_VABI sceGnmAreSubmitsAllowed(void) {
    return 1;
}

int APS5_VABI sceGnmAddEqEvent(KernelEqueue eq, std::uint64_t id, void* udata) {
    if (id != SCE_GNM_EQ_EVENT_GFX_EOP && id > SCE_GNM_EQ_EVENT_COMPUTE6_REL_MEM) throw std::runtime_error("sceGnmAddEqEvent: unknown event type " + std::to_string(id));
    return AgcDriverAddQueueEqEvent_nid_postfix(eq, static_cast<int>(id), id == SCE_GNM_EQ_EVENT_GFX_EOP ? 0u : UnmappedComputeQueue, udata);
}

int APS5_VABI sceGnmDingDong(void) {
    NotImplemented_nid_no_patch("bX5IbRvECXk");
    return 0;
}

int APS5_VABI sceGnmGetResourceType(void) {
    NotImplemented_nid_no_patch("bdqdvIkLPIU");
    return 0;
}

int APS5_VABI sceGnmDingDongForWorkload(void) {
    NotImplemented_nid_no_patch("byXlqupd8cE");
    return 0;
}

int APS5_VABI sceGnmSetVgtControl(void) {
    NotImplemented_nid_no_patch("cFCp0NX8wf0");
    return 0;
}

int APS5_VABI sceGnmDrawIndirectCountMulti(void) {
    NotImplemented_nid_no_patch("cUCo8OvArrw");
    return 0;
}

int APS5_VABI sceGnmDriverTriggerCapture(void) {
    NotImplemented_nid_no_patch("d88anrgNoKY");
    return 0;
}

int APS5_VABI sceGnmGetPhysicalCounterFromVirtualized(void) {
    NotImplemented_nid_no_patch("dewXw5roLs0");
    return 0;
}

bool APS5_VABI sceRazorIsLoaded(void) {
    return false;
}

int APS5_VABI sceGnmRequestMipStatsReportAndReset(void) {
    NotImplemented_nid_no_patch("f85orjx7qts");
    return 0;
}

int APS5_VABI sceGnmComputeWaitOnAddress(void) {
    NotImplemented_nid_no_patch("ffrNQOshows");
    return 0;
}

int APS5_VABI sceGnmUnregisterOwnerAndResources(void) {
    return SCE_GNM_ERROR_FAILURE;
}

int APS5_VABI sceGnmGetProtectionFaultTimeStamp(void) {
    NotImplemented_nid_no_patch("fzJdEihTFV4");
    return 0;
}

int APS5_VABI sceGnmSetVsShader(void) {
    NotImplemented_nid_no_patch("gAhCn6UiU4Y");
    return 0;
}

int APS5_VABI sceGnmRequestFlipAndSubmitDone(void) {
    NotImplemented_nid_no_patch("gObODli-OH8");
    return 0;
}

int APS5_VABI sceGnmValidateDrawCommandBuffers(void) {
    NotImplemented_nid_no_patch("hsZPf1lON7E");
    return 0;
}

void APS5_VABI sceGnmFlushGarlic(void) {
}

int APS5_VABI sceGnmGetLastWaitedAddress(void) {
    NotImplemented_nid_no_patch("iFirFzgYsvw");
    return 0;
}

int APS5_VABI sceGnmBeginWorkload(std::uint32_t workloadStream, std::uint64_t* workload) {
    if (workload == nullptr) return 3;
    *workload = workloadStream < 0x10 ? 1 : 0;
    return workloadStream > 0xf ? 1 : 0;
}


int APS5_VABI sceGnmRegisterOwnerForSystem(void) {
    return SCE_GNM_ERROR_FAILURE;
}

int APS5_VABI sceGnmDrawIndexMultiInstanced(void) {
    NotImplemented_nid_no_patch("jHdPvIzlpKc");
    return 0;
}

int APS5_VABI sceGnmSubmitCommandBuffersForWorkload(void) {
    NotImplemented_nid_no_patch("jRcI8VcgTz4");
    return 0;
}

bool APS5_VABI sceGnmIsUserPaEnabled(void) {
    return false;
}

int APS5_VABI sceGnmInsertSetMarker(void) {
    NotImplemented_nid_no_patch("jiItzS6+22g");
    return 0;
}

int APS5_VABI sceGnmSetGsRingSizes(void) {
    NotImplemented_nid_no_patch("jtkqXpAOY6w");
    return 0;
}

int APS5_VABI sceGnmGetShaderStatus(void) {
    NotImplemented_nid_no_patch("k7iGTvDQPLQ");
    return 0;
}

int APS5_VABI sceGnmUnregisterResource(void) {
    return SCE_GNM_ERROR_FAILURE;
}

std::uintptr_t APS5_VABI sceGnmGetTheTessellationFactorRingBufferBaseAddress(void) {
    return AgcDriverTessellationFactorRingBase_nid_postfix();
}

int APS5_VABI sceGnmUpdatePsShader350(void) {
    NotImplemented_nid_no_patch("mLVL7N7BVBg");
    return 0;
}

int APS5_VABI sceGnmGetShaderProgramBaseAddress(void) {
    NotImplemented_nid_no_patch("nEyFbYUloIM");
    return 0;
}


int APS5_VABI sceGnmUpdateGsShader(void) {
    NotImplemented_nid_no_patch("nLM2i2+65hA");
    return 0;
}

int APS5_VABI sceGnmRegisterResource(void) {
    return SCE_GNM_ERROR_FAILURE;
}

int APS5_VABI sceGnmGetGpuBlockStatus(void) {
    NotImplemented_nid_no_patch("oL4hGI1PMpw");
    return 0;
}

int APS5_VABI sceGnmDrawIndexOffset(void) {
    NotImplemented_nid_no_patch("oYM+YzfCm2Y");
    return 0;
}

int APS5_VABI sceGnmSetResourceRegistrationUserMemory(void) {
    return SCE_GNM_ERROR_FAILURE;
}

int APS5_VABI sceGnmValidateDispatchCommandBuffers(void) {
    NotImplemented_nid_no_patch("qGP74T5OWJc");
    return 0;
}

int APS5_VABI sceGnmDebugHardwareStatus(void) {
    NotImplemented_nid_no_patch("qpGITzPE+Zc");
    return 0;
}

bool APS5_VABI sceGnmValidateOnSubmitEnabled(void) {
    return false;
}

int APS5_VABI sceGnmDrawOpaqueAuto(void) {
    NotImplemented_nid_no_patch("stDSYW2SBVM");
    return 0;
}

int APS5_VABI sceGnmGetGpuInfoStatus(void) {
    NotImplemented_nid_no_patch("tZCSL5ulnB4");
    return 0;
}

int APS5_VABI sceGnmDrawIndexIndirectCountMulti(void) {
    NotImplemented_nid_no_patch("thbPcG7E7qk");
    return 0;
}

int APS5_VABI sceRazorCaptureImmediate(void) {
    NotImplemented_nid_no_patch("u9YKpRRHe-M");
    return 0;
}

int APS5_VABI sceGnmSetLsShader(void) {
    NotImplemented_nid_no_patch("vckdzbQ46SI");
    return 0;
}

int APS5_VABI sceGnmDispatchIndirectOnMec(void) {
    NotImplemented_nid_no_patch("wED4ZXCFJT0");
    return 0;
}

int APS5_VABI sceGnmSubmitAndFlipCommandBuffers(void) {
    NotImplemented_nid_no_patch("xbxNatawohc");
    return 0;
}


int APS5_VABI sceGnmUnregisterAllResourcesForOwner(void) {
    return SCE_GNM_ERROR_FAILURE;
}

int APS5_VABI sceGnmSubmitDone(void) {
    NotImplemented_nid_no_patch("yvZ73uQUqrk");
    return 0;
}

int APS5_VABI sceGnmSubmitCommandBuffers(void) {
    NotImplemented_nid_no_patch("zwY0YV91TTI");
    return 0;
}

}
