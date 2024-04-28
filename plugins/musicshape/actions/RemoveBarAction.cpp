/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "RemoveBarAction.h"

#include "../core/Part.h"
#include "../core/Sheet.h"
#include "../core/Staff.h"

#include "../commands/RemoveBarCommand.h"

#include "../MusicShape.h"
#include "../SimpleEntryTool.h"

#include <KLocalizedString>

using namespace MusicCore;

RemoveBarAction::RemoveBarAction(SimpleEntryTool *tool)
    : AbstractMusicAction(i18n("Remove bar"), tool)
{
    setCheckable(false);
}

void RemoveBarAction::mousePress(Staff *staff, int barIdx, const QPointF &pos)
{
    Q_UNUSED(staff);
    Q_UNUSED(pos);

    m_tool->addCommand(new RemoveBarCommand(m_tool->shape(), barIdx));
}
