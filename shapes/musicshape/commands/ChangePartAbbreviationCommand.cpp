/* This file is part of the KDE project
 * Copyright (C) 2007 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "ChangePartAbbreviationCommand.h"
#include "../core/Sheet.h"
#include "../core/Part.h"
#include "../MusicShape.h"

#include "klocale.h"

using namespace MusicCore;

ChangePartAbbreviationCommand::ChangePartAbbreviationCommand(MusicShape* shape, Part* part, const QString& name)
    : m_sheet(shape->sheet()),
    m_part(part),
    m_shape(shape),
    m_name(name),
    m_oldName(part->shortName(false))
{
    setText(i18n("Change part short name"));
}

void ChangePartAbbreviationCommand::redo()
{
    m_part->setShortName(m_name);
}

void ChangePartAbbreviationCommand::undo()
{
    m_part->setShortName(m_oldName);
}
