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
}

Part* Voice::part()
{
    return d->part;
}

VoiceBar* Voice::bar(Bar* bar)
{
    if (bar->index() >= d->bars.size()) {
        for (int i = d->bars.size(); i <= bar->index(); i++) {
            VoiceBar* vb = new VoiceBar(this, part()->sheet()->bar(i));
            d->bars.append(vb);
        }
    }
    return d->bars[bar->index()];
}

} // namespace MusicCore
