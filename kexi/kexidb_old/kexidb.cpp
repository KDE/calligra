/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Joseph Wenninger<jowenn@kde.org>

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

#include <kdebug.h>
#include <qtextcodec.h> //tmp

#include "kexidbinterfacemanager.h"
#include "kexidbrecordset.h"
#include "kexidb.h"

KexiDB::KexiDB(QObject *parent, const char *name) : QObject(parent, name)
{
	kdDebug() << "KexiDB::KexiDB()" << endl;
	m_dbwatcher = new KexiDBWatcher(this, "dbwatcher");
	m_encoding = Latin1;
}


bool
KexiDB::connect(QString, QString, QString, QString, QString)
{
	kdDebug() << "KexiDB::connect(host): sorry you are wrong here" << endl;
	return false;
}

bool
KexiDB::connect(QString, QString, QString, QString, QString, QString, bool)
{
	kdDebug() << "KexiDB::connect(db): sorry you are wrong here" << endl;
	return false;
}

bool
KexiDB::load(QString, bool persistant)
{
	return false;
}

QStringList
KexiDB::databases()
{
	return QStringList();
}

KexiDBRecordSet*
KexiDB::queryRecord(QString, bool)
{
	return 0;
}



QString
KexiDB::driverName()
{
	return QString::fromLatin1("NONE");
}

/*
QStringList
KexiDB::tableNames()
{
	return QStringList();
}

const KexiDBTable *const KexiDB::table(const QString&)
{
	return 0;
}

*/

bool
KexiDB::query(QString)
{
	return false;
}

QString
KexiDB::escape(const QString &)
{
	return QString::null;
}

QString
KexiDB::escape(const QByteArray& str)
{
	return QString::null;
}

bool
KexiDB::alterField(const QString& /*table*/, const QString& /*field*/, const QString& /*newFieldName*/,
	KexiDBField::ColumnType /*dtype*/, int /*length*/, int /*precision*/,
	KexiDBField::ColumnConstraints /*constraints*/, bool /*binary*/, bool /*unsignedType*/,
	const QString& /*defaultVal*/)
{
	return false;
}

bool
KexiDB::createField(const QString& /*table*/, const QString& /*field*/, KexiDBField::ColumnType /*dtype*/,
	int /*length*/, int /*precision*/, KexiDBField::ColumnConstraints /*constraints*/, bool /*binary*/,
	bool /*unsignedType*/, const QString& /*defaultVal*/)
{
	return false;
}

bool KexiDB::createTable(const KexiDBTable& tableDef) {
	if (tableDef.fieldCount()<1) return false;
	KexiDBField f=tableDef.field(0);
	if (!createField(f,KexiDBTableStruct(),true)) return false;
	for(int i=1;i<int(tableDef.fieldCount());i++)
	{
		f=tableDef.field(i);
		if (!createField(f,KexiDBTableStruct(),false))
			return false;
	}
	return true;

}

bool
KexiDB::alterField(const KexiDBField& changedField, unsigned int index,
	KexiDBTableStruct fields)
{
	return false;
}

bool
KexiDB::createField(const KexiDBField& newField, KexiDBTableStruct fields,
	bool createTable)
{
	return false;
}


unsigned long
KexiDB::affectedRows()
{
	return 0;
}

KexiDBTableStruct
KexiDB::getStructure(const QString&)
{
	return KexiDBTableStruct();
}


QStringList
KexiDB::getColumns(const QString& table)
{
	QStringList res;
	KexiDBTableStruct tmp=getStructure(table);
	for (KexiDBField *f=tmp.first();f;f=tmp.next())
	{
		res.append(f->name());
	}
	tmp.setAutoDelete(true);
	tmp.clear();

	return res;
}

QString
KexiDB::getNativeDataType(const KexiDBField::ColumnType& t)
{
	return QString::null;
}

KexiDB::~KexiDB()
{
}

const QString
KexiDB::decode(const char *c)
{
	switch(m_encoding)
	{
		case Latin1:
			return QString::fromLatin1(c);

		case Utf8:
			return QString::fromUtf8(c);

		case Ascii:
#if (QT_VERSION >= 310)
			return QString::fromAscii(c);
#else //(js) tmp
		    return QString::fromLatin1( c );
#endif
		case Local8Bit:
			return QString::fromLocal8Bit(c);
	}
	return QString::null;
}

const char*
KexiDB::encode(const QString &v)
{
	switch(m_encoding)
	{
		case Latin1:
			return v.latin1();

		case Utf8:
			return v.utf8();

		case Ascii:
			return v.ascii();

		case Local8Bit:
			return v.local8Bit();
	}
	return v.latin1();
}

void KexiDB::latestError(KexiDBError **error) {
    *error=latestError();
}


#include "kexidb.moc"
