The code generator takes the intermediate code or IR and generates code for the target machine. For this project, we will write target code in assembly language form for ease and use an assembler to generate object, although most compilers generate object code directly. It is in this phase of compilation that the properties of the target machine become the major factor. Not only is it necessary to use instructions as they exist on the target machine but decisions about the representation of data will now also play a major role, such as how many bytes or words variables of integer and floating-point data types occupy in memory.

In our example, 

```c
a[index] = 4 + 2
```

we must now decide how integers are to be stored to generate code for the array indexing. For example, a possible sample code sequence for the given expression might be (in a hypothetical assembly language):

```nasm
MOV R0, index  ;; value of index -> R0
MUL R0, 2      ;; double value in R0
MOV R1, &a     ;; address of a -> R1
ADD R1, R0     ;; add R0 to R1
MOV *R1, 6     ;; constant 6 -> address in R1
```

In this code we have used a C-like convention for addressing modes, so that **&a** is the address of **a** (i.e., the base address of the array) and that ***R1** means indirect register addressing (so that the last instruction stores the value 6 to the address contained in R1). In this code we have also assumed that the machine performs byte addressing and that integers occupy two bytes of memory (hence the use of 2 as the multiplication factor in the second instruction).

## Code generation

For code generation, we need to build the runtime environment, which is the structure of the target computer’s registers and memory that serves to manage memory and maintain the information needed to guide the execution process. There are three kinds on environments: fully static environment (of languages like FORTRAN77), the stack-based environment (of languages like C & C++), and the fully dynamic environment (of functional languages like LISP). It is important to keep in mind that a compiler can maintain an environment only indirectly, in that it must generate code to perform the necessary maintenance operations during program execution. Meaning that part of code generation is not just generating target code from the source code but also generating the environment that allows for maintained execution.

### Memory organization during program execution

### DJ’s Stack-based runtime environment

At the start of a NASM generate code, we request 65536 “quad-words” (e.g. 524,288 bytes) of contiguous memory at program startup (load time), which will serve as our program heap memory. Think of this memory space as an array of quad-words (8 bytes) where each index holds 8 bytes and can store a value between 0 to 18,446,744,073,709,551,615 and at the start all indices are initialized to 0. **The Heap Pointer** (`R15`) is located in Low Memory (the `.bss` section) and points exactly to the first byte (index 0) of our heap memory array. The purpose of the heap pointer is to  track the "frontier" of the heap. Everything *below* `R15` is taken (allocated objects). Everything *above* `R15` is fresh, zeroed-out memory waiting to be used. This pointer only moves UP (to higher addresses).

**The Stack Pointer (`RSP`)** is located in High Memory (The Kernel gives this to you). The OS sets this to a very high address (e.g., `0x7ffffff...`) before your code even runs. It points to the "Top" of the stack. The purpose of the stack pointer is to track the current workspace. It points to the last valid piece of data pushed onto the stack. It grows **DOWN** (to lower addresses).

The "Danger Zone" (Stack Overflow) happens if `RSP` moves so far down (left) and `R15` moves so far up (right) that they collide, but in modern OS layouts, they are gigabytes apart, so this rarely happens unless you have infinite recursion.

**The Frame Pointer (`RBP`)** is located inside the Stack (High Memory). At the very start, it is undefined. However, your first instruction is usually `mov rbp, rsp`. This makes `RBP` point to exactly the same place as `RSP`. The purpose of the frame pointer is to act as an **anchor**. While `RSP` is wildly moving up and down to calculate math expressions (like `2 + 3 * 4`), `RBP` stays frozen at the start of the function. This allows you to reliably find local variables (e.g., "Local variable 1 is always 8 bytes below the anchor"), arguments and other function metadata. `RBP` always point to the frame of the currently running function. If you want info about the currently running function you will use the `RBP` as the reference point. It stays still during a function, but when you call a new function, it jumps to the new `RSP` of that function and stores the old anchor of the previous function. When you return from the current function, it jumps back to the old `RBP`. This way you can always confidently made queries using the `RBP` and be sure you queries are concerning the currently running function. In DJ, `RBP` is always pointing to memory index 5 of the frame. Index 0 to 4 contains metadata about the function and local variables, if any are defined, will start from index 6. Just as a reminder each index holds 8 bytes of memory.

> **It is crucial to understand that every expression (including method calls) gets translated into NASM code that, when finished executing, leaves the expression’s result on the top of the stack.** This is the fundamental "Golden Rule" or "Invariant" of your compiler's code generation strategy. This design pattern is known as a **Stack Machine** model. It simplifies the compiler logic immensely because every single function, operation, or expression can assume two things: first, if it needs input, that input is already sitting on the stack; and second, when it finishes its work, it must leave its own output on the top of the stack. This "contract" ensures that complex, nested expressions like `(2 + 3) * foo()` can be generated recursively without the compiler needing to track which CPU registers are currently in use.
> 
> 
> When the compiler encounters a binary operation like `PLUS_EXPR` (e.g., `A + B`), it does not immediately generate an `add` instruction. Instead, it recursively calls code generation for the left child (`A`) first - which may be a simple integer or a complex expression itself. Because of the Golden Rule, the generated assembly for `A` will calculate its value and effectively "push" it onto the stack (by decrementing `RSP` and writing to `[rsp]`). The compiler then immediately calls code generation for the right child (`B`) - which again may be a simple integer or a complex expression itself. The assembly for `B` runs, decrementing `RSP` again and placing its result at the new top. At this precise moment, the CPU stack holds the temporary result of `B` at `[rsp]` and the temporary result of `A` safely stored underneath it at `[rsp + 8]`. The stack has grown by two slots, holding the inputs ready for the operation.
> 
> Once both operands are on the stack, the compiler generates the actual logic to consume them and produce the single result. It emits instructions to load the right operand from `[rsp]` into a register (like `RBX`) and the left operand from `[rsp + 8]` into another register (like `RAX`). The addition is performed in the registers (`add rax, rbx`), leaving the sum in `RAX`. To satisfy the invariant that the expression must leave *only* one result, the compiler effectively merges the two stack slots back into one. It calls `incSP()` to "pop" the top slot (discarding `B`'s memory), which moves the stack pointer back down to `A`'s slot. Finally, it overwrites that slot with the sum (`mov [rsp], rax`). From the perspective of the outer program, the two inputs `A` and `B` have vanished, replaced perfectly by their sum.
> 
> This model applies universally because every single expression type—whether a simple number, a variable, methods calls, or a complex logic gate—obeys the same contract.
> 

Each index of memory in the stack or heap is 8 bytes. Using 8 bytes is a design choice called Word Alignment. For storing addresses, 8 bytes is non-negotiable on a 64-bit processor (x86-64). An address is a number between `0` and `(2^{64})-1`. To store a number that big, you physically need 64 bits, which is exactly 8 bytes. For example, `new A()` returns a memory address like `0x00007FFF5FBFF7A0`. That value requires the full 8-byte register/slot. However, using 8 bytes to always store single for integers and characters is a convenience choice by me, the compiler writer. The number `5` could technically fit in 1 byte (`00000101`). However, mixing sizes (1 byte for bools, 4 bytes for ints, 8 bytes for pointers) makes managing the stack header difficult. You would have to constantly calculate different offsets (`rsp + 1`, then `rsp + 4`...). For a serious compiler, it is very much worth it and required that you be this efficient with memory, but for this project, I promote everything to 8 bytes for ease. So when you store the number `5`, it is stored as:

```
00 00 00 00 00 00 00 05  (8 bytes total)
```

That way the stack and heap are completely uniform which allows us to conceptualize memory as a true array, albeit very memory inefficient.My current implementation trades memory efficiency (using 8 bytes for a `1` or `0` boolean) for compiler simplicity. Fixing this problem would be the primary directive when upgrading this compiler. 

### How instructions are laid out in the assembly file

Here is the precise structure of the assembly file your compiler produces:

1. Helper functions for printing, reading and exiting the program.
2. **Init Heap and Stack**: Before you can do anything, you need memory. You declare the BSS (Heap) and set up the stack frame pointers (`rbp`, `rsp`) and heap pointer (`r15`) so subsequent instructions have a valid environment to work in.
3. **Allocate space for main locals**: The "Main Block" is essentially a function itself. Just like any other method, it needs its local variables initialized to 0 on the stack before the logic starts.
4. **Generate code for the main block** (This is the entry point): This is the **actual execution** of your program. The CPU executes instructions sequentially, so this code *must* be placed immediately after initialization.
5. **Generate code for program termination**: This is critical. Once the main block is done, the CPU keeps reading the next lines of memory. We call the `_exit_program` helper function here. If we didn’t, the CPU would accidentally "fall through" into the code for the first class method below, executing it with garbage data. This acts as a hard stop.
6. **Generate code for all the class methods**: These are distinct subroutines (functions). They sit passively in memory, waiting to be `call`ed or `jmp`ed to. They are never executed unless explicitly invoked by the main block or another method.
    
    Once a method is invoked, code generation for each method constructs a temporary environment called a **Stack Frame**. This frame gives the method its own private scratchpad for local variables and ensures it knows where to return when finished.
    
    1. The Prologue (`genPrologue`)
        
        The prologue is the setup phase. It runs immediately when the method starts. Its job is to "freeze" the state of the caller so we can safely create a new workspace.
        
        First, you execute `push rbp`. This saves the *caller's* Frame Pointer onto the stack. If you didn't do this, `rbp` would be overwritten, and when the method finished, the program wouldn't remember where the previous function's variables were located.
        
        Next, you execute `mov rbp, rsp`. This creates the new anchor. By making `rbp` point to the current stack top, you establish a stable reference point (the "Base") for the current method. No matter how much you push/pop during calculations, `rbp` stays fixed.
        
        Finally, you allocate space for local variables. You look up how many locals the method needs (e.g., `numLocals = 3`) and loop that many times, executing `decSP()` followed by `mov qword [rsp], 0`. This effectively grows the stack downward to reserve clean, zeroed-out slots for variables.
        
    2. The Body (`genBody`)
        
        The body generation is relatively straightforward compared to the prologue/epilogue. It is responsible for translating the actual logic of the method.
        
        The function `genBody` first has to perform a search. It traverses the Global Abstract Syntax Tree (AST) to find the specific `ClassDecl` and `MethodDecl` corresponding to the method being generated. Once found, it locates the `EXPR_LIST` node, which represents the sequence of expressions inside the method.
        
        It then calls `codeGenExprs`. This function iterates through every expression in the list (assignments, print statements, loops) and generates the assembly for them. Crucially, because `rbp` was set up in the prologue, any reference to a local variable (e.g., `x = 5`) is translated into an offset from `rbp` (e.g., `mov [rbp - 8], 5`).
        
    3. The Epilogue (`genEpilogue`)
        
        The epilogue is the cleanup phase. It runs when the method is finished and needs to return control to the caller.
        
        First, it handles the **Return Value**. By convention in your stack machine, the result of the last expression is sitting at the top of the stack (`[rsp]`). The epilogue moves this value into the `RAX` register (`mov rax, [rsp]`), which is the standard register for return values in x86.
        
        Next, it tears down the stack frame. It runs `mov rsp, rbp` to instantly discard all local variables and temporary calculations, resetting the stack pointer to where it was at the start. Then, `pop rbp` restores the *caller's* anchor point.
        
        Finally, it performs a specific cleanup for your custom calling convention. When the method was called, the caller pushed 5 items onto the stack (Arguments, Static Class ID, Static Method ID, `this` pointer, and Return Address). The instruction `add rsp, 40` (5 items × 8 bytes) moves the stack pointer past these items, effectively "popping" the arguments so they don't clog the stack. It then jumps to the return address that was saved.
        
    
    When a method is running (inside `genBody`), the memory relative to Frame pointer (`RBP`) looks exactly like this:
    
    **Positive Offsets (The Caller's Data)**
    
    - `RBP + 40`: **Argument** (The value passed to the method)
    - `RBP + 32`: **`this` Pointer** (Address of the object)
    - `RBP + 24`: **Static Class ID** (For VTable checks)
    - `RBP + 16`: **Static Method ID** (For VTable checks)
    - `RBP + 8`: **Return Address** (Where code execution resumes after return)
    
    **Zero Offset (The Anchor)**
    
    - `RBP + 0`: **Old RBP** (The link back to the previous function's frame).
    
    **Negative Offsets (The Method's Data)**
    
    - `RBP - 8`: **Local Variable 0**
    - `RBP - 16`: **Local Variable 1**
    - ...
    - `RBP - X`: **Temporary Calculation** (e.g., result of `2+2`)
    
    ```
           FRAME MEMORY
          | Argument 1  |  <-- [RBP + 40] (Stable Reference)
          | ...         |
          | Return Addr |  <-- [RBP + 8]
          +-------------+
    RBP-> | Old RBP     |  <-- [RBP + 0]  (THE ANCHOR: Holds address of Caller's Anchor)
          +-------------+
          | Local Var 1 |  <-- [RBP - 8]  (Stable Reference)
          | Local Var 2 |  <-- [RBP - 16]
          | ...         |
          | Math Temp   |  <-- RSP is here, moving up/down
    ```
    
    This layout explains why our code uses `[rbp + 8]` to access the parameter and `[rbp - 8]` to access the first local variable. The "Saved RBP" and "Return Address" sit comfortably in between them.
    
    Let’s look at a concrete example:
    
    **First,** remember that memory is an array of slots. Every slot holds one full 64-bit (8 bytes) value.
    
    **The Starting State (The Caller's World)**
    Imagine we are inside the `Main` function. The Frame Pointer (RBP) is currently **Index 1000**. This is our anchor. If `Main` wants to access its first local variable, it looks at `RAM[999]` (RBP - 1). The Stack Pointer (RSP) is currently at **Index 990**, meaning `Main` is currently using the 10 slots between the anchor and the stack top for its own work. We are about to call a method `foo(5)`.
    
    **The Transition (Building the Bridge)**
    Before calling `foo`, `Main` must push the "Call Packet" onto the stack. It pushes the argument `5` (RSP goes to **989**). Then it pushes the four internal values: Static Class ID, Static Method ID, `this` pointer, and finally the Return Address. Since we pushed 5 items total, the Stack Pointer moves down by 5 slots. It is now at **Index 985**. The slots from 985 to 989 hold the data `foo` will need.
    
    **The Anchor (The Prologue)**
    We enter `foo`. The first instruction is `push rbp`. This saves the *old* anchor into the stack. `RAM[984]` now holds the value **1000** (the address of Main's anchor). The RSP is now **984**.
    The next instruction is `mov rbp, rsp`. We update the Frame Pointer to equal **984**.
    
    - **The Frozen Anchor:** For the entire duration of `foo`, **RBP = 984**.
    - **The Link:** If we look inside `RAM[984]`, we see **1000**. This is our breadcrumb trail back to `Main`.
    - **Relative Access:** The argument `5` is exactly 5 slots "above" us at `RAM[989]` (`RBP + 5`).
    
    **The Execution (The Body)**
    Now `foo` runs. It needs one slot for a local variable, so we decrement RSP to **983**. `RAM[983]` is now reserved for "Local Var 0". If `foo` does math (like `2 + 3`), it pushes `2` (RSP -> **982**) and `3` (RSP -> **981**). It adds them, pops both, and pushes the result `5` (RSP -> **982**). Notice that while the Stack Pointer is bouncing around in the 980s, the Frame Pointer stays rock solid at **984**, ensuring we never lose track of where the variables end and the Caller's data begins.
    
    **The Restoration (The Epilogue)**
    The method finishes. The result (`5`) is temporarily moved to a register. We execute `mov rsp, rbp`. The Stack Pointer instantly snaps from **982** back to **984**. The local variable at 983 is effectively abandoned.
    Next, we execute `pop rbp`. The CPU looks at `RAM[984]` (the current stack top), reads the value **1000**, and puts it back into the RBP register. RBP is now **1000** again—we are mentally back in `Main`. The RSP automatically moves up one slot to **985** (because we popped).
    
    Finally, the return logic "cleans up" the arguments. We add 5 to the Stack Pointer (simulating popping the 5 argument slots). The RSP moves from 985 back to **990**. This is exactly where we started in Step 1! The only difference is that `Main` now pushes the result (`5`) onto the stack, leaving RSP at **989**. The invariant holds: one expression was evaluated, and one result is left on the stack.
    
7. **Generate Virtual table**: This is your custom Dispatcher (`_VTable_Dispatch`). It's a large block of comparison logic. It sits at the end (or anywhere outside the main flow) and is jumped to whenever a dynamic method call happens.  In standard C++ or Java, the "Virtual Table" is usually a data array stored in memory. However, my implementation is unique: my VTable is not a data structure; it is **executable code**. It is a massive, generated "Switch Statement" (or decision tree) located at the `_VTable_Dispatch` label.
    
    Every "row" in your generated VTable checks a specific combination of three inputs to decide where to jump.
    
    1. **StaticClass#**: What the compiler *thought* the object was at compile time.
    2. **StaticMethod#**: The index of the method being called.
    3. **DynamicCallerType#**: The actual type ID stored inside the object on the heap.
    
    The table maps or routes each combination to the actual code that should run using the **DynamicClass#, t**he class that actually owns the code we should run, and the **DynamicMethod#**, the method index inside that class. Hence the Vtable has 5 columns, the first 3 are considered the input and the last two are considered the output.
    
    Let's use an example to trace the generation:
    
    ```java
    // Class 1
    class Animal extends Object {
        nat speak(nat volume) {
            printNat(111); // "Animal Speak"
            return 1;
        }
    }
    
    // Class 2: Overrides 'speak'
    class Dog extends Animal {
        nat speak(nat volume) {
            printNat(222); // "Dog Speak" (Different behavior)
            return 1;
        }
    }
    
    // Class 3: Inherits 'speak' (No override)
    class Cat extends Animal {
        // No speak method defined here
        nat meow() {
            printNat(333);
            return 1;
        }
    }
    ```
    
    Class 1: `Animal` (Method 0: `speak`)
    Class 2: `Dog` (Extends `Animal`, Overrides `speak`)
    Class 3: `Cat` (Extends `Animal`, Inherits `speak`)
    
    The `genVTable` loop runs for every class to generate its dispatch rules.
    
    **Pass 1: Generating for `Animal` (Class 1)**
    
    - Self-Rule: If Static is `Animal` and Dynamic is `Animal` → Call `Animal.speak`.
        
        *Tuple:* `(1, 0, 1) -> Jump class1method0`
        
    
    **Pass 2: Generating for `Dog` (Class 2)**
    
    - **Self-Rule:** If Static is `Dog` and Dynamic is `Dog` → Call `Dog.speak`.
        
        *Tuple:* `(2, 0, 2) -> Jump class2method0`
        
    - **Parent-Rule:** The loop looks up the parent tree and sees the parent `Animal`. It sees that `Dog` **overrides** `speak`. It generates: If Static is `Animal`, but Dynamic is `Dog` →Call `Dog.speak`.
        
        *Tuple:* `(1, 0, 2) -> Jump class2method0`
        
    
    **Pass 3: Generating for `Cat` (Class 3)**
    
    - **Self-Rule:** If Static is `Cat` and Dynamic is `Cat` → Call `Animal.speak` (Inherited).
        
        *Tuple:* `(3, 0, 3) -> Jump class1method0`
        
    - **Parent-Rule:** It looks up the parent tree and sees the parent `Animal`. It sees that `Cat` **does not override** `speak`. It generates: If Static is `Animal`, but Dynamic is `Cat` (3) → Call `Animal.speak`.
        
        *Tuple:* `(1, 0, 3) -> Jump class1method0`
        
    
    ---
    
    Now, let's trace exactly what happens in memory when you run this code:
    
    ```java
    Animal a = new Dog();
    a.speak();
    ```
    
    The variable a is of type Animal (Static Class 1). speak is Method 0. However, a holds a pointer to a Dog object (Type 2).
    
    The `METHOD_CALL_EXPR` code pushes the call packet to set up the frame in memory like discusses before:
    
    - Push **Argument**
    - Push **`this` Pointer** (Address of the `Dog` object).
    - Push **Static Class 1** (`Animal`).
    - Push **Static Method 0** (`speak`).
    - Push **Return Address**.
    - Push the Old Frame Pointer and Set the new Frame Pointer
    - **Then Jump** to `_VTable_Dispatch`.
    
    The CPU lands at `_VTable_Dispatch`, which uses the new Frame pointer `RBP` to get the static class, static method, and the dynamic caller/object type (using the `this` pointer) of the currently running function. It starts running down the list of comparison blocks ("rows") generated above.
    
    **Check 1 (The Animal Row):**
    
    - Is Static Class == 1? **Yes.**
    - Is Static Method == 0? **Yes.**
    - Is Dynamic Type (from `[this]`) == 1 (`Animal`)? **No.** (It's a `Dog`, Type 2).
    - *Result:* comparison fails, jump to next label.
    
    **Check 2 (The Dog-as-Animal Row):**
    
    - Is Static Class == 1? **Yes.**
    - Is Static Method == 0? **Yes.**
    - Is Dynamic Type (from `[this]`) == 2 (`Dog`)? **Yes!**
    - *Result:* Match found!
    - **Action:** `jmp class2method0` (This jumps to the `Dog` version of the code).
    
    Even though the variable was type `Animal`, the system successfully identified that the object was a `Dog` and routed execution to the `Dog`'s method.
    
    If the object had been a `Cat`, Check 2 would have failed, and it would have fallen through to the "Cat-as-Animal" row, which would have routed it to `class1method0` (the base `Animal` method).
    
    Take a look at the table
    
    | **Static Class** | **Static Method** | **Dynamic Type** | **Dynamic Class** | **Dynamic Method** |
    | --- | --- | --- | --- | --- |
    | `Animal` (1) | `speak` (0) | `Animal` (1) | `Animal` (1) | `speak` (0) |
    | `Animal` (1) | `speak` (0) | `Dog` (2) | **`Dog`** (2) | `speak` (0) |
    | `Animal` (1) | `speak` (0) | `Cat` (3) | `Animal` (1) | `speak` (0) |
    | `Dog` (2) | `speak` (0) | `Dog` (2) | `Dog` (2) | `speak` (0) |

### Implementation

In the includes/ folder, create a file called `codegen.h`:

```
📂 DjCompiler/
├── 📂 includes/
│   ├── 📄 codegen.h
│   └── ...
└── ...
```

`codegen.h`:

```c
/* File codegen.h: Header File for Code Generator for DJ compiler */

#ifndef CODEGEN_H 
#define CODEGEN_H

#include "typecheck.h"
#include <stdio.h>

/* Perform code generation for the compiler's input program.
   The code generation is based on the enhanced symbol tables built
   in setupSymbolTables, which is declared in symtbl.h.

   This method writes NASM code for the whole program to the 
   specified outputFile (which must be open and ready for writes 
   before calling generateDISM).

   This method assumes that setupSymbolTables(), declared in 
   symtbl.h, and typecheckProgram(), declared in typecheck.h, 
   have already executed.
*/
void generateNASM(FILE *outputFile);

#endif
```

Then, In the `src/` folder, create a folder called `codegen/` and within that folder create a file called `codegen.c`:

```
📂 DjCompiler/
├── 📂 src/
│   ├── 📂 codegen/
│   │   └── 📄 codegen.c
│   └── ...
└── ...
```

`codegen.c`:

```c
#include "../../include/codegen.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORD_SIZE 8

/* Global for the output file */
FILE *fout;

/* Global to the virtual table label */
unsigned int VTableLabel = 0;

/* Global to the next unique label number to use */
unsigned int labelNumber = 1;

/* Forward Decls */
void codeGenExpr(ASTree *, int, int);
void codeGenExprs(ASTree *, int, int);
void internalCGerror(const char *fmt, ...);
void incSP();
void decSP();
void checkNullDereference();
void genPrologue(int, int);
void genEpilogue(int, int);
void genBody(int, int);
void genVTable();
int getFieldOffset(int, char *);

/* --- HELPER FUNCTIONS FOR ASM GENERATION --- */

void genLibLessHelpers() {
  // _exit_program (Unchanged)
  fprintf(fout, "\n_exit_program:\n");
  fprintf(fout, "    mov rax, 60\n");
  fprintf(fout, "    syscall\n");

  // _print_int (FIXED)
  fprintf(fout, "\n_print_int:\n");
  fprintf(fout, "    push rbp\n");
  fprintf(fout, "    mov rbp, rsp\n");
  fprintf(fout, "    push rbx\n");
  fprintf(fout, "    push rcx\n");
  fprintf(fout, "    push rdx\n");
  fprintf(fout, "    push rsi\n");
  fprintf(fout, "    push rdi\n");
  fprintf(fout, "    mov rcx, 0\n");
  fprintf(fout, "    mov rbx, 10\n");
  fprintf(fout, "    cmp rax, 0\n");
  fprintf(fout, "    jne .convert_loop\n");
  fprintf(fout, "    dec rsp\n");
  fprintf(fout, "    mov byte [rsp], '0'\n");
  fprintf(fout, "    inc rcx\n");
  fprintf(fout, "    jmp .print_digits\n");
  fprintf(fout, ".convert_loop:\n");
  fprintf(fout, "    cmp rax, 0\n");
  fprintf(fout, "    je .print_digits\n");
  fprintf(fout, "    xor rdx, rdx\n");
  fprintf(fout, "    div rbx\n");
  fprintf(fout, "    add rdx, '0'\n");
  fprintf(fout, "    dec rsp\n");
  fprintf(fout, "    mov [rsp], dl\n");
  fprintf(fout, "    inc rcx\n");
  fprintf(fout, "    jmp .convert_loop\n");
  fprintf(fout, ".print_digits:\n");
  fprintf(fout, "    mov rax, 1\n");
  fprintf(fout, "    mov rdi, 1\n");
  fprintf(fout, "    mov rsi, rsp\n");
  fprintf(fout, "    mov rdx, rcx\n"); // Move count to RDX
  fprintf(fout, "    syscall\n");      // RCX is clobbered here!

  // *** FIX IS HERE: Use RDX instead of RCX ***
  fprintf(fout, "    add rsp, rdx\n");

  fprintf(fout, "    dec rsp\n");
  fprintf(fout, "    mov byte [rsp], 10\n");
  fprintf(fout, "    mov rax, 1\n");
  fprintf(fout, "    mov rdi, 1\n");
  fprintf(fout, "    mov rsi, rsp\n");
  fprintf(fout, "    mov rdx, 1\n");
  fprintf(fout, "    syscall\n");
  fprintf(fout, "    inc rsp\n");
  fprintf(fout, "    pop rdi\n");
  fprintf(fout, "    pop rsi\n");
  fprintf(fout, "    pop rdx\n");
  fprintf(fout, "    pop rcx\n");
  fprintf(fout, "    pop rbx\n");
  fprintf(fout, "    pop rbp\n");
  fprintf(fout, "    ret\n");

  // _read_int (Unchanged)
  fprintf(fout, "\n_read_int:\n");
  fprintf(fout, "    push rbp\n");
  fprintf(fout, "    mov rbp, rsp\n");
  fprintf(fout, "    push rbx\n");
  fprintf(fout, "    push rcx\n");
  fprintf(fout, "    push rdx\n");
  fprintf(fout, "    push rsi\n");
  fprintf(fout, "    push rdi\n");
  fprintf(fout, "    mov rax, 0\n");
  fprintf(fout, "    mov rdi, 0\n");
  fprintf(fout, "    lea rsi, [rel input_buffer]\n");
  fprintf(fout, "    mov rdx, 20\n");
  fprintf(fout, "    syscall\n");
  fprintf(fout, "    xor rax, rax\n");
  fprintf(fout, "    lea rsi, [rel input_buffer]\n");
  fprintf(fout, "    xor rbx, rbx\n");
  fprintf(fout, ".parse_loop:\n");
  fprintf(fout, "    mov bl, [rsi]\n");
  fprintf(fout, "    cmp bl, 10\n");
  fprintf(fout, "    je .parse_done\n");
  fprintf(fout, "    cmp bl, '0'\n");
  fprintf(fout, "    jl .parse_done\n");
  fprintf(fout, "    cmp bl, '9'\n");
  fprintf(fout, "    jg .parse_done\n");
  fprintf(fout, "    sub bl, '0'\n");
  fprintf(fout, "    imul rax, 10\n");
  fprintf(fout, "    add rax, rbx\n");
  fprintf(fout, "    inc rsi\n");
  fprintf(fout, "    jmp .parse_loop\n");
  fprintf(fout, ".parse_done:\n");
  fprintf(fout, "    pop rdi\n");
  fprintf(fout, "    pop rsi\n");
  fprintf(fout, "    pop rdx\n");
  fprintf(fout, "    pop rcx\n");
  fprintf(fout, "    pop rbx\n");
  fprintf(fout, "    pop rbp\n");
  fprintf(fout, "    ret\n");
}

/* Main Entry Point for Code Generation */
void generateNASM(FILE *outputFile) {
  fout = outputFile;

  fprintf(fout, "section .bss\n");
  fprintf(fout, "    heap_memory resq 65536\n");
  fprintf(fout, "    input_buffer resb 21\n");

  fprintf(fout, "\nsection .text\n");
  fprintf(fout, "    global _start\n");

  genLibLessHelpers();

  fprintf(fout, "\n_start:\n");
  fprintf(fout, "    mov rbp, rsp\n");

  // R15 will act as our Heap Pointer
  fprintf(fout, "    lea r15, [rel heap_memory]\n");

  // Initialize Main Block Locals (push 0s onto stack)
  for (int i = 0; i < numMainBlockLocals; i++) {
    decSP();
    fprintf(fout, "    mov qword [rsp], 0\n");
  }

  // Generate code for main block expressions
  codeGenExprs(mainExprs, -1, -1);

  // Clean exit
  fprintf(fout, "    mov rdi, 0\n");
  fprintf(fout, "    call _exit_program\n");

  // Generate Code for All Methods
  ClassDecl *class;
  for (int i = 1; i < numClasses; i++) {
    class = &classesST[i];
    for (int j = 0; j < class->numMethods; j++) {
      fprintf(fout, "class%dmethod%d: ; %s.%s\n", i, j, class->className,
              class->methodList[j].methodName);
      genPrologue(i, j);
      genBody(i, j);
      genEpilogue(i, j);
    }
  }

  // Generate VTable (Dispatcher)
  genVTable();
}

void internalCGerror(const char *fmt, ...) {
  va_list args;
  // Initialize the argument list
  va_start(args, fmt);
  printf("Internal Error: ");
  // Print the user's formatted message
  vprintf(fmt, args);
  printf("\n");
  // Clean up
  va_end(args);
  exit(EXIT_FAILURE);
}

/* Stack Operations: Stack Grows Down in x86 */
void incSP() { fprintf(fout, "    add rsp, %d\n", WORD_SIZE); }

void decSP() { fprintf(fout, "    sub rsp, %d\n", WORD_SIZE); }

void checkNullDereference() {
  fprintf(fout, "    cmp qword [rsp], 0\n");
  fprintf(fout, "    jne .L_null_ok_%d\n", labelNumber);
  fprintf(fout, "    mov rdi, 1\n");
  fprintf(fout, "    call _exit_program\n");
  fprintf(fout, ".L_null_ok_%d:\n", labelNumber);
  labelNumber++;
}

/* Expression Code Generation */
void codeGenExpr(ASTree *t, int classNumber, int methodNumber) {
  int endLabel, trueLabel, falseLabel;
  int exprType, offset, methodReturnAddr;
  char *idVal, *fieldName;

  if (classNumber == 0 && t->typ != NAT_LITERAL_EXPR)
    internalCGerror("Error in Object class codegen.");

  switch (t->typ) {
  case NAT_LITERAL_EXPR:
    decSP();
    fprintf(fout, "    mov qword [rsp], %d\n", t->natVal);
    break;

  case NULL_EXPR:
    decSP();
    fprintf(fout, "    mov qword [rsp], 0\n");
    break;

  case NEW_EXPR: {
    int objTyp = classNameToNumber(t->children->data->idVal);
    int currTyp = objTyp;

    // Allocate space for fields on heap (R15)
    while (currTyp > 0) {
      ClassDecl *class = &classesST[currTyp];
      for (int i = 0; i < class->numVars; i++) {
        fprintf(fout, "    mov qword [r15], 0\n");
        fprintf(fout, "    add r15, %d\n", WORD_SIZE);
      }
      currTyp = class->superclass;
    }

    // Store Type ID at start of object (Wait, DJ objects store type ID first?)
    // Based on DISM logic, it seems fields are allocated, but where is type ID?
    // Actually, the simplest object model: [TypeID][Field1][Field2]...
    // Let's alloc the type ID first.

    // **Correction to match logic**: The old code pushed fields then ID.
    // We will store TypeID at the address, and fields follow.
    fprintf(fout, "    mov rax, %d\n", objTyp);
    fprintf(fout, "    mov [r15], rax\n");
    decSP();
    fprintf(fout, "    mov [rsp], r15\n");         // Push Object Address
    fprintf(fout, "    add r15, %d\n", WORD_SIZE); // Move heap past Type ID

    // Now advance heap for all fields (init to 0)
    currTyp = objTyp;
    while (currTyp > 0) {
      ClassDecl *class = &classesST[currTyp];
      for (int i = 0; i < class->numVars; i++) {
        fprintf(fout, "    mov qword [r15], 0\n");
        fprintf(fout, "    add r15, %d\n", WORD_SIZE);
      }
      currTyp = class->superclass;
    }
  } break;

  case THIS_EXPR:
    // In our stack layout (see Prologue), 'this' is at [rbp + 32]
    decSP();
    fprintf(fout, "    mov rax, [rbp + 32]\n");
    fprintf(fout, "    mov [rsp], rax\n");
    break;

  case READ_EXPR:
    fprintf(fout, "    call _read_int\n");
    decSP();
    fprintf(fout, "    mov [rsp], rax\n");
    break;

  case PRINT_EXPR:
    codeGenExpr(t->children->data, classNumber, methodNumber);
    fprintf(fout, "    mov rax, [rsp]\n");
    fprintf(fout, "    call _print_int\n");
    break;

  case WHILE_EXPR: {
    int whileLabel = labelNumber++;
    endLabel = labelNumber++;
    fprintf(fout, ".L%d:\n", whileLabel);
    codeGenExpr(t->children->data, classNumber, methodNumber);
    fprintf(fout, "    mov rax, [rsp]\n");
    fprintf(fout, "    cmp rax, 0\n");
    fprintf(fout, "    je .L%d\n", endLabel);
    incSP(); // Pop condition
    codeGenExprs(t->children->next->data, classNumber, methodNumber);
    fprintf(fout, "    jmp .L%d\n", whileLabel);
    fprintf(fout, ".L%d:\n", endLabel);
    incSP(); // Pop condition
    decSP(); // Loop result 0
    fprintf(fout, "    mov qword [rsp], 0\n");
  } break;

  case IF_THEN_ELSE_EXPR:
    codeGenExpr(t->children->data, classNumber, methodNumber);
    falseLabel = labelNumber++;
    endLabel = labelNumber++;
    fprintf(fout, "    mov rax, [rsp]\n");
    fprintf(fout, "    cmp rax, 0\n");
    fprintf(fout, "    je .L%d\n", falseLabel);
    incSP(); // Pop condition
    codeGenExprs(t->children->next->data, classNumber, methodNumber);
    fprintf(fout, "    jmp .L%d\n", endLabel);
    fprintf(fout, ".L%d:\n", falseLabel);
    incSP(); // Pop condition
    codeGenExprs(t->children->next->next->data, classNumber, methodNumber);
    fprintf(fout, ".L%d:\n", endLabel);
    break;

  case PLUS_EXPR:
    codeGenExpr(t->children->data, classNumber, methodNumber);
    codeGenExpr(t->children->next->data, classNumber, methodNumber);
    fprintf(fout, "    mov rax, [rsp + %d]\n", WORD_SIZE);
    fprintf(fout, "    mov rbx, [rsp]\n");
    fprintf(fout, "    add rax, rbx\n");
    incSP();
    fprintf(fout, "    mov [rsp], rax\n");
    break;

  case MINUS_EXPR:
    codeGenExpr(t->children->data, classNumber, methodNumber);
    codeGenExpr(t->children->next->data, classNumber, methodNumber);
    fprintf(fout, "    mov rax, [rsp + %d]\n", WORD_SIZE);
    fprintf(fout, "    mov rbx, [rsp]\n");
    fprintf(fout, "    sub rax, rbx\n");
    incSP();
    fprintf(fout, "    mov [rsp], rax\n");
    break;

  case TIMES_EXPR:
    codeGenExpr(t->children->data, classNumber, methodNumber);
    codeGenExpr(t->children->next->data, classNumber, methodNumber);
    fprintf(fout, "    mov rax, [rsp + %d]\n", WORD_SIZE);
    fprintf(fout, "    mov rbx, [rsp]\n");
    fprintf(fout, "    imul rax, rbx\n");
    incSP();
    fprintf(fout, "    mov [rsp], rax\n");
    break;

  case EQUALITY_EXPR:
    codeGenExpr(t->children->data, classNumber, methodNumber);
    codeGenExpr(t->children->next->data, classNumber, methodNumber);
    trueLabel = labelNumber++;
    endLabel = labelNumber++;
    fprintf(fout, "    mov rax, [rsp + %d]\n", WORD_SIZE);
    fprintf(fout, "    mov rbx, [rsp]\n");
    fprintf(fout, "    cmp rax, rbx\n");
    fprintf(fout, "    je .L%d\n", trueLabel);
    incSP();
    fprintf(fout, "    mov qword [rsp], 0\n");
    fprintf(fout, "    jmp .L%d\n", endLabel);
    fprintf(fout, ".L%d:\n", trueLabel);
    incSP();
    fprintf(fout, "    mov qword [rsp], 1\n");
    fprintf(fout, ".L%d:\n", endLabel);
    break;

  case LESS_THAN_EXPR:
    codeGenExpr(t->children->data, classNumber, methodNumber);
    codeGenExpr(t->children->next->data, classNumber, methodNumber);
    trueLabel = labelNumber++;
    endLabel = labelNumber++;
    fprintf(fout, "    mov rax, [rsp + %d]\n", WORD_SIZE);
    fprintf(fout, "    mov rbx, [rsp]\n");
    fprintf(fout, "    cmp rax, rbx\n");
    fprintf(fout, "    jl .L%d\n", trueLabel);
    incSP();
    fprintf(fout, "    mov qword [rsp], 0\n");
    fprintf(fout, "    jmp .L%d\n", endLabel);
    fprintf(fout, ".L%d:\n", trueLabel);
    incSP();
    fprintf(fout, "    mov qword [rsp], 1\n");
    fprintf(fout, ".L%d:\n", endLabel);
    break;

  case NOT_EXPR:
    codeGenExpr(t->children->data, classNumber, methodNumber);
    trueLabel = labelNumber++;
    endLabel = labelNumber++;
    fprintf(fout, "    mov rax, [rsp]\n");
    fprintf(fout, "    cmp rax, 0\n");
    fprintf(fout, "    je .L%d\n", trueLabel);
    fprintf(fout, "    mov qword [rsp], 0\n");
    fprintf(fout, "    jmp .L%d\n", endLabel);
    fprintf(fout, ".L%d:\n", trueLabel);
    fprintf(fout, "    mov qword [rsp], 1\n");
    fprintf(fout, ".L%d:\n", endLabel);
    break;

  case OR_EXPR:
    trueLabel = labelNumber++;
    endLabel = labelNumber++;
    codeGenExpr(t->children->data, classNumber, methodNumber);
    fprintf(fout, "    mov rax, [rsp]\n");
    fprintf(fout, "    cmp rax, 0\n");
    fprintf(fout, "    jne .L%d\n", trueLabel);
    incSP();
    codeGenExpr(t->children->next->data, classNumber, methodNumber);
    fprintf(fout, "    mov rax, [rsp]\n");
    fprintf(fout, "    cmp rax, 0\n");
    fprintf(fout, "    jne .L%d\n", trueLabel);
    fprintf(fout, "    jmp .L%d\n", endLabel);
    fprintf(fout, ".L%d:\n", trueLabel);
    fprintf(fout, "    mov qword [rsp], 1\n");
    fprintf(fout, ".L%d:\n", endLabel);
    break;

  case ASSERT_EXPR:
    trueLabel = labelNumber++;
    codeGenExpr(t->children->data, classNumber, methodNumber);
    fprintf(fout, "    mov rax, [rsp]\n");
    fprintf(fout, "    cmp rax, 0\n");
    fprintf(fout, "    jne .L%d\n", trueLabel);
    // Failure Case: Exit the program with error code
    fprintf(fout, "    mov rdi, 1\n");
    fprintf(fout, "    call _exit_program\n");
    // Success Case
    fprintf(fout, ".L%d:\n", trueLabel);
    break;

  case ASSIGN_EXPR:
    codeGenExpr(t->children->next->data, classNumber, methodNumber);
    idVal = t->children->data->idVal;
    int found = 0;
    if (classNumber > 0) {
      ClassDecl *class = &classesST[classNumber];
      MethodDecl *method = &class->methodList[methodNumber];

      // Method Param
      if (strCompare(idVal, method->paramName)) {
        found = 1;
        // Param is at [rbp + 8] based on stack layout derived
        fprintf(fout, "    mov rbx, rbp\n");
        fprintf(fout, "    add rbx, 8\n");
      }
      if (!found) {
        for (int i = 0; i < method->numLocals; i++) {
          if (strCompare(idVal, method->localST[i].varName)) {
            found = 1;
            // Local 0 is at [rbp - 8], Local 1 at [rbp - 16]
            fprintf(fout, "    mov rbx, rbp\n");
            fprintf(fout, "    sub rbx, %d\n", (i + 1) * WORD_SIZE);
          }
        }
      }
      if (!found) {
        offset = getFieldOffset(classNumber, idVal);
        // Field: Load 'this' from [rbp + 32], add offset
        fprintf(fout, "    mov rbx, [rbp + 32]\n"); // this
        fprintf(fout, "    add rbx, %d\n",
                (offset + 1) * WORD_SIZE); // +1 for TypeID
      }
    } else {
      for (int i = 0; i < numMainBlockLocals; i++) {
        if (strCompare(idVal, mainBlockST[i].varName)) {
          fprintf(fout, "    mov rbx, rbp\n");
          fprintf(fout, "    sub rbx, %d\n", (i + 1) * WORD_SIZE);
        }
      }
    }
    fprintf(fout, "    mov rax, [rsp]\n");
    fprintf(fout, "    mov [rbx], rax\n");
    break;

  case DOT_ASSIGN_EXPR:
    codeGenExpr(t->children->next->next->data, classNumber,
                methodNumber);                                 // Val
    codeGenExpr(t->children->data, classNumber, methodNumber); // Obj
    checkNullDereference();
    exprType = typeExpr(t->children->data, classNumber, methodNumber);
    fieldName = t->children->next->data->idVal;
    offset = getFieldOffset(exprType, fieldName);

    fprintf(fout, "    mov rbx, [rsp]\n"); // Obj
    fprintf(fout, "    add rbx, %d\n", (offset + 1) * WORD_SIZE);
    fprintf(fout, "    mov rax, [rsp + %d]\n", WORD_SIZE); // Val
    fprintf(fout, "    mov [rbx], rax\n");
    incSP(); // Pop Obj, leave Val
    break;

  case ID_EXPR:
    idVal = t->children->data->idVal;
    decSP();
    int foundID = 0;
    if (classNumber > 0) {
      ClassDecl *class = &classesST[classNumber];
      MethodDecl *method = &class->methodList[methodNumber];
      if (strCompare(idVal, method->paramName)) {
        foundID = 1;
        // Param at [rbp + 8]
        fprintf(fout, "    mov rax, [rbp + 8]\n");
      }
      if (!foundID) {
        for (int i = 0; i < method->numLocals; i++) {
          if (strCompare(idVal, method->localST[i].varName)) {
            foundID = 1;
            fprintf(fout, "    mov rax, [rbp - %d]\n", (i + 1) * WORD_SIZE);
          }
        }
      }
      if (!foundID) {
        int offset = getFieldOffset(classNumber, idVal);
        fprintf(fout, "    mov rax, [rbp + 32]\n"); // this
        fprintf(fout, "    mov rax, [rax + %d]\n", (offset + 1) * WORD_SIZE);
      }
    } else {
      for (int i = 0; i < numMainBlockLocals; i++) {
        if (strCompare(idVal, mainBlockST[i].varName)) {
          fprintf(fout, "    mov rax, [rbp - %d]\n", (i + 1) * WORD_SIZE);
        }
      }
    }
    fprintf(fout, "    mov [rsp], rax\n");
    break;

  case DOT_ID_EXPR:
    codeGenExpr(t->children->data, classNumber, methodNumber);
    checkNullDereference();
    exprType = typeExpr(t->children->data, classNumber, methodNumber);
    fieldName = t->children->next->data->idVal;
    offset = getFieldOffset(exprType, fieldName);
    fprintf(fout, "    mov rax, [rsp]\n");
    fprintf(fout, "    mov rax, [rax + %d]\n", (offset + 1) * WORD_SIZE);
    fprintf(fout, "    mov [rsp], rax\n");
    break;

  case METHOD_CALL_EXPR:
  case DOT_METHOD_CALL_EXPR:
    methodReturnAddr = labelNumber++;
    // 1. Push Return Address Label
    decSP();
    fprintf(fout, "    mov rax, .L_ret_%d\n", methodReturnAddr);
    fprintf(fout, "    mov [rsp], rax\n");

    // 2. Push 'this'
    if (t->typ == METHOD_CALL_EXPR) {
      decSP();
      fprintf(fout,
              "    mov rax, [rbp + 32]\n"); // Load 'this' from current context
      fprintf(fout, "    mov [rsp], rax\n");
    } else {
      codeGenExpr(t->children->data, classNumber, methodNumber);
      checkNullDereference();
    }

    // 3. Static Class
    decSP();
    fprintf(fout, "    mov qword [rsp], %d\n", t->staticClassNum);

    // 4. Static Method
    decSP();
    fprintf(fout, "    mov qword [rsp], %d\n", t->staticMemberNum);

    // 5. Arg
    ASTree *argExpr = (t->typ == METHOD_CALL_EXPR)
                          ? t->children->next->data
                          : t->children->next->next->data;
    codeGenExpr(argExpr, classNumber, methodNumber);

    fprintf(fout, "    jmp _VTable_Dispatch\n");
    fprintf(fout, ".L_ret_%d:\n", methodReturnAddr);
    break;

  default:
    internalCGerror("Unknown Expression Node on line %d", t->lineNumber);
  }
}

void codeGenExprs(ASTree *exprList, int classNumber, int methodNumber) {
  ASTList *expr = exprList->children;
  while (expr && expr->data) {
    codeGenExpr(expr->data, classNumber, methodNumber);
    if (expr->next != NULL) {
      incSP();
    }
    expr = expr->next;
  }
}

void genPrologue(int classNumber, int methodNumber) {
  // x86 Prologue
  fprintf(fout, "    push rbp\n");
  fprintf(fout, "    mov rbp, rsp\n");

  // Allocate space for locals
  int numLocals = classesST[classNumber].methodList[methodNumber].numLocals;
  for (int i = 0; i < numLocals; i++) {
    decSP();
    fprintf(fout, "    mov qword [rsp], 0\n");
  }
}

void genEpilogue(int classNumber, int methodNumber) {
  // Return value is currently at [rsp]
  fprintf(fout, "    mov rax, [rsp]\n"); // Pop result to RAX

  // Restore stack and RBP
  fprintf(fout, "    mov rsp, rbp\n");
  fprintf(fout, "    pop rbp\n");

  // Stack now has: [Arg] [SMethod] [SClass] [This] [RetAddr]
  // We need to jump to RetAddr, and we want to pop arguments?
  // Usually callee cleans up, or caller. In stack machine, let's keep it
  // simple: Just pop the return address and jump to it. The caller (L_ret_X)
  // will handle stack result? Actually, looking at METHOD_CALL_EXPR, we pushed
  // 5 things. We need to pop those 5 things and push result.

  // Get Return Address
  fprintf(fout, "    mov rbx, [rsp + 32]\n");

  // We are discarding the calling arguments here effectively
  // Move stack pointer past the 5 pushed items (5 * 8 = 40)
  fprintf(fout, "    add rsp, 40\n");

  // Push result back
  decSP();
  fprintf(fout, "    mov [rsp], rax\n");

  // Jump to return address
  fprintf(fout, "    jmp rbx\n");
}

void genBody(int classNumber, int methodNumber) {
  if (classNumber == 0)
    internalCGerror("Cannot generate body for Object class.");

  // Traverse AST to find body
  ASTList *classNode = wholeProgram->children->data->children;
  for (int i = 1; i < classNumber; i++)
    classNode = classNode->next;

  ASTList *methodList =
      classNode->data->children->next->next->next->data->children;

  for (int i = 0; i < methodNumber; i++)
    methodList = methodList->next;

  ASTList *runner = methodList->data->children;
  while (runner != NULL && runner->data->typ != EXPR_LIST) {
    runner = runner->next;
  }

  if (runner != NULL && runner->data->typ == EXPR_LIST) {
    codeGenExprs(runner->data, classNumber, methodNumber);
  } else {
    internalCGerror("Could not find EXPR_LIST (method body) in AST.");
  }
}

void addDynamicMethodInfo(int staticClass, int staticMethod, int dynamicType,
                          int dynamicClassToCall, int dynamicMethodToCall) {
  int nextRowLabel = labelNumber++;
  // Stack Entering VTable: [Arg] [SMethod] [SClass] [This] [RetAddr]
  // Offsets from RSP:
  // +0: Arg
  // +8: SMethod
  // +16: SClass
  // +24: This

  // Check Static Class
  fprintf(fout, "    mov rax, [rsp + 16]\n");
  fprintf(fout, "    cmp rax, %d\n", staticClass);
  fprintf(fout, "    jne .L%d\n", nextRowLabel);

  // Check Static Method
  fprintf(fout, "    mov rax, [rsp + 8]\n");
  fprintf(fout, "    cmp rax, %d\n", staticMethod);
  fprintf(fout, "    jne .L%d\n", nextRowLabel);

  // Check Dynamic Type (Load 'this' -> Load TypeID)
  fprintf(fout, "    mov rax, [rsp + 24]\n"); // Load 'this' ptr
  fprintf(fout, "    mov rax, [rax]\n");      // Load TypeID from [this]
  fprintf(fout, "    cmp rax, %d\n", dynamicType);
  fprintf(fout, "    jne .L%d\n", nextRowLabel);

  // Dispatch
  fprintf(fout, "    jmp class%dmethod%d\n", dynamicClassToCall,
          dynamicMethodToCall);

  fprintf(fout, ".L%d:\n", nextRowLabel);
}

void genVTable() {
  fprintf(fout, "_VTable_Dispatch:\n");
  int found, methodIndex;
  for (int i = 1; i < numClasses; i++) {
    ClassDecl *class = &classesST[i];
    for (int j = 0; j < class->numMethods; j++)
      addDynamicMethodInfo(i, j, i, i, j);
    int superclassType = class->superclass;
    while (superclassType > 0) {
      ClassDecl *superclass = &classesST[superclassType];
      for (int j = 0; j < superclass->numMethods; j++) {
        found = 0;
        methodIndex = 0;
        for (int k = 0; k < class->numMethods; k++) {
          if (strCompare(superclass->methodList[j].methodName,
                         class->methodList[k].methodName)) {
            found = 1;
            methodIndex = k;
            break;
          }
        }
        if (found)
          addDynamicMethodInfo(superclassType, j, i, i, methodIndex);
        else
          addDynamicMethodInfo(superclassType, j, i, superclassType, j);
      }
      superclassType = superclass->superclass;
    }
  }
  fprintf(fout, "    mov rdi, 44\n");
  fprintf(fout, "    call _exit_program\n");
}

int getFieldOffset(int objType, char *fieldName) {
  int offset = 0;
  int found = 0;
  int padding = 0;
  ClassDecl *currClass;
  int classType = objType;
  while (classType > 0) {
    currClass = &classesST[classType];
    for (int i = 0; i < currClass->numVars; i++) {
      if (found) {
        padding++;
      } else if (strCompare(fieldName, currClass->varList[i].varName)) {
        offset = i;
        found = 1;
        break;
      }
    }
    classType = currClass->superclass;
  }
  if (!found)
    internalCGerror("Couldn't get field offset.");
  return padding + offset;
}
```

## Integrating the code generator in our program

1. Include `"codegen.h"` in `%code provides {…}` in the **Declaration** section of our Bison file
    
    ```c
    %code provides {
      // ...
      #include "../include/codegen.h"
      // ...
    }
    ```
    
2. Setup symbol table and type check program in our `main` function in the **User Code** section using the following code snippet.
    
    ```c
    int main(int argc, char *argv[]) {
    	// ... (Your existing code) ...
      
      /* generate NASM code */
    	FILE *out = fopen("program.asm", "w");
    	if (out == NULL) {
    	    printf("ERROR: could not open program.asm for writing\n");
    	    exit(-1);
    	}
    	
    	generateNASM(out);
    	fclose(out);
      
      return 0;
    }
    ```
    

Our entire Bison file, `dj.y`, should now look like this:

```c
/* dj.y: PARSER for DJ */

%code provides {
  #include "lex.yy.c"
  #include "ast.h"
  #include "symtbl.h"
  #include "typecheck.h"
  #include "codegen.h"
    
  #define PARSE_DEBUG 1
  #define SYMBOLTABLE_DEBUG 1

  /* Symbols in this grammer are represented as ASTs */
  #define YYSTYPE ASTree *

  /* Declare global AST for entire program */
  ASTree *pgmAST;
 
  /* Function for printing generic syntax-error messages */
  void yyerror(const char *str) {
    printf("Syntax error on line %d at token %s\n",yylineno,yytext);
    printf("(This version of the compiler exits after finding the first ");
    printf("syntax error.)\n");
    exit(-1);
  }
}

%token FINAL CLASS ID EXTENDS MAIN NATTYPE 
%token NATLITERAL PRINTNAT READNAT PLUS MINUS TIMES EQUALITY LESS
%token ASSERT OR NOT IF ELSE WHILE
%token ASSIGN NUL NEW THIS DOT 
%token SEMICOLON LBRACE RBRACE LPAREN RPAREN
%token ENDOFFILE

%start pgm

%right ASSERT
%right ASSIGN
%left OR
%nonassoc LESS
%nonassoc EQUALITY
%right NOT
%left PLUS MINUS
%left TIMES
%right DOT

%%

pgm 
    : dj ENDOFFILE {
        pgmAST = $1;
        return 0;
    }
    ;

dj 
    : MAIN LBRACE expression_list RBRACE {
        $$ = newAST(PROGRAM, newAST(CLASS_DECL_LIST, NULL, 0, NULL, 0), 0, NULL, yylineno);
        appendToChildrenList($$, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList($$, $3);
    }
    | MAIN LBRACE variable_declaration_list expression_list RBRACE {
        $$ = newAST(PROGRAM, newAST(CLASS_DECL_LIST, NULL, 0, NULL, 0), 0, NULL, yylineno);
        appendToChildrenList($$, $3);
        appendToChildrenList($$, $4);
    }
    | class_list MAIN LBRACE expression_list RBRACE {
        $$ = newAST(PROGRAM, $1, 0, NULL, yylineno);
        appendToChildrenList($$, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList($$, $4);
    }
    | class_list MAIN LBRACE variable_declaration_list expression_list RBRACE {
        $$ = newAST(PROGRAM, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $4);
        appendToChildrenList($$, $5);
    }
    ;

class_list 
    : class_list class {
        $$ = $1;
        appendToChildrenList($$, $2);
    }
    | class {
        $$ = newAST(CLASS_DECL_LIST, $1, 0, NULL, yylineno);
    }
    ;

class
    : CLASS identifier EXTENDS identifier LBRACE RBRACE {
        $$ = newAST(NONFINAL_CLASS_DECL, $2, 0, NULL, yylineno);
        appendToChildrenList($$, $4);
        appendToChildrenList($$, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList($$, newAST(METHOD_DECL_LIST, NULL, 0, NULL, 0));
    }
    | CLASS identifier EXTENDS identifier LBRACE variable_declaration_list RBRACE {
        $$ = newAST(NONFINAL_CLASS_DECL, $2, 0, NULL, yylineno);
        appendToChildrenList($$, $4);
        appendToChildrenList($$, $6);
        appendToChildrenList($$, newAST(METHOD_DECL_LIST, NULL, 0, NULL, 0));
    }
    | CLASS identifier EXTENDS identifier LBRACE method_list RBRACE {
        $$ = newAST(NONFINAL_CLASS_DECL, $2, 0, NULL, yylineno);
        appendToChildrenList($$, $4);
        appendToChildrenList($$, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList($$, $6);
    }
    | CLASS identifier EXTENDS identifier LBRACE variable_declaration_list method_list RBRACE {
        $$ = newAST(NONFINAL_CLASS_DECL, $2, 0, NULL, yylineno);
        appendToChildrenList($$, $4);
        appendToChildrenList($$, $6);
        appendToChildrenList($$, $7);
    }
    | FINAL CLASS identifier EXTENDS identifier LBRACE RBRACE {
        $$ = newAST(FINAL_CLASS_DECL, $3, 0, NULL, yylineno);
        appendToChildrenList($$, $5);
        appendToChildrenList($$, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList($$, newAST(METHOD_DECL_LIST, NULL, 0, NULL, 0));
    }
    | FINAL CLASS identifier EXTENDS identifier LBRACE variable_declaration_list RBRACE {
        $$ = newAST(FINAL_CLASS_DECL, $3, 0, NULL, yylineno);
        appendToChildrenList($$, $5);
        appendToChildrenList($$, $7);
        appendToChildrenList($$, newAST(METHOD_DECL_LIST, NULL, 0, NULL, 0));
    }
    | FINAL CLASS identifier EXTENDS identifier LBRACE method_list RBRACE {
        $$ = newAST(FINAL_CLASS_DECL, $3, 0, NULL, yylineno);
        appendToChildrenList($$, $5);
        appendToChildrenList($$, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList($$, $7);   
    }
    | FINAL CLASS identifier EXTENDS identifier LBRACE variable_declaration_list method_list RBRACE {
        $$ = newAST(FINAL_CLASS_DECL, $3, 0, NULL, yylineno);
        appendToChildrenList($$, $5);
        appendToChildrenList($$, $7);
        appendToChildrenList($$, $8);
    }
    ;

method_list 
    : method_list method {
        $$ = $1;
        appendToChildrenList($$, $2);
    }
    | method {
        $$ = newAST(METHOD_DECL_LIST, $1, 0, NULL, yylineno);
    }
    ;

method
    : data_type identifier LPAREN data_type identifier RPAREN LBRACE expression_list RBRACE {
        $$ = newAST(NONFINAL_METHOD_DECL, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $2);
        appendToChildrenList($$, $4);
        appendToChildrenList($$, $5);
        appendToChildrenList($$, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList($$, $8);
    }
    | data_type identifier LPAREN data_type identifier RPAREN LBRACE variable_declaration_list expression_list RBRACE {
        $$ = newAST(NONFINAL_METHOD_DECL, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $2);
        appendToChildrenList($$, $4);
        appendToChildrenList($$, $5);
        appendToChildrenList($$, $8);
        appendToChildrenList($$, $9);
    }
    | FINAL data_type identifier LPAREN data_type identifier RPAREN LBRACE expression_list RBRACE {
        $$ = newAST(FINAL_METHOD_DECL, $2, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
        appendToChildrenList($$, $5);
        appendToChildrenList($$, $6);
        appendToChildrenList($$, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList($$, $9);
    }
    | FINAL data_type identifier LPAREN data_type identifier RPAREN LBRACE variable_declaration_list expression_list RBRACE {
        $$ = newAST(FINAL_METHOD_DECL, $2, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
        appendToChildrenList($$, $5);
        appendToChildrenList($$, $6);
        appendToChildrenList($$, $9);
        appendToChildrenList($$, $10);
    }
    ;

variable_declaration_list 
    : variable_declaration_list variable_declaration SEMICOLON {
        $$ = $1;
        appendToChildrenList($$, $2);
    }
    | variable_declaration SEMICOLON {
        $$ = newAST(VAR_DECL_LIST, $1, 0, NULL, yylineno);
    }
    ;

variable_declaration 
    : data_type identifier {
        $$ = newAST(VAR_DECL, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $2);
    }
    ;

expression_list 
    : expression_list expression SEMICOLON {
        $$ = $1;
        appendToChildrenList($$, $2);
    }
    | expression SEMICOLON {
        $$ = newAST(EXPR_LIST, $1, 0, NULL, yylineno);
    }
    ;

expression 
    : NUL { 
        $$ = newAST(NULL_EXPR, NULL, 0, NULL, yylineno);
    }
    | NATLITERAL { 
        $$ = newAST(NAT_LITERAL_EXPR, NULL, atoi(yytext), NULL, yylineno);
    }
    | identifier { 
        $$ = newAST(ID_EXPR, $1, 0, NULL, yylineno);
    }
    | THIS { 
        $$ = newAST(THIS_EXPR, NULL, 0, NULL, yylineno); 
    }
    | identifier LPAREN expression RPAREN { 
        $$ = newAST(METHOD_CALL_EXPR, $1, 0, NULL, yylineno); 
        appendToChildrenList($$, $3); 
    }
    | NEW identifier LPAREN RPAREN { 
        $$ = newAST(NEW_EXPR, $2, 0, NULL, yylineno); 
    }
    | LPAREN expression RPAREN { 
        $$ = $2;
    }
    | expression DOT identifier {
        $$ = newAST(DOT_ID_EXPR, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
    }
    | expression DOT identifier LPAREN expression RPAREN {
        $$ = newAST(DOT_METHOD_CALL_EXPR, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
        appendToChildrenList($$, $5);
    }
    | expression PLUS expression {
        $$ = newAST(PLUS_EXPR, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
    }
    | expression MINUS expression {
        $$ = newAST(MINUS_EXPR, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
    }
    | expression TIMES expression {
        $$ = newAST(TIMES_EXPR, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
    }
    | expression EQUALITY expression {
        $$ = newAST(EQUALITY_EXPR, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
    }
    | expression LESS expression {
        $$ = newAST(LESS_THAN_EXPR, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
    }
    | NOT expression {
        $$ = newAST(NOT_EXPR, $2, 0, NULL, yylineno);
    }
    | expression OR expression {
        $$ = newAST(OR_EXPR, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
    }
    | identifier ASSIGN expression {
        $$ = newAST(ASSIGN_EXPR, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
    }
    | expression DOT identifier ASSIGN expression {
        $$ = newAST(DOT_ASSIGN_EXPR, $1, 0, NULL, yylineno);
        appendToChildrenList($$, $3);
        appendToChildrenList($$, $5);
    }
    | IF LPAREN expression RPAREN LBRACE expression_list RBRACE ELSE LBRACE expression_list RBRACE {
        $$ = newAST(IF_THEN_ELSE_EXPR, $3, 0, NULL, yylineno);
        appendToChildrenList($$, $6);
        appendToChildrenList($$, $10);
    }
    | WHILE LPAREN expression RPAREN LBRACE expression_list RBRACE {
        $$ = newAST(WHILE_EXPR, $3, 0, NULL, yylineno);
        appendToChildrenList($$, $6);
    }
    | ASSERT expression {
        $$ = newAST(ASSERT_EXPR, $2, 0, NULL, yylineno);
    }
    | PRINTNAT LPAREN expression RPAREN {
        $$ = newAST(PRINT_EXPR, $3, 0, NULL, yylineno);
    }
    | READNAT LPAREN RPAREN {
        $$ = newAST(READ_EXPR, NULL, 0, NULL, yylineno);
    }
    ;

data_type
    : NATTYPE {
        $$ = newAST(NAT_TYPE, NULL, 0, NULL, yylineno);
    }
    | identifier {
        $$ = $1;
    }
    ;

identifier
    : ID {
        $$ = newAST(AST_ID, NULL, 0, getID(yytext), yylineno);
    }
    ;

%%

int main(int argc, char *argv[]) {
  if(argc!=2) {
    printf("Usage: parsedj filename\n");
    exit(-1);
  }
  
  // open the input file
  yyin = fopen(argv[1],"r");
  if(yyin==NULL) {
    printf("ERROR: could not open file %s\n",argv[1]);
    exit(-1);
  }
  
  /* parse the input program */
  yyparse();
  
  if (PARSE_DEBUG) {
    printf("****** begin abstract syntax tree for DJ program ******\n");
    printAST(pgmAST);
    printf("****** end abstract syntax tree for DJ program ******\n");
  }
  
  /* set up symbol tables */
  setupSymbolTables(pgmAST);
  if (SYMBOLTABLE_DEBUG) {
    printf("****** begin classes symbol table for DJ program ******\n");
    for (int i = 0; i < numClasses; i++)
    {
        printf("Class: %s; Super: %d; isFinal: %d\n", classesST[i].className, classesST[i].superclass, classesST[i].isFinal);
        for (int j = 0; j < classesST[i].numVars; j++)
            printf("   FieldType: %d; FieldName: %s\n", classesST[i].varList[j].type, classesST[i].varList[j].varName); 
        for (int j = 0; j < classesST[i].numMethods; j++)
           printf("   MethodName: %s; ReturnType: %d; ParamName: %s; ParamType: %d; isFinal: %d\n", classesST[i].methodList[j].methodName, classesST[i].methodList[j].returnType, classesST[i].methodList[j].paramName, classesST[i].methodList[j].paramType, classesST[i].methodList[j].isFinal); 
    }
    for (int i = 0; i < numMainBlockLocals; i++)
        printf("MainLocalType: %d; MainLocalName: %s\n", mainBlockST[i].type, mainBlockST[i].varName); 
    printf("****** end classes symbol table for DJ program ******\n");
  }

	/* typecheck the input program */
  typecheckProgram();
  
  /* generate NASM code */
	FILE *out = fopen("program.asm", "w");
	if (out == NULL) {
	    printf("ERROR: could not open program.asm for writing\n");
	    exit(-1);
	}
	
	generateNASM(out);
	fclose(out);
  
  return 0;
}
```

### Compilation

Make and run our compiler program using the provided `sh` script in our root project direction. At this point, I recommend setting the `DEBUG_SCAN`, `DEBUG_AST`, and `DEBUG_SYMTBL` macros to `0` in the Flex file, and Bison file. However, feel free to turn them on for debugging when building out the compiler further.

Now running the program, shouldn’t give us an output in the terminal but instead it should create a file called `program.asm` in our project root directory. We have successfully compiled the source code.

```bash
**…/DjCompiler ❯** ./dj.sh tests/good/good3.dj
**…/DjCompiler ❯** ls
Permissions Size User   Date Modified Name
drwxr-xr-x     - groove  9 Jan 14:56   bin
drwxr-xr-x     - groove  9 Jan 14:56  󱧼 build
drwxr-xr-x     - groove  9 Jan 14:46   include
drwxr-xr-x     - groove  9 Jan 14:56  󰣞 src
drwxr-xr-x     - groove  9 Jan 13:21   test
.rwxr-xr-x    89 groove  9 Jan 13:21   dj.sh
.rw-r--r--  3.5k groove  9 Jan 13:21   Makefile
.rw-r--r--  1.6k groove  9 Jan 14:56   program.dism
```

### Running tests

Running tests with code generation on is not recommended. Things may break. To run lex, syntax, and semantic tests, please comment out the `generateNASM(out)` in the main function and use `make run`. To run code generation tests, please uncomment the line (if previously commented) and test files one at the time using the `.sh` script, like the example above.
