/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SelectionAction.h"

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

#include <KoIcon.h>

#include "MusicDebug.h"
#include <KLocalizedString>

#include <math.h>

using namespace MusicCore;

SelectionAction::SelectionAction(SimpleEntryTool *tool)
    : AbstractMusicAction(koIcon("select"), i18n("Select"), tool)
{
    m_firstBar = -1;
}

// inline static qreal sqr(qreal a) { return a*a; }

void SelectionAction::mousePress(Staff *staff, int barIdx, const QPointF &pos)
{
    Q_UNUSED(pos);
    /*Part* part = staff->part();
    Sheet* sheet = part->sheet();
    Bar* bar = sheet->bar(barIdx);

    // loop over all chords
    qreal closestDist = 1e9;
    Chord* chord = 0;

    // outer loop, loop over all voices
    for (int v = 0; v < part->voiceCount(); v++) {
        Voice* voice = part->voice(v);
        VoiceBar* vb = voice->bar(bar);

        // next loop over all chords
        for (int e = 0; e < vb->elementCount(); e++) {
            Chord* c = dynamic_cast<Chord*>(vb->element(e));
            if (!c) continue;

            qreal centerX = c->x() + (c->width() / 2);
            qreal centerY = c->y() + (c->height() / 2);
            qreal dist = sqrt(sqr(centerX - pos.x()) + sqr(centerY - pos.y()));
            if (dist < closestDist) {
                closestDist = dist;
                chord = c;
            }
        }
    }*/

    m_firstBar = barIdx;
    m_startStaff = staff;
    m_tool->setSelection(barIdx, barIdx, staff, staff);
}

void SelectionAction::mouseMove(Staff *staff, int barIdx, const QPointF &pos)
{
    Q_UNUSED(pos);
    m_tool->setSelection(qMin(m_firstBar, barIdx), qMax(m_firstBar, barIdx), m_startStaff, staff);
}
