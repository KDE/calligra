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

/**
 * A Chord is used to represent one or more notes that have the same duration and starting time and are in the same
 * voice. A Chord is also used to represent rests, when it has no notes. When a chord does not have any notes its
 * staff should be set to know on which staff to display the rest. When the chord contains notes the notes can have
 * their own staff, so you could have one chord spreading multiple staves (with all staves in the same part).
 */
class Chord : public MusicElement
{
public:
    /**
     * This enum represents the various supported durations for chords/rests.
     */
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

    /**
     * Creates a new Chord instance, not specifying the staff on which the chord should be placed. Add this note to
     * a VoiceBar instance using the addElement method.
     *
     * @param duration the duration of the chord
     * @param dots the number of dots of the chord, each dot multiplies the length of the chord by 1.5
     */
    Chord(Duration duration, int dots = 0);
    
    /**
     * This constructor is overloaded for convenience, to avoid having to call the setStaff method to set the staff
     * when creating rests.
     *
     * @param staff the staff on which the chord should be placed
     * @param duration the duration of the chord
     * @param dots the number of dots of the chord, each dot multiplies the length of the chord by 1.5
     */
    Chord(Staff* staff, Duration duration, int dots = 0);
    
    /**
     * Destructor.
     */
    virtual ~Chord();
    
    /**
     * Returns the duration of the chord.
     */
    Duration duration() const;
    
    /**
     * Changes the duration of the chord.
     *
     * @param duration the new duration
     */
    void setDuration(Duration duration);
    
    /**
     * Returns the number of dots of this chord. Each dot multiplies the duration by a factor 1.5.
     */
    int dots() const;
    
    /**
     * Changes the number of dots of the chord.
     *
     * @param dots the new number of dots
     */
    void setDots(int dots);

    /**
     * Returns the number of notes in this chord.
     */
    int noteCount() const;
    
    /**
     * Returns the note at the given index in this chord.
     *
     * @param index the index of the note to return
     */
    Note* note(int index);
    
    /**
     * Adds a new note to this chord. The note will be drawn on the given staff and will have the given pitch and
     * accidentals.
     *
     * @param staff the staff the note should be drawn on
     * @param pitch the pitch of the new note
     * @param accidentals the number of accidentals of the note
     */
    Note* addNote(Staff* staff, int pitch, int accidentals = 0);

    /**
     * Adds an existing note to this chord. This will transfer ownership of the note to the chord. When the chord is
     * deleted, all notes in it are also deleted.
     *
     * @param note the note to add
     */
    void addNote(Note* note);
    
    /**
     * Removes a note from this chord. If deleteNote is true the note is not only removed, but also deleted.
     *
     * @param index the index of the note to remove
     * @param deleteNote should the note not only be removed, but also deleted
     */
    void removeNote(int index, bool deleteNote = true);
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_CHORD_H
