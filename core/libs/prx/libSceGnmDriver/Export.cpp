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


std::int32_t APS5_VABI sceGnmSetupMipStatsReport(void* output_buffer, std::uint32_t size_in_bytes, std::uint8_t intervals_between_reports, std::uint8_t num_reports_before_reset, std::uint32_t mip_stats_reset_force) {
    static_cast<void>(output_buffer);
    static_cast<void>(size_in_bytes);
    static_cast<void>(intervals_between_reports);
    static_cast<void>(num_reports_before_reset);
    static_cast<void>(mip_stats_reset_force);
    NotImplemented_nid_no_patch("+xuDhxlWRPg");
    return {};
}



int APS5_VABI sceGnmLogicalCuIndexToPhysicalCuIndex(void) {
    NotImplemented_nid_no_patch("26PM5Mzl8zc");
    return {};
}

int APS5_VABI sceGnmMapComputeQueue(std::uint32_t pipe_id, std::uint32_t queue_id, std::uint64_t ring_base_addr, std::uint32_t ring_size_dw, std::uint32_t* read_ptr_addr) {
    static_cast<void>(pipe_id);
    static_cast<void>(queue_id);
    static_cast<void>(ring_base_addr);
    static_cast<void>(ring_size_dw);
    static_cast<void>(read_ptr_addr);
    NotImplemented_nid_no_patch("29oKvKXzEZo");
    return {};
}

std::int32_t APS5_VABI sceGnmFindResourcesPublic(void) {
    NotImplemented_nid_no_patch("4Mv9OXypBG8");
    return {};
}

int APS5_VABI sceGnmGetResourceBaseAddressAndSizeInBytes(void) {
    NotImplemented_nid_no_patch("4PKnYXOhcx4");
    return {};
}


int APS5_VABI sceGnmValidateGetDiagnostics(void) {
    NotImplemented_nid_no_patch("5SHGNwLXBV4");
    return {};
}


int APS5_VABI sceGnmCreateWorkloadStream(std::uint64_t name, std::uint32_t* workloadStream) {
    if (name == 0 || workloadStream == nullptr) return 3;
    *workloadStream = 1;
    return 0;
}

int APS5_VABI sceGnmQueryResourceRegistrationUserMemoryRequirements(void) {
    return SCE_GNM_ERROR_FAILURE;
}

std::int32_t APS5_VABI sceGnmRequestFlipAndSubmitDoneForWorkload(std::uint64_t workload, std::uint32_t* gpu_addr, std::uint32_t gpu_addr_size_in_bytes, std::uint32_t vo_handle, std::uint32_t buf_idx, std::uint32_t flip_mode, std::int64_t flip_arg) {
    static_cast<void>(workload);
    static_cast<void>(gpu_addr);
    static_cast<void>(gpu_addr_size_in_bytes);
    static_cast<void>(vo_handle);
    static_cast<void>(buf_idx);
    static_cast<void>(flip_mode);
    static_cast<void>(flip_arg);
    NotImplemented_nid_no_patch("6YRHhh5mHCs");
    return {};
}


int APS5_VABI sceGnmRegisterGdsResource(void) {
    return SCE_GNM_ERROR_FAILURE;
}

int APS5_VABI sceGnmMapComputeQueueWithPriority(std::uint32_t pipe_id, std::uint32_t queue_id, std::uint64_t ring_base_addr, std::uint32_t ring_size_dw, std::uint32_t* read_ptr_addr, std::uint32_t pipePriority) {
    static_cast<void>(pipe_id);
    static_cast<void>(queue_id);
    static_cast<void>(ring_base_addr);
    static_cast<void>(ring_size_dw);
    static_cast<void>(read_ptr_addr);
    static_cast<void>(pipePriority);
    NotImplemented_nid_no_patch("A+uGq+3KFtQ");
    return {};
}

int APS5_VABI sceGnmUnmapComputeQueue(std::uint32_t vqid) {
    static_cast<void>(vqid);
    NotImplemented_nid_no_patch("ArSg-TGinhk");
    return {};
}

int APS5_VABI sceGnmValidateDisableDiagnostics2(void) {
    NotImplemented_nid_no_patch("BgM3t3LvcNk");
    return {};
}


std::int32_t APS5_VABI sceGnmComputeWaitSemaphore(std::uint32_t* cmdbuf, std::uint32_t size, std::uint64_t sem_addr, std::uint32_t flags) {
    static_cast<void>(cmdbuf);
    static_cast<void>(size);
    static_cast<void>(sem_addr);
    static_cast<void>(flags);
    NotImplemented_nid_no_patch("EJapNl2+pgU");
    return {};
}

std::uint32_t APS5_VABI sceGnmGetOffChipTessellationBufferSize(void) {
    return 0x800000u;
}


int APS5_VABI sceGnmEndWorkload(std::uint64_t workload) {
    if (workload == 0) return 2;
    return ((workload >> 56u) & 0xffu) > 0xf ? 2 : 0;
}

std::uint32_t APS5_VABI sceGnmGetGpuCoreClockFrequency(void) {
    return 800000000u;
}




int APS5_VABI sceGnmGetEqTimeStamp(void) {
    NotImplemented_nid_no_patch("H7-fgvEutM0");
    return {};
}

int APS5_VABI sceGnmDisableMipStatsReport(void) {
    NotImplemented_nid_no_patch("HHo1BAljZO8");
    return {};
}


int APS5_VABI sceGnmValidateGetVersion(void) {
    return 0;
}

int APS5_VABI sceGnmSetResourceUserData(void) {
    NotImplemented_nid_no_patch("K3BKBBYKUSE");
    return {};
}

int APS5_VABI sceGnmLogicalTcaUnitToPhysical(void) {
    NotImplemented_nid_no_patch("Kl0Z3LH07QI");
    return {};
}

int APS5_VABI sceGnmGetNumTcaUnits(void) {
    NotImplemented_nid_no_patch("KnldROUkWJY");
    return {};
}


int APS5_VABI sceGnmValidateResetState(void) {
    NotImplemented_nid_no_patch("MBMa6EFu4Ko");
    return {};
}



int APS5_VABI sceGnmGetResourceName(void) {
    NotImplemented_nid_no_patch("O0S96YnD04U");
    return {};
}

int APS5_VABI sceGnmDeleteEqEvent(KernelEqueue eq, std::uint64_t id) {
    if (id != SCE_GNM_EQ_EVENT_GFX_EOP && id > SCE_GNM_EQ_EVENT_COMPUTE6_REL_MEM) throw std::runtime_error("sceGnmDeleteEqEvent: unknown event type " + std::to_string(id));
    return AgcDriverDeleteEqEvent_nid_postfix(eq, static_cast<int>(id));
}

int APS5_VABI sceGnmValidationRegisterMemoryCheckCallback(void) {
    NotImplemented_nid_no_patch("Q7t4VEYLafI");
    return {};
}

int APS5_VABI sceGnmGetOwnerName(void) {
    NotImplemented_nid_no_patch("QJjPjlmPAL0");
    return {};
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
    return {};
}

int APS5_VABI sceGnmValidateDisableDiagnostics(void) {
    NotImplemented_nid_no_patch("SXw4dZEkgpA");
    return {};
}

bool APS5_VABI sceGnmDriverCaptureInProgress(void) {
    return false;
}

int APS5_VABI sceGnmGetResourceShaderGuid(void) {
    NotImplemented_nid_no_patch("UBv7FkVfzcQ");
    return {};
}


int APS5_VABI sceGnmGetResourceUserData(void) {
    NotImplemented_nid_no_patch("UoBuWAhKk7U");
    return {};
}

int APS5_VABI sceGnmGetEqEventType(const KernelEvent* ev) {
    if (ev == nullptr || ev->filter != -14 || (ev->ident != SCE_GNM_EQ_EVENT_GFX_EOP && ev->ident > SCE_GNM_EQ_EVENT_COMPUTE6_REL_MEM)) throw std::runtime_error("sceGnmGetEqEventType: not a Gnm graphics event");
    return static_cast<int>(ev->ident);
}

int APS5_VABI sceGnmDestroyWorkloadStream(std::uint32_t workloadStream) {
    static_cast<void>(workloadStream);
    return 0;
}





int APS5_VABI sceGnmSetWaveLimitMultipliers(void) {
    NotImplemented_nid_no_patch("XiyzNZ9J4nQ");
    return {};
}


int APS5_VABI sceGnmRegisterOwner(void) {
    return SCE_GNM_ERROR_FAILURE;
}


int APS5_VABI sceGnmInsertSetColorMarker(void) {
    NotImplemented_nid_no_patch("aj3L-iaFmyk");
    return {};
}

int APS5_VABI sceGnmAreSubmitsAllowed(void) {
    return 1;
}

int APS5_VABI sceGnmAddEqEvent(KernelEqueue eq, std::uint64_t id, void* udata) {
    if (id != SCE_GNM_EQ_EVENT_GFX_EOP && id > SCE_GNM_EQ_EVENT_COMPUTE6_REL_MEM) throw std::runtime_error("sceGnmAddEqEvent: unknown event type " + std::to_string(id));
    return AgcDriverAddQueueEqEvent_nid_postfix(eq, static_cast<int>(id), id == SCE_GNM_EQ_EVENT_GFX_EOP ? 0u : UnmappedComputeQueue, udata);
}

void APS5_VABI sceGnmDingDong(std::uint32_t gnm_vqid, std::uint32_t next_offs_dw) {
    static_cast<void>(gnm_vqid);
    static_cast<void>(next_offs_dw);
    NotImplemented_nid_no_patch("bX5IbRvECXk");
}

int APS5_VABI sceGnmGetResourceType(void) {
    NotImplemented_nid_no_patch("bdqdvIkLPIU");
    return {};
}

void APS5_VABI sceGnmDingDongForWorkload(std::uint32_t gnm_vqid, std::uint32_t next_offs_dw, std::uint64_t workload_id) {
    static_cast<void>(gnm_vqid);
    static_cast<void>(next_offs_dw);
    static_cast<void>(workload_id);
    NotImplemented_nid_no_patch("byXlqupd8cE");
}


std::int32_t APS5_VABI sceGnmDrawIndirectCountMulti(std::uint32_t* cmdbuf, std::uint32_t size, std::uint32_t data_offset, std::uint32_t max_count, std::uint64_t count_addr, std::uint32_t shader_stage, std::uint32_t vertex_sgpr_offset, std::uint32_t instance_sgpr_offset, std::uint32_t flags) {
    static_cast<void>(cmdbuf);
    static_cast<void>(size);
    static_cast<void>(data_offset);
    static_cast<void>(max_count);
    static_cast<void>(count_addr);
    static_cast<void>(shader_stage);
    static_cast<void>(vertex_sgpr_offset);
    static_cast<void>(instance_sgpr_offset);
    static_cast<void>(flags);
    NotImplemented_nid_no_patch("cUCo8OvArrw");
    return {};
}

int APS5_VABI sceGnmDriverTriggerCapture(void) {
    NotImplemented_nid_no_patch("d88anrgNoKY");
    return {};
}

int APS5_VABI sceGnmGetPhysicalCounterFromVirtualized(void) {
    NotImplemented_nid_no_patch("dewXw5roLs0");
    return {};
}

bool APS5_VABI sceRazorIsLoaded(void) {
    return false;
}

int APS5_VABI sceGnmRequestMipStatsReportAndReset(void) {
    NotImplemented_nid_no_patch("f85orjx7qts");
    return {};
}

std::int32_t APS5_VABI sceGnmComputeWaitOnAddress(std::uint32_t* cmdbuf, std::uint32_t size, std::uintptr_t addr, std::uint32_t mask, std::uint32_t cmp_func, std::uint32_t ref) {
    static_cast<void>(cmdbuf);
    static_cast<void>(size);
    static_cast<void>(addr);
    static_cast<void>(mask);
    static_cast<void>(cmp_func);
    static_cast<void>(ref);
    NotImplemented_nid_no_patch("ffrNQOshows");
    return {};
}

int APS5_VABI sceGnmUnregisterOwnerAndResources(void) {
    return SCE_GNM_ERROR_FAILURE;
}

std::uint32_t APS5_VABI sceGnmGetProtectionFaultTimeStamp(void) {
    NotImplemented_nid_no_patch("fzJdEihTFV4");
    return {};
}


std::int32_t APS5_VABI sceGnmRequestFlipAndSubmitDone(std::uint32_t* gpu_addr, std::uint32_t gpu_addr_size_in_bytes, std::uint32_t video_out_handle, std::uint32_t display_buffer_index, std::uint32_t flip_mode, std::int64_t flip_arg) {
    static_cast<void>(gpu_addr);
    static_cast<void>(gpu_addr_size_in_bytes);
    static_cast<void>(video_out_handle);
    static_cast<void>(display_buffer_index);
    static_cast<void>(flip_mode);
    static_cast<void>(flip_arg);
    NotImplemented_nid_no_patch("gObODli-OH8");
    return {};
}

int APS5_VABI sceGnmValidateDrawCommandBuffers(void) {
    NotImplemented_nid_no_patch("hsZPf1lON7E");
    return {};
}

void APS5_VABI sceGnmFlushGarlic(void) {
}

int APS5_VABI sceGnmGetLastWaitedAddress(void) {
    NotImplemented_nid_no_patch("iFirFzgYsvw");
    return {};
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
    return {};
}


bool APS5_VABI sceGnmIsUserPaEnabled(void) {
    return false;
}


std::int32_t APS5_VABI sceGnmSetGsRingSizes(std::uint32_t esgs_ring_size, std::uint32_t gsvs_ring_size) {
    static_cast<void>(esgs_ring_size);
    static_cast<void>(gsvs_ring_size);
    NotImplemented_nid_no_patch("jtkqXpAOY6w");
    return {};
}

int APS5_VABI sceGnmGetShaderStatus(void) {
    NotImplemented_nid_no_patch("k7iGTvDQPLQ");
    return {};
}

int APS5_VABI sceGnmUnregisterResource(void) {
    return SCE_GNM_ERROR_FAILURE;
}

std::uintptr_t APS5_VABI sceGnmGetTheTessellationFactorRingBufferBaseAddress(void) {
    return AgcDriverTessellationFactorRingBase_nid_postfix();
}


int APS5_VABI sceGnmGetShaderProgramBaseAddress(void) {
    NotImplemented_nid_no_patch("nEyFbYUloIM");
    return {};
}



int APS5_VABI sceGnmRegisterResource(void) {
    return SCE_GNM_ERROR_FAILURE;
}

int APS5_VABI sceGnmGetGpuBlockStatus(void) {
    NotImplemented_nid_no_patch("oL4hGI1PMpw");
    return {};
}


int APS5_VABI sceGnmSetResourceRegistrationUserMemory(void) {
    return SCE_GNM_ERROR_FAILURE;
}

int APS5_VABI sceGnmValidateDispatchCommandBuffers(void) {
    NotImplemented_nid_no_patch("qGP74T5OWJc");
    return {};
}

int APS5_VABI sceGnmDebugHardwareStatus(void) {
    NotImplemented_nid_no_patch("qpGITzPE+Zc");
    return {};
}

bool APS5_VABI sceGnmValidateOnSubmitEnabled(void) {
    return false;
}

int APS5_VABI sceGnmDrawOpaqueAuto(void) {
    NotImplemented_nid_no_patch("stDSYW2SBVM");
    return {};
}

int APS5_VABI sceGnmGetGpuInfoStatus(void) {
    NotImplemented_nid_no_patch("tZCSL5ulnB4");
    return {};
}

std::int32_t APS5_VABI sceGnmDrawIndexIndirectCountMulti(std::uint32_t* cmdbuf, std::uint32_t size, std::uint32_t data_offset, std::uint32_t max_count, std::uint64_t count_addr, std::uint32_t shader_stage, std::uint32_t vertex_sgpr_offset, std::uint32_t instance_sgpr_offset, std::uint32_t flags) {
    static_cast<void>(cmdbuf);
    static_cast<void>(size);
    static_cast<void>(data_offset);
    static_cast<void>(max_count);
    static_cast<void>(count_addr);
    static_cast<void>(shader_stage);
    static_cast<void>(vertex_sgpr_offset);
    static_cast<void>(instance_sgpr_offset);
    static_cast<void>(flags);
    NotImplemented_nid_no_patch("thbPcG7E7qk");
    return {};
}

int APS5_VABI sceRazorCaptureImmediate(void) {
    NotImplemented_nid_no_patch("u9YKpRRHe-M");
    return {};
}


std::int32_t APS5_VABI sceGnmDispatchIndirectOnMec(std::uint32_t* cmdbuf, std::uint32_t size, std::uint64_t args, std::uint32_t modifier) {
    static_cast<void>(cmdbuf);
    static_cast<void>(size);
    static_cast<void>(args);
    static_cast<void>(modifier);
    NotImplemented_nid_no_patch("wED4ZXCFJT0");
    return {};
}



int APS5_VABI sceGnmUnregisterAllResourcesForOwner(void) {
    return SCE_GNM_ERROR_FAILURE;
}



}
