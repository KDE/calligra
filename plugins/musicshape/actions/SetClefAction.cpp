/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SetClefAction.h"

#include "../core/Bar.h"
#include "../core/Part.h"
#include "../core/Sheet.h"
#include "../core/Staff.h"

#include "../commands/SetClefCommand.h"

#include "../MusicShape.h"
#include "../SimpleEntryTool.h"

#include <KoIcon.h>

#include "MusicDebug.h"
#include <KLocalizedString>

using namespace MusicCore;

static QIcon getIcon(Clef::ClefShape shape)
{
    const char *const id = (shape == Clef::GClef) ? koIconNameCStr("music-clef-trebble")
        : (shape == Clef::FClef)                  ? koIconNameCStr("music-clef-bass")
        : (shape == Clef::CClef)                  ? koIconNameCStr("music-clef-alto")
                                                  :
                                 /* else */ koIconNameCStr("music-clef");

    return QIcon::fromTheme(QLatin1String(id));
}

static QString getText(Clef::ClefShape shape, int line)
{
    switch (shape) {
    case Clef::GClef:
        return i18nc("Treble clef", "Treble");
    case Clef::FClef:
        return i18nc("Bass clef", "Bass");
    case Clef::CClef:
        switch (line) {
        case 1:
            return i18nc("Soprano clef", "Soprano");
        case 3:
            return i18nc("Alto clef", "Alto");
        case 4:
            return i18nc("Tenor clef", "Tenor");
        default:
            return i18n("C clef on line %1", line);
        }
    }
    return i18n("Unknown clef");
}

SetClefAction::SetClefAction(Clef::ClefShape shape, int line, int octaveChange, SimpleEntryTool *tool)
    : AbstractMusicAction(getIcon(shape), getText(shape, line), tool)
    , m_shape(shape)
    , m_line(line)
    , m_octaveChange(octaveChange)
{
    setCheckable(false);
}

void SetClefAction::mousePress(Staff *staff, int barIdx, const QPointF &pos)
{
    Q_UNUSED(pos);

    Bar *bar = staff->part()->sheet()->bar(barIdx);
    m_tool->addCommand(new SetClefCommand(m_tool->shape(), bar, staff, m_shape, m_line, m_octaveChange));
}
