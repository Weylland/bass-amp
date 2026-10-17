@echo off
setlocal

set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Release

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1
set PATH=%APPDATA%\Python\Python314\Scripts;%PATH%

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=%CONFIG% || exit /b 1
cmake --build build || exit /b 1

echo.
echo Standalone: build\BassAmp_artefacts\%CONFIG%\Standalone\Bass Amp.exe
echo VST3:       build\BassAmp_artefacts\%CONFIG%\VST3\Bass Amp.vst3
