
/*  A Bison parser, made from sqlparser.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	SQL_TYPE	257
#define	SQL_ABS	258
#define	ACOS	259
#define	AMPERSAND	260
#define	SQL_ABSOLUTE	261
#define	ADA	262
#define	ADD	263
#define	ADD_DAYS	264
#define	ADD_HOURS	265
#define	ADD_MINUTES	266
#define	ADD_MONTHS	267
#define	ADD_SECONDS	268
#define	ADD_YEARS	269
#define	ALL	270
#define	ALLOCATE	271
#define	ALTER	272
#define	AND	273
#define	ANY	274
#define	ARE	275
#define	AS	276
#define	ASIN	277
#define	ASC	278
#define	ASCII	279
#define	ASSERTION	280
#define	ATAN	281
#define	ATAN2	282
#define	AUTHORIZATION	283
#define	AUTO_INCREMENT	284
#define	AVG	285
#define	BEFORE	286
#define	SQL_BEGIN	287
#define	BETWEEN	288
#define	BIGINT	289
#define	BINARY	290
#define	BIT	291
#define	BIT_LENGTH	292
#define	BREAK	293
#define	BY	294
#define	CASCADE	295
#define	CASCADED	296
#define	CASE	297
#define	CAST	298
#define	CATALOG	299
#define	CEILING	300
#define	CENTER	301
#define	SQL_CHAR	302
#define	CHAR_LENGTH	303
#define	CHARACTER_STRING_LITERAL	304
#define	CHECK	305
#define	CLOSE	306
#define	COALESCE	307
#define	COBOL	308
#define	COLLATE	309
#define	COLLATION	310
#define	COLUMN	311
#define	COMMIT	312
#define	COMPUTE	313
#define	CONCAT	314
#define	CONNECT	315
#define	CONNECTION	316
#define	CONSTRAINT	317
#define	CONSTRAINTS	318
#define	CONTINUE	319
#define	CONVERT	320
#define	CORRESPONDING	321
#define	COS	322
#define	COT	323
#define	COUNT	324
#define	CREATE	325
#define	CURDATE	326
#define	CURRENT	327
#define	CURRENT_DATE	328
#define	CURRENT_TIME	329
#define	CURRENT_TIMESTAMP	330
#define	CURTIME	331
#define	CURSOR	332
#define	DATABASE	333
#define	SQL_DATE	334
#define	DATE_FORMAT	335
#define	DATE_REMAINDER	336
#define	DATE_VALUE	337
#define	DAY	338
#define	DAYOFMONTH	339
#define	DAYOFWEEK	340
#define	DAYOFYEAR	341
#define	DAYS_BETWEEN	342
#define	DEALLOCATE	343
#define	DEC	344
#define	DECLARE	345
#define	DEFAULT	346
#define	DEFERRABLE	347
#define	DEFERRED	348
#define	SQL_DELETE	349
#define	DESC	350
#define	DESCRIBE	351
#define	DESCRIPTOR	352
#define	DIAGNOSTICS	353
#define	DICTIONARY	354
#define	DIRECTORY	355
#define	DISCONNECT	356
#define	DISPLACEMENT	357
#define	DISTINCT	358
#define	DOMAIN_TOKEN	359
#define	SQL_DOUBLE	360
#define	DOUBLE_QUOTED_STRING	361
#define	DROP	362
#define	ELSE	363
#define	END	364
#define	END_EXEC	365
#define	EQUAL	366
#define	ESCAPE	367
#define	EXCEPT	368
#define	SQL_EXCEPTION	369
#define	EXEC	370
#define	EXECUTE	371
#define	EXISTS	372
#define	EXP	373
#define	EXPONENT	374
#define	EXTERNAL	375
#define	EXTRACT	376
#define	SQL_FALSE	377
#define	FETCH	378
#define	FIRST	379
#define	SQL_FLOAT	380
#define	FLOOR	381
#define	FN	382
#define	FOR	383
#define	FOREIGN	384
#define	FORTRAN	385
#define	FOUND	386
#define	FOUR_DIGITS	387
#define	FROM	388
#define	FULL	389
#define	GET	390
#define	GLOBAL	391
#define	GO	392
#define	GOTO	393
#define	GRANT	394
#define	GREATER_OR_EQUAL	395
#define	GREATER_THAN	396
#define	GROUP	397
#define	HAVING	398
#define	HOUR	399
#define	HOURS_BETWEEN	400
#define	IDENTITY	401
#define	IFNULL	402
#define	SQL_IGNORE	403
#define	IMMEDIATE	404
#define	SQL_IN	405
#define	INCLUDE	406
#define	INDEX	407
#define	INDICATOR	408
#define	INITIALLY	409
#define	INNER	410
#define	INPUT	411
#define	INSENSITIVE	412
#define	INSERT	413
#define	INTEGER	414
#define	INTERSECT	415
#define	INTERVAL	416
#define	INTO	417
#define	IS	418
#define	ISOLATION	419
#define	JOIN	420
#define	JUSTIFY	421
#define	KEY	422
#define	LANGUAGE	423
#define	LAST	424
#define	LCASE	425
#define	LEFT	426
#define	LENGTH	427
#define	LESS_OR_EQUAL	428
#define	LESS_THAN	429
#define	LEVEL	430
#define	LIKE	431
#define	LINE_WIDTH	432
#define	LOCAL	433
#define	LOCATE	434
#define	LOG	435
#define	SQL_LONG	436
#define	LOWER	437
#define	LTRIM	438
#define	LTRIP	439
#define	MATCH	440
#define	SQL_MAX	441
#define	MICROSOFT	442
#define	SQL_MIN	443
#define	MINUS	444
#define	MINUTE	445
#define	MINUTES_BETWEEN	446
#define	MOD	447
#define	MODIFY	448
#define	MODULE	449
#define	MONTH	450
#define	MONTHS_BETWEEN	451
#define	MUMPS	452
#define	NAMES	453
#define	NATIONAL	454
#define	NCHAR	455
#define	NEXT	456
#define	NODUP	457
#define	NONE	458
#define	NOT	459
#define	NOT_EQUAL	460
#define	NOW	461
#define	SQL_NULL	462
#define	NULLIF	463
#define	NUMERIC	464
#define	OCTET_LENGTH	465
#define	ODBC	466
#define	OF	467
#define	SQL_OFF	468
#define	SQL_ON	469
#define	ONLY	470
#define	OPEN	471
#define	OPTION	472
#define	OR	473
#define	ORDER	474
#define	OUTER	475
#define	OUTPUT	476
#define	OVERLAPS	477
#define	PAGE	478
#define	PARTIAL	479
#define	SQL_PASCAL	480
#define	PERSISTENT	481
#define	CQL_PI	482
#define	PLI	483
#define	POSITION	484
#define	PRECISION	485
#define	PREPARE	486
#define	PRESERVE	487
#define	PRIMARY	488
#define	PRIOR	489
#define	PRIVILEGES	490
#define	PROCEDURE	491
#define	PRODUCT	492
#define	PUBLIC	493
#define	QUARTER	494
#define	QUIT	495
#define	RAND	496
#define	READ_ONLY	497
#define	REAL	498
#define	REFERENCES	499
#define	REPEAT	500
#define	REPLACE	501
#define	RESTRICT	502
#define	REVOKE	503
#define	RIGHT	504
#define	ROLLBACK	505
#define	ROWS	506
#define	RPAD	507
#define	RTRIM	508
#define	SCHEMA	509
#define	SCREEN_WIDTH	510
#define	SCROLL	511
#define	SECOND	512
#define	SECONDS_BETWEEN	513
#define	SELECT	514
#define	SEQUENCE	515
#define	SETOPT	516
#define	SET	517
#define	SHOWOPT	518
#define	SIGN	519
#define	INTEGER_CONST	520
#define	REAL_CONST	521
#define	SIN	522
#define	SQL_SIZE	523
#define	SMALLINT	524
#define	SOME	525
#define	SPACE	526
#define	SQL	527
#define	SQL_TRUE	528
#define	SQLCA	529
#define	SQLCODE	530
#define	SQLERROR	531
#define	SQLSTATE	532
#define	SQLWARNING	533
#define	SQRT	534
#define	STDEV	535
#define	SUBSTRING	536
#define	SUM	537
#define	SYSDATE	538
#define	SYSDATE_FORMAT	539
#define	SYSTEM	540
#define	TABLE	541
#define	TAN	542
#define	TEMPORARY	543
#define	THEN	544
#define	THREE_DIGITS	545
#define	TIME	546
#define	TIMESTAMP	547
#define	TIMEZONE_HOUR	548
#define	TIMEZONE_MINUTE	549
#define	TINYINT	550
#define	TO	551
#define	TO_CHAR	552
#define	TO_DATE	553
#define	TRANSACTION	554
#define	TRANSLATE	555
#define	TRANSLATION	556
#define	TRUNCATE	557
#define	GENERAL_TITLE	558
#define	TWO_DIGITS	559
#define	UCASE	560
#define	UNION	561
#define	UNIQUE	562
#define	SQL_UNKNOWN	563
#define	UPDATE	564
#define	UPPER	565
#define	USAGE	566
#define	USER	567
#define	USER_DEFINED_NAME	568
#define	USER_DEFINED_NAME_DOT_ASTERISK	569
#define	USING	570
#define	VALUE	571
#define	VALUES	572
#define	VARBINARY	573
#define	VARCHAR	574
#define	VARYING	575
#define	VENDOR	576
#define	VIEW	577
#define	WEEK	578
#define	WHEN	579
#define	WHENEVER	580
#define	WHERE	581
#define	WHERE_CURRENT_OF	582
#define	WITH	583
#define	WORD_WRAPPED	584
#define	WORK	585
#define	WRAPPED	586
#define	YEAR	587
#define	YEARS_BETWEEN	588
#define	ILIKE	589
#define	SIMILAR	590

#line 394 "sqlparser.y"

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
//	QPtrList<KexiDB::TableSchema> tableList;
	QDict<KexiDB::TableSchema> tableDict;
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

	void setError(const QString& errName, const QString& errDesc)
	{
		parser->setError( KexiDB::ParserError(errName, errDesc, ctoken, current) );
		yyerror(errName.latin1());
	}

	void tableNotFoundError(const QString& tableName)
	{
		setError( i18n("Table not found"), i18n("Unknown table \"%1\"").arg(tableName) );
	}

	bool parseData(KexiDB::Parser *p, const char *data)
	{
		if (!dummy)
			dummy = new KexiDB::TableSchema();
		parser = p;
		parser->clear();
		field = 0;
		fieldList.clear();
		requiresTable = false;

		if (!data) {
			KexiDB::ParserError err(i18n("Error"), i18n("No query specified"), ctoken, current);
			parser->setError(err);
			yyerror("");
			return false;
		}
	
		tokenize(data);
		yyparse();

		bool ok = true;
		if(parser->operation() == KexiDB::Parser::OP_Select)
		{
			kdDebug() << "parseData(): " << tableDict.count() << " loaded tables" << endl;
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
			parser->select()->removeTable(dummy);
		}
		else {
			ok = false;
		}

		tableDict.clear();
		return ok;
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
			tableDict.insert(s->name(), s);
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

#line 558 "sqlparser.y"
typedef union {
	char stringValue[255];
	int integerValue;
	struct realType realValue;
	KexiDB::Field::Type coltype;
	KexiDB::Field *field;
	KexiDB::BaseExpr *expr;
	KexiDB::NArgExpr *exprlist;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		136
#define	YYFLAG		-32768
#define	YYNTBASE	355

#define YYTRANSLATE(x) ((unsigned)(x) <= 590 ? yytranslate[x] : 377)

static const short yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,   343,   338,     2,   347,   344,
   345,   337,   336,   341,   335,   342,   348,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,   340,     2,
   349,     2,   346,   339,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
   353,     2,   354,   352,     2,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
    87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
    97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
   117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
   127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
   157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
   167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
   177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
   187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
   197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
   207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
   217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
   227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
   237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
   247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
   257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
   267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
   277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
   287,   288,   289,   290,   291,   292,   293,   294,   295,   296,
   297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
   307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
   317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
   327,   328,   329,   330,   331,   332,   333,   334,   350,   351
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     5,     8,    11,    12,    13,    21,    25,    27,
    30,    34,    37,    39,    42,    45,    47,    49,    54,    59,
    60,    63,    67,    70,    74,    79,    81,    84,    88,    92,
    95,    99,   103,   107,   111,   115,   119,   123,   127,   131,
   135,   139,   143,   148,   150,   154,   156,   158,   160,   162,
   166,   170,   171,   174,   181,   189,   196,   203,   211,   213,
   217,   219,   221,   225,   227,   229,   231,   235,   238,   240,
   245,   247
};

static const short yyrhs[] = {   356,
   340,     0,   356,     0,   356,   357,     0,   356,   364,     0,
     0,     0,    71,   287,   314,   358,   344,   359,   345,     0,
   359,   341,   360,     0,   360,     0,   314,   363,     0,   314,
   363,   361,     0,   361,   362,     0,   362,     0,   234,   168,
     0,   205,   208,     0,    30,     0,     3,     0,     3,   344,
   266,   345,     0,   320,   344,   266,   345,     0,     0,   365,
   373,     0,   365,   373,   369,     0,   365,   369,     0,   365,
   373,   366,     0,   365,   373,   369,   366,     0,   260,     0,
   327,   367,     0,   367,    19,   367,     0,   367,   219,   367,
     0,   205,   367,     0,   367,   336,   367,     0,   367,   335,
   367,     0,   367,   348,   367,     0,   367,   337,   367,     0,
   367,   338,   367,     0,   367,   206,   367,     0,   367,   142,
   367,     0,   367,   141,   367,     0,   367,   175,   367,     0,
   367,   174,   367,     0,   367,   177,   367,     0,   367,   151,
   367,     0,   314,   344,   368,   345,     0,   314,     0,   314,
   342,   314,     0,   208,     0,    50,     0,   266,     0,   267,
     0,   344,   367,   345,     0,   368,   341,   367,     0,     0,
   134,   370,     0,   369,   172,   166,   314,   215,   375,     0,
   369,   172,   221,   166,   314,   215,   375,     0,   369,   156,
   166,   314,   215,   375,     0,   369,   250,   166,   314,   215,
   375,     0,   369,   250,   221,   166,   314,   215,   375,     0,
   371,     0,   371,   341,   372,     0,   372,     0,   314,     0,
   373,   341,   374,     0,   374,     0,   375,     0,   376,     0,
   375,    22,   314,     0,   375,   314,     0,   367,     0,   104,
   344,   375,   345,     0,   337,     0,   315,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   615,   617,   620,   622,   623,   626,   632,   635,   636,   641,
   651,   665,   666,   671,   677,   682,   689,   695,   702,   708,
   716,   736,   808,   812,   816,   822,   831,   842,   848,   853,
   858,   862,   866,   870,   874,   878,   882,   886,   890,   894,
   898,   902,   906,   911,   925,   936,   943,   951,   959,   964,
   971,   977,   983,   987,   992,   997,  1002,  1007,  1014,  1020,
  1021,  1026,  1063,  1070,  1078,  1088,  1093,  1102,  1113,  1152,
  1160,  1170
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","SQL_TYPE",
"SQL_ABS","ACOS","AMPERSAND","SQL_ABSOLUTE","ADA","ADD","ADD_DAYS","ADD_HOURS",
"ADD_MINUTES","ADD_MONTHS","ADD_SECONDS","ADD_YEARS","ALL","ALLOCATE","ALTER",
"AND","ANY","ARE","AS","ASIN","ASC","ASCII","ASSERTION","ATAN","ATAN2","AUTHORIZATION",
"AUTO_INCREMENT","AVG","BEFORE","SQL_BEGIN","BETWEEN","BIGINT","BINARY","BIT",
"BIT_LENGTH","BREAK","BY","CASCADE","CASCADED","CASE","CAST","CATALOG","CEILING",
"CENTER","SQL_CHAR","CHAR_LENGTH","CHARACTER_STRING_LITERAL","CHECK","CLOSE",
"COALESCE","COBOL","COLLATE","COLLATION","COLUMN","COMMIT","COMPUTE","CONCAT",
"CONNECT","CONNECTION","CONSTRAINT","CONSTRAINTS","CONTINUE","CONVERT","CORRESPONDING",
"COS","COT","COUNT","CREATE","CURDATE","CURRENT","CURRENT_DATE","CURRENT_TIME",
"CURRENT_TIMESTAMP","CURTIME","CURSOR","DATABASE","SQL_DATE","DATE_FORMAT","DATE_REMAINDER",
"DATE_VALUE","DAY","DAYOFMONTH","DAYOFWEEK","DAYOFYEAR","DAYS_BETWEEN","DEALLOCATE",
"DEC","DECLARE","DEFAULT","DEFERRABLE","DEFERRED","SQL_DELETE","DESC","DESCRIBE",
"DESCRIPTOR","DIAGNOSTICS","DICTIONARY","DIRECTORY","DISCONNECT","DISPLACEMENT",
"DISTINCT","DOMAIN_TOKEN","SQL_DOUBLE","DOUBLE_QUOTED_STRING","DROP","ELSE",
"END","END_EXEC","EQUAL","ESCAPE","EXCEPT","SQL_EXCEPTION","EXEC","EXECUTE",
"EXISTS","EXP","EXPONENT","EXTERNAL","EXTRACT","SQL_FALSE","FETCH","FIRST","SQL_FLOAT",
"FLOOR","FN","FOR","FOREIGN","FORTRAN","FOUND","FOUR_DIGITS","FROM","FULL","GET",
"GLOBAL","GO","GOTO","GRANT","GREATER_OR_EQUAL","GREATER_THAN","GROUP","HAVING",
"HOUR","HOURS_BETWEEN","IDENTITY","IFNULL","SQL_IGNORE","IMMEDIATE","SQL_IN",
"INCLUDE","INDEX","INDICATOR","INITIALLY","INNER","INPUT","INSENSITIVE","INSERT",
"INTEGER","INTERSECT","INTERVAL","INTO","IS","ISOLATION","JOIN","JUSTIFY","KEY",
"LANGUAGE","LAST","LCASE","LEFT","LENGTH","LESS_OR_EQUAL","LESS_THAN","LEVEL",
"LIKE","LINE_WIDTH","LOCAL","LOCATE","LOG","SQL_LONG","LOWER","LTRIM","LTRIP",
"MATCH","SQL_MAX","MICROSOFT","SQL_MIN","MINUS","MINUTE","MINUTES_BETWEEN","MOD",
"MODIFY","MODULE","MONTH","MONTHS_BETWEEN","MUMPS","NAMES","NATIONAL","NCHAR",
"NEXT","NODUP","NONE","NOT","NOT_EQUAL","NOW","SQL_NULL","NULLIF","NUMERIC",
"OCTET_LENGTH","ODBC","OF","SQL_OFF","SQL_ON","ONLY","OPEN","OPTION","OR","ORDER",
"OUTER","OUTPUT","OVERLAPS","PAGE","PARTIAL","SQL_PASCAL","PERSISTENT","CQL_PI",
"PLI","POSITION","PRECISION","PREPARE","PRESERVE","PRIMARY","PRIOR","PRIVILEGES",
"PROCEDURE","PRODUCT","PUBLIC","QUARTER","QUIT","RAND","READ_ONLY","REAL","REFERENCES",
"REPEAT","REPLACE","RESTRICT","REVOKE","RIGHT","ROLLBACK","ROWS","RPAD","RTRIM",
"SCHEMA","SCREEN_WIDTH","SCROLL","SECOND","SECONDS_BETWEEN","SELECT","SEQUENCE",
"SETOPT","SET","SHOWOPT","SIGN","INTEGER_CONST","REAL_CONST","SIN","SQL_SIZE",
"SMALLINT","SOME","SPACE","SQL","SQL_TRUE","SQLCA","SQLCODE","SQLERROR","SQLSTATE",
"SQLWARNING","SQRT","STDEV","SUBSTRING","SUM","SYSDATE","SYSDATE_FORMAT","SYSTEM",
"TABLE","TAN","TEMPORARY","THEN","THREE_DIGITS","TIME","TIMESTAMP","TIMEZONE_HOUR",
"TIMEZONE_MINUTE","TINYINT","TO","TO_CHAR","TO_DATE","TRANSACTION","TRANSLATE",
"TRANSLATION","TRUNCATE","GENERAL_TITLE","TWO_DIGITS","UCASE","UNION","UNIQUE",
"SQL_UNKNOWN","UPDATE","UPPER","USAGE","USER","USER_DEFINED_NAME","USER_DEFINED_NAME_DOT_ASTERISK",
"USING","VALUE","VALUES","VARBINARY","VARCHAR","VARYING","VENDOR","VIEW","WEEK",
"WHEN","WHENEVER","WHERE","WHERE_CURRENT_OF","WITH","WORD_WRAPPED","WORK","WRAPPED",
"YEAR","YEARS_BETWEEN","'-'","'+'","'*'","'%'","'@'","';'","','","'.'","'$'",
"'('","')'","'?'","'\\''","'/'","'='","ILIKE","SIMILAR","'^'","'['","']'","TopLevelStatement",
"Statement","CreateTableStatement","@1","ColDefs","ColDef","ColKeys","ColKey",
"ColType","SelectStatement","Select","WhereClause","aExpr","aExprList","Tables",
"FlatTableList","aFlatTableList","FlatTable","ColViews","ColItem","ColExpression",
"ColWildCard", NULL
};
#endif

static const short yyr1[] = {     0,
   355,   355,   356,   356,   356,   358,   357,   359,   359,   360,
   360,   361,   361,   362,   362,   362,   363,   363,   363,   363,
   364,   364,   364,   364,   364,   365,   366,   367,   367,   367,
   367,   367,   367,   367,   367,   367,   367,   367,   367,   367,
   367,   367,   367,   367,   367,   367,   367,   367,   367,   367,
   368,   368,   369,   369,   369,   369,   369,   369,   370,   371,
   371,   372,   373,   373,   374,   374,   374,   374,   375,   375,
   376,   376
};

static const short yyr2[] = {     0,
     2,     1,     2,     2,     0,     0,     7,     3,     1,     2,
     3,     2,     1,     2,     2,     1,     1,     4,     4,     0,
     2,     3,     2,     3,     4,     1,     2,     3,     3,     2,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     4,     1,     3,     1,     1,     1,     1,     3,
     3,     0,     2,     6,     7,     6,     6,     7,     1,     3,
     1,     1,     3,     1,     1,     1,     3,     2,     1,     4,
     1,     1
};

static const short yydefact[] = {     5,
     2,     0,    26,     1,     3,     4,     0,     0,    47,     0,
     0,     0,    46,    48,    49,    44,    72,    71,     0,    69,
    23,    21,    64,    65,    66,     6,     0,    62,    53,    59,
    61,    30,     0,    52,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    24,    22,     0,    68,     0,     0,
     0,    45,     0,    50,    28,    38,    37,    42,    40,    39,
    41,    36,    29,    32,    31,    34,    35,    33,     0,     0,
     0,     0,     0,    27,    63,    25,    67,     0,    70,    60,
     0,    43,     0,     0,     0,     0,     0,    20,     0,     9,
    51,     0,     0,     0,     0,     0,    17,     0,    10,     0,
     7,    56,    54,     0,    57,     0,     0,     0,    16,     0,
     0,    11,    13,     8,    55,    58,     0,     0,    15,    14,
    12,    18,    19,     0,     0,     0
};

static const short yydefgoto[] = {   134,
     1,     5,    59,    99,   100,   122,   123,   109,     6,     7,
    55,    20,    63,    21,    29,    30,    31,    22,    23,    24,
    25
};

static const short yypact[] = {-32768,
   -62,  -271,-32768,-32768,-32768,-32768,   -44,  -282,-32768,  -311,
  -280,   -38,-32768,-32768,-32768,  -314,-32768,-32768,   -38,   -15,
  -153,  -110,-32768,   -20,-32768,-32768,   -40,-32768,-32768,  -306,
-32768,  -128,  -278,-32768,   -19,   -38,   -38,   -38,   -38,   -38,
   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,  -129,
  -165,  -151,   -38,   -42,-32768,  -145,  -274,-32768,  -303,  -302,
  -280,-32768,  -319,-32768,  -128,  -103,  -103,   -98,  -103,  -103,
   -98,  -103,    -1,  -317,  -317,-32768,-32768,-32768,  -272,  -270,
  -121,  -264,  -115,   -15,-32768,-32768,-32768,  -262,-32768,-32768,
   -38,-32768,  -161,  -160,  -257,  -157,  -255,     4,  -316,-32768,
   -15,   -40,   -40,  -154,   -40,  -152,  -279,  -277,   -13,  -262,
-32768,-32768,-32768,   -40,-32768,   -40,  -200,  -198,-32768,  -139,
   -95,   -13,-32768,-32768,-32768,-32768,  -269,  -268,-32768,-32768,
-32768,-32768,-32768,    75,    82,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,   -26,-32768,   -37,-32768,-32768,-32768,
    30,   313,-32768,    65,-32768,-32768,    27,-32768,    35,   -22,
-32768
};


#define	YYLAST		404


static const short yytable[] = {    36,
    80,    57,    50,    36,    60,     9,   107,     9,     2,     9,
    50,     9,    37,    38,    82,     8,   119,    36,    51,    47,
    48,    91,    39,    11,   110,    92,    51,    33,   111,    34,
    49,    26,    27,    28,    61,    62,    79,-32768,-32768,    87,
    88,    93,    89,    94,    95,    40,    41,    39,    42,    96,
    97,    98,-32768,   102,   103,    81,   104,   105,   106,    10,
   114,    10,   116,    10,   117,   127,   118,   128,   129,    83,
-32768,-32768,   130,    42,   135,   132,   133,    43,-32768,   112,
   113,   136,   115,   124,   131,    86,    56,    90,    85,    11,
     0,   125,     0,   126,     0,     0,    52,     0,     0,     0,
     0,     0,-32768,     0,    52,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    37,    38,     0,     0,    37,    38,     0,     0,     0,
     0,    39,     0,     0,     0,    39,     0,     0,     0,    37,
    38,     0,     0,     0,     0,     0,     0,     0,     0,    39,
     0,     0,     0,     0,    40,    41,     0,    42,    40,    41,
    12,    42,    12,    13,    12,    13,    12,    13,     0,    13,
     0,     0,    40,    41,     0,    42,     0,     0,     0,     0,
     0,    53,     0,     0,     0,     0,    43,     0,     0,     0,
    43,   120,     0,     0,     0,     0,     0,     3,     0,    44,
     0,     0,     0,    44,    43,     0,    45,    46,    47,    48,
     0,     0,     0,     0,     0,     0,    53,     0,     0,    49,
   121,    14,    15,    14,    15,    14,    15,    14,    15,     0,
    54,    45,    46,    47,    48,     0,    45,    46,    47,    48,
     0,     0,     0,     0,    49,     0,     0,     0,     0,    49,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    16,
    17,    16,    17,    16,     0,    16,     0,     4,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    18,    58,    18,     0,     0,     0,     0,    19,
     0,    19,     0,    19,     0,    19,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    45,    46,    47,    48,    45,
    46,    47,    48,   108,    32,    64,     0,     0,    49,     0,
     0,    35,    49,    45,    46,    47,    48,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    49,     0,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,     0,     0,     0,    84,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   101
};

static const short yycheck[] = {    19,
   166,    22,   156,    19,    27,    50,     3,    50,    71,    50,
   156,    50,   141,   142,   166,   287,    30,    19,   172,   337,
   338,   341,   151,   134,   341,   345,   172,   342,   345,   344,
   348,   314,   344,   314,   341,   314,   166,   141,   142,   314,
   344,   314,   345,   314,   166,   174,   175,   151,   177,   314,
   166,   314,   151,   215,   215,   221,   314,   215,   314,   104,
   215,   104,   215,   104,   344,   266,   344,   266,   208,   221,
   174,   175,   168,   177,     0,   345,   345,   206,   177,   102,
   103,     0,   105,   110,   122,    56,    22,    61,    54,   134,
    -1,   114,    -1,   116,    -1,    -1,   250,    -1,    -1,    -1,
    -1,    -1,   206,    -1,   250,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   141,   142,    -1,    -1,   141,   142,    -1,    -1,    -1,
    -1,   151,    -1,    -1,    -1,   151,    -1,    -1,    -1,   141,
   142,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   151,
    -1,    -1,    -1,    -1,   174,   175,    -1,   177,   174,   175,
   205,   177,   205,   208,   205,   208,   205,   208,    -1,   208,
    -1,    -1,   174,   175,    -1,   177,    -1,    -1,    -1,    -1,
    -1,   327,    -1,    -1,    -1,    -1,   206,    -1,    -1,    -1,
   206,   205,    -1,    -1,    -1,    -1,    -1,   260,    -1,   219,
    -1,    -1,    -1,   219,   206,    -1,   335,   336,   337,   338,
    -1,    -1,    -1,    -1,    -1,    -1,   327,    -1,    -1,   348,
   234,   266,   267,   266,   267,   266,   267,   266,   267,    -1,
   341,   335,   336,   337,   338,    -1,   335,   336,   337,   338,
    -1,    -1,    -1,    -1,   348,    -1,    -1,    -1,    -1,   348,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   314,
   315,   314,   315,   314,    -1,   314,    -1,   340,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   337,   314,   337,    -1,    -1,    -1,    -1,   344,
    -1,   344,    -1,   344,    -1,   344,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   335,   336,   337,   338,   335,
   336,   337,   338,   320,    12,   345,    -1,    -1,   348,    -1,
    -1,    19,   348,   335,   336,   337,   338,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   348,    -1,    36,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    91
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/local/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 616 "sqlparser.y"
{ ;
    break;}
case 2:
#line 617 "sqlparser.y"
{ ;
    break;}
case 3:
#line 621 "sqlparser.y"
{ YYACCEPT; ;
    break;}
case 4:
#line 622 "sqlparser.y"
{  ;
    break;}
case 6:
#line 628 "sqlparser.y"
{
	parser->setOperation(KexiDB::Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
;
    break;}
case 9:
#line 637 "sqlparser.y"
{
;
    break;}
case 10:
#line 643 "sqlparser.y"
{
	kdDebug() << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

//	delete field;
	field = 0;
;
    break;}
case 11:
#line 652 "sqlparser.y"
{
	kdDebug() << "adding field " << yyvsp[-2].stringValue << endl;
	field->setName(yyvsp[-2].stringValue);
	parser->table()->addField(field);

//	if(field->isPrimaryKey())
//		parser->table()->addPrimaryKey(field->name());

//	delete field;
//	field = 0;
;
    break;}
case 13:
#line 667 "sqlparser.y"
{
;
    break;}
case 14:
#line 673 "sqlparser.y"
{
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
;
    break;}
case 15:
#line 678 "sqlparser.y"
{
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
;
    break;}
case 16:
#line 683 "sqlparser.y"
{
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
;
    break;}
case 17:
#line 691 "sqlparser.y"
{
	field = new KexiDB::Field();
	field->setType(yyvsp[0].coltype);
;
    break;}
case 18:
#line 696 "sqlparser.y"
{
	kdDebug() << "sql + length" << endl;
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].coltype);
;
    break;}
case 19:
#line 703 "sqlparser.y"
{
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(KexiDB::Field::Text);
;
    break;}
case 20:
#line 709 "sqlparser.y"
{
	// SQLITE compatibillity
	field = new KexiDB::Field();
	field->setType(KexiDB::Field::InvalidType);
;
    break;}
case 21:
#line 718 "sqlparser.y"
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
;
    break;}
case 22:
#line 737 "sqlparser.y"
{
//TODO: move this to all SELECT versions
	//fix fields
	KexiDB::BaseExpr *e;
	for (KexiDB::BaseExpr::ListIterator it(yyvsp[-1].exprlist->list); (e = it.current()); ++it)
	{
		if (e->cl == KexiDBExpr_Variable) {
			KexiDB::VariableExpr *v_e = static_cast<KexiDB::VariableExpr *>(e);
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
					parser->select()->addAsterisk( new KexiDB::QueryAsterisk(parser->select()) );
				}
				else {
				//find first table that has this field
				//TODO
				}
			}
			else {//table.fieldname
				KexiDB::TableSchema *ts = tableDict[tableName];
				if (ts) {
					if (fieldName=="*") {
						parser->select()->addAsterisk( new KexiDB::QueryAsterisk(parser->select(), ts) );
					}
					else {
						kdDebug() << " --it's a table.name" << endl;
						KexiDB::Field *realField = ts->field(fieldName);
						if (realField) {
	//						const int pos = parser->select()->fieldPos(f);
	//						parser->select()->removeField(f);
	//						parser->select()->insertField(pos, realField);
							parser->select()->addField(realField);
	//							f->setExpression( 0 ); //remove expr.
		//						f->setTable( ts );
	//						fieldRemoved = true;
						}
						else {
							setError(i18n("Field not found"), i18n("Table \"%1\" has no \"%2\" field")
								.arg(tableName).arg(fieldName));
							break;
						}
					}
				}
				else {
					tableNotFoundError(tableName);
					break;
				}
			}
		}
	}
	delete yyvsp[-1].exprlist; //no longer needed
	
	for (QDictIterator<KexiDB::TableSchema> it(tableDict); it.current(); ++it) {
		parser->select()->addTable( it.current() );
	}

	/* set parent table if there's only one */
	if (parser->select()->tables()->count()==1)
		parser->select()->setParentTable(parser->select()->tables()->first());

	kdDebug() << "Select ColViews Tables" << endl;
;
    break;}
case 23:
#line 809 "sqlparser.y"
{
	kdDebug() << "Select ColViews Tables" << endl;
;
    break;}
case 24:
#line 813 "sqlparser.y"
{
	kdDebug() << "Select ColViews Conditions" << endl;
;
    break;}
case 25:
#line 817 "sqlparser.y"
{
	kdDebug() << "Select ColViews Tables Conditions" << endl;
;
    break;}
case 26:
#line 824 "sqlparser.y"
{
	kdDebug() << "SELECT" << endl;
	parser->createSelect();
	parser->setOperation(KexiDB::Parser::OP_Select);
;
    break;}
case 27:
#line 833 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
;
    break;}
case 28:
#line 844 "sqlparser.y"
{
//	kdDebug() << "AND " << $3.debugString() << endl;
	yyval.expr = new KexiDB::BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, AND, yyvsp[0].expr );
;
    break;}
case 29:
#line 849 "sqlparser.y"
{
//	kdDebug() << "OR " << $3 << endl;
	yyval.expr = new KexiDB::BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, OR, yyvsp[0].expr );
;
    break;}
case 30:
#line 854 "sqlparser.y"
{
	yyval.expr = new KexiDB::UnaryExpr( NOT, yyvsp[0].expr );
//	$$->setName($1->name() + " NOT " + $3->name());
;
    break;}
case 31:
#line 859 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '+', yyvsp[0].expr);
;
    break;}
case 32:
#line 863 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '-', yyvsp[0].expr);
;
    break;}
case 33:
#line 867 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '/', yyvsp[0].expr);
;
    break;}
case 34:
#line 871 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '*', yyvsp[0].expr);
;
    break;}
case 35:
#line 875 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '%', yyvsp[0].expr);
;
    break;}
case 36:
#line 879 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL, yyvsp[0].expr);
;
    break;}
case 37:
#line 883 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_THAN, yyvsp[0].expr);
;
    break;}
case 38:
#line 887 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_OR_EQUAL, yyvsp[0].expr);
;
    break;}
case 39:
#line 891 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_THAN, yyvsp[0].expr);
;
    break;}
case 40:
#line 895 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_OR_EQUAL, yyvsp[0].expr);
;
    break;}
case 41:
#line 899 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LIKE, yyvsp[0].expr);
;
    break;}
case 42:
#line 903 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SQL_IN, yyvsp[0].expr);
;
    break;}
case 43:
#line 907 "sqlparser.y"
{
	kdDebug() << "  + function: " << yyvsp[-3].stringValue << "(" << yyvsp[-1].exprlist->debugString() << ")" << endl;
	yyval.expr = new KexiDB::FunctionExpr(yyvsp[-3].stringValue, yyvsp[-1].exprlist);
;
    break;}
case 44:
#line 912 "sqlparser.y"
{
	yyval.expr = new KexiDB::VariableExpr( yyvsp[0].stringValue );
	
//TODO: simplify this later if that's 'only one field name' expression
	kdDebug() << "  + identifier: " << yyvsp[0].stringValue << endl;
//	$$ = new KexiDB::Field();
//	$$->setName($1);
//	$$->setTable(dummy);

//	parser->select()->addField(field);
	requiresTable = true;
;
    break;}
case 45:
#line 926 "sqlparser.y"
{
	yyval.expr = new KexiDB::VariableExpr( QString(yyvsp[-2].stringValue) + "." + QString(yyvsp[0].stringValue) );
	kdDebug() << "  + identifier.identifier: " << yyvsp[0].stringValue << "." << yyvsp[-2].stringValue << endl;
//	$$ = new KexiDB::Field();
//	s->setTable($1);
//	$$->setName($3);
	//$$->setTable(parser->db()->tableSchema($1));
//	parser->select()->addField(field);
//??	requiresTable = true;
;
    break;}
case 46:
#line 937 "sqlparser.y"
{
	yyval.expr = new KexiDB::ConstExpr( SQL_NULL, QVariant() );
	kdDebug() << "  + NULL" << endl;
//	$$ = new KexiDB::Field();
	//$$->setName(QString::null);
;
    break;}
case 47:
#line 944 "sqlparser.y"
{
	yyval.expr = new KexiDB::ConstExpr( CHARACTER_STRING_LITERAL, yyvsp[0].stringValue );
//	$$ = new KexiDB::Field();
//	$$->setName($1);
//	parser->select()->addField(field);
	kdDebug() << "  + constant \"" << yyvsp[0].stringValue << "\"" << endl;
;
    break;}
case 48:
#line 952 "sqlparser.y"
{
	yyval.expr = new KexiDB::ConstExpr( INTEGER_CONST, yyvsp[0].integerValue );
//	$$ = new KexiDB::Field();
//	$$->setName(QString::number($1));
//	parser->select()->addField(field);
	kdDebug() << "  + int constant: " << yyvsp[0].integerValue << endl;
;
    break;}
case 49:
#line 960 "sqlparser.y"
{
	yyval.expr = new KexiDB::ConstExpr( REAL_CONST, QPoint( yyvsp[0].realValue.integer, yyvsp[0].realValue.fractional ) );
	kdDebug() << "  + real constant: " << yyvsp[0].realValue.integer << "." << yyvsp[0].realValue.fractional << endl;
;
    break;}
case 50:
#line 965 "sqlparser.y"
{
	kdDebug() << "(expr)" << endl;
	yyval.expr = yyvsp[-1].expr;
;
    break;}
case 51:
#line 973 "sqlparser.y"
{
	yyvsp[-2].exprlist->add( yyvsp[0].expr );
	yyval.exprlist = yyvsp[-2].exprlist;
;
    break;}
case 52:
#line 978 "sqlparser.y"
{
	yyval.exprlist = new KexiDB::NArgExpr(0/*unknown*/);
;
    break;}
case 53:
#line 985 "sqlparser.y"
{
;
    break;}
case 54:
#line 988 "sqlparser.y"
{
	kdDebug() << "LEFT JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].expr << endl;
	addTable(yyvsp[-2].stringValue);
;
    break;}
case 55:
#line 993 "sqlparser.y"
{
	kdDebug() << "LEFT OUTER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].expr << endl;
	addTable(yyvsp[-2].stringValue);
;
    break;}
case 56:
#line 998 "sqlparser.y"
{
	kdDebug() << "INNER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].expr << endl;
	addTable(yyvsp[-2].stringValue);
;
    break;}
case 57:
#line 1003 "sqlparser.y"
{
	kdDebug() << "RIGHT JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].expr << endl;
	addTable(yyvsp[-2].stringValue);
;
    break;}
case 58:
#line 1008 "sqlparser.y"
{
	kdDebug() << "RIGHT OUTER JOIN: '" << yyvsp[-2].stringValue << "' ON " << yyvsp[0].expr << endl;
	addTable(yyvsp[-2].stringValue);
;
    break;}
case 59:
#line 1016 "sqlparser.y"
{
;
    break;}
case 61:
#line 1022 "sqlparser.y"
{
;
    break;}
case 62:
#line 1028 "sqlparser.y"
{
	kdDebug() << "FROM: '" << yyvsp[0].stringValue << "'" << endl;

//	KexiDB::TableSchema *schema = parser->db()->tableSchema($1);
//	parser->select()->setParentTable(schema);
//	parser->select()->addTable(schema);
//	requiresTable = false;
	addTable(yyvsp[0].stringValue);

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
;
    break;}
case 63:
#line 1065 "sqlparser.y"
{
	yyval.exprlist = yyvsp[-2].exprlist;
	yyval.exprlist->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColViews , ColItem" << endl;
;
    break;}
case 64:
#line 1071 "sqlparser.y"
{
	yyval.exprlist = new KexiDB::NArgExpr(0);
	yyval.exprlist->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColItem" << endl;
;
    break;}
case 65:
#line 1080 "sqlparser.y"
{
//	$$ = new KexiDB::Field();
//	dummy->addField($$);
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column expr: '" << yyvsp[0].expr->debugString() << "'" << endl;
;
    break;}
case 66:
#line 1089 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column wildcard: '" << yyvsp[0].expr->debugString() << "'" << endl;
;
    break;}
case 67:
#line 1094 "sqlparser.y"
{
//	$$ = new KexiDB::Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = yyvsp[-2].expr;
//TODO	parser->select()->setAlias($$, $3);
	kdDebug() << " added column expr: '" << yyvsp[-2].expr->debugString() << "' as '" << yyvsp[0].stringValue << "'" << endl;
;
    break;}
case 68:
#line 1103 "sqlparser.y"
{
//	$$ = new KexiDB::Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = yyvsp[-1].expr;
//TODO	parser->select()->setAlias($$, $2);
	kdDebug() << " added column expr: '" << yyvsp[-1].expr->debugString() << "' as '" << yyvsp[0].stringValue << "'" << endl;
;
    break;}
case 69:
#line 1115 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
;
    break;}
case 70:
#line 1153 "sqlparser.y"
{
	yyval.expr = yyvsp[-1].expr;
//TODO
//	$$->setName("DISTINCT(" + $3->name() + ")");
;
    break;}
case 71:
#line 1162 "sqlparser.y"
{
	yyval.expr = new KexiDB::VariableExpr("*");
	kdDebug() << "all columns" << endl;

//	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), dummy);
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
;
    break;}
case 72:
#line 1171 "sqlparser.y"
{
	yyval.expr = new KexiDB::VariableExpr(yyvsp[0].stringValue);
	kdDebug() << "  + all columns from " << yyvsp[0].stringValue << endl;
//	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), parser->db()->tableSchema($1));
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/local/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 1180 "sqlparser.y"


