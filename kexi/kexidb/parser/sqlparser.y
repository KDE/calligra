%token PERCENT
%token SQL_TYPE
%token SQL_ABS
%token ACOS
%token AMPERSAND
%token SQL_ABSOLUTE
%token ACOS
%token ADA
%token ADD
%token ADD_DAYS
%token ADD_HOURS
%token ADD_MINUTES
%token ADD_MONTHS
%token ADD_SECONDS
%token ADD_YEARS
%token ALL
%token ALLOCATE
%token ALTER
%token AND
%token ANY
%token ARE
%token ARITHMETIC_MINUS
%token ARITHMETIC_PLUS
%token AS
%token ASIN
%token ASC
%token ASCII
%token ASSERTION
%token ASTERISK
%token AT
%token ATAN
%token ATAN2
%token AUTHORIZATION
%token AUTO_INCREMENT
%token AVG
%token BEFORE
%token SQL_BEGIN
%token BETWEEN
%token BIGINT
%token BINARY
%token BIT
%token BIT_LENGTH
%token BREAK
%token BY
%token CASCADE
%token CASCADED
%token CASE
%token CAST
%token CATALOG
%token CEILING
%token CENTER
%token SQL_CHAR
%token CHAR_LENGTH
%token CHARACTER_STRING_LITERAL
%token CHECK
%token CLOSE
%token COALESCE
%token COBOL
%token COLLATE
%token COLLATION
%token COLON
%token COLUMN
%token COMMA
%token COMMIT
%token COMPUTE
%token CONCAT
%token CONNECT
%token CONNECTION
%token CONSTRAINT
%token CONSTRAINTS
%token CONTINUE
%token CONVERT
%token CORRESPONDING
%token COS
%token COT
%token COUNT
%token CREATE
%token CURDATE
%token CURRENT
%token CURRENT_DATE
%token CURRENT_TIME
%token CURRENT_TIMESTAMP
%token CURTIME
%token CURSOR
%token DATABASE
%token SQL_DATE
%token DATE_FORMAT
%token DATE_REMAINDER
%token DATE_VALUE
%token DAY
%token DAYOFMONTH
%token DAYOFWEEK
%token DAYOFYEAR
%token DAYS_BETWEEN
%token DEALLOCATE
%token DEC
%token DECLARE
%token DEFAULT
%token DEFERRABLE
%token DEFERRED
%token SQL_DELETE
%token DESC
%token DESCRIBE
%token DESCRIPTOR
%token DIAGNOSTICS
%token DICTIONARY
%token DIRECTORY
%token DISCONNECT
%token DISPLACEMENT
%token DISTINCT
%token DOLLAR_SIGN
%token DOMAIN_TOKEN
%token DOT
%token SQL_DOUBLE
%token DOUBLE_QUOTED_STRING
%token DROP
%token ELSE
%token END
%token END_EXEC
%token EQUAL
%token ESCAPE
%token EXCEPT
%token SQL_EXCEPTION
%token EXEC
%token EXECUTE
%token EXISTS
%token EXP
%token EXPONENT
%token EXTERNAL
%token EXTRACT
%token SQL_FALSE
%token FETCH
%token FIRST
%token SQL_FLOAT
%token FLOOR
%token FN
%token FOR
%token FOREIGN
%token FORTRAN
%token FOUND
%token FOUR_DIGITS
%token FROM
%token FULL
%token GET
%token GLOBAL
%token GO
%token GOTO
%token GRANT
%token GREATER_THAN
%token GREATER_OR_EQUAL
%token GROUP
%token HAVING
%token HOUR
%token HOURS_BETWEEN
%token IDENTITY
%token IFNULL
%token SQL_IGNORE
%token IMMEDIATE
%token SQL_IN
%token INCLUDE
%token INDEX
%token INDICATOR
%token INITIALLY
%token INNER
%token INPUT
%token INSENSITIVE
%token INSERT
%token INTEGER
%token INTERSECT
%token INTERVAL
%token INTO
%token IS
%token ISOLATION
%token JOIN
%token JUSTIFY
%token KEY
%token LANGUAGE
%token LAST
%token LCASE
%token LEFT
%token LEFTPAREN
%token LENGTH
%token LESS_OR_EQUAL
%token LESS_THAN
%token LEVEL
%token LIKE
%token LINE_WIDTH
%token LOCAL
%token LOCATE
%token LOG
%token SQL_LONG
%token LOWER
%token LTRIM
%token LTRIP
%token MATCH
%token SQL_MAX
%token MICROSOFT
%token SQL_MIN
%token MINUS
%token MINUTE
%token MINUTES_BETWEEN
%token MOD
%token MODIFY
%token MODULE
%token MONTH
%token MONTHS_BETWEEN
%token MUMPS
%token NAMES
%token NATIONAL
%token NCHAR
%token NEXT
%token NODUP
%token NONE
%token NOT
%token NOT_EQUAL
%token NOW
%token SQL_NULL
%token NULLIF
%token NUMERIC
%token OCTET_LENGTH
%token ODBC
%token OF
%token SQL_OFF
%token SQL_ON
%token ONLY
%token OPEN
%token OPTION
%token OR
%token ORDER
%token OUTER
%token OUTPUT
%token OVERLAPS
%token PAGE
%token PARTIAL
%token SQL_PASCAL
%token PERSISTENT
%token CQL_PI
%token PLI
%token POSITION
%token PRECISION
%token PREPARE
%token PRESERVE
%token PRIMARY
%token PRIOR
%token PRIVILEGES
%token PROCEDURE
%token PRODUCT
%token PUBLIC
%token QUARTER
%token QUESTION_MARK
%token QUIT
%token RAND
%token READ_ONLY
%token REAL
%token REFERENCES
%token REPEAT
%token REPLACE
%token RESTRICT
%token REVOKE
%token RIGHT
%token RIGHTPAREN
%token ROLLBACK
%token ROWS
%token RPAD
%token RTRIM
%token SCHEMA
%token SCREEN_WIDTH
%token SCROLL
%token SECOND
%token SECONDS_BETWEEN
%token SELECT
%token SEMICOLON
%token SEQUENCE
%token SETOPT
%token SET
%token SHOWOPT
%token SIGN
%token SIGNED_INTEGER
%token SIN
%token SINGLE_QUOTE
%token SQL_SIZE
%token SLASH
%token SMALLINT
%token SOME
%token SPACE
%token SQL
%token SQL_TRUE
%token SQLCA
%token SQLCODE
%token SQLERROR
%token SQLSTATE
%token SQLWARNING
%token SQRT
%token STDEV
%token SUBSTRING
%token SUM
%token SYSDATE
%token SYSDATE_FORMAT
%token SYSTEM
%token TABLE
%token TAN
%token TEMPORARY
%token THEN
%token THREE_DIGITS
%token TIME
%token TIMESTAMP
%token TIMEZONE_HOUR
%token TIMEZONE_MINUTE
%token TINYINT
%token TO
%token TO_CHAR
%token TO_DATE
%token TRANSACTION
%token TRANSLATE
%token TRANSLATION
%token TRUNCATE
%token GENERAL_TITLE
%token TWO_DIGITS
%token UCASE
%token UNION
%token UNIQUE
%token SQL_UNKNOWN
%token UNSIGNED_INTEGER
%token UPDATE
%token UPPER
%token USAGE
%token USER
%token USER_DEFINED_NAME
%token USING
%token VALUE
%token VALUES
%token VARBINARY
%token VARCHAR
%token VARYING
%token VENDOR
%token VIEW
%token WEEK
%token WHEN
%token WHENEVER
%token WHERE
%token WHERE_CURRENT_OF
%token WITH
%token WORD_WRAPPED
%token WORK
%token WRAPPED
%token YEAR
%token YEARS_BETWEEN

%type <stringValue> USER_DEFINED_NAME
%type <stringValue> CHARACTER_STRING_LITERAL
%type <stringValue> DOUBLE_QUOTED_STRING

%type <field> ColExpression
%type <field> ColView

%type <coltype> SQL_TYPE
%type <integerValue> UNSIGNED_INTEGER
%type <integerValue> SIGNED_INTEGER

%left EQUAL NOT_EQUAL GREATER_THAN GREATER_OR_EQUAL LESS_THAN LESS_OR_EQUAL LIKE PERCENT NOT
%left ARITHMETIC_PLUS ARITHMETIC_MINUS
%left ASTERISK SLASH

%{
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
			//take the dummy table out of the query
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
%}

%union {
	char stringValue[255];
	int integerValue;
	KexiDB::Field::Type coltype;
	KexiDB::Field *field;
}

%%

TopLevelStatement :
	Statement SEMICOLON { }
	| Statement   { }
	;

Statement :
	Statement CreateTableStatement 		{ YYACCEPT; }
	| Statement SelectStatement 		{  }
	|
	;

CreateTableStatement :
CREATE TABLE USER_DEFINED_NAME
{
	parser->setOperation(KexiDB::Parser::OP_CreateTable);
	parser->createTable($3);
}
LEFTPAREN ColDefs RIGHTPAREN
;

ColDefs:
ColDefs COMMA ColDef|ColDef
{
}
;

ColDef:
USER_DEFINED_NAME ColType
{
	kdDebug() << "adding field " << $1 << endl;
	field->setName($1);
	parser->table()->addField(field);

//	delete field;
	field = 0;
}
| USER_DEFINED_NAME ColType ColKeys
{
	kdDebug() << "adding field " << $1 << endl;
	field->setName($1);
	parser->table()->addField(field);

//	if(field->isPrimaryKey())
//		parser->table()->addPrimaryKey(field->name());

//	delete field;
//	field = 0;
}
;

ColKeys:
ColKeys ColKey|ColKey
{
}
;

ColKey:
PRIMARY KEY
{
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
}
| NOT SQL_NULL
{
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
}
| AUTO_INCREMENT
{
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
}
;

ColType:
SQL_TYPE
{
	field = new KexiDB::Field();
	field->setType($1);
}
| SQL_TYPE LEFTPAREN UNSIGNED_INTEGER RIGHTPAREN
{
	kdDebug() << "sql + length" << endl;
	field = new KexiDB::Field();
	field->setPrecision($3);
	field->setType($1);
}
| VARCHAR LEFTPAREN UNSIGNED_INTEGER RIGHTPAREN
{
	field = new KexiDB::Field();
	field->setPrecision($3);
	field->setType(KexiDB::Field::Text);
}
|
{
	// SQLITE compatibillity
	field = new KexiDB::Field();
	field->setType(KexiDB::Field::InvalidType);
}
;

SelectStatement:
Select ColViews
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
| Select ColViews Tables
{
	kdDebug() << "Select ColViews Tables" << endl;
}
| Select Tables
{
	kdDebug() << "Select ColViews Tables" << endl;
}
| Select ColViews Conditions
{
	kdDebug() << "Select ColViews Conditions" << endl;
}
| Select ColViews Tables Conditions
{
	kdDebug() << "Select ColViews Tables Conditions" << endl;
}
;

Select:
SELECT
{
	kdDebug() << "SELECT" << endl;
	parser->createSelect();
	parser->setOperation(KexiDB::Parser::OP_Select);
}
;

Conditions:
WHERE ColExpression
{
	kdDebug() << "WHERE " << $2 << endl;
}
| Conditions AND ColExpression
{
	kdDebug() << "AND " << $3 << endl;
}
| Conditions OR ColExpression
{
	kdDebug() << "OR " << $3 << endl;
}
| LEFTPAREN Conditions RIGHTPAREN
{
	kdDebug() << "()" << endl;
}
;

Tables:
FROM FlatTableList
{
}
| Tables LEFT JOIN USER_DEFINED_NAME SQL_ON ColExpression
{
	kdDebug() << "LEFT JOIN: '" << $4 << "' ON " << $6 << endl;
	addTable($4);
}
| Tables LEFT OUTER JOIN USER_DEFINED_NAME SQL_ON ColExpression
{
	kdDebug() << "LEFT OUTER JOIN: '" << $5 << "' ON " << $7 << endl;
	addTable($5);
}
| Tables INNER JOIN USER_DEFINED_NAME SQL_ON ColExpression
{
	kdDebug() << "INNER JOIN: '" << $4 << "' ON " << $6 << endl;
	addTable($4);
}
| Tables RIGHT JOIN USER_DEFINED_NAME SQL_ON ColExpression
{
	kdDebug() << "RIGHT JOIN: '" << $4 << "' ON " << $6 << endl;
	addTable($4);
}
| Tables RIGHT OUTER JOIN USER_DEFINED_NAME SQL_ON ColExpression
{
	kdDebug() << "RIGHT OUTER JOIN: '" << $5 << "' ON " << $7 << endl;
	addTable($5);
}
;

FlatTableList:
FlatTableList COMMA FlatTable|FlatTable
{
}
;

FlatTable:
USER_DEFINED_NAME
{
	kdDebug() << "FROM: '" << $1 << "'" << endl;

	KexiDB::TableSchema *schema = parser->db()->tableSchema($1);
	parser->select()->setParentTable(schema);
	parser->select()->addTable(schema);
	requiresTable = false;
	addTable($1);

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
;



ColViews:
ColViews COMMA ColItem|ColItem
{
}
;

ColItem:
ColExpression
{
	kdDebug() << " adding field '" << $1->name() << "'" << endl;
	parser->select()->addField($1);
//	parser->fieldList()->append($1);
}
| ColWildCard
{
}
| ColExpression AS USER_DEFINED_NAME
{
	kdDebug() << " adding field '" << $1->name() << "' as '" << $3 << "'" << endl;
//	parser->fieldList()->append($1);
	parser->select()->addField($1);
	parser->select()->setAlias($1, $3);
}
| ColExpression USER_DEFINED_NAME
{
	kdDebug() << " adding field '" << $1->name() << "' as '" << $2 << "'" << endl;
//	parser->fieldList()->append($1);
	parser->select()->addField($1);
	parser->select()->setAlias($1, $2);
}
;

ColView:
USER_DEFINED_NAME
{
	kdDebug() << "  + col " << $1 << endl;
	$$ = new KexiDB::Field();
	$$->setName($1);
	$$->setTable(dummy);
//	parser->select()->addField(field);
	requiresTable = true;
}
| USER_DEFINED_NAME DOT USER_DEFINED_NAME
{
	kdDebug() << "  + col " << $3 << " from " << $1 << endl;
	$$ = new KexiDB::Field();
//	s->setTable($1);
	$$->setName($3);
	$$->setTable(parser->db()->tableSchema($1));
//	parser->select()->addField(field);
	requiresTable = true;
}
| SQL_NULL
{
	$$ = new KexiDB::Field();
	$$->setName(QString::null);
}
| CHARACTER_STRING_LITERAL
{
	$$ = new KexiDB::Field();
	$$->setName($1);
//	parser->select()->addField(field);
	kdDebug() << "  + constant " << $1 << endl;
}
| SIGNED_INTEGER
{
	$$ = new KexiDB::Field();
	$$->setName(QString::number($1));
//	parser->select()->addField(field);
	kdDebug() << "  + numerical constant " << $1 << endl;
}
| UNSIGNED_INTEGER
{
	$$ = new KexiDB::Field();
	$$->setName(QString::number($1));
//	parser->select()->addField(field);
	kdDebug() << "  + numerical constant " << $1 << endl;
}
;

ColExpression:
ColView
{
	$$ = $1;
	kdDebug() << "to expression: " << $$->name() << endl;
}
| ColExpression ARITHMETIC_PLUS ColExpression
{
	kdDebug() << $1->name() << " + " << $3->name() << endl;
	$$->setName($1->name() + " + " + $3->name());
}
| ColExpression ARITHMETIC_MINUS ColExpression
{
	kdDebug() << $1->name() << " - " << $3->name() << endl;
	$$->setName($1->name() + " - " + $3->name());
}
| ColExpression SLASH ColExpression
{
	kdDebug() << $1->name() << " / " << $3->name() << endl;
	$$->setName($1->name() + " / " + $3->name());
}
| ColExpression ASTERISK ColExpression
{
	kdDebug() << $1->name() << " * " << $3->name() << endl;
	$$->setName($1->name() + " * " + $3->name());
}
| ColExpression NOT ColExpression
{
	kdDebug() << $1->name() << " NOT " << $3->name() << endl;
	$$->setName($1->name() + " NOT " + $3->name());
}
| ColExpression EQUAL ColExpression
{
	kdDebug() << $1->name() << " = " << $3->name() << endl;
	$$->setName($1->name() + " = " + $3->name());
}
| ColExpression NOT_EQUAL ColExpression
{
	kdDebug() << $1->name() << " <> " << $3->name() << endl;
	$$->setName($1->name() + " <> " + $3->name());
}
| ColExpression GREATER_THAN ColExpression
{
	kdDebug() << $1->name() << " > " << $3->name() << endl;
	$$->setName($1->name() + " > " + $3->name());
}
| ColExpression GREATER_OR_EQUAL ColExpression
{
	kdDebug() << $1->name() << " >= " << $3->name() << endl;
	$$->setName($1->name() + " >= " + $3->name());
}
| ColExpression LESS_THAN ColExpression
{
	kdDebug() << $1->name() << " < " << $3->name() << endl;
	$$->setName($1->name() + " < " + $3->name());
}
| ColExpression LESS_OR_EQUAL ColExpression
{
	kdDebug() << $1->name() << " <= " << $3->name() << endl;
	$$->setName($1->name() + " <= " + $3->name());
}
| ColExpression LIKE ColExpression
{
	kdDebug() << $1->name() << " LIKE " << $3->name() << endl;
	$$->setName($1->name() + " LIKE " + $3->name());
}
| ColExpression PERCENT ColExpression
{
	kdDebug() << $1->name() << " % " << $3->name() << endl;
	$$->setName($1->name() + " % " + $3->name());
}
| LEFTPAREN ColExpression RIGHTPAREN
{
	kdDebug() << "(" << $2->name() << ")" << endl;
	$$ = $2;
	$$->setName("(" + $2->name() + ")");
}
| SUM LEFTPAREN ColExpression RIGHTPAREN
{
	$$ = $3;
	$$->setName("SUM(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
| SQL_MIN LEFTPAREN ColExpression RIGHTPAREN
{
	$$ = $3;
	$$->setName("MIN(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
| SQL_MAX LEFTPAREN ColExpression RIGHTPAREN
{
	$$ = $3;
	$$->setName("MAX(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
| AVG LEFTPAREN ColExpression RIGHTPAREN
{
	$$ = $3;
	$$->setName("AVG(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
| DISTINCT LEFTPAREN ColExpression RIGHTPAREN
{
	$$ = $3;
	$$->setName("DISTINCT(" + $3->name() + ")");
}
;

ColWildCard:
ASTERISK
{
	kdDebug() << "all columns" << endl;
//	field = new KexiDB::Field();
//	field->setName("*");
	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), dummy);
	parser->select()->addAsterisk(ast);
//	fieldList.append(ast);
	requiresTable = true;
}
| USER_DEFINED_NAME DOT ASTERISK
{
	kdDebug() << "  + all columns from " << $1 << endl;
	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), parser->db()->tableSchema($1));
	parser->select()->addAsterisk(ast);
//	fieldList.append(ast);
	requiresTable = true;
}
;

%%

