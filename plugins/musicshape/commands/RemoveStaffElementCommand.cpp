/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "RemoveStaffElementCommand.h"
#include "../core/Bar.h"
#include "../core/Clef.h"
#include "../core/KeySignature.h"
#include "../core/Staff.h"
#include "../core/StaffElement.h"
#include <KLocalizedString>

#include "../MusicShape.h"

using namespace MusicCore;

RemoveStaffElementCommand::RemoveStaffElementCommand(MusicShape *shape, StaffElement *se, Bar *bar)
    : m_shape(shape)
    , m_element(se)
    , m_bar(bar)
    , m_index(m_bar->indexOfStaffElement(se))
{
    if (dynamic_cast<Clef *>(se)) {
        setText(kundo2_i18n("Remove clef"));
    } else {
        setText(kundo2_i18n("Remove staff element"));
    }
}

void RemoveStaffElementCommand::redo()
{
    m_bar->removeStaffElement(m_element, false);
    if (dynamic_cast<KeySignature *>(m_element)) {
        m_element->staff()->updateAccidentals(m_bar);
    }
    m_shape->engrave();
    m_shape->update();
}

void RemoveStaffElementCommand::undo()
{
    m_bar->addStaffElement(m_element, m_index);
    if (dynamic_cast<KeySignature *>(m_element)) {
        m_element->staff()->updateAccidentals(m_bar);
    }
    m_shape->engrave();
    m_shape->update();
}
