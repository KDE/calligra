/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDB_DRIVER_MNGR_P_H
#define KEXIDB_DRIVER_MNGR_P_H

#include <kexidb/object.h>

#include <qobject.h>
#include <qdict.h>

namespace KexiDB {

/*! Internal class of driver manager.
*/
class KEXI_DB_EXPORT DriverManagerInternal : public QObject, public KexiDB::Object
{
	Q_OBJECT
	public:
		~DriverManagerInternal();

		/*! Tries to load db driver \a name.
			\return db driver, or 0 if error (then error message is also set) */
		KexiDB::Driver* driver(const QString& name);

		static DriverManagerInternal *self();

		/*! returns list of available drivers names. That drivers can be loaded
			by first use of driver() method. */
//		const QStringList driverNames();

		/*! \return info. about driver (service info) */
//		KService::Ptr serviceInfo(const QString &name);

		//! Returns a map structure of the services. Not necessary for everyday use.
//		const ServicesMap& services() { return m_services; }

		/*! increments the refcount for the manager*/
		void incRefCount();
		/*! decrements the refcount for the manager
			if the refcount reaches a value less than 1 the manager is freed */
		void decRefCount();
	
	protected slots:
		/*! Used to destroy all drivers on QApplication quit, so even if there are 
		 DriverManager's static instances that are destroyed on program 
		 "static destruction", drivers are not kept after QApplication death.
		*/
		void slotAppQuits();

	protected:
		/*! Used by self() */
		DriverManagerInternal();

		bool lookupDrivers();

		static KexiDB::DriverManagerInternal* s_self;

		KexiDB::DriverManager::ServicesMap m_services; //! services map
		KexiDB::DriverManager::ServicesMap m_services_lcase; //! as above but service names in lowercase
		KexiDB::DriverManager::ServicesMap m_services_by_mimetype;
		KexiDB::Driver::InfoMap m_driversInfo; //! used to store drivers information
		QDict<KexiDB::Driver> m_drivers;
		ulong m_refCount;

		QString m_serverErrMsg;
		int m_serverResultNum;
		QString m_serverResultName;
		//! result names for KParts::ComponentFactory::ComponentLoadingError
		QMap<int,QString> m_componentLoadingErrors;

		bool lookupDriversNeeded : 1;

	friend class DriverManager;
};
}

#endif

