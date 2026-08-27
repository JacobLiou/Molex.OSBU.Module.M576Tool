#!/usr/bin/env python3
"""Export MCS Z4671 bin DAC: one row per (sw,ch), LOW/ROOM/HIGH as columns."""
from __future__ import annotations

import argparse
import struct
from pathlib import Path

TEMP_CALIB_NUM = 4
SW_COUNT = 34
CH_COUNT = 32  # PORT_MAX_COUNT(20) + MID_MAX_COUNT(12)
IDX_LOW, IDX_ROOM, IDX_HIGH = 0, 1, 2

H1 = 8 + 8 + 4
H2 = 4 + 2 + 2 + 4 + 16 + 32 + 32 + 16
IMG = 32
PAYLOAD_OFF = H1 + H2 + IMG * 2
DAC_OFF = 120  # offset of wCalibPtrDAC inside stLutSettingZ4671
TEMP_POINT_OFF = DAC_OFF + SW_COUNT * TEMP_CALIB_NUM * CH_COUNT * 2 * 2


def lut_sw_to_optical_block(sw: int):
    """Reverse of M576McsBlock1To32ToLutSwIdx0 for sw 0..31."""
    if 0 <= sw <= 15:
        return sw + 17
    if 16 <= sw <= 31:
        return sw - 15
    return None


def dac_word_off(sw: int, temp: int, ch: int, axis: int) -> int:
    return DAC_OFF + (((sw * TEMP_CALIB_NUM + temp) * CH_COUNT + ch) * 2 + axis) * 2


def cstr(buf: bytes) -> str:
    return buf.split(b"\x00", 1)[0].decode("ascii", errors="replace").strip()


def read_yx(lut: bytes, sw: int, temp: int, ch: int):
    y = struct.unpack_from("<h", lut, dac_word_off(sw, temp, ch, 0))[0]
    x = struct.unpack_from("<h", lut, dac_word_off(sw, temp, ch, 1))[0]
    return y, x


def temp_point(lut: bytes, sw: int, temp: int) -> int:
    return struct.unpack_from("<h", lut, TEMP_POINT_OFF + (sw * TEMP_CALIB_NUM + temp) * 2)[0]


def export_bin(bin_path: Path, out_path: Path) -> None:
    data = bin_path.read_bytes()
    need = PAYLOAD_OFF + TEMP_POINT_OFF + SW_COUNT * TEMP_CALIB_NUM * 2
    if len(data) < need:
        raise SystemExit(f"file too small: {len(data)} < {need}")

    lut = data[PAYLOAD_OFF:]
    lut_sn = cstr(lut[4:36])
    lut_pn = cstr(lut[68:100])
    calib_date = cstr(lut[100:116])

    header = [
        "sn_label",
        "sw_lut_idx",
        "optical_block",
        "ch_idx",
        "ch_kind",
        "temp_point_LOW_0p1C",
        "temp_point_ROOM_0p1C",
        "temp_point_HIGH_0p1C",
        "LOW_dac_y",
        "LOW_dac_x",
        "ROOM_dac_y",
        "ROOM_dac_x",
        "HIGH_dac_y",
        "HIGH_dac_x",
        "delta_ROOM_LOW_y",
        "delta_ROOM_LOW_x",
        "delta_HIGH_LOW_y",
        "delta_HIGH_LOW_x",
        "delta_HIGH_ROOM_y",
        "delta_HIGH_ROOM_x",
    ]

    rows: list[list[str]] = []
    for sw in range(SW_COUNT):
        sn_label = f"SN{sw + 1}"
        opt = lut_sw_to_optical_block(sw)
        opt_s = "" if opt is None else str(opt)
        tp_low = temp_point(lut, sw, IDX_LOW)
        tp_room = temp_point(lut, sw, IDX_ROOM)
        tp_high = temp_point(lut, sw, IDX_HIGH)
        for ch in range(CH_COUNT):
            ly, lx = read_yx(lut, sw, IDX_LOW, ch)
            ry, rx = read_yx(lut, sw, IDX_ROOM, ch)
            hy, hx = read_yx(lut, sw, IDX_HIGH, ch)
            kind = "PORT" if ch < 20 else "MID"
            rows.append(
                [
                    sn_label,
                    str(sw),
                    opt_s,
                    str(ch),
                    kind,
                    str(tp_low),
                    str(tp_room),
                    str(tp_high),
                    str(ly),
                    str(lx),
                    str(ry),
                    str(rx),
                    str(hy),
                    str(hx),
                    str(ry - ly),
                    str(rx - lx),
                    str(hy - ly),
                    str(hx - lx),
                    str(hy - ry),
                    str(hx - rx),
                ]
            )

    with out_path.open("w", encoding="utf-8-sig", newline="\n") as f:
        f.write(f"# source_bin={bin_path.name}\n")
        f.write(f"# lut_sn={lut_sn} lut_pn={lut_pn} calib_date={calib_date}\n")
        f.write(
            "# one row per (sw,ch); columns LOW/ROOM/HIGH; [0]=dacY [1]=dacX signed int16\n"
        )
        f.write(
            "# optical_block=reverse(M576McsBlock1To32ToLutSwIdx0); blank for SN33/34\n"
        )
        f.write(
            "# delta_* = right_minus_left for side-by-side compare\n"
        )
        f.write(",".join(header) + "\n")
        for r in rows:
            f.write(",".join(r) + "\n")

    print(f"wrote {out_path}")
    print(f"rows={len(rows)} (34 sw * 32 ch; LOW/ROOM/HIGH as columns)")
    print(f"lut_sn={lut_sn} pn={lut_pn} calib={calib_date}")
    for name, ti in (("LOW", IDX_LOW), ("ROOM", IDX_ROOM), ("HIGH", IDX_HIGH)):
        nz = 0
        for sw in range(32):
            for ch in range(18):
                y, x = read_yx(lut, sw, ti, ch)
                if y != 0 or x != 0:
                    nz += 1
        print(f"  temp {name}: nonzero path slots (sw0-31,ch0-17) = {nz}/576")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        "bin",
        nargs="?",
        default=str(Path(__file__).with_name("K8226253-1_backup.bin")),
        help="MCS backup/standard .bin path",
    )
    ap.add_argument(
        "-o",
        "--out",
        default="",
        help="output CSV path (default: <bin_stem>_dac_all_temps.csv)",
    )
    args = ap.parse_args()
    bin_path = Path(args.bin)
    out_path = Path(args.out) if args.out else bin_path.with_name(bin_path.stem + "_dac_all_temps.csv")
    export_bin(bin_path, out_path)


if __name__ == "__main__":
    main()
