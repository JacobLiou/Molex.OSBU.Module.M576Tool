# -*- coding: utf-8 -*-
"""Compare 1310 cal IL vs 1550 PD IL MAX-MIN -> diff.csv"""
import re
import sys
from pathlib import Path

import pandas as pd

ROOT = Path(__file__).resolve().parents[1]
XLSX = ROOT / "\u6302\u673a\u4e0d\u826f\u901a\u9053SN.xlsx"
OUT = ROOT / "diff.csv"

IN_COL = "\u5165\u5149\u901a\u9053\u540d"
OUT_COL = "\u51fa\u5149\u901a\u9053\u540d"


def normalize_columns(df):
    cols = list(df.columns)
    new_cols = []
    x64_count = 0
    x18_count = 0
    for c in cols:
        cs = str(c)
        if cs.startswith("1X64"):
            x64_count += 1
            new_cols.append("1X64" if x64_count == 1 else "1X64_2")
        elif cs.startswith("1X18"):
            x18_count += 1
            new_cols.append("1X18" if x18_count == 1 else "1X18_2")
        else:
            new_cols.append(c)
    df = df.copy()
    df.columns = new_cols
    return df


def load_sheet(xlsx, sheet_name):
    df = normalize_columns(pd.read_excel(xlsx, sheet_name=sheet_name, header=0))
    if "MAX-MIN" not in df.columns:
        raise ValueError(f"{sheet_name} missing MAX-MIN column")
    df["MAX-MIN"] = pd.to_numeric(df["MAX-MIN"], errors="coerce")
    return df.dropna(subset=["MAX-MIN"])


def main():
    if not XLSX.is_file():
        print(f"missing {XLSX}", file=sys.stderr)
        return 1

    xl = pd.ExcelFile(XLSX)
    sn_to_sheets = {}
    for s in xl.sheet_names:
        m = re.match(r"^(.+)_(1310|1550)$", s, re.I)
        if not m:
            continue
        sn, band = m.group(1), m.group(2).lower()
        sn_to_sheets.setdefault(sn.upper(), {})[band] = s

    merge_cols = ["MCS1", "1X64", "1X18", "MCS2", "1X64_2", "1X18_2"]
    rows = []

    for sn_upper, pair in sorted(sn_to_sheets.items()):
        if "1310" not in pair or "1550" not in pair:
            continue
        s1310 = load_sheet(XLSX, pair["1310"])
        s1550 = load_sheet(XLSX, pair["1550"])
        merged = s1310.merge(
            s1550,
            on=merge_cols,
            how="outer",
            suffixes=("_1310", "_1550"),
            indicator=True,
        )
        for _, r in merged.iterrows():
            il1310 = r.get("MAX-MIN_1310")
            il1550 = r.get("MAX-MIN_1550")
            if pd.isna(il1310) and pd.isna(il1550):
                continue
            in_name = r.get(f"{IN_COL}_1310", r.get(f"{IN_COL}_1550"))
            out_name = r.get(f"{OUT_COL}_1310", r.get(f"{OUT_COL}_1550"))
            row = {
                "SN": sn_upper,
                "MCS1": r.get("MCS1"),
                "1X64": r.get("1X64"),
                "1X18": r.get("1X18"),
                "MCS2": r.get("MCS2"),
                "1X64_2": r.get("1X64_2"),
                "1X18_2": r.get("1X18_2"),
                IN_COL: in_name,
                OUT_COL: out_name,
                "MAX-MIN_1310": il1310,
                "MAX-MIN_1550": il1550,
            }
            if pd.notna(il1310) and pd.notna(il1550):
                row["MAX-MIN_diff_1310_minus_1550"] = round(
                    float(il1310) - float(il1550), 4
                )
            else:
                row["MAX-MIN_diff_1310_minus_1550"] = None
            row["match"] = r["_merge"]
            rows.append(row)

    out = pd.DataFrame(rows)
    col_order = [
        "SN",
        "MCS1",
        "1X64",
        "1X18",
        "MCS2",
        "1X64_2",
        "1X18_2",
        IN_COL,
        OUT_COL,
        "MAX-MIN_1310",
        "MAX-MIN_1550",
        "MAX-MIN_diff_1310_minus_1550",
        "match",
    ]
    out = out[col_order]
    out.to_csv(OUT, index=False, encoding="utf-8-sig")

    both = out[out["match"] == "both"]
    print(f"written {OUT} rows={len(out)} machines={out['SN'].nunique()}")
    print(
        f"matched={len(both)} 1310_only={(out['match']=='left_only').sum()} "
        f"1550_only={(out['match']=='right_only').sum()}"
    )
    if len(both):
        print(f"1310 mean={both['MAX-MIN_1310'].mean():.4f} max={both['MAX-MIN_1310'].max():.4f}")
        print(f"1550 mean={both['MAX-MIN_1550'].mean():.4f} max={both['MAX-MIN_1550'].max():.4f}")
        print(f"diff mean={both['MAX-MIN_diff_1310_minus_1550'].mean():.4f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
