/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TiedNoteAction.h"

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

#include "../commands/ToggleTiedNoteCommand.h"

#include <KoIcon.h>

#include <KLocalizedString>

#include <math.h>

using namespace MusicCore;

TiedNoteAction::TiedNoteAction(SimpleEntryTool *tool)
    : AbstractNoteMusicAction(koIcon("music-tiednote"), i18n("Tied notes"), tool)
{
}

void TiedNoteAction::mousePress(Chord *chord, Note *note, qreal distance, const QPointF &pos)
{
    Q_UNUSED(chord);
    Q_UNUSED(pos);

    if (!note)
        return;
    if (distance > 15)
        return; // bah, magic numbers are ugly....

    m_tool->addCommand(new ToggleTiedNoteCommand(m_tool->shape(), note));
}
