# -*- mode: python ; coding: utf-8 -*-
# PyInstaller spec: bundle tools/*.py next to the frozen extractor.

import os
from pathlib import Path

_spec_dir = Path(SPECPATH)
_tools = _spec_dir.parent / "tools"

_tool_scripts = [
    "recal_log_cmds.py",
    "extract_recal_sweep_csv.py",
    "extract_recal_peak_summary_csv.py",
]
_datas = [(str(_tools / name), "tools") for name in _tool_scripts if (_tools / name).is_file()]

a = Analysis(
    ["extract_comm_logs.py"],
    pathex=[str(_tools)],
    binaries=[],
    datas=_datas,
    hiddenimports=[
        "extract_recal_sweep_csv",
        "extract_recal_peak_summary_csv",
        "recal_log_cmds",
    ],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.datas,
    [],
    name="ExtractCommLogs",
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
