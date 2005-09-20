/***************************************************************************
 * kexidbdriver.h
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

#ifndef KROSS_KEXIDB_KEXIDBDRIVER_H
#define KROSS_KEXIDB_KEXIDBDRIVER_H

#include <qstring.h>
#include <qguardedptr.h>

#include <api/object.h>
#include <api/variant.h>
#include <api/list.h>
#include <api/class.h>

#include <kexidb/driver.h>

namespace Kross { namespace KexiDB {

    /**
     * Class to handle \a ::KexiDB::Driver instances. Such
     * drivers are managed by the \a KexiDBDriverManager.
     */
    class KexiDBDriver : public Kross::Api::Class<KexiDBDriver>
    {
        public:

            /**
             * Constructor.
             *
             * \param driver The \a ::KexiDB::Driver instance
             *        this driver wraps.
             */
            KexiDBDriver(::KexiDB::Driver* driver);

            /**
             * Destructor.
             */
            virtual ~KexiDBDriver();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

        private:
            ::KexiDB::Driver* driver();
            QGuardedPtr< ::KexiDB::Driver > m_driver;

            /**
             * Return the major version number of this driver.
             * See \see ::KexiDB::Driver::versionMajor
             */
            Kross::Api::Object::Ptr versionMajor(Kross::Api::List::Ptr);

            /**
             * Return the minor version number of this driver.
             * See \see ::KexiDB::Driver::versionMinor
             */
            Kross::Api::Object::Ptr versionMinor(Kross::Api::List::Ptr);

            /**
             * Return a driver-specific escaped SQL string.
             * See \see ::KexiDB::Driver::escapeString
             */
            Kross::Api::Object::Ptr escapeString(Kross::Api::List::Ptr);

            /**
             * Return the escaped and convert as second argument
             * passed \a Kross::Api::Variant value to the as first
             * argument passed \a KexiDBField::type.
             * See \see ::KexiDB::Driver::valueToSQL
             */
            Kross::Api::Object::Ptr valueToSQL(Kross::Api::List::Ptr);

            /**
             * Create a new KexiDBConnection object and return it.
             * See \see ::KexiDB::Driver::createConnection
             */
            Kross::Api::Object::Ptr createConnection(Kross::Api::List::Ptr);

            /**
             * Return a list of KexiDBConnection objects.
             * See \see ::KexiDB::Driver::connectionList
             */
            Kross::Api::Object::Ptr connectionList(Kross::Api::List::Ptr);

    };

}}

#endif

