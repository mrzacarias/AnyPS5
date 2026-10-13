# Project technical debt

Entries about one library or component are in [debt](debt/), one file per library or component, under the same sections as this page. Entries that belong to none stay here.

### Build

- The bundled [SPIRV-Tools build](../../core/shader/recompiler/PatchSpirvTools.cmake) replaces the redundancy-elimination source with a build-directory copy that releases processed traversal maps. Remove this patch when the pinned revision includes [SPIRV-Tools #6962](https://github.com/KhronosGroup/SPIRV-Tools/pull/6962). System-dependency builds need the fix in their installed SPIRV-Tools package.
- Building on Windows requires a specific version of mingw - MinGW-w64 GCC 15.2.0 (`winlibs-gcc15`, `x86_64-ucrt-posix-seh`)
- Even compiled prx libraries on Windows require nearby (static linking of these dependencies causes conflicts):
  - libgcc_s_seh-1.dll
  - libstdc++-6.dll
  - libwinpthread-1.dll

### Direct memory

- Windows direct-memory backing reserves a paging-file section and commits each mapped guest page before setting its protection. Committed physical contents remain in the section across unmap/remap and are released when the backing and its remaining views are destroyed; commitment is not recovered merely by unmapping a page. Linux retains its existing backing. Peak gameplay memory and Windows versions other than the tested host remain unverified.

### Silent stubs

Throughout the project, every function at every stage either **does exactly what it's supposed to or throws an exception**. Everywhere... except:

### Unknown function info

- AudioIn and NpSessionSignaling exports added without an implementation have assumed signatures
- `sceNetCtlGetIfStat` signature and 56-byte layout come from PPSA09030's call site and SharpProspero's `SceNetCtlIfStat`; the device kind is always 0, the byte counters are the host's totals over its up non-loopback hardware interfaces (not "since cleared"), and the not-connected result is assumed to match `sceNetCtlGetState`
- Font, Http2, Net, Ssl, SystemService and libkernel exports imported by PPSA12544 and added without an implementation have unknown signatures
- The Share, NpTrophy2, NpEntitlementAccess and WebBrowserDialog exports imported by PPSA12544's Unity plugins and added without an implementation have unknown signatures
- Acm, GameLiveStreaming, Net, NpEntitlementAccess, NpManager, NpWebApi2, VoiceQoS, WebBrowserDialog and libkernel exports imported by PPSA14632 and added without an implementation are declared without parameters

### Functional

- [Cancellation coverage](ARCHITECTURE.md#thread-cancellation) checks deferred request handling and recovery, not full asynchronous support or console ABI compatibility. Cancellation during file/socket I/O, sleep interruption, cancellation versus exit/detach, condition signal conservation under cancellation, and safe rejection of non-null invalid or reclaimed thread handles remain unverified. No console title or homebrew establishes the cleanup NIDs or buffer layout. Accepted asynchronous mode currently has no diagnostic for its deferred behavior.
- `--to-intel` guest module trampolines are covered only by a synthetic relinker test; no game title has been verified with them on Linux or Windows.
- On macOS the [driver](../../core/libs/prx/libSceAgcDriver/Execution/src/VulkanDevice.cpp) turns off MoltenVK's Metal argument buffers (`MVK_CONFIG_USE_METAL_ARGUMENT_BUFFERS`, set through VK_EXT_layer_settings). With them, draws recorded into a batch whose resources are served from host imports of guest memory come out empty: PPSA28997 shows only its videos and full-screen passes, and its sprites, text and UI appear once the draws are synchronous or the descriptors are bound as discrete resources. Why the argument buffers lose those draws is not known. macOS also has no write watch, so CPU stores to GPU-resident surfaces are found by comparing them against their snapshots, which takes a large share of the driver thread in PPSA28997 on an M1 Pro.
- On macOS every f32 add, subtract, multiply and fma the [SPIR-V backend](../../core/shader/recompiler/SpirvBackend/src/SpirvAlu/SpirvAluEmitterMath.cpp) decorates `NoContraction` (so a multiply and an add are rounded apart, as on the console) becomes a call: SPIRV-Cross writes it through `[[clang::optnone]]` helpers (`spvFMul`, `spvFAdd`) that Metal neither inlines nor optimizes. `APS5_ALLOW_F32_CONTRACTION=1` leaves the decoration out, so Metal may fuse a multiply and an add into one rounding. On an M1 Pro it takes PPSA28997's GPU time in gameplay at 1920x1080, with `APS5_NO_TINY_PRODUCT_FLUSH=1` and `APS5_NO_F32_NAN_BITS=1`, from 60 to 24 ms a frame.
- On arm64 hosts the guest's `long double` is x87 extended precision while the host's is a `double` ([X87Extended.hpp](../../core/libs/prx/libc/include/X87Extended.hpp)): `printf` with `L` conversions, `strtold` and `wcstold` handle the x87 value in software, and `scanf` with `L` conversions ([Formatting.cpp](../../core/libs/prx/libc/src/Formatting.cpp), [WindowsScanning.hpp](../../core/libs/prx/libc/include/WindowsScanning.hpp)) throws.
- The arm64 runner [aps5-fex](../../core/fex/src/GuestCpu.cpp) runs guest code through FEXCore from [3rdparty/FEX](../../3rdparty/FEX): Madeira's FEX at its last MIT-licensed commit with fifteen later FEX-Emu fixes, but without FEX-Emu's other changes since FEX-2607, among them its code cache. Under it:
  - a library that calls a function pointer the guest gave it reaches guest code through a fault, which costs microseconds per call. libc's `qsort`, `bsearch` and application heap use [CallProgram](../../core/libs/prx/libc/include/ProgramCall.hpp) instead; libSceFont's allocator, libSceNgs2's callbacks, thread entry functions and signal handlers still go through the fault;
  - [longjmp](../../core/libs/prx/libc/src/JumpBuffer.cpp) and a [fiber switch](../../core/libs/prx/libSceFiber/Export.cpp) that would change the MXCSR or the x87 control word throw, as FEXCore's rounding mode does not follow a change of the saved guest state; a fiber whose entry function returns ends the process;
  - `swprintf` through the [bridge](../../core/fex/src/Bridge.cpp) throws: it formats the host's 32-bit `wchar_t`;
  - `sceKernelLoadStartModule` opens only the modules the executable names, which the [runner](../../core/fex/src/GuestProgram.cpp) loads at start;
  - guest signal handlers, and C++ exceptions that unwind through a host frame between guest frames, are not supported;
  - RDTSCP and RDPID give processor 0, vector loads and stores are not ordered as x86 orders them (FEXCore's default), and flush-to-zero and denormals-are-zero as separate controls were checked only on an M3 Pro, whose cores have FEAT_AFP.
