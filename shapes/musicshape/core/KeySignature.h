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
#ifndef MUSIC_CORE_KEYSIGNATURE_H
#define MUSIC_CORE_KEYSIGNATURE_H

#include "MusicElement.h"

namespace MusicCore {

class Staff;

/**
 * This class represents a key signature.
 */
class KeySignature : public MusicElement {
public:
    /**
     * Create a new key signature instance.
     *
     * @param accidentals the number of accidentals in this key signature, positive values for sharps, negative values
     * for flats.
     */
    explicit KeySignature(int accidentals);

    /**
     * Create a new key signature instance that should be placed on a specific staff.
     */
    KeySignature(Staff* staff, int accidentals);

    /**
     * Destructor.
     */
    virtual ~KeySignature();

    /**
     * Returns the number of accidentals in this key signature. Returns a positive value for sharps or a negative value
     * for flats.
     */
    int accidentals() const;

    /**
     * Sets the number of accidentals in this key signature. Use positive values for sharps and negative values for
     * flats.
     *
     * @param accidentals the new accidentals for this key signature
     */
    void setAccidentals(int accidentals);

    /**
     * Returns the accidentals for a note with the given pitch.
     *
     * @param pitch the pitch of the note for which to return the accidentals
     */
    int accidentals(int pitch) const;
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CLEF_KEYSIGNATURE_H
