/* This file is part of the KDE project
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
 * Copyright (C) 2009 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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
#include "KWCanvasBase.h"

// kword includes
#include "KWCanvas.h"
#include "KWGui.h"
#include "KWViewMode.h"
#include "KWPage.h"
#include "KWPageCacheManager.h"

// koffice libs includes
#include <KoShapeManager.h>
#include <KoPointerEvent.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoToolProxy.h>
#include <KoGridData.h>
#include <KoShape.h>
#include <KoViewConverter.h>

// KDE + Qt includes
#include <KDebug>
#include <QBrush>
#include <QPainter>
#include <QPainterPath>
#include <QThread>

//#define DEBUG_REPAINT

KWCanvasBase::KWCanvasBase(KWDocument *document, QObject *parent)
    : KoCanvasBase(document),
      m_document(document),
      m_shapeManager(0),
      m_toolProxy(0),
      m_viewMode(0),
      m_viewConverter(0),
      m_cacheEnabled(false),
      m_currentZoom(0.0),
      m_pageCacheManager(0)
{
    m_shapeManager = new KoShapeManager(this);
    m_toolProxy = new KoToolProxy(this, parent);
}

KWCanvasBase::~KWCanvasBase()
{
    delete m_shapeManager;
    delete m_viewMode;
    delete m_pageCacheManager;
}

void KWCanvasBase::gridSize(qreal *horizontal, qreal *vertical) const
{
    *horizontal = m_document->gridData().gridX();
    *vertical = m_document->gridData().gridY();
}

void KWCanvasBase::addCommand(QUndoCommand *command)
{
    m_document->addCommand(command);
}

KoShapeManager *KWCanvasBase::shapeManager() const
{
    return m_shapeManager;
}

/// reimplemented method from superclass
KoUnit KWCanvasBase::unit() const
{
    return m_document->unit();
}

/// reimplemented method from superclass
KoToolProxy *KWCanvasBase::toolProxy() const
{
    return m_toolProxy;
}

void KWCanvasBase::clipToDocument(const KoShape *shape, QPointF &move) const
{
    Q_ASSERT(shape);
    const QPointF absPos = shape->absolutePosition();
    const QPointF destination = absPos + move;
    qreal bottomOfPage = 0.0;
    KWPage page;
    foreach (const KWPage &p, m_document->pageManager()->pages()) {
        bottomOfPage += p.height();
        if (bottomOfPage >= absPos.y())
            page = p;
        if (bottomOfPage >= destination.y()) {
            page = p;
            break;
        }
    }
    if (!page.isValid()) { // shape was not in any page to begin with, can't propose anything sane...
        move.setX(0);
        move.setY(0);
        return;
    }
    QRectF pageRect(page.rect().adjusted(5, 5, -5, -5));
    QPainterPath path(shape->absoluteTransformation(0).map(shape->outline()));
    QRectF shapeBounds = path.boundingRect();
    shapeBounds.moveTopLeft(shapeBounds.topLeft() + move);
    if (!shapeBounds.intersects(pageRect)) {
        if (shapeBounds.left() > pageRect.right()) // need to move to the left some
            move.setX(move.x() + (pageRect.right() - shapeBounds.left()));
        else if (shapeBounds.right() < pageRect.left()) // need to move to the right some
            move.setX(move.x() + pageRect.left() - shapeBounds.right());

        if (shapeBounds.top() > pageRect.bottom()) // need to move up some
            move.setY(move.y() + (pageRect.bottom() - shapeBounds.top()));
        else if (shapeBounds.bottom() < pageRect.top()) // need to move down some
            move.setY(move.y() + pageRect.top() - shapeBounds.bottom());
    }
}

KoGuidesData *KWCanvasBase::guidesData()
{
    return &m_document->guidesData();
}

KWDocument *KWCanvasBase::document() const
{
    return m_document;
}

KWViewMode *KWCanvasBase::viewMode() const
{
    return m_viewMode;
}

void KWCanvasBase::ensureVisible(const QRectF &rect)
{
    QRectF viewRect = m_viewMode->documentToView(rect);
    canvasController()->ensureVisible(viewRect);
}

void KWCanvasBase::paintPageDecorations(QPainter &painter, KWViewMode::ViewMap &viewMap)
{
    painter.save();

    const QRectF       pageRect = viewMap.page.rect();
    const KoPageLayout pageLayout = viewMap.page.pageStyle().pageLayout();

    // Get the coordinates of the border rect in view coordinates.
    QPointF topLeftCorner = viewConverter()->documentToView(pageRect.topLeft()
                                                            + QPointF(pageLayout.leftMargin,
                                                                      pageLayout.topMargin));
    QPointF bottomRightCorner = viewConverter()->documentToView(pageRect.bottomRight()
                                                                + QPointF(-pageLayout.rightMargin,
                                                                          -pageLayout.bottomMargin));
    QRectF borderRect = QRectF(topLeftCorner, bottomRightCorner);

    // Actually paint the border
    paintBorder(painter, pageLayout.border, borderRect);

    painter.restore();
}

void KWCanvasBase::paintBorder(QPainter &painter, const KoBorder &border, const QRectF &borderRect) const
{
    // Get the zoom.
    qreal zoomX;
    qreal zoomY;
    viewConverter()->zoom(&zoomX, &zoomY);

    KoBorder::BorderData borderSide = border.leftBorderData();
    painter.save();
    paintBorderSide(painter, borderSide, borderRect.topLeft(), borderRect.bottomLeft(),
                    zoomX, 1, 0);
    borderSide = border.topBorderData();
    painter.restore();
    painter.save();
    paintBorderSide(painter, borderSide, borderRect.topLeft(), borderRect.topRight(),
                    zoomY, 0, 1);

    borderSide = border.rightBorderData();
    painter.restore();
    painter.save();
    paintBorderSide(painter, borderSide, borderRect.topRight(), borderRect.bottomRight(),
                    zoomX, -1, 0);

    borderSide = border.bottomBorderData();
    painter.restore();
    painter.save();
    paintBorderSide(painter, borderSide, borderRect.bottomLeft(), borderRect.bottomRight(),
                    zoomY, 0, -1);
    painter.restore();
}

void KWCanvasBase::paintBorderSide(QPainter &painter, const KoBorder::BorderData &borderData,
                                   const QPointF &lineStart, const QPointF &lineEnd, qreal zoom,
                                   int inwardsX, int inwardsY) const
{
    // Return if nothing to paint
    if (borderData.style == KoBorder::BorderNone)
        return;

    // Set up the painter and inner and outer pens.
    QPen pen = painter.pen();
    // Line color
    pen.setColor(borderData.color);

    // Line style
    switch (borderData.style) {
    case KoBorder::BorderNone: break; // No line
    case KoBorder::BorderDotted: pen.setStyle(Qt::DotLine); break;
    case KoBorder::BorderDashed: pen.setStyle(Qt::DashLine); break;
    case KoBorder::BorderSolid: pen.setStyle(Qt::SolidLine); break;
    case KoBorder::BorderDouble: pen.setStyle(Qt::SolidLine); break; // Handled separately
    case KoBorder::BorderGroove: pen.setStyle(Qt::SolidLine); break; // FIXME
    case KoBorder::BorderRidge: pen.setStyle(Qt::SolidLine); break; // FIXME
    case KoBorder::BorderInset: pen.setStyle(Qt::SolidLine); break; // FIXME
    case KoBorder::BorderOutset: pen.setStyle(Qt::SolidLine); break; // FIXME

    case KoBorder::BorderDashDotPattern: pen.setStyle(Qt::DashDotLine); break;
    case KoBorder::BorderDashDotDotPattern: pen.setStyle(Qt::DashDotDotLine); break;
    }

    if (borderData.style == KoBorder::BorderDouble) {
        // outerWidth is the width of the outer line.  The offsets
        // are the distances from the center line of the whole
        // border to the centerlines of the outer and inner
        // borders respectively.
        qreal outerWidth = borderData.width - borderData.innerWidth - borderData.spacing;
        qreal outerOffset = borderData.width / 2.0 + outerWidth / 2.0;
        qreal innerOffset = borderData.width / 2.0 - borderData.innerWidth / 2.0;

        QPointF outerOffset2D(-inwardsX * outerOffset, -inwardsY * outerOffset);
        QPointF innerOffset2D(inwardsX * innerOffset, inwardsY * innerOffset);

        // Draw the outer line.
        pen.setWidthF(zoom * outerWidth);
        painter.setPen(pen);
        painter.drawLine(lineStart + outerOffset2D, lineEnd + outerOffset2D);

        // Draw the inner line
        pen.setWidthF(zoom * borderData.innerWidth);
        painter.setPen(pen);
        painter.drawLine(lineStart + innerOffset2D, lineEnd + innerOffset2D);
    }
    else {
        pen.setWidthF(zoom * borderData.width);
        painter.setPen(pen);
        painter.drawLine(lineStart, lineEnd);
    }
}

void KWCanvasBase::paint(QPainter &painter, const QRectF &paintRect)
{
    painter.translate(-m_documentOffset);

    //    static int iteration = 0;
    //    iteration++;

    if (m_viewMode->hasPages()) {
        int pageContentArea = 0;
        // Create a list of clipRects in the document space from the
        // current view. Each rect corresponds to a part of a page
        // that is shown on the canvas.
        //
        // Then go through them and paint each one.
        QList<KWViewMode::ViewMap> map =
                m_viewMode->clipRectToDocument(paintRect.toRect().translated(m_documentOffset));

        foreach (KWViewMode::ViewMap vm, map) {

            painter.save();

            // Set up the painter to clip the part of the canvas that contains the rect.
            painter.translate(vm.distance.x(), vm.distance.y());
            vm.clipRect = vm.clipRect.adjusted(-1, -1, 1, 1);
            painter.setClipRect(vm.clipRect);

            // Paint the background of the page.
            QColor color = Qt::white;
#ifdef DEBUG_REPAINT
            color = QColor(random() % 255, random() % 255, random() % 255);
#endif
            painter.fillRect(vm.clipRect, QBrush(color));

            // Paint the contents of the page.
            painter.setRenderHint(QPainter::Antialiasing);

            if (m_cacheEnabled && m_pageCacheManager) {

                // clear the cache if the zoom changed
                if (m_currentZoom != viewConverter()->zoom()) {
                    m_pageCacheManager->clear();
                    m_currentZoom = viewConverter()->zoom();
                }

                // we take the cache object from the cache, grabbing ownership because
                // QCache can decide to delete stuff from the cache at any moment.
                KWPageCache *pageCache = m_pageCacheManager->take(vm.page);

                if (!pageCache) {
                    pageCache = m_pageCacheManager->cache(QSize(viewConverter()->documentToViewX(vm.page.width()),
                                                                viewConverter()->documentToViewY(vm.page.height())));
                }

                Q_ASSERT(pageCache->cache);

                QSizeF pageSizeDocument(vm.page.width(), vm.page.height());
                QSizeF pageSizeView = viewConverter()->documentToView(pageSizeDocument);

                qreal  pageTopDocument = vm.page.offsetInDocument();
                qreal  pageTopView = viewConverter()->documentToViewY(pageTopDocument);

                QRectF pageRectDocument = vm.page.rect(vm.page.pageNumber());
                QRectF pageRectView = viewConverter()->documentToView(pageRectDocument);

                // translated from the page topleft to 0,0 for our cache image
                QRect clipRectOnPage = vm.clipRect.translated(-pageRectView.x(), -pageTopView);

#if 1 // render bit by bit

                // create exposed rects when the page is to be completely repainted.
                // we cannot wait for the updateCanvas calls to actually tell us which parts
                // need painting, because updateCanvas is not called when a page is done
                // layouting.
                if (pageCache->allExposed)  {

                    pageCache->exposed.clear();
                    QRect rc(QPoint(0,0), pageSizeView.toSize());

                    const int UPDATE_SIZE = 64; //pixels

                    if (rc.height() < UPDATE_SIZE) {
                        pageCache->exposed << rc;
                    }
                    else {
                        int row = 0;
                        int hleft = rc.height();
                        int w = rc.width();
                        while (hleft > 0) {
                            QRect rc2(0, row, w, qMin(hleft, UPDATE_SIZE));
                            pageCache->exposed << rc2;
                            hleft -= UPDATE_SIZE;
                            row += UPDATE_SIZE;
                        }
                    }
                    pageCache->allExposed = false;
                }

                // There is stuff to be repainted, so collect all the repaintable
                // rects that are in view and paint them.
                if (!pageCache->exposed.isEmpty()) {
                    QRegion paintRegion;
                    QRegion remainingUnExposed;
                    const QVector<QRect> &exposed = pageCache->exposed;
                    for (int i = 0; i < exposed.size(); ++i) {

                        QRect rc = exposed.at(i);

                        if (rc.intersects(clipRectOnPage)) {
                            paintRegion += rc;
                            QPainter gc(pageCache->cache);
                            gc.eraseRect(rc);
                            gc.end();
                        }
                        else {
                            remainingUnExposed += rc;
                        }
                    }

                    pageCache->exposed = remainingUnExposed.rects();

                    // paint the exposed regions of the page
                    QPainter gc(pageCache->cache);
                    gc.translate(0, -pageTopView);
                    gc.setClipRegion(paintRegion.translated(0, pageTopView));

                    shapeManager()->paint(gc, *viewConverter(), false);

                    //                    pageCache->cache.save(QString("page_%1_iteration_%2.png")
                    //                                           .arg(vm.page.pageNumber())
                    //                                           .arg(iteration));

                }
#else // use the thumbnailer to render the whole page in one go
                if (pageCache->allExposed || !pageCache->exposed.isEmpty()) {
                    pageCache->cache = vm.page.thumbnail(pageSizeView.toSize(), m_shapeManager);

                }
#endif
                // paint from the cached page image on the original painter

                QRect dst = QRect(pageRectView.x() + clipRectOnPage.x(),
                                  pageRectView.y() + clipRectOnPage.y(),
                                  clipRectOnPage.width(),
                                  clipRectOnPage.height());
                painter.drawImage(dst, *pageCache->cache, clipRectOnPage);

                // put the cache back
                m_pageCacheManager->insert(vm.page, pageCache, 100 * viewConverter()->zoom());

            }
            else {
                m_shapeManager->paint(painter, *(viewConverter()), false);
            }
            // Paint the page decorations: border, shadow, etc.
            paintPageDecorations(painter, vm);

            // Paint the grid
            painter.save();
            painter.translate(-vm.distance.x(), -vm.distance.y());
            painter.setRenderHint(QPainter::Antialiasing, false);
            const QRectF clipRect = viewConverter()->viewToDocument(vm.clipRect);
            document()->gridData().paintGrid(painter, *(viewConverter()), clipRect);
            document()->guidesData().paintGuides(painter, *(viewConverter()), clipRect);
            painter.restore();

            // paint whatever the tool wants to paint
            m_toolProxy->paint(painter, *(viewConverter()));
            painter.restore();

            int contentArea = qRound(vm.clipRect.width() * vm.clipRect.height());
            if (contentArea > pageContentArea) {
                pageContentArea = contentArea;
            }
        }
    } else {
        // TODO paint the main-text-flake directly
        kWarning(32003) << "Non-page painting not implemented yet!";
    }
}

void KWCanvasBase::updateCanvas(const QRectF &rc)
{
    QRectF zoomedRect = m_viewMode->documentToView(rc);
    QList<KWViewMode::ViewMap> map = m_viewMode->clipRectToDocument(zoomedRect.toRect());
    foreach (KWViewMode::ViewMap vm, map) {
        vm.clipRect.adjust(-2, -2, 2, 2); // grow for anti-aliasing
        QRect finalClip((int)(vm.clipRect.x() + vm.distance.x() - m_documentOffset.x()),
                        (int)(vm.clipRect.y() + vm.distance.y() - m_documentOffset.y()),
                        vm.clipRect.width(), vm.clipRect.height());

        if (m_cacheEnabled) {
            QRectF pageRectDocument = vm.page.rect(vm.page.pageNumber());
            QRectF pageRectView = viewConverter()->documentToView(pageRectDocument);

            if (!m_pageCacheManager) {
                // no pageCacheManager, so create one for the current view. This happens only once!
                // so on zoom change, we don't re-pre-generate weight/zoom images.
                m_pageCacheManager = new KWPageCacheManager(pageRectView.size().toSize(), m_cacheSize, viewConverter()->zoom());
            }

            if (!m_currentZoom == viewConverter()->zoom()) {
                m_currentZoom = viewConverter()->zoom();
                m_pageCacheManager->clear();
            }

            KWPageCache *pageCache = m_pageCacheManager->take(vm.page);
            if (pageCache) {
                if (rc.isNull()) {
                    pageCache->allExposed = true;
                    pageCache->exposed.clear();
                }
                else {
                    qreal  pageTopDocument = vm.page.offsetInDocument();
                    qreal  pageTopView = viewConverter()->documentToViewY(pageTopDocument);

                    // translated from the page topleft to 0,0 for our cache image
                    QRect clipRectOnPage = vm.clipRect.translated(-pageRectView.x(), -pageTopView);

                    pageCache->exposed.append(clipRectOnPage);
                }
                m_pageCacheManager->insert(vm.page, pageCache, viewConverter()->zoom());
            }
        }

        updateCanvasInternal(finalClip);
    }
}


KoViewConverter *KWCanvasBase::viewConverter() const
{
    return m_viewConverter;
}

void KWCanvasBase::setCacheEnabled(bool enabled, int cacheSize)
{
    m_cacheEnabled = enabled;
    // we scale the maxcost by 100 to be able to use the qreal zoomfactor to
    // use the totalcost to relate to the zoom factor.
    m_cacheSize = cacheSize;
}
