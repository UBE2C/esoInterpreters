# SmallFuck Interpreter — C

A C implementation of the [SmallFuck](https://www.codewars.com/kata/58678d29dbca9a68d80000d3) interpreter, based on the CodeWars kata of the same name.

## What is SmallFuck?

SmallFuck is a minimal esoteric language operating on a finite binary tape (cells containing only `0` or `1`) with five commands:

| Command | Description |
|---------|-------------|
| `>`     | Move the tape pointer one cell to the right |
| `<`     | Move the tape pointer one cell to the left |
| `*`     | Flip the bit at the current tape cell (0 → 1, 1 → 0) |
| `[`     | Jump past the matching `]` if the current cell is `0` |
| `]`     | Jump back to the matching `[` if the current cell is `1` |

All other characters are ignored. Tape overflow and underflow are treated as errors.

## Requirements

- CMake >= 3.15
- A C11-compliant compiler (GCC, Clang, or MSVC)

## Building

```bash
git clone https://github.com/UBE2C/Interpreters.git
cd Interpreters/SmallFuck/C_implementation
mkdir build && cd build
cmake ..
cmake --build .
```

The compiled binary `sfInterpreter` (or `sfInterpreter.exe` on Windows) will be placed in the `build/` directory.

## Usage

The interpreter accepts code and tape either as inline strings (via flags) or as files (by path with the appropriate extension).

```
sfInterpreter [options]
```

### Flags

| Flag                  | Description                                      |
|-----------------------|--------------------------------------------------|
| `-c`, `--code` `<code>`   | Provide SmallFuck code as an inline string   |
| `-t`, `--tape` `<tape>`   | Provide the tape as an inline binary string  |
| `-o`, `--output` `<file>` | Write the output tape to a `.txt` file       |

### File arguments (no flag needed)

| Extension | Description            |
|-----------|------------------------|
| `.sf`     | SmallFuck source file  |
| `.sft`    | SmallFuck tape file    |

### Examples

**Inline code and tape:**
```bash
./sfInterpreter -c "*>*>*" -t "00000"
```

**From files:**
```bash
./sfInterpreter program.sf tape.sft
```

**With output to file:**
```bash
./sfInterpreter program.sf tape.sft -o result.txt
```

The output tape state is printed to stdout, or written to the specified output file if `-o` is provided. On tape overflow or underflow, the current tape state is saved/printed before exiting with a failure code.

## Project Structure

```
C_implementation/
├── src/
│   └── sfInterpreter.c     # Main interpreter source
├── utils/
│   ├── cDataTypes.c/h      # Shared data types and utility functions
│   ├── cHashTable.c/h      # Hash table implementation (used for loop mapping)
│   └── cStack.c/h          # Stack implementation (used for loop parsing)
├── CMakeLists.txt
└── README.md
```

## Implementation Notes

- The tape is represented internally as an `int32_t` array and translated to/from a binary character string (`'0'`/`'1'`) for I/O.
- Loop jump points are pre-computed before execution using a stack and a hash map, mapping each `[` to its matching `]` and vice versa, for O(1) jumps at runtime.
- Memory is dynamically allocated for code and tape buffers and freed on exit.
