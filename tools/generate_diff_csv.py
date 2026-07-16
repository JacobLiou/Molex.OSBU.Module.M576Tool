import csv
import math
from pathlib import Path


def find_pairs(root: Path):
    pairs = []
    for backup_file in root.rglob("*_backupAll1310DAC.csv"):
        standard_file = backup_file.with_name(
            backup_file.name.replace("_backupAll1310DAC.csv", "_standardAll1310DAC.csv")
        )
        if standard_file.exists():
            pairs.append((backup_file, standard_file))
    return sorted(pairs)


def read_csv_rows(path: Path):
    with path.open("r", newline="", encoding="utf-8-sig") as f:
        lines = [line for line in f if not line.startswith("#") and line.strip()]

    reader = csv.DictReader(lines)
    rows = list(reader)
    if reader.fieldnames is None:
        raise ValueError(f"No header found in {path}")
    return reader.fieldnames, rows


def to_int(value: str, field_name: str, src: Path):
    try:
        return int(value)
    except ValueError as exc:
        raise ValueError(f"Invalid integer in {src} field {field_name}: {value}") from exc


def main():
    root = Path(__file__).resolve().parent

    pairs = find_pairs(root)
    if not pairs:
        raise SystemExit("No backup/standard file pairs found.")

    rows_by_group = {}
    fieldnames_by_group = {}

    for backup_path, standard_path in pairs:
        backup_fields, backup_rows = read_csv_rows(backup_path)
        standard_fields, standard_rows = read_csv_rows(standard_path)

        if backup_fields != standard_fields:
            raise ValueError(
                f"Header mismatch:\n  {backup_path}\n  {standard_path}\n"
                f"backup={backup_fields}\nstandard={standard_fields}"
            )

        required = {"dac_y", "dac_x"}
        if not required.issubset(set(backup_fields)):
            raise ValueError(f"Required columns missing in {backup_path}: {required}")

        key_fields = [c for c in backup_fields if c not in ("dac_y", "dac_x")]

        backup_map = {}
        for row in backup_rows:
            key = tuple(row[k] for k in key_fields)
            backup_map[key] = row

        standard_map = {}
        for row in standard_rows:
            key = tuple(row[k] for k in key_fields)
            standard_map[key] = row

        missing_in_standard = sorted(set(backup_map) - set(standard_map))
        missing_in_backup = sorted(set(standard_map) - set(backup_map))
        if missing_in_standard or missing_in_backup:
            raise ValueError(
                f"Key mismatch between files:\n  {backup_path}\n  {standard_path}\n"
                f"missing_in_standard={len(missing_in_standard)}\n"
                f"missing_in_backup={len(missing_in_backup)}"
            )

        pair_name = backup_path.parent.name
        if pair_name not in rows_by_group:
            rows_by_group[pair_name] = []

            fieldnames = [
                "group",
                "backup_file",
                "standard_file",
                *key_fields,
                "backup_dac_y",
                "standard_dac_y",
                "delta_dac_y",
                "abs_delta_dac_y",
                "backup_dac_x",
                "standard_dac_x",
                "delta_dac_x",
                "abs_delta_dac_x",
                "euclidean_diff",
                "diff_ratio_pct",
            ]
            fieldnames_by_group[pair_name] = fieldnames

        for key in sorted(backup_map.keys()):
            b = backup_map[key]
            s = standard_map[key]

            b_y = to_int(b["dac_y"], "dac_y", backup_path)
            s_y = to_int(s["dac_y"], "dac_y", standard_path)
            b_x = to_int(b["dac_x"], "dac_x", backup_path)
            s_x = to_int(s["dac_x"], "dac_x", standard_path)

            delta_y = s_y - b_y
            delta_x = s_x - b_x
            abs_delta_y = abs(delta_y)
            abs_delta_x = abs(delta_x)
            euclid_diff = math.sqrt(delta_y * delta_y + delta_x * delta_x)

            baseline_norm = math.sqrt(b_y * b_y + b_x * b_x)
            diff_ratio_pct = (euclid_diff / baseline_norm * 100.0) if baseline_norm else 0.0

            out_row = {
                "group": pair_name,
                "backup_file": backup_path.name,
                "standard_file": standard_path.name,
            }
            for i, k in enumerate(key_fields):
                out_row[k] = key[i]

            out_row.update(
                {
                    "backup_dac_y": b_y,
                    "standard_dac_y": s_y,
                    "delta_dac_y": delta_y,
                    "abs_delta_dac_y": abs_delta_y,
                    "backup_dac_x": b_x,
                    "standard_dac_x": s_x,
                    "delta_dac_x": delta_x,
                    "abs_delta_dac_x": abs_delta_x,
                    "euclidean_diff": f"{euclid_diff:.6f}",
                    "diff_ratio_pct": f"{diff_ratio_pct:.6f}",
                }
            )

            rows_by_group[pair_name].append(out_row)

    total_rows = 0
    for group_name in sorted(rows_by_group.keys()):
        output_path = root / group_name / "diff.csv"
        rows = rows_by_group[group_name]
        fieldnames = fieldnames_by_group[group_name]

        with output_path.open("w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=fieldnames)
            writer.writeheader()
            writer.writerows(rows)

        total_rows += len(rows)
        print(f"Wrote {len(rows)} rows to {output_path}")

    print(f"Done. Total rows across groups: {total_rows}")


if __name__ == "__main__":
    main()
