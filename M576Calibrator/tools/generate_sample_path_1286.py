# -*- coding: utf-8 -*-
"""
Generate path CSV (1286 rows) per PRD:
  64 + 3 + 32*18 + 32*18 + 64 + 3 = 1286

Wire format (Z4744 Command B): RECAL 1 <target> <c1> <c2> <c3> <c4>
  c1..c4 = [1#1x64 ch][1#MCS ch][2#MCS ch][2#1x64 ch] — four channel numbers only.

Columns: target_index, ch1, ch2, ch3, ch4
(Internal helpers still compute legacy 8-tuple p1b,p1c,... then take channel-only fields.)
"""
from __future__ import print_function
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(SCRIPT_DIR, "..", "output", "standard.csv")


def to_recal1_four(path8):
    """From (p1b,p1c,p2b,p2c,p3b,p3c,p4b,p4c) to (c1,c2,c3,c4) = (p1c,p2c,p3c,p4c)."""
    p1b, p1c, p2b, p2c, p3b, p3c, p4b, p4c = path8
    return p1c, p2c, p3c, p4c


def path_for_1x64_front(c1):
    """Return (p1b,p1c,p2b,p2c,p3b,p3c,p4b,p4c) with p1b=1, p1c=c1."""
    assert 1 <= c1 <= 64
    p1b, p1c = 1, c1
    if c1 <= 32:
        p2b, p2c = 1, ((c1 - 1) % 18) + 1
        p3b, p3c = 2, 1
    else:
        p2b, p2c = 1, 1
        p3b, p3c = 2, ((c1 - 33) % 18) + 1
    p4b = 2
    p4c = ((c1 + 30) % 64) + 1
    return p1b, p1c, p2b, p2c, p3b, p3c, p4b, p4c


def path_for_2x64_back(c4):
    """2#1x64 segment as leading context; symmetric to front."""
    assert 1 <= c4 <= 64
    p4b, p4c = 2, c4
    if c4 <= 32:
        p3b, p3c = 2, ((c4 - 1) % 18) + 1
        p2b, p2c = 1, 1
    else:
        p3b, p3c = 2, 1
        p2b, p2c = 1, ((c4 - 33) % 18) + 1
    p1b = 1
    p1c = ((c4 + 15) % 64) + 1
    return p1b, p1c, p2b, p2c, p3b, p3c, p4b, p4c


def path_mcs_grid(which_mcs, sw, ch):
    """
    which_mcs: 3 -> calibrate 1# MCS (target index 3), 4 -> 2# MCS
    sw: 1..32, ch: 1..18
    Build a valid path visiting the intended MCS channel.
    """
    assert 1 <= sw <= 32 and 1 <= ch <= 18
    idx = (sw - 1) * 18 + (ch - 1)  # 0..575
    p1b = 1
    p1c = (idx % 64) + 1
    p2b, p3b = 1, 2
    if which_mcs == 3:
        p2c = ch
        p3c = ((sw + ch) % 18) + 1
    else:
        p2c = ((idx // 32) % 18) + 1
        p3c = ch
    p4b = 2
    p4c = ((idx // 18) % 64) + 1
    tgt = which_mcs
    return (tgt,) + (p1b, p1c, p2b, p2c, p3b, p3c, p4b, p4c)


def main():
    rows = []
    rows.append("target_index,ch1,ch2,ch3,ch4")

    # 64 — 1#1x64 Stage_1 / Stage_2 style sweep: target 1, sweep 1#1x64 ch 1..64
    for c in range(1, 65):
        p8 = path_for_1x64_front(c)
        c1, c2, c3, c4 = to_recal1_four(p8)
        rows.append(",".join(str(x) for x in (1, c1, c2, c3, c4)))

    # 3 — alignment / boundary (target 2 = 1#1x64 Stage_2 per PRD table)
    for c in (1, 32, 64):
        p8 = path_for_1x64_front(c)
        c1, c2, c3, c4 = to_recal1_four(p8)
        rows.append(",".join(str(x) for x in (2, c1, c2, c3, c4)))

    # 576 — 1# MCS (target 3): FW — ch1=sw 1..32, ch2/ch3 paired 1..18, ch4=ch1+32
    for sw in range(1, 33):
        for ch in range(1, 19):
            rows.append(",".join(str(x) for x in (3, sw, ch, ch, sw + 32)))

    # 576 — 2# MCS: 32 x 18
    for sw in range(1, 33):
        for ch in range(1, 19):
            row = path_mcs_grid(4, sw, ch)
            tgt = row[0]
            c1, c2, c3, c4 = to_recal1_four(row[1:9])
            rows.append(",".join(str(x) for x in (tgt, c1, c2, c3, c4)))

    # 64 — 2#1x64 Stage_1/2 sweep: target 5, sweep 2#1x64 ch 1..64
    for c in range(1, 65):
        p8 = path_for_2x64_back(c)
        c1, c2, c3, c4 = to_recal1_four(p8)
        rows.append(",".join(str(x) for x in (5, c1, c2, c3, c4)))

    # 3 — tail alignment (target 6 = 2#1x64 Stage_2)
    for c in (1, 32, 64):
        p8 = path_for_2x64_back(c)
        c1, c2, c3, c4 = to_recal1_four(p8)
        rows.append(",".join(str(x) for x in (6, c1, c2, c3, c4)))

    data_lines = [r for r in rows if not r.startswith("target_index")]
    assert len(data_lines) == 1286, len(data_lines)

    out_abs = os.path.abspath(OUT)
    os.makedirs(os.path.dirname(out_abs), exist_ok=True)
    with open(out_abs, "w", encoding="utf-8-sig", newline="") as f:
        f.write("# PRD: 1286 steps; Z4744 RECAL 1 = target + 4 path channels (ch1..ch4)\n")
        f.write("# ch1=[1#1x64] ch2=[1#MCS] ch3=[2#MCS] ch4=[2#1x64]\n")
        for r in rows:
            f.write(r + "\n")

    print("Wrote", out_abs, "lines:", len(rows), "data rows:", len(data_lines))


if __name__ == "__main__":
    main()
