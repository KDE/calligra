/* This file is part of the KDE project
   Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "TestIdentifier.h"
#include "kexiutils/identifier.h"
#include <QTest>

void TestIdentifier::initTestCase()
{
}

void TestIdentifier::testString2FileName_data()
{
    QTest::addColumn<QString>("string1");
    QTest::addColumn<QString>("string2");

    QTest::newRow("empty") << "" << "";
    QTest::newRow("whitespace") << " \n   \t" << "";
    QTest::newRow("special chars") << ": \\-abc" << "_abc";
    QTest::newRow("special chars2") << " */$" << "_";
}

void TestIdentifier::testString2FileName()
{
    QFETCH(QString, string1);
    QFETCH(QString, string2);
    QCOMPARE(KexiUtils::string2Identifier(string1), string2);
}

void TestIdentifier::cleanupTestCase()
{
}

QTEST_MAIN(TestIdentifier)
