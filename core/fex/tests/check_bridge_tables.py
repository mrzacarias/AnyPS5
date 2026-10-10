"""Check the bridge's tables against the libraries' sources.

    python3 check_bridge_tables.py <Bridge.cpp> <libraries source directory>

On arm64 the bridge cannot tell from a library function how to pass its arguments: a variadic
function takes its variadic arguments on the stack, and a long double is the guest's x87 value.
Bridge.cpp lists both kinds by name, so a library export that is variadic or uses a long double
must be listed there, with the right number of fixed arguments. arm64 macOS also packs stack
arguments at their natural size instead of in 8-byte slots, so an export whose stack arguments
land elsewhere when packed must be listed in PackedExports with their sizes."""
import pathlib
import re
import sys

DEFINITION = re.compile(r"^([^\n;{}]*?)\b(\w+_nid_postfix)\s*\(([^;{}()]*(?:\([^()]*\)[^;{}()]*)*)\)\s*(?:noexcept\s*)?\{", re.M)
LONG_DOUBLE = re.compile(r"\blong double\b|\bGuestLongDouble\b")
EXPORT = re.compile(r"\bAPS5_VABI\s+(\w+)\s*\(([^;{}()]*(?:\([^()]*\)[^;{}()]*)*)\)\s*(?:noexcept\s*)?(?:try\s*)?\{", re.M)
FLOATING = re.compile(r"^(?:const\s+)?(?:float|double)\b")
NARROW = {"bool": 1, "char": 1, "int8_t": 1, "uint8_t": 1, "short": 2, "int16_t": 2, "uint16_t": 2, "int": 4, "unsigned": 4,
          "unsigned int": 4, "int32_t": 4, "uint32_t": 4}
PACKED_SLOTS = 4


def parameters(text):
    """The parameter list split at its top-level commas."""
    depth, current, result = 0, "", []
    for character in text:
        if character in "(<[":
            depth += 1
        elif character in ")>]":
            depth -= 1
        if character == "," and depth == 0:
            result.append(current.strip())
            current = ""
        else:
            current += character
    if current.strip() and current.strip() != "void":
        result.append(current.strip())
    return result


def stack_sizes(arguments):
    """The sizes of the integer arguments past the eighth, which the bridge passes on the stack."""
    sizes = []
    for argument in [argument for argument in arguments if not FLOATING.match(argument)][8:]:
        if "*" in argument or "&" in argument:
            sizes.append(8)
            continue
        kind = re.sub(r"\b(?:const|volatile)\b|std::", "", argument).split()
        sizes.append(NARROW.get(" ".join(kind[:-1]), 8))
    return sizes


def packing_differs(sizes):
    """Whether natural-size packing puts any stack argument somewhere other than its 8-byte slot."""
    offset = 0
    for slot, size in enumerate(sizes):
        offset = (offset + size - 1) // size * size
        if offset != slot * 8:
            return True
        offset += size
    return False


def main():
    bridge = pathlib.Path(sys.argv[1]).read_text()
    variadic = {name: int(fixed) for name, fixed in re.findall(r'\{"(\w+_nid_postfix)",\s*\w+,\s*(\d+)\}', bridge)}
    x87 = set(re.findall(r'"(\w+_nid_postfix)"', bridge.split("X87Results[]", 1)[1].split(";", 1)[0]))
    packed = {name: [int(size) for size in sizes.split(",")]
              for name, sizes in re.findall(r'\{"(\w+)",\s*\{([\d,\s]+)\}\}', bridge.split("PackedExports[]", 1)[1].split(";", 1)[0])}

    problems = []
    defined_variadic, defined_x87, defined_packed = set(), set(), set()
    for path in sorted(pathlib.Path(sys.argv[2]).rglob("*")):
        if path.suffix not in (".c", ".cpp", ".mm") or "tests" in path.parts:
            continue
        for match in DEFINITION.finditer(path.read_text(errors="ignore")):
            prefix, name, listed = match.groups()
            where = f"{path}:{match.string.count(chr(10), 0, match.start(2)) + 1}"
            arguments = parameters(listed)
            if arguments and arguments[-1] == "...":
                defined_variadic.add(name)
                if name not in variadic:
                    problems.append(f"{where}: {name} is variadic but not in VariadicExports")
                elif variadic[name] != len(arguments) - 1:
                    problems.append(f"{where}: {name} has {len(arguments) - 1} fixed arguments, VariadicExports says {variadic[name]}")
            if LONG_DOUBLE.search(prefix):
                defined_x87.add(name)
                if name not in x87:
                    problems.append(f"{where}: {name} returns a long double but is not in X87Results")
            if any(LONG_DOUBLE.search(argument) for argument in arguments):
                problems.append(f"{where}: {name} takes a long double, which the bridge cannot pass")
        for match in EXPORT.finditer(path.read_text(errors="ignore")):
            name, listed = match.groups()
            where = f"{path}:{match.string.count(chr(10), 0, match.start(1)) + 1}"
            sizes = stack_sizes(parameters(listed))
            if not packing_differs(sizes):
                continue
            defined_packed.add(name)
            if len(sizes) > PACKED_SLOTS:
                problems.append(f"{where}: {name} passes {len(sizes)} arguments on the stack, PackedExports holds {PACKED_SLOTS}")
            elif name not in packed:
                problems.append(f"{where}: {name} has stack arguments of sizes {sizes} but is not in PackedExports")
            elif packed[name] != sizes:
                problems.append(f"{where}: {name} has stack arguments of sizes {sizes}, PackedExports says {packed[name]}")
    problems += [f"VariadicExports lists {name}, which no library defines as variadic" for name in sorted(set(variadic) - defined_variadic)]
    problems += [f"X87Results lists {name}, which no library defines as returning a long double" for name in sorted(x87 - defined_x87)]
    problems += [f"PackedExports lists {name}, whose stack arguments need no repacking" for name in sorted(set(packed) - defined_packed)]

    for problem in problems:
        print(problem)
    if problems:
        return 1
    print(f"{len(defined_variadic)} variadic, {len(defined_x87)} long double and {len(defined_packed)} packed exports are in the bridge's tables")
    return 0


if __name__ == "__main__":
    sys.exit(main())
