#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Smoke tests for comm log extractors (step-successful sweeps)."""

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

_TOOLS = Path(__file__).resolve().parent
if str(_TOOLS) not in sys.path:
    sys.path.insert(0, str(_TOOLS))

from recal_log_cmds import (  # noqa: E402
    block_axis_outcome,
    parse_block_sweep_cmds,
    parse_recal_sweep_wire_step,
    split_step_blocks,
)
import extract_recal_sweep_csv as sweep_mod  # noqa: E402


class TestRecalLogExtract(unittest.TestCase):
    def test_block_outcome_retry_then_success(self) -> None:
        block = [
            "[RECAL] #1 SEND RECAL 3 0 | RECAL 3 0 9999 9999 64 4 80\\r",
            "[RECAL] #1 RECV RECAL 3 0 | 1,2,3",
            "  peak retry: RECAL 3 0 Y attempt 1/10 code=fail",
            "[RECAL] #2 SEND RECAL 3 0 | RECAL 3 0 9999 -110 64 4 80\\r",
            "[RECAL] #2 RECV RECAL 3 0 | 4,5,6",
            "  RECAL 3 0 -> 33 power samples, sweep col0=-110",
            "  RECAL 3 1 Base DAC (Y@peak, row=3)=-293",
            "[RECAL] #3 SEND RECAL 3 1 | RECAL 3 1 9999 -293 64 4 80\\r",
            "[RECAL] #3 RECV RECAL 3 1 | 7,8,9",
            "  -> peak row=3 col=15; linear DAC at cross-peak: Y=-293 X=-2643",
        ]
        y_ok, cross_ok, skip_x = block_axis_outcome(block)
        self.assertTrue(y_ok)
        self.assertTrue(cross_ok)
        self.assertFalse(skip_x)
        cmd0, cmd1, _ = parse_block_sweep_cmds(block)
        self.assertEqual(cmd0, "RECAL 3 0 9999 -110 64 4 80")
        self.assertEqual(cmd1, "RECAL 3 1 9999 -293 64 4 80")
        self.assertEqual(parse_recal_sweep_wire_step(cmd0, "0"), "4")
        self.assertEqual(parse_recal_sweep_wire_step(cmd1, "1"), "4")

    def test_collect_successful_omits_failed_retry(self) -> None:
        run = [
            "Run Path Started",
            "Step 1/576 (slot 1) RECAL 1 -> OK",
            "[RECAL] #10 SEND RECAL 3 0 | RECAL 3 0 9999 9999 64 4 80\\r",
            "[RECAL] #10 RECV RECAL 3 0 | 1,2,3,4 | 100ms",
            "  peak retry: RECAL 3 0 Y attempt 1/10",
            "[RECAL] #11 SEND RECAL 3 0 | RECAL 3 0 9999 -50 64 4 80\\r",
            "[RECAL] #11 RECV RECAL 3 0 | 5,6,7,8 | 100ms",
            "  RECAL 3 0 -> 4 power samples, sweep col0=-50",
            "[RECAL] #12 SEND RECAL 3 1 | RECAL 3 1 9999 100 64 4 80\\r",
            "[RECAL] #12 RECV RECAL 3 1 | 9,10,11,12 | 100ms",
            "  -> peak row=1 col=2; linear DAC at cross-peak: Y=100 X=200",
            "Path run finished (PM all slots).",
        ]
        send_index = {10: "RECAL 3 0 9999 9999 64 4 80", 11: "RECAL 3 0 9999 -50 64 4 80", 12: "RECAL 3 1 9999 100 64 4 80"}
        rows = sweep_mod._collect_rows_successful_per_step(run, send_index, skip_log=None)
        self.assertEqual(len(rows), 2)
        self.assertEqual(rows[0][0], "RECAL 3 0 9999 -50 64 4 80")
        self.assertEqual(rows[0][1], [5, 6, 7, 8])
        self.assertEqual(rows[1][0], "RECAL 3 1 9999 100 64 4 80")

    def test_comm_2026_06_04_row_count_sanity(self) -> None:
        log = Path(__file__).resolve().parent.parent / "dataAnalysis" / "comm_2026-06-04.log"
        if not log.is_file():
            self.skipTest("comm_2026-06-04.log not in workspace")
        text = log.read_text(encoding="utf-8", errors="replace")
        lines = text.splitlines()
        from recal_log_cmds import build_send_index, split_runs

        run = split_runs(lines)[0]
        send_index = build_send_index(run)
        ok_rows = sweep_mod._collect_rows_successful_per_step(run, send_index, skip_log=None)
        all_rows = sweep_mod._collect_rows_all_attempts(run, send_index, skip_log=None)
        self.assertLess(len(ok_rows), len(all_rows))
        self.assertGreater(len(all_rows) - len(ok_rows), 50)


if __name__ == "__main__":
    unittest.main()
