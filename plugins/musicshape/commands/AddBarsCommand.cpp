/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "AddBarsCommand.h"
#include "../MusicShape.h"
#include "../core/Sheet.h"

#include <KLocalizedString>

using namespace MusicCore;

AddBarsCommand::AddBarsCommand(MusicShape *shape, int bars)
    : m_sheet(shape->sheet())
    , m_bars(bars)
    , m_shape(shape)
{
    setText(kundo2_i18n("Add measures"));
}

void AddBarsCommand::redo()
{
    m_sheet->addBars(m_bars);
    m_shape->engrave();
    m_shape->update();
}

void AddBarsCommand::undo()
{
    m_sheet->removeBars(m_sheet->barCount() - m_bars, m_bars);
    m_shape->engrave();
    m_shape->update();
}
