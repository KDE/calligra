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
    class EventManager;

    /**
     * The ScriptContainer class represents a single scriptfile.
     */
    class /*KROSS_MAIN_EXPORT*/ ScriptContainer : public QObject
    {
            Q_OBJECT

        public:

            /**
             * Constructor.
             *
             * \param manager The \a Manager instance used to
             *       create this ScriptContainer.
             * \param name The unique name this ScriptContainer
             *       has. It's used e.g. at the \a Manager to
             *       identify the ScriptContainer.
             */
            ScriptContainer(Manager* manager, const QString& name);

            /**
             * Destructor.
             */
            ~ScriptContainer();

            /**
             * Return the \a Manager this ScriptContainer belongs too.
             */
            Manager* getManager();

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
             * Return a list of functionnames the with
             * \a setCode defined scriptcode spends.
             */
            const QStringList& getFunctionNames();

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
            Kross::Api::Object* execute();

            /**
             * Call a function in the script container.
             *
             * \param functionname The name of the function
             *       to call.
             * \param arguments Optional list of arguments
             *       passed to the function.
             * \return \a Kross::Api::Object instance representing
             *        the functioncall returnvalue.
             */
            Kross::Api::Object* callFunction(const QString& functionname, Kross::Api::List* arguments = 0);

            /**
             * Connect QObject signal with function.
             */
            bool connect(QObject *sender, const QCString& signal, const QString& functionname);

            /**
             * Disconnect QObject signal from function.
             */
            bool disconnect(QObject *sender, const QCString& signal, const QString& functionname);

        signals:
            //void done(const QVariant& result);
            //void success();
            //void error();

        public slots:
            //void execute();

        private:
            Manager* m_manager;
            Script* m_script;
            EventManager* m_eventmanager;
            QMap<QString, QtObject*> m_qtobjects;

            QString m_name;
            QString m_code;
            QString m_interpretername;

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

