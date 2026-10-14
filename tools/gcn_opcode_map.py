#!/usr/bin/env python3
import argparse
import re
import sys
from pathlib import Path

# GCN (GFX7) encodings whose opcodes this map translates, with the RDNA encodings each may land in, in order of preference.
FAMILIES = {
    "SOP2": ["SOP2"], "SOPK": ["SOPK"], "SOP1": ["SOP1"], "SOPC": ["SOPC"], "SOPP": ["SOPP"],
    "VOP2": ["VOP2", "VOP3"], "VOP1": ["VOP1", "VOP3"], "VOPC": ["VOPC", "VOP3"], "VOP3": ["VOP3"],
    "DS": ["DS"], "MUBUF": ["MUBUF"], "MTBUF": ["MTBUF"],
}

# GCN names whose RDNA instruction goes by another name.
ALIASES = {
    "V_SUBB_U32": "VSubCoCiU32",
    "V_SUBBREV_U32": "VSubrevCoCiU32",
    "S_ADDK_I32": "SAddI32",
    "V_QSAD_U8": "VQsadPkU16U8",
    "V_MQSAD_U8": "VMqsadPkU16U8",
    "BUFFER_WBINVL1": "BufferGl0Inv",
    "BUFFER_WBINVL1_SC": "BufferGl0Inv",
}

# GCN shifts that take their operands in the other order: RDNA keeps only the reversed forms.
SWAPPED = {
    "V_LSHR_B32": "VLshrrevB32", "V_ASHR_I32": "VAshrrevI32", "V_LSHL_B32": "VLshlrevB32",
    "V_LSHL_B64": "VLshlrevB64", "V_LSHR_B64": "VLshrrevB64", "V_ASHR_I64": "VAshrrevI64",
}

ENTRY = re.compile(r"\{0x([0-9a-fA-F]+)u,\s*RdnaOpcode::(\w+)")
CASE = re.compile(r"case 0x([0-9a-fA-F]+)u:")
RETURN = re.compile(r"return RdnaOpcode::(\w+);")


def camel(name):
    return "".join(part[:1].upper() + part[1:].lower() for part in name.split("_"))


def gcn_tables(opcodes_h):
    text = opcodes_h.read_text(encoding="utf-8")
    tables = {}
    for family in FAMILIES:
        body = re.search(r"enum class Opcode%s : u32 \{(.*?)\n\};" % family, text, re.S).group(1)
        entries = {}
        for name, number in re.findall(r"^\s*(\w+)\s*=\s*(\d+),", body, re.M):
            if not name.startswith("OP_RANGE"):
                entries.setdefault(int(number), name)
        tables[family] = entries
    return tables


def table_body(text, name):
    match = re.search(r"constexpr \w+ %s\[\] = \{(.*?)\n\};" % name, text, re.S)
    return match.group(1)


def switch_body(text, function):
    start = text.index("RdnaOpcode %s(" % function)
    return text[start:text.index("\n}\n", start)]


def switch_entries(body):
    entries, pending = [], []
    for line in body.splitlines():
        pending += [int(value, 16) for value in CASE.findall(line)]
        returned = RETURN.search(line)
        if returned:
            entries += [(value, returned.group(1)) for value in pending]
            pending = []
    return entries


def rdna_tables(source):
    vector = (source / "RdnaVectorOpDecoder.cpp").read_text(encoding="utf-8")
    scalar = (source / "RdnaScalarOpDecoder.cpp").read_text(encoding="utf-8")
    memory = (source / "RdnaMemoryOpDecoder.cpp").read_text(encoding="utf-8")
    entries = lambda text, name: [(int(number, 16), opcode) for number, opcode in ENTRY.findall(table_body(text, name))]
    vop2 = entries(vector, "vop2Opcodes")
    control = switch_body(memory, "cacheControlOpcode")
    cache_control = control[control.index("\n    }\n"):]
    vopc = entries(vector, "vopcOpcodes")
    # VOP3 opcodes in the order the decoder looks them up: its own table, then the VOPC, VOP2 and VOP1 opcodes it promotes.
    unpromoted = {0x20, 0x21, 0x2C, 0x2D, 0x37, 0x38, 0x02, 0x0D, 0x3C}
    vop3 = entries(vector, "vop3Opcodes") + vopc + [(0x100 + n, op) for n, op in vop2 if n not in unpromoted]
    vop3 += [(0x180 + n, op) for n, op in entries(vector, "vop3EncodedVop1Opcodes")]
    return {
        "VOP2": vop2, "VOP1": entries(vector, "vop1Opcodes"), "VOPC": vopc, "VOP3": vop3,
        "SOP1": switch_entries(switch_body(scalar, "decodeSop1Opcode")), "SOP2": switch_entries(switch_body(scalar, "decodeSop2Opcode")),
        "SOPC": switch_entries(switch_body(scalar, "decodeSopcOpcode")), "SOPK": switch_entries(switch_body(scalar, "decodeSopkOpcode")),
        "SOPP": switch_entries(switch_body(scalar, "decodeSoppOpcode")),
        "DS": entries(memory, "dsOpcodes"), "MUBUF": entries(memory, "mubufOpcodes") + switch_entries(cache_control), "MTBUF": entries(memory, "mtbufOpcodes"),
    }


def find(table, opcode, preferred):
    numbers = [number for number, name in table if name == opcode]
    if not numbers:
        return None
    return preferred if preferred in numbers else numbers[0]


def rdna_name(name):
    if name in ALIASES:
        return ALIASES[name]
    # The SOPK compares decode to the SOPC ones, GCN signaling compares to the plain ones, and GCN spells the integer ne and t compares lg and tru.
    name = re.sub(r"^S_CMPK_", "S_CMP_", re.sub(r"^V_CMPS(X?)_", r"V_CMP\1_", name))
    if re.search(r"_[IU](32|64)$", name):
        name = re.sub(r"^(V_CMPX?)_LG_", r"\1_NE_", re.sub(r"^(V_CMPX?)_TRU_", r"\1_T_", name))
    return camel(name)


def map_opcode(family, number, name, rdna):
    if name in SWAPPED:
        found = find(rdna["VOP3"], SWAPPED[name], -1)
        return ("Vop3Swapped", found) if found is not None else ("None", 0)
    wanted = rdna_name(name)
    for target in FAMILIES[family]:
        found = find(rdna[target], wanted, number)
        if found is not None:
            return ("Same" if target == family else "Vop3", found)
    return ("None", 0)


def render(gcn, rdna):
    lines = [
        "// Generated by tools/gcn_opcode_map.py from the GCN opcode tables of shadPS4 (src/shader_recompiler/frontend/opcodes.h)",
        "// and the RDNA opcode tables of this decoder. Do not edit; rerun the tool.",
        "",
    ]
    counts = {}
    for family, entries in gcn.items():
        lines.append("constexpr GcnOpcode gcn%sOpcodes[] = {" % camel(family))
        for number in sorted(entries):
            target, rdna_number = map_opcode(family, number, entries[number], rdna)
            counts[target] = counts.get(target, 0) + 1
            lines.append('    {0x%02xu, GcnTarget::%s, 0x%02xu, "%s"},' % (number, target, rdna_number, entries[number].lower()))
        lines.append("};")
        lines.append("")
    return "\n".join(lines), counts


def main(argv=None):
    parser = argparse.ArgumentParser(description="Map the GCN opcodes of a shadPS4 checkout onto the RDNA opcodes of the AnyPS5 shader decoder")
    parser.add_argument("shadps4", type=Path, help="path to a shadPS4 source checkout")
    parser.add_argument("--out", type=Path, default=Path(__file__).resolve().parent.parent / "core/shader/recompiler/RdnaDecoder/src/GcnOpcodeMap.inc")
    arguments = parser.parse_args(argv)
    source = Path(__file__).resolve().parent.parent / "core/shader/recompiler/RdnaDecoder/src"
    text, counts = render(gcn_tables(arguments.shadps4 / "src/shader_recompiler/frontend/opcodes.h"), rdna_tables(source))
    arguments.out.write_text(text, encoding="utf-8")
    print(", ".join(f"{count} {target}" for target, count in sorted(counts.items())), file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
