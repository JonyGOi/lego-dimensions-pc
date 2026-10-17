param([string]$Name, [string]$Out)
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Text;
using System.Runtime.InteropServices;
public static class WinP {
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, StringBuilder s, int n);
    [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr h, IntPtr dc, uint flags);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L, T, R, B; }
}
"@
$ids = (Get-Process $Name -ErrorAction SilentlyContinue).Id
$best = [IntPtr]::Zero; $area = 0; $title = ''
[WinP]::EnumWindows({ param($h, $l)
    $procId = 0; [WinP]::GetWindowThreadProcessId($h, [ref]$procId) | Out-Null
    if ($ids -contains [int]$procId -and [WinP]::IsWindowVisible($h)) {
        $r = New-Object WinP+RECT; [WinP]::GetWindowRect($h, [ref]$r) | Out-Null
        $a = ($r.R - $r.L) * ($r.B - $r.T)
        if ($a -gt $script:area) { $script:area = $a; $script:best = $h; $sb = New-Object System.Text.StringBuilder 300; [WinP]::GetWindowText($h, $sb, 300) | Out-Null; $script:title = $sb.ToString() }
    }
    return $true }, [IntPtr]::Zero) | Out-Null
if ($best -eq [IntPtr]::Zero) { "sem janela de $Name"; exit 1 }
$wr = New-Object WinP+RECT
[WinP]::GetWindowRect($best, [ref]$wr) | Out-Null
$w = $wr.R - $wr.L; $hh = $wr.B - $wr.T
$bmp = New-Object System.Drawing.Bitmap $w, $hh
$g = [System.Drawing.Graphics]::FromImage($bmp)
$dc = $g.GetHdc()
$ok = [WinP]::PrintWindow($best, $dc, 2)
$g.ReleaseHdc($dc)
$bmp.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png)
"PrintWindow=$ok ${w}x${hh} titulo: $title"
