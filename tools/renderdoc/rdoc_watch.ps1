# Abre o qrenderdoc com o script de captura, vigia a janela do shadPS4 e cria os arquivos-gatilho
# quando a cena das pecas (tela lilas com faixas pretas em cima e embaixo) aparece.
$sp = Split-Path -Parent $MyInvocation.MyCommand.Path
$out = 'C:\Jogos\lego-dimensions-pc\build\rdoc'
New-Item -ItemType Directory -Force $out | Out-Null
Add-Type -AssemblyName System.Drawing
Start-Process -FilePath 'C:\Program Files\RenderDoc\qrenderdoc.exe' -ArgumentList '--python', "`"$sp\rdoc_capture.py`""
$sw = [Diagnostics.Stopwatch]::StartNew()
$fired = 0; $last = -100; $n = 0
$title = $false
while ($sw.Elapsed.TotalSeconds -lt 392) {
    Start-Sleep -Seconds 3
    $png = "$out\vigia.png"
    $r = & "$sp\shotp.ps1" -Name shadps4 -Out $png 2>$null
    if (-not (Test-Path $png)) { continue }
    try { $bmp = New-Object System.Drawing.Bitmap $png } catch { continue }
    $w = $bmp.Width; $h = $bmp.Height
    $sr = 0; $sg = 0; $sb = 0; $c = 0
    for ($y = [int]($h * 0.3); $y -lt [int]($h * 0.8); $y += 23) { for ($x = [int]($w * 0.1); $x -lt [int]($w * 0.9); $x += 31) { $p = $bmp.GetPixel($x, $y); $sr += $p.R; $sg += $p.G; $sb += $p.B; $c++ } }
    $top = $bmp.GetPixel([int]($w / 2), 60)
    $bmp.Dispose()
    $mr = [int]($sr / $c); $mg = [int]($sg / $c); $mb = [int]($sb / $c)
    $lilac = ($mr -gt 110 -and $mb -gt 135 -and $mg -gt 80 -and $mg -lt 195 -and $mb -gt $mg + 18 -and $mr -gt $mg + 6 -and ($top.R + $top.G + $top.B) -lt 60)
    "t={0:N0} media {1},{2},{3} topo {4} lilas={5}" -f $sw.Elapsed.TotalSeconds, $mr, $mg, $mb, ($top.R + $top.G + $top.B), $lilac
    if (-not $title -and $sw.Elapsed.TotalSeconds -gt 372) { $title = $true; Copy-Item $png "$out\cena_titulo.png" -Force; New-Item -ItemType File "$out\gatilho_titulo" -Force | Out-Null; "GATILHO titulo" }
    if ($lilac -and $fired -lt 3 -and ($sw.Elapsed.TotalSeconds - $last) -gt 9) {
        $fired++; $last = $sw.Elapsed.TotalSeconds
        Copy-Item $png "$out\cena_$fired.png" -Force
        New-Item -ItemType File "$out\gatilho_$fired" -Force | Out-Null
        "GATILHO $fired"
    }
}
Start-Sleep -Seconds 25   # a ultima captura precisa terminar de ser gravada
New-Item -ItemType File "$out\fim" -Force | Out-Null
Start-Sleep -Seconds 4
Get-Process shadps4, qrenderdoc -ErrorAction SilentlyContinue | Stop-Process -Force -Confirm:$false
Get-Content "$out\rdoc_capture.log"
Get-ChildItem "$env:APPDATA\shadPS4\captures" -Filter *.rdc | Sort-Object LastWriteTime -Descending | Select-Object -First 5 Name, Length, LastWriteTime

