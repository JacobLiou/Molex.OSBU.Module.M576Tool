# -*- coding: utf-8 -*-
"""Generate IL vs Run Path map from comm recal_sweeps.csv."""
import argparse
import csv
import json
import re
from pathlib import Path

IL_ROWS = [
    (4, 7, 36, "MPO06-1", "MPO54-1", 0.7323),
    (5, 13, 37, "MPO08-1", "MPO56-1", 0.2707),
    (5, 15, 37, "MPO08-3", "MPO56-3", 0.1696),
    (5, 16, 37, "MPO08-4", "MPO56-4", 0.2545),
    (5, 18, 37, "MPO08-6", "MPO56-6", 0.2094),
    (6, 2, 38, "MPO08-8", "MPO56-8", 0.2404),
    (6, 6, 38, "MPO08-12", "MPO56-12", 0.1885),
    (11, 7, 43, "MPO16-7", "MPO64-7", 0.9973),
    (13, 7, 45, "MPO19-7", "MPO67-7", 0.6051),
    (14, 7, 46, "MPO21-1", "MPO69-1", 2.353),
    (15, 7, 47, "MPO22-7", "MPO70-7", 0.5017),
    (19, 8, 51, "MPO28-8", "MPO76-8", 1.1819),
    (19, 11, 51, "MPO28-11", "MPO76-11", 0.3002),
    (25, 12, 57, "MPO37-12", "MPO85-12", 2.2313),
    (26, 7, 58, "MPO39-1", "MPO87-1", 0.5321),
    (26, 8, 58, "MPO39-2", "MPO87-2", 2.2015),
    (28, 8, 60, "MPO42-2", "MPO90-2", 3.5917),
    (30, 7, 62, "MPO45-1", "MPO93-1", 1.0814),
    (31, 7, 63, "MPO46-7", "MPO94-7", 1.9543),
    (31, 11, 63, "MPO46-11", "MPO94-11", 2.9899),
    (32, 12, 64, "MPO48-6", "MPO96-6", 1.6695),
]


def step_num(c1: int, c2: int) -> int:
    return (c1 - 1) * 18 + c2


def load_comm_by_route(csv_path: Path):
    by_route = {}
    line_no = 1
    with csv_path.open("r", encoding="utf-8-sig", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            line_no += 1
            path = row.get("path", "")
            m = re.search(r"tgt=\d+ c1=(\d+) c2=(\d+) c3=(\d+) c4=(\d+)", path)
            if not m:
                continue
            key = tuple(map(int, m.groups()))
            row["_line"] = line_no
            by_route.setdefault(key, []).append(row)
    return by_route


def summarize_sweeps(rows):
    sweeps = []
    for r in rows:
        path = r["path"]
        slot = int(re.search(r"slot=(\d+)", path).group(1))
        cmd = r["cmd"]
        parts = cmd.split()
        axis = "Y" if len(parts) > 2 and parts[2] == "0" else (
            "X" if len(parts) > 2 and parts[2] == "1" else "?"
        )
        sweeps.append(
            {
                "csv_line": r["_line"],
                "slot": slot,
                "axis": axis,
                "attempt": int(r["attempt"]),
                "peak_ok": int(r["peak_ok"]),
                "code": r["code"],
                "col0": r["c0"],
                "cmd": cmd,
            }
        )
    return sweeps


def build_entries(by_route):
    entries = []
    for c1, c2, c4, mpo_in, mpo_out, il in sorted(IL_ROWS, key=lambda x: -x[5]):
        c3 = c2
        key = (c1, c2, c3, c4)
        rows = by_route.get(key, [])
        sweeps = summarize_sweeps(rows)
        fails = [s for s in sweeps if s["peak_ok"] == 0]
        entries.append(
            {
                "il_max_min": il,
                "ch1_1x64": c1,
                "ch2_mcs1": c2,
                "ch3_mcs2": c3,
                "ch4_1x64": c4,
                "run_path_step": step_num(c1, c2),
                "mpo_in": mpo_in,
                "mpo_out": mpo_out,
                "recal1_slot1": f"RECAL 1 3 {c1} {c2} {c3} {c4}",
                "recal1_slot2": f"RECAL 1 4 {c1} {c2} {c3} {c4}",
                "sweep_count": len(sweeps),
                "sweep_fail_count": len(fails),
                "sweeps": sweeps,
            }
        )
    return entries


def render_md(entries, date_tag: str) -> str:
    lines = [
        f"# IL vs Run Path ({date_tag})",
        "",
        "Sources: validationIL.png + comm recal_sweeps.csv",
        "",
        "Rule: step = (ch1-1)*18 + ch2",
        "",
        "## Summary (MAX-MIN desc)",
        "",
        "| MAX-MIN | step | ch1/ch4 | ch2/ch3 | MPO in->out | sweeps | fails | note |",
        "|--------:|-----:|---------|---------|-------------|--------:|------:|------|",
    ]
    for e in entries:
        fails = [s for s in e["sweeps"] if s["peak_ok"] == 0]
        if fails:
            note = "; ".join(
                f"slot{s['slot']}{s['axis']} att{s['attempt']} {s['code']}" for s in fails
            )
        elif e["sweep_count"] == 0:
            note = "not in comm"
        else:
            note = "all peak_ok=1"
        lines.append(
            f"| {e['il_max_min']:.4f} | {e['run_path_step']}/576 | "
            f"{e['ch1_1x64']}/{e['ch4_1x64']} | {e['ch2_mcs1']}/{e['ch3_mcs2']} | "
            f"{e['mpo_in']}->{e['mpo_out']} | {e['sweep_count']} | {e['sweep_fail_count']} | {note} |"
        )

    lines.extend(["", "## Detail per path", ""])
    for e in entries:
        lines.append(f"### {e['mpo_in']} -> {e['mpo_out']} (IL={e['il_max_min']:.4f})")
        lines.append("")
        lines.append(
            f"- step {e['run_path_step']}/576 | ch1={e['ch1_1x64']} ch2={e['ch2_mcs1']} "
            f"ch3={e['ch3_mcs2']} ch4={e['ch4_1x64']}"
        )
        lines.append(f"- `{e['recal1_slot1']}` / `{e['recal1_slot2']}`")
        lines.append("")
        if not e["sweeps"]:
            lines.append("_No rows._")
            lines.append("")
            continue
        lines.append("| csv_line | slot | axis | att | ok | code | col0 | cmd |")
        lines.append("|---------:|-----:|:---|----:|:--:|:-----|-----:|:-----|")
        for s in e["sweeps"]:
            ok = "Y" if s["peak_ok"] else "N"
            lines.append(
                f"| {s['csv_line']} | {s['slot']} | {s['axis']} | {s['attempt']} | {ok} | "
                f"{s['code']} | {s['col0']} | `{s['cmd']}` |"
            )
        lines.append("")
    return "\n".join(lines) + "\n"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("csv", type=Path)
    ap.add_argument("--out-md", type=Path, required=True)
    ap.add_argument("--out-json", type=Path, required=True)
    args = ap.parse_args()

    date_m = re.search(r"(\d{4}-\d{2}-\d{2})", args.csv.name)
    date_tag = date_m.group(1) if date_m else "unknown"

    by_route = load_comm_by_route(args.csv)
    entries = build_entries(by_route)
    args.out_json.write_text(json.dumps(entries, ensure_ascii=False, indent=2), encoding="utf-8")
    args.out_md.write_text(render_md(entries, date_tag), encoding="utf-8")
    print(f"paths={len(entries)} sweeps={sum(len(e['sweeps']) for e in entries)}")
    print(args.out_md)
    print(args.out_json)


if __name__ == "__main__":
    main()
