/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@kde.org>

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

#include <connection.h>
#include <tableschema.h>
//#include "kexidbselect.h"
#include "parser.h"
#include "sqlparser.h"

namespace KexiDB
{

Parser::Parser(Connection *db)
{
	m_db = db;

	m_operation = OP_None;
//	m_table = 0;
	m_select = 0;
}

void
Parser::createTable(const char *t)
{
#if 0
	if(m_table)
		return;

	m_table = new KexiDBTable(t);
#endif
}

void
Parser::createSelect()
{
	if(m_select)
		return;

	m_select = new QuerySchema();
}

void
Parser::parse(const QString &statement)
{
	clear();
	parseData(this, statement.latin1());
}

void
Parser::clear()
{
//	delete m_table;
//	m_table = 0;
	setOperation(OP_None);
}

Parser::~Parser()
{
}

}

