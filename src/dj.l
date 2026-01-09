/* dj.l: SCANNER for DJ */

%option yylineno
%option noyywrap

%{
  #define DEBUG_SCAN 0
  typedef int Token;
  Token scanned(Token t);
%}

digits 0|([1-9][0-9]*)
id ["_"a-zA-Z]["_"a-zA-Z0-9]*
wspace [ \t\r\n]+
comment "//".*\n?

%%

{wspace}    {/*skip whitespace*/}
{comment}   {/*skip comments*/}
final       {if(DEBUG_SCAN) printf("FINAL "); return FINAL;}
class       {if(DEBUG_SCAN) printf("CLASS "); return CLASS;} 
extends     {if(DEBUG_SCAN) printf("EXTENDS "); return EXTENDS;}
main        {if(DEBUG_SCAN) printf("MAIN "); return MAIN;}
nat         {if(DEBUG_SCAN) printf("NATTYPE "); return NATTYPE;}
printNat    {if(DEBUG_SCAN) printf("PRINTNAT "); return PRINTNAT;}
readNat     {if(DEBUG_SCAN) printf("READNAT "); return READNAT;}
"+"         {if(DEBUG_SCAN) printf("PLUS "); return PLUS;}
"-"         {if(DEBUG_SCAN) printf("MINUS "); return MINUS;}
"*"         {if(DEBUG_SCAN) printf("TIMES "); return TIMES;}
"=="        {if(DEBUG_SCAN) printf("EQUALITY "); return EQUALITY;}
"<"         {if(DEBUG_SCAN) printf("LESS "); return LESS;}
assert      {if(DEBUG_SCAN) printf("ASSERT "); return ASSERT;}
"||"        {if(DEBUG_SCAN) printf("OR "); return OR;}
"!"         {if(DEBUG_SCAN) printf("NOT "); return NOT;}
if          {if(DEBUG_SCAN) printf("IF "); return IF;}
else        {if(DEBUG_SCAN) printf("ELSE "); return ELSE;}
while       {if(DEBUG_SCAN) printf("WHILE "); return WHILE;}
"="         {if(DEBUG_SCAN) printf("ASSIGN "); return ASSIGN;}
null        {if(DEBUG_SCAN) printf("NUL "); return NUL;}
new         {if(DEBUG_SCAN) printf("NEW "); return NEW;}
this        {if(DEBUG_SCAN) printf("THIS "); return THIS;}
"."         {if(DEBUG_SCAN) printf("DOT "); return DOT;}
";"         {if(DEBUG_SCAN) printf("SEMICOLON "); return SEMICOLON;}
"{"         {if(DEBUG_SCAN) printf("LBRACE "); return LBRACE;}
"}"         {if(DEBUG_SCAN) printf("RBRACE "); return RBRACE;}
"("         {if(DEBUG_SCAN) printf("LPAREN "); return LPAREN;}
")"         {if(DEBUG_SCAN) printf("RPAREN "); return RPAREN;}
{digits}    {if(DEBUG_SCAN) printf("NATLITERAL(%s) ", yytext); return NATLITERAL;}
{id}        {if(DEBUG_SCAN) printf("ID(%s) ", yytext); return ID;}
<<EOF>>     {if(DEBUG_SCAN) printf("ENDOFFILE\n"); return ENDOFFILE;}
.           {if(DEBUG_SCAN) printf("\n");
             printf("Lex error on line %d: Illegal character %s\n", yylineno, yytext);
             exit(-1);}
%%
