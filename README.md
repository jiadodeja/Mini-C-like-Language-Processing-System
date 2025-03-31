# 🖥️ MCL Language Processing System

This repository contains the implementation of a **Lexical Analyzer**, **Recursive-Descent Parser**, and **Interpreter** for a Mini C-Like Language (**MCL**). It spans three programming segments: PA1, PA2, and PA3.

## 📚 Table of Contents
- [PA1: Lexical Analyzer](#pa1-lexical-analyzer)
- [PA2: Recursive-Descent Parser](#pa2-recursive-descent-parser)
- [PA3: Interpreter](#pa3-interpreter)

## 🧾 PA1: Lexical Analyzer

This component tokenizes source code written in MCL. It classifies lexemes into:
- **Keywords:** PROGRAM, IF, ELSE, PRINT...
- **Identifiers:** Case-sensitive variable names
- **Constants:** Integer, Float, Boolean, String, Char
- **Operators & Delimiters:** +, -, *, /, =, {}, (), ;, etc.

The analyzer uses a DFA (Deterministic Finite Automaton) to recognize tokens character by character. Errors are flagged if the token is invalid.

## 🧩 PA2: Recursive-Descent Parser

This parser validates MCL source code against its EBNF grammar. It builds a parse tree and reports syntax errors with line numbers. Each non-terminal rule in the grammar is implemented as a C++ function.

## ⚙️ PA3: Interpreter

This stage extends the parser to interpret and execute MCL code in real time.

Features:
- Variable declaration and symbol tables
- Arithmetic and logical expression evaluation
- Statement execution: assignment, if-else, print
- Type checking and casting
- Runtime error detection (e.g., using uninitialized vars)

Implements a `Value` class to represent values with associated types at runtime.



