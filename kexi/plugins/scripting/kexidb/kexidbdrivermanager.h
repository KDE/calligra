/***************************************************************************
 * kexidbdrivermanager.h
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KROSS_KEXIDB_KEXIDBDRIVERMANAGER_H
#define KROSS_KEXIDB_KEXIDBDRIVERMANAGER_H

#include <qstring.h>

#include <api/object.h>
#include <api/variant.h>
#include <api/list.h>
#include <api/class.h>

#include <kexidb/drivermanager.h>

namespace Kross { namespace KexiDB {

    /**
     * Class to wrap \a ::KexiDB::DriverManager and
     * make the functionality accessible.
     *
     * The drivermanager is the base class to access
     * \a KexiDBDriver objects.
     */
    class KexiDBDriverManager : public Kross::Api::Class<KexiDBDriverManager>
    {
        public:

            /**
             * Constructor.
             */
            KexiDBDriverManager();

            /**
             * Destructor.
             */
            virtual ~KexiDBDriverManager();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

        private:

            /**
             * Method to access the m_drivermanager. Use
             * this rather then the m_drivermanager direct
             * cause the method does some extra checks.
             *
             * \throw QString Description of the error.
             * \return The \a ::KexiDB::DriverManager
             *         instance this class holds.
             */
            inline ::KexiDB::DriverManager& driverManager();

            /// ::KexiDB::DriverManager instance.
            ::KexiDB::DriverManager m_drivermanager;

            /**
             * Return a stringlist with avaible drivernames.
             *
             * See \see ::KexiDB::DriverManager::driverNames()
             */
            Kross::Api::Object::Ptr driverNames(Kross::Api::List::Ptr);

            /**
             * Return the to the defined drivername matching
             * \a KexiDBDriver object.
             *
             * See \see ::KexiDB::DriverManager::driver()
             */
            Kross::Api::Object::Ptr driver(Kross::Api::List::Ptr);

            /**
             * Return the to the defined mimetype-string
             * matching drivername.
             *
             * See \see ::KexiDB::DriverManager::lookupByMime()
             */
            Kross::Api::Object::Ptr lookupByMime(Kross::Api::List::Ptr);

            /**
             * Return the matching mimetype for the defined file.
             * \todo move that functionality to a more common place.
             * See \see ::KexiDB::DriverManager::lookupByMime()
             */
            Kross::Api::Object::Ptr mimeForFile(Kross::Api::List::Ptr);

            /**
             * Return a new \a KexiDBConnectionData object.
             */
            Kross::Api::Object::Ptr createConnectionData(Kross::Api::List::Ptr);

            /**
             * Create and return a \a KexiDBConnectionData object. Fill the
             * content of the \a KexiDBConnectionData object with the defined
             * file as. The file could be e.g. a *.kexi file or a *.kexis file.
             */
            Kross::Api::Object::Ptr createConnectionDataByFile(Kross::Api::List::Ptr);

            /**
             * Return a new \a KexiDBField object.
             */
            Kross::Api::Object::Ptr field(Kross::Api::List::Ptr);

            /**
             * Return a new \a KexiDBTableSchema object.
             */
            Kross::Api::Object::Ptr tableSchema(Kross::Api::List::Ptr);

            /**
             * Return a new \a KexiDBQuerySchema object.
             */
            Kross::Api::Object::Ptr querySchema(Kross::Api::List::Ptr);
    };

}}

#endif

