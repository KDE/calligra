
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
#define	BITWISE_SHIFT_LEFT	293
#define	BITWISE_SHIFT_RIGHT	294
#define	BREAK	295
#define	BY	296
#define	CASCADE	297
#define	CASCADED	298
#define	CASE	299
#define	CAST	300
#define	CATALOG	301
#define	CEILING	302
#define	CENTER	303
#define	SQL_CHAR	304
#define	CHAR_LENGTH	305
#define	CHARACTER_STRING_LITERAL	306
#define	CHECK	307
#define	CLOSE	308
#define	COALESCE	309
#define	COBOL	310
#define	COLLATE	311
#define	COLLATION	312
#define	COLUMN	313
#define	COMMIT	314
#define	COMPUTE	315
#define	CONCAT	316
#define	CONCATENATION	317
#define	CONNECT	318
#define	CONNECTION	319
#define	CONSTRAINT	320
#define	CONSTRAINTS	321
#define	CONTINUE	322
#define	CONVERT	323
#define	CORRESPONDING	324
#define	COS	325
#define	COT	326
#define	COUNT	327
#define	CREATE	328
#define	CURDATE	329
#define	CURRENT	330
#define	CURRENT_DATE	331
#define	CURRENT_TIME	332
#define	CURRENT_TIMESTAMP	333
#define	CURTIME	334
#define	CURSOR	335
#define	DATABASE	336
#define	SQL_DATE	337
#define	DATE_FORMAT	338
#define	DATE_REMAINDER	339
#define	DATE_VALUE	340
#define	DAY	341
#define	DAYOFMONTH	342
#define	DAYOFWEEK	343
#define	DAYOFYEAR	344
#define	DAYS_BETWEEN	345
#define	DEALLOCATE	346
#define	DEC	347
#define	DECLARE	348
#define	DEFAULT	349
#define	DEFERRABLE	350
#define	DEFERRED	351
#define	SQL_DELETE	352
#define	DESC	353
#define	DESCRIBE	354
#define	DESCRIPTOR	355
#define	DIAGNOSTICS	356
#define	DICTIONARY	357
#define	DIRECTORY	358
#define	DISCONNECT	359
#define	DISPLACEMENT	360
#define	DISTINCT	361
#define	DOMAIN_TOKEN	362
#define	SQL_DOUBLE	363
#define	DOUBLE_QUOTED_STRING	364
#define	DROP	365
#define	ELSE	366
#define	END	367
#define	END_EXEC	368
#define	EQUAL	369
#define	ESCAPE	370
#define	EXCEPT	371
#define	SQL_EXCEPTION	372
#define	EXEC	373
#define	EXECUTE	374
#define	EXISTS	375
#define	EXP	376
#define	EXPONENT	377
#define	EXTERNAL	378
#define	EXTRACT	379
#define	SQL_FALSE	380
#define	FETCH	381
#define	FIRST	382
#define	SQL_FLOAT	383
#define	FLOOR	384
#define	FN	385
#define	FOR	386
#define	FOREIGN	387
#define	FORTRAN	388
#define	FOUND	389
#define	FOUR_DIGITS	390
#define	FROM	391
#define	FULL	392
#define	GET	393
#define	GLOBAL	394
#define	GO	395
#define	GOTO	396
#define	GRANT	397
#define	GREATER_OR_EQUAL	398
#define	GREATER_THAN	399
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
#define	LESS_THAN	431
#define	LEVEL	432
#define	LIKE	433
#define	LINE_WIDTH	434
#define	LOCAL	435
#define	LOCATE	436
#define	LOG	437
#define	SQL_LONG	438
#define	LOWER	439
#define	LTRIM	440
#define	LTRIP	441
#define	MATCH	442
#define	SQL_MAX	443
#define	MICROSOFT	444
#define	SQL_MIN	445
#define	MINUS	446
#define	MINUTE	447
#define	MINUTES_BETWEEN	448
#define	MOD	449
#define	MODIFY	450
#define	MODULE	451
#define	MONTH	452
#define	MONTHS_BETWEEN	453
#define	MUMPS	454
#define	NAMES	455
#define	NATIONAL	456
#define	NCHAR	457
#define	NEXT	458
#define	NODUP	459
#define	NONE	460
#define	NOT	461
#define	NOT_EQUAL	462
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
#define	SIMILAR	525
#define	SIMILAR_TO	526
#define	NOT_SIMILAR_TO	527
#define	INTEGER_CONST	528
#define	REAL_CONST	529
#define	SIN	530
#define	SQL_SIZE	531
#define	SMALLINT	532
#define	SOME	533
#define	SPACE	534
#define	SQL	535
#define	SQL_TRUE	536
#define	SQLCA	537
#define	SQLCODE	538
#define	SQLERROR	539
#define	SQLSTATE	540
#define	SQLWARNING	541
#define	SQRT	542
#define	STDEV	543
#define	SUBSTRING	544
#define	SUM	545
#define	SYSDATE	546
#define	SYSDATE_FORMAT	547
#define	SYSTEM	548
#define	TABLE	549
#define	TAN	550
#define	TEMPORARY	551
#define	THEN	552
#define	THREE_DIGITS	553
#define	TIME	554
#define	TIMESTAMP	555
#define	TIMEZONE_HOUR	556
#define	TIMEZONE_MINUTE	557
#define	TINYINT	558
#define	TO	559
#define	TO_CHAR	560
#define	TO_DATE	561
#define	TRANSACTION	562
#define	TRANSLATE	563
#define	TRANSLATION	564
#define	TRUNCATE	565
#define	GENERAL_TITLE	566
#define	TWO_DIGITS	567
#define	UCASE	568
#define	UNION	569
#define	UNIQUE	570
#define	SQL_UNKNOWN	571
#define	UPDATE	572
#define	UPPER	573
#define	USAGE	574
#define	USER	575
#define	IDENTIFIER	576
#define	IDENTIFIER_DOT_ASTERISK	577
#define	ERROR_DIGIT_BEFORE_IDENTIFIER	578
#define	USING	579
#define	VALUE	580
#define	VALUES	581
#define	VARBINARY	582
#define	VARCHAR	583
#define	VARYING	584
#define	VENDOR	585
#define	VIEW	586
#define	WEEK	587
#define	WHEN	588
#define	WHENEVER	589
#define	WHERE	590
#define	WHERE_CURRENT_OF	591
#define	WITH	592
#define	WORD_WRAPPED	593
#define	WORK	594
#define	WRAPPED	595
#define	XOR	596
#define	YEAR	597
#define	YEARS_BETWEEN	598
#define	__LAST_TOKEN	599
#define	ILIKE	600

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
			
		ParserError err(i18n("Syntax Error"), i18n("Syntax Error near \"%1\"").arg(ctoken)+lexerErr, ctoken, current);
		parser->setError(err);
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

#line 764 "sqlparser.y"
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
#define	YYNTBASE	368

#define YYTRANSLATE(x) ((unsigned)(x) <= 600 ? yytranslate[x] : 398)

static const short yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,   354,   349,   365,   358,   355,
   356,   348,   347,   352,   346,   353,   359,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,   351,     2,
   360,     2,   357,   350,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
   363,     2,   364,   362,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,   366,     2,   367,     2,     2,     2,     2,
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
   337,   338,   339,   340,   341,   342,   343,   344,   345,   361
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,     8,    11,    13,    15,    16,    24,    28,
    30,    33,    37,    40,    42,    45,    48,    50,    52,    57,
    62,    63,    66,    70,    73,    77,    82,    84,    87,    89,
    93,    97,   101,   103,   107,   111,   113,   117,   121,   125,
   129,   131,   135,   139,   143,   145,   149,   153,   157,   161,
   163,   167,   171,   175,   180,   186,   188,   192,   197,   199,
   202,   205,   208,   211,   213,   218,   222,   224,   226,   228,
   230,   234,   238,   239,   242,   246,   248,   250,   253,   257,
   261,   263,   265,   267,   271,   274,   276,   281,   283
};

static const short yyrhs[] = {   369,
     0,   370,   351,   369,     0,   370,     0,   370,   351,     0,
   371,     0,   378,     0,     0,    74,   295,   322,   372,   355,
   373,   356,     0,   373,   352,   374,     0,   374,     0,   322,
   377,     0,   322,   377,   375,     0,   375,   376,     0,   376,
     0,   239,   170,     0,   207,   210,     0,    30,     0,     3,
     0,     3,   355,   274,   356,     0,   329,   355,   274,   356,
     0,     0,   379,   394,     0,   379,   394,   391,     0,   379,
   391,     0,   379,   394,   380,     0,   379,   394,   391,   380,
     0,   265,     0,   336,   381,     0,   382,     0,   383,    19,
   382,     0,   383,   224,   382,     0,   383,   342,   382,     0,
   383,     0,   384,    39,   383,     0,   384,    40,   383,     0,
   384,     0,   385,   347,   384,     0,   385,   346,   384,     0,
   385,   365,   384,     0,   385,   366,   384,     0,   385,     0,
   386,   359,   385,     0,   386,   348,   385,     0,   386,   349,
   385,     0,   386,     0,   387,   145,   386,     0,   387,   144,
   386,     0,   387,   177,   386,     0,   387,   176,   386,     0,
   387,     0,   388,   208,   387,     0,   388,   179,   387,     0,
   388,   153,   387,     0,   388,   271,   305,   387,     0,   388,
   207,   271,   305,   387,     0,   388,     0,   388,   211,   210,
     0,   388,   211,   207,   210,     0,   389,     0,   346,   389,
     0,   347,   389,     0,   367,   389,     0,   207,   389,     0,
   322,     0,   322,   355,   390,   356,     0,   322,   353,   322,
     0,   210,     0,    52,     0,   274,     0,   275,     0,   355,
   381,   356,     0,   390,   352,   381,     0,     0,   137,   392,
     0,   392,   352,   393,     0,   393,     0,   322,     0,   322,
   322,     0,   322,    22,   322,     0,   394,   352,   395,     0,
   395,     0,   396,     0,   397,     0,   396,    22,   322,     0,
   396,   322,     0,   381,     0,   107,   355,   396,   356,     0,
   348,     0,   322,   353,   348,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   826,   836,   841,   842,   851,   856,   862,   868,   871,   872,
   877,   887,   901,   902,   907,   913,   918,   925,   931,   938,
   944,   952,   973,  1145,  1149,  1153,  1159,  1169,  1180,  1185,
  1191,  1195,  1199,  1204,  1209,  1213,  1218,  1224,  1228,  1232,
  1236,  1241,  1246,  1250,  1254,  1259,  1264,  1268,  1272,  1276,
  1281,  1286,  1290,  1294,  1298,  1302,  1307,  1312,  1316,  1321,
  1327,  1331,  1335,  1339,  1352,  1358,  1369,  1376,  1384,  1392,
  1397,  1404,  1410,  1416,  1457,  1463,  1470,  1506,  1515,  1528,
  1535,  1543,  1553,  1558,  1570,  1584,  1623,  1631,  1641
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","SQL_TYPE",
"SQL_ABS","ACOS","AMPERSAND","SQL_ABSOLUTE","ADA","ADD","ADD_DAYS","ADD_HOURS",
"ADD_MINUTES","ADD_MONTHS","ADD_SECONDS","ADD_YEARS","ALL","ALLOCATE","ALTER",
"AND","ANY","ARE","AS","ASIN","ASC","ASCII","ASSERTION","ATAN","ATAN2","AUTHORIZATION",
"AUTO_INCREMENT","AVG","BEFORE","SQL_BEGIN","BETWEEN","BIGINT","BINARY","BIT",
"BIT_LENGTH","BITWISE_SHIFT_LEFT","BITWISE_SHIFT_RIGHT","BREAK","BY","CASCADE",
"CASCADED","CASE","CAST","CATALOG","CEILING","CENTER","SQL_CHAR","CHAR_LENGTH",
"CHARACTER_STRING_LITERAL","CHECK","CLOSE","COALESCE","COBOL","COLLATE","COLLATION",
"COLUMN","COMMIT","COMPUTE","CONCAT","CONCATENATION","CONNECT","CONNECTION",
"CONSTRAINT","CONSTRAINTS","CONTINUE","CONVERT","CORRESPONDING","COS","COT",
"COUNT","CREATE","CURDATE","CURRENT","CURRENT_DATE","CURRENT_TIME","CURRENT_TIMESTAMP",
"CURTIME","CURSOR","DATABASE","SQL_DATE","DATE_FORMAT","DATE_REMAINDER","DATE_VALUE",
"DAY","DAYOFMONTH","DAYOFWEEK","DAYOFYEAR","DAYS_BETWEEN","DEALLOCATE","DEC",
"DECLARE","DEFAULT","DEFERRABLE","DEFERRED","SQL_DELETE","DESC","DESCRIBE","DESCRIPTOR",
"DIAGNOSTICS","DICTIONARY","DIRECTORY","DISCONNECT","DISPLACEMENT","DISTINCT",
"DOMAIN_TOKEN","SQL_DOUBLE","DOUBLE_QUOTED_STRING","DROP","ELSE","END","END_EXEC",
"EQUAL","ESCAPE","EXCEPT","SQL_EXCEPTION","EXEC","EXECUTE","EXISTS","EXP","EXPONENT",
"EXTERNAL","EXTRACT","SQL_FALSE","FETCH","FIRST","SQL_FLOAT","FLOOR","FN","FOR",
"FOREIGN","FORTRAN","FOUND","FOUR_DIGITS","FROM","FULL","GET","GLOBAL","GO",
"GOTO","GRANT","GREATER_OR_EQUAL","GREATER_THAN","HAVING","HOUR","HOURS_BETWEEN",
"IDENTITY","IFNULL","SQL_IGNORE","IMMEDIATE","SQL_IN","INCLUDE","INDEX","INDICATOR",
"INITIALLY","INNER","INPUT","INSENSITIVE","INSERT","INTEGER","INTERSECT","INTERVAL",
"INTO","IS","ISOLATION","JOIN","JUSTIFY","KEY","LANGUAGE","LAST","LCASE","LEFT",
"LENGTH","LESS_OR_EQUAL","LESS_THAN","LEVEL","LIKE","LINE_WIDTH","LOCAL","LOCATE",
"LOG","SQL_LONG","LOWER","LTRIM","LTRIP","MATCH","SQL_MAX","MICROSOFT","SQL_MIN",
"MINUS","MINUTE","MINUTES_BETWEEN","MOD","MODIFY","MODULE","MONTH","MONTHS_BETWEEN",
"MUMPS","NAMES","NATIONAL","NCHAR","NEXT","NODUP","NONE","NOT","NOT_EQUAL","NOW",
"SQL_NULL","SQL_IS","SQL_IS_NULL","SQL_IS_NOT_NULL","NULLIF","NUMERIC","OCTET_LENGTH",
"ODBC","OF","SQL_OFF","SQL_ON","ONLY","OPEN","OPTION","OR","ORDER","OUTER","OUTPUT",
"OVERLAPS","PAGE","PARTIAL","SQL_PASCAL","PERSISTENT","CQL_PI","PLI","POSITION",
"PRECISION","PREPARE","PRESERVE","PRIMARY","PRIOR","PRIVILEGES","PROCEDURE",
"PRODUCT","PUBLIC","QUARTER","QUIT","RAND","READ_ONLY","REAL","REFERENCES","REPEAT",
"REPLACE","RESTRICT","REVOKE","RIGHT","ROLLBACK","ROWS","RPAD","RTRIM","SCHEMA",
"SCREEN_WIDTH","SCROLL","SECOND","SECONDS_BETWEEN","SELECT","SEQUENCE","SETOPT",
"SET","SHOWOPT","SIGN","SIMILAR","SIMILAR_TO","NOT_SIMILAR_TO","INTEGER_CONST",
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
"','","'.'","'$'","'('","')'","'?'","'\\''","'/'","'='","ILIKE","'^'","'['",
"']'","'&'","'|'","'~'","TopLevelStatement","StatementList","Statement","CreateTableStatement",
"@1","ColDefs","ColDef","ColKeys","ColKey","ColType","SelectStatement","Select",
"WhereClause","aExpr","aExpr2","aExpr3","aExpr4","aExpr5","aExpr6","aExpr7",
"aExpr8","aExpr9","aExprList","Tables","FlatTableList","FlatTable","ColViews",
"ColItem","ColExpression","ColWildCard", NULL
};
#endif

static const short yyr1[] = {     0,
   368,   369,   369,   369,   370,   370,   372,   371,   373,   373,
   374,   374,   375,   375,   376,   376,   376,   377,   377,   377,
   377,   378,   378,   378,   378,   378,   379,   380,   381,   382,
   382,   382,   382,   383,   383,   383,   384,   384,   384,   384,
   384,   385,   385,   385,   385,   386,   386,   386,   386,   386,
   387,   387,   387,   387,   387,   387,   388,   388,   388,   389,
   389,   389,   389,   389,   389,   389,   389,   389,   389,   389,
   389,   390,   390,   391,   392,   392,   393,   393,   393,   394,
   394,   395,   395,   395,   395,   396,   396,   397,   397
};

static const short yyr2[] = {     0,
     1,     3,     1,     2,     1,     1,     0,     7,     3,     1,
     2,     3,     2,     1,     2,     2,     1,     1,     4,     4,
     0,     2,     3,     2,     3,     4,     1,     2,     1,     3,
     3,     3,     1,     3,     3,     1,     3,     3,     3,     3,
     1,     3,     3,     3,     1,     3,     3,     3,     3,     1,
     3,     3,     3,     4,     5,     1,     3,     4,     1,     2,
     2,     2,     2,     1,     4,     3,     1,     1,     1,     1,
     3,     3,     0,     2,     3,     1,     1,     2,     3,     3,
     1,     1,     1,     3,     2,     1,     4,     1,     3
};

static const short yydefact[] = {     0,
     0,    27,     1,     3,     5,     6,     0,     0,     4,    68,
     0,     0,     0,    67,    69,    70,    64,     0,     0,    88,
     0,     0,    86,    29,    33,    36,    41,    45,    50,    56,
    59,    24,    22,    81,    82,    83,     7,     2,     0,    77,
    74,    76,    64,    63,     0,    73,    60,    61,     0,    62,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    25,    23,     0,    85,     0,     0,
     0,    78,     0,     0,    66,    89,     0,    71,    30,    31,
    32,    34,    35,    38,    37,    39,    40,    43,    44,    42,
    47,    46,    49,    48,    53,    52,     0,    51,     0,    57,
     0,    28,    80,    26,    84,     0,    87,    79,    75,     0,
    65,     0,    58,    54,    21,     0,    10,    72,    55,    18,
     0,    11,     0,     8,     0,     0,    17,     0,     0,    12,
    14,     9,     0,     0,    16,    15,    13,    19,    20,     0,
     0,     0
};

static const short yydefgoto[] = {   150,
     3,     4,     5,    79,   126,   127,   140,   141,   132,     6,
     7,    75,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    87,    32,    41,    42,    33,    34,    35,    36
};

static const short yypact[] = {   -67,
  -282,-32768,-32768,  -306,-32768,-32768,   -50,  -296,   -67,-32768,
  -317,  -269,   -36,-32768,-32768,-32768,  -323,   -36,   -36,-32768,
   -36,   -36,-32768,-32768,   -18,    20,  -326,  -325,  -126,  -143,
-32768,-32768,  -133,-32768,   -19,-32768,-32768,-32768,   -40,   -14,
  -290,-32768,  -299,-32768,  -313,-32768,-32768,-32768,  -278,-32768,
   -36,   -36,   -36,   -36,   -36,   -36,   -36,   -36,   -36,   -36,
   -36,   -36,   -36,   -36,   -36,   -36,   -36,   -36,  -205,   -36,
  -155,  -226,   -36,   -46,-32768,  -256,  -241,-32768,  -273,  -272,
  -239,-32768,  -269,  -237,-32768,-32768,  -327,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,  -219,-32768,  -122,-32768,
   -36,-32768,-32768,-32768,-32768,  -233,-32768,-32768,-32768,   -36,
-32768,   -36,-32768,-32768,    -3,  -319,-32768,-32768,-32768,  -265,
  -264,   -25,  -233,-32768,  -182,  -181,-32768,  -116,   -75,   -25,
-32768,-32768,  -260,  -259,-32768,-32768,-32768,-32768,-32768,    98,
    99,-32768
};

static const short yypgoto[] = {-32768,
    91,-32768,-32768,-32768,-32768,   -32,-32768,   -38,-32768,-32768,
-32768,    27,   -10,    19,    22,   -15,    13,   -17,   -53,-32768,
     9,-32768,    71,-32768,    23,-32768,    31,    68,-32768
};


#define	YYLAST		331


static const short yytable[] = {   130,
    51,    10,    77,    12,   137,    10,     1,    81,    85,    67,
    49,    10,     8,   105,   106,    10,   108,    63,    64,    56,
    57,    44,    60,    61,   120,    37,    47,    48,   121,    45,
    50,    46,   133,    62,    86,    68,   134,    39,    58,    59,
    94,    95,    96,    97,     9,   101,   102,   103,   104,    65,
    66,   109,    40,    84,   110,    46,    11,   124,    54,    55,
    11,    83,   112,    69,    70,   107,    11,    71,   129,    89,
    90,    91,    98,    99,   100,    92,    93,    88,   111,    73,
   115,   116,   118,   117,    85,   122,    12,   123,   125,   135,
   136,   143,   144,   145,   146,   148,   149,   151,   152,    38,
   142,   147,   114,    76,   113,   119,    80,     0,     0,   128,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    72,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    13,     0,     0,    14,
    13,     0,     0,    14,     0,     0,    13,     0,     0,    14,
    13,     0,     0,    14,     0,     0,     0,     0,     0,     0,
     0,   138,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     2,     0,     0,
     0,     0,    73,     0,     0,    52,     0,     0,     0,     0,
     0,     0,     0,   139,     0,     0,     0,     0,    74,     0,
     0,     0,     0,    15,    16,     0,     0,    15,    16,     0,
     0,     0,     0,    15,    16,     0,     0,    15,    16,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    17,     0,     0,     0,    17,     0,     0,     0,     0,
     0,    43,     0,     0,     0,    43,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    18,    19,    20,     0,    18,
    19,    20,    78,     0,    21,    18,    19,    82,    21,    18,
    19,     0,     0,     0,    21,     0,    22,     0,    21,     0,
    22,     0,     0,    53,     0,   131,    22,     0,     0,     0,
    22
};

static const short yycheck[] = {     3,
    19,    52,    22,   137,    30,    52,    74,    22,   322,   153,
    21,    52,   295,    67,    68,    52,    70,   144,   145,   346,
   347,    13,   348,   349,   352,   322,    18,    19,   356,   353,
    22,   355,   352,   359,   348,   179,   356,   355,   365,   366,
    56,    57,    58,    59,   351,    63,    64,    65,    66,   176,
   177,   207,   322,   353,   210,   355,   107,   111,    39,    40,
   107,   352,    73,   207,   208,   271,   107,   211,   122,    51,
    52,    53,    60,    61,    62,    54,    55,   356,   305,   336,
   322,   355,   322,   356,   322,   305,   137,   210,   322,   355,
   355,   274,   274,   210,   170,   356,   356,     0,     0,     9,
   133,   140,    76,    33,    74,    83,    39,    -1,    -1,   120,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   271,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   207,    -1,    -1,   210,
   207,    -1,    -1,   210,    -1,    -1,   207,    -1,    -1,   210,
   207,    -1,    -1,   210,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   207,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   265,    -1,    -1,
    -1,    -1,   336,    -1,    -1,   224,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   239,    -1,    -1,    -1,    -1,   352,    -1,
    -1,    -1,    -1,   274,   275,    -1,    -1,   274,   275,    -1,
    -1,    -1,    -1,   274,   275,    -1,    -1,   274,   275,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   322,    -1,    -1,    -1,   322,    -1,    -1,    -1,    -1,
    -1,   322,    -1,    -1,    -1,   322,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   346,   347,   348,    -1,   346,
   347,   348,   322,    -1,   355,   346,   347,   322,   355,   346,
   347,    -1,    -1,    -1,   355,    -1,   367,    -1,   355,    -1,
   367,    -1,    -1,   342,    -1,   329,   367,    -1,    -1,    -1,
   367
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
#line 828 "sqlparser.y"
{
//todo: multiple statements
//todo: not only "select" statements
	parser->setOperation(Parser::OP_Select);
	parser->setQuerySchema(yyvsp[0].querySchema);
;
    break;}
case 2:
#line 838 "sqlparser.y"
{
//todo: multiple statements
;
    break;}
case 4:
#line 843 "sqlparser.y"
{
	yyval.querySchema = yyvsp[-1].querySchema;
;
    break;}
case 5:
#line 853 "sqlparser.y"
{
YYACCEPT;
;
    break;}
case 6:
#line 857 "sqlparser.y"
{
	yyval.querySchema = yyvsp[0].querySchema;
;
    break;}
case 7:
#line 864 "sqlparser.y"
{
	parser->setOperation(Parser::OP_CreateTable);
	parser->createTable(yyvsp[0].stringValue);
;
    break;}
case 10:
#line 873 "sqlparser.y"
{
;
    break;}
case 11:
#line 879 "sqlparser.y"
{
	kdDebug() << "adding field " << yyvsp[-1].stringValue << endl;
	field->setName(yyvsp[-1].stringValue);
	parser->table()->addField(field);

//	delete field;
	field = 0;
;
    break;}
case 12:
#line 888 "sqlparser.y"
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
#line 903 "sqlparser.y"
{
;
    break;}
case 15:
#line 909 "sqlparser.y"
{
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
;
    break;}
case 16:
#line 914 "sqlparser.y"
{
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
;
    break;}
case 17:
#line 919 "sqlparser.y"
{
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
;
    break;}
case 18:
#line 927 "sqlparser.y"
{
	field = new Field();
	field->setType(yyvsp[0].colType);
;
    break;}
case 19:
#line 932 "sqlparser.y"
{
	kdDebug() << "sql + length" << endl;
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(yyvsp[-3].colType);
;
    break;}
case 20:
#line 939 "sqlparser.y"
{
	field = new Field();
	field->setPrecision(yyvsp[-1].integerValue);
	field->setType(Field::Text);
;
    break;}
case 21:
#line 945 "sqlparser.y"
{
	// SQLITE compatibillity
	field = new Field();
	field->setType(Field::InvalidType);
;
    break;}
case 22:
#line 954 "sqlparser.y"
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
#line 974 "sqlparser.y"
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
;
    break;}
case 24:
#line 1146 "sqlparser.y"
{
	kdDebug() << "Select ColViews Tables" << endl;
;
    break;}
case 25:
#line 1150 "sqlparser.y"
{
	kdDebug() << "Select ColViews Conditions" << endl;
;
    break;}
case 26:
#line 1154 "sqlparser.y"
{
	kdDebug() << "Select ColViews Tables Conditions" << endl;
;
    break;}
case 27:
#line 1161 "sqlparser.y"
{
	kdDebug() << "SELECT" << endl;
//	parser->createSelect();
//	parser->setOperation(Parser::OP_Select);
	yyval.querySchema = new QuerySchema();
;
    break;}
case 28:
#line 1171 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
;
    break;}
case 30:
#line 1187 "sqlparser.y"
{
//	kdDebug() << "AND " << $3.debugString() << endl;
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, AND, yyvsp[0].expr );
;
    break;}
case 31:
#line 1192 "sqlparser.y"
{
	yyval.expr = new BinaryExpr( KexiDBExpr_Logical, yyvsp[-2].expr, OR, yyvsp[0].expr );
;
    break;}
case 32:
#line 1196 "sqlparser.y"
{
	yyval.expr = new BinaryExpr( KexiDBExpr_Arithm, yyvsp[-2].expr, XOR, yyvsp[0].expr );
;
    break;}
case 34:
#line 1206 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_LEFT, yyvsp[0].expr);
;
    break;}
case 35:
#line 1210 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, BITWISE_SHIFT_RIGHT, yyvsp[0].expr);
;
    break;}
case 37:
#line 1220 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '+', yyvsp[0].expr);
	yyval.expr->debug();
;
    break;}
case 38:
#line 1225 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '-', yyvsp[0].expr);
;
    break;}
case 39:
#line 1229 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '&', yyvsp[0].expr);
;
    break;}
case 40:
#line 1233 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '|', yyvsp[0].expr);
;
    break;}
case 42:
#line 1243 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '/', yyvsp[0].expr);
;
    break;}
case 43:
#line 1247 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '*', yyvsp[0].expr);
;
    break;}
case 44:
#line 1251 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Arithm, yyvsp[-2].expr, '%', yyvsp[0].expr);
;
    break;}
case 46:
#line 1261 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_THAN, yyvsp[0].expr);
;
    break;}
case 47:
#line 1265 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, GREATER_OR_EQUAL, yyvsp[0].expr);
;
    break;}
case 48:
#line 1269 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_THAN, yyvsp[0].expr);
;
    break;}
case 49:
#line 1273 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LESS_OR_EQUAL, yyvsp[0].expr);
;
    break;}
case 51:
#line 1283 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, NOT_EQUAL, yyvsp[0].expr);
;
    break;}
case 52:
#line 1287 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, LIKE, yyvsp[0].expr);
;
    break;}
case 53:
#line 1291 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-2].expr, SQL_IN, yyvsp[0].expr);
;
    break;}
case 54:
#line 1295 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-3].expr, SIMILAR_TO, yyvsp[0].expr);
;
    break;}
case 55:
#line 1299 "sqlparser.y"
{
	yyval.expr = new BinaryExpr(KexiDBExpr_Relational, yyvsp[-4].expr, NOT_SIMILAR_TO, yyvsp[0].expr);
;
    break;}
case 57:
#line 1309 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( SQL_IS_NULL, yyvsp[-2].expr );
;
    break;}
case 58:
#line 1313 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( SQL_IS_NOT_NULL, yyvsp[-3].expr );
;
    break;}
case 60:
#line 1324 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( '-', yyvsp[0].expr );
;
    break;}
case 61:
#line 1328 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( '+', yyvsp[0].expr );
;
    break;}
case 62:
#line 1332 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( '~', yyvsp[0].expr );
;
    break;}
case 63:
#line 1336 "sqlparser.y"
{
	yyval.expr = new UnaryExpr( NOT, yyvsp[0].expr );
;
    break;}
case 64:
#line 1340 "sqlparser.y"
{
	yyval.expr = new VariableExpr( QString::fromLatin1(yyvsp[0].stringValue) );
	
//TODO: simplify this later if that's 'only one field name' expression
	kdDebug() << "  + identifier: " << yyvsp[0].stringValue << endl;
//	$$ = new Field();
//	$$->setName($1);
//	$$->setTable(dummy);

//	parser->select()->addField(field);
	requiresTable = true;
;
    break;}
case 65:
#line 1353 "sqlparser.y"
{
	kdDebug() << "  + function: " << yyvsp[-3].stringValue << "(" << yyvsp[-1].exprList->debugString() << ")" << endl;
	yyval.expr = new FunctionExpr(yyvsp[-3].stringValue, yyvsp[-1].exprList);
;
    break;}
case 66:
#line 1359 "sqlparser.y"
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
case 67:
#line 1370 "sqlparser.y"
{
	yyval.expr = new ConstExpr( SQL_NULL, QVariant() );
	kdDebug() << "  + NULL" << endl;
//	$$ = new Field();
	//$$->setName(QString::null);
;
    break;}
case 68:
#line 1377 "sqlparser.y"
{
	yyval.expr = new ConstExpr( CHARACTER_STRING_LITERAL, yyvsp[0].stringValue );
//	$$ = new Field();
//	$$->setName($1);
//	parser->select()->addField(field);
	kdDebug() << "  + constant \"" << yyvsp[0].stringValue << "\"" << endl;
;
    break;}
case 69:
#line 1385 "sqlparser.y"
{
	yyval.expr = new ConstExpr( INTEGER_CONST, yyvsp[0].integerValue );
//	$$ = new Field();
//	$$->setName(QString::number($1));
//	parser->select()->addField(field);
	kdDebug() << "  + int constant: " << yyvsp[0].integerValue << endl;
;
    break;}
case 70:
#line 1393 "sqlparser.y"
{
	yyval.expr = new ConstExpr( REAL_CONST, QPoint( yyvsp[0].realValue.integer, yyvsp[0].realValue.fractional ) );
	kdDebug() << "  + real constant: " << yyvsp[0].realValue.integer << "." << yyvsp[0].realValue.fractional << endl;
;
    break;}
case 71:
#line 1398 "sqlparser.y"
{
	kdDebug() << "(expr)" << endl;
	yyval.expr = new UnaryExpr('(', yyvsp[-1].expr);
;
    break;}
case 72:
#line 1406 "sqlparser.y"
{
	yyvsp[-2].exprList->add( yyvsp[0].expr );
	yyval.exprList = yyvsp[-2].exprList;
;
    break;}
case 73:
#line 1411 "sqlparser.y"
{
	yyval.exprList = new NArgExpr(0, 0/*unknown*/);
;
    break;}
case 74:
#line 1418 "sqlparser.y"
{
	yyval.exprList = yyvsp[0].exprList;
;
    break;}
case 75:
#line 1459 "sqlparser.y"
{
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add(yyvsp[0].expr);
;
    break;}
case 76:
#line 1464 "sqlparser.y"
{
	yyval.exprList = new NArgExpr(KexiDBExpr_TableList, IDENTIFIER); //ok?
	yyval.exprList->add(yyvsp[0].expr);
;
    break;}
case 77:
#line 1472 "sqlparser.y"
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
case 78:
#line 1507 "sqlparser.y"
{
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary, 
		new VariableExpr(QString::fromLatin1(yyvsp[-1].stringValue)), 0,
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
;
    break;}
case 79:
#line 1516 "sqlparser.y"
{
	//table + alias
	yyval.expr = new BinaryExpr(
		KexiDBExpr_SpecialBinary,
		new VariableExpr(QString::fromLatin1(yyvsp[-2].stringValue)), AS,
		new VariableExpr(QString::fromLatin1(yyvsp[0].stringValue))
	);
;
    break;}
case 80:
#line 1530 "sqlparser.y"
{
	yyval.exprList = yyvsp[-2].exprList;
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColViews , ColItem" << endl;
;
    break;}
case 81:
#line 1536 "sqlparser.y"
{
	yyval.exprList = new NArgExpr(0,0);
	yyval.exprList->add( yyvsp[0].expr );
	kdDebug() << "ColViews: ColItem" << endl;
;
    break;}
case 82:
#line 1545 "sqlparser.y"
{
//	$$ = new Field();
//	dummy->addField($$);
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column expr: '" << yyvsp[0].expr->debugString() << "'" << endl;
;
    break;}
case 83:
#line 1554 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
	kdDebug() << " added column wildcard: '" << yyvsp[0].expr->debugString() << "'" << endl;
;
    break;}
case 84:
#line 1559 "sqlparser.y"
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
case 85:
#line 1571 "sqlparser.y"
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
case 86:
#line 1586 "sqlparser.y"
{
	yyval.expr = yyvsp[0].expr;
;
    break;}
case 87:
#line 1624 "sqlparser.y"
{
	yyval.expr = yyvsp[-1].expr;
//TODO
//	$$->setName("DISTINCT(" + $3->name() + ")");
;
    break;}
case 88:
#line 1633 "sqlparser.y"
{
	yyval.expr = new VariableExpr("*");
	kdDebug() << "all columns" << endl;

//	QueryAsterisk *ast = new QueryAsterisk(parser->select(), dummy);
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
;
    break;}
case 89:
#line 1642 "sqlparser.y"
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
#line 1656 "sqlparser.y"


const char * const tname(int type) { return (type>=254 && type<=__LAST_TOKEN) ? yytname[type-254] : ""; }
