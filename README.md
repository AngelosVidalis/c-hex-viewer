# C Hex Viewer

A lightweight command-line hex viewer and experimental byte editor written in C. Inspect a file's raw bytes as hexadecimal and ASCII, highlight specific values, and make basic byte edits.

## Features

- **Hex + ASCII view:** Displays bytes in uppercase hexadecimal alongside printable characters. Non-printable characters appear as `.`.
- **File offsets:** Shows the hexadecimal offset at the start of each row.
- **Custom row width:** Choose between 1 and 64 bytes per line.
- **Byte highlighting:** Highlight occurrences of one byte value in red in both the hex and ASCII views.
- **Individual byte addresses:** Use `-r` to display the offset of each byte.
- **Interactive byte editor:** Substitute or append bytes; byte deletion is experimental.
- **Binary file support:** Read files of any extension.

## Build

Compile `hex.c` with GCC:

```bash
gcc -std=gnu11 -Wall -Wextra hex.c -o hex -lm
```

On Windows with a compatible GCC toolchain, use `-o hex.exe` instead. The deletion code uses POSIX functions such as `ftruncate()`.

## Usage

```bash
./hex <filename> <bytes_per_line> [0xHH] [-r]
```

| Argument | Description |
| --- | --- |
| `filename` | File to inspect. |
| `bytes_per_line` | Number of bytes per row (1–64). |
| `0xHH` | Optional byte value to highlight, such as `0xFF`. |
| `-r` | Show the address of every byte. |

**Note:** In the current argument parser, `-r` must come after a highlighting argument (`0xHH`).

### Examples

```bash
./hex sample.bin 16
./hex picture.jpg 16 0xFF
./hex picture.jpg 16 0xFF -r
```

Example output:

```text
00000000    48 65 6C 6C 6F           | Hello
```

Each row contains a starting offset, hexadecimal byte values, and an ASCII representation. The program also displays the total number of bytes read.

## Byte editor

After the file is displayed, the interactive editor accepts commands in this format:

```text
-f <OFFSET> -<OP> <HEX>
```

| Operation | Example | Behavior |
| --- | --- | --- |
| `-s` Substitute | `-f 0010 -s FF` | Overwrite one byte at offset `0x10` with `FF`. |
| `-a` Append | `-f 0000 -a 11` | Append `11` to the end of the file. The offset is currently ignored. |
| `-d` Delete | `-f 0010 -d` | Attempt to remove one byte at offset `0x10` (experimental). |

After an edit, the program prints the affected portion of the file and returns to the command prompt.

**Warning:** Editing changes the original file. Work on a copy. The editor is still under development, especially byte deletion; it should not be relied on for important files.

## Implementation

The project uses standard C file I/O (`fopen`, `fread`, `fwrite`, `fseek`, `ftell`) and formatted output (`printf`), with `strtol` for numeric input and `isprint` for ASCII display. The deletion code also uses POSIX `ftruncate`.

Built as a practical C learning project covering pointers, arrays, binary I/O, file offsets, command-line parsing, and byte manipulation.
