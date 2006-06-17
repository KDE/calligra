/***************************************************************************
 * kexidbtransaction.h
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

#ifndef KROSS_KEXIDB_KEXIDBTRANSACTION_H
#define KROSS_KEXIDB_KEXIDBTRANSACTION_H

#include <qstring.h>

#include <api/class.h>

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
             * \param transaction The \a ::KexiDB::Transaction instance
             *        this object wraps.
             */
            KexiDBTransaction(::KexiDB::Transaction& transaction);

            /**
             * Destructor.
             */
            virtual ~KexiDBTransaction();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// Return the ::KexiDB::Transaction instance.
            ::KexiDB::Transaction& transaction();

        private:
            ::KexiDB::Transaction& m_transaction;

            /// Return true if the transaction is avtive (ie. started).
            Kross::Api::Object::Ptr isActive(Kross::Api::List::Ptr);

            /// Return true if the transaction is uinitialised (null).
            Kross::Api::Object::Ptr isNull(Kross::Api::List::Ptr);
    };

}}

#endif

