// -*- Mode: c++-mode; c-basic-offset: 2; indent-tabs-mode: t; tab-width: 2; -*-
/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Joseph Wenninger<jowenn@kde.org>
   Copyright (C) 2003   Zack Rusin <zack@kde.org>

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

KexiDB::~KexiDB()
{
}

QStringList
KexiDB::databases()
{
	return QStringList();
}

KexiDB::DBType
KexiDB::dbType() const
{
	return NoDB;
}

QString
KexiDB::driverName() const
{
	return QString::fromLatin1("NONE");
}

bool KexiDB::createTable(const KexiDBTable& tableDef)
{
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

unsigned long
KexiDB::affectedRows() const
{
	return 0;
}

KexiDBWatcher*
KexiDB::watcher() const
{
	return m_dbwatcher;
}

KexiDBTableStruct
KexiDB::structure(const QString&) const
{
	return KexiDBTableStruct();
}


QStringList
KexiDB::columns(const QString& table) const
{
	QStringList res;
	KexiDBTableStruct tmp=structure(table);
	for (KexiDBField *f=tmp.first();f;f=tmp.next())
	{
		res.append(f->name());
	}
	tmp.setAutoDelete(true);
	tmp.clear();

	return res;
}

RelationList
KexiDB::relations() const
{
	return m_relations;
}

const QString
KexiDB::decode(const char *c)
{
/*	kdDebug() << "fromLatin1: " << QString::fromLatin1(c) << endl;
	kdDebug() << "fromAscii: " << QString::fromAscii(c) << endl;
	kdDebug() << "fromLocal8Bit: " << QString::fromLocal8Bit(c) << endl;
	kdDebug() << "fromUtf8: " << QString::fromUtf8(c) << endl;*/
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

void KexiDB::latestError(KexiDBError **error)
{
	*error=latestError();
}


bool KexiDB::load(const QString& /*file*/, bool /*persistant*/ )
{
	return false;
}

#include "kexidb.moc"
