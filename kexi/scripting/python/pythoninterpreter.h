/***************************************************************************
 * pythoninterpreter.h
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
//#include "CXX/Objects.hxx"
//#include "CXX/Extensions.hxx"

#include <qstring.h>
#include <kdebug.h>

#include "../api/object.h"
#include "../api/interpreter.h"
#include "pythonmodule.h"
//#include "pythonextension.h"

namespace Kross { namespace Python {

    /**
     * Python interpreter bridge.
     *
     * See \a Kross::Api::Interpreter
     */
    class PythonInterpreter : public Kross::Api::Interpreter
    {
        public:

            /**
             * Constructor.
             */
            PythonInterpreter();

            /**
             * Destructor.
             */
            virtual ~PythonInterpreter();

            /**
             * Return a list of mimetypes the Python-Interpreter
             * supports.
             *
             * See \see Kross::Api::Interpreter::execute()
             */
            virtual const QStringList mimeTypes();

            /**
             * Execute/interpret a Python-string.
             *
             * See \see Kross::Api::Interpreter::execute()
             */
            virtual bool execute(const QString& execstring);

        private:

            /**
             * Python uses so called threads to separate
             * executions. The PyThreadState holds the
             * thread we use for this Python bridge.
             */
            PyThreadState *m_gtstate;

            /**
             * The to \a PythonModule translated
             * \a Kross::Api::Module instances.
             */
            QMap<QString, PythonModule*> m_pythonmodules;

    };

}}

#endif
