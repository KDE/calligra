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
#include <qstringlist.h>
#include <qmap.h>
//#include <qvariant.h>
//#include <kdebug.h>
#include <ksharedptr.h>

class QObject;

#include "../api/object.h"

namespace Kross { namespace Api {

    // Forward declarations.
    class Interpreter;
    class Object;
    class EventSlot;
    class EventSignal;
    class ScriptContainer;
    class EventCollection;
    class ManagerPrivate;

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

        public:

            /**
             * Destructor.
             */
            ~Manager();

            /**
             * Return the Manager instance. Always use this
             * function to access the Manager singleton.
             */
            static Manager* scriptManager() {
                static Manager m_manager = Manager();
                return &m_manager;
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
            Object::Ptr getModule(const QString& name);

            /**
             * Return all avaible \a Module objects.
             *
             * \return Map of modules.
             */
            QMap<QString, Object::Ptr> getModules();

            /**
             * Add a new \a Module to the list of avaible
             * modules.
             *
             * \param module The \a Module to add.
             * \return true if the module was added
             *         successfully else false.
             */
            bool addModule(Object::Ptr module);

//QValueList<> getSlots();
//EventSlot::Ptr addSlot(QObject* sender, const QCString& slot);

//QValueList<EventSignal*> getEventSignals();
//void addSignal(EventSignal* eventsignal);

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
            KSharedPtr<ScriptContainer> getScriptContainer(const QString& scriptname);

            /**
             * Return the existing \a EventCollection with the defined name
             * or create a new \a EventCollection instance and associate
             * the passed name with it if there exists no such collection.
             *
             * \param collectionname The name of the \a EventCollection. This
             *        should be an unique string to identify the instance.
             * \return The \a EventCollection instance.
             */
            KSharedPtr<EventCollection> getEventCollection(const QString& collectionname);

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

            /**
             * Return a list of names of the at the backend
             * supported interpreters.
             */
            const QStringList getInterpreters();

        private:
            /// Private d-pointer class.
            ManagerPrivate* d;
    };

}}

#endif

