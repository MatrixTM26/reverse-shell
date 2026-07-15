## reverse-shell
a list of simple linux and windows reverse shell script.

### installation & usage

```bash
git clone https://github.com/MatrixTM26/reverse-shell.git
cd reverse-shell
```

### compile

examples:

- C

```bash
gcc -o shellc C-shell/shell-2.c
```

- C++

```bash
g++ -o shellcpp CPP-shell/shell-2.c
```

- ASM

```bash
nasm -f elf64 -o shellasm.o ASM-shell/shell.asm
gcc shellasm.o -o shellasm -no-pie -lc
```
