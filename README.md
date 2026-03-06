# Esoteric Language Interpreters

Have you ever thought to yourself: "Hmm, C/C++, too easy. Python, child's play. I need something more... exotic..."? Well, then it's time for you to dive into some esoteric programming languages!

This repository contains interpreters for (some...) Brainfuck-family esoteric programming languages, implemented in both C and Python, so you can test your newfound knowledge to your heart's content!

If your favourite Brainfuck-family language interpreter is not here yet, don't worry — more is on the way (eventually...), or you are welcome to submit your own.

Have fun! :)

---

## Available Interpreters

| Language | C Implementation | Python Implementation |
|---|---|---|
| [miniStringFuck](miniStringFuck/C_implementation/README.md) | ✅ | ✅ |
| [SmallFuck](SmallFuck/C_implementation/README.md) | ✅ | ✅ |

---

## Repository Structure

```
Interpreters/
├── README.md
├── LICENSE
├── miniStringFuck/
│   ├── C_implementation/
│   │   ├── src/
│   │   ├── CMakeLists.txt
│   │   └── README.md
│   └── Python_implementation/
│       ├── msf_interpreter.py
│       └── README.md
└── SmallFuck/
    ├── C_implementation/
    │   ├── src/
    │   ├── utils/
    │   ├── CMakeLists.txt
    │   └── README.md
    └── Python_implementation/
        ├── sf_interpreter.py
        └── README.md
```

---

## Contributing

Contributions are welcome! If you'd like to add an interpreter for a language that isn't here yet, feel free to open a pull request. To keep things consistent, please follow these guidelines:

- Each language gets its own directory under the repo root, named after the language
- Each implementation lives in a `C_implementation/` or `Python_implementation/` subdirectory
- Each implementation must include its own `README.md` with build/usage instructions
- C implementations should use CMake as the build system
- Python implementations should target Python 3.10+ and avoid external dependencies where possible

---

## License

This repository is licensed under the MIT License. See [LICENSE](LICENSE) for details.
