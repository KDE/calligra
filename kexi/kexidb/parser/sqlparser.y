/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

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
%token AS
%token ASIN
%token ASC
%token ASCII
%token ASSERTION
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
%token COLUMN
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
%token DOMAIN_TOKEN
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
%token GREATER_OR_EQUAL
%token GREATER_THAN
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
%token SEQUENCE
%token SETOPT
%token SET
%token SHOWOPT
%token SIGN
%token INTEGER_CONST
%token REAL_CONST
%token SIN
%token SQL_SIZE
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
//%token UNSIGNED_INTEGER
%token UPDATE
%token UPPER
%token USAGE
%token USER
%token USER_DEFINED_NAME
%token USER_DEFINED_NAME_DOT_ASTERISK
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

%token '-' '+'
%token '*'
%token '%'
%token '@'
%token ';'
%token ','
%token '.'
%token '$'
//%token '<'
//%token '>'
%token '(' ')'
%token '?'
%token '\''
%token '/'

%type <stringValue> USER_DEFINED_NAME
%type <stringValue> USER_DEFINED_NAME_DOT_ASTERISK
%type <stringValue> CHARACTER_STRING_LITERAL
%type <stringValue> DOUBLE_QUOTED_STRING

/*
%type <field> ColExpression
%type <field> ColView
*/
%type <expr> ColExpression
%type <expr> ColWildCard
//%type <expr> ColView
%type <expr> ColItem
%type <exprlist> ColViews
%type <expr> aExpr
%type <exprlist> aExprList
%type <expr> WhereClause

%type <coltype> SQL_TYPE
%type <integerValue> INTEGER_CONST
%type <realValue> REAL_CONST
/*%type <integerValue> SIGNED_INTEGER */

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
#include <qcstring.h>

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

		if (parser->error().type().isEmpty() 
			&& (qstricmp(str, "syntax error")==0 || qstricmp(str, "parse error")==0))
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
%}

%union {
	char stringValue[255];
	int integerValue;
	struct realType realValue;
	KexiDB::Field::Type coltype;
	KexiDB::Field *field;
	KexiDB::BaseExpr *expr;
	KexiDB::NArgExpr *exprlist;
}

//%left '=' NOT_EQUAL '>' GREATER_OR_EQUAL '<' LESS_OR_EQUAL LIKE '%' NOT
//%left '+' '-'
//%left ASTERISK SLASH

/* precedence: lowest to highest */
%left		UNION EXCEPT
%left		INTERSECT
%left		OR
%left		AND
%right	NOT
//%right		'='
//%nonassoc	'<' '>'
//%nonassoc '=' '<' '>' "<=" ">=" "<>" ":=" LIKE ILIKE SIMILAR
%nonassoc '=' LESS_THAN GREATER_THAN LESS_OR_EQUAL GREATER_OR_EQUAL NOT_EQUAL 
%nonassoc SQL_IN LIKE ILIKE SIMILAR
//%nonassoc	LIKE ILIKE SIMILAR
//%nonassoc	ESCAPE
//%nonassoc	OVERLAPS
%nonassoc	BETWEEN
//%nonassoc	IN_P
//%left		POSTFIXOP		// dummy for postfix Op rules 
//%left		Op OPERATOR		// multi-character ops and user-defined operators 
//%nonassoc	NOTNULL
//%nonassoc	ISNULL
//%nonassoc	IS NULL_P TRUE_P FALSE_P UNKNOWN // sets precedence for IS NULL, etc 
%left		'+' '-'
%left		'*' '/' '%'
%left		'^'
// Unary Operators 
//%left		AT ZONE			// sets precedence for AT TIME ZONE
//%right		UMINUS
%left		'[' ']'
%left		'(' ')'
//%left		TYPECAST
%left		'.'

/*
 * These might seem to be low-precedence, but actually they are not part
 * of the arithmetic hierarchy at all in their use as JOIN operators.
 * We make them high-precedence to support their use as function names.
 * They wouldn't be given a precedence at all, were it not that we need
 * left-associativity among the JOIN rules themselves.
 */
/*%left		JOIN UNIONJOIN CROSS LEFT FULL RIGHT INNER_P NATURAL
*/
%%

TopLevelStatement :
	Statement ';' { }
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
'(' ColDefs ')'
;

ColDefs:
ColDefs ',' ColDef|ColDef
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
| SQL_TYPE '(' INTEGER_CONST ')'
{
	kdDebug() << "sql + length" << endl;
	field = new KexiDB::Field();
	field->setPrecision($3);
	field->setType($1);
}
| VARCHAR '(' INTEGER_CONST ')'
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
//TODO: move this to all SELECT versions
	//fix fields
	KexiDB::BaseExpr *e;
	for (KexiDB::BaseExpr::ListIterator it($2->list); (e = it.current()); ++it)
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
	delete $2; //no longer needed
	
	for (QDictIterator<KexiDB::TableSchema> it(tableDict); it.current(); ++it) {
		parser->select()->addTable( it.current() );
	}

	/* set parent table if there's only one */
	if (parser->select()->tables()->count()==1)
		parser->select()->setParentTable(parser->select()->tables()->first());

	kdDebug() << "Select ColViews Tables" << endl;
}
| Select Tables
{
	kdDebug() << "Select ColViews Tables" << endl;
}
| Select ColViews WhereClause
{
	kdDebug() << "Select ColViews Conditions" << endl;
}
| Select ColViews Tables WhereClause
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

WhereClause:
WHERE aExpr
{
	$$ = $2;
}
/*|
{
	$$ = NULL;
}*/
;

aExpr:
aExpr AND aExpr
{
//	kdDebug() << "AND " << $3.debugString() << endl;
	$$ = new KexiDB::BinaryExpr( KexiDBExpr_Logical, $1, AND, $3 );
}
| aExpr OR aExpr
{
//	kdDebug() << "OR " << $3 << endl;
	$$ = new KexiDB::BinaryExpr( KexiDBExpr_Logical, $1, OR, $3 );
}
| NOT aExpr
{
	$$ = new KexiDB::UnaryExpr( NOT, $2 );
//	$$->setName($1->name() + " NOT " + $3->name());
}
| aExpr '+' aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, $1, '+', $3);
}
| aExpr '-' aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, $1, '-', $3);
}
| aExpr '/' aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, $1, '/', $3);
}
| aExpr '*' aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, $1, '*', $3);
}
| aExpr '%' aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, $1, '%', $3);
}
| aExpr NOT_EQUAL aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, NOT_EQUAL, $3);
}
| aExpr GREATER_THAN %prec GREATER_OR_EQUAL aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, GREATER_THAN, $3);
}
| aExpr GREATER_OR_EQUAL aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, GREATER_OR_EQUAL, $3);
}
| aExpr LESS_THAN %prec LESS_OR_EQUAL aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, LESS_THAN, $3);
}
| aExpr LESS_OR_EQUAL aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, LESS_OR_EQUAL, $3);
}
| aExpr LIKE aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, LIKE, $3);
}
| aExpr SQL_IN aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, SQL_IN, $3);
}
| USER_DEFINED_NAME '(' aExprList ')'
{
	kdDebug() << "  + function: " << $1 << "(" << $3->debugString() << ")" << endl;
	$$ = new KexiDB::FunctionExpr($1, $3);
}
| USER_DEFINED_NAME
{
	$$ = new KexiDB::VariableExpr( $1 );
	
//TODO: simplify this later if that's 'only one field name' expression
	kdDebug() << "  + identifier: " << $1 << endl;
//	$$ = new KexiDB::Field();
//	$$->setName($1);
//	$$->setTable(dummy);

//	parser->select()->addField(field);
	requiresTable = true;
}
/*TODO: shall we also support db name? */
| USER_DEFINED_NAME '.' USER_DEFINED_NAME
{
	$$ = new KexiDB::VariableExpr( QString($1) + "." + QString($3) );
	kdDebug() << "  + identifier.identifier: " << $3 << "." << $1 << endl;
//	$$ = new KexiDB::Field();
//	s->setTable($1);
//	$$->setName($3);
	//$$->setTable(parser->db()->tableSchema($1));
//	parser->select()->addField(field);
//??	requiresTable = true;
}
| SQL_NULL
{
	$$ = new KexiDB::ConstExpr( SQL_NULL, QVariant() );
	kdDebug() << "  + NULL" << endl;
//	$$ = new KexiDB::Field();
	//$$->setName(QString::null);
}
| CHARACTER_STRING_LITERAL
{
	$$ = new KexiDB::ConstExpr( CHARACTER_STRING_LITERAL, $1 );
//	$$ = new KexiDB::Field();
//	$$->setName($1);
//	parser->select()->addField(field);
	kdDebug() << "  + constant \"" << $1 << "\"" << endl;
}
| INTEGER_CONST
{
	$$ = new KexiDB::ConstExpr( INTEGER_CONST, $1 );
//	$$ = new KexiDB::Field();
//	$$->setName(QString::number($1));
//	parser->select()->addField(field);
	kdDebug() << "  + int constant: " << $1 << endl;
}
| REAL_CONST
{
	$$ = new KexiDB::ConstExpr( REAL_CONST, QPoint( $1.integer, $1.fractional ) );
	kdDebug() << "  + real constant: " << $1.integer << "." << $1.fractional << endl;
}
| '(' aExpr ')'
{
	kdDebug() << "(expr)" << endl;
	$$ = $2;
}
;

aExprList:
aExprList ',' aExpr
{
	$1->add( $3 );
	$$ = $1;
}
|/* EMPTY */
{
	$$ = new KexiDB::NArgExpr(0/*unknown*/);
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
aFlatTableList
{
}
;

aFlatTableList:
aFlatTableList ',' FlatTable|FlatTable
{
}
;

FlatTable:
USER_DEFINED_NAME
{
	kdDebug() << "FROM: '" << $1 << "'" << endl;

//	KexiDB::TableSchema *schema = parser->db()->tableSchema($1);
//	parser->select()->setParentTable(schema);
//	parser->select()->addTable(schema);
//	requiresTable = false;
	addTable($1);

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
}
;



ColViews:
ColViews ',' ColItem
{
	$$ = $1;
	$$->add( $3 );
	kdDebug() << "ColViews: ColViews , ColItem" << endl;
}
|ColItem
{
	$$ = new KexiDB::NArgExpr(0);
	$$->add( $1 );
	kdDebug() << "ColViews: ColItem" << endl;
}
;

ColItem:
ColExpression
{
//	$$ = new KexiDB::Field();
//	dummy->addField($$);
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	$$ = $1;
	kdDebug() << " added column expr: '" << $1->debugString() << "'" << endl;
}
| ColWildCard
{
	$$ = $1;
	kdDebug() << " added column wildcard: '" << $1->debugString() << "'" << endl;
}
| ColExpression AS USER_DEFINED_NAME
{
//	$$ = new KexiDB::Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	$$ = $1;
//TODO	parser->select()->setAlias($$, $3);
	kdDebug() << " added column expr: '" << $1->debugString() << "' as '" << $3 << "'" << endl;
}
| ColExpression USER_DEFINED_NAME
{
//	$$ = new KexiDB::Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	$$ = $1;
//TODO	parser->select()->setAlias($$, $2);
	kdDebug() << " added column expr: '" << $1->debugString() << "' as '" << $2 << "'" << endl;
}
;

ColExpression:
aExpr
{
	$$ = $1;
}
/*
| SUM '(' ColExpression ')'
{
//	$$ = new KexiDB::AggregationExpr( SUM,  );
//TODO
//	$$->setName("SUM(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
| SQL_MIN '(' ColExpression ')'
{
	$$ = $3;
//TODO
//	$$->setName("MIN(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
| SQL_MAX '(' ColExpression ')'
{
	$$ = $3;
//TODO
//	$$->setName("MAX(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
| AVG '(' ColExpression ')'
{
	$$ = $3;
//TODO
//	$$->setName("AVG(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}*/
//?
| DISTINCT '(' ColExpression ')' 
{
	$$ = $3;
//TODO
//	$$->setName("DISTINCT(" + $3->name() + ")");
}
;

ColWildCard:
'*'
{
	$$ = new KexiDB::VariableExpr("*");
	kdDebug() << "all columns" << endl;

//	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), dummy);
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
}
| USER_DEFINED_NAME_DOT_ASTERISK
{
	$$ = new KexiDB::VariableExpr($1);
	kdDebug() << "  + all columns from " << $1 << endl;
//	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), parser->db()->tableSchema($1));
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
}
;

%%

