/*
 * This file is part of KWord
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KWSCRIPTINGPART_H
#define KWSCRIPTINGPART_H

#include <QDockWidget>
#include <kparts/plugin.h>

class QTreeView;

namespace Kross {
    class Action;
    class ActionCollectionProxyModel;
    class GUIClient;
}

/**
* The KWScriptingDocker class implements a widget
* displayed in a docker.
*/
class KWScriptingDocker : public QDockWidget
{
        Q_OBJECT
    public:
        KWScriptingDocker(QWidget* parent, Kross::GUIClient* guiclient);
        virtual ~KWScriptingDocker();
    private slots:
        void runScript();
        void stopScript();
    private:
        Kross::GUIClient* m_guiclient;
        Kross::ActionCollectionProxyModel* m_model;
        QTreeView* m_view;
};

/**
* The KWScriptingPart class implements a KPart component
* to integrate scripting into KWord.
*/
class KWScriptingPart : public KParts::Plugin
{
        Q_OBJECT
    public:
        KWScriptingPart(QObject* parent, const QStringList&);
        virtual ~KWScriptingPart();
    private Q_SLOTS:
        void started(Kross::Action*);
        void finished(Kross::Action*);
    private:
        /// \internal d-pointer class.
        class Private;
        /// \internal d-pointer instance.
        Private* const d;
};

#endif
