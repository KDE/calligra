/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDB_DRIVER_H
#define KEXIDB_DRIVER_H

#include <qobject.h>
#include <qptrlist.h>
#include <qptrdict.h>
#include <qstringlist.h>
#include <qvaluevector.h>

#include <kexidb/object.h>

class KService;

namespace KexiDB {

class Connection;
class ConnectionData;
class ConnectionInternal;
class DriverManager;
class DriverBehaviour;
class DriverPrivate;

/*! This class is a prototype of the database driver.
 Driver allows create new connections and groups these as a parent.
 Before destrucion, connections are destructed.

*/
class KEXI_DB_EXPORT Driver : public QObject, public KexiDB::Object
{
	Q_OBJECT
	public:
		/*! Helpful for retrieving info about driver from using 
		 KexiDB::DriverManager::driversInfo() without loading driver libraries. */
		typedef struct Info {
			QString name, caption, comment, fileDBMimeType;
			bool fileBased;
		};
		typedef QMap<QString,Info> InfoMap;
		
		/*! Features supported by driver (sum of few Features enum items). */
		enum Features {
			NoFeatures = 0,
			//! if single trasactions are only supported
			SingleTransactions = 1,   
			//! if multiple concurent trasactions are supported
			//! (this implies !SingleTransactions)
			MultipleTransactions = 2, 
//(js) NOT YET IN USE:
			//! if nested trasactions are supported
			//! (this implies !SingleTransactions 
			//! and MultipleTransactions)
			NestedTransactions = (MultipleTransactions+4),
			//! if forward moving is supported for cursors
			//! (if not available, no cursors available at all)
			CursorForward = 8, 
			//! if backward moving is supported for cursors
			//! (this implies CursorForward)
			CursorBackward = (CursorForward+16)
		};
		
		virtual ~Driver();

//		typedef QPtrList<Connection> ConnectionsList;

		/*! Creates connection using \a conn_data as parameters. 
		 \return 0 and sets error message on error.
		 driverName member of \a conn_data will be updated with this driver name.
		 */
		Connection *createConnection( ConnectionData &conn_data );

		/*! \return list of created connections. */
		const QPtrList<Connection> connectionsList() const;

//		/*! \return a name equal to the service name (X-Kexi-DriverName) 
//		 stored in given service .desktop file. */
//		QString driverName() { return m_driverName; }

		/*! \return a name of MIME type of files handled by this driver 
		 if it is a file-based database's driver 
		 (equal X-Kexi-FileDBDriverMime service property)
		 otherwise returns null string. \sa isFileDriver()
		*/
		QString fileDBDriverMime() const { return m_fileDBDriverMime; }

		/*! Info about the driver as a service. */
		const KService* service() const { return m_service; }

		/*! \return true if this driver is file-based */
		bool isFileDriver() const { return m_isFileDriver; }

		/*! \return true if \a n is a system object's name, 
		 eg. name of build-in system table that cannot be used or created by a user,
		 and in most cases user even shouldn't see this. The list is specific for 
		 a given driver implementation. By default returns true if \a n starts with "kexi__".
		 Note for driver developers: Also call Driver::isSystemObjectName()
		 from your reimplementation.
		 \sa isSystemFieldName().
		*/
		virtual bool isSystemObjectName( const QString& n ) const;

		/*! \return true if \a n is a system database's name, 
		 eg. name of build-in, system database that cannot be used or created by a user,
		 and in most cases user even shouldn't see this. The list is specific for 
		 a given driver implementation. For implemenation.
		 \sa isSystemObjectName().
		*/
		virtual bool isSystemDatabaseName( const QString& n ) const = 0;

		/*! \return true if \a n is a system field's name, build-in system 
		 field that cannot be used or created by a user,
		 and in most cases user even shouldn't see this. The list is specific for 
		 a given driver implementation. For implemenation.
		 \sa isSystemObjectName().
		*/
		virtual bool isSystemFieldName( const QString& n ) const = 0;

		//! \return driver's features that are combination of Driver::Features enum.
		int features() const { return m_features; }

		//! \return true if transaction are supported (single or multiple)
		bool transactionsSupported() const { return m_features & (SingleTransactions | MultipleTransactions); }
		
		/*! SQL-implementation-dependent name of given type */
		QString sqlTypeName(int id_t) const { return m_typeNames[id_t]; }

		/*! used when we do not have Driver instance yet */
		static QString defaultSQLTypeName(int id_t);
		
		/*! \return true if this driver's implementation is valid. 
		 Just few constriants are checked to ensure that driver 
		 developer didn't forget about something. 
		 This method is called automatically on createConnection(),
		 and proper error message is set properly on any error. */
		virtual bool isValid();
	
		//! driver's static version information, it is automatically implemented
		//! in implementation using KEXIDB_DRIVER macro (see driver_p.h)
		virtual int versionMajor() const = 0;
		virtual int versionMinor() const = 0;

	protected:
		/*! For reimplemenation: creates and returns connection object 
		 with additional structures specific for a given driver.
		 Connection object should inherit Connection and have a destructor 
		 that descructs all allocated driver-dependent connection structures. */
		virtual Connection *drv_createConnection( ConnectionData &conn_data ) = 0;
//virtual ConnectionInternal* createConnectionInternalObject( Connection& conn ) = 0;

		/*! Used by DriverManager. 
		 Note for driver developers: Reimplement this.
		 In your reimplementation you should initialize:
		 - m_typeNames - to types accepted by your engine
		 - m_isFileDriver - to true or false depending if your driver is file-based
		 - m_features - to combination of selected values from Features enum
		 
		 You may also want to change options in DriverBehaviour *beh member.
		 See drivers/mySQL/mysqldriver.cpp for usage example.
		 */
		Driver( QObject *parent, const char *name, const QStringList &args = QStringList() );

		/*! \return connection \a conn , do not deletes it nor affect.
		 Returns 0 if \a conn is not owned by this driver.
		 After this, you are owner of \a conn object, so you should
		 eventually delete it. Better use Connection destructor. */
		Connection* removeConnection( Connection *conn );

		QPtrDict<KexiDB::Connection> m_connections;

//(js)now QObject::name() is reused:
//		/*! The name equal to the service name (X-Kexi-DriverName) 
//		 stored in given service .desktop file. Set this in subclasses. */
//		QString m_driverName;
		
		/*! Name of MIME type of files handled by this driver 
		 if it is a file-based database's driver 
		 (equal X-Kexi-FileDBDriverMime service property) */
		QString m_fileDBDriverMime;

		/*! Info about the driver as a service. */
		KService *m_service;

		/*! Internal constant flag: Set this in subclass if driver is a file driver */
		bool m_isFileDriver : 1;

		/*! Internal constant flag: Set this in subclass if after successfull drv_createDatabased()
		 database is in opened state (as after useDatabase()). For most engines this is not true. */
		bool m_isDBOpenedAfterCreate : 1;

		/*! List of system objects names, eg. build-in system tables that cannot be used by user,
		 and in most cases user even shouldn't see these.
		 The list contents is driver dependent (by default is empty) - fill this in subclass ctor. */
//		QStringList m_systemObjectNames;

		/*! List of system fields names, build-in system fields that cannot be used by user,
		 and in most cases user even shouldn't see these.
		 The list contents is driver dependent (by default is empty) - fill this in subclass ctor. */
//		QStringList m_systemFieldNames;

		/*! Features (like transactions, etc.) supported by this driver
		 (sum of selected  Features enum items). 
		 This member should be filled in driver implementation's constructor 
		 (by default m_features==NoFeatures). */
		int m_features;

		//! real type names for this engine
		QValueVector<QString> m_typeNames;

	friend class Connection;
	friend class Cursor;
	friend class DriverManagerInternal;

		DriverBehaviour *beh;
		
		DriverPrivate *d; //unused
};

} //namespace KexiDB

//! driver's static version information, automatically impemented for KexiDB drivers
//! put this into Driver class declaration just like Q_OBJECT macro
#define KEXIDB_DRIVER \
	public: \
	virtual int versionMajor() const; \
	virtual int versionMinor() const;

#endif

