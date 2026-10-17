param([string]$Name, [int]$Timeout = 168, [string]$Extra = '')
# Roteiro de teste: jogo novo numa COPIA dos saves ate a cutscene de Gotham, sem o controle e sem o app do Toy Pad.
if (Get-Process lego_dimensions -ErrorAction SilentlyContinue) { "jogo aberto"; exit 1 }
$sp = Split-Path -Parent $MyInvocation.MyCommand.Path
$root = 'C:\Jogos\lego-dimensions-pc'
$pt = "$root\build\portal_teste.json"
Set-Content -Path $pt -Value '{}' -Encoding ascii
$env:LD_SAVE_UI_AUTO = 'Slot00'; $env:LD_MACRO_NEWGAME = '1'
$p = Start-Process cmd -ArgumentList '/c', "cd /d $root && build\lego_dimensions.exe --no-keyboard --no-sdl-pad --toypad-port 0 --portal build\portal_teste.json --saves-dir savedata_teste32 --load-game-macro --input-script 72:cross,78:cross,84:cross,90:cross --fps60 --hitch-log $Extra --timeout $Timeout > build\$Name.log 2>&1" -PassThru -WindowStyle Minimized
$env:LD_SAVE_UI_AUTO = $null; $env:LD_MACRO_NEWGAME = $null
Start-Sleep 96
Set-Content -Path $pt -Value '{"pad2": {"uid": [4,163,55,18,34,55,66], "id": 1}}' -Encoding ascii
$last = 96; $names = @()
for ($t = 108; $t -le 156; $t += 6) { Start-Sleep ($t - $last); $last = $t; & "$sp\shot.ps1" -Out "$root\build\${Name}_$t.png" | Out-Null; $names += $t }
$p.WaitForExit(90000) | Out-Null
$list = ($names -join ',')
cmd /c "cd /d $root\build && python -c ""from PIL import Image;n=[$list];m=Image.new('RGB',(400*3,225*3));[m.paste(Image.open('${Name}_%d.png'%x).convert('RGB').resize((400,225)),((i%3)*400,(i//3)*225)) for i,x in enumerate(n)];m.save('${Name}_montagem.png')"""
Select-String -Path "$root\build\$Name.log" -Pattern "Fase 28 \(eng|Fase 36|AQUECIMENTO|enviados \d+ \| fora|CRASH DUMP|par\(es\) fora" | ForEach-Object { $_.Line.Substring(0, [Math]::Min(360, $_.Line.Length)) }
"media fps: " + [Math]::Round((Select-String -Path "$root\build\$Name.log" -Pattern "^\[PERF\] (\d+\.\d) quadros" | ForEach-Object { [double]$_.Matches[0].Groups[1].Value } | Measure-Object -Average).Average, 1)
