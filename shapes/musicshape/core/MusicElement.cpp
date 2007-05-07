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
#include "MusicElement.h"

namespace MusicCore {

class MusicElement::Private
{
public:
    Staff* staff;
    int length;
    double x;
    double y;
    double width;
    double height;
};

MusicElement::MusicElement(int length) : d(new Private)
{
    d->staff = 0;
    d->length = length;
    d->x = 0;
    d->y = 0;
    d->width = 0;
    d->height = 0;
}

MusicElement::~MusicElement()
{
    delete d;
}

Staff* MusicElement::staff()
{
    return d->staff;
}

void MusicElement::setStaff(Staff* staff)
{
    d->staff = staff;
}

double MusicElement::x() const
{
    return d->x;
}

void MusicElement::setX(double x)
{
    d->x = x;
}

double MusicElement::y() const
{
    return d->y;
}

void MusicElement::setY(double y)
{
    d->y = y;
}

double MusicElement::width() const
{
    return d->width;
}

void MusicElement::setWidth(double width)
{
    d->width = width;
}

double MusicElement::height() const
{
    return d->height;
}

void MusicElement::setHeight(double height)
{
    d->height = height;
}

int MusicElement::length() const
{
    return d->length;
}

void MusicElement::setLength(int length)
{
    d->length = length;
}

} // namespace MusicCore
