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
#ifndef MUSIC_CORE_MUSICELEMENT_H
#define MUSIC_CORE_MUSICELEMENT_H

namespace MusicCore {

class VoiceBar;
class Staff;

/**
 * This is the base class for all musical elements that can be added to a bar.
 */
class MusicElement
{
public:
    /**
     * Creates a new MusicElement.
     */
    explicit MusicElement(int length = 0);

    /**
     * Destructor.
     */
    virtual ~MusicElement();

    /**
     * Returns the staff this music element should be displayed on. It can also be NULL, for example if the element
     * should not be visible.
     */
    Staff* staff();

    /**
     * Sets the staff this element should be displayed on.
     *
     * @param staff the new staff this element should be displayed on
     */
    void setStaff(Staff* staff);

    /**
     * Returns the x position of this musical element. The x position of an element is measured relative to the left
     * barline of the bar the element is in.
     */
    double x() const;

    /**
     * Sets the x position of this musical element.
     */
    void setX(double x);

    /**
     * Returns the y position of this musical element. The y position of an element is measure relative to the center
     * of the staff it is in, although some musical elements that have a notion of pitch such as notes/rests/clefs or
     * key signatures might have a different reference point.
     */
    double y() const;

    /**
     * Sets the y position of this musical element.
     */
    void setY(double y);

    /**
     * Returns the width of this musical element.
     */
    double width() const;

    /**
     * Returns the height of this musical element.
     */
    double height() const;

    /**
     * Returns the duration of this musical elements in ticks.
     */
    int length() const;

    /**
     * This enum contains constants representing the lengths of various notes. The length of a 128th note is
     * 1*2*3*4*5*7 ticks to allow n-tuples with n from 3..10. The length of the other notes are mutliples of
     * this number. All durations and time-stamps are calculated in these units.
     */
    enum NoteLength {
        Note128Length = 1*2*3*4*5*7,
        Note64Length = Note128Length * 2,
        Note32Length = Note64Length * 2,
        Note16Length = Note32Length * 2,
        Note8Length = Note16Length * 2,
        QuarterLength = Note8Length * 2,
        HalfLength = QuarterLength * 2,
        WholeLength = HalfLength * 2,
        DoubleWholeLength = WholeLength * 2
    };
protected:
    /**
     * Changes the duration of this musical element.
     *
     * @param length the new duration of this musical element
     */
    void setLength(int length);

    /**
     * Sets the width of this musical element.
     *
     * @param width the new width of this musical element
     */
    void setWidth(double width);

    /**
     * Sets the height of this musical element.
     *
     * @param height the new height of this musical element
     */
    void setHeight(double height);
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_MUSICELEMENT_H
