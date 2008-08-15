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
#include "frames/KWFrame.h"

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

    const QRectF pagerect = page->rect();
    const double pageheight = page->height();
    const double pageoffset = page->offsetInDocument();

#ifdef __GNUC__
    #warning the logic to delete pages is still broken
#endif

    foreach(KWFrameSet* fs, m_document->frameSets()) {
        foreach(KWFrame *f, fs->frames()) {
            QPointF pos = f->shape()->absolutePosition();
            if (pagerect.contains(pos)) {
                // remove all frames on the page
                fs->removeFrame(f);
            }
            else if (pos.y() > pagerect.top()) { //> pageoffset) {
                // move all frames that follow this page up the height of this page.

//disabled for now for better testing
                //pos.setY(pos.y() - pageheight);
                //f->shape()->setAbsolutePosition(pos);
            }
        }
        // remove empty framesets
        if (fs->frameCount() < 1)
            m_document->removeFrameSet(fs);
    }

    //TODO Alter frame properties to not auto-create a frame again.

    m_document->m_pageManager.removePage(page);
    m_document->firePageSetupChanged();
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

