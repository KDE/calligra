#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
#line 365 "sqlparser.y"
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
/*#include "sqlparser.h"*/
#include "sqltypes.h"

/*	using namespace std;*/

	#define YY_NO_UNPUT
	#define YYSTACK_USE_ALLOCA 1
	#define YYMAXDEPTH 255

	KexiDB::Parser *parser;
	KexiDB::Field *field;
	bool requiresTable;
	QPtrList<KexiDB::Field> fieldList;
	QPtrList<KexiDB::TableSchema> tableList;
	KexiDB::TableSchema *dummy = 0;
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

		if(parser->error().type().isEmpty() && strcmp(str, "syntax error") == 0)
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
		if (!dummy)
			dummy = new KexiDB::TableSchema();
		parser = p;
		field = 0;
		fieldList.clear();
		requiresTable = false;

		if (!data) {
			KexiDB::ParserError err(i18n("Error"), i18n("No query specified"), ctoken, current);
			parser->setError(err);
			yyerror("");
			return;
		}
	
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
			/*take the dummy table out of the query*/
			parser->select()->removeTable(dummy);
		}

		tableList.clear();
	}

	void addTable(const QString &table)
	{
		kdDebug() << "addTable() " << table << endl;
		KexiDB::TableSchema *s = parser->db()->tableSchema(table);
		if(!s)
		{
			KexiDB::ParserError err(i18n("Field List Error"), i18n("Table '%1' does not exist").arg(table), ctoken, current);
			parser->setError(err);
			yyerror("fieldlisterror");
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
#line 508 "sqlparser.y"
typedef union {
	char stringValue[255];
	int integerValue;
	KexiDB::Field::Type coltype;
	KexiDB::Field *field;
} YYSTYPE;
#line 161 "y.tab.c"
#define PERCENT 257
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
#define TO 565
#define TO_CHAR 566
#define TO_DATE 567
#define TRANSACTION 568
#define TRANSLATE 569
#define TRANSLATION 570
#define TRUNCATE 571
#define GENERAL_TITLE 572
#define TWO_DIGITS 573
#define UCASE 574
#define UNION 575
#define UNIQUE 576
#define SQL_UNKNOWN 577
#define UNSIGNED_INTEGER 578
#define UPDATE 579
#define UPPER 580
#define USAGE 581
#define USER 582
#define USER_DEFINED_NAME 583
#define USING 584
#define VALUE 585
#define VALUES 586
#define VARBINARY 587
#define VARCHAR 588
#define VARYING 589
#define VENDOR 590
#define VIEW 591
#define WEEK 592
#define WHEN 593
#define WHENEVER 594
#define WHERE 595
#define WHERE_CURRENT_OF 596
#define WITH 597
#define WORD_WRAPPED 598
#define WORK 599
#define WRAPPED 600
#define YEAR 601
#define YEARS_BETWEEN 602
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    3,    3,    3,    6,    4,    7,    7,    8,
    8,   10,   10,   11,   11,   11,    9,    9,    9,    9,
    5,    5,    5,    5,    5,   12,   15,   15,   15,   15,
   14,   14,   14,   14,   14,   14,   16,   16,   17,   13,
   13,   18,   18,   18,   18,    2,    2,    2,    2,    2,
    2,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,   19,   19,
};
short yylen[] = {                                         2,
    2,    1,    2,    2,    0,    0,    7,    3,    1,    2,
    3,    2,    1,    2,    2,    1,    1,    4,    4,    0,
    2,    3,    2,    3,    4,    1,    2,    3,    3,    3,
    2,    6,    7,    6,    6,    7,    3,    1,    1,    3,
    1,    1,    1,    3,    2,    1,    3,    1,    1,    1,
    1,    1,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    4,    4,    4,    4,
    4,    1,    3,
};
short yydefred[] = {                                      5,
    0,    0,    0,   26,    1,    3,    4,    0,    0,   72,
    0,   49,    0,    0,    0,    0,    0,   48,   50,    0,
   51,    0,    0,   52,    0,    0,   41,   43,    6,    0,
    0,   39,    0,   38,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   45,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   66,    0,
    0,    0,   73,   47,    0,    0,    0,   44,   56,    0,
    0,    0,    0,    0,    0,    0,    0,   55,   40,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   70,   71,   37,   69,   68,   67,   30,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    9,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    7,    0,    0,    0,
    0,    0,    0,    0,   16,    0,    0,    0,   13,    8,
    0,    0,    0,    0,   15,   14,   12,   18,   19,
};
short yydgoto[] = {                                       1,
   23,   24,    2,    6,    7,   64,  116,  117,  125,  138,
  139,    8,   25,   26,   60,   33,   34,   27,   28,
};
short yysindex[] = {                                      0,
    0, -304, -517,    0,    0,    0,    0, -277, -544,    0,
 -388,    0, -387, -529, -247, -381, -379,    0,    0, -378,
    0, -309, -254,    0, -313, -390,    0,    0,    0, -247,
 -247,    0, -258,    0, -307,  700, -247, -247, -247, -284,
 -247, -247, -247, -520, -247, -247, -247, -247, -247, -247,
 -247, -247, -247, -247,    0, -243, -430, -247, -383, -262,
 -365, -396, -395, -371,  845,  883, -529, -516,    0,  887,
  895,  899,    0,    0, -269, -280, -280,    0,    0, -269,
 -269, -269, -269, -269, -269, -269, -269,    0,    0, -263,
  467, -262, -247, -247, -515, -514, -359, -512, -357, -510,
    0,    0,    0,    0,    0,    0,    0,  467,  467, -405,
 -404, -507, -401, -504, -256, -302,    0, -247, -247, -399,
 -247, -398, -354, -353, -271, -510,    0,  467,  467, -247,
  467, -247, -493, -492,    0, -385, -340, -271,    0,    0,
  467,  467, -424, -422,    0,    0,    0,    0,    0,
};
short yyrindex[] = {                                      0,
    0,   95,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  783,   37,    0,   14,   22,    0,    0,    0,    0,
    0,    0,   40,    0,    1,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   27,   51,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  190,   10,   19,    0,    0,  211,
  295,  377,  400,  482,  505,  610,  712,    0,    0,    0,
   31,   93,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   35,   56,    0,
    0,    0,    0,    0, -272,    0,    0,    0,    0,    0,
    0,    0, -268,    0, -298,    0,    0,   77,   96,    0,
  140,    0,    0,    0,    0,    0,    0, -292,    0,    0,
  162,  171,    0,    0,    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
 1392,    0,    0,    0,    0,    0,    0,  -29,    0,    0,
  -40,    0,    0,   74,  -15,    0,   33,   45,    0,
};
#define YYTABLESIZE 1524
short yytable[] = {                                      73,
   46,  123,   41,   45,   56,   57,   10,   42,   43,   54,
   93,   93,   11,   21,   45,  126,   20,  135,   53,   10,
   17,   23,   42,   43,   44,   11,   22,    3,   61,   45,
   27,   12,   96,   98,   28,   61,   42,    9,   29,   31,
   10,   90,   11,   92,   62,   20,   11,   30,   31,   17,
   24,   62,   57,   32,   37,   29,   38,   39,   40,   67,
   68,   12,   78,   95,  100,   12,   74,  110,  111,  112,
  113,  114,  115,  118,  119,  120,   34,  121,  122,  130,
  132,  133,  134,   14,  143,  144,  145,   13,   97,   99,
  146,  148,   25,  149,    2,   32,  140,  147,   59,  103,
   89,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   13,    0,   14,
   46,   13,   57,    0,   63,    0,    0,    0,    0,    0,
    0,   63,    0,    0,    0,    0,    0,    0,    0,   35,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   47,
   48,    0,    0,    0,    0,    0,    0,    0,   15,    0,
    0,   33,    0,    0,   58,    0,    0,    0,    0,    0,
   36,    0,    0,   16,    0,   17,    0,    0,    0,    0,
    0,    0,    0,   49,   50,    0,   51,    0,   15,   65,
    0,    0,   15,    0,   18,    0,   20,  136,    0,    0,
   17,    0,    0,   16,    0,   17,    0,   16,    0,   17,
   58,   58,    0,  127,   52,   53,    0,   10,    0,   94,
   94,    4,    5,   11,   18,   20,  137,    0,   18,   17,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   20,    0,    0,    0,   17,    0,    0,
    0,    0,  107,    0,    0,   19,   54,   46,    0,    0,
    0,    0,    0,    0,    0,    0,   54,   54,    0,    0,
    0,    0,    0,   20,   46,   53,    0,   46,   46,   46,
    0,   58,   54,   54,   46,   19,   54,   54,   54,   19,
    0,    0,   53,    0,   60,   53,   53,   53,   74,    0,
   21,    0,    0,   20,   27,   22,    0,   20,   28,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   46,    0,
    0,    0,    0,    0,    0,    0,    0,   54,   55,   29,
   21,  124,   46,    0,   21,   35,   53,    0,    0,   22,
    0,   54,    0,    0,    0,   21,    0,    0,    0,    0,
   53,    0,    0,   23,   42,    0,    0,    0,   22,    0,
    0,    0,   27,    0,    0,    0,   28,    0,   42,    0,
    0,   31,    0,    0,    0,   46,   61,    0,    0,    0,
    0,    0,   24,    0,   54,    0,    0,   29,    0,    0,
    0,    0,    0,   53,    0,    0,    0,   46,    0,   63,
    0,    0,    0,    0,   46,   46,   54,    0,   34,    0,
    0,    0,    0,   54,   54,   53,    0,    0,    0,   46,
    0,    0,   53,   53,   25,    0,    0,   32,   54,    0,
    0,    0,    0,   42,    0,   46,   46,   53,   46,   46,
    0,   46,    0,    0,   54,   54,   65,   54,   54,    0,
   54,    0,    0,   53,   53,    0,   53,   53,   31,   53,
    0,    0,    0,   65,    0,    0,    0,   58,   65,   46,
   46,   35,   42,    0,   31,   31,    0,    0,   54,   54,
    0,   62,    0,   46,   58,    0,    0,   53,   53,   58,
    0,    0,   54,   33,    0,   34,    0,    0,    0,    0,
    0,   53,   36,    0,   64,    0,    0,   65,    0,    0,
    0,   34,   34,   27,   32,   46,   46,   28,    0,    0,
    0,   65,    0,    0,   54,   54,   46,   46,   58,    0,
   32,   32,    0,   53,   53,   54,   54,   46,   29,   21,
   21,    0,   58,    0,   53,   53,   27,   23,   23,    0,
   28,   60,   22,   22,   31,    0,   27,   27,   35,    0,
   28,   28,   42,   42,   65,   31,   31,    0,   60,    0,
    0,   29,    0,   60,   35,   35,   24,   24,    0,    0,
   33,   29,   29,   46,    0,   58,   65,    0,    0,   36,
    0,   34,   54,   65,   65,   46,   33,   33,    0,    0,
    0,   53,   34,   34,   54,   36,   36,   58,   65,   57,
   32,    0,   60,   53,   58,   58,    0,    0,   25,   25,
    0,   32,   32,    0,   65,   65,   60,   65,   65,   58,
   65,   42,    0,   61,   31,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   58,   58,    0,   58,   58,
   61,   58,    0,    0,   35,   61,   63,    0,   65,   65,
    0,    0,    0,    0,    0,   35,   35,    0,    0,   60,
    0,   34,   65,   63,    0,    0,   33,    0,   63,   58,
   58,    0,    0,    0,    0,   36,    0,   33,   33,    0,
   32,   60,    0,   58,   61,    0,   36,   36,   60,   60,
    0,    0,    0,    0,   65,   65,    0,    0,   61,    0,
    0,   59,    0,   60,    0,   65,   65,   63,    0,    0,
    0,    0,    0,   41,    0,   58,   58,    0,    0,   60,
   60,   63,   60,   60,   35,   60,   58,   58,   62,    0,
    0,    0,    0,   42,   43,    0,    0,    0,    0,    0,
   45,   61,    0,    0,    0,   62,   33,    0,    0,    0,
   62,   64,    0,   60,   60,   36,    0,    0,    0,    0,
    0,    0,   65,   61,   63,    0,    0,   60,   64,    0,
   61,   61,   46,   64,   65,    0,    0,    0,    0,    0,
    0,    0,    0,   58,    0,   61,   63,    0,    0,   62,
    0,    0,    0,   63,   63,   58,    0,    0,    0,   60,
   60,   61,   61,   62,   61,   61,    0,   61,   63,    0,
   60,   60,   64,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   63,   63,   64,   63,   63,    0,
   63,   46,    0,    0,    0,   61,   61,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   62,    0,    0,   61,
    0,    0,    0,    0,    0,    0,   57,    0,   63,   63,
   47,   48,    0,    0,    0,    0,    0,   60,   62,   64,
    0,    0,   63,   57,    0,   62,   62,    0,   57,   60,
    0,   61,   61,    0,    0,    0,    0,    0,    0,    0,
   62,   64,   61,   61,   49,   50,    0,   51,   64,   64,
    0,    0,    0,    0,   63,   63,   62,   62,    0,   62,
   62,    0,   62,   64,    0,   63,   63,   57,    0,    0,
    0,    0,    0,    0,    0,   52,   53,    0,    0,   64,
   64,   57,   64,   64,    0,   64,    0,    0,    0,    0,
   62,   62,    0,    0,    0,    0,   41,    0,    0,   61,
    0,    0,    0,    0,   62,    0,    0,    0,   59,    0,
    0,   61,    0,   64,   64,    0,   42,   43,    0,    0,
    0,    0,   63,   45,   57,   59,    0,   64,    0,    0,
   59,    0,    0,    0,   63,    0,   62,   62,    0,    0,
    0,    0,    0,   54,    0,    0,   57,   62,   62,    0,
    0,    0,    0,   57,   57,    0,    0,    0,    0,   64,
   64,    0,    0,    0,    0,    0,    0,    0,   57,   59,
   64,   64,    0,    0,    0,    0,    0,    0,    0,   46,
    0,    0,    0,   59,   57,   57,    0,   57,   57,    0,
   57,    0,    0,    0,    0,    0,    0,    0,    0,   46,
   46,   46,    0,    0,   62,    0,   46,    0,    0,    0,
    0,    0,    0,    0,   46,    0,   62,    0,   57,   57,
    0,    0,    0,    0,    0,    0,   59,   64,    0,    0,
    0,    0,   57,    0,    0,    0,    0,    0,    0,   64,
   46,   41,    0,   47,   48,    0,    0,    0,   59,    0,
    0,    0,    0,    0,   46,   59,   59,    0,    0,    0,
    0,   42,   43,    0,   57,   57,    0,    0,   45,    0,
   59,    0,    0,    0,    0,   57,   57,   49,   50,   41,
   51,    0,    0,   41,    0,    0,   59,   59,    0,   59,
   59,   41,   59,    0,    0,   41,    0,   46,    0,   42,
   43,    0,    0,   42,   43,    0,   45,    0,   52,   53,
   45,   42,   43,    0,    0,   42,   43,    0,   45,   46,
   59,   59,   45,    0,    0,    0,   46,   46,    0,    0,
    0,    0,   57,    0,   59,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   57,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   69,    0,    0,   46,   46,
   46,   46,    0,   46,    0,    0,   59,   59,    0,    0,
    0,    0,    0,    0,    0,    0,   54,   59,   59,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   47,   48,
    0,   46,   46,    0,    0,    0,    0,   46,    0,    0,
    0,   46,    0,    0,    0,    0,    0,    0,    0,   46,
    0,    0,    0,   46,    0,    0,    0,    0,    0,    0,
    0,    0,   49,   50,    0,   51,   47,   48,    0,    0,
   47,   48,    0,    0,   59,    0,    0,    0,   47,   48,
    0,    0,   47,   48,    0,    0,   59,    0,   46,   46,
    0,    0,    0,   52,   53,    0,    0,    0,    0,   46,
   49,   50,    0,   51,   49,   50,    0,   51,    0,    0,
    0,    0,   49,   50,    0,   51,   49,   50,    0,   51,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   52,   53,    0,    0,   52,   53,    0,    0,    0,
  101,    0,    0,   52,   53,   46,    0,   52,   53,    0,
    0,    0,    0,    0,    0,    0,    0,   46,    0,    0,
    0,   54,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  102,    0,
    0,    0,  104,    0,    0,    0,   36,    0,    0,    0,
  105,    0,    0,    0,  106,    0,    0,    0,    0,   54,
    0,   65,   66,   54,    0,    0,    0,    0,   70,   71,
   72,   54,   75,   76,   77,   54,   79,   80,   81,   82,
   83,   84,   85,   86,   87,   88,    0,    0,    0,   91,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  108,  109,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  128,
  129,    0,  131,    0,    0,    0,    0,    0,    0,    0,
    0,  141,    0,  142,
};
short yycheck[] = {                                     284,
    0,  258,  257,  284,  318,  436,  284,  277,  278,    0,
  274,  274,  290,    0,  284,  318,  289,  289,    0,  318,
  289,    0,  277,  278,  279,  318,    0,  332,  419,  284,
    0,  309,  429,  429,    0,  419,    0,  555,  583,    0,
  284,   57,  290,   59,  435,  318,  290,  436,  436,  318,
    0,  435,  436,  583,  436,    0,  436,  436,  368,  318,
  368,  309,  583,  429,  436,  309,  583,  583,  583,  429,
  583,  429,  583,  479,  479,  583,    0,  479,  583,  479,
  479,  436,  436,  397,  578,  578,  472,  365,  485,  485,
  431,  516,    0,  516,    0,    0,  126,  138,   25,   67,
   56,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  365,   -1,  397,
  375,  365,  436,   -1,  515,   -1,   -1,   -1,   -1,   -1,
   -1,  515,   -1,   -1,   -1,   -1,   -1,   -1,   -1,    0,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  404,
  405,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  436,   -1,
   -1,    0,   -1,   -1,  595,   -1,   -1,   -1,   -1,   -1,
    0,   -1,   -1,  451,   -1,  453,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  438,  439,   -1,  441,   -1,  436,    0,
   -1,   -1,  436,   -1,  472,   -1,  469,  469,   -1,   -1,
  469,   -1,   -1,  451,   -1,  453,   -1,  451,   -1,  453,
    0,  595,   -1,  516,  469,  470,   -1,  516,   -1,  483,
  483,  526,  527,  516,  472,  498,  498,   -1,  472,  498,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  516,   -1,   -1,   -1,  516,   -1,   -1,
   -1,   -1,  516,   -1,   -1,  533,  537,  257,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  257,  537,   -1,   -1,
   -1,   -1,   -1,  551,  274,  257,   -1,  277,  278,  279,
   -1,  595,  537,  274,  284,  533,  277,  278,  279,  533,
   -1,   -1,  274,   -1,    0,  277,  278,  279,  583,   -1,
  578,   -1,   -1,  551,  274,  583,   -1,  551,  274,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  318,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  318,  583,  274,
  578,  588,  332,   -1,  578,  583,  318,   -1,   -1,  583,
   -1,  332,   -1,   -1,   -1,  332,   -1,   -1,   -1,   -1,
  332,   -1,   -1,  332,  318,   -1,   -1,   -1,  332,   -1,
   -1,   -1,  332,   -1,   -1,   -1,  332,   -1,  332,   -1,
   -1,  332,   -1,   -1,   -1,  375,    0,   -1,   -1,   -1,
   -1,   -1,  332,   -1,  375,   -1,   -1,  332,   -1,   -1,
   -1,   -1,   -1,  375,   -1,   -1,   -1,  397,   -1,    0,
   -1,   -1,   -1,   -1,  404,  405,  397,   -1,  332,   -1,
   -1,   -1,   -1,  404,  405,  397,   -1,   -1,   -1,  419,
   -1,   -1,  404,  405,  332,   -1,   -1,  332,  419,   -1,
   -1,   -1,   -1,  397,   -1,  435,  436,  419,  438,  439,
   -1,  441,   -1,   -1,  435,  436,  257,  438,  439,   -1,
  441,   -1,   -1,  435,  436,   -1,  438,  439,  419,  441,
   -1,   -1,   -1,  274,   -1,   -1,   -1,  257,  279,  469,
  470,  332,  436,   -1,  435,  436,   -1,   -1,  469,  470,
   -1,    0,   -1,  483,  274,   -1,   -1,  469,  470,  279,
   -1,   -1,  483,  332,   -1,  419,   -1,   -1,   -1,   -1,
   -1,  483,  332,   -1,    0,   -1,   -1,  318,   -1,   -1,
   -1,  435,  436,  483,  419,  515,  516,  483,   -1,   -1,
   -1,  332,   -1,   -1,  515,  516,  526,  527,  318,   -1,
  435,  436,   -1,  515,  516,  526,  527,  537,  483,  526,
  527,   -1,  332,   -1,  526,  527,  516,  526,  527,   -1,
  516,  257,  526,  527,  515,   -1,  526,  527,  419,   -1,
  526,  527,  526,  527,  375,  526,  527,   -1,  274,   -1,
   -1,  516,   -1,  279,  435,  436,  526,  527,   -1,   -1,
  419,  526,  527,  583,   -1,  375,  397,   -1,   -1,  419,
   -1,  515,  583,  404,  405,  595,  435,  436,   -1,   -1,
   -1,  583,  526,  527,  595,  435,  436,  397,  419,    0,
  515,   -1,  318,  595,  404,  405,   -1,   -1,  526,  527,
   -1,  526,  527,   -1,  435,  436,  332,  438,  439,  419,
  441,  595,   -1,  257,  595,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  435,  436,   -1,  438,  439,
  274,  441,   -1,   -1,  515,  279,  257,   -1,  469,  470,
   -1,   -1,   -1,   -1,   -1,  526,  527,   -1,   -1,  375,
   -1,  595,  483,  274,   -1,   -1,  515,   -1,  279,  469,
  470,   -1,   -1,   -1,   -1,  515,   -1,  526,  527,   -1,
  595,  397,   -1,  483,  318,   -1,  526,  527,  404,  405,
   -1,   -1,   -1,   -1,  515,  516,   -1,   -1,  332,   -1,
   -1,    0,   -1,  419,   -1,  526,  527,  318,   -1,   -1,
   -1,   -1,   -1,  257,   -1,  515,  516,   -1,   -1,  435,
  436,  332,  438,  439,  595,  441,  526,  527,  257,   -1,
   -1,   -1,   -1,  277,  278,   -1,   -1,   -1,   -1,   -1,
  284,  375,   -1,   -1,   -1,  274,  595,   -1,   -1,   -1,
  279,  257,   -1,  469,  470,  595,   -1,   -1,   -1,   -1,
   -1,   -1,  583,  397,  375,   -1,   -1,  483,  274,   -1,
  404,  405,    0,  279,  595,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  583,   -1,  419,  397,   -1,   -1,  318,
   -1,   -1,   -1,  404,  405,  595,   -1,   -1,   -1,  515,
  516,  435,  436,  332,  438,  439,   -1,  441,  419,   -1,
  526,  527,  318,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  435,  436,  332,  438,  439,   -1,
  441,  375,   -1,   -1,   -1,  469,  470,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  375,   -1,   -1,  483,
   -1,   -1,   -1,   -1,   -1,   -1,  257,   -1,  469,  470,
  404,  405,   -1,   -1,   -1,   -1,   -1,  583,  397,  375,
   -1,   -1,  483,  274,   -1,  404,  405,   -1,  279,  595,
   -1,  515,  516,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  419,  397,  526,  527,  438,  439,   -1,  441,  404,  405,
   -1,   -1,   -1,   -1,  515,  516,  435,  436,   -1,  438,
  439,   -1,  441,  419,   -1,  526,  527,  318,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  469,  470,   -1,   -1,  435,
  436,  332,  438,  439,   -1,  441,   -1,   -1,   -1,   -1,
  469,  470,   -1,   -1,   -1,   -1,  257,   -1,   -1,  583,
   -1,   -1,   -1,   -1,  483,   -1,   -1,   -1,  257,   -1,
   -1,  595,   -1,  469,  470,   -1,  277,  278,   -1,   -1,
   -1,   -1,  583,  284,  375,  274,   -1,  483,   -1,   -1,
  279,   -1,   -1,   -1,  595,   -1,  515,  516,   -1,   -1,
   -1,   -1,   -1,  537,   -1,   -1,  397,  526,  527,   -1,
   -1,   -1,   -1,  404,  405,   -1,   -1,   -1,   -1,  515,
  516,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  419,  318,
  526,  527,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,
   -1,   -1,   -1,  332,  435,  436,   -1,  438,  439,   -1,
  441,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  277,
  278,  279,   -1,   -1,  583,   -1,  284,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  375,   -1,  595,   -1,  469,  470,
   -1,   -1,   -1,   -1,   -1,   -1,  375,  583,   -1,   -1,
   -1,   -1,  483,   -1,   -1,   -1,   -1,   -1,   -1,  595,
  318,  257,   -1,  404,  405,   -1,   -1,   -1,  397,   -1,
   -1,   -1,   -1,   -1,  332,  404,  405,   -1,   -1,   -1,
   -1,  277,  278,   -1,  515,  516,   -1,   -1,  284,   -1,
  419,   -1,   -1,   -1,   -1,  526,  527,  438,  439,  257,
  441,   -1,   -1,  257,   -1,   -1,  435,  436,   -1,  438,
  439,  257,  441,   -1,   -1,  257,   -1,  375,   -1,  277,
  278,   -1,   -1,  277,  278,   -1,  284,   -1,  469,  470,
  284,  277,  278,   -1,   -1,  277,  278,   -1,  284,  397,
  469,  470,  284,   -1,   -1,   -1,  404,  405,   -1,   -1,
   -1,   -1,  583,   -1,  483,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  595,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  516,   -1,   -1,  436,  375,
  438,  439,   -1,  441,   -1,   -1,  515,  516,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  537,  526,  527,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  404,  405,
   -1,  469,  470,   -1,   -1,   -1,   -1,  375,   -1,   -1,
   -1,  375,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  375,
   -1,   -1,   -1,  375,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  438,  439,   -1,  441,  404,  405,   -1,   -1,
  404,  405,   -1,   -1,  583,   -1,   -1,   -1,  404,  405,
   -1,   -1,  404,  405,   -1,   -1,  595,   -1,  526,  527,
   -1,   -1,   -1,  469,  470,   -1,   -1,   -1,   -1,  537,
  438,  439,   -1,  441,  438,  439,   -1,  441,   -1,   -1,
   -1,   -1,  438,  439,   -1,  441,  438,  439,   -1,  441,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  469,  470,   -1,   -1,  469,  470,   -1,   -1,   -1,
  516,   -1,   -1,  469,  470,  583,   -1,  469,  470,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  595,   -1,   -1,
   -1,  537,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  516,   -1,
   -1,   -1,  516,   -1,   -1,   -1,   15,   -1,   -1,   -1,
  516,   -1,   -1,   -1,  516,   -1,   -1,   -1,   -1,  537,
   -1,   30,   31,  537,   -1,   -1,   -1,   -1,   37,   38,
   39,  537,   41,   42,   43,  537,   45,   46,   47,   48,
   49,   50,   51,   52,   53,   54,   -1,   -1,   -1,   58,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   93,   94,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  118,
  119,   -1,  121,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  130,   -1,  132,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 602
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"PERCENT","SQL_TYPE","SQL_ABS",
"ACOS","AMPERSAND","SQL_ABSOLUTE","ADA","ADD","ADD_DAYS","ADD_HOURS",
"ADD_MINUTES","ADD_MONTHS","ADD_SECONDS","ADD_YEARS","ALL","ALLOCATE","ALTER",
"AND","ANY","ARE","ARITHMETIC_MINUS","ARITHMETIC_PLUS","AS","ASIN","ASC",
"ASCII","ASSERTION","ASTERISK","AT","ATAN","ATAN2","AUTHORIZATION",
"AUTO_INCREMENT","AVG","BEFORE","SQL_BEGIN","BETWEEN","BIGINT","BINARY","BIT",
"BIT_LENGTH","BREAK","BY","CASCADE","CASCADED","CASE","CAST","CATALOG",
"CEILING","CENTER","SQL_CHAR","CHAR_LENGTH","CHARACTER_STRING_LITERAL","CHECK",
"CLOSE","COALESCE","COBOL","COLLATE","COLLATION","COLON","COLUMN","COMMA",
"COMMIT","COMPUTE","CONCAT","CONNECT","CONNECTION","CONSTRAINT","CONSTRAINTS",
"CONTINUE","CONVERT","CORRESPONDING","COS","COT","COUNT","CREATE","CURDATE",
"CURRENT","CURRENT_DATE","CURRENT_TIME","CURRENT_TIMESTAMP","CURTIME","CURSOR",
"DATABASE","SQL_DATE","DATE_FORMAT","DATE_REMAINDER","DATE_VALUE","DAY",
"DAYOFMONTH","DAYOFWEEK","DAYOFYEAR","DAYS_BETWEEN","DEALLOCATE","DEC",
"DECLARE","DEFAULT","DEFERRABLE","DEFERRED","SQL_DELETE","DESC","DESCRIBE",
"DESCRIPTOR","DIAGNOSTICS","DICTIONARY","DIRECTORY","DISCONNECT","DISPLACEMENT",
"DISTINCT","DOLLAR_SIGN","DOMAIN_TOKEN","DOT","SQL_DOUBLE",
"DOUBLE_QUOTED_STRING","DROP","ELSE","END","END_EXEC","EQUAL","ESCAPE","EXCEPT",
"SQL_EXCEPTION","EXEC","EXECUTE","EXISTS","EXP","EXPONENT","EXTERNAL","EXTRACT",
"SQL_FALSE","FETCH","FIRST","SQL_FLOAT","FLOOR","FN","FOR","FOREIGN","FORTRAN",
"FOUND","FOUR_DIGITS","FROM","FULL","GET","GLOBAL","GO","GOTO","GRANT",
"GREATER_THAN","GREATER_OR_EQUAL","GROUP","HAVING","HOUR","HOURS_BETWEEN",
"IDENTITY","IFNULL","SQL_IGNORE","IMMEDIATE","SQL_IN","INCLUDE","INDEX",
"INDICATOR","INITIALLY","INNER","INPUT","INSENSITIVE","INSERT","INTEGER",
"INTERSECT","INTERVAL","INTO","IS","ISOLATION","JOIN","JUSTIFY","KEY",
"LANGUAGE","LAST","LCASE","LEFT","LEFTPAREN","LENGTH","LESS_OR_EQUAL",
"LESS_THAN","LEVEL","LIKE","LINE_WIDTH","LOCAL","LOCATE","LOG","SQL_LONG",
"LOWER","LTRIM","LTRIP","MATCH","SQL_MAX","MICROSOFT","SQL_MIN","MINUS",
"MINUTE","MINUTES_BETWEEN","MOD","MODIFY","MODULE","MONTH","MONTHS_BETWEEN",
"MUMPS","NAMES","NATIONAL","NCHAR","NEXT","NODUP","NONE","NOT","NOT_EQUAL",
"NOW","SQL_NULL","NULLIF","NUMERIC","OCTET_LENGTH","ODBC","OF","SQL_OFF",
"SQL_ON","ONLY","OPEN","OPTION","OR","ORDER","OUTER","OUTPUT","OVERLAPS","PAGE",
"PARTIAL","SQL_PASCAL","PERSISTENT","CQL_PI","PLI","POSITION","PRECISION",
"PREPARE","PRESERVE","PRIMARY","PRIOR","PRIVILEGES","PROCEDURE","PRODUCT",
"PUBLIC","QUARTER","QUESTION_MARK","QUIT","RAND","READ_ONLY","REAL",
"REFERENCES","REPEAT","REPLACE","RESTRICT","REVOKE","RIGHT","RIGHTPAREN",
"ROLLBACK","ROWS","RPAD","RTRIM","SCHEMA","SCREEN_WIDTH","SCROLL","SECOND",
"SECONDS_BETWEEN","SELECT","SEMICOLON","SEQUENCE","SETOPT","SET","SHOWOPT",
"SIGN","SIGNED_INTEGER","SIN","SINGLE_QUOTE","SQL_SIZE","SLASH","SMALLINT",
"SOME","SPACE","SQL","SQL_TRUE","SQLCA","SQLCODE","SQLERROR","SQLSTATE",
"SQLWARNING","SQRT","STDEV","SUBSTRING","SUM","SYSDATE","SYSDATE_FORMAT",
"SYSTEM","TABLE","TAN","TEMPORARY","THEN","THREE_DIGITS","TIME","TIMESTAMP",
"TIMEZONE_HOUR","TIMEZONE_MINUTE","TINYINT","TO","TO_CHAR","TO_DATE",
"TRANSACTION","TRANSLATE","TRANSLATION","TRUNCATE","GENERAL_TITLE","TWO_DIGITS",
"UCASE","UNION","UNIQUE","SQL_UNKNOWN","UNSIGNED_INTEGER","UPDATE","UPPER",
"USAGE","USER","USER_DEFINED_NAME","USING","VALUE","VALUES","VARBINARY",
"VARCHAR","VARYING","VENDOR","VIEW","WEEK","WHEN","WHENEVER","WHERE",
"WHERE_CURRENT_OF","WITH","WORD_WRAPPED","WORK","WRAPPED","YEAR",
"YEARS_BETWEEN",
};
char *yyrule[] = {
"$accept : TopLevelStatement",
"TopLevelStatement : Statement SEMICOLON",
"TopLevelStatement : Statement",
"Statement : Statement CreateTableStatement",
"Statement : Statement SelectStatement",
"Statement :",
"$$1 :",
"CreateTableStatement : CREATE TABLE USER_DEFINED_NAME $$1 LEFTPAREN ColDefs RIGHTPAREN",
"ColDefs : ColDefs COMMA ColDef",
"ColDefs : ColDef",
"ColDef : USER_DEFINED_NAME ColType",
"ColDef : USER_DEFINED_NAME ColType ColKeys",
"ColKeys : ColKeys ColKey",
"ColKeys : ColKey",
"ColKey : PRIMARY KEY",
"ColKey : NOT SQL_NULL",
"ColKey : AUTO_INCREMENT",
"ColType : SQL_TYPE",
"ColType : SQL_TYPE LEFTPAREN UNSIGNED_INTEGER RIGHTPAREN",
"ColType : VARCHAR LEFTPAREN UNSIGNED_INTEGER RIGHTPAREN",
"ColType :",
"SelectStatement : Select ColViews",
"SelectStatement : Select ColViews Tables",
"SelectStatement : Select Tables",
"SelectStatement : Select ColViews Conditions",
"SelectStatement : Select ColViews Tables Conditions",
"Select : SELECT",
"Conditions : WHERE ColExpression",
"Conditions : Conditions AND ColExpression",
"Conditions : Conditions OR ColExpression",
"Conditions : LEFTPAREN Conditions RIGHTPAREN",
"Tables : FROM FlatTableList",
"Tables : Tables LEFT JOIN USER_DEFINED_NAME SQL_ON ColExpression",
"Tables : Tables LEFT OUTER JOIN USER_DEFINED_NAME SQL_ON ColExpression",
"Tables : Tables INNER JOIN USER_DEFINED_NAME SQL_ON ColExpression",
"Tables : Tables RIGHT JOIN USER_DEFINED_NAME SQL_ON ColExpression",
"Tables : Tables RIGHT OUTER JOIN USER_DEFINED_NAME SQL_ON ColExpression",
"FlatTableList : FlatTableList COMMA FlatTable",
"FlatTableList : FlatTable",
"FlatTable : USER_DEFINED_NAME",
"ColViews : ColViews COMMA ColItem",
"ColViews : ColItem",
"ColItem : ColExpression",
"ColItem : ColWildCard",
"ColItem : ColExpression AS USER_DEFINED_NAME",
"ColItem : ColExpression USER_DEFINED_NAME",
"ColView : USER_DEFINED_NAME",
"ColView : USER_DEFINED_NAME DOT USER_DEFINED_NAME",
"ColView : SQL_NULL",
"ColView : CHARACTER_STRING_LITERAL",
"ColView : SIGNED_INTEGER",
"ColView : UNSIGNED_INTEGER",
"ColExpression : ColView",
"ColExpression : ColExpression ARITHMETIC_PLUS ColExpression",
"ColExpression : ColExpression ARITHMETIC_MINUS ColExpression",
"ColExpression : ColExpression SLASH ColExpression",
"ColExpression : ColExpression ASTERISK ColExpression",
"ColExpression : ColExpression NOT ColExpression",
"ColExpression : ColExpression EQUAL ColExpression",
"ColExpression : ColExpression NOT_EQUAL ColExpression",
"ColExpression : ColExpression GREATER_THAN ColExpression",
"ColExpression : ColExpression GREATER_OR_EQUAL ColExpression",
"ColExpression : ColExpression LESS_THAN ColExpression",
"ColExpression : ColExpression LESS_OR_EQUAL ColExpression",
"ColExpression : ColExpression LIKE ColExpression",
"ColExpression : ColExpression PERCENT ColExpression",
"ColExpression : LEFTPAREN ColExpression RIGHTPAREN",
"ColExpression : SUM LEFTPAREN ColExpression RIGHTPAREN",
"ColExpression : SQL_MIN LEFTPAREN ColExpression RIGHTPAREN",
"ColExpression : SQL_MAX LEFTPAREN ColExpression RIGHTPAREN",
"ColExpression : AVG LEFTPAREN ColExpression RIGHTPAREN",
"ColExpression : DISTINCT LEFTPAREN ColExpression RIGHTPAREN",
"ColWildCard : ASTERISK",
"ColWildCard : USER_DEFINED_NAME DOT ASTERISK",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 970 "sqlparser.y"

#line 1063 "y.tab.c"
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 518 "sqlparser.y"
{ }
break;
case 2:
#line 519 "sqlparser.y"
{ }
break;
case 3:
#line 523 "sqlparser.y"
{ YYACCEPT; }
break;
case 4:
#line 524 "sqlparser.y"
{  }
break;
case 6:
#line 530 "sqlparser.y"
{
	parser->setOperation(KexiDB::Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
}
break;
case 9:
#line 539 "sqlparser.y"
{
}
break;
case 10:
#line 545 "sqlparser.y"
{
	kdDebug() << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

/*	delete field;*/
	field = 0;
}
break;
case 11:
#line 554 "sqlparser.y"
{
	kdDebug() << "adding field " << yyvsp[-2].stringValue << endl;
	field->setName(yyvsp[-2].stringValue);
	parser->table()->addField(field);

/*	if(field->isPrimaryKey())*/
/*		parser->table()->addPrimaryKey(field->name());*/

/*	delete field;*/
/*	field = 0;*/
}
break;
case 13:
#line 569 "sqlparser.y"
{
}
break;
case 14:
#line 575 "sqlparser.y"
{
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
}
break;
case 15:
#line 580 "sqlparser.y"
{
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
}
break;
case 16:
#line 585 "sqlparser.y"
{
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
}
break;
case 17:
#line 593 "sqlparser.y"
{
	field = new KexiDB::Field();
	field->setType(yyvsp[0].coltype);
}
break;
case 18:
#line 598 "sqlparser.y"
{
	kdDebug() << "sql + length" << endl;
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].coltype);
}
break;
case 19:
#line 605 "sqlparser.y"
{
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(KexiDB::Field::Text);
}
break;
case 20:
#line 611 "sqlparser.y"
{
	/* SQLITE compatibillity*/
	field = new KexiDB::Field();
	field->setType(KexiDB::Field::InvalidType);
}
break;
case 21:
#line 620 "sqlparser.y"
{
	kdDebug() << "Select ColViews" << endl;
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
case 22:
#line 639 "sqlparser.y"
{
	kdDebug() << "Select ColViews Tables" << endl;
}
break;
case 23:
#line 643 "sqlparser.y"
{
	kdDebug() << "Select ColViews Tables" << endl;
}
break;
case 24:
#line 647 "sqlparser.y"
{
	kdDebug() << "Select ColViews Conditions" << endl;
}
break;
case 25:
#line 651 "sqlparser.y"
{
	kdDebug() << "Select ColViews Tables Conditions" << endl;
}
break;
case 26:
#line 658 "sqlparser.y"
{
	kdDebug() << "SELECT" << endl;
	parser->createSelect();
	parser->setOperation(KexiDB::Parser::OP_Select);
}
break;
case 27:
#line 667 "sqlparser.y"
{
	kdDebug() << "WHERE " << yyvsp[0].field << endl;
}
break;
case 28:
#line 671 "sqlparser.y"
{
	kdDebug() << "AND " << yyvsp[0].field << endl;
}
break;
case 29:
#line 675 "sqlparser.y"
{
	kdDebug() << "OR " << yyvsp[0].field << endl;
}
break;
case 30:
#line 679 "sqlparser.y"
{
	kdDebug() << "()" << endl;
}
break;
case 31:
#line 686 "sqlparser.y"
{
}
break;
case 32:
#line 689 "sqlparser.y"
{
	kdDebug() << "LEFT JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
break;
case 33:
#line 694 "sqlparser.y"
{
	kdDebug() << "LEFT OUTER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
break;
case 34:
#line 699 "sqlparser.y"
{
	kdDebug() << "INNER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
break;
case 35:
#line 704 "sqlparser.y"
{
	kdDebug() << "RIGHT JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
break;
case 36:
#line 709 "sqlparser.y"
{
	kdDebug() << "RIGHT OUTER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
break;
case 38:
#line 717 "sqlparser.y"
{
}
break;
case 39:
#line 723 "sqlparser.y"
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

		if(item->table() && !item->isQueryAsterisk())
		{
			KexiDB::Field *f = item->table()->field(item->name());
			if(!f)
			{
				KexiDB::ParserError err(i18n("Field List Error"), i18n("Unknown column '%1' in table '%2'").arg(item->name()).arg(schema->name()), ctoken, current);
				parser->setError(err);
				yyerror("fieldlisterror");
			}	
		}
	}
}
break;
case 41:
#line 758 "sqlparser.y"
{
}
break;
case 42:
#line 764 "sqlparser.y"
{
	kdDebug() << " adding field '" << yyvsp[0].field->name() << "'" << endl;
	parser->select()->addField(yyvsp[0].field);
/*	parser->fieldList()->append($1);*/
}
break;
case 43:
#line 770 "sqlparser.y"
{
}
break;
case 44:
#line 773 "sqlparser.y"
{
	kdDebug() << " adding field '" << yyvsp[-2].field->name() << "' as '" << yyvsp[0].stringValue << "'" << endl;
/*	parser->fieldList()->append($1);*/
	parser->select()->addField(yyvsp[-2].field);
	parser->select()->setAlias(yyvsp[-2].field, yyvsp[0].stringValue);
}
break;
case 45:
#line 780 "sqlparser.y"
{
	kdDebug() << " adding field '" << yyvsp[-1].field->name() << "' as '" << yyvsp[0].stringValue << "'" << endl;
/*	parser->fieldList()->append($1);*/
	parser->select()->addField(yyvsp[-1].field);
	parser->select()->setAlias(yyvsp[-1].field, yyvsp[0].stringValue);
}
break;
case 46:
#line 790 "sqlparser.y"
{
	kdDebug() << "  + col " << yyvsp[0].stringValue << endl;
	yyval.field = new KexiDB::Field();
	yyval.field->setName(yyvsp[0].stringValue);
	yyval.field->setTable(dummy);
/*	parser->select()->addField(field);*/
	requiresTable = true;
}
break;
case 47:
#line 799 "sqlparser.y"
{
	kdDebug() << "  + col " << yyvsp[0].stringValue << " from " << yyvsp[-2].stringValue << endl;
	yyval.field = new KexiDB::Field();
/*	s->setTable($1);*/
	yyval.field->setName(yyvsp[0].stringValue);
	yyval.field->setTable(parser->db()->tableSchema(yyvsp[-2].stringValue));
/*	parser->select()->addField(field);*/
	requiresTable = true;
}
break;
case 48:
#line 809 "sqlparser.y"
{
	yyval.field = new KexiDB::Field();
	yyval.field->setName(QString::null);
}
break;
case 49:
#line 814 "sqlparser.y"
{
	yyval.field = new KexiDB::Field();
	yyval.field->setName(yyvsp[0].stringValue);
/*	parser->select()->addField(field);*/
	kdDebug() << "  + constant " << yyvsp[0].stringValue << endl;
}
break;
case 50:
#line 821 "sqlparser.y"
{
	yyval.field = new KexiDB::Field();
	yyval.field->setName(QString::number(yyvsp[0].integerValue));
/*	parser->select()->addField(field);*/
	kdDebug() << "  + numerical constant " << yyvsp[0].integerValue << endl;
}
break;
case 51:
#line 828 "sqlparser.y"
{
	yyval.field = new KexiDB::Field();
	yyval.field->setName(QString::number(yyvsp[0].integerValue));
/*	parser->select()->addField(field);*/
	kdDebug() << "  + numerical constant " << yyvsp[0].integerValue << endl;
}
break;
case 52:
#line 838 "sqlparser.y"
{
	yyval.field = yyvsp[0].field;
	kdDebug() << "to expression: " << yyval.field->name() << endl;
}
break;
case 53:
#line 843 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " + " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " + " + yyvsp[0].field->name());
}
break;
case 54:
#line 848 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " - " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " - " + yyvsp[0].field->name());
}
break;
case 55:
#line 853 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " / " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " / " + yyvsp[0].field->name());
}
break;
case 56:
#line 858 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " * " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " * " + yyvsp[0].field->name());
}
break;
case 57:
#line 863 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " NOT " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " NOT " + yyvsp[0].field->name());
}
break;
case 58:
#line 868 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " = " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " = " + yyvsp[0].field->name());
}
break;
case 59:
#line 873 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " <> " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " <> " + yyvsp[0].field->name());
}
break;
case 60:
#line 878 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " > " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " > " + yyvsp[0].field->name());
}
break;
case 61:
#line 883 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " >= " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " >= " + yyvsp[0].field->name());
}
break;
case 62:
#line 888 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " < " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " < " + yyvsp[0].field->name());
}
break;
case 63:
#line 893 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " <= " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " <= " + yyvsp[0].field->name());
}
break;
case 64:
#line 898 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " LIKE " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " LIKE " + yyvsp[0].field->name());
}
break;
case 65:
#line 903 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " % " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " % " + yyvsp[0].field->name());
}
break;
case 66:
#line 908 "sqlparser.y"
{
	kdDebug() << "(" << yyvsp[-1].field->name() << ")" << endl;
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("(" + yyvsp[-1].field->name() + ")");
}
break;
case 67:
#line 914 "sqlparser.y"
{
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("SUM(" + yyvsp[-1].field->name() + ")");
/*wait	$$->containsGroupingAggregate(true);*/
/*wait	parser->select()->grouped(true);*/
}
break;
case 68:
#line 921 "sqlparser.y"
{
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("MIN(" + yyvsp[-1].field->name() + ")");
/*wait	$$->containsGroupingAggregate(true);*/
/*wait	parser->select()->grouped(true);*/
}
break;
case 69:
#line 928 "sqlparser.y"
{
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("MAX(" + yyvsp[-1].field->name() + ")");
/*wait	$$->containsGroupingAggregate(true);*/
/*wait	parser->select()->grouped(true);*/
}
break;
case 70:
#line 935 "sqlparser.y"
{
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("AVG(" + yyvsp[-1].field->name() + ")");
/*wait	$$->containsGroupingAggregate(true);*/
/*wait	parser->select()->grouped(true);*/
}
break;
case 71:
#line 942 "sqlparser.y"
{
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("DISTINCT(" + yyvsp[-1].field->name() + ")");
}
break;
case 72:
#line 950 "sqlparser.y"
{
	kdDebug() << "all columns" << endl;
/*	field = new KexiDB::Field();*/
/*	field->setName("*");*/
	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), dummy);
	parser->select()->addAsterisk(ast);
/*	fieldList.append(ast);*/
	requiresTable = true;
}
break;
case 73:
#line 960 "sqlparser.y"
{
	kdDebug() << "  + all columns from " << yyvsp[-2].stringValue << endl;
	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), parser->db()->tableSchema(yyvsp[-2].stringValue));
	parser->select()->addAsterisk(ast);
/*	fieldList.append(ast);*/
	requiresTable = true;
}
break;
#line 1733 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
