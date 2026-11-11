# Quick Diagnostic: ARP + ICMP + single UDP probe
# Run this FIRST before any stress test

$targetIP   = "192.168.2.10"
$targetPort = 9999

Write-Host "============================================"
Write-Host "  Quick Diagnostic"
Write-Host "============================================"

# Step 1: ARP resolution
Write-Host "`n[1] ARP: resolving $targetIP ..."
try {
    $arp = arp -a | Select-String $targetIP
    if ($arp) {
        Write-Host "  ARP cache hit: $arp" -ForegroundColor Green
    } else {
        Write-Host "  ARP cache MISS - pinging to trigger ARP..." -ForegroundColor Yellow
        ping -n 1 -w 1000 $targetIP | Out-Null
        $arp = arp -a | Select-String $targetIP
        if ($arp) {
            Write-Host "  After ping: $arp" -ForegroundColor Green
        } else {
            Write-Host "  STILL NO ARP ENTRY! Board not responding to ARP." -ForegroundColor Red
            Write-Host "  Check: link cable, board power, MAC/PHY init." -ForegroundColor Red
            exit 1
        }
    }
} catch {
    Write-Host "  arp command failed" -ForegroundColor Red
}

# Step 2: Ping
Write-Host "`n[2] ICMP ping ..."
ping -n 4 -w 1000 $targetIP

# Step 3: Single UDP probe (with 5s timeout)
Write-Host "`n[3] Single UDP probe to ${targetIP}:${targetPort} ..."
$udp = New-Object System.Net.Sockets.UdpClient
$udp.Client.ReceiveTimeout = 5000
$remote = [System.Net.IPEndPoint]::new([System.Net.IPAddress]::Parse($targetIP), $targetPort)
$anyEndPt = [System.Net.IPEndPoint]::new([System.Net.IPAddress]::Any, 0)

$probe = [byte[]]@(0xDE, 0xAD, 0xBE, 0xEF)
try {
    $udp.Send($probe, $probe.Length, $remote) | Out-Null
    Write-Host "  Sent 4-byte probe. Waiting for echo (5s)..."
    $reply = $udp.Receive([ref]$anyEndPt)
    Write-Host "  GOT ECHO! Length=$($reply.Length) Data=$([BitConverter]::ToString($reply))" -ForegroundColor Green
} catch [System.Net.Sockets.SocketException] {
    Write-Host "  TIMEOUT - no echo received from board" -ForegroundColor Red
    Write-Host "  Board may not be listening on port 9999, or firewall blocking." -ForegroundColor Red
} finally {
    $udp.Close()
}

Write-Host "`n============================================"
Write-Host "  Diagnostic done. Now check board-side:"
Write-Host "    g_echo_stats.udp_rx_packets  (should be >= 1)"
Write-Host "    g_echo_stats.udp_tx_packets  (should be >= 1 if echo works)"
Write-Host "    g_echo_stats.udp_err         (should be 0)"
Write-Host "============================================"