#!/usr/bib/env python3


# ----- Necessary imports ----- #
import argparse
import re

# ----- Initialize the argument parser ----- #
a_parser: argparse.ArgumentParser = argparse.ArgumentParser(
    prog="SmallFuck Interpreter",
    description="A python implementation of a simple interpreter for the esoteric SmallFuck language.",
)

# Add the positional/optional code arguments
exclusive_code: argparse._MutuallyExclusiveGroup = (
    a_parser.add_mutually_exclusive_group(required=True)
)

_ = exclusive_code.add_argument(
    "code_path",
    nargs="?",
    help="The path to a .sf file, containing the code to be executed.",
)

_ = exclusive_code.add_argument(
    "-c",
    "--code",
    help="The code argument for the interpreter, taking the code to be interpreted as an inline string.",
)

# Add the positional/optional tape arguments
exclusive_tape: argparse._MutuallyExclusiveGroup = (
    a_parser.add_mutually_exclusive_group(required=True)
)

_ = exclusive_tape.add_argument(
    "tape_path",
    nargs="?",
    help="The path to a .sft file, containing the tape to be executed.",
)

_ = exclusive_tape.add_argument(
    "-t",
    "--tape",
    help="The tape argument for the interpreter, taking the tape to be interpreted as an inline string.",
)

# Add the optional output arguments
_ = a_parser.add_argument(
    "-o",
    "--output",
    help="The path/name argument for the output to be written as a .txt file (adding the extension is not necessary).",
)


# ----- Define the main function ----- #
def main() -> None:
    args: argparse.Namespace = a_parser.parse_args()
    code: str = ""
    tape: str = ""
    output_string: str = ""

    # Prepare arguments
    if args.code_path:
        with open(args.code_path) as c:
            code = clean_string(c.read())

    else:
        code = clean_string(args.code)

    if args.tape_path:
        with open(args.tape_path) as t:
            tape = clean_string(t.read())

    else:
        tape = clean_string(args.tape)

    # Call interpreter
    output_string: str = SF_interpreter(code=code, tape=tape)

    if args.output:
        filename: str = (
            args.output if re.search(r"\..+$", args.output) else args.output + ".txt"
        )

        with open(file=filename, mode="w") as f:
            _ = f.write(output_string)
    else:
        print(output_string)

    return None


# ----- Define the main function ----- #
def SF_interpreter(code: str, tape: str):
    bit_array: list[int] = []
    code_pointer: int = 0
    tape_pointer: int = 0
    output_string: str = ""
    loop_map: dict[int, int] = map_loops(code=code)

    for i in range(len(tape)):
        bit_array.append(int(tape[i]))

    while code_pointer < len(code):
        if code[code_pointer] == ">":
            tape_pointer += 1
            if tape_pointer > len(bit_array) - 1:
                break

        elif code[code_pointer] == "<":
            tape_pointer -= 1
            if tape_pointer < 0:
                break

        elif code[code_pointer] == "*":
            bit_array[tape_pointer] ^= 1

        elif code[code_pointer] == "[":
            if bit_array[tape_pointer] == 0:
                code_pointer = loop_map[code_pointer]

            else:
                pass

        elif code[code_pointer] == "]":
            if bit_array[tape_pointer] == 1:
                code_pointer = loop_map[code_pointer]

            else:
                pass

        code_pointer += 1  # next loop iter, move code pointer

    for i in range(len(bit_array)):
        output_string += str(bit_array[i])

    return output_string


def map_loops(code: str) -> dict[int, int]:
    stack: list[int] = []
    output_map: dict[int, int] = {}

    for i, instr in enumerate(code):
        if instr == "[":
            stack.append(i)

        elif instr == "]":
            # forward mapping
            loop_start: int = stack.pop()
            output_map[loop_start] = i

            # reverse mapping
            output_map[i] = loop_start

        else:
            pass

    return output_map


def clean_string(string: str) -> str:
    output: str = ""
    output = output.join([char for char in string if char != "\n"])

    return output


# ----- Call the main function ----- #
if __name__ == "__main__":
    main()
