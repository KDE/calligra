/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Note.h"
#include "Chord.h"

namespace MusicCore {

class Note::Private {
public:
    Staff* staff;
    int pitch;
    int accidentals;
    bool tied;
    bool drawAccidentals;
};

Note::Note(Chord* chord, Staff* staff, int pitch, int accidentals) : QObject(chord), d(new Private)
{
    d->staff = staff;
    d->pitch = pitch;
    d->accidentals = accidentals;
    d->tied = false;
    d->drawAccidentals = false;
}

Note::~Note()
{
    delete d;
}

Staff* Note::staff()
{
    return d->staff;
}

Chord* Note::chord()
{
    return qobject_cast<Chord*>(parent());
}

void Note::setStaff(Staff* staff)
{
    d->staff = staff;
}

int Note::pitch() const
{
    return d->pitch;
}

int Note::accidentals() const
{
    return d->accidentals;
}

void Note::setAccidentals(int accidentals)
{
    d->accidentals = accidentals;
}

bool Note::drawAccidentals() const
{
    return d->drawAccidentals;
}

void Note::setDrawAccidentals(bool drawAccidentals)
{
    d->drawAccidentals = drawAccidentals;
}

bool Note::isStartTie() const
{
    return d->tied;
}

void Note::setStartTie(bool startTie)
{
    d->tied = startTie;
}

} // namespace MusicCore
