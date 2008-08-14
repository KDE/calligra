/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#include "KWPageRemoveCommand.h"
#include "KWDocument.h"
#include "KWPage.h"

#include <KoShapeMoveCommand.h>

#include <KLocale>

KWPageRemoveCommand::KWPageRemoveCommand( KWDocument *document, KWPage *page, QUndoCommand *parent)
    : QUndoCommand(i18n("Remove Page"), parent),
    m_document(document)
{
    Q_ASSERT(page);
    Q_ASSERT(document);
    m_pageNumber = page->pageNumber();
    Q_ASSERT(page->pageSettings());
    m_masterPageName = page->pageSettings()->masterName();
    Q_ASSERT(document->pageManager()->page(m_pageNumber) == page);
    m_pageSide = page->pageSide();
    m_pageLayout = page->pageSettings()->pageLayout();
    m_orientation = page->orientationHint();
    m_direction = page->directionHint();
}

KWPageRemoveCommand::~KWPageRemoveCommand() {
}

void KWPageRemoveCommand::redo() {
    QUndoCommand::redo();
    KWPage *page = m_document->pageManager()->page(m_pageNumber);
    Q_ASSERT(page);
    m_document->m_pageManager.removePage(page);
    m_document->firePageSetupChanged();

#ifdef __GNUC__
    #warning implement logic to delete pages
#endif
    // TODO move all frames that follow this page up the height of this page.
    // TODO remove all frames on this page
    // Alter frame properties to not auto-create a frame again.

    m_document->relayout();
}

void KWPageRemoveCommand::undo() {
    QUndoCommand::undo();

    KWPage *page = m_document->m_pageManager.insertPage(m_pageNumber);
    page->setPageSide(m_pageSide);
    m_pageLayout.orientation = m_orientation;
    page->pageSettings()->setPageLayout(m_pageLayout);
    page->setDirectionHint(m_direction);
    KWPageSettings *pageSettings = m_document->pageManager()->pageSettings(m_masterPageName);
    if (pageSettings)
        page->setPageSettings(pageSettings);
    m_document->firePageSetupChanged();
}

