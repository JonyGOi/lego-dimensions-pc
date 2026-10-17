@echo off
rem Fase 50: compila o port numa pasta separada (build_dist) para a distribuicao, sem mexer no build\
rem (que pode estar em uso pelo jogo aberto). Mesma configuracao do build.bat.
setlocal
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -property installationPath`) do set "VSROOT=%%i"
if not defined VSROOT (
    echo ERRO: Visual Studio nao encontrado
    exit /b 1
)
call "%VSROOT%\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1
set "PATH=%VSROOT%\VC\Tools\Llvm\x64\bin;%PATH%"
set "ROOT=%~dp0..\.."
cmake -S "%ROOT%" -B "%ROOT%\build_dist" -G Ninja ^
    -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl ^
    -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_POLICY_VERSION_MINIMUM=3.5 || exit /b 1
cmake --build "%ROOT%\build_dist" --target lego_dimensions || exit /b 1
echo OK: %ROOT%\build_dist\lego_dimensions.exe
