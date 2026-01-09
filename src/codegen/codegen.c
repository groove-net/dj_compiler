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
