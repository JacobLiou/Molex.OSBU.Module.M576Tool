# -*- coding: utf-8 -*-
"""Extract up to 6-device calib sweeps for bad-IL paths (K7797782)."""
import csv
import re
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
CSV_PATH = HERE / "comm_2026-07-14_recal_sweeps.csv"
OUT_SUMMARY = HERE / "bad_IL_calib_summary.csv"
OUT_DETAIL = HERE / "bad_IL_calib_sweeps_extract.csv"
OUT_MD = HERE / "bad_IL_calib_extract.md"
MAP1 = ROOT / "M576Calibrator" / "output" / "pm_1x64_1Mapping.csv"
MAP2 = ROOT / "M576Calibrator" / "output" / "pm_1x64_2Mapping.csv"

# (il, step, mpo_in, mpo_out, c1, c2, c4) — from 挂机不良通道SN_2026_7_20.xlsx / K7797782_1310, Span>0.15
BAD = [
    (1.0303, 469, "MPO40-1", "MPO88-1", 27, 1, 59),
    (0.5817, 470, "MPO40-2", "MPO88-2", 27, 2, 59),
    (0.2844, 471, "MPO40-3", "MPO88-3", 27, 3, 59),
    (0.3286, 472, "MPO40-4", "MPO88-4", 27, 4, 59),
    (1.0663, 473, "MPO40-5", "MPO88-5", 27, 5, 59),
    (1.9068, 474, "MPO40-6", "MPO88-6", 27, 6, 59),
    (1.9719, 475, "MPO40-7", "MPO88-7", 27, 7, 59),
    (1.0705, 476, "MPO40-8", "MPO88-8", 27, 8, 59),
    (0.3226, 477, "MPO40-9", "MPO88-9", 27, 9, 59),
    (0.2104, 478, "MPO40-10", "MPO88-10", 27, 10, 59),
    (0.5507, 479, "MPO40-11", "MPO88-11", 27, 11, 59),
    (0.9071, 480, "MPO40-12", "MPO88-12", 27, 12, 59),
    (1.0357, 481, "MPO41-1", "MPO89-1", 27, 13, 59),
    (0.7990, 482, "MPO41-2", "MPO89-2", 27, 14, 59),
    (0.6081, 483, "MPO41-3", "MPO89-3", 27, 15, 59),
    (0.8781, 484, "MPO41-4", "MPO89-4", 27, 16, 59),
    (1.6439, 485, "MPO41-5", "MPO89-5", 27, 17, 59),
    (2.4622, 486, "MPO41-6", "MPO89-6", 27, 18, 59),
]

# SW_x -> cascade RECAL 1 for 1# / 2#
CASCADE_1 = {
    2: "RECAL 1 1 15 1 1 47",
    3: "RECAL 1 1 32 1 1 64",
    4: "RECAL 1 1 49 1 1 17",
}
CASCADE_2 = {
    2: "RECAL 1 5 47 1 1 15",
    3: "RECAL 1 5 64 1 1 32",
    4: "RECAL 1 5 17 1 1 49",
}

DEV_CAS1 = "1#SW1-cascade"
DEV_LEAF1 = "1#leaf"
DEV_CAS2 = "2#SW1-cascade"
DEV_LEAF2 = "2#leaf"
DEV_MCS1 = "1#MCS"
DEV_MCS2 = "2#MCS"
DEVICES = [DEV_CAS1, DEV_LEAF1, DEV_CAS2, DEV_LEAF2, DEV_MCS1, DEV_MCS2]

# Chinese labels for markdown
DEV_CN = {
    DEV_CAS1: "1#SW1级联",
    DEV_LEAF1: "1#叶子",
    DEV_CAS2: "2#SW1级联",
    DEV_LEAF2: "2#叶子",
    DEV_MCS1: "1#MCS",
    DEV_MCS2: "2#MCS",
}


def parse_recal1(s):
    if not s or s == "-":
        return None
    m = re.match(r"RECAL 1 (\d+) (\d+) (\d+) (\d+) (\d+)", s)
    return tuple(map(int, m.groups())) if m else None


def fmt_recal(t, c1, c2, c3, c4):
    return "RECAL 1 %d %d %d %d %d" % (t, c1, c2, c3, c4)


def load_mapping(path, sw_col):
    rows = []
    with path.open("r", encoding="utf-8-sig", newline="") as f:
        for row in csv.DictReader(f):
            rows.append({
                "tgt": int(row["target_index"]),
                "c1": int(row["ch1"]),
                "c2": int(row["ch2"]),
                "c3": int(row["ch3"]),
                "c4": int(row["ch4"]),
                "sw": int(row[sw_col]),
                "chy": int(row["CH_y"]),
            })
    return rows


def find_leaf(map_rows, c1, c4, prefer_tgts):
    hits = [r for r in map_rows if r["c1"] == c1 and r["c4"] == c4]
    if not hits:
        return None
    for tgt in prefer_tgts:
        for r in hits:
            if r["tgt"] == tgt:
                return r
    return hits[0]


def resolve_devices(c1, c2, c4, map1, map2):
    leaf1 = find_leaf(map1, c1, c4, prefer_tgts=(2, 1))
    leaf2 = find_leaf(map2, c1, c4, prefer_tgts=(6, 5))
    if not leaf1 or not leaf2:
        raise RuntimeError("leaf not found for c1=%s c4=%s" % (c1, c4))

    leaf1_cmd = fmt_recal(leaf1["tgt"], leaf1["c1"], leaf1["c2"], leaf1["c3"], leaf1["c4"])
    leaf2_cmd = fmt_recal(leaf2["tgt"], leaf2["c1"], leaf2["c2"], leaf2["c3"], leaf2["c4"])
    return {
        DEV_CAS1: CASCADE_1.get(leaf1["sw"], "-"),
        DEV_LEAF1: leaf1_cmd,
        DEV_CAS2: CASCADE_2.get(leaf2["sw"], "-"),
        DEV_LEAF2: leaf2_cmd,
        DEV_MCS1: fmt_recal(3, c1, c2, c2, c4),
        DEV_MCS2: fmt_recal(4, c1, c2, c2, c4),
        "_leaf1_sw": leaf1["sw"],
        "_leaf1_chy": leaf1["chy"],
        "_leaf2_sw": leaf2["sw"],
        "_leaf2_chy": leaf2["chy"],
    }


def verdict_of(sweeps):
    n = len(sweeps)
    fails = sum(1 for s in sweeps if s["peak_ok"] != "1")
    y_ok = any(s["axis"] == "Y" and s["peak_ok"] == "1" for s in sweeps)
    x_ok = any(s["axis"] == "X" and s["peak_ok"] == "1" for s in sweeps)
    if n == 0:
        v = "NOT_FOUND"
    elif y_ok and x_ok:
        v = "OK"
    elif y_ok:
        v = "Y-only"
    elif x_ok:
        v = "X-only"
    else:
        v = "FAIL"
    return v, n, fails, y_ok, x_ok


def main():
    map1 = load_mapping(MAP1, "1_1X64_SW_x")
    map2 = load_mapping(MAP2, "2_1X64SW_x")

    path_cmds = []
    wanted = set()
    for il, step, mi, mo, c1, c2, c4 in BAD:
        cmds = resolve_devices(c1, c2, c4, map1, map2)
        path_cmds.append((il, step, mi, mo, c1, c2, c4, cmds))
        for dev in DEVICES:
            k = parse_recal1(cmds[dev])
            if k:
                wanted.add(k)

    matches = []
    with CSV_PATH.open("r", encoding="utf-8-sig", newline="") as f:
        for line_no, row in enumerate(csv.DictReader(f), start=2):
            path = row.get("path", "")
            m = re.search(r"RECAL 1 (\d+) (\d+) (\d+) (\d+) (\d+)", path)
            if not m:
                continue
            key = tuple(map(int, m.groups()))
            if key not in wanted:
                continue
            recal1 = "RECAL 1 %d %d %d %d %d" % key
            pows = []
            for i in range(1, 128):
                v = row.get("c%d" % i, "")
                if v is None or v == "":
                    break
                try:
                    pows.append(float(v))
                except ValueError:
                    break
            span = (max(pows) - min(pows)) if pows else float("nan")
            argmax = pows.index(max(pows)) if pows else -1
            sm = re.search(r"step=(\d+)/", path)
            slot_m = re.search(r"slot=(\d+)", path)
            file_m = re.search(r"^(pm_[^|]+)", path)
            cmd3 = row.get("cmd", "")
            parts = cmd3.split()
            axis = "?"
            if len(parts) >= 3:
                if parts[2] == "0":
                    axis = "Y"
                elif parts[2] == "1":
                    axis = "X"
            matches.append({
                "csv_line": line_no,
                "recal1": recal1,
                "path_file": file_m.group(1) if file_m else "",
                "slot": int(slot_m.group(1)) if slot_m else -1,
                "step_in_path": int(sm.group(1)) if sm else -1,
                "cmd": cmd3,
                "axis": axis,
                "attempt": row.get("attempt", ""),
                "peak_ok": row.get("peak_ok", ""),
                "code": row.get("code", ""),
                "col0": row.get("c0", ""),
                "n_samples": len(pows),
                "span_raw": ("%.0f" % span) if pows else "",
                "span_db": ("%.4f" % (span / 10000.0)) if pows else "",
                "argmax_idx": argmax,
                "peak_raw": ("%.0f" % pows[argmax]) if pows else "",
            })

    by_cmd = defaultdict(list)
    for m in matches:
        by_cmd[m["recal1"]].append(m)

    summary_rows = []
    detail_rows = []
    md = []
    md.append("# K7797782 不良 IL 通道 — 当时定标扫频提取（六器件）")
    md.append("")
    md.append(
        "源文件：`K7797782/comm_2026-07-14_recal_sweeps.csv`"
        "（匹配 **%d** 条扫频；唯一 RECAL 1 **%d** 条）"
        % (len(matches), len(by_cmd))
    )
    md.append("")
    md.append(
        "1×64 为两级 MEMS（SW1 级联口 + SW2/3/4 叶子）；"
        "端到端 IL 光路最多对应 **六** 条定标指令。"
        "若叶子已在 SW1（CH01–14），对应级联列为 `—`。"
    )
    md.append("")
    md.append(
        "判定 **OK**：Y 与 X 均至少一次 `peak_ok=1`。"
        "`N扫/M败` = 扫频条数 / `peak_ok≠1` 条数。"
    )
    md.append("")
    md.append("映射依据：`pm_1x64_1Mapping.csv` / `pm_1x64_2Mapping.csv`；拓扑见 `doc/设备开关图.png`。")
    md.append("")
    md.append("## 总览")
    md.append("")
    md.append(
        "| IL | step | MPO 入→出 | 1#SW1级联 | 1#叶子 | 2#SW1级联 | 2#叶子 | 1#MCS | 2#MCS |"
    )
    md.append("|---:|-----:|-----------|----------|--------|----------|--------|-------|-------|")

    overview_cmds = []
    for il, step, mi, mo, c1, c2, c4, cmds in path_cmds:
        cells = []
        for dev in DEVICES:
            cmd = cmds[dev]
            if cmd == "-":
                cells.append("—")
                summary_rows.append({
                    "IL": "%.4f" % il,
                    "IL_step": step,
                    "MPO_in": mi,
                    "MPO_out": mo,
                    "c1": c1,
                    "c2": c2,
                    "c4": c4,
                    "device": DEV_CN[dev],
                    "RECAL1": "-",
                    "mems_SW": "",
                    "sweep_count": 0,
                    "peak_ok_count": 0,
                    "peak_fail_count": 0,
                    "Y_ok": 0,
                    "X_ok": 0,
                    "verdict": "N/A",
                    "path_files": "",
                    "csv_lines": "",
                })
                continue
            sweeps = by_cmd.get(cmd, [])
            v, n, fails, y_ok, x_ok = verdict_of(sweeps)
            oks = sum(1 for s in sweeps if s["peak_ok"] == "1")
            cells.append("%s(%d扫/%d败)" % (v, n, fails))
            if "leaf" in dev:
                sw_note = "SW%d" % (
                    cmds["_leaf1_sw"] if dev.startswith("1#") else cmds["_leaf2_sw"]
                )
            elif "cascade" in dev:
                sw_note = "SW1"
            else:
                sw_note = ""
            summary_rows.append({
                "IL": "%.4f" % il,
                "IL_step": step,
                "MPO_in": mi,
                "MPO_out": mo,
                "c1": c1,
                "c2": c2,
                "c4": c4,
                "device": DEV_CN[dev],
                "RECAL1": cmd,
                "mems_SW": sw_note,
                "sweep_count": n,
                "peak_ok_count": oks,
                "peak_fail_count": fails,
                "Y_ok": int(y_ok),
                "X_ok": int(x_ok),
                "verdict": v,
                "path_files": ";".join(sorted(set(s["path_file"] for s in sweeps))),
                "csv_lines": ";".join(str(s["csv_line"]) for s in sweeps),
            })
        md.append(
            "| %.3f | %d | %s→%s | %s |"
            % (il, step, mi, mo, " | ".join(cells))
        )
        overview_cmds.append((il, step, mi, mo, c1, c2, c4, cmds, cells))

    md.append("")
    md.append("## 结论摘要")
    md.append("")
    md.append(
        "- 典型两级 1×64 路径：**六** 条定标指令"
        "（1#SW1级联 + 1#叶子 + 2#SW1级联 + 2#叶子 + 1#MCS + 2#MCS）。"
    )
    md.append(
        "- CH01–14（SW1 直连）无对应级联列（`—`），该路有效指令可能为 5 条。"
    )
    md.append(
        "- 叶子所属 SW 以 Mapping 的 `SW_x` 为准（不要只用光学 CH 猜 2# 侧）。"
    )

    n_ok = sum(1 for r in summary_rows if r["verdict"] == "OK")
    n_na = sum(1 for r in summary_rows if r["verdict"] == "N/A")
    n_fail = sum(1 for r in summary_rows if r["verdict"] not in ("OK", "N/A"))
    n_act = len(summary_rows) - n_na
    md.append(
        "- 本 SN：有效器件指令 **%d** 条中 OK=%d，非 OK=%d（级联空列 N/A=%d）。"
        % (n_act, n_ok, n_fail, n_na)
    )
    md.append("")

    md.append("## 指令对照（六列）")
    md.append("")
    md.append(
        "| IL | step | 1#SW1级联 | 1#叶子 | 2#SW1级联 | 2#叶子 | 1#MCS | 2#MCS |"
    )
    md.append("|---:|-----:|----------|--------|----------|--------|-------|-------|")
    for il, step, mi, mo, c1, c2, c4, cmds, _ in overview_cmds:
        cells = []
        for dev in DEVICES:
            cmd = cmds[dev]
            cells.append("`%s`" % cmd if cmd != "-" else "—")
        md.append("| %.3f | %d | %s |" % (il, step, " | ".join(cells)))
    md.append("")

    for idx, (il, step, mi, mo, c1, c2, c4, cmds, _) in enumerate(overview_cmds, 1):
        md.append(
            "## %d. IL=%.3f  step=%d/576  %s→%s  "
            "(c1=%d c2=%d c4=%d; 1#叶子=SW%d/CH%d; 2#叶子=SW%d/CH%d)"
            % (
                idx, il, step, mi, mo, c1, c2, c4,
                cmds["_leaf1_sw"], cmds["_leaf1_chy"],
                cmds["_leaf2_sw"], cmds["_leaf2_chy"],
            )
        )
        md.append("")
        for dev in DEVICES:
            cmd = cmds[dev]
            dcn = DEV_CN[dev]
            if cmd == "-":
                md.append("### %s — `—`  [N/A]  （SW1 直连，无级联口）" % dcn)
                md.append("")
                continue
            sweeps = by_cmd.get(cmd, [])
            v, n, fails, _, _ = verdict_of(sweeps)
            md.append(
                "### %s — `%s`  [%s]  扫频=%d 失败=%d"
                % (dcn, cmd, v, n, fails)
            )
            md.append("")
            if n == 0:
                md.append("_comm 中未找到该指令_")
                md.append("")
                continue
            md.append(
                "| csv行 | 路径文件 | slot | step | 轴 | 次数 | peak_ok | code | col0 | 点数 | span_dB | argmax |"
            )
            md.append(
                "|------:|----------|-----:|-----:|:--:|-----:|--------:|------|-----:|-----:|--------:|-------:|"
            )
            for s in sweeps:
                md.append(
                    "| %s | %s | %s | %s | %s | %s | %s | %s | %s | %s | %s | %s |"
                    % (
                        s["csv_line"], s["path_file"], s["slot"], s["step_in_path"],
                        s["axis"], s["attempt"], s["peak_ok"], s["code"], s["col0"],
                        s["n_samples"], s["span_db"], s["argmax_idx"],
                    )
                )
                detail_rows.append({
                    "IL": "%.4f" % il,
                    "IL_step": step,
                    "MPO_in": mi,
                    "MPO_out": mo,
                    "c1": c1,
                    "c2": c2,
                    "c4": c4,
                    "device": dcn,
                    "RECAL1": cmd,
                    "csv_line": s["csv_line"],
                    "path_file": s["path_file"],
                    "slot": s["slot"],
                    "step_in_path": s["step_in_path"],
                    "cmd": s["cmd"],
                    "axis": s["axis"],
                    "attempt": s["attempt"],
                    "peak_ok": s["peak_ok"],
                    "code": s["code"],
                    "col0": s["col0"],
                    "n_samples": s["n_samples"],
                    "span_raw": s["span_raw"],
                    "span_db": s["span_db"],
                    "argmax_idx": s["argmax_idx"],
                    "peak_raw": s["peak_raw"],
                })
            md.append("")

    with OUT_SUMMARY.open("w", encoding="utf-8-sig", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(summary_rows[0].keys()))
        w.writeheader()
        w.writerows(summary_rows)

    with OUT_DETAIL.open("w", encoding="utf-8-sig", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(detail_rows[0].keys()))
        w.writeheader()
        w.writerows(detail_rows)

    OUT_MD.write_text("\n".join(md) + "\n", encoding="utf-8")
    print("matched=%d unique_recal1=%d" % (len(matches), len(by_cmd)))
    print("summary=%d detail=%d ok=%d na=%d other=%d" % (
        len(summary_rows), len(detail_rows), n_ok, n_na, n_fail))
    for il, step, mi, mo, c1, c2, c4, cmds, cells in overview_cmds:
        print("IL=%.3f step=%d %s->%s leaf1=SW%d leaf2=SW%d" % (
            il, step, mi, mo, cmds["_leaf1_sw"], cmds["_leaf2_sw"]))
        for dev in DEVICES:
            print("  %s %s" % (DEV_CN[dev], cmds[dev]))
        print("  verdict:", " | ".join(cells))


if __name__ == "__main__":
    main()
