/* This file is part of the KDE project
   Copyright 2007 Sascha Pfau <MrPeacock@web.de>

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

#ifndef KSPREAD_TEST_DATETIME_FUNCTIONS
#define KSPREAD_TEST_DATETIME_FUNCTIONS

#include <QtGui>
#include <QtTest/QtTest>

#include <Value.h>

namespace KSpread
{

class TestDatetimeFunctions: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testYEARFRAC();
    void testDATEDIF();
    void testISLEAPYEAR();
    void testWEEKNUM();
    void testWEEKSINYEAR();
    void testWORKDAY();
    void testNETWORKDAY();
    void testUNIX2DATE();
    void testDATE2UNIX();
    void testDATE();
    void testDATEVALUE();
    void testDAY();
    void testDAYS();
    void testDAYSINMONTH();
    void testDAYSINYEAR();
    void testDAYS360();
    void testEDATE();
    void testEOMONTH();
    void testHOUR();
    void testISOWEEKNUM();
    void testMINUTE();
    void testMONTH();
    void testMONTHS();
    void testNOW();
    void testSECOND();
    void testTIME();
    void testTIMEVALUE();
    void testTODAY();
    void testWEEKDAY();
    void testYEAR();
    void testYEARS();
    void testWEEKS();

private:
    Value evaluate(const QString&, Value& ex);
};

} // namespace KSpread

#endif // KSPREAD_TEST_DATETIME_FUNCTIONS
