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
#include "Sheet.h"
#include "Part.h"
#include "PartGroup.h"
#include "Bar.h"
#include <QtCore/QList>

namespace MusicCore {

class Sheet::Private
{
public:
    QList<Part*> parts;
    QList<PartGroup*> partGroups;
    QList<Bar*> bars;
};

Sheet::Sheet() : d(new Private)
{
}

Sheet::~Sheet()
{
    delete d;
}

int Sheet::partCount() const
{
    return d->parts.size();
}

Part* Sheet::part(int index)
{
    Q_ASSERT( index >= 0 && index < partCount() );
    return d->parts[index];
}

Part* Sheet::addPart(QString name)
{
    Part* part = new Part(this, name);
    d->parts.append(part);
    return part;
}

Part* Sheet::insertPart(int before, QString name)
{
    Q_ASSERT( before >= 0 && before <= partCount() );
    Part* part = new Part(this, name);
    d->parts.insert(before, part);
    return part;
}

void Sheet::removePart(int index)
{
    Q_ASSERT( index >= 0 && index < partCount() );
    Part* part = d->parts.takeAt(index);
    delete part;
}

void Sheet::removePart(Part* part)
{
    Q_ASSERT( part && part->sheet() == this);
    int index = d->parts.indexOf(part);
    Q_ASSERT( index != -1 );
    removePart(index);
}

int Sheet::partGroupCount() const
{
    return d->partGroups.size();
}

PartGroup* Sheet::partGroup(int index)
{
    Q_ASSERT( index >= 0 && index < partGroupCount() );
    return d->partGroups[index];
}

PartGroup* Sheet::addPartGroup(int firstPart, int lastPart)
{
    Q_ASSERT( firstPart >= 0 && firstPart < partCount() );
    Q_ASSERT( lastPart >= 0 && lastPart < partCount() );
    PartGroup *group = new PartGroup( this, firstPart, lastPart );
    d->partGroups.append(group);
    return group;
}

void Sheet::removePartGroup(PartGroup* group)
{
    Q_ASSERT( group && group->sheet() == this );
    int index = d->partGroups.indexOf(group);
    Q_ASSERT( index != -1 );
    d->partGroups.removeAt(index);
    delete group;
}

int Sheet::barCount() const
{
    return d->bars.size();
}

Bar* Sheet::bar(int index)
{
    Q_ASSERT( index >= 0 && index < barCount() );
    return d->bars[index];
}

void Sheet::addBars(int count)
{
    for (int i = 0, bc = barCount(); i < count; i++) {
	d->bars.append(new Bar(this, bc + i));
    }
}

Bar* Sheet::addBar()
{
    Bar* bar = new Bar(this, barCount());
    d->bars.append(bar);
    return bar;
}

Bar* Sheet::insertBar(int before)
{
    Q_ASSERT( before >= 0 && before <= barCount() );
    Bar* bar = new Bar(this, before);
    d->bars.insert(before, bar);
    for (int i = before+1, count = barCount(); i < count; i++) {
        d->bars[i]->setIndex(i);
    }
    return bar;
}

void Sheet::removeBar(int index)
{
    Q_ASSERT( index >= 0 && index < barCount() );
    Bar* bar = d->bars.takeAt(index);
    delete bar;
    for (int i = index, count = barCount(); i < count; i++) {
        d->bars[i]->setIndex(i);
    }
}

void Sheet::removeBars(int index, int count)
{
    Q_ASSERT( index >= 0 && count > 0 && index + count <= barCount() );
    // XXX silly implementation, could really use improvement I think
    for (int i = 0; i < count; i++) {
        delete d->bars.takeAt(index);
    }
    for (int i = index, n = barCount(); i < n; i++) {
        d->bars[i]->setIndex(i);
    }
}

} // namespace MusicCore
