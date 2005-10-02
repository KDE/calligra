/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIPROJECTDATA_H
#define KEXIPROJECTDATA_H

#include <kexidb/connectiondata.h>
#include <kexidb/schemadata.h>

#include <qdatetime.h>

class KexiProjectDataPrivate;

/**
 Kexi project core data member:
 - project name
 - database name
 - connection data
 - date and time of last opening
*/
class KEXICORE_EXPORT KexiProjectData : public QObject, public KexiDB::SchemaData
{
	public:
		typedef QPtrList<KexiProjectData> List;
		typedef QMap<QCString,QString> ObjectInfo;
		
		KexiProjectData();

		KexiProjectData( const KexiDB::ConnectionData &cdata, 
			const QString& dbname = QString::null, const QString& caption = QString::null );
			
		/*! Constructs a copy of \a pdata */
		KexiProjectData( const KexiProjectData& pdata );
			
//		const QString& driverName, const QString& databaseName, const QString &hostName, unsigned short int port,
//			 const QString& userName, const QString &password, const QString& fileName);

		~KexiProjectData();

		KexiProjectData& operator=(const KexiProjectData& pdata);

		/*! \return true if there is final mode set in internal 
		 project settings. */
		bool finalMode() const;
		
		KexiDB::ConnectionData* connectionData();

		const KexiDB::ConnectionData* constConnectionData() const;

		//! in fact, this is the same as KexiDB::SchemaData::name()
		QString databaseName() const;
		void setDatabaseName(const QString& dbName);

		QDateTime lastOpened() const;
		void setLastOpened(const QDateTime& lastOpened);
		QString description() const;
		void setDescription(const QString& desc);

		/*! objects to open on startup (come from command line "-open" option)
		 It's public for convenience */
		QValueList<ObjectInfo> autoopenObjects;

	private:
		KexiProjectDataPrivate *d;
};

#endif
