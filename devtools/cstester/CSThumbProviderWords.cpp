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

#include <KoPart.h>

#include <KWDocument.h>
#include <KWPage.h>
#include <KWCanvasItem.h>

#include <frames/KWFrame.h>
#include <frames/KWFrameSet.h>
#include <frames/KWTextFrameSet.h>

#include <KoShapeManager.h>
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

QVector<QImage> CSThumbProviderWords::createThumbnails(const QSize &thumbSize)
{
    KWCanvasItem *canvasItem = static_cast<KWCanvasItem*>(m_doc->documentPart()->canvasItem(m_doc));
    KoZoomHandler zoomHandler;

    while (!m_doc->layoutFinishedAtleastOnce()) {
        QCoreApplication::processEvents();

        if (!QCoreApplication::hasPendingEvents())
            break;
    }

    KWPageManager *pageManager = m_doc->pageManager();
    KoShapeManager *shapeManager = canvasItem->shapeManager();

    QVector<QImage> thumbnails;

    foreach(const KWPage &page, pageManager->pages()) {

        QRectF pRect(page.rect());
        KoPageLayout layout;
        layout.width = pRect.width();
        layout.height = pRect.height();

        KoPAUtil::setZoom(layout, thumbSize, zoomHandler);
        QRect pageRect = KoPAUtil::pageRect(layout, thumbSize, zoomHandler);

        QImage thumbnail(thumbSize, QImage::Format_RGB32);
        thumbnail.fill(QColor(Qt::white).rgb());
        QPainter p(&thumbnail);

        QImage img = page.thumbnail(pageRect.size(), shapeManager);
        p.drawImage(pageRect, img);

        p.setPen(Qt::black);
        p.drawRect(pageRect);

        thumbnails.append(thumbnail);
    }

    return thumbnails;
}


