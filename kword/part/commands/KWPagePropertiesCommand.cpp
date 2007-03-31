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

#include "KWPagePropertiesCommand.h"
#include "KWDocument.h"
#include "KWPage.h"
#include "frames/KWFrame.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWFrameLayout.h"
#include "commands/KWPageInsertCommand.h"
#include "commands/KWFrameDeleteCommand.h"

#include <KoShapeMoveCommand.h>

#include <KLocale>

KWPagePropertiesCommand::KWPagePropertiesCommand( KWDocument *document, KWPage *page, const KoPageLayout &newLayout, QUndoCommand *parent)
    : QUndoCommand(i18n("Page Properties"), parent),
    m_document(document),
    m_page(page),
    m_oldLayout( page->pageLayout() ),
    m_newLayout(newLayout)
{
    // move
    QList<KoShape *> shapes;
    QList<QPointF> previousPositions;
    QList<QPointF> newPositions;

    QRectF rect = page->rect();
    QRectF newRect(0, rect.top(), m_newLayout.width * (m_newLayout.left < 0 ? 2:1), m_newLayout.height);
    const double bottom = rect.bottom();
    const double sizeDifference = m_newLayout.height - m_oldLayout.height;
    foreach(KWFrameSet *fs, document->frameSets()) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*> (fs);
        bool remove = tfs && tfs->textFrameSetType() == KWord::MainTextFrameSet;
        foreach(KWFrame *frame, fs->frames()) {
            KoShape *shape = frame->shape();
            if(remove && shape->boundingRect().intersects(page->rect())) {
               if(m_oldLayout.left < 0 && m_newLayout.left >= 0 &&
                       shape->position().x() >= rect.center().x()) // before it was a pageSpread.
                    new KWFrameDeleteCommand(document, frame, this);
            }
            else if(shape->position().y() > bottom) { // shape should be moved down
                shapes.append(shape);
                previousPositions.append(shape->position());
                newPositions.append(shape->position() + QPointF(0, sizeDifference) );
            }
            else if(shape->position().y() > rect.top()) { // Let see if the shape needs to be moved to fit in the page
                QRectF br = shape->boundingRect();
                if(! newRect.contains(br)) {
                    shapes.append(shape);
                    previousPositions.append(shape->position());
                    QPointF newPos = shape->position() - rect.bottomRight() + newRect.bottomRight();
                    newPositions.append(QPointF(qMax(0.0, newPos.x()) , qMax(newRect.y(), newPos.y())));
                }
            }
        }
    }
    if(shapes.count() > 0)
        new KoShapeMoveCommand(shapes, previousPositions, newPositions, this);

    if(page->pageNumber() % 2 == 1 && newLayout.left < 0)
        new KWPageInsertCommand(m_document, page->pageNumber()-1, this);
}

void KWPagePropertiesCommand::redo() {
    QUndoCommand::redo();
    setLayout(m_newLayout);
    m_document->m_frameLayout.createNewFramesForPage(m_page->pageNumber());
    m_document->firePageSetupChanged();
}

void KWPagePropertiesCommand::undo() {
    QUndoCommand::undo();
    setLayout(m_newLayout);
    setLayout(m_oldLayout);
    m_document->m_frameLayout.createNewFramesForPage(m_page->pageNumber());
    m_document->firePageSetupChanged();
}

void KWPagePropertiesCommand::setLayout(const KoPageLayout &layout) {
    bool pageSpread = layout.left < 0;
    m_page->setWidth(layout.width * (pageSpread?2:1));
    m_page->setHeight(layout.height);
    m_page->setTopMargin(layout.top);
    m_page->setBottomMargin(layout.bottom);
    m_page->setPageEdgeMargin(layout.pageEdge);
    m_page->setMarginClosestBinding(layout.bindingSide);
    m_page->setLeftMargin(layout.left);
    m_page->setRightMargin(layout.right);

    if(pageSpread)
        m_page->setPageSide(KWPage::PageSpread);
    else
        m_page->setPageSide( m_page->pageNumber()%2==0 ? KWPage::Left : KWPage::Right);
}
