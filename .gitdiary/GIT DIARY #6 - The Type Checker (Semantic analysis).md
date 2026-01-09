The semantics of a program are its "meaning," as opposed to its syntax, or structure. The semantics of a program determine its runtime behavior, but most programming languages have features that can be determined prior to execution and yet cannot be conveniently expressed as syntax and analyzed by the parser. Such features are referred to as **static semantics**, and the analysis of such semantics is the task of the semantic analyzer. (The "dynamic" semantics of a program—those properties of a program that can only be determined by executing it—cannot be determined by a compiler, since it does not execute the program.)

Typical static semantic features of common programming languages include declarations and type checking. The extra pieces of information (such as data types) computed by the semantic analyzer are called **attributes**, and these are often added to the tree as annotations, or "decorations." (Attributes may also be entered into the symbol table.)

In our running example of the C expression

```c
a[index] = 4 + 2
```

typical type information that would be gathered prior to the analysis of this line might be that **a** is an array of integer values with subscripts from a subrange of the integers and that **index** is an integer variable. Then the semantic analyzer would annotate the syntax tree with the types of all the subexpressions and then check that the assignment makes sense for these types, declaring a type mismatch error if not. In our example, all the types make sense, and the result of semantic analysis on the syntax tree could be represented by the following annotated tree:

![image.png](attachment:23ce624d-6d09-435a-9f92-a934632342ec:image.png)

## String methods

Before we continue, we are gonna need a couple string methods which we will use for the next for stages.

In the `includes/` folder, create a file called `strmethods.h`:

```
📂 DjCompiler/
├── 📂 includes/
│   ├── 📄 strmethods.h
│   └── ...
└── ...
```

`strmethods.h`:

```c
/* File strmethods.h: Basic functions for working with strings */

#ifndef STRMETHODS_H
#define STRMETHODS_H

int strCompare(const char *s1, const char *s2);
char *strConcat(const char *first, ...);

#endif
```

Then, In the `src/` folder, create a folder called `strmethods/` and within that folder create a file called `strmethods.c`:

```
📂 DjCompiler/
├── 📂 src/
│   ├── 📂 strmethods/
│   │   └── 📄 strmethods.c
│   └── ...
└── ...
```

`strmethods.c`:

```c
#include "../../include/strmethods.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* Returns true(1) if strings are identical */
int strCompare(const char *s1, const char *s2) {
  while (*s1)
    if (*s1++ != *s2++)
      return 0;
  if (*s2)
    return 0;
  else
    return 1;
}

/* Returns pointer to the concatenated string */
char *strConcat(const char *first, ...) {
  if (first == NULL)
    return NULL;

  va_list args;
  size_t total_len = strlen(first);

  // First pass: calculate total length
  va_start(args, first);
  const char *s;
  while ((s = va_arg(args, const char *)) != NULL) {
    total_len += strlen(s);
  }
  va_end(args);

  // Allocate result buffer
  char *result = malloc(total_len + 1); // +1 for null terminator
  if (result == NULL)
    return NULL;

  // Second pass: copy strings
  strcpy(result, first);
  va_start(args, first);
  while ((s = va_arg(args, const char *)) != NULL) {
    strcat(result, s);
  }
  va_end(args);

  return result;
}
```

## Symbol Table

Now before, we can do any serious type checking, we are going to need a symbol table.

A symbol table is a data structure used by a compiler to store and manage information about identifiers (Variable,s methods, classes) encountered during the parsing and semantic analysis phases of compilation. The role of the symbol table in type checking is to ensure the correct types are assigned to variables and functions and to keep track of scope (global, local, data) as well as store I`D` and `NAT` values where application.

The symbol table phase is responsible for extracting *all declarative structure* from the AST and normalizing it into a set of indexed, queryable data structures. Its primary purpose is to decouple semantic analysis from syntactic structure. After symbol-table construction completes, the typechecker no longer needs to traverse declaration subtrees in the AST; instead, it operates against compact, preprocessed tables that represent the program’s namespace and type universe.

A key design decision in this compiler is to represent **types as integers** rather than as structured objects. This choice simplifies comparisons, enables constant-time equality checks, and allows subtype relationships to be resolved via table traversal rather than pointer graphs. All user-defined classes are assigned consecutive positive integers, while built-in and sentinel types occupy reserved negative values. This representation enforces a clean separation between syntactic identifiers and semantic types.

The symbol table construction proceeds in two logical passes. In the first pass, the compiler counts and registers all classes, ensuring that every class name is assigned a stable index. This pass also constructs a placeholder entry for the built-in `Object` class, which anchors the inheritance hierarchy. Importantly, no semantic validation occurs during this phase; undefined types and duplicate names are permitted to exist temporarily. This allows the compiler to build a complete global view of the program before enforcing correctness constraints.

In the second pass, each class entry is populated with its full semantic content. This includes superclass links, field declarations, method declarations, method parameters, and local variables. Each declaration records both its type and the source line on which it appears, enabling precise error reporting during later phases. Method bodies are retained as AST subtrees rather than flattened, reflecting the fact that executable semantics are still expressed structurally rather than symbolically.

The result of symbol table construction is a **fully indexed semantic model** of the program: a global array of classes, each containing arrays of fields and methods, each method containing its own local scope. In addition, the main block is treated as a pseudo-scope with its own local symbol table and expression list. This uniform treatment of scopes simplifies downstream analysis.

### Implementation

In the `includes/` folder, create a file called `symtbl.h`:

```
📂 DjCompiler/
├── 📂 includes/
│   ├── 📄 symtbl.h
│   └── ...
└── ...
```

`symtbl.h`:

```c
/* File symtbl.h: Enhanced symbol-table data structures for DJ */

#ifndef SYMTBL_H
#define SYMTBL_H

#include "ast.h"
#include "strmethods.h"

/* METHOD TO SET UP GLOBALS (INCLUDING SYMBOL TABLES) */
/* Using the parameter AST representing the whole program, allocate
   and build the global symbol-table data.

   NOTE on typing conventions:
   This compiler represents types as integers.
   The DJ type denoted by int i is:
     The ith class declared in the source program, if i > 0
     Object, if i = 0
     nat, if i = -1
     "any-object" (i.e., the type of "null"), if i = -2
     "no-type" (i.e., an illegal type, or Object's superclass), if i = -3
   When i>=0, the symbol table for type (i.e., class) i is
   at classesST[i].

   NOTE on typechecking:
   This method does NOT perform any checks on the symbol tables
   it builds, although the entry for the Object class
   (in classesST[0]) will of course be free of errors.
   E.g., variable/method/class names may appear multiple times
   in the tables, and types may be invalid.
   If the DJ program declares a variable of an undefined type,
   that type will appear as -3 in the symbol table. */
void setupSymbolTables(ASTree *fullProgramAST);

/* HELPER METHOD TO CONVERT CLASS NAMES TO NUMBERS */
/* Returns the number for a given class name.
   Returns: 0 for Object,
     1 for first class declared in program,
     2 for 2nd class declared in program, etc.,
     and -3 for a nonexistent class name.
   Determines number for the given class using the global
   variable wholeProgram (defined below).  */
int classNameToNumber(char *className);

/* TYPEDEFS FOR ENHANCED SYMBOL TABLES */
/* Encapsulate all information relevant to a DJ variable:
   the variable name, source-program line number on which the variable is
   declared, variable type, and line number on which the type appears. */
typedef struct vdecls {
  char *varName;
  int varNameLineNumber;
  int type;
  int typeLineNumber;
} VarDecl;

/* Encapsulate all information relevant to a DJ method:
   the method name, return type, parameter name, parameter type,
   flag to indicate whether the method is declared "final",
   local variables, and method body. */
typedef struct mdecls {
  char *methodName;
  int methodNameLineNumber;
  int returnType;
  int returnTypeLineNumber;
  char *paramName;
  int paramNameLineNumber;
  int paramType;
  int paramTypeLineNumber;
  int isFinal;

  // An array of this method's local variables
  int numLocals;    // size of the array
  VarDecl *localST; // the array itself

  // The method's executable body
  ASTree *bodyExprs;
} MethodDecl;

/* Encapsulate all information relevant to a DJ class:
   the class name, superclass, flag to indicate whether the class is "final",
   and arrays of information about the class's variables and methods. */
typedef struct classinfo {
  char *className;
  int classNameLineNumber;
  int superclass;
  int superclassLineNumber;
  int isFinal;

  // array of variable information--the ith element of
  // the array encapsulates information about the ith
  // variable field in this class
  int numVars;      // size of the array
  VarDecl *varList; // the array itself

  // array of method information--the ith element of the array
  // encapsulates information about the ith method in this class
  int numMethods;         // size of the array
  MethodDecl *methodList; // the array itself
} ClassDecl;

/* GLOBALS THAT PROVIDE EASY ACCESS TO PARTS OF THE AST */
/* THESE GLOBALS GET SET IN setupSymbolTables */
// The entire program's AST
extern ASTree *wholeProgram;

// The expression list in the main block of the DJ program
extern ASTree *mainExprs;

// Array (symbol table) of locals in the main block
extern int numMainBlockLocals; // size of the array
extern VarDecl *mainBlockST;   // the array itself

// Array (symbol table) of class declarations
// Note that the minimum array size is 1,
//   due to the always-present Object class
extern int numClasses;       // size of the array
extern ClassDecl *classesST; // the array itself

#endif
```

Then, In the `src/` folder, create a folder called `symtbl/` and within that folder create a file called `symtbl.c`:

```
📂 DjCompiler/
├── 📂 src/
│   ├── 📂 symtbl/
│   │   └── 📄 symtbl.c
│   └── ...
└── ...
```

`symtbl.c`:

```c
#include "../../include/symtbl.h"

ASTree *wholeProgram;
int numClasses;
ClassDecl *classesST;
int numMainBlockLocals;
VarDecl *mainBlockST;
ASTree *mainExprs;

/* METHOD TO SET UP GLOBALS (INCLUDING SYMBOL TABLES) */
/* Using the parameter AST representing the whole program, allocate
   and build the global symbol-table data.

   NOTE on typing conventions:
   This compiler represents types as integers.
   The DJ type denoted by int i is:
     The ith class declared in the source program, if i > 0
     Object, if i = 0
     nat, if i = -1
     "any-object" (i.e., the type of "null"), if i = -2
     "no-type" (i.e., an illegal type, or Object's superclass), if i = -3
   When i>=0, the symbol table for type (i.e., class) i is
   at classesST[i].

   NOTE on typechecking:
   This method does NOT perform any checks on the symbol tables
   it builds, although the entry for the Object class
   (in classesST[0]) will of course be free of errors.
   E.g., variable/method/class names may appear multiple times
   in the tables, and types may be invalid.
   If the DJ program declares a variable of an undefined type,
   that type will appear as -3 in the symbol table. */
void setupSymbolTables(ASTree *fullProgramAST) {
  // Set Program Root Node
  wholeProgram = fullProgramAST;

  // Set Number of Classes
  numClasses = 1;
  ASTList *currClass = fullProgramAST->children->data->children;
  while (currClass && currClass->data) {
    numClasses++;
    currClass = currClass->next;
  }

  // Partial Class Symbol Table
  classesST = (ClassDecl *)malloc(sizeof(ClassDecl) * numClasses);
  // Setup Object Class
  classesST[0].className = "Object";
  classesST[0].superclass = -4;
  classesST[0].isFinal = 0;
  // Setup User Classes
  int classIdx = 1;
  currClass = fullProgramAST->children->data->children;
  while (currClass && currClass->data) {
    // className
    classesST[classIdx].className = currClass->data->children->data->idVal;
    // classNameLineNumber
    classesST[classIdx].classNameLineNumber =
        currClass->data->children->data->lineNumber;
    // superclassLineNumber
    classesST[classIdx].superclassLineNumber =
        currClass->data->children->next->data->lineNumber;
    // isFinal
    if (currClass->data->typ == NONFINAL_CLASS_DECL)
      classesST[classIdx].isFinal = 0;
    else if (currClass->data->typ == FINAL_CLASS_DECL)
      classesST[classIdx].isFinal = 1;
    // numVars
    int numFields = 0;
    ASTList *currField = currClass->data->children->next->next->data->children;
    while (currField && currField->data) {
      numFields++;
      currField = currField->next;
    }
    classesST[classIdx].numVars = numFields;
    // numMethods
    int numMethods = 0;
    ASTList *currMethod =
        currClass->data->children->next->next->next->data->children;
    while (currMethod && currMethod->data) {
      numMethods++;
      currMethod = currMethod->next;
    }
    classesST[classIdx].numMethods = numMethods;

    classIdx++;
    currClass = currClass->next;
  }

  // Complete Class Symbol Table
  classIdx = 1;
  currClass = fullProgramAST->children->data->children;
  while (currClass && currClass->data) {
    // superclass
    classesST[classIdx].superclass =
        classNameToNumber(currClass->data->children->next->data->idVal);
    // varList
    classesST[classIdx].varList =
        (VarDecl *)malloc(sizeof(VarDecl) * classesST[classIdx].numVars);
    int fieldIdx = 0;
    ASTList *currField = currClass->data->children->next->next->data->children;
    while (currField && currField->data) {
      // varName
      classesST[classIdx].varList[fieldIdx].varName =
          currField->data->children->next->data->idVal;
      // varNameLineNumber
      classesST[classIdx].varList[fieldIdx].varNameLineNumber =
          currField->data->children->next->data->lineNumber;
      // type
      if (currField->data->children->data->typ == NAT_TYPE)
        classesST[classIdx].varList[fieldIdx].type = -1;
      else if (currField->data->children->data->typ == AST_ID)
        classesST[classIdx].varList[fieldIdx].type =
            classNameToNumber(currField->data->children->data->idVal);
      // typeLineNumber
      classesST[classIdx].varList[fieldIdx].typeLineNumber =
          currField->data->children->data->lineNumber;

      fieldIdx++;
      currField = currField->next;
    }
    // methodList
    classesST[classIdx].methodList = (MethodDecl *)malloc(
        sizeof(MethodDecl) * classesST[classIdx].numMethods);
    int methodIdx = 0;
    ASTList *currMethod =
        currClass->data->children->next->next->next->data->children;
    while (currMethod && currMethod->data) {
      // methodName
      classesST[classIdx].methodList[methodIdx].methodName =
          currMethod->data->children->next->data->idVal;
      // methodNameLineNumber
      classesST[classIdx].methodList[methodIdx].methodNameLineNumber =
          currMethod->data->children->next->data->lineNumber;
      // returnType
      if (currMethod->data->children->data->typ == NAT_TYPE)
        classesST[classIdx].methodList[methodIdx].returnType = -1;
      else if (currMethod->data->children->data->typ == AST_ID)
        classesST[classIdx].methodList[methodIdx].returnType =
            classNameToNumber(currMethod->data->children->data->idVal);
      // returnTypeLineNumber
      classesST[classIdx].methodList[methodIdx].returnTypeLineNumber =
          currMethod->data->children->data->lineNumber;
      // paramName
      classesST[classIdx].methodList[methodIdx].paramName =
          currMethod->data->children->next->next->next->data->idVal;
      // paramNameLineNumber
      classesST[classIdx].methodList[methodIdx].paramNameLineNumber =
          currMethod->data->children->next->next->next->data->lineNumber;
      // paramType
      if (currMethod->data->children->next->next->data->typ == NAT_TYPE)
        classesST[classIdx].methodList[methodIdx].paramType = -1;
      else if (currMethod->data->children->next->next->data->typ == AST_ID)
        classesST[classIdx].methodList[methodIdx].paramType = classNameToNumber(
            currMethod->data->children->next->next->data->idVal);
      // paramTypeLineNumber
      classesST[classIdx].methodList[methodIdx].paramTypeLineNumber =
          currMethod->data->children->next->next->data->lineNumber;
      // isFinal
      if (currMethod->data->typ == NONFINAL_METHOD_DECL)
        classesST[classIdx].methodList[methodIdx].isFinal = 0;
      else if (currMethod->data->typ == FINAL_METHOD_DECL)
        classesST[classIdx].methodList[methodIdx].isFinal = 1;
      // numLocals
      int numLocals = 0;
      ASTList *currLocal =
          currMethod->data->children->next->next->next->next->data->children;
      while (currLocal && currLocal->data) {
        numLocals++;
        currLocal = currLocal->next;
      }
      classesST[classIdx].methodList[methodIdx].numLocals = numLocals;
      // localST
      classesST[classIdx].methodList[methodIdx].localST =
          (VarDecl *)malloc(sizeof(VarDecl) * numLocals);
      int localIdx = 0;
      currLocal =
          currMethod->data->children->next->next->next->next->data->children;
      while (currLocal && currLocal->data) {
        classesST[classIdx].methodList[methodIdx].localST[localIdx].varName =
            currLocal->data->children->next->data->idVal;
        classesST[classIdx]
            .methodList[methodIdx]
            .localST[localIdx]
            .varNameLineNumber =
            currLocal->data->children->next->data->lineNumber;

        if (currLocal->data->children->data->typ == NAT_TYPE)
          classesST[classIdx].methodList[methodIdx].localST[localIdx].type = -1;
        else if (currLocal->data->children->data->typ == AST_ID)
          classesST[classIdx].methodList[methodIdx].localST[localIdx].type =
              classNameToNumber(currLocal->data->children->data->idVal);

        classesST[classIdx]
            .methodList[methodIdx]
            .localST[localIdx]
            .typeLineNumber = currLocal->data->children->data->lineNumber;

        localIdx++;
        currLocal = currLocal->next;
      }
      // bodyExprs
      classesST[classIdx].methodList[methodIdx].bodyExprs =
          currMethod->data->children->next->next->next->next->next->data;

      methodIdx++;
      currMethod = currMethod->next;
    }

    classIdx++;
    currClass = currClass->next;
  }

  // Set Number of Main Block Locals
  numMainBlockLocals = 0;
  ASTList *currLocal = fullProgramAST->children->next->data->children;
  while (currLocal && currLocal->data) {
    numMainBlockLocals++;
    currLocal = currLocal->next;
  }

  // Set Main Block Symbol Table
  mainBlockST = (VarDecl *)malloc(sizeof(VarDecl) * numMainBlockLocals);
  int localIdx = 0;
  currLocal = fullProgramAST->children->next->data->children;
  while (currLocal && currLocal->data) {
    // varName
    mainBlockST[localIdx].varName =
        currLocal->data->children->next->data->idVal;
    // varNameLineNumber
    mainBlockST[localIdx].varNameLineNumber =
        currLocal->data->children->next->data->lineNumber;
    // type
    if (currLocal->data->children->data->typ == NAT_TYPE)
      mainBlockST[localIdx].type = -1;
    else if (currLocal->data->children->data->typ == AST_ID)
      mainBlockST[localIdx].type =
          classNameToNumber(currLocal->data->children->data->idVal);
    // typeLineNumber
    mainBlockST[localIdx].typeLineNumber =
        currLocal->data->children->data->lineNumber;

    localIdx++;
    currLocal = currLocal->next;
  }

  // Set Main Block Expression List
  mainExprs = fullProgramAST->children->next->next->data;
}

/* HELPER METHOD TO CONVERT CLASS NAMES TO NUMBERS */
/* Returns the number for a given class name.
   Returns: 0 for Object,
     1 for first class declared in program,
     2 for 2nd class declared in program, etc.,
     and -4 for a nonexistent class name.
   Determines number for the given class using the global
   variable wholeProgram (defined below).  */
int classNameToNumber(char *className) {
  // Search for class
  for (int i = 0; i < numClasses; i++)
    if (strCompare(className, classesST[i].className))
      return i;

  // Class Not Found
  return -3;
}
```

## Typechecker

The typechecker is organized as a top-down semantic verifier that operates over the symbol tables and expression ASTs. Its entry point performs global checks first, followed by progressively more localized checks. This mirrors the structure of the language itself: class hierarchies constrain methods, methods constrain expressions, and expressions constrain values.

The first global check ensures that the class inheritance graph is acyclic. Because classes are indexed numerically and superclasses are stored as integers, cycle detection reduces to bounded upward traversal rather than graph algorithms. Any cycle detected at this stage invalidates the entire program, as it undermines subtype reasoning everywhere else.

Once inheritance validity is established, each class is checked independently. Class-level checks enforce constraints such as valid superclass references, prohibition of inheritance from final classes, and uniqueness of class names. These checks rely exclusively on the symbol table and do not inspect method bodies.

### Type Checking DJ Programs

1. Make sure class names are unique
2. Check that all types in classes symbol table and main block symbol table are valid
    1. For everything except Object superclass type ≥ 0
    2. class type is not superclass type
    3. superclass not marked final
    4. class field types ≥ -1 (aka NAT)
    5. all method local variables types ≥ -1
    6. method return & parameter types ≥ -1
3. Check that the class hierarchy is indeed acyclical. To to this walk up to the top (`Object`) with `numOfClasses` steps for each class, if you can reach the top (`Object`) from each class, there are no cycles. Alternatively, using a `isSubType(class A, class B)` function, which returns true if class A is a subtype of class B, on each class to check if a class if a sub type of itself by passing the same class as both arguments is another method. If this check return true for any class a cycle exists.
4. Check that all field names are unique in their classes and superclasses.
5. For all declared methods:
    1. check that methods name is unique in its defining class
    2. check that methods in class with the same method as a method in a superclass have the same signature and not final
    3. check that method parameters & local variables names have no duplicates
    4. check that method expression list body is well typed and should be a subtype of the method’s declared return type.
6. check that main block variable names have no dependencies
7. check that main block expression list is well typed (≥ -2)
8. Each expression in our AST needs to be type checked and resolved into a single type. We assign types numbers. In DJ, `NULL` is -2, `NATLITERAL` is -1, `Object` is 0, and each user defined class type is numbered 1 … n. Example of expression type checking is, e.g, checking that in the `E` in a `NOT_EXPR` (`!E`) resolves to a type of -1 (`NATLITERAL`) or checking that `E1` and `E2` in a `PLUS_EXPR` (`E1 + E2`) both resolve to a type of -1 (`NATLITERAL`). Study the `typeExpr` and `typeExprs` functions in *typecheck.c* to see how we type check different expression and resolve them expression into a single type

### Implementation

In the includes/ folder, create a file called `typecheck.h`:

```
📂 DjCompiler/
├── 📂 includes/
│   ├── 📄 typecheck.h
│   └── ...
└── ...
```

`typecheck.h`:

```c
/* File typecheck.h: Typechecker for DJ */

#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "strmethods.h"
#include "symtbl.h"

/* This method performs all typechecking for the entire DJ program.
   This method assumes setupSymbolTables(), declared in symtbl.h,
   has already executed to set all the global variables (i.e., the
   enhanced symbol tables) declared in symtbl.h.
   If this method finds a typing error, it reports the error and
   exits the compiler.
*/
void typecheckProgram();

/* HELPER METHODS FOR typecheckProgram(): */

/* Returns nonzero iff sub is a subtype of super */
int isSubtype(int sub, int super);

/* Returns the type of the expression AST in the given context.
   Also sets t->staticClassNum and t->staticMemberNum attributes as needed.
   If classContainingExpr < 0 then this expression is in the main block of
   the program; otherwise the expression is in the given class.
*/
int typeExpr(ASTree *t, int classContainingExpr, int methodContainingExpr);

/* Returns the type of the EXPR_LIST AST in the given context. */
int typeExprs(ASTree *t, int classContainingExprs, int methodContainingExprs);

#endif
```

Then, In the `src/` folder, create a folder called `typecheck/` and within that folder create a file called `typecheck.c`:

```
📂 DjCompiler/
├── 📂 src/
│   ├── 📂 typecheck/
│   │   └── 📄 typecheck.c
│   └── ...
└── ...
```

`typecheck.c`:

```c
#include "../../include/typecheck.h"
#include <stdio.h>

typedef enum { INTERNAL_ERR, EXTERNAL_ERR } ErrorType;

void assertAcyclicClassHeirarchies();
void checkClass(ClassDecl *class, int tableIdx, int tableSize);
void checkField(VarDecl *var, int classContainingField, int tableIdx,
                int tableSize);
void checkMethod(MethodDecl *method, int classContainingMethod, int tableIdx,
                 int tableSize);
void checkVariable(VarDecl *var, int tableIdx, int tableSize);
void checkMainBlock();
int join(int t1, int t2);
void exitWithError(ErrorType errType, const char *errmsg, int lineNum);

/* This method performs all typechecking for the entire DJ program.
   This method assumes setupSymbolTables(), declared in symtbl.h,
   has already executed to set all the global variables (i.e., the
   enhanced symbol tables) declared in symtbl.h.
   If this method finds a typing error, it reports the error and
   exits the compiler.
*/
void typecheckProgram() {
  // Assert that there are no cycles in class inheritance
  assertAcyclicClassHeirarchies();
  for (int i = 1; i < numClasses; i++) {
    // Check each class
    checkClass(&classesST[i], i, numClasses);

    int numFields = classesST[i].numVars;
    for (int j = 0; j < numFields; j++)
      // Check each class field
      checkField(&classesST[i].varList[j], i, j, numFields);

    int numMethods = classesST[i].numMethods;
    for (int j = 0; j < numMethods; j++)
      // Check each class method
      checkMethod(&classesST[i].methodList[j], i, j, numMethods);
  }

  // Check main block
  checkMainBlock();
}

/* HELPER METHODS FOR typecheckProgram():
===================================================================================*/

/* Exits program if a cycle exists in the class heirarchy */
void assertAcyclicClassHeirarchies() {
  for (int i = 1; i < numClasses; i++) {
    int counter = 0;
    ClassDecl *currClass = &classesST[i];
    while (currClass->superclass != -4) {
      if (counter > numClasses)
        exitWithError(EXTERNAL_ERR,
                      strConcat("Circular base type dependency involving `",
                                classesST[i].className, "` and `",
                                classesST[classesST[i].superclass].className,
                                "`", NULL),
                      classesST[i].superclassLineNumber);

      currClass = &classesST[currClass->superclass];
      counter++;
    }
  }
}

/* Class semantics */
void checkClass(ClassDecl *class, int tableIdx, int tableSize) {
  // A class can only extend an exisiting class
  if (class->superclass < 0)
    exitWithError(
        EXTERNAL_ERR,
        strConcat("`", class->className,
                  "` cannot be extended. The type name could not be found",
                  NULL),
        class->superclassLineNumber);

  // A class cannot inherit from a final class
  if (classesST[class->superclass].isFinal)
    exitWithError(EXTERNAL_ERR,
                  strConcat("`", class->className,
                            "` cannot derive from final type `",
                            classesST[class->superclass].className, "`", NULL),
                  class->superclassLineNumber);

  // User defined class cannot have the name `Object`. Keyword already taken.
  if (strCompare(class->className, "Object"))
    exitWithError(EXTERNAL_ERR, "Keyword `Object` is already taken",
                  class->classNameLineNumber);

  // Check that class name is unique in the program
  for (int i = 1; i < (tableSize - tableIdx); i++)
    if (strCompare(class->className, class[i].className))
      exitWithError(
          EXTERNAL_ERR,
          strConcat("The namespace already contains a definition for `",
                    class->className, "`", NULL),
          class->classNameLineNumber);
}

/* Field semantics */
void checkField(VarDecl *var, int classContainingField, int tableIdx,
                int tableSize) {
  // Check that field type is not badly typed
  if (var->type < -1)
    exitWithError(EXTERNAL_ERR,
                  strConcat("The type name for `",
                            classesST[classContainingField].className, ".",
                            var->varName, "` could not be found"),
                  var->typeLineNumber);

  // Check that field name is unique in the class
  for (int i = 1; i < (tableSize - tableIdx); i++) {
    if (strCompare(var->varName, var[i].varName))
      exitWithError(EXTERNAL_ERR,
                    strConcat("The type `",
                              classesST[classContainingField].className,
                              "` already contains a definition for `",
                              var->varName, "`", NULL),
                    var->varNameLineNumber);
  }

  // Check that feild name is unique in the superclasses
  ClassDecl *currClass = &classesST[classContainingField];
  while (currClass->superclass >= 0) {
    currClass = &classesST[currClass->superclass];
    for (int i = 0; i < currClass->numVars; i++)
      if (strCompare(var->varName, currClass->varList[i].varName))
        exitWithError(
            EXTERNAL_ERR,
            strConcat("The type `", classesST[classContainingField].className,
                      "` already contains a definition for `", var->varName,
                      "` in its super `", currClass->className, "`", NULL),
            var->varNameLineNumber);
  }
}

/* Method semantics */
void checkMethod(MethodDecl *method, int classContainingMethod, int tableIdx,
                 int tableSize) {
  // TODO: Method overloading?
  // Check that method name is unique in the class
  for (int i = 1; i < (tableSize - tableIdx); i++)
    if (strCompare(method->methodName, method[i].methodName))
      exitWithError(
          EXTERNAL_ERR,
          strConcat("Type `", classesST[classContainingMethod].className,
                    "` already defines a member called `", method->methodName,
                    "` with the same parameter types", NULL),
          method->methodNameLineNumber);

  // Check for methods in super classes to override
  // You can only override a method with the same signature (name, parameter
  // type, return type) and that is not final
  ClassDecl *currClass =
      &(classesST[classesST[classContainingMethod].superclass]);
  while (currClass->superclass != -4) {
    for (int i = 0; i < currClass->numMethods; i++) {
      if (strCompare(method->methodName, currClass->methodList[i].methodName)) {
        if (currClass->methodList[i].isFinal)
          exitWithError(EXTERNAL_ERR,
                        strConcat("`",
                                  classesST[classContainingMethod].className,
                                  ".", method->methodName,
                                  "()`: cannot override inherited member `",
                                  currClass->className, ".",
                                  currClass->methodList[i].methodName,
                                  "()` because it is final", NULL),
                        method->methodNameLineNumber);
        if (method->returnType != currClass->methodList[i].returnType)
          exitWithError(
              EXTERNAL_ERR,
              strConcat("`", classesST[classContainingMethod].className, ".",
                        method->methodName,
                        "()`: return type does not match overridden member `",
                        currClass->className, ".",
                        currClass->methodList[i].methodName, "()`", NULL),
              method->methodNameLineNumber);
        if (method->paramType != currClass->methodList[i].paramType)
          exitWithError(
              EXTERNAL_ERR,
              strConcat("`", classesST[classContainingMethod].className, ".",
                        method->methodName,
                        "()`: param type does not match overridden member `",
                        currClass->className, ".",
                        currClass->methodList[i].methodName, "()`", NULL),
              method->methodNameLineNumber);
      }
    }
    currClass = &classesST[currClass->superclass];
  }

  // Valdiate return type
  if (method->returnType < -1)
    exitWithError(EXTERNAL_ERR,
                  strConcat("`", classesST[classContainingMethod].className,
                            ".", method->methodName,
                            "()`: return type name could not be found", NULL),
                  method->returnTypeLineNumber);

  // Validate param type
  if (method->paramType < -1)
    exitWithError(EXTERNAL_ERR,
                  strConcat("`", classesST[classContainingMethod].className,
                            ".", method->methodName,
                            "()`: parameter type name could not be found"),
                  method->paramTypeLineNumber);

  // Validate each method local
  for (int i = 0; i < method->numLocals; i++) {
    if (strCompare(method->paramName, method->localST[i].varName))
      exitWithError(
          EXTERNAL_ERR,
          strConcat("A local variable named `", method->localST[i].varName,
                    "` cannot be declared in this scope because that name is "
                    "used in an enclosing local scope to define a parameter",
                    NULL),
          method->localST[i].varNameLineNumber);
    checkVariable(&method->localST[i], i, method->numLocals);
  }

  // Typecheck method body and assert that it matches method return type
  if (!isSubtype(typeExprs(method->bodyExprs, classContainingMethod, tableIdx),
                 method->returnType))
    exitWithError(EXTERNAL_ERR,
                  strConcat("`", classesST[classContainingMethod].className,
                            ".", method->methodName, "()`: bad return type",
                            NULL),
                  method->returnTypeLineNumber);
}

/* Variable semantics */
void checkVariable(VarDecl *var, int tableIdx, int tableSize) {
  // Check that variable type is not badly typed
  if (var->type < -1)
    exitWithError(EXTERNAL_ERR, "The variable type name could not be found",
                  var->typeLineNumber);

  // Check that variable name is unique in the method
  for (int i = 1; i < (tableSize - tableIdx); i++)
    if (strCompare(var->varName, var[tableIdx + i].varName))
      exitWithError(EXTERNAL_ERR,
                    strConcat("A local variable named `", var->varName,
                              "` is already defined in this scope", NULL),
                    var->varNameLineNumber);
}

/* Main block sementics */
void checkMainBlock() {
  // Validate each main block local
  for (int i = 0; i < numMainBlockLocals; i++)
    checkVariable(mainBlockST, i, numMainBlockLocals);

  // Typechecking the main body expressions
  typeExprs(wholeProgram->children->next->next->data, -4, -4);
}

/* Returns nonzero iff sub is a subtype of super */
int isSubtype(int sub, int super) {
  // Return true, if sub and super are the same type
  if (sub == super)
    return 1;

  // Return true, if sub is null and super is an Object type
  if (super >= 0 && sub == -2)
    return 1;

  // Climb up Class Hierarchy from sub to super, and return true if found
  ClassDecl *subClass = &(classesST[sub]);
  while (subClass->superclass != -4) {
    if (subClass->superclass == super)
      return 1;
    subClass = &classesST[subClass->superclass];
  }

  // Otherwise return false
  return 0;
}

/* Returns the type of the expression AST in the given context.
   Also sets t->staticClassNum and t->staticMemberNum attributes as needed.
   If classContainingExpr < 0 then this expression is in the main block of
   the program; otherwise the expression is in the given class.
*/
int typeExpr(ASTree *t, int classContainingExpr, int methodContainingExpr) {
  /* Input Validation */
  if (t == NULL)
    exitWithError(INTERNAL_ERR, "Attempted to type check a null node",
                  t->lineNumber);

  /* Setting the staticClassNum and staticMemberNum AST-node attributes for code
   * generation */
  t->staticClassNum = classContainingExpr;
  t->staticMemberNum = methodContainingExpr;

  /* Nat literal return type nat (-1) */
  if (t->typ == NAT_LITERAL_EXPR)
    return -1;

  /* Null expressigon return type null (-2) */
  else if (t->typ == NULL_EXPR)
    return -2;

  /* The not expression e.g. !E is well-typed (with type nat) exactly when the E
   * is a nat type */
  else if (t->typ == NOT_EXPR) {
    int exprType =
        typeExpr(t->children->data, classContainingExpr, methodContainingExpr);

    if (exprType == -1)
      return -1;
    else
      exitWithError(EXTERNAL_ERR,
                    "Bad type for '!' operation. Must be a nat type",
                    t->children->data->lineNumber);
  }

  /* Nat literal operations e.g 5+5, 7-4, or 2*3 */
  else if (t->typ == PLUS_EXPR || t->typ == MINUS_EXPR ||
           t->typ == TIMES_EXPR) {
    int operand1Type =
        typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
    int operand2Type = typeExpr(t->children->next->data, classContainingExpr,
                                methodContainingExpr);

    if (operand1Type == -1 && operand2Type == -1)
      return -1;
    else {
      int lineNumErr;
      if (operand1Type != -1)
        lineNumErr = t->children->data->lineNumber;
      else if (operand2Type != -1)
        lineNumErr = t->children->next->data->lineNumber;

      switch (t->typ) {
      case PLUS_EXPR:
        exitWithError(EXTERNAL_ERR,
                      "Bad type for '+' operation. Must be a nat type",
                      lineNumErr);
        break;
      case MINUS_EXPR:
        exitWithError(EXTERNAL_ERR,
                      "Bad type for '-' operation. Must be a nat type",
                      lineNumErr);
        break;
      case TIMES_EXPR:
        exitWithError(EXTERNAL_ERR,
                      "Bad type for '*' operation. Must be a nat type",
                      lineNumErr);
        break;
      default:
        break;
      }
    }
  }

  /* Conditional expressions e.g. E1==E2, E1<E2, or E1||E2 are well typed (with
  type nat) exactly when (1) E1 has valid type T1, (2) E2 has valid type T2, and
  (3) either T1 is a subtype of T2 or T2 is a subtype of T1. */
  else if (t->typ == EQUALITY_EXPR || t->typ == LESS_THAN_EXPR ||
           t->typ == OR_EXPR) {
    int operand1type =
        typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
    int operand2type = typeExpr(t->children->next->data, classContainingExpr,
                                methodContainingExpr);

    if (isSubtype(operand2type, operand1type) ||
        isSubtype(operand1type, operand2type))
      return -1;
    else {
      switch (t->typ) {
      case EQUALITY_EXPR:
        exitWithError(EXTERNAL_ERR, "Incompatible types for '==' comparison",
                      t->lineNumber);
        break;
      case LESS_THAN_EXPR:
        exitWithError(EXTERNAL_ERR, "Incompatible types for '<' comparison",
                      t->lineNumber);
        break;
      case OR_EXPR:
        exitWithError(EXTERNAL_ERR, "Incompatible types for '||' comparison",
                      t->lineNumber);
        break;
      default:
        break;
      }
    }
  }

  /* An assert expression is well typed (with nat type) exactly when its
  subexpression has nat type. */
  else if (t->typ == ASSERT_EXPR) {
    int exprType =
        typeExpr(t->children->data, classContainingExpr, methodContainingExpr);

    if (exprType == -1)
      return -1;
    else
      exitWithError(EXTERNAL_ERR,
                    "Bad argument for 'assert()'. Must be a nat type",
                    t->children->data->lineNumber);
  }

  /* An if-then-else expression is well typed exactly when its if-expression has
  nat type and either (1) the then- and else-expression-lists both have nat
  type, or (2) the then and else-expression-lists both have an object type. In
  case (1), the type of the entire if-then-else is nat. In case (2), the type of
  the entire if-then-else expression is the join of the types of the then- and
  else-expression-lists. For example, the expression if(true) {null;} else{new
  Object();} has type Object because the join of “any-object” type and Object is
  Object. */
  else if (t->typ == IF_THEN_ELSE_EXPR) {
    int conditionType =
        typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
    int thenExprsType = typeExprs(t->children->next->data, classContainingExpr,
                                  methodContainingExpr);
    int elseExprsType = typeExprs(t->children->next->next->data,
                                  classContainingExpr, methodContainingExpr);
    if (conditionType == -1) {
      if (thenExprsType == -1 && elseExprsType == -1)
        return -1;
      else if ((thenExprsType == -2 || thenExprsType >= 0) &&
               (elseExprsType == -2 || elseExprsType >= 0))
        return join(thenExprsType, elseExprsType);
      else
        exitWithError(
            EXTERNAL_ERR,
            "Final expressions in if and else blocks must be joinable",
            t->lineNumber);
    } else
      exitWithError(EXTERNAL_ERR,
                    "Bad condition for `if()`. Must be a nat type",
                    t->children->data->lineNumber);
  }

  /* A while-loop expression is well typed (with a nat type) exactly when its
  loop-test expression has nat type and its loop-body expression-list is well
  typed. */
  else if (t->typ == WHILE_EXPR) {
    int conditionType =
        typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
    typeExprs(t->children->next->data, classContainingExpr,
              methodContainingExpr);

    if (conditionType == -1)
      return -1;
    else
      exitWithError(EXTERNAL_ERR,
                    "Bad condition for `while()`. Must be a nat type",
                    t->children->data->lineNumber);
  }

  /* A print expression is well typed (with a nat type) exactly when its
  expression argument has a nat type. */
  else if (t->typ == PRINT_EXPR) {
    int exprType =
        typeExpr(t->children->data, classContainingExpr, methodContainingExpr);

    if (exprType == -1)
      return -1;
    else
      exitWithError(EXTERNAL_ERR,
                    "Bad argument for `printNat()`. Must be a nat type",
                    t->children->data->lineNumber);
  }

  /* A read expression is well typed (with a nat type). */
  else if (t->typ == READ_EXPR)
    return -1;

  /* A use of the `this` keyword must appear inside a declaration of some class
  C, in which case this has type C. */
  else if (t->typ == THIS_EXPR) {
    if (classContainingExpr > 0)
      return classContainingExpr;
    else
      exitWithError(
          EXTERNAL_ERR,
          "The `this` expression cannot be used in the main block. It must "
          "only be used inside a method declaration of some class",
          t->lineNumber);
  }

  /* The expression new C() has type C. */
  else if (t->typ == NEW_EXPR) {
    for (int i = 0; i < numClasses; i++)
      if (strCompare(t->children->data->idVal, classesST[i].className))
        return i;
    exitWithError(EXTERNAL_ERR,
                  strConcat("The type name `", t->children->data->idVal,
                            "` could not be found", NULL),
                  t->children->data->lineNumber);
  }

  /* The dot-method-call expression is well typed, when the expression preceding
  the dot evalutates to a class type that contains the method in itself or a
  superclass. Additionally, the argument expression must be a subtype of the
  method's declared parameter type. It should return the method return type */
  else if (t->typ == DOT_METHOD_CALL_EXPR) {
    int objType =
        typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
    int argType = typeExpr(t->children->next->next->data, classContainingExpr,
                           methodContainingExpr);

    if (objType > 0) {
      int found = 0;
      int currClassNum = objType; // Track the class ID
      ClassDecl *currClass = &(classesST[objType]);

      // Loop while valid class (superclass -4 means we went past Object)
      while (currClassNum >= 0) {
        MethodDecl *currMethod = currClass->methodList;
        for (int i = 0; i < currClass->numMethods; i++) {
          if (strCompare(currMethod[i].methodName,
                         t->children->next->data->idVal)) {
            found = 1;

            t->staticClassNum = currClassNum;
            t->staticMemberNum = i;

            if (isSubtype(argType, currMethod[i].paramType))
              return currMethod[i].returnType;
          }
        }
        if (found)
          break; // Stop climbing if found

        currClassNum = currClass->superclass;
        if (currClassNum >= 0)
          currClass = &classesST[currClassNum];
      }

      if (found)
        exitWithError(EXTERNAL_ERR,
                      strConcat("Bad argument for `",
                                t->children->next->data->idVal, "()`", NULL),
                      t->children->next->next->data->lineNumber);
      else
        exitWithError(EXTERNAL_ERR,
                      strConcat("The definition for `",
                                t->children->next->data->idVal,
                                "` does not exist within `",
                                classesST[objType].className, "`", NULL),
                      t->children->next->data->lineNumber);

    } else if (objType == 0)
      exitWithError(EXTERNAL_ERR,
                    "Cannot call a method from the root `Object` class",
                    t->children->data->lineNumber);
    else
      exitWithError(EXTERNAL_ERR, "Cannot call a method from a Non-Object type",
                    t->children->data->lineNumber);
  }

  /* The method-call expression e.g ID(E) is well typed, exactly when a method
  named ID exists in the current class and or super class. Additionally, the
  argument expression must be a subtype of the method's declared parameter type.
  It should return the method return type. */
  else if (t->typ == METHOD_CALL_EXPR) {
    int argType = typeExpr(t->children->next->data, classContainingExpr,
                           methodContainingExpr);

    if (classContainingExpr > 0) {
      int found = 0;
      int currClassNum = classContainingExpr; // Start at current class
      ClassDecl *currClass = &(classesST[classContainingExpr]);

      while (currClassNum >= 0) {
        MethodDecl *currMethod = currClass->methodList;
        for (int i = 0; i < currClass->numMethods; i++) {
          if (strCompare(currMethod[i].methodName, t->children->data->idVal)) {
            found = 1;

            t->staticClassNum = currClassNum;
            t->staticMemberNum = i;

            if (isSubtype(argType, currMethod[i].paramType))
              return currMethod[i].returnType;
          }
        }
        if (found)
          break;

        currClassNum = currClass->superclass;
        if (currClassNum >= 0)
          currClass = &classesST[currClassNum];
      }

      if (found)
        exitWithError(EXTERNAL_ERR,
                      strConcat("Bad argument for `", t->children->data->idVal,
                                "()`", NULL),
                      t->children->next->data->lineNumber);
      else
        exitWithError(EXTERNAL_ERR,
                      strConcat("The name `", t->children->data->idVal,
                                "` does not exist in the current context",
                                NULL),
                      t->children->data->lineNumber);
    } else {
      // ... (Keep your error handling for main block calls here)
      exitWithError(EXTERNAL_ERR,
                    "Method calls cannot be made from the main block. "
                    "Did you mean 'printNat' or 'new ClassName'? ",
                    t->lineNumber);
    }
  }

  /* The dot-id expression is well typed, when the expression preceding the dot
  evaluates to a class type that contains the id as a global variable in the
  class. The return type is the type of the global variable found. */
  else if (t->typ == DOT_ID_EXPR) {
    int objType =
        typeExpr(t->children->data, classContainingExpr, methodContainingExpr);

    if (objType > 0) {
      ClassDecl *currClass = &classesST[objType];
      while (currClass->superclass != -4) {
        VarDecl *currVariable = &(currClass->varList[0]);
        for (int i = 0; i < currClass->numVars; i++)
          if (strCompare(currVariable[i].varName,
                         t->children->next->data->idVal))
            return currVariable[i].type;
        currClass = &classesST[currClass->superclass];
      }
      exitWithError(EXTERNAL_ERR,
                    strConcat("The definition for `",
                              t->children->next->data->idVal,
                              "` does not exist within `",
                              classesST[objType].className, "`", NULL),
                    t->children->next->data->lineNumber);
    } else if (objType == 0)
      exitWithError(EXTERNAL_ERR,
                    "Cannot get field from the root `Object` class",
                    t->children->data->lineNumber);
    else
      exitWithError(EXTERNAL_ERR, "Cannot get field from a Non-Object type",
                    t->children->data->lineNumber);
  }

  /* The id expression is well typed in a method, when the id exists either as a
  method parameter, local, or field. The id could exists as a global varibale in
  the current class or super class. The id epression is well typed in the main
  block, when the id exists only as a local variable. The return type is the
  type of the variable found.*/
  else if (t->typ == ID_EXPR) {
    int variableType = -4;

    // Expression in main block
    if (classContainingExpr < 0) {
      // Search main block locals for variable
      for (int i = 0; i < numMainBlockLocals; i++) {
        if (strCompare(t->children->data->idVal, mainBlockST[i].varName)) {
          variableType = mainBlockST[i].type;
          break;
        }
      }
    }
    // Expression in method block
    else if (classContainingExpr > 0) {
      // Search method parameters for variable
      if (strCompare(classesST[classContainingExpr]
                         .methodList[methodContainingExpr]
                         .paramName,
                     t->children->data->idVal))
        variableType = classesST[classContainingExpr]
                           .methodList[methodContainingExpr]
                           .paramType;
      // If not found, search method locals for variable
      if (variableType == -4) {
        VarDecl *currVariable = classesST[classContainingExpr]
                                    .methodList[methodContainingExpr]
                                    .localST;
        int numOfLocals = classesST[classContainingExpr]
                              .methodList[methodContainingExpr]
                              .numLocals;
        for (int i = 0; i < numOfLocals; i++) {
          if (strCompare(currVariable[i].varName, t->children->data->idVal)) {
            variableType = currVariable[i].type;
            break;
          }
        }
      }
      // If not found, search current class and its super class fields for
      // variable
      if (variableType == -4) {
        ClassDecl *currClass = &classesST[classContainingExpr];
        while (currClass->superclass != -4) {
          VarDecl *currVariable = currClass->varList;
          int numOfFields = currClass->numVars;
          for (int i = 0; i < numOfFields; i++) {
            if (strCompare(currVariable[i].varName, t->children->data->idVal)) {
              variableType = currVariable[i].type;
              break;
            }
          }
          if (variableType != -4)
            break;
          currClass = &classesST[currClass->superclass];
        }
      }
    }

    // Not found
    if (variableType == -4)
      exitWithError(EXTERNAL_ERR,
                    strConcat("The name `", t->children->data->idVal,
                              "` does not exist in the current context", NULL),
                    t->children->data->lineNumber);

    return variableType;
  }

  /* The assignment expressions e.g.  E1.ID=E2 (where ID is an identifier and E1
  & E2 are expressions) is well typed (with whatever type ID has) exactly when
  (1) E1 is an expression with type > 0 (2) ID is a well-typed field that exists
  within E1's type and (3) E2’s type is a subtype of ID’s type. The return type
  is ID's type. */
  else if (t->typ == DOT_ASSIGN_EXPR) {
    int objType =
        typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
    int variableType = -4;
    int expr2Type = typeExpr(t->children->next->next->data, classContainingExpr,
                             methodContainingExpr);

    // Check that expression 1 is an Object type
    if (objType > 0) {
      // Search current class and its super classes fields for variable
      int found = 0;
      ClassDecl *currClass = &classesST[objType];
      while (currClass->superclass != -4) {
        VarDecl *currVariable = &(currClass->varList[0]);
        for (int i = 0; i < currClass->numVars; i++) {
          if (strCompare(currVariable[i].varName,
                         t->children->next->data->idVal)) {
            variableType = currVariable[i].type;
            found = 1;
            break;
          }
        }
        if (found)
          break;
        currClass = &classesST[currClass->superclass];
      }
      // Not Found
      if (variableType == -4)
        exitWithError(EXTERNAL_ERR,
                      strConcat("The definition for `",
                                t->children->next->data->idVal,
                                "` does not exist within `",
                                classesST[objType].className, "`", NULL),
                      t->children->next->data->lineNumber);
    } else
      exitWithError(EXTERNAL_ERR, "Cannot get field from a Non-Object type",
                    t->children->data->lineNumber);

    if (isSubtype(expr2Type, variableType))
      return variableType;
    else
      exitWithError(EXTERNAL_ERR, "Incompatible types for assignment",
                    t->lineNumber);
  }

  /* The assignment expressions e.g. ID=E (where ID is an identifier and E is an
  expression) is well typed (with whatever type ID has) exactly when (1) ID is a
  well-typed variable and (2) E’s type is a subtype of ID’s type. */
  else if (t->typ == ASSIGN_EXPR) {
    int variableType = -4;
    int exprType = typeExpr(t->children->next->data, classContainingExpr,
                            methodContainingExpr);

    // Expression in main block
    if (classContainingExpr < 0) {
      // Search main block locals for variable
      for (int i = 0; i < numMainBlockLocals; i++) {
        if (strCompare(t->children->data->idVal, mainBlockST[i].varName)) {
          variableType = mainBlockST[i].type;
          break;
        }
      }
    }
    // Expression in method block
    else if (classContainingExpr > 0) {
      // Search method parameters for variable
      if (strCompare(classesST[classContainingExpr]
                         .methodList[methodContainingExpr]
                         .paramName,
                     t->children->data->idVal))
        variableType = classesST[classContainingExpr]
                           .methodList[methodContainingExpr]
                           .paramType;
      // If not found, search method locals for variable
      if (variableType == -4) {
        VarDecl *currVariable = classesST[classContainingExpr]
                                    .methodList[methodContainingExpr]
                                    .localST;
        int numOfLocals = classesST[classContainingExpr]
                              .methodList[methodContainingExpr]
                              .numLocals;
        for (int i = 0; i < numOfLocals; i++) {
          if (strCompare(currVariable[i].varName, t->children->data->idVal)) {
            variableType = currVariable[i].type;
            break;
          }
        }
      }
      // If not found, search current class and its super classes fields for
      // variable
      if (variableType == -4) {
        ClassDecl *currClass = &classesST[classContainingExpr];
        while (currClass->superclass != -4) {
          VarDecl *currVariable = currClass->varList;
          int numOfFields = currClass->numVars;
          for (int i = 0; i < numOfFields; i++) {
            if (strCompare(currVariable[i].varName, t->children->data->idVal)) {
              variableType = currVariable[i].type;
              break;
            }
          }
          if (variableType != -4)
            break;
          currClass = &classesST[currClass->superclass];
        }
      }
    }

    // Not found
    if (variableType == -4)
      exitWithError(EXTERNAL_ERR,
                    strConcat("The name `", t->children->data->idVal,
                              "` does not exist in the current context", NULL),
                    t->children->data->lineNumber);

    if (isSubtype(exprType, variableType))
      return variableType;
    else
      exitWithError(EXTERNAL_ERR, "Incompatible types for assignment",
                    t->lineNumber);
  }

  /* Type checking logic for the given expression type has not been implemented
     yet */
  else
    exitWithError(
        INTERNAL_ERR,
        "Attempted to type check an expression that has no type check logic",
        t->lineNumber);

  /* This returns occurs only after internal type checking error or source code
  semantic error has occured. These errors terminate the program so this return
  should never be invoked. */
  return -4;
}

/* Returns the type of the EXPR_LIST AST in the given context. */
int typeExprs(ASTree *t, int classContainingExprs, int methodContainingExprs) {
  // Cannot have an expression list inside the Object class
  if (classContainingExprs == 0)
    exitWithError(INTERNAL_ERR,
                  "Attempted to typecheck an expression in the Object class. "
                  "classContainingExpr cannot be 0 (Object) when type checking "
                  "expression lists using typeExprs()",
                  t->lineNumber);

  // Loop over expression list and get type
  int type = -4;
  if (t->typ == EXPR_LIST) {
    ASTList *currExpr = t->children;
    while (currExpr != NULL) {
      type =
          typeExpr(currExpr->data, classContainingExprs, methodContainingExprs);

      /* By default, the if statement below should never run, because by default
      typeExpr() above exits the program when it encounters a badly-typed
      expression before it returns -4. If this preemptive exit is removed or a
      badly typed expression slips through without getting handled, then it
      should exit here. */
      if (type == -4) {
        printf("Failed to compile\n");
        exit(EXIT_FAILURE);
      }

      currExpr = currExpr->next;
    }
  } else
    exitWithError(
        INTERNAL_ERR,
        "Attempted to type check a node that is not an expression list",
        t->lineNumber);

  // This indicates that the expression list is empty
  if (type == -4)
    exitWithError(INTERNAL_ERR, "Expression list is empty", t->lineNumber);

  // Return type
  return type;
}

/* Joins two types by returning the closest super type that they both share */
int join(int t1, int t2) {
  if (isSubtype(t1, t2))
    return t2;
  if (isSubtype(t2, t1))
    return t1;
  return join(classesST[t1].superclass, t2);
}

/* Error Handler */
void exitWithError(ErrorType errType, const char *errmsg, int lineNum) {
  switch (errType) {
  case INTERNAL_ERR:
    printf("Internal-Error occurred on line %d -> \"%s\". See 'typecheck.c' "
           "for more details.\n",
           lineNum, errmsg);
    break;
  case EXTERNAL_ERR:
    printf("Semantic analysis error on line %d:\n\t%s.\n", lineNum, errmsg);
    break;
  default:
    printf("Undefined error occured on line %d: %s\n", lineNum, errmsg);
    break;
  }
  exit(EXIT_FAILURE);
}
```

## Separation of Responsibilities

An important architectural principle evident in this design is the strict separation between **symbol discovery** and **semantic validation**. The symbol table builder never reports errors related to type correctness, inheritance legality, or name collisions. Its only responsibility is to record what the program declares, not whether those declarations are legal.

This separation allows the typechecker to operate under a strong assumption: *all symbols exist in a normalized, indexable form*. As a result, the typechecker can focus exclusively on enforcing language rules, rather than reconstructing context from the AST.

## Integrating our symbol table and type checker in our program

1. Include `"../include/symtbl.h"` & `"../include/typecheck.h"` in `%code provides {…}` in the **Declaration** section of our Bison file
    
    ```c
    %code provides {
      // ...
      #include "../include/symtbl.h"
      #include "../include/typecheck.h"
        
      #define DEBUG_SYMTBL 1
      // ...
    }
    ```
    
2. Setup symbol table and type check program in our `main` function in the **User Code** section using the following code snippet.
    
    ```c
    int main(int argc, char *argv[]) {
    	// ... (Your existing code) ... 
      
      /* set up symbol tables */
      setupSymbolTables(pgmAST);
      if (DEBUG_SYMTBL) {
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
      
      return 0;
    }
    ```
    

Our entire Bison file, `dj.l`, should now look like this:

```bash
/* dj.y: PARSER for DJ */

%code provides {
  #include "lex.yy.c"
  #include "../include/ast.h"
  #include "../include/symtbl.h"
  #include "../include/typecheck.h"
    
  #define DEBUG_SYMTBL 1
  #define DEBUG_AST 0

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
  if (DEBUG_AST) {
    printf("****** begin abstract syntax tree for DJ program ******\n");
    printAST(pgmAST);
    printf("****** end abstract syntax tree for DJ program ******\n");
  }
  
  /* set up symbol tables */
  setupSymbolTables(pgmAST);
  if (DEBUG_SYMTBL) {
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

  return 0;
}
```

### Compilation

Make and run our compiler program using the provided `sh` script in our root project direction. At this point, I recommend setting the `DEBUG_SCAN`  and `DEBUG_AST` macros to `0` in the Flex file, and Bison file. 

With the `DEBUG_SYMTBL` macro set to `1`,  you can now see the visualization of the symbol table for your program.

```bash
**…/DjCompiler ❯** ./dj.sh tests/good/good1.dj
****** begin classes symbol table for DJ program ******
Class: Object; Super: -4; isFinal: 0
Class: C; Super: 0; isFinal: 0
****** end classes symbol table for DJ program ******

**…/DjCompiler ❯** ./dj.sh tests/good/good3.dj
****** begin classes symbol table for DJ program ******
Class: Object; Super: -4; isFinal: 0
MainLocalType: -1; MainLocalName: x
****** end classes symbol table for DJ program ******

```

### Running tests

It is at this point, you want to begin writing rigorous test cases. With some good examples in the `test/good` folder and some bad examples in the `test/bad` folder, you can run `make test` to automatically run tests on all your example programs:

```c
**…/DjCompiler ❯** make test
--- Good Cases (Expect Success) ---
--- Bad Cases (Expect Failure) ---

📊 Result: 79 / 79 passed.
```
