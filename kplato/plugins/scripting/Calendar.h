/* This file is part of the KOffice project
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

#ifndef SCRIPTING_CALENDAR_H
#define SCRIPTING_CALENDAR_H

#include <QObject>
#include <QVariant>

namespace KPlato {
    class Calendar;
}

namespace Scripting {
    class Project;
    class Calendar;

    /**
    * The Calendar class represents a calendar in a project.
    */
    class Calendar : public QObject
    {
            Q_OBJECT
        public:
            /// Create a calendar
            Calendar( Project *project, KPlato::Calendar *calendar, QObject *parent );
            /// Destructor
            virtual ~Calendar() {}
        
            KPlato::Calendar *kplatoCalendar() const { return m_calendar; }
            
        public Q_SLOTS:
            /// Return the calendars id
            QString id() const;
            /// Return number of child calendars
            int childCount() const;
            /// Return the child calendar at @p index
            QObject *childAt( int index );
        
        protected:
            Project *m_project;
            KPlato::Calendar *m_calendar;
    };

}

#endif
