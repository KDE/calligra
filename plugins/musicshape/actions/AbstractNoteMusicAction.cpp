/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "AbstractNoteMusicAction.h"

#include "../MusicShape.h"
#include "../Renderer.h"
#include "../SimpleEntryTool.h"

#include "../core/Bar.h"
#include "../core/Chord.h"
#include "../core/Clef.h"
#include "../core/Note.h"
#include "../core/Part.h"
#include "../core/Sheet.h"
#include "../core/Staff.h"
#include "../core/Voice.h"
#include "../core/VoiceBar.h"

#include <math.h>

using namespace MusicCore;

AbstractNoteMusicAction::AbstractNoteMusicAction(const QIcon &icon, const QString &text, SimpleEntryTool *tool)
    : AbstractMusicAction(icon, text, tool)
{
}

AbstractNoteMusicAction::AbstractNoteMusicAction(const QString &text, SimpleEntryTool *tool)
    : AbstractMusicAction(text, tool)
{
}

static inline qreal sqr(qreal a)
{
    return a * a;
}

void AbstractNoteMusicAction::mousePress(Staff *staff, int barIdx, const QPointF &pos)
{
    Part *part = staff->part();
    Sheet *sheet = part->sheet();
    Bar *bar = sheet->bar(barIdx);

    Clef *clef = staff->lastClefChange(barIdx, 0);

    // loop over all noteheads
    qreal closestDist = 1e9;
    Note *closestNote = nullptr;
    Chord *chord = nullptr;

    // outer loop, loop over all voices
    for (int v = 0; v < part->voiceCount(); v++) {
        Voice *voice = part->voice(v);
        VoiceBar *vb = voice->bar(bar);

        // next loop over all chords
        for (int e = 0; e < vb->elementCount(); e++) {
            Chord *c = dynamic_cast<Chord *>(vb->element(e));
            if (!c)
                continue;

            qreal centerX = c->x() + (c->width() / 2);

            // check if it is a rest
            if (c->noteCount() == 0) {
                qreal centerY = c->y() + (c->height() / 2);
                qreal dist = sqrt(sqr(centerX - pos.x()) + sqr(centerY - pos.y()));
                if (dist < closestDist) {
                    closestDist = dist;
                    closestNote = nullptr;
                    chord = c;
                }
            }

            // lastly loop over all noteheads
            for (int n = 0; n < c->noteCount(); n++) {
                Note *note = c->note(n);
                if (note->staff() != staff)
                    continue;

                int line = clef->pitchToLine(note->pitch());
                qreal centerY = line * staff->lineSpacing() / 2;

                qreal dist = sqrt(sqr(centerX - pos.x()) + sqr(centerY - pos.y()));
                if (dist < closestDist) {
                    closestDist = dist;
                    closestNote = note;
                    chord = c;
                }
            }
        }
    }

    StaffElement *se = nullptr;
    for (int e = 0; e < bar->staffElementCount(staff); e++) {
        StaffElement *elem = bar->staffElement(staff, e);
        qreal centerX = elem->x() + (elem->width() / 2);
        qreal centerY = elem->y() + (elem->height() / 2);
        qreal dist = sqrt(sqr(centerX - pos.x()) + sqr(centerY - pos.y()));
        if (dist < closestDist) {
            se = elem;
            closestDist = dist;
        }
    }

    if (se) {
        mousePress(se, closestDist, pos);
    } else {
        mousePress(chord, closestNote, closestDist, pos);
    }
}

void AbstractNoteMusicAction::mousePress(StaffElement *, qreal, const QPointF &)
{
    // empty default implementation
}

void AbstractNoteMusicAction::mouseMove(Staff *staff, int barIdx, const QPointF &pos)
{
    Part *part = staff->part();
    Sheet *sheet = part->sheet();
    Bar *bar = sheet->bar(barIdx);

    Clef *clef = staff->lastClefChange(barIdx, 0);

    // loop over all noteheads
    qreal closestDist = 1e9;
    Note *closestNote = nullptr;
    Chord *chord = nullptr;

    // outer loop, loop over all voices
    for (int v = 0; v < part->voiceCount(); v++) {
        Voice *voice = part->voice(v);
        VoiceBar *vb = voice->bar(bar);

        // next loop over all chords
        for (int e = 0; e < vb->elementCount(); e++) {
            Chord *c = dynamic_cast<Chord *>(vb->element(e));
            if (!c)
                continue;

            qreal centerX = c->x() + (c->width() / 2);

            // check if it is a rest
            if (c->noteCount() == 0) {
                qreal centerY = c->y() + (c->height() / 2);
                qreal dist = sqrt(sqr(centerX - pos.x()) + sqr(centerY - pos.y()));
                if (dist < closestDist) {
                    closestDist = dist;
                    closestNote = nullptr;
                    chord = c;
                }
            }

            // lastly loop over all noteheads
            for (int n = 0; n < c->noteCount(); n++) {
                Note *note = c->note(n);
                if (note->staff() != staff)
                    continue;

                int line = clef->pitchToLine(note->pitch());
                qreal centerY = line * staff->lineSpacing() / 2;

                qreal dist = sqrt(sqr(centerX - pos.x()) + sqr(centerY - pos.y()));
                if (dist < closestDist) {
                    closestDist = dist;
                    closestNote = note;
                    chord = c;
                }
            }
        }
    }

    StaffElement *se = nullptr;
    for (int e = 0; e < bar->staffElementCount(staff); e++) {
        StaffElement *elem = bar->staffElement(staff, e);
        qreal centerX = elem->x() + (elem->width() / 2);
        qreal centerY = elem->y() + (elem->height() / 2);
        qreal dist = sqrt(sqr(centerX - pos.x()) + sqr(centerY - pos.y()));
        if (dist < closestDist) {
            se = elem;
            closestDist = dist;
        }
    }

    if (se) {
        mouseMove(se, closestDist, pos);
    } else {
        mouseMove(chord, closestNote, closestDist, pos);
    }
}

void AbstractNoteMusicAction::mouseMove(Chord *, Note *, qreal, const QPointF &)
{
    // empty default implementation
}

void AbstractNoteMusicAction::mouseMove(StaffElement *, qreal, const QPointF &)
{
    // empty default implementation
}
