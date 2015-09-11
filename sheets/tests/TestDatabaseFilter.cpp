/* This file is part of the KDE project
   Copyright 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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

#include "TestDatabaseFilter.h"

#include <sheets/database/Filter.h>

#include <QTest>

using namespace Calligra::Sheets;

void DatabaseFilterTest::testIsEmpty()
{
    Filter f;
    QVERIFY(f.isEmpty());
}

void DatabaseFilterTest::testEmptyEquals()
{
    Filter a, b;
    QVERIFY2(a == b, "Two empty filters are equal");
}

void DatabaseFilterTest::testSimpleEquals()
{
    Filter a;
    Filter b;
    a.addCondition(Filter::AndComposition, 0, Filter::Match, "test");
    QVERIFY(a != b);
    b.addCondition(Filter::AndComposition, 0, Filter::Match, "test");
    QVERIFY(a == b);
}

void DatabaseFilterTest::testNotEquals1()
{
    Filter a;
    Filter b;
    a.addCondition(Filter::AndComposition, 0, Filter::Match, "test");
    b.addCondition(Filter::AndComposition, 0, Filter::Match, "test2");
    QVERIFY(a != b);
}

void DatabaseFilterTest::testNotEquals2()
{
    Filter a;
    Filter b;
    a.addCondition(Filter::AndComposition, 0, Filter::Match, "test");
    b.addCondition(Filter::AndComposition, 0, Filter::NotMatch, "test");
    QVERIFY(a != b);
}

void DatabaseFilterTest::testAndEquals()
{
    Filter a;
    Filter b;
    a.addCondition(Filter::AndComposition, 0, Filter::Match, "test");
    b.addCondition(Filter::AndComposition, 0, Filter::Match, "test");
    a.addCondition(Filter::AndComposition, 0, Filter::Match, "test2");
    b.addCondition(Filter::AndComposition, 0, Filter::Match, "test2");
    QVERIFY(a == b);
}

void DatabaseFilterTest::testOrEquals()
{
    Filter a;
    Filter b;
    a.addCondition(Filter::OrComposition, 0, Filter::Match, "test");
    b.addCondition(Filter::OrComposition, 0, Filter::Match, "test");
    a.addCondition(Filter::OrComposition, 0, Filter::Match, "test2");
    b.addCondition(Filter::OrComposition, 0, Filter::Match, "test2");
    QVERIFY(a == b);
}

QTEST_MAIN(DatabaseFilterTest)
