/* This file is part of the KOffice project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
 * Copyright (c) 2008 Dag Andersen <kplato@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTING_PROJECT_H
#define SCRIPTING_PROJECT_H

#include <QObject>
#include <kptroject.h>

#include "Module.h"

namespace Scripting {

    /**
    * The Project class represents a project in the document.
    */
    class Project : public QObject
    {
            Q_OBJECT
        public:
            Project( Module* module, KPlato::Project &project )
                : QObject( module ), m_project( project ) {}
            virtual ~Project() {}

        public Q_SLOTS:

            int name() const { return m_project->name(); }

        private:
            KPlato::Project &m_project;
    };

}

#endif
