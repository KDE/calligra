/* This file is part of the Calligra project
 * Copyright (c) 2008 Dag Andersen <danders@get2net>
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

#ifndef SCRIPTING_SCHEDULE_H
#define SCRIPTING_SCHEDULE_H

#include <QObject>
#include <QDate>
#include <QVariant>


namespace KPlato {
    class ScheduleManager;
}

namespace Scripting {
    class Project;
    class Schedule;

    /**
    * The Schedule class represents a schedule manager in a project.
    */
    class Schedule : public QObject
    {
            Q_OBJECT
        public:
            /// Create a schedule
            Schedule( Project *project, KPlato::ScheduleManager *schedule, QObject *parent );
            /// Destructor
            virtual ~Schedule() {}
        
        public Q_SLOTS:
            qlonglong id() const;
            QString name() const;
            bool isScheduled() const;
            
            QDate startDate();
            QDate endDate();
            
            /// Return type of schedule
            int childCount() const;
            /// Return the child schedule at @p index
            QObject *childAt( int index );

        private:
            Project *m_project;
            KPlato::ScheduleManager *m_schedule;
    };

}

#endif
