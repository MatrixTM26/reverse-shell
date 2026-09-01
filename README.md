## reverse-shell

a list of simple linux and windows reverse shell script writen in low-mid level language.

---

### installation & usage

```bash
git clone https://github.com/MatrixTM26/reverse-shell.git
cd reverse-shell
```

---

### compile

examples:

- C

```bash
gcc -o shellc C-shell/shell-1.c
```

- C (using hex binary version)

```bash
gcc -z execstack -o shellc C-shell/shell-3.c
```

- C++

```bash
g++ -o shellcpp CPP-shell/shell-1.cpp
```

- ASM

```bash
nasm -f elf64 -o shellasm.o ASM-shell/shell.asm
gcc shellasm.o -o shellasm -no-pie -lc
```

- JAVA

```bash
javac JAVA-shell/shell.java
```

---

<p align="center">
    &copy;
    Copyright 2023-2026 
    <a href="https://github.com/matrixtm26">@MatrixTM26</a>
    &nbsp;
    &middot;
    &nbsp;
    All right reserved.
    <br>
    Licensed under
    &nbsp;
    <a href="./LICENSE">AGPL-V3</a>
</p>
