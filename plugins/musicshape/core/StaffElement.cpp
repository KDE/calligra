/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "StaffElement.h"

namespace MusicCore
{

class StaffElement::Private
{
public:
    Staff *staff;
    Bar *bar;
    int startTime;
    qreal x;
    qreal y;
    qreal width;
    qreal height;
};

StaffElement::StaffElement(Staff *staff, int startTime)
    : d(new Private)
{
    d->staff = staff;
    d->bar = nullptr;
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

Staff *StaffElement::staff()
{
    return d->staff;
}

Bar *StaffElement::bar()
{
    return d->bar;
}

void StaffElement::setBar(Bar *bar)
{
    d->bar = bar;
}

qreal StaffElement::x() const
{
    return d->x;
}

void StaffElement::setX(qreal x)
{
    if (d->x == x)
        return;
    d->x = x;
    Q_EMIT xChanged(x);
}

qreal StaffElement::y() const
{
    return d->y;
}

void StaffElement::setY(qreal y)
{
    if (d->y == y)
        return;
    d->y = y;
    Q_EMIT yChanged(y);
}

qreal StaffElement::width() const
{
    return d->width;
}

void StaffElement::setWidth(qreal width)
{
    if (d->width == width)
        return;
    d->width = width;
    Q_EMIT widthChanged(width);
}

qreal StaffElement::height() const
{
    return d->height;
}

void StaffElement::setHeight(qreal height)
{
    if (d->height == height)
        return;
    d->height = height;
    Q_EMIT heightChanged(height);
}

int StaffElement::startTime() const
{
    return d->startTime;
}

void StaffElement::setStartTime(int startTime)
{
    if (d->startTime == startTime)
        return;
    d->startTime = startTime;
    Q_EMIT startTimeChanged(startTime);
}

} // namespace MusicCore
