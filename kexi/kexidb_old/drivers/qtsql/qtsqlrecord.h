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

#ifndef QTSQLRECORD_H
#define QTSQLRECORD_H

#include <qvariant.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qvaluevector.h>
#include <qvaluelist.h>
#include <qobject.h>

// the qt db includes
#include <qsqlresult.h>
#include <qsqlrecord.h>
#include <qsqlcursor.h>
#include <qsqldatabase.h>

//#include <mysql/mysql.h>

#include "qtsqlresult.h"

#include <kexidbrecordset.h>
#include <kexidbfield.h>

class KexiDB;
class QtSqlRecord;
class QtSqlResult;

typedef struct UpdateItem
{
	unsigned int record;
	QString field;
	QVariant value;
	bool done;
};

typedef QValueList<int> InsertList;
typedef QValueList<UpdateItem> UpdateBuffer;
typedef QMap<QString , QVariant> ContentBuffer;
typedef QValueVector<QString> FieldName;
typedef QMap<uint, QVariant> KeyBuffer;

class QtSqlRecord : public KexiDBRecordSet //, public QtSqlResult
{
	Q_OBJECT

	public:
		QtSqlRecord();
		QtSqlRecord(QSqlDatabase *realdb, KexiDB *db, const char *name, const QString& query);
		QtSqlRecord(/*MYSQL_RES*/void *result, QtSqlDB *db, const char *name, bool buffer, QtSqlRecord *parent=0);
		virtual ~QtSqlRecord();

		//KexiDBRecord members
//		QtSqlRecord *query(KexiDB *db, QString query) static;
////
		KexiDBUpdateRecord *writeOut();
		bool writeOut(KexiDBUpdateRecord*);
		KexiDBUpdateRecord *insert(bool wantNotification=false);
		KexiDBUpdateRecord *updateCurrent();
		KexiDBUpdateRecord *update(unsigned long record);
		KexiDBUpdateRecord *update(QMap<QString,QVariant> fieldNameValueMap);

////
		bool readOnly();

		void reset();
		bool commit(unsigned int record, bool insertBuffer);

		QVariant value(unsigned int field);
		QVariant value(QString field);

		QVariant::Type type(unsigned int field);
		QVariant::Type type(QString field);

		KexiDBField::ColumnType sqlType(unsigned int field);
		KexiDBField::ColumnType sqlType(QString field);

		KexiDBField* fieldInfo(unsigned int column);
		KexiDBField* fieldInfo(QString column);

		bool update(unsigned int record, unsigned int field, QVariant value);
		bool update(unsigned int record, QString field, QVariant value);

		bool deleteRecord(uint record);

		int insert();

		QtSqlRecord *operator++();
		QtSqlRecord *operator--();

		void gotoRecord(unsigned long record);

		unsigned int fieldCount();

		QString fieldName(unsigned int field);

		bool next();
		bool prev();

		bool isForignField(uint field);

		unsigned long last_id();

		//needed members
		void takeInsertBuffer(QtSqlRecord *buffer);

		virtual KexiDBError *latestError();

		virtual unsigned int	numRows();

	protected:
		bool		findKey(); /* finds the key for updateing */
		QString		forignUpdate(const QString &field, const QString &value, const QString &key, bool n);

		QString		m_queryString;
		QSqlQuery	*m_query;
		QSqlDatabase	*m_db;
//		QtSqlDB		*m_db;
//		QSqlCursor	m_record;
		QSqlRecord	m_record;

		QtSqlRecord	*m_parent; /* parent if current is a insert-buffer */

		QString		m_keyField; /* fieldname which contains primary/unique key */
//		QVariant	m_keyContent; /* the key is buffered so requeries are possible */
		QString		m_table;    /* table for update table ... */
		KeyBuffer	m_keyBuffer; /* each record (key) holds a key-value */
		bool		m_readOnly; /* is that record read only? */

		FieldName	m_fieldNames; /* field name vector */

		InsertList	m_insertList;   /* insert-buffers, don't wan't to loose them */
		UpdateBuffer	m_updateBuffer; /* update buffer */
		ContentBuffer	m_contentBuffer; /* interesting if buffer was enabled */

		int		m_lastItem;

		unsigned long	m_lastID;
		KexiDBError	m_error;
};

#endif
