/* A Bison parser, made by GNU Bison 1.875a.  */

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
     SQL_TYPE = 258,
     SQL_ABS = 259,
     ACOS = 260,
     AMPERSAND = 261,
     SQL_ABSOLUTE = 262,
     ADA = 263,
     ADD = 264,
     ADD_DAYS = 265,
     ADD_HOURS = 266,
     ADD_MINUTES = 267,
     ADD_MONTHS = 268,
     ADD_SECONDS = 269,
     ADD_YEARS = 270,
     ALL = 271,
     ALLOCATE = 272,
     ALTER = 273,
     AND = 274,
     ANY = 275,
     ARE = 276,
     ARITHMETIC_MINUS = 277,
     ARITHMETIC_PLUS = 278,
     AS = 279,
     ASIN = 280,
     ASC = 281,
     ASCII = 282,
     ASSERTION = 283,
     ASTERISK = 284,
     AT = 285,
     ATAN = 286,
     ATAN2 = 287,
     AUTHORIZATION = 288,
     AUTO_INCREMENT = 289,
     AVG = 290,
     BEFORE = 291,
     SQL_BEGIN = 292,
     BETWEEN = 293,
     BIGINT = 294,
     BINARY = 295,
     BIT = 296,
     BIT_LENGTH = 297,
     BREAK = 298,
     BY = 299,
     CASCADE = 300,
     CASCADED = 301,
     CASE = 302,
     CAST = 303,
     CATALOG = 304,
     CEILING = 305,
     CENTER = 306,
     SQL_CHAR = 307,
     CHAR_LENGTH = 308,
     CHARACTER_STRING_LITERAL = 309,
     CHECK = 310,
     CLOSE = 311,
     COALESCE = 312,
     COBOL = 313,
     COLLATE = 314,
     COLLATION = 315,
     COLON = 316,
     COLUMN = 317,
     COMMA = 318,
     COMMIT = 319,
     COMPUTE = 320,
     CONCAT = 321,
     CONNECT = 322,
     CONNECTION = 323,
     CONSTRAINT = 324,
     CONSTRAINTS = 325,
     CONTINUE = 326,
     CONVERT = 327,
     CORRESPONDING = 328,
     COS = 329,
     COT = 330,
     COUNT = 331,
     CREATE = 332,
     CURDATE = 333,
     CURRENT = 334,
     CURRENT_DATE = 335,
     CURRENT_TIME = 336,
     CURRENT_TIMESTAMP = 337,
     CURTIME = 338,
     CURSOR = 339,
     DATABASE = 340,
     SQL_DATE = 341,
     DATE_FORMAT = 342,
     DATE_REMAINDER = 343,
     DATE_VALUE = 344,
     DAY = 345,
     DAYOFMONTH = 346,
     DAYOFWEEK = 347,
     DAYOFYEAR = 348,
     DAYS_BETWEEN = 349,
     DEALLOCATE = 350,
     DEC = 351,
     DECLARE = 352,
     DEFAULT = 353,
     DEFERRABLE = 354,
     DEFERRED = 355,
     SQL_DELETE = 356,
     DESC = 357,
     DESCRIBE = 358,
     DESCRIPTOR = 359,
     DIAGNOSTICS = 360,
     DICTIONARY = 361,
     DIRECTORY = 362,
     DISCONNECT = 363,
     DISPLACEMENT = 364,
     DISTINCT = 365,
     DOLLAR_SIGN = 366,
     DOMAIN_TOKEN = 367,
     DOT = 368,
     SQL_DOUBLE = 369,
     DOUBLE_QUOTED_STRING = 370,
     DROP = 371,
     ELSE = 372,
     END = 373,
     END_EXEC = 374,
     EQUAL = 375,
     ESCAPE = 376,
     EXCEPT = 377,
     SQL_EXCEPTION = 378,
     EXEC = 379,
     EXECUTE = 380,
     EXISTS = 381,
     EXP = 382,
     EXPONENT = 383,
     EXTERNAL = 384,
     EXTRACT = 385,
     SQL_FALSE = 386,
     FETCH = 387,
     FIRST = 388,
     SQL_FLOAT = 389,
     FLOOR = 390,
     FN = 391,
     FOR = 392,
     FOREIGN = 393,
     FORTRAN = 394,
     FOUND = 395,
     FOUR_DIGITS = 396,
     FROM = 397,
     FULL = 398,
     GET = 399,
     GLOBAL = 400,
     GO = 401,
     GOTO = 402,
     GRANT = 403,
     GREATER_THAN = 404,
     GREATER_OR_EQUAL = 405,
     GROUP = 406,
     HAVING = 407,
     HOUR = 408,
     HOURS_BETWEEN = 409,
     IDENTITY = 410,
     IFNULL = 411,
     SQL_IGNORE = 412,
     IMMEDIATE = 413,
     SQL_IN = 414,
     INCLUDE = 415,
     INDEX = 416,
     INDICATOR = 417,
     INITIALLY = 418,
     INNER = 419,
     INPUT = 420,
     INSENSITIVE = 421,
     INSERT = 422,
     INTEGER = 423,
     INTERSECT = 424,
     INTERVAL = 425,
     INTO = 426,
     IS = 427,
     ISOLATION = 428,
     JOIN = 429,
     JUSTIFY = 430,
     KEY = 431,
     LANGUAGE = 432,
     LAST = 433,
     LCASE = 434,
     LEFT = 435,
     LEFTPAREN = 436,
     LENGTH = 437,
     LESS_OR_EQUAL = 438,
     LESS_THAN = 439,
     LEVEL = 440,
     LIKE = 441,
     LINE_WIDTH = 442,
     LOCAL = 443,
     LOCATE = 444,
     LOG = 445,
     SQL_LONG = 446,
     LOWER = 447,
     LTRIM = 448,
     LTRIP = 449,
     MATCH = 450,
     SQL_MAX = 451,
     MICROSOFT = 452,
     SQL_MIN = 453,
     MINUS = 454,
     MINUTE = 455,
     MINUTES_BETWEEN = 456,
     MOD = 457,
     MODIFY = 458,
     MODULE = 459,
     MONTH = 460,
     MONTHS_BETWEEN = 461,
     MUMPS = 462,
     NAMES = 463,
     NATIONAL = 464,
     NCHAR = 465,
     NEXT = 466,
     NODUP = 467,
     NONE = 468,
     NOT = 469,
     NOT_EQUAL = 470,
     NOW = 471,
     SQL_NULL = 472,
     NULLIF = 473,
     NUMERIC = 474,
     OCTET_LENGTH = 475,
     ODBC = 476,
     OF = 477,
     SQL_OFF = 478,
     SQL_ON = 479,
     ONLY = 480,
     OPEN = 481,
     OPTION = 482,
     OR = 483,
     ORDER = 484,
     OUTER = 485,
     OUTPUT = 486,
     OVERLAPS = 487,
     PAGE = 488,
     PARTIAL = 489,
     SQL_PASCAL = 490,
     PERSISTENT = 491,
     CQL_PI = 492,
     PLI = 493,
     POSITION = 494,
     PRECISION = 495,
     PREPARE = 496,
     PRESERVE = 497,
     PRIMARY = 498,
     PRIOR = 499,
     PRIVILEGES = 500,
     PROCEDURE = 501,
     PRODUCT = 502,
     PUBLIC = 503,
     QUARTER = 504,
     QUESTION_MARK = 505,
     QUIT = 506,
     RAND = 507,
     READ_ONLY = 508,
     REAL = 509,
     REFERENCES = 510,
     REPEAT = 511,
     REPLACE = 512,
     RESTRICT = 513,
     REVOKE = 514,
     RIGHT = 515,
     RIGHTPAREN = 516,
     ROLLBACK = 517,
     ROWS = 518,
     RPAD = 519,
     RTRIM = 520,
     SCHEMA = 521,
     SCREEN_WIDTH = 522,
     SCROLL = 523,
     SECOND = 524,
     SECONDS_BETWEEN = 525,
     SELECT = 526,
     SEMICOLON = 527,
     SEQUENCE = 528,
     SETOPT = 529,
     SET = 530,
     SHOWOPT = 531,
     SIGN = 532,
     SIGNED_INTEGER = 533,
     SIN = 534,
     SINGLE_QUOTE = 535,
     SQL_SIZE = 536,
     SLASH = 537,
     SMALLINT = 538,
     SOME = 539,
     SPACE = 540,
     SQL = 541,
     SQL_TRUE = 542,
     SQLCA = 543,
     SQLCODE = 544,
     SQLERROR = 545,
     SQLSTATE = 546,
     SQLWARNING = 547,
     SQRT = 548,
     STDEV = 549,
     SUBSTRING = 550,
     SUM = 551,
     SYSDATE = 552,
     SYSDATE_FORMAT = 553,
     SYSTEM = 554,
     TABLE = 555,
     TAN = 556,
     TEMPORARY = 557,
     THEN = 558,
     THREE_DIGITS = 559,
     TIME = 560,
     TIMESTAMP = 561,
     TIMEZONE_HOUR = 562,
     TIMEZONE_MINUTE = 563,
     TINYINT = 564,
     TITLE = 565,
     TO = 566,
     TO_CHAR = 567,
     TO_DATE = 568,
     TRANSACTION = 569,
     TRANSLATE = 570,
     TRANSLATION = 571,
     TRUNCATE = 572,
     GENERAL_TITLE = 573,
     TWO_DIGITS = 574,
     UCASE = 575,
     UNION = 576,
     UNIQUE = 577,
     SQL_UNKNOWN = 578,
     UNSIGNED_INTEGER = 579,
     UPDATE = 580,
     UPPER = 581,
     USAGE = 582,
     USER = 583,
     USER_DEFINED_NAME = 584,
     USING = 585,
     VALUE = 586,
     VALUES = 587,
     VARBINARY = 588,
     VARCHAR = 589,
     VARYING = 590,
     VENDOR = 591,
     VIEW = 592,
     WEEK = 593,
     WHEN = 594,
     WHENEVER = 595,
     WHERE = 596,
     WHERE_CURRENT_OF = 597,
     WITH = 598,
     WORD_WRAPPED = 599,
     WORK = 600,
     WRAPPED = 601,
     YEAR = 602,
     YEARS_BETWEEN = 603
   };
#endif
#define SQL_TYPE 258
#define SQL_ABS 259
#define ACOS 260
#define AMPERSAND 261
#define SQL_ABSOLUTE 262
#define ADA 263
#define ADD 264
#define ADD_DAYS 265
#define ADD_HOURS 266
#define ADD_MINUTES 267
#define ADD_MONTHS 268
#define ADD_SECONDS 269
#define ADD_YEARS 270
#define ALL 271
#define ALLOCATE 272
#define ALTER 273
#define AND 274
#define ANY 275
#define ARE 276
#define ARITHMETIC_MINUS 277
#define ARITHMETIC_PLUS 278
#define AS 279
#define ASIN 280
#define ASC 281
#define ASCII 282
#define ASSERTION 283
#define ASTERISK 284
#define AT 285
#define ATAN 286
#define ATAN2 287
#define AUTHORIZATION 288
#define AUTO_INCREMENT 289
#define AVG 290
#define BEFORE 291
#define SQL_BEGIN 292
#define BETWEEN 293
#define BIGINT 294
#define BINARY 295
#define BIT 296
#define BIT_LENGTH 297
#define BREAK 298
#define BY 299
#define CASCADE 300
#define CASCADED 301
#define CASE 302
#define CAST 303
#define CATALOG 304
#define CEILING 305
#define CENTER 306
#define SQL_CHAR 307
#define CHAR_LENGTH 308
#define CHARACTER_STRING_LITERAL 309
#define CHECK 310
#define CLOSE 311
#define COALESCE 312
#define COBOL 313
#define COLLATE 314
#define COLLATION 315
#define COLON 316
#define COLUMN 317
#define COMMA 318
#define COMMIT 319
#define COMPUTE 320
#define CONCAT 321
#define CONNECT 322
#define CONNECTION 323
#define CONSTRAINT 324
#define CONSTRAINTS 325
#define CONTINUE 326
#define CONVERT 327
#define CORRESPONDING 328
#define COS 329
#define COT 330
#define COUNT 331
#define CREATE 332
#define CURDATE 333
#define CURRENT 334
#define CURRENT_DATE 335
#define CURRENT_TIME 336
#define CURRENT_TIMESTAMP 337
#define CURTIME 338
#define CURSOR 339
#define DATABASE 340
#define SQL_DATE 341
#define DATE_FORMAT 342
#define DATE_REMAINDER 343
#define DATE_VALUE 344
#define DAY 345
#define DAYOFMONTH 346
#define DAYOFWEEK 347
#define DAYOFYEAR 348
#define DAYS_BETWEEN 349
#define DEALLOCATE 350
#define DEC 351
#define DECLARE 352
#define DEFAULT 353
#define DEFERRABLE 354
#define DEFERRED 355
#define SQL_DELETE 356
#define DESC 357
#define DESCRIBE 358
#define DESCRIPTOR 359
#define DIAGNOSTICS 360
#define DICTIONARY 361
#define DIRECTORY 362
#define DISCONNECT 363
#define DISPLACEMENT 364
#define DISTINCT 365
#define DOLLAR_SIGN 366
#define DOMAIN_TOKEN 367
#define DOT 368
#define SQL_DOUBLE 369
#define DOUBLE_QUOTED_STRING 370
#define DROP 371
#define ELSE 372
#define END 373
#define END_EXEC 374
#define EQUAL 375
#define ESCAPE 376
#define EXCEPT 377
#define SQL_EXCEPTION 378
#define EXEC 379
#define EXECUTE 380
#define EXISTS 381
#define EXP 382
#define EXPONENT 383
#define EXTERNAL 384
#define EXTRACT 385
#define SQL_FALSE 386
#define FETCH 387
#define FIRST 388
#define SQL_FLOAT 389
#define FLOOR 390
#define FN 391
#define FOR 392
#define FOREIGN 393
#define FORTRAN 394
#define FOUND 395
#define FOUR_DIGITS 396
#define FROM 397
#define FULL 398
#define GET 399
#define GLOBAL 400
#define GO 401
#define GOTO 402
#define GRANT 403
#define GREATER_THAN 404
#define GREATER_OR_EQUAL 405
#define GROUP 406
#define HAVING 407
#define HOUR 408
#define HOURS_BETWEEN 409
#define IDENTITY 410
#define IFNULL 411
#define SQL_IGNORE 412
#define IMMEDIATE 413
#define SQL_IN 414
#define INCLUDE 415
#define INDEX 416
#define INDICATOR 417
#define INITIALLY 418
#define INNER 419
#define INPUT 420
#define INSENSITIVE 421
#define INSERT 422
#define INTEGER 423
#define INTERSECT 424
#define INTERVAL 425
#define INTO 426
#define IS 427
#define ISOLATION 428
#define JOIN 429
#define JUSTIFY 430
#define KEY 431
#define LANGUAGE 432
#define LAST 433
#define LCASE 434
#define LEFT 435
#define LEFTPAREN 436
#define LENGTH 437
#define LESS_OR_EQUAL 438
#define LESS_THAN 439
#define LEVEL 440
#define LIKE 441
#define LINE_WIDTH 442
#define LOCAL 443
#define LOCATE 444
#define LOG 445
#define SQL_LONG 446
#define LOWER 447
#define LTRIM 448
#define LTRIP 449
#define MATCH 450
#define SQL_MAX 451
#define MICROSOFT 452
#define SQL_MIN 453
#define MINUS 454
#define MINUTE 455
#define MINUTES_BETWEEN 456
#define MOD 457
#define MODIFY 458
#define MODULE 459
#define MONTH 460
#define MONTHS_BETWEEN 461
#define MUMPS 462
#define NAMES 463
#define NATIONAL 464
#define NCHAR 465
#define NEXT 466
#define NODUP 467
#define NONE 468
#define NOT 469
#define NOT_EQUAL 470
#define NOW 471
#define SQL_NULL 472
#define NULLIF 473
#define NUMERIC 474
#define OCTET_LENGTH 475
#define ODBC 476
#define OF 477
#define SQL_OFF 478
#define SQL_ON 479
#define ONLY 480
#define OPEN 481
#define OPTION 482
#define OR 483
#define ORDER 484
#define OUTER 485
#define OUTPUT 486
#define OVERLAPS 487
#define PAGE 488
#define PARTIAL 489
#define SQL_PASCAL 490
#define PERSISTENT 491
#define CQL_PI 492
#define PLI 493
#define POSITION 494
#define PRECISION 495
#define PREPARE 496
#define PRESERVE 497
#define PRIMARY 498
#define PRIOR 499
#define PRIVILEGES 500
#define PROCEDURE 501
#define PRODUCT 502
#define PUBLIC 503
#define QUARTER 504
#define QUESTION_MARK 505
#define QUIT 506
#define RAND 507
#define READ_ONLY 508
#define REAL 509
#define REFERENCES 510
#define REPEAT 511
#define REPLACE 512
#define RESTRICT 513
#define REVOKE 514
#define RIGHT 515
#define RIGHTPAREN 516
#define ROLLBACK 517
#define ROWS 518
#define RPAD 519
#define RTRIM 520
#define SCHEMA 521
#define SCREEN_WIDTH 522
#define SCROLL 523
#define SECOND 524
#define SECONDS_BETWEEN 525
#define SELECT 526
#define SEMICOLON 527
#define SEQUENCE 528
#define SETOPT 529
#define SET 530
#define SHOWOPT 531
#define SIGN 532
#define SIGNED_INTEGER 533
#define SIN 534
#define SINGLE_QUOTE 535
#define SQL_SIZE 536
#define SLASH 537
#define SMALLINT 538
#define SOME 539
#define SPACE 540
#define SQL 541
#define SQL_TRUE 542
#define SQLCA 543
#define SQLCODE 544
#define SQLERROR 545
#define SQLSTATE 546
#define SQLWARNING 547
#define SQRT 548
#define STDEV 549
#define SUBSTRING 550
#define SUM 551
#define SYSDATE 552
#define SYSDATE_FORMAT 553
#define SYSTEM 554
#define TABLE 555
#define TAN 556
#define TEMPORARY 557
#define THEN 558
#define THREE_DIGITS 559
#define TIME 560
#define TIMESTAMP 561
#define TIMEZONE_HOUR 562
#define TIMEZONE_MINUTE 563
#define TINYINT 564
#define TITLE 565
#define TO 566
#define TO_CHAR 567
#define TO_DATE 568
#define TRANSACTION 569
#define TRANSLATE 570
#define TRANSLATION 571
#define TRUNCATE 572
#define GENERAL_TITLE 573
#define TWO_DIGITS 574
#define UCASE 575
#define UNION 576
#define UNIQUE 577
#define SQL_UNKNOWN 578
#define UNSIGNED_INTEGER 579
#define UPDATE 580
#define UPPER 581
#define USAGE 582
#define USER 583
#define USER_DEFINED_NAME 584
#define USING 585
#define VALUE 586
#define VALUES 587
#define VARBINARY 588
#define VARCHAR 589
#define VARYING 590
#define VENDOR 591
#define VIEW 592
#define WEEK 593
#define WHEN 594
#define WHENEVER 595
#define WHERE 596
#define WHERE_CURRENT_OF 597
#define WITH 598
#define WORD_WRAPPED 599
#define WORK 600
#define WRAPPED 601
#define YEAR 602
#define YEARS_BETWEEN 603




/* Copy the first part of user declarations.  */
#line 357 "sqlparser.y"

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#include <qobject.h>
#include <kdebug.h>

#include <connection.h>
#include <queryschema.h>
#include <field.h>
#include <tableschema.h>

#include "parser.h"
#include "sqlparser.h"
#include "sqltypes.h"

//	using namespace std;

	#define YYSTACK_USE_ALLOCA 1
	#define YYMAXDEPTH 255

	KexiDB::Parser *parser;
	KexiDB::Field *field;
	bool requiresTable;

	int yyparse();
	int yylex();
	void tookenize(const char *data);

	void yyerror(const char *str)
	{
		kdDebug() << "error: " << str << endl;
		parser->setOperation(KexiDB::Parser::OP_Error);
	}

	void parseData(KexiDB::Parser *p, const char *data)
	{
		parser = p;
		field = 0;
		requiresTable = false;
		tookenize(data);
		yyparse();
		if(requiresTable)
		{
			yyerror("No tables used");
		}
	}

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
#line 425 "sqlparser.y"
typedef union YYSTYPE {
	char stringValue[255];
	int integerValue;
	KexiDB::Field::Type coltype;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 846 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 858 "y.tab.c"

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
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   331

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  349
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  13
/* YYNRULES -- Number of rules. */
#define YYNRULES  32
/* YYNRULES -- Number of states. */
#define YYNSTATES  51

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
     345,   346,   347,   348
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     6,     9,    10,    11,    19,    23,    25,
      28,    32,    35,    37,    40,    43,    45,    47,    52,    57,
      58,    61,    65,    67,    71,    73,    75,    77,    81,    85,
      87,    89,    91
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     350,     0,    -1,   350,   351,    -1,   350,   358,    -1,    -1,
      -1,    77,   300,   329,   352,   181,   353,   261,    -1,   353,
      63,   354,    -1,   354,    -1,   329,   357,    -1,   329,   357,
     355,    -1,   355,   356,    -1,   356,    -1,   243,   176,    -1,
     214,   217,    -1,    34,    -1,     3,    -1,     3,   181,   324,
     261,    -1,   334,   181,   324,   261,    -1,    -1,   359,   360,
      -1,   358,   142,   329,    -1,   271,    -1,   360,    63,   361,
      -1,   361,    -1,    29,    -1,   329,    -1,   329,   113,   329,
      -1,   329,   113,    29,    -1,    54,    -1,   278,    -1,   324,
      -1,   361,    24,   329,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   434,   434,   435,   436,   441,   440,   449,   449,   455,
     464,   479,   479,   485,   490,   495,   503,   508,   515,   522,
     530,   548,   556,   565,   565,   571,   580,   588,   597,   606,
     613,   620,   627
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SQL_TYPE", "SQL_ABS", "ACOS", "AMPERSAND", 
  "SQL_ABSOLUTE", "ADA", "ADD", "ADD_DAYS", "ADD_HOURS", "ADD_MINUTES", 
  "ADD_MONTHS", "ADD_SECONDS", "ADD_YEARS", "ALL", "ALLOCATE", "ALTER", 
  "AND", "ANY", "ARE", "ARITHMETIC_MINUS", "ARITHMETIC_PLUS", "AS", 
  "ASIN", "ASC", "ASCII", "ASSERTION", "ASTERISK", "AT", "ATAN", "ATAN2", 
  "AUTHORIZATION", "AUTO_INCREMENT", "AVG", "BEFORE", "SQL_BEGIN", 
  "BETWEEN", "BIGINT", "BINARY", "BIT", "BIT_LENGTH", "BREAK", "BY", 
  "CASCADE", "CASCADED", "CASE", "CAST", "CATALOG", "CEILING", "CENTER", 
  "SQL_CHAR", "CHAR_LENGTH", "CHARACTER_STRING_LITERAL", "CHECK", "CLOSE", 
  "COALESCE", "COBOL", "COLLATE", "COLLATION", "COLON", "COLUMN", "COMMA", 
  "COMMIT", "COMPUTE", "CONCAT", "CONNECT", "CONNECTION", "CONSTRAINT", 
  "CONSTRAINTS", "CONTINUE", "CONVERT", "CORRESPONDING", "COS", "COT", 
  "COUNT", "CREATE", "CURDATE", "CURRENT", "CURRENT_DATE", "CURRENT_TIME", 
  "CURRENT_TIMESTAMP", "CURTIME", "CURSOR", "DATABASE", "SQL_DATE", 
  "DATE_FORMAT", "DATE_REMAINDER", "DATE_VALUE", "DAY", "DAYOFMONTH", 
  "DAYOFWEEK", "DAYOFYEAR", "DAYS_BETWEEN", "DEALLOCATE", "DEC", 
  "DECLARE", "DEFAULT", "DEFERRABLE", "DEFERRED", "SQL_DELETE", "DESC", 
  "DESCRIBE", "DESCRIPTOR", "DIAGNOSTICS", "DICTIONARY", "DIRECTORY", 
  "DISCONNECT", "DISPLACEMENT", "DISTINCT", "DOLLAR_SIGN", "DOMAIN_TOKEN", 
  "DOT", "SQL_DOUBLE", "DOUBLE_QUOTED_STRING", "DROP", "ELSE", "END", 
  "END_EXEC", "EQUAL", "ESCAPE", "EXCEPT", "SQL_EXCEPTION", "EXEC", 
  "EXECUTE", "EXISTS", "EXP", "EXPONENT", "EXTERNAL", "EXTRACT", 
  "SQL_FALSE", "FETCH", "FIRST", "SQL_FLOAT", "FLOOR", "FN", "FOR", 
  "FOREIGN", "FORTRAN", "FOUND", "FOUR_DIGITS", "FROM", "FULL", "GET", 
  "GLOBAL", "GO", "GOTO", "GRANT", "GREATER_THAN", "GREATER_OR_EQUAL", 
  "GROUP", "HAVING", "HOUR", "HOURS_BETWEEN", "IDENTITY", "IFNULL", 
  "SQL_IGNORE", "IMMEDIATE", "SQL_IN", "INCLUDE", "INDEX", "INDICATOR", 
  "INITIALLY", "INNER", "INPUT", "INSENSITIVE", "INSERT", "INTEGER", 
  "INTERSECT", "INTERVAL", "INTO", "IS", "ISOLATION", "JOIN", "JUSTIFY", 
  "KEY", "LANGUAGE", "LAST", "LCASE", "LEFT", "LEFTPAREN", "LENGTH", 
  "LESS_OR_EQUAL", "LESS_THAN", "LEVEL", "LIKE", "LINE_WIDTH", "LOCAL", 
  "LOCATE", "LOG", "SQL_LONG", "LOWER", "LTRIM", "LTRIP", "MATCH", 
  "SQL_MAX", "MICROSOFT", "SQL_MIN", "MINUS", "MINUTE", "MINUTES_BETWEEN", 
  "MOD", "MODIFY", "MODULE", "MONTH", "MONTHS_BETWEEN", "MUMPS", "NAMES", 
  "NATIONAL", "NCHAR", "NEXT", "NODUP", "NONE", "NOT", "NOT_EQUAL", "NOW", 
  "SQL_NULL", "NULLIF", "NUMERIC", "OCTET_LENGTH", "ODBC", "OF", 
  "SQL_OFF", "SQL_ON", "ONLY", "OPEN", "OPTION", "OR", "ORDER", "OUTER", 
  "OUTPUT", "OVERLAPS", "PAGE", "PARTIAL", "SQL_PASCAL", "PERSISTENT", 
  "CQL_PI", "PLI", "POSITION", "PRECISION", "PREPARE", "PRESERVE", 
  "PRIMARY", "PRIOR", "PRIVILEGES", "PROCEDURE", "PRODUCT", "PUBLIC", 
  "QUARTER", "QUESTION_MARK", "QUIT", "RAND", "READ_ONLY", "REAL", 
  "REFERENCES", "REPEAT", "REPLACE", "RESTRICT", "REVOKE", "RIGHT", 
  "RIGHTPAREN", "ROLLBACK", "ROWS", "RPAD", "RTRIM", "SCHEMA", 
  "SCREEN_WIDTH", "SCROLL", "SECOND", "SECONDS_BETWEEN", "SELECT", 
  "SEMICOLON", "SEQUENCE", "SETOPT", "SET", "SHOWOPT", "SIGN", 
  "SIGNED_INTEGER", "SIN", "SINGLE_QUOTE", "SQL_SIZE", "SLASH", 
  "SMALLINT", "SOME", "SPACE", "SQL", "SQL_TRUE", "SQLCA", "SQLCODE", 
  "SQLERROR", "SQLSTATE", "SQLWARNING", "SQRT", "STDEV", "SUBSTRING", 
  "SUM", "SYSDATE", "SYSDATE_FORMAT", "SYSTEM", "TABLE", "TAN", 
  "TEMPORARY", "THEN", "THREE_DIGITS", "TIME", "TIMESTAMP", 
  "TIMEZONE_HOUR", "TIMEZONE_MINUTE", "TINYINT", "TITLE", "TO", "TO_CHAR", 
  "TO_DATE", "TRANSACTION", "TRANSLATE", "TRANSLATION", "TRUNCATE", 
  "GENERAL_TITLE", "TWO_DIGITS", "UCASE", "UNION", "UNIQUE", 
  "SQL_UNKNOWN", "UNSIGNED_INTEGER", "UPDATE", "UPPER", "USAGE", "USER", 
  "USER_DEFINED_NAME", "USING", "VALUE", "VALUES", "VARBINARY", "VARCHAR", 
  "VARYING", "VENDOR", "VIEW", "WEEK", "WHEN", "WHENEVER", "WHERE", 
  "WHERE_CURRENT_OF", "WITH", "WORD_WRAPPED", "WORK", "WRAPPED", "YEAR", 
  "YEARS_BETWEEN", "$accept", "Statement", "CreateTableStatement", "@1", 
  "ColDefs", "ColDef", "ColKeys", "ColKey", "ColType", "SelectStatement", 
  "Select", "ColViews", "ColView", 0
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
     595,   596,   597,   598,   599,   600,   601,   602,   603
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   349,   350,   350,   350,   352,   351,   353,   353,   354,
     354,   355,   355,   356,   356,   356,   357,   357,   357,   357,
     358,   358,   359,   360,   360,   361,   361,   361,   361,   361,
     361,   361,   361
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     2,     0,     0,     7,     3,     1,     2,
       3,     2,     1,     2,     2,     1,     1,     4,     4,     0,
       2,     3,     1,     3,     1,     1,     1,     3,     3,     1,
       1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       4,     0,     1,     0,    22,     2,     3,     0,     0,     0,
      25,    29,    30,    31,    26,    20,    24,     5,    21,     0,
       0,     0,     0,    28,    27,    23,    32,     0,    19,     0,
       8,    16,     0,     9,     0,     6,     0,     0,    15,     0,
       0,    10,    12,     7,     0,     0,    14,    13,    11,    17,
      18
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     1,     5,    22,    29,    30,    41,    42,    33,     6,
       7,    15,    16
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -322
static const short yypact[] =
{
    -322,     3,  -322,  -294,  -322,  -322,  -135,   -28,  -321,  -320,
    -322,  -322,  -322,  -322,  -103,   -52,   -12,  -322,  -322,   -27,
     -28,  -316,  -167,  -322,  -322,   -12,  -322,  -314,    -3,   -58,
    -322,  -165,  -164,   -30,  -314,  -322,  -306,  -305,  -322,  -197,
    -155,   -30,  -322,  -322,  -239,  -238,  -322,  -322,  -322,  -322,
    -322
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -322,  -322,  -322,  -322,  -322,   -10,  -322,   -16,  -322,  -322,
    -322,  -322,     7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      31,    10,    23,     2,    38,    34,     8,     9,    17,    18,
      19,    20,    21,    26,    27,    28,    36,    37,    44,    45,
      46,    47,    49,    50,    43,    48,    11,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       3,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    39,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    35,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     4,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,     0,
       0,    14,    24,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    32
};

static const short yycheck[] =
{
       3,    29,    29,     0,    34,    63,   300,   142,   329,   329,
     113,    63,    24,   329,   181,   329,   181,   181,   324,   324,
     217,   176,   261,   261,    34,    41,    54,    20,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      77,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   214,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   261,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   243,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     278,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   271,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   324,    -1,    -1,    -1,
      -1,   329,   329,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   334
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,   350,     0,    77,   271,   351,   358,   359,   300,   142,
      29,    54,   278,   324,   329,   360,   361,   329,   329,   113,
      63,    24,   352,    29,   329,   361,   329,   181,   329,   353,
     354,     3,   334,   357,    63,   261,   181,   181,    34,   214,
     243,   355,   356,   354,   324,   324,   217,   176,   356,   261,
     261
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
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
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
#line 434 "sqlparser.y"
    { YYACCEPT; }
    break;

  case 3:
#line 435 "sqlparser.y"
    { YYACCEPT; }
    break;

  case 5:
#line 441 "sqlparser.y"
    {
	parser->setOperation(KexiDB::Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
}
    break;

  case 8:
#line 450 "sqlparser.y"
    {
}
    break;

  case 9:
#line 456 "sqlparser.y"
    {
	kdDebug() << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

//	delete field;
	field = 0;
}
    break;

  case 10:
#line 465 "sqlparser.y"
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

  case 12:
#line 480 "sqlparser.y"
    {
}
    break;

  case 13:
#line 486 "sqlparser.y"
    {
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
}
    break;

  case 14:
#line 491 "sqlparser.y"
    {
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
}
    break;

  case 15:
#line 496 "sqlparser.y"
    {
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
}
    break;

  case 16:
#line 504 "sqlparser.y"
    {
	field = new KexiDB::Field();
	field->setType(yyvsp[0].coltype);
}
    break;

  case 17:
#line 509 "sqlparser.y"
    {
	kdDebug() << "sql + length" << endl;
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].coltype);
}
    break;

  case 18:
#line 516 "sqlparser.y"
    {
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(KexiDB::Field::Text);
}
    break;

  case 19:
#line 522 "sqlparser.y"
    {
	// SQLITE compatibillity
	field = new KexiDB::Field();
	field->setType(KexiDB::Field::InvalidType);
}
    break;

  case 20:
#line 531 "sqlparser.y"
    {
/*
	parser->select()->setBaseTable($4);
	if(parser->select()->unresolvedWildcard() && parser->db())
	{
		//resolve a (pure) wildcard

		TableStruct s = parser->db()->structure($4);

		for(Field *it = s.first(); it; it = s.next())
		{
			parser->select()->addCol(*it);
		}
		parser->select()->setUnresolvedWildcard(false);
	}
*/
}
    break;

  case 21:
#line 549 "sqlparser.y"
    {
	kdDebug() << "FROM: '" << yyvsp[0].stringValue << "'" << endl;
	requiresTable = false;
}
    break;

  case 22:
#line 557 "sqlparser.y"
    {
	kdDebug() << "SELECT" << endl;
	parser->createSelect();
	parser->setOperation(KexiDB::Parser::OP_Select);
}
    break;

  case 24:
#line 566 "sqlparser.y"
    {
}
    break;

  case 25:
#line 572 "sqlparser.y"
    {
	kdDebug() << "all columns" << endl;
	field = new KexiDB::Field();
	field->setName("*");
	parser->select()->addField(field);
//	parser->select()->setUnresolvedWildcard(true);
	requiresTable = true;
}
    break;

  case 26:
#line 581 "sqlparser.y"
    {
	kdDebug() << "  + col " << yyvsp[0].stringValue << endl;
	field = new KexiDB::Field();
	field->setName(yyvsp[0].stringValue);
	parser->select()->addField(field);
	requiresTable = true;
}
    break;

  case 27:
#line 589 "sqlparser.y"
    {
	kdDebug() << "  + col " << yyvsp[0].stringValue << " from " << yyvsp[-2].stringValue << endl;
	field = new KexiDB::Field();
//	s->setTable($1);
	field->setName(yyvsp[0].stringValue);
	parser->select()->addField(field);
	requiresTable = true;
}
    break;

  case 28:
#line 598 "sqlparser.y"
    {
	kdDebug() << "  + all columns from " << yyvsp[-2].stringValue << endl;
	field = new KexiDB::Field();
	field->setName("*");
	parser->select()->addField(field);
//	parser->select()->setUnresolvedWildcard(true);
	requiresTable = true;
}
    break;

  case 29:
#line 607 "sqlparser.y"
    {
	field = new KexiDB::Field();
	field->setName(yyvsp[0].stringValue);
	parser->select()->addField(field);
	kdDebug() << "  + constant " << yyvsp[0].stringValue << endl;
}
    break;

  case 30:
#line 614 "sqlparser.y"
    {
	field = new KexiDB::Field();
	field->setName(QString::number(yyvsp[0].integerValue));
	parser->select()->addField(field);
	kdDebug() << "  + numerical constant " << yyvsp[0].integerValue << endl;
}
    break;

  case 31:
#line 621 "sqlparser.y"
    {
	field = new KexiDB::Field();
	field->setName(QString::number(yyvsp[0].integerValue));
	parser->select()->addField(field);
	kdDebug() << "  + numerical constant " << yyvsp[0].integerValue << endl;
}
    break;

  case 32:
#line 628 "sqlparser.y"
    {
	kdDebug() << "  => alias: " << yyvsp[0].stringValue << endl;
	if(field->name() == "*")
	{
		kdDebug() << "can't use aliases ond wildcards!" << endl;
		yyerror("syntax error");
		YYERROR;
	}
}
    break;


    }

/* Line 999 of yacc.c.  */
#line 2201 "y.tab.c"

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
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
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


#line 639 "sqlparser.y"



