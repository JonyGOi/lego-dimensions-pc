@echo off
rem Configura e compila com clang-cl + Ninja do Visual Studio.
setlocal
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -property installationPath`) do set "VSROOT=%%i"
if not defined VSROOT (
    echo ERRO: Visual Studio nao encontrado
    exit /b 1
)
call "%VSROOT%\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1
set "PATH=%VSROOT%\VC\Tools\Llvm\x64\bin;%PATH%"

cmake -S "%~dp0." -B "%~dp0build" -G Ninja ^
    -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl ^
    -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_POLICY_VERSION_MINIMUM=3.5 || exit /b 1
cmake --build "%~dp0build" || exit /b 1
echo.
echo OK: %~dp0build\lego_dimensions.exe
