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
#include "KWDocument_p.h"
#include "KWPage.h"
#include "KWPageManager_p.h"
#include "frames/KWFrameSet.h"
#include "frames/KWFrame.h"

#include <KoShapeMoveCommand.h>
#include <KoParagraphStyle.h>
#include <QTextBlock>
#include <QTextDocument>
#include <kdebug.h>
#include <KoTextShapeData.h>

#include <KLocale>

class KWPageInsertCommand::Private
{
public:
    Private(KWDocument *d, int page, const QString &master)
        : document(d),
        pageNumber(page +1),
        masterPageName(master),
        pageCreated(false),
        shapeMoveCommand(0)
    {
    }

    ~Private()
    {
        delete shapeMoveCommand;
    }

    KWDocument *document;
    int pageNumber;
    QString masterPageName;
    bool pageCreated;
    KoShapeMoveCommand *shapeMoveCommand;

    // the page we inserted. This will only be valid when the page is added to the document.
    KWPage page;

    // this struct is the way the page looks. When the page is removed from the document we effectively take over
    // ownership by moving all the content here.
    KWPageManagerPrivate::Page pageData;
};


KWPageInsertCommand::KWPageInsertCommand(KWDocument *document, int afterPageNum, const QString &masterPageName, QUndoCommand *parent)
    : QUndoCommand(i18n("Insert Page"), parent),
    d(new KWPageInsertCommand::Private(document, afterPageNum, masterPageName))
{
}

KWPageInsertCommand::~KWPageInsertCommand()
{
    delete d;
}

void KWPageInsertCommand::redo()
{
    QUndoCommand::redo();

    if (! d->pageCreated) { // create the page the first time.
        d->pageCreated = true;
        //KWPage prevPage = m_document->pageManager().page(m_afterPageNum);
        KWPageStyle pageStyle = d->document->pageManager()->pageStyle(d->masterPageName);
        d->page = d->document->pageManager()->insertPage(d->pageNumber, pageStyle);
        // move shapes after this page down.
        QList<KoShape *> shapes;
        QList<QPointF> previousPositions;
        QList<QPointF> newPositions;

        QRectF rect = d->page.rect();
        foreach(KWFrameSet *fs, d->document->frameSets()) {
            foreach(KWFrame *frame, fs->frames()) {
                KoShape *shape = frame->shape();
                if (shape->position().y() >= rect.top()) { // frame should be moved down
                    shapes.append(shape);
                    previousPositions.append(shape->position());
                    newPositions.append(shape->position() + QPointF(0, rect.height()));
                }
            }
        }

        if (shapes.count() > 0)
            d->shapeMoveCommand = new KoShapeMoveCommand(shapes, previousPositions, newPositions);
    } else { // we inserted it before, lets do so again.
        d->pageData.pageNumber = d->pageNumber;
        d->document->pageManager()->priv()->insertPage(d->pageData);
        d->page = d->document->pageManager()->page(d->pageNumber);
    }

    // make sure we have updated the view before we do anything else
    d->document->firePageSetupChanged();

    if (d->shapeMoveCommand)
        d->shapeMoveCommand->redo();
    Q_ASSERT(d->page.isValid());
    PageProcessingQueue *ppq = new PageProcessingQueue(d->document);
    ppq->addPage(d->page);
}

void KWPageInsertCommand::undo()
{
    QUndoCommand::undo();
    if (! d->page.isValid())
        return;
    KWPageManagerPrivate *priv = d->document->pageManager()->priv();
    d->pageData = priv->pages[priv->pageNumbers.value(d->page.pageNumber())]; // backup what the page looks like.
    QRectF rect = d->page.rect();

    // any frames left should be removed.
    foreach(KWFrameSet *fs, d->document->frameSets()) {
        foreach(KWFrame *frame, fs->frames()) {
            KoShape *shape = frame->shape();
            if (rect.contains(shape->absolutePosition())) // frame should be moved down
                delete shape;
        }
    }
    d->document->pageManager()->removePage(d->page);
    d->document->firePageSetupChanged();
    d->page = KWPage(); // invalidate
    if (d->shapeMoveCommand)
        d->shapeMoveCommand->undo();
}

KWPage KWPageInsertCommand::page() const
{
    return d->page;
}
