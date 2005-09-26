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
             * \param interpretername The name of the interpreter.
             *        This could be something like "kjs" or "python".
             */
            Interpreter(const QString& interpretername);

            /**
             * Destructor.
             */
            virtual ~Interpreter();

            /**
             * Each interpreter is able to define options we could
             * use to manipulate the interpreter behaviour.
             */
            class Option
            {
                public:
                    /// Map of options.
                    typedef QMap<QString, Option*> Map;
                    /// Constructor.
                    Option(const QString& name, const QString& comment, const QVariant& value)
                        : m_name(name), m_comment(comment), m_value(value) {}
                    /// The short name of the option.
                    QString m_name;
                    /// A description of the option.
                    QString m_comment;
                    /// The value the option has.
                    QVariant m_value;
            };

            /**
             * Return the value of the \a Interpreter::Option instance
             * defined with name. If there doesn't exists an option
             * with such a name, the defaultvalue is returned.
             */
            const QVariant& getOption(const QString name, const QVariant& defaultvalue = QVariant());

            /**
             * Return a \a Interpreter::Option::Map of avaible
             * \a Interpreter::Option  instances.
             */
            Option::Map getOptions();

            /**
             * Set the \a Interpreter::Option value.
             */
            bool setOption(const QString name, const QVariant& value);

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
            /// Name of this interpreter.
            QString m_interpretername;
            /// List of mimetypes this interpreter supports.
            QStringList m_mimetypes;
            /// Map of \a Option instances. Interpreter-implementations use them.
            Option::Map m_options;
    };

}}

#endif

