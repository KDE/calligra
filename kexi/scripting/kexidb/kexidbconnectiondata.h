/***************************************************************************
 * kexidbconnectiondata.h
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

#ifndef KROSS_KEXIDB_KEXIDBCONNECTIONDATA_H
#define KROSS_KEXIDB_KEXIDBCONNECTIONDATA_H

#include <qstring.h>

#include "../api/object.h"
#include "../api/variant.h"
#include "../api/list.h"
//#include "../api/module.h"
#include "../api/class.h"

#include <kexidb/connection.h>
#include <kexidb/connectiondata.h>

namespace Kross { namespace KexiDB {

    // Forward declaration.
    class KexiDBConnectionData;

    class KexiDBConnectionDataManager : public Kross::Api::Class<KexiDBConnectionDataManager>
    {
        public:

            /// Constructor.
            KexiDBConnectionDataManager();

            /// Destructor.
            ~KexiDBConnectionDataManager();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// See \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

            //TODO
            //KexiDB::ConnectionData::List list() const;
            //KexiDB::ConnectionData::List list;
            //list.setAutoDelete(true);

        private:
            QValueList<KexiDBConnectionData*> m_datas;

            Kross::Api::Object* createConnectionData(Kross::Api::List*);
    };

    /**
     * The KexiDBConnection class handles \a ::KexiDB::Connection
     * instances and wraps parts of the functionality.
     */
    class KexiDBConnectionData : public Kross::Api::Class<KexiDBConnectionData>
    {
        public:

            /**
             * Constructor.
             *
             * \param data The \a ::KexiDB::ConnectionData instance
             *        this class wraps.
             */
            KexiDBConnectionData(::KexiDB::ConnectionData* data);

            /**
             * Destructor.
             */
            virtual ~KexiDBConnectionData();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// See \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

            ::KexiDB::ConnectionData* getConnectionData() const;

        private:
            ::KexiDB::ConnectionData* m_data;

            Kross::Api::Object* connName(Kross::Api::List*);
            Kross::Api::Object* setConnName(Kross::Api::List*);

            Kross::Api::Object* description(Kross::Api::List*);
            Kross::Api::Object* setDescription(Kross::Api::List*);

            Kross::Api::Object* driverName(Kross::Api::List*);
            Kross::Api::Object* setDriverName(Kross::Api::List*);

            Kross::Api::Object* localSocketFileName(Kross::Api::List*);

            // For serverbased drivers
            Kross::Api::Object* hostName(Kross::Api::List*);
            Kross::Api::Object* setHostName(Kross::Api::List*);
            Kross::Api::Object* port(Kross::Api::List*);
            Kross::Api::Object* setPort(Kross::Api::List*);
            Kross::Api::Object* password(Kross::Api::List*);
            Kross::Api::Object* setPassword(Kross::Api::List*);
            Kross::Api::Object* userName(Kross::Api::List*);
            Kross::Api::Object* setUserName(Kross::Api::List*);

            // For filebased drivers
            Kross::Api::Object* fileName(Kross::Api::List*);
            Kross::Api::Object* setFileName(Kross::Api::List*);
            Kross::Api::Object* dbPath(Kross::Api::List*);
            Kross::Api::Object* dbFileName(Kross::Api::List*);

            Kross::Api::Object* serverInfoString(Kross::Api::List*);
    };

}}

#endif

