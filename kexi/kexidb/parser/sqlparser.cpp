/* A Bison parser, made by GNU Bison 1.875b.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

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
     DATE_CONST = 530,
     DATETIME_CONST = 531,
     TIME_CONST = 532,
     SIN = 533,
     SQL_SIZE = 534,
     SMALLINT = 535,
     SOME = 536,
     SPACE = 537,
     SQL = 538,
     SQL_TRUE = 539,
     SQLCA = 540,
     SQLCODE = 541,
     SQLERROR = 542,
     SQLSTATE = 543,
     SQLWARNING = 544,
     SQRT = 545,
     STDEV = 546,
     SUBSTRING = 547,
     SUM = 548,
     SYSDATE = 549,
     SYSDATE_FORMAT = 550,
     SYSTEM = 551,
     TABLE = 552,
     TAN = 553,
     TEMPORARY = 554,
     THEN = 555,
     THREE_DIGITS = 556,
     TIME = 557,
     TIMESTAMP = 558,
     TIMEZONE_HOUR = 559,
     TIMEZONE_MINUTE = 560,
     TINYINT = 561,
     TO = 562,
     TO_CHAR = 563,
     TO_DATE = 564,
     TRANSACTION = 565,
     TRANSLATE = 566,
     TRANSLATION = 567,
     TRUNCATE = 568,
     GENERAL_TITLE = 569,
     TWO_DIGITS = 570,
     UCASE = 571,
     UNION = 572,
     UNIQUE = 573,
     SQL_UNKNOWN = 574,
     UPDATE = 575,
     UPPER = 576,
     USAGE = 577,
     USER = 578,
     IDENTIFIER = 579,
     IDENTIFIER_DOT_ASTERISK = 580,
     USING = 581,
     VALUE = 582,
     VALUES = 583,
     VARBINARY = 584,
     VARCHAR = 585,
     VARYING = 586,
     VENDOR = 587,
     VIEW = 588,
     WEEK = 589,
     WHEN = 590,
     WHENEVER = 591,
     WHERE = 592,
     WHERE_CURRENT_OF = 593,
     WITH = 594,
     WORD_WRAPPED = 595,
     WORK = 596,
     WRAPPED = 597,
     XOR = 598,
     YEAR = 599,
     YEARS_BETWEEN = 600,
     SCAN_ERROR = 601,
     __LAST_TOKEN = 602,
     ILIKE = 603
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
#define DATE_CONST 530
#define DATETIME_CONST 531
#define TIME_CONST 532
#define SIN 533
#define SQL_SIZE 534
#define SMALLINT 535
#define SOME 536
#define SPACE 537
#define SQL 538
#define SQL_TRUE 539
#define SQLCA 540
#define SQLCODE 541
#define SQLERROR 542
#define SQLSTATE 543
#define SQLWARNING 544
#define SQRT 545
#define STDEV 546
#define SUBSTRING 547
#define SUM 548
#define SYSDATE 549
#define SYSDATE_FORMAT 550
#define SYSTEM 551
#define TABLE 552
#define TAN 553
#define TEMPORARY 554
#define THEN 555
#define THREE_DIGITS 556
#define TIME 557
#define TIMESTAMP 558
#define TIMEZONE_HOUR 559
#define TIMEZONE_MINUTE 560
#define TINYINT 561
#define TO 562
#define TO_CHAR 563
#define TO_DATE 564
#define TRANSACTION 565
#define TRANSLATE 566
#define TRANSLATION 567
#define TRUNCATE 568
#define GENERAL_TITLE 569
#define TWO_DIGITS 570
#define UCASE 571
#define UNION 572
#define UNIQUE 573
#define SQL_UNKNOWN 574
#define UPDATE 575
#define UPPER 576
#define USAGE 577
#define USER 578
#define IDENTIFIER 579
#define IDENTIFIER_DOT_ASTERISK 580
#define USING 581
#define VALUE 582
#define VALUES 583
#define VARBINARY 584
#define VARCHAR 585
#define VARYING 586
#define VENDOR 587
#define VIEW 588
#define WEEK 589
#define WHEN 590
#define WHENEVER 591
#define WHERE 592
#define WHERE_CURRENT_OF 593
#define WITH 594
#define WORD_WRAPPED 595
#define WORK 596
#define WRAPPED 597
#define XOR 598
#define YEAR 599
#define YEARS_BETWEEN 600
#define SCAN_ERROR 601
#define __LAST_TOKEN 602
#define ILIKE 603




/* Copy the first part of user declarations.  */
#line 436 "sqlparser.y"

#ifndef YYDEBUG /* compat. */
# define YYDEBUG 0
#endif
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <assert.h>
#include <limits.h>
//TODO OK?
#ifdef Q_WS_WIN
//workaround for bug on msvc
# undef LLONG_MIN
#endif
#ifndef LLONG_MAX
# define LLONG_MAX     0x7fffffffffffffffLL
#endif
#ifndef LLONG_MIN
# define LLONG_MIN     0x8000000000000000LL
#endif
#ifndef LLONG_MAX
# define ULLONG_MAX    0xffffffffffffffffLL
#endif

#ifdef _WIN32
# include <malloc.h>
#endif

#include <qobject.h>
#include <kdebug.h>
#include <klocale.h>
#include <qptrlist.h>
#include <qcstring.h>
#include <qvariant.h>

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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 508 "sqlparser.y"
typedef union YYSTYPE {
	char stringValue[255];
	Q_LLONG integerValue;
	bool booleanValue;
	struct realType realValue;
	KexiDB::Field::Type colType;
	KexiDB::Field *field;
	KexiDB::BaseExpr *expr;
	KexiDB::NArgExpr *exprList;
	KexiDB::ConstExpr *constExpr;
	KexiDB::QuerySchema *querySchema;
	SelectOptionsInternal *selectOptions;
	OrderByColumnInternal::List *orderByColumns;
	QVariant *variantValue;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 860 "sqlparser.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 872 "sqlparser.tab.c"

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
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

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
	    (To)[yyi] = (From)[yyi];		\
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
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
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
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   334

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  372
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  37
/* YYNRULES -- Number of rules. */
#define YYNRULES  107
/* YYNRULES -- Number of states. */
#define YYNSTATES  175

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   603

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned short yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   356,   351,   369,   360,
     357,   358,   350,   349,   354,   348,   355,   361,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   353,
     363,   362,   364,   359,   352,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   367,     2,   368,   366,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   370,     2,   371,     2,     2,     2,
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
     345,   346,   347,   365
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     9,    11,    14,    16,    18,    19,
      27,    31,    33,    36,    40,    43,    45,    48,    51,    53,
      55,    60,    65,    66,    69,    73,    76,    80,    85,    87,
      89,    93,    98,   103,   106,   108,   111,   115,   120,   122,
     126,   128,   130,   132,   134,   138,   142,   146,   148,   152,
     156,   160,   164,   168,   170,   174,   178,   182,   186,   190,
     194,   196,   199,   202,   204,   208,   212,   214,   218,   222,
     226,   230,   232,   236,   240,   244,   246,   249,   252,   255,
     258,   260,   263,   267,   269,   271,   273,   275,   277,   281,
     285,   289,   293,   296,   300,   302,   304,   307,   311,   315,
     317,   319,   321,   325,   328,   330,   335,   337
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     373,     0,    -1,   374,    -1,   375,   353,   374,    -1,   375,
      -1,   375,   353,    -1,   376,    -1,   383,    -1,    -1,    75,
     297,   324,   377,   357,   378,   358,    -1,   378,   354,   379,
      -1,   379,    -1,   324,   382,    -1,   324,   382,   380,    -1,
     380,   381,    -1,   381,    -1,   239,   170,    -1,   206,   210,
      -1,    31,    -1,     4,    -1,     4,   357,   273,   358,    -1,
     330,   357,   273,   358,    -1,    -1,   384,   405,    -1,   384,
     405,   402,    -1,   384,   402,    -1,   384,   405,   385,    -1,
     384,   405,   402,   385,    -1,   265,    -1,   386,    -1,   225,
      43,   387,    -1,   386,   225,    43,   387,    -1,   225,    43,
     387,   386,    -1,   337,   390,    -1,   388,    -1,   388,   389,
      -1,   388,   354,   387,    -1,   388,   389,   354,   387,    -1,
     324,    -1,   324,   355,   324,    -1,   273,    -1,    25,    -1,
     100,    -1,   391,    -1,   392,    20,   391,    -1,   392,   224,
     391,    -1,   392,   343,   391,    -1,   392,    -1,   393,   364,
     392,    -1,   393,   145,   392,    -1,   393,   363,   392,    -1,
     393,   176,   392,    -1,   393,   362,   392,    -1,   393,    -1,
     394,   207,   393,    -1,   394,   208,   393,    -1,   394,   178,
     393,    -1,   394,   153,   393,    -1,   394,   271,   393,    -1,
     394,   272,   393,    -1,   394,    -1,   394,   212,    -1,   394,
     213,    -1,   395,    -1,   396,    40,   395,    -1,   396,    41,
     395,    -1,   396,    -1,   397,   349,   396,    -1,   397,   348,
     396,    -1,   397,   369,   396,    -1,   397,   370,   396,    -1,
     397,    -1,   398,   361,   397,    -1,   398,   350,   397,    -1,
     398,   351,   397,    -1,   398,    -1,   348,   398,    -1,   349,
     398,    -1,   371,   398,    -1,   206,   398,    -1,   324,    -1,
     324,   400,    -1,   324,   355,   324,    -1,   210,    -1,    53,
      -1,   273,    -1,   274,    -1,   399,    -1,   357,   390,   358,
      -1,   357,   401,   358,    -1,   390,   354,   401,    -1,   390,
     354,   390,    -1,   138,   403,    -1,   403,   354,   404,    -1,
     404,    -1,   324,    -1,   324,   324,    -1,   324,    23,   324,
      -1,   405,   354,   406,    -1,   406,    -1,   407,    -1,   408,
      -1,   407,    23,   324,    -1,   407,   324,    -1,   390,    -1,
     108,   357,   407,   358,    -1,   350,    -1,   324,   355,   350,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   577,   577,   587,   591,   592,   602,   606,   614,   613,
     622,   622,   628,   637,   652,   652,   658,   663,   668,   676,
     681,   688,   695,   703,   710,   715,   721,   727,   736,   746,
     752,   758,   765,   775,   784,   793,   803,   811,   823,   828,
     833,   840,   844,   851,   856,   861,   865,   870,   875,   879,
     883,   887,   891,   896,   901,   906,   910,   914,   918,   922,
     927,   932,   936,   941,   946,   950,   955,   960,   965,   969,
     973,   978,   983,   987,   991,   996,  1002,  1006,  1010,  1014,
    1018,  1031,  1037,  1048,  1055,  1061,  1078,  1084,  1089,  1097,
    1107,  1112,  1121,  1162,  1167,  1175,  1210,  1219,  1233,  1239,
    1248,  1257,  1262,  1274,  1289,  1333,  1342,  1351
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
  "DATE_CONST", "DATETIME_CONST", "TIME_CONST", "SIN", "SQL_SIZE", 
  "SMALLINT", "SOME", "SPACE", "SQL", "SQL_TRUE", "SQLCA", "SQLCODE", 
  "SQLERROR", "SQLSTATE", "SQLWARNING", "SQRT", "STDEV", "SUBSTRING", 
  "SUM", "SYSDATE", "SYSDATE_FORMAT", "SYSTEM", "TABLE", "TAN", 
  "TEMPORARY", "THEN", "THREE_DIGITS", "TIME", "TIMESTAMP", 
  "TIMEZONE_HOUR", "TIMEZONE_MINUTE", "TINYINT", "TO", "TO_CHAR", 
  "TO_DATE", "TRANSACTION", "TRANSLATE", "TRANSLATION", "TRUNCATE", 
  "GENERAL_TITLE", "TWO_DIGITS", "UCASE", "UNION", "UNIQUE", 
  "SQL_UNKNOWN", "UPDATE", "UPPER", "USAGE", "USER", "IDENTIFIER", 
  "IDENTIFIER_DOT_ASTERISK", "USING", "VALUE", "VALUES", "VARBINARY", 
  "VARCHAR", "VARYING", "VENDOR", "VIEW", "WEEK", "WHEN", "WHENEVER", 
  "WHERE", "WHERE_CURRENT_OF", "WITH", "WORD_WRAPPED", "WORK", "WRAPPED", 
  "XOR", "YEAR", "YEARS_BETWEEN", "SCAN_ERROR", "__LAST_TOKEN", "'-'", 
  "'+'", "'*'", "'%'", "'@'", "';'", "','", "'.'", "'$'", "'('", "')'", 
  "'?'", "'''", "'/'", "'='", "'<'", "'>'", "ILIKE", "'^'", "'['", "']'", 
  "'&'", "'|'", "'~'", "$accept", "TopLevelStatement", "StatementList", 
  "Statement", "CreateTableStatement", "@1", "ColDefs", "ColDef", 
  "ColKeys", "ColKey", "ColType", "SelectStatement", "Select", 
  "SelectOptions", "WhereClause", "OrderByClause", "OrderByColumnId", 
  "OrderByOption", "aExpr", "aExpr2", "aExpr3", "aExpr4", "aExpr5", 
  "aExpr6", "aExpr7", "aExpr8", "aExpr9", "aExpr10", "aExprList", 
  "aExprList2", "Tables", "FlatTableList", "FlatTable", "ColViews", 
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
     595,   596,   597,   598,   599,   600,   601,   602,    45,    43,
      42,    37,    64,    59,    44,    46,    36,    40,    41,    63,
      39,    47,    61,    60,    62,   603,    94,    91,    93,    38,
     124,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   372,   373,   374,   374,   374,   375,   375,   377,   376,
     378,   378,   379,   379,   380,   380,   381,   381,   381,   382,
     382,   382,   382,   383,   383,   383,   383,   383,   384,   385,
     385,   385,   385,   386,   387,   387,   387,   387,   388,   388,
     388,   389,   389,   390,   391,   391,   391,   391,   392,   392,
     392,   392,   392,   392,   393,   393,   393,   393,   393,   393,
     393,   394,   394,   394,   395,   395,   395,   396,   396,   396,
     396,   396,   397,   397,   397,   397,   398,   398,   398,   398,
     398,   398,   398,   398,   398,   398,   398,   398,   399,   400,
     401,   401,   402,   403,   403,   404,   404,   404,   405,   405,
     406,   406,   406,   406,   407,   407,   408,   408
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     3,     1,     2,     1,     1,     0,     7,
       3,     1,     2,     3,     2,     1,     2,     2,     1,     1,
       4,     4,     0,     2,     3,     2,     3,     4,     1,     1,
       3,     4,     4,     2,     1,     2,     3,     4,     1,     3,
       1,     1,     1,     1,     3,     3,     3,     1,     3,     3,
       3,     3,     3,     1,     3,     3,     3,     3,     3,     3,
       1,     2,     2,     1,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     3,     1,     2,     2,     2,     2,
       1,     2,     3,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     2,     3,     1,     1,     2,     3,     3,     1,
       1,     1,     3,     2,     1,     4,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,    28,     0,     2,     4,     6,     7,     0,     0,
       1,     5,    84,     0,     0,     0,    83,    85,    86,    80,
       0,     0,   106,     0,     0,   104,    43,    47,    53,    60,
      63,    66,    71,    75,    87,    25,    23,    99,   100,   101,
       8,     3,     0,    95,    92,    94,    80,    79,     0,     0,
      81,    76,    77,     0,    78,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    61,    62,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    26,    29,    24,     0,   103,     0,     0,
       0,    96,     0,     0,    82,   107,     0,     0,    88,    44,
      45,    46,    49,    51,    52,    50,    48,    57,    56,    54,
      55,    58,    59,    64,    65,    68,    67,    69,    70,    73,
      74,    72,     0,    33,    98,     0,    27,   102,     0,   105,
      97,    93,     0,    89,    40,    38,    30,    34,     0,    22,
       0,    11,    91,    90,     0,    32,    41,    42,     0,    35,
      31,    19,     0,    12,     0,     9,    39,    36,     0,     0,
       0,    18,     0,     0,    13,    15,    10,    37,     0,     0,
      17,    16,    14,    20,    21
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     3,     4,     5,     6,    88,   140,   141,   164,   165,
     153,     7,     8,    83,    84,   136,   137,   149,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    50,    97,
      35,    44,    45,    36,    37,    38,    39
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -334
static const short yypact[] =
{
     -67,  -275,  -334,    26,  -334,  -319,  -334,  -334,   -50,  -283,
    -334,   -67,  -334,  -300,  -262,   -37,  -334,  -334,  -334,  -326,
     -37,   -37,  -334,   -37,   -37,  -334,  -334,   -18,  -134,  -143,
    -334,    10,  -325,  -331,  -334,  -334,  -133,  -334,   -19,  -334,
    -334,  -334,   -40,   -11,  -291,  -334,  -318,  -334,  -307,   -37,
    -334,  -334,  -334,  -287,  -334,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,  -334,  -334,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
      24,   -37,   -47,  -334,  -152,  -216,  -250,  -334,  -272,  -271,
    -238,  -334,  -262,  -235,  -334,  -334,  -264,  -267,  -334,  -334,
    -334,  -334,  -334,  -334,  -334,  -334,  -334,  -334,  -334,  -334,
    -334,  -334,  -334,  -334,  -334,  -334,  -334,  -334,  -334,  -334,
    -334,  -334,  -258,  -334,  -334,    53,  -334,  -334,  -227,  -334,
    -334,  -334,   -37,  -334,  -334,  -257,  -237,   -25,  -258,    -3,
    -333,  -334,  -264,  -334,  -225,  -334,  -334,  -334,  -258,  -253,
    -334,  -255,  -254,   -24,  -227,  -334,  -334,  -334,  -258,  -169,
    -168,  -334,  -104,   -63,   -24,  -334,  -334,  -334,  -249,  -248,
    -334,  -334,  -334,  -334,  -334
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -334,  -334,    97,  -334,  -334,  -334,  -334,   -43,  -334,   -52,
    -334,  -334,  -334,    28,   -22,  -120,  -334,  -334,    -9,    -1,
      18,   -17,  -334,   -12,     8,    16,    12,  -334,  -334,   -16,
      79,  -334,    25,  -334,    36,    77,  -334
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
     146,   151,    55,    12,    86,    14,    12,   161,     1,    80,
      63,    58,    90,    12,    53,   134,    12,    94,   150,    77,
      78,   154,     9,    73,    74,   155,    10,    47,   157,    48,
      79,    49,    51,    52,    11,    64,    54,    93,   167,    49,
      96,    40,    59,    95,    75,    76,   107,   108,   109,   110,
      71,    72,   111,   112,    99,   100,   101,    42,    13,   113,
     114,    13,    43,    92,    65,    66,   135,   122,    13,    67,
      68,    98,   123,   125,   127,   147,   102,   103,   104,   105,
     106,   115,   116,   117,   118,   128,   130,   129,    14,    94,
     132,   133,    80,   119,   120,   121,   138,   139,   144,   156,
      81,   158,   159,   160,   168,   169,   170,   171,    41,   173,
     174,   166,   172,   126,   145,    85,   143,   131,   124,    89,
       0,    81,     0,   142,     0,     0,     0,     0,    69,    70,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,     0,     0,    15,
      16,     0,     0,    16,     0,     0,    15,     0,     0,    15,
      16,     0,     0,    16,     0,     0,     0,     0,     0,     0,
       0,     0,   162,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     2,     0,
       0,     0,     0,     0,    81,     0,    56,     0,     0,     0,
       0,     0,     0,     0,     0,   163,     0,     0,     0,     0,
       0,    82,     0,    17,    18,     0,    17,    18,    60,    61,
      62,     0,     0,    17,    18,     0,    17,    18,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    19,     0,     0,    19,     0,     0,
       0,     0,     0,     0,    46,     0,     0,    46,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    20,    21,
      22,    20,    21,    22,     0,    87,     0,    23,    20,    21,
      23,    20,    21,    91,     0,     0,     0,    23,     0,     0,
      23,    24,     0,     0,    24,    57,     0,   152,     0,   148,
       0,    24,     0,     0,    24
};

static const short yycheck[] =
{
      25,     4,    20,    53,    23,   138,    53,    31,    75,   225,
     153,   145,    23,    53,    23,   273,    53,   324,   138,   350,
     351,   354,   297,   348,   349,   358,     0,    15,   148,   355,
     361,   357,    20,    21,   353,   178,    24,   355,   158,   357,
      49,   324,   176,   350,   369,   370,    63,    64,    65,    66,
      40,    41,    69,    70,    55,    56,    57,   357,   108,    71,
      72,   108,   324,   354,   207,   208,   324,    43,   108,   212,
     213,   358,    81,   225,   324,   100,    58,    59,    60,    61,
      62,    73,    74,    75,    76,   357,   324,   358,   138,   324,
     354,   358,   225,    77,    78,    79,    43,   324,   355,   324,
     337,   354,   357,   357,   273,   273,   210,   170,    11,   358,
     358,   154,   164,    85,   136,    36,   132,    92,    82,    42,
      -1,   337,    -1,   132,    -1,    -1,    -1,    -1,   271,   272,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   206,    -1,    -1,   206,
     210,    -1,    -1,   210,    -1,    -1,   206,    -1,    -1,   206,
     210,    -1,    -1,   210,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   206,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   265,    -1,
      -1,    -1,    -1,    -1,   337,    -1,   224,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   239,    -1,    -1,    -1,    -1,
      -1,   354,    -1,   273,   274,    -1,   273,   274,   362,   363,
     364,    -1,    -1,   273,   274,    -1,   273,   274,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   324,    -1,    -1,   324,    -1,    -1,
      -1,    -1,    -1,    -1,   324,    -1,    -1,   324,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   348,   349,
     350,   348,   349,   350,    -1,   324,    -1,   357,   348,   349,
     357,   348,   349,   324,    -1,    -1,    -1,   357,    -1,    -1,
     357,   371,    -1,    -1,   371,   343,    -1,   330,    -1,   354,
      -1,   371,    -1,    -1,   371
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,    75,   265,   373,   374,   375,   376,   383,   384,   297,
       0,   353,    53,   108,   138,   206,   210,   273,   274,   324,
     348,   349,   350,   357,   371,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   402,   405,   406,   407,   408,
     324,   374,   357,   324,   403,   404,   324,   398,   355,   357,
     400,   398,   398,   390,   398,    20,   224,   343,   145,   176,
     362,   363,   364,   153,   178,   207,   208,   212,   213,   271,
     272,    40,    41,   348,   349,   369,   370,   350,   351,   361,
     225,   337,   354,   385,   386,   402,    23,   324,   377,   407,
      23,   324,   354,   355,   324,   350,   390,   401,   358,   391,
     391,   391,   392,   392,   392,   392,   392,   393,   393,   393,
     393,   393,   393,   395,   395,   396,   396,   396,   396,   397,
     397,   397,    43,   390,   406,   225,   385,   324,   357,   358,
     324,   404,   354,   358,   273,   324,   387,   388,    43,   324,
     378,   379,   390,   401,   355,   386,    25,   100,   354,   389,
     387,     4,   330,   382,   354,   358,   324,   387,   354,   357,
     357,    31,   206,   239,   380,   381,   379,   387,   273,   273,
     210,   170,   381,   358,   358
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
#define YYEMPTY		(-2)
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
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

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

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
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
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

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

  if (yyss + yystacksize - 1 <= yyssp)
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
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
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

      if (yyss + yystacksize - 1 <= yyssp)
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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
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
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 578 "sqlparser.y"
    {
//todo: multiple statements
//todo: not only "select" statements
	parser->setOperation(Parser::OP_Select);
	parser->setQuerySchema(yyvsp[0].querySchema);
;}
    break;

  case 3:
#line 588 "sqlparser.y"
    {
//todo: multiple statements
;}
    break;

  case 5:
#line 593 "sqlparser.y"
    {
	yyval.querySchema = yyvsp[-1].querySchema;
;}
    break;

  case 6:
#line 603 "sqlparser.y"
    {
YYACCEPT;
;}
    break;

  case 7:
#line 607 "sqlparser.y"
    {
	yyval.querySchema = yyvsp[0].querySchema;
;}
    break;

  case 8:
#line 614 "sqlparser.y"
    {
	parser->setOperation(Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
;}
    break;

  case 11:
#line 623 "sqlparser.y"
    {
;}
    break;

  case 12:
#line 629 "sqlparser.y"
    {
	KexiDBDbg << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

//	delete field;
	field = 0;
;}
    break;

  case 13:
#line 638 "sqlparser.y"
    {
	KexiDBDbg << "adding field " << yyvsp[-2].stringValue << endl;
	field->setName(yyvsp[-2].stringValue);
	parser->table()->addField(field);

//	if(field->isPrimaryKey())
//		parser->table()->addPrimaryKey(field->name());

//	delete field;
//	field = 0;
;}
    break;

  case 15:
#line 653 "sqlparser.y"
    {
;}
    break;

  case 16:
#line 659 "sqlparser.y"
    {
	field->setPrimaryKey(true);
	KexiDBDbg << "primary" << endl;
;}
    break;

  case 17:
#line 664 "sqlparser.y"
    {
	field->setNotNull(true);
	KexiDBDbg << "not_null" << endl;
;}
    break;

  case 18:
#line 669 "sqlparser.y"
    {
	field->setAutoIncrement(true);
	KexiDBDbg << "ainc" << endl;
;}
    break;

  case 19:
#line 677 "sqlparser.y"
    {
	field = new Field();
	field->setType(yyvsp[0].colType);
;}
    break;

  case 20:
#line 682 "sqlparser.y"
    {
	KexiDBDbg << "sql + length" << endl;
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].colType);
;}
    break;

  case 21:
#line 689 "sqlparser.y"
    {
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(Field::Text);
;}
    break;

  case 22:
#line 695 "sqlparser.y"
    {
	// SQLITE compatibillity
	field = new Field();
	field->setType(Field::InvalidType);
;}
    break;

  case 23:
#line 704 "sqlparser.y"
    {
	KexiDBDbg << "Select ColViews=" << yyvsp[0].exprList->debugString() << endl;

	if (!(yyval.querySchema = buildSelectQuery( yyvsp[-1].querySchema, yyvsp[0].exprList )))
		return 0;
;}
    break;

  case 24:
#line 711 "sqlparser.y"
    {
	if (!(yyval.querySchema = buildSelectQuery( yyvsp[-2].querySchema, yyvsp[-1].exprList, yyvsp[0].exprList )))
		return 0;
;}
    break;

  case 25:
#line 716 "sqlparser.y"
    {
	KexiDBDbg << "Select ColViews Tables" << endl;
	if (!(yyval.querySchema = buildSelectQuery( yyvsp[-1].querySchema, 0, yyvsp[0].exprList )))
		return 0;
;}
    break;

  case 26:
#line 722 "sqlparser.y"
    {
	KexiDBDbg << "Select ColViews Conditions" << endl;
	if (!(yyval.querySchema = buildSelectQuery( yyvsp[-2].querySchema, yyvsp[-1].exprList, 0, yyvsp[0].selectOptions )))
		return 0;
;}
    break;

  case 27:
#line 728 "sqlparser.y"
    {
	KexiDBDbg << "Select ColViews Tables SelectOptions" << endl;
	if (!(yyval.querySchema = buildSelectQuery( yyvsp[-3].querySchema, yyvsp[-2].exprList, yyvsp[-1].exprList, yyvsp[0].selectOptions )))
		return 0;
;}
    break;

  case 28:
#line 737 "sqlparser.y"
    {
	KexiDBDbg << "SELECT" << endl;
//	parser->createSelect();
//	parser->setOperation(Parser::OP_Select);
	yyval.querySchema = new QuerySchema();
;}
    break;

  case 29:
#line 747 "sqlparser.y"
    {
	KexiDBDbg << "WhereClause" << endl;
	yyval.selectOptions = new SelectOptionsInternal;
	yyval.selectOptions->whereExpr = yyvsp[0].expr;
;}
    break;

  case 30:
#line 753 "sqlparser.y"
    {
	KexiDBDbg << "OrderByClause" << endl;
	yyval.selectOptions = new SelectOptionsInternal;
	yyval.selectOptions->orderByColumns = yyvsp[0].orderByColumns;
;}
    break;

  case 31:
#line 759 "sqlparser.y"
    {
	KexiDBDbg << "WhereClause ORDER BY OrderByClause" << endl;
	yyval.selectOptions = new SelectOptionsInternal;
	yyval.selectOptions->whereExpr = yyvsp[-3].expr;
	yyval.selectOptions->orderByColumns = yyvsp[0].orderByColumns;
;}
    break;

  case 32:
#line 766 "sqlparser.y"
    {
	KexiDBDbg << "OrderByClause WhereClause" << endl;
	yyval.selectOptions = new SelectOptionsInternal;
	yyval.selectOptions->whereExpr = yyvsp[0].expr;
	yyval.selectOptions->orderByColumns = yyvsp[-1].orderByColumns;
;}
    break;

  case 33:
#line 776 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
;}
    break;

  case 34:
#line 785 "sqlparser.y"
    {
	KexiDBDbg << "ORDER BY IDENTIFIER" << endl;
	yyval.orderByColumns = new OrderByColumnInternal::List;
	OrderByColumnInternal orderByColumn;
	orderByColumn.setColumnByNameOrNumber( *yyvsp[0].variantValue );
	yyval.orderByColumns->append( orderByColumn );
	delete yyvsp[0].variantValue;
;}
    break;

  case 35:
#line 794 "sqlparser.y"
    {
	KexiDBDbg << "ORDER BY IDENTIFIER OrderByOption" << endl;
	yyval.orderByColumns = new OrderByColumnInternal::List;
	OrderByColumnInternal orderByColumn;
	orderByColumn.setColumnByNameOrNumber( *yyvsp[-1].variantValue );
	orderByColumn.ascending = yyvsp[0].booleanValue;
	yyval.orderByColumns->append( orderByColumn );
	delete yyvsp[-1].variantValue;
;}
    break;

  case 36:
#line 804 "sqlparser.y"
    {
	yyval.orderByColumns = yyvsp[0].orderByColumns;
	OrderByColumnInternal orderByColumn;
	orderByColumn.setColumnByNameOrNumber( *yyvsp[-2].variantValue );
	yyval.orderByColumns->append( orderByColumn );
	delete yyvsp[-2].variantValue;
;}
    break;

  case 37:
#line 812 "sqlparser.y"
    {
	yyval.orderByColumns = yyvsp[0].orderByColumns;
	OrderByColumnInternal orderByColumn;
	orderByColumn.setColumnByNameOrNumber( *yyvsp[-3].variantValue );
	orderByColumn.ascending = yyvsp[-2].booleanValue;
	yyval.orderByColumns->append( orderByColumn );
	delete yyvsp[-3].variantValue;
;}
    break;

  case 38:
#line 824 "sqlparser.y"
    {
	yyval.variantValue = new QVariant( QString::fromUtf8(yyvsp[0].stringValue) );
	KexiDBDbg << "OrderByColumnId: " << *yyval.variantValue << endl;
;}
    break;

  case 39:
#line 829 "sqlparser.y"
    {
	yyval.variantValue = new QVariant( QString::fromUtf8(yyvsp[-2].stringValue)+"."+QString::fromUtf8(yyvsp[0].stringValue) );
	KexiDBDbg << "OrderByColumnId: " << *yyval.variantValue << endl;
;}
    break;

  case 40:
#line 834 "sqlparser.y"
    {
	yyval.variantValue = new QVariant(yyvsp[0].integerValue);
	KexiDBDbg << "OrderByColumnId: " << *yyval.variantValue << endl;
;}
    break;

  case 41:
#line 841 "sqlparser.y"
    {
	yyval.booleanValue = true;
;}
    break;

  case 42:
#line 845 "sqlparser.y"
    {
	yyval.booleanValue = false;
;}
    break;

  case 44:
#line 857 "sqlparser.y"
    {
//	KexiDBDbg << "AND " << $3.debugString() << endl;
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, AND, yyvsp[0].expr );
;}
    break;

  case 45:
#line 862 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, OR, yyvsp[0].expr );
;}
    break;

  case 46:
#line 866 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr( KexiDBExpr_Arithm, yyvsp[-2].expr, XOR, yyvsp[0].expr );
;}
    break;

  case 48:
#line 876 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '>', yyvsp[0].expr);
;}
    break;

  case 49:
#line 880 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_OR_EQUAL, yyvsp[0].expr);
;}
    break;

  case 50:
#line 884 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '<', yyvsp[0].expr);
;}
    break;

  case 51:
#line 888 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_OR_EQUAL, yyvsp[0].expr);
;}
    break;

  case 52:
#line 892 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '=', yyvsp[0].expr);
;}
    break;

  case 54:
#line 902 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL, yyvsp[0].expr);
;}
    break;

  case 55:
#line 907 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL2, yyvsp[0].expr);
;}
    break;

  case 56:
#line 911 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LIKE, yyvsp[0].expr);
;}
    break;

  case 57:
#line 915 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SQL_IN, yyvsp[0].expr);
;}
    break;

  case 58:
#line 919 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SIMILAR_TO, yyvsp[0].expr);
;}
    break;

  case 59:
#line 923 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_SIMILAR_TO, yyvsp[0].expr);
;}
    break;

  case 61:
#line 933 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( SQL_IS_NULL, yyvsp[-1].expr );
;}
    break;

  case 62:
#line 937 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( SQL_IS_NOT_NULL, yyvsp[-1].expr );
;}
    break;

  case 64:
#line 947 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_LEFT, yyvsp[0].expr);
;}
    break;

  case 65:
#line 951 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_RIGHT, yyvsp[0].expr);
;}
    break;

  case 67:
#line 961 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '+', yyvsp[0].expr);
	yyval.expr->debug();
;}
    break;

  case 68:
#line 966 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '-', yyvsp[0].expr);
;}
    break;

  case 69:
#line 970 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '&', yyvsp[0].expr);
;}
    break;

  case 70:
#line 974 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '|', yyvsp[0].expr);
;}
    break;

  case 72:
#line 984 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '/', yyvsp[0].expr);
;}
    break;

  case 73:
#line 988 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '*', yyvsp[0].expr);
;}
    break;

  case 74:
#line 992 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '%', yyvsp[0].expr);
;}
    break;

  case 76:
#line 1003 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( '-', yyvsp[0].expr );
;}
    break;

  case 77:
#line 1007 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( '+', yyvsp[0].expr );
;}
    break;

  case 78:
#line 1011 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( '~', yyvsp[0].expr );
;}
    break;

  case 79:
#line 1015 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( NOT, yyvsp[0].expr );
;}
    break;

  case 80:
#line 1019 "sqlparser.y"
    {
	yyval.expr = new VariableExpr( QString::fromUtf8(yyvsp[0].stringValue) );
	
//TODO: simplify this later if that's 'only one field name' expression
	KexiDBDbg << "  + identifier: " << yyvsp[0].stringValue << endl;
//	$$ = new Field();
//	$$->setName($1);
//	$$->setTable(dummy);

//	parser->select()->addField(field);
//	requiresTable = true;
;}
    break;

  case 81:
#line 1032 "sqlparser.y"
    {
	KexiDBDbg << "  + function: " << yyvsp[-1].stringValue << "(" << yyvsp[0].exprList->debugString() << ")" << endl;
	yyval.expr = new FunctionExpr(yyvsp[-1].stringValue, yyvsp[0].exprList);
;}
    break;

  case 82:
#line 1038 "sqlparser.y"
    {
	yyval.expr = new VariableExpr( QString::fromUtf8(yyvsp[-2].stringValue) + "." + QString::fromUtf8(yyvsp[0].stringValue) );
	KexiDBDbg << "  + identifier.identifier: " << yyvsp[0].stringValue << "." << yyvsp[-2].stringValue << endl;
//	$$ = new Field();
//	s->setTable($1);
//	$$->setName($3);
	//$$->setTable(parser->db()->tableSchema($1));
//	parser->select()->addField(field);
//??	requiresTable = true;
;}
    break;

  case 83:
#line 1049 "sqlparser.y"
    {
	yyval.expr = new ConstExpr( SQL_NULL, QVariant() );
	KexiDBDbg << "  + NULL" << endl;
//	$$ = new Field();
	//$$->setName(QString::null);
;}
    break;

  case 84:
#line 1056 "sqlparser.y"
    {
	QString s( QString::fromUtf8(yyvsp[0].stringValue) );
	yyval.expr = new ConstExpr( CHARACTER_STRING_LITERAL, s.mid(1,s.length()-2) );
	KexiDBDbg << "  + constant " << s << endl;
;}
    break;

  case 85:
#line 1062 "sqlparser.y"
    {
	QVariant val;
	if (yyvsp[0].integerValue <= INT_MAX && yyvsp[0].integerValue >= INT_MIN)
		val = (int)yyvsp[0].integerValue;
	else if (yyvsp[0].integerValue <= UINT_MAX && yyvsp[0].integerValue >= 0)
		val = (uint)yyvsp[0].integerValue;
	else if (yyvsp[0].integerValue <= (Q_LLONG)LLONG_MAX && yyvsp[0].integerValue >= (Q_LLONG)LLONG_MIN)
		val = (Q_LLONG)yyvsp[0].integerValue;

//	if ($1 < ULLONG_MAX)
//		val = (Q_ULLONG)$1;
//TODO ok?

	yyval.expr = new ConstExpr( INTEGER_CONST, val );
	KexiDBDbg << "  + int constant: " << val.toString() << endl;
;}
    break;

  case 86:
#line 1079 "sqlparser.y"
    {
	yyval.expr = new ConstExpr( REAL_CONST, QPoint( yyvsp[0].realValue.integer, yyvsp[0].realValue.fractional ) );
	KexiDBDbg << "  + real constant: " << yyvsp[0].realValue.integer << "." << yyvsp[0].realValue.fractional << endl;
;}
    break;

  case 88:
#line 1090 "sqlparser.y"
    {
	KexiDBDbg << "(expr)" << endl;
	yyval.expr = new UnaryExpr('(', yyvsp[-1].expr);
;}
    break;

  case 89:
#line 1098 "sqlparser.y"
    {
//	$$ = new NArgExpr(0, 0);
//	$$->add( $1 );
//	$$->add( $3 );
	yyval.exprList = yyvsp[-1].exprList;
;}
    break;

  case 90:
#line 1108 "sqlparser.y"
    {
	yyval.exprList = yyvsp[0].exprList;
	yyval.exprList->prepend( yyvsp[-2].expr );
;}
    break;

  case 91:
#line 1113 "sqlparser.y"
    {
	yyval.exprList = new NArgExpr(0, 0);
	yyval.exprList->add( yyvsp[-2].expr );
	yyval.exprList->add( yyvsp[0].expr );
;}
    break;

  case 92:
#line 1122 "sqlparser.y"
    {
	yyval.exprList = yyvsp[0].exprList;
;}
    break;

  case 93:
#line 1163 "sqlparser.y"
    {
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add(yyvsp[0].expr);
;}
    break;

  case 94:
#line 1168 "sqlparser.y"
    {
	yyval.exprList = new NArgExpr(KexiDBExpr_TableList, IDENTIFIER); //ok?
	yyval.exprList->add(yyvsp[0].expr);
;}
    break;

  case 95:
#line 1176 "sqlparser.y"
    {
	KexiDBDbg << "FROM: '" << yyvsp[0].stringValue << "'" << endl;

//	TableSchema *schema = parser->db()->tableSchema($1);
//	parser->select()->setParentTable(schema);
//	parser->select()->addTable(schema);
//	requiresTable = false;
	
//addTable($1);

	yyval.expr = new VariableExpr(QString::fromUtf8(yyvsp[0].stringValue));

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
;}
    break;

  case 96:
#line 1211 "sqlparser.y"
    {
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, 
		new VariableExpr(QString::fromUtf8(yyvsp[-1].stringValue)), 0,
		new VariableExpr(QString::fromUtf8(yyvsp[0].stringValue))
	);
;}
    break;

  case 97:
#line 1220 "sqlparser.y"
    {
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary,
		new VariableExpr(QString::fromUtf8(yyvsp[-2].stringValue)), AS,
		new VariableExpr(QString::fromUtf8(yyvsp[0].stringValue))
	);
;}
    break;

  case 98:
#line 1234 "sqlparser.y"
    {
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add( yyvsp[0].expr );
	KexiDBDbg << "ColViews: ColViews , ColItem" << endl;
;}
    break;

  case 99:
#line 1240 "sqlparser.y"
    {
	yyval.exprList = new NArgExpr(0,0);
	yyval.exprList->add( yyvsp[0].expr );
	KexiDBDbg << "ColViews: ColItem" << endl;
;}
    break;

  case 100:
#line 1249 "sqlparser.y"
    {
//	$$ = new Field();
//	dummy->addField($$);
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = yyvsp[0].expr;
	KexiDBDbg << " added column expr: '" << yyvsp[0].expr->debugString() << "'" << endl;
;}
    break;

  case 101:
#line 1258 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
	KexiDBDbg << " added column wildcard: '" << yyvsp[0].expr->debugString() << "'" << endl;
;}
    break;

  case 102:
#line 1263 "sqlparser.y"
    {
//	$$ = new Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, yyvsp[-2].expr, AS,
		new VariableExpr(QString::fromUtf8(yyvsp[0].stringValue))
//		new ConstExpr(IDENTIFIER, QString::fromLocal8Bit($3))
	);
	KexiDBDbg << " added column expr: " << yyval.expr->debugString() << endl;
;}
    break;

  case 103:
#line 1275 "sqlparser.y"
    {
//	$$ = new Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, yyvsp[-1].expr, 0, 
		new VariableExpr(QString::fromUtf8(yyvsp[0].stringValue))
//		new ConstExpr(IDENTIFIER, QString::fromLocal8Bit($2))
	);
	KexiDBDbg << " added column expr: " << yyval.expr->debugString() << endl;
;}
    break;

  case 104:
#line 1290 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
;}
    break;

  case 105:
#line 1334 "sqlparser.y"
    {
	yyval.expr = yyvsp[-1].expr;
//TODO
//	$$->setName("DISTINCT(" + $3->name() + ")");
;}
    break;

  case 106:
#line 1343 "sqlparser.y"
    {
	yyval.expr = new VariableExpr("*");
	KexiDBDbg << "all columns" << endl;

//	QueryAsterisk *ast = new QueryAsterisk(parser->select(), dummy);
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
;}
    break;

  case 107:
#line 1352 "sqlparser.y"
    {
	QString s = QString::fromUtf8(yyvsp[-2].stringValue);
	s+=".*";
	yyval.expr = new VariableExpr(s);
	KexiDBDbg << "  + all columns from " << s << endl;
;}
    break;


    }

/* Line 999 of yacc.c.  */
#line 2915 "sqlparser.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

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
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



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
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
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


#line 1366 "sqlparser.y"



const char * const tname(int offset) { return yytname[offset]; }
