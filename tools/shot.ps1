param([string]$Out)
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
public static class Win {
    [DllImport("user32.dll", CharSet = CharSet.Ansi)] public static extern IntPtr FindWindowA(string cls, IntPtr title);
    [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr h, IntPtr dc, uint flags);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L, T, R, B; }
}
"@
# pela classe da janela do jogo (MainWindowHandle as vezes devolve uma janela auxiliar do processo)
$h = [Win]::FindWindowA("LegoDimensionsPcWindow", [IntPtr]::Zero)
if ($h -eq [IntPtr]::Zero) { "sem janela"; exit 1 }
$wr = New-Object Win+RECT
[Win]::GetWindowRect($h, [ref]$wr) | Out-Null
$w = $wr.R - $wr.L; $hh = $wr.B - $wr.T
$bmp = New-Object System.Drawing.Bitmap $w, $hh
$g = [System.Drawing.Graphics]::FromImage($bmp)
$dc = $g.GetHdc()
$ok = [Win]::PrintWindow($h, $dc, 2)   # PW_RENDERFULLCONTENT: inclui o conteudo do swapchain
$g.ReleaseHdc($dc)
$bmp.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png)
$c = $bmp.GetPixel([int]($w / 2), [int]($hh / 2))
"PrintWindow=$ok janela ${w}x${hh} pixel central R=$($c.R) G=$($c.G) B=$($c.B)"

