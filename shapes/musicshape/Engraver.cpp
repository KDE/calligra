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

#include <QtCore/QList>

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

    double maxX = 0;
    // now for a very simple engraver, simply layout all elements one after another
    Q_FOREACH(VoiceBar* vb, voices) {
        double x = 10;
        for (int i = 0; i < vb->elementCount(); i++) {
            vb->element(i)->setX(x);
            x += 30;
        }
        maxX = qMax(maxX, x);
    }

    bar->setSize(maxX + 20);
}
