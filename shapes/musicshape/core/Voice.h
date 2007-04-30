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
#ifndef MUSIC_CORE_VOICE_H
#define MUSIC_CORE_VOICE_H

#include <QtCore/QString>

namespace MusicCore {

class Part;
class VoiceBar;
class Bar;

/**
 * A voice contains the actual musical elements in a piece of music.
 */
class Voice
{
public:
    /**
     * Returns the part this voice is part of.
     */
    Part* part();

    /**
     * Returns the number of bars in this voice. This should always return the same as part()->sheet()->barCount().
     */
    int barCount() const;

    /**
     * Returns the bar with the given index in this voice.
     *
     * @param index the index of the bar to return.
     */
    VoiceBar* bar(int index);

    /**
     * Returns the bar in this voice that contains the elements in the given bar in this piece of music.
     *
     * @param bar the bar for which to return the VoiceBar instance.
     */
    VoiceBar* bar(Bar* bar);
private:
    Voice(Part* part);
    ~Voice();
    friend class Part;
    /**
     * This method is called by the part (and indirectly by the sheet) when bars are added to the piece.
     */
    void insertBars(int before, int count);
    /**
     * This method is called by the part (and indirectly by the sheet) when bars are removed from the piece. This
     * method makes sure the bars are also removed from this voice.
     */
    void removeBars(int index, int count);
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_VOICE_H
