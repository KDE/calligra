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
#ifndef MUSIC_CORE_CHORD_H
#define MUSIC_CORE_CHORD_H

#include "MusicElement.h"

namespace MusicCore {

class Note;

class Chord : public MusicElement
{
public:
    enum Duration {
        HundredTwentyEighth,
        SixtyFourth,
        ThirtySecond,
        Sixteenth,
        Eighth,
        Quarter,
        Half,
        Whole,
        Breve
    };

    Duration duration() const;
    int dots() const;

    int noteCount() const;
    Note* note(int index);
    Note* addNote(Staff* staff, int pitch, int accidentals = 0);

    Chord(Duration duration, int dots = 0);
    Chord(Staff* staff, Duration duration, int dots = 0);
    virtual ~Chord();
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_CHORD_H
