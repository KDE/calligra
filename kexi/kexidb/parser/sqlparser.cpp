/* A Bison parser, made from sqlparser.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON	1

/* Pure parsers.  */
#define YYPURE	0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     UMINUS = 258,
     SQL_TYPE = 259,
     SQL_ABS = 260,
     ACOS = 261,
     AMPERSAND = 262,
     SQL_ABSOLUTE = 263,
     ADA = 264,
     ADD = 265,
     ADD_DAYS = 266,
     ADD_HOURS = 267,
     ADD_MINUTES = 268,
     ADD_MONTHS = 269,
     ADD_SECONDS = 270,
     ADD_YEARS = 271,
     ALL = 272,
     ALLOCATE = 273,
     ALTER = 274,
     AND = 275,
     ANY = 276,
     ARE = 277,
     AS = 278,
     ASIN = 279,
     ASC = 280,
     ASCII = 281,
     ASSERTION = 282,
     ATAN = 283,
     ATAN2 = 284,
     AUTHORIZATION = 285,
     AUTO_INCREMENT = 286,
     AVG = 287,
     BEFORE = 288,
     SQL_BEGIN = 289,
     BETWEEN = 290,
     BIGINT = 291,
     BINARY = 292,
     BIT = 293,
     BIT_LENGTH = 294,
     BITWISE_SHIFT_LEFT = 295,
     BITWISE_SHIFT_RIGHT = 296,
     BREAK = 297,
     BY = 298,
     CASCADE = 299,
     CASCADED = 300,
     CASE = 301,
     CAST = 302,
     CATALOG = 303,
     CEILING = 304,
     CENTER = 305,
     SQL_CHAR = 306,
     CHAR_LENGTH = 307,
     CHARACTER_STRING_LITERAL = 308,
     CHECK = 309,
     CLOSE = 310,
     COALESCE = 311,
     COBOL = 312,
     COLLATE = 313,
     COLLATION = 314,
     COLUMN = 315,
     COMMIT = 316,
     COMPUTE = 317,
     CONCAT = 318,
     CONCATENATION = 319,
     CONNECT = 320,
     CONNECTION = 321,
     CONSTRAINT = 322,
     CONSTRAINTS = 323,
     CONTINUE = 324,
     CONVERT = 325,
     CORRESPONDING = 326,
     COS = 327,
     COT = 328,
     COUNT = 329,
     CREATE = 330,
     CURDATE = 331,
     CURRENT = 332,
     CURRENT_DATE = 333,
     CURRENT_TIME = 334,
     CURRENT_TIMESTAMP = 335,
     CURTIME = 336,
     CURSOR = 337,
     DATABASE = 338,
     SQL_DATE = 339,
     DATE_FORMAT = 340,
     DATE_REMAINDER = 341,
     DATE_VALUE = 342,
     DAY = 343,
     DAYOFMONTH = 344,
     DAYOFWEEK = 345,
     DAYOFYEAR = 346,
     DAYS_BETWEEN = 347,
     DEALLOCATE = 348,
     DEC = 349,
     DECLARE = 350,
     DEFAULT = 351,
     DEFERRABLE = 352,
     DEFERRED = 353,
     SQL_DELETE = 354,
     DESC = 355,
     DESCRIBE = 356,
     DESCRIPTOR = 357,
     DIAGNOSTICS = 358,
     DICTIONARY = 359,
     DIRECTORY = 360,
     DISCONNECT = 361,
     DISPLACEMENT = 362,
     DISTINCT = 363,
     DOMAIN_TOKEN = 364,
     SQL_DOUBLE = 365,
     DOUBLE_QUOTED_STRING = 366,
     DROP = 367,
     ELSE = 368,
     END = 369,
     END_EXEC = 370,
     EQUAL = 371,
     ESCAPE = 372,
     EXCEPT = 373,
     SQL_EXCEPTION = 374,
     EXEC = 375,
     EXECUTE = 376,
     EXISTS = 377,
     EXP = 378,
     EXPONENT = 379,
     EXTERNAL = 380,
     EXTRACT = 381,
     SQL_FALSE = 382,
     FETCH = 383,
     FIRST = 384,
     SQL_FLOAT = 385,
     FLOOR = 386,
     FN = 387,
     FOR = 388,
     FOREIGN = 389,
     FORTRAN = 390,
     FOUND = 391,
     FOUR_DIGITS = 392,
     FROM = 393,
     FULL = 394,
     GET = 395,
     GLOBAL = 396,
     GO = 397,
     GOTO = 398,
     GRANT = 399,
     GREATER_OR_EQUAL = 400,
     HAVING = 401,
     HOUR = 402,
     HOURS_BETWEEN = 403,
     IDENTITY = 404,
     IFNULL = 405,
     SQL_IGNORE = 406,
     IMMEDIATE = 407,
     SQL_IN = 408,
     INCLUDE = 409,
     INDEX = 410,
     INDICATOR = 411,
     INITIALLY = 412,
     INNER = 413,
     INPUT = 414,
     INSENSITIVE = 415,
     INSERT = 416,
     INTEGER = 417,
     INTERSECT = 418,
     INTERVAL = 419,
     INTO = 420,
     IS = 421,
     ISOLATION = 422,
     JOIN = 423,
     JUSTIFY = 424,
     KEY = 425,
     LANGUAGE = 426,
     LAST = 427,
     LCASE = 428,
     LEFT = 429,
     LENGTH = 430,
     LESS_OR_EQUAL = 431,
     LEVEL = 432,
     LIKE = 433,
     LINE_WIDTH = 434,
     LOCAL = 435,
     LOCATE = 436,
     LOG = 437,
     SQL_LONG = 438,
     LOWER = 439,
     LTRIM = 440,
     LTRIP = 441,
     MATCH = 442,
     SQL_MAX = 443,
     MICROSOFT = 444,
     SQL_MIN = 445,
     MINUS = 446,
     MINUTE = 447,
     MINUTES_BETWEEN = 448,
     MOD = 449,
     MODIFY = 450,
     MODULE = 451,
     MONTH = 452,
     MONTHS_BETWEEN = 453,
     MUMPS = 454,
     NAMES = 455,
     NATIONAL = 456,
     NCHAR = 457,
     NEXT = 458,
     NODUP = 459,
     NONE = 460,
     NOT = 461,
     NOT_EQUAL = 462,
     NOT_EQUAL2 = 463,
     NOW = 464,
     SQL_NULL = 465,
     SQL_IS = 466,
     SQL_IS_NULL = 467,
     SQL_IS_NOT_NULL = 468,
     NULLIF = 469,
     NUMERIC = 470,
     OCTET_LENGTH = 471,
     ODBC = 472,
     OF = 473,
     SQL_OFF = 474,
     SQL_ON = 475,
     ONLY = 476,
     OPEN = 477,
     OPTION = 478,
     OR = 479,
     ORDER = 480,
     OUTER = 481,
     OUTPUT = 482,
     OVERLAPS = 483,
     PAGE = 484,
     PARTIAL = 485,
     SQL_PASCAL = 486,
     PERSISTENT = 487,
     CQL_PI = 488,
     PLI = 489,
     POSITION = 490,
     PRECISION = 491,
     PREPARE = 492,
     PRESERVE = 493,
     PRIMARY = 494,
     PRIOR = 495,
     PRIVILEGES = 496,
     PROCEDURE = 497,
     PRODUCT = 498,
     PUBLIC = 499,
     QUARTER = 500,
     QUIT = 501,
     RAND = 502,
     READ_ONLY = 503,
     REAL = 504,
     REFERENCES = 505,
     REPEAT = 506,
     REPLACE = 507,
     RESTRICT = 508,
     REVOKE = 509,
     RIGHT = 510,
     ROLLBACK = 511,
     ROWS = 512,
     RPAD = 513,
     RTRIM = 514,
     SCHEMA = 515,
     SCREEN_WIDTH = 516,
     SCROLL = 517,
     SECOND = 518,
     SECONDS_BETWEEN = 519,
     SELECT = 520,
     SEQUENCE = 521,
     SETOPT = 522,
     SET = 523,
     SHOWOPT = 524,
     SIGN = 525,
     SIMILAR_TO = 526,
     NOT_SIMILAR_TO = 527,
     INTEGER_CONST = 528,
     REAL_CONST = 529,
     SIN = 530,
     SQL_SIZE = 531,
     SMALLINT = 532,
     SOME = 533,
     SPACE = 534,
     SQL = 535,
     SQL_TRUE = 536,
     SQLCA = 537,
     SQLCODE = 538,
     SQLERROR = 539,
     SQLSTATE = 540,
     SQLWARNING = 541,
     SQRT = 542,
     STDEV = 543,
     SUBSTRING = 544,
     SUM = 545,
     SYSDATE = 546,
     SYSDATE_FORMAT = 547,
     SYSTEM = 548,
     TABLE = 549,
     TAN = 550,
     TEMPORARY = 551,
     THEN = 552,
     THREE_DIGITS = 553,
     TIME = 554,
     TIMESTAMP = 555,
     TIMEZONE_HOUR = 556,
     TIMEZONE_MINUTE = 557,
     TINYINT = 558,
     TO = 559,
     TO_CHAR = 560,
     TO_DATE = 561,
     TRANSACTION = 562,
     TRANSLATE = 563,
     TRANSLATION = 564,
     TRUNCATE = 565,
     GENERAL_TITLE = 566,
     TWO_DIGITS = 567,
     UCASE = 568,
     UNION = 569,
     UNIQUE = 570,
     SQL_UNKNOWN = 571,
     UPDATE = 572,
     UPPER = 573,
     USAGE = 574,
     USER = 575,
     IDENTIFIER = 576,
     IDENTIFIER_DOT_ASTERISK = 577,
     ERROR_DIGIT_BEFORE_IDENTIFIER = 578,
     USING = 579,
     VALUE = 580,
     VALUES = 581,
     VARBINARY = 582,
     VARCHAR = 583,
     VARYING = 584,
     VENDOR = 585,
     VIEW = 586,
     WEEK = 587,
     WHEN = 588,
     WHENEVER = 589,
     WHERE = 590,
     WHERE_CURRENT_OF = 591,
     WITH = 592,
     WORD_WRAPPED = 593,
     WORK = 594,
     WRAPPED = 595,
     XOR = 596,
     YEAR = 597,
     YEARS_BETWEEN = 598,
     __LAST_TOKEN = 599,
     ILIKE = 600
   };
#endif
#define UMINUS 258
#define SQL_TYPE 259
#define SQL_ABS 260
#define ACOS 261
#define AMPERSAND 262
#define SQL_ABSOLUTE 263
#define ADA 264
#define ADD 265
#define ADD_DAYS 266
#define ADD_HOURS 267
#define ADD_MINUTES 268
#define ADD_MONTHS 269
#define ADD_SECONDS 270
#define ADD_YEARS 271
#define ALL 272
#define ALLOCATE 273
#define ALTER 274
#define AND 275
#define ANY 276
#define ARE 277
#define AS 278
#define ASIN 279
#define ASC 280
#define ASCII 281
#define ASSERTION 282
#define ATAN 283
#define ATAN2 284
#define AUTHORIZATION 285
#define AUTO_INCREMENT 286
#define AVG 287
#define BEFORE 288
#define SQL_BEGIN 289
#define BETWEEN 290
#define BIGINT 291
#define BINARY 292
#define BIT 293
#define BIT_LENGTH 294
#define BITWISE_SHIFT_LEFT 295
#define BITWISE_SHIFT_RIGHT 296
#define BREAK 297
#define BY 298
#define CASCADE 299
#define CASCADED 300
#define CASE 301
#define CAST 302
#define CATALOG 303
#define CEILING 304
#define CENTER 305
#define SQL_CHAR 306
#define CHAR_LENGTH 307
#define CHARACTER_STRING_LITERAL 308
#define CHECK 309
#define CLOSE 310
#define COALESCE 311
#define COBOL 312
#define COLLATE 313
#define COLLATION 314
#define COLUMN 315
#define COMMIT 316
#define COMPUTE 317
#define CONCAT 318
#define CONCATENATION 319
#define CONNECT 320
#define CONNECTION 321
#define CONSTRAINT 322
#define CONSTRAINTS 323
#define CONTINUE 324
#define CONVERT 325
#define CORRESPONDING 326
#define COS 327
#define COT 328
#define COUNT 329
#define CREATE 330
#define CURDATE 331
#define CURRENT 332
#define CURRENT_DATE 333
#define CURRENT_TIME 334
#define CURRENT_TIMESTAMP 335
#define CURTIME 336
#define CURSOR 337
#define DATABASE 338
#define SQL_DATE 339
#define DATE_FORMAT 340
#define DATE_REMAINDER 341
#define DATE_VALUE 342
#define DAY 343
#define DAYOFMONTH 344
#define DAYOFWEEK 345
#define DAYOFYEAR 346
#define DAYS_BETWEEN 347
#define DEALLOCATE 348
#define DEC 349
#define DECLARE 350
#define DEFAULT 351
#define DEFERRABLE 352
#define DEFERRED 353
#define SQL_DELETE 354
#define DESC 355
#define DESCRIBE 356
#define DESCRIPTOR 357
#define DIAGNOSTICS 358
#define DICTIONARY 359
#define DIRECTORY 360
#define DISCONNECT 361
#define DISPLACEMENT 362
#define DISTINCT 363
#define DOMAIN_TOKEN 364
#define SQL_DOUBLE 365
#define DOUBLE_QUOTED_STRING 366
#define DROP 367
#define ELSE 368
#define END 369
#define END_EXEC 370
#define EQUAL 371
#define ESCAPE 372
#define EXCEPT 373
#define SQL_EXCEPTION 374
#define EXEC 375
#define EXECUTE 376
#define EXISTS 377
#define EXP 378
#define EXPONENT 379
#define EXTERNAL 380
#define EXTRACT 381
#define SQL_FALSE 382
#define FETCH 383
#define FIRST 384
#define SQL_FLOAT 385
#define FLOOR 386
#define FN 387
#define FOR 388
#define FOREIGN 389
#define FORTRAN 390
#define FOUND 391
#define FOUR_DIGITS 392
#define FROM 393
#define FULL 394
#define GET 395
#define GLOBAL 396
#define GO 397
#define GOTO 398
#define GRANT 399
#define GREATER_OR_EQUAL 400
#define HAVING 401
#define HOUR 402
#define HOURS_BETWEEN 403
#define IDENTITY 404
#define IFNULL 405
#define SQL_IGNORE 406
#define IMMEDIATE 407
#define SQL_IN 408
#define INCLUDE 409
#define INDEX 410
#define INDICATOR 411
#define INITIALLY 412
#define INNER 413
#define INPUT 414
#define INSENSITIVE 415
#define INSERT 416
#define INTEGER 417
#define INTERSECT 418
#define INTERVAL 419
#define INTO 420
#define IS 421
#define ISOLATION 422
#define JOIN 423
#define JUSTIFY 424
#define KEY 425
#define LANGUAGE 426
#define LAST 427
#define LCASE 428
#define LEFT 429
#define LENGTH 430
#define LESS_OR_EQUAL 431
#define LEVEL 432
#define LIKE 433
#define LINE_WIDTH 434
#define LOCAL 435
#define LOCATE 436
#define LOG 437
#define SQL_LONG 438
#define LOWER 439
#define LTRIM 440
#define LTRIP 441
#define MATCH 442
#define SQL_MAX 443
#define MICROSOFT 444
#define SQL_MIN 445
#define MINUS 446
#define MINUTE 447
#define MINUTES_BETWEEN 448
#define MOD 449
#define MODIFY 450
#define MODULE 451
#define MONTH 452
#define MONTHS_BETWEEN 453
#define MUMPS 454
#define NAMES 455
#define NATIONAL 456
#define NCHAR 457
#define NEXT 458
#define NODUP 459
#define NONE 460
#define NOT 461
#define NOT_EQUAL 462
#define NOT_EQUAL2 463
#define NOW 464
#define SQL_NULL 465
#define SQL_IS 466
#define SQL_IS_NULL 467
#define SQL_IS_NOT_NULL 468
#define NULLIF 469
#define NUMERIC 470
#define OCTET_LENGTH 471
#define ODBC 472
#define OF 473
#define SQL_OFF 474
#define SQL_ON 475
#define ONLY 476
#define OPEN 477
#define OPTION 478
#define OR 479
#define ORDER 480
#define OUTER 481
#define OUTPUT 482
#define OVERLAPS 483
#define PAGE 484
#define PARTIAL 485
#define SQL_PASCAL 486
#define PERSISTENT 487
#define CQL_PI 488
#define PLI 489
#define POSITION 490
#define PRECISION 491
#define PREPARE 492
#define PRESERVE 493
#define PRIMARY 494
#define PRIOR 495
#define PRIVILEGES 496
#define PROCEDURE 497
#define PRODUCT 498
#define PUBLIC 499
#define QUARTER 500
#define QUIT 501
#define RAND 502
#define READ_ONLY 503
#define REAL 504
#define REFERENCES 505
#define REPEAT 506
#define REPLACE 507
#define RESTRICT 508
#define REVOKE 509
#define RIGHT 510
#define ROLLBACK 511
#define ROWS 512
#define RPAD 513
#define RTRIM 514
#define SCHEMA 515
#define SCREEN_WIDTH 516
#define SCROLL 517
#define SECOND 518
#define SECONDS_BETWEEN 519
#define SELECT 520
#define SEQUENCE 521
#define SETOPT 522
#define SET 523
#define SHOWOPT 524
#define SIGN 525
#define SIMILAR_TO 526
#define NOT_SIMILAR_TO 527
#define INTEGER_CONST 528
#define REAL_CONST 529
#define SIN 530
#define SQL_SIZE 531
#define SMALLINT 532
#define SOME 533
#define SPACE 534
#define SQL 535
#define SQL_TRUE 536
#define SQLCA 537
#define SQLCODE 538
#define SQLERROR 539
#define SQLSTATE 540
#define SQLWARNING 541
#define SQRT 542
#define STDEV 543
#define SUBSTRING 544
#define SUM 545
#define SYSDATE 546
#define SYSDATE_FORMAT 547
#define SYSTEM 548
#define TABLE 549
#define TAN 550
#define TEMPORARY 551
#define THEN 552
#define THREE_DIGITS 553
#define TIME 554
#define TIMESTAMP 555
#define TIMEZONE_HOUR 556
#define TIMEZONE_MINUTE 557
#define TINYINT 558
#define TO 559
#define TO_CHAR 560
#define TO_DATE 561
#define TRANSACTION 562
#define TRANSLATE 563
#define TRANSLATION 564
#define TRUNCATE 565
#define GENERAL_TITLE 566
#define TWO_DIGITS 567
#define UCASE 568
#define UNION 569
#define UNIQUE 570
#define SQL_UNKNOWN 571
#define UPDATE 572
#define UPPER 573
#define USAGE 574
#define USER 575
#define IDENTIFIER 576
#define IDENTIFIER_DOT_ASTERISK 577
#define ERROR_DIGIT_BEFORE_IDENTIFIER 578
#define USING 579
#define VALUE 580
#define VALUES 581
#define VARBINARY 582
#define VARCHAR 583
#define VARYING 584
#define VENDOR 585
#define VIEW 586
#define WEEK 587
#define WHEN 588
#define WHENEVER 589
#define WHERE 590
#define WHERE_CURRENT_OF 591
#define WITH 592
#define WORD_WRAPPED 593
#define WORK 594
#define WRAPPED 595
#define XOR 596
#define YEAR 597
#define YEARS_BETWEEN 598
#define __LAST_TOKEN 599
#define ILIKE 600




/* Copy the first part of user declarations.  */
#line 428 "sqlparser.y"

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <assert.h>

#ifdef _WIN32
# include <malloc.h>
#endif

#include <qobject.h>
#include <kdebug.h>
#include <klocale.h>
#include <qptrlist.h>
#include <qcstring.h>

#include <connection.h>
#include <queryschema.h>
#include <field.h>
#include <tableschema.h>

#include "parser.h"
#include "parser_p.h"
#include "sqltypes.h"

int yylex();

//	using namespace std;
using namespace KexiDB;

#define YY_NO_UNPUT
#define YYSTACK_USE_ALLOCA 1
#define YYMAXDEPTH 255

	extern "C"
	{
		int yywrap()
		{
			return 1;
		}
	}

#if 0
	struct yyval
	{
		QString parserUserName;
		int integerValue;
		KexiDBField::ColumnType coltype;
	}
#endif


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#ifndef YYSTYPE
#line 481 "sqlparser.y"
typedef union {
	char stringValue[255];
	int integerValue;
	struct realType realValue;
	KexiDB::Field::Type colType;
	KexiDB::Field *field;
	KexiDB::BaseExpr *expr;
	KexiDB::NArgExpr *exprList;
	KexiDB::ConstExpr *constExpr;
	KexiDB::QuerySchema *querySchema;
} yystype;
/* Line 193 of /usr/share/bison/yacc.c.  */
#line 828 "sqlparser.tab.c"
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} yyltype;
# define YYLTYPE yyltype
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* Copy the second part of user declarations.  */


/* Line 213 of /usr/share/bison/yacc.c.  */
#line 849 "sqlparser.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];	\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  10
#define YYLAST   330

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  369
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  31
/* YYNRULES -- Number of rules. */
#define YYNRULES  92
/* YYNRULES -- Number of states. */
#define YYNSTATES  152

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   600

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned short yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   353,   348,   366,   357,
     354,   355,   347,   346,   351,   345,   352,   358,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   350,
     360,   359,   361,   356,   349,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   364,     2,   365,   363,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   367,     2,   368,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     362
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     9,    11,    14,    16,    18,    19,
      27,    31,    33,    36,    40,    43,    45,    48,    51,    53,
      55,    60,    65,    66,    69,    73,    76,    80,    85,    89,
      91,    94,    96,   100,   104,   108,   110,   114,   118,   120,
     124,   128,   132,   136,   138,   142,   146,   150,   152,   156,
     160,   164,   168,   172,   174,   178,   182,   186,   190,   194,
     198,   200,   203,   206,   208,   211,   214,   217,   220,   222,
     226,   228,   230,   232,   234,   236,   240,   243,   247,   249,
     251,   254,   258,   262,   264,   266,   268,   272,   275,   277,
     282,   287,   289
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     370,     0,    -1,   371,    -1,   372,   350,   371,    -1,   372,
      -1,   372,   350,    -1,   373,    -1,   380,    -1,    -1,    75,
     294,   321,   374,   354,   375,   355,    -1,   375,   351,   376,
      -1,   376,    -1,   321,   379,    -1,   321,   379,   377,    -1,
     377,   378,    -1,   378,    -1,   239,   170,    -1,   206,   210,
      -1,    31,    -1,     4,    -1,     4,   354,   273,   355,    -1,
     328,   354,   273,   355,    -1,    -1,   381,   396,    -1,   381,
     396,   393,    -1,   381,   393,    -1,   381,   396,   382,    -1,
     381,   396,   393,   382,    -1,   381,   393,   382,    -1,   265,
      -1,   335,   383,    -1,   384,    -1,   385,    20,   384,    -1,
     385,   224,   384,    -1,   385,   341,   384,    -1,   385,    -1,
     386,    40,   385,    -1,   386,    41,   385,    -1,   386,    -1,
     387,   346,   386,    -1,   387,   345,   386,    -1,   387,   366,
     386,    -1,   387,   367,   386,    -1,   387,    -1,   388,   358,
     387,    -1,   388,   347,   387,    -1,   388,   348,   387,    -1,
     388,    -1,   389,   361,   388,    -1,   389,   145,   388,    -1,
     389,   360,   388,    -1,   389,   176,   388,    -1,   389,   359,
     388,    -1,   389,    -1,   390,   207,   389,    -1,   390,   208,
     389,    -1,   390,   178,   389,    -1,   390,   153,   389,    -1,
     390,   271,   389,    -1,   390,   272,   389,    -1,   390,    -1,
     390,   212,    -1,   390,   213,    -1,   391,    -1,   345,   391,
      -1,   346,   391,    -1,   368,   391,    -1,   206,   391,    -1,
     321,    -1,   321,   352,   321,    -1,   210,    -1,    53,    -1,
     273,    -1,   274,    -1,   392,    -1,   354,   383,   355,    -1,
     138,   394,    -1,   394,   351,   395,    -1,   395,    -1,   321,
      -1,   321,   321,    -1,   321,    23,   321,    -1,   396,   351,
     397,    -1,   397,    -1,   398,    -1,   399,    -1,   398,    23,
     321,    -1,   398,   321,    -1,   383,    -1,   321,   354,   396,
     355,    -1,   108,   354,   398,   355,    -1,   347,    -1,   321,
     352,   347,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   545,   545,   555,   560,   561,   570,   575,   583,   581,
     590,   591,   596,   606,   620,   621,   626,   632,   637,   644,
     650,   657,   663,   671,   679,   684,   690,   696,   702,   710,
     720,   727,   732,   738,   742,   746,   751,   756,   760,   765,
     771,   775,   779,   783,   788,   793,   797,   801,   806,   811,
     815,   819,   823,   827,   832,   837,   842,   846,   850,   854,
     858,   863,   868,   872,   877,   883,   887,   891,   895,   914,
     925,   932,   940,   948,   953,   958,   986,  1027,  1033,  1040,
    1076,  1085,  1098,  1105,  1113,  1123,  1128,  1140,  1154,  1159,
    1198,  1206,  1216
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "UMINUS", "SQL_TYPE", "SQL_ABS", "ACOS", 
  "AMPERSAND", "SQL_ABSOLUTE", "ADA", "ADD", "ADD_DAYS", "ADD_HOURS", 
  "ADD_MINUTES", "ADD_MONTHS", "ADD_SECONDS", "ADD_YEARS", "ALL", 
  "ALLOCATE", "ALTER", "AND", "ANY", "ARE", "AS", "ASIN", "ASC", "ASCII", 
  "ASSERTION", "ATAN", "ATAN2", "AUTHORIZATION", "AUTO_INCREMENT", "AVG", 
  "BEFORE", "SQL_BEGIN", "BETWEEN", "BIGINT", "BINARY", "BIT", 
  "BIT_LENGTH", "BITWISE_SHIFT_LEFT", "BITWISE_SHIFT_RIGHT", "BREAK", 
  "BY", "CASCADE", "CASCADED", "CASE", "CAST", "CATALOG", "CEILING", 
  "CENTER", "SQL_CHAR", "CHAR_LENGTH", "CHARACTER_STRING_LITERAL", 
  "CHECK", "CLOSE", "COALESCE", "COBOL", "COLLATE", "COLLATION", "COLUMN", 
  "COMMIT", "COMPUTE", "CONCAT", "CONCATENATION", "CONNECT", "CONNECTION", 
  "CONSTRAINT", "CONSTRAINTS", "CONTINUE", "CONVERT", "CORRESPONDING", 
  "COS", "COT", "COUNT", "CREATE", "CURDATE", "CURRENT", "CURRENT_DATE", 
  "CURRENT_TIME", "CURRENT_TIMESTAMP", "CURTIME", "CURSOR", "DATABASE", 
  "SQL_DATE", "DATE_FORMAT", "DATE_REMAINDER", "DATE_VALUE", "DAY", 
  "DAYOFMONTH", "DAYOFWEEK", "DAYOFYEAR", "DAYS_BETWEEN", "DEALLOCATE", 
  "DEC", "DECLARE", "DEFAULT", "DEFERRABLE", "DEFERRED", "SQL_DELETE", 
  "DESC", "DESCRIBE", "DESCRIPTOR", "DIAGNOSTICS", "DICTIONARY", 
  "DIRECTORY", "DISCONNECT", "DISPLACEMENT", "DISTINCT", "DOMAIN_TOKEN", 
  "SQL_DOUBLE", "DOUBLE_QUOTED_STRING", "DROP", "ELSE", "END", "END_EXEC", 
  "EQUAL", "ESCAPE", "EXCEPT", "SQL_EXCEPTION", "EXEC", "EXECUTE", 
  "EXISTS", "EXP", "EXPONENT", "EXTERNAL", "EXTRACT", "SQL_FALSE", 
  "FETCH", "FIRST", "SQL_FLOAT", "FLOOR", "FN", "FOR", "FOREIGN", 
  "FORTRAN", "FOUND", "FOUR_DIGITS", "FROM", "FULL", "GET", "GLOBAL", 
  "GO", "GOTO", "GRANT", "GREATER_OR_EQUAL", "HAVING", "HOUR", 
  "HOURS_BETWEEN", "IDENTITY", "IFNULL", "SQL_IGNORE", "IMMEDIATE", 
  "SQL_IN", "INCLUDE", "INDEX", "INDICATOR", "INITIALLY", "INNER", 
  "INPUT", "INSENSITIVE", "INSERT", "INTEGER", "INTERSECT", "INTERVAL", 
  "INTO", "IS", "ISOLATION", "JOIN", "JUSTIFY", "KEY", "LANGUAGE", "LAST", 
  "LCASE", "LEFT", "LENGTH", "LESS_OR_EQUAL", "LEVEL", "LIKE", 
  "LINE_WIDTH", "LOCAL", "LOCATE", "LOG", "SQL_LONG", "LOWER", "LTRIM", 
  "LTRIP", "MATCH", "SQL_MAX", "MICROSOFT", "SQL_MIN", "MINUS", "MINUTE", 
  "MINUTES_BETWEEN", "MOD", "MODIFY", "MODULE", "MONTH", "MONTHS_BETWEEN", 
  "MUMPS", "NAMES", "NATIONAL", "NCHAR", "NEXT", "NODUP", "NONE", "NOT", 
  "NOT_EQUAL", "NOT_EQUAL2", "NOW", "SQL_NULL", "SQL_IS", "SQL_IS_NULL", 
  "SQL_IS_NOT_NULL", "NULLIF", "NUMERIC", "OCTET_LENGTH", "ODBC", "OF", 
  "SQL_OFF", "SQL_ON", "ONLY", "OPEN", "OPTION", "OR", "ORDER", "OUTER", 
  "OUTPUT", "OVERLAPS", "PAGE", "PARTIAL", "SQL_PASCAL", "PERSISTENT", 
  "CQL_PI", "PLI", "POSITION", "PRECISION", "PREPARE", "PRESERVE", 
  "PRIMARY", "PRIOR", "PRIVILEGES", "PROCEDURE", "PRODUCT", "PUBLIC", 
  "QUARTER", "QUIT", "RAND", "READ_ONLY", "REAL", "REFERENCES", "REPEAT", 
  "REPLACE", "RESTRICT", "REVOKE", "RIGHT", "ROLLBACK", "ROWS", "RPAD", 
  "RTRIM", "SCHEMA", "SCREEN_WIDTH", "SCROLL", "SECOND", 
  "SECONDS_BETWEEN", "SELECT", "SEQUENCE", "SETOPT", "SET", "SHOWOPT", 
  "SIGN", "SIMILAR_TO", "NOT_SIMILAR_TO", "INTEGER_CONST", "REAL_CONST", 
  "SIN", "SQL_SIZE", "SMALLINT", "SOME", "SPACE", "SQL", "SQL_TRUE", 
  "SQLCA", "SQLCODE", "SQLERROR", "SQLSTATE", "SQLWARNING", "SQRT", 
  "STDEV", "SUBSTRING", "SUM", "SYSDATE", "SYSDATE_FORMAT", "SYSTEM", 
  "TABLE", "TAN", "TEMPORARY", "THEN", "THREE_DIGITS", "TIME", 
  "TIMESTAMP", "TIMEZONE_HOUR", "TIMEZONE_MINUTE", "TINYINT", "TO", 
  "TO_CHAR", "TO_DATE", "TRANSACTION", "TRANSLATE", "TRANSLATION", 
  "TRUNCATE", "GENERAL_TITLE", "TWO_DIGITS", "UCASE", "UNION", "UNIQUE", 
  "SQL_UNKNOWN", "UPDATE", "UPPER", "USAGE", "USER", "IDENTIFIER", 
  "IDENTIFIER_DOT_ASTERISK", "ERROR_DIGIT_BEFORE_IDENTIFIER", "USING", 
  "VALUE", "VALUES", "VARBINARY", "VARCHAR", "VARYING", "VENDOR", "VIEW", 
  "WEEK", "WHEN", "WHENEVER", "WHERE", "WHERE_CURRENT_OF", "WITH", 
  "WORD_WRAPPED", "WORK", "WRAPPED", "XOR", "YEAR", "YEARS_BETWEEN", 
  "__LAST_TOKEN", "'-'", "'+'", "'*'", "'%'", "'@'", "';'", "','", "'.'", 
  "'$'", "'('", "')'", "'?'", "'''", "'/'", "'='", "'<'", "'>'", "ILIKE", 
  "'^'", "'['", "']'", "'&'", "'|'", "'~'", "$accept", 
  "TopLevelStatement", "StatementList", "Statement", 
  "CreateTableStatement", "@1", "ColDefs", "ColDef", "ColKeys", "ColKey", 
  "ColType", "SelectStatement", "Select", "WhereClause", "aExpr", 
  "aExpr2", "aExpr3", "aExpr4", "aExpr5", "aExpr6", "aExpr7", "aExpr8", 
  "aExpr9", "aExpr10", "Tables", "FlatTableList", "FlatTable", "ColViews", 
  "ColItem", "ColExpression", "ColWildCard", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   572,   573,   574,
     575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
     585,   586,   587,   588,   589,   590,   591,   592,   593,   594,
     595,   596,   597,   598,   599,    45,    43,    42,    37,    64,
      59,    44,    46,    36,    40,    41,    63,    39,    47,    61,
      60,    62,   600,    94,    91,    93,    38,   124,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   369,   370,   371,   371,   371,   372,   372,   374,   373,
     375,   375,   376,   376,   377,   377,   378,   378,   378,   379,
     379,   379,   379,   380,   380,   380,   380,   380,   380,   381,
     382,   383,   384,   384,   384,   384,   385,   385,   385,   386,
     386,   386,   386,   386,   387,   387,   387,   387,   388,   388,
     388,   388,   388,   388,   389,   389,   389,   389,   389,   389,
     389,   390,   390,   390,   391,   391,   391,   391,   391,   391,
     391,   391,   391,   391,   391,   392,   393,   394,   394,   395,
     395,   395,   396,   396,   397,   397,   397,   397,   398,   398,
     398,   399,   399
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     3,     1,     2,     1,     1,     0,     7,
       3,     1,     2,     3,     2,     1,     2,     2,     1,     1,
       4,     4,     0,     2,     3,     2,     3,     4,     3,     1,
       2,     1,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     3,     1,     3,     3,
       3,     3,     3,     1,     3,     3,     3,     3,     3,     3,
       1,     2,     2,     1,     2,     2,     2,     2,     1,     3,
       1,     1,     1,     1,     1,     3,     2,     3,     1,     1,
       2,     3,     3,     1,     1,     1,     3,     2,     1,     4,
       4,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,    29,     0,     2,     4,     6,     7,     0,     0,
       1,     5,    71,     0,     0,     0,    70,    72,    73,    68,
       0,     0,    91,     0,     0,    88,    31,    35,    38,    43,
      47,    53,    60,    63,    74,    25,    23,    83,    84,    85,
       8,     3,     0,    79,    76,    78,    68,    67,     0,     0,
      64,    65,     0,    66,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    61,    62,     0,     0,     0,
      28,     0,    26,    24,     0,    87,     0,    68,     0,     0,
      80,     0,     0,    69,    92,     0,    75,    32,    33,    34,
      36,    37,    40,    39,    41,    42,    45,    46,    44,    49,
      51,    52,    50,    48,    57,    56,    54,    55,    58,    59,
      30,    82,    27,    86,     0,    90,    81,    77,    89,    22,
       0,    11,    19,     0,    12,     0,     9,     0,     0,    18,
       0,     0,    13,    15,    10,     0,     0,    17,    16,    14,
      20,    21
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     3,     4,     5,     6,    86,   130,   131,   142,   143,
     134,     7,     8,    80,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    44,    45,    36,    37,    38,
      39
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -331
static const short yypact[] =
{
     -66,  -272,  -331,    26,  -331,  -314,  -331,  -331,   -51,  -292,
    -331,   -66,  -331,  -288,  -259,   -38,  -331,  -331,  -331,  -321,
     -38,   -38,  -331,   -38,   -38,  -331,  -331,   -19,     4,  -329,
    -328,  -142,  -143,  -331,  -331,  -268,  -127,  -331,   -17,  -331,
    -331,  -331,   -45,   -16,  -273,  -331,  -271,  -331,  -308,   -48,
    -331,  -331,  -270,  -331,   -38,   -38,   -38,   -38,   -38,   -38,
     -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
     -38,   -38,   -38,   -38,   -38,  -331,  -331,   -38,   -38,   -38,
    -331,   -48,  -331,  -268,  -241,  -331,  -266,  -275,  -269,  -239,
    -331,  -259,  -238,  -331,  -331,  -330,  -331,  -331,  -331,  -331,
    -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,
    -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,  -331,
    -331,  -331,  -331,  -331,  -237,  -331,  -331,  -331,  -331,    -4,
    -323,  -331,  -265,  -264,   -27,  -237,  -331,  -182,  -181,  -331,
    -117,   -76,   -27,  -331,  -331,  -260,  -258,  -331,  -331,  -331,
    -331,  -331
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -331,  -331,    85,  -331,  -331,  -331,  -331,   -37,  -331,   -43,
    -331,  -331,  -331,   -22,   -11,    17,     1,    -6,    11,   -18,
     -31,  -331,     3,  -331,    64,  -331,    10,    53,    22,    62,
    -331
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
     132,    54,    12,    66,   139,    12,    84,    89,    12,     1,
      71,    14,    52,    93,    82,    12,    59,    60,    47,    63,
      64,    81,     9,    50,    51,   128,    10,    53,   135,    40,
      65,    48,   136,    49,    67,    72,    11,    61,    62,    94,
     114,   115,   116,   117,    57,    58,   118,   119,   109,   110,
     111,   112,   113,   102,   103,   104,   105,    13,   100,   101,
      13,   122,    43,    13,    73,    74,    42,    79,   120,    75,
      76,    97,    98,    99,   106,   107,   108,    92,    91,    49,
     123,    92,   126,    93,   129,    96,   125,    14,   124,   137,
     138,   145,   146,   147,   148,   150,    41,   151,   144,   149,
      83,   127,    95,   121,    88,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    77,    78,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,     0,     0,    15,    16,
       0,    15,    16,     0,     0,    16,     0,     0,    15,     0,
       0,     0,    16,     0,     0,     0,     0,     0,     0,   140,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     2,
       0,     0,     0,     0,     0,    55,     0,     0,    79,     0,
       0,     0,   141,     0,     0,     0,     0,    68,    69,    70,
       0,     0,    17,    18,    81,    17,    18,     0,    17,    18,
       0,     0,     0,     0,     0,    17,    18,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,     0,     0,    19,     0,     0,    87,     0,     0,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    20,    21,    22,    20,    21,    22,
      20,    21,     0,    23,    85,    90,    23,    20,    21,    23,
       0,     0,     0,     0,     0,     0,    23,    24,     0,     0,
      24,     0,    56,    24,   133,     0,     0,     0,     0,     0,
      24
};

static const short yycheck[] =
{
       4,    20,    53,   145,    31,    53,    23,    23,    53,    75,
     153,   138,    23,   321,    36,    53,   345,   346,    15,   347,
     348,   351,   294,    20,    21,   355,     0,    24,   351,   321,
     358,   352,   355,   354,   176,   178,   350,   366,   367,   347,
      71,    72,    73,    74,    40,    41,    77,    78,    66,    67,
      68,    69,    70,    59,    60,    61,    62,   108,    57,    58,
     108,    83,   321,   108,   207,   208,   354,   335,    79,   212,
     213,    54,    55,    56,    63,    64,    65,   352,   351,   354,
     321,   352,   321,   321,   321,   355,   355,   138,   354,   354,
     354,   273,   273,   210,   170,   355,    11,   355,   135,   142,
      36,    91,    49,    81,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   271,   272,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   206,    -1,    -1,   206,   210,
      -1,   206,   210,    -1,    -1,   210,    -1,    -1,   206,    -1,
      -1,    -1,   210,    -1,    -1,    -1,    -1,    -1,    -1,   206,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   265,
      -1,    -1,    -1,    -1,    -1,   224,    -1,    -1,   335,    -1,
      -1,    -1,   239,    -1,    -1,    -1,    -1,   359,   360,   361,
      -1,    -1,   273,   274,   351,   273,   274,    -1,   273,   274,
      -1,    -1,    -1,    -1,    -1,   273,   274,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     321,    -1,    -1,   321,    -1,    -1,   321,    -1,    -1,    -1,
      -1,    -1,    -1,   321,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   345,   346,   347,   345,   346,   347,
     345,   346,    -1,   354,   321,   321,   354,   345,   346,   354,
      -1,    -1,    -1,    -1,    -1,    -1,   354,   368,    -1,    -1,
     368,    -1,   341,   368,   328,    -1,    -1,    -1,    -1,    -1,
     368
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,    75,   265,   370,   371,   372,   373,   380,   381,   294,
       0,   350,    53,   108,   138,   206,   210,   273,   274,   321,
     345,   346,   347,   354,   368,   383,   384,   385,   386,   387,
     388,   389,   390,   391,   392,   393,   396,   397,   398,   399,
     321,   371,   354,   321,   394,   395,   321,   391,   352,   354,
     391,   391,   383,   391,    20,   224,   341,    40,    41,   345,
     346,   366,   367,   347,   348,   358,   145,   176,   359,   360,
     361,   153,   178,   207,   208,   212,   213,   271,   272,   335,
     382,   351,   382,   393,    23,   321,   374,   321,   398,    23,
     321,   351,   352,   321,   347,   396,   355,   384,   384,   384,
     385,   385,   386,   386,   386,   386,   387,   387,   387,   388,
     388,   388,   388,   388,   389,   389,   389,   389,   389,   389,
     383,   397,   382,   321,   354,   355,   321,   395,   355,   321,
     375,   376,     4,   328,   379,   351,   355,   354,   354,    31,
     206,   239,   377,   378,   376,   273,   273,   210,   170,   378,
     355,   355
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)           \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#define YYLEX	yylex ()

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*-----------------------------.
| Print this symbol on YYOUT.  |
`-----------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yysymprint (FILE* yyout, int yytype, YYSTYPE yyvalue)
#else
yysymprint (yyout, yytype, yyvalue)
    FILE* yyout;
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyout, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyout, yytoknum[yytype], yyvalue);
# endif
    }
  else
    YYFPRINTF (yyout, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyout, ")");
}
#endif /* YYDEBUG. */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yydestruct (int yytype, YYSTYPE yyvalue)
#else
yydestruct (yytype, yyvalue)
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  switch (yytype)
    {
      default:
        break;
    }
}



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of parse errors so far.  */
int yynerrs;


int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with.  */

  if (yychar <= 0)		/* This means end of input.  */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more.  */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

      /* We have to keep this `#if YYDEBUG', since we use variables
	 which are defined only if `YYDEBUG' is set.  */
      YYDPRINTF ((stderr, "Next token is "));
      YYDSYMPRINT ((stderr, yychar1, yylval));
      YYDPRINTF ((stderr, "\n"));
    }

  /* If the proper action on seeing token YYCHAR1 is to reduce or to
     detect an error, take that action.  */
  yyn += yychar1;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yychar1)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];



#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn - 1, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] >= 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif
  switch (yyn)
    {
        case 2:
#line 547 "sqlparser.y"
    {
//todo: multiple statements
//todo: not only "select" statements
	parser->setOperation(Parser::OP_Select);
	parser->setQuerySchema(yyvsp[0].querySchema);
}
    break;

  case 3:
#line 557 "sqlparser.y"
    {
//todo: multiple statements
}
    break;

  case 5:
#line 562 "sqlparser.y"
    {
	yyval.querySchema = yyvsp[-1].querySchema;
}
    break;

  case 6:
#line 572 "sqlparser.y"
    {
YYACCEPT;
}
    break;

  case 7:
#line 576 "sqlparser.y"
    {
	yyval.querySchema = yyvsp[0].querySchema;
}
    break;

  case 8:
#line 583 "sqlparser.y"
    {
	parser->setOperation(Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
}
    break;

  case 11:
#line 592 "sqlparser.y"
    {
}
    break;

  case 12:
#line 598 "sqlparser.y"
    {
	kdDebug() << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

//	delete field;
	field = 0;
}
    break;

  case 13:
#line 607 "sqlparser.y"
    {
	kdDebug() << "adding field " << yyvsp[-2].stringValue << endl;
	field->setName(yyvsp[-2].stringValue);
	parser->table()->addField(field);

//	if(field->isPrimaryKey())
//		parser->table()->addPrimaryKey(field->name());

//	delete field;
//	field = 0;
}
    break;

  case 15:
#line 622 "sqlparser.y"
    {
}
    break;

  case 16:
#line 628 "sqlparser.y"
    {
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
}
    break;

  case 17:
#line 633 "sqlparser.y"
    {
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
}
    break;

  case 18:
#line 638 "sqlparser.y"
    {
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
}
    break;

  case 19:
#line 646 "sqlparser.y"
    {
	field = new Field();
	field->setType(yyvsp[0].colType);
}
    break;

  case 20:
#line 651 "sqlparser.y"
    {
	kdDebug() << "sql + length" << endl;
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].colType);
}
    break;

  case 21:
#line 658 "sqlparser.y"
    {
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(Field::Text);
}
    break;

  case 22:
#line 664 "sqlparser.y"
    {
	// SQLITE compatibillity
	field = new Field();
	field->setType(Field::InvalidType);
}
    break;

  case 23:
#line 673 "sqlparser.y"
    {
	kdDebug() << "Select ColViews=" << yyvsp[0].exprList->debugString() << endl;

	if (!(yyval.querySchema = parseSelect( yyvsp[-1].querySchema, yyvsp[0].exprList )))
		return 0;
}
    break;

  case 24:
#line 680 "sqlparser.y"
    {
	if (!(yyval.querySchema = parseSelect( yyvsp[-2].querySchema, yyvsp[-1].exprList, yyvsp[0].exprList )))
		return 0;
}
    break;

  case 25:
#line 685 "sqlparser.y"
    {
	kdDebug() << "Select ColViews Tables" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-1].querySchema, 0, yyvsp[0].exprList )))
		return 0;
}
    break;

  case 26:
#line 691 "sqlparser.y"
    {
	kdDebug() << "Select ColViews Conditions" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-2].querySchema, yyvsp[-1].exprList, 0, yyvsp[0].expr )))
		return 0;
}
    break;

  case 27:
#line 697 "sqlparser.y"
    {
	kdDebug() << "Select ColViews Tables Conditions" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-3].querySchema, yyvsp[-2].exprList, yyvsp[-1].exprList, yyvsp[0].expr )))
		return 0;
}
    break;

  case 28:
#line 703 "sqlparser.y"
    {
	kdDebug() << "Select Tables Conditions" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-2].querySchema, 0, yyvsp[-1].exprList, yyvsp[0].expr )))
		return 0;
}
    break;

  case 29:
#line 712 "sqlparser.y"
    {
	kdDebug() << "SELECT" << endl;
//	parser->createSelect();
//	parser->setOperation(Parser::OP_Select);
	yyval.querySchema = new QuerySchema();
}
    break;

  case 30:
#line 722 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
}
    break;

  case 32:
#line 734 "sqlparser.y"
    {
//	kdDebug() << "AND " << .debugString() << endl;
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, AND, yyvsp[0].expr );
}
    break;

  case 33:
#line 739 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, OR, yyvsp[0].expr );
}
    break;

  case 34:
#line 743 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr( KexiDBExpr_Arithm, yyvsp[-2].expr, XOR, yyvsp[0].expr );
}
    break;

  case 36:
#line 753 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_LEFT, yyvsp[0].expr);
}
    break;

  case 37:
#line 757 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_RIGHT, yyvsp[0].expr);
}
    break;

  case 39:
#line 767 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '+', yyvsp[0].expr);
	yyval.expr->debug();
}
    break;

  case 40:
#line 772 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '-', yyvsp[0].expr);
}
    break;

  case 41:
#line 776 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '&', yyvsp[0].expr);
}
    break;

  case 42:
#line 780 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '|', yyvsp[0].expr);
}
    break;

  case 44:
#line 790 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '/', yyvsp[0].expr);
}
    break;

  case 45:
#line 794 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '*', yyvsp[0].expr);
}
    break;

  case 46:
#line 798 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '%', yyvsp[0].expr);
}
    break;

  case 48:
#line 808 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '>', yyvsp[0].expr);
}
    break;

  case 49:
#line 812 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_OR_EQUAL, yyvsp[0].expr);
}
    break;

  case 50:
#line 816 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '<', yyvsp[0].expr);
}
    break;

  case 51:
#line 820 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_OR_EQUAL, yyvsp[0].expr);
}
    break;

  case 52:
#line 824 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '=', yyvsp[0].expr);
}
    break;

  case 54:
#line 834 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL, yyvsp[0].expr);
}
    break;

  case 55:
#line 839 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL2, yyvsp[0].expr);
}
    break;

  case 56:
#line 843 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LIKE, yyvsp[0].expr);
}
    break;

  case 57:
#line 847 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SQL_IN, yyvsp[0].expr);
}
    break;

  case 58:
#line 851 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SIMILAR_TO, yyvsp[0].expr);
}
    break;

  case 59:
#line 855 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_SIMILAR_TO, yyvsp[0].expr);
}
    break;

  case 61:
#line 865 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( SQL_IS_NULL, yyvsp[-1].expr );
}
    break;

  case 62:
#line 869 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( SQL_IS_NOT_NULL, yyvsp[-1].expr );
}
    break;

  case 64:
#line 880 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( '-', yyvsp[0].expr );
}
    break;

  case 65:
#line 884 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( '+', yyvsp[0].expr );
}
    break;

  case 66:
#line 888 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( '~', yyvsp[0].expr );
}
    break;

  case 67:
#line 892 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( NOT, yyvsp[0].expr );
}
    break;

  case 68:
#line 896 "sqlparser.y"
    {
	yyval.expr = new VariableExpr( QString::fromLatin1(yyvsp[0].stringValue) );
	
//TODO: simplify this later if that's 'only one field name' expression
	kdDebug() << "  + identifier: " << yyvsp[0].stringValue << endl;
//	$$ = new Field();
//	$$->setName();
//	$$->setTable(dummy);

//	parser->select()->addField(field);
//	requiresTable = true;
}
    break;

  case 69:
#line 915 "sqlparser.y"
    {
	yyval.expr = new VariableExpr( QString::fromLatin1(yyvsp[-2].stringValue) + "." + QString::fromLatin1(yyvsp[0].stringValue) );
	kdDebug() << "  + identifier.identifier: " << yyvsp[0].stringValue << "." << yyvsp[-2].stringValue << endl;
//	$$ = new Field();
//	s->setTable();
//	$$->setName();
	//$$->setTable(parser->db()->tableSchema());
//	parser->select()->addField(field);
//??	requiresTable = true;
}
    break;

  case 70:
#line 926 "sqlparser.y"
    {
	yyval.expr = new ConstExpr( SQL_NULL, QVariant() );
	kdDebug() << "  + NULL" << endl;
//	$$ = new Field();
	//$$->setName(QString::null);
}
    break;

  case 71:
#line 933 "sqlparser.y"
    {
	yyval.expr = new ConstExpr( CHARACTER_STRING_LITERAL, yyvsp[0].stringValue );
//	$$ = new Field();
//	$$->setName();
//	parser->select()->addField(field);
	kdDebug() << "  + constant \"" << yyvsp[0].stringValue << "\"" << endl;
}
    break;

  case 72:
#line 941 "sqlparser.y"
    {
	yyval.expr = new ConstExpr( INTEGER_CONST, yyvsp[0].integerValue );
//	$$ = new Field();
//	$$->setName(QString::number());
//	parser->select()->addField(field);
	kdDebug() << "  + int constant: " << yyvsp[0].integerValue << endl;
}
    break;

  case 73:
#line 949 "sqlparser.y"
    {
	yyval.expr = new ConstExpr( REAL_CONST, QPoint( yyvsp[0].realValue.integer, yyvsp[0].realValue.fractional ) );
	kdDebug() << "  + real constant: " << yyvsp[0].realValue.integer << "." << yyvsp[0].realValue.fractional << endl;
}
    break;

  case 75:
#line 960 "sqlparser.y"
    {
	kdDebug() << "(expr)" << endl;
	yyval.expr = new UnaryExpr('(', yyvsp[-1].expr);
}
    break;

  case 76:
#line 988 "sqlparser.y"
    {
	yyval.exprList = yyvsp[0].exprList;
}
    break;

  case 77:
#line 1029 "sqlparser.y"
    {
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add(yyvsp[0].expr);
}
    break;

  case 78:
#line 1034 "sqlparser.y"
    {
	yyval.exprList = new NArgExpr(KexiDBExpr_TableList, IDENTIFIER); //ok?
	yyval.exprList->add(yyvsp[0].expr);
}
    break;

  case 79:
#line 1042 "sqlparser.y"
    {
	kdDebug() << "FROM: '" << yyvsp[0].stringValue << "'" << endl;

//	TableSchema *schema = parser->db()->tableSchema();
//	parser->select()->setParentTable(schema);
//	parser->select()->addTable(schema);
//	requiresTable = false;
	
//addTable();

	yyval.expr = new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue));

	/*
//TODO: this isn't ok for more tables:
	Field::ListIterator it = parser->select()->fieldsIterator();
	for(Field *item; (item = it.current()); ++it)
	{
		if(item->table() == dummy)
		{
			item->setTable(schema);
		}

		if(item->table() && !item->isQueryAsterisk())
		{
			Field *f = item->table()->field(item->name());
			if(!f)
			{
				ParserError err(i18n("Field List Error"), i18n("Unknown column '%1' in table '%2'").arg(item->name()).arg(schema->name()), ctoken, current);
				parser->setError(err);
				yyerror("fieldlisterror");
			}	
		}
	}*/
}
    break;

  case 80:
#line 1077 "sqlparser.y"
    {
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, 
		new VariableExpr(QString::fromLatin1(yyvsp[-1].stringValue)), 0,
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
}
    break;

  case 81:
#line 1086 "sqlparser.y"
    {
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary,
		new VariableExpr(QString::fromLatin1(yyvsp[-2].stringValue)), AS,
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
}
    break;

  case 82:
#line 1100 "sqlparser.y"
    {
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColViews , ColItem" << endl;
}
    break;

  case 83:
#line 1106 "sqlparser.y"
    {
	yyval.exprList = new NArgExpr(0,0);
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColItem" << endl;
}
    break;

  case 84:
#line 1115 "sqlparser.y"
    {
//	$$ = new Field();
//	dummy->addField($$);
//	$$->setExpression(  );
//	parser->select()->addField($$);
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column expr: '" << yyvsp[0].expr->debugString() << "'" << endl;
}
    break;

  case 85:
#line 1124 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column wildcard: '" << yyvsp[0].expr->debugString() << "'" << endl;
}
    break;

  case 86:
#line 1129 "sqlparser.y"
    {
//	$$ = new Field();
//	$$->setExpression(  );
//	parser->select()->addField($$);
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, yyvsp[-2].expr, AS,
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
//		new ConstExpr(IDENTIFIER, QString::fromLocal8Bit())
	);
	kdDebug() << " added column expr: " << yyval.expr->debugString() << endl;
}
    break;

  case 87:
#line 1141 "sqlparser.y"
    {
//	$$ = new Field();
//	$$->setExpression(  );
//	parser->select()->addField($$);
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, yyvsp[-1].expr, 0, 
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
//		new ConstExpr(IDENTIFIER, QString::fromLocal8Bit())
	);
	kdDebug() << " added column expr: " << yyval.expr->debugString() << endl;
}
    break;

  case 88:
#line 1156 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
}
    break;

  case 89:
#line 1160 "sqlparser.y"
    {
	yyval.expr = new FunctionExpr( yyvsp[-3].stringValue, yyvsp[-1].exprList );
}
    break;

  case 90:
#line 1199 "sqlparser.y"
    {
	yyval.expr = yyvsp[-1].expr;
//TODO
//	$$->setName("DISTINCT(" + ->name() + ")");
}
    break;

  case 91:
#line 1208 "sqlparser.y"
    {
	yyval.expr = new VariableExpr("*");
	kdDebug() << "all columns" << endl;

//	QueryAsterisk *ast = new QueryAsterisk(parser->select(), dummy);
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
}
    break;

  case 92:
#line 1217 "sqlparser.y"
    {
	QString s = QString::fromLatin1(yyvsp[-2].stringValue);
	s+=".*";
	yyval.expr = new VariableExpr(s);
	kdDebug() << "  + all columns from " << s << endl;
}
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 2663 "sqlparser.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyssp > yyss)
	    {
	      YYDPRINTF ((stderr, "Error: popping "));
	      YYDSYMPRINT ((stderr,
			    yystos[*yyssp],
			    *yyvsp));
	      YYDPRINTF ((stderr, "\n"));
	      yydestruct (yystos[*yyssp], *yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yydestruct (yychar1, yylval);
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDPRINTF ((stderr, "Error: popping "));
      YYDSYMPRINT ((stderr,
		    yystos[*yyssp], *yyvsp));
      YYDPRINTF ((stderr, "\n"));

      yydestruct (yystos[yystate], *yyvsp);
      yyvsp--;
      yystate = *--yyssp;


#if YYDEBUG
      if (yydebug)
	{
	  short *yyssp1 = yyss - 1;
	  YYFPRINTF (stderr, "Error: state stack now");
	  while (yyssp1 != yyssp)
	    YYFPRINTF (stderr, " %d", *++yyssp1);
	  YYFPRINTF (stderr, "\n");
	}
#endif
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 1231 "sqlparser.y"



const char * const tname(int offset) { return yytname[offset]; }
