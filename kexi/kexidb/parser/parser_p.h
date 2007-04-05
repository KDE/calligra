/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIDB_PARSER_P_H
#define KEXIDB_PARSER_P_H

#include <q3valuelist.h>
#include <q3dict.h>
#include <q3asciicache.h>
#include <qstring.h>

#include <kexidb/queryschema.h>
#include <kexidb/tableschema.h>
#include <kexidb/connection.h>
#include <kexidb/expression.h>
#include "sqltypes.h"
#include "parser.h"

namespace KexiDB {

//! @internal
class ParserPrivate
{
	public:
		ParserPrivate();
		~ParserPrivate();

		void clear();

		int operation;
		TableSchema *table;
		QuerySchema *select;
		Connection *db;
		QString statement;
		ParserError error;
		Q3AsciiCache<char> reservedKeywords;
		bool initialized : 1;
};


/*! Data used on parsing. @internal */
class ParseInfo
{
	public:
		ParseInfo(QuerySchema *query);
		~ParseInfo();

		//! collects positions of tables/aliases with the same names
		Q3Dict< Q3ValueList<int> > repeatedTablesAndAliases;

		QString errMsg, errDescr; //helpers
		QuerySchema *querySchema;
};

}

void yyerror(const char *str);
void setError(const QString& errName, const QString& errDesc);
void setError(const QString& errDesc);
//bool parseData(KexiDB::Parser *p, const char *data);
bool addColumn( KexiDB::ParseInfo& parseInfo, KexiDB::BaseExpr* columnExpr );
KexiDB::QuerySchema* buildSelectQuery( 
	KexiDB::QuerySchema* querySchema, KexiDB::NArgExpr* colViews, 
	KexiDB::NArgExpr* tablesList = 0, SelectOptionsInternal* options = 0 ); //KexiDB::BaseExpr* whereExpr = 0 );

extern KexiDB::Parser *parser;
extern KexiDB::Field *field;


#endif
