/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SetClefCommand.h"

#include "../core/Bar.h"

#include "../MusicShape.h"

#include <KLocalizedString>

using namespace MusicCore;

SetClefCommand::SetClefCommand(MusicShape *shape, Bar *bar, Staff *staff, Clef::ClefShape clefShape, int line, int octaveChange)
    : m_shape(shape)
    , m_bar(bar)
    , m_clef(new Clef(staff, 0, clefShape, line, octaveChange))
    , m_oldClef(nullptr)
{
    setText(kundo2_i18n("Change clef"));

    for (int i = 0; i < bar->staffElementCount(staff); i++) {
        Clef *c = dynamic_cast<Clef *>(bar->staffElement(staff, i));
        if (c && c->startTime() == 0) {
            m_oldClef = c;
            break;
        }
    }
}

void SetClefCommand::redo()
{
    if (m_oldClef)
        m_bar->removeStaffElement(m_oldClef, false);
    m_bar->addStaffElement(m_clef);
    m_shape->engrave();
    m_shape->update();
}

void SetClefCommand::undo()
{
    m_bar->removeStaffElement(m_clef, false);
    if (m_oldClef)
        m_bar->addStaffElement(m_oldClef);
    m_shape->engrave();
    m_shape->update();
}
