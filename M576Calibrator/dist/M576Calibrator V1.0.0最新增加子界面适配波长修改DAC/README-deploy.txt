M576Calibrator - offline folder

Built with Visual Studio toolset v143 (VS 2022).
Run: M576CalibratorApp.exe
Requires: Windows x64/x86 capable of running 32-bit apps; USB serial driver for 429F if needed.

CRT/MFC DLLs are VC143 redistributables copied next to the exe when packaging on a machine with VS.
If a PC still reports missing DLLs, install: redist\vc_redist.x86.exe

Config: M576Calibrator.ini next to the exe ([PeakFinder] MinProminenceDb etc.; restart after edit).

Default path CSVs - per trans, under .\output\ :
  PM: pm_mcs1.csv, pm_mcs2.csv, pm_1x64_1.csv, pm_1x64_2.csv
      plus firmware MEMS slot map: pm_1x64_1Mapping.csv, pm_1x64_2Mapping.csv (required for PM 1x64 calibration).
  PD: pd_mcs1.csv, pd_mcs2.csv, pd_1x64_1.csv, pd_1x64_2.csv
Optional: generate the eight PM/PD path CSVs from standard_pm or standard_pd via tools\split_path_csv_eight.ps1 under repo output\.

Diagnosis flow (Run/Stop Diagnosis buttons):
  Input:  output\diagnosis_sw.csv  (SW groups with pipe separators; optional first token = channel)
  Output: output\diagnosis_log.csv  (append: 7 cols - Channel, s1/s2/s3 pd+opm; fflush per row)
  After each SW group: per s1,s2,s3: SW3+WL+pd+opm (no precheck; see diagnosis_sw.csv.example).
