/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "StaffSystem.h"
#include "Clef.h"
#include "Sheet.h"

namespace MusicCore
{

class StaffSystem::Private
{
public:
    qreal top;
    qreal height;
    int firstBar;
    qreal indent;
    qreal lineWidth;
    QList<Clef *> clefs;
};

StaffSystem::StaffSystem(Sheet *sheet)
    : QObject(sheet)
    , d(new Private)
{
    d->top = 0.0;
    d->height = 100.0;
    d->firstBar = 0;
    d->indent = 0;
    d->lineWidth = 100;
}

StaffSystem::~StaffSystem()
{
    delete d;
}

qreal StaffSystem::top() const
{
    return d->top;
}

qreal StaffSystem::height() const
{
    return d->height;
}

void StaffSystem::setHeight(qreal height)
{
    d->height = height;
}

void StaffSystem::setTop(qreal top)
{
    if (d->top == top)
        return;
    d->top = top;
    Q_EMIT topChanged(top);
}

int StaffSystem::firstBar() const
{
    return d->firstBar;
}

void StaffSystem::setFirstBar(int bar)
{
    if (d->firstBar == bar)
        return;
    d->firstBar = bar;
    Q_EMIT firstBarChanged(bar);
}

qreal StaffSystem::indent() const
{
    return d->indent;
}

void StaffSystem::setIndent(qreal indent)
{
    d->indent = indent;
}

void StaffSystem::setLineWidth(qreal width)
{
    d->lineWidth = width;
}

QList<Clef *> StaffSystem::clefs() const
{
    return d->clefs;
}

Clef *StaffSystem::clef(Staff *staff) const
{
    foreach (Clef *c, d->clefs) {
        if (c->staff() == staff) {
            return c;
        }
    }
    return nullptr;
}

void StaffSystem::setClefs(QList<Clef *> clefs)
{
    d->clefs = clefs;
}

} // namespace MusicCore
