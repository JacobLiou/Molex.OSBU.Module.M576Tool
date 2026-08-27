@echo off
setlocal EnableExtensions EnableDelayedExpansion

rem Package M576Calibrator for offline copy (Win32 Release + VC143 CRT/MFC DLLs next to exe).
rem Usage:
rem   package_m576.bat              - MSBuild Release|Win32, then stage dist
rem   package_m576.bat nobuild      - skip build, only copy from existing Release output
rem   package_m576.bat nopkill      - do not taskkill M576CalibratorApp.exe before build (default kills if running to avoid LNK1104)
rem   package_m576.bat nobuild nopkill  - either order for two flags

set "SCRIPT_DIR=%~dp0"
set "SLN=%SCRIPT_DIR%M576Calibrator.sln"
set "APP_REL=%SCRIPT_DIR%M576CalibratorApp\Release\M576CalibratorApp.exe"
set "INI_REL=%SCRIPT_DIR%M576CalibratorApp\Release\M576Calibrator.ini"
set "INI_SRC=%SCRIPT_DIR%M576CalibratorApp\M576Calibrator.ini"
set "OUT_SRC=%SCRIPT_DIR%M576CalibratorApp\Release\output"
set "CSV_SRC=%SCRIPT_DIR%output"
set "DIST=%SCRIPT_DIR%dist\M576Calibrator"

set "NOBUILD=0"
set "NOPKILL=0"
if /I "%~1"=="nobuild" set "NOBUILD=1"
if /I "%~1"=="nopkill" set "NOPKILL=1"
if /I "%~2"=="nobuild" set "NOBUILD=1"
if /I "%~2"=="nopkill" set "NOPKILL=1"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
  echo ERROR: vswhere not found: "%VSWHERE%"
  echo Install Visual Studio Build Tools or VS with C++ workload.
  exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\Current\Bin\MSBuild.exe`) do set "MSBUILD=%%i"
if not defined MSBUILD (
  for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do set "MSBUILD=%%i"
)
if not defined MSBUILD (
  echo ERROR: MSBuild.exe not found via vswhere.
  exit /b 1
)

if "%NOBUILD%"=="0" (
  echo Building Release ^| Win32 ^(v143^) ...
  rem Linker LNK1104 if exe is running, held by AV, or synced (e.g. OneDrive). Clear read-only then optionally end process.
  if exist "%APP_REL%" attrib -R "%APP_REL%" >nul 2>&1
  if "%NOPKILL%"=="0" (
    echo Closing M576CalibratorApp.exe if running ^(avoids linker LNK1104: cannot open exe^)...
    taskkill /F /IM M576CalibratorApp.exe >nul 2>&1
    ping -n 2 127.0.0.1 >nul
    if exist "%APP_REL%" attrib -R "%APP_REL%" >nul 2>&1
  )
  rem LTCG can leave a corrupt *.ipdb / *.iobj after interrupted links (C1354 / LNK1257). Always drop them before build.
  call :clean_ltcg_intermediates
  "%MSBUILD%" "%SLN%" /p:Configuration=Release /p:Platform=Win32 /v:m /nologo
  if errorlevel 1 (
    echo WARNING: Build failed; cleaning LTCG intermediates and retrying once ^(C1354/LNK1257 corrupt ipdb^)...
    call :clean_ltcg_intermediates
    "%MSBUILD%" "%SLN%" /t:M576CalibratorApp /p:Configuration=Release /p:Platform=Win32 /v:m /nologo
  )
  if errorlevel 1 (
    echo ERROR: Build failed.
    echo Hint: Close M576CalibratorApp.exe and any Explorer window inside Release\, then retry.
    echo       If C1354/LNK1257 ^(corrupt ipdb^): delete M576CalibratorApp\Release\*.ipdb *.iobj and rebuild.
    echo       Or run: package_m576.bat nobuild
    exit /b 1
  )
) else (
  echo Skipping build ^(nobuild^).
)

if not exist "%APP_REL%" (
  echo ERROR: Missing built exe: "%APP_REL%"
  echo Build first or remove nobuild flag.
  exit /b 1
)

echo Staging: "%DIST%"
if exist "%DIST%" rmdir /s /q "%DIST%"
mkdir "%DIST%" 2>nul
mkdir "%DIST%\output" 2>nul
mkdir "%DIST%\redist" 2>nul

copy /Y "%APP_REL%" "%DIST%\" >nul
if errorlevel 1 (
  echo ERROR: Copy exe failed.
  exit /b 1
)

set "INI_OK=0"
if exist "%INI_REL%" (
  copy /Y "%INI_REL%" "%DIST%\M576Calibrator.ini" >nul
  if not errorlevel 1 set "INI_OK=1"
)
if "!INI_OK!"=="0" if exist "%INI_SRC%" (
  copy /Y "%INI_SRC%" "%DIST%\M576Calibrator.ini" >nul
  if not errorlevel 1 set "INI_OK=1"
)
if "!INI_OK!"=="0" (
  echo WARNING: M576Calibrator.ini not found in Release or M576CalibratorApp\; PeakFinder MinProminenceDb will use built-in default.
) else (
  echo Copied: M576Calibrator.ini ^(exe-side config; restart app after edit^).
)

set "CSV_OK=0"
rem Per-trans path CSVs (4 PM + 4 PD). Prefer build output, else solution output\.
for %%F in (
  pm_mcs1.csv
  pm_mcs2.csv
  pm_1x64_1.csv
  pm_1x64_2.csv
  pm_1x64_1Mapping.csv
  pm_1x64_2Mapping.csv
  pd_mcs1.csv
  pd_mcs2.csv
  pd_1x64_1.csv
  pd_1x64_2.csv
) do (
  if exist "%OUT_SRC%\%%F" (
    copy /Y "%OUT_SRC%\%%F" "%DIST%\output\" >nul
    if not errorlevel 1 set "CSV_OK=1"
  )
  if not exist "%OUT_SRC%\%%F" if exist "%CSV_SRC%\%%F" (
    copy /Y "%CSV_SRC%\%%F" "%DIST%\output\" >nul
    if not errorlevel 1 set "CSV_OK=1"
  )
)

set "DIAG_OK=0"
if exist "%OUT_SRC%\diagnosis_sw.csv" (
  copy /Y "%OUT_SRC%\diagnosis_sw.csv" "%DIST%\output\" >nul
  if not errorlevel 1 set "DIAG_OK=1"
)
if "!DIAG_OK!"=="0" if exist "%CSV_SRC%\diagnosis_sw.csv" (
  copy /Y "%CSV_SRC%\diagnosis_sw.csv" "%DIST%\output\" >nul
  if not errorlevel 1 set "DIAG_OK=1"
)
if "!DIAG_OK!"=="0" (
  echo WARNING: Diagnosis sw CSV not found in "%OUT_SRC%" or "%CSV_SRC%": diagnosis_sw.csv
  echo          Run Diagnosis will fail until a valid CSV is placed at exe\output\diagnosis_sw.csv.
)
if "!CSV_OK!"=="0" (
  echo WARNING: No per-trans path CSVs found in "%OUT_SRC%" or "%CSV_SRC%".
  echo          Expected: pm_mcs1, pm_mcs2, pm_1x64_1, pm_1x64_2, pm_1x64_1Mapping, pm_1x64_2Mapping, pd_mcs1, pd_mcs2, pd_1x64_1, pd_1x64_2 ^(.csv^).
  echo          Generate from tools\split_path_csv_eight.ps1 or copy from a built M576CalibratorApp\Release\output folder.
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
if not defined VSINSTALL (
  for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -property installationPath`) do set "VSINSTALL=%%i"
)
if not defined VSINSTALL (
  echo WARNING: Could not resolve VS installation path; skipping CRT/MFC DLL copy.
  goto :readme
)

set "MSVC_REDIST_BASE=!VSINSTALL!\VC\Redist\MSVC"
if not exist "!MSVC_REDIST_BASE!" (
  echo WARNING: Not found: "!MSVC_REDIST_BASE!"
  echo Skipping CRT/MFC DLL copy. Install VC++ x86 Redistributable on target PC.
  goto :readme
)

rem Latest 14.xx redist folder only (skip entries like "v143")
set "MSVC_VER="
for /f "delims=" %%d in ('dir /b /ad /o-n "!MSVC_REDIST_BASE!\14.*" 2^>nul') do (
  set "MSVC_VER=%%d"
  goto :have_msvc_ver
)
:have_msvc_ver
if not defined MSVC_VER (
  echo WARNING: No MSVC Redist subfolder matching 14.* under "!MSVC_REDIST_BASE!"
  goto :readme
)

set "X86_BASE=!MSVC_REDIST_BASE!\!MSVC_VER!\x86"
set "CRT_DIR="
set "MFC_DIR="

if exist "!X86_BASE!\Microsoft.VC143.CRT" set "CRT_DIR=!X86_BASE!\Microsoft.VC143.CRT"
if exist "!X86_BASE!\Microsoft.VC143.MFC"  set "MFC_DIR=!X86_BASE!\Microsoft.VC143.MFC"

if not defined CRT_DIR (
  echo WARNING: No Microsoft.VC143.CRT under "!X86_BASE!"
)
if not defined MFC_DIR (
  echo WARNING: No Microsoft.VC143.MFC under "!X86_BASE!"
)

if defined CRT_DIR (
  echo Copying CRT DLLs from: "!CRT_DIR!"
  copy /Y "!CRT_DIR!\*.dll" "%DIST%\" >nul 2>nul
)
if defined MFC_DIR (
  echo Copying MFC DLLs from: "!MFC_DIR!"
  copy /Y "!MFC_DIR!\*.dll" "%DIST%\" >nul 2>nul
)

set "VCREDIST=!VSINSTALL!\VC\Redist\MSVC\!MSVC_VER!\vc_redist.x86.exe"
if exist "!VCREDIST!" (
  copy /Y "!VCREDIST!" "%DIST%\redist\" >nul
  echo Optional installer copied: dist\M576Calibrator\redist\vc_redist.x86.exe
) else (
  echo NOTE: vc_redist.x86.exe not found at "!VCREDIST!" ^(optional^).
)

:readme
rem Write README without a parenthesized block (pipes/colons inside "echo ... | ..." break cmd parsing).
set "README=%DIST%\README-deploy.txt"
> "%README%" echo M576Calibrator - offline folder
>> "%README%" echo.
>> "%README%" echo Built with Visual Studio toolset v143 (VS 2022).
>> "%README%" echo Run: M576CalibratorApp.exe
>> "%README%" echo Requires: Windows x64/x86 capable of running 32-bit apps; USB serial driver for 429F if needed.
>> "%README%" echo.
>> "%README%" echo CRT/MFC DLLs are VC143 redistributables copied next to the exe when packaging on a machine with VS.
>> "%README%" echo If a PC still reports missing DLLs, install: redist\vc_redist.x86.exe
>> "%README%" echo.
>> "%README%" echo Config: M576Calibrator.ini next to the exe ([PeakFinder] MinProminenceDb etc.; restart after edit).
>> "%README%" echo.
>> "%README%" echo Default path CSVs - per trans, under .\output\ :
>> "%README%" echo   PM: pm_mcs1.csv, pm_mcs2.csv, pm_1x64_1.csv, pm_1x64_2.csv
>> "%README%" echo       plus firmware MEMS slot map: pm_1x64_1Mapping.csv, pm_1x64_2Mapping.csv (required for PM 1x64 calibration).
>> "%README%" echo   PD: pd_mcs1.csv, pd_mcs2.csv, pd_1x64_1.csv, pd_1x64_2.csv
>> "%README%" echo Optional: generate the eight PM/PD path CSVs from standard_pm or standard_pd via tools\split_path_csv_eight.ps1 under repo output\.
>> "%README%" echo.
>> "%README%" echo Diagnosis flow (Run/Stop Diagnosis buttons):
>> "%README%" echo   Input:  output\diagnosis_sw.csv  (SW groups with pipe separators; optional first token = channel)
>> "%README%" echo   Output: output\diagnosis_log.csv  (append: 7 cols - Channel, s1/s2/s3 pd+opm; fflush per row)
>> "%README%" echo   After each SW group: per s1,s2,s3: SW3+WL+pd+opm (no precheck; see diagnosis_sw.csv.example).

echo.
echo Done. Output: "%DIST%"
echo See README-deploy.txt inside the folder.
exit /b 0

:clean_ltcg_intermediates
rem Drop Whole-Program-Optimization cache that often corrupts across interrupted links.
for %%D in (
  "%SCRIPT_DIR%M576CalibratorApp\Release"
  "%SCRIPT_DIR%Z4671Core\Release"
  "%SCRIPT_DIR%CrossPeakTest\Release"
) do (
  if exist "%%~D\*.ipdb" del /f /q "%%~D\*.ipdb" >nul 2>&1
  if exist "%%~D\*.iobj" del /f /q "%%~D\*.iobj" >nul 2>&1
)
exit /b 0
