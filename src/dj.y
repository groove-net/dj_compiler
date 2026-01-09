/* dj.y: PARSER for DJ */

%code provides {
  #include "lex.yy.c"
  
  /* Function for printing generic syntax-error messages */
  void yyerror(const char *str) {
    printf("Syntax error on line %d at token %s\n",
           yylineno,yytext);
    printf("Halting compiler.\n");
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

%%

pgm :
    ;

%%

int main(int argc, char **argv) {
  if(argc!=2) {
    printf("Usage: dj <filename>\n");
    exit(-1);
  }
  
  // open the input file
  yyin = fopen(argv[1],"r");
  if(yyin==NULL) {
    printf("ERROR: could not open file %s\n",argv[1]);
    exit(-1);
  }
  
  // tokenize the input file
  while(yylex()!=ENDOFFILE) { }
  
  return 0;
}
