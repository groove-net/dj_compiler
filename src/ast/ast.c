/******************************************************/
/*                                                    */
/* Program : AST Generator                            */
/*                                                    */
/******************************************************/
#include "../../include/ast.h"
#include <stdio.h>
#include <stdlib.h>

/* METHODS TO CREATE AND MANIPULATE THE AST */
/* Create a new AST node of type t having only one child.
   (That is, create a one-node list of children for the new tree.)
   If the child argument is NULL, then the single list node in the
   new AST node's list of children will have a NULL data field.
   If t is NAT_LITERAL_EXPR then the proper natAttribute should be
   given; otherwise natAttribute is ignored.
   If t is AST_ID then the proper idAttribute should be given;
   otherwise idAttribute is ignored.
*/
ASTree *newAST(ASTNodeType t, ASTree *child, unsigned int natAttribute,
               char *idAttribute, unsigned int lineNum) {
  ASTList *childNode = (ASTList *)malloc(sizeof(ASTList));
  childNode->data = child;
  childNode->next = NULL;

  ASTree *root = (ASTree *)malloc(sizeof(ASTree));
  root->typ = t;
  root->natVal = natAttribute;
  root->idVal = idAttribute;
  root->lineNumber = lineNum;
  root->children = childNode;
  root->childrenTail = childNode;

  return root;
}

/* Append an AST node onto a parent's list of children */
void appendToChildrenList(ASTree *parent, ASTree *newChild) {
  ASTList *newChildNode = (ASTList *)malloc(sizeof(ASTList));
  newChildNode->data = newChild;
  newChildNode->next = NULL;

  parent->childrenTail->next = newChildNode;
  parent->childrenTail = newChildNode;
}

void printASTPreorder(ASTree *t, int level) {
  if (t == NULL)
    return;

  for (int i = 0; i < level; i++)
    printf("  ");
  switch (t->typ) {

  case PROGRAM:
    printf("PROGRAM");
    break;
  case CLASS_DECL_LIST:
    printf("CLASS_DECL_LIST");
    break;
  case FINAL_CLASS_DECL:
    printf("FINAL_CLASS_DECL");
    break;
  case NONFINAL_CLASS_DECL:
    printf("NONFINAL_CLASS_DECL");
    break;
  case VAR_DECL_LIST:
    printf("VAR_DECL_LIST");
    break;
  case VAR_DECL:
    printf("VAR_DECL");
    break;
  case METHOD_DECL_LIST:
    printf("METHOD_DECL_LIST");
    break;
  case FINAL_METHOD_DECL:
    printf("FINAL_METHOD_DECL");
    break;
  case NONFINAL_METHOD_DECL:
    printf("NONFINAL_METHOD_DECL");
    break;
  case NAT_TYPE:
    printf("NAT_TYPE");
    break;
  case AST_ID:
    printf("AST_ID(%s)", t->idVal);
    break;
  case EXPR_LIST:
    printf("EXPR_LIST");
    break;
  case DOT_METHOD_CALL_EXPR:
    printf("DOT_METHOD_CALL_EXPR");
    break;
  case METHOD_CALL_EXPR:
    printf("METHOD_CALL_EXPR");
    break;
  case DOT_ID_EXPR:
    printf("DOT_ID_EXPR");
    break;
  case ID_EXPR:
    printf("ID_EXPR");
    break;
  case DOT_ASSIGN_EXPR:
    printf("DOT_ASSIGN_EXPR");
    break;
  case ASSIGN_EXPR:
    printf("ASSIGN_EXPR");
    break;
  case PLUS_EXPR:
    printf("PLUS_EXPR");
    break;
  case MINUS_EXPR:
    printf("MINUS_EXPR");
    break;
  case TIMES_EXPR:
    printf("TIMES_EXPR");
    break;
  case EQUALITY_EXPR:
    printf("EQUALITY_EXPR");
    break;
  case LESS_THAN_EXPR:
    printf("LESS_THAN_EXPR");
    break;
  case NOT_EXPR:
    printf("NOT_EXPR");
    break;
  case OR_EXPR:
    printf("OR_EXPR");
    break;
  case ASSERT_EXPR:
    printf("ASSERT_EXPR");
    break;
  case IF_THEN_ELSE_EXPR:
    printf("IF_THEN_ELSE_EXPR");
    break;
  case WHILE_EXPR:
    printf("WHILE_EXPR");
    break;
  case PRINT_EXPR:
    printf("PRINT_EXPR");
    break;
  case READ_EXPR:
    printf("READ_EXPR");
    break;
  case THIS_EXPR:
    printf("THIS_EXPR");
    break;
  case NEW_EXPR:
    printf("NEW_EXPR");
    break;
  case NULL_EXPR:
    printf("NULL_EXPR");
    break;
  case NAT_LITERAL_EXPR:
    printf("NAT_LITERAL_EXPR(%d)", t->natVal);
    break;
  default:
    printf("--- error occured ---");
  }
  if (t->lineNumber == 0)
    printf("   (N/A)\n");
  else
    printf("   (ends on line %d)\n", t->lineNumber);

  // Incrememt tree level
  level++;

  // Recurse on children from left to right
  ASTList *firstchild = t->children;
  while (firstchild != t->childrenTail) {
    printASTPreorder(firstchild->data, level);
    firstchild = firstchild->next;
  }
  printASTPreorder(firstchild->data, level);
}

/* Print the AST to stdout with indentations marking tree depth. */
void printAST(ASTree *t) { printASTPreorder(t, 0); }

// Check if a character is alphanumeric (A-Z, a-z, 0-9) or underscore (_)
int is_valid_char(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
         (c >= '0' && c <= '9') || (c == '_');
}

char *getID(const char *src) {
  const char *start = src;
  while (*src && is_valid_char(*src)) {
    src++; // Count valid characters
  }

  size_t length = src - start; // Get the length of valid characters
  char *result = (char *)malloc(length + 1); // Allocate memory for new string

  if (!result) {
    return NULL; // Return NULL if memory allocation fails
  }

  for (size_t i = 0; i < length; i++) {
    result[i] = start[i]; // Copy valid characters
  }
  result[length] = '\0'; // Null-terminate the string

  return result;
}
