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
#include "KeySignature.h"

namespace MusicCore {

class KeySignature::Private {
public:
    int accidentals[7];
};

KeySignature::KeySignature(int accidentals) : d(new Private)
{
    setAccidentals(accidentals);
}

KeySignature::KeySignature(Staff* staff, int accidentals) : d(new Private)
{
    setAccidentals(accidentals);
    setStaff(staff);
}

KeySignature::~KeySignature()
{
    delete d;
}

int KeySignature::accidentals() const
{
    int sum = 0;
    for (int i = 0; i < 7; i++) {
        sum += d->accidentals[i];
    }
    return sum;
}

void KeySignature::setAccidentals(int accidentals)
{
    // first zero the accidentals array
    for (int i = 0; i < 7; i++) {
        d->accidentals[i] = 0;
    }

    // now add sharps
    int idx = 3;
    for (int i = 0; i < accidentals; i++) {
        d->accidentals[idx]++;
        idx = (idx + 4) % 7;
    }

    // and flats
    idx = 6;
    for (int i = 0; i > accidentals; i--) {
        d->accidentals[idx]--;
        idx = (idx + 3) % 7;
    }
}

int KeySignature::accidentals(int pitch) const
{
    return d->accidentals[ ((pitch % 7) + 7) % 7 ];
}

} // namespace MusicCore

