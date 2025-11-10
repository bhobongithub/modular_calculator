The Calculator Task

Overview

This is a modular calculator program written in C++17.  
It reads multiple math expressions (questions) from a text file, evaluates each one, and outputs both the results and any errors to both the terminal and to an output text file.

Each question can contain:
- Arithmetic operations (`+`, `-`, `*`, `/`, `^`)
- Variable assignments (`x = 10`)
- Built-in math functions (`sin()`, `cos()`)
- Numbers in base 2, 10, or 16

Files and Their Roles

1. include/calc.h - Header file containing all class and struct declarations (interfaces for Lexer, Parser, Evaluator, IOHandler, etc.). 
2. main.cpp - Entry point. Reads input, processes questions, displays and saves results. 
3. io.cpp - Handles reading the input file and writing results to an output file. 
4. processor.cpp - Coordinates parsing, evaluating, and context handling for each question. 
5. tokenizer_parser.cpp - Implements the Lexer and Parser for converting text into AST (Abstract Syntax Tree). 
6. evaluator.cpp - Evaluates expressions and variable assignments. 

Environment Requirements

To compile and run this program successfully, ensure you have:

- A C++17-compatible compiler, such as:
  g++ ≥ 7.0
  clang++ ≥ 6.0
  MSVC (Visual Studio Build Tools 2019 or newer)
- Windows, Linux, or macOS
- Basic console environment (Command Prompt, Terminal, or PowerShell)


Compilation Instructions

From the project directory, run the following command:

On Windows (MSVC):

-cl /std:c++17 /EHsc main.cpp io.cpp processor.cpp tokenizer_parser.cpp evaluator.cpp

On Linux / macOS (g++):

-g++ -std=c++17 main.cpp io.cpp processor.cpp tokenizer_parser.cpp evaluator.cpp -o calculator

This creates an executable:
- `main.exe` (Windows)
- `./calculator` (Linux/macOS)

Running the Program

-Default mode (uses `input.txt` in the same folder):
  .\main.exe

-Specify a custom input file:
.\main.exe "C:\path\to\your\inputfile.txt" (be sure to leave a single space between the executable file and the questions file path)

The results will appear on the screen and be saved to `output.txt` (in the same directory where the source files are stored).


Input Format:

The input file must contain one or more math questions separated by `----`.

Example (`sample1.txt`):

```
----
sin(30) + sin(70)
----
x = 20
y = 40
x + y
----
base=16
FF0045 + 456
----
```

---

Output:

Example output to terminal and 'output.txt':

```
--- Question 1 (base=10) ---
sin(30) + sin(70)
Answer: 0.214140942535
----------------------------------

--- Question 2 (base=10) ---
x = 20
y = 40
x + y
Answer: 60.000000000000
----------------------------------

--- Question 3 (base=16) ---
FF0045 + 456
Error: Undefined variable: FF0045
----------------------------------
```




