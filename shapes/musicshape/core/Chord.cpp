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
#include "Chord.h"
#include "Note.h"
#include <QtCore/QList>

namespace MusicCore {

class Chord::Private {
public:
    Chord::Duration duration;
    int dots;
    QList<Note*> notes;
};

Chord::Chord(Duration duration, int dots) : d(new Private)
{
    d->duration = duration;
    d->dots = dots;
}

Chord::Chord(Staff* staff, Duration duration, int dots) : d(new Private)
{
    d->duration = duration;
    d->dots = dots;
    setStaff(staff);
}

Chord::~Chord()
{
    delete d;
}

Chord::Duration Chord::duration() const
{
    return d->duration;
}

int Chord::dots() const
{
    return d->dots;
}

int Chord::noteCount() const
{
    return d->notes.size();
}

Note* Chord::note(int index)
{
    Q_ASSERT( index >= 0 && index < noteCount() );
    return d->notes[index];
}

Note* Chord::addNote(Staff* staff, int pitch, int accidentals)
{
    Note *n = new Note(this, staff, pitch, accidentals);
    d->notes.append(n);
    return n;
}


} // namespace MusicCore
