/*
 * This file is part of KPlato
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 * Copyright (c) 2008 Dag Andersen <kplato@kde.org>
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

#include <QString>
#include <QStringList>
#include <QObject>

#include <KoScriptingModule.h>

#include <kptpart.h>


namespace KPlato {
    class Part;
    class Node;
}

namespace Scripting {

    class Project;
    class Node;

    /**
    * The Module class enables access to the KPlato functionality
    * from within the scripting backends.
    *
    * Python example that prints the documents Url and some other information;
    * \code
    * import KPlato
    * print KPlato.document().url()
    * print KPlato.document().documentInfoTitle()
    * print KPlato.document().documentInfoAuthorName()
    * print KPlato.project().name()
    * \endcode
    */
    class Module : public KoScriptingModule
    {
            Q_OBJECT
        public:
            explicit Module(QObject* parent = 0);
            virtual ~Module();

            KPlato::Part* part();
            virtual KoDocument* doc();
            
        public Q_SLOTS:

            /// Return the project
            QObject *project();
            /// Number of child nodes in the project
            int nodeCount();
            /// Return node at @p index
            QObject *nodeAt( int index );
            
            /// Number of children of @p parent
            int childCount( QObject *parent );
            /// Return child node at @p index of @p parent
            QObject *childAt(  QObject *parent, int index );
            
            /// Number of resource groups
            int resourceGroupCount();
            /// Return resource group at @p index
            QObject *resourceGroupAt( int index );

            /// Number of resources in @p group
            int resourceCount( QObject *group );
            /// Return resource at @p index in @p group
            QObject *resourceAt( QObject *group, int index );

            /// Number of schedule managers in @p group
            int scheduleCount() const;
            /// Return the schedule at @p index
            QObject *scheduleAt( int index );
            /// Return child schedule at @p index of @p parent
            QObject *scheduleAt( QObject *parent, int index );

        private:
            /// \internal d-pointer class.
            class Private;
            /// \internal d-pointer instance.
            Private* const d;
    };

}

#endif
