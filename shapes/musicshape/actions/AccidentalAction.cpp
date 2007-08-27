/* This file is part of the KDE project
 * Copyright 2007 Marijn Kruisselbrink <m.Kruisselbrink@student.tue.nl>
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
#include "AccidentalAction.h"

#include "../SimpleEntryTool.h"
#include "../MusicShape.h"
#include "../Renderer.h"

#include "../core/Staff.h"
#include "../core/Part.h"
#include "../core/Sheet.h"
#include "../core/Bar.h"
#include "../core/VoiceBar.h"
#include "../core/Chord.h"
#include "../core/Note.h"
#include "../core/Voice.h"
#include "../core/Clef.h"

#include "../commands/SetAccidentalsCommand.h"

#include <kicon.h>
#include <kdebug.h>
#include <klocale.h>

#include <math.h>

using namespace MusicCore;

static QIcon getIcon(int accidentals)
{
    switch (accidentals) {
        case -2: return KIcon("music-doubleflat");
        case -1: return KIcon("music-flat");
        case 0:  return KIcon("music-natural");
        case 1:  return KIcon("music-cross");
        case 2:  return KIcon("music-doublecross");
    }
    return KIcon();
}

static QString getText(int accidentals)
{
    switch (accidentals) {
        case -2: return i18n("Double flat");
        case -1: return i18nc("lowered half a step", "Flat");
        case 0:  return i18n("Natural");
        case 1:  return i18nc("raised half a step", "Sharp");
        case 2:  return i18n("Double sharp");
    }
    if (accidentals < 0) {
        return i18n("%1 flats", -accidentals);
    } else {
        return i18n("%1 sharps", accidentals);
    }
}

AccidentalAction::AccidentalAction(int accidentals, SimpleEntryTool* tool)
    : AbstractMusicAction(getIcon(accidentals), getText(accidentals), tool)
    , m_accidentals(accidentals)
{
}

void AccidentalAction::renderPreview(QPainter& painter, const QPointF& point)
{
    m_tool->shape()->renderer()->renderAccidental(painter, m_accidentals, point, Qt::gray);
}

static inline double sqr(double a)
{
    return a * a;
}

void AccidentalAction::mousePress(Staff* staff, int barIdx, const QPointF& pos)
{
    Part* part = staff->part();
    Sheet* sheet = part->sheet();
    Bar* bar = sheet->bar(barIdx);

    Clef* clef = staff->lastClefChange(barIdx, 0);

    // loop over all noteheads
    double closestDist = 1e9;
    Note* closestNote = 0;

    // outer loop, loop over all voices
    for (int v = 0; v < part->voiceCount(); v++) {
        Voice* voice = part->voice(v);
        VoiceBar* vb = voice->bar(bar);

        // next loop over all chords
        for (int e = 0; e < vb->elementCount(); e++) {
            Chord* c = dynamic_cast<Chord*>(vb->element(e));
            if (!c) continue;

            double centerX = c->x() + (c->width() / 2);
            // lastly loop over all noteheads
            for (int n = 0; n < c->noteCount(); n++) {
                Note* note = c->note(n);
                if (note->staff() != staff) continue;

                int line = clef->pitchToLine(note->pitch());
                double centerY = line * staff->lineSpacing() / 2;

                double dist = sqrt(sqr(centerX - pos.x()) + sqr(centerY - pos.y()));
                if (dist < closestDist) {
                    closestDist = dist;
                    closestNote = note;
                }
            }
        }
    }

    if (!closestNote) return;
    if (closestDist > 15) return; // bah, magic numbers are ugly....

    m_tool->addCommand(new SetAccidentalsCommand(m_tool->shape(), closestNote, m_accidentals));
}
