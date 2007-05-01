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

class Clef : public MusicElement {
public:
    enum ClefShape {
        GClef,
        FClef,
        CClef,
        Trebble = GClef,
        Bass = FClef,
        Alto = CClef,
        Tenor = CClef,
        Soprano = CClef
    };

    Clef(ClefShape shape, int line, int octaveChange);
    Clef(Staff* staff, ClefShape shape, int line, int octaveChange);
    virtual ~Clef();

    ClefShape shape() const;
    void setShape(ClefShape shape);
    int line() const;
    void setLine(int line);
    int octaveChange() const;
    void setOctaveChange(int octaveChange) const;

private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CLEF_PART_H
