# -*- coding: utf-8 -*-
"""
Generate path_0330-style CSV (1286 rows) per PRD:
  64 + 3 + 32*18 + 32*18 + 64 + 3 = 1286

Columns: target_index, p1b, p1c, p2b, p2c, p3b, p3c, p4b, p4c
Segments: [1#1x64][1#MCS][2#MCS][2#1x64] — ranges: 1x64 ch 1..64, MCS ch 1..18, block 1..2.

Routing hints from PRD (for 1#1x64 channel c):
  c in 1..32  -> 1# MCS side; c in 33..64 -> 2# MCS side
"""
from __future__ import print_function

OUT = r"..\sample_path.csv"


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
    # Header (skipped by loader if contains 'target')
    rows.append("target_index,p1b,p1c,p2b,p2c,p3b,p3c,p4b,p4c")

    # 64 — 1#1x64 Stage_1 / Stage_2 style sweep: target 1, sweep 1#1x64 ch 1..64
    for c in range(1, 65):
        p = path_for_1x64_front(c)
        rows.append(",".join(str(x) for x in (1,) + p))

    # 3 — alignment / boundary (target 2 = 1#1x64 Stage_2 per PRD table)
    for c in (1, 32, 64):
        p = path_for_1x64_front(c)
        rows.append(",".join(str(x) for x in (2,) + p))

    # 576 — 1# MCS: 32 x 18
    for sw in range(1, 33):
        for ch in range(1, 19):
            row = path_mcs_grid(3, sw, ch)
            rows.append(",".join(str(x) for x in row))

    # 576 — 2# MCS: 32 x 18
    for sw in range(1, 33):
        for ch in range(1, 19):
            row = path_mcs_grid(4, sw, ch)
            rows.append(",".join(str(x) for x in row))

    # 64 — 2#1x64 Stage_1/2 sweep: target 5, sweep 2#1x64 ch 1..64
    for c in range(1, 65):
        p = path_for_2x64_back(c)
        rows.append(",".join(str(x) for x in (5,) + p))

    # 3 — tail alignment (target 6 = 2#1x64 Stage_2)
    for c in (1, 32, 64):
        p = path_for_2x64_back(c)
        rows.append(",".join(str(x) for x in (6,) + p))

    data_lines = [r for r in rows if not r.startswith("target_index")]
    assert len(data_lines) == 1286, len(data_lines)

    with open(OUT, "w", encoding="utf-8-sig", newline="") as f:
        f.write("# PRD: 1286 steps = 64+3+576+576+64+3; path_0330 style 9-field CSV\n")
        f.write("# target 1..6 per PRD switch stages; verify with firmware before production\n")
        for r in rows:
            f.write(r + "\n")

    print("Wrote", OUT, "lines:", len(rows), "data rows:", len(data_lines))


if __name__ == "__main__":
    main()
