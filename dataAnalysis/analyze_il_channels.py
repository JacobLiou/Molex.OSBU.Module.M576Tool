#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Analyze comm_*.log for selected path steps (IL bad channels).

User "channel" IDs map to Run Path step index (Step N/576).
Extracts RECAL 1/3 per slot and scores peak quality from sweep power arrays.

Outputs:
  comm_<date>_il_channels_summary.csv
  comm_<date>_il_channels_detail.csv
  comm_<date>_il_channels/  (per-step comm excerpts)
"""

from __future__ import annotations

import argparse
import csv
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable, List, Optional, Sequence

_TOOLS = Path(__file__).resolve().parent.parent / "tools"
if str(_TOOLS) not in sys.path:
    sys.path.insert(0, str(_TOOLS))

INVALID_POW = {-999999.0, -999900.0}
MIN_SPAN_RAW = 2500.0
EDGE_FRAC_WARN = 0.20

STEP_RE = re.compile(r"Step (\d+)/(\d+) \(slot (\d+)\) RECAL (\d+) -> OK")
RECAL1_SEND_RE = re.compile(
    r"SEND RECAL 1 \| RECAL 1 (\d+) (\d+) (\d+) (\d+) (\d+)"
)
RECV_RECAL30_RE = re.compile(r"RECV RECAL 3 0 \| ([^|]+)\|")
RECV_RECAL31_RE = re.compile(r"RECV RECAL 3 1 \| ([^|]+)\|")
PEAK_LINE_RE = re.compile(
    r"-> peak row=(\d+) col=(\d+); linear DAC at cross-peak: Y=([-\d.]+).*?X=([-\d.]+)"
)
# Log labels (Unicode escapes to keep source ASCII-safe)
Y_MAX_RE = re.compile(
    r"Y\u9884\u77b5\(PM\) \u5168\u5c40\u6700\u5927\u70b9: \u4e0b\u6807=(\d+) \u529f\u7387=([-\d.eE+]+)"
)
X_MAX_RE = re.compile(
    r"\u4ea4\u53c9 X\u8f74\(PM\) \u5168\u5c40\u6700\u5927\u70b9: \u4e0b\u6807=(\d+) \u529f\u7387=([-\d.eE+]+)"
)
Y_CROSS_RE = re.compile(
    r"\u4ea4\u53c9 Y\u8f74\(PM\) \u5168\u5c40\u6700\u5927\u70b9: \u4e0b\u6807=(\d+) \u529f\u7387=([-\d.eE+]+)"
)

DEFAULT_CHANNELS: dict[int, float] = {
    42: 0.3032,
    43: 0.2078,
    49: 0.1626,
    181: 0.2561,
    182: 0.3142,
    183: 0.211,
    184: 0.2908,
    185: 0.3971,
    186: 0.2893,
    187: 0.2032,
    188: 0.2132,
    189: 0.2608,
    190: 0.3285,
    191: 0.2871,
    192: 0.1847,
    217: 0.2073,
    240: 0.1558,
    505: 0.1527,
    516: 0.185,
    565: 0.4472,
    567: 0.3433,
    568: 0.6649,
    570: 0.3862,
}

SLOT_LABEL = {
    1: "1#MCS_PM",
    2: "2#MCS_PM",
    3: "1#1x64_MEMS",
    4: "2#1x64_MEMS",
}


@dataclass
class SweepStats:
    n: int = 0
    span: Optional[float] = None
    peak_idx: Optional[int] = None
    peak_power: Optional[float] = None
    edge_frac: Optional[float] = None
    flat: bool = False
    edge_warn: bool = False


@dataclass
class StepAnalysis:
    path_step: int
    il_db: float
    slot: int
    recal_cmd: str
    line_no: int
    recal1: str = ""
    target: int = 0
    c1: int = 0
    c2: int = 0
    c3: int = 0
    c4: int = 0
    peak_found: bool = False
    peak_row: Optional[int] = None
    peak_col: Optional[int] = None
    fit_y: Optional[float] = None
    fit_x: Optional[float] = None
    y_pre: SweepStats = field(default_factory=SweepStats)
    y_cross: SweepStats = field(default_factory=SweepStats)
    x_cross: SweepStats = field(default_factory=SweepStats)
    flags: List[str] = field(default_factory=list)
    quality: str = ""
    notes: str = ""
    excerpt_lines: List[str] = field(default_factory=list)


def parse_powers_csv(blob: str) -> List[float]:
    vals: List[float] = []
    for part in blob.split(","):
        part = part.strip()
        if not part:
            continue
        try:
            vals.append(float(part))
        except ValueError:
            continue
    return vals


def parse_recal_recv_powers(blob: str) -> List[float]:
    """First CSV field is sweep col0 (DAC); remaining fields are power samples."""
    vals = parse_powers_csv(blob)
    if len(vals) <= 1:
        return []
    return vals[1:]


def valid_powers(vals: Sequence[float]) -> List[float]:
    return [v for v in vals if v not in INVALID_POW]


def sweep_stats(vals: Sequence[float], peak_idx: Optional[int] = None) -> SweepStats:
    st = SweepStats()
    good = valid_powers(vals)
    if not good:
        return st
    st.n = len(good)
    pmax = max(good)
    pmin = min(good)
    st.span = pmax - pmin
    if peak_idx is not None and 0 <= peak_idx < len(vals) and vals[peak_idx] not in INVALID_POW:
        st.peak_idx = peak_idx
        st.peak_power = vals[peak_idx]
    else:
        best_i = 0
        best_v = good[0]
        for i, v in enumerate(vals):
            if v in INVALID_POW:
                continue
            if v > best_v:
                best_v = v
                best_i = i
        st.peak_idx = best_i
        st.peak_power = best_v
    if st.span is not None:
        st.flat = st.span < MIN_SPAN_RAW
    if st.peak_idx is not None and st.n > 1:
        st.edge_frac = st.peak_idx / (st.n - 1)
        st.edge_warn = st.edge_frac <= EDGE_FRAC_WARN or st.edge_frac >= (1.0 - EDGE_FRAC_WARN)
    return st


def collect_flags(block: Sequence[str]) -> List[str]:
    flags: List[str] = []
    for pat in (
        "no peak",
        "peak retry",
        "flat",
        "outside sweep",
        "StrictInc",
        "StrictDec",
        "skip X sweep",
    ):
        if any(pat in ln for ln in block):
            flags.append(pat)
    return flags


def score_quality(row: StepAnalysis) -> tuple[str, str]:
    notes: List[str] = []
    if not row.peak_found:
        return "FAIL", "no cross-peak line (-> peak row=...)"

    if "no peak" in row.flags:
        notes.append("log contains 'no peak'")
    if "peak retry" in row.flags:
        notes.append("peak retry / recentre")
    if "outside sweep" in row.flags:
        notes.append("peak outside sweep window")
    if "StrictInc" in row.flags or "StrictDec" in row.flags:
        notes.append("monotone sweep at edge")

    flats = []
    if row.y_pre.flat:
        flats.append(f"Y-pre span={row.y_pre.span:.0f}")
    if row.y_cross.flat:
        flats.append(f"Y-cross span={row.y_cross.span:.0f}")
    if row.x_cross.flat:
        flats.append(f"X-cross span={row.x_cross.span:.0f}")
    if flats:
        notes.append("flat/low contrast: " + ", ".join(flats))

    edges = []
    for name, st in (("Y-pre", row.y_pre), ("Y-cross", row.y_cross), ("X-cross", row.x_cross)):
        if st.edge_warn and st.peak_idx is not None and st.n:
            edges.append(f"{name} idx={st.peak_idx}/{st.n - 1}")
    if edges:
        notes.append("peak near sweep edge: " + ", ".join(edges))

    if "no peak" in row.flags or "outside sweep" in row.flags:
        return "POOR", "; ".join(notes) if notes else "peak retry or failure"
    if flats or edges or "StrictInc" in row.flags or "StrictDec" in row.flags:
        return "MARGINAL", "; ".join(notes) if notes else "weak peak shape"
    return "GOOD", "; ".join(notes) if notes else "normal peak"


def find_step_blocks(lines: Sequence[str]) -> List[tuple[int, int, int, int, str, int]]:
    out: List[tuple[int, int, int, int, str, int]] = []
    for i, ln in enumerate(lines):
        m = STEP_RE.search(ln)
        if m:
            out.append((i, int(m.group(1)), int(m.group(3)), int(m.group(4)), m.group(2), i + 1))
    return out


def analyze_block(
    path_step: int,
    il_db: float,
    block: Sequence[str],
    meta: tuple[int, int, int, str, int],
) -> StepAnalysis:
    idx, _step_i, slot, recal_cmd, _total, line_no = meta
    row = StepAnalysis(
        path_step=path_step,
        il_db=il_db,
        slot=slot,
        recal_cmd=str(recal_cmd),
        line_no=line_no,
        excerpt_lines=list(block),
    )
    row.flags = collect_flags(block)

    for ln in block:
        m1 = RECAL1_SEND_RE.search(ln)
        if m1:
            row.target, row.c1, row.c2, row.c3, row.c4 = map(int, m1.groups())
            row.recal1 = f"RECAL 1 {row.target} {row.c1} {row.c2} {row.c3} {row.c4}"
            break

    pow_y0: List[float] = []
    pow_x1: List[float] = []
    y_pre_idx: Optional[int] = None
    y_cross_idx: Optional[int] = None
    x_cross_idx: Optional[int] = None

    for ln in block:
        m = RECV_RECAL30_RE.search(ln)
        if m:
            pow_y0 = parse_recal_recv_powers(m.group(1))
        m = RECV_RECAL31_RE.search(ln)
        if m:
            pow_x1 = parse_recal_recv_powers(m.group(1))
        m = Y_MAX_RE.search(ln)
        if m:
            y_pre_idx = int(m.group(1))
        m = Y_CROSS_RE.search(ln)
        if m:
            y_cross_idx = int(m.group(1))
        m = X_MAX_RE.search(ln)
        if m:
            x_cross_idx = int(m.group(1))
        m = PEAK_LINE_RE.search(ln)
        if m:
            row.peak_found = True
            row.peak_row = int(m.group(1))
            row.peak_col = int(m.group(2))
            row.fit_y = float(m.group(3))
            row.fit_x = float(m.group(4))

    if pow_y0:
        row.y_pre = sweep_stats(pow_y0, y_pre_idx)
        row.y_cross = sweep_stats(
            pow_y0, y_cross_idx if y_cross_idx is not None else row.peak_row
        )
    if pow_x1:
        row.x_cross = sweep_stats(
            pow_x1, x_cross_idx if x_cross_idx is not None else row.peak_col
        )

    row.quality, row.notes = score_quality(row)
    return row


def iter_target_analyses(
    lines: Sequence[str],
    channels: dict[int, float],
) -> List[StepAnalysis]:
    blocks = find_step_blocks(lines)
    by_step: dict[int, List[tuple]] = {}
    for b in blocks:
        by_step.setdefault(b[1], []).append(b)

    results: List[StepAnalysis] = []
    for step in sorted(channels):
        il = channels[step]
        entries = by_step.get(step, [])
        if not entries:
            results.append(
                StepAnalysis(
                    path_step=step,
                    il_db=il,
                    slot=0,
                    recal_cmd="",
                    line_no=0,
                    quality="MISSING",
                    notes="step not in log",
                )
            )
            continue
        for pos, meta in enumerate(entries):
            idx = meta[0]
            global_pos = blocks.index(meta)
            next_idx = blocks[global_pos + 1][0] if global_pos + 1 < len(blocks) else len(lines)
            block = lines[idx:next_idx]
            results.append(analyze_block(step, il, block, meta))
    return results


def row_to_summary_dict(r: StepAnalysis) -> dict[str, str]:
    return {
        "path_step": str(r.path_step),
        "il_db": f"{r.il_db:.4f}",
        "slot": str(r.slot),
        "slot_name": SLOT_LABEL.get(r.slot, f"slot{r.slot}"),
        "recal1_cmd": r.recal1,
        "target": str(r.target),
        "c1_1x64": str(r.c1),
        "c2_mcs": str(r.c2),
        "c3_mcs": str(r.c3),
        "c4_1x64": str(r.c4),
        "peak_found": "Y" if r.peak_found else "N",
        "peak_row": "" if r.peak_row is None else str(r.peak_row),
        "peak_col": "" if r.peak_col is None else str(r.peak_col),
        "fit_dac_y": "" if r.fit_y is None else str(r.fit_y),
        "fit_dac_x": "" if r.fit_x is None else str(r.fit_x),
        "y_pre_span": "" if r.y_pre.span is None else f"{r.y_pre.span:.0f}",
        "y_cross_span": "" if r.y_cross.span is None else f"{r.y_cross.span:.0f}",
        "x_cross_span": "" if r.x_cross.span is None else f"{r.x_cross.span:.0f}",
        "flags": "|".join(r.flags),
        "quality": r.quality,
        "notes": r.notes,
        "log_line": str(r.line_no),
    }


def write_excerpts(out_dir: Path, rows: Iterable[StepAnalysis]) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    for r in rows:
        if not r.excerpt_lines:
            continue
        name = f"step_{r.path_step:03d}_slot{r.slot}.txt"
        (out_dir / name).write_text("\n".join(r.excerpt_lines), encoding="utf-8")


def main(argv: Sequence[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="Analyze IL bad channels in comm log")
    ap.add_argument("log_file", type=Path, nargs="?", help="comm_YYYY-MM-DD.log")
    ap.add_argument("-o", "--output-dir", type=Path, default=None)
    ap.add_argument("--no-excerpt", action="store_true")
    args = ap.parse_args(list(argv) if argv is not None else None)

    log_path = args.log_file
    if log_path is None:
        here = Path(__file__).resolve().parent
        cands = sorted(here.glob("comm_*.log"))
        if not cands:
            print("error: no comm_*.log in dataAnalysis/", file=sys.stderr)
            return 2
        log_path = cands[-1]

    if not log_path.is_file():
        print(f"error: not found: {log_path}", file=sys.stderr)
        return 2

    out_dir = args.output_dir or log_path.parent
    stem = log_path.stem
    summary_csv = out_dir / f"{stem}_il_channels_summary.csv"
    detail_csv = out_dir / f"{stem}_il_channels_detail.csv"
    excerpt_dir = out_dir / f"{stem}_il_channels"

    lines = log_path.read_text(encoding="utf-8", errors="replace").splitlines()
    rows = iter_target_analyses(lines, DEFAULT_CHANNELS)

    fields = list(row_to_summary_dict(rows[0]).keys()) if rows else []
    with summary_csv.open("w", newline="", encoding="utf-8-sig") as f:
        w = csv.DictWriter(f, fieldnames=fields, lineterminator="\n")
        w.writeheader()
        for r in rows:
            w.writerow(row_to_summary_dict(r))

    agg_fields = [
        "path_step",
        "il_db",
        "pm_slot1_quality",
        "pm_slot2_quality",
        "mems_slot3_quality",
        "mems_slot4_quality",
        "worst_quality",
        "peak_found_all",
        "key_issue",
    ]
    by_step: dict[int, List[StepAnalysis]] = {}
    for r in rows:
        by_step.setdefault(r.path_step, []).append(r)

    qual_rank = {"GOOD": 0, "MARGINAL": 1, "POOR": 2, "FAIL": 3, "MISSING": 4}

    def slot_q(step: int, slot: int) -> str:
        for r in by_step.get(step, []):
            if r.slot == slot:
                return r.quality
        return ""

    with detail_csv.open("w", newline="", encoding="utf-8-sig") as f:
        w = csv.DictWriter(f, fieldnames=agg_fields, lineterminator="\n")
        w.writeheader()
        for step in sorted(DEFAULT_CHANNELS):
            grp = by_step.get(step, [])
            qualities = [r.quality for r in grp]
            worst = max(qualities, key=lambda q: qual_rank.get(q, 9)) if qualities else "MISSING"
            issues = [
                r.notes
                for r in grp
                if r.quality in ("POOR", "MARGINAL", "FAIL") and r.notes
            ]
            w.writerow(
                {
                    "path_step": str(step),
                    "il_db": f"{DEFAULT_CHANNELS[step]:.4f}",
                    "pm_slot1_quality": slot_q(step, 1),
                    "pm_slot2_quality": slot_q(step, 2),
                    "mems_slot3_quality": slot_q(step, 3),
                    "mems_slot4_quality": slot_q(step, 4),
                    "worst_quality": worst,
                    "peak_found_all": "Y"
                    if grp and all(r.peak_found for r in grp)
                    else "N",
                    "key_issue": issues[0] if issues else "",
                }
            )

    if not args.no_excerpt:
        write_excerpts(excerpt_dir, rows)

    print(f"Analyzed {len(DEFAULT_CHANNELS)} path steps, {len(rows)} slot records")
    print(f"Wrote: {summary_csv}")
    print(f"Wrote: {detail_csv}")
    if not args.no_excerpt:
        print(f"Wrote excerpts: {excerpt_dir}/")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
