/* This file is part of the KDE project
   Copyright (C) 2008 Dag Andersen <danders@get2net.dk>

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
#include "AppointmentIntervalTester.h"
#include <kptappointment.h>
#include <kptdatetime.h>
#include <kptduration.h>

#include <qtest_kde.h>
#include <kdebug.h>

#include <QMutableMapIterator>

namespace KPlato
{

void AppointmentIntervalTester::add() {
    AppointmentIntervalList lst;
    DateTime dt1 = DateTime( KDateTime::currentDateTime( KDateTime::LocalZone ) );
    DateTime dt2 = dt1 + Duration( 0, 1, 0 );
    double load = 1;
    
    // Add an interval
    lst.add( dt1, dt2, load );
    QCOMPARE( dt1, lst.values().first()->startTime() );
    QCOMPARE( dt2, lst.values().first()->endTime() );
    QCOMPARE( load, lst.values().first()->load() );
    
    // add load
    lst.add( dt1, dt2, load );
    QCOMPARE( dt1, lst.values().first()->startTime() );
    QCOMPARE( dt2, lst.values().first()->endTime() );
    QCOMPARE( load*2, lst.values().first()->load() );
    
    // Add an interval after
    DateTime dt3 = dt2 + Duration( 0, 4, 0 );
    DateTime dt4 = dt3 + Duration( 0, 1, 0 );
    
    lst.add( dt3, dt4, load );
    QCOMPARE( dt1, lst.values().first()->startTime() );
    QCOMPARE( dt2, lst.values().first()->endTime() );
    QCOMPARE( load*2, lst.values().first()->load() );
    
    QCOMPARE( dt3, lst.values().last()->startTime() );
    QCOMPARE( dt4, lst.values().last()->endTime() );
    QCOMPARE( load, lst.values().last()->load() );

    // Add an interval in between
    DateTime dt5 = dt2 + Duration( 0, 2, 0 );
    DateTime dt6 = dt5 + Duration( 0, 1, 0 );
    
    lst.add( dt5, dt6, load );
    QMutableMapIterator<QString, AppointmentInterval*> i( lst );
    i.next();
    QCOMPARE( dt1, i.value()->startTime() );
    QCOMPARE( dt2, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt5, i.value()->startTime() );
    QCOMPARE( dt6, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt3, i.value()->startTime() );
    QCOMPARE( dt4, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    
    // Add an overlapping interval at start
    DateTime dt7 = dt1 - Duration( 0, 1, 0 );
    DateTime dt8 = dt7 + Duration( 0, 2, 0 );
    
    lst.add( dt7, dt8, load );
    i.toFront();
    i.next();
    QCOMPARE( dt7, i.value()->startTime() );
    QCOMPARE( dt1, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt1, i.value()->startTime() );
    QCOMPARE( dt8, i.value()->endTime() );
    QCOMPARE( load*3, i.value()->load() );
    i.next();
    QCOMPARE( dt5, i.value()->startTime() );
    QCOMPARE( dt6, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt3, i.value()->startTime() );
    QCOMPARE( dt4, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );

    // Add an overlapping interval at start > start, end == end
    DateTime dt9 = dt7 +  Duration( 0, 0, 30 );
    DateTime dt10 = dt9 + Duration( 0, 0, 30 );
    
    lst.add( dt9, dt10, load );
    i.toFront();
    i.next();
    QCOMPARE( dt7, i.value()->startTime() );
    QCOMPARE( dt9, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt9, i.value()->startTime() );
    QCOMPARE( dt10, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt1, i.value()->startTime() );
    QCOMPARE( dt8, i.value()->endTime() );
    QCOMPARE( load*3, i.value()->load() );
    i.next();
    QCOMPARE( dt5, i.value()->startTime() );
    QCOMPARE( dt6, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt3, i.value()->startTime() );
    QCOMPARE( dt4, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );

    // Add an overlapping interval at start > start, end < end
    DateTime dt11 = dt3 +  Duration( 0, 0, 10 );
    DateTime dt12 = dt11 + Duration( 0, 0, 30 );
    
    lst.add( dt11, dt12, load );
    i.toFront();
    i.next();
    QCOMPARE( dt7, i.value()->startTime() );
    QCOMPARE( dt9, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt9, i.value()->startTime() );
    QCOMPARE( dt10, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt1, i.value()->startTime() );
    QCOMPARE( dt8, i.value()->endTime() );
    QCOMPARE( load*3, i.value()->load() );
    i.next();
    QCOMPARE( dt5, i.value()->startTime() );
    QCOMPARE( dt6, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt3, i.value()->startTime() );
    QCOMPARE( dt11, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt11, i.value()->startTime() );
    QCOMPARE( dt12, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt12, i.value()->startTime() );
    QCOMPARE( dt4, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );

    // Add an interval overlapping 2 intervals at start == start.1, end == end.2
    lst.clear();
    
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt1, dt4, load );
    i.toFront();
    i.next();
    QCOMPARE( dt1, i.value()->startTime() );
    QCOMPARE( dt2, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt2, i.value()->startTime() );
    QCOMPARE( dt3, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt3, i.value()->startTime() );
    QCOMPARE( dt4, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );

    // Add an interval overlapping 2 intervals at start < start.1, end == end.2
    lst.clear();
    dt5 = dt1 - Duration( 0, 1, 0 );
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt5, dt4, load );
    i.toFront();
    i.next();
    QCOMPARE( dt5, i.value()->startTime() );
    QCOMPARE( dt1, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt1, i.value()->startTime() );
    QCOMPARE( dt2, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt2, i.value()->startTime() );
    QCOMPARE( dt3, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt3, i.value()->startTime() );
    QCOMPARE( dt4, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );

    // Add an interval overlapping 2 intervals at start < start.1, end > end.2
    lst.clear();
    dt5 = dt1 - Duration( 0, 1, 0 );
    dt6 = dt4 + Duration( 0, 1, 0 );
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt5, dt6, load );
    i.toFront();
    i.next();
    QCOMPARE( dt5, i.value()->startTime() );
    QCOMPARE( dt1, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt1, i.value()->startTime() );
    QCOMPARE( dt2, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt2, i.value()->startTime() );
    QCOMPARE( dt3, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt3, i.value()->startTime() );
    QCOMPARE( dt4, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt4, i.value()->startTime() );
    QCOMPARE( dt6, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );

    // Add an interval overlapping 2 intervals at start < start.1, end < end.2
    lst.clear();
    dt5 = dt1 - Duration( 0, 1, 0 );
    dt6 = dt4 - Duration( 0, 0, 30 );
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt5, dt6, load );
    i.toFront();
    i.next();
    QCOMPARE( dt5, i.value()->startTime() );
    QCOMPARE( dt1, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt1, i.value()->startTime() );
    QCOMPARE( dt2, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt2, i.value()->startTime() );
    QCOMPARE( dt3, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt3, i.value()->startTime() );
    QCOMPARE( dt6, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt6, i.value()->startTime() );
    QCOMPARE( dt4, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );

    // Add an interval overlapping 2 intervals at start > start.1, end < end.2
    lst.clear();
    dt5 = dt1 + Duration( 0, 0, 30 );
    dt6 = dt4 - Duration( 0, 0, 30 );
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt5, dt6, load );
    i.toFront();
    i.next();
    QCOMPARE( dt1, i.value()->startTime() );
    QCOMPARE( dt5, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt5, i.value()->startTime() );
    QCOMPARE( dt2, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt2, i.value()->startTime() );
    QCOMPARE( dt3, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt3, i.value()->startTime() );
    QCOMPARE( dt6, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt6, i.value()->startTime() );
    QCOMPARE( dt4, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );

    // Add an interval overlapping 2 intervals at start > start.1, end == end.2
    lst.clear();
    dt5 = dt1 + Duration( 0, 0, 30 );
    dt6 = dt4;
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt5, dt6, load );
    i.toFront();
    i.next();
    QCOMPARE( dt1, i.value()->startTime() );
    QCOMPARE( dt5, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt5, i.value()->startTime() );
    QCOMPARE( dt2, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt2, i.value()->startTime() );
    QCOMPARE( dt3, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt3, i.value()->startTime() );
    QCOMPARE( dt6, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );

    // Add an interval overlapping 2 intervals at start > start.1, end > end.2
    lst.clear();
    dt5 = dt1 + Duration( 0, 0, 30 );
    dt6 = dt4 + Duration( 0, 0, 30 );
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt5, dt6, load );
    i.toFront();
    i.next();
    QCOMPARE( dt1, i.value()->startTime() );
    QCOMPARE( dt5, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt5, i.value()->startTime() );
    QCOMPARE( dt2, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt2, i.value()->startTime() );
    QCOMPARE( dt3, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );
    i.next();
    QCOMPARE( dt3, i.value()->startTime() );
    QCOMPARE( dt4, i.value()->endTime() );
    QCOMPARE( load*2, i.value()->load() );
    i.next();
    QCOMPARE( dt4, i.value()->startTime() );
    QCOMPARE( dt6, i.value()->endTime() );
    QCOMPARE( load, i.value()->load() );


}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::AppointmentIntervalTester )

#include "AppointmentIntervalTester.moc"
