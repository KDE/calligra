/* This file is part of the KDE project
 * Copyright 2007 Marijn Kruisselbrink <m.Kruisselbrink@student.tue.nl>
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
#include "SetKeySignatureCommand.h"

#include "../core/Bar.h"
#include "../core/KeySignature.h"

#include "../MusicShape.h"

#include <klocale.h>

using namespace MusicCore;

SetKeySignatureCommand::SetKeySignatureCommand(MusicShape* shape, Bar* bar, Staff* staff, int accidentals)
    : m_shape(shape), m_bar(bar), m_keySignature(new KeySignature(staff, 0, accidentals)), m_oldKeySignature(NULL)
{
    setText(i18n("Change key signature"));
    
    for (int i = 0; i < bar->staffElementCount(staff); i++) {
        KeySignature* c = dynamic_cast<KeySignature*>(bar->staffElement(staff, i));
        if (c && c->startTime() == 0) {
            m_oldKeySignature = c;
            break;
        }
    }
}

void SetKeySignatureCommand::redo()
{
    if (m_oldKeySignature) m_bar->removeStaffElement(m_oldKeySignature, false);
    m_bar->addStaffElement(m_keySignature);
    m_shape->engrave();
    m_shape->repaint();
}

void SetKeySignatureCommand::undo()
{
    m_bar->removeStaffElement(m_keySignature, false);
    if (m_oldKeySignature) m_bar->addStaffElement(m_oldKeySignature);
    m_shape->engrave();
    m_shape->repaint();
}
