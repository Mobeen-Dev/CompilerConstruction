# Citu Compiler Project (Phase 1)

Welcome to the **Citu Compiler** repository for the Compiler Construction course at **Information Technology University (ITU)**.

- **Instructor:** Dr. Umar Suleman (`umarsuleman@gmail.com`)
- **Teaching Assistant:** `mscs24052@itu.edu.pk`
- **Student Name:** Mobeen Qamar
- **Student ID:** BSCS23050
- **Repository URL:** [https://github.com/Mobeen-Dev/CompilerConstruction.git](https://github.com/Mobeen-Dev/CompilerConstruction.git)

---

## 📌 Project Overview
**Citu** is a compiled procedural language featuring exact-width numerical data types and Romanized Urdu conversational keywords for blocks and control structures (`jabtak`, `shro`, `khatam`, `agar`, `magar`, `nikal`, `medalo`, `wapis`).

---

## 📂 Repository Structure
```text
├── citu-grammar.md                     # Original Citu grammar notes & reference
├── doc/
│   └── citu_language_specification.md   # Comprehensive Phase 1 Specification Document
└── examples/
    ├── 01_linear_search.citu           # Program 1: Array linear search & early exit
    ├── 02_factorial_fibonacci.citu     # Program 2: Math series & factorial computation
    └── 03_bubble_sort.citu             # Program 3: In-place bubble sort algorithm
```

---

## 📖 Key Documentation
The complete, authoritative Phase 1 submission document is located at:
👉 **[doc/citu_language_specification.md](doc/citu_language_specification.md)**

It provides comprehensive coverage of:
1. **Three Example Programs:** Fully documented Citu code with line-by-line analyses.
2. **Citu Language Grammar:** Complete Flex regular expressions, EBNF grammar rules, and dangling-else ambiguity resolution.
3. **Compiler Data Structures:** C++ definitions for the Lexer Token Stream, Scoped Symbol Table (Cactus Stack), Abstract Syntax Tree (AST with Visitor Pattern), and Diagnostics Engine.
4. **Intermediate File Formats:** Detailed specifications and concrete dumps for `.tok`, `.ast`, `.sym`, `.tac` (Three-Address Code), and `.asm` (GNU x86-64 assembly).
5. **Git Details & Submission:** Student metadata, collaborator invitation instructions, and commit verification.
