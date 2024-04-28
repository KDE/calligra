/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "RemoveBarCommand.h"
#include "../core/Bar.h"
#include "../core/Sheet.h"
#include <KLocalizedString>

#include "../MusicShape.h"

using namespace MusicCore;

RemoveBarCommand::RemoveBarCommand(MusicShape *shape, int barIdx)
    : m_shape(shape)
    , m_bar(m_shape->sheet()->bar(barIdx))
    , m_index(barIdx)
{
    setText(kundo2_i18n("Remove bar"));
}

void RemoveBarCommand::redo()
{
    m_bar->sheet()->removeBar(m_index, false);
    m_shape->engrave();
    m_shape->update();
}

void RemoveBarCommand::undo()
{
    m_bar->sheet()->insertBar(m_index, m_bar);
    m_shape->engrave();
    m_shape->update();
}
