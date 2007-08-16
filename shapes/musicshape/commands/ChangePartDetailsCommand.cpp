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
#include "../core/Staff.h"
#include "../core/Clef.h"
#include "../core/Bar.h"
#include "../core/Sheet.h"

#include "../MusicShape.h"

using namespace MusicCore;

ChangePartDetailsCommand::ChangePartDetailsCommand(MusicShape* shape, Part* part, const QString& name, const QString& abbr, int staffCount)
    : m_shape(shape), m_part(part), m_oldName(part->name()), m_newName(name), m_oldAbbr(part->shortName(false))
    , m_newAbbr(abbr), m_oldStaffCount(part->staffCount()), m_newStaffCount(staffCount)
{
    setText("Change part details");
    
    if (m_newStaffCount > m_oldStaffCount) {
        for (int i = 0; i < m_newStaffCount - m_oldStaffCount; i++) {
            Staff* s = new Staff(m_part);
            m_part->sheet()->bar(0)->addStaffElement(new Clef(s, 0, Clef::GClef, 2));
            m_staves.append(s);
        }
    } else if (m_newStaffCount < m_oldStaffCount) {
        for (int i = m_newStaffCount; i < m_oldStaffCount; i++) {
            m_staves.append(m_part->staff(i));
        }
    }
}

void ChangePartDetailsCommand::redo()
{
    m_part->setName(m_newName);
    m_part->setShortName(m_newAbbr);
    if (m_newStaffCount > m_oldStaffCount) {
        foreach (Staff* s, m_staves) {
            m_part->addStaff(s);
        }
    } else if (m_newStaffCount < m_oldStaffCount) {
        foreach (Staff* s, m_staves) {
            m_part->removeStaff(s, false);
        }
    }
    if (m_newStaffCount != m_oldStaffCount) {
        m_shape->sheet()->setStaffSystemCount(0);
        m_shape->engrave();
        m_shape->repaint();
    }        
}

void ChangePartDetailsCommand::undo()
{
    m_part->setName(m_oldName);
    m_part->setShortName(m_oldAbbr);
    if (m_oldStaffCount > m_newStaffCount) {
        foreach (Staff* s, m_staves) {
            m_part->addStaff(s);
        }
    } else if (m_oldStaffCount < m_newStaffCount) {
        foreach (Staff* s, m_staves) {
            m_part->removeStaff(s, false);
        }
    }
    if (m_newStaffCount != m_oldStaffCount) {
        m_shape->sheet()->setStaffSystemCount(0);
        m_shape->engrave();
        m_shape->repaint();
    }        
}
