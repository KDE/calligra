/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXIPROJECTDATA_H
#define KEXIPROJECTDATA_H

#include <kexidb/connectiondata.h>
#include <kexidb/schemadata.h>

#include <qdatetime.h>

/**
 Kexi project core data member:
 - project name
 - database name
 - connection data
 - data+time of last opening
*/

class KEXICORE_EXPORT KexiProjectData : public QObject, public KexiDB::SchemaData
{
	public:
		typedef QPtrList<KexiProjectData> List;
		
		KexiProjectData();

		KexiProjectData( const KexiDB::ConnectionData &cdata, 
			const QString& dbname = QString::null, const QString& caption = QString::null );
			
		/*! Constructs a copy of \a pdata */
		KexiProjectData( KexiProjectData& pdata );
			
//		const QString& driverName, const QString& databaseName, const QString &hostName, unsigned short int port,
//			 const QString& userName, const QString &password, const QString& fileName);

		~KexiProjectData();
		
		/**
		 * connect to a embedded database
		 */
//		KexiProjectConnectionData(const QString &driverName, const QString &fileName=QString::null);

		KexiDB::ConnectionData* connectionData();

		const KexiDB::ConnectionData* constConnectionData() const;

		//! in fact, this is the same as KexiDB::SchemaData::name()
		QString databaseName() const;
		void setDatabaseName(const QString& dbName);

		inline QDateTime lastOpened() const { return m_lastOpened; }
		void setLastOpened(const QDateTime& lastOpened) { m_lastOpened=lastOpened; }
		inline QString description() const { return m_desc; }
		void setDescription(const QString& desc) { m_desc=desc; }

		//! objects to open on startup (come from command line "-open" option)
		QValueList< QPair<QString,QString> > autoopenObjects;

/*		static const QString &generateTmpName();

		static KexiProjectConnectionData* loadInfo(QDomElement &e);
		void	writeInfo(QDomDocument &doc);

		void	flush(KoStore *store);
		void	provide(KoStore *store);
		void	clean();
		void    setDriverName(const QString &driverName);
 		void    setDatabaseName(const QString &databaseName);
		QString driverName() const;
		QString databaseName() const;*/
	private:
		KexiDB::ConnectionData m_connData;
//		QString  m_driverName;
		
		QDateTime m_lastOpened;
		QString m_desc;
};

#endif
