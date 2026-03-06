# miniStringFuck Interpreter — Python

A Python implementation of the [miniStringFuck](https://www.codewars.com/kata/586dd26a69b6fd46dd0000c0) interpreter, based on the CodeWars kata of the same name.

## What is miniStringFuck?

miniStringFuck is a minimal esoteric language with only two commands operating on a single 8-bit memory cell (initially `0`):

| Command | Description |
|---------|-------------|
| `+`     | Increment the memory cell by 1 (wraps from 255 back to 0) |
| `.`     | Append the character at the current memory cell value to the output |

All other characters are ignored.

## Requirements

- Python 3.10+

No external dependencies — only the standard library is used.

## Usage

```bash
python3 msf_interpreter.py "<code>"
```

The interpreted output is printed to stdout.

### Example

To output the character `A` (ASCII 65):

```bash
python3 msf_interpreter.py "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
```

Output:
```
A
```

## Project Structure

```
Python_implementation/
├── msf_interpreter.py   # Interpreter source
└── README.md
```
