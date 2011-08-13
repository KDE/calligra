/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KPlato_ResourceModelTester_h
#define KPlato_ResourceModelTester_h

#include <QtTest/QtTest>

#include "kptresourceappointmentsmodel.h"

#include "kptproject.h"
#include "kptdatetime.h"

namespace QTest
{
    template<>
            char *toString(const KPlato::DateTime &dt)
    {
        return toString( dt.toString() );
    }
}

namespace KPlato
{

class Task;

class ResourceModelTester : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void internalAppointments();
    void externalAppointments();
    void externalOverbook();

private:
    void printDebug( long id ) const;
    void printSchedulingLog( const ScheduleManager &sm ) const;

    Project *m_project;
    Calendar *m_calendar;
    Task *m_task;
    Resource *m_resource;

    ResourceAppointmentsGanttModel m_model;

};

} //namespace KPlato

#endif
