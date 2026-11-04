# Shell

A small POSIX command-line shell written in C to explore process creation, command execution and file descriptors.

## Implemented concepts

- Interactive command prompt and external commands through `fork` and `execvp`.
- A built-in `cd` command and `exit` command.
- Basic pipe and input/output redirection code.

This is a learning implementation, not a replacement for Bash. Quoting, expansion, job control and full shell grammar are not implemented. Pipe and redirection combinations remain experimental.

## Build and run

Use Linux or another POSIX environment with a C compiler and Make. Native Windows is not supported; use a configured Linux environment such as WSL.

```sh
make
./build/shell
```

Try `pwd`, `ls`, `cd /tmp`, and `exit`. Avoid running privileged commands in an experimental shell.

## Source layout

`Shell/main.c` and `Shell/header.h` are the primary implementation. The nested `Shell/Shell/` directory contains an earlier source variant and is preserved for reference; the build uses only the primary version.

Generated executables are excluded from version control. `make clean` removes the local build directory. The automated smoke check exercises blank input, long input, a missing command and end-of-file; it does not establish complete pipeline correctness.
