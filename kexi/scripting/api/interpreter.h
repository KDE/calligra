/***************************************************************************
 * interpreter.h
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

#ifndef KROSS_API_INTERPRETER_H
#define KROSS_API_INTERPRETER_H

#include <qstring.h>
#include <qmap.h>
#include <kdebug.h>

#include "object.h"

namespace Kross { namespace Api {

    // Forward declaration.
    class Manager;

    /**
     * Base class for interpreters.
     *
     * Each scripting backend needs to inheritate it's own
     * interpreter from this class and implementate there
     * backend related stuff.
     * The Interpreter will be managed by the \a Kross::Manager
     * class.
     */
    class Interpreter
    {
        public:

            /**
             * Constructor.
             */
            Interpreter(Manager* manager, const QString& interpretername);

            /**
             * Destructor.
             */
            virtual ~Interpreter();

            /**
             * Return the name of the interpreter.
             *
             * \return Name of the interpreter, for
             *         example "python" or "kjs".
             */
            const QString& getInterpretername();

            /**
             * List of mimetypes this interpreter supports.
             *
             * \return QStringList with mimetypes like
             *         "application/x-javascript".
             */
            virtual const QStringList mimeTypes() = 0;

            /**
             * Execute a scriptcode-string. Use \a setScript to
             * set the string that should be executed via this
             * function.
             *
             * \param code The scriptcode to execute.
             * \return true if execution was successfully else false.
             */
            virtual bool execute(const QString& code) = 0;

            /**
             * Execute a function in a script-string. This function
             * behaves similar as the one above.
             *
             * \throw Kross::Api::Exception if execution failes.
             * \param code The scriptcode to execute.
             * \param name The name of the function to execute.
             * \param args The arguments passed to the function as
             *        \a List object.
             * \return A \a Object object representing the returnvalue
             *         of the function call.
             */
            virtual Kross::Api::Object* execute(const QString& code, const QString& name, Kross::Api::List* args) = 0;

        protected:
            /// The Manager class this instance is child of.
            Manager* m_manager;
            /// Name of this interpreter.
            QString m_interpretername;
            /// List of mimetypes this interpreter supports.
            QStringList m_mimetypes;
    };

}}

#endif

