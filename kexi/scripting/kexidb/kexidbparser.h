/***************************************************************************
 * kexidbparser.h
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

#ifndef KROSS_KEXIDB_KEXIDBPARSER_H
#define KROSS_KEXIDB_KEXIDBPARSER_H

#include <qstring.h>

#include "../api/object.h"
#include "../api/list.h"
#include "../api/class.h"

#include <kexidb/drivermanager.h>
#include <kexidb/parser/parser.h>

namespace Kross { namespace KexiDB {

    class KexiDBParser : public Kross::Api::Class<KexiDBParser>
    {
        public:

            /**
             * Constructor.
             */
            KexiDBParser(::KexiDB::Parser* parser);

            /**
             * Destructor.
             */
            virtual ~KexiDBParser();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// See \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

        private:
            ::KexiDB::Parser* m_parser;

            Kross::Api::Object* parse(Kross::Api::List*);
            Kross::Api::Object* clear(Kross::Api::List*);
            Kross::Api::Object* operation(Kross::Api::List*);
            Kross::Api::Object* table(Kross::Api::List*);
            Kross::Api::Object* query(Kross::Api::List*);
            Kross::Api::Object* connection(Kross::Api::List*);
            Kross::Api::Object* statement(Kross::Api::List*);

            Kross::Api::Object* errorType(Kross::Api::List*);
            Kross::Api::Object* errorMsg(Kross::Api::List*);
            Kross::Api::Object* errorAt(Kross::Api::List*);
    };

}}

#endif

