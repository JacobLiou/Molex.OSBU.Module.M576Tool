#!/usr/bin/env python3
"""Summarize CrossPeakTest peak_analysis CSV: fit stats + RECAL3 path pairing (Y then X)."""
import csv
import sys
from collections import Counter
from pathlib import Path


def load_rows(path: Path):
    with path.open(encoding="utf-8-sig", newline="") as f:
        return list(csv.DictReader(f))


def analyze(path: Path):
    rows = load_rows(path)
    n = len(rows)
    ok = sum(1 for r in rows if r.get("fit_ok") == "1")
    fail = n - ok
    by_mode = Counter()
    fail_mode = Counter()
    fail_code = Counter()
    fail_trend = Counter()
    for r in rows:
        sm = r.get("sweep_mode", "?")
        by_mode[sm] += 1
        if r.get("fit_ok") != "1":
            fail_mode[sm] += 1
            fail_code[r.get("validate_code", "?")] += 1
            fail_trend[r.get("trend", "?")] += 1

    # Path pairing: mode0 then next mode1 with same offset (approximate path step)
    paths = 0
    pass_paths = 0
    fail_paths = []
    prev0 = None
    for r in rows:
        sm = r.get("sweep_mode", "")
        if sm == "0":
            prev0 = r
            continue
        if sm != "1" or prev0 is None:
            prev0 = None
            continue
        paths += 1
        y_ok = prev0.get("fit_ok") == "1"
        x_ok = r.get("fit_ok") == "1"
        if y_ok and x_ok:
            pass_paths += 1
        else:
            fail_paths.append(
                (
                    prev0.get("line_no"),
                    r.get("line_no"),
                    prev0.get("validate_code") if not y_ok else "Ok",
                    r.get("validate_code") if not x_ok else "Ok",
                )
            )
        prev0 = None

    return {
        "path": path.name,
        "rows": n,
        "ok": ok,
        "fail": fail,
        "by_mode": dict(by_mode),
        "fail_mode": dict(fail_mode),
        "fail_code": fail_code.most_common(12),
        "fail_trend": fail_trend.most_common(8),
        "paths": paths,
        "pass_paths": pass_paths,
        "fail_paths": fail_paths[:20],
        "fail_path_count": len(fail_paths),
    }


def main():
    files = sys.argv[1:]
    if not files:
        print("usage: analyze_peak_csv.py <peak_analysis.csv> ...")
        return 1
    for fp in files:
        p = Path(fp)
        if not p.is_file():
            print(f"MISSING: {p}")
            continue
        s = analyze(p)
        pct = 100.0 * s["ok"] / s["rows"] if s["rows"] else 0.0
        path_pct = 100.0 * s["pass_paths"] / s["paths"] if s["paths"] else 0.0
        print(f"\n{'='*72}")
        print(f"FILE: {s['path']}")
        print(f"  sweep rows:     {s['rows']}")
        print(f"  fit_ok:         {s['ok']}  ({pct:.2f}%)")
        print(f"  fit_fail:       {s['fail']}")
        print(f"  by sweep_mode:  {s['by_mode']}")
        print(f"  fail by mode:   {s['fail_mode']}")
        print(f"  fail codes:     {s['fail_code']}")
        print(f"  fail trends:    {s['fail_trend']}")
        print(f"  path pairs:     {s['paths']}  PASS {s['pass_paths']} ({path_pct:.2f}%)  FAIL {s['fail_path_count']}")
        if s["fail_paths"]:
            print("  first FAIL pairs (Y_line, X_line, Y_code, X_code):")
            for t in s["fail_paths"][:10]:
                print(f"    L{t[0]}+L{t[1]}  Y={t[2]}  X={t[3]}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
