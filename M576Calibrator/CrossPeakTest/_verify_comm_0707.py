# Read-only comm CSV verification (no algorithm changes)
import csv
import re
from collections import defaultdict

MIN_SPAN_RAW = 3000.0  # default MinProminenceDb raw scale ~0.3 dB * 10000


def parse_row(fields):
    if len(fields) < 6:
        return None
    path, cmd = fields[0], fields[1]
    try:
        attempt = int(fields[2])
        peak_ok = int(fields[3])
        code = fields[4]
    except ValueError:
        return None
    nums = []
    for f in fields[5:]:
        if not f.strip():
            continue
        try:
            nums.append(float(f))
        except ValueError:
            break
    if len(nums) < 2:
        return None
    col0, pows = nums[0], nums[1:]
    m = re.search(r"RECAL (\d+) (\d+) (\d+) (\d+) (\d+) (\d+) (\d+)", cmd)
    if not m:
        return None
    return {
        "path": path,
        "cmd": cmd,
        "attempt": attempt,
        "peak_ok": peak_ok,
        "code": code,
        "col0": col0,
        "pows": pows,
        "offset": int(m.group(5)),
        "mode": int(m.group(2)),
        "baseY": int(m.group(4)),
        "baseX": int(m.group(3)),
    }


def valid_powers(pows):
    return [p for p in pows if p > -900000.0]


def trend(pows):
    valid = valid_powers(pows)
    if len(valid) < 4:
        return "TooFew", 0.0
    lo, hi = min(valid), max(valid)
    span = hi - lo
    if span < MIN_SPAN_RAW:
        return "Flat", span
    eps = max(1e-6, span * 0.01)
    inc = all(valid[i] > valid[i - 1] + eps for i in range(1, len(valid)))
    dec = all(valid[i] < valid[i - 1] - eps for i in range(1, len(valid)))
    if inc:
        return "StrictInc", span
    if dec:
        return "StrictDec", span
    return "NonMono", span


def mono_tail(pows, min_tail=4):
    valid = valid_powers(pows)
    if len(valid) < min_tail:
        return "TooFew"
    n = len(valid)
    start = max(0, n - max(min_tail, n // 2))
    tail = valid[start:]
    lo, hi = min(tail), max(tail)
    span = hi - lo
    if span < MIN_SPAN_RAW:
        return "FlatTail"
    eps = max(1e-6, span * 0.01)
    if all(tail[i] > tail[i - 1] + eps for i in range(1, len(tail))):
        return "StrictIncTail"
    if all(tail[i] < tail[i - 1] - eps for i in range(1, len(tail))):
        return "StrictDecTail"
    return "NonMonoTail"


def step_from_path(path):
    m = re.search(r"step=(\d+)/", path)
    return int(m.group(1)) if m else -1


def analyze_file(label):
    path = (
        rf"C:\Users\menghl2\WorkSpace\Projects\Tools\M576增加1310波长自校准软件开发"
        rf"\M576Calibrator\CrossPeakTest\comm_2026-07-07_recal_sweeps {label}.csv"
    )
    rows = []
    with open(path, newline="", encoding="utf-8") as f:
        reader = csv.reader(f)
        next(reader)
        for fields in reader:
            row = parse_row(fields)
            if row:
                rows.append(row)

    print(f"=== Machine dacRange={label}: {len(rows)} sweeps ===")
    csv_ok = sum(1 for r in rows if r["peak_ok"] == 1)
    print(f"csv_peak_ok: {csv_ok}/{len(rows)} ({100.0 * csv_ok / len(rows):.1f}%)")

    att5p = [r for r in rows if r["attempt"] >= 5]
    att5_ok = [r for r in att5p if r["peak_ok"] == 1]
    print(f"attempt>=5: {len(att5p)} rows, peak_ok=1: {len(att5_ok)}")

    inc_ok = [r for r in rows if r["peak_ok"] == 1 and trend(r["pows"])[0] == "StrictInc"]
    dec_ok = [r for r in rows if r["peak_ok"] == 1 and trend(r["pows"])[0] == "StrictDec"]
    print(f"ALL rows peak_ok=1 + full StrictInc: {len(inc_ok)}")
    print(f"ALL rows peak_ok=1 + full StrictDec: {len(dec_ok)}")

    tail_inc_att5 = [r for r in att5_ok if mono_tail(r["pows"]) == "StrictIncTail"]
    print(f"attempt>=5 peak_ok=1 + StrictIncTail (half-window): {len(tail_inc_att5)}")

    suspicious = []
    for r in att5_ok:
        tr, span = trend(r["pows"])
        if tr in ("StrictInc", "StrictDec"):
            suspicious.append((r, tr, span))
    print(f"SUSPICIOUS attempt>=5 ok=1 + FULL mono: {len(suspicious)}")
    for r, tr, span in suspicious:
        print(
            f"  step={step_from_path(r['path'])} att={r['attempt']} {tr} span={span:.0f} "
            f"off={r['offset']} code={r['code']} mode={r['mode']} baseY={r['baseY']}"
        )

    # csv ok but code not Ok
    bad_code_ok = [r for r in rows if r["peak_ok"] == 1 and r["code"] != "Ok"]
    print(f"peak_ok=1 but code!=Ok: {len(bad_code_ok)}")

    # csv mismatch: peak_ok=0 but looks like clear bell on @200
    # Group by path for heavy retry sequences
    by_path = defaultdict(list)
    for r in rows:
        by_path[r["path"]].append(r)

    heavy = sorted(((p, max(x["attempt"] for x in rs)) for p, rs in by_path.items()), key=lambda x: -x[1])
    print("Top retry sequences:")
    for p, ma in heavy[:10]:
        if ma < 5:
            continue
        rs = sorted(by_path[p], key=lambda x: x["attempt"])
        parts = []
        for x in rs:
            tr, sp = trend(x["pows"])
            tag = "Ok" if x["peak_ok"] else x["code"][:12]
            parts.append(f"a{x['attempt']}:{tag}@{x['offset']}({tr})")
        print(f"  max={ma} step={step_from_path(p)}")
        print(f"    {' | '.join(parts)}")

    # Stitch-like: attempt 4+ with offset 200
    stitch_like = [r for r in rows if r["attempt"] >= 4 and r["offset"] == 200]
    print(f"attempt>=4 @200 (stitch/explore): {len(stitch_like)}")
    stitch_ok = [r for r in stitch_like if r["peak_ok"] == 1]
    for r in stitch_ok[:12]:
        tr, sp = trend(r["pows"])
        print(
            f"  step={step_from_path(r['path'])} att={r['attempt']} ok=1 {tr} span={sp:.0f} code={r['code']}"
        )

    # Compare attempt 5 ok rows: is it fineRefine @64 after coarse?
    print("attempt==5 peak_ok=1 detail:")
    for r in [x for x in rows if x["attempt"] == 5 and x["peak_ok"] == 1]:
        tr, sp = trend(r["pows"])
        tail = mono_tail(r["pows"])
        print(
            f"  step={step_from_path(r['path'])} off={r['offset']} code={r['code']} "
            f"trend={tr} tail={tail} span={sp:.0f} n={len(valid_powers(r['pows']))} mode={r['mode']}"
        )
    print()


if __name__ == "__main__":
    analyze_file("65")
    analyze_file("208")
