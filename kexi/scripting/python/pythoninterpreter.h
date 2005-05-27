/***************************************************************************
 * pythoninterpreter.h
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

#ifndef KROSS_PYTHON_INTERPRETER_H
#define KROSS_PYTHON_INTERPRETER_H

#include <Python.h>
#include "CXX/Objects.hxx"
//#include "CXX/Extensions.hxx"

#include <qstring.h>
#include <kdebug.h>

#include "../api/object.h"
#include "../api/interpreter.h"
#include "../main/manager.h"
//#include "../api/script.h"
#include "../main/scriptcontainer.h"

namespace Kross { namespace Python {

    // Forward declarations.
    class PythonSecurity;

    /**
     * Python interpreter bridge.
     *
     * Implements an \a Kross::Api::Interpreter for the python
     * interpreter.
     */
    class PythonInterpreter : public Kross::Api::Interpreter
    {
            // The friend-classes that need to access the
            // internal interpreter functionality.
            friend class PythonModule;
            friend class PythonScript;
            friend class PythonSecurity;

        public:

            /**
             * Constructor.
             */
            PythonInterpreter(Kross::Api::Manager* manager, const QString& interpretername);

            /**
             * Destructor.
             */
            virtual ~PythonInterpreter();

            /**
             * Return a list of mimetypes the Python-Interpreter supports.
             * See \see Kross::Api::Interpreter::execute
             */
            virtual const QStringList mimeTypes();

            /**
             * Return a \a PythonScript instance.
             */
            virtual Kross::Api::Script* createScript(Kross::Api::ScriptContainer* scriptcontainer);

        private:
            /// The __main__ python module.
            PythonModule* m_module;
            /// The \a PythonSecurity python module to wrap the RestrictedPython functionality.
            PythonSecurity* m_security;

            /**
             * Python uses so called threads to separate
             * executions. The PyThreadState holds the
             * thread we use for this Python bridge.
             */
            PyThreadState* m_globalthreadstate;
            PyThreadState* m_threadstate;
    };

}}

#endif
