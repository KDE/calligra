/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "RemovePartCommand.h"
#include "../MusicShape.h"
#include "../core/Part.h"
#include "../core/Sheet.h"

#include "KLocalizedString"

using namespace MusicCore;

RemovePartCommand::RemovePartCommand(MusicShape *shape, Part *part)
    : m_sheet(part->sheet())
    , m_part(part)
    , m_shape(shape)
    , m_partIndex(m_sheet->partIndex(part))
{
    setText(kundo2_i18n("Remove part"));
}

void RemovePartCommand::redo()
{
    m_sheet->removePart(m_part, false);
    m_sheet->setStaffSystemCount(0);
    m_shape->engrave();
    m_shape->update();
}

void RemovePartCommand::undo()
{
    m_sheet->insertPart(m_partIndex, m_part);
    m_sheet->setStaffSystemCount(0);
    m_shape->engrave();
    m_shape->update();
}
