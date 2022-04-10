/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TestDatabaseFilter.h"

#include <core/DataFilter.h>

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
    a.addCondition(Filter::AndComposition, 0, AbstractCondition::Match, "test");
    QVERIFY(a != b);
    b.addCondition(Filter::AndComposition, 0, AbstractCondition::Match, "test");
    QVERIFY(a == b);
}

void DatabaseFilterTest::testNotEquals1()
{
    Filter a;
    Filter b;
    a.addCondition(Filter::AndComposition, 0, AbstractCondition::Match, "test");
    b.addCondition(Filter::AndComposition, 0, AbstractCondition::Match, "test2");
    QVERIFY(a != b);
}

void DatabaseFilterTest::testNotEquals2()
{
    Filter a;
    Filter b;
    a.addCondition(Filter::AndComposition, 0, AbstractCondition::Match, "test");
    b.addCondition(Filter::AndComposition, 0, AbstractCondition::NotMatch, "test");
    QVERIFY(a != b);
}

void DatabaseFilterTest::testAndEquals()
{
    Filter a;
    Filter b;
    a.addCondition(Filter::AndComposition, 0, AbstractCondition::Match, "test");
    b.addCondition(Filter::AndComposition, 0, AbstractCondition::Match, "test");
    a.addCondition(Filter::AndComposition, 0, AbstractCondition::Match, "test2");
    b.addCondition(Filter::AndComposition, 0, AbstractCondition::Match, "test2");
    QVERIFY(a == b);
}

void DatabaseFilterTest::testOrEquals()
{
    Filter a;
    Filter b;
    a.addCondition(Filter::OrComposition, 0, AbstractCondition::Match, "test");
    b.addCondition(Filter::OrComposition, 0, AbstractCondition::Match, "test");
    a.addCondition(Filter::OrComposition, 0, AbstractCondition::Match, "test2");
    b.addCondition(Filter::OrComposition, 0, AbstractCondition::Match, "test2");
    QVERIFY(a == b);
}

QTEST_MAIN(DatabaseFilterTest)
