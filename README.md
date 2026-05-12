# MASMIF — MASM Inject Forge

A declarative assembly patching framework for 32-bit Windows executables.

MASMIF is a reverse-engineering oriented toolchain designed for modifying existing x86 applications without rebuilding the entire program from source code.

Instead of decompiling and recompiling the whole application, MASMIF focuses on surgical binary modification:
injecting custom assembly code, variables and procedures directly into existing PE executables.

The project was created for practical reverse-engineering workflows involving tools such as:

* x64dbg
* HxD
* PE Viewer

---

# What MASMIF Actually Is

MASMIF is not just an injector.

Internally, it contains:

* a preprocessor,
* lexer,
* parser,
* declaration system,
* assembly code generator,
* assembly conversion layer,
* PE patching system,
* MAP parser,
* x64dbg label generator.

MASMIF generates assembly source code, assembles it through MASM (`ml.exe` and `link.exe`) and then transfers newly generated machine code into the target executable.

The tool operates entirely on 32-bit PE applications.

---

# Why Only 32-bit?

MASMIF was originally designed for personal reverse-engineering workflows targeting legacy x86 applications.

The tool focuses entirely on 32-bit PE executables and currently does not support 64-bit binaries.

This was an intentional design decision based on the needs of the original workflow.

---

# Purpose

MASMIF was created for:

* educational purposes,
* reverse-engineering research,
* low-level experimentation,
* executable analysis,
* game modification workflows.

The project is not intended for malicious usage.

---

# Toolchain Requirements

* Windows
* MASM32
* 32-bit target executable

Recommended tool - x64dbg

---

# Philosophy

Traditional reverse-engineering workflows often require:

* reconstructing entire projects,
* rebuilding massive codebases,
* dealing with incomplete decompilation,
* fighting compiler and linker behavior.

MASMIF was created for a different workflow:

> "I only want to surgically modify selected parts of an existing executable."

The goal of MASMIF is:

* modifying specific code regions,
* injecting new code and data,
* extending functionality,
* adding new PE sections,
* creating hooks into existing code,
* simplifying low-level binary patching.

---

# Example Use Cases

* Game modding
* Reverse-engineering experiments
* Assembly-based extensions
* Code hooking
* Binary instrumentation
* PE analysis
* Low-level debugging

---

# Example Workflow

1. Create MASMIF project.
2. Select target executable.
3. Write declaration script and assembly code.
4. Run build process.
5. Open executable in x64dbg and debug immediately.

---

# Features

## Declarative Injection DSL

MASMIF uses its own declaration-oriented scripting language.

The DSL describes where code and data should be injected and placed inside the target executable.

The DSL focuses on assembly-oriented binary modification rather than replacing assembly itself.

---

## Automatic PE Section Support

MASMIF can automatically create new PE sections when additional space is required.

---

## Clean Base System

MASMIF preserves an untouched base copy of the original executable.

Each build starts from this clean state instead of repeatedly patching an already modified executable.

This prevents duplicated PE sections.

---

## x64dbg Integration

MASMIF automatically generates `.dd32` label files compatible with x64dbg.

This allows the debugger to immediately recognize:

* injected procedures,
* variables,
* generated symbols.

As a result, debugging modified executables becomes significantly easier.

---

## Assembly Convenience Layer

MASMIF contains a custom assembly conversion system designed to improve MASM usability.

### Hexadecimal Conversion

MASMIF supports modern hexadecimal syntax:

```asm
mov eax, 0xFF
```

Automatically converted into MASM-compatible syntax:

```asm
mov eax, 0FFh
```

---

## Quote Semantics

MASMIF intentionally distinguishes between single and double quotes.

### Single Quotes

Single quotes are treated as raw assembly literals and remain untouched.

Example:

```asm
mov eax, '1234'
```

remains unchanged.

### Double Quotes

Double quotes are treated as textual objects and may:

* receive null terminators,
* process escape sequences,

Example:

```asm
db "text1 \r\n text2"
```

becomes:

```asm
db 'text1 ',13,10,' text2',0 
```

---

## Automatic String Declaration

Inline strings can be used directly inside assembly code:

```asm
push "Hello world\r\n"
call ShowMessage
```

MASMIF automatically converts this into:

```asm
push OFFSET ____TXT_0
call ShowMessage

____TXT_0 db 'Hello world',13,10,0
```

Features:

* automatic string declaration,
* automatic OFFSET reference generation.

---

# Main Pipeline

```text
Project Script
      ↓
Preprocessor
      ↓
Lexer
      ↓
Parser
      ↓
Auto string declaration
      ↓
Assembly Generator
      ↓
ASM Converter
      ↓
Generated .asm
      ↓
ml.exe + link.exe
      ↓
result.exe + result.map
      ↓
MAP Lexer/Parser
      ↓
Address Resolution
      ↓
Binary Patching
      ↓
Target Executable
      ↓
x64dbg Labels (.dd32)
```

---

# Future Plans

Planned ideas include:

* better diagnostics and error reporting,
* standalone assembly converter,
* smarter new code/data placement in designated free space.
