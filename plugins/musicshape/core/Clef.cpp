/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Clef.h"

namespace MusicCore
{

class Clef::Private
{
public:
    ClefShape shape;
    int line;
    int octaveChange;
};

Clef::Clef(Staff *staff, int startTime, Clef::ClefShape shape, int line, int octaveChange)
    : StaffElement(staff, startTime)
    , d(new Private)
{
    d->shape = shape;
    d->line = line;
    d->octaveChange = octaveChange;
    setWidth(13);
}

Clef::~Clef()
{
    delete d;
}

int Clef::priority() const
{
    return 150;
}

Clef::ClefShape Clef::shape() const
{
    return d->shape;
}

void Clef::setShape(ClefShape shape)
{
    if (d->shape == shape)
        return;
    d->shape = shape;
    emit shapeChanged(shape);
}

int Clef::line() const
{
    return d->line;
}

void Clef::setLine(int line)
{
    if (d->line == line)
        return;
    d->line = line;
    emit lineChanged(line);
}

int Clef::octaveChange() const
{
    return d->octaveChange;
}

void Clef::setOctaveChange(int octaveChange)
{
    if (d->octaveChange == octaveChange)
        return;
    d->octaveChange = octaveChange;
    emit octaveChangeChanged(octaveChange);
}

int Clef::lineToPitch(int line) const
{
    int pitch = 0;
    switch (d->shape) {
    case GClef:
        pitch = 4;
        break;
    case FClef:
        pitch = -4;
        break;
    case CClef:
        pitch = 0;
        break;
    }
    // d->line is the line which has pitch 'pitch' (not counting spaces between lines)
    // 'line' is the position (including spaces between lines) of which to know the pitch
    return line - 2 * d->line + 2 + pitch;
}

int Clef::pitchToLine(int pitch) const
{
    int line = 0;
    switch (d->shape) {
    case GClef:
        line = 14;
        break;
    case FClef:
        line = 6;
        break;
    case CClef:
        line = 10;
        break;
    }
    line -= 2 * d->line;
    line -= pitch;
    return line;
}

} // namespace MusicCore
