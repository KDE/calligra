/*
 * This file is part of Calligra project
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 * Copyright (c) 2008, 2011 Dag Andersen <danders@get2net>
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

#ifndef SCRIPTING_MODULE_H
#define SCRIPTING_MODULE_H

#include "kplatoscripting_export.h"

#include <QString>
#include <QStringList>
#include <QObject>

#include <KoScriptingModule.h>

#include <kptmaindocument.h>

class QWidget;
class KUndo2Command;

namespace KPlato {
    class MainDocument;
    class NamedCommand;
}

namespace Scripting {

    class Project;

    /**
    * The Module class enables access to the KPlato functionality
    * from within the scripting backends.
    *
    * Python example that prints the documents Url and some other information;
    * \code
    * import Plan
    * print Plan.document().url()
    * print Plan.document().documentInfoTitle()
    * print Plan.document().documentInfoAuthorName()
    * print Plan.project().name()
    * \endcode
    */
    class Module : public KoScriptingModule
    {
            Q_OBJECT
        public:
            explicit Module(QObject* parent = 0);
            virtual ~Module();

            KPlato::MainDocument* part();
            virtual KoDocument* doc();
            void addCommand( KUndo2Command *cmd );

        public Q_SLOTS:

            /// Open Plan document in this module
            void openUrl( const QString &url );
            /// Open another Plan document in a sub-module with name @p tag
            QObject *openDocument( const QString tag, const QString &url );
            /// Start a command with @p name
            void beginCommand( const QString &name );
            /// End a command started with beginCommand()
            void endCommand();
            /// Revert a command started with beginCommand(), bit not ended with endCommand()
            void revertCommand();
            /// Return the project
            QObject *project();
            /// Return a schedule list view
            QWidget *createScheduleListView( QWidget *parent );
            /// Return a node property list view
            QWidget *createDataQueryView( QWidget *parent );

        private Q_SLOTS:
            void slotAddCommand( KUndo2Command *cmd );

        private:
            /// \internal d-pointer class.
            class Private;
            /// \internal d-pointer instance.
            Private* const d;
    };

}

#endif
