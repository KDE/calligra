/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)                  
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#include "krscriptfunctions.h"
#include <kexidb/cursor.h>
#include <kdebug.h>

KRScriptFunctions::KRScriptFunctions(KexiDB::Connection *c)
{
	_conn = c;
}


KRScriptFunctions::~KRScriptFunctions()
{
}

void KRScriptFunctions::setSource(const QString &s)
{
	_source = s;	
}

void KRScriptFunctions::setWhere(const QString&w)
{
	_where = w;
}

qreal KRScriptFunctions::math(const QString &function, const QString &field)
{
	qreal ret;
	QString sql;
	
	sql = "SELECT " + function +"(" + field + ") FROM " + _source + " WHERE(" + _where + ")";
	
	kDebug() << sql << endl;
	KexiDB::Cursor *curs= _conn->executeQuery(sql);
	
	if (curs)
	{
		ret = curs->value(0).toDouble();
	}
	else
	{
		ret = 0.0;
	}
	
	return ret;
}

qreal KRScriptFunctions::sum(const QString &field)
{
	return math("SUM", field);
}

qreal KRScriptFunctions::avg(const QString &field)
{
	return math("AVG", field);
}

qreal KRScriptFunctions::min(const QString &field)
{
	return math("MIN", field);
}

qreal KRScriptFunctions::max(const QString &field)
{
	return math("MAX", field);
}

qreal KRScriptFunctions::count(const QString &field)
{
	return math("COUNT", field);
}

