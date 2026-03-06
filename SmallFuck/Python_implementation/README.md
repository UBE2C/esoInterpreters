# SmallFuck Interpreter — Python

A Python implementation of the [SmallFuck](https://www.codewars.com/kata/58678d29dbca9a68d80000d3) interpreter, based on the CodeWars kata of the same name.

## What is SmallFuck?

SmallFuck is a minimal esoteric language operating on a finite binary tape (cells containing only `0` or `1`) with five commands:

| Command | Description |
|---------|-------------|
| `>`     | Move the tape pointer one cell to the right |
| `<`     | Move the tape pointer one cell to the left |
| `*`     | Flip the bit at the current tape cell (0 → 1, 1 → 0) |
| `[`     | Jump past the matching `]` if the current cell is `0` |
| `]`     | Jump back to the matching `[` if the current cell is `1` |

All other characters are ignored. Tape overflow and underflow halt execution.

## Requirements

- Python 3.10+

No external dependencies — only the standard library is used.

## Usage

The interpreter accepts code and tape either as inline strings (via flags) or as files (by path with the appropriate extension).

```
python3 sfInterpreter.py [options]
```

### Flags

| Flag                      | Description                                                                          |
|---------------------------|--------------------------------------------------------------------------------------|
| `-c`, `--code` `<code>`   | Provide SmallFuck code as an inline string                                           |
| `-t`, `--tape` `<tape>`   | Provide the tape as an inline binary string                                          |
| `-o`, `--output` `<file>` | Write the output tape to a file (`.txt` extension added automatically if not provided) |

### File arguments (no flag needed)

| Extension | Description           |
|-----------|-----------------------|
| `.sf`     | SmallFuck source file |
| `.sft`    | SmallFuck tape file   |

Exactly one code source and one tape source must be provided. Providing both a file and an inline string for the same input is an error.

### Examples

**Inline code and tape:**
```bash
python3 sfInterpreter.py -c "*>*>*" -t "00000"
```

**From files:**
```bash
python3 sfInterpreter.py program.sf tape.sft
```

**Mixed (file for code, inline for tape):**
```bash
python3 sfInterpreter.py program.sf -t "00000"
```

**With output to file:**
```bash
python3 sfInterpreter.py program.sf tape.sft -o result
# writes to result.txt
```

The output tape state is printed to stdout, or written to the specified output file if `-o` is provided.

## Project Structure

```
Python_implementation/
├── sfInterpreter.py   # Interpreter source
└── README.md
```

## Implementation Notes

- Loop jump points are pre-computed before execution using a stack and a dictionary, mapping each `[` to its matching `]` and vice versa, for O(1) jumps at runtime.
- The tape is represented internally as a `list[int]` of `0`s and `1`s, translated to/from a binary string for I/O.
- Tape overflow and underflow silently halt execution and print the current tape state.
- If `-o` is provided without a file extension, `.txt` is appended automatically.
