/***************************************************************************
 * manager.h
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

#ifndef KROSS_API_MANAGER_H
#define KROSS_API_MANAGER_H

#include <qstring.h>
//#include <qvaluelist.h>
#include <qmap.h>
//#include <qvariant.h>
//#include <kdebug.h>

namespace Kross { namespace Api {

    // Forward declarations.
    class Interpreter;
    class Script;
    class Object;

    /**
     * The Manager class is the main entry point to work with
     * Kross. It spends an abstraction layer between what is
     * under the hood of Kross and the functionality you need
     * to access.
     * Use \a Interpreter to just work with some implementated
     * interpreter like python. While \a Script spends a more
     * flexible container.
     */
    class KROSS_MAIN_EXPORT Manager
    {
        public:

            /**
             * Constructor.
             */
            Manager();

            /**
             * Destructor.
             */
            ~Manager();

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
             * Return all avaible \a Module objects.
             *
             * \return Map of modules.
             */
            QMap<QString, Object*> getModules();

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
             * Return the existing \a Script with scriptname
             * or create a new \a Script instance and associate
             * the passed scriptname with it.
             *
             * \param scriptname The name of the script. This
             *        should be unique for each \a Script and
             *        could be something like the filename.
             * \return The \a Script instance matching to
             *         scriptname.
             */
            Script* getScript(const QString& scriptname);

            /**
             * Return the \a Interpreter instance defined by
             * the interpretername.
             *
             * \param interpretername The name of the interpreter.
             *        e.g. "python" or "kjs".
             * \return The Interpreter instance or NULL if there
             *         does not exists an interpreter with such
             *         an interpretername.
             */
            Interpreter* getInterpreter(const QString& interpretername);

        private:
            /// List of script instances.
            QMap<QString, Script*> m_scripts;
            /// List of interpreter instances.
            QMap<QString, Interpreter*> m_interpreter;
            /// List of avaible modules.
            QMap<QString, Object*> m_modules;
    };

}}

#endif

