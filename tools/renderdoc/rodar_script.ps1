param([string]$Script, [string]$Done, [int]$Timeout = 600)
# Roda um script rdoc_*.py dentro do qrenderdoc (--python) e fecha o qrenderdoc quando o arquivo $Done
# aparecer com "script terminou" (ou no tempo limite). As variaveis RDOC_* ja devem estar no ambiente.
$sp = Split-Path -Parent $MyInvocation.MyCommand.Path
if (Test-Path $Done) { Remove-Item $Done -Force }
$p = Start-Process -FilePath 'C:\Program Files\RenderDoc\qrenderdoc.exe' -ArgumentList '--python', "`"$sp\$Script`"" -PassThru -WindowStyle Minimized
$sw = [Diagnostics.Stopwatch]::StartNew()
while ($sw.Elapsed.TotalSeconds -lt $Timeout) {
    Start-Sleep -Seconds 2
    if ((Test-Path $Done) -and ((Get-Content $Done -Raw -ErrorAction SilentlyContinue) -match 'script terminou')) { break }
    if ($p.HasExited) { break }
}
if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force }
"tempo: {0:N0} s" -f $sw.Elapsed.TotalSeconds
