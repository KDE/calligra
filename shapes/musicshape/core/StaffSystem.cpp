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
#include "StaffSystem.h"

namespace MusicCore {

class StaffSystem::Private
{
public:
    Sheet* sheet;
    double top;
    int firstBar;
};

StaffSystem::StaffSystem(Sheet* sheet)
    : d(new Private)
{
    d->sheet = sheet;
    d->top = 0.0;
    d->firstBar = 0;
}

StaffSystem::~StaffSystem()
{
    delete d;
}

double StaffSystem::top() const
{
    return d->top;
}

void StaffSystem::setTop(double top)
{
    d->top = top;
}

int StaffSystem::firstBar() const
{
    return d->firstBar;
}

void StaffSystem::setFirstBar(int bar)
{
    d->firstBar = bar;
}

} // namespace MusicCore

#include "StaffSystem.moc"
