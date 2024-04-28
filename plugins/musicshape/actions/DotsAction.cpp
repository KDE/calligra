/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "DotsAction.h"

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

#include "../commands/AddDotCommand.h"

#include <KoIcon.h>

#include "MusicDebug.h"
#include <KLocalizedString>

#include <math.h>

using namespace MusicCore;

DotsAction::DotsAction(SimpleEntryTool *tool)
    : AbstractNoteMusicAction(koIcon("music-dottednote"), i18n("Dots"), tool)
{
}

void DotsAction::mousePress(Chord *chord, Note *note, qreal distance, const QPointF &pos)
{
    Q_UNUSED(note);
    Q_UNUSED(pos);

    if (!chord)
        return;
    if (distance > 10)
        return; // bah, magic numbers are ugly....

    m_tool->addCommand(new AddDotCommand(m_tool->shape(), chord));
}
