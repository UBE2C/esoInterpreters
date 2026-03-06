#!/usr/bib/env python3


# ----- Necessary imports ----- #
import argparse

# ----- Initialize the argument parser ----- #
a_parser: argparse.ArgumentParser = argparse.ArgumentParser(
    prog="Mini-StringFuck Interpreter",
    description="A python implementation of a simple interpreter for the esoteric mini-StringFuck language.",
)

# Add the main positional argument
_ = a_parser.add_argument(
    "code",
    help="The main and only argument for the interpreter, taking the code to be interpreted as a string.",
)


# ----- Define the main function ----- #
def main() -> None:
    args: argparse.Namespace = a_parser.parse_args()

    output: str = MSF_interpreter(code=args.code)
    print("\n", output)

    return None


# ----- Define additional functions ----- #
def MSF_interpreter(code: str) -> str:
    mem_cell: int = 0
    mem_array: list[int] = []
    output_str: str = ""

    for i in range(len(code)):
        if code[i] == "+":  # Increment as needed
            mem_cell += 1
            if mem_cell == 256:  # Ensure it is an 8 bit cell
                mem_cell = 0
        elif code[i] == ".":
            mem_array.append(mem_cell)  # Output the cell into an int array

    for i in range(len(mem_array)):  # Translate the int array to ASCII chars
        output_str += chr(mem_array[i])

    return output_str


# ----- Call the main function ----- #
if __name__ == "__main__":
    main()
