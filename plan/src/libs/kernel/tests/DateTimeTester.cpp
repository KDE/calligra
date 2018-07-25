/* This file is part of the KDE project
   Copyright (C) 2006-2007 Dag Andersen <danders@get2net.dk>
   Copyright (C) 2016 Dag Andersen <danders@get2net.dk>
   
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

#include "DateTimeTester.h"
#include <kptdatetime.h>
#include <kptduration.h>

#include "debug.cpp"

namespace KPlato
{

void DateTimeTester::subtractDay()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(8, 0, 0, 0 ));
    DateTime dt2(QDate(2006, 1, 2), QTime(8, 0, 0, 0));
    Duration d(1, 0, 0, 0, 0);

    QVERIFY((dt2-dt1).toString() == d.toString());
    QVERIFY((dt1-dt2).toString() == d.toString()); // result always positive
    QVERIFY((dt2-d).toString() == dt1.toString());
}
void DateTimeTester::subtractHour()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(0, 0, 0, 0 ));
    DateTime dt2(QDate(2006, 1, 1), QTime(1, 0, 0, 0));
    Duration d(0, 1, 0, 0, 0);

    QVERIFY((dt2-dt1).toString() == d.toString());
    QVERIFY((dt1-dt2).toString() == d.toString()); // result always positive
    QVERIFY((dt2-d).toString() == dt1.toString());
}
void DateTimeTester::subtractMinute()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(0, 0, 0, 0 ));
    DateTime dt2(QDate(2006, 1, 1), QTime(0, 1, 0, 0));
    Duration d(0, 0, 1, 0, 0);

    QVERIFY((dt2-dt1).toString() == d.toString());
    QVERIFY((dt1-dt2).toString() == d.toString()); // result always positive
    QVERIFY((dt2-d).toString() == dt1.toString());
}
void DateTimeTester::subtractSecond()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(0, 0, 0, 0 ));
    DateTime dt2(QDate(2006, 1, 1), QTime(0, 0, 1, 0));
    Duration d(0, 0, 0, 1, 0);

    QVERIFY((dt2-dt1).toString() == d.toString());
    QVERIFY((dt1-dt2).toString() == d.toString()); // result always positive
    QVERIFY((dt2-d).toString() == dt1.toString());
    
}

void DateTimeTester::subtractMillisecond()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(0, 0, 0, 0 ));
    DateTime dt2(QDate(2006, 1, 1), QTime(0, 0, 0, 1));
    Duration d(0, 0, 0, 0, 1);

    QVERIFY((dt2-dt1).toString() == d.toString());
    QVERIFY((dt1-dt2).toString() == d.toString()); // result always positive
    QVERIFY((dt2-d) == dt1);
    
    dt1 = DateTime(QDate(2006, 1, 1), QTime(0, 0, 0, 1 ));
    dt2 = DateTime(QDate(2006, 1, 1), QTime(0, 0, 0, 0));
    d = Duration(0, 0, 0, 0, 1);

    QVERIFY((dt2-dt1).toString() == d.toString());
    QVERIFY((dt1-dt2).toString() == d.toString()); // result always positive
    QVERIFY((dt1-d) == dt2);
    
    dt1 = DateTime(QDate(2006, 1, 1), QTime(0, 0, 1, 1 ));
    dt2 = DateTime(QDate(2006, 1, 1), QTime(0, 1, 0, 0));
    d = Duration(0, 0, 0, 58, 999);

    QVERIFY((dt2-dt1).toString() == d.toString());
    QVERIFY((dt1-dt2).toString() == d.toString()); // result always positive
    QVERIFY((dt2-d) == dt1);   
}

void DateTimeTester::addDay()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(8, 0, 0, 0 ));
    DateTime dt2(QDate(2006, 1, 2), QTime(8, 0, 0, 0));
    Duration d(1, 0, 0, 0, 0);
    
    QVERIFY((dt1+d) == dt2);
}
void DateTimeTester::addHour()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(0, 0, 0, 0 ));
    DateTime dt2(QDate(2006, 1, 1), QTime(1, 0, 0, 0));
    Duration d(0, 1, 0, 0, 0);

    QVERIFY((dt1+d) == dt2);
}
void DateTimeTester::addMinute()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(0, 0, 0, 0 ));
    DateTime dt2(QDate(2006, 1, 1), QTime(0, 1, 0, 0));
    Duration d(0, 0, 1, 0, 0);

    QVERIFY((dt1+d) == dt2);
}
void DateTimeTester::addSecond()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(0, 0, 0, 0 ));
    DateTime dt2(QDate(2006, 1, 1), QTime(0, 0, 1, 0));
    Duration d(0, 0, 0, 1, 0);

    QVERIFY((dt1+d) == dt2);
    
}
void DateTimeTester::addMillisecond()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(0, 0, 0, 0 ));
    DateTime dt2(QDate(2006, 1, 1), QTime(0, 0, 0, 1));
    Duration d(0, 0, 0, 0, 1);

    QVERIFY((dt1+d) == dt2);
    
}

void DateTimeTester::timeZones()
{
    QByteArray tz("TZ=Europe/Copenhagen");
    putenv(tz.data());
    
    QTimeZone testZone("Europe/London");
    
    DateTime dt1(QDate(2006, 1, 1), QTime(8, 0, 0, 0 ), testZone);
    
    DateTime dt2 = dt1;
    qDebug()<<dt1<<dt2;
    QCOMPARE(dt1.timeZone(), dt2.timeZone());
    
    dt2 += Duration(1, 0, 0, 0, 0);
    qDebug()<<dt2;
    QCOMPARE(dt1.timeZone(), dt2.timeZone());
    
    dt2 -= Duration(1, 0, 0, 0, 0);
    qDebug()<<dt1<<dt2;
    QCOMPARE(dt1.timeZone(), dt2.timeZone());
    QCOMPARE(dt2, dt1);
    
    dt2 = dt1 + Duration(1, 0, 0, 0, 0);
    qDebug()<<dt1<<dt2;
    QCOMPARE(dt1.timeZone(), dt2.timeZone());

    dt2 = dt2 - Duration(1, 0, 0, 0, 0);
    qDebug()<<dt1<<dt2;
    QCOMPARE(dt1.timeZone(), dt2.timeZone());
    QCOMPARE(dt2, dt1);
    
    DateTime dt3 = QDateTime(QDate(2006, 1, 1), QTime(8, 0, 0, 0 ), testZone);
    qDebug()<<dt3;
    QCOMPARE(dt3.timeZone(), testZone);
    
    DateTime dt4(QDateTime(QDate(2006, 1, 1), QTime(8, 0, 0, 0 ), Qt::UTC));
    dt4 += Duration(1, 0, 0, 0, 0);
    qDebug()<<dt4;
    QCOMPARE(dt4.timeSpec(), Qt::UTC);

    unsetenv("TZ");
}

} //namespace KPlato

QTEST_GUILESS_MAIN( KPlato::DateTimeTester )
