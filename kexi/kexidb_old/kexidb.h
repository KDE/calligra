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

	//! db engine type
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

	//These two should be const but the reality is that
	//most current drivers make those a mutators
	virtual QStringList       databases();
	virtual QStringList       tableNames()=0;
	//! Returns true if database named by dbName is considered as system database
	virtual bool isSystemDatabase(QString &dbName)=0;

	virtual QString           driverName() const;
	virtual DBType            dbType() const;
	virtual KexiDBTableStruct	structure(const QString& table) const;
	virtual QString	          nativeDataType(const KexiDBField::ColumnType& t) const=0;
	virtual unsigned long		  affectedRows() const;
	virtual KexiDBWatcher*    watcher() const;
	virtual const KexiDBTable *const table(const QString&)=0;
	QStringList			          columns(const QString& table) const;
	RelationList			        relations() const;
	/**
	 * returns the selected encoding
	 */
	Encoding encoding() { return m_encoding; }

public slots:
	virtual KexiDBRecordSet* queryRecord(const QString& query, bool buffer=false) =0;

	/*! connect to database hope that is ansi-compatible */
	virtual bool connect(const QString& host, const QString& user, const QString& password,
											 const QString& socket, const QString& port) =0;
	virtual bool connect(const QString& host, const QString& user, const QString& password,
											 const QString& socket, const QString& port,
											 const QString& db, bool create = false) =0;
	/*! connect method for file-based databases*/
	virtual bool load(const QString& file, bool persistant=false);

	virtual bool query(const QString& statement)  =0;
	virtual QString escape(const QString &str)    =0;
	virtual QString escape(const QByteArray& str) =0;
	virtual bool alterField(const QString& table, const QString& field, const QString& newFieldName,
													KexiDBField::ColumnType dtype, int length, int precision, KexiDBField::ColumnConstraints constraints,
													bool binary, bool unsignedType, const QString& defaultVal) =0;
	virtual bool createField(const QString& table, const QString& field, KexiDBField::ColumnType dtype,
													 int length, int precision, KexiDBField::ColumnConstraints constraints, bool binary, bool unsignedType,
													 const QString& defaultVal) =0;
	virtual bool alterField(const KexiDBField& changedField,
													unsigned int index, KexiDBTableStruct fields) = 0;
	virtual bool createField(const KexiDBField& newField,
													 KexiDBTableStruct fields, bool createTable = false) = 0;
	/* the createTable method should be overloaded to get better performance */
	virtual bool createTable(const KexiDBTable& tableDef);


	virtual bool commitWork() { return false; }

	void setRelations(const RelationList& r) { m_relations = r; }

	/*!
	 *  the last error which occured. The pointer must neither be stored by the caller nor
	 *  freed by the caller. If there has been no error the pointer to a KexiDBError containing an error code of 0 is returned;
	 */
	void latestError(KexiDBError **error);

	/**
	 * forces the db to read out in a special encoding
	 */
	virtual void setEncoding(Encoding enc) { m_encoding = enc; }



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
	class KexiDBPrivate;
	KexiDBPrivate *d;
};

#endif
