/***************************************************************************
 * scriptaction.h
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KROSS_API_SCRIPTACTION_H
#define KROSS_API_SCRIPTACTION_H

#include <qdom.h>
#include <kaction.h>

namespace Kross { namespace Api {

    // Forward declarations.
    class ScriptContainer;
    class ScriptActionPrivate;

    /**
     * A ScriptAction extends a KAction by providing a wrapper around
     * a \a ScriptContainer to execute scripting code on activation.
     */
    class ScriptAction : public KAction
    {
            Q_OBJECT

            /// The name of the interpreter used to execute the scripting code.
            Q_PROPERTY(QString interpretername READ getInterpreterName WRITE setInterpreterName)

            /// The scripting code which should be executed.
            Q_PROPERTY(QString code READ getCode WRITE setCode)

            /// The scriptfile which should be executed.
            Q_PROPERTY(QString file READ getFile WRITE setFile)

        public:

            /// A list of \a ScriptAction instances.
            typedef QValueList< ScriptAction* > List;

            /**
             * Constructor.
             *
             * \param name The name the \a ScriptAction has.
             */
            explicit ScriptAction(const char* name, const QString& text = QString::null);

            /**
             * Constructor.
             *
             * \param element The QDomElement which will be used
             *        to setup the \a ScriptAction attributes.
             */
            explicit ScriptAction(const QDomElement& element);

            /**
             * Destructor.
             */
            virtual ~ScriptAction();

            /**
             * @return the name of the script
             */
            const QString& getName() const;
            /**
             * \return the name of the interpreter which will be used
             * on activation to execute the scripting code.
             */
            const QString& getInterpreterName() const;

            /**
             * Set the name of the interpreter which will be used
             * on activation to execute the scripting code.
             *
             * \param name The name of the \a Interpreter . This
             *        could be e.g. "python".
             */
            void setInterpreterName(const QString& name);

            /**
             * \return the scripting code which will be executed
             * on activation.
             */
            const QString& getCode() const;

            /**
             * Set the scripting code which will be executed
             * on activation.
             *
             * \param code The scripting code.
             */
            void setCode(const QString& code);

            /**
             * \return the scriptfile which will be executed on
             * activation. The file needs to be local file we
             * are able to use QFile on.
             */
            const QString& getFile() const;

            /**
             * Set the scriptfile which will be executed on
             * activation.
             *
             * \param scriptfile The scriptfile.
             */
            void setFile(const QString& scriptfile);

            /**
             * \return the \a ScriptContainer instance this
             * \a ScriptAction wraps.
             */
            ScriptContainer* getScriptContainer() const;

            /**
             * \return a list of all kind of logs this \a ScriptAction
             * does remember.
             */
            const QStringList& getLogs() const;

        public slots:

            /**
             * If the \a ScriptAction got activated the \a ScriptContainer
             * got executed. Once this slot got executed it will emit a
             * \a success() or \a failed() signal.
             */
            virtual void activate();

            /**
             * This method behaves like the \a activated() method above except
             * that it doesn't emit the \a success() and \a failed() signals
             * but provides them in the passed call-by-reference parameters.
             * 
             * \param errormessage A message describing the error.
             * \param tracedetails A more detailed backtrace or QString::null
             *        if there was no backtrace avaiable. Those parameters
             *        will only change if the execution failed.
             * \return true if the execution was successfully else false.
             */
            virtual bool activate(QString& errormessage, QString& tracedetails);

            /**
             * This slot finalizes the \a ScriptContainer and tries to clean
             * any still running script.
             */
            void finalize();

        signals:

            /**
            * This signal got emitted after this \a ScriptAction got
            * executed successfully.
            */
            void success();

            /**
            * This signal got emitted after the try to execute this
            * \a ScriptAction failed. The \p errormessage contains
            * the error message.
            */
            void failed(const QString& errormessage, const QString& tracedetails);

        private:
            /// Internaly used private d-pointer.
            ScriptActionPrivate* d;
    };

}}

#endif

