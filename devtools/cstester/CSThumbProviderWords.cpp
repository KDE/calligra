/*
 * This file is part of Calligra
 *
 * SPDX-FileCopyrightText: 2011 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Thorsten Zachmann thorsten.zachmann@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#include "CSThumbProviderWords.h"

#include <KoPart.h>

#include <KWCanvasItem.h>
#include <KWDocument.h>
#include <KWPage.h>

#include <frames/KWFrame.h>
#include <frames/KWFrameSet.h>
#include <frames/KWTextFrameSet.h>

#include <KoPAUtil.h>
#include <KoShapeManager.h>
#include <KoShapePainter.h>
#include <KoZoomHandler.h>

#include <QApplication>
#include <QPainter>

CSThumbProviderWords::CSThumbProviderWords(KWDocument *doc)
    : m_doc(doc)
{
}

CSThumbProviderWords::~CSThumbProviderWords() = default;

QVector<QImage> CSThumbProviderWords::createThumbnails(const QSize &thumbSize)
{
    KWCanvasItem *canvasItem = static_cast<KWCanvasItem *>(m_doc->documentPart()->canvasItem(m_doc));
    KoZoomHandler zoomHandler;

    KWPageManager *pageManager = m_doc->pageManager();
    KoShapeManager *shapeManager = canvasItem->shapeManager();

    QVector<QImage> thumbnails;

    foreach (const KWPage &page, pageManager->pages()) {
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
