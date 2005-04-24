/***************************************************************************
 * manager.h
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

#ifndef KROSS_API_MANAGER_H
#define KROSS_API_MANAGER_H

#include <qstring.h>
#include <qmap.h>
//#include <qvariant.h>
//#include <kdebug.h>

class QObject;

namespace Kross { namespace Api {

    // Forward declarations.
    class ScriptContainer;
    class Interpreter;
    class Object;
    class EventSlot;

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
        protected:

            /**
             * Constructor.
             */
            Manager();

            /**
             * Destructor.
             */
            ~Manager();

        public:

            /**
             * Return the Manager instance. Always use this
             * function to work with Kross.
             */
            static Manager* scriptManager() {
                static Manager* m_manager = 0;
                if(! m_manager)
                    m_manager = new Manager();
                return m_manager;
            }

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
             * Return all \a EventSlot instances.
             *
             * \return List of \a EventSlot instances.
             */
            QValueList<EventSlot*> getEventSlots();

            /**
             * Add a from \a EventSlot inherited class
             * that implements handling of additional
             * Qt slot macros.
             *
             * \param eventslot The \a EventSlot that spends
             *       slots to get Qt signals translated
             *       into a functioncall.
             */
            void addEventSlot(EventSlot* eventslot);

            /**
             * Return the existing \a ScriptContainer with scriptname
             * or create a new \a ScriptContainer instance and associate
             * the passed scriptname with it.
             *
             * \param scriptname The name of the script. This
             *        should be unique for each \a Script and
             *        could be something like the filename.
             * \return The \a ScriptContainer instance matching to
             *         scriptname.
             */
            ScriptContainer* getScriptContainer(const QString& scriptname);

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
            QMap<QString, ScriptContainer*> m_scriptcontainers;
            /// List of interpreter instances.
            QMap<QString, Interpreter*> m_interpreter;
            /// List of avaible modules.
            QMap<QString, Object*> m_modules;
            /// The buildin \a EventSlot for basic Qt slots.
            EventSlot* m_buildin_slot;
            /// List of additional \a EventSlot instances.
            QValueList<EventSlot*> m_slots;
    };

}}

#endif

