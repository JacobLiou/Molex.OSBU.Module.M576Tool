# FullEdit Bin Implementation Plan

> See also: `docs/superpowers/specs/2026-08-26-fulledit-bin-dac-design.md`

## Tasks

- [x] Spec + INV-33 + 使用说明研发节
- [x] PathDacImpact + FullEditDacCsv + CrossPeakTest
- [x] FullEditDacIo MCS/MEMS export-diff-write
- [x] CM576FullEditDlg + 主窗 FullEdit 按钮

## Verify

```text
msbuild M576Calibrator\CrossPeakTest\CrossPeakTest.vcxproj /p:Configuration=Release /p:Platform=Win32
M576Calibrator\CrossPeakTest\Release\CrossPeakTest.exe
msbuild M576Calibrator\M576CalibratorApp\M576CalibratorApp.vcxproj /p:Configuration=Release /p:Platform=Win32
```
