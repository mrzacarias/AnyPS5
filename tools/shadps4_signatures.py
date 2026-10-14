#!/usr/bin/env python3
import argparse
import json
import re
import sys
from pathlib import Path

REGISTRATION = re.compile(r'LIB_FUNCTION\(\s*"([A-Za-z0-9+\-]{11})"\s*,\s*"([^"]+)"\s*,\s*\d+\s*,\s*"([^"]+)"\s*,\s*([A-Za-z_][\w:]*)\s*\)')
ENUM = re.compile(r'\benum\s+(?:class\s+|struct\s+)?([A-Za-z_]\w*)')
ALIAS = re.compile(r'\busing\s+([A-Za-z_]\w*)\s*=\s*([^;]+);')
TYPEDEF = re.compile(r'\btypedef\s+([^;(]+?)\s+([A-Za-z_]\w*)\s*;')

PRIMITIVES = {
    "void": "void", "bool": "bool", "char": "char", "float": "float", "double": "double",
    "int": "int", "unsigned": "unsigned", "unsigned int": "unsigned int", "long": "long", "short": "short",
    "s8": "std::int8_t", "s16": "std::int16_t", "s32": "std::int32_t", "s64": "std::int64_t",
    "u8": "std::uint8_t", "u16": "std::uint16_t", "u32": "std::uint32_t", "u64": "std::uint64_t",
    "int8_t": "std::int8_t", "int16_t": "std::int16_t", "int32_t": "std::int32_t", "int64_t": "std::int64_t",
    "uint8_t": "std::uint8_t", "uint16_t": "std::uint16_t", "uint32_t": "std::uint32_t", "uint64_t": "std::uint64_t",
    "size_t": "std::size_t", "std::size_t": "std::size_t", "uintptr_t": "std::uintptr_t", "intptr_t": "std::intptr_t",
    "VAddr": "std::uint64_t", "PAddr": "std::uint64_t", "f32": "float", "f64": "double", "wchar_t": "wchar_t",
}


def strip_comments(text):
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    return re.sub(r"//[^\n]*", "", text)


def split_top(text, separator=","):
    parts, depth, current = [], 0, []
    for character in text:
        if character in "(<[":
            depth += 1
        elif character in ")>]":
            depth -= 1
        if character == separator and depth == 0:
            parts.append("".join(current))
            current = []
        else:
            current.append(character)
    if "".join(current).strip():
        parts.append("".join(current))
    return [part.strip() for part in parts]


def definitions(text):
    found = {}
    for match in re.finditer(r"\bPS4_SYSV_ABI\s+([A-Za-z_][\w:]*)\s*\(", text):
        name = match.group(1).split("::")[-1]
        start = match.end()
        depth, index = 1, start
        while index < len(text) and depth:
            depth += {"(": 1, ")": -1}.get(text[index], 0)
            index += 1
        params = text[start:index - 1]
        rest = text[index:index + 40].lstrip()
        if not (rest.startswith("{") or rest.startswith("noexcept") or rest.startswith("try")):
            continue
        line_start = max(text.rfind(";", 0, match.start()), text.rfind("}", 0, match.start()), text.rfind("{", 0, match.start()), text.rfind("\n\n", 0, match.start()))
        returned = " ".join(text[line_start + 1:match.start()].split())
        returned = re.sub(r"^(?:static|inline|extern \"C\"|\[\[[^\]]*\]\])\s+", "", returned).strip()
        found[name] = (returned, params)
    return found


def map_type(raw, enums, aliases):
    text = " ".join(raw.replace("\n", " ").split())
    if "(" in text:
        return "void*"
    const = "const " if re.search(r"\bconst\b", text) else ""
    stars = text.count("*")
    reference = "&" in text
    base = re.sub(r"\b(const|volatile|struct|enum|class)\b|[*&]", " ", text)
    base = " ".join(base.split())
    base = re.sub(r"^(?:[A-Za-z_]\w*::)+", "", base)
    for _ in range(4):
        if base in aliases and base not in PRIMITIVES:
            base = aliases[base]
        else:
            break
    if "(" in base:
        return const + "void*" + "*" * stars
    if reference:
        stars += 1
    if base in PRIMITIVES:
        mapped = PRIMITIVES[base]
    elif base in enums and stars == 0:
        return "int"
    elif stars:
        return const + "void" + "*" * stars
    else:
        return None
    return const + mapped + "*" * stars if stars else mapped


def signature(returned, params, enums, aliases):
    result = map_type(returned, enums, aliases) if returned else None
    if result is None:
        return None
    if params.strip() in ("", "void"):
        return {"returns": result, "params": []}
    mapped = []
    for index, param in enumerate(split_top(params)):
        if param == "...":
            return None
        match = re.match(r"^(.*?)([A-Za-z_]\w*)\s*(\[\s*\w*\s*\])?$", param.split("=")[0].strip())
        if match and match.group(1).strip() and not param.rstrip().endswith("*"):
            type_text, name = match.group(1), match.group(2)
            if match.group(3):
                type_text += "*"
        else:
            type_text, name = param, f"arg{index}"
        kind = map_type(type_text, enums, aliases)
        if kind is None or kind == "void":
            return None
        mapped.append([kind, name])
    return {"returns": result, "params": mapped}


def main(argv=None):
    parser = argparse.ArgumentParser(description="Index the PS4 library functions of a shadPS4 checkout by NID with portable signatures")
    parser.add_argument("shadps4", type=Path, help="path to a shadPS4 source checkout")
    parser.add_argument("--out", type=Path, required=True, help="output JSON file")
    arguments = parser.parse_args(argv)
    sources = sorted((arguments.shadps4 / "src").rglob("*.[ch]*"))
    enums, aliases, defined, registrations = set(), {}, {}, []
    for path in sources:
        if path.suffix not in (".cpp", ".h", ".hpp"):
            continue
        text = strip_comments(path.read_text(encoding="utf-8", errors="replace"))
        enums.update(ENUM.findall(text))
        for name, target in ALIAS.findall(text):
            aliases.setdefault(name, " ".join(target.split()))
        for target, name in TYPEDEF.findall(text):
            aliases.setdefault(name, " ".join(target.split()))
        if "core/libraries" in path.as_posix():
            for name, value in definitions(text).items():
                defined.setdefault(name, value)
            registrations.extend(REGISTRATION.findall(text))
    index = {}
    for nid, library, module, function in registrations:
        name = function.split("::")[-1]
        entry = {"name": name, "library": library, "module": module}
        if name in defined:
            typed = signature(*defined[name], enums, aliases)
            if typed is not None:
                entry.update(typed)
        index.setdefault(nid, entry)
    arguments.out.write_text(json.dumps(index, indent=1, sort_keys=True) + "\n", encoding="utf-8")
    typed = sum(1 for entry in index.values() if "returns" in entry)
    print(f"{len(index)} NIDs, {typed} with a portable signature", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
