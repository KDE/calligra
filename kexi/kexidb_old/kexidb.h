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

#ifndef KEXIDB_H
#define KEXIDB_H

#include <qobject.h>
#include <qptrlist.h>

#include "kexidbresult.h"
#include "kexidbfield.h"

class KexiDBInterfaceManager;
class KexiDBDriver;
class KexiDBRecord;

typedef QPtrList<KexiDBField> KexiDBTableStruct;

class KexiDB : public QObject
{
	Q_OBJECT

	public:
		KexiDB(QObject *parent=0, const char *name=0);
		~KexiDB();

		KexiDB *add(const QString &driver);
		KexiDBDriver *driverInfo(const QString &driver);

		QStringList getDrivers();

		//now driver related functions

		virtual QString driverName();

		virtual KexiDBRecord* queryRecord(QString query, bool buffer=false);

		/*! connect to database hope that is ansi-compatible */
		virtual bool connect(QString host, QString user, QString password, QString socket, QString port);
		virtual bool connect(QString host, QString user, QString password, QString socket, QString port,
			QString db, bool create = false);
		/*! connect method for file-based databases*/
		virtual bool load(QString file);

		/*! hope thats ansi-compatilbe too */
		virtual QStringList databases();
		virtual QStringList tables();

		virtual bool query(QString statement);
		virtual QString escape(const QString &str);
		virtual QString escape(const QByteArray& str);
		virtual bool alterField(const QString& table, const QString& field, const QString& newFieldName,
			KexiDBField::ColumnType dtype, int length, int precision, KexiDBField::ColumnConstraints constraints,
			bool binary, bool unsignedType, const QString& defaultVal);
		virtual bool createField(const QString& table, const QString& field, KexiDBField::ColumnType dtype,
			int length, int precision, KexiDBField::ColumnConstraints constraints, bool binary, bool unsignedType,
			const QString& defaultVal);

		virtual KexiDBResult		*getResult();

		virtual KexiDBTableStruct	getStructure(const QString& table);

		QStringList			getColumns(const QString& table);

		virtual unsigned long		affectedRows();

		void appendManager(KexiDBInterfaceManager *m);


	protected:
		KexiDBInterfaceManager	*manager();

		KexiDBInterfaceManager	*m_manager;
		KexiDBDriver		*m_currentDriver;
};

#endif
