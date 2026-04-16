@echo off
setlocal EnableExtensions EnableDelayedExpansion

rem Package M576Calibrator for offline copy (Win32 Release + VC143 CRT/MFC DLLs next to exe).
rem Usage:
rem   package_m576.bat           - MSBuild Release|Win32, then stage dist
rem   package_m576.bat nobuild   - skip build, only copy from existing Release output

set "SCRIPT_DIR=%~dp0"
set "SLN=%SCRIPT_DIR%M576Calibrator.sln"
set "APP_REL=%SCRIPT_DIR%M576CalibratorApp\Release\M576CalibratorApp.exe"
set "OUT_SRC=%SCRIPT_DIR%M576CalibratorApp\Release\output"
set "CSV_SRC=%SCRIPT_DIR%output"
set "DIST=%SCRIPT_DIR%dist\M576Calibrator"

set "NOBUILD=0"
if /I "%~1"=="nobuild" set "NOBUILD=1"

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
  "%MSBUILD%" "%SLN%" /p:Configuration=Release /p:Platform=Win32 /v:m /nologo
  if errorlevel 1 (
    echo ERROR: Build failed.
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

set "CSV_OK=0"
if exist "%OUT_SRC%\standard_pm.csv" (
  copy /Y "%OUT_SRC%\standard_pm.csv" "%DIST%\output\" >nul && set "CSV_OK=1"
)
if exist "%OUT_SRC%\standard_pd.csv" (
  copy /Y "%OUT_SRC%\standard_pd.csv" "%DIST%\output\" >nul && set "CSV_OK=1"
)
if "!CSV_OK!"=="0" (
  if exist "%CSV_SRC%\standard_pm.csv" copy /Y "%CSV_SRC%\standard_pm.csv" "%DIST%\output\" >nul
  if exist "%CSV_SRC%\standard_pd.csv" copy /Y "%CSV_SRC%\standard_pd.csv" "%DIST%\output\" >nul
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
> "%DIST%\README-deploy.txt" (
  echo M576Calibrator - offline folder
  echo.
  echo Built with Visual Studio toolset v143 ^(VS 2022^).
  echo Run: M576CalibratorApp.exe
  echo Requires: Windows x64/x86 capable of running 32-bit apps; USB serial driver for 429F if needed.
  echo.
  echo CRT/MFC DLLs are VC143 redistributables copied next to the exe when packaging on a machine with VS.
  echo If a PC still reports missing DLLs, install: redist\vc_redist.x86.exe
  echo.
  echo Default CSV paths are .\output\standard_pm.csv and .\output\standard_pd.csv
)

echo.
echo Done. Output: "%DIST%"
echo See README-deploy.txt inside the folder.
exit /b 0
