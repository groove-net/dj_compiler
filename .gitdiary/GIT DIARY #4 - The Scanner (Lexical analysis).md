## 1. Introduction

This phase of the compiler does the actual reading of the source program, which is usually in the form of a stream of characters. The scanner performs what is called **lexical analysis**: it collects sequences of characters from the the source code and form them into logical units called **tokens**, which are like the words of a natural language such as English. Thus, a scanner can be thought to perform a function similar to spelling.

As an example, consider the following line of code, which could be part of a C program:

```c
a[index] = 4 + 2
```

This code contains 12 nonblank characters but only 8 tokens:

- **`a`**: ID
- **`[`**: LBRACKET
- **`index`**: ID
- **`]`**: RBRACKET
- **`=`**: ASSIGN
- **`4`**: NUM
- **`+`**: PLUS
- **`2`**: NUM

Each token consists of one or more characters that are collected into a unit before further processing takes place.

---

The scanning, or **lexical analysis**, phase of a compiler has the task of reading the source program as a file of characters and dividing it up into tokens. **Tokens** are like the words of a natural language: each token is a sequence of characters that represents a unit of information in the source program. Typical examples are **keywords**, such as **`if`** and **`while`**, which are fixed strings of letters; **identifiers**, which are user-defined strings, usually consisting of letters and numbers and beginning with a letter; **special symbols**, such as the arithmetic symbols **`+`** and **`*`**; as well as a few multicharacter symbols, such as **`>=`** and **`<>`**. In each case a token represents a certain pattern of characters that is recognized, or matched, by the scanner from the beginning of the remaining input characters.

Since the task performed by the scanner is a special case of pattern matching, we need to study methods of pattern specification and recognition as they apply to the scanning process. These methods are primarily those of **regular expressions** and **finite automata**. However, a scanner is also the part of the compiler that handles the input of the source code, and since this input often involves a significant time overhead, the scanner must operate as efficiently as possible. Thus, we need also to pay close attention to the practical details of the scanner structure.

Tokens are logical entities that are usually defined as an enumerated type. For example, tokens might be defined in C as

```c
typedef enum
    { IF, THEN, ELSE, PLUS, MINUS, NUM, ID, ... }
    TokenType;
```

Tokens fall into several categories. These include the **reserved words**, such as **`IF`** and **`THEN`**, which represent the strings of characters "if" and "then". A second category is that of **special symbols**, such as the arithmetic symbols **`PLUS`** and **`MINUS`**, which represent the characters "+" and "-". Finally, there are tokens that can represent multiple strings. Examples are **`NUM`** and **`ID`**, which represent numbers and identifiers.

Tokens as logical entities must be clearly distinguished from the strings of characters that they represent. For example, the reserved word token **`IF`** must be distinguished from the string of two characters "if" that it represents. To make this distinction clearer, the string of characters represented by a token is sometimes called its **string value** or its **lexeme**. Some tokens have only one lexeme: reserved words have this property. A token may represent potentially infinitely many lexemes, however. Identifiers, for example, are all represented by the single token **`ID`**, but they have many different string values representing their individual names. These names cannot be ignored, since a compiler must keep track of them in a symbol table. Thus, a scanner must also construct the string values of at least some of the tokens. Any value associated to a token is called an **attribute** of the token, and the string value is an example of an attribute. Tokens may also have other attributes. For example, a **NUM** token may have a string value attribute such as "32767," which consists of five numeric characters, but it will also have a numeric value attribute that consists of the actual value 32767 computed from its string value. In the case of a special symbol token such as **PLUS**, there is not only the string value "+" but also the actual arithmetic operation + that is associated with it. Indeed, the token symbol itself may be viewed as simply another attribute, and the token viewed as the collection of all of its attributes.

A scanner needs to compute at least as many attributes of a token as necessary to allow further processing. For example, the string value of a **NUM** token needs to be computed, but its numeric value need not be computed immediately, since it is computable from its string value. On the other hand, if its numeric value is computed, then its string value may be discarded. Sometimes the scanner itself may perform the operations necessary to record an attribute in the appropriate place, or it may simply pass on the attribute to a later phase of the compiler. For example, a scanner could use the string value of an identifier to enter it into the symbol table, or it could pass it along to be entered at a later stage.

Since the scanner will have to compute possibly several attributes for each token, it is often helpful to collect all the attributes into a single structured data type, which we could call a token record. Such a record could be declared in C as

```c
typedef struct
    { TokenType tokenval;
      char * stringval;
      int numval;
    } TokenRecord;
```

or possibly as a union

```c
typedef struct
    { TokenType tokenval;
      union
      { char * stringval;
        int numval;
      } attribute;
    } TokenRecord;
```

(which assumes that the string value attribute is needed only for identifiers and the numeric value attribute only for numbers). A more common arrangement is for the scanner to return the token value only and place the other attributes in variables where they can be accessed by other parts of the compiler.

Although the task of the scanner is to convert the entire source program into a sequence of tokens, the scanner will rarely do this all at once. Instead, the scanner will operate under the control of the parser, returning the single next token from the input on demand via a function that will have a declaration similar to the C declaration

```c
TokenType getToken(void);
```

The **getToken** function declared in this manner will, when called, return the next token from the input, as well as compute additional attributes, such as the string value of the token. The string of input characters is usually not made a parameter to this function, but is kept in a buffer or provided by the system input facilities.

As an example of the operation of **getToken**, consider the following line of C source code, which we used as an example earlier:

```c
**a[index] = 4 + 2**
```

Suppose that this line of code is stored in an input buffer as follows, with the next input character indicated by the arrow:

```c
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
|   |   |   |   | a | [ | i | n | d | e | x | ] |   | = |   | 4 |   | + |   | 2 |   |   |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  ^
```

A call to **getToken** will now need to skip the next four blanks, recognize the string "a" consisting of the single character *a* as the next token, and return the token value **ID** as the next token, leaving the input buffer as follows:

```c
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
|   |   |   |   | a | [ | i | n | d | e | x | ] |   | = |   | 4 |   | + |   | 2 |   |   |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
                      ^
```

Thus, a subsequent call to **getToken** will begin the recognition process again with the left bracket character.

## 2. A little introduction to Lex & YACC

To recap, in the early stages of compiler construction, our first goal is to take a raw stream of characters—the source code—and group them into logical and meaningful units called **tokens** using a scanner or lexer. The next goal, which will be tackled in the next devlog, is to… We *could* write the code to perform the scanning and parsing of source code by hand but that would be tedious especially for a non-trivial language. To aleviate this headache, we will use two industry-standard tools for auto-generating scanner code and a parser code based on specified input rules: Lex (the scanner code generator) and YACC (the parser code generator).

If you have been programming for any length of time in a Unix environment, you will have encountered the mystical programs Lex & YACC, or as they are known to GNU/Linux users worldwide, Flex & Bison, where Flex is a Lex implementation by Vern Paxson and Bison the GNU version of YACC. We will call these programs Flex and Bison throughout - the newer versions are upwardly compatible, so you can use Flex and Bison when trying our examples.

These programs are massively useful, but as with your C compiler, their manpage does not explain the language they understand, nor how to use them. YACC is really amazing when used in combination with Lex, however, the Bison manpage does not describe how to integrate Lex generated code with your Bison program.

When properly used, these programs allow you to tokenize and parse complex languages with ease. This is a great boon when you want to read a configuration file, or want to write a compiler for any language you (or anyone else) might have invented.

With a little help, which this document will hopefully provide, you will find that you will never write a scanner and parser by hand - Flex & Bison are the tools to do this.

## 3. Introduction to Bison

Bison is a **parser code generator** (a version of Yacc). Instead of writing the complex parsing logic by hand in C, you write the grammar rules in the Bison file, and Bison automatically generates the C code to parse it.

However, in this first stage of building a compiler where our current main focus is the scanner, Bison serves only two simple purposes:

1. It establishes the **program entry point** that wires and merges all the different parts of our compiler.
2. It defines the **canonical set of token identifiers** that represent your language in which the rest of the system will use.

### Installation

On Arch Linux, you can install Bison via `pacman`:

```bash
**$** sudo pacman -S bison
**$** bison --version
```

### The Bison file

The Bison file is the blueprint or specification for the **Parser**. It ends with the `.y` extension. You should already have a `.y` file in your `src/` folder, a Bison file called `dj.y`:

A Bison file is divided into three sections separated by `%%` delimiters: the **declarations section**, the **grammar rules section**, and the **user code section**.

1. **Declarations section**: Used for token definitions, C declarations, and configuration.
2. **Grammar rules section**: Where syntax productions are defined.
3. **User code section**: Arbitrary C code copied verbatim into the generated output.

At this stage of development, only the **declarations** and **user code** sections are relevant. We use the declarations section to define our **token vocabulary** from the previous devlog. When you write `%token CLASS`, you are telling Bison to assign a unique integer ID to that name. These tokens are the shared language between our scanner and our eventual parser — both components agree on token identity through this shared enumeration. Lastly, we use the user code section to define the main function, i.e., the entry point to our compiler program.

`dj.y`:

```c
/* dj.y: PARSER for DJ */

%code provides {
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

int main(int argc, char *argv[]) {
  return 0;
}
```

The **grammar rule** section is left empty for now; we aren't performing any syntax analysis or parsing yet, we are just ensuring that the source text can be reliably decomposed into a stream of well-formed tokens. Once we move to syntax analysis in the next devlog, this section will contain the recursive rules that define the DJ language structure.

## 4. The Lexical Scanner (Flex)

If Bison defines *what* tokens exist, Flex defines *how* they are recognized in raw source text.

The lexer’s responsibility is deliberately narrow:

- Read a sequence of characters
- Group them into lexemes
- Classify each lexeme as a token
- Reject illegal character sequences early

This stage is purely mechanical. No syntax, no semantics—only pattern recognition.

### Installation

```bash
**$** sudo pacman -S flex
**$** flex --version
```

### The Flex file

The Flex file is the blueprint or specification for the `Scanner`. It ends with the `.l` extension. You should already have a `.l` file in your `src/` folder, a Flex file called `dj.l`:

A Flex file is also divided into three sections, separated by `%%`:

1. **Definitions and options** This section sets up the environment for the scanner. It contains:
    - **Flex Options:** Directives to configure Flex behavior (e.g., `%option yylineno` to track line numbers automatically).
    - **C Declarations:** Enclosed in `%{ %}`, this includes necessary C headers (e.g., `#include "dj.tab.h"` to access token definitions).
    - **Regex Macros:** Named shortcuts for complex regular expressions (e.g., defining `DIGIT` as `[0-9]`) to make the rules section more readable.
2. **Tokenization Rules** This is the core of the scanner where pattern matching occurs. It consists of a list of rules where each rule has two parts:
    - **Pattern:** A **Regular Expression** that matches a specific sequence of characters in the source code (like keywords, identifiers, or symbols).
    - **Action:** A block of C code executed when that pattern is matched. This usually involves returning a specific **Token Type** (e.g., `return NUM;`) or handling attributes like string values.
3. **User code** This section allows you to include arbitrary C functions that are copied verbatim to the end of the generated C file. It often includes helper functions called by the actions in the rules section.

Below is what a complete Flex file for Diminished Java (DJ) would like. We’ll walk through the details soon:

`dj.l`

```c
/* dj.l: SCANNER for DJ */

%option yylineno
%option noyywrap

%{
  #define DEBUG_SCAN 1
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
```

Soon enough, let’s walk through each section:

1. **Definitions and options**
    
    ```c
    /* dj.l: SCANNER for DJ */
    
    %option yylineno
    %option noyywrap
    
    %{
      #define DEBUG_SCAN 1
      typedef int Token;
      Token scanned(Token t);
    %}
    ```
    
    This section configures the generated lexer and declares helper code.
    
    - `%option yylineno` instructs Flex to maintain a global line counter, which is invaluable for diagnostics.
    - `%option noyywrap` disables the default end-of-file handler, simplifying single-file input handling.
    - The `%{ ... %}` block is copied verbatim into the generated C file and is used for forward declarations and macros. For example, we have a `DEBUG_SCAN` macro which enables token tracing. This is not merely convenience; it is a diagnostic tool that allows you to *observe* the lexer’s behavior before it is embedded in a full parser.
    
    Next we have the following code block:
    
    ```c
    digits 0|([1-9][0-9]*)
    id ["_"a-zA-Z]["_"a-zA-Z0-9]*
    wspace [ \t\r\n]+
    comment "//".*\n?
    ```
    
    These definitions act as macros for regular expressions. Regular expressions represent patterns of strings of characters. A regular expression *r* is completely defined by the set of strings that it matches. This set is called the **language generated by the regular expression** and is written as *L(r)*. Here the word *language* is used only to mean “set of strings” and has (at least at this stage) no specific relationship to a programming language. This language depends, first, on the character set that is available. Generally, this will be the set of ASCII characters or some subset of it. Sometimes the set will be more general than the ASCII character set, in which case the set elements are referred to as **symbols**. This set of legal symbols is called the **alphabet**.
    
    A regular expression *r* will also contain characters from the alphabet, but such characters have a different meaning: in a regular expression, all symbols indicate *patterns*.
    
    Last, a regular expression *r* may contain characters that have special meanings. Such characters are called **metacharacters** or **metasymbols**. These generally may not be legal characters in the alphabet, or we could not distinguish their use as metacharacter from their use as a member of the alphabet. Often, however, it is not possible to require such an exclusion, and a convention must be used to differentiate the two possible uses of a metacharacter. In many situations this is done by using an **escape character** that “turns off” the special meaning of a metacharacter. Common escape characters are the backslash and quotes. In the code, above quotes are used as an escape. Note that escape characters are themselves metacharacters, if they are also legal characters in the alphabet.
    
    You may need to do some independent readon on regular expression on your own.
    
    These macros exist to improve readability, eliminate duplication, and centralize language decisions when defining tokenization rules in the section below. Here is a simple breakdown of each pattern macro:
    
    - `digits 0|([1-9][0-9]*)`
        
        This pattern defines how integer numbers are recognized, ensuring valid formatting (e.g., preventing leading zeros like `05`).
        
        - **`0`**: It matches the single digit `0` exactly.
        - **`|`**: The pipe symbol acts as an "OR" operator.
        - **`[1-9]`**: Alternatively, the number can start with a non-zero digit (1 through 9).
        - **`[0-9]*`**: After that initial non-zero digit, it matches zero or more additional digits (0 through 9).
    - `id ["_"a-zA-Z]["_"a-zA-Z0-9]*`
        
        This pattern defines a valid identifier (variable or function name), following standard C-like naming conventions.
        
        - **`["_"a-zA-Z]`**: The identifier **must start** with either an underscore (`_`), a lowercase letter (`a-z`), or an uppercase letter (`A-Z`).
        - **`["_"a-zA-Z0-9]*`**: After the first character, the identifier may contain any combination of underscores, letters, or digits (`0-9`). The  indicates there can be zero or more of these subsequent characters.
    - `wspace [ \t\r\n]+`
        
        This pattern is used to identify (and typically skip) whitespace in the source code.
        
        - **`[ \t\r\n]`**: This character class matches four specific characters: a literal space, a tab (`\t`), a carriage return (`\r`), or a newline (`\n`).
        - **`+`**: The plus sign means it matches a sequence of **one or more** of these characters. This allows the scanner to treat a block of multiple spaces or newlines as a single whitespace unit.
    - `comment "//".*\n?`
        
        This pattern recognizes single-line comments that begin with double slashes.
        
        - **`"//"`**: It begins by matching the literal characters `//` exactly.
        - **`.*`**: It then matches any sequence of characters appearing after the slashes (typically until the end of the line).
        - **`\n?`**: Finally, it matches an optional newline character (`\n`) at the end. The `?` means the newline is valid but not strictly required (useful if the file ends with a comment but no newline).
2. **Tokenization Rules**
    
    ```c
    %%
    
    {wspace}    {/*skip whitespace*/}
    {comment}   {/*skip comments*/}
    final       {if(DEBUG_SCAN) printf("FINAL "); return FINAL;}
    class       {if(DEBUG_SCAN) printf("CLASS "); return CLASS;} 
    ...
    {id}        {if(DEBUG_SCAN) printf("ID(%s) ", yytext); return ID;}
    <<EOF>>     {if(DEBUG_SCAN) printf("ENDOFFILE\n"); return ENDOFFILE;}
    .           {if(DEBUG_SCAN) printf("\n");
                 printf("Lex error on line %d: Illegal character %s\n", yylineno, yytext);
                 exit(-1);}
    %%
    ```
    
    This section is the heart of the lexer. This sections use regular expression to map patterns to tokens. Each mapping is called a tokenization rule. We can make use of the regular expression macros defined in the previous section. 
    
    Rules are evaluated **top-to-bottom**, using the longest-match rule first. This ordering is significant. Keywords appear before identifiers so that `class` is not mistakenly classified as an `ID`. For every rule or pattern matched, we print the corresponding token if the `DEBUG_SCAN` flag is `1` then return the corresponding token. Notice that for tokens like `ID` and `NATLITERAL` we can use the `yytext` variable provided by Flex to print the string of character the token represents. This is only useful for these two tokens as they can represent multiple strings, i.e. user-defined identifiers and numbers.
    
    Whitespace and comments are explicitly discarded. This is not an omission—it is a design choice. Tokens represent *syntactic units*, not textual trivia.
    
    The final catch-all rule (`.`) ensures that illegal characters are detected immediately, rather than being silently ignored or misclassified. Notice that we can use the `yylineno` to print the line number the token error occured.
    
3. **User code**
    
    This section is empty.
    

### How Flex Generates Code

When you run `flex` on our `dj.l` file, like so:

```bash
**$** flex dj.l
```

It produces a `lex.yy.c` file, which contains a fully-formed scanner implemented as a deterministic finite automaton (DFA). We may treat this code as a black box, but understand that Flex has built us a C program that can recognize the regular expressions and patterns we’ve defined and matches them to the corresponsing token based on our tokenization rules.

> *To formally understand how these patterns are recognized within the source code, we must introduce the mathematical concepts of Finite Automata.*

**FINITE AUTOMATA**

Finite automata, or finite-state machines, are a mathematical way of describing particular kinds of algorithms (or “machines”). In particular, finite automata can be used to describe the process of recognizing patterns in input strings, and so can be used to construct scanners. There is also, of course, a strong relationship between finite automata and regular expressions, and we will see in the next section how to construct a finite automaton from a regular expression. Before we begin the study of finite automata proper, however, let us consider an elucidating example.

We will not go in depth but understand that there is an algorithm for translating a regular expression into a DFA. There also exists an algorithm for translating a DFA into a regular expression, so that the two notions are equivalent. However, because of the compactness of regular expressions, they are usually preferred to DFAs as token descriptions, and so scanner generation commonly begins with regular expressions and proceeds through the construction of a DFA to a final scanner program. For this reason, our interest will be only in an algorithm that performs this direction of the equivalence.

The simplest algorithm for translating a regular expression into a DFA proceeds via an intermediate construction, in which an NFA is derived from the regular expression, and then the NFA is used to construct an equivalent DFA. There exist algorithms that can translate a regular expression directly into a DFA, but they are more complex, and the intermediate construction is also of some interest. Thus, we concentrate on describing two algorithms, one that translates a regular expression into an NFA and a second that translates an NFA into a DFA (and sometimes a third that minimizes that DFA). Combined with one last algorithm for translating a DFA into a program, the process of constructing a scanner can be automated in these steps described below:

**regular expression** → **NFA** → **DFA** → **Minimized DFA** → **program**

This is also how we would want build a scanner if we decided to do it by hand. But since we are using Flex, we do not need to understand this process too deeply.
> 

 The generated scanner program exposes only two relevant interfaces for us to use outside the scanner program:

- `FILE *yyin` — address to the input stream which we must provide
- `int yylex()` — the tokenization function

We will use these in our main program (in the Bison file). 

Before we move on, make sure you have run the flex command above to generate the `lex.yy.c` file!

## 5. Integrating Flex and Bison

We must make a couple changes to our Bison file, `dj.y`, to integrate our `lex.yy.c` file generated with Flex into the main program:

1. At the top of our Bison fil, use the `%code provides` to include the `lex.yy.c` file:
    
    ```c
    %code provides {
      #include "lex.yy.c"
    }
    ```
    
2. Modify the main function so that it uses the `yyin` & `yylex()` provided by `*lex.yy.c`* to tokenize the input file:
    
    ```c
    int main(int argc, char *argv[]) {
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
    ```
    

The entire `dj.y` file should look like this:

```c
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

int main(int argc, char *argv[]) {
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
```

### How Bison Generates Code

When you run `bison` on our `dj.y` file, like so:

```bash
**$** bison dj.y
```

It produces a `*dj.tab.c*` file, which as of now mostly servers as the entry point to our program. Later, it will generate code that will perform parsing and syntax analysis once we’ve defined grammar rules in the Bison file while still serving as the entry point to our program.

## 6. Compilation

Make and run our compiler program using the provided `sh` script in our root project direction. It should be called `dj.sh` if we named our programming language “dj” with the init script from the previous dev log. The script & Makefile runs `flex` and `bison` for us, before compiling the entire project with gcc and executing the program.

Right now, all our program does is scan and tokenizer input source code. With the `DEBUG_SCAN` flag on, we should be see the results when we run the scanner on some of our examples:

```bash
**…/DjCompiler ❯** ./dj.sh test/good/good1.dj
CLASS ID(C) EXTENDS ID(Object) LBRACE RBRACE MAIN LBRACE NATLITERAL(0) SEMICOLON RBRACE ENDOFFILE

**…/DjCompiler ❯** ./dj.sh test/good/good3.dj
MAIN LBRACE NATTYPE ID(x) SEMICOLON ID(x) ASSIGN NATLITERAL(0) SEMICOLON ID(x) ASSIGN ID(x) PLUS NATLITERAL(1) SEMICOLON ID(x) ASSIGN ID(x) PLUS NATLITERAL(1) SEMICOLON ID(x) ASSIGN ID(x) PLUS NATLITERAL(1) SEMICOLON ID(x) ASSIGN ID(x) PLUS NATLITERAL(1) SEMICOLON PRINTNAT LPAREN ID(x) RPAREN SEMICOLON RBRACE ENDOFFILE
```
