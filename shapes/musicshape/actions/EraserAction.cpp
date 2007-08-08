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
#include "EraserAction.h"

#include "../SimpleEntryTool.h"
#include "../MusicShape.h"
#include "../Renderer.h"

#include "../core/Chord.h"
#include "../core/Note.h"
#include "../core/VoiceBar.h"
#include "../core/Clef.h"
#include "../core/Voice.h"
#include "../core/Sheet.h"
#include "../core/Bar.h"
#include "../core/Part.h"
#include "../core/Staff.h"

#include "../commands/RemoveNoteCommand.h"
#include "../commands/RemoveChordCommand.h"

#include <kicon.h>
#include <kdebug.h>
#include <klocale.h>

#include <math.h>

using namespace MusicCore;

EraserAction::EraserAction(SimpleEntryTool* tool)
    : AbstractMusicAction(KIcon("eraser"), i18n("Eraser"), tool)
{
}

void EraserAction::renderPreview(QPainter& painter, const QPointF& point)
{
}

inline static double sqr(double a) { return a*a; }

void EraserAction::mousePress(Staff* staff, int barIdx, const QPointF& pos)
{
    Part* part = staff->part();
    Sheet* sheet = part->sheet();
    Bar* bar = sheet->bar(barIdx);

    Clef* clef = staff->lastClefChange(barIdx, 0);

    double realX = pos.x() / bar->scale();

    // loop over all noteheads
    double closestDist = 1e9;
    Note* closestNote;
    Chord* chord = 0;

    // outer loop, loop over all voices
    for (int v = 0; v < part->voiceCount(); v++) {
        Voice* voice = part->voice(v);
        VoiceBar* vb = voice->bar(bar);

        // next loop over all chords
        for (int e = 0; e < vb->elementCount(); e++) {
            Chord* c = dynamic_cast<Chord*>(vb->element(e));
            if (!c) continue;

            double centerX = c->x() + (c->width() / 2);

            if (c->noteCount() == 0) {
                double centerY = c->y() + (c->height() / 2);
                double dist = sqrt(sqr(centerX - realX) + sqr(centerY - pos.y()));
                if (dist < closestDist) {
                    closestDist = dist;
                    closestNote = NULL;
                    chord = c;
                }
            }
            // lastly loop over all noteheads
            for (int n = 0; n < c->noteCount(); n++) {
                Note* note = c->note(n);
                if (note->staff() != staff) continue;

                int line = 14;
                if (clef->shape() == Clef::FClef) line = 4;
                line -= 2 * clef->line();
                line = line - note->pitch();
                double centerY = line * staff->lineSpacing() / 2;

                double dist = sqrt(sqr(centerX - realX) + sqr(centerY - pos.y()));
                if (dist < closestDist) {
                    closestDist = dist;
                    closestNote = note;
                    chord = c;
                }
            }
        }
    }

    if (!chord) return;
    if (closestDist > 10) return; // bah, magic numbers are ugly....

    if (closestNote && chord->noteCount() > 1) {
        m_tool->addCommand(new RemoveNoteCommand(m_tool->shape(), chord, closestNote));
    } else {
        m_tool->addCommand(new RemoveChordCommand(m_tool->shape(), chord));
    }
}
