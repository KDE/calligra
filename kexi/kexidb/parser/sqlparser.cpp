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
     PERCENT = 258,
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
     ARITHMETIC_MINUS = 278,
     ARITHMETIC_PLUS = 279,
     AS = 280,
     ASIN = 281,
     ASC = 282,
     ASCII = 283,
     ASSERTION = 284,
     ASTERISK = 285,
     AT = 286,
     ATAN = 287,
     ATAN2 = 288,
     AUTHORIZATION = 289,
     AUTO_INCREMENT = 290,
     AVG = 291,
     BEFORE = 292,
     SQL_BEGIN = 293,
     BETWEEN = 294,
     BIGINT = 295,
     BINARY = 296,
     BIT = 297,
     BIT_LENGTH = 298,
     BREAK = 299,
     BY = 300,
     CASCADE = 301,
     CASCADED = 302,
     CASE = 303,
     CAST = 304,
     CATALOG = 305,
     CEILING = 306,
     CENTER = 307,
     SQL_CHAR = 308,
     CHAR_LENGTH = 309,
     CHARACTER_STRING_LITERAL = 310,
     CHECK = 311,
     CLOSE = 312,
     COALESCE = 313,
     COBOL = 314,
     COLLATE = 315,
     COLLATION = 316,
     COLON = 317,
     COLUMN = 318,
     COMMA = 319,
     COMMIT = 320,
     COMPUTE = 321,
     CONCAT = 322,
     CONNECT = 323,
     CONNECTION = 324,
     CONSTRAINT = 325,
     CONSTRAINTS = 326,
     CONTINUE = 327,
     CONVERT = 328,
     CORRESPONDING = 329,
     COS = 330,
     COT = 331,
     COUNT = 332,
     CREATE = 333,
     CURDATE = 334,
     CURRENT = 335,
     CURRENT_DATE = 336,
     CURRENT_TIME = 337,
     CURRENT_TIMESTAMP = 338,
     CURTIME = 339,
     CURSOR = 340,
     DATABASE = 341,
     SQL_DATE = 342,
     DATE_FORMAT = 343,
     DATE_REMAINDER = 344,
     DATE_VALUE = 345,
     DAY = 346,
     DAYOFMONTH = 347,
     DAYOFWEEK = 348,
     DAYOFYEAR = 349,
     DAYS_BETWEEN = 350,
     DEALLOCATE = 351,
     DEC = 352,
     DECLARE = 353,
     DEFAULT = 354,
     DEFERRABLE = 355,
     DEFERRED = 356,
     SQL_DELETE = 357,
     DESC = 358,
     DESCRIBE = 359,
     DESCRIPTOR = 360,
     DIAGNOSTICS = 361,
     DICTIONARY = 362,
     DIRECTORY = 363,
     DISCONNECT = 364,
     DISPLACEMENT = 365,
     DISTINCT = 366,
     DOLLAR_SIGN = 367,
     DOMAIN_TOKEN = 368,
     DOT = 369,
     SQL_DOUBLE = 370,
     DOUBLE_QUOTED_STRING = 371,
     DROP = 372,
     ELSE = 373,
     END = 374,
     END_EXEC = 375,
     EQUAL = 376,
     ESCAPE = 377,
     EXCEPT = 378,
     SQL_EXCEPTION = 379,
     EXEC = 380,
     EXECUTE = 381,
     EXISTS = 382,
     EXP = 383,
     EXPONENT = 384,
     EXTERNAL = 385,
     EXTRACT = 386,
     SQL_FALSE = 387,
     FETCH = 388,
     FIRST = 389,
     SQL_FLOAT = 390,
     FLOOR = 391,
     FN = 392,
     FOR = 393,
     FOREIGN = 394,
     FORTRAN = 395,
     FOUND = 396,
     FOUR_DIGITS = 397,
     FROM = 398,
     FULL = 399,
     GET = 400,
     GLOBAL = 401,
     GO = 402,
     GOTO = 403,
     GRANT = 404,
     GREATER_THAN = 405,
     GREATER_OR_EQUAL = 406,
     GROUP = 407,
     HAVING = 408,
     HOUR = 409,
     HOURS_BETWEEN = 410,
     IDENTITY = 411,
     IFNULL = 412,
     SQL_IGNORE = 413,
     IMMEDIATE = 414,
     SQL_IN = 415,
     INCLUDE = 416,
     INDEX = 417,
     INDICATOR = 418,
     INITIALLY = 419,
     INNER = 420,
     INPUT = 421,
     INSENSITIVE = 422,
     INSERT = 423,
     INTEGER = 424,
     INTERSECT = 425,
     INTERVAL = 426,
     INTO = 427,
     IS = 428,
     ISOLATION = 429,
     JOIN = 430,
     JUSTIFY = 431,
     KEY = 432,
     LANGUAGE = 433,
     LAST = 434,
     LCASE = 435,
     LEFT = 436,
     LEFTPAREN = 437,
     LENGTH = 438,
     LESS_OR_EQUAL = 439,
     LESS_THAN = 440,
     LEVEL = 441,
     LIKE = 442,
     LINE_WIDTH = 443,
     LOCAL = 444,
     LOCATE = 445,
     LOG = 446,
     SQL_LONG = 447,
     LOWER = 448,
     LTRIM = 449,
     LTRIP = 450,
     MATCH = 451,
     SQL_MAX = 452,
     MICROSOFT = 453,
     SQL_MIN = 454,
     MINUS = 455,
     MINUTE = 456,
     MINUTES_BETWEEN = 457,
     MOD = 458,
     MODIFY = 459,
     MODULE = 460,
     MONTH = 461,
     MONTHS_BETWEEN = 462,
     MUMPS = 463,
     NAMES = 464,
     NATIONAL = 465,
     NCHAR = 466,
     NEXT = 467,
     NODUP = 468,
     NONE = 469,
     NOT = 470,
     NOT_EQUAL = 471,
     NOW = 472,
     SQL_NULL = 473,
     NULLIF = 474,
     NUMERIC = 475,
     OCTET_LENGTH = 476,
     ODBC = 477,
     OF = 478,
     SQL_OFF = 479,
     SQL_ON = 480,
     ONLY = 481,
     OPEN = 482,
     OPTION = 483,
     OR = 484,
     ORDER = 485,
     OUTER = 486,
     OUTPUT = 487,
     OVERLAPS = 488,
     PAGE = 489,
     PARTIAL = 490,
     SQL_PASCAL = 491,
     PERSISTENT = 492,
     CQL_PI = 493,
     PLI = 494,
     POSITION = 495,
     PRECISION = 496,
     PREPARE = 497,
     PRESERVE = 498,
     PRIMARY = 499,
     PRIOR = 500,
     PRIVILEGES = 501,
     PROCEDURE = 502,
     PRODUCT = 503,
     PUBLIC = 504,
     QUARTER = 505,
     QUESTION_MARK = 506,
     QUIT = 507,
     RAND = 508,
     READ_ONLY = 509,
     REAL = 510,
     REFERENCES = 511,
     REPEAT = 512,
     REPLACE = 513,
     RESTRICT = 514,
     REVOKE = 515,
     RIGHT = 516,
     RIGHTPAREN = 517,
     ROLLBACK = 518,
     ROWS = 519,
     RPAD = 520,
     RTRIM = 521,
     SCHEMA = 522,
     SCREEN_WIDTH = 523,
     SCROLL = 524,
     SECOND = 525,
     SECONDS_BETWEEN = 526,
     SELECT = 527,
     SEMICOLON = 528,
     SEQUENCE = 529,
     SETOPT = 530,
     SET = 531,
     SHOWOPT = 532,
     SIGN = 533,
     SIGNED_INTEGER = 534,
     SIN = 535,
     SINGLE_QUOTE = 536,
     SQL_SIZE = 537,
     SLASH = 538,
     SMALLINT = 539,
     SOME = 540,
     SPACE = 541,
     SQL = 542,
     SQL_TRUE = 543,
     SQLCA = 544,
     SQLCODE = 545,
     SQLERROR = 546,
     SQLSTATE = 547,
     SQLWARNING = 548,
     SQRT = 549,
     STDEV = 550,
     SUBSTRING = 551,
     SUM = 552,
     SYSDATE = 553,
     SYSDATE_FORMAT = 554,
     SYSTEM = 555,
     TABLE = 556,
     TAN = 557,
     TEMPORARY = 558,
     THEN = 559,
     THREE_DIGITS = 560,
     TIME = 561,
     TIMESTAMP = 562,
     TIMEZONE_HOUR = 563,
     TIMEZONE_MINUTE = 564,
     TINYINT = 565,
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
#define PERCENT 258
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
#define ARITHMETIC_MINUS 278
#define ARITHMETIC_PLUS 279
#define AS 280
#define ASIN 281
#define ASC 282
#define ASCII 283
#define ASSERTION 284
#define ASTERISK 285
#define AT 286
#define ATAN 287
#define ATAN2 288
#define AUTHORIZATION 289
#define AUTO_INCREMENT 290
#define AVG 291
#define BEFORE 292
#define SQL_BEGIN 293
#define BETWEEN 294
#define BIGINT 295
#define BINARY 296
#define BIT 297
#define BIT_LENGTH 298
#define BREAK 299
#define BY 300
#define CASCADE 301
#define CASCADED 302
#define CASE 303
#define CAST 304
#define CATALOG 305
#define CEILING 306
#define CENTER 307
#define SQL_CHAR 308
#define CHAR_LENGTH 309
#define CHARACTER_STRING_LITERAL 310
#define CHECK 311
#define CLOSE 312
#define COALESCE 313
#define COBOL 314
#define COLLATE 315
#define COLLATION 316
#define COLON 317
#define COLUMN 318
#define COMMA 319
#define COMMIT 320
#define COMPUTE 321
#define CONCAT 322
#define CONNECT 323
#define CONNECTION 324
#define CONSTRAINT 325
#define CONSTRAINTS 326
#define CONTINUE 327
#define CONVERT 328
#define CORRESPONDING 329
#define COS 330
#define COT 331
#define COUNT 332
#define CREATE 333
#define CURDATE 334
#define CURRENT 335
#define CURRENT_DATE 336
#define CURRENT_TIME 337
#define CURRENT_TIMESTAMP 338
#define CURTIME 339
#define CURSOR 340
#define DATABASE 341
#define SQL_DATE 342
#define DATE_FORMAT 343
#define DATE_REMAINDER 344
#define DATE_VALUE 345
#define DAY 346
#define DAYOFMONTH 347
#define DAYOFWEEK 348
#define DAYOFYEAR 349
#define DAYS_BETWEEN 350
#define DEALLOCATE 351
#define DEC 352
#define DECLARE 353
#define DEFAULT 354
#define DEFERRABLE 355
#define DEFERRED 356
#define SQL_DELETE 357
#define DESC 358
#define DESCRIBE 359
#define DESCRIPTOR 360
#define DIAGNOSTICS 361
#define DICTIONARY 362
#define DIRECTORY 363
#define DISCONNECT 364
#define DISPLACEMENT 365
#define DISTINCT 366
#define DOLLAR_SIGN 367
#define DOMAIN_TOKEN 368
#define DOT 369
#define SQL_DOUBLE 370
#define DOUBLE_QUOTED_STRING 371
#define DROP 372
#define ELSE 373
#define END 374
#define END_EXEC 375
#define EQUAL 376
#define ESCAPE 377
#define EXCEPT 378
#define SQL_EXCEPTION 379
#define EXEC 380
#define EXECUTE 381
#define EXISTS 382
#define EXP 383
#define EXPONENT 384
#define EXTERNAL 385
#define EXTRACT 386
#define SQL_FALSE 387
#define FETCH 388
#define FIRST 389
#define SQL_FLOAT 390
#define FLOOR 391
#define FN 392
#define FOR 393
#define FOREIGN 394
#define FORTRAN 395
#define FOUND 396
#define FOUR_DIGITS 397
#define FROM 398
#define FULL 399
#define GET 400
#define GLOBAL 401
#define GO 402
#define GOTO 403
#define GRANT 404
#define GREATER_THAN 405
#define GREATER_OR_EQUAL 406
#define GROUP 407
#define HAVING 408
#define HOUR 409
#define HOURS_BETWEEN 410
#define IDENTITY 411
#define IFNULL 412
#define SQL_IGNORE 413
#define IMMEDIATE 414
#define SQL_IN 415
#define INCLUDE 416
#define INDEX 417
#define INDICATOR 418
#define INITIALLY 419
#define INNER 420
#define INPUT 421
#define INSENSITIVE 422
#define INSERT 423
#define INTEGER 424
#define INTERSECT 425
#define INTERVAL 426
#define INTO 427
#define IS 428
#define ISOLATION 429
#define JOIN 430
#define JUSTIFY 431
#define KEY 432
#define LANGUAGE 433
#define LAST 434
#define LCASE 435
#define LEFT 436
#define LEFTPAREN 437
#define LENGTH 438
#define LESS_OR_EQUAL 439
#define LESS_THAN 440
#define LEVEL 441
#define LIKE 442
#define LINE_WIDTH 443
#define LOCAL 444
#define LOCATE 445
#define LOG 446
#define SQL_LONG 447
#define LOWER 448
#define LTRIM 449
#define LTRIP 450
#define MATCH 451
#define SQL_MAX 452
#define MICROSOFT 453
#define SQL_MIN 454
#define MINUS 455
#define MINUTE 456
#define MINUTES_BETWEEN 457
#define MOD 458
#define MODIFY 459
#define MODULE 460
#define MONTH 461
#define MONTHS_BETWEEN 462
#define MUMPS 463
#define NAMES 464
#define NATIONAL 465
#define NCHAR 466
#define NEXT 467
#define NODUP 468
#define NONE 469
#define NOT 470
#define NOT_EQUAL 471
#define NOW 472
#define SQL_NULL 473
#define NULLIF 474
#define NUMERIC 475
#define OCTET_LENGTH 476
#define ODBC 477
#define OF 478
#define SQL_OFF 479
#define SQL_ON 480
#define ONLY 481
#define OPEN 482
#define OPTION 483
#define OR 484
#define ORDER 485
#define OUTER 486
#define OUTPUT 487
#define OVERLAPS 488
#define PAGE 489
#define PARTIAL 490
#define SQL_PASCAL 491
#define PERSISTENT 492
#define CQL_PI 493
#define PLI 494
#define POSITION 495
#define PRECISION 496
#define PREPARE 497
#define PRESERVE 498
#define PRIMARY 499
#define PRIOR 500
#define PRIVILEGES 501
#define PROCEDURE 502
#define PRODUCT 503
#define PUBLIC 504
#define QUARTER 505
#define QUESTION_MARK 506
#define QUIT 507
#define RAND 508
#define READ_ONLY 509
#define REAL 510
#define REFERENCES 511
#define REPEAT 512
#define REPLACE 513
#define RESTRICT 514
#define REVOKE 515
#define RIGHT 516
#define RIGHTPAREN 517
#define ROLLBACK 518
#define ROWS 519
#define RPAD 520
#define RTRIM 521
#define SCHEMA 522
#define SCREEN_WIDTH 523
#define SCROLL 524
#define SECOND 525
#define SECONDS_BETWEEN 526
#define SELECT 527
#define SEMICOLON 528
#define SEQUENCE 529
#define SETOPT 530
#define SET 531
#define SHOWOPT 532
#define SIGN 533
#define SIGNED_INTEGER 534
#define SIN 535
#define SINGLE_QUOTE 536
#define SQL_SIZE 537
#define SLASH 538
#define SMALLINT 539
#define SOME 540
#define SPACE 541
#define SQL 542
#define SQL_TRUE 543
#define SQLCA 544
#define SQLCODE 545
#define SQLERROR 546
#define SQLSTATE 547
#define SQLWARNING 548
#define SQRT 549
#define STDEV 550
#define SUBSTRING 551
#define SUM 552
#define SYSDATE 553
#define SYSDATE_FORMAT 554
#define SYSTEM 555
#define TABLE 556
#define TAN 557
#define TEMPORARY 558
#define THEN 559
#define THREE_DIGITS 560
#define TIME 561
#define TIMESTAMP 562
#define TIMEZONE_HOUR 563
#define TIMEZONE_MINUTE 564
#define TINYINT 565
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
#line 364 "sqlparser.y"

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#ifdef _WIN32
# include <malloc.h>
#endif

#include <qobject.h>
#include <kdebug.h>
#include <klocale.h>
#include <qptrlist.h>

#include <connection.h>
#include <queryschema.h>
#include <field.h>
#include <tableschema.h>

#include "parser.h"
//#include "sqlparser.h"
#include "sqltypes.h"

//	using namespace std;

	#define YY_NO_UNPUT
	#define YYSTACK_USE_ALLOCA 1
	#define YYMAXDEPTH 255

	KexiDB::Parser *parser;
	KexiDB::Field *field;
	bool requiresTable;
	QPtrList<KexiDB::Field> fieldList;
	QPtrList<KexiDB::TableSchema> tableList;
	KexiDB::TableSchema *dummy;
	int current = 0;
	QString ctoken = "";

	int yyparse();
	int yylex();
	void tokenize(const char *data);

	void yyerror(const char *str)
	{
		kdDebug() << "error: " << str << endl;
		kdDebug() << "at character " << current << " near tooken " << ctoken << endl;
		parser->setOperation(KexiDB::Parser::OP_Error);

		if(strcmp(str, "syntax error") == 0)
		{
			kdDebug() << parser->statement() << endl;
			QString ptrline = "";
			for(int i=0; i < current; i++)
				ptrline += " ";

			ptrline += "^";

			kdDebug() << ptrline << endl;

			KexiDB::ParserError err(i18n("Syntax Error"), i18n("Syntax Error near '%1'").arg(ctoken), ctoken, current);
			parser->setError(err);
		}
	}

	void parseData(KexiDB::Parser *p, const char *data)
	{
		dummy = new KexiDB::TableSchema();
		parser = p;
		field = 0;
		fieldList.clear();
		requiresTable = false;
		tokenize(data);
		yyparse();

		if(parser->operation() == KexiDB::Parser::OP_Select)
		{
			kdDebug() << "parseData(): " << tableList.count() << " loaded tables" << endl;
			for(KexiDB::TableSchema *s = tableList.first(); s; s = tableList.next())
			{
				kdDebug() << "  " << s->name() << endl;
			}

			KexiDB::Field::ListIterator it = parser->select()->fieldsIterator();
			for(KexiDB::Field *item; (item = it.current()); ++it)
			{
				if(tableList.findRef(item->table()) == -1)
				{
					KexiDB::ParserError err(i18n("Field List Error"), i18n("Unknown table '%1' in field list").arg(item->table()->name()), ctoken, current);
					parser->setError(err);

					yyerror("fieldlisterror");
				}
			}
		}

		tableList.clear();
	}

	void addTable(const QString &table)
	{
		kdDebug() << "addTable() " << table << endl;
		KexiDB::TableSchema *s = parser->db()->tableSchema(table);
		if(!s)
		{
			KexiDB::ParserError err(i18n("Field List Error"), i18n("Table '%1' doesn't exist").arg(table), ctoken, current);
			parser->setError(err);
			yyerror("field list error");
		}
		else
		{
			tableList.append(s);
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
#line 497 "sqlparser.y"
typedef union YYSTYPE {
	char stringValue[255];
	int integerValue;
	KexiDB::Field::Type coltype;
	KexiDB::Field *field;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 912 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 924 "y.tab.c"

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
#define YYLAST   499

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  349
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  20
/* YYNRULES -- Number of rules. */
#define YYNRULES  70
/* YYNRULES -- Number of states. */
#define YYNSTATES  147

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
      58,    61,    64,    68,    70,    73,    77,    81,    85,    88,
      95,   103,   110,   117,   125,   129,   131,   133,   137,   139,
     141,   143,   147,   150,   152,   156,   158,   160,   162,   164,
     166,   170,   174,   178,   182,   186,   190,   194,   198,   202,
     206,   210,   214,   218,   222,   227,   232,   237,   242,   247,
     249
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     350,     0,    -1,   350,   351,    -1,   350,   358,    -1,    -1,
      -1,    78,   301,   329,   352,   182,   353,   262,    -1,   353,
      64,   354,    -1,   354,    -1,   329,   357,    -1,   329,   357,
     355,    -1,   355,   356,    -1,   356,    -1,   244,   177,    -1,
     215,   218,    -1,    35,    -1,     4,    -1,     4,   182,   324,
     262,    -1,   334,   182,   324,   262,    -1,    -1,   359,   364,
      -1,   358,   361,    -1,   358,   361,   360,    -1,   272,    -1,
     341,   367,    -1,   360,    20,   367,    -1,   360,   229,   367,
      -1,   182,   360,   262,    -1,   143,   362,    -1,   361,   181,
     175,   329,   225,   367,    -1,   361,   181,   231,   175,   329,
     225,   367,    -1,   361,   165,   175,   329,   225,   367,    -1,
     361,   261,   175,   329,   225,   367,    -1,   361,   261,   231,
     175,   329,   225,   367,    -1,   362,    64,   363,    -1,   363,
      -1,   329,    -1,   364,    64,   365,    -1,   365,    -1,   367,
      -1,   368,    -1,   367,    25,   329,    -1,   367,   329,    -1,
     329,    -1,   329,   114,   329,    -1,   218,    -1,    55,    -1,
     279,    -1,   324,    -1,   366,    -1,   367,    24,   367,    -1,
     367,    23,   367,    -1,   367,   283,   367,    -1,   367,    30,
     367,    -1,   367,   215,   367,    -1,   367,   121,   367,    -1,
     367,   216,   367,    -1,   367,   150,   367,    -1,   367,   151,
     367,    -1,   367,   185,   367,    -1,   367,   184,   367,    -1,
     367,   187,   367,    -1,   367,     3,   367,    -1,   182,   367,
     262,    -1,   297,   182,   367,   262,    -1,   199,   182,   367,
     262,    -1,   197,   182,   367,   262,    -1,    36,   182,   367,
     262,    -1,   111,   182,   367,   262,    -1,    30,    -1,   329,
     114,    30,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   507,   507,   508,   509,   514,   513,   522,   522,   528,
     537,   552,   552,   558,   563,   568,   576,   581,   588,   595,
     603,   621,   625,   632,   641,   645,   649,   653,   660,   663,
     668,   673,   678,   683,   691,   691,   697,   732,   732,   738,
     744,   747,   754,   764,   773,   783,   788,   795,   802,   812,
     817,   822,   827,   832,   837,   842,   847,   852,   857,   862,
     867,   872,   877,   882,   888,   895,   902,   909,   916,   924,
     934
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PERCENT", "SQL_TYPE", "SQL_ABS", "ACOS", 
  "AMPERSAND", "SQL_ABSOLUTE", "ADA", "ADD", "ADD_DAYS", "ADD_HOURS", 
  "ADD_MINUTES", "ADD_MONTHS", "ADD_SECONDS", "ADD_YEARS", "ALL", 
  "ALLOCATE", "ALTER", "AND", "ANY", "ARE", "ARITHMETIC_MINUS", 
  "ARITHMETIC_PLUS", "AS", "ASIN", "ASC", "ASCII", "ASSERTION", 
  "ASTERISK", "AT", "ATAN", "ATAN2", "AUTHORIZATION", "AUTO_INCREMENT", 
  "AVG", "BEFORE", "SQL_BEGIN", "BETWEEN", "BIGINT", "BINARY", "BIT", 
  "BIT_LENGTH", "BREAK", "BY", "CASCADE", "CASCADED", "CASE", "CAST", 
  "CATALOG", "CEILING", "CENTER", "SQL_CHAR", "CHAR_LENGTH", 
  "CHARACTER_STRING_LITERAL", "CHECK", "CLOSE", "COALESCE", "COBOL", 
  "COLLATE", "COLLATION", "COLON", "COLUMN", "COMMA", "COMMIT", "COMPUTE", 
  "CONCAT", "CONNECT", "CONNECTION", "CONSTRAINT", "CONSTRAINTS", 
  "CONTINUE", "CONVERT", "CORRESPONDING", "COS", "COT", "COUNT", "CREATE", 
  "CURDATE", "CURRENT", "CURRENT_DATE", "CURRENT_TIME", 
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
  "TIMEZONE_HOUR", "TIMEZONE_MINUTE", "TINYINT", "TO", "TO_CHAR", 
  "TO_DATE", "TRANSACTION", "TRANSLATE", "TRANSLATION", "TRUNCATE", 
  "GENERAL_TITLE", "TWO_DIGITS", "UCASE", "UNION", "UNIQUE", 
  "SQL_UNKNOWN", "UNSIGNED_INTEGER", "UPDATE", "UPPER", "USAGE", "USER", 
  "USER_DEFINED_NAME", "USING", "VALUE", "VALUES", "VARBINARY", "VARCHAR", 
  "VARYING", "VENDOR", "VIEW", "WEEK", "WHEN", "WHENEVER", "WHERE", 
  "WHERE_CURRENT_OF", "WITH", "WORD_WRAPPED", "WORK", "WRAPPED", "YEAR", 
  "YEARS_BETWEEN", "$accept", "Statement", "CreateTableStatement", "@1", 
  "ColDefs", "ColDef", "ColKeys", "ColKey", "ColType", "SelectStatement", 
  "Select", "Conditions", "Tables", "FlatTableList", "FlatTable", 
  "ColViews", "ColItem", "ColView", "ColExpression", "ColWildCard", 0
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
     358,   358,   358,   359,   360,   360,   360,   360,   361,   361,
     361,   361,   361,   361,   362,   362,   363,   364,   364,   365,
     365,   365,   365,   366,   366,   366,   366,   366,   366,   367,
     367,   367,   367,   367,   367,   367,   367,   367,   367,   367,
     367,   367,   367,   367,   367,   367,   367,   367,   367,   368,
     368
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     2,     0,     0,     7,     3,     1,     2,
       3,     2,     1,     2,     2,     1,     1,     4,     4,     0,
       2,     2,     3,     1,     2,     3,     3,     3,     2,     6,
       7,     6,     6,     7,     3,     1,     1,     3,     1,     1,
       1,     3,     2,     1,     3,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     4,     4,     4,     4,     4,     1,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       4,     0,     1,     0,    23,     2,     3,     0,     0,     0,
      21,    69,     0,    46,     0,     0,     0,     0,    45,    47,
       0,    48,    43,    20,    38,    49,    39,    40,     5,    36,
      28,    35,     0,     0,     0,     0,     0,    22,     0,     0,
      43,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    42,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,     0,     0,     0,     0,    63,     0,     0,     0,
      70,    44,    37,    62,    51,    50,    41,    53,    55,    57,
      58,    60,    59,    61,    54,    56,    52,     0,    34,     0,
       0,     0,    27,     0,     0,    25,    26,    67,    68,    66,
      65,    64,    19,     0,     8,     0,     0,     0,     0,     0,
      16,     0,     9,     0,     6,    31,    29,     0,    32,     0,
       0,     0,    15,     0,     0,    10,    12,     7,    30,    33,
       0,     0,    14,    13,    11,    17,    18
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     5,    62,   113,   114,   135,   136,   122,     6,
       7,    37,    10,    30,    31,    23,    24,    25,    26,    27
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -313
static const short yypact[] =
{
    -313,     4,  -313,  -292,  -313,  -313,  -125,   -28,  -312,  -310,
    -126,  -313,  -156,  -313,  -153,   -31,  -152,  -149,  -313,  -313,
    -148,  -313,   -77,   -21,  -313,  -313,    -2,  -313,  -313,  -313,
     -20,  -313,  -127,  -165,  -166,  -161,   -31,    -7,   -31,   -31,
     -65,     8,   -31,   -31,   -31,   -27,   -28,   -31,   -31,   -31,
    -279,   -31,   -31,   -31,   -31,   -31,   -31,   -31,   -31,   -31,
     -31,  -313,  -131,  -310,  -276,  -275,  -118,   -13,  -271,  -116,
     195,   -31,   -31,    12,    22,  -269,  -313,    70,   122,   178,
    -313,  -313,  -313,    17,   -24,   -24,  -313,  -313,    17,    17,
      17,    17,    17,    17,    17,    17,  -313,  -268,  -313,  -163,
    -162,  -265,  -313,  -160,  -262,   195,   195,  -313,  -313,  -313,
    -313,  -313,    -4,   -52,  -313,   -31,   -31,  -157,   -31,  -154,
    -113,  -110,   -15,  -268,  -313,   195,   195,   -31,   195,   -31,
    -250,  -249,  -313,  -142,  -100,   -15,  -313,  -313,   195,   195,
    -184,  -183,  -313,  -313,  -313,  -313,  -313
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -313,  -313,  -313,  -313,  -313,   -42,  -313,   -51,  -313,  -313,
    -313,    51,  -313,  -313,    23,  -313,    41,  -313,   370,  -313
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
     120,    47,    11,    80,     2,    12,    51,    71,    12,     8,
      65,    47,   123,    71,    68,    47,    34,    28,     9,    29,
     132,    48,    49,    50,    13,    47,    38,    13,    51,    39,
      42,    48,    49,    43,    44,    48,    49,    45,    51,    32,
      48,    49,    51,    46,    63,    48,    49,    51,    64,    75,
      86,    97,    51,    99,   100,    33,    34,   101,   103,   104,
      81,   112,   115,   116,   117,   118,    66,   119,   127,   130,
      69,   129,   131,    47,   140,   141,   142,   143,   145,   146,
      14,   137,     3,    14,   144,    67,    98,    82,     0,     0,
       0,     0,     0,    48,    49,     0,     0,     0,     0,     0,
      51,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    52,
       0,     0,     0,     0,     0,    47,     0,     0,     0,    52,
       0,     0,     0,    52,     0,    35,     0,     0,     0,     0,
       0,     0,     0,    52,     0,    48,    49,     0,    53,    54,
       0,    15,    51,     0,    15,     0,     0,     0,    53,    54,
       0,     0,    53,    54,     0,     0,    16,     0,    17,    16,
       0,    17,    53,    54,     0,    36,     0,     0,     0,     0,
       0,    47,    55,    56,     0,    57,     0,    18,     0,     0,
      18,    52,    55,    56,     0,    57,    55,    56,    47,    57,
     133,    48,    49,     0,     0,     0,    55,    56,    51,    57,
     124,     0,     0,    58,    59,    36,    72,     0,    48,    49,
      53,    54,    72,    58,    59,    51,     0,    58,    59,   134,
       0,     0,     0,     0,     0,     0,     0,    58,    59,     0,
       0,     0,     0,    52,     0,     0,     0,     0,    19,   102,
       0,    19,     0,     0,    55,    56,     0,    57,     0,    60,
       0,     0,     0,     0,     0,     0,    20,     0,     0,    20,
      76,     0,    53,    54,   107,     0,     4,     0,     0,     0,
       0,    60,     0,     0,   108,    58,    59,     0,     0,     0,
       0,    60,     0,    21,     0,    60,    21,     0,    40,    52,
      60,    22,    81,     0,     0,    60,    55,    56,     0,    57,
       0,     0,     0,     0,     0,     0,    52,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    61,    53,    54,
     121,     0,   109,     0,     0,     0,     0,    58,    59,     0,
       0,     0,     0,     0,     0,    53,    54,     0,     0,     0,
       0,     0,     0,    60,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    56,     0,    57,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    55,
      56,     0,    57,     0,   110,    41,     0,     0,     0,     0,
       0,     0,     0,    58,    59,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    60,    70,     0,    73,    74,
      58,    59,    77,    78,    79,     0,     0,    83,    84,    85,
       0,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     111,   105,   106,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    60,     0,
       0,     0,     0,     0,     0,   125,   126,     0,   128,     0,
       0,     0,     0,     0,     0,     0,     0,   138,     0,   139
};

static const short yycheck[] =
{
       4,     3,    30,    30,     0,    36,    30,    20,    36,   301,
     175,     3,    64,    20,   175,     3,   182,   329,   143,   329,
      35,    23,    24,    25,    55,     3,   182,    55,    30,   182,
     182,    23,    24,   182,   182,    23,    24,   114,    30,   165,
      23,    24,    30,    64,    64,    23,    24,    30,   175,   114,
     329,   182,    30,   329,   329,   181,   182,   175,   329,   175,
     329,   329,   225,   225,   329,   225,   231,   329,   225,   182,
     231,   225,   182,     3,   324,   324,   218,   177,   262,   262,
     111,   123,    78,   111,   135,    34,    63,    46,    -1,    -1,
      -1,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    -1,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   121,
      -1,    -1,    -1,    -1,    -1,     3,    -1,    -1,    -1,   121,
      -1,    -1,    -1,   121,    -1,   261,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   121,    -1,    23,    24,    -1,   150,   151,
      -1,   182,    30,    -1,   182,    -1,    -1,    -1,   150,   151,
      -1,    -1,   150,   151,    -1,    -1,   197,    -1,   199,   197,
      -1,   199,   150,   151,    -1,   341,    -1,    -1,    -1,    -1,
      -1,     3,   184,   185,    -1,   187,    -1,   218,    -1,    -1,
     218,   121,   184,   185,    -1,   187,   184,   185,     3,   187,
     215,    23,    24,    -1,    -1,    -1,   184,   185,    30,   187,
     262,    -1,    -1,   215,   216,   341,   229,    -1,    23,    24,
     150,   151,   229,   215,   216,    30,    -1,   215,   216,   244,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   215,   216,    -1,
      -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,   279,   262,
      -1,   279,    -1,    -1,   184,   185,    -1,   187,    -1,   283,
      -1,    -1,    -1,    -1,    -1,    -1,   297,    -1,    -1,   297,
     262,    -1,   150,   151,   262,    -1,   272,    -1,    -1,    -1,
      -1,   283,    -1,    -1,   262,   215,   216,    -1,    -1,    -1,
      -1,   283,    -1,   324,    -1,   283,   324,    -1,   329,   121,
     283,   329,   329,    -1,    -1,   283,   184,   185,    -1,   187,
      -1,    -1,    -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   329,   150,   151,
     334,    -1,   262,    -1,    -1,    -1,    -1,   215,   216,    -1,
      -1,    -1,    -1,    -1,    -1,   150,   151,    -1,    -1,    -1,
      -1,    -1,    -1,   283,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   184,   185,    -1,   187,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   184,
     185,    -1,   187,    -1,   262,    15,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   215,   216,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   283,    36,    -1,    38,    39,
     215,   216,    42,    43,    44,    -1,    -1,    47,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     262,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   283,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   283,    -1,
      -1,    -1,    -1,    -1,    -1,   115,   116,    -1,   118,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   127,    -1,   129
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,   350,     0,    78,   272,   351,   358,   359,   301,   143,
     361,    30,    36,    55,   111,   182,   197,   199,   218,   279,
     297,   324,   329,   364,   365,   366,   367,   368,   329,   329,
     362,   363,   165,   181,   182,   261,   341,   360,   182,   182,
     329,   367,   182,   182,   182,   114,    64,     3,    23,    24,
      25,    30,   121,   150,   151,   184,   185,   187,   215,   216,
     283,   329,   352,    64,   175,   175,   231,   360,   175,   231,
     367,    20,   229,   367,   367,   114,   262,   367,   367,   367,
      30,   329,   365,   367,   367,   367,   329,   367,   367,   367,
     367,   367,   367,   367,   367,   367,   367,   182,   363,   329,
     329,   175,   262,   329,   175,   367,   367,   262,   262,   262,
     262,   262,   329,   353,   354,   225,   225,   329,   225,   329,
       4,   334,   357,    64,   262,   367,   367,   225,   367,   225,
     182,   182,    35,   215,   244,   355,   356,   354,   367,   367,
     324,   324,   218,   177,   356,   262,   262
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
#line 507 "sqlparser.y"
    { YYACCEPT; }
    break;

  case 3:
#line 508 "sqlparser.y"
    {  }
    break;

  case 5:
#line 514 "sqlparser.y"
    {
	parser->setOperation(KexiDB::Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
}
    break;

  case 8:
#line 523 "sqlparser.y"
    {
}
    break;

  case 9:
#line 529 "sqlparser.y"
    {
	kdDebug() << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

//	delete field;
	field = 0;
}
    break;

  case 10:
#line 538 "sqlparser.y"
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
#line 553 "sqlparser.y"
    {
}
    break;

  case 13:
#line 559 "sqlparser.y"
    {
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
}
    break;

  case 14:
#line 564 "sqlparser.y"
    {
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
}
    break;

  case 15:
#line 569 "sqlparser.y"
    {
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
}
    break;

  case 16:
#line 577 "sqlparser.y"
    {
	field = new KexiDB::Field();
	field->setType(yyvsp[0].coltype);
}
    break;

  case 17:
#line 582 "sqlparser.y"
    {
	kdDebug() << "sql + length" << endl;
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].coltype);
}
    break;

  case 18:
#line 589 "sqlparser.y"
    {
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(KexiDB::Field::Text);
}
    break;

  case 19:
#line 595 "sqlparser.y"
    {
	// SQLITE compatibillity
	field = new KexiDB::Field();
	field->setType(KexiDB::Field::InvalidType);
}
    break;

  case 20:
#line 604 "sqlparser.y"
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
#line 622 "sqlparser.y"
    {
	kdDebug() << "from detail" << endl;
}
    break;

  case 22:
#line 626 "sqlparser.y"
    {
	kdDebug() << "from detail (conditioned)" << endl;
}
    break;

  case 23:
#line 633 "sqlparser.y"
    {
	kdDebug() << "SELECT" << endl;
	parser->createSelect();
	parser->setOperation(KexiDB::Parser::OP_Select);
}
    break;

  case 24:
#line 642 "sqlparser.y"
    {
	kdDebug() << "WHERE " << yyvsp[0].field << endl;
}
    break;

  case 25:
#line 646 "sqlparser.y"
    {
	kdDebug() << "AND " << yyvsp[0].field << endl;
}
    break;

  case 26:
#line 650 "sqlparser.y"
    {
	kdDebug() << "OR " << yyvsp[0].field << endl;
}
    break;

  case 27:
#line 654 "sqlparser.y"
    {
	kdDebug() << "()" << endl;
}
    break;

  case 28:
#line 661 "sqlparser.y"
    {
}
    break;

  case 29:
#line 664 "sqlparser.y"
    {
	kdDebug() << "LEFT JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
    break;

  case 30:
#line 669 "sqlparser.y"
    {
	kdDebug() << "LEFT OUTER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
    break;

  case 31:
#line 674 "sqlparser.y"
    {
	kdDebug() << "INNER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
    break;

  case 32:
#line 679 "sqlparser.y"
    {
	kdDebug() << "RIGHT JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
    break;

  case 33:
#line 684 "sqlparser.y"
    {
	kdDebug() << "RIGHT OUTER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
    break;

  case 35:
#line 692 "sqlparser.y"
    {
}
    break;

  case 36:
#line 698 "sqlparser.y"
    {
	kdDebug() << "FROM: '" << yyvsp[0].stringValue << "'" << endl;

	KexiDB::TableSchema *schema = parser->db()->tableSchema(yyvsp[0].stringValue);
	parser->select()->setParentTable(schema);
	parser->select()->addTable(schema);
	requiresTable = false;
	addTable(yyvsp[0].stringValue);

	KexiDB::Field::ListIterator it = parser->select()->fieldsIterator();
	for(KexiDB::Field *item; (item = it.current()); ++it)
	{
		if(item->table() == dummy)
		{
			item->setTable(schema);
		}

		if(!item->isQueryAsterisk())
		{
			KexiDB::Field *f = item->table()->field(item->name());
			if(!f)
			{
				KexiDB::ParserError err(i18n("FIeld List Error Error"), i18n("Unknown column '%1' in table '%2'").arg(item->name()).arg(schema->name()), ctoken, current);
				parser->setError(err);
				yyerror("field list error");
			}	
		}
	}
}
    break;

  case 38:
#line 733 "sqlparser.y"
    {
}
    break;

  case 39:
#line 739 "sqlparser.y"
    {
	kdDebug() << " adding field '" << yyvsp[0].field->name() << "'" << endl;
	parser->select()->addField(yyvsp[0].field);
//	parser->fieldList()->append($1);
}
    break;

  case 40:
#line 745 "sqlparser.y"
    {
}
    break;

  case 41:
#line 748 "sqlparser.y"
    {
	kdDebug() << " adding field '" << yyvsp[-2].field->name() << "' as '" << yyvsp[0].stringValue << "'" << endl;
//	parser->fieldList()->append($1);
	parser->select()->addField(yyvsp[-2].field);
	parser->select()->setAlias(yyvsp[-2].field, yyvsp[0].stringValue);
}
    break;

  case 42:
#line 755 "sqlparser.y"
    {
	kdDebug() << " adding field '" << yyvsp[-1].field->name() << "' as '" << yyvsp[0].stringValue << "'" << endl;
//	parser->fieldList()->append($1);
	parser->select()->addField(yyvsp[-1].field);
	parser->select()->setAlias(yyvsp[-1].field, yyvsp[0].stringValue);
}
    break;

  case 43:
#line 765 "sqlparser.y"
    {
	kdDebug() << "  + col " << yyvsp[0].stringValue << endl;
	yyval.field = new KexiDB::Field();
	yyval.field->setName(yyvsp[0].stringValue);
	yyval.field->setTable(dummy);
//	parser->select()->addField(field);
	requiresTable = true;
}
    break;

  case 44:
#line 774 "sqlparser.y"
    {
	kdDebug() << "  + col " << yyvsp[0].stringValue << " from " << yyvsp[-2].stringValue << endl;
	yyval.field = new KexiDB::Field();
//	s->setTable($1);
	yyval.field->setName(yyvsp[0].stringValue);
	yyval.field->setTable(parser->db()->tableSchema(yyvsp[-2].stringValue));
//	parser->select()->addField(field);
	requiresTable = true;
}
    break;

  case 45:
#line 784 "sqlparser.y"
    {
	yyval.field = new KexiDB::Field();
	yyval.field->setName(QString::null);
}
    break;

  case 46:
#line 789 "sqlparser.y"
    {
	yyval.field = new KexiDB::Field();
	yyval.field->setName(yyvsp[0].stringValue);
//	parser->select()->addField(field);
	kdDebug() << "  + constant " << yyvsp[0].stringValue << endl;
}
    break;

  case 47:
#line 796 "sqlparser.y"
    {
	yyval.field = new KexiDB::Field();
	yyval.field->setName(QString::number(yyvsp[0].integerValue));
//	parser->select()->addField(field);
	kdDebug() << "  + numerical constant " << yyvsp[0].integerValue << endl;
}
    break;

  case 48:
#line 803 "sqlparser.y"
    {
	yyval.field = new KexiDB::Field();
	yyval.field->setName(QString::number(yyvsp[0].integerValue));
//	parser->select()->addField(field);
	kdDebug() << "  + numerical constant " << yyvsp[0].integerValue << endl;
}
    break;

  case 49:
#line 813 "sqlparser.y"
    {
	yyval.field = yyvsp[0].field;
	kdDebug() << "to expression: " << yyval.field->name() << endl;
}
    break;

  case 50:
#line 818 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " + " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " + " + yyvsp[0].field->name());
}
    break;

  case 51:
#line 823 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " - " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " - " + yyvsp[0].field->name());
}
    break;

  case 52:
#line 828 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " / " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " / " + yyvsp[0].field->name());
}
    break;

  case 53:
#line 833 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " * " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " * " + yyvsp[0].field->name());
}
    break;

  case 54:
#line 838 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " NOT " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " NOT " + yyvsp[0].field->name());
}
    break;

  case 55:
#line 843 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " = " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " = " + yyvsp[0].field->name());
}
    break;

  case 56:
#line 848 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " <> " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " <> " + yyvsp[0].field->name());
}
    break;

  case 57:
#line 853 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " > " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " > " + yyvsp[0].field->name());
}
    break;

  case 58:
#line 858 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " >= " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " >= " + yyvsp[0].field->name());
}
    break;

  case 59:
#line 863 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " < " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " < " + yyvsp[0].field->name());
}
    break;

  case 60:
#line 868 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " <= " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " <= " + yyvsp[0].field->name());
}
    break;

  case 61:
#line 873 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " LIKE " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " LIKE " + yyvsp[0].field->name());
}
    break;

  case 62:
#line 878 "sqlparser.y"
    {
	kdDebug() << yyvsp[-2].field->name() << " % " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " % " + yyvsp[0].field->name());
}
    break;

  case 63:
#line 883 "sqlparser.y"
    {
	kdDebug() << "(" << yyvsp[-1].field->name() << ")" << endl;
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("(" + yyvsp[-1].field->name() + ")");
}
    break;

  case 64:
#line 889 "sqlparser.y"
    {
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("SUM(" + yyvsp[-1].field->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
    break;

  case 65:
#line 896 "sqlparser.y"
    {
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("MIN(" + yyvsp[-1].field->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
    break;

  case 66:
#line 903 "sqlparser.y"
    {
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("MAX(" + yyvsp[-1].field->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
    break;

  case 67:
#line 910 "sqlparser.y"
    {
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("AVG(" + yyvsp[-1].field->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
    break;

  case 68:
#line 917 "sqlparser.y"
    {
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("DISTINCT(" + yyvsp[-1].field->name() + ")");
}
    break;

  case 69:
#line 925 "sqlparser.y"
    {
	kdDebug() << "all columns" << endl;
//	field = new KexiDB::Field();
//	field->setName("*");
	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), dummy);
	parser->select()->addAsterisk(ast);
//	fieldList.append(ast);
	requiresTable = true;
}
    break;

  case 70:
#line 935 "sqlparser.y"
    {
	kdDebug() << "  + all columns from " << yyvsp[-2].stringValue << endl;
	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), parser->db()->tableSchema(yyvsp[-2].stringValue));
	parser->select()->addAsterisk(ast);
//	fieldList.append(ast);
	requiresTable = true;
}
    break;


    }

/* Line 999 of yacc.c.  */
#line 2680 "y.tab.c"

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


#line 944 "sqlparser.y"



