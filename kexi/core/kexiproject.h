/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIPROJECT_H
#define KEXIPROJECT_H

#include <qobject.h>
#include <qvaluelist.h>

#include "kexiprojectdata.h"
#include <kexidb/object.h>

//class KexiProjectConnectionData;

namespace KexiDB
{
	class DriverManager;
	class Driver;
	class Connection;
}

namespace KexiPart
{
//	class Manager;
	class Item;
	class Info;
}

typedef QValueList<KexiPart::Item> ItemList;

/**
 * this class represents a project it contains data about connections, 
 * current file state etc..
 */
class KexiProject : public QObject, public KexiDB::Object
{
	Q_OBJECT

	public:
		/*! Constructor 1. Creates a new object using \a pdata, which will be owned. */
		KexiProject(KexiProjectData* pdata);
//		/*! Constructor 1. Creates a new object using \a pdata, which will be owned. */
//		KexiProject(KexiDB::ConnectionData *cdata);
		~KexiProject();

		//! Opens existing project using project data.
		bool open();

		//! Creates new, empty project using project data.
		bool create();
		
		/*! Opens project using created connection
		 \return true on success, otherwise false and appropriate error is set. */
//		bool		open(KexiDB::Connection* conn);

		/**
		 * opens a project/xml-connection
		 * @returns true on success
		 */
	//	bool		open(const QString &doc);

		/**
		 * @returns a error wich may have occured at actions like open/openConnection or QString::null if none
		 */
//		const QString		error() { return m_error; }

		/**
		 * @returns the part manager
		 */
//		KexiPart::Manager	*partManager() { return m_partManager; }

		/**
		 * @returns true if a we are connected to a database
		 */
		bool			isConnected();

		/**
		 * @returns all items of a type in this project
		 */
		ItemList		items(KexiPart::Info *i);

		/**
		 * @returns the database connection assosiated with this project
		 */
		KexiDB::Connection	*dbConnection() const { return m_connection; }


	protected:
//		bool			openConnection(KexiProjectConnectionData *connection);

		/*! Creates connection using project data.
		 \return true on success, otherwise false and appropriate error is set. */
		bool createConnection();
		
		void closeConnection();

		void initProject();

		//! reimplementation
		virtual void setError(int code = ERR_OTHER, const QString &msg = QString::null );
		virtual void setError( const QString &msg );
		virtual void setError( KexiDB::Object *obj );

				
	signals:
		/**
		 * this signal gets emmited after succesfully connected to a db
		 */
//		void			dbAvailable();

		/** signal emitted on error */
		void error(const QString &title, KexiDB::Object *obj);

	private:
//		KexiDB::DriverManager		*m_drvManager;
		KexiDB::Connection		*m_connection;
		KexiProjectData* m_data;
//		KexiDB::ConnectionData *m_conn_data_to_use; //!< 
		
		QString m_error_title;
//		KexiProjectConnectionData	*m_connData;
//js		KexiPart::Manager		*m_partManager;
//		QString				m_error;
};

#endif

