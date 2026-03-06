import random
import subprocess

# ===============================
# Configuration
# ===============================

BINARY = "./msfInterpreter"  # path to your compiled CLI interpreter
TIMEOUT = 2  # seconds per execution


# ===============================
# Reference Interpreter
# ===============================


def ref_interpreter(code):
    cell = 0
    output = []

    for ch in code:
        if ch == "+":
            cell = (cell + 1) % 256
        elif ch == ".":
            output.append(chr(cell))

    return "".join(output)


# ===============================
# CLI Wrapper
# ===============================


def run_cli(code):
    try:
        result = subprocess.run(
            [BINARY, code],
            capture_output=True,
            timeout=TIMEOUT,
        )
    except subprocess.TimeoutExpired:
        print("Timeout!")
        return None

    if result.returncode != 0:
        print("Return code:", result.returncode)
        print("STDERR:", result.stderr)
        raise RuntimeError(
            f"Interpreter exited with error.\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )

    # Decode raw bytes — no newline stripping, the binary prints no trailing newline
    return result.stdout.decode("latin-1")


# ===============================
# Fixed Tests
# ===============================


def fixed_tests():
    # All expected values are derived from ref_interpreter to avoid
    # hand-calculation errors (cell state carries over between prints!).
    tests = [
        # Single character prints
        "+" * 65 + ".",  # 'A'
        "+" * 72 + ".",  # 'H'
        "+" * 101 + ".",  # 'e'
        "+" * 33 + ".",  # '!'
        "+" * 255 + ".",  # '\xff'
        ".",  # '\x00' (cell starts at 0)
        # Multi-character — cell is NOT reset between prints
        "+" * 72 + "." + "+" * 29 + "." + "+" * 7 + ".." + "+" * 3 + ".",  # Hello
        "+" * 87 + "." + "+" * 14 + ".",  # Wo
        # No output commands → empty string
        "+" * 100,
        # Empty program → empty string
        "",
        # Wrap-around: 256 increments bring cell back to 0
        "+" * 256 + ".",  # '\x00'
        "+" * 256 + "+" * 65 + ".",  # 'A' after one full wrap
        "+" * 512 + "+" * 65 + ".",  # 'A' after two full wraps
        # Print multiple times with wrap in between
        "+" * 255 + ".+.",  # '\xff' then '\x00'
    ]

    for code in tests:
        expected = ref_interpreter(code)
        actual = run_cli(code)
        assert actual == expected, (
            f"\nFAIL [fixed]\ncode={code!r}\nexpected={expected!r}\nactual={actual!r}"
        )


# ===============================
# Ignore Non-Commands Tests
# ===============================


def ignore_tests():
    # Non-command characters must be silently ignored.
    base_programs = [
        "+" * 65 + ".",
        "+" * 72 + "." + "+" * 29 + "." + "+" * 7 + ".." + "+" * 3 + ".",
        "+" * 255 + ".+.",
        "+" * 128 + ".",
    ]

    noise_sources = [
        lambda s: s.replace("+", "+ ").replace(".", ". "),  # spaces
        lambda s: s.replace("+", "+\n").replace(".", ".\n"),  # newlines
        lambda s: "abc" + s + "xyz",  # leading/trailing letters
        lambda s: s.replace("+", "+!@#").replace(".", ".---"),  # symbols
        lambda s: s.replace("+", "+\t").replace(".", ".\t"),  # tabs
        lambda s: "123" + s.replace(".", ";.;"),  # digits and semicolons
    ]

    for base in base_programs:
        for noise_fn in noise_sources:
            code = noise_fn(base)
            expected = ref_interpreter(code)
            actual = run_cli(code)
            assert actual == expected, (
                f"\nFAIL [ignore]\ncode={code!r}\nexpected={expected!r}\nactual={actual!r}"
            )


# ===============================
# Wrap-Around Tests
# ===============================


def wrap_tests():
    tests = [
        # Exactly at boundary
        "+" * 255 + ".",  # \xff
        "+" * 255 + "+.",  # \x00 (256th increment wraps)
        # Multiple wraps
        "+" * (256 * 3) + "+" * 42 + ".",  # '*' (42)
        "+" * (256 * 7 + 33) + ".",  # '!' (33)
        # Wrap mid-output
        "+" * 250 + "." + "+" * 10 + ".",  # \xfa then \x04
        "+" * 200 + "." + "+" * 100 + ".",  # \xc8 then \x2c
        # Wrap right at 255->0 boundary between two prints
        "+" * 255 + ".+." + "+" * 64 + ".",  # \xff, \x00, 'A'
    ]

    for code in tests:
        expected = ref_interpreter(code)
        actual = run_cli(code)
        assert actual == expected, (
            f"\nFAIL [wrap]\ncode={code!r}\nexpected={expected!r}\nactual={actual!r}"
        )


# ===============================
# Random Program Generator
# ===============================


def get_random_program(max_increments_per_segment, n_prints, noise_ratio=0.3):
    """
    Builds a random MiniStringFuck program by interleaving + and . commands
    with random non-command noise characters.
    """
    noise_chars = "abcdefghijklmnopqrstuvwxyz!@#$%^&()-_={}|;':,<>?/ "
    commands = []

    for _ in range(n_prints):
        inc = random.randint(0, max_increments_per_segment)
        commands.extend(["+"] * inc)
        commands.append(".")

    # Inject noise characters at random positions
    result = []
    for ch in commands:
        if random.random() < noise_ratio:
            result.append(random.choice(noise_chars))
        result.append(ch)

    return "".join(result)


# ===============================
# Random Tests
# ===============================


def random_tests():
    configs = [
        # (max_increments_per_segment, n_prints, n_tests)
        (10, 5, 30),
        (50, 10, 30),
        (300, 15, 20),
        (256, 20, 20),  # exercises wrap-around
    ]

    for max_inc, n_prints, n_tests in configs:
        for _ in range(n_tests):
            code = get_random_program(max_inc, n_prints)
            expected = ref_interpreter(code)
            actual = run_cli(code)
            assert actual == expected, (
                f"\nFAIL [random]\ncode={code!r}\nexpected={expected!r}\nactual={actual!r}"
            )


# ===============================
# Main Runner
# ===============================


def main():
    print("Running fixed tests...")
    fixed_tests()

    print("Running ignore (non-command) tests...")
    ignore_tests()

    print("Running wrap-around tests...")
    wrap_tests()

    print("Running random tests...")
    random_tests()

    print("\nAll tests passed 🎉")


if __name__ == "__main__":
    main()
