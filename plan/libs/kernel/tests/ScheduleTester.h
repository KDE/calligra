/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPlato_ScheduleTester_h
#define KPlato_ScheduleTester_h

#include <QtTest/QtTest>
#include <QDate>
#include <QTime>

#include "kptschedule.h"
#include "kptdatetime.h"

namespace QTest
{
    template<>
            char *toString(const KPlato::DateTime &dt)
    {
        return toString( dt.toString() );
    }
    template<>
            char *toString(const KPlato::DateTimeInterval &dt)
    {
        if ( dt.first.isValid() && dt.second.isValid() )
            return toString( dt.first.toString() + " - " + dt.second.toString() );
        
        return toString( "invalid interval" );
    }
}

namespace KPlato
{

class Task;

class ScheduleTester : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    
    void available();
    void busy();

private:
    ResourceSchedule resourceSchedule;
    NodeSchedule nodeSchedule;

    QDate date;
    QTime t1;
    QTime t2;
    QTime t3;

};

} //namespace KPlato

#endif
