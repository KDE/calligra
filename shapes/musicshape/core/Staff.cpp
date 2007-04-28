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
#include "Staff.h"
#include "Part.h"

namespace MusicCore {

class Staff::Private
{
public:
    Part* part;
    double spacing;
};

Staff::Staff(Part* part) : d(new Private)
{
    d->part = part;
    d->spacing = 60;
}

Staff::~Staff()
{
    delete d;
}

Part* Staff::part()
{
    return d->part;
}

double Staff::spacing() const
{
    return d->spacing;
}

void Staff::setSpacing(double spacing)
{
    d->spacing = spacing;
}

double Staff::top()
{
    for (int i = 0; i < part()->staffCount(); i++) {
        if (part()->staff(i) == this) return 30 + 100 * i;
    }
    Q_ASSERT(false);
    return 30;
}

int Staff::lineCount() const
{
    return 5;
}

double Staff::lineSpacing() const
{
    return 8;
}

} // namespace MusicCore
