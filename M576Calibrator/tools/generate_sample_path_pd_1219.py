# -*- coding: utf-8 -*-
"""
Generate PD path CSVs (1219 rows total) per Z4744 0415 Command C (RECAL 2).

Wire: RECAL 2 <target> <2#1x64 ch> <MCS ch>
  target: 1=2#1x64 Stage_1, 2=Stage_2, 3=1# MCS, 4=2# MCS
  Budget: 64 + 3 + 576 + 576 = 1219

Writes split files under output/:
  pd_mcs1.csv, pd_mcs2.csv, pd_1x64_1.csv (empty data), pd_1x64_2.csv
Also writes pd_1x64_2Mapping.csv if pm_1x64_2Mapping.csv is present (port->SW/CH).
"""
from __future__ import print_function
import csv
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(SCRIPT_DIR, "..", "output")

HEADER = "target_index,ch1,ch2"
COMMENT = [
    "# PD (RECAL 2) path table: target + 2 switch channels",
    "# ch1=[2#1x64 ch] (1..64), ch2=[MCS ch] (1..18)",
    "# MCS side implied by ch1: 1..32 -> 1# MCS; 33..64 -> 2# MCS",
    "# Budget: 64 (Stage_1) + 3 (Stage_2) + 576 (1# MCS) + 576 (2# MCS) = 1219",
]


def write_csv(path, rows):
    with open(path, "w", encoding="utf-8", newline="") as f:
        for c in COMMENT:
            f.write(c + "\n")
        f.write(HEADER + "\n")
        for r in rows:
            f.write(r + "\n")
    print("Wrote", os.path.abspath(path), "data rows:", len(rows))


def build_port_map_from_pm():
    pm_map = os.path.join(OUT_DIR, "pm_1x64_2Mapping.csv")
    port_to_sw = {}
    if not os.path.isfile(pm_map):
        return port_to_sw
    with open(pm_map, newline="", encoding="utf-8") as f:
        reader = csv.reader(f)
        header = None
        for row in reader:
            if not row or (row[0].startswith("#")):
                continue
            if header is None:
                if row[0].strip().lower().startswith("target"):
                    header = row
                    continue
            if len(row) < 7:
                continue
            ch4 = int(row[4])
            sw = int(row[5])
            chy = int(row[6])
            port_to_sw[ch4] = (sw, chy)
    return port_to_sw


def main():
    os.makedirs(OUT_DIR, exist_ok=True)

    rows_s1 = []
    for c in range(1, 65):
        ch2 = ((c - 1) % 18) + 1
        rows_s1.append("1,{0},{1}".format(c, ch2))
    rows_s2 = ["2,1,1", "2,32,1", "2,64,1"]
    rows_mcs1 = []
    for sw in range(1, 33):
        for ch in range(1, 19):
            rows_mcs1.append("3,{0},{1}".format(sw, ch))
    rows_mcs2 = []
    for sw in range(1, 33):
        for ch in range(1, 19):
            rows_mcs2.append("4,{0},{1}".format(sw + 32, ch))

    assert len(rows_s1) + len(rows_s2) + len(rows_mcs1) + len(rows_mcs2) == 1219

    write_csv(os.path.join(OUT_DIR, "pd_1x64_2.csv"), rows_s1 + rows_s2)
    write_csv(os.path.join(OUT_DIR, "pd_mcs1.csv"), rows_mcs1)
    write_csv(os.path.join(OUT_DIR, "pd_mcs2.csv"), rows_mcs2)
    # PD does not calibrate 1#1x64 — keep header-only file for slot symmetry.
    write_csv(os.path.join(OUT_DIR, "pd_1x64_1.csv"), [])

    port_map = build_port_map_from_pm()
    map_path = os.path.join(OUT_DIR, "pd_1x64_2Mapping.csv")
    with open(map_path, "w", encoding="utf-8", newline="") as f:
        f.write(
            "# PD 2#1x64 mapping: target,ch1,ch2,SW_x,CH_y "
            "(SW/CH from pm_1x64_2Mapping by 2#1x64 port)\n"
        )
        f.write("target_index,ch1,ch2,SW_x,CH_y\n")
        n = 0
        for r in rows_s1 + rows_s2:
            t, c1, c2 = [int(x) for x in r.split(",")]
            if c1 not in port_map:
                raise SystemExit("missing PM map for port ch1={0}".format(c1))
            sw, chy = port_map[c1]
            f.write("{0},{1},{2},{3},{4}\n".format(t, c1, c2, sw, chy))
            n += 1
    print("Wrote", os.path.abspath(map_path), "rows:", n)


if __name__ == "__main__":
    main()
