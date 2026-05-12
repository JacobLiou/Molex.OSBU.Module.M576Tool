#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Extract cross-axis global max (power) and linear DAC fit from M576Calibrator comm logs.

Scope: same Run Path window as extract_recal_sweep_csv.py (Run Path Started ... Path run finished).

One CSV row per log line:
  Step i/total (slot s) RECAL 1 -> OK   (PM)
  Step i/total (slot s) RECAL 2 -> OK   (PD)

Columns:
  run_index: 1-based Run Path segment in this log.
  step_i, step_total, slot, recal_cmd: from the Step line (recal_cmd is 1 or 2).
  recal_switch: first number from nearest preceding SEND RECAL 1|2 line; NULL if not found.
  base_dac_y, base_dac_x: RECAL 3 0/1 (or 5 0/5 1) sweep first cells; NULL if that sweep line absent.
  max_cross_x: power at cross X-axis global max log line; NULL if missing/skipped.
  max_cross_y: power at cross Y-axis global max log line; NULL if missing/skipped.
  fit_x, fit_y: from linear DAC at cross-peak Y= ... X= ...; NULL if missing.
  dac_y_at_max_cross, dac_x_at_max_cross: base + index * step from last SEND RECAL 3/5 in step block; NULL if missing.

Optional --format stats (10 columns for analysis):
  base_dac_y, base_dac_x, max_cross_x_index, max_cross_y_index, max_cross_x, max_cross_y, fit_x, fit_y,
  dac_y_at_max_cross, dac_x_at_max_cross
  dac_y_at_max_cross = base_dac_y + max_cross_y_index * step_y (step from last SEND RECAL 3/5 mode 0 in step block).
  dac_x_at_max_cross = base_dac_x + max_cross_x_index * step_x (from SEND RECAL 3/5 mode 1).
  Use --dac-step N if logs omit SEND lines (fallback for both axes when missing).

Examples:
  python tools/extract_recal_peak_summary_csv.py comm_2026-05-07.log
  python tools/extract_recal_peak_summary_csv.py dataAnalysis/comm_2026-05-06.log -o dataAnalysis/comm_2026-05-06_peak_summary.csv --format stats
  python tools/extract_recal_peak_summary_csv.py dataAnalysis/comm_2026-05-11.log -o dataAnalysis/comm_2026-05-11_peak_summary.csv --format stats --utf8-bom
  python tools/extract_recal_peak_summary_csv.py comm_2026-05-06.log -o out.csv --utf8-bom
"""

from __future__ import annotations

import argparse
import csv
import re
import sys
from pathlib import Path
from typing import Iterable, List, Sequence

RUN_START = "Run Path Started"
RUN_END_MARKERS = (
    "Path run finished (PM all slots).",
    "Path run finished (PD all slots).",
)

STEP_RE = re.compile(
    r"Step (\d+)/(\d+) \(slot (\d+)\) RECAL ([12]) -> OK",
)
# RECAL3 cross X axis global max: index then power (Chinese via escapes).
_CROSS_MAX_RE = re.compile(
    r"RECAL([35])\s+"
    r"\u4ea4\u53c9\s+"
    r"([XY])"
    r"\u8f74"
    r"\((?:PM|PD)\)\s+"
    r"\u5168\u5c40\u6700\u5927\u70b9:\s+"
    r"\u4e0b\u6807=(\d+)\s+"
    r"\u529f\u7387="
    r"([-\d.eE+]+)",
)
_FIT_RE = re.compile(
    r"linear DAC at cross-peak:\s*Y=([-\d.eE+]+).*?X=([-\d.eE+]+)",
)
# Sweep first cells: Y axis then X axis (PM RECAL 3 0 / 3 1; PD RECAL 5 0 X_start + RECAL 5 1 sweep col0).
_RECAL30_SWEEP_COL0_RE = re.compile(
    r"RECAL 3 0 -> \d+ power samples, sweep col0=([-\d.eE+]+)",
)
_RECAL31_SWEEP_COL0_RE = re.compile(
    r"RECAL 3 1 -> \d+ power samples, sweep col0=([-\d.eE+]+)",
)
_RECAL50_XSTART_RE = re.compile(r"RECAL 5 0 -> \d+ samples \(X_start=([-\d.eE+]+)\)")
_RECAL51_SWEEP_COL0_RE = re.compile(
    r"RECAL 5 1 -> \d+ samples, sweep col0=([-\d.eE+]+)",
)
_SEND_RECAL1_RE = re.compile(r"SEND RECAL 1 \| RECAL 1 (\d+)")
_SEND_RECAL2_RE = re.compile(r"SEND RECAL 2 \| RECAL 2 (\d+)")
# TraceSend: RECAL 3/5 <mode> <base> <offset> <step> <delay> (see RecalSession::ExchangeRecal3ReadSweep).
_SEND_RECAL35_SWEEP_RE = re.compile(
    r"SEND RECAL ([35]) ([01]) \| RECAL \1 \2\s+(-?\d+)\s+(-?\d+)\s+(-?\d+)\s+(-?\d+)",
)


def _split_runs(lines: Iterable[str]) -> List[List[str]]:
    runs: List[List[str]] = []
    in_run = False
    buf: List[str] = []
    for line in lines:
        if RUN_START in line:
            if in_run and buf:
                runs.append(buf)
            in_run = True
            buf = []
            continue
        if not in_run:
            continue
        if any(m in line for m in RUN_END_MARKERS):
            runs.append(buf)
            buf = []
            in_run = False
            continue
        buf.append(line)
    if in_run and buf:
        runs.append(buf)
    return runs


def _lookup_recal_switch(run_lines: List[str], step_line_index: int, recal_cmd: str) -> str | None:
    pat = _SEND_RECAL1_RE if recal_cmd == "1" else _SEND_RECAL2_RE
    lo = max(0, step_line_index - 50)
    for j in range(step_line_index - 1, lo - 1, -1):
        m = pat.search(run_lines[j])
        if m:
            return m.group(1)
    return None


def _parse_block(
    block_lines: Sequence[str],
) -> tuple[
    str | None,
    str | None,
    str | None,
    str | None,
    str | None,
    str | None,
    str | None,
    str | None,
    str | None,
    str | None,
]:
    """Returns base_dac_y, base_dac_x, idx_x, idx_y, max_x_power, max_y_power, fit_x, fit_y, step_y, step_x (last match wins)."""
    max_x: str | None = None
    max_y: str | None = None
    idx_x: str | None = None
    idx_y: str | None = None
    fit_x: str | None = None
    fit_y: str | None = None
    base_y: str | None = None
    base_x: str | None = None
    step_y: str | None = None
    step_x: str | None = None
    for line in block_lines:
        sm = _SEND_RECAL35_SWEEP_RE.search(line)
        if sm:
            sweep_mode = sm.group(2)
            step_val = sm.group(5)
            if sweep_mode == "0":
                step_y = step_val
            else:
                step_x = step_val
        m30 = _RECAL30_SWEEP_COL0_RE.search(line)
        if m30:
            base_y = m30.group(1)
        m50 = _RECAL50_XSTART_RE.search(line)
        if m50:
            base_y = m50.group(1)
        m31 = _RECAL31_SWEEP_COL0_RE.search(line)
        if m31:
            base_x = m31.group(1)
        m51 = _RECAL51_SWEEP_COL0_RE.search(line)
        if m51:
            base_x = m51.group(1)
        m = _CROSS_MAX_RE.search(line)
        if m:
            axis = m.group(2)
            idx = m.group(3)
            power = m.group(4)
            if axis == "X":
                max_x = power
                idx_x = idx
            else:
                max_y = power
                idx_y = idx
        fm = _FIT_RE.search(line)
        if fm:
            fit_x = fm.group(1)
            fit_y = fm.group(2)
    return base_y, base_x, idx_x, idx_y, max_x, max_y, fit_x, fit_y, step_y, step_x


def _null(v: str | None) -> str:
    return v if v is not None else "NULL"


def _dac_at_max_cross(base_s: str | None, idx_s: str | None, step_s: str | None) -> str:
    """base + index * step; NULL if any operand missing or non-numeric."""
    if base_s is None or idx_s is None or step_s is None:
        return "NULL"
    try:
        b = float(base_s)
        i = int(idx_s)
        s = int(step_s)
        v = b + float(i) * float(s)
        if v == int(v):
            return str(int(v))
        return str(v)
    except (ValueError, OverflowError):
        return "NULL"


def extract_rows(
    all_run_lines: List[str],
    run_index: int,
    *,
    dac_step_fallback: int | None = None,
) -> List[dict[str, str]]:
    rows: List[dict[str, str]] = []
    step_indices: List[int] = []
    for i, line in enumerate(all_run_lines):
        if STEP_RE.search(line):
            step_indices.append(i)
    for k, idx in enumerate(step_indices):
        m = STEP_RE.search(all_run_lines[idx])
        assert m
        step_i, step_total, slot, recal_cmd = m.group(1), m.group(2), m.group(3), m.group(4)
        end = step_indices[k + 1] if k + 1 < len(step_indices) else len(all_run_lines)
        block = all_run_lines[idx + 1 : end]
        sw = _lookup_recal_switch(all_run_lines, idx, recal_cmd)
        b_y, b_x, ix, iy, max_x, max_y, fx, fy, st_y, st_x = _parse_block(block)
        if st_y is None and dac_step_fallback is not None:
            st_y = str(dac_step_fallback)
        if st_x is None and dac_step_fallback is not None:
            st_x = str(dac_step_fallback)
        dac_y_max = _dac_at_max_cross(b_y, iy, st_y)
        dac_x_max = _dac_at_max_cross(b_x, ix, st_x)
        rows.append(
            {
                "run_index": str(run_index),
                "step_i": step_i,
                "step_total": step_total,
                "slot": slot,
                "recal_cmd": recal_cmd,
                "recal_switch": _null(sw),
                "base_dac_y": _null(b_y),
                "base_dac_x": _null(b_x),
                "max_cross_x_index": _null(ix),
                "max_cross_y_index": _null(iy),
                "max_cross_x": _null(max_x),
                "max_cross_y": _null(max_y),
                "fit_x": _null(fx),
                "fit_y": _null(fy),
                "dac_y_at_max_cross": dac_y_max,
                "dac_x_at_max_cross": dac_x_max,
            }
        )
    return rows


def main(argv: Sequence[str] | None = None) -> int:
    ap = argparse.ArgumentParser(
        description="Extract cross-peak max (power) and linear DAC fit per Step line from comm logs.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    ap.add_argument("log_file", type=Path, help="Path to comm_*.log")
    ap.add_argument(
        "-o",
        "--output",
        type=Path,
        default=None,
        help="Output CSV (default: <log_stem>_peak_summary.csv beside the log)",
    )
    ap.add_argument("--utf8-bom", action="store_true", help="UTF-8 with BOM for Excel.")
    ap.add_argument(
        "--format",
        choices=("full", "stats"),
        default="full",
        help="full: all columns; stats: 10 columns (adds dac_*_at_max_cross vs fit)",
    )
    ap.add_argument(
        "--dac-step",
        type=int,
        default=None,
        metavar="N",
        help="If SEND RECAL 3/5 step cannot be parsed, use N for both step_y and step_x.",
    )
    args = ap.parse_args(list(argv) if argv is not None else None)

    log_path = args.log_file
    if not log_path.is_file():
        print(f"error: log not found: {log_path}", file=sys.stderr)
        return 2

    out = args.output
    if out is None:
        out = log_path.with_name(f"{log_path.stem}_peak_summary.csv")

    text = log_path.read_text(encoding="utf-8", errors="replace")
    lines = text.splitlines()
    runs = _split_runs(lines)

    fieldnames_full = [
        "run_index",
        "step_i",
        "step_total",
        "slot",
        "recal_cmd",
        "recal_switch",
        "base_dac_y",
        "base_dac_x",
        "max_cross_x_index",
        "max_cross_y_index",
        "max_cross_x",
        "max_cross_y",
        "fit_x",
        "fit_y",
        "dac_y_at_max_cross",
        "dac_x_at_max_cross",
    ]
    fieldnames_stats = [
        "base_dac_y",
        "base_dac_x",
        "max_cross_x_index",
        "max_cross_y_index",
        "max_cross_x",
        "max_cross_y",
        "fit_x",
        "fit_y",
        "dac_y_at_max_cross",
        "dac_x_at_max_cross",
    ]
    all_rows: List[dict[str, str]] = []
    for ri, run_lines in enumerate(runs, start=1):
        all_rows.extend(
            extract_rows(run_lines, ri, dac_step_fallback=args.dac_step),
        )

    enc = "utf-8-sig" if args.utf8_bom else "utf-8"
    out.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = fieldnames_stats if args.format == "stats" else fieldnames_full
    with out.open("w", newline="", encoding=enc) as f:
        w = csv.DictWriter(f, fieldnames=fieldnames, extrasaction="ignore", lineterminator="\n")
        w.writeheader()
        for row in all_rows:
            w.writerow({k: row[k] for k in fieldnames})

    print(f"wrote {len(all_rows)} rows -> {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
