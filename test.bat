@echo off
setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1
set PATH=%APPDATA%\Python\Python314\Scripts;%PATH%
cd /d "%~dp0"

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release >nul || exit /b 1
cmake --build build --target BassAmpTests || exit /b 1
"build\BassAmpTests_artefacts\Release\BassAmpTests.exe" %*
