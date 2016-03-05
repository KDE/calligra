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

#include "CSThumbProviderTables.h"

#include <sheets/part/Doc.h>
#include <sheets/Sheet.h>
#include <sheets/Map.h>
#include <sheets/PrintSettings.h>
#include <sheets/ui/SheetView.h>
#include <sheets/SheetPrint.h>
#include <KoZoomHandler.h>
#include <KoShapePainter.h>
#include <KoPAUtil.h>
#include <QPainter>

CSThumbProviderTables::CSThumbProviderTables(Calligra::Sheets::Doc *doc)
: m_doc(doc)
{
}

CSThumbProviderTables::~CSThumbProviderTables()
{
}

QVector<QImage> CSThumbProviderTables::createThumbnails(const QSize &thumbSize)
{
    QVector<QImage> thumbnails;
    if (0 != m_doc->map()) {
        foreach(Calligra::Sheets::Sheet* sheet, m_doc->map()->sheetList()) {
            QImage thumbnail(thumbSize, QImage::Format_RGB32);
            thumbnail.fill(QColor(Qt::white).rgb());
            QPainter p(&thumbnail);

            KoPageLayout pageLayout;
            pageLayout.format = KoPageFormat::IsoA4Size;
            pageLayout.leftMargin = 0;
            pageLayout.rightMargin = 0;
            pageLayout.topMargin = 0;
            pageLayout.bottomMargin = 0;
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

            KoShapePainter shapePainter(direction == Qt::LeftToRight ? (KoShapeManagerPaintingStrategy *)0 : (KoShapeManagerPaintingStrategy *)0 /*RightToLeftPaintingStrategy(shapeManager, d->canvas)*/);
            shapePainter.setShapes(sheet->shapes());
            shapePainter.paint(p, zoomHandler);

            thumbnails.append(thumbnail);
        }
    }
    return thumbnails;
}
