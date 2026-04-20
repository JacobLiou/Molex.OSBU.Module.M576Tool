# -*- coding: utf-8 -*-
"""
Generate PD path CSV (1219 rows) per PRD Command C (Z4744 RECAL 2).

Wire format: RECAL 2 <target> <2#1x64 ch> <MCS ch>
  target: 1 = 2#1x64 Stage_1, 2 = 2#1x64 Stage_2, 3 = 1# MCS, 4 = 2# MCS
  ch1   : 2#1x64 ch (1..64); routes to 1# MCS when ch1<=32, 2# MCS when ch1>=33
  ch2   : MCS ch (1..18)

Row budget (derived from topology: 2#1x64 -> single MCS -> PD):
  target=1 (2#1x64 Stage_1) : 64
  target=2 (2#1x64 Stage_2) : 3   (boundary c1 = 1, 32, 64; mirrors PM)
  target=3 (1# MCS)         : 32 * 18 = 576
  target=4 (2# MCS)         : 32 * 18 = 576
  Total                     : 1219

Columns: target_index, ch1, ch2
"""
from __future__ import print_function
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(SCRIPT_DIR, "..", "output", "standard_pd.csv")


def main():
    rows = []
    rows.append("target_index,ch1,ch2")

    # 64 -- target=1 (2#1x64 Stage_1): sweep 2#1x64 ch 1..64; ch2 cycles 1..18.
    for c in range(1, 65):
        ch2 = ((c - 1) % 18) + 1
        rows.append("1,{0},{1}".format(c, ch2))

    # 3 -- target=2 (2#1x64 Stage_2): boundary sanity (mirrors PM target=2).
    for c in (1, 32, 64):
        rows.append("2,{0},1".format(c))

    # 576 -- target=3 (1# MCS): 32 switches x 18 channels; ch1 must be in 1..32.
    for sw in range(1, 33):
        for ch in range(1, 19):
            rows.append("3,{0},{1}".format(sw, ch))

    # 576 -- target=4 (2# MCS): 32 switches x 18 channels; ch1 in 33..64.
    for sw in range(1, 33):
        for ch in range(1, 19):
            rows.append("4,{0},{1}".format(sw + 32, ch))

    data_lines = [r for r in rows if not r.startswith("target_index")]
    assert len(data_lines) == 1219, len(data_lines)

    out_abs = os.path.abspath(OUT)
    os.makedirs(os.path.dirname(out_abs), exist_ok=True)
    # UTF-8 without BOM: data rows are ASCII; avoids locale/BOM issues with MFC text read.
    with open(out_abs, "w", encoding="utf-8", newline="") as f:
        f.write("# PD (RECAL 2) path table: target + 2 switch channels\n")
        f.write("# ch1=[2#1x64 ch] (1..64), ch2=[MCS ch] (1..18)\n")
        f.write("# MCS side implied by ch1: 1..32 -> 1# MCS; 33..64 -> 2# MCS\n")
        f.write("# Budget: 64 (Stage_1) + 3 (Stage_2) + 576 (1# MCS) + 576 (2# MCS) = 1219\n")
        for r in rows:
            f.write(r + "\n")

    print("Wrote", out_abs, "lines:", len(rows), "data rows:", len(data_lines))


if __name__ == "__main__":
    main()
