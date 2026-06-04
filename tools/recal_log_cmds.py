#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Shared RECAL comm-log parsing: run windows, SEND/RECV seq pairing, full wire commands."""

from __future__ import annotations

import re
from typing import Iterable, List, Sequence, Tuple

RUN_START = "Run Path Started"
RUN_END_MARKERS = (
    "Path run finished (PM all slots).",
    "Path run finished (PD all slots).",
)

_SEND_LINE_RE = re.compile(
    r"\[RECAL\] #(\d+) SEND [^|]+ \| (RECAL[^\|]+?)(?:\\r)?\s*$"
)
_RECV_HEAD_RE = re.compile(r"\[RECAL\] #(\d+) RECV (.+)$")
_SEND_SWEEP_MODE_RE = re.compile(
    r"SEND RECAL ([35]) ([01]) \| (RECAL \1 \2\s+(?:[-\d]+\s+){4,5})(?:\\r)?"
)
_SEND_PATH1_RE = re.compile(
    r"SEND RECAL 1 \| (RECAL 1\s+[-\d]+(?:\s+[-\d]+){4})(?:\\r)?"
)
_SEND_PATH2_RE = re.compile(
    r"SEND RECAL 2 \| (RECAL 2\s+[-\d]+(?:\s+[-\d]+){2})(?:\\r)?"
)
_SKIP_X_SWEEP_RE = re.compile(r"skip X sweep \(RECAL [35] 1\)")
_STEP_PATH_RE = re.compile(
    r"Step (\d+)/(\d+) \(slot (\d+)\) RECAL ([12]) -> OK",
)
# Successful axis sweep (after peak/flat retries exhausted or first-shot OK).
_RECAL30_OK_RE = re.compile(r"RECAL 3 0 -> \d+ power samples")
_RECAL31_BASE_RE = re.compile(r"RECAL 3 1 Base DAC \(Y@peak")
_RECAL50_OK_RE = re.compile(r"RECAL 5 0 -> \d+ samples")
_RECAL51_BASE_RE = re.compile(r"RECAL 5 1 Base DAC \(Y@peak")
_CROSS_PEAK_OK_RE = re.compile(r"-> peak row=")

# Public alias for step-line regex (used by peak_summary / tests).
STEP_PATH_RE = _STEP_PATH_RE


def split_runs(lines: Iterable[str]) -> List[List[str]]:
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


def normalize_cmd(payload: str) -> str:
    return payload.strip().rstrip("\\r").rstrip("\r").strip()


def parse_send_payload(line: str) -> str | None:
    m = _SEND_LINE_RE.search(line)
    if not m:
        return None
    return normalize_cmd(m.group(2))


def build_send_index(run_lines: Sequence[str]) -> dict[int, str]:
    index: dict[int, str] = {}
    for line in run_lines:
        m = _SEND_LINE_RE.search(line)
        if m:
            index[int(m.group(1))] = normalize_cmd(m.group(2))
    return index


def parse_recv_seq(line: str) -> int | None:
    if "[RECAL]" not in line or " RECV " not in line:
        return None
    head = line.split(" | ", 1)[0]
    m = _RECV_HEAD_RE.search(head)
    if not m:
        return None
    return int(m.group(1))


def parse_recv_cmd(line: str) -> str | None:
    if "[RECAL]" not in line or " RECV " not in line:
        return None
    head = line.split(" | ", 1)[0]
    m = _RECV_HEAD_RE.search(head)
    if not m:
        return None
    return m.group(2).strip()


def lookup_cmd_path(
    run_lines: Sequence[str],
    step_line_index: int,
    recal_cmd: str,
) -> str | None:
    pat = _SEND_PATH1_RE if recal_cmd == "1" else _SEND_PATH2_RE
    lo = max(0, step_line_index - 50)
    for j in range(step_line_index - 1, lo - 1, -1):
        m = pat.search(run_lines[j])
        if m:
            return normalize_cmd(m.group(1))
        payload = parse_send_payload(run_lines[j])
        if payload and payload.startswith(f"RECAL {recal_cmd} "):
            return payload
    return None


def parse_block_sweep_cmds(
    block_lines: Sequence[str],
) -> Tuple[str | None, str | None, bool]:
    """
    Returns (cmd_sweep_0, cmd_sweep_1, skip_x_sweep).
    Last matching SEND RECAL 3|5 mode 0/1 wins; skip_x forces cmd_sweep_1 to NULL.
    """
    cmd0: str | None = None
    cmd1: str | None = None
    skip_x = False
    for line in block_lines:
        if _SKIP_X_SWEEP_RE.search(line):
            skip_x = True
        if " SEND " not in line:
            continue
        payload = parse_send_payload(line)
        if not payload:
            continue
        parts = payload.split()
        if len(parts) >= 3 and parts[0] == "RECAL" and parts[1] in ("3", "5"):
            if parts[2] == "0":
                cmd0 = payload
            elif parts[2] == "1":
                cmd1 = payload
    if skip_x:
        cmd1 = None
    return cmd0, cmd1, skip_x


def split_step_blocks(run_lines: Sequence[str]) -> List[List[str]]:
    """Lines between consecutive 'Step i/n (slot s) RECAL 1|2 -> OK' markers."""
    indices: List[int] = []
    for i, line in enumerate(run_lines):
        if _STEP_PATH_RE.search(line):
            indices.append(i)
    blocks: List[List[str]] = []
    for k, idx in enumerate(indices):
        end = indices[k + 1] if k + 1 < len(indices) else len(run_lines)
        blocks.append(list(run_lines[idx + 1 : end]))
    return blocks


def block_axis_outcome(block_lines: Sequence[str]) -> tuple[bool, bool, bool]:
    """
    Returns (y_pre_ok, cross_ok, skip_x_sweep).
    y_pre_ok: Y-axis pre-sweep produced usable samples (RECAL 3/5 mode 0).
    cross_ok: 2D cross peak line present (mode 1 completed).
    """
    y_ok = False
    cross_ok = False
    skip_x = False
    for line in block_lines:
        if _SKIP_X_SWEEP_RE.search(line):
            skip_x = True
        if _CROSS_PEAK_OK_RE.search(line):
            cross_ok = True
        if (
            _RECAL30_OK_RE.search(line)
            or _RECAL31_BASE_RE.search(line)
            or _RECAL50_OK_RE.search(line)
            or _RECAL51_BASE_RE.search(line)
        ):
            y_ok = True
    return y_ok, cross_ok, skip_x


def parse_recal_sweep_wire_step(wire_cmd: str, sweep_mode: str) -> str | None:
    """Extract Step DAC from RECAL 3/5 mode-0/1 wire command (legacy 5-param or dual-base 6-param)."""
    parts = wire_cmd.split()
    if len(parts) < 7 or parts[0] != "RECAL" or parts[1] not in ("3", "5"):
        return None
    if parts[2] != sweep_mode:
        return None
    if parts[1] == "3":
        if len(parts) >= 8:
            return parts[6]
        if len(parts) >= 7:
            return parts[5]
    if len(parts) >= 7:
        return parts[5]
    return None
