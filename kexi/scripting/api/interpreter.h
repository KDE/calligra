/***************************************************************************
 * interpreter.h
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
            Interpreter();

            /**
             * Destructor.
             */
            virtual ~Interpreter();

            /**
             * Check if a \a Module with the defined
             * name is avaible.
             *
             * \param name The name of the module.
             * \return true if the module is avaible
             *         else false.
             */
            bool hasModule(const QString& name);

            /**
             * Return the \a Module with the defined
             * name.
             *
             * \param name The name of the module.
             * \return The \a Module if there is such a
             *         module avaible else NULL.
             */
            Object* getModule(const QString& name);

            /**
             * Add a new \a Module to the list of avaible
             * modules.
             *
             * \param module The \a Module to add.
             * \return true if the module was added
             *         successfully else false.
             */
            bool addModule(Object* module);

            /**
             * List of mimetypes this interpreter supports.
             *
             * \return QStringList with mimetypes like
             *         "application/x-javascript".
             */
            virtual const QStringList mimeTypes() = 0;

            /**
             * Execute/interpret a string.
             *
             * \param execstring The string to execute.
             * \return true if execution was successfully
             *         else false.
             */
            virtual bool execute(const QString& execstring) = 0;

        protected:
            /// List of avaible modules.
            QMap<QString, Object*> m_modules;
            /// List of mimetypes this interpreter supports.
            QStringList m_mimetypes;
    };

}}

#endif

