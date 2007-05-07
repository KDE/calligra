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
#include "TimeSignature.h"
#include <QtCore/QString>

namespace MusicCore {

class TimeSignature::Private {
public:
    int beats;
    int beat;
    TimeSignatureType type;
    Private() : beats(0), beat(0), type(Classical) {}
};

TimeSignature::TimeSignature(int beats, int beat, TimeSignatureType type) : d(new Private)
{
    setBeats(beats);
    setBeat(beat);
    d->type = type;
}

TimeSignature::TimeSignature(Staff* staff, int beats, int beat, TimeSignatureType type) : d(new Private)
{
    setBeats(beats);
    setBeat(beat);
    d->type = type;
    setStaff(staff);
}

TimeSignature::~TimeSignature()
{
    delete d;
}

int TimeSignature::beats() const
{
    return d->beats;
}

void TimeSignature::setBeats(int beats)
{
    d->beats = beats;
    int beatsLen = QString::number(d->beats).length();
    int beatLen = QString::number(d->beat).length();
    setWidth(15 * qMax(beatsLen, beatLen));
}

int TimeSignature::beat() const
{
    return d->beat;
}

void TimeSignature::setBeat(int beat)
{
    d->beat = beat;
    int beatsLen = QString::number(d->beats).length();
    int beatLen = QString::number(d->beat).length();
    setWidth(15 * qMax(beatsLen, beatLen));
}

TimeSignature::TimeSignatureType TimeSignature::type() const
{
    return d->type;
}

void TimeSignature::setType(TimeSignatureType type)
{
    d->type = type;
}

} // namespace MusicCore

