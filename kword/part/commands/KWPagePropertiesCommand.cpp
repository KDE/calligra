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
#include "frame/KWFrame.h"
#include "frame/KWTextFrameSet.h"

#include <KoShapeDeleteCommand.h>
#include <KoShapeMoveCommand.h>

KWPagePropertiesCommand::KWPagePropertiesCommand( KWDocument *document, KWPage *page, const KoPageLayout &newLayout, QUndoCommand *parent)
    : QUndoCommand(parent),
    m_document(document),
    m_page(page),
    m_oldLayout( page->pageLayout() ),
    m_newLayout(newLayout)
{
    if(page->pageNumber() % 2 == 1 && newLayout.left < 0) {
        //kDebug() << "  have to insert an empty page.\n";
    }

    // move
    QList<KoShape *> shapes;
    QList<QPointF> previousPositions;
    QList<QPointF> newPositions;

    QRectF rect = page->rect();
    const double bottom = rect.bottom();
    foreach(KWFrameSet *fs, document->frameSets()) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*> (fs);
        bool remove = tfs && tfs->textFrameSetType() == KWord::MainTextFrameSet;
        foreach(KWFrame *frame, fs->frames()) {
            KoShape *shape = frame->shape();
            if(remove && shape->boundingRect().intersects(page->rect())) {
                new KoShapeDeleteCommand(document, shape, this);
            }
            else if(shape->position().y() > bottom) { // frame should be moved down
                shapes.append(shape);
                previousPositions.append(shape->position());
                newPositions.append(shape->position() + QPointF(0, rect.height()) );
            }
            else if(shape->position().y() > rect.top()) { // Let see if the frame needs to be moved to fit in the page
                // TODO
            }
        }
    }
    new KoShapeMoveCommand(shapes, previousPositions, newPositions, this);
}

void KWPagePropertiesCommand::redo() {
    setLayout(m_newLayout);
    m_document->markPageChanged(m_page);
}

void KWPagePropertiesCommand::undo() {
    setLayout(m_oldLayout);
    m_document->markPageChanged(m_page);
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

    // if pagespread; set side
}
