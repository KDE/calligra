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
#ifndef MUSIC_CORE_BAR_H
#define MUSIC_CORE_BAR_H

namespace MusicCore {

class Sheet;
class Voice;
class VoiceBar;

/**
 * A bar (in the US also known as a measure) is a part of a piece of music. A piece of music is a two-dimensional
 * thing, with multiple staffs that are played concurrently and multiple bars that are played sequentially.
 */
class Bar
{
public:
    /**
     * Creates a new bar in the given sheet of music. The bar is not actually added to the sheet, to do that call
     * the addBar method of the sheet.
     *
     * @param sheet the sheet to create a bar for
     */
    explicit Bar(Sheet* sheet);

    /**
     * Destructor.
     */
    ~Bar();

    /**
     * Returns the sheet this bar is part of.
     */
    Sheet* sheet();

    /**
     * Changes the sheet this bar is part of. This method should not be called after the bar has been added to a sheet.
     */
    void setSheet(Sheet* sheet);

    /**
     * Returns a VoiceBar instance for a specific voice.
     *
     * @param voice the voice for which to return the VoiceBar
     */
    VoiceBar* voice(Voice* voice);

    /**
     * Returns the size of the bar.
     */
    double size() const;

    /**
     * Sets the size of the bar.
     *
     * @param size the new size of the bar
     */
    void setSize(double size);
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_PART_H
