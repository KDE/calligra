/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
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
#include "KWFrameDeleteCommand.h"
#include "KWDocument.h"
#include "KWPage.h"
#include "frames/KWFrame.h"

#include <KoShapeMoveCommand.h>

#include <QTextDocument>
#include <kdebug.h>
#include <KLocale>

KWPageRemoveCommand::KWPageRemoveCommand( KWDocument *document, KWPage *page, QUndoCommand *parent)
    : QUndoCommand(i18n("Remove Page"), parent),
    m_document(document)
{
    Q_ASSERT(page);
    Q_ASSERT(document);
    m_pageNumber = page->pageNumber();
    Q_ASSERT(page->pageStyle());
    m_masterPageName = page->pageStyle()->masterName();
    Q_ASSERT(document->pageManager()->page(m_pageNumber) == page);
    m_pageSide = page->pageSide();
    m_pageLayout = page->pageStyle()->pageLayout();
    m_orientation = page->orientationHint();
    m_direction = page->directionHint();
}

KWPageRemoveCommand::~KWPageRemoveCommand() {
    qDeleteAll(m_childcommands);
}

void KWPageRemoveCommand::redo() {
    QUndoCommand::redo();
    KWPage *page = m_document->pageManager()->page(m_pageNumber);
    Q_ASSERT(page);

    const QRectF pagerect = page->rect();
    //const double pageheight = page->height();
    //const double topOfPage = m_document->pageManager()->topOfPage(m_pageNumber);
    //const double pageoffset = page->offsetInDocument();

    const bool firstRun = m_childcommands.count() == 0;
    if (firstRun) {
        QList<KoShape*> shapes;
        QList<QPointF> previousPositions;
        QList<QPointF> newPositions;
        foreach(KWFrameSet* fs, m_document->frameSets()) {
            foreach(KWFrame *f, fs->frames()) {
                QPointF pos = f->shape()->absolutePosition();
                if (pagerect.contains(pos)) {
                    // remove all frames on the page
                    KWFrameDeleteCommand *command = new KWFrameDeleteCommand(m_document, f, this);
                    m_childcommands.append(command);
                }
                else if (pos.y() > pagerect.top()) { //> pageoffset) {
                    // move all frames that follow this page up the height of this page

                    if (KWTextFrameSet*tfs = dynamic_cast<KWTextFrameSet*>(fs)) {
                        // don't move headers, footers or the main frameset
                        if (tfs->textFrameSetType() == KWord::OddPagesHeaderTextFrameSet ||
                            tfs->textFrameSetType() == KWord::EvenPagesHeaderTextFrameSet ||
                            tfs->textFrameSetType() == KWord::OddPagesFooterTextFrameSet ||
                            tfs->textFrameSetType() == KWord::EvenPagesFooterTextFrameSet ||
                            tfs->textFrameSetType() == KWord::MainTextFrameSet) continue;
                    }

                    Q_ASSERT(f->shape());
                    Q_ASSERT(dynamic_cast<KWFrame*>(f->shape()->applicationData()));
                    shapes.append(f->shape());
                    previousPositions.append(f->shape()->position());
                    newPositions.append(f->shape()->position() - QPointF(0, pagerect.height()));
                }
            }
        }

        if (shapes.count() > 0) {
            KoShapeMoveCommand *command = new KoShapeMoveCommand(shapes, previousPositions, newPositions);
            m_childcommands.append(command);
        }
    }

    // redo the commands
    foreach(QUndoCommand* command, m_childcommands) {
        command->redo();
    }

    //TODO Alter frame properties to not auto-create a frame again.

    //TODO remove the text include possible pagebreak displayed on the page from the
    //mainframe's QTextDocument. Atm this results in a new page being added after the
    //selected page got removed :-/

    // remove the page
    m_document->m_pageManager.removePage(page);

    // update changes
    m_document->firePageSetupChanged();
    m_document->relayout(); //needed?
}

void KWPageRemoveCommand::undo() {
    QUndoCommand::undo();

    // insert the page
    KWPage *page = m_document->m_pageManager.insertPage(m_pageNumber);
    page->setPageSide(m_pageSide);
    m_pageLayout.orientation = m_orientation;
    page->pageStyle()->setPageLayout(m_pageLayout);
    page->setDirectionHint(m_direction);
    KWPageStyle *pageStyle = m_document->pageManager()->pageStyle(m_masterPageName);
    if (pageStyle)
        page->setPageStyle(pageStyle);

    // undo the KWFrameDeleteCommand commands
    foreach(QUndoCommand* command, m_childcommands) {
        command->undo();
    }

    // update changes
    m_document->firePageSetupChanged();
    m_document->relayout(); //needed?
}

