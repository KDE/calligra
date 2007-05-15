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
#include "Engraver.h"
#include "core/Bar.h"
#include "core/Sheet.h"
#include "core/Voice.h"
#include "core/Part.h"
#include "core/VoiceBar.h"
#include "core/MusicElement.h"
#include "core/Clef.h"

#include <limits.h>

#include <QtCore/QList>
#include <QtCore/QVarLengthArray>

#include <kdebug.h>

using namespace MusicCore;

Engraver::Engraver()
{
}

void Engraver::engraveSheet(Sheet* sheet, bool engraveBars)
{
    if (engraveBars) {
        // engrave all bars in the sheet
        for (int i = 0; i < sheet->barCount(); i++) {
            engraveBar(sheet->bar(i));
        }
    }

    QPointF p(0, 0);
    for (int i = 0; i < sheet->barCount(); i++) {
        sheet->bar(i)->setPosition(p);
        p.setX(p.x() + sheet->bar(i)->size());
    }

    // now layout bars in staff systems
    // TODO
}

void Engraver::engraveBar(Bar* bar)
{
    Sheet* sheet = bar->sheet();

    // collect all voices in all parts
    QList<VoiceBar*> voices;
    for (int p = 0; p < sheet->partCount(); p++) {
        Part* part = sheet->part(p);
        for (int v = 0; v < part->voiceCount(); v++) {
            voices.append(bar->voice(part->voice(v)));
        }
    }

    QVarLengthArray<int> nextTime(voices.size());
    QVarLengthArray<int> nextIndex(voices.size());
    // initialize stuff to 0
    for (int i = 0; i < voices.size(); i++) {
        nextTime[i] = 0;
        nextIndex[i] = 0;
    }

    double x = 0; // this is the end position of the last placed elements
    // loop until all elements are placed
    for (;;) {
        // find earliest start time
        int time = INT_MAX;
        for (int i = 0; i < voices.size(); i++) {
            if (nextIndex[i] < voices[i]->elementCount()) {
                if (nextTime[i] < time) time = nextTime[i];
            }
        }
        // none found, break
        if (time == INT_MAX) break;

        double maxEnd = x;
        // now update all items with correct start time
        for (int i = 0; i < voices.size(); i++) {
            if (nextTime[i] == time && nextIndex[i] < voices[i]->elementCount()) {
                double xpos = x + 15;
                voices[i]->element(nextIndex[i])->setX(xpos);
                double xend = voices[i]->element(nextIndex[i])->width() + xpos;
                if (xend > maxEnd) maxEnd = xend;
                nextTime[i] += voices[i]->element(nextIndex[i])->length();
                nextIndex[i]++;
            }
        }

        x = maxEnd;
    }
    bar->setSize(x + 15);
}
