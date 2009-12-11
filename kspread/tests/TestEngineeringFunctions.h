/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Ariya Hidayat <ariya@kde.org>

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

#ifndef KSPREAD_TEST_ENGINEERING_FUNCTIONS
#define KSPREAD_TEST_ENGINEERING_FUNCTIONS

#include <QtGui>
#include <QtTest/QtTest>

#include <Value.h>

namespace KSpread
{

class TestEngineeringFunctions: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testCOMPLEX();
    void testERF();
    void testERFC();
    void testIMABS();
    void testIMAGINARY();
    void testIMARGUMENT();
    void testIMCONJUGATE();
    void testIMCOS();
    void testIMCOSH();
    void testIMDIV();
    void testIMEXP();
    void testIMLN();
    void testIMLOG10();
    void testIMLOG2();
    void testIMPOWER();
    void testIMPRODUCT();
    void testIMREAL();
    void testIMSIN();
    void testIMSINH();
    void testIMSQRT();
    void testIMSUB();
    void testIMSUM();
    void testIMTAN();
    void testIMTANH();

private:
    Value evaluate(const QString&);
};

} // namespace KSpread

#endif // KSPREAD_TEST_ENGINEERING_FUNCTIONS
