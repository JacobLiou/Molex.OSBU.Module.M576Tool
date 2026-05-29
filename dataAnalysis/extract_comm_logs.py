#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Extract recal_sweeps + peak_summary CSV from comm_*.log in the working folder.

Place ExtractCommLogs.exe next to comm_YYYY-MM-DD.log (e.g. under M576 output/) and double-click.
Outputs (overwrite if present):
  comm_<date>_recal_sweeps.csv   (cmd + power samples, UTF-8 BOM, header)
  comm_<date>_peak_summary.csv   (cmd_path + cmd_sweep_* + stats, UTF-8 BOM)
"""

from __future__ import annotations

import sys
from pathlib import Path


def _bootstrap_tools_path() -> None:
    candidates: list[Path] = []
    if getattr(sys, "frozen", False):
        meipass = Path(getattr(sys, "_MEIPASS", ""))
        candidates.extend([meipass / "tools", meipass])
    else:
        here = Path(__file__).resolve().parent
        candidates.extend([here.parent / "tools", here / "tools"])
    for p in candidates:
        if (p / "extract_recal_sweep_csv.py").is_file():
            s = str(p)
            if s not in sys.path:
                sys.path.insert(0, s)
            return
    raise RuntimeError("tools extract scripts not found (extract_recal_sweep_csv.py).")


_bootstrap_tools_path()

import extract_recal_peak_summary_csv as peak_mod  # noqa: E402
import extract_recal_sweep_csv as sweep_mod  # noqa: E402
from recal_log_cmds import RUN_START  # noqa: E402


def app_dir() -> Path:
    if getattr(sys, "frozen", False):
        return Path(sys.executable).resolve().parent
    return Path(__file__).resolve().parent


def message_box(title: str, text: str, error: bool = False) -> None:
    try:
        import ctypes

        flags = 0x10 if error else 0x40
        ctypes.windll.user32.MessageBoxW(0, text, title, flags)
    except Exception:
        print(f"{title}: {text}", file=sys.stderr)


def find_comm_logs(folder: Path) -> list[Path]:
    logs = sorted(folder.glob("comm_*.log"), key=lambda p: p.name.lower())
    return [p for p in logs if p.is_file()]


def log_has_run_path(log_path: Path) -> bool:
    try:
        text = log_path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return False
    return RUN_START in text


def process_one_log(log_path: Path) -> tuple[bool, str]:
    stem = log_path.stem
    out_sweeps = log_path.with_name(f"{stem}_recal_sweeps.csv")
    out_peak = log_path.with_name(f"{stem}_peak_summary.csv")

    rc_s = sweep_mod.main(
        [
            str(log_path),
            "-o",
            str(out_sweeps),
            "--with-header",
            "--utf8-bom",
        ]
    )
    if rc_s != 0:
        return False, f"{log_path.name}: recal_sweeps failed (code {rc_s})"

    rc_p = peak_mod.main(
        [
            str(log_path),
            "-o",
            str(out_peak),
            "--format",
            "stats",
            "--utf8-bom",
        ]
    )
    if rc_p != 0:
        return False, f"{log_path.name}: peak_summary failed (code {rc_p})"

    return True, f"{log_path.name}\n  -> {out_sweeps.name}\n  -> {out_peak.name}"


def main() -> int:
    folder = app_dir()
    logs = find_comm_logs(folder)
    if not logs:
        message_box(
            "ExtractCommLogs",
            f"No comm_*.log found in:\n{folder}\n\n"
            "Copy this program next to your comm log files and run again.",
            error=True,
        )
        return 1

    lines: list[str] = [f"Folder: {folder}", f"Found {len(logs)} comm_*.log file(s).", ""]
    errors: list[str] = []
    ok_count = 0
    skip_count = 0
    for log_path in logs:
        if not log_has_run_path(log_path):
            lines.append(f"{log_path.name}: skipped (no \"{RUN_START}\" in log)")
            skip_count += 1
            continue
        ok, msg = process_one_log(log_path)
        lines.append(msg)
        if ok:
            ok_count += 1
        else:
            errors.append(msg)

    lines.append("")
    if ok_count == 0 and not errors:
        message_box(
            "ExtractCommLogs",
            "\n".join(lines)
            + "\n\nNo log contained a Run Path segment; nothing written.",
            error=True,
        )
        return 1
    if errors:
        lines.append(f"OK: {ok_count}, skipped: {skip_count}, failed: {len(errors)}")
        message_box("ExtractCommLogs", "\n".join(lines), error=True)
        return 2

    lines.append(
        f"Done: {ok_count} log(s) -> CSV updated (overwrite). Skipped: {skip_count}."
    )
    message_box("ExtractCommLogs", "\n".join(lines))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
