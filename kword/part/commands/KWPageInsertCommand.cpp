/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
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

#include "KWPageInsertCommand.h"
#include "KWDocument.h"
#include "KWPage.h"
#include "frames/KWFrameSet.h"
#include "frames/KWFrame.h"

#include <KoShapeMoveCommand.h>
#include <KoParagraphStyle.h>
#include <QTextBlock>
#include <QTextDocument>
#include <kdebug.h>
#include <KoTextShapeData.h>

#include <KLocale>

KWPageInsertCommand::KWPageInsertCommand(KWDocument *document, int afterPageNum, QUndoCommand *parent, const QString &masterPageName)
        : QUndoCommand(i18n("Insert Page"), parent),
        m_document(document),
        m_afterPageNum(afterPageNum),
        m_masterPageName(masterPageName),
        m_shapeMoveCommand(0)
{
}

KWPageInsertCommand::~KWPageInsertCommand()
{
}

void KWPageInsertCommand::redo()
{
    QUndoCommand::redo();
    KWPageStyle pageStyle = m_document->pageManager()->pageStyle(m_masterPageName);
    if (!m_page.isValid()) {
        // Appending a page
        KWPage prevPage = m_document->m_pageManager.page(m_afterPageNum);
        m_page = m_document->m_pageManager.insertPage(m_afterPageNum + 1, pageStyle);
        if (prevPage.isValid())
            m_page.setDirectionHint(prevPage.directionHint());
        if (m_page.pageNumber() % 2 == 0 && m_document->m_pageManager.preferPageSpread()) // should be a pageSpread
            m_page.setPageSide(KWPage::PageSpread);
        PageProcessingQueue *ppq = new PageProcessingQueue(m_document);
        ppq->addPage(m_page);

        // move shapes after this page down.
        QList<KoShape *> shapes;
        QList<QPointF> previousPositions;
        QList<QPointF> newPositions;

        QRectF rect = m_page.rect();
        foreach(KWFrameSet *fs, m_document->frameSets()) {
            foreach(KWFrame *frame, fs->frames()) {
                KoShape *shape = frame->shape();
                if (shape->position().y() > rect.top()) { // frame should be moved down
                    shapes.append(shape);
                    previousPositions.append(shape->position());
                    newPositions.append(shape->position() + QPointF(0, rect.height()));
                }
            }
        }

        //FIXME if (false) ??? see also KWPageRemoveCommand
        if (false && shapes.count() > 0)
            m_shapeMoveCommand = new KoShapeMoveCommand(shapes, previousPositions, newPositions);
    } else {
        // Inserting a page
        if (!m_masterPageName.isEmpty())
            if (pageStyle.isValid())
                m_page.setPageStyle(pageStyle);
        m_document->m_pageManager.insertPage(m_page);
    }
    if (m_shapeMoveCommand)
        m_shapeMoveCommand->redo();
    m_document->firePageSetupChanged();
}

void KWPageInsertCommand::undo()
{
    QUndoCommand::undo();
    if (!m_page.isValid()) return;
    m_document->m_pageManager.removePage(m_page);
    if (m_shapeMoveCommand)
        m_shapeMoveCommand->redo();
    m_document->firePageSetupChanged();
}

