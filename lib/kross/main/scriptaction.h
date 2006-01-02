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

#include "scriptcontainer.h"

namespace Kross { namespace Api {

    // Forward declarations.
    class ScriptContainer;
    class ScriptActionCollection;
    class ScriptActionPrivate;

    /**
     * A ScriptAction extends a KAction by providing a wrapper around
     * a \a ScriptContainer to execute scripting code on activation.
     */
    class ScriptAction
        : public KAction
        , public Kross::Api::ScriptContainer
    {
            Q_OBJECT

            /// The name of the interpreter used to execute the scripting code.
            //Q_PROPERTY(QString interpretername READ getInterpreterName WRITE setInterpreterName)

            /// The scripting code which should be executed.
            //Q_PROPERTY(QString code READ getCode WRITE setCode)

            /// The scriptfile which should be executed.
            //Q_PROPERTY(QString file READ getFile WRITE setFile)

        public:

            /// Shared pointer to implement reference-counting.
            typedef KSharedPtr<ScriptAction> Ptr;

            /// A list of \a ScriptAction instances.
            //typedef QValueList<ScriptAction::Ptr> List;

            /**
             * Constructor.
             *
             * \param file The KURL scriptfile this \a ScriptAction
             *        points to.
             */
            explicit ScriptAction(const QString& file);

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
             * Set the name of the interpreter which will be used
             * on activation to execute the scripting code.
             *
             * \param name The name of the \a Interpreter . This
             *        could be e.g. "python".
             */
            void setInterpreterName(const QString& name);

            /**
             * \return a list of all kind of logs this \a ScriptAction
             * does remember.
             */
            const QStringList& getLogs() const;

            /**
             * Attach this \a ScriptAction to the \a ScriptActionCollection
             * \p collection .
             */
            void attach(ScriptActionCollection* collection);

            /**
             * Detach this \a ScriptAction from the \a ScriptActionCollection
             * \p collection .
             */
            void detach(ScriptActionCollection* collection);

            /**
             * Detach this \a ScriptAction from all \a ScriptActionCollection
             * instance his \a ScriptAction is attached to.
             */
            void detachAll();

        public slots:

            /**
             * If the \a ScriptAction got activated the \a ScriptContainer
             * got executed. Once this slot got executed it will emit a
             * \a success() or \a failed() signal.
             */
            virtual void activate();

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

    class ScriptActionCollection
    {
        private:
            QValueList<ScriptAction::Ptr> m_actions;
            KActionMenu* m_actionmenu;
            bool m_dirty;

        public:
            ScriptActionCollection(const QString& text, KActionCollection* ac, const char* name)
                : m_actionmenu( new KActionMenu(text, ac, name) )
                , m_dirty(true) {}

            ~ScriptActionCollection() {
                for(QValueList<ScriptAction::Ptr>::Iterator it = m_actions.begin(); it != m_actions.end(); ++it)
                    (*it)->detach(this);
            }

            QValueList<ScriptAction::Ptr> actions() { return m_actions; }
            KActionMenu* actionMenu() { return m_actionmenu; }

            void attach(ScriptAction::Ptr action) {
                m_dirty = true;
                m_actions.append(action);
                m_actionmenu->insert(action);
                action->attach(this);
            }

            void detach(ScriptAction::Ptr action) {
                m_dirty = true;
                m_actions.remove(action);
                m_actionmenu->remove(action);
                action->detach(this);
            }

            void clear() {
                for(QValueList<ScriptAction::Ptr>::Iterator it = m_actions.begin(); it != m_actions.end(); ++it) {
                    m_actionmenu->remove(*it);
                    (*it)->detach(this);
                }
                m_actions.clear();
            }

    };

}}

#endif

