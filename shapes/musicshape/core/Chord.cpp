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

Chord::Chord(Duration duration, int dots) : MusicElement(), d(new Private)
{
    d->duration = duration;
    d->dots = dots;

    int baseLength = durationToTicks(duration);
    for (int i = 0; i < dots; i++) {
        baseLength += baseLength >> 1;
    }
    setLength(baseLength);
}

Chord::Chord(Staff* staff, Duration duration, int dots) : d(new Private)
{
    d->duration = duration;
    d->dots = dots;
    setStaff(staff);
}

Chord::~Chord()
{
    Q_FOREACH(Note* n, d->notes) delete n;
    delete d;
}

Chord::Duration Chord::duration() const
{
    return d->duration;
}

void Chord::setDuration(Duration duration)
{
    d->duration = duration;
}

int Chord::dots() const
{
    return d->dots;
}

void Chord::setDots(int dots)
{
    d->dots = dots;
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
    Note *n = new Note(staff, pitch, accidentals);
    d->notes.append(n);
    return n;
}

void Chord::addNote(Note* note)
{
    d->notes.append(note);
}

void Chord::removeNote(int index, bool deleteNote)
{
    Q_ASSERT( index >= 0 && index < noteCount() );
    Note* n = d->notes.takeAt(index);
    if (deleteNote) {
        delete n;
    }
}

int Chord::durationToTicks(Duration duration)
{
    switch (duration) {
        case HundredTwentyEighth: return Note128Length;
        case SixtyFourth:         return Note64Length;
        case ThirtySecond:        return Note32Length;
        case Sixteenth:           return Note16Length;
        case Eighth:              return Note8Length;
        case Quarter:             return QuarterLength;
        case Half:                return HalfLength;
        case Whole:               return WholeLength;
        case Breve:               return DoubleWholeLength;
    }
    return 0;
}

} // namespace MusicCore
