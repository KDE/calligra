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
    5,    5,    5,   12,   15,   15,   15,   15,   14,   14,
   14,   14,   14,   14,   16,   16,   17,   13,   13,   18,
   18,   18,   18,    2,    2,    2,    2,    2,    2,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,   19,
   19,
};
short yylen[] = {                                         2,
    2,    1,    2,    2,    0,    0,    7,    3,    1,    2,
    3,    2,    1,    2,    2,    1,    1,    4,    4,    0,
    2,    2,    3,    1,    2,    3,    3,    3,    2,    6,
    7,    6,    6,    7,    3,    1,    1,    3,    1,    1,
    1,    3,    2,    1,    3,    1,    1,    1,    1,    1,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    4,    4,    4,    4,    4,    1,
    3,
};
short yydefred[] = {                                      5,
    0,    0,    0,   24,    1,    3,    0,    0,    0,    0,
    0,   70,    0,   47,    0,    0,    0,    0,   46,   48,
    0,   49,    0,    0,   50,    0,   39,   41,    6,   37,
    0,   36,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   43,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   64,    0,    0,    0,
   71,   45,    0,    0,    0,   42,   54,    0,    0,    0,
    0,    0,    0,    0,    0,   53,   38,    0,   35,    0,
    0,    0,   28,    0,    0,    0,    0,   68,   69,   67,
   66,   65,    0,    0,    9,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    7,    0,    0,    0,    0,    0,
    0,    0,   16,    0,    0,    0,   13,    8,    0,    0,
    0,    0,   15,   14,   12,   18,   19,
};
short yydgoto[] = {                                       1,
   24,   25,    2,    6,    7,   63,  114,  115,  123,  136,
  137,    8,   26,   11,   38,   31,   32,   27,   28,
};
short yysindex[] = {                                      0,
    0, -298, -537,    0,    0,    0, -357, -270, -542, -539,
 -376,    0, -390,    0, -389, -239, -388, -387,    0,    0,
 -386,    0, -316, -254,    0, -265,    0,    0,    0,    0,
 -264,    0, -374, -412, -420, -393, -239, -262, -239, -239,
 -312, -246, -239, -239, -239, -282, -239, -239, -239, -526,
 -239, -239, -239, -239, -239, -239, -239, -239, -239, -239,
    0, -270, -378, -539, -521, -520, -365, -266, -518, -363,
  900, -239, -239, -196,  821, -516,    0,  845,  861,  883,
    0,    0, -251, -277, -277,    0,    0, -251, -251, -251,
 -251, -251, -251, -251, -251,    0,    0, -515,    0, -410,
 -407, -508,    0, -403, -505,  900,  900,    0,    0,    0,
    0,    0, -258, -313,    0, -239, -239, -400, -239, -399,
 -353, -352, -280, -515,    0,  900,  900, -239,  900, -239,
 -493, -492,    0, -385, -342, -280,    0,    0,  900,  900,
 -426, -425,    0,    0,    0,    0,    0,
};
short yyrindex[] = {                                      0,
    0,   93,    0,    0,    0,    0,   21,    0,    0,    0,
    6,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  783,    4,    0,   13,    0,    0,    0,    0,
   77,    0,    0,    0,    0,    0,    0,   15,    0,    0,
    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   28,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  190,   10,   19,    0,    0,  211,  295,  377,
  400,  482,  505,  610,  712,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   35,   37,    0,    0,    0,
    0,    0, -247,    0,    0,    0,    0,    0,    0,    0,
 -244,    0, -296,    0,    0,  162,  171,    0,  305,    0,
    0,    0,    0,    0,    0, -289,    0,    0,  477,  639,
    0,    0,    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
 1357,    0,    0,    0,    0,    0,    0,  -30,    0,    0,
  -40,    0,    0,    0,   62,    0,   34,   38,    0,
};
#define YYTABLESIZE 1487
short yytable[] = {                                     121,
   44,   81,   47,   40,  124,   22,   51,   72,  133,   52,
   47,   72,   21,   12,   23,   35,   66,    9,   51,   13,
    4,   10,   48,   49,   50,   48,   49,   25,   11,   51,
   48,   49,   51,    3,   26,   69,   27,   51,   14,   10,
   29,   20,   33,   30,   17,   39,   40,   43,   44,   45,
   13,   46,   62,   64,   65,   76,   86,   98,   34,   35,
   47,  100,  101,  102,  104,  105,   82,  113,  116,   14,
   20,  117,   67,   17,  118,  119,   29,  120,  128,  130,
   48,   49,  131,  132,  141,  142,  143,   51,  144,  146,
  147,   70,    2,  138,   15,  145,   68,   99,    0,   97,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   52,    0,    0,    0,    0,   15,    0,    0,   52,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   36,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   53,
   54,    0,    0,    0,    0,    0,    0,   53,   54,    0,
    0,   32,    0,    0,    0,   16,    0,    0,    0,    0,
   30,    0,    0,    0,   37,    0,    0,    0,   52,    0,
   17,    0,   18,   55,   56,    0,   57,    0,  134,   63,
    0,   55,   56,    0,   57,    0,   16,    0,    0,    0,
    0,   19,  125,    0,    0,    0,    0,   53,   54,    0,
   56,   17,    0,   18,   58,   59,   73,  135,   37,   10,
   73,   20,   58,   59,   17,    0,   11,    4,    5,    0,
    0,    0,   19,    0,    0,    0,    0,    0,    0,    0,
    0,   55,   56,    0,   57,    0,    0,    0,    0,  103,
   20,    0,    0,   17,    0,    0,    0,   44,    0,   60,
    0,    0,   20,    0,    0,    0,   52,    0,   20,   77,
    0,   17,   58,   59,   44,   51,    0,   44,   44,   44,
   21,    0,   60,   52,   44,   60,   52,   52,   52,    0,
   60,    0,   51,   20,   58,   51,   51,   51,    0,    0,
   82,   25,    0,    0,   33,    0,    0,   22,   26,    0,
   27,   21,   23,    0,    0,    0,    0,    0,   44,  108,
    0,   40,    0,    0,    0,    0,    0,   52,   61,  122,
    0,    0,   44,    0,    0,   40,   51,   22,   22,    0,
   60,   52,    0,   41,   21,    0,   23,    0,    0,    0,
   51,    0,    4,    0,    0,    0,    0,    0,    0,   25,
    0,    0,    0,    0,    0,    0,   26,    0,   27,    0,
    0,    0,    0,    0,    0,   44,   59,    0,    0,    0,
    0,    0,    0,    0,   52,    0,    0,    0,    0,    0,
    0,    0,    0,   51,    0,    0,    0,   44,    0,   61,
   40,    0,   22,    0,   44,   44,   52,    0,   29,   21,
    0,   23,    0,   52,   52,   51,    0,    0,    0,   44,
    0,    0,   51,   51,   25,    0,    0,    0,   52,    0,
    0,   26,    0,   27,    0,   44,   44,   51,   44,   44,
    0,   44,    0,    0,   52,   52,   63,   52,   52,    0,
   52,    0,    0,   51,   51,    0,   51,   51,    0,   51,
    0,    0,    0,   63,    0,    0,    0,   56,   63,   44,
   44,    0,    0,   29,    0,    0,   31,    0,   52,   52,
    0,   60,    0,   44,   56,    0,    0,   51,   51,   56,
    0,    0,   52,   32,    0,   29,    0,    0,    0,    0,
    0,   51,   30,    0,   62,    0,    0,   63,    0,    0,
   25,   29,   29,    0,    0,   44,   44,   26,    0,   27,
    0,   63,    0,    0,   52,   52,   44,   44,   56,   40,
   40,   22,   22,   51,   51,   52,   52,   44,   21,   21,
   23,   23,   56,   25,   51,   51,    4,    4,    0,    0,
   26,   58,   27,   25,   25,    0,    0,    0,   32,    0,
   26,   26,   27,   27,   63,    0,    0,   30,   58,    0,
    0,    0,    0,   58,    0,    0,    0,    0,    0,    0,
   32,    0,    0,   44,    0,   56,   63,    0,    0,   30,
    0,   29,   52,   63,   63,   44,   32,   32,    0,    0,
    0,   51,   29,   29,   52,   30,   30,   56,   63,   55,
    0,    0,   58,   51,   56,   56,    0,    0,    0,    0,
    0,    0,    0,    0,   63,   63,   58,   63,   63,   56,
   63,    0,    0,   59,    0,    0,   33,    0,   34,    0,
    0,    0,    0,    0,    0,   56,   56,    0,   56,   56,
   59,   56,    0,    0,    0,   59,   61,    0,   63,   63,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   58,
    0,   29,   63,   61,    0,    0,   32,    0,   61,   56,
   56,    0,    0,    0,    0,   30,    0,   32,   32,    0,
    0,   58,    0,   56,   59,    0,   30,   30,   58,   58,
    0,   33,    0,    0,   63,   63,    0,    0,   59,    0,
    0,   57,    0,   58,    0,   63,   63,   61,    0,    0,
    0,    0,    0,   33,    0,   56,   56,    0,    0,   58,
   58,   61,   58,   58,    0,   58,   56,   56,   60,   33,
   33,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   59,    0,    0,    0,   60,   32,    0,    0,    0,
   60,   62,    0,   58,   58,   30,    0,    0,    0,    0,
    0,    0,   63,   59,   61,    0,    0,   58,   62,    0,
   59,   59,   44,   62,   63,    0,    0,    0,    0,    0,
    0,    0,    0,   56,    0,   59,   61,    0,    0,   60,
    0,    0,    0,   61,   61,   56,    0,    0,   31,   58,
   58,   59,   59,   60,   59,   59,    0,   59,   61,   33,
   58,   58,   62,    0,    0,    0,    0,    0,    0,    0,
   33,   33,    0,    0,   61,   61,   62,   61,   61,    0,
   61,    0,    0,    0,    0,   59,   59,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   60,    0,    0,   59,
    0,    0,    0,    0,    0,    0,   55,    0,   61,   61,
    0,    0,    0,   31,    0,    0,    0,   58,   60,   62,
    0,    0,   61,   55,    0,   60,   60,    0,   55,   58,
    0,   59,   59,    0,    0,   31,    0,    0,    0,   33,
   60,   62,   59,   59,    0,    0,    0,    0,   62,   62,
    0,   31,   31,    0,   61,   61,   60,   60,    0,   60,
   60,    0,   60,   62,    0,   61,   61,   55,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   62,
   62,   55,   62,   62,    0,   62,    0,    0,    0,    0,
   60,   60,    0,    0,    0,    0,    0,    0,    0,   59,
    0,    0,    0,    0,   60,    0,    0,    0,   57,    0,
   34,   59,    0,   62,   62,    0,    0,    0,    0,    0,
    0,    0,   61,    0,   55,   57,    0,   62,    0,    0,
   57,   31,    0,    0,   61,    0,   60,   60,    0,    0,
    0,    0,   31,   31,    0,    0,   55,   60,   60,    0,
    0,    0,    0,   55,   55,    0,    0,    0,    0,   62,
   62,    0,    0,    0,    0,    0,    0,    0,   55,   57,
   62,   62,    0,    0,    0,   34,    0,    0,    0,   44,
    0,    0,    0,   57,   55,   55,    0,   55,   55,    0,
   55,    0,    0,    0,    0,    0,    0,   34,    0,   44,
   44,   44,    0,    0,   60,    0,   44,    0,    0,    0,
    0,   31,    0,   34,   34,    0,   60,   47,   55,   55,
    0,    0,    0,    0,    0,    0,   57,   62,    0,    0,
    0,    0,   55,    0,    0,    0,    0,   48,   49,   62,
   44,   47,    0,    0,   51,    0,    0,    0,   57,    0,
    0,    0,    0,    0,   44,   57,   57,   47,    0,    0,
    0,   48,   49,    0,   55,   55,    0,    0,   51,    0,
   57,    0,    0,    0,    0,   55,   55,   48,   49,   47,
    0,    0,    0,    0,   51,    0,   57,   57,    0,   57,
   57,    0,   57,   34,    0,    0,   47,   44,    0,   48,
   49,    0,    0,    0,   34,   34,   51,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   48,   49,    0,   44,
   57,   57,    0,   51,    0,    0,   44,   44,    0,    0,
    0,    0,   55,    0,   57,   52,    0,    0,    0,    0,
    0,    0,    0,    0,   55,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   52,
   44,   44,    0,   44,   53,   54,   57,   57,    0,    0,
    0,    0,    0,   34,    0,   52,    0,   57,   57,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   53,   54,
    0,   44,   44,    0,    0,    0,    0,   52,   55,   56,
    0,   57,    0,    0,   53,   54,    0,    0,    0,    0,
    0,    0,    0,    0,   52,    0,    0,    0,    0,    0,
    0,    0,   55,   56,    0,   57,   53,   54,    0,   58,
   59,    0,    0,    0,   57,    0,    0,    0,   55,   56,
    0,   57,    0,   53,   54,    0,   57,    0,   44,   44,
    0,    0,    0,   58,   59,    0,    0,    0,    0,   44,
   55,   56,    0,   57,    0,    0,    0,    0,    0,   58,
   59,    0,    0,    0,    0,    0,  109,   55,   56,    0,
   57,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   58,   59,    0,    0,    0,    0,   60,    0,    0,
  110,    0,    0,    0,    0,   44,    0,    0,   58,   59,
    0,    0,   42,    0,    0,    0,  111,    0,    0,    0,
    0,   60,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   71,    0,   74,   75,   60,  112,   78,
   79,   80,    0,   83,   84,   85,    0,   87,   88,   89,
   90,   91,   92,   93,   94,   95,   96,    0,    0,   60,
    0,    0,    0,    0,    0,    0,    0,    0,  106,  107,
    0,    0,    0,    0,    0,    0,   60,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  126,  127,    0,  129,    0,    0,    0,    0,
    0,    0,    0,    0,  139,    0,  140,
};
short yycheck[] = {                                     258,
    0,  284,  257,    0,  318,    0,  284,  274,  289,    0,
  257,  274,    0,  284,    0,  436,  429,  555,    0,  290,
    0,  318,  277,  278,  279,  277,  278,    0,  318,  284,
  277,  278,  284,  332,    0,  429,    0,  284,  309,  397,
  583,  289,  419,  583,  289,  436,  436,  436,  436,  436,
  290,  368,  318,  318,  429,  368,  583,  436,  435,  436,
  257,  583,  583,  429,  583,  429,  583,  583,  479,  309,
  318,  479,  485,  318,  583,  479,    0,  583,  479,  479,
  277,  278,  436,  436,  578,  578,  472,  284,  431,  516,
  516,  485,    0,  124,  365,  136,   35,   64,   -1,   62,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  375,   -1,   -1,   -1,   -1,  365,   -1,   -1,  375,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  515,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  404,
  405,   -1,   -1,   -1,   -1,   -1,   -1,  404,  405,   -1,
   -1,    0,   -1,   -1,   -1,  436,   -1,   -1,   -1,   -1,
    0,   -1,   -1,   -1,  595,   -1,   -1,   -1,  375,   -1,
  451,   -1,  453,  438,  439,   -1,  441,   -1,  469,    0,
   -1,  438,  439,   -1,  441,   -1,  436,   -1,   -1,   -1,
   -1,  472,  516,   -1,   -1,   -1,   -1,  404,  405,   -1,
    0,  451,   -1,  453,  469,  470,  483,  498,  595,  516,
  483,  469,  469,  470,  469,   -1,  516,  526,  527,   -1,
   -1,   -1,  472,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  438,  439,   -1,  441,   -1,   -1,   -1,   -1,  516,
  498,   -1,   -1,  498,   -1,   -1,   -1,  257,   -1,  537,
   -1,   -1,  533,   -1,   -1,   -1,  257,   -1,  516,  516,
   -1,  516,  469,  470,  274,  257,   -1,  277,  278,  279,
  551,   -1,  537,  274,  284,  537,  277,  278,  279,   -1,
  537,   -1,  274,  533,    0,  277,  278,  279,   -1,   -1,
  583,  274,   -1,   -1,    0,   -1,   -1,  578,  274,   -1,
  274,  551,  583,   -1,   -1,   -1,   -1,   -1,  318,  516,
   -1,  318,   -1,   -1,   -1,   -1,   -1,  318,  583,  588,
   -1,   -1,  332,   -1,   -1,  332,  318,  332,  578,   -1,
  537,  332,   -1,  583,  332,   -1,  332,   -1,   -1,   -1,
  332,   -1,  332,   -1,   -1,   -1,   -1,   -1,   -1,  332,
   -1,   -1,   -1,   -1,   -1,   -1,  332,   -1,  332,   -1,
   -1,   -1,   -1,   -1,   -1,  375,    0,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  375,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  375,   -1,   -1,   -1,  397,   -1,    0,
  397,   -1,  397,   -1,  404,  405,  397,   -1,  332,  397,
   -1,  397,   -1,  404,  405,  397,   -1,   -1,   -1,  419,
   -1,   -1,  404,  405,  397,   -1,   -1,   -1,  419,   -1,
   -1,  397,   -1,  397,   -1,  435,  436,  419,  438,  439,
   -1,  441,   -1,   -1,  435,  436,  257,  438,  439,   -1,
  441,   -1,   -1,  435,  436,   -1,  438,  439,   -1,  441,
   -1,   -1,   -1,  274,   -1,   -1,   -1,  257,  279,  469,
  470,   -1,   -1,  397,   -1,   -1,    0,   -1,  469,  470,
   -1,    0,   -1,  483,  274,   -1,   -1,  469,  470,  279,
   -1,   -1,  483,  332,   -1,  419,   -1,   -1,   -1,   -1,
   -1,  483,  332,   -1,    0,   -1,   -1,  318,   -1,   -1,
  483,  435,  436,   -1,   -1,  515,  516,  483,   -1,  483,
   -1,  332,   -1,   -1,  515,  516,  526,  527,  318,  526,
  527,  526,  527,  515,  516,  526,  527,  537,  526,  527,
  526,  527,  332,  516,  526,  527,  526,  527,   -1,   -1,
  516,  257,  516,  526,  527,   -1,   -1,   -1,  397,   -1,
  526,  527,  526,  527,  375,   -1,   -1,  397,  274,   -1,
   -1,   -1,   -1,  279,   -1,   -1,   -1,   -1,   -1,   -1,
  419,   -1,   -1,  583,   -1,  375,  397,   -1,   -1,  419,
   -1,  515,  583,  404,  405,  595,  435,  436,   -1,   -1,
   -1,  583,  526,  527,  595,  435,  436,  397,  419,    0,
   -1,   -1,  318,  595,  404,  405,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  435,  436,  332,  438,  439,  419,
  441,   -1,   -1,  257,   -1,   -1,  332,   -1,    0,   -1,
   -1,   -1,   -1,   -1,   -1,  435,  436,   -1,  438,  439,
  274,  441,   -1,   -1,   -1,  279,  257,   -1,  469,  470,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  375,
   -1,  595,  483,  274,   -1,   -1,  515,   -1,  279,  469,
  470,   -1,   -1,   -1,   -1,  515,   -1,  526,  527,   -1,
   -1,  397,   -1,  483,  318,   -1,  526,  527,  404,  405,
   -1,  397,   -1,   -1,  515,  516,   -1,   -1,  332,   -1,
   -1,    0,   -1,  419,   -1,  526,  527,  318,   -1,   -1,
   -1,   -1,   -1,  419,   -1,  515,  516,   -1,   -1,  435,
  436,  332,  438,  439,   -1,  441,  526,  527,  257,  435,
  436,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  375,   -1,   -1,   -1,  274,  595,   -1,   -1,   -1,
  279,  257,   -1,  469,  470,  595,   -1,   -1,   -1,   -1,
   -1,   -1,  583,  397,  375,   -1,   -1,  483,  274,   -1,
  404,  405,    0,  279,  595,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  583,   -1,  419,  397,   -1,   -1,  318,
   -1,   -1,   -1,  404,  405,  595,   -1,   -1,  332,  515,
  516,  435,  436,  332,  438,  439,   -1,  441,  419,  515,
  526,  527,  318,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  526,  527,   -1,   -1,  435,  436,  332,  438,  439,   -1,
  441,   -1,   -1,   -1,   -1,  469,  470,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  375,   -1,   -1,  483,
   -1,   -1,   -1,   -1,   -1,   -1,  257,   -1,  469,  470,
   -1,   -1,   -1,  397,   -1,   -1,   -1,  583,  397,  375,
   -1,   -1,  483,  274,   -1,  404,  405,   -1,  279,  595,
   -1,  515,  516,   -1,   -1,  419,   -1,   -1,   -1,  595,
  419,  397,  526,  527,   -1,   -1,   -1,   -1,  404,  405,
   -1,  435,  436,   -1,  515,  516,  435,  436,   -1,  438,
  439,   -1,  441,  419,   -1,  526,  527,  318,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  435,
  436,  332,  438,  439,   -1,  441,   -1,   -1,   -1,   -1,
  469,  470,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  583,
   -1,   -1,   -1,   -1,  483,   -1,   -1,   -1,  257,   -1,
  332,  595,   -1,  469,  470,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  583,   -1,  375,  274,   -1,  483,   -1,   -1,
  279,  515,   -1,   -1,  595,   -1,  515,  516,   -1,   -1,
   -1,   -1,  526,  527,   -1,   -1,  397,  526,  527,   -1,
   -1,   -1,   -1,  404,  405,   -1,   -1,   -1,   -1,  515,
  516,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  419,  318,
  526,  527,   -1,   -1,   -1,  397,   -1,   -1,   -1,  257,
   -1,   -1,   -1,  332,  435,  436,   -1,  438,  439,   -1,
  441,   -1,   -1,   -1,   -1,   -1,   -1,  419,   -1,  277,
  278,  279,   -1,   -1,  583,   -1,  284,   -1,   -1,   -1,
   -1,  595,   -1,  435,  436,   -1,  595,  257,  469,  470,
   -1,   -1,   -1,   -1,   -1,   -1,  375,  583,   -1,   -1,
   -1,   -1,  483,   -1,   -1,   -1,   -1,  277,  278,  595,
  318,  257,   -1,   -1,  284,   -1,   -1,   -1,  397,   -1,
   -1,   -1,   -1,   -1,  332,  404,  405,  257,   -1,   -1,
   -1,  277,  278,   -1,  515,  516,   -1,   -1,  284,   -1,
  419,   -1,   -1,   -1,   -1,  526,  527,  277,  278,  257,
   -1,   -1,   -1,   -1,  284,   -1,  435,  436,   -1,  438,
  439,   -1,  441,  515,   -1,   -1,  257,  375,   -1,  277,
  278,   -1,   -1,   -1,  526,  527,  284,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  277,  278,   -1,  397,
  469,  470,   -1,  284,   -1,   -1,  404,  405,   -1,   -1,
   -1,   -1,  583,   -1,  483,  375,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  595,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  375,
  438,  439,   -1,  441,  404,  405,  515,  516,   -1,   -1,
   -1,   -1,   -1,  595,   -1,  375,   -1,  526,  527,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  404,  405,
   -1,  469,  470,   -1,   -1,   -1,   -1,  375,  438,  439,
   -1,  441,   -1,   -1,  404,  405,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  375,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  438,  439,   -1,  441,  404,  405,   -1,  469,
  470,   -1,   -1,   -1,  583,   -1,   -1,   -1,  438,  439,
   -1,  441,   -1,  404,  405,   -1,  595,   -1,  526,  527,
   -1,   -1,   -1,  469,  470,   -1,   -1,   -1,   -1,  537,
  438,  439,   -1,  441,   -1,   -1,   -1,   -1,   -1,  469,
  470,   -1,   -1,   -1,   -1,   -1,  516,  438,  439,   -1,
  441,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  469,  470,   -1,   -1,   -1,   -1,  537,   -1,   -1,
  516,   -1,   -1,   -1,   -1,  583,   -1,   -1,  469,  470,
   -1,   -1,   16,   -1,   -1,   -1,  516,   -1,   -1,   -1,
   -1,  537,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   37,   -1,   39,   40,  537,  516,   43,
   44,   45,   -1,   47,   48,   49,   -1,   51,   52,   53,
   54,   55,   56,   57,   58,   59,   60,   -1,   -1,  537,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   72,   73,
   -1,   -1,   -1,   -1,   -1,   -1,  537,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  116,  117,   -1,  119,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  128,   -1,  130,
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
"SelectStatement : SelectStatement Tables",
"SelectStatement : SelectStatement Tables Conditions",
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
#line 961 "sqlparser.y"

#line 1053 "y.tab.c"
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
#line 638 "sqlparser.y"
{
	kdDebug() << "from detail" << endl;
}
break;
case 23:
#line 642 "sqlparser.y"
{
	kdDebug() << "from detail (conditioned)" << endl;
}
break;
case 24:
#line 649 "sqlparser.y"
{
	kdDebug() << "SELECT" << endl;
	parser->createSelect();
	parser->setOperation(KexiDB::Parser::OP_Select);
}
break;
case 25:
#line 658 "sqlparser.y"
{
	kdDebug() << "WHERE " << yyvsp[0].field << endl;
}
break;
case 26:
#line 662 "sqlparser.y"
{
	kdDebug() << "AND " << yyvsp[0].field << endl;
}
break;
case 27:
#line 666 "sqlparser.y"
{
	kdDebug() << "OR " << yyvsp[0].field << endl;
}
break;
case 28:
#line 670 "sqlparser.y"
{
	kdDebug() << "()" << endl;
}
break;
case 29:
#line 677 "sqlparser.y"
{
}
break;
case 30:
#line 680 "sqlparser.y"
{
	kdDebug() << "LEFT JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
break;
case 31:
#line 685 "sqlparser.y"
{
	kdDebug() << "LEFT OUTER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
break;
case 32:
#line 690 "sqlparser.y"
{
	kdDebug() << "INNER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
break;
case 33:
#line 695 "sqlparser.y"
{
	kdDebug() << "RIGHT JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
break;
case 34:
#line 700 "sqlparser.y"
{
	kdDebug() << "RIGHT OUTER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].field << endl;
	addTable(yyvsp[-2].stringValue);
}
break;
case 36:
#line 708 "sqlparser.y"
{
}
break;
case 37:
#line 714 "sqlparser.y"
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
case 39:
#line 749 "sqlparser.y"
{
}
break;
case 40:
#line 755 "sqlparser.y"
{
	kdDebug() << " adding field '" << yyvsp[0].field->name() << "'" << endl;
	parser->select()->addField(yyvsp[0].field);
/*	parser->fieldList()->append($1);*/
}
break;
case 41:
#line 761 "sqlparser.y"
{
}
break;
case 42:
#line 764 "sqlparser.y"
{
	kdDebug() << " adding field '" << yyvsp[-2].field->name() << "' as '" << yyvsp[0].stringValue << "'" << endl;
/*	parser->fieldList()->append($1);*/
	parser->select()->addField(yyvsp[-2].field);
	parser->select()->setAlias(yyvsp[-2].field, yyvsp[0].stringValue);
}
break;
case 43:
#line 771 "sqlparser.y"
{
	kdDebug() << " adding field '" << yyvsp[-1].field->name() << "' as '" << yyvsp[0].stringValue << "'" << endl;
/*	parser->fieldList()->append($1);*/
	parser->select()->addField(yyvsp[-1].field);
	parser->select()->setAlias(yyvsp[-1].field, yyvsp[0].stringValue);
}
break;
case 44:
#line 781 "sqlparser.y"
{
	kdDebug() << "  + col " << yyvsp[0].stringValue << endl;
	yyval.field = new KexiDB::Field();
	yyval.field->setName(yyvsp[0].stringValue);
	yyval.field->setTable(dummy);
/*	parser->select()->addField(field);*/
	requiresTable = true;
}
break;
case 45:
#line 790 "sqlparser.y"
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
case 46:
#line 800 "sqlparser.y"
{
	yyval.field = new KexiDB::Field();
	yyval.field->setName(QString::null);
}
break;
case 47:
#line 805 "sqlparser.y"
{
	yyval.field = new KexiDB::Field();
	yyval.field->setName(yyvsp[0].stringValue);
/*	parser->select()->addField(field);*/
	kdDebug() << "  + constant " << yyvsp[0].stringValue << endl;
}
break;
case 48:
#line 812 "sqlparser.y"
{
	yyval.field = new KexiDB::Field();
	yyval.field->setName(QString::number(yyvsp[0].integerValue));
/*	parser->select()->addField(field);*/
	kdDebug() << "  + numerical constant " << yyvsp[0].integerValue << endl;
}
break;
case 49:
#line 819 "sqlparser.y"
{
	yyval.field = new KexiDB::Field();
	yyval.field->setName(QString::number(yyvsp[0].integerValue));
/*	parser->select()->addField(field);*/
	kdDebug() << "  + numerical constant " << yyvsp[0].integerValue << endl;
}
break;
case 50:
#line 829 "sqlparser.y"
{
	yyval.field = yyvsp[0].field;
	kdDebug() << "to expression: " << yyval.field->name() << endl;
}
break;
case 51:
#line 834 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " + " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " + " + yyvsp[0].field->name());
}
break;
case 52:
#line 839 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " - " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " - " + yyvsp[0].field->name());
}
break;
case 53:
#line 844 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " / " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " / " + yyvsp[0].field->name());
}
break;
case 54:
#line 849 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " * " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " * " + yyvsp[0].field->name());
}
break;
case 55:
#line 854 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " NOT " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " NOT " + yyvsp[0].field->name());
}
break;
case 56:
#line 859 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " = " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " = " + yyvsp[0].field->name());
}
break;
case 57:
#line 864 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " <> " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " <> " + yyvsp[0].field->name());
}
break;
case 58:
#line 869 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " > " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " > " + yyvsp[0].field->name());
}
break;
case 59:
#line 874 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " >= " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " >= " + yyvsp[0].field->name());
}
break;
case 60:
#line 879 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " < " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " < " + yyvsp[0].field->name());
}
break;
case 61:
#line 884 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " <= " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " <= " + yyvsp[0].field->name());
}
break;
case 62:
#line 889 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " LIKE " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " LIKE " + yyvsp[0].field->name());
}
break;
case 63:
#line 894 "sqlparser.y"
{
	kdDebug() << yyvsp[-2].field->name() << " % " << yyvsp[0].field->name() << endl;
	yyval.field->setName(yyvsp[-2].field->name() + " % " + yyvsp[0].field->name());
}
break;
case 64:
#line 899 "sqlparser.y"
{
	kdDebug() << "(" << yyvsp[-1].field->name() << ")" << endl;
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("(" + yyvsp[-1].field->name() + ")");
}
break;
case 65:
#line 905 "sqlparser.y"
{
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("SUM(" + yyvsp[-1].field->name() + ")");
/*wait	$$->containsGroupingAggregate(true);*/
/*wait	parser->select()->grouped(true);*/
}
break;
case 66:
#line 912 "sqlparser.y"
{
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("MIN(" + yyvsp[-1].field->name() + ")");
/*wait	$$->containsGroupingAggregate(true);*/
/*wait	parser->select()->grouped(true);*/
}
break;
case 67:
#line 919 "sqlparser.y"
{
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("MAX(" + yyvsp[-1].field->name() + ")");
/*wait	$$->containsGroupingAggregate(true);*/
/*wait	parser->select()->grouped(true);*/
}
break;
case 68:
#line 926 "sqlparser.y"
{
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("AVG(" + yyvsp[-1].field->name() + ")");
/*wait	$$->containsGroupingAggregate(true);*/
/*wait	parser->select()->grouped(true);*/
}
break;
case 69:
#line 933 "sqlparser.y"
{
	yyval.field = yyvsp[-1].field;
	yyval.field->setName("DISTINCT(" + yyvsp[-1].field->name() + ")");
}
break;
case 70:
#line 941 "sqlparser.y"
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
case 71:
#line 951 "sqlparser.y"
{
	kdDebug() << "  + all columns from " << yyvsp[-2].stringValue << endl;
	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), parser->db()->tableSchema(yyvsp[-2].stringValue));
	parser->select()->addAsterisk(ast);
/*	fieldList.append(ast);*/
	requiresTable = true;
}
break;
#line 1710 "y.tab.c"
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
