/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "PixmapCachingCellView.h"

#include <QPainter>
#include <QTransform>
#include <qmath.h>

#include <kdebug.h>

#include "PixmapCachingSheetView.h"
#include "../Cell.h"

namespace KSpread {

class PixmapCachingCellView::Private {
public:
    QPixmap* pixmap;
};

PixmapCachingCellView::PixmapCachingCellView(PixmapCachingSheetView *sheetView)
    : CellView(sheetView), d(new Private)
{
    d->pixmap = 0;
}

PixmapCachingCellView::PixmapCachingCellView(PixmapCachingSheetView *sheetView, int col, int row)
    : CellView(sheetView, col, row), d(new Private)
{
    d->pixmap = 0;
}

PixmapCachingCellView::~PixmapCachingCellView()
{
    delete d->pixmap;
    delete d;
}

void PixmapCachingCellView::paintCellContents(const QRectF &paintRegion, QPainter &painter, const QRegion &clipRegion, const QPointF &coordinate,
                                              const Cell &cell, SheetView *sheetView) const
{
    QTransform t = painter.transform();
    if (!d->pixmap) {
        QRectF cellRect = t.mapRect(QRectF(coordinate, QSizeF(cellWidth(), cellHeight())));
        d->pixmap = new QPixmap((cellRect.size()).toSize());
        d->pixmap->fill(QColor(255, 255, 255, 0));
        QPainter pixmapPainter(d->pixmap);
        pixmapPainter.setClipRect(d->pixmap->rect());
        pixmapPainter.scale(t.m11(), t.m22());
        CellView::paintCellContents(paintRegion.translated(-coordinate), pixmapPainter, pixmapPainter.clipRegion(), QPointF(0, 0), cell, sheetView);
    }
    QPointF p = t.map(coordinate);
    painter.resetTransform();
    painter.drawPixmap(p, *d->pixmap);
    painter.setTransform(t, false);
}

} // namespace KSpread

#include "PixmapCachingCellView.moc"
