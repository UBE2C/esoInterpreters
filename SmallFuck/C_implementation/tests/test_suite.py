import random
import subprocess
import sys

# ===============================
# Configuration
# ===============================

BINARY = "./sfTest"  # path to your compiled CLI interpreter
MAX_STEPS = 10_000  # same as CodeWars reference
TIMEOUT = 2  # seconds per execution


# ===============================
# Reference Interpreter
# ===============================


def ref_interpreter(code, tape, max_steps=MAX_STEPS):
    arr = [int(ch) for ch in tape]
    tape_pointer = 0
    code_pointer = 0
    depth = 0
    striding = False
    dx = 1
    steps = 0

    while 0 <= tape_pointer < len(arr) and 0 <= code_pointer < len(code):
        ch = code[code_pointer]

        if striding:
            if ch == "[":
                depth += 1
            elif ch == "]":
                depth -= 1
            if depth == 0:
                striding = False
                dx = 1
        else:
            if ch == ">":
                tape_pointer += 1
            elif ch == "<":
                tape_pointer -= 1
            elif ch == "*":
                arr[tape_pointer] ^= 1
            elif ch == "[" and arr[tape_pointer] == 0:
                striding = True
                depth = 1
                dx = 1
            elif ch == "]" and arr[tape_pointer] != 0:
                striding = True
                depth = -1
                dx = -1

        code_pointer += dx
        steps += 1

        if steps >= max_steps:
            return None

    return "".join(str(i) for i in arr)


# ===============================
# CLI Wrapper
# ===============================


def run_cli(code, tape):
    try:
        result = subprocess.run(
            [BINARY, "-c", code, "-t", tape],
            capture_output=True,
            text=True,
            timeout=TIMEOUT,
        )
    except subprocess.TimeoutExpired:
        print("Timeout!")
        return None

    # Look for the line with the tape state
    for line in result.stdout.splitlines():
        line = line.strip()
        if line.startswith("Output tape state:"):
            return line.split(":", 1)[1].strip()  # return only the tape string

    # If no tape output is found, treat as crash
    print("Return code:", result.returncode)
    print("STDERR:", result.stderr)
    raise RuntimeError(
        f"Interpreter did not produce tape output.\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
    )


# ===============================
# Fixed Tests
# ===============================


def fixed_tests():
    tests = [
        ("*", "00101100", "10101100"),
        (">*>*", "00101100", "01001100"),
        ("*>*>*>*>*>*>*>*", "00101100", "11010011"),
        ("*>*>>*>>>*>*", "00101100", "11111111"),
        (">>>>>*<*<<*", "00101100", "00000000"),
    ]

    for code, tape, expected in tests:
        actual = run_cli(code, tape)
        assert actual == expected, (
            f"\nFAIL\ncode={code}\ntape={tape}\nexpected={expected}\nactual={actual}"
        )


# ===============================
# Ignore Non-Commands Tests
# ===============================


def ignore_tests():
    tests = [
        ("iwmlis *!BOSS 333 ^v$#@", "00101100"),
        (">*>*;;;.!.,+-++--!!-!!!-", "00101100"),
        ("*,,...,..,..++-->++++-*>--+>*>+++->>..,+-,*>*", "00101100"),
        (">>nssewww>>wwess>*<nnn*<<ee*", "00101100"),
    ]

    for code, tape in tests:
        expected = ref_interpreter(code, tape)
        actual = run_cli(code, tape)
        assert actual == expected, (
            f"\nFAIL\ncode={code}\ntape={tape}\nexpected={expected}\nactual={actual}"
        )


# ===============================
# Out of Bounds Tests
# ===============================


def oob_tests():
    tests = [
        ("*>>>*>*>>*>>>>>>>*>*>*>*>>>**>>**", "0" * 16),
        ("<<<*>*>*>*>*>>>*>>>>>*>*", "0" * 16),
        (
            "*>*>*>>>*>>>>>*<<<<<<<<<<<<<<<<<<<<<*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*>*>*",
            "1" * 32,
        ),
        (">>*>*>*<<*<*<<*>*", "1101"),
    ]

    for code, tape in tests:
        expected = ref_interpreter(code, tape)
        actual = run_cli(code, tape)
        assert actual == expected, (
            f"\nFAIL\ncode={code}\ntape={tape}\nexpected={expected}\nactual={actual}"
        )


# ===============================
# Random Program Generator
# ===============================


def get_random_program(code_length, tape_length, loop_attempts):
    valid_alphabet = "<>*"
    invalid_alphabet = "abcdefghijklmnopqrstuvwxyz!()#?${}"
    code = [
        random.choice(valid_alphabet if random.random() > 0.2 else invalid_alphabet)
        for _ in range(code_length)
    ]

    # Add loops safely
    for _ in range(loop_attempts):
        start_index = random.randint(0, code_length - 2)
        end_index = random.randint(start_index + 1, code_length - 1)
        if code[start_index] not in "[]" and code[end_index] not in "[]":
            code[start_index] = "["
            code[end_index] = "]"

    # Make sure brackets are balanced
    open_count = 0
    for i, c in enumerate(code):
        if c == "[":
            open_count += 1
        elif c == "]":
            if open_count > 0:
                open_count -= 1
            else:
                # unmatched closing bracket → replace with valid command
                code[i] = random.choice(valid_alphabet)
    # append missing closing brackets at end if any unmatched '[' remain
    code += ["]"] * open_count

    tape = [random.choice("01") for _ in range(tape_length)]
    code_str = ">" * (tape_length // 2) + "".join(code)
    tape_str = "".join(tape)
    expected = ref_interpreter(code_str, tape_str)
    return code_str, tape_str, expected


# ===============================
# Random Tests
# ===============================


def random_tests():
    configs = [
        (10, 0, 25),
        (10, 2, 25),
        (100, 10, 25),
        (500, 20, 25),
    ]

    for length, loops, n_tests in configs:
        tested = 0
        while tested < n_tests:
            code, tape, expected = get_random_program(length, length, loops)
            if expected is None:
                continue

            actual = run_cli(code, tape)
            assert actual == expected, (
                f"\nFAIL\ncode={code!r}\ntape={tape!r}\n"
                f"expected={expected}\nactual={actual}"
            )
            tested += 1


# ===============================
# Main Runner
# ===============================


def main():
    print("Running fixed tests...")
    fixed_tests()

    print("Running ignore tests...")
    ignore_tests()

    print("Running OOB tests...")
    oob_tests()

    print("Running random tests...")
    random_tests()

    print("\nAll tests passed 🎉")


if __name__ == "__main__":
    main()
