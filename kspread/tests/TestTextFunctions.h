/* This file is part of the KDE project
   Copyright 2007 Brad Hards <bradh@frogmouth.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
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

#ifndef KSPREAD_TEST_TEXT_FUNCTIONS
#define KSPREAD_TEST_TEXT_FUNCTIONS

#include <QtGui>
#include <QtTest/QtTest>

#include <Value.h>

namespace KSpread
{

class TestTextFunctions: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testASC();
    void testCHAR();
    void testCLEAN();
    void testCODE();
    void testCONCATENATE();
    void testEXACT();
    void testFIND();
    void testFIXED();
    void testJIS();
    void testLEFT();
    void testLEN();
    void testLOWER();
    void testMID();
    void testPROPER();
    void testREPLACE();
    void testREPT();
    void testRIGHT();
    void testSEARCH();
    void testSUBSTITUTE();
    void testT();
    void testTRIM();
    void testUNICHAR();
    void testUNICODE();
    void testUPPER();
    void testROT13();
    void testBAHTTEXT();

private:
    Value evaluate(const QString&, Value& ex);
};

} // namespace KSpread

#endif
