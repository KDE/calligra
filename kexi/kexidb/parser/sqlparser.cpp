
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
#define	SIN	529
#define	SQL_SIZE	530
#define	SMALLINT	531
#define	SOME	532
#define	SPACE	533
#define	SQL	534
#define	SQL_TRUE	535
#define	SQLCA	536
#define	SQLCODE	537
#define	SQLERROR	538
#define	SQLSTATE	539
#define	SQLWARNING	540
#define	SQRT	541
#define	STDEV	542
#define	SUBSTRING	543
#define	SUM	544
#define	SYSDATE	545
#define	SYSDATE_FORMAT	546
#define	SYSTEM	547
#define	TABLE	548
#define	TAN	549
#define	TEMPORARY	550
#define	THEN	551
#define	THREE_DIGITS	552
#define	TIME	553
#define	TIMESTAMP	554
#define	TIMEZONE_HOUR	555
#define	TIMEZONE_MINUTE	556
#define	TINYINT	557
#define	TO	558
#define	TO_CHAR	559
#define	TO_DATE	560
#define	TRANSACTION	561
#define	TRANSLATE	562
#define	TRANSLATION	563
#define	TRUNCATE	564
#define	GENERAL_TITLE	565
#define	TWO_DIGITS	566
#define	UCASE	567
#define	UNION	568
#define	UNIQUE	569
#define	SQL_UNKNOWN	570
#define	UPDATE	571
#define	UPPER	572
#define	USAGE	573
#define	USER	574
#define	IDENTIFIER	575
#define	IDENTIFIER_DOT_ASTERISK	576
#define	ERROR_DIGIT_BEFORE_IDENTIFIER	577
#define	USING	578
#define	VALUE	579
#define	VALUES	580
#define	VARBINARY	581
#define	VARCHAR	582
#define	VARYING	583
#define	VENDOR	584
#define	VIEW	585
#define	WEEK	586
#define	WHEN	587
#define	WHENEVER	588
#define	WHERE	589
#define	WHERE_CURRENT_OF	590
#define	WITH	591
#define	WORD_WRAPPED	592
#define	WORK	593
#define	WRAPPED	594
#define	XOR	595
#define	YEAR	596
#define	YEARS_BETWEEN	597
#define	__LAST_TOKEN	598
#define	ILIKE	599

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
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		152
#define	YYFLAG		-32768
#define	YYNTBASE	369

#define YYTRANSLATE(x) ((unsigned)(x) <= 599 ? yytranslate[x] : 399)

static const short yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,   353,   348,   366,   357,   354,
   355,   347,   346,   351,   345,   352,   358,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,   350,   360,
   359,   361,   356,   349,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
   364,     2,   365,   363,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,   367,     2,   368,     2,     2,     2,     2,
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
   337,   338,   339,   340,   341,   342,   343,   344,   362
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
   228,   230,   232,   234,   236,   240,   244,   248,   251,   255,
   257,   259,   262,   266,   270,   272,   274,   276,   280,   283,
   285,   290,   295,   297
};

static const short yyrhs[] = {   370,
     0,   371,   350,   370,     0,   371,     0,   371,   350,     0,
   372,     0,   379,     0,     0,    75,   294,   321,   373,   354,
   374,   355,     0,   374,   351,   375,     0,   375,     0,   321,
   378,     0,   321,   378,   376,     0,   376,   377,     0,   377,
     0,   239,   170,     0,   206,   210,     0,    31,     0,     4,
     0,     4,   354,   273,   355,     0,   328,   354,   273,   355,
     0,     0,   380,   395,     0,   380,   395,   392,     0,   380,
   392,     0,   380,   395,   381,     0,   380,   395,   392,   381,
     0,   380,   392,   381,     0,   265,     0,   335,   382,     0,
   383,     0,   384,    20,   383,     0,   384,   224,   383,     0,
   384,   341,   383,     0,   384,     0,   385,   361,   384,     0,
   385,   145,   384,     0,   385,   360,   384,     0,   385,   176,
   384,     0,   385,   359,   384,     0,   385,     0,   386,   207,
   385,     0,   386,   208,   385,     0,   386,   178,   385,     0,
   386,   153,   385,     0,   386,   271,   385,     0,   386,   272,
   385,     0,   386,     0,   386,   212,     0,   386,   213,     0,
   387,     0,   388,    40,   387,     0,   388,    41,   387,     0,
   388,     0,   389,   346,   388,     0,   389,   345,   388,     0,
   389,   366,   388,     0,   389,   367,   388,     0,   389,     0,
   390,   358,   389,     0,   390,   347,   389,     0,   390,   348,
   389,     0,   390,     0,   345,   390,     0,   346,   390,     0,
   368,   390,     0,   206,   390,     0,   321,     0,   321,     0,
     0,   321,   352,   321,     0,   210,     0,    53,     0,   273,
     0,   274,     0,   391,     0,   354,   382,   355,     0,   354,
     0,   355,     0,   382,   351,     0,     0,   138,   393,     0,
   393,   351,   394,     0,   394,     0,   321,     0,   321,   321,
     0,   321,    23,   321,     0,   395,   351,   396,     0,   396,
     0,   397,     0,   398,     0,   397,    23,   321,     0,   397,
   321,     0,   382,     0,   321,   354,   395,   355,     0,   108,
   354,   397,   355,     0,   347,     0,   321,   352,   347,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   545,   555,   560,   561,   570,   575,   581,   587,   590,   591,
   596,   606,   620,   621,   626,   632,   637,   644,   650,   657,
   663,   671,   679,   684,   690,   696,   702,   710,   720,   727,
   732,   738,   742,   746,   751,   756,   760,   764,   768,   772,
   777,   782,   787,   791,   795,   799,   803,   808,   813,   817,
   822,   827,   831,   836,   842,   846,   850,   854,   859,   864,
   868,   872,   877,   883,   887,   891,   895,   908,   914,   925,
   932,   940,   948,   953,   958,   966,   975,   986,  1027,  1033,
  1040,  1076,  1085,  1098,  1105,  1113,  1123,  1128,  1140,  1154,
  1159,  1198,  1206,  1216
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
"REAL_CONST","SIN","SQL_SIZE","SMALLINT","SOME","SPACE","SQL","SQL_TRUE","SQLCA",
"SQLCODE","SQLERROR","SQLSTATE","SQLWARNING","SQRT","STDEV","SUBSTRING","SUM",
"SYSDATE","SYSDATE_FORMAT","SYSTEM","TABLE","TAN","TEMPORARY","THEN","THREE_DIGITS",
"TIME","TIMESTAMP","TIMEZONE_HOUR","TIMEZONE_MINUTE","TINYINT","TO","TO_CHAR",
"TO_DATE","TRANSACTION","TRANSLATE","TRANSLATION","TRUNCATE","GENERAL_TITLE",
"TWO_DIGITS","UCASE","UNION","UNIQUE","SQL_UNKNOWN","UPDATE","UPPER","USAGE",
"USER","IDENTIFIER","IDENTIFIER_DOT_ASTERISK","ERROR_DIGIT_BEFORE_IDENTIFIER",
"USING","VALUE","VALUES","VARBINARY","VARCHAR","VARYING","VENDOR","VIEW","WEEK",
"WHEN","WHENEVER","WHERE","WHERE_CURRENT_OF","WITH","WORD_WRAPPED","WORK","WRAPPED",
"XOR","YEAR","YEARS_BETWEEN","__LAST_TOKEN","'-'","'+'","'*'","'%'","'@'","';'",
"','","'.'","'$'","'('","')'","'?'","'\\''","'/'","'='","'<'","'>'","ILIKE",
"'^'","'['","']'","'&'","'|'","'~'","TopLevelStatement","StatementList","Statement",
"CreateTableStatement","@1","ColDefs","ColDef","ColKeys","ColKey","ColType",
"SelectStatement","Select","WhereClause","aExpr","aExpr2","aExpr3","aExpr4",
"aExpr5","aExpr6","aExpr7","aExpr8","aExpr9","aExpr10","Tables","FlatTableList",
"FlatTable","ColViews","ColItem","ColExpression","ColWildCard", NULL
};
#endif

static const short yyr1[] = {     0,
   369,   370,   370,   370,   371,   371,   373,   372,   374,   374,
   375,   375,   376,   376,   377,   377,   377,   378,   378,   378,
   378,   379,   379,   379,   379,   379,   379,   380,   381,   382,
   383,   383,   383,   383,   384,   384,   384,   384,   384,   384,
   385,   385,   385,   385,   385,   385,   385,   386,   386,   386,
   387,   387,   387,   388,   388,   388,   388,   388,   389,   389,
   389,   389,   390,   390,   390,   390,   390,    -1,   390,   390,
   390,   390,   390,   390,   391,    -1,    -1,   392,   393,   393,
   394,   394,   394,   395,   395,   396,   396,   396,   396,   397,
   397,   397,   398,   398
};

static const short yyr2[] = {     0,
     1,     3,     1,     2,     1,     1,     0,     7,     3,     1,
     2,     3,     2,     1,     2,     2,     1,     1,     4,     4,
     0,     2,     3,     2,     3,     4,     3,     1,     2,     1,
     3,     3,     3,     1,     3,     3,     3,     3,     3,     1,
     3,     3,     3,     3,     3,     3,     1,     2,     2,     1,
     3,     3,     1,     3,     3,     3,     3,     1,     3,     3,
     3,     1,     2,     2,     2,     2,     1,     2,     3,     1,
     1,     1,     1,     1,     3,     3,     3,     2,     3,     1,
     1,     2,     3,     3,     1,     1,     1,     3,     2,     1,
     4,     4,     1,     3
};

static const short yydefact[] = {     0,
     0,    28,     1,     3,     5,     6,     0,     0,     4,    71,
     0,     0,     0,    70,    72,    73,    67,     0,     0,    93,
     0,     0,    90,    30,    34,    40,    47,    50,    53,    58,
    62,    74,    24,    22,    85,    86,    87,     7,     2,     0,
    81,    78,    80,    67,    66,     0,     0,    63,    64,     0,
    65,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    48,    49,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    27,     0,    25,
    23,     0,    89,     0,    67,     0,     0,    82,     0,     0,
    69,    94,     0,    75,    31,    32,    33,    36,    38,    39,
    37,    35,    44,    43,    41,    42,    45,    46,    51,    52,
    55,    54,    56,    57,    60,    61,    59,    29,    84,    26,
    88,     0,    92,    83,    79,    91,    21,     0,    10,    18,
     0,    11,     0,     8,     0,     0,    17,     0,     0,    12,
    14,     9,     0,     0,    16,    15,    13,    19,    20,     0,
     0,     0
};

static const short yydefgoto[] = {   150,
     3,     4,     5,    84,   128,   129,   140,   141,   132,     6,
     7,    78,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    42,    43,    34,    35,    36,    37
};

static const short yypact[] = {   -66,
  -272,-32768,-32768,  -324,-32768,-32768,   -51,  -292,   -66,-32768,
  -318,  -266,   -38,-32768,-32768,-32768,  -321,   -38,   -38,-32768,
   -38,   -38,-32768,-32768,   -19,  -142,  -143,-32768,     0,  -329,
  -328,-32768,  -276,  -127,-32768,   -17,-32768,-32768,-32768,   -45,
   -16,  -289,-32768,  -286,-32768,  -308,   -48,-32768,-32768,  -288,
-32768,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
   -38,   -38,   -38,-32768,-32768,   -38,   -38,   -38,   -38,   -38,
   -38,   -38,   -38,   -38,   -38,   -38,   -38,-32768,   -48,-32768,
  -276,  -240,-32768,  -271,  -296,  -273,  -237,-32768,  -266,  -236,
-32768,-32768,  -330,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,  -235,-32768,-32768,-32768,-32768,    -4,  -323,-32768,  -265,
  -264,   -27,  -235,-32768,  -185,  -182,-32768,  -118,   -77,   -27,
-32768,-32768,  -261,  -260,-32768,-32768,-32768,-32768,-32768,    96,
    97,-32768
};

static const short yypgoto[] = {-32768,
    89,-32768,-32768,-32768,-32768,   -34,-32768,   -40,-32768,-32768,
-32768,   -20,    -9,    23,    -5,   -18,-32768,   -22,     1,     4,
     5,-32768,    67,-32768,    13,    56,    25,    65,-32768
};


#define	YYLAST		330


static const short yytable[] = {   130,
    52,    10,    55,   137,    10,    82,    87,    10,     1,    60,
    12,    50,    91,    80,    10,    70,    71,    45,    74,    75,
    79,     8,    48,    49,   126,     9,    51,   133,    38,    76,
    46,   134,    47,    56,    61,    40,    72,    73,    92,    68,
    69,   103,   104,   105,   106,   109,   110,   107,   108,    98,
    99,   100,   101,   102,    41,    90,    11,    47,    77,    11,
   120,    89,    11,    62,    63,    90,    94,   118,    64,    65,
   111,   112,   113,   114,    95,    96,    97,   115,   116,   117,
   121,   123,   122,   124,    91,   127,    12,   143,   135,   136,
   144,   145,   146,   148,   149,   151,   152,    39,   142,   147,
    81,   125,    93,   119,    86,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    66,    67,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    13,     0,     0,    13,    14,     0,
    13,    14,     0,     0,    14,     0,     0,    13,     0,     0,
     0,    14,     0,     0,     0,     0,     0,     0,   138,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     2,     0,
     0,     0,     0,     0,    53,     0,     0,    77,     0,     0,
     0,   139,     0,     0,     0,     0,    57,    58,    59,     0,
     0,    15,    16,    79,    15,    16,     0,    15,    16,     0,
     0,     0,     0,     0,    15,    16,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    17,
     0,     0,    17,     0,     0,    85,     0,     0,     0,     0,
     0,     0,    44,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    18,    19,    20,    18,    19,    20,    18,
    19,     0,    21,    83,    88,    21,    18,    19,    21,     0,
     0,     0,     0,     0,     0,    21,    22,     0,     0,    22,
     0,    54,    22,   131,     0,     0,     0,     0,     0,    22
};

static const short yycheck[] = {     4,
    20,    53,   145,    31,    53,    23,    23,    53,    75,   153,
   138,    21,   321,    34,    53,   345,   346,    13,   347,   348,
   351,   294,    18,    19,   355,   350,    22,   351,   321,   358,
   352,   355,   354,   176,   178,   354,   366,   367,   347,    40,
    41,    60,    61,    62,    63,    68,    69,    66,    67,    55,
    56,    57,    58,    59,   321,   352,   108,   354,   335,   108,
    81,   351,   108,   207,   208,   352,   355,    77,   212,   213,
    70,    71,    72,    73,    52,    53,    54,    74,    75,    76,
   321,   355,   354,   321,   321,   321,   138,   273,   354,   354,
   273,   210,   170,   355,   355,     0,     0,     9,   133,   140,
    34,    89,    47,    79,    40,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   271,   272,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   206,    -1,    -1,   206,   210,    -1,
   206,   210,    -1,    -1,   210,    -1,    -1,   206,    -1,    -1,
    -1,   210,    -1,    -1,    -1,    -1,    -1,    -1,   206,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   265,    -1,
    -1,    -1,    -1,    -1,   224,    -1,    -1,   335,    -1,    -1,
    -1,   239,    -1,    -1,    -1,    -1,   359,   360,   361,    -1,
    -1,   273,   274,   351,   273,   274,    -1,   273,   274,    -1,
    -1,    -1,    -1,    -1,   273,   274,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   321,
    -1,    -1,   321,    -1,    -1,   321,    -1,    -1,    -1,    -1,
    -1,    -1,   321,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   345,   346,   347,   345,   346,   347,   345,
   346,    -1,   354,   321,   321,   354,   345,   346,   354,    -1,
    -1,    -1,    -1,    -1,    -1,   354,   368,    -1,    -1,   368,
    -1,   341,   368,   328,    -1,    -1,    -1,    -1,    -1,   368
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
#line 547 "sqlparser.y"
{
//todo: multiple statements
//todo: not only "select" statements
	parser->setOperation(Parser::OP_Select);
	parser->setQuerySchema(yyvsp[0].querySchema);
;
    break;}
case 2:
#line 557 "sqlparser.y"
{
//todo: multiple statements
;
    break;}
case 4:
#line 562 "sqlparser.y"
{
	yyval.querySchema = yyvsp[-1].querySchema;
;
    break;}
case 5:
#line 572 "sqlparser.y"
{
YYACCEPT;
;
    break;}
case 6:
#line 576 "sqlparser.y"
{
	yyval.querySchema = yyvsp[0].querySchema;
;
    break;}
case 7:
#line 583 "sqlparser.y"
{
	parser->setOperation(Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
;
    break;}
case 10:
#line 592 "sqlparser.y"
{
;
    break;}
case 11:
#line 598 "sqlparser.y"
{
	kdDebug() << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

//	delete field;
	field = 0;
;
    break;}
case 12:
#line 607 "sqlparser.y"
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
case 14:
#line 622 "sqlparser.y"
{
;
    break;}
case 15:
#line 628 "sqlparser.y"
{
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
;
    break;}
case 16:
#line 633 "sqlparser.y"
{
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
;
    break;}
case 17:
#line 638 "sqlparser.y"
{
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
;
    break;}
case 18:
#line 646 "sqlparser.y"
{
	field = new Field();
	field->setType(yyvsp[0].colType);
;
    break;}
case 19:
#line 651 "sqlparser.y"
{
	kdDebug() << "sql + length" << endl;
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].colType);
;
    break;}
case 20:
#line 658 "sqlparser.y"
{
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(Field::Text);
;
    break;}
case 21:
#line 664 "sqlparser.y"
{
	// SQLITE compatibillity
	field = new Field();
	field->setType(Field::InvalidType);
;
    break;}
case 22:
#line 673 "sqlparser.y"
{
	kdDebug() << "Select ColViews=" << yyvsp[0].exprList->debugString() << endl;

	if (!(yyval.querySchema = parseSelect( yyvsp[-1].querySchema, yyvsp[0].exprList )))
		return 0;
;
    break;}
case 23:
#line 680 "sqlparser.y"
{
	if (!(yyval.querySchema = parseSelect( yyvsp[-2].querySchema, yyvsp[-1].exprList, yyvsp[0].exprList )))
		return 0;
;
    break;}
case 24:
#line 685 "sqlparser.y"
{
	kdDebug() << "Select ColViews Tables" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-1].querySchema, 0, yyvsp[0].exprList )))
		return 0;
;
    break;}
case 25:
#line 691 "sqlparser.y"
{
	kdDebug() << "Select ColViews Conditions" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-2].querySchema, yyvsp[-1].exprList, 0, yyvsp[0].expr )))
		return 0;
;
    break;}
case 26:
#line 697 "sqlparser.y"
{
	kdDebug() << "Select ColViews Tables Conditions" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-3].querySchema, yyvsp[-2].exprList, yyvsp[-1].exprList, yyvsp[0].expr )))
		return 0;
;
    break;}
case 27:
#line 703 "sqlparser.y"
{
	kdDebug() << "Select Tables Conditions" << endl;
	if (!(yyval.querySchema = parseSelect( yyvsp[-2].querySchema, 0, yyvsp[-1].exprList, yyvsp[0].expr )))
		return 0;
;
    break;}
case 28:
#line 712 "sqlparser.y"
{
	kdDebug() << "SELECT" << endl;
//	parser->createSelect();
//	parser->setOperation(Parser::OP_Select);
	yyval.querySchema = new QuerySchema();
;
    break;}
case 29:
#line 722 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
;
    break;}
case 31:
#line 734 "sqlparser.y"
{
//	kdDebug() << "AND " << $3.debugString() << endl;
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, AND, yyvsp[0].expr );
;
    break;}
case 32:
#line 739 "sqlparser.y"
{
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, OR, yyvsp[0].expr );
;
    break;}
case 33:
#line 743 "sqlparser.y"
{
	yyval.expr = new BinaryExpr( KexiDBExpr_Arithm, yyvsp[-2].expr, XOR, yyvsp[0].expr );
;
    break;}
case 35:
#line 753 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '>', yyvsp[0].expr);
;
    break;}
case 36:
#line 757 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_OR_EQUAL, yyvsp[0].expr);
;
    break;}
case 37:
#line 761 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '<', yyvsp[0].expr);
;
    break;}
case 38:
#line 765 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_OR_EQUAL, yyvsp[0].expr);
;
    break;}
case 39:
#line 769 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, '=', yyvsp[0].expr);
;
    break;}
case 41:
#line 779 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL, yyvsp[0].expr);
;
    break;}
case 42:
#line 784 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL2, yyvsp[0].expr);
;
    break;}
case 43:
#line 788 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LIKE, yyvsp[0].expr);
;
    break;}
case 44:
#line 792 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SQL_IN, yyvsp[0].expr);
;
    break;}
case 45:
#line 796 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SIMILAR_TO, yyvsp[0].expr);
;
    break;}
case 46:
#line 800 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_SIMILAR_TO, yyvsp[0].expr);
;
    break;}
case 48:
#line 810 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( SQL_IS_NULL, yyvsp[-1].expr );
;
    break;}
case 49:
#line 814 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( SQL_IS_NOT_NULL, yyvsp[-1].expr );
;
    break;}
case 51:
#line 824 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_LEFT, yyvsp[0].expr);
;
    break;}
case 52:
#line 828 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_RIGHT, yyvsp[0].expr);
;
    break;}
case 54:
#line 838 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '+', yyvsp[0].expr);
	yyval.expr->debug();
;
    break;}
case 55:
#line 843 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '-', yyvsp[0].expr);
;
    break;}
case 56:
#line 847 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '&', yyvsp[0].expr);
;
    break;}
case 57:
#line 851 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '|', yyvsp[0].expr);
;
    break;}
case 59:
#line 861 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '/', yyvsp[0].expr);
;
    break;}
case 60:
#line 865 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '*', yyvsp[0].expr);
;
    break;}
case 61:
#line 869 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '%', yyvsp[0].expr);
;
    break;}
case 63:
#line 880 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( '-', yyvsp[0].expr );
;
    break;}
case 64:
#line 884 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( '+', yyvsp[0].expr );
;
    break;}
case 65:
#line 888 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( '~', yyvsp[0].expr );
;
    break;}
case 66:
#line 892 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( NOT, yyvsp[0].expr );
;
    break;}
case 67:
#line 896 "sqlparser.y"
{
	yyval.expr = new VariableExpr( QString::fromLatin1(yyvsp[0].stringValue) );
	
//TODO: simplify this later if that's 'only one field name' expression
	kdDebug() << "  + identifier: " << yyvsp[0].stringValue << endl;
//	$$ = new Field();
//	$$->setName($1);
//	$$->setTable(dummy);

//	parser->select()->addField(field);
//	requiresTable = true;
;
    break;}
case 68:
#line 909 "sqlparser.y"
{
	kdDebug() << "  + function: " << yyvsp[-1].stringValue << "(" << yyvsp[0].exprList->debugString() << ")" << endl;
	yyval.expr = new FunctionExpr(yyvsp[-1].stringValue, yyvsp[0].exprList);
;
    break;}
case 69:
#line 915 "sqlparser.y"
{
	yyval.expr = new VariableExpr( QString::fromLatin1(yyvsp[-2].stringValue) + "." + QString::fromLatin1(yyvsp[0].stringValue) );
	kdDebug() << "  + identifier.identifier: " << yyvsp[0].stringValue << "." << yyvsp[-2].stringValue << endl;
//	$$ = new Field();
//	s->setTable($1);
//	$$->setName($3);
	//$$->setTable(parser->db()->tableSchema($1));
//	parser->select()->addField(field);
//??	requiresTable = true;
;
    break;}
case 70:
#line 926 "sqlparser.y"
{
	yyval.expr = new ConstExpr( SQL_NULL, QVariant() );
	kdDebug() << "  + NULL" << endl;
//	$$ = new Field();
	//$$->setName(QString::null);
;
    break;}
case 71:
#line 933 "sqlparser.y"
{
	yyval.expr = new ConstExpr( CHARACTER_STRING_LITERAL, yyvsp[0].stringValue );
//	$$ = new Field();
//	$$->setName($1);
//	parser->select()->addField(field);
	kdDebug() << "  + constant \"" << yyvsp[0].stringValue << "\"" << endl;
;
    break;}
case 72:
#line 941 "sqlparser.y"
{
	yyval.expr = new ConstExpr( INTEGER_CONST, yyvsp[0].integerValue );
//	$$ = new Field();
//	$$->setName(QString::number($1));
//	parser->select()->addField(field);
	kdDebug() << "  + int constant: " << yyvsp[0].integerValue << endl;
;
    break;}
case 73:
#line 949 "sqlparser.y"
{
	yyval.expr = new ConstExpr( REAL_CONST, QPoint( yyvsp[0].realValue.integer, yyvsp[0].realValue.fractional ) );
	kdDebug() << "  + real constant: " << yyvsp[0].realValue.integer << "." << yyvsp[0].realValue.fractional << endl;
;
    break;}
case 75:
#line 960 "sqlparser.y"
{
	kdDebug() << "(expr)" << endl;
	yyval.expr = new UnaryExpr('(', yyvsp[-1].expr);
;
    break;}
case 76:
#line 968 "sqlparser.y"
{
//	$$ = new NArgExpr(0, 0);
//	$$->add( $1 );
//	$$->add( $3 );
;
    break;}
case 77:
#line 977 "sqlparser.y"
{
;
    break;}
case 78:
#line 988 "sqlparser.y"
{
	yyval.exprList = yyvsp[0].exprList;
;
    break;}
case 79:
#line 1029 "sqlparser.y"
{
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add(yyvsp[0].expr);
;
    break;}
case 80:
#line 1034 "sqlparser.y"
{
	yyval.exprList = new NArgExpr(KexiDBExpr_TableList, IDENTIFIER); //ok?
	yyval.exprList->add(yyvsp[0].expr);
;
    break;}
case 81:
#line 1042 "sqlparser.y"
{
	kdDebug() << "FROM: '" << yyvsp[0].stringValue << "'" << endl;

//	TableSchema *schema = parser->db()->tableSchema($1);
//	parser->select()->setParentTable(schema);
//	parser->select()->addTable(schema);
//	requiresTable = false;
	
//addTable($1);

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
;
    break;}
case 82:
#line 1077 "sqlparser.y"
{
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, 
		new VariableExpr(QString::fromLatin1(yyvsp[-1].stringValue)), 0,
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
;
    break;}
case 83:
#line 1086 "sqlparser.y"
{
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary,
		new VariableExpr(QString::fromLatin1(yyvsp[-2].stringValue)), AS,
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
;
    break;}
case 84:
#line 1100 "sqlparser.y"
{
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColViews , ColItem" << endl;
;
    break;}
case 85:
#line 1106 "sqlparser.y"
{
	yyval.exprList = new NArgExpr(0,0);
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColItem" << endl;
;
    break;}
case 86:
#line 1115 "sqlparser.y"
{
//	$$ = new Field();
//	dummy->addField($$);
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column expr: '" << yyvsp[0].expr->debugString() << "'" << endl;
;
    break;}
case 87:
#line 1124 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column wildcard: '" << yyvsp[0].expr->debugString() << "'" << endl;
;
    break;}
case 88:
#line 1129 "sqlparser.y"
{
//	$$ = new Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, yyvsp[-2].expr, AS,
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
//		new ConstExpr(IDENTIFIER, QString::fromLocal8Bit($3))
	);
	kdDebug() << " added column expr: " << yyval.expr->debugString() << endl;
;
    break;}
case 89:
#line 1141 "sqlparser.y"
{
//	$$ = new Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, yyvsp[-1].expr, 0, 
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
//		new ConstExpr(IDENTIFIER, QString::fromLocal8Bit($2))
	);
	kdDebug() << " added column expr: " << yyval.expr->debugString() << endl;
;
    break;}
case 90:
#line 1156 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
;
    break;}
case 91:
#line 1160 "sqlparser.y"
{
	yyval.expr = new FunctionExpr( yyvsp[-3].stringValue, yyvsp[-1].exprList );
;
    break;}
case 92:
#line 1199 "sqlparser.y"
{
	yyval.expr = yyvsp[-1].expr;
//TODO
//	$$->setName("DISTINCT(" + $3->name() + ")");
;
    break;}
case 93:
#line 1208 "sqlparser.y"
{
	yyval.expr = new VariableExpr("*");
	kdDebug() << "all columns" << endl;

//	QueryAsterisk *ast = new QueryAsterisk(parser->select(), dummy);
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
;
    break;}
case 94:
#line 1217 "sqlparser.y"
{
	QString s = QString::fromLatin1(yyvsp[-2].stringValue);
	s+=".*";
	yyval.expr = new VariableExpr(s);
	kdDebug() << "  + all columns from " << s << endl;
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
#line 1231 "sqlparser.y"


const char * const tname(int offset) { return yytname[offset]; }
