/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2010-2011 Jarosław Staniek <staniek@kde.org>
 * Copyright (C) 2011 Shantanu Tushar <jhahoneyk@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */


#include "CanvasController.h"

#include <KoDocument.h>
#include <KMimeType>
#include <KMimeTypeTrader>
#include <KoView.h>
#include <KoCanvasBase.h>
#include <KWCanvasItem.h>
#include <KDebug>

#include <QPoint>
#include <QSize>
#include <QGraphicsWidget>

CanvasController::CanvasController(KActionCollection* actionCollection)
    : KoCanvasController(actionCollection), QDeclarativeItem()
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

void CanvasController::openDocument(const QString& path)
{
    QString error;
    QString mimetype = KMimeType::findByPath(path)->name();
    KoDocument *doc = KMimeTypeTrader::createPartInstanceFromQuery<KoDocument>(mimetype, 0, 0, QString(),
                                                                               QVariantList(), &error);
    doc->openUrl(KUrl(path));

    // get the one canvas item for this document
    KWCanvasItem *canvas = dynamic_cast<KWCanvasItem*>(doc->canvasItem());
    if (canvas) {
        // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
        connect(proxyObject, SIGNAL(moveDocumentOffset(const QPoint&)), canvas, SLOT(setDocumentOffset(QPoint)));
        // whenever the size of the document viewed in the canvas changes, inform the zoom controller
        connect(canvas, SIGNAL(documentSize(QSizeF)), zoomController(), SLOT(setDocumentSize(QSizeF)));
        canvas->updateSize();

        setCanvas(static_cast<KoCanvasBase*>(canvas));
    }
}

void CanvasController::setVastScrolling(qreal factor)
{
    kDebug() << factor;
}

void CanvasController::setZoomWithWheel(bool zoom)
{
    kDebug() << zoom;
}

void CanvasController::updateDocumentSize(const QSize& sz, bool recalculateCenter)
{
    kDebug() << sz << recalculateCenter;
}

void CanvasController::setScrollBarValue(const QPoint& value)
{
    kDebug() << value;
}

QPoint CanvasController::scrollBarValue() const
{
    return QPoint();
}

void CanvasController::pan(const QPoint& distance)
{
    kDebug() << distance;
}

QPoint CanvasController::preferredCenter() const
{
    return QPoint();
}

void CanvasController::setPreferredCenter(const QPoint& viewPoint)
{
    kDebug() << viewPoint;
}

void CanvasController::recenterPreferred()
{
    kDebug() << "BLEH";
}

void CanvasController::zoomTo(const QRect& rect)
{
    kDebug() << rect;
}

void CanvasController::zoomBy(const QPoint& center, qreal zoom)
{
    kDebug() << center << zoom;
}

void CanvasController::zoomOut(const QPoint& center)
{
    kDebug() << center;
}

void CanvasController::zoomIn(const QPoint& center)
{
    kDebug() << center;
}

void CanvasController::ensureVisible(KoShape* shape)
{
    kDebug() << shape;
}

void CanvasController::ensureVisible(const QRectF& rect, bool smooth)
{
    kDebug() << rect << smooth;
}

int CanvasController::canvasOffsetY() const
{
    kDebug() << "ASKING";
    return 0;
}

int CanvasController::canvasOffsetX() const
{
    return 0;
}

int CanvasController::visibleWidth() const
{
    kDebug() << "ASKING";
    return 0;
}

int CanvasController::visibleHeight() const
{
    kDebug() << "ASKING";
    return 0;
}

KoCanvasBase* CanvasController::canvas() const
{
    kDebug() << "ASKING";
    return 0;
}

void CanvasController::setCanvas(KoCanvasBase* canvas)
{
    QGraphicsWidget *widget = dynamic_cast<QGraphicsWidget*>(canvas);
    widget->setParentItem(this);
    widget->setVisible(true);
    widget->setGeometry(0,0,width(),height());
    KoToolManager::instance()->addController(this);
}

void CanvasController::setDrawShadow(bool drawShadow)
{
    kDebug() << "ASKING";
    kDebug() << drawShadow;
}

QSize CanvasController::viewportSize() const
{
    kDebug() << "ASKING";
    return QSize();
}

void CanvasController::scrollContentsBy(int dx, int dy)
{
    kDebug() << dx << dy;
}

#include "CanvasController.moc"
