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
#include "StaffElement.h"

namespace MusicCore {

class StaffElement::Private
{
public:
    Staff* staff;
    int startTime;
    double x;
    double y;
    double width;
    double height;
};

StaffElement::StaffElement(Staff* staff, int startTime) : d(new Private)
{
    d->staff = staff;
    d->startTime = startTime;
    d->x = 0;
    d->y = 0;
    d->width = 0;
    d->height = 0;
}

StaffElement::~StaffElement()
{
    delete d;
}

Staff* StaffElement::staff()
{
    return d->staff;
}

double StaffElement::x() const
{
    return d->x;
}

void StaffElement::setX(double x)
{
    d->x = x;
}

double StaffElement::y() const
{
    return d->y;
}

void StaffElement::setY(double y)
{
    d->y = y;
}

double StaffElement::width() const
{
    return d->width;
}

void StaffElement::setWidth(double width)
{
    d->width = width;
}

double StaffElement::height() const
{
    return d->height;
}

void StaffElement::setHeight(double height)
{
    d->height = height;
}

int StaffElement::startTime() const
{
    return d->startTime;
}

void StaffElement::setStartTime(int startTime)
{
    d->startTime = startTime;
}

} // namespace MusicCore
