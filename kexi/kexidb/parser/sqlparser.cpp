
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
#define	HAVING	397
#define	HOUR	398
#define	HOURS_BETWEEN	399
#define	IDENTITY	400
#define	IFNULL	401
#define	SQL_IGNORE	402
#define	IMMEDIATE	403
#define	SQL_IN	404
#define	INCLUDE	405
#define	INDEX	406
#define	INDICATOR	407
#define	INITIALLY	408
#define	INNER	409
#define	INPUT	410
#define	INSENSITIVE	411
#define	INSERT	412
#define	INTEGER	413
#define	INTERSECT	414
#define	INTERVAL	415
#define	INTO	416
#define	IS	417
#define	ISOLATION	418
#define	JOIN	419
#define	JUSTIFY	420
#define	KEY	421
#define	LANGUAGE	422
#define	LAST	423
#define	LCASE	424
#define	LEFT	425
#define	LENGTH	426
#define	LESS_OR_EQUAL	427
#define	LESS_THAN	428
#define	LEVEL	429
#define	LIKE	430
#define	LINE_WIDTH	431
#define	LOCAL	432
#define	LOCATE	433
#define	LOG	434
#define	SQL_LONG	435
#define	LOWER	436
#define	LTRIM	437
#define	LTRIP	438
#define	MATCH	439
#define	SQL_MAX	440
#define	MICROSOFT	441
#define	SQL_MIN	442
#define	MINUS	443
#define	MINUTE	444
#define	MINUTES_BETWEEN	445
#define	MOD	446
#define	MODIFY	447
#define	MODULE	448
#define	MONTH	449
#define	MONTHS_BETWEEN	450
#define	MUMPS	451
#define	NAMES	452
#define	NATIONAL	453
#define	NCHAR	454
#define	NEXT	455
#define	NODUP	456
#define	NONE	457
#define	NOT	458
#define	NOT_EQUAL	459
#define	NOW	460
#define	SQL_NULL	461
#define	NULLIF	462
#define	NUMERIC	463
#define	OCTET_LENGTH	464
#define	ODBC	465
#define	OF	466
#define	SQL_OFF	467
#define	SQL_ON	468
#define	ONLY	469
#define	OPEN	470
#define	OPTION	471
#define	OR	472
#define	ORDER	473
#define	OUTER	474
#define	OUTPUT	475
#define	OVERLAPS	476
#define	PAGE	477
#define	PARTIAL	478
#define	SQL_PASCAL	479
#define	PERSISTENT	480
#define	CQL_PI	481
#define	PLI	482
#define	POSITION	483
#define	PRECISION	484
#define	PREPARE	485
#define	PRESERVE	486
#define	PRIMARY	487
#define	PRIOR	488
#define	PRIVILEGES	489
#define	PROCEDURE	490
#define	PRODUCT	491
#define	PUBLIC	492
#define	QUARTER	493
#define	QUIT	494
#define	RAND	495
#define	READ_ONLY	496
#define	REAL	497
#define	REFERENCES	498
#define	REPEAT	499
#define	REPLACE	500
#define	RESTRICT	501
#define	REVOKE	502
#define	RIGHT	503
#define	ROLLBACK	504
#define	ROWS	505
#define	RPAD	506
#define	RTRIM	507
#define	SCHEMA	508
#define	SCREEN_WIDTH	509
#define	SCROLL	510
#define	SECOND	511
#define	SECONDS_BETWEEN	512
#define	SELECT	513
#define	SEQUENCE	514
#define	SETOPT	515
#define	SET	516
#define	SHOWOPT	517
#define	SIGN	518
#define	INTEGER_CONST	519
#define	REAL_CONST	520
#define	SIN	521
#define	SQL_SIZE	522
#define	SMALLINT	523
#define	SOME	524
#define	SPACE	525
#define	SQL	526
#define	SQL_TRUE	527
#define	SQLCA	528
#define	SQLCODE	529
#define	SQLERROR	530
#define	SQLSTATE	531
#define	SQLWARNING	532
#define	SQRT	533
#define	STDEV	534
#define	SUBSTRING	535
#define	SUM	536
#define	SYSDATE	537
#define	SYSDATE_FORMAT	538
#define	SYSTEM	539
#define	TABLE	540
#define	TAN	541
#define	TEMPORARY	542
#define	THEN	543
#define	THREE_DIGITS	544
#define	TIME	545
#define	TIMESTAMP	546
#define	TIMEZONE_HOUR	547
#define	TIMEZONE_MINUTE	548
#define	TINYINT	549
#define	TO	550
#define	TO_CHAR	551
#define	TO_DATE	552
#define	TRANSACTION	553
#define	TRANSLATE	554
#define	TRANSLATION	555
#define	TRUNCATE	556
#define	GENERAL_TITLE	557
#define	TWO_DIGITS	558
#define	UCASE	559
#define	UNION	560
#define	UNIQUE	561
#define	SQL_UNKNOWN	562
#define	UPDATE	563
#define	UPPER	564
#define	USAGE	565
#define	USER	566
#define	IDENTIFIER	567
#define	IDENTIFIER_DOT_ASTERISK	568
#define	ERROR_DIGIT_BEFORE_IDENTIFIER	569
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

#line 723 "sqlparser.y"
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



#define	YYFINAL		115
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
     0,     2,     6,     8,    11,    13,    15,    16,    24,    28,
    30,    33,    37,    40,    42,    45,    48,    50,    52,    57,
    62,    63,    66,    70,    73,    77,    82,    84,    87,    91,
    95,    98,   102,   106,   110,   114,   118,   122,   126,   130,
   134,   138,   142,   146,   151,   153,   157,   159,   161,   163,
   165,   169,   173,   174,   177,   181,   183,   185,   188,   192,
   196,   198,   200,   202,   206,   209,   211,   216,   218
};

static const short yyrhs[] = {   356,
     0,   357,   340,   356,     0,   357,     0,   357,   340,     0,
   358,     0,   365,     0,     0,    71,   286,   313,   359,   344,
   360,   345,     0,   360,   341,   361,     0,   361,     0,   313,
   364,     0,   313,   364,   362,     0,   362,   363,     0,   363,
     0,   233,   167,     0,   204,   207,     0,    30,     0,     3,
     0,     3,   344,   265,   345,     0,   320,   344,   265,   345,
     0,     0,   366,   373,     0,   366,   373,   370,     0,   366,
   370,     0,   366,   373,   367,     0,   366,   373,   370,   367,
     0,   259,     0,   327,   368,     0,   368,    19,   368,     0,
   368,   218,   368,     0,   204,   368,     0,   368,   336,   368,
     0,   368,   335,   368,     0,   368,   348,   368,     0,   368,
   337,   368,     0,   368,   338,   368,     0,   368,   205,   368,
     0,   368,   142,   368,     0,   368,   141,   368,     0,   368,
   174,   368,     0,   368,   173,   368,     0,   368,   176,   368,
     0,   368,   150,   368,     0,   313,   344,   369,   345,     0,
   313,     0,   313,   342,   313,     0,   207,     0,    50,     0,
   265,     0,   266,     0,   344,   368,   345,     0,   369,   341,
   368,     0,     0,   134,   371,     0,   371,   341,   372,     0,
   372,     0,   313,     0,   313,   313,     0,   313,    22,   313,
     0,   373,   341,   374,     0,   374,     0,   375,     0,   376,
     0,   375,    22,   313,     0,   375,   313,     0,   368,     0,
   104,   344,   375,   345,     0,   337,     0,   314,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   782,   792,   797,   798,   807,   812,   818,   824,   827,   828,
   833,   843,   857,   858,   863,   869,   874,   881,   887,   894,
   900,   908,   929,  1063,  1067,  1071,  1077,  1087,  1098,  1104,
  1109,  1114,  1118,  1122,  1126,  1130,  1134,  1138,  1142,  1146,
  1150,  1154,  1158,  1162,  1167,  1181,  1192,  1199,  1207,  1215,
  1220,  1227,  1233,  1239,  1280,  1286,  1293,  1329,  1338,  1351,
  1358,  1366,  1376,  1381,  1393,  1407,  1446,  1454,  1464
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
"GLOBAL","GO","GOTO","GRANT","GREATER_OR_EQUAL","GREATER_THAN","HAVING","HOUR",
"HOURS_BETWEEN","IDENTITY","IFNULL","SQL_IGNORE","IMMEDIATE","SQL_IN","INCLUDE",
"INDEX","INDICATOR","INITIALLY","INNER","INPUT","INSENSITIVE","INSERT","INTEGER",
"INTERSECT","INTERVAL","INTO","IS","ISOLATION","JOIN","JUSTIFY","KEY","LANGUAGE",
"LAST","LCASE","LEFT","LENGTH","LESS_OR_EQUAL","LESS_THAN","LEVEL","LIKE","LINE_WIDTH",
"LOCAL","LOCATE","LOG","SQL_LONG","LOWER","LTRIM","LTRIP","MATCH","SQL_MAX",
"MICROSOFT","SQL_MIN","MINUS","MINUTE","MINUTES_BETWEEN","MOD","MODIFY","MODULE",
"MONTH","MONTHS_BETWEEN","MUMPS","NAMES","NATIONAL","NCHAR","NEXT","NODUP","NONE",
"NOT","NOT_EQUAL","NOW","SQL_NULL","NULLIF","NUMERIC","OCTET_LENGTH","ODBC",
"OF","SQL_OFF","SQL_ON","ONLY","OPEN","OPTION","OR","ORDER","OUTER","OUTPUT",
"OVERLAPS","PAGE","PARTIAL","SQL_PASCAL","PERSISTENT","CQL_PI","PLI","POSITION",
"PRECISION","PREPARE","PRESERVE","PRIMARY","PRIOR","PRIVILEGES","PROCEDURE",
"PRODUCT","PUBLIC","QUARTER","QUIT","RAND","READ_ONLY","REAL","REFERENCES","REPEAT",
"REPLACE","RESTRICT","REVOKE","RIGHT","ROLLBACK","ROWS","RPAD","RTRIM","SCHEMA",
"SCREEN_WIDTH","SCROLL","SECOND","SECONDS_BETWEEN","SELECT","SEQUENCE","SETOPT",
"SET","SHOWOPT","SIGN","INTEGER_CONST","REAL_CONST","SIN","SQL_SIZE","SMALLINT",
"SOME","SPACE","SQL","SQL_TRUE","SQLCA","SQLCODE","SQLERROR","SQLSTATE","SQLWARNING",
"SQRT","STDEV","SUBSTRING","SUM","SYSDATE","SYSDATE_FORMAT","SYSTEM","TABLE",
"TAN","TEMPORARY","THEN","THREE_DIGITS","TIME","TIMESTAMP","TIMEZONE_HOUR","TIMEZONE_MINUTE",
"TINYINT","TO","TO_CHAR","TO_DATE","TRANSACTION","TRANSLATE","TRANSLATION","TRUNCATE",
"GENERAL_TITLE","TWO_DIGITS","UCASE","UNION","UNIQUE","SQL_UNKNOWN","UPDATE",
"UPPER","USAGE","USER","IDENTIFIER","IDENTIFIER_DOT_ASTERISK","ERROR_DIGIT_BEFORE_IDENTIFIER",
"USING","VALUE","VALUES","VARBINARY","VARCHAR","VARYING","VENDOR","VIEW","WEEK",
"WHEN","WHENEVER","WHERE","WHERE_CURRENT_OF","WITH","WORD_WRAPPED","WORK","WRAPPED",
"YEAR","YEARS_BETWEEN","'-'","'+'","'*'","'%'","'@'","';'","','","'.'","'$'",
"'('","')'","'?'","'\\''","'/'","'='","ILIKE","SIMILAR","'^'","'['","']'","TopLevelStatement",
"StatementList","Statement","CreateTableStatement","@1","ColDefs","ColDef","ColKeys",
"ColKey","ColType","SelectStatement","Select","WhereClause","aExpr","aExprList",
"Tables","FlatTableList","FlatTable","ColViews","ColItem","ColExpression","ColWildCard", NULL
};
#endif

static const short yyr1[] = {     0,
   355,   356,   356,   356,   357,   357,   359,   358,   360,   360,
   361,   361,   362,   362,   363,   363,   363,   364,   364,   364,
   364,   365,   365,   365,   365,   365,   366,   367,   368,   368,
   368,   368,   368,   368,   368,   368,   368,   368,   368,   368,
   368,   368,   368,   368,   368,   368,   368,   368,   368,   368,
   368,   369,   369,   370,   371,   371,   372,   372,   372,   373,
   373,   374,   374,   374,   374,   375,   375,   376,   376
};

static const short yyr2[] = {     0,
     1,     3,     1,     2,     1,     1,     0,     7,     3,     1,
     2,     3,     2,     1,     2,     2,     1,     1,     4,     4,
     0,     2,     3,     2,     3,     4,     1,     2,     3,     3,
     2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     4,     1,     3,     1,     1,     1,     1,
     3,     3,     0,     2,     3,     1,     1,     2,     3,     3,
     1,     1,     1,     3,     2,     1,     4,     1,     1
};

static const short yydefact[] = {     0,
     0,    27,     1,     3,     5,     6,     0,     0,     4,    48,
     0,     0,     0,    47,    49,    50,    45,    69,    68,     0,
    66,    24,    22,    61,    62,    63,     7,     2,     0,    57,
    54,    56,    31,     0,    53,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    25,    23,     0,    65,     0,     0,     0,    58,
     0,    46,     0,    51,    29,    39,    38,    43,    41,    40,
    42,    37,    30,    33,    32,    35,    36,    34,    28,    60,
    26,    64,     0,    67,    59,    55,     0,    44,    21,     0,
    10,    52,    18,     0,    11,     0,     8,     0,     0,    17,
     0,     0,    12,    14,     9,     0,     0,    16,    15,    13,
    19,    20,     0,     0,     0
};

static const short yydefgoto[] = {   113,
     3,     4,     5,    57,    90,    91,   103,   104,    95,     6,
     7,    53,    21,    63,    22,    31,    32,    23,    24,    25,
    26
};

static const short yypact[] = {   -68,
  -275,-32768,-32768,  -313,-32768,-32768,   -44,  -294,   -68,-32768,
  -316,  -284,   -38,-32768,-32768,-32768,  -319,-32768,-32768,   -38,
   -15,-32768,  -125,-32768,   -21,-32768,-32768,-32768,   -40,   -17,
  -311,-32768,  -128,  -282,-32768,   -19,   -38,   -38,   -38,   -38,
   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
   -38,   -42,-32768,  -295,  -280,-32768,  -310,  -308,  -274,-32768,
  -284,-32768,  -324,-32768,  -128,  -106,  -106,  -101,  -106,  -106,
  -101,  -106,    -1,  -322,  -322,-32768,-32768,-32768,   -15,-32768,
-32768,-32768,  -273,-32768,-32768,-32768,   -38,-32768,     4,  -321,
-32768,   -15,  -306,  -303,   -28,  -273,-32768,  -223,  -222,-32768,
  -160,  -117,   -28,-32768,-32768,  -293,  -292,-32768,-32768,-32768,
-32768,-32768,    51,    54,-32768
};

static const short yypgoto[] = {-32768,
    46,-32768,-32768,-32768,-32768,   -39,-32768,   -47,-32768,-32768,
-32768,     5,    63,-32768,    35,-32768,     0,-32768,    11,    36,
-32768
};


#define	YYLAST		347


static const short yytable[] = {    37,
    55,   100,     1,    37,    59,    10,    93,    10,    12,    10,
     8,    10,    38,    39,    48,    49,    87,    37,    27,    96,
    88,    40,    34,    97,    35,    50,     9,    29,    30,    61,
    62,    51,    82,    83,-32768,-32768,    84,    98,    85,    89,
    99,   106,   107,    40,    41,    42,   108,    43,-32768,   109,
   114,   111,   112,   115,    28,   110,   105,    54,    81,    11,
    86,    11,    80,    11,    58,     0,-32768,-32768,     0,    43,
     0,     0,     0,     0,-32768,    33,    44,     0,     0,     0,
     0,     0,    36,     0,     0,     0,     0,     0,     0,    12,
     0,     0,     0,     0,     0,     0,     0,     0,-32768,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,     0,     0,     0,     0,     0,     0,
     0,    38,    39,     0,     0,    38,    39,     0,     0,     0,
    40,     0,     0,     0,    40,     0,     0,     0,     0,    38,
    39,     0,     0,     0,     0,     0,     0,     0,    40,    92,
     0,     0,     0,    41,    42,     0,    43,    41,    42,    13,
    43,    13,    14,    13,    14,    13,    14,     0,    14,     0,
     0,    41,    42,     0,    43,   101,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    44,     0,     0,     0,    44,
     2,     0,     0,     0,     0,     0,     0,     0,    45,     0,
     0,    51,    45,    44,   102,     0,    46,    47,    48,    49,
     0,     0,     0,     0,     0,    52,     0,     0,     0,    50,
    15,    16,    15,    16,    15,    16,    15,    16,    46,    47,
    48,    49,     0,    46,    47,    48,    49,     0,     0,     0,
     0,    50,     0,     0,     0,     0,    50,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    17,    18,
    17,    18,    17,     0,    17,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    56,    19,     0,    19,    60,     0,     0,     0,    20,
     0,    20,     0,    20,     0,    20,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    46,    47,    48,    49,    46,
    47,    48,    49,    94,     0,    64,     0,     0,    50,     0,
     0,     0,    50,    46,    47,    48,    49,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    50
};

static const short yycheck[] = {    19,
    22,    30,    71,    19,    22,    50,     3,    50,   134,    50,
   286,    50,   141,   142,   337,   338,   341,    19,   313,   341,
   345,   150,   342,   345,   344,   348,   340,   344,   313,   341,
   313,   327,   313,   344,   141,   142,   345,   344,   313,   313,
   344,   265,   265,   150,   173,   174,   207,   176,   150,   167,
     0,   345,   345,     0,     9,   103,    96,    23,    54,   104,
    61,   104,    52,   104,    29,    -1,   173,   174,    -1,   176,
    -1,    -1,    -1,    -1,   176,    13,   205,    -1,    -1,    -1,
    -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,   134,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   205,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   141,   142,    -1,    -1,   141,   142,    -1,    -1,    -1,
   150,    -1,    -1,    -1,   150,    -1,    -1,    -1,    -1,   141,
   142,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   150,    87,
    -1,    -1,    -1,   173,   174,    -1,   176,   173,   174,   204,
   176,   204,   207,   204,   207,   204,   207,    -1,   207,    -1,
    -1,   173,   174,    -1,   176,   204,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   205,    -1,    -1,    -1,   205,
   259,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   218,    -1,
    -1,   327,   218,   205,   233,    -1,   335,   336,   337,   338,
    -1,    -1,    -1,    -1,    -1,   341,    -1,    -1,    -1,   348,
   265,   266,   265,   266,   265,   266,   265,   266,   335,   336,
   337,   338,    -1,   335,   336,   337,   338,    -1,    -1,    -1,
    -1,   348,    -1,    -1,    -1,    -1,   348,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   313,   314,
   313,   314,   313,    -1,   313,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   313,   337,    -1,   337,   313,    -1,    -1,    -1,   344,
    -1,   344,    -1,   344,    -1,   344,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   335,   336,   337,   338,   335,
   336,   337,   338,   320,    -1,   345,    -1,    -1,   348,    -1,
    -1,    -1,   348,   335,   336,   337,   338,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   348
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
#line 784 "sqlparser.y"
{
//todo: multiple statements
//todo: not only "select" statements
	parser->setOperation(KexiDB::Parser::OP_Select);
	parser->setQuerySchema(yyvsp[0].querySchema);
;
    break;}
case 2:
#line 794 "sqlparser.y"
{
//todo: multiple statements
;
    break;}
case 4:
#line 799 "sqlparser.y"
{
	yyval.querySchema = yyvsp[-1].querySchema;
;
    break;}
case 5:
#line 809 "sqlparser.y"
{
YYACCEPT;
;
    break;}
case 6:
#line 813 "sqlparser.y"
{
	yyval.querySchema = yyvsp[0].querySchema;
;
    break;}
case 7:
#line 820 "sqlparser.y"
{
	parser->setOperation(KexiDB::Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
;
    break;}
case 10:
#line 829 "sqlparser.y"
{
;
    break;}
case 11:
#line 835 "sqlparser.y"
{
	kdDebug() << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

//	delete field;
	field = 0;
;
    break;}
case 12:
#line 844 "sqlparser.y"
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
#line 859 "sqlparser.y"
{
;
    break;}
case 15:
#line 865 "sqlparser.y"
{
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
;
    break;}
case 16:
#line 870 "sqlparser.y"
{
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
;
    break;}
case 17:
#line 875 "sqlparser.y"
{
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
;
    break;}
case 18:
#line 883 "sqlparser.y"
{
	field = new KexiDB::Field();
	field->setType(yyvsp[0].colType);
;
    break;}
case 19:
#line 888 "sqlparser.y"
{
	kdDebug() << "sql + length" << endl;
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].colType);
;
    break;}
case 20:
#line 895 "sqlparser.y"
{
	field = new KexiDB::Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(KexiDB::Field::Text);
;
    break;}
case 21:
#line 901 "sqlparser.y"
{
	// SQLITE compatibillity
	field = new KexiDB::Field();
	field->setType(KexiDB::Field::InvalidType);
;
    break;}
case 22:
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
;
    break;}
case 23:
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
		QString tname = t_e->name.latin1();
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
;
    break;}
case 24:
#line 1064 "sqlparser.y"
{
	kdDebug() << "Select ColViews Tables" << endl;
;
    break;}
case 25:
#line 1068 "sqlparser.y"
{
	kdDebug() << "Select ColViews Conditions" << endl;
;
    break;}
case 26:
#line 1072 "sqlparser.y"
{
	kdDebug() << "Select ColViews Tables Conditions" << endl;
;
    break;}
case 27:
#line 1079 "sqlparser.y"
{
	kdDebug() << "SELECT" << endl;
//	parser->createSelect();
//	parser->setOperation(KexiDB::Parser::OP_Select);
	yyval.querySchema = new QuerySchema();
;
    break;}
case 28:
#line 1089 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
;
    break;}
case 29:
#line 1100 "sqlparser.y"
{
//	kdDebug() << "AND " << $3.debugString() << endl;
	yyval.expr = new KexiDB::BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, AND, yyvsp[0].expr );
;
    break;}
case 30:
#line 1105 "sqlparser.y"
{
//	kdDebug() << "OR " << $3 << endl;
	yyval.expr = new KexiDB::BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, OR, yyvsp[0].expr );
;
    break;}
case 31:
#line 1110 "sqlparser.y"
{
	yyval.expr = new KexiDB::UnaryExpr( NOT, yyvsp[0].expr );
//	$$->setName($1->name() + " NOT " + $3->name());
;
    break;}
case 32:
#line 1115 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '+', yyvsp[0].expr);
;
    break;}
case 33:
#line 1119 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '-', yyvsp[0].expr);
;
    break;}
case 34:
#line 1123 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '/', yyvsp[0].expr);
;
    break;}
case 35:
#line 1127 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '*', yyvsp[0].expr);
;
    break;}
case 36:
#line 1131 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '%', yyvsp[0].expr);
;
    break;}
case 37:
#line 1135 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL, yyvsp[0].expr);
;
    break;}
case 38:
#line 1139 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_THAN, yyvsp[0].expr);
;
    break;}
case 39:
#line 1143 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_OR_EQUAL, yyvsp[0].expr);
;
    break;}
case 40:
#line 1147 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_THAN, yyvsp[0].expr);
;
    break;}
case 41:
#line 1151 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_OR_EQUAL, yyvsp[0].expr);
;
    break;}
case 42:
#line 1155 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LIKE, yyvsp[0].expr);
;
    break;}
case 43:
#line 1159 "sqlparser.y"
{
	yyval.expr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SQL_IN, yyvsp[0].expr);
;
    break;}
case 44:
#line 1163 "sqlparser.y"
{
	kdDebug() << "  + function: " << yyvsp[-3].stringValue << "(" << yyvsp[-1].exprList->debugString() << ")" << endl;
	yyval.expr = new KexiDB::FunctionExpr(yyvsp[-3].stringValue, yyvsp[-1].exprList);
;
    break;}
case 45:
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
;
    break;}
case 46:
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
;
    break;}
case 47:
#line 1193 "sqlparser.y"
{
	yyval.expr = new KexiDB::ConstExpr( SQL_NULL, QVariant() );
	kdDebug() << "  + NULL" << endl;
//	$$ = new KexiDB::Field();
	//$$->setName(QString::null);
;
    break;}
case 48:
#line 1200 "sqlparser.y"
{
	yyval.expr = new KexiDB::ConstExpr( CHARACTER_STRING_LITERAL, yyvsp[0].stringValue );
//	$$ = new KexiDB::Field();
//	$$->setName($1);
//	parser->select()->addField(field);
	kdDebug() << "  + constant \"" << yyvsp[0].stringValue << "\"" << endl;
;
    break;}
case 49:
#line 1208 "sqlparser.y"
{
	yyval.expr = new KexiDB::ConstExpr( INTEGER_CONST, yyvsp[0].integerValue );
//	$$ = new KexiDB::Field();
//	$$->setName(QString::number($1));
//	parser->select()->addField(field);
	kdDebug() << "  + int constant: " << yyvsp[0].integerValue << endl;
;
    break;}
case 50:
#line 1216 "sqlparser.y"
{
	yyval.expr = new KexiDB::ConstExpr( REAL_CONST, QPoint( yyvsp[0].realValue.integer, yyvsp[0].realValue.fractional ) );
	kdDebug() << "  + real constant: " << yyvsp[0].realValue.integer << "." << yyvsp[0].realValue.fractional << endl;
;
    break;}
case 51:
#line 1221 "sqlparser.y"
{
	kdDebug() << "(expr)" << endl;
	yyval.expr = yyvsp[-1].expr;
;
    break;}
case 52:
#line 1229 "sqlparser.y"
{
	yyvsp[-2].exprList->add( yyvsp[0].expr );
	yyval.exprList = yyvsp[-2].exprList;
;
    break;}
case 53:
#line 1234 "sqlparser.y"
{
	yyval.exprList = new KexiDB::NArgExpr(0, 0/*unknown*/);
;
    break;}
case 54:
#line 1241 "sqlparser.y"
{
	yyval.exprList = yyvsp[0].exprList;
;
    break;}
case 55:
#line 1282 "sqlparser.y"
{
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add(yyvsp[0].expr);
;
    break;}
case 56:
#line 1287 "sqlparser.y"
{
	yyval.exprList = new KexiDB::NArgExpr(KexiDBExpr_TableList, IDENTIFIER); //ok?
	yyval.exprList->add(yyvsp[0].expr);
;
    break;}
case 57:
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
;
    break;}
case 58:
#line 1330 "sqlparser.y"
{
	//table + alias
	yyval.expr = new KexiDB::BinaryExpr(
		KexiDBExpr_SpecialBinary, 
		new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[-1].stringValue)), 0,
		new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
;
    break;}
case 59:
#line 1339 "sqlparser.y"
{
	//table + alias
	yyval.expr = new KexiDB::BinaryExpr(
		KexiDBExpr_SpecialBinary,
		new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[-2].stringValue)), AS,
		new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
;
    break;}
case 60:
#line 1353 "sqlparser.y"
{
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColViews , ColItem" << endl;
;
    break;}
case 61:
#line 1359 "sqlparser.y"
{
	yyval.exprList = new KexiDB::NArgExpr(0,0);
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColItem" << endl;
;
    break;}
case 62:
#line 1368 "sqlparser.y"
{
//	$$ = new KexiDB::Field();
//	dummy->addField($$);
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column expr: '" << yyvsp[0].expr->debugString() << "'" << endl;
;
    break;}
case 63:
#line 1377 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column wildcard: '" << yyvsp[0].expr->debugString() << "'" << endl;
;
    break;}
case 64:
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
;
    break;}
case 65:
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
;
    break;}
case 66:
#line 1409 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
;
    break;}
case 67:
#line 1447 "sqlparser.y"
{
	yyval.expr = yyvsp[-1].expr;
//TODO
//	$$->setName("DISTINCT(" + $3->name() + ")");
;
    break;}
case 68:
#line 1456 "sqlparser.y"
{
	yyval.expr = new KexiDB::VariableExpr("*");
	kdDebug() << "all columns" << endl;

//	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), dummy);
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
;
    break;}
case 69:
#line 1465 "sqlparser.y"
{
	yyval.expr = new KexiDB::VariableExpr(QString::fromLatin1(yyvsp[0].stringValue));
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
#line 1480 "sqlparser.y"


