/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

//#include <qstringlist.h>
#include <iostream.h>

#include <kgenericfactory.h>
#include <kdebug.h>
#include <klocale.h>

#include <kexidberror.h>

#include "cqldb.h"
#include "cqlrecord.h"

K_EXPORT_COMPONENT_FACTORY(kexicqlinterface, KGenericFactory<CqlDB>( "kexicqlinterface" ));

CqlDB::CqlDB(QObject *parent, const char *name, const QStringList &)
 : KexiDB(parent, name)
{
	kdDebug() << "CqlDB::CqlDB()" << endl;

	try
	{
		m_db = new SqlHandle();
	}
	catch(CqlException& ex)
	{
		cerr << ex << endl;
		m_db = 0;
	}

	kdDebug() << "CqlDB::CqlDB(): handle:" << m_db << endl;
}

QString
CqlDB::driverName()
{
	return QString("cql");
}

bool
CqlDB::load(QString file)
{
	kdDebug() << "CqlDB::load()" << endl;

	if(!m_db)
		return false;

	kdDebug() << "CqlDB::load(): proceeding" << endl;

	try
	{
		m_db->connect("PUBLIC");
	}
	catch(CqlException& ex)
	{
		cerr << ex << endl;
		throw new KexiDBError(0, i18n("Connection failed"));
	}
}

QStringList
CqlDB::tables()
{
	QStringList tables;
	TableIdList *tlist = m_db->getTableIds("PUBLIC");
	for(TableId *table = tlist->first(); table; table = tlist->next())
	{
		tables.append(cqlString(table->tableName()));
	}

	return tables;
}

bool
CqlDB::query(QString statement)
{
	kdDebug() << "CqlDB::query()" << endl;

	if(!m_db)
		return false;

	QString rs(statement + ";");

	kdDebug() << "CqlDB::query() query: " << rs << endl;
	try
	{
		m_db->executeImmediate(rs.latin1());
	}
	catch(CqlException& ex)
	{
		throw new KexiDBError(0, i18n("Query failed"));
		cerr << ex << endl;
	}

	//FIXME just for now commiting after every query:
//	m_db->executeImmediate("COMMIT WORK;");
	kdDebug() << "CqlDB::query() query executed" << endl;

	kdDebug() << "CqlDB::query(): cursor created" << endl;

	return true;
}

KexiDBRecord*
CqlDB::queryRecord(QString statement, bool buffer)
{
	kdDebug() << "CqlDB::queryRecord()" << endl;
//	query(statement);

	if(!m_db)
		return false;

	QString rs(statement + ";");

//	try
//	{
		CqlRecord *record = new CqlRecord(m_db, rs);
		return record;
//	}
//	catch(KexiDBError *e)
//	{
//		throw e;
//		return 0;
//	}

	kdDebug() << "CqlDB::queryRecord(): record created" << endl;

}

bool
CqlDB::alterField(const QString& table, const QString& field, const QString& newFieldName,
	KexiDBField::ColumnType dtype, int length, int precision, KexiDBField::ColumnConstraints constraints,
	bool binary, bool unsignedType, const QString& defaultVal)
{
}

// cql->kexi

static QString
CqlDB::cqlString(const CqlString &str)
{
	return QString(str.text());
}

static QString
CqlDB::cqlFixedString(const CqlFixedLengthString &str)
{
	return QString(str.text());
}

static QString
CqlDB::errorText(CqlException &ex)
{
	return cqlString(ex.errorText());
}

CqlDB::~CqlDB()
{
}

#include "cqldb.moc"
