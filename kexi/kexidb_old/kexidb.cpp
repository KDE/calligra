/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Joseph Wenninger <jowenn@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include <kdebug.h>

#include "kexidbinterfacemanager.h"
#include "kexidbrecord.h"
#include "kexidb.h"

KexiDB::KexiDB(QObject *parent, const char *name) : QObject(parent, name)
{
	kdDebug() << "KexiDB::KexiDB()" << endl;
	m_dbwatcher = new KexiDBWatcher(this, "dbwatcher");
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

KexiDBRecord*
KexiDB::queryRecord(QString, bool)
{
	return 0;
}



QString
KexiDB::driverName()
{
	return QString::fromLatin1("NONE");
}

QStringList
KexiDB::tables()
{
	return QStringList();
}


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
	if (tableDef.count()<1) return false;
	KexiDBTable::const_iterator it=tableDef.begin();
	if (!createField(*it,KexiDBTableStruct(),true)) return false;
	++it;
	for(;it!=tableDef.end();++it)
	{
		if (!createField(*it,KexiDBTableStruct(),false))
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

KexiDBResult*
KexiDB::getResult()
{
	return new KexiDBResult(this);
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

void KexiDB::latestError(KexiDBError **error) {
    *error=latestError();
}


#include "kexidb.moc"
