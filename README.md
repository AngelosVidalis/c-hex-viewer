# C Hex Viewer

A lightweight command-line hex viewer written in C. It reads a file as raw bytes and displays each byte in hexadecimal alongside a printable ASCII representation.

## Features

- Read files in binary mode using `fread()`.
- Display hexadecimal offsets and byte values.
- Display printable ASCII characters; replace non-printable characters with `.`.
- Choose between 1 and 64 bytes per line.
- Align the ASCII column when the last line is incomplete.

## Requirements

- A C compiler such as GCC or Clang.

## Build

```bash
gcc -std=c11 -Wall -Wextra -o hexv hexv.c
```

## Usage

```bash
./hexv <filename> <bytes_per_line>
```

For example:

```bash
./hexv samples/sample_text.bin 16
./hexv samples/sample_binary.bin 8
```

The second argument must be an integer from **1 to 64**. The current implementation also requires the filename to contain `.bin`.

## Output

Each row shows:

- **LOCATION:** Offset of the row's first byte in hexadecimal.
- **HEX:** Each byte printed as two hexadecimal digits.
- **ASCII:** Printable characters, or `.` for non-printable bytes.

For a file beginning with `Hello`, a row starts like this:

```text
LOCATION                HEX                       ASCII
00000000    48 65 6C 6C 6F  | Hello
```

(The spaces depend on the selected bytes-per-line value.)

## Sample files

Place these files in the `samples/` folder:

- `sample_text.bin` — text plus control characters and other non-printable bytes.
- `sample_binary.bin` — binary patterns including high-value bytes.

## Notes

This is a learning project to practice C pointers, command-line arguments, binary file I/O, input validation with `strtol()`, and formatted output.
