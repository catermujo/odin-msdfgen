@echo off

setlocal EnableDelayedExpansion

set SRC=msdfgen
if not exist "%SRC%\NUL" (
    git clone https://github.com/Chlumsky/msdfgen "%SRC%" --depth=1
)

pushd "%SRC%"
set BIN=build
cmake . -B "%BIN%" -A x64 -DCMAKE_BUILD_TYPE=Release -DMSDFGEN_INSTALL=ON

set NCORE=%NUMBER_OF_PROCESSORS%
set DLL_EXT=dll

echo Building project...
cmake --build "%BIN%" --config Release -j %NCORE%
copy /y "%BIN%\msdfgen-config.h" .
copy /y "%BIN%\Release\*.%DLL_EXT%" ..
popd

set VCPKG_LIB=%SRC%\%BIN%\vcpkg_installed\x64-windows-static\

REM Static dependencies for linking
set STATIC_DEPS=brotlicommon.lib brotlidec.lib bz2.lib freetype.lib libpng16.lib tinyxml2.lib zlib.lib skia.lib

echo Building shared libs...
cl /LD /EHsc /O2 /I. /I"%SRC%" msdfgen-c\msdfgen-core.cpp "%SRC%\%BIN%\Release\msdfgen-core.lib" /Fe:core.%DLL_EXT% /link /IMPLIB:core_shared.lib /DEF:msdfgen-c\msdfgen-core.def
del msdfgen-core.obj msdfgen-core.exp

REM Build ext DLL with all dependencies
set EXT_LIBS="%SRC%\%BIN%\Release\msdfgen-core.lib" "%SRC%\%BIN%\Release\msdfgen-ext.lib"
for %%f in (%STATIC_DEPS%) do (
    set EXT_LIBS=!EXT_LIBS! "%VCPKG_LIB%\lib\%%f"
)
cl /LD /EHsc /O2 /I. /I"%SRC%" msdfgen-c\msdfgen-ext.cpp !EXT_LIBS! /Fe:ext.%DLL_EXT% /link /IMPLIB:ext_shared.lib /DEF:msdfgen-c\msdfgen-ext.def
del msdfgen-ext.obj msdfgen-ext.exp

echo Building static libs...
cl /c /EHsc /O2 /I. /I"%SRC%" msdfgen-c\msdfgen-core.cpp /Fo:core.obj
lib /OUT:core.lib "%SRC%\%BIN%\Release\msdfgen-core.lib" core.obj
del core.obj

set STATIC_DEPS=brotlicommon.lib brotlidec.lib bz2.lib libpng16.lib tinyxml2.lib zlib.lib skia.lib

REM Build ext static lib with all dependencies
set EXT_STATIC_LIBS="%SRC%\%BIN%\Release\msdfgen-ext.lib"
for %%f in (%STATIC_DEPS%) do (
    set EXT_STATIC_LIBS=!EXT_STATIC_LIBS! "%VCPKG_LIB%\lib\%%f"
)
cl /c /EHsc /O2 /I. /I"%SRC%" msdfgen-c\msdfgen-ext.cpp /Fo:ext.obj
lib /OUT:ext.lib !EXT_STATIC_LIBS! ext.obj
del ext.obj

echo Build completed successfully!
