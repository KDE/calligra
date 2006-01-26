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
#include "DurationTester.h"
#include <kptduration.h>
#include <kunittest/runner.h>
#include <kunittest/module.h>

using namespace KUnitTest;

KUNITTEST_MODULE(kunittest_DurationTester, "Duration Tester");
KUNITTEST_MODULE_REGISTER_TESTER(DurationTester);

void DurationTester::allTests() {
    testArithmetic();
}

//FIXME: Define a operator<< for Duration

void DurationTester::testArithmetic() {
    KPlato::Duration d(0, 2, 0);
    
    CHECK((d+d).toString(), KPlato::Duration(0, 4, 0).toString());
    CHECK((d-d).toString(), KPlato::Duration(0, 0, 0).toString());
    CHECK((d/2).toString(), KPlato::Duration(0, 1, 0).toString());
}

