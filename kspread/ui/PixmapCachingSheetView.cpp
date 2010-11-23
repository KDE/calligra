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

#include "PixmapCachingSheetView.h"

#include "PixmapCachingCellView.h"

#include "../Sheet.h"
#include "../part/CanvasBase.h"

#include <QCache>
#include <QPainter>

#include <kdebug.h>

using namespace KSpread;

#define TILESIZE 256

class PixmapCachingSheetView::Private
{
public:
    Private(PixmapCachingSheetView* q) : q(q) {}
    PixmapCachingSheetView* q;
    QCache<int, QPixmap> tileCache;
    QPointF lastScale;

    QPixmap* getTile(const Sheet* sheet, int x, int y);
};

PixmapCachingSheetView::PixmapCachingSheetView(const Sheet* sheet)
    : SheetView(sheet), d(new Private(this))
{
    d->tileCache.setMaxCost(128); // number of tiles to cache
}

PixmapCachingSheetView::~PixmapCachingSheetView()
{
    delete d;
}

CellView* PixmapCachingSheetView::createDefaultCellView()
{
    return new PixmapCachingCellView(this);
}

CellView* PixmapCachingSheetView::createCellView(int col, int row)
{
    return new PixmapCachingCellView(this, col, row);
}

QPixmap* PixmapCachingSheetView::Private::getTile(const Sheet* sheet, int x, int y)
{
    int idx = x << 16 | y;
    if (tileCache.contains(idx)) return tileCache.object(idx);

    QPixmap* pm = new QPixmap(TILESIZE, TILESIZE);
    pm->fill(QColor(255, 255, 255, 0));
    QPainter pixmapPainter(pm);
    pixmapPainter.setClipRect(pm->rect());
    pixmapPainter.scale(lastScale.x(), lastScale.y());

    QRect globalPixelRect(QPoint(x * TILESIZE, y * TILESIZE), QSize(TILESIZE, TILESIZE));
    QRectF docRect(
            globalPixelRect.x() / lastScale.x(),
            globalPixelRect.y() / lastScale.y(),
            globalPixelRect.width() / lastScale.x(),
            globalPixelRect.height() / lastScale.y()
    );

    pixmapPainter.translate(-docRect.x(), -docRect.y());

    double loffset, toffset;
    const int left = sheet->leftColumn(docRect.left(), loffset);
    const int right = sheet->rightColumn(docRect.right());
    const int top = sheet->topRow(docRect.top(), toffset);
    const int bottom = sheet->bottomRow(docRect.bottom());
    QRect cellRect(left, top, right - left + 1, bottom - top + 1);

    kDebug() << globalPixelRect << docRect;
    kDebug() << cellRect;

    q->setVisibleRect(cellRect);
    q->SheetView::paintCells(pixmapPainter, docRect, QPointF(loffset, toffset));
    //pm->save(QString("/tmp/tile%1.png").arg(idx));
    tileCache.insert(idx, pm);
    return pm;
}

void PixmapCachingSheetView::paintCells(QPainter& painter, const QRectF& paintRect, const QPointF& topLeft, const CanvasBase* canvas)
{
    if (!canvas) {
        SheetView::paintCells(painter, paintRect, topLeft);
        return;
    }
    // paintRect:   the canvas area, that should be painted; in document coordinates;
    //              no layout direction consideration; scrolling offset applied;
    //              independent from painter transformations
    // topLeft:     the document coordinate of the top left cell's top left corner;
    //              no layout direction consideration; independent from painter
    //              transformations

    QTransform t = painter.transform();

    // figure out scaling from the transformation... not really perfect, but should work as long as rotation is in 90 degree steps I think
    const qreal cos_sx = t.m11();
    const qreal sin_sx = t.m12();
    const qreal msin_sy = t.m21();
    const qreal cos_sy = t.m22();

    const qreal sx = sqrt(cos_sx*cos_sx + sin_sx*sin_sx);
    const qreal sy = sqrt(cos_sy*cos_sy + msin_sy*msin_sy);

    QPointF scale = QPointF(sx, sy);
    kDebug() << t << scale;
    if (scale != d->lastScale) {
        d->tileCache.clear();
    }
    d->lastScale = scale;

    QPointF o = canvas->offset();
    QRectF pixelRect = t.mapRect(paintRect.translated(o));

    QRect tiles;
    tiles.setLeft(pixelRect.left() / TILESIZE);
    tiles.setRight((pixelRect.right() + TILESIZE-1) / TILESIZE);
    tiles.setTop(pixelRect.top() / TILESIZE);
    tiles.setBottom((pixelRect.bottom() + TILESIZE - 1) / TILESIZE);
    kDebug() << paintRect << pixelRect << tiles << topLeft << scale << o;

    QRect savedVisRect = visibleRect();

    const Sheet* s = sheet();
    for (int x = tiles.left(); x < tiles.right(); x++) {
        for (int y = tiles.top(); y < tiles.bottom(); y++) {
            QPixmap *p = d->getTile(s, x, y);
            QPointF pt(x * TILESIZE / scale.x(), y * TILESIZE / scale.y());
            QRectF r(pt, QSizeF(TILESIZE / sx, TILESIZE / sy));
            painter.drawPixmap(r, *p, p->rect());
        }
    }

    setVisibleRect(savedVisRect);
}

void PixmapCachingSheetView::invalidateRegion(const Region &region)
{
    // TODO: figure out which tiles to invalidate
    d->tileCache.clear();

    SheetView::invalidateRegion(region);
}

void PixmapCachingSheetView::invalidate()
{
    d->tileCache.clear();

    SheetView::invalidate();
}

#include "PixmapCachingSheetView.moc"
