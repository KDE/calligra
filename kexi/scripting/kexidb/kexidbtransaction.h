/***************************************************************************
 * kexidbtransaction.h
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

#ifndef KROSS_KEXIDB_KEXIDBTRANSACTION_H
#define KROSS_KEXIDB_KEXIDBTRANSACTION_H

#include <qstring.h>

//#include "../api/object.h"
//#include "../api/list.h"
#include "../api/class.h"

#include <kexidb/drivermanager.h>
#include <kexidb/transaction.h>

namespace Kross { namespace KexiDB {

    // Forward declaration.
    class KexiDBConnection;

    class KexiDBTransaction : public Kross::Api::Class<KexiDBTransaction>
    {
        public:

            /**
             * Constructor.
             *
             * \param connection The \a KexiDBConnection object this
             *        transaction belongs to.
             * \param transaction The \a ::KexiDB::Transaction instance
             *        this object wraps.
             */
            KexiDBTransaction(KexiDBConnection* connection, ::KexiDB::Transaction& transaction);

            /**
             * Destructor.
             */
            virtual ~KexiDBTransaction();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// See \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

            ::KexiDB::Transaction& transaction();

        private:
            ::KexiDB::Transaction& m_transaction;

            /// Return the KexiDBConnection object this KexiDBTransaction belongs to.
            Kross::Api::Object* connection(Kross::Api::List*);

            /// Return true if the transaction is avtive (ie. started).
            Kross::Api::Object* isActive(Kross::Api::List*);

            /// Return true if the transaction is uinitialised (null).
            Kross::Api::Object* isNull(Kross::Api::List*);
    };

}}

#endif

