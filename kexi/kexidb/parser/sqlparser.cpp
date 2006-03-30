
/*  A Bison parser, made from sqlparser.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	UMINUS	257
#define	SQL_TYPE	258
#define	SQL_ABS	259
#define	ACOS	260
#define	AMPERSAND	261
#define	SQL_ABSOLUTE	262
#define	ADA	263
#define	ADD	264
#define	ADD_DAYS	265
#define	ADD_HOURS	266
#define	ADD_MINUTES	267
#define	ADD_MONTHS	268
#define	ADD_SECONDS	269
#define	ADD_YEARS	270
#define	ALL	271
#define	ALLOCATE	272
#define	ALTER	273
#define	AND	274
#define	ANY	275
#define	ARE	276
#define	AS	277
#define	ASIN	278
#define	ASC	279
#define	ASCII	280
#define	ASSERTION	281
#define	ATAN	282
#define	ATAN2	283
#define	AUTHORIZATION	284
#define	AUTO_INCREMENT	285
#define	AVG	286
#define	BEFORE	287
#define	SQL_BEGIN	288
#define	BETWEEN	289
#define	BIGINT	290
#define	BINARY	291
#define	BIT	292
#define	BIT_LENGTH	293
#define	BITWISE_SHIFT_LEFT	294
#define	BITWISE_SHIFT_RIGHT	295
#define	BREAK	296
#define	BY	297
#define	CASCADE	298
#define	CASCADED	299
#define	CASE	300
#define	CAST	301
#define	CATALOG	302
#define	CEILING	303
#define	CENTER	304
#define	SQL_CHAR	305
#define	CHAR_LENGTH	306
#define	CHARACTER_STRING_LITERAL	307
#define	CHECK	308
#define	CLOSE	309
#define	COALESCE	310
#define	COBOL	311
#define	COLLATE	312
#define	COLLATION	313
#define	COLUMN	314
#define	COMMIT	315
#define	COMPUTE	316
#define	CONCAT	317
#define	CONCATENATION	318
#define	CONNECT	319
#define	CONNECTION	320
#define	CONSTRAINT	321
#define	CONSTRAINTS	322
#define	CONTINUE	323
#define	CONVERT	324
#define	CORRESPONDING	325
#define	COS	326
#define	COT	327
#define	COUNT	328
#define	CREATE	329
#define	CURDATE	330
#define	CURRENT	331
#define	CURRENT_DATE	332
#define	CURRENT_TIME	333
#define	CURRENT_TIMESTAMP	334
#define	CURTIME	335
#define	CURSOR	336
#define	DATABASE	337
#define	SQL_DATE	338
#define	DATE_FORMAT	339
#define	DATE_REMAINDER	340
#define	DATE_VALUE	341
#define	DAY	342
#define	DAYOFMONTH	343
#define	DAYOFWEEK	344
#define	DAYOFYEAR	345
#define	DAYS_BETWEEN	346
#define	DEALLOCATE	347
#define	DEC	348
#define	DECLARE	349
#define	DEFAULT	350
#define	DEFERRABLE	351
#define	DEFERRED	352
#define	SQL_DELETE	353
#define	DESC	354
#define	DESCRIBE	355
#define	DESCRIPTOR	356
#define	DIAGNOSTICS	357
#define	DICTIONARY	358
#define	DIRECTORY	359
#define	DISCONNECT	360
#define	DISPLACEMENT	361
#define	DISTINCT	362
#define	DOMAIN_TOKEN	363
#define	SQL_DOUBLE	364
#define	DOUBLE_QUOTED_STRING	365
#define	DROP	366
#define	ELSE	367
#define	END	368
#define	END_EXEC	369
#define	EQUAL	370
#define	ESCAPE	371
#define	EXCEPT	372
#define	SQL_EXCEPTION	373
#define	EXEC	374
#define	EXECUTE	375
#define	EXISTS	376
#define	EXP	377
#define	EXPONENT	378
#define	EXTERNAL	379
#define	EXTRACT	380
#define	SQL_FALSE	381
#define	FETCH	382
#define	FIRST	383
#define	SQL_FLOAT	384
#define	FLOOR	385
#define	FN	386
#define	FOR	387
#define	FOREIGN	388
#define	FORTRAN	389
#define	FOUND	390
#define	FOUR_DIGITS	391
#define	FROM	392
#define	FULL	393
#define	GET	394
#define	GLOBAL	395
#define	GO	396
#define	GOTO	397
#define	GRANT	398
#define	GREATER_OR_EQUAL	399
#define	HAVING	400
#define	HOUR	401
#define	HOURS_BETWEEN	402
#define	IDENTITY	403
#define	IFNULL	404
#define	SQL_IGNORE	405
#define	IMMEDIATE	406
#define	SQL_IN	407
#define	INCLUDE	408
#define	INDEX	409
#define	INDICATOR	410
#define	INITIALLY	411
#define	INNER	412
#define	INPUT	413
#define	INSENSITIVE	414
#define	INSERT	415
#define	INTEGER	416
#define	INTERSECT	417
#define	INTERVAL	418
#define	INTO	419
#define	IS	420
#define	ISOLATION	421
#define	JOIN	422
#define	JUSTIFY	423
#define	KEY	424
#define	LANGUAGE	425
#define	LAST	426
#define	LCASE	427
#define	LEFT	428
#define	LENGTH	429
#define	LESS_OR_EQUAL	430
#define	LEVEL	431
#define	LIKE	432
#define	LINE_WIDTH	433
#define	LOCAL	434
#define	LOCATE	435
#define	LOG	436
#define	SQL_LONG	437
#define	LOWER	438
#define	LTRIM	439
#define	LTRIP	440
#define	MATCH	441
#define	SQL_MAX	442
#define	MICROSOFT	443
#define	SQL_MIN	444
#define	MINUS	445
#define	MINUTE	446
#define	MINUTES_BETWEEN	447
#define	MOD	448
#define	MODIFY	449
#define	MODULE	450
#define	MONTH	451
#define	MONTHS_BETWEEN	452
#define	MUMPS	453
#define	NAMES	454
#define	NATIONAL	455
#define	NCHAR	456
#define	NEXT	457
#define	NODUP	458
#define	NONE	459
#define	NOT	460
#define	NOT_EQUAL	461
#define	NOT_EQUAL2	462
#define	NOW	463
#define	SQL_NULL	464
#define	SQL_IS	465
#define	SQL_IS_NULL	466
#define	SQL_IS_NOT_NULL	467
#define	NULLIF	468
#define	NUMERIC	469
#define	OCTET_LENGTH	470
#define	ODBC	471
#define	OF	472
#define	SQL_OFF	473
#define	SQL_ON	474
#define	ONLY	475
#define	OPEN	476
#define	OPTION	477
#define	OR	478
#define	ORDER	479
#define	OUTER	480
#define	OUTPUT	481
#define	OVERLAPS	482
#define	PAGE	483
#define	PARTIAL	484
#define	SQL_PASCAL	485
#define	PERSISTENT	486
#define	CQL_PI	487
#define	PLI	488
#define	POSITION	489
#define	PRECISION	490
#define	PREPARE	491
#define	PRESERVE	492
#define	PRIMARY	493
#define	PRIOR	494
#define	PRIVILEGES	495
#define	PROCEDURE	496
#define	PRODUCT	497
#define	PUBLIC	498
#define	QUARTER	499
#define	QUIT	500
#define	RAND	501
#define	READ_ONLY	502
#define	REAL	503
#define	REFERENCES	504
#define	REPEAT	505
#define	REPLACE	506
#define	RESTRICT	507
#define	REVOKE	508
#define	RIGHT	509
#define	ROLLBACK	510
#define	ROWS	511
#define	RPAD	512
#define	RTRIM	513
#define	SCHEMA	514
#define	SCREEN_WIDTH	515
#define	SCROLL	516
#define	SECOND	517
#define	SECONDS_BETWEEN	518
#define	SELECT	519
#define	SEQUENCE	520
#define	SETOPT	521
#define	SET	522
#define	SHOWOPT	523
#define	SIGN	524
#define	SIMILAR_TO	525
#define	NOT_SIMILAR_TO	526
#define	INTEGER_CONST	527
#define	REAL_CONST	528
#define	DATE_CONST	529
#define	DATETIME_CONST	530
#define	TIME_CONST	531
#define	SIN	532
#define	SQL_SIZE	533
#define	SMALLINT	534
#define	SOME	535
#define	SPACE	536
#define	SQL	537
#define	SQL_TRUE	538
#define	SQLCA	539
#define	SQLCODE	540
#define	SQLERROR	541
#define	SQLSTATE	542
#define	SQLWARNING	543
#define	SQRT	544
#define	STDEV	545
#define	SUBSTRING	546
#define	SUM	547
#define	SYSDATE	548
#define	SYSDATE_FORMAT	549
#define	SYSTEM	550
#define	TABLE	551
#define	TAN	552
#define	TEMPORARY	553
#define	THEN	554
#define	THREE_DIGITS	555
#define	TIME	556
#define	TIMESTAMP	557
#define	TIMEZONE_HOUR	558
#define	TIMEZONE_MINUTE	559
#define	TINYINT	560
#define	TO	561
#define	TO_CHAR	562
#define	TO_DATE	563
#define	TRANSACTION	564
#define	TRANSLATE	565
#define	TRANSLATION	566
#define	TRUNCATE	567
#define	GENERAL_TITLE	568
#define	TWO_DIGITS	569
#define	UCASE	570
#define	UNION	571
#define	UNIQUE	572
#define	SQL_UNKNOWN	573
#define	UPDATE	574
#define	UPPER	575
#define	USAGE	576
#define	USER	577
#define	IDENTIFIER	578
#define	IDENTIFIER_DOT_ASTERISK	579
#define	USING	580
#define	VALUE	581
#define	VALUES	582
#define	VARBINARY	583
#define	VARCHAR	584
#define	VARYING	585
#define	VENDOR	586
#define	VIEW	587
#define	WEEK	588
#define	WHEN	589
#define	WHENEVER	590
#define	WHERE	591
#define	WHERE_CURRENT_OF	592
#define	WITH	593
#define	WORD_WRAPPED	594
#define	WORK	595
#define	WRAPPED	596
#define	XOR	597
#define	YEAR	598
#define	YEARS_BETWEEN	599
#define	SCAN_ERROR	600
#define	__LAST_TOKEN	601
#define	ILIKE	602

#line 433 "sqlparser.y"

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
#include <q3ptrlist.h>
#include <q3cstring.h>

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

#line 501 "sqlparser.y"
typedef union {
	char stringValue[255];
	qint64 integerValue;
	struct realType realValue;
	KexiDB::Field::Type colType;
	KexiDB::Field *field;
	KexiDB::BaseExpr *expr;
	KexiDB::NArgExpr *exprList;
	KexiDB::ConstExpr *constExpr;
	KexiDB::QuerySchema *querySchema;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		156
#define	YYFLAG		-32768
#define	YYNTBASE	372

#define YYTRANSLATE(x) ((unsigned)(x) <= 602 ? yytranslate[x] : 404)

static const short yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,   356,   351,   369,   360,   357,
   358,   350,   349,   354,   348,   355,   361,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,   353,   363,
   362,   364,   359,   352,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
   367,     2,   368,   366,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,   370,     2,   371,     2,     2,     2,     2,
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
   327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
   337,   338,   339,   340,   341,   342,   343,   344,   345,   346,
   347,   365
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,     8,    11,    13,    15,    16,    24,    28,
    30,    33,    37,    40,    42,    45,    48,    50,    52,    57,
    62,    63,    66,    70,    73,    77,    82,    86,    88,    91,
    93,    97,   101,   105,   107,   111,   115,   119,   123,   127,
   129,   133,   137,   141,   145,   149,   153,   155,   158,   161,
   163,   167,   171,   173,   177,   181,   185,   189,   191,   195,
   199,   203,   205,   208,   211,   214,   217,   219,   222,   226,
   228,   230,   232,   234,   236,   240,   244,   248,   252,   255,
   259,   261,   263,   266,   270,   274,   276,   278,   280,   284,
   287,   289,   294,   296
};

static const short yyrhs[] = {   373,
     0,   374,   353,   373,     0,   374,     0,   374,   353,     0,
   375,     0,   382,     0,     0,    75,   297,   324,   376,   357,
   377,   358,     0,   377,   354,   378,     0,   378,     0,   324,
   381,     0,   324,   381,   379,     0,   379,   380,     0,   380,
     0,   239,   170,     0,   206,   210,     0,    31,     0,     4,
     0,     4,   357,   273,   358,     0,   330,   357,   273,   358,
     0,     0,   383,   400,     0,   383,   400,   397,     0,   383,
   397,     0,   383,   400,   384,     0,   383,   400,   397,   384,
     0,   383,   397,   384,     0,   265,     0,   337,   385,     0,
   386,     0,   387,    20,   386,     0,   387,   224,   386,     0,
   387,   343,   386,     0,   387,     0,   388,   364,   387,     0,
   388,   145,   387,     0,   388,   363,   387,     0,   388,   176,
   387,     0,   388,   362,   387,     0,   388,     0,   389,   207,
   388,     0,   389,   208,   388,     0,   389,   178,   388,     0,
   389,   153,   388,     0,   389,   271,   388,     0,   389,   272,
   388,     0,   389,     0,   389,   212,     0,   389,   213,     0,
   390,     0,   391,    40,   390,     0,   391,    41,   390,     0,
   391,     0,   392,   349,   391,     0,   392,   348,   391,     0,
   392,   369,   391,     0,   392,   370,   391,     0,   392,     0,
   393,   361,   392,     0,   393,   350,   392,     0,   393,   351,
   392,     0,   393,     0,   348,   393,     0,   349,   393,     0,
   371,   393,     0,   206,   393,     0,   324,     0,   324,   395,
     0,   324,   355,   324,     0,   210,     0,    53,     0,   273,
     0,   274,     0,   394,     0,   357,   385,   358,     0,   357,
   396,   358,     0,   385,   354,   396,     0,   385,   354,   385,
     0,   138,   398,     0,   398,   354,   399,     0,   399,     0,
   324,     0,   324,   324,     0,   324,    23,   324,     0,   400,
   354,   401,     0,   401,     0,   402,     0,   403,     0,   402,
    23,   324,     0,   402,   324,     0,   385,     0,   108,   357,
   402,   358,     0,   350,     0,   324,   355,   350,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   565,   575,   580,   581,   590,   595,   601,   607,   610,   611,
   616,   626,   640,   641,   646,   652,   657,   664,   670,   677,
   683,   691,   699,   704,   710,   716,   722,   730,   740,   747,
   752,   758,   762,   766,   771,   776,   780,   784,   788,   792,
   797,   802,   807,   811,   815,   819,   823,   828,   833,   837,
   842,   847,   851,   856,   862,   866,   870,   874,   879,   884,
   888,   892,   897,   903,   907,   911,   915,   928,   934,   945,
   952,   958,   975,   980,   985,   993,  1003,  1009,  1017,  1058,
  1064,  1071,  1107,  1116,  1129,  1136,  1144,  1154,  1159,  1171,
  1185,  1230,  1238,  1248
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","UMINUS",
"SQL_TYPE","SQL_ABS","ACOS","AMPERSAND","SQL_ABSOLUTE","ADA","ADD","ADD_DAYS",
"ADD_HOURS","ADD_MINUTES","ADD_MONTHS","ADD_SECONDS","ADD_YEARS","ALL","ALLOCATE",
"ALTER","AND","ANY","ARE","AS","ASIN","ASC","ASCII","ASSERTION","ATAN","ATAN2",
"AUTHORIZATION","AUTO_INCREMENT","AVG","BEFORE","SQL_BEGIN","BETWEEN","BIGINT",
"BINARY","BIT","BIT_LENGTH","BITWISE_SHIFT_LEFT","BITWISE_SHIFT_RIGHT","BREAK",
"BY","CASCADE","CASCADED","CASE","CAST","CATALOG","CEILING","CENTER","SQL_CHAR",
"CHAR_LENGTH","CHARACTER_STRING_LITERAL","CHECK","CLOSE","COALESCE","COBOL",
"COLLATE","COLLATION","COLUMN","COMMIT","COMPUTE","CONCAT","CONCATENATION","CONNECT",
"CONNECTION","CONSTRAINT","CONSTRAINTS","CONTINUE","CONVERT","CORRESPONDING",
"COS","COT","COUNT","CREATE","CURDATE","CURRENT","CURRENT_DATE","CURRENT_TIME",
"CURRENT_TIMESTAMP","CURTIME","CURSOR","DATABASE","SQL_DATE","DATE_FORMAT","DATE_REMAINDER",
"DATE_VALUE","DAY","DAYOFMONTH","DAYOFWEEK","DAYOFYEAR","DAYS_BETWEEN","DEALLOCATE",
"DEC","DECLARE","DEFAULT","DEFERRABLE","DEFERRED","SQL_DELETE","DESC","DESCRIBE",
"DESCRIPTOR","DIAGNOSTICS","DICTIONARY","DIRECTORY","DISCONNECT","DISPLACEMENT",
"DISTINCT","DOMAIN_TOKEN","SQL_DOUBLE","DOUBLE_QUOTED_STRING","DROP","ELSE",
"END","END_EXEC","EQUAL","ESCAPE","EXCEPT","SQL_EXCEPTION","EXEC","EXECUTE",
"EXISTS","EXP","EXPONENT","EXTERNAL","EXTRACT","SQL_FALSE","FETCH","FIRST","SQL_FLOAT",
"FLOOR","FN","FOR","FOREIGN","FORTRAN","FOUND","FOUR_DIGITS","FROM","FULL","GET",
"GLOBAL","GO","GOTO","GRANT","GREATER_OR_EQUAL","HAVING","HOUR","HOURS_BETWEEN",
"IDENTITY","IFNULL","SQL_IGNORE","IMMEDIATE","SQL_IN","INCLUDE","INDEX","INDICATOR",
"INITIALLY","INNER","INPUT","INSENSITIVE","INSERT","INTEGER","INTERSECT","INTERVAL",
"INTO","IS","ISOLATION","JOIN","JUSTIFY","KEY","LANGUAGE","LAST","LCASE","LEFT",
"LENGTH","LESS_OR_EQUAL","LEVEL","LIKE","LINE_WIDTH","LOCAL","LOCATE","LOG",
"SQL_LONG","LOWER","LTRIM","LTRIP","MATCH","SQL_MAX","MICROSOFT","SQL_MIN","MINUS",
"MINUTE","MINUTES_BETWEEN","MOD","MODIFY","MODULE","MONTH","MONTHS_BETWEEN",
"MUMPS","NAMES","NATIONAL","NCHAR","NEXT","NODUP","NONE","NOT","NOT_EQUAL","NOT_EQUAL2",
"NOW","SQL_NULL","SQL_IS","SQL_IS_NULL","SQL_IS_NOT_NULL","NULLIF","NUMERIC",
"OCTET_LENGTH","ODBC","OF","SQL_OFF","SQL_ON","ONLY","OPEN","OPTION","OR","ORDER",
"OUTER","OUTPUT","OVERLAPS","PAGE","PARTIAL","SQL_PASCAL","PERSISTENT","CQL_PI",
"PLI","POSITION","PRECISION","PREPARE","PRESERVE","PRIMARY","PRIOR","PRIVILEGES",
"PROCEDURE","PRODUCT","PUBLIC","QUARTER","QUIT","RAND","READ_ONLY","REAL","REFERENCES",
"REPEAT","REPLACE","RESTRICT","REVOKE","RIGHT","ROLLBACK","ROWS","RPAD","RTRIM",
"SCHEMA","SCREEN_WIDTH","SCROLL","SECOND","SECONDS_BETWEEN","SELECT","SEQUENCE",
"SETOPT","SET","SHOWOPT","SIGN","SIMILAR_TO","NOT_SIMILAR_TO","INTEGER_CONST",
"REAL_CONST","DATE_CONST","DATETIME_CONST","TIME_CONST","SIN","SQL_SIZE","SMALLINT",
"SOME","SPACE","SQL","SQL_TRUE","SQLCA","SQLCODE","SQLERROR","SQLSTATE","SQLWARNING",
"SQRT","STDEV","SUBSTRING","SUM","SYSDATE","SYSDATE_FORMAT","SYSTEM","TABLE",
"TAN","TEMPORARY","THEN","THREE_DIGITS","TIME","TIMESTAMP","TIMEZONE_HOUR","TIMEZONE_MINUTE",
"TINYINT","TO","TO_CHAR","TO_DATE","TRANSACTION","TRANSLATE","TRANSLATION","TRUNCATE",
"GENERAL_TITLE","TWO_DIGITS","UCASE","UNION","UNIQUE","SQL_UNKNOWN","UPDATE",
"UPPER","USAGE","USER","IDENTIFIER","IDENTIFIER_DOT_ASTERISK","USING","VALUE",
"VALUES","VARBINARY","VARCHAR","VARYING","VENDOR","VIEW","WEEK","WHEN","WHENEVER",
"WHERE","WHERE_CURRENT_OF","WITH","WORD_WRAPPED","WORK","WRAPPED","XOR","YEAR",
"YEARS_BETWEEN","SCAN_ERROR","__LAST_TOKEN","'-'","'+'","'*'","'%'","'@'","';'",
"','","'.'","'$'","'('","')'","'?'","'\\''","'/'","'='","'<'","'>'","ILIKE",
"'^'","'['","']'","'&'","'|'","'~'","TopLevelStatement","StatementList","Statement",
"CreateTableStatement","@1","ColDefs","ColDef","ColKeys","ColKey","ColType",
"SelectStatement","Select","WhereClause","aExpr","aExpr2","aExpr3","aExpr4",
"aExpr5","aExpr6","aExpr7","aExpr8","aExpr9","aExpr10","aExprList","aExprList2",
"Tables","FlatTableList","FlatTable","ColViews","ColItem","ColExpression","ColWildCard", NULL
};
#endif

static const short yyr1[] = {     0,
   372,   373,   373,   373,   374,   374,   376,   375,   377,   377,
   378,   378,   379,   379,   380,   380,   380,   381,   381,   381,
   381,   382,   382,   382,   382,   382,   382,   383,   384,   385,
   386,   386,   386,   386,   387,   387,   387,   387,   387,   387,
   388,   388,   388,   388,   388,   388,   388,   389,   389,   389,
   390,   390,   390,   391,   391,   391,   391,   391,   392,   392,
   392,   392,   393,   393,   393,   393,   393,   393,   393,   393,
   393,   393,   393,   393,   394,   395,   396,   396,   397,   398,
   398,   399,   399,   399,   400,   400,   401,   401,   401,   401,
   402,   402,   403,   403
};

static const short yyr2[] = {     0,
     1,     3,     1,     2,     1,     1,     0,     7,     3,     1,
     2,     3,     2,     1,     2,     2,     1,     1,     4,     4,
     0,     2,     3,     2,     3,     4,     3,     1,     2,     1,
     3,     3,     3,     1,     3,     3,     3,     3,     3,     1,
     3,     3,     3,     3,     3,     3,     1,     2,     2,     1,
     3,     3,     1,     3,     3,     3,     3,     1,     3,     3,
     3,     1,     2,     2,     2,     2,     1,     2,     3,     1,
     1,     1,     1,     1,     3,     3,     3,     3,     2,     3,
     1,     1,     2,     3,     3,     1,     1,     1,     3,     2,
     1,     4,     1,     3
};

static const short yydefact[] = {     0,
     0,    28,     1,     3,     5,     6,     0,     0,     4,    71,
     0,     0,     0,    70,    72,    73,    67,     0,     0,    93,
     0,     0,    91,    30,    34,    40,    47,    50,    53,    58,
    62,    74,    24,    22,    86,    87,    88,     7,     2,     0,
    82,    79,    81,    67,    66,     0,     0,    68,    63,    64,
     0,    65,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    48,    49,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
    25,    23,     0,    90,     0,     0,     0,    83,     0,     0,
    69,    94,     0,     0,    75,    31,    32,    33,    36,    38,
    39,    37,    35,    44,    43,    41,    42,    45,    46,    51,
    52,    55,    54,    56,    57,    60,    61,    59,    29,    85,
    26,    89,     0,    92,    84,    80,     0,    76,    21,     0,
    10,    78,    77,    18,     0,    11,     0,     8,     0,     0,
    17,     0,     0,    12,    14,     9,     0,     0,    16,    15,
    13,    19,    20,     0,     0,     0
};

static const short yydefgoto[] = {   154,
     3,     4,     5,    85,   130,   131,   144,   145,   136,     6,
     7,    79,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    48,    94,    33,    42,    43,    34,    35,    36,
    37
};

static const short yypact[] = {   -68,
  -275,-32768,-32768,  -324,-32768,-32768,   -51,  -282,   -68,-32768,
  -299,  -262,   -38,-32768,-32768,-32768,  -329,   -38,   -38,-32768,
   -38,   -38,-32768,-32768,   -19,  -135,  -144,-32768,     7,  -332,
  -331,-32768,  -272,  -134,-32768,   -20,-32768,-32768,-32768,   -41,
   -12,  -288,-32768,  -296,-32768,  -310,   -38,-32768,-32768,-32768,
  -283,-32768,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
   -38,   -38,   -38,   -38,-32768,-32768,   -38,   -38,   -38,   -38,
   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,-32768,   -48,
-32768,  -272,  -245,-32768,  -277,  -276,  -243,-32768,  -262,  -241,
-32768,-32768,  -270,  -273,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,  -238,-32768,-32768,-32768,   -38,-32768,    -4,  -333,
-32768,  -270,-32768,  -269,  -268,   -25,  -238,-32768,  -183,  -182,
-32768,  -118,   -77,   -25,-32768,-32768,  -264,  -263,-32768,-32768,
-32768,-32768,-32768,    96,    97,-32768
};

static const short yypgoto[] = {-32768,
    89,-32768,-32768,-32768,-32768,   -37,-32768,   -45,-32768,-32768,
-32768,   -26,    -8,   -22,    -5,   -18,-32768,   -34,     0,     1,
     5,-32768,-32768,   -24,    67,-32768,    13,-32768,    24,    65,
-32768
};


#define	YYLAST		333


static const short yytable[] = {   134,
    53,    10,    83,    12,    10,   141,     1,    81,    61,    56,
    87,    10,    51,    91,    10,    71,    72,    45,    75,    76,
   137,     8,    49,    50,   138,    46,    52,    47,     9,    77,
    96,    97,    98,    62,   110,   111,    73,    74,    93,    92,
    57,    38,   104,   105,   106,   107,    69,    70,   108,   109,
    99,   100,   101,   102,   103,   121,    11,    40,    90,    11,
    47,    41,    63,    64,    78,    89,    11,    65,    66,   119,
   112,   113,   114,   115,    95,   116,   117,   118,   122,   123,
   125,   124,    91,   127,   128,   129,    12,   139,   140,   147,
   148,   149,   150,   152,   153,   155,   156,    39,   151,   146,
    82,   126,   133,   120,    86,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   132,     0,
     0,     0,     0,     0,     0,     0,    67,    68,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    13,     0,     0,    13,    14,     0,
     0,    14,     0,     0,    13,     0,     0,    13,    14,     0,
     0,    14,     0,     0,     0,     0,     0,     0,     0,     0,
   142,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     2,     0,     0,     0,
     0,     0,    78,     0,    54,     0,     0,     0,     0,     0,
     0,     0,     0,   143,     0,     0,     0,     0,     0,    80,
     0,    15,    16,     0,    15,    16,    58,    59,    60,     0,
     0,    15,    16,     0,    15,    16,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    17,     0,     0,    17,     0,     0,     0,     0,
     0,     0,    44,     0,     0,    44,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    18,    19,    20,    18,
    19,    20,     0,    84,     0,    21,    18,    19,    21,    18,
    19,    88,     0,     0,     0,    21,     0,     0,    21,    22,
     0,     0,    22,    55,     0,   135,     0,     0,     0,    22,
     0,     0,    22
};

static const short yycheck[] = {     4,
    20,    53,    23,   138,    53,    31,    75,    34,   153,   145,
    23,    53,    21,   324,    53,   348,   349,    13,   350,   351,
   354,   297,    18,    19,   358,   355,    22,   357,   353,   361,
    53,    54,    55,   178,    69,    70,   369,   370,    47,   350,
   176,   324,    61,    62,    63,    64,    40,    41,    67,    68,
    56,    57,    58,    59,    60,    82,   108,   357,   355,   108,
   357,   324,   207,   208,   337,   354,   108,   212,   213,    78,
    71,    72,    73,    74,   358,    75,    76,    77,   324,   357,
   324,   358,   324,   354,   358,   324,   138,   357,   357,   273,
   273,   210,   170,   358,   358,     0,     0,     9,   144,   137,
    34,    89,   127,    80,    40,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   127,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   271,   272,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   206,    -1,    -1,   206,   210,    -1,
    -1,   210,    -1,    -1,   206,    -1,    -1,   206,   210,    -1,
    -1,   210,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   206,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   265,    -1,    -1,    -1,
    -1,    -1,   337,    -1,   224,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   239,    -1,    -1,    -1,    -1,    -1,   354,
    -1,   273,   274,    -1,   273,   274,   362,   363,   364,    -1,
    -1,   273,   274,    -1,   273,   274,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   324,    -1,    -1,   324,    -1,    -1,    -1,    -1,
    -1,    -1,   324,    -1,    -1,   324,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   348,   349,   350,   348,
   349,   350,    -1,   324,    -1,   357,   348,   349,   357,   348,
   349,   324,    -1,    -1,    -1,   357,    -1,    -1,   357,   371,
    -1,    -1,   371,   343,    -1,   330,    -1,    -1,    -1,   371,
    -1,    -1,   371
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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.  */

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
#line 567 "sqlparser.y"
{
//todo: multiple statements
//todo: not only "select" statements
	parser->setOperation(Parser::OP_Select);
	parser->setQuerySchema(yyvsp[0].querySchema);
;
    break;}
case 2:
#line 577 "sqlparser.y"
{
//todo: multiple statements
;
    break;}
case 4:
#line 582 "sqlparser.y"
{
	yyval.querySchema = yyvsp[-1].querySchema;
;
    break;}
case 5:
#line 592 "sqlparser.y"
{
YYACCEPT;
;
    break;}
case 6:
#line 596 "sqlparser.y"
{
	yyval.querySchema = yyvsp[0].querySchema;
;
    break;}
case 7:
#line 603 "sqlparser.y"
{
	parser->setOperation(Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
;
    break;}
case 10:
#line 612 "sqlparser.y"
{
;
    break;}
case 11:
#line 618 "sqlparser.y"
{
	kDebug() << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

//	delete field;
	field = 0;
;
    break;}
case 12:
#line 627 "sqlparser.y"
{
	kDebug() << "adding field " << yyvsp[-2].stringValue << endl;
	field->setName(yyvsp[-2].stringValue);
	parser->table()->addField(field);

//	if(field->isPrimaryKey())
//		parser->table()->addPrimaryKey(field->name());

//	delete field;
//	field = 0;
;
    break;}
case 14:
#line 642 "sqlparser.y"
{
;
    break;}
case 15:
#line 648 "sqlparser.y"
{
	field->setPrimaryKey(true);
	kDebug() << "primary" << endl;
;
    break;}
case 16:
#line 653 "sqlparser.y"
{
	field->setNotNull(true);
	kDebug() << "not_null" << endl;
;
    break;}
case 17:
#line 658 "sqlparser.y"
{
	field->setAutoIncrement(true);
	kDebug() << "ainc" << endl;
;
    break;}
case 18:
#line 666 "sqlparser.y"
{
	field = new Field();
	field->setType(yyvsp[0].colType);
;
    break;}
case 19:
#line 671 "sqlparser.y"
{
	kDebug() << "sql + length" << endl;
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].colType);
;
    break;}
case 20:
#line 678 "sqlparser.y"
{
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(Field::Text);
;
    break;}
case 21:
#line 684 "sqlparser.y"
{
	// SQLITE compatibillity
	field = new Field();
	field->setType(Field::InvalidType);
;
    break;}
case 22:
#line 693 "sqlparser.y"
{
	kDebug() << "Select ColViews=" << yyvsp[0].exprList->debugString() << endl;

	if (!(yyval.querySchema = parseSelect( yyvsp[-1].querySchema, yyvsp[0].exprList )))
		return 0;
;
    break;}
case 23:
#line 700 "sqlparser.y"
{
	if (!(yyval.querySchema = parseSelect( yyvsp[-2].querySchema, yyvsp[-1].exprList, yyvsp[0].exprList )))
		return 0;
;
    break;}
case 24:
#line 705 "sqlparser.y"
{
	kDebug() << "Select ColViews Tables" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-1].querySchema, 0, yyvsp[0].exprList )))
		return 0;
;
    break;}
case 25:
#line 711 "sqlparser.y"
{
	kDebug() << "Select ColViews Conditions" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-2].querySchema, yyvsp[-1].exprList, 0, yyvsp[0].expr )))
		return 0;
;
    break;}
case 26:
#line 717 "sqlparser.y"
{
	kDebug() << "Select ColViews Tables Conditions" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-3].querySchema, yyvsp[-2].exprList, yyvsp[-1].exprList, yyvsp[0].expr )))
		return 0;
;
    break;}
case 27:
#line 723 "sqlparser.y"
{
	kDebug() << "Select Tables Conditions" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-2].querySchema, 0, yyvsp[-1].exprList, yyvsp[0].expr )))
		return 0;
;
    break;}
case 28:
#line 732 "sqlparser.y"
{
	kDebug() << "SELECT" << endl;
//	parser->createSelect();
//	parser->setOperation(Parser::OP_Select);
	yyval.querySchema = new QuerySchema();
;
    break;}
case 29:
#line 742 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
;
    break;}
case 31:
#line 754 "sqlparser.y"
{
//	kDebug() << "AND " << $3.debugString() << endl;
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, AND, yyvsp[0].expr );
;
    break;}
case 32:
#line 759 "sqlparser.y"
{
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, OR, yyvsp[0].expr );
;
    break;}
case 33:
#line 763 "sqlparser.y"
{
	yyval.expr = new BinaryExpr( KexiDBExpr_Arithm, yyvsp[-2].expr, XOR, yyvsp[0].expr );
;
    break;}
case 35:
#line 773 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '>', yyvsp[0].expr);
;
    break;}
case 36:
#line 777 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_OR_EQUAL, yyvsp[0].expr);
;
    break;}
case 37:
#line 781 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '<', yyvsp[0].expr);
;
    break;}
case 38:
#line 785 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_OR_EQUAL, yyvsp[0].expr);
;
    break;}
case 39:
#line 789 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '=', yyvsp[0].expr);
;
    break;}
case 41:
#line 799 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL, yyvsp[0].expr);
;
    break;}
case 42:
#line 804 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL2, yyvsp[0].expr);
;
    break;}
case 43:
#line 808 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LIKE, yyvsp[0].expr);
;
    break;}
case 44:
#line 812 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SQL_IN, yyvsp[0].expr);
;
    break;}
case 45:
#line 816 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SIMILAR_TO, yyvsp[0].expr);
;
    break;}
case 46:
#line 820 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_SIMILAR_TO, yyvsp[0].expr);
;
    break;}
case 48:
#line 830 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( SQL_IS_NULL, yyvsp[-1].expr );
;
    break;}
case 49:
#line 834 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( SQL_IS_NOT_NULL, yyvsp[-1].expr );
;
    break;}
case 51:
#line 844 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_LEFT, yyvsp[0].expr);
;
    break;}
case 52:
#line 848 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_RIGHT, yyvsp[0].expr);
;
    break;}
case 54:
#line 858 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '+', yyvsp[0].expr);
	yyval.expr->debug();
;
    break;}
case 55:
#line 863 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '-', yyvsp[0].expr);
;
    break;}
case 56:
#line 867 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '&', yyvsp[0].expr);
;
    break;}
case 57:
#line 871 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '|', yyvsp[0].expr);
;
    break;}
case 59:
#line 881 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '/', yyvsp[0].expr);
;
    break;}
case 60:
#line 885 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '*', yyvsp[0].expr);
;
    break;}
case 61:
#line 889 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '%', yyvsp[0].expr);
;
    break;}
case 63:
#line 900 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( '-', yyvsp[0].expr );
;
    break;}
case 64:
#line 904 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( '+', yyvsp[0].expr );
;
    break;}
case 65:
#line 908 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( '~', yyvsp[0].expr );
;
    break;}
case 66:
#line 912 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( NOT, yyvsp[0].expr );
;
    break;}
case 67:
#line 916 "sqlparser.y"
{
	yyval.expr = new VariableExpr( QString::fromUtf8(yyvsp[0].stringValue) );
	
//TODO: simplify this later if that's 'only one field name' expression
	kDebug() << "  + identifier: " << yyvsp[0].stringValue << endl;
//	$$ = new Field();
//	$$->setName($1);
//	$$->setTable(dummy);

//	parser->select()->addField(field);
//	requiresTable = true;
;
    break;}
case 68:
#line 929 "sqlparser.y"
{
	kDebug() << "  + function: " << yyvsp[-1].stringValue << "(" << yyvsp[0].exprList->debugString() << ")" << endl;
	yyval.expr = new FunctionExpr(yyvsp[-1].stringValue, yyvsp[0].exprList);
;
    break;}
case 69:
#line 935 "sqlparser.y"
{
	yyval.expr = new VariableExpr( QString::fromUtf8(yyvsp[-2].stringValue) + "." + QString::fromUtf8(yyvsp[0].stringValue) );
	kDebug() << "  + identifier.identifier: " << yyvsp[0].stringValue << "." << yyvsp[-2].stringValue << endl;
//	$$ = new Field();
//	s->setTable($1);
//	$$->setName($3);
	//$$->setTable(parser->db()->tableSchema($1));
//	parser->select()->addField(field);
//??	requiresTable = true;
;
    break;}
case 70:
#line 946 "sqlparser.y"
{
	yyval.expr = new ConstExpr( SQL_NULL, QVariant() );
	kDebug() << "  + NULL" << endl;
//	$$ = new Field();
	//$$->setName(QString::null);
;
    break;}
case 71:
#line 953 "sqlparser.y"
{
	QString s( QString::fromUtf8(yyvsp[0].stringValue) );
	yyval.expr = new ConstExpr( CHARACTER_STRING_LITERAL, s.mid(1,s.length()-2) );
	kDebug() << "  + constant " << s << endl;
;
    break;}
case 72:
#line 959 "sqlparser.y"
{
	QVariant val;
	if (yyvsp[0].integerValue <= INT_MAX && yyvsp[0].integerValue >= INT_MIN)
		val = (int)yyvsp[0].integerValue;
	else if (yyvsp[0].integerValue <= UINT_MAX && yyvsp[0].integerValue >= 0)
		val = (uint)yyvsp[0].integerValue;
	else if (yyvsp[0].integerValue <= LLONG_MAX && yyvsp[0].integerValue >= LLONG_MIN)
		val = (qint64)yyvsp[0].integerValue;

//	if ($1 < ULLONG_MAX)
//		val = (quint64)$1;
//TODO ok?

	yyval.expr = new ConstExpr( INTEGER_CONST, val );
	kDebug() << "  + int constant: " << val.toString() << endl;
;
    break;}
case 73:
#line 976 "sqlparser.y"
{
	yyval.expr = new ConstExpr( REAL_CONST, QPoint( yyvsp[0].realValue.integer, yyvsp[0].realValue.fractional ) );
	kDebug() << "  + real constant: " << yyvsp[0].realValue.integer << "." << yyvsp[0].realValue.fractional << endl;
;
    break;}
case 75:
#line 987 "sqlparser.y"
{
	kDebug() << "(expr)" << endl;
	yyval.expr = new UnaryExpr('(', yyvsp[-1].expr);
;
    break;}
case 76:
#line 995 "sqlparser.y"
{
//	$$ = new NArgExpr(0, 0);
//	$$->add( $1 );
//	$$->add( $3 );
	yyval.exprList = yyvsp[-1].exprList;
;
    break;}
case 77:
#line 1005 "sqlparser.y"
{
	yyval.exprList = yyvsp[0].exprList;
	yyval.exprList->prepend( yyvsp[-2].expr );
;
    break;}
case 78:
#line 1010 "sqlparser.y"
{
	yyval.exprList = new NArgExpr(0, 0);
	yyval.exprList->add( yyvsp[-2].expr );
	yyval.exprList->add( yyvsp[0].expr );
;
    break;}
case 79:
#line 1019 "sqlparser.y"
{
	yyval.exprList = yyvsp[0].exprList;
;
    break;}
case 80:
#line 1060 "sqlparser.y"
{
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add(yyvsp[0].expr);
;
    break;}
case 81:
#line 1065 "sqlparser.y"
{
	yyval.exprList = new NArgExpr(KexiDBExpr_TableList, IDENTIFIER); //ok?
	yyval.exprList->add(yyvsp[0].expr);
;
    break;}
case 82:
#line 1073 "sqlparser.y"
{
	kDebug() << "FROM: '" << yyvsp[0].stringValue << "'" << endl;

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
;
    break;}
case 83:
#line 1108 "sqlparser.y"
{
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, 
		new VariableExpr(QString::fromUtf8(yyvsp[-1].stringValue)), 0,
		new VariableExpr(QString::fromUtf8(yyvsp[0].stringValue))
	);
;
    break;}
case 84:
#line 1117 "sqlparser.y"
{
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary,
		new VariableExpr(QString::fromUtf8(yyvsp[-2].stringValue)), AS,
		new VariableExpr(QString::fromUtf8(yyvsp[0].stringValue))
	);
;
    break;}
case 85:
#line 1131 "sqlparser.y"
{
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add( yyvsp[0].expr );
	kDebug() << "ColViews: ColViews , ColItem" << endl;
;
    break;}
case 86:
#line 1137 "sqlparser.y"
{
	yyval.exprList = new NArgExpr(0,0);
	yyval.exprList->add( yyvsp[0].expr );
	kDebug() << "ColViews: ColItem" << endl;
;
    break;}
case 87:
#line 1146 "sqlparser.y"
{
//	$$ = new Field();
//	dummy->addField($$);
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = yyvsp[0].expr;
	kDebug() << " added column expr: '" << yyvsp[0].expr->debugString() << "'" << endl;
;
    break;}
case 88:
#line 1155 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
	kDebug() << " added column wildcard: '" << yyvsp[0].expr->debugString() << "'" << endl;
;
    break;}
case 89:
#line 1160 "sqlparser.y"
{
//	$$ = new Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, yyvsp[-2].expr, AS,
		new VariableExpr(QString::fromUtf8(yyvsp[0].stringValue))
//		new ConstExpr(IDENTIFIER, QString::fromLocal8Bit($3))
	);
	kDebug() << " added column expr: " << yyval.expr->debugString() << endl;
;
    break;}
case 90:
#line 1172 "sqlparser.y"
{
//	$$ = new Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, yyvsp[-1].expr, 0, 
		new VariableExpr(QString::fromUtf8(yyvsp[0].stringValue))
//		new ConstExpr(IDENTIFIER, QString::fromLocal8Bit($2))
	);
	kDebug() << " added column expr: " << yyval.expr->debugString() << endl;
;
    break;}
case 91:
#line 1187 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
;
    break;}
case 92:
#line 1231 "sqlparser.y"
{
	yyval.expr = yyvsp[-1].expr;
//TODO
//	$$->setName("DISTINCT(" + $3->name() + ")");
;
    break;}
case 93:
#line 1240 "sqlparser.y"
{
	yyval.expr = new VariableExpr("*");
	kDebug() << "all columns" << endl;

//	QueryAsterisk *ast = new QueryAsterisk(parser->select(), dummy);
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
;
    break;}
case 94:
#line 1249 "sqlparser.y"
{
	QString s = QString::fromUtf8(yyvsp[-2].stringValue);
	s+=".*";
	yyval.expr = new VariableExpr(s);
	kDebug() << "  + all columns from " << s << endl;
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
#line 1263 "sqlparser.y"


const char * const tname(int offset) { return yytname[offset]; }
