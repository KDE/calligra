/* This file is part of the KDE project
   Copyright (C) 2004 Martin Ellis <m.a.ellis@ncl.ac.uk>
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>
 
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "mysqlmigrate.h"

#include <qstring.h>
#include <qregexp.h>
#include <qfile.h>
#include <qvariant.h>
#include <q3valuelist.h>
#include <kdebug.h>

#include <mysql_version.h>
#include <mysql.h>

#include "migration/keximigratedata.h"
#include <kexidb/cursor.h>
#include <kexidb/field.h>
#include <kexidb/utils.h>
#include <kexidb/drivers/mySQL/mysqlconnection_p.cpp>
#include <kexidb/drivermanager.h>
#include <kexiutils/identifier.h>

using namespace KexiMigration;

/* This is the implementation for the MySQL specific import routines. */

KEXIMIGRATE_DRIVER_INFO( MySQLMigrate, mysql )

/* ************************************************************************** */
//! Constructor
/*MySQLMigrate::MySQLMigrate() :
	d(new MySqlConnectionInternal())
{
}*/

//! Constructor (needed for trading interface)
MySQLMigrate::MySQLMigrate(QObject *parent, const QStringList &args) :
	KexiMigrate(parent, args)
	,d(new MySqlConnectionInternal(0))
	,m_mysqlres(0)
{
	KexiDB::DriverManager manager;
	m_kexiDBDriver = manager.driver("mysql");
}

/* ************************************************************************** */
//! Destructor
MySQLMigrate::~MySQLMigrate()
{
	if (m_mysqlres)
		mysql_free_result(m_mysqlres);
	m_mysqlres = 0;
}


/* ************************************************************************** */
/*! Connect to the db backend */
bool MySQLMigrate::drv_connect()
{
	if (!d->db_connect(*m_migrateData->source))
		return false;
	return d->useDatabase(m_migrateData->sourceName);
}


/*! Disconnect from the db backend */
bool MySQLMigrate::drv_disconnect()
{
	return d->db_disconnect();
}


/* ************************************************************************** */
/*! Get the types and properties for each column. */
bool MySQLMigrate::drv_readTableSchema(
	const QString& originalName, KexiDB::TableSchema& tableSchema)
{
//	m_table = new KexiDB::TableSchema(table);

//	//TODO IDEA: ask for user input for captions
//	tableSchema.setCaption(table + " table");

	//Perform a query on the table to get some data
	QString query = QString("SELECT * FROM `") + drv_escapeIdentifier(originalName) + "` LIMIT 0";
	if (!d->executeSQL(query))
	  return false;
	MYSQL_RES *res = mysql_store_result(d->mysql);
	if (!res) {
		kDebug() << "MySQLMigrate::drv_tableNames: null result" << endl;
		return true;
	}
	unsigned int numFlds = mysql_num_fields(res);
	MYSQL_FIELD *fields = mysql_fetch_fields(res); 
	
	for(unsigned int i = 0; i < numFlds; i++) {
		QString fldName(fields[i].name);
		QString fldID( KexiUtils::string2Identifier(fldName) );

		KexiDB::Field *fld = 
			new KexiDB::Field(fldID, type(originalName, &fields[i]));
		
		if(fld->type() == KexiDB::Field::Enum) {
			QStringList values = examineEnumField(originalName, &fields[i]);
		}
		
		fld->setCaption(fldName);
		getConstraints(fields[i].flags, fld);
		getOptions(fields[i].flags, fld);
		tableSchema.addField(fld);
	}
	mysql_free_result(res);
	return true;
}


/*! Get a list of tables and put into the supplied string list */
bool MySQLMigrate::drv_tableNames(QStringList& tableNames)
{
	if (!d->executeSQL("SHOW TABLES"))
	  return false;
	MYSQL_RES *res = mysql_store_result(d->mysql);
	if (!res) {
		kDebug() << "MySQLMigrate::drv_tableNames: null result" << endl;
		return true;
	}
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res)) != NULL) {
		tableNames << QString::fromUtf8(row[0]); //utf8.. ok?
	}
	mysql_free_result(res);
	return true;
}

/*! Fetches single string at column \a columnNumber for each record from result obtained 
 by running \a sqlStatement.
 On success the result is stored in \a stringList and true is returned. 
 \return cancelled if there are no records available. */
tristate MySQLMigrate::drv_queryStringListFromSQL(
	const QString& sqlStatement, uint columnNumber, QStringList& stringList, int numRecords)
{
	stringList.clear();
	if (!d->executeSQL(sqlStatement))
		return false;
	MYSQL_RES *res = mysql_use_result(d->mysql);
	if (!res) {
		kDebug() << "MySQLMigrate::drv_querySingleStringFromSQL(): null result" << endl;
		return true;
	}
	for (int i=0; numRecords == -1 || i < numRecords; i++) {
		MYSQL_ROW row = mysql_fetch_row(res);
		if (!row) {
			tristate r;
			if (mysql_errno(d->mysql))
				r = false;
			else
				r = (numRecords == -1) ? true : cancelled;
			mysql_free_result(res);
			return r;
		}
		uint numFields = mysql_num_fields(res);
		if (columnNumber > (numFields-1)) {
			kWarning() << "MySQLMigrate::drv_querySingleStringFromSQL("<<sqlStatement
				<< "): columnNumber too large (" 
				<< columnNumber << "), expected 0.." << numFields << endl;
			mysql_free_result(res);
			return false;
		}
		unsigned long *lengths = mysql_fetch_lengths(res);
		if (!lengths) {
			mysql_free_result(res);
			return false;
		}
		stringList.append( QString::fromUtf8(row[columnNumber], lengths[columnNumber]) ); //ok? utf8?
	}
	mysql_free_result(res);
	return true;
}

/*! Fetches single record from result obtained 
 by running \a sqlStatement. */
tristate MySQLMigrate::drv_fetchRecordFromSQL(const QString& sqlStatement, 
	KexiDB::RecordData& data, bool &firstRecord)
{
	if (firstRecord || !m_mysqlres) {
		if (m_mysqlres) {
			mysql_free_result(m_mysqlres);
			m_mysqlres = 0;
		}
		if (!d->executeSQL(sqlStatement) || !(m_mysqlres = mysql_use_result(d->mysql)))
			return false;
		firstRecord = false;
	}

	MYSQL_ROW row = mysql_fetch_row(m_mysqlres);
	if (!row) {
		tristate r = cancelled;
		if (mysql_errno(d->mysql))
			r = false;
		mysql_free_result(m_mysqlres);
		m_mysqlres = 0;
		return r;
	}
	const int numFields = mysql_num_fields(m_mysqlres);
	unsigned long *lengths = mysql_fetch_lengths(m_mysqlres);
	if (!lengths) {
		mysql_free_result(m_mysqlres);
		m_mysqlres = 0;
		return false;
	}
	data.resize(numFields);
	for (int i=0; i < numFields; i++)
		data[i] = QString::fromUtf8(row[i], lengths[i] ); //ok? utf8?
	return true;
}

/*! Copy MySQL table to KexiDB database */
bool MySQLMigrate::drv_copyTable(const QString& srcTable, KexiDB::Connection *destConn, 
	KexiDB::TableSchema* dstTable)
{
	if (!d->executeSQL("SELECT * FROM `" + drv_escapeIdentifier(srcTable)) + "`")
		return false;
	MYSQL_RES *res = mysql_use_result(d->mysql);
	if (!res) {
		kDebug() << "MySQLMigrate::drv_copyTable: null result" << endl;
		return true;
	}
	MYSQL_ROW row;
	const KexiDB::QueryColumnInfo::Vector fieldsExpanded( dstTable->query()->fieldsExpanded() );
	while ((row = mysql_fetch_row(res))) {
		const int numFields = qMin((int)fieldsExpanded.count(), (int)mysql_num_fields(res));
		QList<QVariant> vals;
		unsigned long *lengths = mysql_fetch_lengths(res);
		if (!lengths) {
			mysql_free_result(res);
			return false;
		}
		for(int i = 0; i < numFields; i++)
			vals.append( KexiDB::cstringToVariant(row[i], fieldsExpanded.at(i)->field, (int)lengths[i]) );
		if (!destConn->insertRecord(*dstTable, vals)) {
			mysql_free_result(res);
			return false;
		}
		updateProgress();
	}
	if (!row && mysql_errno(d->mysql)) {
		mysql_free_result(res);
		return false;
	}
	/*! @todo Check that wasn't an error, rather than end of result set */
	mysql_free_result(res);
	return true;
}

bool MySQLMigrate::drv_getTableSize(const QString& table, quint64& size)
{
	if (!d->executeSQL("SELECT COUNT(*) FROM `" + drv_escapeIdentifier(table)) + "`")
		return false;
	MYSQL_RES *res = mysql_store_result(d->mysql);
	if (!res) {
		kDebug() << "MySQLMigrate::drv_getTableSize: null result" << endl;
		return true;
	}
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res))) {
		//! @todo check result valid
		size = QString(row[0]).toULongLong();
	}
	mysql_free_result(res);
	return true;
}

//! Convert a MySQL type to a KexiDB type, prompting user if necessary.
KexiDB::Field::Type MySQLMigrate::type(const QString& table,
                                       const MYSQL_FIELD *fld)
{
  // Field type
	KexiDB::Field::Type kexiType = KexiDB::Field::InvalidType;

	switch(fld->type)
	{
  // These are in the same order as mysql_com.h.
	// MySQL names given on the right
		case FIELD_TYPE_DECIMAL:    // DECIMAL or NUMERIC
			break;
		case FIELD_TYPE_TINY:       // TINYINT (-2^7..2^7-1 or 2^8)
			kexiType = KexiDB::Field::Byte;
			break;
		case FIELD_TYPE_SHORT:      // SMALLINT (-2^15..2^15-1 or 2^16)
			kexiType = KexiDB::Field::ShortInteger;
			break;
		case FIELD_TYPE_LONG:       // INTEGER (-2^31..2^31-1 or 2^32)
			kexiType = KexiDB::Field::Integer;
			break;
		case FIELD_TYPE_FLOAT:      // FLOAT
			kexiType = KexiDB::Field::Float;
			break;
		case FIELD_TYPE_DOUBLE:     // DOUBLE or REAL (8 byte)
			kexiType = KexiDB::Field::Double;
			break;
		case FIELD_TYPE_NULL:       // WTF?
			break;
		case FIELD_TYPE_TIMESTAMP:  // TIMESTAMP (promote?)
			kexiType = KexiDB::Field::DateTime;
			break;
		case FIELD_TYPE_LONGLONG:   // BIGINT (-2^63..2^63-1 or 2^64)
		case FIELD_TYPE_INT24:      // MEDIUMINT (-2^23..2^23-1 or 2^24) (promote)
			kexiType = KexiDB::Field::BigInteger;
			break;
		case FIELD_TYPE_DATE:       // DATE
			kexiType = KexiDB::Field::Date;
			break;
		case FIELD_TYPE_TIME:       // TIME
			kexiType = KexiDB::Field::Time;
			break;
		case FIELD_TYPE_DATETIME:   // DATETIME
			kexiType = KexiDB::Field::DateTime;
			break;
		case FIELD_TYPE_YEAR:       // YEAR (promote)
			kexiType = KexiDB::Field::ShortInteger;
			break;
		case FIELD_TYPE_NEWDATE:    // WTF?
		case FIELD_TYPE_ENUM:       // ENUM
			// If MySQL did what it's documentation said it did, we would come here
			// for enum fields ...
			kexiType = KexiDB::Field::Enum;
			break;
		case FIELD_TYPE_SET:        // SET
			//! @todo: Support set column type
			break;
		case FIELD_TYPE_TINY_BLOB:
		case FIELD_TYPE_MEDIUM_BLOB:
		case FIELD_TYPE_LONG_BLOB:
		case FIELD_TYPE_BLOB:       // BLOB or TEXT
		case FIELD_TYPE_VAR_STRING: // VARCHAR
		case FIELD_TYPE_STRING:     // CHAR
			
			if (fld->flags & ENUM_FLAG) {
				// ... instead we come here, using the ENUM_FLAG which is supposed to
				// be deprecated! Duh.
				kexiType = KexiDB::Field::Enum;
				break;
			}
			kexiType = examineBlobField(table, fld);
			break;	
		default:
		  kexiType = KexiDB::Field::InvalidType;
	}

	if (kexiType == KexiDB::Field::InvalidType) {
		return userType(table + '.' + QString::fromUtf8(fld->name));
	}
	return kexiType;
}

//! Distinguish between a BLOB and a TEXT field
/*! MySQL uses the same field type to identify BLOB and TEXT fields.
    This method queries the server to find out if a field is a binary
    field or a text field.  It also considers the length of CHAR and VARCHAR
    fields to see whether Text or LongText is the appropriate Kexi field type.
    Assumes fld is a CHAR, VARCHAR, one of the BLOBs or TEXTs.
    \return KexiDB::Field::Text, KexiDB::Field::LongText or KexiDB::Field::BLOB
*/
KexiDB::Field::Type MySQLMigrate::examineBlobField(const QString& table,
    const MYSQL_FIELD* fld)
{
	QString mysqlType;
	const QString query( "SHOW COLUMNS FROM `" + drv_escapeIdentifier(table) + 
	                "` LIKE '" + QString::fromLatin1(fld->name) + "'");

	if (!d->executeSQL(query)) {
		// Huh? MySQL wont tell us what kind of field it is! Lets guess.
	  return KexiDB::Field::LongText;
	}
	MYSQL_RES *res = mysql_store_result(d->mysql);
	if (!res) {
		kDebug() << "MySQLMigrate::examineBlobField: null result" << endl;
	}
	else {
		MYSQL_ROW row;
		if ((row = mysql_fetch_row(res)))
			mysqlType = QString(row[1]);
		mysql_free_result(res);
	}

	kDebug() << "MySQLMigrate::examineBlobField: considering "
	          << mysqlType << endl;
	if (mysqlType.contains("blob", Qt::CaseInsensitive)) {
		// Doesn't matter how big it is, it's binary
		return KexiDB::Field::BLOB;
	}
	else if (fld->length < 200) {
		return KexiDB::Field::Text;
	}
	return KexiDB::Field::LongText;
}

//! Get the strings that identify values in an enum field
/*! Parse the type of a MySQL enum field as returned by the server in a 
    'DESCRIBE table' or 'SHOW COLUMNS FROM table' statement.  The string
    returned by the server is in the form 'enum('option1','option2').
    In this example, the result should be a string list containing two
    strings, "option1", "option2".
    \return list of possible values the field can take
 */
QStringList MySQLMigrate::examineEnumField(const QString& table,
		const MYSQL_FIELD* fld)
{
	QString vals;
	const QString query("SHOW COLUMNS FROM `" + drv_escapeIdentifier(table) + 
			"` LIKE '" + QString::fromLatin1(fld->name) + "'");

	if (!d->executeSQL(query))
		// Huh? MySQL wont tell us what values it can take.
		return QStringList();
	
	MYSQL_RES *res = mysql_store_result(d->mysql);

	if (!res) {
		kDebug() << "MySQLMigrate::examineEnumField: null result" << endl;
	}
	else {
		MYSQL_ROW row;
		if ((row = mysql_fetch_row(res)))
			vals = QString(row[1]);
		mysql_free_result(res);
	}

	kDebug() << "MySQLMigrate::examineEnumField: considering " 
						<< vals << endl;
	
	// Crash and burn if we get confused...
	if (!vals.startsWith("enum(")) {
		// Huh? We're supposed to be parsing an enum!
		kDebug() << "MySQLMigrate::examineEnumField:1 not an enum!" << endl;
		return QStringList();
	}
	if (!vals.endsWith(")")) {
		kDebug() << "MySQLMigrate::examineEnumField:2 not an enum!" << endl;
		return QStringList();
	}
	
	// It'd be nice to use QString.section or QStringList.split, but we need
	// to be careful as enum values can have commas and quote marks in them
	// e.g. CREATE TABLE t(f enum('option,''') gives one option: "option,'"
	vals = vals.remove(0,5);
	QRegExp rx = QRegExp("^'((?:[^,']|,|'')*)'");
	QStringList values = QStringList();
	int index = 0;

	while ((index = rx.indexIn(vals, index, QRegExp::CaretAtOffset)) != -1) {
		int len = rx.matchedLength();
		if (len != -1) {
			kDebug() << "MySQLMigrate::examineEnumField:3 " << rx.cap(1) << endl;
			values << rx.cap(1);
		} else {
			kDebug() << "MySQLMigrate::examineEnumField:4 lost" << endl;
		}
		
		QChar next = vals[index + len];
		if (next != QChar(',') && next != QChar(')')) {
			kDebug() << "MySQLMigrate::examineEnumField:5 " << next << endl;
		}
		index += len + 1;
	}

	return values;
}

void MySQLMigrate::getConstraints(int flags, KexiDB::Field* fld)
{
	fld->setPrimaryKey(flags & PRI_KEY_FLAG);
	fld->setAutoIncrement(flags & AUTO_INCREMENT_FLAG);
	fld->setNotNull(flags & NOT_NULL_FLAG);
	fld->setUniqueKey(flags & UNIQUE_KEY_FLAG);
	//! @todo: Keys and uniqueness
}

void MySQLMigrate::getOptions(int flags, KexiDB::Field* fld)
{
	fld->setUnsigned(flags & UNSIGNED_FLAG);
}


#include "mysqlmigrate.moc"
