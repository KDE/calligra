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

#include <QMultiMap>

#include "DateTimeTester.h"
#include "debug.cpp"

namespace KPlato
{

void AppointmentIntervalTester::interval()
{
    DateTime dt1 = DateTime( QDate( 2011, 01, 02 ), QTime( 7, 0, 0 ) );
    DateTime dt2 = DateTime( QDate( 2011, 01, 02 ), QTime( 8, 0, 0 ) );
    DateTime dt3 = DateTime( QDate( 2011, 01, 02 ), QTime( 9, 0, 0 ) );
    DateTime dt4 = DateTime( QDate( 2011, 01, 02 ), QTime( 10, 0, 0 ) );

    AppointmentInterval i1( dt1, dt2, 1 );
    AppointmentInterval i2( dt1, dt2, 1 );

    QVERIFY( ! ( i1 < i2 ) );
    QVERIFY( ! ( i2 < i1 ) );

    QVERIFY( i1.intersects( i2 ) );
    QVERIFY( i2.intersects( i1 ) );

    AppointmentInterval i3( dt2, dt3, 1 );

    QVERIFY( i1 < i3 );
    QVERIFY( ! ( i3 < i1 ) );

    QVERIFY( ! i1.intersects( i3 ) );
    QVERIFY( ! i3.intersects( i1 ) );

    AppointmentInterval i4( dt2, dt4, 1 );

    QVERIFY( i1 < i4 );
    QVERIFY( i2 < i4 );
    QVERIFY( i3 < i4 );

    QVERIFY( ! i1.intersects( i4 ) );
    QVERIFY( ! i4.intersects( i1 ) );
    QVERIFY( i3.intersects( i4 ) );
    QVERIFY( i4.intersects( i3 ) );

    AppointmentInterval i5( dt3, dt4, 1 );
    QVERIFY( ! i1.intersects( i4 ) );
    QVERIFY( ! i4.intersects( i1 ) );

}

void AppointmentIntervalTester::addInterval()
{
    AppointmentIntervalList lst;
    DateTime dt1 = DateTime( QDate( 2011, 01, 02 ), QTime( 7, 0, 0 ) );
    DateTime dt2 = dt1 + Duration( 0, 1, 0 );
    double load = 1;
    
    qDebug()<<"Add an interval"<<dt1<<dt2;
    qDebug()<<endl<<lst;
    lst.add( dt1, dt2, load );
    qDebug()<<endl<<lst;

    QCOMPARE( dt1, lst.map().values().first().startTime() );
    QCOMPARE( dt2, lst.map().values().first().endTime() );
    QCOMPARE( load, lst.map().values().first().load() );
    
    qDebug()<<"add load";
    qDebug()<<endl<<lst;
    lst.add( dt1, dt2, load );
    qDebug()<<endl<<lst;

    QCOMPARE( dt1, lst.map().values().first().startTime() );
    QCOMPARE( dt2, lst.map().values().first().endTime() );
    QCOMPARE( load*2, lst.map().values().first().load() );
    
    DateTime dt3 = dt2 + Duration( 0, 4, 0 );
    DateTime dt4 = dt3 + Duration( 0, 1, 0 );
    qDebug()<<"Add an interval after:"<<dt3<<dt4;
    
    qDebug()<<endl<<lst;
    lst.add( dt3, dt4, load );
    qDebug()<<endl<<lst;

    QCOMPARE( dt1, lst.map().values().first().startTime() );
    QCOMPARE( dt2, lst.map().values().first().endTime() );
    QCOMPARE( load*2, lst.map().values().first().load() );

    QCOMPARE( dt3, lst.map().values().last().startTime() );
    QCOMPARE( dt4, lst.map().values().last().endTime() );
    QCOMPARE( load, lst.map().values().last().load() );

    DateTime dt5 = dt2 + Duration( 0, 2, 0 );
    DateTime dt6 = dt5 + Duration( 0, 1, 0 );
    qDebug()<<"Add an interval in between:"<<dt5<<dt6;

    qDebug()<<endl<<lst;
    lst.add( dt5, dt6, load );
    qDebug()<<endl<<lst;
{
    QMap<QDate, AppointmentInterval>::const_iterator i( lst.map().constBegin() );

    QCOMPARE( dt1, i.value().startTime() );
    QCOMPARE( dt2, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt5, i.value().startTime() );
    QCOMPARE( dt6, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt3, i.value().startTime() );
    QCOMPARE( dt4, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
}    
    DateTime dt7 = dt1 - Duration( 0, 1, 0 );
    DateTime dt8 = dt7 + Duration( 0, 2, 0 );
    qDebug()<<"Add an overlapping interval at start:"<<dt7<<dt8;
    
    qDebug()<<endl<<lst;
    lst.add( dt7, dt8, load );
    qDebug()<<endl<<lst;
{
    QMap<QDate, AppointmentInterval>::const_iterator i( lst.map().constBegin() );

    QCOMPARE( dt7, i.value().startTime() );
    QCOMPARE( dt1, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt1, i.value().startTime() );
    QCOMPARE( dt8, i.value().endTime() );
    QCOMPARE( load*3, i.value().load() );
    ++i;
    QCOMPARE( dt5, i.value().startTime() );
    QCOMPARE( dt6, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt3, i.value().startTime() );
    QCOMPARE( dt4, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
}
    DateTime dt9 = dt7 +  Duration( 0, 0, 30 );
    DateTime dt10 = dt9 + Duration( 0, 0, 30 );
    qDebug()<<"Add an overlapping interval at start > start, end == end"<<dt9<<dt10;
    
    qDebug()<<endl<<lst;
    lst.add( dt9, dt10, load );
    qDebug()<<endl<<lst;
{
    QMap<QDate, AppointmentInterval>::const_iterator i( lst.map().constBegin() );

    QCOMPARE( dt7, i.value().startTime() );
    QCOMPARE( dt9, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt9, i.value().startTime() );
    QCOMPARE( dt10, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt1, i.value().startTime() );
    QCOMPARE( dt8, i.value().endTime() );
    QCOMPARE( load*3, i.value().load() );
    ++i;
    QCOMPARE( dt5, i.value().startTime() );
    QCOMPARE( dt6, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt3, i.value().startTime() );
    QCOMPARE( dt4, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
}
    DateTime dt11 = dt3 +  Duration( 0, 0, 10 );
    DateTime dt12 = dt11 + Duration( 0, 0, 30 );
    qDebug()<<"Add an overlapping interval at start > start, end < end:"<<dt11<<dt12;
    
    qDebug()<<endl<<lst;
    lst.add( dt11, dt12, load );
    qDebug()<<endl<<lst;
{
    QCOMPARE( lst.map().count(), 7 );
    QMap<QDate, AppointmentInterval>::const_iterator i( lst.map().constBegin() );

    QCOMPARE( dt7, i.value().startTime() );
    QCOMPARE( dt9, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt9, i.value().startTime() );
    QCOMPARE( dt10, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt1, i.value().startTime() );
    QCOMPARE( dt8, i.value().endTime() );
    QCOMPARE( load*3, i.value().load() );
    ++i;
    QCOMPARE( dt5, i.value().startTime() );
    QCOMPARE( dt6, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt3, i.value().startTime() );
    QCOMPARE( dt11, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt11, i.value().startTime() );
    QCOMPARE( dt12, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt12, i.value().startTime() );
    QCOMPARE( dt4, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
}
    qDebug()<<"Add an interval overlapping 2 intervals at start == start.1, end == end.2"<<dt1<<dt4;
    lst.clear();
    
    qDebug()<<endl<<lst;
    lst.add( dt1, dt2, load );
    qDebug()<<endl<<lst;
    lst.add( dt3, dt4, load );
    qDebug()<<endl<<lst;
    lst.add( dt1, dt4, load );
    qDebug()<<endl<<lst;
{
    QCOMPARE( lst.map().count(), 3 );
    QMap<QDate, AppointmentInterval>::const_iterator i( lst.map().constBegin() );

    QCOMPARE( dt1, i.value().startTime() );
    QCOMPARE( dt2, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt2, i.value().startTime() );
    QCOMPARE( dt3, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt3, i.value().startTime() );
    QCOMPARE( dt4, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
}
    lst.clear();
    dt5 = dt1 - Duration( 0, 1, 0 );
    qDebug()<<"Add an interval overlapping 2 intervals at start < start.1, end == end.2"<<dt5<<dt4;

    qDebug()<<endl<<lst;
    lst.add( dt1, dt2, load );
    qDebug()<<endl<<lst;
    lst.add( dt3, dt4, load );
    qDebug()<<endl<<lst;
    lst.add( dt5, dt4, load );
    qDebug()<<endl<<lst;
{
    QCOMPARE( lst.map().count(), 4 );
    QMap<QDate, AppointmentInterval>::const_iterator i( lst.map().constBegin() );

    QCOMPARE( dt5, i.value().startTime() );
    QCOMPARE( dt1, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt1, i.value().startTime() );
    QCOMPARE( dt2, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt2, i.value().startTime() );
    QCOMPARE( dt3, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt3, i.value().startTime() );
    QCOMPARE( dt4, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
}
    // Add an interval overlapping 2 intervals at start < start.1, end > end.2
    lst.clear();
    dt5 = dt1 - Duration( 0, 1, 0 );
    dt6 = dt4 + Duration( 0, 1, 0 );
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt5, dt6, load );
{
    QMap<QDate, AppointmentInterval>::const_iterator i( lst.map().constBegin() );

    QCOMPARE( dt5, i.value().startTime() );
    QCOMPARE( dt1, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt1, i.value().startTime() );
    QCOMPARE( dt2, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt2, i.value().startTime() );
    QCOMPARE( dt3, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt3, i.value().startTime() );
    QCOMPARE( dt4, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt4, i.value().startTime() );
    QCOMPARE( dt6, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
}
    // Add an interval overlapping 2 intervals at start < start.1, end < end.2
    lst.clear();
    dt5 = dt1 - Duration( 0, 1, 0 );
    dt6 = dt4 - Duration( 0, 0, 30 );
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt5, dt6, load );
{
    QMap<QDate, AppointmentInterval>::const_iterator i( lst.map().constBegin() );

    QCOMPARE( dt5, i.value().startTime() );
    QCOMPARE( dt1, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt1, i.value().startTime() );
    QCOMPARE( dt2, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt2, i.value().startTime() );
    QCOMPARE( dt3, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt3, i.value().startTime() );
    QCOMPARE( dt6, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt6, i.value().startTime() );
    QCOMPARE( dt4, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
}
    // Add an interval overlapping 2 intervals at start > start.1, end < end.2
    lst.clear();
    dt5 = dt1 + Duration( 0, 0, 30 );
    dt6 = dt4 - Duration( 0, 0, 30 );
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt5, dt6, load );
{
    QMap<QDate, AppointmentInterval>::const_iterator i( lst.map().constBegin() );

    QCOMPARE( dt1, i.value().startTime() );
    QCOMPARE( dt5, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt5, i.value().startTime() );
    QCOMPARE( dt2, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt2, i.value().startTime() );
    QCOMPARE( dt3, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt3, i.value().startTime() );
    QCOMPARE( dt6, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt6, i.value().startTime() );
    QCOMPARE( dt4, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
}
    // Add an interval overlapping 2 intervals at start > start.1, end == end.2
    lst.clear();
    dt5 = dt1 + Duration( 0, 0, 30 );
    dt6 = dt4;
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt5, dt6, load );
{
    QMap<QDate, AppointmentInterval>::const_iterator i( lst.map().constBegin() );

    QCOMPARE( dt1, i.value().startTime() );
    QCOMPARE( dt5, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt5, i.value().startTime() );
    QCOMPARE( dt2, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt2, i.value().startTime() );
    QCOMPARE( dt3, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt3, i.value().startTime() );
    QCOMPARE( dt6, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
}
    // Add an interval overlapping 2 intervals at start > start.1, end > end.2
    lst.clear();
    dt5 = dt1 + Duration( 0, 0, 30 );
    dt6 = dt4 + Duration( 0, 0, 30 );
    lst.add( dt1, dt2, load );
    lst.add( dt3, dt4, load );
    lst.add( dt5, dt6, load );
{
    QMap<QDate, AppointmentInterval>::const_iterator i( lst.map().constBegin() );

    QCOMPARE( dt1, i.value().startTime() );
    QCOMPARE( dt5, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt5, i.value().startTime() );
    QCOMPARE( dt2, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt2, i.value().startTime() );
    QCOMPARE( dt3, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
    ++i;
    QCOMPARE( dt3, i.value().startTime() );
    QCOMPARE( dt4, i.value().endTime() );
    QCOMPARE( load*2, i.value().load() );
    ++i;
    QCOMPARE( dt4, i.value().startTime() );
    QCOMPARE( dt6, i.value().endTime() );
    QCOMPARE( load, i.value().load() );
}

}

void AppointmentIntervalTester::addTangentIntervals()
{
    // Add an interval overlapping 2 intervals at start > start.1, end > end.2
    AppointmentIntervalList lst;

    DateTime dt1( QDate( 2010, 1, 1 ), QTime( 0, 0, 0 ) );
    DateTime dt2( QDate( 2010, 1, 1 ), QTime( 12, 0, 0 ) );
    double load = 1.;

    qDebug()<<endl<<lst;
    lst.add( dt2, dt1.addDays( 1 ), load );
    qDebug()<<endl<<lst;
    QCOMPARE( lst.map().count(), 1 );

    lst.add( dt1, dt2, load );
    QCOMPARE( lst.map().count(), 2 );
    QCOMPARE( lst.map().values().at( 0 ).startTime(), dt1 );
    QCOMPARE( lst.map().values().at( 1 ).endTime(), DateTime( dt1.addDays( 1 ) ) );
    
    // add with a 12 hours hole
    lst.add( dt2.addDays( 1 ), dt1.addDays( 2 ), load );
    QCOMPARE( lst.map().count(), 3 );
    
    // fill the hole
    lst.add( dt1.addDays( 1 ), dt2.addDays( 1 ), load );
    QCOMPARE( lst.map().count(), 4 );
}

void AppointmentIntervalTester::addAppointment()
{
    Appointment app1, app2;

    DateTime dt1 = DateTime( QDate( 2011, 01, 02 ), QTime( 7, 0, 0 ) );
    DateTime dt2 = dt1 + Duration( 0, 1, 0 );
    double load = 1;
    
    app2.addInterval( dt1, dt2, load );
    app1 += app2;
    QCOMPARE( dt1, app1.intervals().map().values().first().startTime() );
    QCOMPARE( dt2, app1.intervals().map().values().first().endTime() );
    QCOMPARE( load, app1.intervals().map().values().first().load() );

    app1 += app2;
    qDebug()<<load<<app1.intervals().map().values().first().load();
    QCOMPARE( dt1, app1.intervals().map().values().first().startTime() );
    QCOMPARE( dt2, app1.intervals().map().values().first().endTime() );
    QCOMPARE( load*2, app1.intervals().map().values().first().load() );
}

void AppointmentIntervalTester::subtractList()
{
    QString s;

    AppointmentIntervalList lst1;
    AppointmentIntervalList lst2;
    DateTime dt1 = DateTime( QDate( 2011, 01, 02 ), QTime( 7, 0, 0 ) );
    DateTime dt2 = dt1 + Duration( 0, 3, 0 );
    double load = 100;
    
    lst1.add( dt1, dt2, load );
    QCOMPARE( dt1, lst1.map().values().first().startTime() );
    QCOMPARE( dt2, lst1.map().values().first().endTime() );
    QCOMPARE( load, lst1.map().values().first().load() );
    
    lst2 += lst1;
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().first().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QCOMPARE( lst2.map().count(), 1 );
    
    lst2 -= lst1;
    QVERIFY( lst2.isEmpty() );
    
    lst2.add( dt1, dt2, load * 2. );
    lst2 -= lst1;
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().first().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QCOMPARE( lst2.map().count(), 1 );
    
    lst1.clear();
    DateTime dt3 = dt2 + Duration( 0, 6, 0 );
    DateTime dt4 = dt3 + Duration( 0, 1, 0 );
    lst1.add( dt3, dt4, load );
    qDebug()<<"Subtract non-overlapping intervals:";

    qDebug()<<endl<<lst2<<endl<<"minus"<<endl<<lst1;
    lst2 -= lst1;
    qDebug()<<endl<<"result:"<<endl<<lst2;
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().first().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QCOMPARE( lst2.map().count(), 1 );
    
    DateTime dt5 = dt1 - Duration( 0, 6, 0 );
    DateTime dt6 = dt5 + Duration( 0, 1, 0 );
    lst1.add( dt5, dt6, load );

    qDebug()<<"-------- lst2 -= lst1";
    qDebug()<<endl<<lst2<<endl<<lst1;
    lst2 -= lst1;
    qDebug()<<endl<<lst2;
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().first().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QCOMPARE( lst2.map().count(), 1 );

    s = "Subtract tangent intervals";
    qDebug()<<s;
    lst1.clear();
    lst1.add( dt1.addDays( -1 ), dt1, load ); // before
    Debug::print( lst2, "List2: " + s );
    Debug::print( lst1, "List1: " + s );

    lst2 -= lst1;
    Debug::print( lst2, "Result: " + s );
    
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().first().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QCOMPARE( lst2.map().count(), 1 );

    lst1.clear();
    lst1.add( dt2, dt2.addDays( 1 ), load ); // after

    lst2 -= lst1;
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().first().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QVERIFY( lst2.map().count() == 1 );
    
    // Subtract overlapping intervals
    lst1.clear();
    dt3 = dt1 + Duration( 0, 1, 0 );
    // starts at start, end in the middle
    lst1.add( dt1, dt3, load / 2. );

    s = "Subtract half the load of the first hour of the interval";
    qDebug()<<s;
    Debug::print( lst2, "List2: " + s );
    Debug::print( lst1, "List1: " + s );

    lst2 -= lst1;
    Debug::print( lst2, s );

    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt3, lst2.map().values().first().endTime() );
    QCOMPARE( load / 2., lst2.map().values().first().load() );

    QCOMPARE( dt3, lst2.map().values().at( 1 ).startTime() );
    QCOMPARE( dt2, lst2.map().values().at( 1 ).endTime() );
    QCOMPARE( load, lst2.map().values().at( 1 ).load() );

    s = "Subtract all load from first interval";
    qDebug()<<s;
    lst2 -= lst1; // remove first interval
    QCOMPARE( lst2.map().count(), 1 );
    QCOMPARE( dt3, lst2.map().values().at( 0 ).startTime() );
    QCOMPARE( dt2, lst2.map().values().at( 0 ).endTime() );
    QCOMPARE( load, lst2.map().values().at( 0 ).load() );

    s = "Subtract half the load from last hour of the interval";
    qDebug()<<s;
    lst1.clear();
    dt4 = dt2 - Duration( 0, 1, 0 );
    lst1.add( dt4, dt2, 50. );
    
    Debug::print( lst1, "List1: " + s );
    Debug::print( lst2, "List2: " + s );
    lst2 -= lst1;
    
    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt3, lst2.map().values().at( 0 ).startTime() );
    QCOMPARE( dt4, lst2.map().values().at( 0 ).endTime() );
    QCOMPARE( load, lst2.map().values().at( 0 ).load() );

    QCOMPARE( dt4, lst2.map().values().at( 1 ).startTime() );
    QCOMPARE( dt2, lst2.map().values().at( 1 ).endTime() );
    QCOMPARE( 50., lst2.map().values().at( 1 ).load() );

    s = "Subtract all load from last interval";
    qDebug()<<s;
    Debug::print( lst1, "List1: " + s );
    Debug::print( lst2, "List2: " + s );

    AppointmentInterval i = lst2.map().values().at( 0 );
    lst2 -= lst1;
    Debug::print( lst2, "Result: " + s );

    QCOMPARE( lst2.map().count(), 1 );
    QCOMPARE( i.startTime(), lst2.map().values().at( 0 ).startTime() );
    QCOMPARE( i.endTime(), lst2.map().values().at( 0 ).endTime() );
    QCOMPARE( i.load(), lst2.map().values().at( 0 ).load() );

    // Subtract overlapping intervals (start < start, end > end)
    lst1.clear();
    lst2.clear();
    lst2.add( dt1, dt2, 100. );

    dt3 = dt1 + Duration( 0, 1, 0 );
    // starts before start, end in the middle
    lst1.add( dt1.addSecs( -10 ), dt3, load / 2. );

    s = "Subtract half the load of the first hour of the interval";
    qDebug()<<s;
    Debug::print( lst2, "List2: " + s );
    Debug::print( lst1, "List1: " + s );

    lst2 -= lst1;
    Debug::print( lst2, s );

    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt3, lst2.map().values().first().endTime() );
    QCOMPARE( load / 2., lst2.map().values().first().load() );

    QCOMPARE( dt3, lst2.map().values().at( 1 ).startTime() );
    QCOMPARE( dt2, lst2.map().values().at( 1 ).endTime() );
    QCOMPARE( load, lst2.map().values().at( 1 ).load() );

    s = "Subtract all load from first interval";
    qDebug()<<s;
    lst2 -= lst1; // remove first interval
    QCOMPARE( lst2.map().count(), 1 );
    QCOMPARE( dt3, lst2.map().values().at( 0 ).startTime() );
    QCOMPARE( dt2, lst2.map().values().at( 0 ).endTime() );
    QCOMPARE( load, lst2.map().values().at( 0 ).load() );

    s = "Subtract half the load from last hour of the interval";
    qDebug()<<s;
    lst1.clear();
    dt4 = dt2 - Duration( 0, 1, 0 );
    lst1.add( dt4, dt2.addSecs( 10 ), 50. );
    
    Debug::print( lst2, "List2: " + s );
    Debug::print( lst1, "List1: " + s );
    lst2 -= lst1;
    
    Debug::print( lst2, "Result: " + s );

    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt3, lst2.map().values().at( 0 ).startTime() );
    QCOMPARE( dt4, lst2.map().values().at( 0 ).endTime() );
    QCOMPARE( load, lst2.map().values().at( 0 ).load() );

    QCOMPARE( dt4, lst2.map().values().at( 1 ).startTime() );
    QCOMPARE( dt2, lst2.map().values().at( 1 ).endTime() );
    QCOMPARE( 50., lst2.map().values().at( 1 ).load() );

    s = "Subtract all load from last interval";
    qDebug()<<s;
    Debug::print( lst1, "List1: " + s );
    Debug::print( lst2, "List2: " + s );

    i = lst2.map().values().at( 0 );
    qDebug()<<"i:"<<i;
    lst2 -= lst1;
    Debug::print( lst2, "Result: " + s );

    QCOMPARE( lst2.map().count(), 1 );
    QCOMPARE( i.startTime(), lst2.map().values().at( 0 ).startTime() );
    QCOMPARE( i.endTime(), lst2.map().values().at( 0 ).endTime() );
    QCOMPARE( i.load(), lst2.map().values().at( 0 ).load() );
}

void AppointmentIntervalTester::subtractListMidnight()
{
    QString s;

    AppointmentIntervalList lst1;
    AppointmentIntervalList lst2;
    DateTime dt1 = DateTime( QDate( 2011, 01, 02 ), QTime( 22, 0, 0 ) );
    DateTime dt2 = dt1 + Duration( 0, 3, 0 );
    double load = 100;
    
    lst1.add( dt1, dt2, load );
    QCOMPARE( lst1.map().count(), 2 );
    QCOMPARE( dt1, lst1.map().values().first().startTime() );
    QCOMPARE( dt2, lst1.map().values().last().endTime() );
    QCOMPARE( load, lst1.map().values().first().load() );
    QCOMPARE( load, lst1.map().values().last().load() );
    
    lst2 += lst1;
    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().last().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QCOMPARE( load, lst2.map().values().last().load() );
    
    lst2 -= lst1;
    QVERIFY( lst2.isEmpty() );
    
    lst2.add( dt1, dt2, load * 2. );
    lst2 -= lst1;
    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().last().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QCOMPARE( load, lst2.map().values().last().load() );
    
    lst1.clear();
    DateTime dt3 = dt2 + Duration( 0, 6, 0 );
    DateTime dt4 = dt3 + Duration( 0, 1, 0 );
    lst1.add( dt3, dt4, load );
    qDebug()<<"Subtract non-overlapping intervals:";

    qDebug()<<endl<<lst2<<endl<<"minus"<<endl<<lst1;
    lst2 -= lst1;
    qDebug()<<endl<<"result:"<<endl<<lst2;
    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().last().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QCOMPARE( load, lst2.map().values().last().load() );
    
    DateTime dt5 = dt1 - Duration( 0, 6, 0 );
    DateTime dt6 = dt5 + Duration( 0, 1, 0 );
    lst1.add( dt5, dt6, load );

    qDebug()<<"-------- lst2 -= lst1";
    qDebug()<<endl<<lst2<<endl<<lst1;
    lst2 -= lst1;
    qDebug()<<endl<<lst2;
    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().last().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QCOMPARE( load, lst2.map().values().last().load() );

    s = "Subtract tangent intervals";
    qDebug()<<s;
    lst1.clear();
    lst1.add( dt1.addDays( -1 ), dt1, load ); // before
    Debug::print( lst2, "List2: " + s );
    Debug::print( lst1, "List1: " + s );

    lst2 -= lst1;
    Debug::print( lst2, "Result: " + s );
    
    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().last().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QCOMPARE( load, lst2.map().values().last().load() );

    lst1.clear();
    lst1.add( dt2, dt2.addDays( 1 ), load ); // after

    lst2 -= lst1;
    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt2, lst2.map().values().last().endTime() );
    QCOMPARE( load, lst2.map().values().first().load() );
    QCOMPARE( load, lst2.map().values().last().load() );
    
    // Subtract overlapping intervals
    lst1.clear();
    dt3 = dt1 + Duration( 0, 1, 0 );
    // starts at start, end in the middle (at 23:00)
    lst1.add( dt1, dt3, load / 2. );

    s = "Subtract half the load of the first hour of the interval";
    qDebug()<<s;
    Debug::print( lst2, "List2: " + s );
    Debug::print( lst1, "List1: " + s );

    lst2 -= lst1;
    Debug::print( lst2, s );

    QCOMPARE( lst2.map().count(), 3 );
    QCOMPARE( dt1, lst2.map().values().first().startTime() );
    QCOMPARE( dt3, lst2.map().values().first().endTime() );
    QCOMPARE( load / 2., lst2.map().values().first().load() );

    QCOMPARE( dt3, lst2.map().values().at( 1 ).startTime() );
    QCOMPARE( dt2, lst2.map().values().at( 2 ).endTime() );
    QCOMPARE( load, lst2.map().values().at( 1 ).load() );
    QCOMPARE( load, lst2.map().values().at( 2 ).load() );

    s = "Subtract all load from first interval";
    qDebug()<<s;
    lst2 -= lst1; // remove first interval
    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt3, lst2.map().values().at( 0 ).startTime() );
    QCOMPARE( dt2, lst2.map().values().at( 1 ).endTime() );
    QCOMPARE( load, lst2.map().values().at( 0 ).load() );
    QCOMPARE( load, lst2.map().values().at( 1 ).load() );

    s = "Subtract half the load from last 30 min of the last interval";
    qDebug()<<s;
    lst1.clear();
    dt4 = dt2 - Duration( 0, 0, 30 );
    lst1.add( dt4, dt2, 50. );
    
    Debug::print( lst1, "List1: " + s );
    Debug::print( lst2, "List2: " + s );
    lst2 -= lst1;
    
    QCOMPARE( lst2.map().count(), 3 );
    QCOMPARE( dt3, lst2.map().values().at( 0 ).startTime() );
    QCOMPARE( dt4, lst2.map().values().at( 1 ).endTime() );
    QCOMPARE( load, lst2.map().values().at( 0 ).load() );
    QCOMPARE( load, lst2.map().values().at( 1 ).load() );

    QCOMPARE( dt4, lst2.map().values().at( 2 ).startTime() );
    QCOMPARE( dt2, lst2.map().values().at( 2 ).endTime() );
    QCOMPARE( 50., lst2.map().values().at( 2 ).load() );

    s = "Subtract all load from last interval";
    qDebug()<<s;
    Debug::print( lst1, "List1: " + s );
    Debug::print( lst2, "List2: " + s );

    lst2 -= lst1;
    Debug::print( lst2, "Result: " + s );

    QCOMPARE( lst2.map().count(), 2 );
    QCOMPARE( dt3, lst2.map().values().at( 0 ).startTime() );
    QCOMPARE( dt4, lst2.map().values().at( 1 ).endTime() );
    QCOMPARE( load, lst2.map().values().at( 0 ).load() );
    QCOMPARE( load, lst2.map().values().at( 1 ).load() );

}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::AppointmentIntervalTester )

#include "AppointmentIntervalTester.moc"
