%{
#include "sqlparser.h"
#include "sqltypes.h"
#include <iostream>

#include <field.h>

using namespace std;
%}


%{
%}

%option case-insensitive
whitespace [ \t\n]
DateSeparator [-/]
NondelimiterTerminator [ \t\n\',()<>\.:=*+-/<>@;?]
%x DelimOrSep
%x Name
%%
"@" return AMPERSAND;
";" return SEMICOLON;
"*" return ASTERISK;
"+" return ARITHMETIC_PLUS;
"-" return ARITHMETIC_MINUS;
"/" return SLASH;
"(" return LEFTPAREN;
")" return RIGHTPAREN;
"." return DOT;
"$" return DOLLAR_SIGN;
"%" return PERCENT;

"=" return EQUAL;
"<>" return NOT_EQUAL;
"<" return LESS_THAN;
"<=" return LESS_OR_EQUAL;
">" return GREATER_THAN;
">=" return GREATER_OR_EQUAL;


"'" return SINGLE_QUOTE;
"?" return QUESTION_MARK;
":" return COLON;
"," return COMMA;
[-][0123456789][0123456789]* {
	yylval.integerValue = atoi(yytext);
	return SIGNED_INTEGER;
}
[0123456789]+ {
	yylval.integerValue = atoi(yytext);
	return UNSIGNED_INTEGER;
}
[0123456789][0123456789][0123456789][0123456789]{DateSeparator}[0123456789][0123456789]{DateSeparator}[0123456789][0123456789] {
	char digitBuffer[ 10 ];
	digitBuffer[ 0 ] = yytext[ 0 ];
	digitBuffer[ 1 ] = yytext[ 1 ];
	digitBuffer[ 2 ] = yytext[ 2 ];
	digitBuffer[ 3 ] = yytext[ 3 ];
	digitBuffer[ 4 ] = 0;
//X 	sscanf( digitBuffer, "%hd", &yylval.date_.year );

	digitBuffer[ 0 ] = yytext[ 5 ];
	digitBuffer[ 1 ] = yytext[ 6 ];
	digitBuffer[ 2 ] = 0;
//X 	sscanf( digitBuffer, "%hu", &yylval.date_.month );

	digitBuffer[ 0 ] = yytext[ 8 ];
	digitBuffer[ 1 ] = yytext[ 9 ];
	digitBuffer[ 2 ] = 0;
//X 	sscanf( digitBuffer, "%hu", &yylval.date_.day );
	return DATE_VALUE;
}
ABS{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_ABS;
}
ABSOLUTE{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_ABSOLUTE;
}
"ACOS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ACOS;
}
"ADA"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ADA;
}
"ADD"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ADD;
}
"ADD_DAYS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ADD_DAYS;
}
"ADD_HOURS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ADD_HOURS;
}
"ADD_MINUTES"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ADD_MINUTES;
}
"ADD_MONTHS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ADD_MONTHS;
}
"ADD_SECONDS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ADD_SECONDS;
}
"ADD_YEARS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ADD_YEARS;
}
"ALL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
//	yylval.comparisonMod = SetComparisonConstants::MOD_ALL;
	return ALL;
}
"ALLOCATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ALLOCATE;
}
"ALTER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ALTER;
}
"AND"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
//	yylval.logicalOp = ParserConstants::LOP_AND;
	return AND;
}
"ANY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
//X 	yylval.comparisonMod = SetComparisonConstants::MOD_ANY;
	return ANY;
}
"ARE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ARE;
}
"ASCII"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ASCII;
}
"ASC"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ASC;
}
"ASIN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ASIN;
}
"AS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return AS;
}
"ASSERTION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ASSERTION;
}
"ATAN2"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ATAN2;
}
"ATAN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ATAN;
}
"AT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return AT;
}
"AUTHORIZATION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return AUTHORIZATION;
}
"AUTO_INCREMENT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return AUTO_INCREMENT;
}

"AVG"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	//yylval.aggregate = ParserConstants::AGGREGATE_AVG;
	return AVG;
}
"BEFORE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return BEFORE;
}
"BEGIN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_BEGIN;
}
"BETWEEN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return BETWEEN;
}
"BIGINT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	yylval.coltype = KexiDB::Field::BigInteger;
	return SQL_TYPE;
}
"BINARY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	yylval.coltype = KexiDB::Field::BLOB;
	return SQL_TYPE;
}
"BIT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	yylval.coltype = KexiDB::Field::ShortInteger;
	return SQL_TYPE;
}
"BIT_LENGTH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return BIT_LENGTH;
}
"BREAK"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return BREAK;
}
"BY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return BY;
}
"CASCADE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CASCADE;
}
"CASCADED"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CASCADED;
}
"CASE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CASE;
}
"CAST"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CAST;
}
"CATALOG"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CATALOG;
}
"CEILING"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CEILING;
}
"CENTER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CENTER;
}
"CHAR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	yylval.coltype = KexiDB::Field::Text;
	return SQL_TYPE;
}
"CHAR_LENGTH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CHAR_LENGTH;
}
"CHARACTER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	yylval.coltype = KexiDB::Field::Text;
	return SQL_TYPE;
}
"CHARACTER_LENGTH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CHAR_LENGTH;
}
"CHECK"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CHECK;
}
"CLOSE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CLOSE;
}
"COALESCE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return COALESCE;
}
"COBOL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return COBOL;
}
"COLLATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return COLLATE;
}
"COLLATION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return COLLATION;
}
"COLUMN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return COLUMN;
}
"COMMIT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return COMMIT;
}
"COMPUTE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return COMPUTE;
}
"CONCAT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CONCAT;
}
"CONNECT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CONNECT;
}
"CONNECTION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CONNECTION;
}
"CONSTRAINT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CONSTRAINT;
}
"CONSTRAINTS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CONSTRAINTS;
}
"CONTINUE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CONTINUE;
}
"CONVERT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CONVERT;
}
"CORRESPONDING"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CORRESPONDING;
}
"COS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return COS;
}
"COT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return COT;
}
"COUNT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return COUNT;
}
"CREATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CREATE;
}
"CURDATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CURDATE;
}
"CURRENT_DATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CURRENT_DATE;
}
"CURRENT_TIME"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CURRENT_TIME;
}
"CURRENT_TIMESTAMP"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CURRENT_TIMESTAMP;
}
"WHERE{Whitespace}CURRENT{Whitespace}OF{NondelimiterTerminator}" {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return WHERE_CURRENT_OF;
}
"CURRENT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CURRENT;
}
"CURSOR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CURSOR;
}
"CURTIME"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CURTIME;
}
"DATABASE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DATABASE;
}
"DATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	yylval.coltype = KexiDB::Field::Date;
	return SQL_TYPE;
}
"DATE_FORMAT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DATE_FORMAT;
}
"DATE_REMAINDER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DATE_REMAINDER;
}
"DAYOFMONTH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DAYOFMONTH;
}
"DAYOFWEEK"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DAYOFWEEK;
}
"DAYOFYEAR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DAYOFYEAR;
}
"DAYS_BETWEEN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DAYS_BETWEEN;
}
"DAY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DAY;
}
"DEALLOCATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DEALLOCATE;
}
"DEC"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DEC;
}
"DECIMAL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DEC;
}
"DECLARE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DECLARE;
}
"DEFAULT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DEFAULT;
}
"DEFERRABLE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DEFERRABLE;
}
"DEFERRED"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DEFERRED;
}
"DELETE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_DELETE;
}
"DESC"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DESC;
}
"DESCRIBE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DESCRIBE;
}
"DESCRIPTOR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DESCRIPTOR;
}
"DIAGNOSTICS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DIAGNOSTICS;
}
"DICTIONARY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DICTIONARY;
}
"DIRECTORY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DIRECTORY;
}
"DISCONNECT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DISCONNECT;
}
"DISPLACEMENT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DISPLACEMENT;
}
"DISTINCT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DISTINCT;
}
"DOMAIN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DOMAIN_TOKEN;
}
"DOUBLE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_DOUBLE;
}
"DROP"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return DROP;
}
"ELSE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ELSE;
}
"END"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return END;
}
"END_EXEC"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return END_EXEC;
}
"ESCAPE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ESCAPE;
}
"EXCEPT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return EXCEPT;
}
"EXCEPTION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_EXCEPTION;
}
"EXEC"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return EXEC;
}
"EXECUTE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return EXECUTE;
}
"EXISTS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return EXISTS;
}
"EXP"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return EXP;
}
"EXTERNAL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return EXTERNAL;
}
"EXTRACT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return EXTRACT;
}
"FALSE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_FALSE;
}
"FETCH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return FETCH;
}
"FIRST"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return FIRST;
}
"FLOAT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_FLOAT;
}
"FLOOR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return FLOOR;
}
"FN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return FN;
}
"FOR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return FOR;
}
"FOREIGN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return FOREIGN;
}
"FORTRAN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return FORTRAN;
}
"FOUND"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return FOUND;
}
"FROM"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return FROM;
}
"FULL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return FULL;
}
"GET"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return GET;
}
"GLOBAL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return GLOBAL;
}
"GO"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return GO;
}
"GOTO"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return GOTO;
}
"GRANT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return GRANT;
}
"GROUP"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return GROUP;
}
"HAVING"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return HAVING;
}
"HOUR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return HOUR;
}
"HOURS_BETWEEN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return HOURS_BETWEEN;
}
"IDENTITY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return IDENTITY;
}
"IFNULL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return IFNULL;
}
"IGNORE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_IGNORE;
}
"IMMEDIATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return IMMEDIATE;
}
"IN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_IN;
}
"INCLUDE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INCLUDE;
}
"INDEX"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INDEX;
}
"INDICATOR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INDICATOR;
}
"INITIALLY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INITIALLY;
}
"INNER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INNER;
}
"INPUT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INPUT;
}
"INSENSITIVE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INSENSITIVE;
}
"INSERT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INSERT;
}
"INTEGER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	yylval.coltype = KexiDB::Field::Integer;
	return SQL_TYPE;
}
"INT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INTEGER;
}
"INTERSECT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INTERSECT;
}
"INTERSECTION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INTERSECT;
}
"INTERVAL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return INTERVAL;
}
"INTO"{whitespace} {
	BEGIN Name;
	return INTO;
}
"IS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return IS;
}
"ISOLATION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ISOLATION;
}
"JOIN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return JOIN;
}
"JUSTIFY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return JUSTIFY;
}
"KEY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return KEY;
}
"LANGUAGE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LANGUAGE;
}
"LAST"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LAST;
}
"LCASE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LCASE;
}
"LEFT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LEFT;
}
"LENGTH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LENGTH;
}
"LEVEL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LEVEL;
}
"LIKE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LIKE;
}
"LINE_WIDTH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LINE_WIDTH;
}
"LOCAL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LOCAL;
}
"LOCATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LOCATE;
}
"LOG"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LOG;
}
"LONG"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	yylval.coltype = KexiDB::Field::Integer;
	return SQL_TYPE;
}
"LOWER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LOWER;
}
"LTRIM"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return LTRIM;
}
"MATCH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return MATCH;
}
"MAX"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
//	yylval.aggregate = ParserConstants::AGGREGATE_MAX;
	return SQL_MAX;
}
"MICROSOFT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return MICROSOFT;
}
"MIN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	//yylval.aggregate = ParserConstants::AGGREGATE_MIN;
	return SQL_MIN;
}
"MINUS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return MINUS;
}
"MINUTE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return MINUTE;
}
"MINUTES_BETWEEN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return MINUTES_BETWEEN;
}
"MODIFY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return MODIFY;
}
"MOD"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return MOD;
}
"MODULE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return MODULE;
}
"MONTH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return MONTH;
}
"MONTHS_BETWEEN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return MONTHS_BETWEEN;
}
"MUMPS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return MUMPS;
}
"NAMES"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return NAMES;
}
"NATIONAL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return NATIONAL;
}
"NCHAR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return NCHAR;
}
"NEXT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return NEXT;
}
"NODUP"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return NODUP;
}
"NONE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return NONE;
}
"NOT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return NOT;
}
"NOW"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return NOW;
}
"NULL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_NULL;
}
"NULLIF"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return NULLIF;
}
"NUMERIC"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return NUMERIC;
}
"OCTET_LENGTH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return OCTET_LENGTH;
}
"ODBC"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ODBC;
}
"OF"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return OF;
}
"OFF"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_OFF;
}
"ON"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_ON;
}
"ONLY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ONLY;
}
"OPEN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return OPEN;
}
"OPTION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return OPTION;
}
"OR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	//yylval.logicalOp = ParserConstants::LOP_OR;
	return OR;
}
"ORDER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ORDER;
}
"OUTER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return OUTER;
}
"OUTPUT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return OUTPUT;
}
"OVERLAPS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return OVERLAPS;
}
"PAGE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PAGE;
}
"PARTIAL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PARTIAL;
}
"PASCAL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_PASCAL;
}
"PERSISTENT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PERSISTENT;
}
"PI"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return CQL_PI;
}
"PLI"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PLI;
}
"POSITION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return POSITION;
}
"PRECISION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PRECISION;
}
"PREPARE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PREPARE;
}
"PRESERVE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PRESERVE;
}
"PRIMARY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PRIMARY;
}
"PRIOR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PRIOR;
}
"PRIVILEGES"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PRIVILEGES;
}
"PROCEDURE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PROCEDURE;
}
"PRODUCT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PRODUCT;
}
"PUBLIC"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return PUBLIC;
}
"QUARTER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return QUARTER;
}
"QUIT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return QUIT;
}
"RAND"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return RAND;
}
"READ_ONLY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return READ_ONLY;
}
"REAL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return REAL;
}
"REFERENCES"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return REFERENCES;
}
"REPEAT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return REPEAT;
}
"REPLACE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return REPLACE;
}
"RESTRICT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return RESTRICT;
}
"REVOKE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return REVOKE;
}
"RIGHT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return RIGHT;
}
"ROLLBACK"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ROLLBACK;
}
"ROWS"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return ROWS;
}
"RPAD"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return RPAD;
}
"RTRIM"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return RTRIM;
}
"SCHEMA"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SCHEMA;
}
"SCREEN_WIDTH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SCREEN_WIDTH;
}
"SCROLL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SCROLL;
}
"SECOND"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SECOND;
}
"SECONDS_BETWEEN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SECONDS_BETWEEN;
}
"SELECT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SELECT;
}
"SEQUENCE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SEQUENCE;
}
"SETOPT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SETOPT;
}
"SET"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SET;
}
"SHOWOPT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SHOWOPT;
}
"SIGN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SIGN;
}
"SIN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SIN;
}
"SIZE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_SIZE;
}
"SMALLINT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SMALLINT;
}
"SOME"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SOME;
}
"SQL"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL;
}
"SQLCA"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQLCA;
}
"SQLCODE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQLCODE;
}
"SQLERROR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQLERROR;
}
"SQLSTATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQLSTATE;
}
"SQLWARNING"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQLWARNING;
}
"SQRT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQRT;
}
"STDEV"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return STDEV;
}
"SUBSTRING"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SUBSTRING;
}
"SUM"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	//yylval.aggregate = ParserConstants::AGGREGATE_SUM;
	return SUM;
}
"SYSDATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SYSDATE;
}
"SYSDATE_FORMAT"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SYSDATE_FORMAT;
}
"SYSTEM"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SYSTEM;
}
"TABLE"{whitespace} {
	BEGIN Name;
	return TABLE;
}
"TAN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TAN;
}
"TEMPORARY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TEMPORARY;
}
"THEN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return THEN;
}
"TIME"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TIME;
}
"TIMESTAMP"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TIMESTAMP;
}
"TIMEZONE_HOUR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TIMEZONE_HOUR;
}
"TIMEZONE_MINUTE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TIMEZONE_MINUTE;
}
"TO"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TO;
}
"TO_CHAR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TO_CHAR;
}
"TO_DATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TO_DATE;
}
"TRANSACTION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TRANSACTION;
}
"TRANSLATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TRANSLATE;
}
"TRANSLATION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TRANSLATION;
}
"TRUE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_TRUE;
}
"TRUNCATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return TRUNCATE;
}
"GENERAL_TITLE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return GENERAL_TITLE;
}
"UCASE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return UCASE;
}
"UNION"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return UNION;
}
"UNIQUE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return UNIQUE;
}
"UNKNOWN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return SQL_UNKNOWN;
}
"UPDATE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return UPDATE;
}
"UPPER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return UPPER;
}
"USAGE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return USAGE;
}
"USER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return USER;
}
"USING"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return USING;
}
"VALUES"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return VALUES;
}
"VALUE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return VALUE;
}
"VARBINARY"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return VARBINARY;
}
"VARCHAR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
//	yylval.coltype = KexiDBField::SQLVarchar;
//	return SQL_TYPE;
	return VARCHAR;
}
"VARYING"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return VARYING;
}
"VENDOR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return VENDOR;
}
"VIEW"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return VIEW;
}
"WEEK"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return WEEK;
}
"WHEN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return WHEN;
}
"WHENEVER"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return WHENEVER;
}
"WHERE"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return WHERE;
}
"WITH"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return WITH;
}
"WORD_WRAPPED"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return WORD_WRAPPED;
}
"WORK"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return WORK;
}
"WRAPPED"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return WRAPPED;
}
"YEAR"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return YEAR;
}
"YEARS_BETWEEN"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return YEARS_BETWEEN;
}
"E"{NondelimiterTerminator} {
	unput( yytext[ yyleng - 1 ] );
	BEGIN DelimOrSep;
	return EXPONENT;
}
[-][-].*$ ;

['][^']*['] { 
strcpy(yylval.stringValue, yytext);
return CHARACTER_STRING_LITERAL;
}

["][^"].*["]\n {
strcpy(yylval.stringValue, yytext);
return DOUBLE_QUOTED_STRING;
}

[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_]* {
//	BEGIN DelimOrSep;
	strcpy(yylval.stringValue, yytext);
//	yylval.stringValue = yytext;
//	int len = strlen( yylval.parserUserName );
//	for( scannerPtr = yylval.parserUserName + ( len - 1 ); *scannerPtr == ' ' && scannerPtr >= yylval.parserUserName; scannerPtr-- )
//		*scannerPtr = 0;
	return USER_DEFINED_NAME;
}
{whitespace}* ;

<DelimOrSep>"@" {
	BEGIN 0;
	return AMPERSAND;
}
<DelimOrSep>";" {
	BEGIN 0;
	return SEMICOLON;
}
<DelimOrSep>"*" {
	BEGIN 0;
	return ASTERISK;
}
<DelimOrSep>"+" {
	BEGIN 0;
	return ARITHMETIC_PLUS;
}
<DelimOrSep>"-" {
	BEGIN 0;
	return ARITHMETIC_MINUS;
}
<DelimOrSep>"/" {
	BEGIN 0;
	return SLASH;
}
<DelimOrSep>"(" {
	BEGIN 0;
	return LEFTPAREN;
}
<DelimOrSep>")" {
	BEGIN 0;
	return RIGHTPAREN;
}
<DelimOrSep>"%" {
	BEGIN 0;
//	yylval.comparisonOp = SetComparisonConstants::SETCOMP_EQUAL;
	return PERCENT;
}
<DelimOrSep>"=" {
	BEGIN 0;
//	yylval.comparisonOp = SetComparisonConstants::SETCOMP_EQUAL;
	return EQUAL;
}
<DelimOrSep>"<>" {
	BEGIN 0;
//	yylval.comparisonOp = SetComparisonConstants::SETCOMP_NOT_EQUAL;
	return NOT_EQUAL;
}
<DelimOrSep>">" {
	BEGIN 0;
//	yylval.comparisonOp = SetComparisonConstants::SETCOMP_GREATER;
	return GREATER_THAN;
}
<DelimOrSep>">=" {
	BEGIN 0;
//	yylval.comparisonOp = SetComparisonConstants::SETCOMP_GREATER_OR_EQUAL;
	return GREATER_OR_EQUAL;
}
<DelimOrSep>"<=" {
	BEGIN 0;
//	yylval.comparisonOp = SetComparisonConstants::SETCOMP_LESS_OR_EQUAL;
	return LESS_OR_EQUAL;
}
<DelimOrSep>"<" {
	BEGIN 0;
	//yylval.comparisonOp = SetComparisonConstants::SETCOMP_LESS;
	return LESS_THAN;
}
<DelimOrSep>"'" {
	BEGIN 0;
	return SINGLE_QUOTE;
}
<DelimOrSep>"?" {
	BEGIN 0;
	return QUESTION_MARK;
}
<DelimOrSep>":" {
	BEGIN 0;
	return COLON;
}
<DelimOrSep>"," {
	BEGIN 0;
	return COMMA;
}
<DelimOrSep>{whitespace}+ {
	BEGIN 0;
}
<Name>[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_]* {
	BEGIN DelimOrSep;
//	strcpy( yylval.parserUserName, yytext );
	strcpy(yylval.stringValue, yytext);
	return USER_DEFINED_NAME;
}
%%

void tookenize(const char *data)
{
	yy_switch_to_buffer(yy_scan_string(data));
}

