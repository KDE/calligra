/* This file is part of the KDE project
   Copyright 2004,2007 Ariya Hidayat <ariya@kde.org>

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

#ifndef KSPREAD_TEST_FORMULA
#define KSPREAD_TEST_FORMULA

#include <QtGui>
#include <QtTest/QtTest>
#include <Value.h>

namespace KSpread
{

class TestFormula: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testTokenizer();
    void testConstant();
    void testInvalid();
    void testUnary();
    void testBinary();
    void testOperators();
    void testComparsion();
    void testString();
    void testFunction();
    void testInlineArrays();
    void testEquality();

private:
    Value evaluate(const QString&, Value&);
};

} // namespace KSpread

#endif // KSPREAD_TEST_FORMULA
