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

namespace Kross { namespace Api {

    // Forward declarations.
    class Manager;
    class Object;
    class List;
    class QtObject;
    class Script;

    /**
     * The ScriptContainer class represents a single scriptfile.
     */
    class /*KROSS_MAIN_EXPORT*/ ScriptContainer : private QObject
    {
            Q_OBJECT

        public:

            /**
             * Constructor.
             */
            explicit ScriptContainer(Manager* manager, const QString& name);

            /**
             * Destructor.
             */
            ~ScriptContainer();

            /**
             * Return the name is ScriptContainer is reachable as.
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
             * Return the name of the interpreter used
             * on \a execute.
             */
            const QString& getInterpreterName();

            /**
             * Set the name of the interpreter used
             * on \a execute.
             */
            void setInterpreterName(const QString&);

            /**
             * Return the functionname to call on \a execute.
             */
            const QString& getFunctionName();

            /**
             * Set the functionname to call on \a execute.
             */
            void setFunctionName(const QString&);

            /**
             * Return a list of arguments to pass if the with
             * \a setFunctionName defined function got called
             * on \a execute.
             */
            Kross::Api::List* getFunctionArguments();

            /**
             * Set the list of arguments to pass if the with
             * \a setFunctionName defined function got called
             * on \a execute.
             */
            void setFunctionArguments(Kross::Api::List*);

            /**
             * Return the interpreter dependend from \a Script
             * inherited class this ScriptContainer uses or
             * NULL if not initialized jet.
             */
            Script* getScript();

            /**
             * Execute the script container.
             */
            Kross::Api::Object* execute();

            Kross::Api::Object* callFunction();

        signals:
            //void done();
            //void success();
            //void error();

        public slots:
            //void execute();

        private:
            Manager* m_manager;
            Script* m_script;

            QString m_name;
            QString m_code;
            QString m_interpretername;

            QString m_functionname;
            Kross::Api::List* m_functionargs;

            /**
             * Set the interpreter dependend from \a Script
             * inherited class this ScriptContainer uses.
             *
             * \param script The \a Script to use or NULL
             *        if set dirty (means the ScriptContainer
             *        is uninitialized again).
             */
            void setScript(Script* script = 0);
    };

}}

#endif

