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
    class ScriptContainer;
    class Script;

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
             *
             * \param manager The \a Manager instance used to
             *        create this interpreter.
             * \param interpretername The name of the interpreter.
             *        This could be something like "kjs" or "python".
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
             * Create and return a new interpreter dependend
             * \a Script instance.
             *
             * \param scriptcontainer The \a ScriptContainer
             *        to use for the \a Script instance.
             * \return The from \a Script inherited instance.
             */
            virtual Script* createScript(ScriptContainer* scriptcontainer) = 0;

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

