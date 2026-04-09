sample_path.csv (1286 rows) generation
=======================================

PRD formula (需求Spec PRD.md line 78):
  64 + 3 + 32*18 + 32*18 + 64 + 3 = 1286

Blocks:
  64   — target_index=1, sweep 1#1x64 front channels 1..64
  3    — target_index=2, boundary samples (ch 1,32,64)
  576  — target_index=3, full 1# MCS grid 32 switches x 18 channels
  576  — target_index=4, full 2# MCS grid 32 x 18
  64   — target_index=5, sweep 2#1x64 back channels 1..64
  3    — target_index=6, boundary samples (ch 1,32,64)

Regenerate:
  python tools/generate_sample_path_1286.py

Tune path tuples with firmware; routing helpers follow PRD 1x64 ch 1-32 vs 33-64 to MCS sides.
