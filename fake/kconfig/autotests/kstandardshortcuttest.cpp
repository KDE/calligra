/* This file is part of the KDE libraries
    Copyright (c) 2005 David Faure <faure@kde.org>

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

#include <QtTest/QtTest>
#include "kstandardshortcuttest.h"

QTEST_MAIN( KStandardShortcutTest) // GUI needed by KAccel

#include <kstandardshortcut.h>

void KStandardShortcutTest::testShortcutDefault()
{
    QCOMPARE( QKeySequence::listToString(KStandardShortcut::hardcodedDefaultShortcut( KStandardShortcut::FullScreen )), QLatin1String( "Ctrl+Shift+F" ) );
    QCOMPARE( QKeySequence::listToString(KStandardShortcut::hardcodedDefaultShortcut( KStandardShortcut::BeginningOfLine )), QLatin1String( "Home" ) );
    QCOMPARE( QKeySequence::listToString(KStandardShortcut::hardcodedDefaultShortcut( KStandardShortcut::EndOfLine )), QLatin1String( "End" ) );
    QCOMPARE( QKeySequence::listToString(KStandardShortcut::hardcodedDefaultShortcut( KStandardShortcut::Home )), QLatin1String( "Alt+Home; Home Page" ) );
}

void KStandardShortcutTest::testName()
{
    QCOMPARE( KStandardShortcut::name( KStandardShortcut::BeginningOfLine ), QLatin1String( "BeginningOfLine" ) );
    QCOMPARE( KStandardShortcut::name( KStandardShortcut::EndOfLine ), QLatin1String( "EndOfLine" ) );
    QCOMPARE( KStandardShortcut::name( KStandardShortcut::Home ), QLatin1String( "Home" ) );
}

void KStandardShortcutTest::testLabel()
{
    // Tests run in English, right?
    QCOMPARE( KStandardShortcut::label( KStandardShortcut::FindNext ), QLatin1String( "Find Next" ) );
    QCOMPARE( KStandardShortcut::label( KStandardShortcut::Home ), QLatin1String( "Home" ) );
}

void KStandardShortcutTest::testShortcut()
{
    QCOMPARE( QKeySequence::listToString(KStandardShortcut::shortcut( KStandardShortcut::ZoomIn )), QKeySequence::listToString(KStandardShortcut::zoomIn()) );
}

void KStandardShortcutTest::testFindStdAccel()
{
    QCOMPARE( KStandardShortcut::find( QString( "Ctrl+F" ) ), KStandardShortcut::Find );
    QCOMPARE( KStandardShortcut::find( QString( "Ctrl+Shift+Alt+G" ) ), KStandardShortcut::AccelNone );
}

