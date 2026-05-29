@echo off
setlocal
cd /d "%~dp0"

echo Installing dependencies...
py -3 -m pip install -r requirements-analysis.txt
if errorlevel 1 exit /b 1

echo Building ExtractCommLogs.exe ...
py -3 -m PyInstaller --noconfirm ExtractCommLogs.spec
if errorlevel 1 exit /b 1

echo.
echo Done: dist\ExtractCommLogs.exe
if exist "..\M576Calibrator\output" (
  copy /Y "dist\ExtractCommLogs.exe" "..\M576Calibrator\output\ExtractCommLogs.exe" >nul
  echo Also copied to ..\M576Calibrator\output\ExtractCommLogs.exe
)
echo.
echo Usage: copy ExtractCommLogs.exe next to comm_*.log and double-click.
echo Outputs: comm_*_recal_sweeps.csv, comm_*_peak_summary.csv (overwrite if present).
endlocal
