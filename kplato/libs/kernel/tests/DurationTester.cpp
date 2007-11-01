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
#include "DurationTester.h"
#include <kptduration.h>

namespace KPlato
{

void DurationTester::add() {
    Duration d1(0, 2, 0);
    Duration d2(1, 0, 0);
    
    QVERIFY((d1+d1) == Duration(0, 4, 0));
}
void DurationTester::subtract() {
    Duration d1(0, 2, 0);
    Duration d2(1, 0, 0);
    
    QVERIFY((d1-d1) == Duration(0, 0, 0));
    QVERIFY((d2-d1) == Duration(0, 22, 0));
    QVERIFY((d1-d2) == Duration::zeroDuration); // underflow, return 0
}
void DurationTester::devide() {
    Duration d1(0, 2, 0);
    
    QVERIFY((d1/2) == Duration(0, 1, 0));
}
void DurationTester::equal() {
    Duration d1(0, 2, 0);
    
    QVERIFY(d1==d1);
}
void DurationTester::lessThanOrEqual() {
    Duration d1(0, 2, 0);
    Duration d2(1, 0, 0);
    
    QVERIFY(d1<=d1);
    QVERIFY(d1<=d2);
}
void DurationTester::greaterThanOrEqual() {
    Duration d1(0, 2, 0);
    Duration d2(1, 0, 0);
    
    QVERIFY(d1>=d1);
    QVERIFY(d2>=d1);
}
void DurationTester::notEqual() {
    Duration d1(0, 2, 0);
    Duration d2(1, 0, 0);
    
    QVERIFY(!(d1!=d1));
    QVERIFY(d1!=d2);
    QVERIFY(d2!=d1);
}
void DurationTester::greaterThan() {
    Duration d1(0, 2, 0);
    Duration d2(1, 0, 0);
    
    QVERIFY(d2>d1);
    QVERIFY(d1 > 1*60*60*1000);
}
void DurationTester::lessThan() {
    Duration d1(0, 2, 0);
    Duration d2(1, 0, 0);
    
    QVERIFY(d1<d2);
    QVERIFY(d1 < 3*60*60*1000);
}

} //namespace KPlato

QTEST_MAIN( KPlato::DurationTester )

#include "DurationTester.moc"
