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
%token TITLE
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
%type <coltype> SQL_TYPE
%type <integerValue> UNSIGNED_INTEGER
%type <integerValue> SIGNED_INTEGER

%{
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#include <qobject.h>
#include <connection.h>
#include <queryschema.h>
#include <field.h>
#include <tableschema.h>

#include "parser.h"
#include "sqlparser.h"
#include "sqltypes.h"

//	using namespace std;

	#define YYSTACK_USE_ALLOCA 1
	#define YYMAXDEPTH 255

	KexiDB::Parser *parser;
	KexiDB::Field *field;

	int yyparse();
	int yylex();
	void tookenize(const char *data);

	void yyerror(const char *str)
	{
		std::cerr << "error: " << str << std::endl;
		parser->setOperation(KexiDB::Parser::OP_Error);
	}

	void parseData(KexiDB::Parser *p, const char *data)
	{
		parser = p;
		field = 0;
		tookenize(data);
		yyparse();
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
}

%%

Statement :
	Statement CreateTableStatement 		{ YYACCEPT; }
	| Statement SelectStatement 		{ YYACCEPT; }
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
	std::cout << "adding field " << $1 << std::endl;
	field->setName($1);
	parser->table()->addField(field);

//	delete field;
	field = 0;
}
| USER_DEFINED_NAME ColType ColKeys
{
	std::cout << "adding field " << $1 << std::endl;
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
	std::cout << "primary" << std::endl;
}
| NOT SQL_NULL
{
	field->setNotNull(true);
	std::cout << "not_null" << std::endl;
}
| AUTO_INCREMENT
{
	field->setAutoIncrement(true);
	std::cout << "ainc" << std::endl;
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
	std::cout << "sql + length" << std::endl;
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
Select ColViews FROM USER_DEFINED_NAME
{
	std::cout << "FROM: '" << $4 << "'" << std::endl;

#if 0
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
#endif

	YYACCEPT;
}
;

Select:
SELECT
{
	std::cout << "SELECT" << std::endl;
	parser->createSelect();
	parser->setOperation(KexiDB::Parser::OP_Select);
}
;

ColViews:
ColViews COMMA ColView|ColView
{
}
;

ColView:
ASTERISK
{
	std::cout << "all columns" << std::endl;
//	parser->select()->setUnresolvedWildcard(true);
}
| USER_DEFINED_NAME
{
	std::cout << "  + col " << $1 << std::endl;
	KexiDB::Field *s = new KexiDB::Field();
	s->setName($1);
	parser->select()->addField(s);
}
| USER_DEFINED_NAME DOT USER_DEFINED_NAME
{
	std::cout << "  + col " << $3 << " based on " << $1 << std::endl;
	KexiDB::Field *s = new KexiDB::Field();
//	s->setTable($1);
	s->setName($3);
	parser->select()->addField(s);
}
| USER_DEFINED_NAME DOT ASTERISK
{
	std::cout << "  + all columns from " << $1 << std::endl;
//	parser->select()->setUnresolvedWildcard(true);
}
;

%%

