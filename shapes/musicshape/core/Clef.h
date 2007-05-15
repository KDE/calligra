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
#ifndef MUSIC_CORE_CLEF_H
#define MUSIC_CORE_CLEF_H

#include "MusicElement.h"

namespace MusicCore {

class Staff;

/**
 * This class represents a clef music element. This same class is used both for clef changes at the start of bars as for
 * mid-bar clef changes.
 */
class Clef : public MusicElement {
public:
    /**
     * The various supported shapes for a clef. Currently ownly G, F and C clefs are supported.
     */
    enum ClefShape {
        GClef,
        FClef,
        CClef,
        Trebble = GClef,    ///< Convenience value if you can't remember what clef a trebble clef is
        Bass = FClef,       ///< Convenience value if you can't remember what clef a bass clef is
        Alto = CClef,       ///< Convenience value if you can't remember what clef an alto clef is
        Tenor = CClef,      ///< Convenience value if you can't remember what clef a tenor clef is
        Soprano = CClef     ///< Convenience value if you can't remember what clef a soprano clef is
    };

    /**
     * Create a new clef instance, not specifiying on which staff to add the clef. To add the clef to a staff, later
     * call the setStaff method. The clef will have the given shape, and will be drawn on the given line. Optionally
     * you can also specify a number of octaves the notes should be shifted from what is shown.
     *
     * @param shape the shape of the new clef
     * @param line the line at which to draw the clef
     * @param octaveChange the octaveChange to apply to notes following this clef
     */
    Clef(ClefShape shape, int line, int octaveChange = 0);
    
    /**
     * Convenience constructor that removes the need to call setStaff after creating the clef to set the staff.
     *
     * @param staff the staff on which the clef should be shown
     * @param shape the shape of the new clef
     * @param line the line at which to draw the clef
     * @param octaveChange the octaveChange to apply to notes following this clef
     */
    Clef(Staff* staff, ClefShape shape, int line, int octaveChange = 0);
    
    /**
     * Destructor.
     */
    virtual ~Clef();

    /**
     * Returns the shape of the clef.
     */
    ClefShape shape() const;
    
    /**
     * Changes the shape of the clef.
     *
     * @param shape the new shape of the clef
     */
    void setShape(ClefShape shape);
    
    /**
     * Returns the line the clef is displayed on.
     */
    int line() const;
    
    /**
     * Changes the line the clef is displayed on. The bottom line is 1, each higher line +1.
     *
     * @param line the new line this clef is displayed on
     */
    void setLine(int line);
    
    /**
     * Returns the octave change of this clef.
     */
    int octaveChange() const;
    
    /**
     * Changes the octave change of this clef.
     *
     * @param octaveChange the new octave change for this clef
     */
    void setOctaveChange(int octaveChange) const;

    /**
     * Returns the pitch for a given line.
     */
    int lineToPitch(int line) const;
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CLEF_PART_H
