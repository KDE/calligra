/* This file is part of the KDE project
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
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

#include "KWChangePageStyleCommand.h"

#include <KLocale>

KWChangePageStyleCommand::KWChangePageStyleCommand(KWPage &page, const KWPageStyle &newStyle, QUndoCommand *parent)
    : QUndoCommand(i18n("Set Page Style"), parent),
    m_newStyle(newStyle),
    m_oldStyle(page.pageStyle()),
    m_page(page)
{
}

void KWChangePageStyleCommand::redo()
{
    QUndoCommand::redo();
    m_page.setPageStyle(m_newStyle);
}

void KWChangePageStyleCommand::undo()
{
    QUndoCommand::undo();
    m_page.setPageStyle(m_oldStyle);
}
