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

#ifndef KEXIDBRECORD_H
#define KEXIDBRECORD_H

#include "kexidbfield.h"
#include "kexidbupdaterecord.h"
#include "kdebug.h"
#include <qobject.h>

#define BOOL bool

class QVariant;
class KexiDB;
class KexiDBError;

class KEXIDB_EXPORT KexiDBRecordSet : public QObject
{
	Q_OBJECT

	public:
		/*!
		 *  this class provides a easy function to browse
		 *  between records, update, delete and insert-functions (if there is a primery or unique key)
		 *  it is thougt for things like forms, reports, and table-widgets.
		 *  using KexiDBRecord instead of manual insert/update/delte queries is highly recommanded,
		 *  because the ANSI query hasn't to be translated into database native query!
		 *
		 *  this class will be subclassed, so the queries (caused by updates, inserts and deletes) are database-independend
		 *  @param buffer use true if you have e.g. a form-desingner and don't want to save your own,
		 *  but note, that it will be quite memory-consuming so use it with care
		 */
		KexiDBRecordSet(KexiDB *parent, const char *name);
		virtual ~KexiDBRecordSet() {kdDebug()<<"KexiDBRecord::~KexiDBRecord()"<<endl;};

	signals:
		/* the record is being deleted, after that signals returns, so DON'T store it */
		void recordInserted(KexiDBUpdateRecord*);
	public slots:

//		virtual KexiDBRecord *query(KexiDB *db, QString query) static;

		/*!
		 *  returns true if we either don't have the right to update, delete and insert
		 *  or no key (primary/unique) was found
		 */
		virtual bool readOnly() = 0;

		/*!
		 *  resets all updates to the database-defaults
		 */
		virtual void reset() = 0;

		/*!
		 * executes the sql statments for insert/update operations
                 * returns the first record, which failed to update/insert
		 */
		virtual KexiDBUpdateRecord *writeOut()=0;
		virtual bool writeOut(KexiDBUpdateRecord*)=0;
		/*!
		 *  returns the value of the nth field
		 */
		virtual QVariant value(unsigned int field) = 0;

		/*!
		 *  returns the value of the field "field"
		 */
		virtual QVariant value(QString field) = 0;


		/*!
		 *  returns QVariant::Type of the nth field
		 */
		virtual QVariant::Type type(unsigned int field) = 0;

		/*!
		 *  returns QVariant::Type of the field "field"
		 */
		virtual QVariant::Type type(QString field) = 0;

		/*!
		 *  returns KexiDBField::ColumnType of the nth field
		 */
		virtual KexiDBField::ColumnType sqlType(unsigned int field) = 0;

		/*!
		 *  returns KexiDBField::ColumnType of the field "field"
		 */
		virtual KexiDBField::ColumnType sqlType(QString field) = 0;

		/*!
		 *  returns KexiDBField*
		 */
		virtual KexiDBField* fieldInfo(unsigned int field) = 0;

		/*!
		 *  returns KexiDBField*
		 */
		virtual KexiDBField* fieldInfo(QString field) = 0;


		/*!
		 *  deletes the current record
		 *  the changes have to be commited in order to take effect
		 *  returns true, if delete is possible
		 */
		virtual bool deleteRecord(uint record) = 0;

		/*!
		 * creates a buffer which enables nice updateing with like
		 *  @code KexiDBRecord *insertBuffer = record->insert();
		 *  @code insertBuffer->update("Name", QString("Your name"));
		 *  @code record->commit(true); //commits changes on current record and inserts insertBuffer
		 *
		 *  @returns a record-identification integer
		 * after a writeOut has been done the record is deleted automatically, so don't store the 
		 * pointer. It can be use d for identifying a record during a recordInsertedHandler();
		 */
		virtual KexiDBUpdateRecord *insert(bool wantNotification=false) = 0;


		/*!
                 * creates an updatee record, which can be used during a writeOut
		 * for modifying the current record
		 * if there is no current record 0 should be returned
		 * if uniqueness can't be quaranteed 0 should be returned too
                 */
		virtual KexiDBUpdateRecord *updateCurrent()=0;
		virtual KexiDBUpdateRecord *update(unsigned long record)=0;
		// That's the important one, since it is the only one used at the moment
		virtual KexiDBUpdateRecord *update(QMap<QString,QVariant> fieldNameValueMap)=0;

		/*!
		 *  directly changes to the nth record
		 *  returns 0 if the record "record" doesn't exists
		 */
		virtual void gotoRecord(unsigned long record) = 0;

		/*!
		 *  returns the number of avaible fields
		 */
		virtual unsigned int fieldCount() = 0;

		/*!
		 *  returns the name of field "field"
		 */
		virtual QString fieldName(unsigned int field) = 0;

		/*!
		 *  returns true if there is "yet another record"
		 */
		virtual bool next() = 0;
		virtual bool prev() = 0;

		virtual unsigned long last_id() = 0;
		virtual bool isForignField(uint field) { return false; }

/*!
		 *  the last error which occured. The pointer must neither be stored by the caller nor
		 *  freed by the caller. If there has been no error the pointer to a KexiDBError containing an error code of 0 is returned;
		 */
		void latestError(KexiDBError **error);
	public:
/*!
		 *  the last error which occured. The pointer must neither be stored by the caller nor
		 *  freed by the caller. If there has been no error the pointer to a KexiDBError containing an error code of 0 is returned;
		 */
		virtual KexiDBError *latestError()=0;


		/*! returns the numbers of rows in the result */
		virtual unsigned int	numRows()=0;

		KexiDB *database();
	private:
		KexiDB *m_db;
};

#endif
