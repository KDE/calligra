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

#include "KWNewPageStyleCommand.h"
#include "KWDocument.h"
#include "KWPageManager.h"

#include <KDebug>

KWNewPageStyleCommand::KWNewPageStyleCommand(KWDocument *document, const KWPageStyle &newStyle, QUndoCommand *parent)
    : QUndoCommand(i18n("Insert Page Style"), parent),
    m_newStyle(newStyle),
    m_document(document)
{
    Q_ASSERT(document);
}

void KWNewPageStyleCommand::redo()
{
    QUndoCommand::redo();
    m_document->pageManager()->addPageStyle(m_newStyle);
}

void KWNewPageStyleCommand::undo()
{
    QUndoCommand::undo();
    m_document->pageManager()->removePageStyle(m_newStyle);
}
