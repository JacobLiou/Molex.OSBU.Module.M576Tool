#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Extract firmware RECAL sweep payloads from M576Calibrator comm logs into CSV.

Scope: lines between "Run Path Started" and "Path run finished (PM all slots)." or
"Path run finished (PD all slots).".

Source lines look like:
  [2026-05-07 09:43:09.182] [RECAL] #3 RECV RECAL 3 0 | 2122,-271065,... | 4890ms

Output: one CSV row per sweep response; first column is the matching SEND wire command
(e.g. RECAL 3 0 9999 64 4 80), then integer power samples.

Examples:
  python tools/extract_recal_sweep_csv.py comm_2026-05-07.log
  python tools/extract_recal_sweep_csv.py comm_2026-05-07.log -o sweeps.csv --with-header
  python tools/extract_recal_sweep_csv.py comm_2026-05-07.log --last-run-only --utf8-bom
"""

from __future__ import annotations

import argparse
import csv
import re
import sys
from pathlib import Path
from typing import Iterable, List, Sequence, Tuple

_TOOLS_DIR = Path(__file__).resolve().parent
if str(_TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(_TOOLS_DIR))

from recal_log_cmds import build_send_index, parse_recv_cmd, parse_recv_seq, split_runs

# Commands that carry comma-separated sweep samples in logs.
_CMD_RE = re.compile(r"^RECAL\s+3\s+[01]\s*$|^RECAL\s+5\b", re.IGNORECASE)
_INT_TOKEN_RE = re.compile(r"^-?\d+$")


def _parse_recv_sweep_line(line: str) -> Tuple[str, str] | None:
    """
    If line is a successful [RECAL] RECV trace with pipe-separated payload, return (command, payload_str).
    Otherwise return None.
    """
    if "[RECAL]" not in line or " RECV " not in line:
        return None
    parts = line.split(" | ")
    if len(parts) < 3:
        return None
    tail_ms = parts[-1].strip()
    if not tail_ms.endswith("ms"):
        return None
    payload = " | ".join(parts[1:-1]).strip()
    cmd = parse_recv_cmd(line)
    if not cmd:
        return None
    return cmd, payload


def _payload_to_ints(payload: str) -> List[int] | None:
    p = payload.strip()
    if not p or p.upper() == "OK":
        return None
    out: List[int] = []
    for tok in p.split(","):
        t = tok.strip()
        if not t:
            continue
        if not _INT_TOKEN_RE.match(t):
            return None
        out.append(int(t))
    return out if out else None


def _collect_rows(
    run_lines: Sequence[str],
    send_index: dict[int, str],
    *,
    skip_log: List[str] | None,
) -> List[Tuple[str, List[int]]]:
    rows: List[Tuple[str, List[int]]] = []
    for line in run_lines:
        parsed = _parse_recv_sweep_line(line)
        if not parsed:
            continue
        cmd, payload = parsed
        if not _CMD_RE.match(cmd):
            continue
        ints = _payload_to_ints(payload)
        if ints is None:
            if skip_log is not None:
                skip_log.append(f"skip non-integer payload cmd={cmd!r} line={line[:200]!r}")
            continue
        seq = parse_recv_seq(line)
        wire_cmd = send_index.get(seq, "NULL") if seq is not None else "NULL"
        if wire_cmd == "NULL" and skip_log is not None:
            skip_log.append(f"no SEND for seq={seq} recv_cmd={cmd!r}")
        rows.append((wire_cmd, ints))
    return rows


def _write_csv(
    rows: Sequence[Tuple[str, Sequence[int]]],
    out_path: Path,
    *,
    with_header: bool,
    utf8_bom: bool,
) -> int:
    if not rows:
        ncols = 0
    else:
        ncols = max(len(r[1]) for r in rows)
    encoding = "utf-8-sig" if utf8_bom else "utf-8"
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("w", newline="", encoding=encoding) as f:
        w = csv.writer(f, lineterminator="\n")
        if with_header:
            hdr = ["cmd"] + ([f"c{i}" for i in range(ncols)] if ncols > 0 else [])
            w.writerow(hdr)
        for wire_cmd, r in rows:
            padded = [wire_cmd] + list(r) + [""] * (ncols - len(r))
            w.writerow(padded)
    return ncols


def main(argv: Sequence[str] | None = None) -> int:
    p = argparse.ArgumentParser(
        description="Extract RECAL 3/5 sweep integers + wire cmd from comm log (Run Path window).",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    p.add_argument(
        "log_file",
        type=Path,
        help="Path to comm_*.log",
    )
    p.add_argument(
        "-o",
        "--output",
        type=Path,
        default=None,
        help="Output CSV path (default: <log_stem>_recal_sweeps.csv next to the log file)",
    )
    p.add_argument(
        "--last-run-only",
        action="store_true",
        help="Only use the last Run Path Started ... Path run finished segment.",
    )
    p.add_argument(
        "--with-header",
        action="store_true",
        help="Write header row cmd,c0,c1,...",
    )
    p.add_argument(
        "--utf8-bom",
        action="store_true",
        help="Write UTF-8 with BOM (Excel-friendly).",
    )
    p.add_argument(
        "--skipped-log",
        type=Path,
        default=None,
        help="Append skip reasons (invalid payload) to this file; default prints to stderr.",
    )
    args = p.parse_args(list(argv) if argv is not None else None)

    log_path: Path = args.log_file
    if not log_path.is_file():
        print(f"error: log file not found: {log_path}", file=sys.stderr)
        return 2

    out_path = args.output
    if out_path is None:
        out_path = log_path.with_name(f"{log_path.stem}_recal_sweeps.csv")

    text = log_path.read_text(encoding="utf-8", errors="replace")
    lines = text.splitlines()
    runs = split_runs(lines)
    if args.last_run_only and runs:
        runs = [runs[-1]]
    elif not runs:
        print("warning: no Run Path segments found; output will be empty.", file=sys.stderr)

    skip_list: List[str] = []
    all_rows: List[Tuple[str, List[int]]] = []
    for run in runs:
        send_index = build_send_index(run)
        all_rows.extend(_collect_rows(run, send_index, skip_log=skip_list))

    if args.skipped_log:
        if skip_list:
            with args.skipped_log.open("a", encoding="utf-8") as sf:
                for s in skip_list:
                    sf.write(s + "\n")
    else:
        for s in skip_list:
            print(s, file=sys.stderr)

    ncols = _write_csv(all_rows, out_path, with_header=args.with_header, utf8_bom=args.utf8_bom)
    print(f"wrote {len(all_rows)} rows x {ncols} sample cols -> {out_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
