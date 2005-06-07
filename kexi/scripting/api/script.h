/***************************************************************************
 * script.h
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

#ifndef KROSS_API_SCRIPT_H
#define KROSS_API_SCRIPT_H

#include <qstring.h>
#include <qstringlist.h>

#include "class.h"

namespace Kross { namespace Api {

    // Forward declarations.
    class Object;
    class Interpreter;
    class ScriptContainer;
    class List;

    /**
     * Base class for interpreter dependend functionality
     * each script spends.
     */
    class Script
    {
        public:

            /**
             * Constructor.
             */
            Script(Interpreter* interpreter, ScriptContainer* scriptcontainer);

            /**
             * Destructor.
             */
            virtual ~Script();

            /**
             * Execute the script.
             */
            virtual Kross::Api::Object::Ptr execute() = 0;

            /**
             * Return a list of callable functionnames this
             * script spends.
             */
            virtual const QStringList& getFunctionNames() = 0;

            /**
             * Call a function.
             */
            virtual Kross::Api::Object::Ptr callFunction(const QString& name, Kross::Api::List::Ptr args) = 0;

            /**
             * Return a list of classes.
             */
            virtual const QStringList& getClassNames() = 0;

            /**
             * Create and return a new class instance.
             */
            virtual Kross::Api::Object::Ptr classInstance(const QString& name) = 0;

        protected:
            /// The \a Interpreter used to create this Script instance.
            Interpreter* m_interpreter;
            /// The \a ScriptContainer associated with this Script.
            ScriptContainer* m_scriptcontainer;
            //Object* m_object;
    };

}}

#endif

