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
#include "Voice.h"
#include "VoiceBar.h"
#include "Bar.h"
#include "Part.h"
#include "Sheet.h"
#include <QtCore/QList>

namespace MusicCore {

class Voice::Private
{
public:
    Part* part;
    QList<VoiceBar*> bars;
};

Voice::Voice(Part* part) : d(new Private)
{
    d->part = part;
    for (int i = 0, c = part->sheet()->barCount(); i < c; i++) {
        d->bars.append(new VoiceBar(this, part->sheet()->bar(i)));
    }
}

Voice::~Voice()
{
    Q_FOREACH(VoiceBar* vb, d->bars) delete vb;
    delete d;
}

Part* Voice::part()
{
    return d->part;
}

int Voice::barCount() const
{
    return d->bars.size();
}

VoiceBar* Voice::bar(Bar* bar)
{
    return this->bar(bar->index());
}

VoiceBar* Voice::bar(int index)
{
    Q_ASSERT( index >= 0 && index < barCount() );
    VoiceBar* vb = d->bars[index];
    if (!vb) {
        vb = d->bars[index] = new VoiceBar(this, part()->sheet()->bar(index));
    }
    return vb;
}

void Voice::insertBars(int before, int count)
{
    for (int i = 0; i < count; i++) {
        d->bars.insert(before + i, new VoiceBar(this, part()->sheet()->bar(before + i)));
    }
}

void Voice::removeBars(int before, int count)
{
    for (int i = 0; i < count; i++) {
        delete d->bars.takeAt(before);
    }
}

} // namespace MusicCore
