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

#ifndef DATETIMETESTER_H
#define DATETIMETESTER_H

#include <kunittest/tester.h>
#include <kptduration.h>

class KoXmlWriter;

class DateTimeTester : public KUnitTest::Tester {
    public:
        void allTests();
    private:
        void testSubtract();
        void testAdd();
        
};

#endif
