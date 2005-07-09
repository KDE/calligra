/***************************************************************************
 * scriptcontainer.h
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

#ifndef KROSS_API_SCRIPTCONTAINER_H
#define KROSS_API_SCRIPTCONTAINER_H

#include <qstring.h>
#include <qvariant.h>
#include <qobject.h>
//#include <kdebug.h>
//#include "object.h"
#include <ksharedptr.h>

namespace Kross { namespace Api {

    // Forward declarations.
    class Object;
    class List;
    class EventCollection;
    class ScriptContainerPrivate;

    /**
     * The ScriptContainer class is something like a single
     * standalone scriptfile.
     *
     * Once you've such a ScriptContainer instance you're
     * able to perform actions with it like to execute
     * scripting code. The \a Manager takes care of
     * handling the ScriptContainer instances application
     * width.
     */
    class ScriptContainer : public KShared
    {
            // We protected the constructor cause ScriptContainer
            // instances should be created only within the
            // Manager::getScriptContainer() method.
            friend class Manager;

        protected:

            /**
             * Constructor.
             *
             * The constructor is protected cause only with the
             * \a ScriptManager it's possible to access
             * \a ScriptContainer instances.
             *
             * \param name The unique name this ScriptContainer
             *       has. It's used e.g. at the \a Manager to
             *       identify the ScriptContainer.
             */
            ScriptContainer(const QString& name);

        public:

            /**
             * Destructor.
             */
            virtual ~ScriptContainer();

            /**
             * Return the name this ScriptContainer is reachable as.
             */
            const QString& getName();

            /**
             * Return the scriptcode this ScriptContainer holds.
             */
            const QString& getCode();

            /**
             * Set the scriptcode this ScriptContainer holds.
             */
            void setCode(const QString&);

            /**
             * \return the name of the interpreter used
             * on \a execute.
             */
            const QString& getInterpreterName();

            /**
             * Set the name of the interpreter used
             * on \a execute.
             */
            void setInterpreterName(const QString&);

            /**
             * Add a QObject to publish it's slots, signals
             * and properties by using \a Kross::Api::QtObject
             * as QObject-wrapper.
             *
             * \param object The QObject to wrap and publish.
             * \param name The name the QObject should be
             *        accessible as. If QString::null, then
             *        the QObject's name() will be used.
             *        Note that each QObject needs it's unique
             *        name to be easy accessible per script.
             * \return true if the QObject was successfully
             *         added else false.
             */
            bool addQObject(QObject* object, const QString& name = QString::null);

            /**
             * Execute the script container.
             */
            Object* execute();

            /**
             * Return a list of functionnames the with
             * \a setCode defined scriptcode spends.
             */
            const QStringList& getFunctionNames();

            /**
             * Call a function in the script container.
             *
             * \param functionname The name of the function
             *       to call.
             * \param arguments Optional list of arguments
             *       passed to the function.
             * \return \a Object instance representing
             *        the functioncall returnvalue.
             */
            KSharedPtr<Object> callFunction(const QString& functionname, List* arguments = 0);

            /**
             * Return a list of classes.
             */
            const QStringList& getClassNames();

            /**
             * Create and return a new class instance.
             */
            KSharedPtr<Object> classInstance(const QString& name);

            /**
             * Return the collection this @a EventContainer spends.
             */
//TODO
KSharedPtr<EventCollection> getEventCollection() {}

            /**
             * Connect QObject signal with function. If the signal
             * got emitted the scriptfunction will be executed.
             *
             * \param sender The QObject that is the sender/emitter
             *       of the signal.
             * \param signal The SIGNAL to connect the scriptfunction
             * \     with.
             * \param function The name of the scriptfunction to
             *       call if the signal got emitted.
             * \return If connection was done successfully true else
             *        false.
             */
            bool connect(QObject *sender, const QCString& signal, const QString& functionname);

            /**
             * Disconnect QObject signal from function.
             */
            bool disconnect(QObject *sender, const QCString& signal, const QString& functionname);

            /**
             * Connect script signal with QObject slot. Each
             * ScriptContainer is able to emit signals too and
             * with that function we're able to connect such
             * signals with some external QObject slot.
             *
             * Default signals each ScriptContainer spends are;
             * - stdout(const QString&)
             *   For messages the script likes to send to stdout.
             *   This is e.g. for the python-interpreter the
             *   case if the script calls the python build-in
             *   print function.
             * - stderr(const QString&)
             *   For messages the script likes to send to stderr.
             *   This includes thrown exceptions.
             *
             * \param signal The Kross SIGNAL to connect with.
             * \param receiver The QObject that should receive the signal.
             * \param slot The SLOT of the receiver to connect the Kross
             *       SIGNAL with.
             */
            bool connect(const QCString& signal, QObject *receiver, const QCString& slot);

            /**
             * Disconnect script signal from QObject slot.
             */
            bool disconnect(const QCString& signal, QObject *receiver, const QCString& slot);

        private:
            /// Internaly used private d-pointer.
            ScriptContainerPrivate* d;

            /**
             * Initialize the \a Script instance.
             */
            void initialize();

            /**
             * Finalize the \a Script instance.
             */
            void finalize();
    };

}}

#endif

