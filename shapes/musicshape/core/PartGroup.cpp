/* This file is part of the KDE project
 * Copyright (C) 2007 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "PartGroup.h"
#include "Sheet.h"

namespace MusicCore {

class PartGroup::Private
{
public:
    Sheet* sheet;
    GroupSymbol symbol;
    QString name;
    int firstPart;
    int lastPart;
    bool commonBarLines;
};

PartGroup::PartGroup(Sheet* sheet, int firstPart, int lastPart) : d(new Private)
{
    d->sheet = sheet;
    d->symbol = DefaultSymbol;
    d->firstPart = firstPart;
    d->lastPart = lastPart;
}

PartGroup::~PartGroup()
{
    delete d;
}

Sheet* PartGroup::sheet()
{
    return d->sheet;
}

int PartGroup::firstPart() const
{
    return d->firstPart;
}

void PartGroup::setFirstPart(int index)
{
    Q_ASSERT( index >= 0 && index < sheet()->partCount() );
    d->firstPart = index;
}

int PartGroup::lastPart() const
{
    return d->lastPart;
}

void PartGroup::setLastPart(int index)
{
    Q_ASSERT( index >= 0 && index < sheet()->partCount() );
    d->lastPart = index;
}

QString PartGroup::name() const
{
    return d->name;
}

PartGroup::GroupSymbol PartGroup::symbol() const
{
    return d->symbol;
}

void PartGroup::setSymbol(GroupSymbol symbol)
{
    d->symbol = symbol;
}

bool PartGroup::commonBarLines() const
{
    return d->commonBarLines;
}

void PartGroup::setCommonBarLines(bool commonBarLines)
{
    d->commonBarLines = commonBarLines;
}

} // namespace MusicCore
