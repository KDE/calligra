/***************************************************************************
 * kjsinterpreter.h
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

#ifndef KROSS_KJS_INTERPRETER_H
#define KROSS_KJS_INTERPRETER_H

#include <qstring.h>
#include <kdebug.h>

#include "../api/object.h"
#include "../api/interpreter.h"

namespace Kross { namespace Kjs {

    // Forward declaration.
    class KjsInterpreterPrivate;

    /**
     * KDE-JavaScript interpreter bridge.
     *
     * See \see Kross::Api::Interpreter
     */
    class KjsInterpreter : public Kross::Api::Interpreter
    {
        public:

            /**
             * Constructor.
             */
            KjsInterpreter();

            /**
             * Destructor.
             */
            virtual ~KjsInterpreter();

            /**
             * Return a list of mimetypes the KJS-Interpreter
             * supports.
             *
             * See \see Kross::Api::Interpreter::execute()
             */
            virtual const QStringList mimeTypes();

            /**
             * Execute/interpret a KJS-string.
             *
             * See \see Kross::Api::Interpreter::execute()
             */
            virtual bool execute(const QString& execstring);

        private:

            /// Private d-pointer class.
            KjsInterpreterPrivate* d;

    };

}}

#endif
