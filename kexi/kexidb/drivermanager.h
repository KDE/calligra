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

#ifndef KEXIDB_DRIVER_MNGR_H
#define KEXIDB_DRIVER_MNGR_H

#include <qobject.h>
#include <qcstring.h>
#include <qmap.h>
#include <qdict.h>

#include <klibloader.h>
#include <kservice.h>

#include <kexidb/object.h>

namespace KexiDB {

class DriverManagerInternal;
class Connection;
class ConnectionData;
class Driver;
/*!

*/
class KEXI_DB_EXPORT DriverManager : public QObject, public KexiDB::Object
{
	public:
		typedef QMap<QString, KService::Ptr> ServicesMap;

		DriverManager();
		virtual ~DriverManager();

		/*! Tries to load db driver with named name \a name.
			The name is case insensitive.
			\return db driver, or 0 if error (then error message is also set) */
		Driver* driver(const QCString& name);

		/*! returns list of available drivers names. 
			That drivers can be loaded by first use of driver() method. */
		const QStringList driverNames();

		/*! \return service information about driver's named with \a name.
			The name is case insensitive. */
		KService::Ptr serviceInfo(const QString &name);

		/*! \return a map structure of the services. Not necessary for everyday use. */
		const ServicesMap& services();

		/**!
		 * looks up a driver by it's file database mimetype
		 * @returns the driver name
		 */
		const QString	lookupByMime(const QString &mime);
	private:
		DriverManagerInternal *d_int;
};

} //namespace KexiDB

#endif
