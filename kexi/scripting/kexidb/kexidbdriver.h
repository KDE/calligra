/***************************************************************************
 * kexidbdriver.h
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

#include "../api/object.h"
#include "../api/variant.h"
#include "../api/list.h"
//#include "../api/module.h"
#include "../api/class.h"

#include <kexidb/driver.h>

namespace Kross { namespace KexiDB {

    class KexiDBDriverManager;

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
             * \param drivermanager The \a KexiDBDriverManager
             *        this driver belongs too.
             * \param driver The \a ::KexiDB::Driver instance
             *        this driver wraps.
             */
            KexiDBDriver(KexiDBDriverManager* drivermanager, ::KexiDB::Driver* driver);

            /**
             * Destructor.
             */
            virtual ~KexiDBDriver();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;
            /// See \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

        private:
            ::KexiDB::Driver* driver();
            QGuardedPtr< ::KexiDB::Driver > m_driver;

            /**
             * Return the major version number of this driver.
             * See \see ::KexiDB::Driver::versionMajor
             */
            Kross::Api::Object* versionMajor(Kross::Api::List*);

            /**
             * Return the minor version number of this driver.
             * See \see ::KexiDB::Driver::versionMinor
             */
            Kross::Api::Object* versionMinor(Kross::Api::List*);

            /**
             * Return a driver-specific escaped SQL string.
             * See \see ::KexiDB::Driver::escapeString
             */
            Kross::Api::Object* escapeString(Kross::Api::List*);

            /**
             * Create a new KexiDBConnection object and return it.
             * See \see ::KexiDB::Driver::createConnection
             */
            Kross::Api::Object* createConnection(Kross::Api::List*);

            /**
             * Return a list of KexiDBConnection objects.
             * See \see ::KexiDB::Driver::connectionList
             */
            Kross::Api::Object* connectionList(Kross::Api::List*);

    };

}}

#endif

