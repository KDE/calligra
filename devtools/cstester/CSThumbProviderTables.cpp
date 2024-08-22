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

#include "CSThumbProviderTables.h"

#include <KoPAUtil.h>
#include <KoShapePainter.h>
#include <KoZoomHandler.h>
#include <QPainter>
#include <sheets/core/Map.h>
#include <sheets/core/PrintSettings.h>
#include <sheets/core/Sheet.h>
#include <sheets/core/SheetPrint.h>
#include <sheets/part/Doc.h>
#include <sheets/ui/SheetView.h>

CSThumbProviderTables::CSThumbProviderTables(Calligra::Sheets::Doc *doc)
    : m_doc(doc)
{
}

CSThumbProviderTables::~CSThumbProviderTables() = default;

QVector<QImage> CSThumbProviderTables::createThumbnails(const QSize &thumbSize)
{
    QVector<QImage> thumbnails;
    if (nullptr != m_doc->map()) {
        for (Calligra::Sheets::SheetBase *bsheet : m_doc->map()->sheetList()) {
            QImage thumbnail(thumbSize, QImage::Format_RGB32);
            thumbnail.fill(QColor(Qt::white).rgb());
            QPainter p(&thumbnail);

            KoPageLayout pageLayout;
            pageLayout.format = KoPageFormat::IsoA4Size;
            pageLayout.leftMargin = 0;
            pageLayout.rightMargin = 0;
            pageLayout.topMargin = 0;
            pageLayout.bottomMargin = 0;
            Calligra::Sheets::Sheet *sheet = dynamic_cast<Calligra::Sheets::Sheet *>(bsheet);
            sheet->printSettings()->setPageLayout(pageLayout);
            sheet->print()->setSettings(*sheet->printSettings(), true);

            Calligra::Sheets::SheetView sheetView(sheet);

            // only paint first page for now
            KoZoomHandler zoomHandler;
            KoPAUtil::setZoom(pageLayout, thumbSize, zoomHandler);
            sheetView.setViewConverter(&zoomHandler);

            QRect range(sheet->print()->cellRange(1));
            // paint also half cells on page edge
            range.setWidth(range.width() + 1);
            sheetView.setPaintCellRange(range); // first page

            QRect pRect(KoPAUtil::pageRect(pageLayout, thumbSize, zoomHandler));

            p.setClipRect(pRect);
            p.translate(pRect.topLeft());
            sheetView.paintCells(p, QRect(0, 0, pageLayout.width, pageLayout.height), QPointF(0, 0));

            const Qt::LayoutDirection direction = sheet->layoutDirection();

            KoShapePainter shapePainter(direction == Qt::LeftToRight
                                            ? (KoShapeManagerPaintingStrategy *)nullptr
                                            : (KoShapeManagerPaintingStrategy *)nullptr /*RightToLeftPaintingStrategy(shapeManager, d->canvas)*/);
            shapePainter.setShapes(sheet->shapes());
            shapePainter.paint(p, zoomHandler);

            thumbnails.append(thumbnail);
        }
    }
    return thumbnails;
}
