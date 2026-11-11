# UDP Echo Performance Test (with full tuning)
# Target: S32K344 board at 192.168.2.10 : UDP 9999
# Usage: powershell -ExecutionPolicy Bypass -File .\udp_echo_test.ps1

$targetIP    = "192.168.2.10"
$targetPort  = 9999
$payloadLen  = 1400
$durationSec = 10

# --- Step 1: 确保 PC 有板子的 ARP ---
Write-Host "[1] Making sure PC has board's ARP ..."
ping -n 2 -w 500 $targetIP | Out-Null
arp -a | Select-String $targetIP | Out-Host

$udp      = New-Object System.Net.Sockets.UdpClient
$udp.Client.ReceiveTimeout = 5000
$udp.Client.Blocking = $true
$udp.Client.ReceiveBufferSize = 1048576   # 1MB buffer - critical!
$udp.Client.SendBufferSize    = 1048576   # 1MB buffer
$remote   = [System.Net.IPEndPoint]::new([System.Net.IPAddress]::Parse($targetIP), $targetPort)
$anyEndPt = [System.Net.IPEndPoint]::new([System.Net.IPAddress]::Any, 0)

$payload = New-Object byte[] $payloadLen
for ($i = 0; $i -lt $payloadLen; $i++) { $payload[$i] = [byte]($i % 256) }

# --- Step 2: UDP probe，确认双向通 ---
Write-Host "[2] Probing bidirectional path (up to 5 attempts) ..."
$probeOk = $false
for ($attempt = 1; $attempt -le 5; $attempt++) {
    Write-Host "  Attempt $attempt ..."
    try {
        $udp.Send($payload, $payload.Length, $remote) | Out-Null
        $reply = $udp.Receive([ref]$anyEndPt)
        if ($reply.Length -eq $payloadLen) {
            Write-Host "  Probe OK! (board ARP ready, path good)" -ForegroundColor Green
            $probeOk = $true
            break
        } else {
            Write-Host "  Size mismatch: sent=$payloadLen recv=$($reply.Length)" -ForegroundColor Yellow
        }
    } catch [System.Net.Sockets.SocketException] {
        Write-Host "  Timeout. Retrying..." -ForegroundColor Yellow
        Start-Sleep -Milliseconds 300
    }
}

if (-not $probeOk) {
    Write-Host ""
    Write-Host "FATAL: No echo received." -ForegroundColor Red
    Write-Host "Try: netsh advfirewall firewall add rule name='Allow UDP 9999' dir=in action=allow protocol=UDP localport=9999"
    exit 1
}

# --- Step 3: 正式计时开始 ---
Write-Host ""
Write-Host "============================================"
Write-Host "  UDP Echo Performance Test"
Write-Host "============================================"
Write-Host ("Target     : {0}:{1}" -f $targetIP, $targetPort)
Write-Host ("Payload    : {0} bytes per packet" -f $payloadLen)
Write-Host ("Duration   : {0} sec (or Ctrl+C to stop)" -f $durationSec)
Write-Host ("Socket buf : 1 MB (RX/TX each)")
Write-Host ""

$sendCount = 0
$recvCount = 0
$sendBytes = 0
$recvBytes = 0
$errCount  = 0
$start     = Get-Date

try {
    while (((Get-Date) - $start).TotalSeconds -lt $durationSec) {
        try {
            $udp.Send($payload, $payload.Length, $remote) | Out-Null
            $sendCount++
            $sendBytes += $payloadLen

            $data = $udp.Receive([ref]$anyEndPt)
            if ($data.Length -eq $payloadLen) {
                $recvCount++
                $recvBytes += $data.Length
            } else {
                $errCount++
            }
        } catch [System.Net.Sockets.SocketException] {
            $errCount++
            $elapsed = [math]::Round(((Get-Date) - $start).TotalSeconds, 1)
            Write-Host ("  TIMEOUT at t={0}s" -f $elapsed) -ForegroundColor Red
            break
        }
    }
} finally {
    $udp.Close()
}

$elapsed = ((Get-Date) - $start).TotalSeconds
if ($elapsed -lt 0.001) { $elapsed = 0.001 }

Write-Host ""
Write-Host "============================================"
Write-Host "  Result"
Write-Host "============================================"
Write-Host ("Elapsed    : {0:N2} s"      -f $elapsed)
Write-Host ("Sent       : {0,10} pkts, {1,12} bytes" -f $sendCount, $sendBytes)
Write-Host ("Echo Recv  : {0,10} pkts, {1,12} bytes" -f $recvCount, $recvBytes)
Write-Host ("Lost       : {0,10} pkts"   -f ($sendCount - $recvCount))
Write-Host ("Errors     : {0,10}"        -f $errCount)
Write-Host ("Loss Rate  : {0:N2} %"      -f $(if ($sendCount -gt 0) { ($sendCount - $recvCount) * 100.0 / $sendCount } else { 0 }))
Write-Host ""
Write-Host ("TX Rate    : {0:N2} Mbps"   -f ($sendBytes * 8.0 / $elapsed / 1MB))
Write-Host ("Echo Rate  : {0:N2} Mbps"   -f ($recvBytes * 8.0 / $elapsed / 1MB))
Write-Host "============================================"