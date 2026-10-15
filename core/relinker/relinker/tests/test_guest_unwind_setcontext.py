from pathlib import Path
import subprocess
import sys
import tempfile

from test_guest_intel_trampolines import main_fixture
from test_guest_none_relocations import NOOP, module
from test_guest_symbol_names import file_offset


SETCONTEXT = 0x1010
# libunwind's _Ux86_64_setcontext from old SDK libc.prx: sigprocmask(SIG_SETMASK, &uc->uc_sigmask, NULL) as a raw syscall.
SIGPROCMASK = bytes.fromhex("5731d2488d37bf03000000b8540100004989ca0f055f")


def fixture(code):
    image = module([NOOP])
    image[SETCONTEXT:SETCONTEXT + len(code) + 1] = code + b"\xc3"
    return image


def main():
    relinker = Path(sys.argv[1]).resolve()
    with tempfile.TemporaryDirectory(prefix="anyps5-guest-unwind-setcontext-") as directory:
        work = Path(directory)

        def convert(name, image):
            case = work / name
            guest = case / "sce_module" / "module.prx"
            guest.parent.mkdir(parents=True)
            guest.write_bytes(image)
            source = case / "input.elf"
            source.write_bytes(main_fixture())
            result = subprocess.run([str(relinker), str(source), str(case / "output.elf")], capture_output=True, text=True, timeout=10)
            return result, case / "app0" / "sce_module" / "module.prx.guest.prx"

        result, artifact = convert("sigprocmask", fixture(SIGPROCMASK))
        assert result.returncode == 0, (result.stdout, result.stderr)
        data = artifact.read_bytes()
        at = file_offset(data, SETCONTEXT)
        assert data[at:at + len(SIGPROCMASK)] == SIGPROCMASK[:19] + b"\x31\xc0" + SIGPROCMASK[21:], data[at:at + len(SIGPROCMASK)].hex()

        other = SIGPROCMASK[:12] + b"\x55" + SIGPROCMASK[13:]
        result, artifact = convert("other-syscall", fixture(other))
        assert result.returncode == 2 and "Forbidden syscall instruction" in result.stderr, result.stderr
    print("Guest unwind setcontext tests passed")


if __name__ == "__main__":
    main()
