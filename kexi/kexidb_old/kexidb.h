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

#ifndef KEXIDB_H
#define KEXIDB_H

#include <qobject.h>
#include <qptrlist.h>


#include "kexidbfield.h"
#include "kexidbtable.h"
#include "kexidbwatcher.h"

class KexiDBInterfaceManager;
class KexiDBDriver;
class KexiDBRecordSet;
class KexiDBError;

typedef struct SourceConnection
{
        QString srcTable;
        QString rcvTable;
        QString srcField;
        QString rcvField;
} SourceConnection;

typedef QValueList<SourceConnection> RelationList;
typedef QPtrList<KexiDBField> KexiDBTableStruct;

class KEXIDB_EXPORT KexiDB : public QObject
{
	Q_OBJECT

	public:

		enum DBType
		{
			NoDB,
			RemoteDB,
			LocalDirectoryDB,
			LocalFileDB
		};

		enum Encoding
		{
			Latin1,
			Utf8,
			Ascii,
			Local8Bit
		};

		KexiDB(QObject *parent=0, const char *name=0);
		~KexiDB();


	public slots:

		//now driver related functions

		virtual QString driverName();
		virtual DBType dbType() { return NoDB; }

		virtual KexiDBRecordSet* queryRecord(QString query, bool buffer=false);

		/*! connect to database hope that is ansi-compatible */
		virtual bool connect(QString host, QString user, QString password, QString socket, QString port);
		virtual bool connect(QString host, QString user, QString password, QString socket, QString port,
			QString db, bool create = false);
		/*! connect method for file-based databases*/
		virtual bool load(QString file, bool persistant=false);

		/*! hope thats ansi-compatilbe too */
		virtual QStringList databases();
		virtual QStringList tableNames()=0;
		virtual const KexiDBTable *const table(const QString&)=0;

		virtual bool query(QString statement);
		virtual QString escape(const QString &str);
		virtual QString escape(const QByteArray& str);
		virtual bool alterField(const QString& table, const QString& field, const QString& newFieldName,
			KexiDBField::ColumnType dtype, int length, int precision, KexiDBField::ColumnConstraints constraints,
			bool binary, bool unsignedType, const QString& defaultVal);
		virtual bool createField(const QString& table, const QString& field, KexiDBField::ColumnType dtype,
			int length, int precision, KexiDBField::ColumnConstraints constraints, bool binary, bool unsignedType,
			const QString& defaultVal);
		virtual bool alterField(const KexiDBField& changedField,
			unsigned int index, KexiDBTableStruct fields);
		virtual bool createField(const KexiDBField& newField,
			KexiDBTableStruct fields, bool createTable = false);
		/* the createTable method should be overloaded to get betterperformance */
		virtual bool createTable(const KexiDBTable& tableDef);

		virtual KexiDBTableStruct	getStructure(const QString& table);
		virtual QString	getNativeDataType(const KexiDBField::ColumnType& t);

		QStringList			getColumns(const QString& table);

		virtual unsigned long		affectedRows();
		virtual KexiDBWatcher		*watcher() { return m_dbwatcher; }

		virtual bool commitWork() { return false; }

		void setRelations(RelationList r) { m_relations = r; }
		RelationList			relations() { return m_relations; }

//js:		KexiDBDriver		*m_currentDriver;
		uint us(int i) { return i; }
				/*!
		 *  the last error which occured. The pointer must neither be stored by the caller nor
		 *  freed by the caller. If there has been no error the pointer to a KexiDBError containing an error code of 0 is returned;
		 */
		void latestError(KexiDBError **error);

		/**
		 * forces the db to read out in a special encoding
		 */
		virtual void setEncoding(Encoding enc) { m_encoding = enc; }

		/**
		 * returns the selected encoding
		 */
		Encoding encoding() { return m_encoding; }

		const QString decode(const char *);
		const char *encode(const QString &);

	public:
		/*!
		 *  the last error which occured. The pointer must neither be stored by the caller nor
		 *  freed by the caller. If there has been no error the pointer to a KexiDBError containing an error code of 0 is returned;
		 */
		virtual KexiDBError *latestError()=0;


	protected:

		KexiDBInterfaceManager	*m_manager;
		KexiDBWatcher		*m_dbwatcher;
		RelationList		m_relations;
		Encoding		m_encoding;
};

#endif
