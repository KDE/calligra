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

class MusicElement
{
public:
    MusicElement();
    virtual ~MusicElement();

    VoiceBar* bar();
    Staff* staff();
    void setStaff(Staff* staff);
private:
    void setBar(VoiceBar* bar);
    friend class VoiceBar;
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_COREMUSICELEMENT_H
