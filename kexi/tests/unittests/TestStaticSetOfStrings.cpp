/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Adam Pigg <piggz1@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "TestStaticSetOfStrings.h"
#include "qtest_kde.h"
#include <QtTest/QtTest>

const char* TestStaticSetOfStrings::keywords[] = {
    "ABORT",
    "ABSOLUTE",
    "ACCESS",
    "ACTION",
    "ADD",
    "AGGREGATE",
    "ALTER",
    "ANALYSE",
    "ANALYZE",
    "ANY",
    "ARRAY",
    "ASSERTION",
    "ASSIGNMENT",
    "AT",
    "AUTHORIZATION",
    "BACKWARD",
    "BIGINT",
    "BINARY",
    0
};

void TestStaticSetOfStrings::initTestCase()
{
    strings.setStrings(keywords);
}

void TestStaticSetOfStrings::testContains()
{
    QCOMPARE(strings.contains("ANY"), true); //test a random string
    QCOMPARE(strings.contains(QString("backward").toUpper().toLocal8Bit()), true);
    QCOMPARE(strings.contains("BIGIN"), false); //test a sub-string
    QCOMPARE(strings.contains("XXXXXXXXXX"), false); //test some garbage
    QCOMPARE(strings.isEmpty(), false);
    QCOMPARE(strings.contains("ABORT"), true); //test start of list
    QCOMPARE(strings.contains("BINARY"), true); //test end of list
}

void TestStaticSetOfStrings::cleanupTestCase()
{

}

QTEST_KDEMAIN(TestStaticSetOfStrings, GUI)

#include "TestStaticSetOfStrings.moc"