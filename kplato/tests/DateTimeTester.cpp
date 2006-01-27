/* This file is part of the KDE project
   Copyright (C) 2006 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

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
#include <kunittest/runner.h>
#include <kunittest/module.h>

#include <qdatetime.h>

using namespace KUnitTest;

KUNITTEST_MODULE(kunittest_DateTimeTester, "DateTime Tester");
KUNITTEST_MODULE_REGISTER_TESTER(DateTimeTester);

void DateTimeTester::allTests() {
    testSubtract();
    testAdd();
}

//FIXME: Define a operator<< for Duration
void DateTimeTester::testSubtract() {
    KPlato::DateTime dt1(QDate(2006, 1, 1), QTime(8, 0, 0));
    KPlato::DateTime dt2(QDate(2006, 1, 1), QTime(10, 0, 0));
    KPlato::Duration d(0, 2, 0);

    COMPARE((dt2-dt1).toString(), d.toString());
    COMPARE((dt1-dt2).toString(), d.toString()); // result always positive
    COMPARE((dt2-d).toString(), dt1.toString());
}

void DateTimeTester::testAdd() {
    KPlato::DateTime dt1(QDate(2006, 1, 1), QTime(8, 0, 0));
    KPlato::DateTime dt2(QDate(2006, 1, 1), QTime(10, 0, 0));
    KPlato::Duration d(0, 2, 0);
    COMPARE((dt1+d).toString(), dt2.toString());
}
