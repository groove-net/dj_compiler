/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1





# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    FINAL = 258,                   /* FINAL  */
    CLASS = 259,                   /* CLASS  */
    ID = 260,                      /* ID  */
    EXTENDS = 261,                 /* EXTENDS  */
    MAIN = 262,                    /* MAIN  */
    NATTYPE = 263,                 /* NATTYPE  */
    NATLITERAL = 264,              /* NATLITERAL  */
    PRINTNAT = 265,                /* PRINTNAT  */
    READNAT = 266,                 /* READNAT  */
    PLUS = 267,                    /* PLUS  */
    MINUS = 268,                   /* MINUS  */
    TIMES = 269,                   /* TIMES  */
    EQUALITY = 270,                /* EQUALITY  */
    LESS = 271,                    /* LESS  */
    ASSERT = 272,                  /* ASSERT  */
    OR = 273,                      /* OR  */
    NOT = 274,                     /* NOT  */
    IF = 275,                      /* IF  */
    ELSE = 276,                    /* ELSE  */
    WHILE = 277,                   /* WHILE  */
    ASSIGN = 278,                  /* ASSIGN  */
    NUL = 279,                     /* NUL  */
    NEW = 280,                     /* NEW  */
    THIS = 281,                    /* THIS  */
    DOT = 282,                     /* DOT  */
    SEMICOLON = 283,               /* SEMICOLON  */
    LBRACE = 284,                  /* LBRACE  */
    RBRACE = 285,                  /* RBRACE  */
    LPAREN = 286,                  /* LPAREN  */
    RPAREN = 287,                  /* RPAREN  */
    ENDOFFILE = 288                /* ENDOFFILE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (void);

/* "%code provides" blocks.  */
#line 3 "src/dj.y"

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

#line 184 "src/dj.tab.c"


/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_FINAL = 3,                      /* FINAL  */
  YYSYMBOL_CLASS = 4,                      /* CLASS  */
  YYSYMBOL_ID = 5,                         /* ID  */
  YYSYMBOL_EXTENDS = 6,                    /* EXTENDS  */
  YYSYMBOL_MAIN = 7,                       /* MAIN  */
  YYSYMBOL_NATTYPE = 8,                    /* NATTYPE  */
  YYSYMBOL_NATLITERAL = 9,                 /* NATLITERAL  */
  YYSYMBOL_PRINTNAT = 10,                  /* PRINTNAT  */
  YYSYMBOL_READNAT = 11,                   /* READNAT  */
  YYSYMBOL_PLUS = 12,                      /* PLUS  */
  YYSYMBOL_MINUS = 13,                     /* MINUS  */
  YYSYMBOL_TIMES = 14,                     /* TIMES  */
  YYSYMBOL_EQUALITY = 15,                  /* EQUALITY  */
  YYSYMBOL_LESS = 16,                      /* LESS  */
  YYSYMBOL_ASSERT = 17,                    /* ASSERT  */
  YYSYMBOL_OR = 18,                        /* OR  */
  YYSYMBOL_NOT = 19,                       /* NOT  */
  YYSYMBOL_IF = 20,                        /* IF  */
  YYSYMBOL_ELSE = 21,                      /* ELSE  */
  YYSYMBOL_WHILE = 22,                     /* WHILE  */
  YYSYMBOL_ASSIGN = 23,                    /* ASSIGN  */
  YYSYMBOL_NUL = 24,                       /* NUL  */
  YYSYMBOL_NEW = 25,                       /* NEW  */
  YYSYMBOL_THIS = 26,                      /* THIS  */
  YYSYMBOL_DOT = 27,                       /* DOT  */
  YYSYMBOL_SEMICOLON = 28,                 /* SEMICOLON  */
  YYSYMBOL_LBRACE = 29,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 30,                    /* RBRACE  */
  YYSYMBOL_LPAREN = 31,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 32,                    /* RPAREN  */
  YYSYMBOL_ENDOFFILE = 33,                 /* ENDOFFILE  */
  YYSYMBOL_YYACCEPT = 34,                  /* $accept  */
  YYSYMBOL_pgm = 35,                       /* pgm  */
  YYSYMBOL_dj = 36,                        /* dj  */
  YYSYMBOL_class_list = 37,                /* class_list  */
  YYSYMBOL_class = 38,                     /* class  */
  YYSYMBOL_method_list = 39,               /* method_list  */
  YYSYMBOL_method = 40,                    /* method  */
  YYSYMBOL_variable_declaration_list = 41, /* variable_declaration_list  */
  YYSYMBOL_variable_declaration = 42,      /* variable_declaration  */
  YYSYMBOL_expression_list = 43,           /* expression_list  */
  YYSYMBOL_expression = 44,                /* expression  */
  YYSYMBOL_data_type = 45,                 /* data_type  */
  YYSYMBOL_identifier = 46                 /* identifier  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   595

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  34
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  13
/* YYNRULES -- Number of rules.  */
#define YYNRULES  53
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  154

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   288


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    50,    50,    57,    62,    67,    72,    81,    85,    91,
      97,   103,   109,   115,   121,   127,   133,   142,   146,   152,
     160,   168,   176,   187,   191,   197,   204,   208,   214,   217,
     220,   223,   226,   230,   233,   236,   240,   245,   249,   253,
     257,   261,   265,   268,   272,   276,   281,   286,   290,   293,
     296,   302,   305,   311
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "FINAL", "CLASS", "ID",
  "EXTENDS", "MAIN", "NATTYPE", "NATLITERAL", "PRINTNAT", "READNAT",
  "PLUS", "MINUS", "TIMES", "EQUALITY", "LESS", "ASSERT", "OR", "NOT",
  "IF", "ELSE", "WHILE", "ASSIGN", "NUL", "NEW", "THIS", "DOT",
  "SEMICOLON", "LBRACE", "RBRACE", "LPAREN", "RPAREN", "ENDOFFILE",
  "$accept", "pgm", "dj", "class_list", "class", "method_list", "method",
  "variable_declaration_list", "variable_declaration", "expression_list",
  "expression", "data_type", "identifier", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-90)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-53)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     117,    -3,     5,   -18,    20,    -6,   126,   -90,     5,   -90,
      23,   214,   -90,   -90,     7,   -90,    34,     5,   -90,   -90,
      22,    41,   445,   445,    43,    49,   -90,     5,   -90,   445,
     214,    60,   247,   528,     5,    -1,   214,     5,    61,   445,
      62,   552,    10,    85,   445,   445,    71,   169,    65,   265,
     -90,   -90,   545,   445,   445,   445,   445,   445,   445,     5,
     -90,   -90,   445,   445,   214,   283,    72,     9,   465,   -90,
     472,   493,    74,   -90,   -90,   -90,   -90,    -9,    -9,    80,
      69,   562,   568,    33,   552,   500,   301,   -90,    16,    63,
     -90,    39,   -90,    40,     5,   -90,   -90,    86,    88,   -90,
     445,   445,   -90,   -90,   -90,    46,    47,     5,   -90,   -90,
       5,   -90,    55,    78,   445,   445,   552,   521,   -90,   -90,
      70,    83,    78,   -90,    63,   319,   337,   -90,   -90,    63,
       5,   102,   -90,     5,   104,   105,   107,   108,   445,   114,
     214,   355,   214,   214,   373,   -90,   214,   391,   409,   -90,
     427,   -90,   -90,   -90
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     8,     0,    53,
       0,     0,     1,     2,     0,     7,     0,     0,    51,    29,
       0,     0,     0,     0,     0,     0,    28,     0,    31,     0,
       0,     0,     0,     0,     0,    30,     0,     0,     0,     0,
       0,    48,    30,    42,     0,     0,     0,     0,     0,     0,
      24,     3,     0,     0,     0,     0,     0,     0,     0,     0,
      27,    25,     0,     0,     0,     0,     0,     0,     0,    50,
       0,     0,     0,    34,    23,     4,    26,    37,    38,    39,
      40,    41,    43,    35,    44,     0,     0,     5,     0,     0,
       9,     0,    18,     0,     0,    52,    49,     0,     0,    33,
       0,     0,    32,     6,    13,     0,     0,     0,    11,    17,
       0,    10,     0,    25,     0,     0,    45,     0,    15,    14,
       0,     0,     0,    12,     0,     0,     0,    36,    16,     0,
       0,     0,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    46,     0,     0,     0,    19,
       0,    21,    20,    22
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -90,   -90,   -90,   -90,   143,   -80,   -89,   -29,   -27,    31,
     103,    87,    -2
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     4,     5,     6,     7,    91,    92,    30,    31,    32,
      33,    34,    42
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      10,     8,   109,    48,   -52,    55,    16,    64,   105,    35,
       9,    11,    89,   112,     9,    38,   109,    18,    59,    89,
      12,     9,    62,   109,    18,    46,   120,    13,    35,    17,
      63,   109,    61,    62,    35,    66,    36,    48,    93,    90,
      37,    63,    89,    89,     9,     9,   104,    18,    18,    89,
      89,     9,     9,    39,    18,    18,   100,    83,    89,   106,
       9,    49,    35,    18,   101,    95,    48,    65,     9,   108,
     111,    18,    40,    89,    44,     9,   118,   119,    18,    48,
      45,    53,    54,    55,   -53,   123,    95,    95,    50,    95,
      67,    95,   113,    74,    69,    86,    59,    53,    54,    55,
     128,    88,    72,    95,    95,   121,    99,    59,   122,   124,
      95,   143,    59,   146,   129,   114,    48,   115,    95,    48,
       1,     2,    95,   135,     3,    41,    43,    95,   134,     1,
       2,   136,    47,    14,   138,    52,   137,   140,    35,   139,
      35,    35,    68,   142,    35,   125,   126,    70,    71,    15,
       0,     0,    52,     0,    94,     0,    77,    78,    79,    80,
      81,    82,     0,     0,     0,    84,    85,     0,    52,   141,
       0,   144,     0,   147,   148,    94,   107,   150,   110,     0,
      94,    53,    54,    55,    56,    57,     0,    58,     0,    52,
       0,     0,   110,    94,     0,     0,    59,     0,     0,   110,
       0,    73,     0,   116,   117,     0,     0,   110,     0,     0,
       0,   130,     0,     0,     0,     0,   133,     0,     0,     9,
       0,     0,    18,    19,    20,    21,     0,     0,    52,    52,
       0,    22,     0,    23,    24,     0,    25,     0,    26,    27,
      28,     0,     0,     0,    52,    29,     0,    52,     0,     0,
      52,    52,     9,    52,     0,     0,    19,    20,    21,     0,
       0,     0,     0,     0,    22,     0,    23,    24,     0,    25,
       9,    26,    27,    28,    19,    20,    21,    51,    29,     0,
       0,     0,    22,     0,    23,    24,     0,    25,     9,    26,
      27,    28,    19,    20,    21,    75,    29,     0,     0,     0,
      22,     0,    23,    24,     0,    25,     9,    26,    27,    28,
      19,    20,    21,    87,    29,     0,     0,     0,    22,     0,
      23,    24,     0,    25,     9,    26,    27,    28,    19,    20,
      21,   103,    29,     0,     0,     0,    22,     0,    23,    24,
       0,    25,     9,    26,    27,    28,    19,    20,    21,   131,
      29,     0,     0,     0,    22,     0,    23,    24,     0,    25,
       9,    26,    27,    28,    19,    20,    21,   132,    29,     0,
       0,     0,    22,     0,    23,    24,     0,    25,     9,    26,
      27,    28,    19,    20,    21,   145,    29,     0,     0,     0,
      22,     0,    23,    24,     0,    25,     9,    26,    27,    28,
      19,    20,    21,   149,    29,     0,     0,     0,    22,     0,
      23,    24,     0,    25,     9,    26,    27,    28,    19,    20,
      21,   151,    29,     0,     0,     0,    22,     0,    23,    24,
       0,    25,     9,    26,    27,    28,    19,    20,    21,   152,
      29,     0,     0,     0,    22,     0,    23,    24,     0,    25,
       9,    26,    27,    28,    19,    20,    21,   153,    29,     0,
       0,     0,    22,     0,    23,    24,     0,    25,     0,    26,
      27,    28,     0,     0,     0,     0,    29,    53,    54,    55,
      56,    57,     0,    58,    53,    54,    55,    56,    57,     0,
      58,     0,    59,     0,     0,     0,     0,    96,     0,    59,
       0,     0,     0,     0,    97,    53,    54,    55,    56,    57,
       0,    58,    53,    54,    55,    56,    57,     0,    58,     0,
      59,     0,     0,     0,     0,    98,     0,    59,     0,     0,
       0,     0,   102,    53,    54,    55,    56,    57,     0,    58,
      53,    54,    55,    56,    57,     0,    58,     0,    59,     0,
       0,     0,     0,   127,     0,    59,    60,    53,    54,    55,
      56,    57,     0,    58,    53,    54,    55,    56,    57,     0,
      58,     0,    59,    76,    53,    54,    55,    56,   -53,    59,
      53,    54,    55,    56,    57,     0,     0,     0,     0,    59,
       0,     0,     0,     0,     0,    59
};

static const yytype_int16 yycheck[] =
{
       2,     4,    91,    30,     5,    14,     8,    36,    88,    11,
       5,    29,     3,    93,     5,    17,   105,     8,    27,     3,
       0,     5,    23,   112,     8,    27,   106,    33,    30,     6,
      31,   120,    34,    23,    36,    37,    29,    64,    67,    30,
       6,    31,     3,     3,     5,     5,    30,     8,     8,     3,
       3,     5,     5,    31,     8,     8,    23,    59,     3,    88,
       5,    30,    64,     8,    31,    67,    93,    36,     5,    30,
      30,     8,    31,     3,    31,     5,    30,    30,     8,   106,
      31,    12,    13,    14,    15,    30,    88,    89,    28,    91,
      29,    93,    94,    28,    32,    64,    27,    12,    13,    14,
      30,    29,    31,   105,   106,   107,    32,    27,   110,    31,
     112,   140,    27,   142,    31,    29,   143,    29,   120,   146,
       3,     4,   124,    21,     7,    22,    23,   129,   130,     3,
       4,   133,    29,     7,    29,    32,    32,    29,   140,    32,
     142,   143,    39,    29,   146,   114,   115,    44,    45,     6,
      -1,    -1,    49,    -1,    67,    -1,    53,    54,    55,    56,
      57,    58,    -1,    -1,    -1,    62,    63,    -1,    65,   138,
      -1,   140,    -1,   142,   143,    88,    89,   146,    91,    -1,
      93,    12,    13,    14,    15,    16,    -1,    18,    -1,    86,
      -1,    -1,   105,   106,    -1,    -1,    27,    -1,    -1,   112,
      -1,    32,    -1,   100,   101,    -1,    -1,   120,    -1,    -1,
      -1,   124,    -1,    -1,    -1,    -1,   129,    -1,    -1,     5,
      -1,    -1,     8,     9,    10,    11,    -1,    -1,   125,   126,
      -1,    17,    -1,    19,    20,    -1,    22,    -1,    24,    25,
      26,    -1,    -1,    -1,   141,    31,    -1,   144,    -1,    -1,
     147,   148,     5,   150,    -1,    -1,     9,    10,    11,    -1,
      -1,    -1,    -1,    -1,    17,    -1,    19,    20,    -1,    22,
       5,    24,    25,    26,     9,    10,    11,    30,    31,    -1,
      -1,    -1,    17,    -1,    19,    20,    -1,    22,     5,    24,
      25,    26,     9,    10,    11,    30,    31,    -1,    -1,    -1,
      17,    -1,    19,    20,    -1,    22,     5,    24,    25,    26,
       9,    10,    11,    30,    31,    -1,    -1,    -1,    17,    -1,
      19,    20,    -1,    22,     5,    24,    25,    26,     9,    10,
      11,    30,    31,    -1,    -1,    -1,    17,    -1,    19,    20,
      -1,    22,     5,    24,    25,    26,     9,    10,    11,    30,
      31,    -1,    -1,    -1,    17,    -1,    19,    20,    -1,    22,
       5,    24,    25,    26,     9,    10,    11,    30,    31,    -1,
      -1,    -1,    17,    -1,    19,    20,    -1,    22,     5,    24,
      25,    26,     9,    10,    11,    30,    31,    -1,    -1,    -1,
      17,    -1,    19,    20,    -1,    22,     5,    24,    25,    26,
       9,    10,    11,    30,    31,    -1,    -1,    -1,    17,    -1,
      19,    20,    -1,    22,     5,    24,    25,    26,     9,    10,
      11,    30,    31,    -1,    -1,    -1,    17,    -1,    19,    20,
      -1,    22,     5,    24,    25,    26,     9,    10,    11,    30,
      31,    -1,    -1,    -1,    17,    -1,    19,    20,    -1,    22,
       5,    24,    25,    26,     9,    10,    11,    30,    31,    -1,
      -1,    -1,    17,    -1,    19,    20,    -1,    22,    -1,    24,
      25,    26,    -1,    -1,    -1,    -1,    31,    12,    13,    14,
      15,    16,    -1,    18,    12,    13,    14,    15,    16,    -1,
      18,    -1,    27,    -1,    -1,    -1,    -1,    32,    -1,    27,
      -1,    -1,    -1,    -1,    32,    12,    13,    14,    15,    16,
      -1,    18,    12,    13,    14,    15,    16,    -1,    18,    -1,
      27,    -1,    -1,    -1,    -1,    32,    -1,    27,    -1,    -1,
      -1,    -1,    32,    12,    13,    14,    15,    16,    -1,    18,
      12,    13,    14,    15,    16,    -1,    18,    -1,    27,    -1,
      -1,    -1,    -1,    32,    -1,    27,    28,    12,    13,    14,
      15,    16,    -1,    18,    12,    13,    14,    15,    16,    -1,
      18,    -1,    27,    28,    12,    13,    14,    15,    16,    27,
      12,    13,    14,    15,    16,    -1,    -1,    -1,    -1,    27,
      -1,    -1,    -1,    -1,    -1,    27
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     7,    35,    36,    37,    38,     4,     5,
      46,    29,     0,    33,     7,    38,    46,     6,     8,     9,
      10,    11,    17,    19,    20,    22,    24,    25,    26,    31,
      41,    42,    43,    44,    45,    46,    29,     6,    46,    31,
      31,    44,    46,    44,    31,    31,    46,    44,    42,    43,
      28,    30,    44,    12,    13,    14,    15,    16,    18,    27,
      28,    46,    23,    31,    41,    43,    46,    29,    44,    32,
      44,    44,    31,    32,    28,    30,    28,    44,    44,    44,
      44,    44,    44,    46,    44,    44,    43,    30,    29,     3,
      30,    39,    40,    41,    45,    46,    32,    32,    32,    32,
      23,    31,    32,    30,    30,    39,    41,    45,    30,    40,
      45,    30,    39,    46,    29,    29,    44,    44,    30,    30,
      39,    46,    46,    30,    31,    43,    43,    32,    30,    31,
      45,    30,    30,    45,    46,    21,    46,    32,    29,    32,
      29,    43,    29,    41,    43,    30,    41,    43,    43,    30,
      43,    30,    30,    30
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    34,    35,    36,    36,    36,    36,    37,    37,    38,
      38,    38,    38,    38,    38,    38,    38,    39,    39,    40,
      40,    40,    40,    41,    41,    42,    43,    43,    44,    44,
      44,    44,    44,    44,    44,    44,    44,    44,    44,    44,
      44,    44,    44,    44,    44,    44,    44,    44,    44,    44,
      44,    45,    45,    46
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     4,     5,     5,     6,     2,     1,     6,
       7,     7,     8,     7,     8,     8,     9,     2,     1,     9,
      10,    10,    11,     3,     2,     2,     3,     2,     1,     1,
       1,     1,     4,     4,     3,     3,     6,     3,     3,     3,
       3,     3,     2,     3,     3,     5,    11,     7,     2,     4,
       3,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* pgm: dj ENDOFFILE  */
#line 50 "src/dj.y"
                   {
        pgmAST = yyvsp[-1];
        return 0;
    }
#line 1374 "src/dj.tab.c"
    break;

  case 3: /* dj: MAIN LBRACE expression_list RBRACE  */
#line 57 "src/dj.y"
                                         {
        yyval = newAST(PROGRAM, newAST(CLASS_DECL_LIST, NULL, 0, NULL, 0), 0, NULL, yylineno);
        appendToChildrenList(yyval, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1384 "src/dj.tab.c"
    break;

  case 4: /* dj: MAIN LBRACE variable_declaration_list expression_list RBRACE  */
#line 62 "src/dj.y"
                                                                   {
        yyval = newAST(PROGRAM, newAST(CLASS_DECL_LIST, NULL, 0, NULL, 0), 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-2]);
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1394 "src/dj.tab.c"
    break;

  case 5: /* dj: class_list MAIN LBRACE expression_list RBRACE  */
#line 67 "src/dj.y"
                                                    {
        yyval = newAST(PROGRAM, yyvsp[-4], 0, NULL, yylineno);
        appendToChildrenList(yyval, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1404 "src/dj.tab.c"
    break;

  case 6: /* dj: class_list MAIN LBRACE variable_declaration_list expression_list RBRACE  */
#line 72 "src/dj.y"
                                                                              {
        yyval = newAST(PROGRAM, yyvsp[-5], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-2]);
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1414 "src/dj.tab.c"
    break;

  case 7: /* class_list: class_list class  */
#line 81 "src/dj.y"
                       {
        yyval = yyvsp[-1];
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1423 "src/dj.tab.c"
    break;

  case 8: /* class_list: class  */
#line 85 "src/dj.y"
            {
        yyval = newAST(CLASS_DECL_LIST, yyvsp[0], 0, NULL, yylineno);
    }
#line 1431 "src/dj.tab.c"
    break;

  case 9: /* class: CLASS identifier EXTENDS identifier LBRACE RBRACE  */
#line 91 "src/dj.y"
                                                        {
        yyval = newAST(NONFINAL_CLASS_DECL, yyvsp[-4], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-2]);
        appendToChildrenList(yyval, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList(yyval, newAST(METHOD_DECL_LIST, NULL, 0, NULL, 0));
    }
#line 1442 "src/dj.tab.c"
    break;

  case 10: /* class: CLASS identifier EXTENDS identifier LBRACE variable_declaration_list RBRACE  */
#line 97 "src/dj.y"
                                                                                  {
        yyval = newAST(NONFINAL_CLASS_DECL, yyvsp[-5], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-3]);
        appendToChildrenList(yyval, yyvsp[-1]);
        appendToChildrenList(yyval, newAST(METHOD_DECL_LIST, NULL, 0, NULL, 0));
    }
#line 1453 "src/dj.tab.c"
    break;

  case 11: /* class: CLASS identifier EXTENDS identifier LBRACE method_list RBRACE  */
#line 103 "src/dj.y"
                                                                    {
        yyval = newAST(NONFINAL_CLASS_DECL, yyvsp[-5], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-3]);
        appendToChildrenList(yyval, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1464 "src/dj.tab.c"
    break;

  case 12: /* class: CLASS identifier EXTENDS identifier LBRACE variable_declaration_list method_list RBRACE  */
#line 109 "src/dj.y"
                                                                                              {
        yyval = newAST(NONFINAL_CLASS_DECL, yyvsp[-6], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-4]);
        appendToChildrenList(yyval, yyvsp[-2]);
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1475 "src/dj.tab.c"
    break;

  case 13: /* class: FINAL CLASS identifier EXTENDS identifier LBRACE RBRACE  */
#line 115 "src/dj.y"
                                                              {
        yyval = newAST(FINAL_CLASS_DECL, yyvsp[-4], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-2]);
        appendToChildrenList(yyval, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList(yyval, newAST(METHOD_DECL_LIST, NULL, 0, NULL, 0));
    }
#line 1486 "src/dj.tab.c"
    break;

  case 14: /* class: FINAL CLASS identifier EXTENDS identifier LBRACE variable_declaration_list RBRACE  */
#line 121 "src/dj.y"
                                                                                        {
        yyval = newAST(FINAL_CLASS_DECL, yyvsp[-5], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-3]);
        appendToChildrenList(yyval, yyvsp[-1]);
        appendToChildrenList(yyval, newAST(METHOD_DECL_LIST, NULL, 0, NULL, 0));
    }
#line 1497 "src/dj.tab.c"
    break;

  case 15: /* class: FINAL CLASS identifier EXTENDS identifier LBRACE method_list RBRACE  */
#line 127 "src/dj.y"
                                                                          {
        yyval = newAST(FINAL_CLASS_DECL, yyvsp[-5], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-3]);
        appendToChildrenList(yyval, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList(yyval, yyvsp[-1]);   
    }
#line 1508 "src/dj.tab.c"
    break;

  case 16: /* class: FINAL CLASS identifier EXTENDS identifier LBRACE variable_declaration_list method_list RBRACE  */
#line 133 "src/dj.y"
                                                                                                    {
        yyval = newAST(FINAL_CLASS_DECL, yyvsp[-6], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-4]);
        appendToChildrenList(yyval, yyvsp[-2]);
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1519 "src/dj.tab.c"
    break;

  case 17: /* method_list: method_list method  */
#line 142 "src/dj.y"
                         {
        yyval = yyvsp[-1];
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1528 "src/dj.tab.c"
    break;

  case 18: /* method_list: method  */
#line 146 "src/dj.y"
             {
        yyval = newAST(METHOD_DECL_LIST, yyvsp[0], 0, NULL, yylineno);
    }
#line 1536 "src/dj.tab.c"
    break;

  case 19: /* method: data_type identifier LPAREN data_type identifier RPAREN LBRACE expression_list RBRACE  */
#line 152 "src/dj.y"
                                                                                            {
        yyval = newAST(NONFINAL_METHOD_DECL, yyvsp[-8], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-7]);
        appendToChildrenList(yyval, yyvsp[-5]);
        appendToChildrenList(yyval, yyvsp[-4]);
        appendToChildrenList(yyval, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1549 "src/dj.tab.c"
    break;

  case 20: /* method: data_type identifier LPAREN data_type identifier RPAREN LBRACE variable_declaration_list expression_list RBRACE  */
#line 160 "src/dj.y"
                                                                                                                      {
        yyval = newAST(NONFINAL_METHOD_DECL, yyvsp[-9], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-8]);
        appendToChildrenList(yyval, yyvsp[-6]);
        appendToChildrenList(yyval, yyvsp[-5]);
        appendToChildrenList(yyval, yyvsp[-2]);
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1562 "src/dj.tab.c"
    break;

  case 21: /* method: FINAL data_type identifier LPAREN data_type identifier RPAREN LBRACE expression_list RBRACE  */
#line 168 "src/dj.y"
                                                                                                  {
        yyval = newAST(FINAL_METHOD_DECL, yyvsp[-8], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-7]);
        appendToChildrenList(yyval, yyvsp[-5]);
        appendToChildrenList(yyval, yyvsp[-4]);
        appendToChildrenList(yyval, newAST(VAR_DECL_LIST, NULL, 0, NULL, 0));
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1575 "src/dj.tab.c"
    break;

  case 22: /* method: FINAL data_type identifier LPAREN data_type identifier RPAREN LBRACE variable_declaration_list expression_list RBRACE  */
#line 176 "src/dj.y"
                                                                                                                            {
        yyval = newAST(FINAL_METHOD_DECL, yyvsp[-9], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-8]);
        appendToChildrenList(yyval, yyvsp[-6]);
        appendToChildrenList(yyval, yyvsp[-5]);
        appendToChildrenList(yyval, yyvsp[-2]);
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1588 "src/dj.tab.c"
    break;

  case 23: /* variable_declaration_list: variable_declaration_list variable_declaration SEMICOLON  */
#line 187 "src/dj.y"
                                                               {
        yyval = yyvsp[-2];
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1597 "src/dj.tab.c"
    break;

  case 24: /* variable_declaration_list: variable_declaration SEMICOLON  */
#line 191 "src/dj.y"
                                     {
        yyval = newAST(VAR_DECL_LIST, yyvsp[-1], 0, NULL, yylineno);
    }
#line 1605 "src/dj.tab.c"
    break;

  case 25: /* variable_declaration: data_type identifier  */
#line 197 "src/dj.y"
                           {
        yyval = newAST(VAR_DECL, yyvsp[-1], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1614 "src/dj.tab.c"
    break;

  case 26: /* expression_list: expression_list expression SEMICOLON  */
#line 204 "src/dj.y"
                                           {
        yyval = yyvsp[-2];
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1623 "src/dj.tab.c"
    break;

  case 27: /* expression_list: expression SEMICOLON  */
#line 208 "src/dj.y"
                           {
        yyval = newAST(EXPR_LIST, yyvsp[-1], 0, NULL, yylineno);
    }
#line 1631 "src/dj.tab.c"
    break;

  case 28: /* expression: NUL  */
#line 214 "src/dj.y"
          { 
        yyval = newAST(NULL_EXPR, NULL, 0, NULL, yylineno);
    }
#line 1639 "src/dj.tab.c"
    break;

  case 29: /* expression: NATLITERAL  */
#line 217 "src/dj.y"
                 { 
        yyval = newAST(NAT_LITERAL_EXPR, NULL, atoi(yytext), NULL, yylineno);
    }
#line 1647 "src/dj.tab.c"
    break;

  case 30: /* expression: identifier  */
#line 220 "src/dj.y"
                 { 
        yyval = newAST(ID_EXPR, yyvsp[0], 0, NULL, yylineno);
    }
#line 1655 "src/dj.tab.c"
    break;

  case 31: /* expression: THIS  */
#line 223 "src/dj.y"
           { 
        yyval = newAST(THIS_EXPR, NULL, 0, NULL, yylineno); 
    }
#line 1663 "src/dj.tab.c"
    break;

  case 32: /* expression: identifier LPAREN expression RPAREN  */
#line 226 "src/dj.y"
                                          { 
        yyval = newAST(METHOD_CALL_EXPR, yyvsp[-3], 0, NULL, yylineno); 
        appendToChildrenList(yyval, yyvsp[-1]); 
    }
#line 1672 "src/dj.tab.c"
    break;

  case 33: /* expression: NEW identifier LPAREN RPAREN  */
#line 230 "src/dj.y"
                                   { 
        yyval = newAST(NEW_EXPR, yyvsp[-2], 0, NULL, yylineno); 
    }
#line 1680 "src/dj.tab.c"
    break;

  case 34: /* expression: LPAREN expression RPAREN  */
#line 233 "src/dj.y"
                               { 
        yyval = yyvsp[-1];
    }
#line 1688 "src/dj.tab.c"
    break;

  case 35: /* expression: expression DOT identifier  */
#line 236 "src/dj.y"
                                {
        yyval = newAST(DOT_ID_EXPR, yyvsp[-2], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1697 "src/dj.tab.c"
    break;

  case 36: /* expression: expression DOT identifier LPAREN expression RPAREN  */
#line 240 "src/dj.y"
                                                         {
        yyval = newAST(DOT_METHOD_CALL_EXPR, yyvsp[-5], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-3]);
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1707 "src/dj.tab.c"
    break;

  case 37: /* expression: expression PLUS expression  */
#line 245 "src/dj.y"
                                 {
        yyval = newAST(PLUS_EXPR, yyvsp[-2], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1716 "src/dj.tab.c"
    break;

  case 38: /* expression: expression MINUS expression  */
#line 249 "src/dj.y"
                                  {
        yyval = newAST(MINUS_EXPR, yyvsp[-2], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1725 "src/dj.tab.c"
    break;

  case 39: /* expression: expression TIMES expression  */
#line 253 "src/dj.y"
                                  {
        yyval = newAST(TIMES_EXPR, yyvsp[-2], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1734 "src/dj.tab.c"
    break;

  case 40: /* expression: expression EQUALITY expression  */
#line 257 "src/dj.y"
                                     {
        yyval = newAST(EQUALITY_EXPR, yyvsp[-2], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1743 "src/dj.tab.c"
    break;

  case 41: /* expression: expression LESS expression  */
#line 261 "src/dj.y"
                                 {
        yyval = newAST(LESS_THAN_EXPR, yyvsp[-2], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1752 "src/dj.tab.c"
    break;

  case 42: /* expression: NOT expression  */
#line 265 "src/dj.y"
                     {
        yyval = newAST(NOT_EXPR, yyvsp[0], 0, NULL, yylineno);
    }
#line 1760 "src/dj.tab.c"
    break;

  case 43: /* expression: expression OR expression  */
#line 268 "src/dj.y"
                               {
        yyval = newAST(OR_EXPR, yyvsp[-2], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1769 "src/dj.tab.c"
    break;

  case 44: /* expression: identifier ASSIGN expression  */
#line 272 "src/dj.y"
                                   {
        yyval = newAST(ASSIGN_EXPR, yyvsp[-2], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1778 "src/dj.tab.c"
    break;

  case 45: /* expression: expression DOT identifier ASSIGN expression  */
#line 276 "src/dj.y"
                                                  {
        yyval = newAST(DOT_ASSIGN_EXPR, yyvsp[-4], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-2]);
        appendToChildrenList(yyval, yyvsp[0]);
    }
#line 1788 "src/dj.tab.c"
    break;

  case 46: /* expression: IF LPAREN expression RPAREN LBRACE expression_list RBRACE ELSE LBRACE expression_list RBRACE  */
#line 281 "src/dj.y"
                                                                                                   {
        yyval = newAST(IF_THEN_ELSE_EXPR, yyvsp[-8], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-5]);
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1798 "src/dj.tab.c"
    break;

  case 47: /* expression: WHILE LPAREN expression RPAREN LBRACE expression_list RBRACE  */
#line 286 "src/dj.y"
                                                                   {
        yyval = newAST(WHILE_EXPR, yyvsp[-4], 0, NULL, yylineno);
        appendToChildrenList(yyval, yyvsp[-1]);
    }
#line 1807 "src/dj.tab.c"
    break;

  case 48: /* expression: ASSERT expression  */
#line 290 "src/dj.y"
                        {
        yyval = newAST(ASSERT_EXPR, yyvsp[0], 0, NULL, yylineno);
    }
#line 1815 "src/dj.tab.c"
    break;

  case 49: /* expression: PRINTNAT LPAREN expression RPAREN  */
#line 293 "src/dj.y"
                                        {
        yyval = newAST(PRINT_EXPR, yyvsp[-1], 0, NULL, yylineno);
    }
#line 1823 "src/dj.tab.c"
    break;

  case 50: /* expression: READNAT LPAREN RPAREN  */
#line 296 "src/dj.y"
                            {
        yyval = newAST(READ_EXPR, NULL, 0, NULL, yylineno);
    }
#line 1831 "src/dj.tab.c"
    break;

  case 51: /* data_type: NATTYPE  */
#line 302 "src/dj.y"
              {
        yyval = newAST(NAT_TYPE, NULL, 0, NULL, yylineno);
    }
#line 1839 "src/dj.tab.c"
    break;

  case 52: /* data_type: identifier  */
#line 305 "src/dj.y"
                 {
        yyval = yyvsp[0];
    }
#line 1847 "src/dj.tab.c"
    break;

  case 53: /* identifier: ID  */
#line 311 "src/dj.y"
         {
        yyval = newAST(AST_ID, NULL, 0, getID(yytext), yylineno);
    }
#line 1855 "src/dj.tab.c"
    break;


#line 1859 "src/dj.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 316 "src/dj.y"


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
