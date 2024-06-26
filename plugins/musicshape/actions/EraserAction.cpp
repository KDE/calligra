/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "EraserAction.h"

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

#include "../commands/RemoveChordCommand.h"
#include "../commands/RemoveNoteCommand.h"
#include "../commands/RemoveStaffElementCommand.h"

#include <KoIcon.h>

#include "MusicDebug.h"
#include <KLocalizedString>

#include <math.h>

using namespace MusicCore;

EraserAction::EraserAction(SimpleEntryTool *tool)
    : AbstractNoteMusicAction(koIcon("draw-eraser"), i18n("Eraser"), tool)
{
}

void EraserAction::mousePress(Chord *chord, Note *note, qreal distance, const QPointF &pos)
{
    Q_UNUSED(pos);

    if (!chord)
        return;
    if (distance > 10)
        return;

    if (note && chord->noteCount() > 1) {
        m_tool->addCommand(new RemoveNoteCommand(m_tool->shape(), chord, note));
    } else {
        m_tool->addCommand(new RemoveChordCommand(m_tool->shape(), chord));
    }
}

void EraserAction::mousePress(StaffElement *se, qreal distance, const QPointF &pos)
{
    Q_UNUSED(pos);

    if (!se)
        return;
    if (distance > 10)
        return;

    Bar *bar = se->bar();
    Sheet *sheet = bar->sheet();
    // remove staff element
    if (bar != sheet->bar(0) || se->startTime() > 0) {
        // don't allow removal of staff elements at the start of the first bar
        m_tool->addCommand(new RemoveStaffElementCommand(m_tool->shape(), se, bar));
    }
}
