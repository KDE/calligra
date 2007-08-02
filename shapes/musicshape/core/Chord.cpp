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

Chord::Chord(Duration duration, int dots) : VoiceElement(), d(new Private)
{
    d->duration = duration;
    d->dots = dots;

    int baseLength = durationToTicks(duration);
    int length = baseLength;
    for (int i = 0; i < dots; i++) {
        length += baseLength >> (i+1);
    }
    setLength(length);
    setWidth(7 + (3 * dots + (dots ? 2 : 0)));
}

Chord::Chord(Staff* staff, Duration duration, int dots) : d(new Private)
{
    d->duration = duration;
    d->dots = dots;
    int baseLength = durationToTicks(duration);
    int length = baseLength;
    for (int i = 0; i < dots; i++) {
        length += baseLength >> (i+1);
    }
    setLength(length);
    setStaff(staff);
    setWidth(7 + (3 * dots + (dots ? 2 : 0)));
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
    if (d->duration == duration) return;
    d->duration = duration;
    int baseLength = durationToTicks(d->duration);
    int length = baseLength;
    for (int i = 0; i < d->dots; i++) {
        length += baseLength >> (i+1);
    }
    setLength(length);
    emit durationChanged(duration);
}

int Chord::dots() const
{
    return d->dots;
}

void Chord::setDots(int dots)
{
    if (d->dots == dots) return;
    d->dots = dots;
    int baseLength = durationToTicks(d->duration);
    int length = baseLength;
    for (int i = 0; i < dots; i++) {
        length += baseLength >> (i+1);
    }
    setLength(length);
    emit dotsChanged(dots);
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
    if (!this->staff()) setStaff(staff);
    return n;
}

void Chord::addNote(Note* note)
{
    if (!staff()) setStaff(note->staff());
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

void Chord::removeNote(Note* note, bool deleteNote)
{
    Q_ASSERT( note );
    int index = d->notes.indexOf(note);
    Q_ASSERT( index != -1 );
    removeNote(index, deleteNote);
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

QString Chord::durationToString(Duration duration)
{
    switch (duration) {
        case HundredTwentyEighth:   return "128th";
        case SixtyFourth:           return "64th";
        case ThirtySecond:          return "32nd";
        case Sixteenth:             return "16th";
        case Eighth:                return "eighth";
        case Quarter:               return "quarter";
        case Half:                  return "half";
        case Whole:                 return "whole";
        case Breve:                 return "breve";
    }
    return "[unknown note length]";
}

} // namespace MusicCore

#include "Chord.moc"
