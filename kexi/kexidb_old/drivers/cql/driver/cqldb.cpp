/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
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
}

QString
CqlDB::driverName()
{
	return QString("cql");
}

KexiDB::DBType
CqlDB::dbType()
{
	return LocalDirectoryDB;
}

bool
CqlDB::load(QString file, bool)
{
	m_error.clear();
	char *dir = (char * )file.latin1();
	kdDebug() << "CqlDB::load()" << endl;
	try
	{
		kdDebug() << "CqlDB::load() directory set to:" << dir << endl;
		m_db = new SqlHandle(0, 0, dir);
	}
	catch(CqlException& ex)
	{
		cerr << ex << endl;
		m_db = 0;
	}

	kdDebug() << "CqlDB::load(): proceeding" << endl;

	try
	{
		m_db->optionCurrentDirectory(dir);
		m_db->connect("PUBLIC");
	}
	catch(CqlException& ex)
	{
		cerr << ex << endl;
		m_error.setup(1,i18n("Connection failed"));
		return false;
		//throw new KexiDBError(0, i18n("Connection failed"));
	}
	return true;
}

QStringList
CqlDB::tables()
{
	QStringList tables;
	TableIdList *tlist = m_db->getTableIds("PUBLIC");
	for(TableId *table = tlist->first(); table; table = tlist->next())
	{
		tables.append(cqlString(table->tableName()).lower().stripWhiteSpace());
	}

	return tables;
}

bool
CqlDB::query(QString statement)
{
	m_error.clear();
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
		m_error.setup(1,i18n("Query failed"));
//		throw new KexiDBError(0, i18n("Query failed"));
		return false;
		cerr << ex << endl;
	}

	//FIXME just for now commiting after every query:
//	m_db->executeImmediate("COMMIT WORK;");
	kdDebug() << "CqlDB::query() query executed" << endl;

	kdDebug() << "CqlDB::query(): cursor created" << endl;

	return true;
}

bool  CqlDB::commitWork() {
	try
	{
		m_db->executeImmediate("COMMIT WORK");
	}
	catch(CqlException& ex)
	{
		m_error.setup(2,i18n("Commit Workk failed"));
//		throw new KexiDBError(0, i18n("Query failed"));
		return false;
		cerr << ex << endl;
	}
}

KexiDBRecordSet*
CqlDB::queryRecord(QString statement, bool buffer)
{
	kdDebug() << "CqlDB::queryRecord()" << endl;
//	query(statement);

	if(!m_db)
		return false;

	QString rs(statement + ";");

//	try
//	{
		CqlRecord *record = new CqlRecord(this, "rec", m_db, rs);
		if (record->latestError()->kexiErrnoFunction()!=0)
		{
			m_error = *record->latestError();
			delete record;
			return 0;
		}
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

QString
CqlDB::cqlString(const CqlString &str)
{
	return QString(str.text());
}

QString
CqlDB::cqlFixedString(const CqlFixedLengthString &str)
{
	return QString(str.text());
}

QString
CqlDB::errorText(CqlException &ex)
{
	return cqlString(ex.errorText());
}

KexiDBField::ColumnType
CqlDB::getInternalDataType(int t)
{
	switch(t)
	{
		case CqlConstants::CQL_LONGVARCHAR:
		case CqlConstants::CQL_CHAR:
		case CqlConstants::CQL_CHARUC:
		case CqlConstants::CQL_VARCHAR:
			return KexiDBField::SQLVarchar;

		case CqlConstants::CQL_INTEGER:
			return KexiDBField::SQLInteger;

		case CqlConstants::CQL_BIGINT:
			return KexiDBField::SQLBigInt;

		case CqlConstants::CQL_LONGVARBINARY:
		case CqlConstants::CQL_VARBINARY:
			return KexiDBField::SQLBoolean;

		case CqlConstants::CQL_DATE:
			return KexiDBField::SQLDate;

		case CqlConstants::CQL_REAL:
		case CqlConstants::CQL_DECIMAL:
			return KexiDBField::SQLDecimal;

		case CqlConstants::CQL_FLOAT:
			return KexiDBField::SQLFloat;

		case CqlConstants::CQL_SMALLINT:
			return KexiDBField::SQLSmallInt;

		case CqlConstants::CQL_TIME:
		case CqlConstants::CQL_TIMESTAMP:
		case CqlConstants::CQL_TINYINT:

		case CqlConstants::CQL_COLUMN_UNDEFINED_TYPE:
			return KexiDBField::SQLInvalid;

		case CqlConstants::CQL_BINARY:
		case CqlConstants::CQL_BIT:
//			return SqlBlob;
			return KexiDBField::SQLVarchar;

		default:
			return KexiDBField::SQLVarchar;
	}
}


QString
CqlDB::getNativeDataType(const KexiDBField::ColumnType& t)
{
	switch(t)
	{
		case KexiDBField::SQLLongVarchar:
			return "LONGVARCHAR";
		case KexiDBField::SQLVarchar:
			return "VARCHAR";
		case KexiDBField::SQLInteger:
			return "INTEGER";
		case KexiDBField::SQLSmallInt:
			return "SMALLINT";
		case KexiDBField::SQLTinyInt:
			return "TINYINT";
		case KexiDBField::SQLNumeric:
			return "NUMERIC";
		case KexiDBField::SQLDouble:
			return "DOUBLE";
		case KexiDBField::SQLBigInt:
			return "BIGINT";
		case KexiDBField::SQLDecimal:
			return "DECIMAL";
		case KexiDBField::SQLFloat:
			return "FLOAT";
		case KexiDBField::SQLBinary:
			return "BINARY";
		case KexiDBField::SQLLongVarBinary:
			return "BINARY";
		case KexiDBField::SQLVarBinary:
			return "VARBINARY";
		case KexiDBField::SQLDate:
			return "DATE";
		case KexiDBField::SQLTime:
			return "TIME";
		case KexiDBField::SQLTimeStamp:
			return "TIMESTAMP";
		case KexiDBField::SQLBoolean:
			return "UNDEFINED";
		case KexiDBField::SQLInterval:
			return "UNDEFINED";
		case KexiDBField::SQLInvalid:
		case KexiDBField::SQLLastType:
			return QString::null;
	}

	return QString::null;
}


bool CqlDB::createTable(const KexiDBTable& tableDef) {

	if (tableDef.tableName().isEmpty()) return false;
	if (tableDef.count()==0) return false;

	QString qstr = "CREATE TABLE " + tableDef.tableName() + " (";
	bool Comma=false;
	for (KexiDBTable::const_iterator it=tableDef.begin();
		it!=tableDef.end();++it) {

		if (Comma) qstr +=',';

		qstr +=(*it).name()+" ";
		qstr += " " + createDefinition(*it);
		Comma=true;
	}
	qstr += ")";

	kdDebug()<<"CqlDB::createTable "<<qstr<<endl;
	try
	{
		m_db->executeImmediate(qstr.latin1());
	}
	catch(CqlException& ex)
	{
		m_error.setup(1,i18n("Table could not be created"));
//		throw new KexiDBError(0, i18n("Query failed"));
		cerr << ex << endl;
		return false;
//
	}
	return true;
}


QString
CqlDB::createDefinition(const KexiDBField& field)
{
	QString qstr = getNativeDataType(field.sqlType());
	bool allowUnsigned = false;

	switch(field.sqlType())
	{
		case KexiDBField::SQLInteger:
		case KexiDBField::SQLSmallInt:
		case KexiDBField::SQLTinyInt:
		case KexiDBField::SQLBigInt:
			allowUnsigned = true;
			break;
		case KexiDBField::SQLVarchar:
			qstr += "(" + QString::number(field.length()) + ")";
			break;
		case KexiDBField::SQLDecimal:
		case KexiDBField::SQLFloat:
		case KexiDBField::SQLDouble:
		case KexiDBField::SQLNumeric:
			allowUnsigned = true;
			qstr += "(" + QString::number(field.length()) + "," + QString::number(field.precision()) + ")";
			break;
		case KexiDBField::SQLInvalid:
		case KexiDBField::SQLBinary:
		case KexiDBField::SQLBoolean:
		case KexiDBField::SQLDate:
		case KexiDBField::SQLLongVarBinary:
		case KexiDBField::SQLTime:
		case KexiDBField::SQLTimeStamp:
		case KexiDBField::SQLVarBinary:
		case KexiDBField::SQLInterval:
		case KexiDBField::SQLLongVarchar:
		case KexiDBField::SQLLastType:
			break;
	}

	if((field.constraints() & KexiDBField::CCNotNull) || field.primary_key())
	{
		qstr += " NOT NULL";
	}
	else
	{
		qstr += " ";
	}

	if (field.primary_key())
		qstr+" PRIMARY KEY";

	if(field.binary() && (field.sqlType() == KexiDBField::SQLVarchar))
	{
		qstr += " BINARY";
	}

	if(field.unsignedType() && allowUnsigned)
	{
		qstr += " UNSIGNED";
	}

	if(!field.defaultValue().toString().isEmpty())
	{
		qstr += " DEFAULT " + field.defaultValue().toString();
	}

	if(field.constraints() & KexiDBField::CCAutoInc)
	{
		qstr += " AUTO_INCREMENT";
	}

	return qstr;
}

KexiDBError *CqlDB::latestError()
{
	return &m_error;
}

CqlDB::~CqlDB()
{
}

#include "cqldb.moc"
