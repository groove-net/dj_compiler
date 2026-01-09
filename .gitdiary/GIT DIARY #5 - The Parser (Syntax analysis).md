## From Token Streams to Structure

With lexical analysis complete, we now move from *tokens* to *structure*. The scanner answers the question: "What symbols exist in this program?" The parser answers a different and more powerful question: "Are these symbols arranged in a way that is meaningful for this language?"

Parsing is the stage where we impose structure on the flat stream of tokens produced by the scanner. 

Let’s say a scanner produces a linear stream of token, such as:

```
ID LBRACKET ID RBRACKET ASSIGN NUM PLUS NUM
```

By itself, this sequence has no hierarchy, precedence, or grouping. This is where the parser comes in. 

## The Parsing Process

The parser receives the source code in the form of tokens from the scanner and performs **syntax analysis**, which determines the structure of the program. This is similar to performing grammatical analysis on a sentence in a natural language. Syntax analysis determines the structural elements of the program as well as their relationships. The results of syntax analysis are usually represented as a **parse tree** or a **syntax tree**.

Here is an overview of how parsing and syntax analysis are typically performed in a compiler:

- **Grammar Rules:** The compiler has a compilation of grammar rules that defines the syntax or structure of the programming language. These rules explicate how tokens can be mixed to constitute valid language constructs like declarations, statements, and expressions. These grammer rules are formally known as Context-Free Grammers (CFG). The CFG specifies which token sequences are *valid* and how smaller syntactic units combine to form larger ones.
    
    Context-Free Grammars (CFGs) and Regular Expressions (REs) both describe formal languages, but they have different expressive power and structure. **Regular Expressions** define Regular Languages, which can be recognized by finite automata (DFA/NFA). They are limited to defining languages with simple, well-structured patterns. **Context-Free Grammars** define Context-Free Languages, which can be recognized by pushdown automata (PDA)—which are like NFAs but with a stack—and are strictly more powerful than regular languages. Every language that can be described by a regular expression can also be described by a context-free grammar. However, not every context-free language can be described by a regular expressi
    
    **This is because Regular Expressions** are flat and define patterns using concatenation, union (`|`), and repetition (`*`). **Context-Free Grammars** have recursive rules, allowing them to capture nested structures, such as balanced parentheses.
    
    While regular expressions are sufficient for tokenization, they are fundamentally inadequate for describing nested and recursive structures such as expressions, blocks, and class definitions. CFGs are explicitly designed to model this kind of hierarchical syntax, which is why essentially all programming languages are defined in terms of CFGs. Regular expressions are used for lexical analysis, pattern matching, and simple text processing, while context-free grammars are used in parsing programming languages, XML, and natural language processing.
    
- **Parsing Algorithm:** The compiler uses a parsing algorithm—such as recursive descent or LALR(1)—for analyzing the token stream and constructing the syntax tree. To determine whether the program exhibits correct syntactic structure, the parser follows these grammar rules.
- **Syntax tree construction**: The result of syntax analysis on our token stream is a **parse tree** or a **syntax tree** which represents the program’s syntactic structure as a tree-like data structure.. As an example, consider again this line of C code:
    
    ```c
    a[index] = 4 + 2
    ```
    
    It represents a structural element called an expression, which is an assignment expression consisting of a subscripted expression on the left and an integer arithmetic expression on the right. This structure can be represented as a parse tree in the following form:
    
    ![image.png](attachment:195a7683-80df-4b35-80fb-1f2f91d0537f:image.png)
    
    Note that the internal nodes of the parse tree are labeled by the names of the structures they represent and that the leaves of the parse tree represent the sequence of tokens from the input. (Names of structures are written in a different typeface to distinguish them from tokens.)
    
    A parse tree is a useful aid in visualizing the syntax of a program or program element, but it is inefficient in its representation of that structure. Parsers tend to generate a syntax tree instead, which is a condensation of the information contained in the parse tree. (Sometimes syntax trees are called **abstract syntax trees**, since they represent a further abstraction from parse trees.) An abstract syntax tree for our example of a C assignment expression is as follows:
    
    ![image.png](attachment:1576c523-4072-49cb-bcf2-4b1984c9a013:image.png)
    
    Note that in the syntax tree many of the nodes have disappeared (including token nodes). For example, if we know that an expression is a subscript operation, then it is no longer necessary to keep the brackets `[` and `]` that represent this operation in the original input.
    
- **Syntax Error Handling:** The parser gets its input from a stream of tokens delivered by the scanner and checks whether this sequence of tokens conveys correct syntax for the given language according to the set of grammar conditions. If there is a sequence of tokens that does not satisfy grammar rules, then this results in a syntax error report from the parser so that developers can see and fix such problems. The possibility of error recovery is provided by the parser to continue parsing, or it can halt and report them back to developers.

## Thinking About Language Structure Modularly

Before writing grammar rules, it is essential to reason about the *shape* of the language.

A well-designed grammar is **compositional**: small building blocks are defined first and then reused to construct larger abstractions. This makes the grammar easier to reason about, easier to extend, and far less error-prone.

For DJ, we can describe the structure of the language in layers, from the most primitive units (at the bottom) to the complete program (at the top):

```
pgm
dj
class_list
class
method_list
method
variable_declaration_list
variable_declaration
expression_list
expression
data_type
identifier
```

Each layer depends only on the layers below it. This bottom-up design mirrors how parsers actually operate and aligns naturally with LR parsing. 

Let walk through what these grammar rules will look like for some of the important layers above.

### 1. `expression`

An **expression** is the central syntactic construct in DJ. Every executable operation in the language—computation, assignment, control flow, and I/O—is modeled as an expression.

Expressions may evaluate to a value (such as a natural number or object reference), or they may exist purely for their side effects. The grammar does not enforce this distinction; that responsibility is deferred to later semantic analysis.

The DJ grammar supports the following classes of expressions:

- **Atomic expressions**
    
    These form the base cases of the recursive definition:
    
    - Null literal
        
        `null` → `NUL`
        
    - Natural number literal
        
        `5` → `NATLITERAL`
        
    - Identifier reference
        
        `x` → `ID`
        
    - `this` reference
        
        `this` → `THIS`
        
- **Object construction and access**
    - Object creation
        
        `new Obj()` → `NEW ID LPAREN RPAREN`
        
    - Field access
        
        `obj.field` → `expression DOT ID`
        
        > Notice that this is a recursive rule. we are using `expression` to define an `expression` rule.
        > 
    - Field assignment
        
        `obj.field = expr` → `expression DOT ID ASSIGN expression`
        
- **Method invocation**
    - Undotted method call
        
        `foo(e)` → `ID LPAREN expression RPAREN`
        
    - Dotted method call
        
        `obj.foo(e)` → `expression DOT ID LPAREN expression RPAREN`
        
- **Arithmetic and logical operations**
    - Addition
        
        `e1 + e2` → `expression PLUS expression`
        
    - Subtraction
        
        `e1 - e2` → `expression MINUS expression`
        
    - Multiplication
        
        `e1 * e2` → `expression TIMES expression`
        
    - Equality comparison
        
        `e1 == e2` → `expression EQUALITY expression`
        
    - Less-than comparison
        
        `e1 < e2` → `expression LESS expression`
        
    - Logical negation
        
        `!e` → `NOT expression`
        
    - Logical OR
        
        `e1 || e2` → `expression OR expression`
        
- **Control and effectful expressions**
    - Assignment
        
        `x = e` → `ID ASSIGN expression`
        
    - Assertion
        
        `assert e` → `ASSERT expression`
        
    - Conditional expression
        
        `if (e) { … } else { … }` → `IF LPAREN expression RPAREN LBRACE expression_list RBRACE ELSE LBRACE expression_list RBRACE`
        
    - Loop expression
        
        `while (e) { … }` → `WHILE LPAREN expression RPAREN LBRACE expression_list RBRACE`
        
- Built-in I/O expressions
    - Print natural number
        
        `printNat(e)` → `PRINTNAT LPAREN expression RPAREN`
        
    - Read natural number
        
        `readNat()` → `READNAT LPAREN RPAREN`
        
- Grouping
    - Parenthesized expression
        
        `(e)` → `LPAREN expression RPAREN`
        

> **Key observation:**
> 
> 
> Expressions are *recursive*. Many expression forms are defined in terms of other expressions, allowing arbitrarily deep nesting. All recursion ultimately terminates at atomic expressions such as literals, identifiers, or `this`.
> 

---

### 2. `expression_list`

Sequencing of expressions is expressed explicitly using `expression_list`. An `expression_list` represents a sequence of expressions, each terminated by a semicolon:

- Single expression `expression;`
- Multiple expressions `expression1; expression2; expression3;`

This structure is used uniformly in:

- Method bodies
- Control-flow blocks
- The `main` block

---

### 3. Variable Declarations

Variable declarations are syntactically simple and uniform:

- Declaration
    
    `type name;`→ `data_type identifier SEMICOLON`
    

These are grouped using `variable_declaration_list`, which allows one or more declarations to appear at the beginning of blocks such as methods or `main`.

The grammar intentionally separates declarations from executable expressions to avoid ambiguity and simplify parsing.

---

### 4. Methods

A **method** consists of:

- A return type
- A name
- A single parameter (typed)
- A body containing optional variable declarations followed by an expression sequence

All methods follow this general structure:

```c
return_type name(parameter_type parameter_name) {
    [variable declarations]
    expression_list
}
```

The grammar supports both natural and object types for return values and parameters, and optionally allows methods to be marked `final`.

---

### 5. Classes

DJ classes are intentionally constrained:

- Every class must extend another class
- Classes do not define constructors
- Class bodies consist of:
    - Zero or more variable declarations
    - Zero or more method definitions

The grammar allows all valid combinations of these components without requiring empty productions.

Classes may also be marked `final`, which is handled syntactically rather than semantically.

---

### 6. The `main` Block

The `main` block is mandatory and serves as the program entry point.

It consists of:

- A block delimited by braces
- An optional list of variable declarations
- A required expression sequence

There is no return value and no parameters.

---

### 7. Programs

A DJ program has a simple top-level structure:

- Zero or more class definitions
- Exactly one `main` block
- End-of-file

---

This entire structure is enforced directly by the grammar, ensuring that malformed programs are rejected during parsing rather than later compilation stages.

## Defining the Context-Free Grammar in Bison (`dj.y`)

With the grammar rules defined above, translating it into a Bison grammar becomes a mechanical process. We define this in the grammar rules section (the middle section) of the Bison file.You’ll notice we added/modified the code in the other two sections. These changes will be explained soon. The most important thing is understanding how we translated the grammar rules define above into the Bison grammar rules section.

```c
/* dj.y: PARSER for DJ */

%code provides {
  #include "lex.yy.c"

  /* Function for printing generic syntax-error messages */
  void yyerror(const char *str) {
    printf("Syntax error on line %d at token %s\n", yylineno ,yytext);
    printf("(This version of the compiler exits after finding the first syntax error.)\n");
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
        return 0;
    }
    ;

dj 
    : MAIN LBRACE expression_list RBRACE
    | MAIN LBRACE variable_declaration_list expression_list RBRACE
    | class_list MAIN LBRACE expression_list RBRACE
    | class_list MAIN LBRACE variable_declaration_list expression_list RBRACE
    ;

class_list 
    : class_list class
    | class
    ;

class
    : CLASS identifier EXTENDS identifier LBRACE RBRACE
    | CLASS identifier EXTENDS identifier LBRACE variable_declaration_list RBRACE
    | CLASS identifier EXTENDS identifier LBRACE method_list RBRACE
    | CLASS identifier EXTENDS identifier LBRACE variable_declaration_list method_list RBRACE
    | FINAL CLASS identifier EXTENDS identifier LBRACE RBRACE
    | FINAL CLASS identifier EXTENDS identifier LBRACE variable_declaration_list RBRACE
    | FINAL CLASS identifier EXTENDS identifier LBRACE method_list RBRACE
    | FINAL CLASS identifier EXTENDS identifier LBRACE variable_declaration_list method_list RBRACE
    ;

method_list 
    : method_list method
    | method
    ;

method
    : data_type identifier LPAREN data_type identifier RPAREN LBRACE expression_list RBRACE
    | data_type identifier LPAREN data_type identifier RPAREN LBRACE variable_declaration_list expression_list RBRACE
    | FINAL data_type identifier LPAREN data_type identifier RPAREN LBRACE expression_list RBRACE
    | FINAL data_type identifier LPAREN data_type identifier RPAREN LBRACE variable_declaration_list expression_list RBRACE
    ;

variable_declaration_list 
    : variable_declaration_list variable_declaration SEMICOLON
    | variable_declaration SEMICOLON
    ;

variable_declaration 
    : data_type identifier
    ;

expression_list 
    : expression_list expression SEMICOLON
    | expression SEMICOLON
    ;

expression 
    : NUL
    | NATLITERAL
    | identifier
    | THIS
    | identifier LPAREN expression RPAREN
    | NEW identifier LPAREN RPAREN
    | LPAREN expression RPAREN
    | expression DOT identifier
    | expression DOT identifier LPAREN expression RPAREN
    | expression PLUS expression
    | expression MINUS expression
    | expression TIMES expression
    | expression EQUALITY expression
    | expression LESS expression
    | NOT expression
    | expression OR expression
    | identifier ASSIGN expression
    | expression DOT identifier ASSIGN expression
    | IF LPAREN expression RPAREN LBRACE expression_list RBRACE ELSE LBRACE expression_list RBRACE
    | WHILE LPAREN expression RPAREN LBRACE expression_list RBRACE
    | ASSERT expression
    | PRINTNAT LPAREN expression RPAREN
    | READNAT LPAREN RPAREN
    ;

data_type
    : NATTYPE
    | identifier
    ;

identifier
    : ID
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

	/* parse the input program */
  yyparse();
  
  return 0;
}
```

> This grammar intentionally avoids epsilon (empty) productions wherever possible. In LR parsing, unnecessary epsilon rules are a common source of ambiguity and conflicts. Instead, optional constructs are expressed by providing explicit alternatives at higher levels of the grammar. While this can lead to longer rule lists, it results in a grammar that is easier to reason about and debug.
> 

Outside of the **Grammar Rules** section, we also made two very important changes to the file in the other sections. 

1. In the **Declaration** section, we introduce **Operator Precedence and Associativity** rules:
    
    Expressions introduce ambiguity unless precedence and associativity are explicitly specified.
    
    For example:
    
    ```c
    2 + 3 * 4
    ```
    
    Without precedence rules, the parser cannot know whether this means:
    
    ```c
    (2 + 3) * 4
    ```
    
    or:
    
    ```c
    2 + (3 * 4)
    ```
    
    In DJ, operator precedence is defined from highest to lowest as:
    
    1. Member access (`.`)
    2. Multiplication
    3. Addition and subtraction
    4. Logical negation
    5. Equality
    6. Less-than
    7. Logical OR
    8. Assignment
    9. Assertion
    
    Associativity determines how operators of the same precedence group:
    
    - Left-associative operators group left-to-right
    - Right-associative operators group right-to-left
    
    Bison allows us to declare this explicitly:
    
    ```c
    %right ASSERT
    %right ASSIGN
    %left OR
    %nonassoc LESS
    %nonassoc EQUALITY
    %right NOT
    %left PLUS MINUS
    %left TIMES
    %right DOT
    ```
    
    These declarations are not optional conveniences—they are essential to preventing shift/reduce conflicts and ensuring the grammar matches the intended semantics of the language.
    
2. In the **User Code** section, we update our main function to call `yyparse` instead of `yylex`. `yyparse` is the parser entry point provided by Bison and it called `yylex` internally.
    
    The `main` function now delegates control to the parser:
    
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
    
    	/* parse the input program */
      yyparse();
      
      return 0;
    }
    ```
    
    At this stage, parsing is purely a **validation step**. The parser does not yet build an AST or perform semantic checks—it simply verifies that the token stream conforms to the grammar.
    
    If `yyparse()` returns successfully, the program is syntactically valid.
    

### Compilation

Make and run our compiler program using the provided `sh` script in our root project direction. At this point, I recommend setting the `DEBUG_SCAN` macro to `0` in the Flex file, `dj.l`.

The program should exit after finding the first syntax error. 

```bash
**…/DjCompiler ❯** ./dj.sh tests/bad/bad6.dj
Syntax error on line 4 at token ;
(This version of the compiler exits after finding the first syntax error.)
```

At this stage, the parser does not yet build an AST. Let’s fix that.

# **Abstract Syntax Tree (AST) Representation**

The next natural step is to construct source code structure/syntax in computer memory using a tree like structure. Having a data reprensation of the source code’s structure allows us to perform steps like type checking, optimization, and code generation.

Every node in the AST represents a particular language construct such as variable declaration, function call or control flow statement. The nodes are arranged in a tree structure form where parent nodes represent higher-level constructs while children nodes represent nested or sub-constructs.

The AST become input to the following phases of compilation semantic analysis, optimization and code generation. By representing the syntactic structure of the program systematically and hierarchically, AST has made those later tasks easier while permitting various analyses and transformations to be carried out on the code.

This guide walks through the architecture of the Abstract Syntax Tree (AST) used by the DJ compiler, starting from the data structures, moving through AST construction utilities, and concluding with how the Bison grammar incrementally builds the AST during parsing.

The overall goal of this design is to:

- Represent DJ programs in a **structured, semantic form**
- Preserve **source-level information** (e.g., line numbers)
- Support **later compiler phases** such as type checking and code generation without recomputation

## Abstract syntax tree and nodes types

In the includes/ folder, create a file called `ast.h`:

```
📂 DjCompiler/
├── 📂 includes/
│   └── 📄 ast.h
└── ...
```

`ast.h`:

```c
/* File ast.h: Abstract-syntax-tree data structure for DJ */

#ifndef AST_H
#define AST_H

#include <stdlib.h>

/* define types of AST nodes */
typedef enum {
  /* program, class, field, and method declarations: */
  PROGRAM, 
  CLASS_DECL_LIST, /* class declarations */
  FINAL_CLASS_DECL,
  NONFINAL_CLASS_DECL,
  VAR_DECL_LIST, /* variable declarations */
  VAR_DECL,   
  METHOD_DECL_LIST, /* method declarations */
  FINAL_METHOD_DECL,
  NONFINAL_METHOD_DECL, 
  /* types, including generic IDs: */
  NAT_TYPE, 
  AST_ID, 
  /* expression-lists: */
  EXPR_LIST,
  /* expressions: */
  DOT_METHOD_CALL_EXPR, /* E.ID(E) */
  METHOD_CALL_EXPR,     /* ID(E) */
  DOT_ID_EXPR,          /* E.ID */
  ID_EXPR,              /* ID */
  DOT_ASSIGN_EXPR,      /* E.ID = E */
  ASSIGN_EXPR,          /* ID = E */
  PLUS_EXPR,            /* E + E */
  MINUS_EXPR,           /* E - E */
  TIMES_EXPR,           /* E * E */
  EQUALITY_EXPR,        /* E==E */
  LESS_THAN_EXPR,       /* E < E */
  NOT_EXPR,             /* !E */
  OR_EXPR,              /* E||E */
  ASSERT_EXPR,          /* assert E */
  IF_THEN_ELSE_EXPR,    /* if(E) {Es} else {Es} */
  WHILE_EXPR,           /* while(E) {Es} */
  PRINT_EXPR,           /* printNat(E) */
  READ_EXPR,            /* readNat() */
  THIS_EXPR,            /* this */
  NEW_EXPR,             /* new */
  NULL_EXPR,            /* null */
  NAT_LITERAL_EXPR,     /* N */
} ASTNodeType;

/* define a list of AST nodes */
typedef struct astlistnode {
  struct astnode *data;
  struct astlistnode *next;
} ASTList;

/* define the actual AST nodes */
typedef struct astnode {
  ASTNodeType typ;
  /* list of children nodes: */
  ASTList *children; /* head of the list of children */
  ASTList *childrenTail;
  /* which source-program line does this node end on: */
  unsigned int lineNumber;
  /* node attributes: */
  unsigned int natVal;
  char *idVal;
  /* Node attributes used on the first 6 kinds of expressions enumerated above
    (E.ID(E), ID(E), E.ID, ID, E.ID = E, and ID = E).
    These attributes get set during type checking and used during code gen,
    so code gen doesn't duplicate the work of the type checker. 
    The attributes store the statically determined class and member number
    of an ID that refers to a method or variable.  
    When these attributes are all 0, the ID refers not to a member of a class
    but instead to a local/parameter variable. */
  unsigned int staticClassNum; /* class number in which this member resides */
  unsigned int staticMemberNum; /* when set to i, this member is the ith 
                                   method/var in the staticClassNum-th class */
} ASTree;

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
  char *idAttribute, unsigned int lineNum);

/* Append an AST node onto a parent's list of children */
void appendToChildrenList(ASTree *parent, ASTree *newChild);

/* Print the AST to stdout with indentations marking tree depth. */
void printAST(ASTree *t);

/* Get identifier */
char *getID(const char *src);

#endif
```

The main take away from the code above is the `ASTNodeType` enumeration. This enumeration defines every syntactic construct that can appear in a DJ program. Each AST node has exactly one `ASTNodeType` indicating what kind of construct it represents. Defining all your AST node types in the `ASTNodeType` enumeration is the most important part of this entire stage. Once you do that, the rest of this stage is easy and almost mechanical, meaning that the rest of the code and how it is applied does not change. 

Every rule in the grammer rules should be mapped to a node type. A lot of the rules will map to the same type.

### How to Think About Declaring AST Nodes

The Abstract Syntax Tree (AST) exists to represent a source program as a structured hierarchy of meaning. Its role is not to mirror the grammar or the token stream, but to encode the *semantic shape* of the program in a form that later compiler phases can reason about.

An AST should be understood as a distilled version of the source code. Everything that survives into the AST must be something that type checking, name resolution, control-flow analysis, or code generation will need. Everything else—keywords, punctuation, precedence rules, and parsing scaffolding—must disappear during AST generation. If a construct exists only to make the grammar work, it does not belong in the AST.

Each AST node must earn its place by representing a semantically distinct construct. If removing a node would not cause loss of information or ambiguity during later phases, then that node should not exist. This principle keeps the AST small, stable, and easy to reason about.

---

### Semantic Meaning as the Primary Criterion

AST nodes should be introduced only for constructs that have semantic significance. A construct has semantic significance if it influences typing rules, scoping, evaluation order, control flow, or runtime behavior. For example, an `if–then–else` expression fundamentally alters control flow and therefore deserves its own node. A binary arithmetic expression similarly represents a semantic operation and must be preserved. By contrast, parentheses exist only to guide parsing and do not carry meaning beyond grouping; they should never appear as AST nodes.

In practice, this means that AST nodes should correspond to *language concepts*, not grammar productions. A grammar rule may expand into multiple tokens and helper nonterminals, but all of that may collapse into a single AST node if the meaning is singular. Each grammer rule resolves to a single AST node. Only important tokens within the rule will become children of the node.

---

### Thinking in Terms of Ownership

A useful mental model is to think of AST nodes in terms of **ownership**. Each node should own the information and substructures that define that construct. A method declaration owns its return type, name, parameters, and body. A class declaration owns its name, superclass, fields, and methods. An expression owns its operands or subexpressions.

This ownership naturally induces a tree structure. Parent nodes represent larger semantic constructs, while children represent the components that define them. If a piece of information does not clearly belong to a construct, it is likely misplaced or unnecessary.

---

### Ordered Children and Fixed Structure

Children of an AST node are not arbitrary; their order and position carry meaning. For example, in an assignment expression, the left-hand side and right-hand side must be distinguished and consistently ordered. The AST should make this structure explicit rather than implicit.

```c
ASSIGN_EXPR  /* ID = E */
├──left-hand side // Stores information about the ID
└──right-hand side // Stores information about the expression
```

## Abstract syntax tree Implementation

Then, In the `src/` folder, create a folder called `ast/` and within that folder create a file called `ast.c`:

```
📂 DjCompiler/
├── 📂 src/
│   ├── 📂 ast/
│   │   └── 📄 ast.c
│   └── ...
└── ...
```

`ast.c`:

```c
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
ASTree *newAST(ASTNodeType t, ASTree *child, unsigned int natAttribute, char *idAttribute, unsigned int lineNum)
{
    ASTList *childNode = (ASTList *) malloc(sizeof(ASTList));
    childNode->data = child;
    childNode->next = NULL;
    
    ASTree *root = (ASTree *) malloc(sizeof(ASTree));
    root->typ = t;
    root->natVal = natAttribute;
    root->idVal = idAttribute;
    root->lineNumber = lineNum;
    root->children = childNode;
    root->childrenTail = childNode;
    
    return root;
}
  
/* Append an AST node onto a parent's list of children */
void appendToChildrenList(ASTree *parent, ASTree *newChild)
{
    ASTList *newChildNode = (ASTList *) malloc(sizeof(ASTList));
    newChildNode->data = newChild;
    newChildNode->next = NULL;
    
    parent->childrenTail->next = newChildNode;
    parent->childrenTail = newChildNode;
}

void printASTPreorder(ASTree *t, int level)
{
    if (t == NULL) return;
    
    for (int i = 0; i < level; i++) printf("  ");
    switch(t->typ)
    {
        
        case PROGRAM: printf("PROGRAM"); break;
        case CLASS_DECL_LIST: printf("CLASS_DECL_LIST"); break;
        case FINAL_CLASS_DECL: printf("FINAL_CLASS_DECL"); break;
        case NONFINAL_CLASS_DECL: printf("NONFINAL_CLASS_DECL"); break;
        case VAR_DECL_LIST: printf("VAR_DECL_LIST"); break;
        case VAR_DECL: printf("VAR_DECL"); break;
        case METHOD_DECL_LIST: printf("METHOD_DECL_LIST"); break;
        case FINAL_METHOD_DECL: printf("FINAL_METHOD_DECL"); break;
        case NONFINAL_METHOD_DECL: printf("NONFINAL_METHOD_DECL"); break;
        case NAT_TYPE: printf("NAT_TYPE"); break;
        case AST_ID: printf("AST_ID(%s)", t->idVal); break;
        case EXPR_LIST: printf("EXPR_LIST"); break;
        case DOT_METHOD_CALL_EXPR: printf("DOT_METHOD_CALL_EXPR"); break;
        case METHOD_CALL_EXPR: printf("METHOD_CALL_EXPR"); break;
        case DOT_ID_EXPR: printf("DOT_ID_EXPR"); break;
        case ID_EXPR: printf("ID_EXPR"); break;
        case DOT_ASSIGN_EXPR: printf("DOT_ASSIGN_EXPR"); break;
        case ASSIGN_EXPR: printf("ASSIGN_EXPR"); break;
        case PLUS_EXPR: printf("PLUS_EXPR"); break;
        case MINUS_EXPR: printf("MINUS_EXPR"); break;
        case TIMES_EXPR: printf("TIMES_EXPR"); break;
        case EQUALITY_EXPR: printf("EQUALITY_EXPR"); break;
        case LESS_THAN_EXPR: printf("LESS_THAN_EXPR"); break;
        case NOT_EXPR: printf("NOT_EXPR"); break;
        case OR_EXPR: printf("OR_EXPR"); break;
        case ASSERT_EXPR: printf("ASSERT_EXPR"); break;
        case IF_THEN_ELSE_EXPR: printf("IF_THEN_ELSE_EXPR"); break;
        case WHILE_EXPR: printf("WHILE_EXPR"); break;
        case PRINT_EXPR: printf("PRINT_EXPR"); break;
        case READ_EXPR: printf("READ_EXPR"); break;
        case THIS_EXPR: printf("THIS_EXPR"); break;
        case NEW_EXPR: printf("NEW_EXPR"); break;
        case NULL_EXPR: printf("NULL_EXPR"); break;
        case NAT_LITERAL_EXPR: printf("NAT_LITERAL_EXPR(%d)", t->natVal); break;
        default: printf("--- error occured ---");
    }
    if (t->lineNumber == 0) printf("   (N/A)\n");
    else printf("   (ends on line %d)\n", t->lineNumber);
    
    // Incrememt tree level
    level++;
    
    // Recurse on children from left to right
    ASTList *firstchild = t->children;
    while (firstchild != t->childrenTail)
    {
        printASTPreorder(firstchild->data, level);
        firstchild = firstchild->next;
    }
    printASTPreorder(firstchild->data, level);
}

/* Print the AST to stdout with indentations marking tree depth. */
void printAST(ASTree *t)
{
    printASTPreorder(t, 0);
}

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
```

## Generate AST using Bison

1. Include `"symtbl.h"` & `"typecheck.h"` in `%code provides {…}` in the **Declaration** section of our Bison file
    
    ```c
    %code provides {
      // ...
      #include "../include/ast.h"
      
      #define DEBUG_AST 1
    
      /* Symbols in this grammer are represented as ASTs */
      #define YYSTYPE ASTree *
    
      /* Declare global AST for entire program */
      ASTree *pgmAST;
      
      // ...
    }
    ```
    
2. Here is where we do majority of the work in building our Abstract Syntax Tree. To build your Abstract Syntax Tree (AST) in Bison, you use the action blocks `{ ... }` inside your grammar rules to construct nodes and link them together as the parser recognizes structure. The function `newAST` acts as your node creator, allocating memory for a generic tree node with a specific type (like `PLUS_EXPR` or `CLASS_DECL`), while `appendToChildrenList` acts as your "gluel," taking a parent node and attaching a child node to its list of sub-elements. Inside these actions, Bison uses special variables to represent data: `$1`, `$2`, etc., correspond to the AST nodes already created for the first, second, etc., symbols on the right side of your rule, while `$$` represents the AST node you are currently building for the left side. Therefore, to implement a rule like 
    
    ```c
    expr: expr PLUS expr
    ```
    
    , you would write
    
    ```c
    { 
    	$$ = newAST(PLUS_EXPR, $1, 0, NULL, yylineno); 
    	appendToChildrenList($$, $3);
    }
    ```
    
    , which effectively says, "Set the result of this rule (`$$`) to be a new Plus node, then attach the left operand (`$1`) and the right operand (`$3`) as its children."
    
    We do this for every grammar rule and Bison turns each rule found into a node in the syntax tree. Study the **Grammar Rules** section below to learn more.
    
3. Setup symbol table and type check program in our `main` function in the **User Code** section using the following code snippet.
    
    ```c
    int main(int argc, char *argv[]) {
    	// ... (Your existing code) ... 
      
      /* parse the input program */
      yyparse();
      if (DEBUG_AST) {
        printf("****** begin abstract syntax tree for DJ program ******\n");
        printAST(pgmAST);
        printf("****** end abstract syntax tree for DJ program ******\n");
      }
      
      return 0;
    }
    ```
    

Our entire Bison file, `dj.l`, should now look like this:

```c
/* dj.y: PARSER for DJ */

%code provides {
  #include "lex.yy.c"
  #include "../include/ast.h"
  
  #define DEBUG_AST 1

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

  return 0;
}
```

The grammar ensures that **every DJ program produces exactly one `PROGRAM` node**.

Children of `PROGRAM` are always, in order:

1. `CLASS_DECL_LIST`
2. `VAR_DECL_LIST` (main variables)
3. `EXPR_LIST` (main body)

This invariant simplifies later compiler passes.

### Compilation

Make and run our compiler program using the provided `sh` script in our root project direction. At this point, I recommend setting the `DEBUG_SCAN` macro to `0` in the Flex file, `dj.l`.

The program will now construct an abstract syntax tree based on your program’s structure. With the `DEBUG_AST` macro set to `1`, we should be able to visualize the tree in the terminal. The program should exit after finding the first syntax error.

```bash
**…/DjCompiler ❯** ./dj.sh tests/good/good1.dj
****** begin abstract syntax tree for DJ program ******
PROGRAM   (ends on line 4)
  CLASS_DECL_LIST   (ends on line 1)
    NONFINAL_CLASS_DECL   (ends on line 1)
      AST_ID(C)   (ends on line 1)
      AST_ID(Object)   (ends on line 1)
      VAR_DECL_LIST   (N/A)
      METHOD_DECL_LIST   (N/A)
  VAR_DECL_LIST   (N/A)
  EXPR_LIST   (ends on line 3)
    NAT_LITERAL_EXPR(0)   (ends on line 3)
****** end abstract syntax tree for DJ program ******

**…/DjCompiler ❯** ./dj.sh tests/good/good3.dj
****** begin abstract syntax tree for DJ program ******
PROGRAM   (ends on line 11)
  CLASS_DECL_LIST   (N/A)
  VAR_DECL_LIST   (ends on line 3)
    VAR_DECL   (ends on line 3)
      NAT_TYPE   (ends on line 3)
      AST_ID(x)   (ends on line 3)
  EXPR_LIST   (ends on line 4)
    ASSIGN_EXPR   (ends on line 4)
      AST_ID(x)   (ends on line 4)
      NAT_LITERAL_EXPR(0)   (ends on line 4)
    ASSIGN_EXPR   (ends on line 5)
      AST_ID(x)   (ends on line 5)
      PLUS_EXPR   (ends on line 5)
        ID_EXPR   (ends on line 5)
          AST_ID(x)   (ends on line 5)
        NAT_LITERAL_EXPR(1)   (ends on line 5)
    ASSIGN_EXPR   (ends on line 6)
      AST_ID(x)   (ends on line 6)
      PLUS_EXPR   (ends on line 6)
        ID_EXPR   (ends on line 6)
          AST_ID(x)   (ends on line 6)
        NAT_LITERAL_EXPR(1)   (ends on line 6)
    ASSIGN_EXPR   (ends on line 7)
      AST_ID(x)   (ends on line 7)
      PLUS_EXPR   (ends on line 7)
        ID_EXPR   (ends on line 7)
          AST_ID(x)   (ends on line 7)
        NAT_LITERAL_EXPR(1)   (ends on line 7)
    ASSIGN_EXPR   (ends on line 8)
      AST_ID(x)   (ends on line 8)
      PLUS_EXPR   (ends on line 8)
        ID_EXPR   (ends on line 8)
          AST_ID(x)   (ends on line 8)
        NAT_LITERAL_EXPR(1)   (ends on line 8)
    PRINT_EXPR   (ends on line 9)
      NAT_LITERAL_EXPR(5)   (ends on line 9)
    PRINT_EXPR   (ends on line 10)
      ID_EXPR   (ends on line 10)
        AST_ID(x)   (ends on line 10)
****** end abstract syntax tree for DJ program ******
```
