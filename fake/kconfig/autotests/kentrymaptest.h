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

#ifndef KENTRYMAPTEST_H
#define KENTRYMAPTEST_H

#include <QtCore/QObject>
#include "kconfigdata.h"

class KEntryMapTest : public QObject
{
    Q_OBJECT

public:
    typedef KEntryMap::EntryOptions EntryOptions;
    typedef KEntryMap::SearchFlags SearchFlags;

    typedef KEntryMap::SearchFlag SearchFlag;
    static const SearchFlag SearchLocalized=KEntryMap::SearchLocalized;
    static const SearchFlag SearchDefaults=KEntryMap::SearchDefaults;

    typedef KEntryMap::EntryOption EntryOption;
    static const EntryOption EntryDirty=KEntryMap::EntryDirty;
    static const EntryOption EntryGlobal=KEntryMap::EntryGlobal;
    static const EntryOption EntryImmutable=KEntryMap::EntryImmutable;
    static const EntryOption EntryDeleted=KEntryMap::EntryDeleted;
    static const EntryOption EntryExpansion=KEntryMap::EntryExpansion;
    static const EntryOption EntryDefault=KEntryMap::EntryDefault;
    static const EntryOption EntryLocalized=KEntryMap::EntryLocalized;
private Q_SLOTS:
    void testKeyOrder();
    void testSimple();
    void testDirty();
    void testDefault();
    void testDelete();
    void testGlobal();
    void testImmutable();
    void testLocale();
};

#endif // KENTRYMAPTEST_H
