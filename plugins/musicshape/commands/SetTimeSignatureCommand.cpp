/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SetTimeSignatureCommand.h"

#include "../core/Bar.h"
#include "../core/Part.h"
#include "../core/Sheet.h"

#include "../MusicShape.h"

#include <KLocalizedString>

using namespace MusicCore;

SetTimeSignatureCommand::SetTimeSignatureCommand(MusicShape *shape, Bar *bar, int beats, int beat)
    : m_shape(shape)
    , m_bar(bar)
{
    setText(kundo2_i18n("Change time signature"));

    Sheet *sheet = bar->sheet();
    for (int p = 0; p < sheet->partCount(); p++) {
        Part *part = sheet->part(p);
        for (int s = 0; s < part->staffCount(); s++) {
            Staff *staff = part->staff(s);
            m_newSigs.append(new TimeSignature(staff, 0, beats, beat));
            for (int i = 0; i < bar->staffElementCount(staff); i++) {
                TimeSignature *ts = dynamic_cast<TimeSignature *>(bar->staffElement(staff, i));
                if (ts) {
                    m_oldSigs.append(ts);
                    break;
                }
            }
        }
    }
}

void SetTimeSignatureCommand::redo()
{
    foreach (TimeSignature *ts, m_oldSigs) {
        m_bar->removeStaffElement(ts, false);
    }
    foreach (TimeSignature *ts, m_newSigs) {
        m_bar->addStaffElement(ts);
    }
    m_shape->engrave();
    m_shape->update();
}

void SetTimeSignatureCommand::undo()
{
    foreach (TimeSignature *ts, m_newSigs) {
        m_bar->removeStaffElement(ts, false);
    }
    foreach (TimeSignature *ts, m_oldSigs) {
        m_bar->addStaffElement(ts);
    }
    m_shape->engrave();
    m_shape->update();
}
