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
#include "../core/Sheet.h"
#include "../core/Part.h"
#include "../core/Staff.h"

#include "../MusicShape.h"

#include <klocale.h>

using namespace MusicCore;

SetKeySignatureCommand::SetKeySignatureCommand(MusicShape* shape, Bar* bar, Staff* staff, int accidentals)
    : m_shape(shape), m_bar(bar)
{
    setText(i18n("Change key signature"));
    
    if (staff) {
        m_newKeySignatures.append(new KeySignature(staff, 0, accidentals));
        
        for (int i = 0; i < bar->staffElementCount(staff); i++) {
            KeySignature* c = dynamic_cast<KeySignature*>(bar->staffElement(staff, i));
            if (c && c->startTime() == 0) {
                m_oldKeySignatures.append(c);
                break;
            }
        }
    } else {
        Sheet* sheet = bar->sheet();
        for (int p = 0; p < sheet->partCount(); p++) {
            Part* part = sheet->part(p);
            for (int s = 0; s < part->staffCount(); s++) {
                Staff* staff = part->staff(s);
                m_newKeySignatures.append(new KeySignature(staff, 0, accidentals));
                for (int i = 0; i < bar->staffElementCount(staff); i++) {
                    KeySignature* ks = dynamic_cast<KeySignature*>(bar->staffElement(staff, i));
                    if (ks) {
                        m_oldKeySignatures.append(ks);
                        break;
                    }
                }
            }
        }        
    }
}

void SetKeySignatureCommand::redo()
{
    foreach (KeySignature* ks, m_oldKeySignatures) {
        m_bar->removeStaffElement(ks, false);
    }
    foreach (KeySignature* ks, m_newKeySignatures) {
        kDebug() << "Adding with" << ks->accidentals() << "accidentals";
        m_bar->addStaffElement(ks);
    }
    m_shape->engrave();
    m_shape->repaint();
}

void SetKeySignatureCommand::undo()
{
    foreach (KeySignature* ks, m_newKeySignatures) {
        m_bar->removeStaffElement(ks, false);
    }
    foreach (KeySignature* ks, m_oldKeySignatures) {
        m_bar->addStaffElement(ks);
    }
    m_shape->engrave();
    m_shape->repaint();
}
