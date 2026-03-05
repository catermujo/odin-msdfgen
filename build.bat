@echo off

setlocal EnableDelayedExpansion

call :ensure_msvc || exit /b 1

REM DUMBAI: Reuse the same Visual Studio environment for both vcpkg resolution and the later cl/lib steps.
if not defined VCPKG_ROOT if defined VSINSTALLDIR if exist "%VSINSTALLDIR%VC\vcpkg" set "VCPKG_ROOT=%VSINSTALLDIR%VC\vcpkg"

set SRC=msdfgen
if not exist "%SRC%" (
    git clone --revision 1874bcf7d9624ccc85b4bc9a85d78116f690f35b https://github.com/Chlumsky/msdfgen "%SRC%" --depth=1 || exit /b 1
)

rem Apply patches
for %%p in (patches\*.patch) do (
    git -C "%SRC%" am --3way "%%p" 2>nul || echo Patch %%p skipped
)

pushd "%SRC%" || exit /b 1

if exist "vcpkg.json" (
    call :ensure_vcpkg_baseline || exit /b 1
)

set BIN=build
cmake . -B "%BIN%" -A x64 -DCMAKE_BUILD_TYPE=Release -DMSDFGEN_INSTALL=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded || exit /b 1

set NCORE=%NUMBER_OF_PROCESSORS%
set DLL_EXT=dll

echo Building project...
cmake --build "%BIN%" --config Release -j %NCORE% || exit /b 1
copy /y "%BIN%\msdfgen-config.h" . >nul || exit /b 1
if exist "%BIN%\Release\*.%DLL_EXT%" (
    copy /y "%BIN%\Release\*.%DLL_EXT%" .. >nul || exit /b 1
) else (
    REM msdfgen's default Windows config often produces static libs only.
    REM We still produce msdf.dll below from the wrapper sources.
    echo No upstream %DLL_EXT% files in %BIN%\Release; continuing...
)
popd || exit /b 1

set VCPKG_LIB=%SRC%\%BIN%\vcpkg_installed\x64-windows-static

REM Static dependencies for linking
set STATIC_DEPS=brotlicommon.lib brotlidec.lib bz2.lib freetype.lib libpng16.lib tinyxml2.lib zlib.lib skia.lib

echo Building shared lib...
set ALL_LIBS="%SRC%\%BIN%\Release\msdfgen-core.lib" "%SRC%\%BIN%\Release\msdfgen-ext.lib"
for %%f in (%STATIC_DEPS%) do (
    set ALL_LIBS=!ALL_LIBS! "%VCPKG_LIB%\lib\%%f"
)
cl /LD /MT /EHsc /O2 /I. /I"%SRC%" msdfgen-c\msdfgen-core.cpp msdfgen-c\msdfgen-ext.cpp !ALL_LIBS! /Fe:msdf.%DLL_EXT% /link /IMPLIB:msdf_shared.lib /DEF:msdfgen-c\msdfgen.def || exit /b 1
if exist msdfgen-core.obj del msdfgen-core.obj
if exist msdfgen-ext.obj del msdfgen-ext.obj
if exist msdf.exp del msdf.exp

echo Building static lib...
set STATIC_DEPS=brotlicommon.lib brotlidec.lib bz2.lib libpng16.lib tinyxml2.lib zlib.lib skia.lib
set ALL_STATIC_LIBS="%SRC%\%BIN%\Release\msdfgen-core.lib" "%SRC%\%BIN%\Release\msdfgen-ext.lib"
for %%f in (%STATIC_DEPS%) do (
    set ALL_STATIC_LIBS=!ALL_STATIC_LIBS! "%VCPKG_LIB%\lib\%%f"
)
cl /c /MT /EHsc /O2 /I. /I"%SRC%" msdfgen-c\msdfgen-core.cpp /Fo:core.obj || exit /b 1
cl /c /MT /EHsc /O2 /I. /I"%SRC%" msdfgen-c\msdfgen-ext.cpp /Fo:ext.obj || exit /b 1
lib /OUT:msdf.lib !ALL_STATIC_LIBS! core.obj ext.obj || exit /b 1
if exist core.obj del core.obj
if exist ext.obj del ext.obj

echo Build completed successfully!
exit /b 0

:ensure_msvc
where cl >nul 2>nul
if not errorlevel 1 goto :eof

REM DUMBAI: Bootstrap the MSVC environment so vcpkg and the wrapper compile/link steps agree on the active toolchain.
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo ERROR: Could not find vswhere.exe.
    exit /b 1
)
for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"
if not defined VSINSTALL (
    echo ERROR: Could not find a Visual Studio installation with MSVC tools.
    exit /b 1
)
call "%VSINSTALL%\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul || exit /b 1
goto :eof

:ensure_vcpkg_baseline
findstr /C:"\"builtin-baseline\"" "vcpkg.json" >nul
if not errorlevel 1 goto :eof

echo vcpkg.json has no builtin-baseline, adding one...
set VCPKG_EXE=

if defined VCPKG_ROOT if exist "%VCPKG_ROOT%\vcpkg.exe" set "VCPKG_EXE=%VCPKG_ROOT%\vcpkg.exe"

if not defined VCPKG_EXE (
    for %%P in (vcpkg.exe) do (
        if not "%%~$PATH:P"=="" set "VCPKG_EXE=%%~$PATH:P"
    )
)

if not defined VCPKG_EXE call :find_vs_vcpkg

if not defined VCPKG_EXE (
    echo ERROR: Could not find vcpkg.exe. Install vcpkg or set VCPKG_ROOT.
    exit /b 1
)

"%VCPKG_EXE%" x-update-baseline --add-initial-baseline || exit /b 1
goto :eof

:find_vs_vcpkg
for %%E in (
    "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\vcpkg\vcpkg.exe"
    "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\VC\vcpkg\vcpkg.exe"
    "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\VC\vcpkg\vcpkg.exe"
    "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools\VC\vcpkg\vcpkg.exe"
    "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\vcpkg\vcpkg.exe"
    "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\VC\vcpkg\vcpkg.exe"
    "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise\VC\vcpkg\vcpkg.exe"
    "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\VC\vcpkg\vcpkg.exe"
) do (
    if exist %%~E (
        set "VCPKG_EXE=%%~E"
        goto :eof
    )
)
goto :eof
