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
     AS = 277,
     ASIN = 278,
     ASC = 279,
     ASCII = 280,
     ASSERTION = 281,
     ATAN = 282,
     ATAN2 = 283,
     AUTHORIZATION = 284,
     AUTO_INCREMENT = 285,
     AVG = 286,
     BEFORE = 287,
     SQL_BEGIN = 288,
     BETWEEN = 289,
     BIGINT = 290,
     BINARY = 291,
     BIT = 292,
     BIT_LENGTH = 293,
     BITWISE_SHIFT_LEFT = 294,
     BITWISE_SHIFT_RIGHT = 295,
     BREAK = 296,
     BY = 297,
     CASCADE = 298,
     CASCADED = 299,
     CASE = 300,
     CAST = 301,
     CATALOG = 302,
     CEILING = 303,
     CENTER = 304,
     SQL_CHAR = 305,
     CHAR_LENGTH = 306,
     CHARACTER_STRING_LITERAL = 307,
     CHECK = 308,
     CLOSE = 309,
     COALESCE = 310,
     COBOL = 311,
     COLLATE = 312,
     COLLATION = 313,
     COLUMN = 314,
     COMMIT = 315,
     COMPUTE = 316,
     CONCAT = 317,
     CONCATENATION = 318,
     CONNECT = 319,
     CONNECTION = 320,
     CONSTRAINT = 321,
     CONSTRAINTS = 322,
     CONTINUE = 323,
     CONVERT = 324,
     CORRESPONDING = 325,
     COS = 326,
     COT = 327,
     COUNT = 328,
     CREATE = 329,
     CURDATE = 330,
     CURRENT = 331,
     CURRENT_DATE = 332,
     CURRENT_TIME = 333,
     CURRENT_TIMESTAMP = 334,
     CURTIME = 335,
     CURSOR = 336,
     DATABASE = 337,
     SQL_DATE = 338,
     DATE_FORMAT = 339,
     DATE_REMAINDER = 340,
     DATE_VALUE = 341,
     DAY = 342,
     DAYOFMONTH = 343,
     DAYOFWEEK = 344,
     DAYOFYEAR = 345,
     DAYS_BETWEEN = 346,
     DEALLOCATE = 347,
     DEC = 348,
     DECLARE = 349,
     DEFAULT = 350,
     DEFERRABLE = 351,
     DEFERRED = 352,
     SQL_DELETE = 353,
     DESC = 354,
     DESCRIBE = 355,
     DESCRIPTOR = 356,
     DIAGNOSTICS = 357,
     DICTIONARY = 358,
     DIRECTORY = 359,
     DISCONNECT = 360,
     DISPLACEMENT = 361,
     DISTINCT = 362,
     DOMAIN_TOKEN = 363,
     SQL_DOUBLE = 364,
     DOUBLE_QUOTED_STRING = 365,
     DROP = 366,
     ELSE = 367,
     END = 368,
     END_EXEC = 369,
     EQUAL = 370,
     ESCAPE = 371,
     EXCEPT = 372,
     SQL_EXCEPTION = 373,
     EXEC = 374,
     EXECUTE = 375,
     EXISTS = 376,
     EXP = 377,
     EXPONENT = 378,
     EXTERNAL = 379,
     EXTRACT = 380,
     SQL_FALSE = 381,
     FETCH = 382,
     FIRST = 383,
     SQL_FLOAT = 384,
     FLOOR = 385,
     FN = 386,
     FOR = 387,
     FOREIGN = 388,
     FORTRAN = 389,
     FOUND = 390,
     FOUR_DIGITS = 391,
     FROM = 392,
     FULL = 393,
     GET = 394,
     GLOBAL = 395,
     GO = 396,
     GOTO = 397,
     GRANT = 398,
     GREATER_OR_EQUAL = 399,
     GREATER_THAN = 400,
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
     LESS_THAN = 432,
     LEVEL = 433,
     LIKE = 434,
     LINE_WIDTH = 435,
     LOCAL = 436,
     LOCATE = 437,
     LOG = 438,
     SQL_LONG = 439,
     LOWER = 440,
     LTRIM = 441,
     LTRIP = 442,
     MATCH = 443,
     SQL_MAX = 444,
     MICROSOFT = 445,
     SQL_MIN = 446,
     MINUS = 447,
     MINUTE = 448,
     MINUTES_BETWEEN = 449,
     MOD = 450,
     MODIFY = 451,
     MODULE = 452,
     MONTH = 453,
     MONTHS_BETWEEN = 454,
     MUMPS = 455,
     NAMES = 456,
     NATIONAL = 457,
     NCHAR = 458,
     NEXT = 459,
     NODUP = 460,
     NONE = 461,
     NOT = 462,
     NOT_EQUAL = 463,
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
     SIMILAR = 526,
     SIMILAR_TO = 527,
     NOT_SIMILAR_TO = 528,
     INTEGER_CONST = 529,
     REAL_CONST = 530,
     SIN = 531,
     SQL_SIZE = 532,
     SMALLINT = 533,
     SOME = 534,
     SPACE = 535,
     SQL = 536,
     SQL_TRUE = 537,
     SQLCA = 538,
     SQLCODE = 539,
     SQLERROR = 540,
     SQLSTATE = 541,
     SQLWARNING = 542,
     SQRT = 543,
     STDEV = 544,
     SUBSTRING = 545,
     SUM = 546,
     SYSDATE = 547,
     SYSDATE_FORMAT = 548,
     SYSTEM = 549,
     TABLE = 550,
     TAN = 551,
     TEMPORARY = 552,
     THEN = 553,
     THREE_DIGITS = 554,
     TIME = 555,
     TIMESTAMP = 556,
     TIMEZONE_HOUR = 557,
     TIMEZONE_MINUTE = 558,
     TINYINT = 559,
     TO = 560,
     TO_CHAR = 561,
     TO_DATE = 562,
     TRANSACTION = 563,
     TRANSLATE = 564,
     TRANSLATION = 565,
     TRUNCATE = 566,
     GENERAL_TITLE = 567,
     TWO_DIGITS = 568,
     UCASE = 569,
     UNION = 570,
     UNIQUE = 571,
     SQL_UNKNOWN = 572,
     UPDATE = 573,
     UPPER = 574,
     USAGE = 575,
     USER = 576,
     IDENTIFIER = 577,
     IDENTIFIER_DOT_ASTERISK = 578,
     ERROR_DIGIT_BEFORE_IDENTIFIER = 579,
     USING = 580,
     VALUE = 581,
     VALUES = 582,
     VARBINARY = 583,
     VARCHAR = 584,
     VARYING = 585,
     VENDOR = 586,
     VIEW = 587,
     WEEK = 588,
     WHEN = 589,
     WHENEVER = 590,
     WHERE = 591,
     WHERE_CURRENT_OF = 592,
     WITH = 593,
     WORD_WRAPPED = 594,
     WORK = 595,
     WRAPPED = 596,
     XOR = 597,
     YEAR = 598,
     YEARS_BETWEEN = 599,
     __LAST_TOKEN = 600,
     ILIKE = 601
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
#define AS 277
#define ASIN 278
#define ASC 279
#define ASCII 280
#define ASSERTION 281
#define ATAN 282
#define ATAN2 283
#define AUTHORIZATION 284
#define AUTO_INCREMENT 285
#define AVG 286
#define BEFORE 287
#define SQL_BEGIN 288
#define BETWEEN 289
#define BIGINT 290
#define BINARY 291
#define BIT 292
#define BIT_LENGTH 293
#define BITWISE_SHIFT_LEFT 294
#define BITWISE_SHIFT_RIGHT 295
#define BREAK 296
#define BY 297
#define CASCADE 298
#define CASCADED 299
#define CASE 300
#define CAST 301
#define CATALOG 302
#define CEILING 303
#define CENTER 304
#define SQL_CHAR 305
#define CHAR_LENGTH 306
#define CHARACTER_STRING_LITERAL 307
#define CHECK 308
#define CLOSE 309
#define COALESCE 310
#define COBOL 311
#define COLLATE 312
#define COLLATION 313
#define COLUMN 314
#define COMMIT 315
#define COMPUTE 316
#define CONCAT 317
#define CONCATENATION 318
#define CONNECT 319
#define CONNECTION 320
#define CONSTRAINT 321
#define CONSTRAINTS 322
#define CONTINUE 323
#define CONVERT 324
#define CORRESPONDING 325
#define COS 326
#define COT 327
#define COUNT 328
#define CREATE 329
#define CURDATE 330
#define CURRENT 331
#define CURRENT_DATE 332
#define CURRENT_TIME 333
#define CURRENT_TIMESTAMP 334
#define CURTIME 335
#define CURSOR 336
#define DATABASE 337
#define SQL_DATE 338
#define DATE_FORMAT 339
#define DATE_REMAINDER 340
#define DATE_VALUE 341
#define DAY 342
#define DAYOFMONTH 343
#define DAYOFWEEK 344
#define DAYOFYEAR 345
#define DAYS_BETWEEN 346
#define DEALLOCATE 347
#define DEC 348
#define DECLARE 349
#define DEFAULT 350
#define DEFERRABLE 351
#define DEFERRED 352
#define SQL_DELETE 353
#define DESC 354
#define DESCRIBE 355
#define DESCRIPTOR 356
#define DIAGNOSTICS 357
#define DICTIONARY 358
#define DIRECTORY 359
#define DISCONNECT 360
#define DISPLACEMENT 361
#define DISTINCT 362
#define DOMAIN_TOKEN 363
#define SQL_DOUBLE 364
#define DOUBLE_QUOTED_STRING 365
#define DROP 366
#define ELSE 367
#define END 368
#define END_EXEC 369
#define EQUAL 370
#define ESCAPE 371
#define EXCEPT 372
#define SQL_EXCEPTION 373
#define EXEC 374
#define EXECUTE 375
#define EXISTS 376
#define EXP 377
#define EXPONENT 378
#define EXTERNAL 379
#define EXTRACT 380
#define SQL_FALSE 381
#define FETCH 382
#define FIRST 383
#define SQL_FLOAT 384
#define FLOOR 385
#define FN 386
#define FOR 387
#define FOREIGN 388
#define FORTRAN 389
#define FOUND 390
#define FOUR_DIGITS 391
#define FROM 392
#define FULL 393
#define GET 394
#define GLOBAL 395
#define GO 396
#define GOTO 397
#define GRANT 398
#define GREATER_OR_EQUAL 399
#define GREATER_THAN 400
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
#define LESS_THAN 432
#define LEVEL 433
#define LIKE 434
#define LINE_WIDTH 435
#define LOCAL 436
#define LOCATE 437
#define LOG 438
#define SQL_LONG 439
#define LOWER 440
#define LTRIM 441
#define LTRIP 442
#define MATCH 443
#define SQL_MAX 444
#define MICROSOFT 445
#define SQL_MIN 446
#define MINUS 447
#define MINUTE 448
#define MINUTES_BETWEEN 449
#define MOD 450
#define MODIFY 451
#define MODULE 452
#define MONTH 453
#define MONTHS_BETWEEN 454
#define MUMPS 455
#define NAMES 456
#define NATIONAL 457
#define NCHAR 458
#define NEXT 459
#define NODUP 460
#define NONE 461
#define NOT 462
#define NOT_EQUAL 463
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
#define SIMILAR 526
#define SIMILAR_TO 527
#define NOT_SIMILAR_TO 528
#define INTEGER_CONST 529
#define REAL_CONST 530
#define SIN 531
#define SQL_SIZE 532
#define SMALLINT 533
#define SOME 534
#define SPACE 535
#define SQL 536
#define SQL_TRUE 537
#define SQLCA 538
#define SQLCODE 539
#define SQLERROR 540
#define SQLSTATE 541
#define SQLWARNING 542
#define SQRT 543
#define STDEV 544
#define SUBSTRING 545
#define SUM 546
#define SYSDATE 547
#define SYSDATE_FORMAT 548
#define SYSTEM 549
#define TABLE 550
#define TAN 551
#define TEMPORARY 552
#define THEN 553
#define THREE_DIGITS 554
#define TIME 555
#define TIMESTAMP 556
#define TIMEZONE_HOUR 557
#define TIMEZONE_MINUTE 558
#define TINYINT 559
#define TO 560
#define TO_CHAR 561
#define TO_DATE 562
#define TRANSACTION 563
#define TRANSLATE 564
#define TRANSLATION 565
#define TRUNCATE 566
#define GENERAL_TITLE 567
#define TWO_DIGITS 568
#define UCASE 569
#define UNION 570
#define UNIQUE 571
#define SQL_UNKNOWN 572
#define UPDATE 573
#define UPPER 574
#define USAGE 575
#define USER 576
#define IDENTIFIER 577
#define IDENTIFIER_DOT_ASTERISK 578
#define ERROR_DIGIT_BEFORE_IDENTIFIER 579
#define USING 580
#define VALUE 581
#define VALUES 582
#define VARBINARY 583
#define VARCHAR 584
#define VARYING 585
#define VENDOR 586
#define VIEW 587
#define WEEK 588
#define WHEN 589
#define WHENEVER 590
#define WHERE 591
#define WHERE_CURRENT_OF 592
#define WITH 593
#define WORD_WRAPPED 594
#define WORK 595
#define WRAPPED 596
#define XOR 597
#define YEAR 598
#define YEARS_BETWEEN 599
#define __LAST_TOKEN 600
#define ILIKE 601




/* Copy the first part of user declarations.  */
#line 423 "sqlparser.y"

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

//	using namespace std;
using namespace KexiDB;

#define YY_NO_UNPUT
#define YYSTACK_USE_ALLOCA 1
#define YYMAXDEPTH 255

Parser *parser;
Field *field;
bool requiresTable;
QPtrList<Field> fieldList;
//	QPtrList<TableSchema> tableList;
//	QDict<TableSchema> tableDict;
//	TableSchema *dummy = 0;
int current = 0;
QString ctoken = "";

int yyparse();
int yylex();
void tokenize(const char *data);

void yyerror(const char *str)
{
	kdDebug() << "error: " << str << endl;
	kdDebug() << "at character " << current << " near tooken " << ctoken << endl;
	parser->setOperation(Parser::OP_Error);

	if (parser->error().type().isEmpty() 
		&& (strlen(str)==0 
		|| qstrnicmp(str, "syntax error", 12)==0 || qstrnicmp(str, "parse error", 11)==0))
	{
		kdDebug() << parser->statement() << endl;
		QString ptrline = "";
		for(int i=0; i < current; i++)
			ptrline += " ";

		ptrline += "^";

		kdDebug() << ptrline << endl;

		//lexer may add error messages
		QString lexerErr = parser->error().error();

		if (lexerErr.isEmpty()) {
			if (qstrnicmp(str, "parse error, expecting `IDENTIFIER'", 35)==0)
				lexerErr = i18n("identifier was expected");
		}
		if (!lexerErr.isEmpty())
			lexerErr.prepend(": ");

		if (parser->isReservedKeyword(ctoken.latin1()))
			parser->setError( ParserError(i18n("Syntax Error"), i18n("\"%1\" is a reserved keyword").arg(ctoken)+lexerErr, ctoken, current) );
		else
			parser->setError( ParserError(i18n("Syntax Error"), i18n("Syntax Error near \"%1\"").arg(ctoken)+lexerErr, ctoken, current) );
	}
}

void setError(const QString& errName, const QString& errDesc)
{
	parser->setError( ParserError(errName, errDesc, ctoken, current) );
	yyerror(errName.latin1());
}

void setError(const QString& errDesc)
{
	setError("", errDesc);
}

/*void tableNotFoundError(const QString& tableName)
{
	setError( i18n("Table not found"), i18n("Unknown table \"%1\"").arg(tableName) );
}*/

/* this is better than assert() */
#define IMPL_ERROR(errmsg) setError("Implementation error", errmsg)

bool parseData(Parser *p, const char *data)
{
/* todo: remove dummy */
//		if (!dummy)
			//dummy = new TableSchema();
/* todo: make this REENTRANT */
	parser = p;
	parser->clear();
	field = 0;
	fieldList.clear();
	requiresTable = false;

	if (!data) {
		ParserError err(i18n("Error"), i18n("No query specified"), ctoken, current);
		parser->setError(err);
		yyerror("");
		parser = 0;
		return false;
	}

	tokenize(data);
	if (!parser->error().type().isEmpty()) {
		parser = 0;
		return false;
	}
	yyparse();

	bool ok = true;
	if(parser->operation() == Parser::OP_Select)
	{
		kdDebug() << "parseData(): ok" << endl;
//			kdDebug() << "parseData(): " << tableDict.count() << " loaded tables" << endl;
/*			TableSchema *ts;
			for(QDictIterator<TableSchema> it(tableDict); TableSchema *s = tableList.first(); s; s = tableList.next())
			{
				kdDebug() << "  " << s->name() << endl;
			}*/
/*removed
			Field::ListIterator it = parser->select()->fieldsIterator();
			for(Field *item; (item = it.current()); ++it)
			{
				if(tableList.findRef(item->table()) == -1)
				{
					ParserError err(i18n("Field List Error"), i18n("Unknown table '%1' in field list").arg(item->table()->name()), ctoken, current);
					parser->setError(err);

					yyerror("fieldlisterror");
					ok = false;
				}
			}*/
			//take the dummy table out of the query
//			parser->select()->removeTable(dummy);
	}
	else {
		ok = false;
	}

//		tableDict.clear();
	parser = 0;
	return ok;
}

	
/* Adds \a column to \a querySchema. \a column can be in a form of
 table.field, tableAlias.field or field
*/
bool addColumn( ParseInfo& parseInfo, BaseExpr* columnExpr )
{
	if (!columnExpr->validate(parseInfo)) {
		setError(parseInfo.errMsg, parseInfo.errDescr);
		return false;
	}

	VariableExpr *v_e = dynamic_cast<VariableExpr*>(columnExpr);
	if (columnExpr->exprClass() == KexiDBExpr_Variable && v_e) {
		//it's a variable:
		if (v_e->name=="*") {//all tables asterisk
			parseInfo.querySchema->addAsterisk( new QueryAsterisk(parseInfo.querySchema) );
		}
		else if (v_e->tableForQueryAsterisk) {//one-table asterisk
			parseInfo.querySchema->addAsterisk( 
				new QueryAsterisk(parseInfo.querySchema, v_e->tableForQueryAsterisk) );
		}
		else if (v_e->field) {//"table.field" or "field" (bound to a table or not)
			parseInfo.querySchema->addField(v_e->field, v_e->tablePositionForField);
		}
		else {
			IMPL_ERROR("addColumn(): unknown case!");
			return false;
		}
		return true;
	}

	//it's complex expression
	Field *field = new Field(parseInfo.querySchema, columnExpr);
	parseInfo.querySchema->addField(field);

#if 0
	kdDebug() << "found variable name: " << varName << endl;
	int dotPos = varName.find('.');
	QString tableName, fieldName;
//TODO: shall we also support db name?
	if (dotPos>0) {
		tableName = varName.left(dotPos);
		fieldName = varName.mid(dotPos+1);
	}
	if (tableName.isEmpty()) {//fieldname only
		fieldName = varName;
		if (fieldName=="*") {
			parseInfo.querySchema->addAsterisk( new QueryAsterisk(parseInfo.querySchema) );
		}
		else {
			//find first table that has this field
			Field *firstField = 0;
			for (TableSchema::ListIterator it(*parseInfo.querySchema->tables()); it.current(); ++it) {
				Field *f = it.current()->field(fieldName);
				if (f) {
					if (!firstField) {
						firstField = f;
					} else if (f->table()!=firstField->table()) {
						//ambiguous field name
						setError(i18n("Ambiguous field name"), 
							i18n("Both table \"%1\" and \"%2\" have defined \"%3\" field. "
								"Use \"<tableName>.%4\" notation to specify table name.")
								.arg(firstField->table()->name()).arg(f->table()->name())
								.arg(fieldName).arg(fieldName));
						return false;
					}
				}
			}
			if (!firstField) {
					setError(i18n("Field not found"), 
						i18n("Table containing \"%1\" field not found").arg(fieldName));
					return false;
			}
			//ok
			parseInfo.querySchema->addField(firstField);
		}
	}
	else {//table.fieldname or tableAlias.fieldname
		tableName = tableName.lower();
		TableSchema *ts = parseInfo.querySchema->table( tableName );
		if (ts) {//table.fieldname
			//check if "table" is covered by an alias
			const QValueList<int> tPositions = parseInfo.querySchema->tablePositions(tableName);
			QValueList<int>::ConstIterator it = tPositions.begin();
			QCString tableAlias;
			bool covered = true;
			for (; it!=tPositions.end() && covered; ++it) {
				tableAlias = parseInfo.querySchema->tableAlias(*it);
				if (tableAlias.isEmpty() || tableAlias.lower()==tableName.latin1())
					covered = false; //uncovered
				kdDebug() << " --" << "covered by " << tableAlias << " alias" << endl;
			}
			if (covered) {
				setError(i18n("Could not access the table directly using its name"), 
					i18n("Table \"%1\" is covered by aliases. Instead of \"%2\", "
					"you can write \"%3\"").arg(tableName)
					.arg(tableName+"."+fieldName).arg(tableAlias+"."+fieldName.latin1()));
				return false;
			}
		}
		
		int tablePosition = -1;
		if (!ts) {//try to find tableAlias
			tablePosition = parseInfo.querySchema->tablePositionForAlias( tableName.latin1() );
			if (tablePosition>=0) {
				ts = parseInfo.querySchema->tables()->at(tablePosition);
				if (ts)
					kdDebug() << " --it's a tableAlias.name" << endl;
			}
		}


		if (ts) {
			QValueList<int> *positionsList = repeatedTablesAndAliases[ tableName ];
			if (!positionsList) {
				IMPL_ERROR(tableName + "." + fieldName + ", !positionsList ");
				return false;
			}

			if (fieldName=="*") {
				if (positionsList->count()>1) {
					setError(i18n("Ambiguous \"%1.*\" expression").arg(tableName),
						i18n("More than one \"%1\" table or alias defined").arg(tableName));
					return false;
				}
				parseInfo.querySchema->addAsterisk( new QueryAsterisk(parseInfo.querySchema, ts) );
			}
			else {
				kdDebug() << " --it's a table.name" << endl;
				Field *realField = ts->field(fieldName);
				if (realField) {
					// check if table or alias is used twice and both have the same column
					// (so the column is ambiguous)
					int numberOfTheSameFields = 0;
					for (QValueList<int>::iterator it = positionsList->begin();
						it!=positionsList->end();++it)
					{
						TableSchema *otherTS = parseInfo.querySchema->tables()->at(*it);
						if (otherTS->field(fieldName))
							numberOfTheSameFields++;
						if (numberOfTheSameFields>1) {
							setError(i18n("Ambiguous \"%1.%2\" expression").arg(tableName).arg(fieldName),
								i18n("More than one \"%1\" table or alias defined containing \"%2\" field").arg(tableName).arg(fieldName));
							return false;
						}
					}

					parseInfo.querySchema->addField(realField, tablePosition);
				}
				else {
					setError(i18n("Field not found"), i18n("Table \"%1\" has no \"%2\" field")
						.arg(tableName).arg(fieldName));
					return false;
				}
			}
		}
		else {
			tableNotFoundError(tableName);
			return false;
		}
	}
#endif
	return true;
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

#ifndef YYSTYPE
#line 771 "sqlparser.y"
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
#line 1125 "sqlparser.tab.c"
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
#line 1146 "sqlparser.tab.c"

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
#define YYLAST   331

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  368
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  31
/* YYNRULES -- Number of rules. */
#define YYNRULES  90
/* YYNRULES -- Number of states. */
#define YYNSTATES  152

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   601

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned short yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   354,   349,   365,   358,
     355,   356,   348,   347,   352,   346,   353,   359,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   351,
       2,   360,     2,   357,   350,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   363,     2,   364,   362,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   366,     2,   367,     2,     2,     2,
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
     345,   361
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     9,    11,    14,    16,    18,    19,
      27,    31,    33,    36,    40,    43,    45,    48,    51,    53,
      55,    60,    65,    66,    69,    73,    76,    80,    85,    87,
      90,    92,    96,   100,   104,   106,   110,   114,   116,   120,
     124,   128,   132,   134,   138,   142,   146,   148,   152,   156,
     160,   164,   166,   170,   174,   178,   183,   189,   191,   195,
     200,   202,   205,   208,   211,   214,   216,   221,   225,   227,
     229,   231,   233,   237,   241,   242,   245,   249,   251,   253,
     256,   260,   264,   266,   268,   270,   274,   277,   279,   284,
     286
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     369,     0,    -1,   370,    -1,   371,   351,   370,    -1,   371,
      -1,   371,   351,    -1,   372,    -1,   379,    -1,    -1,    74,
     295,   322,   373,   355,   374,   356,    -1,   374,   352,   375,
      -1,   375,    -1,   322,   378,    -1,   322,   378,   376,    -1,
     376,   377,    -1,   377,    -1,   239,   170,    -1,   207,   210,
      -1,    30,    -1,     3,    -1,     3,   355,   274,   356,    -1,
     329,   355,   274,   356,    -1,    -1,   380,   395,    -1,   380,
     395,   392,    -1,   380,   392,    -1,   380,   395,   381,    -1,
     380,   395,   392,   381,    -1,   265,    -1,   336,   382,    -1,
     383,    -1,   384,    19,   383,    -1,   384,   224,   383,    -1,
     384,   342,   383,    -1,   384,    -1,   385,    39,   384,    -1,
     385,    40,   384,    -1,   385,    -1,   386,   347,   385,    -1,
     386,   346,   385,    -1,   386,   365,   385,    -1,   386,   366,
     385,    -1,   386,    -1,   387,   359,   386,    -1,   387,   348,
     386,    -1,   387,   349,   386,    -1,   387,    -1,   388,   145,
     387,    -1,   388,   144,   387,    -1,   388,   177,   387,    -1,
     388,   176,   387,    -1,   388,    -1,   389,   208,   388,    -1,
     389,   179,   388,    -1,   389,   153,   388,    -1,   389,   271,
     305,   388,    -1,   389,   207,   271,   305,   388,    -1,   389,
      -1,   389,   211,   210,    -1,   389,   211,   207,   210,    -1,
     390,    -1,   346,   390,    -1,   347,   390,    -1,   367,   390,
      -1,   207,   390,    -1,   322,    -1,   322,   355,   391,   356,
      -1,   322,   353,   322,    -1,   210,    -1,    52,    -1,   274,
      -1,   275,    -1,   355,   382,   356,    -1,   391,   352,   382,
      -1,    -1,   137,   393,    -1,   393,   352,   394,    -1,   394,
      -1,   322,    -1,   322,   322,    -1,   322,    22,   322,    -1,
     395,   352,   396,    -1,   396,    -1,   397,    -1,   398,    -1,
     397,    22,   322,    -1,   397,   322,    -1,   382,    -1,   107,
     355,   397,   356,    -1,   348,    -1,   322,   353,   348,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   833,   833,   843,   848,   849,   858,   863,   871,   869,
     878,   879,   884,   894,   908,   909,   914,   920,   925,   932,
     938,   945,   951,   959,   980,  1152,  1156,  1160,  1166,  1176,
    1187,  1192,  1198,  1202,  1206,  1211,  1216,  1220,  1225,  1231,
    1235,  1239,  1243,  1248,  1253,  1257,  1261,  1266,  1271,  1275,
    1279,  1283,  1288,  1293,  1297,  1301,  1305,  1309,  1314,  1319,
    1323,  1328,  1334,  1338,  1342,  1346,  1359,  1365,  1376,  1383,
    1391,  1399,  1404,  1411,  1417,  1423,  1464,  1470,  1477,  1513,
    1522,  1535,  1542,  1550,  1560,  1565,  1577,  1591,  1630,  1638,
    1648
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
  "AND", "ANY", "ARE", "AS", "ASIN", "ASC", "ASCII", "ASSERTION", "ATAN", 
  "ATAN2", "AUTHORIZATION", "AUTO_INCREMENT", "AVG", "BEFORE", 
  "SQL_BEGIN", "BETWEEN", "BIGINT", "BINARY", "BIT", "BIT_LENGTH", 
  "BITWISE_SHIFT_LEFT", "BITWISE_SHIFT_RIGHT", "BREAK", "BY", "CASCADE", 
  "CASCADED", "CASE", "CAST", "CATALOG", "CEILING", "CENTER", "SQL_CHAR", 
  "CHAR_LENGTH", "CHARACTER_STRING_LITERAL", "CHECK", "CLOSE", "COALESCE", 
  "COBOL", "COLLATE", "COLLATION", "COLUMN", "COMMIT", "COMPUTE", 
  "CONCAT", "CONCATENATION", "CONNECT", "CONNECTION", "CONSTRAINT", 
  "CONSTRAINTS", "CONTINUE", "CONVERT", "CORRESPONDING", "COS", "COT", 
  "COUNT", "CREATE", "CURDATE", "CURRENT", "CURRENT_DATE", "CURRENT_TIME", 
  "CURRENT_TIMESTAMP", "CURTIME", "CURSOR", "DATABASE", "SQL_DATE", 
  "DATE_FORMAT", "DATE_REMAINDER", "DATE_VALUE", "DAY", "DAYOFMONTH", 
  "DAYOFWEEK", "DAYOFYEAR", "DAYS_BETWEEN", "DEALLOCATE", "DEC", 
  "DECLARE", "DEFAULT", "DEFERRABLE", "DEFERRED", "SQL_DELETE", "DESC", 
  "DESCRIBE", "DESCRIPTOR", "DIAGNOSTICS", "DICTIONARY", "DIRECTORY", 
  "DISCONNECT", "DISPLACEMENT", "DISTINCT", "DOMAIN_TOKEN", "SQL_DOUBLE", 
  "DOUBLE_QUOTED_STRING", "DROP", "ELSE", "END", "END_EXEC", "EQUAL", 
  "ESCAPE", "EXCEPT", "SQL_EXCEPTION", "EXEC", "EXECUTE", "EXISTS", "EXP", 
  "EXPONENT", "EXTERNAL", "EXTRACT", "SQL_FALSE", "FETCH", "FIRST", 
  "SQL_FLOAT", "FLOOR", "FN", "FOR", "FOREIGN", "FORTRAN", "FOUND", 
  "FOUR_DIGITS", "FROM", "FULL", "GET", "GLOBAL", "GO", "GOTO", "GRANT", 
  "GREATER_OR_EQUAL", "GREATER_THAN", "HAVING", "HOUR", "HOURS_BETWEEN", 
  "IDENTITY", "IFNULL", "SQL_IGNORE", "IMMEDIATE", "SQL_IN", "INCLUDE", 
  "INDEX", "INDICATOR", "INITIALLY", "INNER", "INPUT", "INSENSITIVE", 
  "INSERT", "INTEGER", "INTERSECT", "INTERVAL", "INTO", "IS", "ISOLATION", 
  "JOIN", "JUSTIFY", "KEY", "LANGUAGE", "LAST", "LCASE", "LEFT", "LENGTH", 
  "LESS_OR_EQUAL", "LESS_THAN", "LEVEL", "LIKE", "LINE_WIDTH", "LOCAL", 
  "LOCATE", "LOG", "SQL_LONG", "LOWER", "LTRIM", "LTRIP", "MATCH", 
  "SQL_MAX", "MICROSOFT", "SQL_MIN", "MINUS", "MINUTE", "MINUTES_BETWEEN", 
  "MOD", "MODIFY", "MODULE", "MONTH", "MONTHS_BETWEEN", "MUMPS", "NAMES", 
  "NATIONAL", "NCHAR", "NEXT", "NODUP", "NONE", "NOT", "NOT_EQUAL", "NOW", 
  "SQL_NULL", "SQL_IS", "SQL_IS_NULL", "SQL_IS_NOT_NULL", "NULLIF", 
  "NUMERIC", "OCTET_LENGTH", "ODBC", "OF", "SQL_OFF", "SQL_ON", "ONLY", 
  "OPEN", "OPTION", "OR", "ORDER", "OUTER", "OUTPUT", "OVERLAPS", "PAGE", 
  "PARTIAL", "SQL_PASCAL", "PERSISTENT", "CQL_PI", "PLI", "POSITION", 
  "PRECISION", "PREPARE", "PRESERVE", "PRIMARY", "PRIOR", "PRIVILEGES", 
  "PROCEDURE", "PRODUCT", "PUBLIC", "QUARTER", "QUIT", "RAND", 
  "READ_ONLY", "REAL", "REFERENCES", "REPEAT", "REPLACE", "RESTRICT", 
  "REVOKE", "RIGHT", "ROLLBACK", "ROWS", "RPAD", "RTRIM", "SCHEMA", 
  "SCREEN_WIDTH", "SCROLL", "SECOND", "SECONDS_BETWEEN", "SELECT", 
  "SEQUENCE", "SETOPT", "SET", "SHOWOPT", "SIGN", "SIMILAR", "SIMILAR_TO", 
  "NOT_SIMILAR_TO", "INTEGER_CONST", "REAL_CONST", "SIN", "SQL_SIZE", 
  "SMALLINT", "SOME", "SPACE", "SQL", "SQL_TRUE", "SQLCA", "SQLCODE", 
  "SQLERROR", "SQLSTATE", "SQLWARNING", "SQRT", "STDEV", "SUBSTRING", 
  "SUM", "SYSDATE", "SYSDATE_FORMAT", "SYSTEM", "TABLE", "TAN", 
  "TEMPORARY", "THEN", "THREE_DIGITS", "TIME", "TIMESTAMP", 
  "TIMEZONE_HOUR", "TIMEZONE_MINUTE", "TINYINT", "TO", "TO_CHAR", 
  "TO_DATE", "TRANSACTION", "TRANSLATE", "TRANSLATION", "TRUNCATE", 
  "GENERAL_TITLE", "TWO_DIGITS", "UCASE", "UNION", "UNIQUE", 
  "SQL_UNKNOWN", "UPDATE", "UPPER", "USAGE", "USER", "IDENTIFIER", 
  "IDENTIFIER_DOT_ASTERISK", "ERROR_DIGIT_BEFORE_IDENTIFIER", "USING", 
  "VALUE", "VALUES", "VARBINARY", "VARCHAR", "VARYING", "VENDOR", "VIEW", 
  "WEEK", "WHEN", "WHENEVER", "WHERE", "WHERE_CURRENT_OF", "WITH", 
  "WORD_WRAPPED", "WORK", "WRAPPED", "XOR", "YEAR", "YEARS_BETWEEN", 
  "__LAST_TOKEN", "'-'", "'+'", "'*'", "'%'", "'@'", "';'", "','", "'.'", 
  "'$'", "'('", "')'", "'?'", "'''", "'/'", "'='", "ILIKE", "'^'", "'['", 
  "']'", "'&'", "'|'", "'~'", "$accept", "TopLevelStatement", 
  "StatementList", "Statement", "CreateTableStatement", "@1", "ColDefs", 
  "ColDef", "ColKeys", "ColKey", "ColType", "SelectStatement", "Select", 
  "WhereClause", "aExpr", "aExpr2", "aExpr3", "aExpr4", "aExpr5", 
  "aExpr6", "aExpr7", "aExpr8", "aExpr9", "aExprList", "Tables", 
  "FlatTableList", "FlatTable", "ColViews", "ColItem", "ColExpression", 
  "ColWildCard", 0
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
     595,   596,   597,   598,   599,   600,    45,    43,    42,    37,
      64,    59,    44,    46,    36,    40,    41,    63,    39,    47,
      61,   601,    94,    91,    93,    38,   124,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   368,   369,   370,   370,   370,   371,   371,   373,   372,
     374,   374,   375,   375,   376,   376,   377,   377,   377,   378,
     378,   378,   378,   379,   379,   379,   379,   379,   380,   381,
     382,   383,   383,   383,   383,   384,   384,   384,   385,   385,
     385,   385,   385,   386,   386,   386,   386,   387,   387,   387,
     387,   387,   388,   388,   388,   388,   388,   388,   389,   389,
     389,   390,   390,   390,   390,   390,   390,   390,   390,   390,
     390,   390,   390,   391,   391,   392,   393,   393,   394,   394,
     394,   395,   395,   396,   396,   396,   396,   397,   397,   398,
     398
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     3,     1,     2,     1,     1,     0,     7,
       3,     1,     2,     3,     2,     1,     2,     2,     1,     1,
       4,     4,     0,     2,     3,     2,     3,     4,     1,     2,
       1,     3,     3,     3,     1,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     3,     4,     5,     1,     3,     4,
       1,     2,     2,     2,     2,     1,     4,     3,     1,     1,
       1,     1,     3,     3,     0,     2,     3,     1,     1,     2,
       3,     3,     1,     1,     1,     3,     2,     1,     4,     1,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,    28,     0,     2,     4,     6,     7,     0,     0,
       1,     5,    69,     0,     0,     0,    68,    70,    71,    65,
       0,     0,    89,     0,     0,    87,    30,    34,    37,    42,
      46,    51,    57,    60,    25,    23,    82,    83,    84,     8,
       3,     0,    78,    75,    77,    65,    64,     0,    74,    61,
      62,     0,    63,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,    24,     0,
      86,     0,     0,     0,    79,     0,     0,    67,    90,     0,
      72,    31,    32,    33,    35,    36,    39,    38,    40,    41,
      44,    45,    43,    48,    47,    50,    49,    54,    53,     0,
      52,     0,    58,     0,    29,    81,    27,    85,     0,    88,
      80,    76,     0,    66,     0,    59,    55,    22,     0,    11,
      73,    56,    19,     0,    12,     0,     9,     0,     0,    18,
       0,     0,    13,    15,    10,     0,     0,    17,    16,    14,
      20,    21
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     3,     4,     5,     6,    81,   128,   129,   142,   143,
     134,     7,     8,    77,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    89,    34,    43,    44,    35,    36,    37,
      38
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -328
static const short yypact[] =
{
     -67,  -282,  -328,    26,  -328,  -295,  -328,  -328,   -50,  -284,
    -328,   -67,  -328,  -296,  -256,   -36,  -328,  -328,  -328,  -323,
     -36,   -36,  -328,   -36,   -36,  -328,  -328,   -18,    34,  -326,
    -325,  -126,  -143,  -328,  -328,  -133,  -328,   -19,  -328,  -328,
    -328,   -40,   -14,  -275,  -328,  -293,  -328,  -313,  -328,  -328,
    -328,  -278,  -328,   -36,   -36,   -36,   -36,   -36,   -36,   -36,
     -36,   -36,   -36,   -36,   -36,   -36,   -36,   -36,   -36,   -36,
     -36,  -192,   -36,  -158,  -225,   -36,   -46,  -328,  -255,  -240,
    -328,  -272,  -271,  -238,  -328,  -256,  -236,  -328,  -328,  -327,
    -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,
    -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -217,
    -328,  -121,  -328,   -36,  -328,  -328,  -328,  -328,  -232,  -328,
    -328,  -328,   -36,  -328,   -36,  -328,  -328,    -3,  -319,  -328,
    -328,  -328,  -264,  -263,   -25,  -232,  -328,  -181,  -180,  -328,
    -115,   -74,   -25,  -328,  -328,  -259,  -258,  -328,  -328,  -328,
    -328,  -328
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -328,  -328,    88,  -328,  -328,  -328,  -328,   -35,  -328,   -41,
    -328,  -328,  -328,    24,   -12,     0,    19,   -17,     8,   -20,
     -55,  -328,     7,  -328,    68,  -328,    20,  -328,    28,    65,
    -328
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
     132,    53,    12,    79,    14,   139,    12,     1,    83,    87,
      69,    51,    12,     9,   107,   108,    12,   110,    65,    66,
      58,    59,    46,    62,    63,   122,    10,    49,    50,   123,
      47,    52,    48,   135,    64,    88,    70,   136,    39,    60,
      61,    96,    97,    98,    99,   103,   104,   105,   106,   111,
      67,    68,   112,    91,    92,    93,    11,    13,   126,    41,
      86,    13,    48,   114,    71,    72,    42,    13,    73,   131,
     100,   101,   102,    56,    57,    94,    95,    85,    90,   109,
     113,    75,   117,   118,   120,   119,    87,    14,   124,   125,
     127,   137,   138,   145,   146,   147,   148,   150,   151,    40,
     144,   149,   116,    78,   115,   121,    82,     0,     0,     0,
     130,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    74,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    15,     0,     0,
      16,    15,     0,     0,    16,     0,     0,    15,     0,     0,
      16,    15,     0,     0,    16,     0,     0,     0,     0,     0,
       0,     0,   140,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     2,     0,
       0,     0,     0,    75,     0,     0,    54,     0,     0,     0,
       0,     0,     0,     0,   141,     0,     0,     0,     0,    76,
       0,     0,     0,     0,    17,    18,     0,     0,    17,    18,
       0,     0,     0,     0,    17,    18,     0,     0,    17,    18,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    19,     0,     0,     0,    19,     0,     0,     0,
       0,     0,    45,     0,     0,     0,    45,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    20,    21,    22,     0,
      20,    21,    22,    80,     0,    23,    20,    21,    84,    23,
      20,    21,     0,     0,     0,    23,     0,    24,     0,    23,
       0,    24,     0,     0,    55,     0,   133,    24,     0,     0,
       0,    24
};

static const short yycheck[] =
{
       3,    19,    52,    22,   137,    30,    52,    74,    22,   322,
     153,    23,    52,   295,    69,    70,    52,    72,   144,   145,
     346,   347,    15,   348,   349,   352,     0,    20,    21,   356,
     353,    24,   355,   352,   359,   348,   179,   356,   322,   365,
     366,    58,    59,    60,    61,    65,    66,    67,    68,   207,
     176,   177,   210,    53,    54,    55,   351,   107,   113,   355,
     353,   107,   355,    75,   207,   208,   322,   107,   211,   124,
      62,    63,    64,    39,    40,    56,    57,   352,   356,   271,
     305,   336,   322,   355,   322,   356,   322,   137,   305,   210,
     322,   355,   355,   274,   274,   210,   170,   356,   356,    11,
     135,   142,    78,    35,    76,    85,    41,    -1,    -1,    -1,
     122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   271,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   207,    -1,    -1,
     210,   207,    -1,    -1,   210,    -1,    -1,   207,    -1,    -1,
     210,   207,    -1,    -1,   210,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   207,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   265,    -1,
      -1,    -1,    -1,   336,    -1,    -1,   224,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   239,    -1,    -1,    -1,    -1,   352,
      -1,    -1,    -1,    -1,   274,   275,    -1,    -1,   274,   275,
      -1,    -1,    -1,    -1,   274,   275,    -1,    -1,   274,   275,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   322,    -1,    -1,    -1,   322,    -1,    -1,    -1,
      -1,    -1,   322,    -1,    -1,    -1,   322,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   346,   347,   348,    -1,
     346,   347,   348,   322,    -1,   355,   346,   347,   322,   355,
     346,   347,    -1,    -1,    -1,   355,    -1,   367,    -1,   355,
      -1,   367,    -1,    -1,   342,    -1,   329,   367,    -1,    -1,
      -1,   367
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,    74,   265,   369,   370,   371,   372,   379,   380,   295,
       0,   351,    52,   107,   137,   207,   210,   274,   275,   322,
     346,   347,   348,   355,   367,   382,   383,   384,   385,   386,
     387,   388,   389,   390,   392,   395,   396,   397,   398,   322,
     370,   355,   322,   393,   394,   322,   390,   353,   355,   390,
     390,   382,   390,    19,   224,   342,    39,    40,   346,   347,
     365,   366,   348,   349,   359,   144,   145,   176,   177,   153,
     179,   207,   208,   211,   271,   336,   352,   381,   392,    22,
     322,   373,   397,    22,   322,   352,   353,   322,   348,   391,
     356,   383,   383,   383,   384,   384,   385,   385,   385,   385,
     386,   386,   386,   387,   387,   387,   387,   388,   388,   271,
     388,   207,   210,   305,   382,   396,   381,   322,   355,   356,
     322,   394,   352,   356,   305,   210,   388,   322,   374,   375,
     382,   388,     3,   329,   378,   352,   356,   355,   355,    30,
     207,   239,   376,   377,   375,   274,   274,   210,   170,   377,
     356,   356
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
#line 835 "sqlparser.y"
    {
//todo: multiple statements
//todo: not only "select" statements
	parser->setOperation(Parser::OP_Select);
	parser->setQuerySchema(yyvsp[0].querySchema);
}
    break;

  case 3:
#line 845 "sqlparser.y"
    {
//todo: multiple statements
}
    break;

  case 5:
#line 850 "sqlparser.y"
    {
	yyval.querySchema = yyvsp[-1].querySchema;
}
    break;

  case 6:
#line 860 "sqlparser.y"
    {
YYACCEPT;
}
    break;

  case 7:
#line 864 "sqlparser.y"
    {
	yyval.querySchema = yyvsp[0].querySchema;
}
    break;

  case 8:
#line 871 "sqlparser.y"
    {
	parser->setOperation(Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
}
    break;

  case 11:
#line 880 "sqlparser.y"
    {
}
    break;

  case 12:
#line 886 "sqlparser.y"
    {
	kdDebug() << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

//	delete field;
	field = 0;
}
    break;

  case 13:
#line 895 "sqlparser.y"
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
#line 910 "sqlparser.y"
    {
}
    break;

  case 16:
#line 916 "sqlparser.y"
    {
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
}
    break;

  case 17:
#line 921 "sqlparser.y"
    {
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
}
    break;

  case 18:
#line 926 "sqlparser.y"
    {
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
}
    break;

  case 19:
#line 934 "sqlparser.y"
    {
	field = new Field();
	field->setType(yyvsp[0].colType);
}
    break;

  case 20:
#line 939 "sqlparser.y"
    {
	kdDebug() << "sql + length" << endl;
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].colType);
}
    break;

  case 21:
#line 946 "sqlparser.y"
    {
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(Field::Text);
}
    break;

  case 22:
#line 952 "sqlparser.y"
    {
	// SQLITE compatibillity
	field = new Field();
	field->setType(Field::InvalidType);
}
    break;

  case 23:
#line 961 "sqlparser.y"
    {
	kdDebug() << "Select ColViews=" << yyvsp[0].exprList->debugString() << endl;
	
/*
	parser->select()->setBaseTable();
	if(parser->select()->unresolvedWildcard() && parser->db())
	{
		//resolve a (pure) wildcard

		TableStruct s = parser->db()->structure();

		for(Field *it = s.first(); it; it = s.next())
		{
			parser->select()->addCol(*it);
		}
		parser->select()->setUnresolvedWildcard(false);
	}
*/
}
    break;

  case 24:
#line 981 "sqlparser.y"
    {
//TODO: move this to all SELECT versions
	
	QuerySchema* querySchema = yyvsp[-2].querySchema;
	yyval.querySchema = querySchema;
	
	//-------tables list
	NArgExpr *tablesList = yyvsp[0].exprList;
	assert( tablesList ); //&& tablesList->exprClass() == KexiDBExpr_TableList );

	uint columnNum = 0;
/*TODO: use this later if there are columns that use database fields, 
        e.g. "SELECT 1 from table1 t, table2 t") is ok however. */
	//used to collect information about first repeated table name or alias:
//	QDict<char> tableNamesAndTableAliases(997, false);
//	QString repeatedTableNameOrTableAlias;

	ParseInfo parseInfo(querySchema);
	
	for (int i=0; i<tablesList->args(); i++, columnNum++) {
		BaseExpr *e = tablesList->arg(i);
		VariableExpr* t_e = 0;
		QCString aliasString;
		if (e->exprClass() == KexiDBExpr_SpecialBinary) {
			BinaryExpr* t_with_alias = dynamic_cast<BinaryExpr*>(e);
			assert(t_with_alias);
			assert(t_with_alias->left()->exprClass() == KexiDBExpr_Variable);
			assert(t_with_alias->right()->exprClass() == KexiDBExpr_Variable
				&& (t_with_alias->type()==AS || t_with_alias->type()==0));
			t_e = dynamic_cast<VariableExpr*>(t_with_alias->left());
			aliasString = dynamic_cast<VariableExpr*>(t_with_alias->right())->name.latin1();
		}
		else {
			t_e = dynamic_cast<VariableExpr*>(e);
		}
		assert(t_e);
		QCString tname = t_e->name.latin1();
		TableSchema *s = parser->db()->tableSchema(tname);
		if(!s) {
			setError(i18n("Field List Error"), 
				i18n("Table \"%1\" does not exist").arg(tname));
//			yyerror("fieldlisterror");
			return 0;
		}
		QCString tableOrAliasName;
		if (!aliasString.isEmpty()) {
			tableOrAliasName = aliasString;
			kdDebug() << "- add alias for table: " << aliasString << endl;
		} else {
			tableOrAliasName = tname;
		}
		// 1. collect information about first repeated table name or alias
		//    (potential ambiguity)
		QValueList<int> *list = parseInfo.repeatedTablesAndAliases[tableOrAliasName];
		if (list) {
			//another table/alias with the same name
			list->append( i );
			kdDebug() << "- another table/alias with name: " << tableOrAliasName << endl;
		}
		else {
			list = new QValueList<int>();
			list->append( i );
			parseInfo.repeatedTablesAndAliases.insert( tableOrAliasName, list );
			kdDebug() << "- first table/alias with name: " << tableOrAliasName << endl;
		}
/*		if (repeatedTableNameOrTableAlias.isEmpty()) {
			if (tableNamesAndTableAliasestname)
				repeatedTableNameOrTableAlias=tname;
			else
				tableNamesAndTableAliases.insert(tname, (const char*)1);
		}
		if (!aliasString.isEmpty()) {
			kdDebug() << "- add alias for table: " << aliasString << endl;
//			querySchema->setTableAlias(columnNum, aliasString);
			//2. collect information about first repeated table name or alias
			//   (potential ambiguity)
			if (repeatedTableNameOrTableAlias.isEmpty()) {
				if (tableNamesAndTableAliasesaliasString)
					repeatedTableNameOrTableAlias=aliasString;
				else
					tableNamesAndTableAliases.insert(aliasString, (const char*)1);
			}
		}*/
		kdDebug() << "addTable: " << tname << endl;
		querySchema->addTable( s, aliasString );
	}

	/* set parent table if there's only one */
//	if (parser->select()->tables()->count()==1)
	if (querySchema->tables()->count()==1)
		querySchema->setParentTable(querySchema->tables()->first());

	//-------add fields
	BaseExpr *e;
	columnNum = 0;
	for (BaseExpr::ListIterator it(yyvsp[-1].exprList->list); (e = it.current()); columnNum++)
	{
		bool moveNext = true; //used to avoid ++it when an item is taken from the list
		BaseExpr *columnExpr = e;
		VariableExpr* aliasVariable = 0;
		if (e->exprClass() == KexiDBExpr_SpecialBinary && dynamic_cast<BinaryExpr*>(e)
			&& (e->type()==AS || e->type()==0))
		{
			//KexiDBExpr_SpecialBinary: with alias
			columnExpr = dynamic_cast<BinaryExpr*>(e)->left();
//			isFieldWithAlias = true;
			aliasVariable = dynamic_cast<VariableExpr*>(dynamic_cast<BinaryExpr*>(e)->right());
			if (!aliasVariable) {
				setError(i18n("Invalid column alias definition")); //ok?
				return 0;
			}
		}

		const int c = columnExpr->exprClass();
		const bool isExpressionField = 
			   c == KexiDBExpr_Const
			|| c == KexiDBExpr_Unary
			|| c == KexiDBExpr_Arithm
			|| c == KexiDBExpr_Logical
			|| c == KexiDBExpr_Relational
			|| c == KexiDBExpr_Const
			|| c == KexiDBExpr_Function;

		if (c == KexiDBExpr_Variable) {
			//just a variable, do nothing, addColumn() will handle this
		}
		else if (isExpressionField) {
			//expression object will be reused, take, will be owned, do not destroy
			yyvsp[-1].exprList->list.take();
			moveNext = false;
		}
		else if (aliasVariable) {
			//take first (left) argument of the special binary expr, will be owned, do not destroy
			dynamic_cast<BinaryExpr*>(e)->list.take(0);
		}
		else {
			setError(i18n("Invalid column definition")); //ok?
			return 0;
		}

		if (!addColumn( parseInfo, columnExpr ))
			return 0;
		
		if (aliasVariable) {
			kdDebug() << "ALIAS \"" << aliasVariable->name << "\" set for column " 
				<< columnNum << endl;
			querySchema->setColumnAlias(columnNum, aliasVariable->name.latin1());
		}
/*		if (e->exprClass() == KexiDBExpr_SpecialBinary && dynamic_cast<BinaryExpr*>(e)
			&& (e->type()==AS || e->type()==0))
		{
			//also add alias
			VariableExpr* aliasVariable =
				dynamic_cast<VariableExpr*>(dynamic_cast<BinaryExpr*>(e)->right());
			if (!aliasVariable) {
				setError(i18n("Invalid column alias definition")); //ok?
				return 0;
			}
			kdDebug() << "ALIAS \"" << aliasVariable->name << "\" set for column " 
				<< columnNum << endl;
			querySchema->setColumnAlias(columnNum, aliasVariable->name.latin1());
		}*/

		if (moveNext)
			++it;
	}
	kdDebug() << "Select ColViews=" << yyvsp[-1].exprList->debugString() 
		<< " Tables=" << yyvsp[0].exprList->debugString() << endl;
	
	delete yyvsp[-1].exprList; //no longer needed
}
    break;

  case 25:
#line 1153 "sqlparser.y"
    {
	kdDebug() << "Select ColViews Tables" << endl;
}
    break;

  case 26:
#line 1157 "sqlparser.y"
    {
	kdDebug() << "Select ColViews Conditions" << endl;
}
    break;

  case 27:
#line 1161 "sqlparser.y"
    {
	kdDebug() << "Select ColViews Tables Conditions" << endl;
}
    break;

  case 28:
#line 1168 "sqlparser.y"
    {
	kdDebug() << "SELECT" << endl;
//	parser->createSelect();
//	parser->setOperation(Parser::OP_Select);
	yyval.querySchema = new QuerySchema();
}
    break;

  case 29:
#line 1178 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
}
    break;

  case 31:
#line 1194 "sqlparser.y"
    {
//	kdDebug() << "AND " << .debugString() << endl;
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, AND, yyvsp[0].expr );
}
    break;

  case 32:
#line 1199 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, OR, yyvsp[0].expr );
}
    break;

  case 33:
#line 1203 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr( KexiDBExpr_Arithm, yyvsp[-2].expr, XOR, yyvsp[0].expr );
}
    break;

  case 35:
#line 1213 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_LEFT, yyvsp[0].expr);
}
    break;

  case 36:
#line 1217 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_RIGHT, yyvsp[0].expr);
}
    break;

  case 38:
#line 1227 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '+', yyvsp[0].expr);
	yyval.expr->debug();
}
    break;

  case 39:
#line 1232 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '-', yyvsp[0].expr);
}
    break;

  case 40:
#line 1236 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '&', yyvsp[0].expr);
}
    break;

  case 41:
#line 1240 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '|', yyvsp[0].expr);
}
    break;

  case 43:
#line 1250 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '/', yyvsp[0].expr);
}
    break;

  case 44:
#line 1254 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '*', yyvsp[0].expr);
}
    break;

  case 45:
#line 1258 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '%', yyvsp[0].expr);
}
    break;

  case 47:
#line 1268 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_THAN, yyvsp[0].expr);
}
    break;

  case 48:
#line 1272 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_OR_EQUAL, yyvsp[0].expr);
}
    break;

  case 49:
#line 1276 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_THAN, yyvsp[0].expr);
}
    break;

  case 50:
#line 1280 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_OR_EQUAL, yyvsp[0].expr);
}
    break;

  case 52:
#line 1290 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL, yyvsp[0].expr);
}
    break;

  case 53:
#line 1294 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LIKE, yyvsp[0].expr);
}
    break;

  case 54:
#line 1298 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SQL_IN, yyvsp[0].expr);
}
    break;

  case 55:
#line 1302 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-3].expr, SIMILAR_TO, yyvsp[0].expr);
}
    break;

  case 56:
#line 1306 "sqlparser.y"
    {
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-4].expr, NOT_SIMILAR_TO, yyvsp[0].expr);
}
    break;

  case 58:
#line 1316 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( SQL_IS_NULL, yyvsp[-2].expr );
}
    break;

  case 59:
#line 1320 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( SQL_IS_NOT_NULL, yyvsp[-3].expr );
}
    break;

  case 61:
#line 1331 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( '-', yyvsp[0].expr );
}
    break;

  case 62:
#line 1335 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( '+', yyvsp[0].expr );
}
    break;

  case 63:
#line 1339 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( '~', yyvsp[0].expr );
}
    break;

  case 64:
#line 1343 "sqlparser.y"
    {
	yyval.expr = new UnaryExpr( NOT, yyvsp[0].expr );
}
    break;

  case 65:
#line 1347 "sqlparser.y"
    {
	yyval.expr = new VariableExpr( QString::fromLatin1(yyvsp[0].stringValue) );
	
//TODO: simplify this later if that's 'only one field name' expression
	kdDebug() << "  + identifier: " << yyvsp[0].stringValue << endl;
//	$$ = new Field();
//	$$->setName();
//	$$->setTable(dummy);

//	parser->select()->addField(field);
	requiresTable = true;
}
    break;

  case 66:
#line 1360 "sqlparser.y"
    {
	kdDebug() << "  + function: " << yyvsp[-3].stringValue << "(" << yyvsp[-1].exprList->debugString() << ")" << endl;
	yyval.expr = new FunctionExpr(yyvsp[-3].stringValue, yyvsp[-1].exprList);
}
    break;

  case 67:
#line 1366 "sqlparser.y"
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

  case 68:
#line 1377 "sqlparser.y"
    {
	yyval.expr = new ConstExpr( SQL_NULL, QVariant() );
	kdDebug() << "  + NULL" << endl;
//	$$ = new Field();
	//$$->setName(QString::null);
}
    break;

  case 69:
#line 1384 "sqlparser.y"
    {
	yyval.expr = new ConstExpr( CHARACTER_STRING_LITERAL, yyvsp[0].stringValue );
//	$$ = new Field();
//	$$->setName();
//	parser->select()->addField(field);
	kdDebug() << "  + constant \"" << yyvsp[0].stringValue << "\"" << endl;
}
    break;

  case 70:
#line 1392 "sqlparser.y"
    {
	yyval.expr = new ConstExpr( INTEGER_CONST, yyvsp[0].integerValue );
//	$$ = new Field();
//	$$->setName(QString::number());
//	parser->select()->addField(field);
	kdDebug() << "  + int constant: " << yyvsp[0].integerValue << endl;
}
    break;

  case 71:
#line 1400 "sqlparser.y"
    {
	yyval.expr = new ConstExpr( REAL_CONST, QPoint( yyvsp[0].realValue.integer, yyvsp[0].realValue.fractional ) );
	kdDebug() << "  + real constant: " << yyvsp[0].realValue.integer << "." << yyvsp[0].realValue.fractional << endl;
}
    break;

  case 72:
#line 1405 "sqlparser.y"
    {
	kdDebug() << "(expr)" << endl;
	yyval.expr = new UnaryExpr('(', yyvsp[-1].expr);
}
    break;

  case 73:
#line 1413 "sqlparser.y"
    {
	yyvsp[-2].exprList->add( yyvsp[0].expr );
	yyval.exprList = yyvsp[-2].exprList;
}
    break;

  case 74:
#line 1418 "sqlparser.y"
    {
	yyval.exprList = new NArgExpr(0, 0/*unknown*/);
}
    break;

  case 75:
#line 1425 "sqlparser.y"
    {
	yyval.exprList = yyvsp[0].exprList;
}
    break;

  case 76:
#line 1466 "sqlparser.y"
    {
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add(yyvsp[0].expr);
}
    break;

  case 77:
#line 1471 "sqlparser.y"
    {
	yyval.exprList = new NArgExpr(KexiDBExpr_TableList, IDENTIFIER); //ok?
	yyval.exprList->add(yyvsp[0].expr);
}
    break;

  case 78:
#line 1479 "sqlparser.y"
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

  case 79:
#line 1514 "sqlparser.y"
    {
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, 
		new VariableExpr(QString::fromLatin1(yyvsp[-1].stringValue)), 0,
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
}
    break;

  case 80:
#line 1523 "sqlparser.y"
    {
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary,
		new VariableExpr(QString::fromLatin1(yyvsp[-2].stringValue)), AS,
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
}
    break;

  case 81:
#line 1537 "sqlparser.y"
    {
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColViews , ColItem" << endl;
}
    break;

  case 82:
#line 1543 "sqlparser.y"
    {
	yyval.exprList = new NArgExpr(0,0);
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColItem" << endl;
}
    break;

  case 83:
#line 1552 "sqlparser.y"
    {
//	$$ = new Field();
//	dummy->addField($$);
//	$$->setExpression(  );
//	parser->select()->addField($$);
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column expr: '" << yyvsp[0].expr->debugString() << "'" << endl;
}
    break;

  case 84:
#line 1561 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column wildcard: '" << yyvsp[0].expr->debugString() << "'" << endl;
}
    break;

  case 85:
#line 1566 "sqlparser.y"
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

  case 86:
#line 1578 "sqlparser.y"
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

  case 87:
#line 1593 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
}
    break;

  case 88:
#line 1631 "sqlparser.y"
    {
	yyval.expr = yyvsp[-1].expr;
//TODO
//	$$->setName("DISTINCT(" + ->name() + ")");
}
    break;

  case 89:
#line 1640 "sqlparser.y"
    {
	yyval.expr = new VariableExpr("*");
	kdDebug() << "all columns" << endl;

//	QueryAsterisk *ast = new QueryAsterisk(parser->select(), dummy);
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
}
    break;

  case 90:
#line 1649 "sqlparser.y"
    {
	QString s = QString::fromLatin1(yyvsp[-2].stringValue);
	s+=".*";
	yyval.expr = new VariableExpr(s);
	kdDebug() << "  + all columns from " << s << endl;
}
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 3126 "sqlparser.tab.c"

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


#line 1663 "sqlparser.y"



const char * const tname(int offset) { return yytname[offset]; }
