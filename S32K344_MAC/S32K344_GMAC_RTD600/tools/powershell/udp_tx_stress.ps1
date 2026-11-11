# UDP TX-Only Stress Test (saturate link, no receive)
# 板子侧用 g_echo_stats.udp_rx_bytes 看实际收到多少
# Usage: powershell -ExecutionPolicy Bypass -File .\udp_tx_stress.ps1

$targetIP   = "192.168.2.10"
$targetPort = 9999
$payloadLen = 1400
$durationSec = 10

$udp = New-Object System.Net.Sockets.UdpClient
$udp.Client.ReceiveTimeout = 1000
$udp.Client.Blocking = $false   # 非阻塞，纯发不等回显
$remote = [System.Net.IPEndPoint]::new([System.Net.IPAddress]::Parse($targetIP), $targetPort)

$payload = New-Object byte[] $payloadLen
for ($i = 0; $i -lt $payloadLen; $i++) { $payload[$i] = [byte]($i % 256) }

$sendCount = 0
$sendBytes = 0
$errCount  = 0
$start     = Get-Date

Write-Host "============================================"
Write-Host "  UDP TX-Only Stress Test"
Write-Host "============================================"
Write-Host ("Target     : {0}:{1}" -f $targetIP, $targetPort)
Write-Host ("Payload    : {0} bytes" -f $payloadLen)
Write-Host ("Duration   : {0} sec" -f $durationSec)
Write-Host "Board side : watch g_echo_stats.udp_rx_bytes"
Write-Host ""

try {
    while (((Get-Date) - $start).TotalSeconds -lt $durationSec) {
        try {
            $udp.Send($payload, $payload.Length, $remote) | Out-Null
            $sendCount++
            $sendBytes += $payloadLen
        } catch {
            $errCount++
        }
    }
} finally {
    $udp.Close()
}

$elapsed = ((Get-Date) - $start).TotalSeconds
if ($elapsed -lt 0.001) { $elapsed = 0.001 }

Write-Host ""
Write-Host ("Sent {0} pkts ({1} bytes) in {2:N2}s" -f $sendCount, $sendBytes, $elapsed)
Write-Host ("TX Rate : {0:N2} Mbps (PC sending)" -f ($sendBytes * 8.0 / $elapsed / 1MB))
Write-Host ("Errors  : {0}" -f $errCount)
Write-Host ""
Write-Host "Now check g_echo_stats.udp_rx_bytes on board to calculate real RX throughput."