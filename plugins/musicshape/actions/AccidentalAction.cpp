/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "AccidentalAction.h"

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

#include "../commands/SetAccidentalsCommand.h"

#include <KoIcon.h>

#include "MusicDebug.h"
#include <KLocalizedString>

#include <math.h>

using namespace MusicCore;

static QIcon getIcon(int accidentals)
{
    static const char *const iconNames[5] = {koIconNameCStr("music-doubleflat"),
                                             koIconNameCStr("music-flat"),
                                             koIconNameCStr("music-natural"),
                                             koIconNameCStr("music-cross"),
                                             koIconNameCStr("music-doublecross")};

    if ((-2 <= accidentals) && (accidentals <= 2)) {
        return QIcon::fromTheme(QLatin1String(iconNames[accidentals + 2]));
    }

    return QIcon();
}

static QString getText(int accidentals)
{
    switch (accidentals) {
    case -2:
        return i18n("Double flat");
    case -1:
        return i18nc("lowered half a step", "Flat");
    case 0:
        return i18n("Natural");
    case 1:
        return i18nc("raised half a step", "Sharp");
    case 2:
        return i18n("Double sharp");
    }
    if (accidentals < 0) {
        return i18n("%1 flats", -accidentals);
    } else {
        return i18n("%1 sharps", accidentals);
    }
}

AccidentalAction::AccidentalAction(int accidentals, SimpleEntryTool *tool)
    : AbstractNoteMusicAction(getIcon(accidentals), getText(accidentals), tool)
    , m_accidentals(accidentals)
{
}

void AccidentalAction::renderPreview(QPainter &painter, const QPointF &point)
{
    m_tool->shape()->renderer()->renderAccidental(painter, m_accidentals, point, Qt::gray);
}

void AccidentalAction::mousePress(Chord *chord, Note *note, qreal distance, const QPointF &pos)
{
    Q_UNUSED(chord);
    Q_UNUSED(pos);

    if (!note)
        return;
    if (distance > 15)
        return; // bah, magic numbers are ugly....

    m_tool->addCommand(new SetAccidentalsCommand(m_tool->shape(), note, m_accidentals));
}
