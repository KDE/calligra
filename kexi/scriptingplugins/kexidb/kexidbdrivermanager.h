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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#ifndef KROSS_KEXIDB_KEXIDBDRIVERMANAGER_H
#define KROSS_KEXIDB_KEXIDBDRIVERMANAGER_H

#include <qstring.h>

#include <scripting/api/object.h>
#include <scripting/api/variant.h>
#include <scripting/api/list.h>
#include <scripting/api/class.h>

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
            /// See \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

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
             * Return a new \a KexiDBConnectionData object.
             */
            Kross::Api::Object::Ptr createConnectionData(Kross::Api::List::Ptr);

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

