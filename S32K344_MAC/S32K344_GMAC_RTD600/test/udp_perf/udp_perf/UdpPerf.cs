using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

class UdpPerf
{
    static async Task Main(string[] args)
    {
        var targetIP   = "192.168.2.10";
        var targetPort = 9999;
        var payloadLen = 1400;
        var durationSec = 10;
        var inflight   = 64;

        Console.WriteLine("=== High-Perf UDP Echo Test ===");
        Console.WriteLine($"Target    : {targetIP}:{targetPort}");
        Console.WriteLine($"Payload   : {payloadLen} bytes");
        Console.WriteLine($"Inflight  : {inflight} parallel");
        Console.WriteLine($"Duration  : {durationSec} sec");
        Console.WriteLine();

        // warmup ARP
        Console.WriteLine("Warmup ...");
        using (var warm = new UdpClient())
        {
            warm.Client.ReceiveTimeout = 2000;
            var tmp = new byte[32];
            for (int i = 0; i < 3; i++)
            {
                warm.Send(tmp, tmp.Length, targetIP, targetPort);
                try { warm.Receive(ref var _ = null); } catch { }
            }
        }
        Console.WriteLine();

        var udp = new UdpClient();
        udp.Client.Bind(new IPEndPoint(IPAddress.Any, 0));
        udp.Client.ReceiveTimeout = 5000;

        var remote = new IPEndPoint(IPAddress.Parse(targetIP), targetPort);

        var payload = new byte[payloadLen];
        var rnd = new Random(42);
        rnd.NextBytes(payload);

        long sentCount = 0, sentBytes = 0;
        long recvCount = 0, recvBytes = 0;
        long lostCount = 0;
        var cts = new CancellationTokenSource();

        // Receiver task
        var recvTask = Task.Run(async () =>
        {
            try
            {
                while (!cts.IsCancellationRequested)
                {
                    var result = await udp.ReceiveAsync(cts.Token);
                    Interlocked.Increment(ref recvCount);
                    Interlocked.Add(ref recvBytes, result.Buffer.Length);
                }
            }
            catch (OperationCanceledException) { }
            catch (Exception ex) { Console.WriteLine($"Recv error: {ex.Message}"); }
        });

        // Sender: keep inflight packets outstanding
        var senderTask = Task.Run(async () =>
        {
            var sem = new SemaphoreSlim(inflight);
            try
            {
                while (!cts.IsCancellationRequested)
                {
                    await sem.WaitAsync(cts.Token);
                    _ = udp.SendAsync(payload, payload.Length, remote)
                        .ContinueWith(t =>
                        {
                            if (t.IsFaulted) Interlocked.Increment(ref lostCount);
                            sem.Release();
                        });
                    Interlocked.Increment(ref sentCount);
                    Interlocked.Add(ref sentBytes, payloadLen);
                }
            }
            catch (OperationCanceledException) { }
        });

        var sw = System.Diagnostics.Stopwatch.StartNew();
        Console.WriteLine("Running ... (Ctrl+C to stop early)");

        try
        {
            await Task.Delay(TimeSpan.FromSeconds(durationSec));
        }
        catch { }

        cts.Cancel();
        await Task.WhenAll(senderTask, recvTask);
        sw.Stop();

        var elapsed = sw.Elapsed.TotalSeconds;
        if (elapsed < 0.001) elapsed = 0.001;

        Console.WriteLine();
        Console.WriteLine("============================================");
        Console.WriteLine("  Result");
        Console.WriteLine("============================================");
        Console.WriteLine($"Elapsed    : {elapsed:N2} s");
        Console.WriteLine($"Sent       : {sentCount,12} pkts, {sentBytes,14} bytes");
        Console.WriteLine($"Echo Recv  : {recvCount,12} pkts, {recvBytes,14} bytes");
        Console.WriteLine($"Lost       : {sentCount - recvCount,12} pkts");
        Console.WriteLine($"Loss Rate  : {(sentCount > 0 ? (sentCount - recvCount) * 100.0 / sentCount : 0):N2} %");
        Console.WriteLine();
        Console.WriteLine($"TX Rate    : {sentBytes * 8.0 / elapsed / 1_000_000:N2} Mbps");
        Console.WriteLine($"Echo Rate  : {recvBytes * 8.0 / elapsed / 1_000_000:N2} Mbps");
        Console.WriteLine("============================================");

        udp.Close();
    }
}