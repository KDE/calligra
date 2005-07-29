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

#include "mainmodule.h"

#include <qstring.h>
#include <qvariant.h>
#include <qobject.h>
//#include <kdebug.h>
#include <ksharedptr.h>

namespace Kross { namespace Api {

    // Forward declarations.
    class Object;
    class List;
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
    class ScriptContainer : public MainModule
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

            /// Shared pointer to implement reference-counting.
            typedef KSharedPtr<ScriptContainer> Ptr;

            /**
             * Destructor.
             */
            virtual ~ScriptContainer();

            /**
             * Return the unique name this ScriptContainer is
             * reachable as.
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
             * Execute the script container.
             */
            Object::Ptr execute();

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
            KSharedPtr<Object> callFunction(const QString& functionname, KSharedPtr<List> arguments = 0);

            /**
             * Return a list of classes.
             */
            const QStringList& getClassNames();

            /**
             * Create and return a new class instance.
             */
            KSharedPtr<Object> classInstance(const QString& name);

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

