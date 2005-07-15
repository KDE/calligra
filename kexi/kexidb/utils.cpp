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
#include <kexidb/cursor.h>

#include <qmap.h>

#include <klocale.h>
#include <kstaticdeleter.h>

using namespace KexiDB;

//! Cache
struct TypeCache
{
	QMap< uint, TypeGroupList > tlist;
	QMap< uint, QStringList > nlist;
	QMap< uint, QStringList > slist;
	QMap< uint, Field::Type > def_tlist;
};

static KStaticDeleter<TypeCache> KexiDB_typeCacheDeleter;
TypeCache *KexiDB_typeCache = 0;

static void initList()
{
	KexiDB_typeCacheDeleter.setObject( KexiDB_typeCache, new TypeCache() );

	for (uint t=0; t<=KexiDB::Field::LastType; t++) {
		const uint tg = KexiDB::Field::typeGroup( t );
		TypeGroupList list;
		QStringList name_list, str_list;
		if (KexiDB_typeCache->tlist.find( tg )!=KexiDB_typeCache->tlist.end()) {
			list = KexiDB_typeCache->tlist[ tg ];
			name_list = KexiDB_typeCache->nlist[ tg ];
			str_list = KexiDB_typeCache->slist[ tg ];
		}
		list+= t;
		name_list += KexiDB::Field::typeName( t );
		str_list += KexiDB::Field::typeString( t );
		KexiDB_typeCache->tlist[ tg ] = list;
		KexiDB_typeCache->nlist[ tg ] = name_list;
		KexiDB_typeCache->slist[ tg ] = str_list;
	}

	KexiDB_typeCache->def_tlist[ Field::InvalidGroup ] = Field::InvalidType;
	KexiDB_typeCache->def_tlist[ Field::TextGroup ] = Field::Text;
	KexiDB_typeCache->def_tlist[ Field::IntegerGroup ] = Field::Integer;
	KexiDB_typeCache->def_tlist[ Field::FloatGroup ] = Field::Float;
	KexiDB_typeCache->def_tlist[ Field::BooleanGroup ] = Field::Boolean;
	KexiDB_typeCache->def_tlist[ Field::DateTimeGroup ] = Field::Date;
	KexiDB_typeCache->def_tlist[ Field::BLOBGroup ] = Field::BLOB;
}

const TypeGroupList KexiDB::typesForGroup(KexiDB::Field::TypeGroup typeGroup)
{
	if (!KexiDB_typeCache)
		initList();
	return KexiDB_typeCache->tlist[ typeGroup ];
}

QStringList KexiDB::typeNamesForGroup(KexiDB::Field::TypeGroup typeGroup)
{
	if (!KexiDB_typeCache)
		initList();
	return KexiDB_typeCache->nlist[ typeGroup ];
}

QStringList KexiDB::typeStringsForGroup(KexiDB::Field::TypeGroup typeGroup)
{
	if (!KexiDB_typeCache)
		initList();
	return KexiDB_typeCache->slist[ typeGroup ];
}

KexiDB::Field::Type KexiDB::defaultTypeForGroup(KexiDB::Field::TypeGroup typeGroup)
{
	if (!KexiDB_typeCache)
		initList();
	return (typeGroup <= Field::LastTypeGroup) ? KexiDB_typeCache->def_tlist[ typeGroup ] : Field::InvalidType;
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
	if (dynamic_cast<Connection*>(obj)) {
		conn = dynamic_cast<Connection*>(obj);
	}
	if (!obj || !obj->error())
		return;
	//lower level message is added to the details, if there is alread message specified
	if (!obj->msgTitle().isEmpty())
		msg += "<p>" + obj->msgTitle();
	
	if (msg.isEmpty())
		msg = "<p>" + obj->errorMsg();
	else
		details += "<p>" + obj->errorMsg();

	if (!obj->serverErrorMsg().isEmpty())
		details += "<p><b><nobr>" +i18n("Message from server:") + "</nobr></b><br>" + obj->serverErrorMsg();
	if (conn && !conn->recentSQLString().isEmpty())
		details += "<p><b><nobr>" +i18n("SQL statement:") + "</nobr></b><br>" + conn->recentSQLString();
	int serverResult;
	QString serverResultName;
	if (obj->serverResult()!=0) {
		serverResult = obj->serverResult();
		serverResultName = obj->serverResultName();
	}
	else {
		serverResult = obj->previousServerResult();
		serverResultName = obj->previousServerResultName();
	}
	if (!serverResultName.isEmpty())
		details += (QString("<p><b><nobr>")+i18n("Server result name:")+"</nobr></b><br>"+serverResultName);
	if (!details.isEmpty()) {
		details += (QString("<p><b><nobr>")+i18n("Server result number:")+"</nobr></b><br>"+QString::number(serverResult));
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

int KexiDB::idForObjectName( Connection &conn, const QString& objName, int objType )
{
	RowData data;
	if (!conn.querySingleRecord(QString("select o_id from kexi__objects where lower(o_name)='%1' and o_type=%2")
		.arg(objName.lower()).arg(objType), data))
		return 0;
	bool ok;
	int id = data[0].toInt(&ok);
	return ok ? id : 0;
}

