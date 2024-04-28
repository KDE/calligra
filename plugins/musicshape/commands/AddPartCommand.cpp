/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "AddPartCommand.h"

#include "../core/Bar.h"
#include "../core/Clef.h"
#include "../core/Part.h"
#include "../core/Sheet.h"
#include "../core/Staff.h"
#include "../core/TimeSignature.h"

#include "../MusicShape.h"

#include <KLocalizedString>

using namespace MusicCore;

AddPartCommand::AddPartCommand(MusicShape *shape)
    : m_sheet(shape->sheet())
    , m_shape(shape)
{
    setText(kundo2_i18n("Add part"));
    m_part = new Part(m_sheet, i18n("Part %1", (m_sheet->partCount() + 1)));
    Staff *s = m_part->addStaff();
    m_part->sheet()->bar(0)->addStaffElement(new Clef(s, 0, Clef::GClef, 2));
    // figure out time signature
    if (m_sheet->partCount() == 0) {
        m_part->sheet()->bar(0)->addStaffElement(new TimeSignature(s, 0, 4, 4));
    } else {
        Staff *curStaff = m_sheet->part(0)->staff(0);
        TimeSignature *ts = curStaff->lastTimeSignatureChange(0);
        if (!ts) {
            m_part->sheet()->bar(0)->addStaffElement(new TimeSignature(s, 0, 4, 4));
        } else {
            m_part->sheet()->bar(0)->addStaffElement(new TimeSignature(s, 0, ts->beats(), ts->beat(), ts->type()));
        }
    }
}

void AddPartCommand::redo()
{
    m_sheet->addPart(m_part);
    m_sheet->setStaffSystemCount(0);
    m_shape->engrave();
    m_shape->update();
}

void AddPartCommand::undo()
{
    m_sheet->removePart(m_part, false);
    m_sheet->setStaffSystemCount(0);
    m_shape->engrave();
    m_shape->update();
}
