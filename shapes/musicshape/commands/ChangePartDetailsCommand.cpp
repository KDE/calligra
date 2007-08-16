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
#include "ChangePartDetailsCommand.h"

#include "../core/Part.h"

using namespace MusicCore;

ChangePartDetailsCommand::ChangePartDetailsCommand(MusicShape* shape, Part* part, const QString& name, const QString& abbr, int staffCount)
    : m_shape(shape), m_part(part), m_oldName(part->name()), m_newName(name), m_oldAbbr(part->shortName(false))
    , m_newAbbr(abbr), m_oldStaffCount(part->staffCount()), m_newStaffCount(staffCount)
{
    setText("Change part details");
}

void ChangePartDetailsCommand::redo()
{
    m_part->setName(m_newName);
    m_part->setShortName(m_newAbbr);
    if (m_newStaffCount > m_oldStaffCount) {
    } else if (m_newStaffCount < m_oldStaffCount) {
    }
}

void ChangePartDetailsCommand::undo()
{
    m_part->setName(m_oldName);
    m_part->setShortName(m_oldAbbr);
    if (m_oldStaffCount > m_newStaffCount) {
    } else if (m_oldStaffCount < m_newStaffCount) {
    }
}
