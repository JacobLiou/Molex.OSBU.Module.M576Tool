@echo off
setlocal
cd /d "%~dp0"

echo Installing dependencies...
py -3 -m pip install -r requirements-analysis.txt
if errorlevel 1 exit /b 1

echo Building RepeatabilityReport.exe ...
py -3 -m PyInstaller --onefile --noconsole --name RepeatabilityReport --distpath dist --workpath build repeatability_report.py
if errorlevel 1 exit /b 1

echo.
echo Done: dist\RepeatabilityReport.exe
echo Copy RepeatabilityReport.exe to the folder containing diagnosis_log.csv and double-click.
endlocal
