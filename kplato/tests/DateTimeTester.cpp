/* This file is part of the KDE project
   Copyright (C) 2006-2007 Dag Andersen <danders@get2net.dk>

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

#include <QDateTime>

namespace KPlato
{

void DateTimeTester::testSubtract()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(8, 0, 0));
    DateTime dt2(QDate(2006, 1, 1), QTime(10, 0, 0));
    Duration d(0, 2, 0);

    QVERIFY((dt2-dt1).toString() == d.toString());
    QVERIFY((dt1-dt2).toString() == d.toString()); // result always positive
    QVERIFY((dt2-d).toString() == dt1.toString());
}

void DateTimeTester::testAdd()
{
    DateTime dt1(QDate(2006, 1, 1), QTime(8, 0, 0));
    DateTime dt2(QDate(2006, 1, 1), QTime(10, 0, 0));
    Duration d(0, 2, 0);
    QVERIFY((dt1+d).toString() == dt2.toString());
}

} //namespace KPlato

QTEST_MAIN( KPlato::DateTimeTester )

#include "DateTimeTester.moc"
