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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kexidb/utils.h>
#include <kexidb/driver.h>
#include <kexidb/cursor.h>

#include <qmap.h>

#include <klocale.h>

using namespace KexiDB;

bool KexiDB::deleteRow(Connection &conn, TableSchema *table, const QString &keyname, const QString &keyval)
{
	return table!=0 && conn.drv_executeSQL("DELETE FROM " + table->name() + " WHERE " + keyname + "=" + conn.driver()->valueToSQL( Field::Text, QVariant(keyval) ));
}

bool KexiDB::deleteRow(Connection &conn, TableSchema *table, const QString &keyname, int keyval)
{
	return table!=0 && conn.drv_executeSQL("DELETE FROM " + table->name() + " WHERE " + keyname + "=" + conn.driver()->valueToSQL( Field::Integer, QVariant(keyval) ));
}

bool KexiDB::replaceRow(Connection &conn, TableSchema *table, const QString &keyname, const QString &keyval, const QString &valname, QVariant val, int ftype)
{
	if (!table || !KexiDB::deleteRow(conn, table, keyname, keyval))
		return false;
	return conn.drv_executeSQL("INSERT INTO " + table->name() + " (" + keyname + "," + valname + ") VALUES (" + conn.driver()->valueToSQL( Field::Text, QVariant(keyval) ) + "," + conn.driver()->valueToSQL( ftype, val) + ")");
}

//! Cache
QMap< uint, TypeGroupList > tlist;
QMap< uint, QStringList > nlist;
QMap< uint, QStringList > slist;

inline void initList()
{
	if (!tlist.isEmpty())
		return;

	for (uint t=0; t<=KexiDB::Field::LastType; t++) {
		const uint tg = KexiDB::Field::typeGroup( t );
		TypeGroupList list;
		QStringList name_list, str_list;
		if (tlist.find( tg )!=tlist.end()) {
			list = tlist[ tg ];
			name_list = nlist[ tg ];
			str_list = slist[ tg ];
		}
		list+= t;
		name_list += KexiDB::Field::typeName( t );
		str_list += KexiDB::Field::typeString( t );
		tlist[ tg ] = list;
		nlist[ tg ] = name_list;
		slist[ tg ] = str_list;
	}
}

const TypeGroupList KexiDB::typesForGroup(KexiDB::Field::TypeGroup typeGroup)
{
	initList();
	return tlist[ typeGroup ];
}

QStringList KexiDB::typeNamesForGroup(KexiDB::Field::TypeGroup typeGroup)
{
	initList();
	return nlist[ typeGroup ];
}

QStringList KexiDB::typeStringsForGroup(KexiDB::Field::TypeGroup typeGroup)
{
	initList();
	return slist[ typeGroup ];
}

void KexiDB::getHTMLErrorMesage(Object* obj, QString& msg, QString &details)
{
	Connection *conn = 0;
	if (!obj || !obj->error()) {
		if (dynamic_cast<Cursor*>(obj)) {
			conn = dynamic_cast<Cursor*>(obj)->connection();
			obj = conn;
		}
		else {
			return;
		}
	}
	if (!obj || !obj->error())
		return;
	//lower level message is added th the details, if there is alread message specified
	if (msg.isEmpty())
		msg = "<p>" + obj->errorMsg();
	else
		details += "<p>" + obj->errorMsg();

	if (!obj->serverErrorMsg().isEmpty())
		details += "<p><b><nobr>" +i18n("Message from server:") + "</nobr></b><br>" + obj->serverErrorMsg();
	if (conn && !conn->recentSQLString().isEmpty())
		details += "<p><b><nobr>" +i18n("SQL statement:") + "</nobr></b><br>" + conn->recentSQLString();
	QString resname = obj->serverResultName();
	if (!resname.isEmpty())
		details += (QString("<p><b><nobr>")+i18n("Server result name:")+"</nobr></b><br>"+resname);
	if (!details.isEmpty()) {
		details += (QString("<p><b><nobr>")+i18n("Result number:")+"</nobr></b><br>"+QString::number(obj->serverResult()));
	}
}

void KexiDB::getHTMLErrorMesage(Object* obj, QString& msg)
{
	getHTMLErrorMesage(obj, msg, msg);
}

void KexiDB::getHTMLErrorMesage(Object* obj, ResultInfo *result)
{
	getHTMLErrorMesage(obj, result->msg, result->desc);
}

QString KexiDB::sqlWhere(KexiDB::Driver *drv, KexiDB::Field::Type t, const QString fieldName, const QVariant value)
{
	if (value.isNull())
		return fieldName + " is NULL";
	return fieldName + "=" + drv->valueToSQL( t, value );
}
