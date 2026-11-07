# C Hex Viewer

A lightweight command-line hex viewer written in C. Inspect a file's raw bytes in hexadecimal alongside their printable ASCII representation, with configurable line widths and optional highlighting of a specific byte value.

## Features

- **Hex + ASCII view:** Display each byte as two uppercase hexadecimal digits and show printable ASCII characters alongside it. Non-printable bytes appear as `.`.
- **Hexadecimal offsets:** Show the starting byte offset for each row.
- **Custom row width:** Choose from **1 to 64 bytes per line**.
- **Optional byte highlighting:** Highlight every occurrence of a chosen byte (for example, `0xFF`) in **red** in the hexadecimal column.
- **Binary-safe file reading:** Open files in `rb` mode and read them using `fread()`; files do not need a `.bin` extension.
- **Aligned columns:** Pad the final, incomplete row to keep the ASCII column aligned.

## Requirements

- A C compiler such as **GCC** or **Clang**.
- A terminal that supports ANSI escape sequences to see colored highlights. The hex viewer can still print byte values without color support, but terminals that do not interpret ANSI sequences may display escape characters.

> **Source code note:** The version shown here uses `bool`, so add `#include <stdbool.h>` alongside the other headers when compiling as C11.

## Build

From the repository root:

```bash
gcc -std=c11 -Wall -Wextra -o hexv hexv.c
```

On Windows using MinGW-w64, you can build `hexv.exe` instead:

```powershell
gcc -std=c11 -Wall -Wextra -o hexv.exe hexv.c
```

## Usage

```text
./hexv <filename> <bytes_per_line> [marked_byte]
```

| Argument | Required | Description |
| --- | --- | --- |
| `filename` | Yes | Path to the file to inspect (any file extension). |
| `bytes_per_line` | Yes | Number of bytes to display per row, from `1` to `64`. |
| `marked_byte` | No | Byte to highlight in the hex column, formatted as `0xHH` (two hexadecimal digits), e.g. `0xFF` or `0x00`. |

### Examples

Display 16 bytes per line:

```bash
./hexv sample_text.bin 16
```

Display 8 bytes per line:

```bash
./hexv sample_binary.bin 8
```

Highlight every `FF` byte in red:

```bash
./hexv sample_binary.bin 16 0xFF
```

Highlight all zero bytes:

```bash
./hexv sample_binary.bin 16 0x00
```

Inspect a file with a different extension:

```bash
./hexv picture.jpg 16 0xFF
```

On Windows (PowerShell), use `./hexv.exe` or `.\hexv.exe` in place of `./hexv`.

## Output format

Each row has three parts:

| Column | Description |
| --- | --- |
| Offset | Hexadecimal file offset of the first byte in that row, starting at `00000000`. |
| Hex | Byte values as uppercase, two-digit hexadecimal numbers separated by spaces. |
| ASCII | Printable characters as text; non-printable bytes displayed as `.`. |

For example, a file containing the text `Hello` produces this row when viewed with 8 bytes per line:

```text
00000000    48 65 6C 6C 6F           | Hello
```

The exact spacing depends on the chosen row width. The current program does **not** print a column-header line.

### Byte highlighting

With the optional third argument, **every matching byte in the hex column** is colored red using ANSI escape sequences (`\x1b[31m` for red and `\x1b[0m` to reset). The ASCII column is not highlighted.

For example:

```bash
./hexv picture.jpg 16 0xFF
```

This makes repeated byte values easier to locate when inspecting binary data. Only one byte value can be highlighted at a time.

## Implementation notes

This project uses `fopen()` / `fread()` / `fclose()` for binary file I/O, `strtol()` to parse numeric command-line arguments, `isprint()` to decide which bytes can be shown as characters, and `printf()` for aligned hexadecimal and ASCII output.

It was built as a C learning project focused on command-line argument handling, validation, arrays, binary I/O, and formatted output.
