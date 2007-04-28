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
#include "Part.h"
#include "Staff.h"
#include "Voice.h"
#include <QtCore/QList>

namespace MusicCore {

class Part::Private
{
public:
    Sheet* sheet;
    QString name;
    QString shortName;
    QList<Staff*> staves;
    QList<Voice*> voices;
};

Part::Part(Sheet* sheet, QString name) : d(new Private)
{
    d->sheet = sheet;
    d->name = name;
}

Part::~Part()
{
    delete d;
}

Sheet* Part::sheet()
{
    return d->sheet;
}

QString Part::name() const
{
    return d->name;
}

void Part::setName(QString name)
{
    d->name = name;
}

QString Part::shortName() const
{
    if (d->shortName.isNull()) {
        return d->name;
    } else {
        return d->shortName;
    }
}

void Part::setShortName(QString name)
{
    d->shortName = name;
}

int Part::staffCount() const
{
    return d->staves.size();
}

Staff* Part::staff(int index)
{
    Q_ASSERT( index >= 0 && index < staffCount() );
    return d->staves[index];
}

Staff* Part::addStaff()
{
    Staff* staff = new Staff(this);
    d->staves.append(staff);
    return staff;
}

Staff* Part::insertStaff(int before)
{
    Q_ASSERT( before >= 0 && before <= staffCount() );
    Staff* staff = new Staff(this);
    d->staves.insert(before, staff);
    return staff;
}

int Part::voiceCount() const
{
    return d->voices.size();
}

Voice* Part::voice(int index)
{
    Q_ASSERT( index >= 0 && index < voiceCount() );
    return d->voices[index];
}

Voice* Part::addVoice()
{
    Voice* voice = new Voice(this);
    d->voices.append(voice);
    return voice;
}

} // namespace
