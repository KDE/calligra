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

KWPagePropertiesCommand::KWPagePropertiesCommand(KWDocument *document, const KWPage &page,
        const KoPageLayout &newLayout, KoText::Direction direction, const KoColumns &columns, QUndoCommand *parent)
    : QUndoCommand(i18n("Page Properties"), parent),
    m_document(document),
    m_page(page),
    m_oldLayout(page.pageStyle().pageLayout()),
    m_newLayout(newLayout),
    m_oldColumns(page.pageStyle().columns()),
    m_newColumns(columns),
    m_oldDirection(page.directionHint()),
    m_newDirection(direction)
{
    // move
    QList<KoShape *> shapes;
    QList<QPointF> previousPositions;
    QList<QPointF> newPositions;

    QRectF rect = page.rect();
    QRectF newRect(0, rect.top(), m_newLayout.width * (m_newLayout.leftMargin < 0 ? 2 : 1), m_newLayout.height);
    const qreal bottom = rect.bottom();
    const qreal sizeDifference = m_newLayout.height - m_oldLayout.height;
    foreach(KWFrameSet *fs, document->frameSets()) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        bool remove = tfs && tfs->textFrameSetType() == KWord::MainTextFrameSet;
        foreach(KWFrame *frame, fs->frames()) {
            KoShape *shape = frame->shape();
            if (remove && shape->boundingRect().intersects(page.rect())) {
                if (m_oldLayout.leftMargin < 0 && m_newLayout.leftMargin >= 0 &&
                        shape->position().x() >= rect.center().x()) // before it was a pageSpread.
                    new KWFrameDeleteCommand(document, frame, this);
            } else if (shape->position().y() > bottom) { // shape should be moved down
                shapes.append(shape);
                previousPositions.append(shape->position());
                newPositions.append(shape->position() + QPointF(0, sizeDifference));
            } else if (shape->position().y() > rect.top()) { // Let see if the shape needs to be moved to fit in the page
                QRectF br = shape->boundingRect();
                if (! newRect.contains(br)) {
                    shapes.append(shape);
                    previousPositions.append(shape->position());
                    QPointF newPos = shape->position() - rect.bottomRight() + newRect.bottomRight();
                    newPositions.append(QPointF(qMax(qreal(0.0), newPos.x()) , qMax(newRect.y(), newPos.y())));
                }
            }
        }
    }
    if (shapes.count() > 0)
        new KoShapeMoveCommand(shapes, previousPositions, newPositions, this);

    if (page.pageNumber() % 2 == 1 && newLayout.leftMargin < 0)
        new KWPageInsertCommand(m_document, page.pageNumber() - 1, QString(), this);
}

void KWPagePropertiesCommand::redo()
{
    QUndoCommand::redo();
    setLayout(m_newLayout);
    m_page.pageStyle().setColumns(m_newColumns);
    m_page.setDirectionHint(m_newDirection);
    m_document->m_frameLayout.createNewFramesForPage(m_page.pageNumber());
    m_document->firePageSetupChanged();
}

void KWPagePropertiesCommand::undo()
{
    QUndoCommand::undo();
    setLayout(m_oldLayout);
    m_page.pageStyle().setColumns(m_oldColumns);
    m_page.setDirectionHint(m_oldDirection);
    m_document->m_frameLayout.createNewFramesForPage(m_page.pageNumber());
    m_document->firePageSetupChanged();
}

void KWPagePropertiesCommand::setLayout(const KoPageLayout &layout)
{
    m_page.pageStyle().setPageLayout(layout);
    if (layout.pageEdge >= 0.0 &&    // assumption based on the KWPageLayout widget.
            m_page.pageNumber() % 2 == 0)
        m_page.setPageSide(KWPage::PageSpread);
    else
        m_page.setPageSide(m_page.pageNumber() % 2 == 0 ? KWPage::Left : KWPage::Right);
}
