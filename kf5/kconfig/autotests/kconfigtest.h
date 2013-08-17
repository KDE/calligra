/* This file is part of the KDE libraries
    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)

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
#ifndef KCONFIGTEST_H
#define KCONFIGTEST_H

#include <QtCore/QObject>

class KConfigTest : public QObject
{
    Q_OBJECT
    Q_ENUMS(Testing)
    Q_FLAGS(Flags)

public:
    enum Testing { Ones=1, Tens=10, Hundreds=100};
    enum bits { bit0=1, bit1=2, bit2=4, bit3=8 };
    Q_DECLARE_FLAGS(Flags, bits)

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testSimple();
    void testDefaults();
    void testLists();
    void testLocale();
    void testEncoding();
    void testPath();
    void testPersistenceOfExpandFlagForPath();
    void testComplex();
    void testEnums();
    void testEntryMap();
    void testInvalid();
    void testDeleteEntry();
    void testDelete();
    void testDeleteWhenLocalized();
    void testDefaultGroup();
    void testEmptyGroup();
    void testCascadingWithLocale();
    void testMerge();
    void testImmutable();
    void testGroupEscape();
    void testRevertAllEntries();
    void testChangeGroup();
    void testGroupCopyTo();
    void testConfigCopyTo();
    void testConfigCopyToSync();
    void testReparent();
    void testAnonymousConfig();

    void testSubGroup();
    void testAddConfigSources();
    void testWriteOnSync();
    void testFailOnReadOnlyFileSync();
    void testDirtyOnEqual();
    void testDirtyOnEqualOverdo();
    void testCreateDir();
    void testSharedConfig();
    void testOptionOrder();
    void testLocaleConfig();
    void testDirtyAfterRevert();
    void testKdeGlobals();

    void testThreads();

    // should be last
    void testSyncOnExit();
private:
    QString m_xdgConfigHome;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(KConfigTest::Flags)

#endif /* KCONFIGTEST_H */
