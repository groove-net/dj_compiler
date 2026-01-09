/* dj.y: PARSER for DJ */

%code provides {
  #include "lex.yy.c"
  #include "../include/ast.h"
  #include "../include/symtbl.h"
  #include "../include/typecheck.h"
  #include "../include/codegen.h"
    
  #define DEBUG_SYMTBL 0
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

int main(int argc, char **argv) {
  if(argc!=2) {
    printf("Usage: parsedj filename\n");
    exit(-1);
  }
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
 
  /* generate NASM code */
	FILE *out = fopen("program.asm", "w");
	if (out == NULL) {
	    printf("ERROR: could not open program.asm for writing\n");
	    exit(-1);
	}
	
	generateNASM(out);
	fclose(out);

  // Execute the Build Commands
  int nasmStatus = system("nasm -f elf64 program.asm -o program.o");
  if (nasmStatus != 0) {
      fprintf(stderr, "Error: NASM failed to assemble.\n");
      return 1;
  }

  int ldStatus = system("ld program.o -o program");
  if (ldStatus != 0) {
      fprintf(stderr, "Error: Linker failed.\n");
      return 1;
  }

  // Run the generated program
  int runStatus = system("./program");
  printf("\n--- Program exited with code: %d ---\n", runStatus);
  
  return 0;
}
