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

#ifndef KEXIDBRECORD_H
#define KEXIDBRECORD_H

#include "kexidbfield.h"

class QVariant;

class KexiDBResult;

class KexiDBRecord
{
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
		KexiDBRecord() {};
		virtual ~KexiDBRecord() {};

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
		 *  commits updates
		 *  use commit(true) if you wan't to commit the insert buffers too
		 *  returns true if the commit succeeded
		 *
		 *  note: if you use commit on a insert-buffer, it will automaticaly get a stand-alone buffer
		 *  and commit on the base-buffer won't work for the insertBuffer anymore!
		 *
		 */
		virtual bool commit(bool insertBuffer=false) = 0;


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
		 *  sets the nth field in the buffer to "value"
		 *  the changes have to be commited in order to take effect
		 *  returns true, if update is possible
		 */
		virtual bool update(unsigned int field, QVariant value) = 0;

		/*!
		 *  sets the field "field" in the buffer to "value"
		 *  the changes have to be commited in order to take effect
		 *  returns true, if update is possible
		 */
		virtual bool update(QString field, QVariant value) = 0;

		/*! 
		 *  deletes the current record
		 *  the changes have to be commited in order to take effect
		 *  returns true, if delete is possible
		 */
		virtual bool deleteRecord() = 0;

		/*!
		 * creates a buffer which enables nice updateing with like
		 *  @code KexiDBRecord *insertBuffer = record->insert();
		 *  @code insertBuffer->update("Name", QString("Your name"));
		 *  @code record->commit(true); //commits changes on current record and inserts insertBuffer
		 *
		 *  returns 0, if insert isn't possible
		 */
		virtual KexiDBRecord *insert() = 0;

		/*!
		 *  directly changes to the nth record
		 *  returns 0 if the record "record" doesn't exists
		 */
		virtual void gotoRecord(unsigned int record) = 0;

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
		
};

#endif
