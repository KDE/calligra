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
#include "Clef.h"

namespace MusicCore {

class Clef::Private {
public:
    ClefShape shape;
    int line;
    int octaveChange;
};

Clef::Clef(Clef::ClefShape shape, int line, int octaveChange) : d(new Private)
{
    d->shape = shape;
    d->line = line;
    d->octaveChange = octaveChange;
    setWidth(23);
}

Clef::Clef(Staff* staff, Clef::ClefShape shape, int line, int octaveChange) : d(new Private)
{
    d->shape = shape;
    d->line = line;
    d->octaveChange = octaveChange;
    setWidth(23);
    setStaff(staff);
}

Clef::~Clef()
{
    delete d;
}

Clef::ClefShape Clef::shape() const
{
    return d->shape;
}

void Clef::setShape(ClefShape shape)
{
    d->shape = shape;
}

int Clef::line() const
{
    return d->line;
}

void Clef::setLine(int line)
{
    d->line = line;
}

int Clef::octaveChange() const
{
    return d->octaveChange;
}

void Clef::setOctaveChange(int octaveChange) const
{
    d->octaveChange = octaveChange;
}

int Clef::lineToPitch(int line) const
{
    return line + 2 * d->line - 2;
}

} // namespace MusicCore

