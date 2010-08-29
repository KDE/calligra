/* This file is part of the KDE project
 * Copyright (C) 2007, 2010 Thomas Zander <zander@kde.org>
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

#include "KWPageStylePropertiesCommand.h"
#include "KWDocument.h"
#include "KWPage.h"
#include "KWPageStyle_p.h"
#include "frames/KWFrame.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWFrameLayout.h"
#include "commands/KWPageInsertCommand.h"
#include "commands/KWFrameDeleteCommand.h"

#include <KoShapeMoveCommand.h>

#include <KLocale>
#include <KDebug>

KWPageStylePropertiesCommand::KWPageStylePropertiesCommand(KWDocument *document, const KWPageStyle &styleBefore, const KWPageStyle &styleAfter, QUndoCommand *parent)
    : QUndoCommand(i18n("Page Properties"), parent),
    m_document(document),
    m_style(styleBefore),
    m_styleBefore(styleBefore),
    m_styleAfter(styleAfter)
{
    Q_ASSERT(m_styleAfter != m_styleBefore); // would be kinda useless
    m_styleBefore.detach("dummy"); // all mine now!
    m_styleAfter.detach("dummy"); // all mine now!

    // figure out which pages change.
    // create a list of  QMap<qreal /* posInDocument */, qreal *distance */ >
    //   which indicates that all frames after posInDocument have to move 'distance'
    // loop over all frames in the doc and move all of them using commands

    // if the change includes a pageSpread change
    //   loop over all pages that this style is assigned to and auto-delete or auto-create new pages
    //   * Notice we can't use the pageRemove command for that, so just store page numbers
    //   to delete and store pagenumbers to insert after.
    //   loop over all pages after the first changed one and set the pageSide. (seems we get to
    //      keep the setLayout() method :P)

#if 0
    // move
    QList<KoShape *> shapes;
    QList<QPointF> previousPositions;
    QList<QPointF> newPositions;

    QRectF rect = page.rect();
    QRectF newRect(0, rect.top(), m_newLayout.width * (m_newLayout.leftMargin < 0 ? 2 : 1), m_newLayout.height);
    const qreal bottom = rect.bottom();
    const qreal sizeDifference = m_newLayout.height - m_oldLayout.height;
    foreach (KWFrameSet *fs, document->frameSets()) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        bool remove = tfs && tfs->textFrameSetType() == KWord::MainTextFrameSet;
        foreach (KWFrame *frame, fs->frames()) {
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

    if (page.pageNumber() % 2 == 0 && newLayout.leftMargin < 0)
        new KWPageInsertCommand(m_document, page.pageNumber(), QString(), this);
#endif
}

void KWPageStylePropertiesCommand::redo()
{
    QUndoCommand::redo();
    m_style.priv()->copyProperties(m_styleAfter.priv());
#if 0
    m_document->m_frameLayout.createNewFramesForPage(m_page.pageNumber());
    m_document->firePageSetupChanged();
#endif
}

void KWPageStylePropertiesCommand::undo()
{
    QUndoCommand::undo();
    m_style.priv()->copyProperties(m_styleBefore.priv());
#if 0
    m_document->m_frameLayout.createNewFramesForPage(m_page.pageNumber());
    m_document->firePageSetupChanged();
#endif
}

#if 0
void KWPageStylePropertiesCommand::setLayout(const KoPageLayout &layout)
{
    KWPageStyle style = m_page.pageStyle();
    style.setPageLayout(layout);

    const bool pageSpread = layout.pageEdge >= 0; // assumption based on the KWPageLayout widget.
    foreach (KWPage page, m_document->m_pageManager.pages(style.name())) {
        if (pageSpread && page.pageNumber() % 2 == 0)
            page.setPageSide(KWPage::PageSpread); //TODO we may have to merge pages...
        else
            page.setPageSide(page.pageNumber() % 2 == 0 ? KWPage::Left : KWPage::Right);
    }
}
#endif
