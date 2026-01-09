# The DJ Compiler

A compiler for the **DJ** (Diminished Java) language, targeting **x86-64 Assembly (NASM)**.

This project implements a complete compilation pipeline from source code to native executable, featuring a custom stack-based runtime environment, dynamic dispatch tables, and a manual memory management system.

---

## 🏗 Architecture

The compiler transforms high-level DJ code into an x86-64 executable through the following stages:

```bash
graph LR
    A[Source (.dj)] -->|Flex| B(Lexer)
    B -->|Bison| C(Parser)
    C --> D(AST Generation)
    D --> E(Type Checker)
    E --> F(Code Generator)
    F -->|NASM| G[Executable]
```

### Key Technical Features

- **Stack Machine Model**: All expression evaluations (arithmetic, logic, calls) are performed purely on the hardware stack (`rsp`), simplifying register allocation.
- **Custom Heap Allocator**: Implements a "Bump Pointer" allocator in the `.bss` section, reserving 512KB of contiguous memory for objects.
- **Dynamic Dispatch**: Polymorphism is handled via a generated **Virtual Table (VTable)** that acts as an executable switchboard for method resolution.
- **Code Generation**: Outputs optimized, formatted NASM x86-64 assembly.

---

## 🚀 Getting Started

### Prerequisites

- `gcc`
- `flex`
- `bison`
- `nasm`
- `make`

### Build & Run

The project includes an automated build system. Use the included wrapper script to compile and run a source file:

```bash
./dj.sh examples/my_program.dj
```

---

## 💻 Language Example

DJ is a statically-typed, object-oriented language similar to a simplified Java. It supports classes, inheritance, and polymorphism.

```c
class Animal extends Object {
    nat speak() { 
        printNat(111); // Output generic sound
        return 0; 
    }
}

class Dog extends Animal {
    nat speak() { 
        printNat(999); // Output bark
        return 0; 
    }
}

main {
    // Heap Allocation & Polymorphism
    Animal a = new Dog();
    
    // VTable dispatch resolves this to Dog.speak()
    a.speak(); 
}
```

---

## 🧠 Memory Model

The runtime uses a simplified memory layout optimized for the x86-64 architecture:

| **Region** | **Component** | **Description** |
| --- | --- | --- |
| **High Mem** | **Stack** | Grows downwards (`sub rsp`). Stores method frames, locals, and temp expression results. |
| **Low Mem** | **Heap** | Fixed 512KB block in `.bss`. Grows upwards via a bump pointer (`r15`). |
| **Text** | **Code** | Contains the main logic, method subroutines, and the VTable dispatcher logic. |

---

## 🛠 Project Structure

- `src/`: Source code (`codegen.c`, `typecheck.c`, `dj.y`, `dj.l`)
- `include/`: Header files defining the AST and Symbol Tables.
- `test/`: Test suite containing good/bad example programs.

---

*Built with C, Flex, Bison, and NASM.*
