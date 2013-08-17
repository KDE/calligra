/* This file is part of the KDE libraries
    Copyright (C) 2007 Thomas Braxton (kde.braxton@gmail.com)

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

#include "kentrymaptest.h"

#include <QtTest>
#include "kconfigdata.h"


const QByteArray group1("A Group");
const QByteArray key1("A Key");
const QByteArray key2("Another Key");
const QByteArray value1("A value");
const QByteArray value2("A different value");

QTEST_MAIN( KEntryMapTest )

void KEntryMapTest::testKeyOrder()
{
    const KEntryKey groupMarker(group1);
    const KEntryKey entry(group1, key1);
    const KEntryKey localized(group1, key1, true, false);
    const KEntryKey localizedDefault(group1, key1, true, true);
    const KEntryKey defaultEntry(group1, key1, false, true);

    // group marker should come before all entries
    QVERIFY(groupMarker < entry);
    QVERIFY(groupMarker < defaultEntry);
    QVERIFY(groupMarker < localized);
    QVERIFY(groupMarker < localizedDefault);

    // localized should come before entry
    QVERIFY(localized < entry);

    // localized-default should come after localized entry
    QVERIFY(localized < localizedDefault);

    // localized-default should come before non-localized entry
    QVERIFY(localizedDefault < entry);

    // default should come after entry
    QVERIFY(entry < defaultEntry);
}

void KEntryMapTest::testSimple()
{
    KEntryMap map;

    map.setEntry(group1, key1, value1, EntryOptions());
    QCOMPARE(map.size(), 2); // the group marker & 1 key
    map.setEntry(group1, key2, value2, EntryOptions());
    QCOMPARE(map.size(), 3); // the group marker & 2 keys

    QVERIFY(map.findEntry(group1) != map.end());
    QVERIFY(map.findEntry(group1.toLower()) == map.end());

    QVERIFY(map.findEntry(group1, key1) != map.end());
    QVERIFY(map.findEntry(group1, key1.toLower()) == map.end());
    QVERIFY(map.findEntry(group1, key2) != map.end());
    QVERIFY(map.findEntry(group1, key2.toUpper()) == map.end());

    QByteArray found = map.findEntry(group1, key1)->mValue;
    QVERIFY(found == value1);
    QVERIFY(found != value2);

    found = map.findEntry(group1, key2)->mValue;
    QVERIFY(found != value1);
    QVERIFY(found == value2);
}

void KEntryMapTest::testDirty()
{
    KEntryMap map;
    bool ret = map.setEntry(group1, key1, value1, EntryDefault);
    QCOMPARE(ret, true);
    ret = map.setEntry(group1, key1, value1, EntryDefault);
    QCOMPARE(ret, false);
    ret = map.setEntry(group1, key2, value2, EntryOptions());
    QCOMPARE(ret, true);
    ret = map.setEntry(group1, key2, value2, EntryOptions());
    QCOMPARE(ret, false);
}

void KEntryMapTest::testDefault()
{
    KEntryMap map;

    map.setEntry(group1, key1, value1, EntryDefault);
    QCOMPARE(map.size(), 3); // group marker, default, entry
    map.setEntry(group1, key2, value2, EntryOptions());
    QCOMPARE(map.size(), 4); // group marker, default1, entry1, entry2

    const KEntryMap::ConstIterator defaultEntry(map.findEntry(group1, key1, SearchDefaults));
    const KEntryMap::ConstIterator entry1(map.findEntry(group1, key1));
    const KEntryMap::ConstIterator entry2(map.findEntry(group1, key2));

    // default set for entry1
    QVERIFY(defaultEntry != map.constEnd());
    QCOMPARE(defaultEntry->mValue, entry1->mValue);

    // no default set for entry2
    QVERIFY(map.findEntry(group1, key2, SearchDefaults) == map.end());

    // change from default
    map.setEntry(group1, key1, value2, EntryOptions());
    QVERIFY(defaultEntry->mValue != entry1->mValue);
    QVERIFY(entry1 != entry2);
    QCOMPARE(entry1->mValue, entry2->mValue);

    // revert entry1
    map.revertEntry(group1, key1);
    QCOMPARE(defaultEntry->mValue, entry1->mValue);

    // revert entry2, no default --> should be marked as deleted
    map.revertEntry(group1, key2);
    QCOMPARE(entry2->mValue, QByteArray());
    QVERIFY(entry2->bDirty);
    QVERIFY(entry2->bReverted);
}

void KEntryMapTest::testDelete()
{
    KEntryMap map;

    map.setEntry(group1, key1, value1, EntryDefault);
    map.setEntry(group1, key2, value2, EntryDefault);
    QCOMPARE(map.size(), 5);

    map.setEntry(group1, key2, QByteArray(), EntryDeleted|EntryDirty);
    QCOMPARE(map.size(), 5); // entry should still be in map, so it can override merged entries later
    QCOMPARE(map.findEntry(group1, key2)->mValue, QByteArray());
}

void KEntryMapTest::testGlobal()
{
    KEntryMap map;

    map.setEntry(group1, key1, value1, EntryGlobal);
    QCOMPARE(map.findEntry(group1, key1)->bGlobal, true);

    // this should create a new key that is not "global"
    map.setEntry(group1, key1, value2, EntryOptions());
    QVERIFY(!map.findEntry(group1, key1)->bGlobal);
}

void KEntryMapTest::testImmutable()
{
    KEntryMap map;

    map.setEntry(group1, key1, value1, EntryImmutable);
    QCOMPARE(map.findEntry(group1, key1)->bImmutable, true); // verify the immutable bit was set

    map.setEntry(group1, key1, value2, EntryOptions());
    QCOMPARE(map.findEntry(group1, key1)->mValue, value1); // verify the value didn't change

    map.clear();

    map.setEntry(group1, QByteArray(), QByteArray(), EntryImmutable);
    QCOMPARE(map.findEntry(group1)->bImmutable, true); // verify the group is immutable

    map.setEntry(group1, key1, value1, EntryOptions()); // should be ignored since the group is immutable
    QVERIFY(map.findEntry(group1, key1) == map.end());
}

void KEntryMapTest::testLocale()
{
    const QByteArray translatedDefault("hola");
    const QByteArray translated("bonjour");
    const QByteArray untranslated("hello");
    KEntryMap map;

    map.setEntry(group1, key1, untranslated, EntryDefault);
    QCOMPARE(map.findEntry(group1, key1)->mValue, untranslated);
    QCOMPARE(map.findEntry(group1, key1, SearchLocalized)->mValue, untranslated); // no localized value yet

    map.setEntry(group1, key1, translated, EntryLocalized);

    QCOMPARE(map.findEntry(group1, key1, SearchLocalized)->mValue, translated); // has localized value now
    QVERIFY(map.findEntry(group1, key1, SearchLocalized)->mValue != map.findEntry(group1, key1)->mValue);
    QCOMPARE(map.findEntry(group1, key1, SearchDefaults|SearchLocalized)->mValue, untranslated); // default should still be untranslated

    map.setEntry(group1, key1, translatedDefault, EntryDefault|EntryLocalized);
    QCOMPARE(map.findEntry(group1, key1, SearchLocalized)->mValue, translatedDefault);
    map.setEntry(group1, key1, translated, EntryLocalized); // set the translated entry to a different locale
    QCOMPARE(map.findEntry(group1, key1, SearchLocalized)->mValue, translated);
}
