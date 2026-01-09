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
