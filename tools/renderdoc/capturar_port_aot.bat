@echo off
rem Fase 48: abre o PORT pelo RenderDoc, COM o pacote AOT pre-carregado (igual ao modo normal), para capturar um quadro com F12 (ou Print Screen).
rem As capturas vao para build\rdoc_user\ e o log do jogo para build\run_rdoc_aot.log.
rem A abertura demora mais que o normal: o RenderDoc acompanha os 95 mil modulos de shader.
setlocal
set "RDOC=C:\Program Files\RenderDoc\renderdoccmd.exe"
set "ROOT=C:\Jogos\lego-dimensions-pc"
if not exist "%RDOC%" (
    echo ERRO: RenderDoc nao encontrado em %RDOC%
    exit /b 1
)
if not exist "%ROOT%\build\rdoc_user" mkdir "%ROOT%\build\rdoc_user"
cd /d "%ROOT%"
echo Abrindo o jogo pelo RenderDoc. No momento do problema aperte F12 (pode apertar mais de uma vez).
echo Depois feche o jogo normalmente.
"%RDOC%" capture -w -d "%ROOT%" -c "%ROOT%\build\rdoc_user\port_aot" "%ROOT%\build\lego_dimensions.exe" --fps60 --hitch-log --target-log %* > "%ROOT%\build\run_rdoc_aot.log" 2>&1
echo.
echo Capturas:
dir /b "%ROOT%\build\rdoc_user\*.rdc"
