/* A Bison parser, made by GNU Bison 1.875.  */

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
     BREAK = 294,
     BY = 295,
     CASCADE = 296,
     CASCADED = 297,
     CASE = 298,
     CAST = 299,
     CATALOG = 300,
     CEILING = 301,
     CENTER = 302,
     SQL_CHAR = 303,
     CHAR_LENGTH = 304,
     CHARACTER_STRING_LITERAL = 305,
     CHECK = 306,
     CLOSE = 307,
     COALESCE = 308,
     COBOL = 309,
     COLLATE = 310,
     COLLATION = 311,
     COLUMN = 312,
     COMMIT = 313,
     COMPUTE = 314,
     CONCAT = 315,
     CONNECT = 316,
     CONNECTION = 317,
     CONSTRAINT = 318,
     CONSTRAINTS = 319,
     CONTINUE = 320,
     CONVERT = 321,
     CORRESPONDING = 322,
     COS = 323,
     COT = 324,
     COUNT = 325,
     CREATE = 326,
     CURDATE = 327,
     CURRENT = 328,
     CURRENT_DATE = 329,
     CURRENT_TIME = 330,
     CURRENT_TIMESTAMP = 331,
     CURTIME = 332,
     CURSOR = 333,
     DATABASE = 334,
     SQL_DATE = 335,
     DATE_FORMAT = 336,
     DATE_REMAINDER = 337,
     DATE_VALUE = 338,
     DAY = 339,
     DAYOFMONTH = 340,
     DAYOFWEEK = 341,
     DAYOFYEAR = 342,
     DAYS_BETWEEN = 343,
     DEALLOCATE = 344,
     DEC = 345,
     DECLARE = 346,
     DEFAULT = 347,
     DEFERRABLE = 348,
     DEFERRED = 349,
     SQL_DELETE = 350,
     DESC = 351,
     DESCRIBE = 352,
     DESCRIPTOR = 353,
     DIAGNOSTICS = 354,
     DICTIONARY = 355,
     DIRECTORY = 356,
     DISCONNECT = 357,
     DISPLACEMENT = 358,
     DISTINCT = 359,
     DOMAIN_TOKEN = 360,
     SQL_DOUBLE = 361,
     DOUBLE_QUOTED_STRING = 362,
     DROP = 363,
     ELSE = 364,
     END = 365,
     END_EXEC = 366,
     EQUAL = 367,
     ESCAPE = 368,
     EXCEPT = 369,
     SQL_EXCEPTION = 370,
     EXEC = 371,
     EXECUTE = 372,
     EXISTS = 373,
     EXP = 374,
     EXPONENT = 375,
     EXTERNAL = 376,
     EXTRACT = 377,
     SQL_FALSE = 378,
     FETCH = 379,
     FIRST = 380,
     SQL_FLOAT = 381,
     FLOOR = 382,
     FN = 383,
     FOR = 384,
     FOREIGN = 385,
     FORTRAN = 386,
     FOUND = 387,
     FOUR_DIGITS = 388,
     FROM = 389,
     FULL = 390,
     GET = 391,
     GLOBAL = 392,
     GO = 393,
     GOTO = 394,
     GRANT = 395,
     GREATER_OR_EQUAL = 396,
     GREATER_THAN = 397,
     HAVING = 398,
     HOUR = 399,
     HOURS_BETWEEN = 400,
     IDENTITY = 401,
     IFNULL = 402,
     SQL_IGNORE = 403,
     IMMEDIATE = 404,
     SQL_IN = 405,
     INCLUDE = 406,
     INDEX = 407,
     INDICATOR = 408,
     INITIALLY = 409,
     INNER = 410,
     INPUT = 411,
     INSENSITIVE = 412,
     INSERT = 413,
     INTEGER = 414,
     INTERSECT = 415,
     INTERVAL = 416,
     INTO = 417,
     IS = 418,
     ISOLATION = 419,
     JOIN = 420,
     JUSTIFY = 421,
     KEY = 422,
     LANGUAGE = 423,
     LAST = 424,
     LCASE = 425,
     LEFT = 426,
     LENGTH = 427,
     LESS_OR_EQUAL = 428,
     LESS_THAN = 429,
     LEVEL = 430,
     LIKE = 431,
     LINE_WIDTH = 432,
     LOCAL = 433,
     LOCATE = 434,
     LOG = 435,
     SQL_LONG = 436,
     LOWER = 437,
     LTRIM = 438,
     LTRIP = 439,
     MATCH = 440,
     SQL_MAX = 441,
     MICROSOFT = 442,
     SQL_MIN = 443,
     MINUS = 444,
     MINUTE = 445,
     MINUTES_BETWEEN = 446,
     MOD = 447,
     MODIFY = 448,
     MODULE = 449,
     MONTH = 450,
     MONTHS_BETWEEN = 451,
     MUMPS = 452,
     NAMES = 453,
     NATIONAL = 454,
     NCHAR = 455,
     NEXT = 456,
     NODUP = 457,
     NONE = 458,
     NOT = 459,
     NOT_EQUAL = 460,
     NOW = 461,
     SQL_NULL = 462,
     NULLIF = 463,
     NUMERIC = 464,
     OCTET_LENGTH = 465,
     ODBC = 466,
     OF = 467,
     SQL_OFF = 468,
     SQL_ON = 469,
     ONLY = 470,
     OPEN = 471,
     OPTION = 472,
     OR = 473,
     ORDER = 474,
     OUTER = 475,
     OUTPUT = 476,
     OVERLAPS = 477,
     PAGE = 478,
     PARTIAL = 479,
     SQL_PASCAL = 480,
     PERSISTENT = 481,
     CQL_PI = 482,
     PLI = 483,
     POSITION = 484,
     PRECISION = 485,
     PREPARE = 486,
     PRESERVE = 487,
     PRIMARY = 488,
     PRIOR = 489,
     PRIVILEGES = 490,
     PROCEDURE = 491,
     PRODUCT = 492,
     PUBLIC = 493,
     QUARTER = 494,
     QUIT = 495,
     RAND = 496,
     READ_ONLY = 497,
     REAL = 498,
     REFERENCES = 499,
     REPEAT = 500,
     REPLACE = 501,
     RESTRICT = 502,
     REVOKE = 503,
     RIGHT = 504,
     ROLLBACK = 505,
     ROWS = 506,
     RPAD = 507,
     RTRIM = 508,
     SCHEMA = 509,
     SCREEN_WIDTH = 510,
     SCROLL = 511,
     SECOND = 512,
     SECONDS_BETWEEN = 513,
     SELECT = 514,
     SEQUENCE = 515,
     SETOPT = 516,
     SET = 517,
     SHOWOPT = 518,
     SIGN = 519,
     INTEGER_CONST = 520,
     REAL_CONST = 521,
     SIN = 522,
     SQL_SIZE = 523,
     SMALLINT = 524,
     SOME = 525,
     SPACE = 526,
     SQL = 527,
     SQL_TRUE = 528,
     SQLCA = 529,
     SQLCODE = 530,
     SQLERROR = 531,
     SQLSTATE = 532,
     SQLWARNING = 533,
     SQRT = 534,
     STDEV = 535,
     SUBSTRING = 536,
     SUM = 537,
     SYSDATE = 538,
     SYSDATE_FORMAT = 539,
     SYSTEM = 540,
     TABLE = 541,
     TAN = 542,
     TEMPORARY = 543,
     THEN = 544,
     THREE_DIGITS = 545,
     TIME = 546,
     TIMESTAMP = 547,
     TIMEZONE_HOUR = 548,
     TIMEZONE_MINUTE = 549,
     TINYINT = 550,
     TO = 551,
     TO_CHAR = 552,
     TO_DATE = 553,
     TRANSACTION = 554,
     TRANSLATE = 555,
     TRANSLATION = 556,
     TRUNCATE = 557,
     GENERAL_TITLE = 558,
     TWO_DIGITS = 559,
     UCASE = 560,
     UNION = 561,
     UNIQUE = 562,
     SQL_UNKNOWN = 563,
     UPDATE = 564,
     UPPER = 565,
     USAGE = 566,
     USER = 567,
     IDENTIFIER = 568,
     IDENTIFIER_DOT_ASTERISK = 569,
     ERROR_DIGIT_BEFORE_IDENTIFIER = 570,
     USING = 571,
     VALUE = 572,
     VALUES = 573,
     VARBINARY = 574,
     VARCHAR = 575,
     VARYING = 576,
     VENDOR = 577,
     VIEW = 578,
     WEEK = 579,
     WHEN = 580,
     WHENEVER = 581,
     WHERE = 582,
     WHERE_CURRENT_OF = 583,
     WITH = 584,
     WORD_WRAPPED = 585,
     WORK = 586,
     WRAPPED = 587,
     YEAR = 588,
     YEARS_BETWEEN = 589,
     SIMILAR = 590,
     ILIKE = 591
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
#define BREAK 294
#define BY 295
#define CASCADE 296
#define CASCADED 297
#define CASE 298
#define CAST 299
#define CATALOG 300
#define CEILING 301
#define CENTER 302
#define SQL_CHAR 303
#define CHAR_LENGTH 304
#define CHARACTER_STRING_LITERAL 305
#define CHECK 306
#define CLOSE 307
#define COALESCE 308
#define COBOL 309
#define COLLATE 310
#define COLLATION 311
#define COLUMN 312
#define COMMIT 313
#define COMPUTE 314
#define CONCAT 315
#define CONNECT 316
#define CONNECTION 317
#define CONSTRAINT 318
#define CONSTRAINTS 319
#define CONTINUE 320
#define CONVERT 321
#define CORRESPONDING 322
#define COS 323
#define COT 324
#define COUNT 325
#define CREATE 326
#define CURDATE 327
#define CURRENT 328
#define CURRENT_DATE 329
#define CURRENT_TIME 330
#define CURRENT_TIMESTAMP 331
#define CURTIME 332
#define CURSOR 333
#define DATABASE 334
#define SQL_DATE 335
#define DATE_FORMAT 336
#define DATE_REMAINDER 337
#define DATE_VALUE 338
#define DAY 339
#define DAYOFMONTH 340
#define DAYOFWEEK 341
#define DAYOFYEAR 342
#define DAYS_BETWEEN 343
#define DEALLOCATE 344
#define DEC 345
#define DECLARE 346
#define DEFAULT 347
#define DEFERRABLE 348
#define DEFERRED 349
#define SQL_DELETE 350
#define DESC 351
#define DESCRIBE 352
#define DESCRIPTOR 353
#define DIAGNOSTICS 354
#define DICTIONARY 355
#define DIRECTORY 356
#define DISCONNECT 357
#define DISPLACEMENT 358
#define DISTINCT 359
#define DOMAIN_TOKEN 360
#define SQL_DOUBLE 361
#define DOUBLE_QUOTED_STRING 362
#define DROP 363
#define ELSE 364
#define END 365
#define END_EXEC 366
#define EQUAL 367
#define ESCAPE 368
#define EXCEPT 369
#define SQL_EXCEPTION 370
#define EXEC 371
#define EXECUTE 372
#define EXISTS 373
#define EXP 374
#define EXPONENT 375
#define EXTERNAL 376
#define EXTRACT 377
#define SQL_FALSE 378
#define FETCH 379
#define FIRST 380
#define SQL_FLOAT 381
#define FLOOR 382
#define FN 383
#define FOR 384
#define FOREIGN 385
#define FORTRAN 386
#define FOUND 387
#define FOUR_DIGITS 388
#define FROM 389
#define FULL 390
#define GET 391
#define GLOBAL 392
#define GO 393
#define GOTO 394
#define GRANT 395
#define GREATER_OR_EQUAL 396
#define GREATER_THAN 397
#define HAVING 398
#define HOUR 399
#define HOURS_BETWEEN 400
#define IDENTITY 401
#define IFNULL 402
#define SQL_IGNORE 403
#define IMMEDIATE 404
#define SQL_IN 405
#define INCLUDE 406
#define INDEX 407
#define INDICATOR 408
#define INITIALLY 409
#define INNER 410
#define INPUT 411
#define INSENSITIVE 412
#define INSERT 413
#define INTEGER 414
#define INTERSECT 415
#define INTERVAL 416
#define INTO 417
#define IS 418
#define ISOLATION 419
#define JOIN 420
#define JUSTIFY 421
#define KEY 422
#define LANGUAGE 423
#define LAST 424
#define LCASE 425
#define LEFT 426
#define LENGTH 427
#define LESS_OR_EQUAL 428
#define LESS_THAN 429
#define LEVEL 430
#define LIKE 431
#define LINE_WIDTH 432
#define LOCAL 433
#define LOCATE 434
#define LOG 435
#define SQL_LONG 436
#define LOWER 437
#define LTRIM 438
#define LTRIP 439
#define MATCH 440
#define SQL_MAX 441
#define MICROSOFT 442
#define SQL_MIN 443
#define MINUS 444
#define MINUTE 445
#define MINUTES_BETWEEN 446
#define MOD 447
#define MODIFY 448
#define MODULE 449
#define MONTH 450
#define MONTHS_BETWEEN 451
#define MUMPS 452
#define NAMES 453
#define NATIONAL 454
#define NCHAR 455
#define NEXT 456
#define NODUP 457
#define NONE 458
#define NOT 459
#define NOT_EQUAL 460
#define NOW 461
#define SQL_NULL 462
#define NULLIF 463
#define NUMERIC 464
#define OCTET_LENGTH 465
#define ODBC 466
#define OF 467
#define SQL_OFF 468
#define SQL_ON 469
#define ONLY 470
#define OPEN 471
#define OPTION 472
#define OR 473
#define ORDER 474
#define OUTER 475
#define OUTPUT 476
#define OVERLAPS 477
#define PAGE 478
#define PARTIAL 479
#define SQL_PASCAL 480
#define PERSISTENT 481
#define CQL_PI 482
#define PLI 483
#define POSITION 484
#define PRECISION 485
#define PREPARE 486
#define PRESERVE 487
#define PRIMARY 488
#define PRIOR 489
#define PRIVILEGES 490
#define PROCEDURE 491
#define PRODUCT 492
#define PUBLIC 493
#define QUARTER 494
#define QUIT 495
#define RAND 496
#define READ_ONLY 497
#define REAL 498
#define REFERENCES 499
#define REPEAT 500
#define REPLACE 501
#define RESTRICT 502
#define REVOKE 503
#define RIGHT 504
#define ROLLBACK 505
#define ROWS 506
#define RPAD 507
#define RTRIM 508
#define SCHEMA 509
#define SCREEN_WIDTH 510
#define SCROLL 511
#define SECOND 512
#define SECONDS_BETWEEN 513
#define SELECT 514
#define SEQUENCE 515
#define SETOPT 516
#define SET 517
#define SHOWOPT 518
#define SIGN 519
#define INTEGER_CONST 520
#define REAL_CONST 521
#define SIN 522
#define SQL_SIZE 523
#define SMALLINT 524
#define SOME 525
#define SPACE 526
#define SQL 527
#define SQL_TRUE 528
#define SQLCA 529
#define SQLCODE 530
#define SQLERROR 531
#define SQLSTATE 532
#define SQLWARNING 533
#define SQRT 534
#define STDEV 535
#define SUBSTRING 536
#define SUM 537
#define SYSDATE 538
#define SYSDATE_FORMAT 539
#define SYSTEM 540
#define TABLE 541
#define TAN 542
#define TEMPORARY 543
#define THEN 544
#define THREE_DIGITS 545
#define TIME 546
#define TIMESTAMP 547
#define TIMEZONE_HOUR 548
#define TIMEZONE_MINUTE 549
#define TINYINT 550
#define TO 551
#define TO_CHAR 552
#define TO_DATE 553
#define TRANSACTION 554
#define TRANSLATE 555
#define TRANSLATION 556
#define TRUNCATE 557
#define GENERAL_TITLE 558
#define TWO_DIGITS 559
#define UCASE 560
#define UNION 561
#define UNIQUE 562
#define SQL_UNKNOWN 563
#define UPDATE 564
#define UPPER 565
#define USAGE 566
#define USER 567
#define IDENTIFIER 568
#define IDENTIFIER_DOT_ASTERISK 569
#define ERROR_DIGIT_BEFORE_IDENTIFIER 570
#define USING 571
#define VALUE 572
#define VALUES 573
#define VARBINARY 574
#define VARCHAR 575
#define VARYING 576
#define VENDOR 577
#define VIEW 578
#define WEEK 579
#define WHEN 580
#define WHENEVER 581
#define WHERE 582
#define WHERE_CURRENT_OF 583
#define WITH 584
#define WORD_WRAPPED 585
#define WORK 586
#define WRAPPED 587
#define YEAR 588
#define YEARS_BETWEEN 589
#define SIMILAR 590
#define ILIKE 591




/* Copy the first part of user declarations.  */
#line 404 "sqlparser.y"

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
//#include "sqlparser.h"
#include "sqltypes.h"

//	using namespace std;
	using namespace KexiDB;

#define YY_NO_UNPUT
#define YYSTACK_USE_ALLOCA 1
#define YYMAXDEPTH 255

Parser *parser;
KexiDB::Field *field;
bool requiresTable;
QPtrList<KexiDB::Field> fieldList;
//	QPtrList<KexiDB::TableSchema> tableList;
//	QDict<KexiDB::TableSchema> tableDict;
//	KexiDB::TableSchema *dummy = 0;
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

	if (parser->error().type().isEmpty() 
		&& (strlen(str)==0 
		|| qstricmp(str, "syntax error")==0 || qstricmp(str, "parse error")==0))
	{
		kdDebug() << parser->statement() << endl;
		QString ptrline = "";
		for(int i=0; i < current; i++)
			ptrline += " ";

		ptrline += "^";

		kdDebug() << ptrline << endl;

		//lexer may add error messages
		QString lexerErr = parser->error().error();
		if (!lexerErr.isEmpty())
			lexerErr.prepend(": ");
			
		KexiDB::ParserError err(i18n("Syntax Error"), i18n("Syntax Error near \"%1\"").arg(ctoken)+lexerErr, ctoken, current);
		parser->setError(err);
	}
}

void setError(const QString& errName, const QString& errDesc)
{
	parser->setError( KexiDB::ParserError(errName, errDesc, ctoken, current) );
	yyerror(errName.latin1());
}

void setError(const QString& errDesc)
{
	setError("", errDesc);
}

void tableNotFoundError(const QString& tableName)
{
	setError( i18n("Table not found"), i18n("Unknown table \"%1\"").arg(tableName) );
}

/* this is better than assert() */
#define IMPL_ERROR(errmsg) setError("Implementation error", errmsg)

bool parseData(KexiDB::Parser *p, const char *data)
{
/* todo: remove dummy */
//		if (!dummy)
			//dummy = new KexiDB::TableSchema();
/* todo: make this REENTRANT */
	parser = p;
	parser->clear();
	field = 0;
	fieldList.clear();
	requiresTable = false;

	if (!data) {
		KexiDB::ParserError err(i18n("Error"), i18n("No query specified"), ctoken, current);
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
	if(parser->operation() == KexiDB::Parser::OP_Select)
	{
		kdDebug() << "parseData(): ok" << endl;
//			kdDebug() << "parseData(): " << tableDict.count() << " loaded tables" << endl;
/*			KexiDB::TableSchema *ts;
			for(QDictIterator<KexiDB::TableSchema> it(tableDict); KexiDB::TableSchema *s = tableList.first(); s; s = tableList.next())
			{
				kdDebug() << "  " << s->name() << endl;
			}*/
/*removed
			KexiDB::Field::ListIterator it = parser->select()->fieldsIterator();
			for(KexiDB::Field *item; (item = it.current()); ++it)
			{
				if(tableList.findRef(item->table()) == -1)
				{
					KexiDB::ParserError err(i18n("Field List Error"), i18n("Unknown table '%1' in field list").arg(item->table()->name()), ctoken, current);
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
bool addColumn( QuerySchema *querySchema, BaseExpr* column, 
	const QDict< QValueList<int> > &repeatedTablesAndAliases )
{
	VariableExpr *v_e = dynamic_cast<VariableExpr*>(column);
	if (column->exprClass() != KexiDBExpr_Variable || !v_e) {
		//err
		return false;
	}
	QString varName = v_e->name;
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
//			querySchema->addAsterisk( new KexiDB::QueryAsterisk(parser->select()) );
			querySchema->addAsterisk( new KexiDB::QueryAsterisk(querySchema) );
		}
		else {
			//find first table that has this field
			Field *firstField = 0;
			for (TableSchema::ListIterator it(*querySchema->tables()); it.current(); ++it) {
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
			querySchema->addField(firstField);
		}
	}
	else {//table.fieldname or tableAlias.fieldname
		tableName = tableName.lower();
		KexiDB::TableSchema *ts = querySchema->table( tableName );
		if (ts) {//table.fieldname
			//check if "table" is covered by an alias
			const QValueList<int> tPositions = querySchema->tablePositions(tableName);
			QValueList<int>::ConstIterator it = tPositions.begin();
			QCString tableAlias;
			bool covered = true;
			for (; it!=tPositions.end() && covered; ++it) {
				tableAlias = querySchema->tableAlias(*it);
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
			tablePosition = querySchema->tablePositionForAlias( tableName.latin1() );
			if (tablePosition>=0) {
				ts = querySchema->tables()->at(tablePosition);
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
//				querySchema->addAsterisk( new KexiDB::QueryAsterisk(parser->select(), ts) );
				querySchema->addAsterisk( new KexiDB::QueryAsterisk(querySchema, ts) );
			}
			else {
				kdDebug() << " --it's a table.name" << endl;
				KexiDB::Field *realField = ts->field(fieldName);
				if (realField) {
					// check if table or alias is used twice and both have the same column
					// (so the column is ambiguous)
					int numberOfTheSameFields = 0;
					for (QValueList<int>::iterator it = positionsList->begin();
						it!=positionsList->end();++it)
					{
						KexiDB::TableSchema *otherTS = querySchema->tables()->at(*it);
						if (otherTS->field(fieldName))
							numberOfTheSameFields++;
						if (numberOfTheSameFields>1) {
							setError(i18n("Ambiguous \"%1.%2\" expression").arg(tableName).arg(fieldName),
								i18n("More than one \"%1\" table or alias defined containing \"%2\" field").arg(tableName).arg(fieldName));
							return false;
						}
					}

					querySchema->addField(realField, tablePosition);
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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 723 "sqlparser.y"
typedef union YYSTYPE {
	char stringValue[255];
	int integerValue;
	struct realType realValue;
	KexiDB::Field::Type colType;
	KexiDB::Field *field;
	KexiDB::BaseExpr *expr;
	KexiDB::NArgExpr *exprList;
	KexiDB::ConstExpr *constExpr;
	KexiDB::QuerySchema *querySchema;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 1078 "sqlparser.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 1090 "sqlparser.tab.c"

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
#define YYLAST   347

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  355
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  23
/* YYNRULES -- Number of rules. */
#define YYNRULES  70
/* YYNRULES -- Number of states. */
#define YYNSTATES  115

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   591

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned short yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   343,   338,     2,   347,
     344,   345,   337,   336,   341,   335,   342,   348,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   340,
       2,   349,     2,   346,   339,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   353,     2,   354,   352,     2,     2,     2,     2,     2,
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
     350,   351
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     5,     9,    11,    14,    16,    18,    19,
      27,    31,    33,    36,    40,    43,    45,    48,    51,    53,
      55,    60,    65,    66,    69,    73,    76,    80,    85,    87,
      90,    94,    98,   101,   105,   109,   113,   117,   121,   125,
     129,   133,   137,   141,   145,   149,   154,   156,   160,   162,
     164,   166,   168,   172,   176,   177,   180,   184,   186,   188,
     191,   195,   199,   201,   203,   205,   209,   212,   214,   219,
     221
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     356,     0,    -1,   357,    -1,   358,   340,   357,    -1,   358,
      -1,   358,   340,    -1,   359,    -1,   366,    -1,    -1,    71,
     286,   313,   360,   344,   361,   345,    -1,   361,   341,   362,
      -1,   362,    -1,   313,   365,    -1,   313,   365,   363,    -1,
     363,   364,    -1,   364,    -1,   233,   167,    -1,   204,   207,
      -1,    30,    -1,     3,    -1,     3,   344,   265,   345,    -1,
     320,   344,   265,   345,    -1,    -1,   367,   374,    -1,   367,
     374,   371,    -1,   367,   371,    -1,   367,   374,   368,    -1,
     367,   374,   371,   368,    -1,   259,    -1,   327,   369,    -1,
     369,    19,   369,    -1,   369,   218,   369,    -1,   204,   369,
      -1,   369,   336,   369,    -1,   369,   335,   369,    -1,   369,
     348,   369,    -1,   369,   337,   369,    -1,   369,   338,   369,
      -1,   369,   205,   369,    -1,   369,   142,   369,    -1,   369,
     141,   369,    -1,   369,   174,   369,    -1,   369,   173,   369,
      -1,   369,   176,   369,    -1,   369,   150,   369,    -1,   313,
     344,   370,   345,    -1,   313,    -1,   313,   342,   313,    -1,
     207,    -1,    50,    -1,   265,    -1,   266,    -1,   344,   369,
     345,    -1,   370,   341,   369,    -1,    -1,   134,   372,    -1,
     372,   341,   373,    -1,   373,    -1,   313,    -1,   313,   313,
      -1,   313,    22,   313,    -1,   374,   341,   375,    -1,   375,
      -1,   376,    -1,   377,    -1,   376,    22,   313,    -1,   376,
     313,    -1,   369,    -1,   104,   344,   376,   345,    -1,   337,
      -1,   314,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   783,   783,   793,   797,   798,   808,   812,   820,   819,
     828,   828,   834,   843,   858,   858,   864,   869,   874,   882,
     887,   894,   901,   909,   929,  1063,  1067,  1071,  1078,  1088,
    1099,  1104,  1109,  1114,  1118,  1122,  1126,  1130,  1134,  1138,
    1142,  1146,  1150,  1154,  1158,  1162,  1167,  1181,  1192,  1199,
    1207,  1215,  1220,  1228,  1234,  1240,  1281,  1286,  1294,  1329,
    1338,  1352,  1358,  1367,  1376,  1381,  1393,  1408,  1446,  1455,
    1464
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
  "BREAK", "BY", "CASCADE", "CASCADED", "CASE", "CAST", "CATALOG", 
  "CEILING", "CENTER", "SQL_CHAR", "CHAR_LENGTH", 
  "CHARACTER_STRING_LITERAL", "CHECK", "CLOSE", "COALESCE", "COBOL", 
  "COLLATE", "COLLATION", "COLUMN", "COMMIT", "COMPUTE", "CONCAT", 
  "CONNECT", "CONNECTION", "CONSTRAINT", "CONSTRAINTS", "CONTINUE", 
  "CONVERT", "CORRESPONDING", "COS", "COT", "COUNT", "CREATE", "CURDATE", 
  "CURRENT", "CURRENT_DATE", "CURRENT_TIME", "CURRENT_TIMESTAMP", 
  "CURTIME", "CURSOR", "DATABASE", "SQL_DATE", "DATE_FORMAT", 
  "DATE_REMAINDER", "DATE_VALUE", "DAY", "DAYOFMONTH", "DAYOFWEEK", 
  "DAYOFYEAR", "DAYS_BETWEEN", "DEALLOCATE", "DEC", "DECLARE", "DEFAULT", 
  "DEFERRABLE", "DEFERRED", "SQL_DELETE", "DESC", "DESCRIBE", 
  "DESCRIPTOR", "DIAGNOSTICS", "DICTIONARY", "DIRECTORY", "DISCONNECT", 
  "DISPLACEMENT", "DISTINCT", "DOMAIN_TOKEN", "SQL_DOUBLE", 
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
  "SQL_NULL", "NULLIF", "NUMERIC", "OCTET_LENGTH", "ODBC", "OF", 
  "SQL_OFF", "SQL_ON", "ONLY", "OPEN", "OPTION", "OR", "ORDER", "OUTER", 
  "OUTPUT", "OVERLAPS", "PAGE", "PARTIAL", "SQL_PASCAL", "PERSISTENT", 
  "CQL_PI", "PLI", "POSITION", "PRECISION", "PREPARE", "PRESERVE", 
  "PRIMARY", "PRIOR", "PRIVILEGES", "PROCEDURE", "PRODUCT", "PUBLIC", 
  "QUARTER", "QUIT", "RAND", "READ_ONLY", "REAL", "REFERENCES", "REPEAT", 
  "REPLACE", "RESTRICT", "REVOKE", "RIGHT", "ROLLBACK", "ROWS", "RPAD", 
  "RTRIM", "SCHEMA", "SCREEN_WIDTH", "SCROLL", "SECOND", 
  "SECONDS_BETWEEN", "SELECT", "SEQUENCE", "SETOPT", "SET", "SHOWOPT", 
  "SIGN", "INTEGER_CONST", "REAL_CONST", "SIN", "SQL_SIZE", "SMALLINT", 
  "SOME", "SPACE", "SQL", "SQL_TRUE", "SQLCA", "SQLCODE", "SQLERROR", 
  "SQLSTATE", "SQLWARNING", "SQRT", "STDEV", "SUBSTRING", "SUM", 
  "SYSDATE", "SYSDATE_FORMAT", "SYSTEM", "TABLE", "TAN", "TEMPORARY", 
  "THEN", "THREE_DIGITS", "TIME", "TIMESTAMP", "TIMEZONE_HOUR", 
  "TIMEZONE_MINUTE", "TINYINT", "TO", "TO_CHAR", "TO_DATE", "TRANSACTION", 
  "TRANSLATE", "TRANSLATION", "TRUNCATE", "GENERAL_TITLE", "TWO_DIGITS", 
  "UCASE", "UNION", "UNIQUE", "SQL_UNKNOWN", "UPDATE", "UPPER", "USAGE", 
  "USER", "IDENTIFIER", "IDENTIFIER_DOT_ASTERISK", 
  "ERROR_DIGIT_BEFORE_IDENTIFIER", "USING", "VALUE", "VALUES", 
  "VARBINARY", "VARCHAR", "VARYING", "VENDOR", "VIEW", "WEEK", "WHEN", 
  "WHENEVER", "WHERE", "WHERE_CURRENT_OF", "WITH", "WORD_WRAPPED", "WORK", 
  "WRAPPED", "YEAR", "YEARS_BETWEEN", "'-'", "'+'", "'*'", "'%'", "'@'", 
  "';'", "','", "'.'", "'$'", "'('", "')'", "'?'", "'''", "'/'", "'='", 
  "SIMILAR", "ILIKE", "'^'", "'['", "']'", "$accept", "TopLevelStatement", 
  "StatementList", "Statement", "CreateTableStatement", "@1", "ColDefs", 
  "ColDef", "ColKeys", "ColKey", "ColType", "SelectStatement", "Select", 
  "WhereClause", "aExpr", "aExprList", "Tables", "FlatTableList", 
  "FlatTable", "ColViews", "ColItem", "ColExpression", "ColWildCard", 0
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
     585,   586,   587,   588,   589,    45,    43,    42,    37,    64,
      59,    44,    46,    36,    40,    41,    63,    39,    47,    61,
     590,   591,    94,    91,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   355,   356,   357,   357,   357,   358,   358,   360,   359,
     361,   361,   362,   362,   363,   363,   364,   364,   364,   365,
     365,   365,   365,   366,   366,   366,   366,   366,   367,   368,
     369,   369,   369,   369,   369,   369,   369,   369,   369,   369,
     369,   369,   369,   369,   369,   369,   369,   369,   369,   369,
     369,   369,   369,   370,   370,   371,   372,   372,   373,   373,
     373,   374,   374,   375,   375,   375,   375,   376,   376,   377,
     377
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     3,     1,     2,     1,     1,     0,     7,
       3,     1,     2,     3,     2,     1,     2,     2,     1,     1,
       4,     4,     0,     2,     3,     2,     3,     4,     1,     2,
       3,     3,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     4,     1,     3,     1,     1,
       1,     1,     3,     3,     0,     2,     3,     1,     1,     2,
       3,     3,     1,     1,     1,     3,     2,     1,     4,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,    28,     0,     2,     4,     6,     7,     0,     0,
       1,     5,    49,     0,     0,     0,    48,    50,    51,    46,
      70,    69,     0,    67,    25,    23,    62,    63,    64,     8,
       3,     0,    58,    55,    57,    32,     0,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    26,    24,     0,    66,     0,
       0,     0,    59,     0,    47,     0,    52,    30,    40,    39,
      44,    42,    41,    43,    38,    31,    34,    33,    36,    37,
      35,    29,    61,    27,    65,     0,    68,    60,    56,     0,
      45,    22,     0,    11,    53,    19,     0,    12,     0,     9,
       0,     0,    18,     0,     0,    13,    15,    10,     0,     0,
      17,    16,    14,    20,    21
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     3,     4,     5,     6,    59,    92,    93,   105,   106,
      97,     7,     8,    55,    23,    65,    24,    33,    34,    25,
      26,    27,    28
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -325
static const short yypact[] =
{
     -68,  -275,  -325,    19,  -325,  -313,  -325,  -325,   -44,  -285,
    -325,   -68,  -325,  -315,  -283,   -38,  -325,  -325,  -325,  -319,
    -325,  -325,   -38,   -15,  -325,  -125,  -325,   -21,  -325,  -325,
    -325,   -40,   -17,  -310,  -325,  -128,  -281,  -325,   -19,   -38,
     -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
     -38,   -38,   -38,   -38,   -42,  -325,  -294,  -279,  -325,  -307,
    -306,  -273,  -325,  -283,  -325,  -324,  -325,  -128,  -106,  -106,
    -101,  -106,  -106,  -101,  -106,    -1,  -322,  -322,  -325,  -325,
    -325,   -15,  -325,  -325,  -325,  -272,  -325,  -325,  -325,   -38,
    -325,     4,  -321,  -325,   -15,  -302,  -301,   -28,  -272,  -325,
    -227,  -218,  -325,  -157,  -116,   -28,  -325,  -325,  -293,  -292,
    -325,  -325,  -325,  -325,  -325
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -325,  -325,    43,  -325,  -325,  -325,  -325,   -43,  -325,   -49,
    -325,  -325,  -325,     1,    61,  -325,    33,  -325,    -4,  -325,
       7,    32,  -325
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yysigned_char yytable[] =
{
      39,    57,   102,     1,    39,    61,    12,    95,    12,    14,
      12,     9,    12,    40,    41,    50,    51,    89,    39,    10,
      98,    90,    42,    36,    99,    37,    52,    11,    29,    31,
      32,    63,    64,    53,    84,    -1,    -1,    85,   108,    86,
      87,    91,   100,   101,    42,    43,    44,   109,    45,    -1,
     110,   111,   113,   114,    30,   107,   112,    83,    56,    88,
      13,    82,    13,    60,    13,     0,     0,    -1,    -1,     0,
      45,     0,     0,     0,     0,    -1,    35,    46,     0,     0,
       0,     0,     0,    38,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,    -1,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,     0,     0,     0,     0,     0,
       0,     0,    40,    41,     0,     0,    40,    41,     0,     0,
       0,    42,     0,     0,     0,    42,     0,     0,     0,     0,
      40,    41,     0,     0,     0,     0,     0,     0,     0,    42,
      94,     0,     0,     0,    43,    44,     0,    45,    43,    44,
      15,    45,    15,    16,    15,    16,    15,    16,     0,    16,
       0,     0,    43,    44,     0,    45,   103,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,     0,     0,     0,
      46,     2,     0,     0,     0,     0,     0,     0,     0,    47,
       0,     0,    53,    47,    46,   104,     0,    48,    49,    50,
      51,     0,     0,     0,     0,     0,    54,     0,     0,     0,
      52,    17,    18,    17,    18,    17,    18,    17,    18,    48,
      49,    50,    51,     0,    48,    49,    50,    51,     0,     0,
       0,     0,    52,     0,     0,     0,     0,    52,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    19,
      20,    19,    20,    19,     0,    19,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    58,    21,     0,    21,    62,     0,     0,     0,
      22,     0,    22,     0,    22,     0,    22,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,    49,    50,    51,
      48,    49,    50,    51,    96,     0,    66,     0,     0,    52,
       0,     0,     0,    52,    48,    49,    50,    51,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    52
};

static const short yycheck[] =
{
      19,    22,    30,    71,    19,    22,    50,     3,    50,   134,
      50,   286,    50,   141,   142,   337,   338,   341,    19,     0,
     341,   345,   150,   342,   345,   344,   348,   340,   313,   344,
     313,   341,   313,   327,   313,   141,   142,   344,   265,   345,
     313,   313,   344,   344,   150,   173,   174,   265,   176,   150,
     207,   167,   345,   345,    11,    98,   105,    56,    25,    63,
     104,    54,   104,    31,   104,    -1,    -1,   173,   174,    -1,
     176,    -1,    -1,    -1,    -1,   176,    15,   205,    -1,    -1,
      -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,
     134,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   205,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   141,   142,    -1,    -1,   141,   142,    -1,    -1,
      -1,   150,    -1,    -1,    -1,   150,    -1,    -1,    -1,    -1,
     141,   142,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   150,
      89,    -1,    -1,    -1,   173,   174,    -1,   176,   173,   174,
     204,   176,   204,   207,   204,   207,   204,   207,    -1,   207,
      -1,    -1,   173,   174,    -1,   176,   204,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   205,    -1,    -1,    -1,
     205,   259,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   218,
      -1,    -1,   327,   218,   205,   233,    -1,   335,   336,   337,
     338,    -1,    -1,    -1,    -1,    -1,   341,    -1,    -1,    -1,
     348,   265,   266,   265,   266,   265,   266,   265,   266,   335,
     336,   337,   338,    -1,   335,   336,   337,   338,    -1,    -1,
      -1,    -1,   348,    -1,    -1,    -1,    -1,   348,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   313,
     314,   313,   314,   313,    -1,   313,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   313,   337,    -1,   337,   313,    -1,    -1,    -1,
     344,    -1,   344,    -1,   344,    -1,   344,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   335,   336,   337,   338,
     335,   336,   337,   338,   320,    -1,   345,    -1,    -1,   348,
      -1,    -1,    -1,   348,   335,   336,   337,   338,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   348
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,    71,   259,   356,   357,   358,   359,   366,   367,   286,
       0,   340,    50,   104,   134,   204,   207,   265,   266,   313,
     314,   337,   344,   369,   371,   374,   375,   376,   377,   313,
     357,   344,   313,   372,   373,   369,   342,   344,   369,    19,
     141,   142,   150,   173,   174,   176,   205,   218,   335,   336,
     337,   338,   348,   327,   341,   368,   371,    22,   313,   360,
     376,    22,   313,   341,   313,   370,   345,   369,   369,   369,
     369,   369,   369,   369,   369,   369,   369,   369,   369,   369,
     369,   369,   375,   368,   313,   344,   345,   313,   373,   341,
     345,   313,   361,   362,   369,     3,   320,   365,   341,   345,
     344,   344,    30,   204,   233,   363,   364,   362,   265,   265,
     207,   167,   364,   345,   345
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
#line 784 "sqlparser.y"
    {
//todo: multiple statements
//todo: not only "select" statements
	parser->setOperation(KexiDB::Parser::OP_Select);
	parser->setQuerySchema(yyvsp[0].querySchema);
;}
    break;

  case 3:
#line 794 "sqlparser.y"
    {
//todo: multiple statements
;}
    break;

  case 5:
#line 799 "sqlparser.y"
    {
	yyval.querySchema = yyvsp[-1].querySchema;
;}
    break;

  case 6:
#line 809 "sqlparser.y"
    {
YYACCEPT;
;}
    break;

  case 7:
#line 813 "sqlparser.y"
    {
	yyval.querySchema = yyvsp[0].querySchema;
;}
    break;

  case 8:
#line 820 "sqlparser.y"
    {
	parser->setOperation(KexiDB::Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
;}
    break;

  case 11:
#line 829 "sqlparser.y"
    {
;}
    break;

  case 12:
#line 835 "sqlparser.y"
    {
	kdDebug() << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

//	delete field;
	field = 0;
;}
    break;

  case 13:
#line 844 "sqlparser.y"
    {
	kdDebug() << "adding field " << yyvsp[-2].stringValue << endl;
	field->setName(yyvsp[-2].stringValue);
	parser->table()->addField(field);

//	if(field->isPrimaryKey())
//		parser->table()->addPrimaryKey(field->name());

//	delete field;
//	field = 0;
;}
    break;

  case 15:
#line 859 "sqlparser.y"
    {
;}
    break;

  case 16:
#line 865 "sqlparser.y"
    {
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
;}
    break;

  case 17:
#line 870 "sqlparser.y"
    {
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
;}
    break;

  case 18:
#line 875 "sqlparser.y"
    {
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
;}
    break;

  case 19:
#line 883 "sqlparser.y"
    {
	field = new KexiDB::Field();
	field->setType(yyvsp[0].colType);
;}
    break;

  case 20:
#line 888 "sqlparser.y"
    {
	kdDebug() << "sql + length" << endl;
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].colType);
;}
    break;

  case 21:
#line 895 "sqlparser.y"
    {
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(KexiDB::Field::Text);
;}
    break;

  case 22:
#line 901 "sqlparser.y"
    {
	// SQLITE compatibillity
	field = new KexiDB::Field();
	field->setType(KexiDB::Field::InvalidType);
;}
    break;

  case 23:
#line 910 "sqlparser.y"
    {
	kdDebug() << "Select ColViews=" << yyvsp[0].exprList->debugString() << endl;
	
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
;}
    break;

  case 24:
#line 930 "sqlparser.y"
    {
//TODO: move this to all SELECT versions
	
	KexiDB::QuerySchema* querySchema = yyvsp[-2].querySchema;
	yyval.querySchema = querySchema;
	
	//-------tables list
	KexiDB::NArgExpr *tablesList = yyvsp[0].exprList;
	assert( tablesList ); //&& tablesList->exprClass() == KexiDBExpr_TableList );

	uint columnNum = 0;
/*TODO: use this later if there are columns that use database fields, 
        e.g. "SELECT 1 from table1 t, table2 t") is ok however. */
	//used to collect information about first repeated table name or alias:
//	QDict<char> tableNamesAndTableAliases(997, false);
//	QString repeatedTableNameOrTableAlias;
	//collects positions of tables/aliases with the same names
	QDict< QValueList<int> > repeatedTablesAndAliases(997, false);
	repeatedTablesAndAliases.setAutoDelete(true);
	
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
		KexiDB::TableSchema *s = parser->db()->tableSchema(tname);
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
		QValueList<int> *list = repeatedTablesAndAliases[tableOrAliasName];
		if (list) {
			//another table/alias with the same name
			list->append( i );
			kdDebug() << "- another table/alias with name: " << tableOrAliasName << endl;
		}
		else {
			list = new QValueList<int>();
			list->append( i );
			repeatedTablesAndAliases.insert( tableOrAliasName, list );
			kdDebug() << "- first table/alias with name: " << tableOrAliasName << endl;
		}
/*		if (repeatedTableNameOrTableAlias.isEmpty()) {
			if (tableNamesAndTableAliases[tname])
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
				if (tableNamesAndTableAliases[aliasString])
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
	KexiDB::BaseExpr *e;
	columnNum = 0;
	for (KexiDB::BaseExpr::ListIterator it(yyvsp[-1].exprList->list); (e = it.current()); ++it, columnNum++)
	{
		BaseExpr *columnVariable;
		if (e->exprClass() == KexiDBExpr_SpecialBinary && dynamic_cast<BinaryExpr*>(e)) {
			columnVariable = dynamic_cast<BinaryExpr*>(e)->left();
		}
		else if (e->exprClass() == KexiDBExpr_Variable) {
			columnVariable = e;
		}
		else {
			setError(i18n("Invalid column definition")); //ok?
			return 0;
		}

		if (!addColumn( querySchema, columnVariable, repeatedTablesAndAliases ))
			return 0;
		
		if (e->exprClass() == KexiDBExpr_SpecialBinary && dynamic_cast<BinaryExpr*>(e)
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
		}
	}
	kdDebug() << "Select ColViews=" << yyvsp[-1].exprList->debugString() 
		<< " Tables=" << yyvsp[0].exprList->debugString() << endl;
	
	delete yyvsp[-1].exprList; //no longer needed
;}
    break;

  case 25:
#line 1064 "sqlparser.y"
    {
	kdDebug() << "Select ColViews Tables" << endl;
;}
    break;

  case 26:
#line 1068 "sqlparser.y"
    {
	kdDebug() << "Select ColViews Conditions" << endl;
;}
    break;

  case 27:
#line 1072 "sqlparser.y"
    {
	kdDebug() << "Select ColViews Tables Conditions" << endl;
;}
    break;

  case 28:
#line 1079 "sqlparser.y"
    {
	kdDebug() << "SELECT" << endl;
//	parser->createSelect();
//	parser->setOperation(KexiDB::Parser::OP_Select);
	yyval.querySchema = new QuerySchema();
;}
    break;

  case 29:
#line 1089 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
;}
    break;

  case 30:
#line 1100 "sqlparser.y"
    {
//	kdDebug() << "AND " << $3.debugString() << endl;
	yyval.expr = new KexiDB::BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, AND, yyvsp[0].expr );
;}
    break;

  case 31:
#line 1105 "sqlparser.y"
    {
//	kdDebug() << "OR " << $3 << endl;
	yyval.expr = new KexiDB::BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, OR, yyvsp[0].expr );
;}
    break;

  case 32:
#line 1110 "sqlparser.y"
    {
	yyval.expr = new KexiDB::UnaryExpr( NOT, yyvsp[0].expr );
//	$$->setName($1->name() + " NOT " + $3->name());
;}
    break;

  case 33:
#line 1115 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '+', yyvsp[0].expr);
;}
    break;

  case 34:
#line 1119 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '-', yyvsp[0].expr);
;}
    break;

  case 35:
#line 1123 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '/', yyvsp[0].expr);
;}
    break;

  case 36:
#line 1127 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '*', yyvsp[0].expr);
;}
    break;

  case 37:
#line 1131 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '%', yyvsp[0].expr);
;}
    break;

  case 38:
#line 1135 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL, yyvsp[0].expr);
;}
    break;

  case 39:
#line 1139 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_THAN, yyvsp[0].expr);
;}
    break;

  case 40:
#line 1143 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_OR_EQUAL, yyvsp[0].expr);
;}
    break;

  case 41:
#line 1147 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_THAN, yyvsp[0].expr);
;}
    break;

  case 42:
#line 1151 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_OR_EQUAL, yyvsp[0].expr);
;}
    break;

  case 43:
#line 1155 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LIKE, yyvsp[0].expr);
;}
    break;

  case 44:
#line 1159 "sqlparser.y"
    {
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SQL_IN, yyvsp[0].expr);
;}
    break;

  case 45:
#line 1163 "sqlparser.y"
    {
	kdDebug() << "  + function: " << yyvsp[-3].stringValue << "(" << yyvsp[-1].exprList->debugString() << ")" << endl;
	yyval.expr = new KexiDB::FunctionExpr(yyvsp[-3].stringValue, yyvsp[-1].exprList);
;}
    break;

  case 46:
#line 1168 "sqlparser.y"
    {
	yyval.expr = new KexiDB::VariableExpr( QString::fromLatin1(yyvsp[0].stringValue) );
	
//TODO: simplify this later if that's 'only one field name' expression
	kdDebug() << "  + identifier: " << yyvsp[0].stringValue << endl;
//	$$ = new KexiDB::Field();
//	$$->setName($1);
//	$$->setTable(dummy);

//	parser->select()->addField(field);
	requiresTable = true;
;}
    break;

  case 47:
#line 1182 "sqlparser.y"
    {
	yyval.expr = new KexiDB::VariableExpr( QString::fromLatin1(yyvsp[-2].stringValue) + "." + QString::fromLatin1(yyvsp[0].stringValue) );
	kdDebug() << "  + identifier.identifier: " << yyvsp[0].stringValue << "." << yyvsp[-2].stringValue << endl;
//	$$ = new KexiDB::Field();
//	s->setTable($1);
//	$$->setName($3);
	//$$->setTable(parser->db()->tableSchema($1));
//	parser->select()->addField(field);
//??	requiresTable = true;
;}
    break;

  case 48:
#line 1193 "sqlparser.y"
    {
	yyval.expr = new KexiDB::ConstExpr( SQL_NULL, QVariant() );
	kdDebug() << "  + NULL" << endl;
//	$$ = new KexiDB::Field();
	//$$->setName(QString::null);
;}
    break;

  case 49:
#line 1200 "sqlparser.y"
    {
	yyval.expr = new KexiDB::ConstExpr( CHARACTER_STRING_LITERAL, yyvsp[0].stringValue );
//	$$ = new KexiDB::Field();
//	$$->setName($1);
//	parser->select()->addField(field);
	kdDebug() << "  + constant \"" << yyvsp[0].stringValue << "\"" << endl;
;}
    break;

  case 50:
#line 1208 "sqlparser.y"
    {
	yyval.expr = new KexiDB::ConstExpr( INTEGER_CONST, yyvsp[0].integerValue );
//	$$ = new KexiDB::Field();
//	$$->setName(QString::number($1));
//	parser->select()->addField(field);
	kdDebug() << "  + int constant: " << yyvsp[0].integerValue << endl;
;}
    break;

  case 51:
#line 1216 "sqlparser.y"
    {
	yyval.expr = new KexiDB::ConstExpr( REAL_CONST, QPoint( yyvsp[0].realValue.integer, yyvsp[0].realValue.fractional ) );
	kdDebug() << "  + real constant: " << yyvsp[0].realValue.integer << "." << yyvsp[0].realValue.fractional << endl;
;}
    break;

  case 52:
#line 1221 "sqlparser.y"
    {
	kdDebug() << "(expr)" << endl;
	yyval.expr = yyvsp[-1].expr;
;}
    break;

  case 53:
#line 1229 "sqlparser.y"
    {
	yyvsp[-2].exprList->add( yyvsp[0].expr );
	yyval.exprList = yyvsp[-2].exprList;
;}
    break;

  case 54:
#line 1234 "sqlparser.y"
    {
	yyval.exprList = new KexiDB::NArgExpr(0, 0/*unknown*/);
;}
    break;

  case 55:
#line 1241 "sqlparser.y"
    {
	yyval.exprList = yyvsp[0].exprList;
;}
    break;

  case 56:
#line 1282 "sqlparser.y"
    {
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add(yyvsp[0].expr);
;}
    break;

  case 57:
#line 1287 "sqlparser.y"
    {
	yyval.exprList = new KexiDB::NArgExpr(KexiDBExpr_TableList, IDENTIFIER); //ok?
	yyval.exprList->add(yyvsp[0].expr);
;}
    break;

  case 58:
#line 1295 "sqlparser.y"
    {
	kdDebug() << "FROM: '" << yyvsp[0].stringValue << "'" << endl;

//	KexiDB::TableSchema *schema = parser->db()->tableSchema($1);
//	parser->select()->setParentTable(schema);
//	parser->select()->addTable(schema);
//	requiresTable = false;
	
//addTable($1);

	yyval.expr = new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue));

	/*
//TODO: this isn't ok for more tables:
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
	}*/
;}
    break;

  case 59:
#line 1330 "sqlparser.y"
    {
	//table + alias
	yyval.expr = new KexiDB::BinaryExpr(
		KexiDBExpr_SpecialBinary, 
		new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[-1].stringValue)), 0,
		new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
;}
    break;

  case 60:
#line 1339 "sqlparser.y"
    {
	//table + alias
	yyval.expr = new KexiDB::BinaryExpr(
		KexiDBExpr_SpecialBinary,
		new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[-2].stringValue)), AS,
		new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
;}
    break;

  case 61:
#line 1353 "sqlparser.y"
    {
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColViews , ColItem" << endl;
;}
    break;

  case 62:
#line 1359 "sqlparser.y"
    {
	yyval.exprList = new KexiDB::NArgExpr(0,0);
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColItem" << endl;
;}
    break;

  case 63:
#line 1368 "sqlparser.y"
    {
//	$$ = new KexiDB::Field();
//	dummy->addField($$);
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column expr: '" << yyvsp[0].expr->debugString() << "'" << endl;
;}
    break;

  case 64:
#line 1377 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column wildcard: '" << yyvsp[0].expr->debugString() << "'" << endl;
;}
    break;

  case 65:
#line 1382 "sqlparser.y"
    {
//	$$ = new KexiDB::Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = new KexiDB::BinaryExpr(
		KexiDBExpr_SpecialBinary, yyvsp[-2].expr, AS,
		new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
//		new KexiDB::ConstExpr(IDENTIFIER, QString::fromLocal8Bit($3))
	);
	kdDebug() << " added column expr: " << yyval.expr->debugString() << endl;
;}
    break;

  case 66:
#line 1394 "sqlparser.y"
    {
//	$$ = new KexiDB::Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = new KexiDB::BinaryExpr(
		KexiDBExpr_SpecialBinary, yyvsp[-1].expr, 0, 
		new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
//		new KexiDB::ConstExpr(IDENTIFIER, QString::fromLocal8Bit($2))
	);
	kdDebug() << " added column expr: " << yyval.expr->debugString() << endl;
;}
    break;

  case 67:
#line 1409 "sqlparser.y"
    {
	yyval.expr = yyvsp[0].expr;
;}
    break;

  case 68:
#line 1447 "sqlparser.y"
    {
	yyval.expr = yyvsp[-1].expr;
//TODO
//	$$->setName("DISTINCT(" + $3->name() + ")");
;}
    break;

  case 69:
#line 1456 "sqlparser.y"
    {
	yyval.expr = new KexiDB::VariableExpr("*");
	kdDebug() << "all columns" << endl;

//	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), dummy);
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
;}
    break;

  case 70:
#line 1465 "sqlparser.y"
    {
	yyval.expr = new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[0].stringValue));
	kdDebug() << "  + all columns from " << yyvsp[0].stringValue << endl;
//	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), parser->db()->tableSchema($1));
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
;}
    break;


    }

/* Line 999 of yacc.c.  */
#line 2979 "sqlparser.tab.c"

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


#line 782 "sqlparser.y"



