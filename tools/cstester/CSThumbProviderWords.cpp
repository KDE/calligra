/*
 * This file is part of Calligra
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Thorsten Zachmann thorsten.zachmann@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "CSThumbProviderWords.h"

#include <KWDocument.h>
#include <KWPage.h>
#include <frames/KWFrame.h>
#include <frames/KWFrameSet.h>
#include <frames/KWTextFrameSet.h>
#include <frames/KWTextDocumentLayout.h>

#include <KoTextShapeData.h>
#include <KoZoomHandler.h>
#include <KoShapePainter.h>
#include <KoPAUtil.h>

#include <QPainter>
#include <QApplication>

CSThumbProviderWords::CSThumbProviderWords(KWDocument *doc)
: m_doc(doc)
{
}

CSThumbProviderWords::~CSThumbProviderWords()
{
}

QList<QPixmap> CSThumbProviderWords::createThumbnails(const QSize &thumbSize)
{
    KoZoomHandler zoomHandler;
    QList<KoShape*> shapes;
    foreach(KWFrameSet *frameSet, m_doc->frameSets()) {
        foreach(KWFrame *frame, frameSet->frames()) {
            KoShape *shape = frame->shape();
            shapes.append(shape);

            // We need to call waitUntilReady so that the Layout is set on the text shape 
            // representing the main text frame.
            shape->waitUntilReady(zoomHandler, false);
            KoTextShapeData *textShapeData = dynamic_cast<KoTextShapeData*>(shape->userData());
            if (textShapeData) {
                // the foul is needed otherwise it does not work
                textShapeData->foul();
                KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout*>(textShapeData->document()->documentLayout());
                if (lay) {
                    while (textShapeData->isDirty()){
                        lay->scheduleLayout();
                        QCoreApplication::processEvents();
                    }
                }
            }
        }
    }

    while (!m_doc->layoutFinishedAtleastOnce()) {
        QCoreApplication::processEvents();

        if (!QCoreApplication::hasPendingEvents())
            break;
    }

    KWPageManager *manager = m_doc->pageManager();

    // recreate the shape list as they are only created when the shape when the frames are added during first layout
    shapes.clear();
    foreach(KWFrameSet* frameSet, m_doc->frameSets()) {
        foreach(KWFrame *frame, frameSet->frames()) {
            shapes.append(frame->shape());
        }
    }

    qDebug() << "Shapes" << shapes.size();

    QList<QPixmap> thumbnails;

    KoShapePainter shapePainter;
    shapePainter.setShapes(shapes);
    foreach(KWPage page, manager->pages()) {
        QRectF pRect(page.rect(page.pageNumber()));
        KoPageLayout layout;
        layout.width = pRect.width();
        layout.height = pRect.height();

        KoPAUtil::setZoom(layout, thumbSize, zoomHandler);

        QPixmap thumbnail(thumbSize);
        thumbnail.fill(Qt::white);
        QPainter p(&thumbnail);
        p.setPen(Qt::black);
        p.drawRect(KoPAUtil::pageRect(layout, thumbSize, zoomHandler));
        shapePainter.paint(p, QRect(QPoint(0,0), thumbSize), pRect);

        thumbnails.append(thumbnail);
    }

    return thumbnails;
}
