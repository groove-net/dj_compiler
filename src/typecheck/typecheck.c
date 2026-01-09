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
