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

#include <QPoint>
#include <QSize>

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
    //setCanvas(doc->canvasItem());
}

void CanvasController::setVastScrolling(qreal factor)
{

}

void CanvasController::setZoomWithWheel(bool zoom)
{

}

void CanvasController::updateDocumentSize(const QSize& sz, bool recalculateCenter)
{

}

void CanvasController::setScrollBarValue(const QPoint& value)
{

}

QPoint CanvasController::scrollBarValue() const
{
    return QPoint();
}

void CanvasController::pan(const QPoint& distance)
{

}

QPoint CanvasController::preferredCenter() const
{
    return QPoint();
}

void CanvasController::setPreferredCenter(const QPoint& viewPoint)
{

}

void CanvasController::recenterPreferred()
{

}

void CanvasController::zoomTo(const QRect& rect)
{

}

void CanvasController::zoomBy(const QPoint& center, qreal zoom)
{

}

void CanvasController::zoomOut(const QPoint& center)
{

}

void CanvasController::zoomIn(const QPoint& center)
{

}

void CanvasController::ensureVisible(KoShape* shape)
{

}

void CanvasController::ensureVisible(const QRectF& rect, bool smooth)
{

}

int CanvasController::canvasOffsetY() const
{
    return 0;
}

int CanvasController::canvasOffsetX() const
{
    return 0;
}

int CanvasController::visibleWidth() const
{
    return 0;
}

int CanvasController::visibleHeight() const
{
    return 0;
}

KoCanvasBase* CanvasController::canvas() const
{
    return 0;
}

void CanvasController::setCanvas(KoCanvasBase* canvas)
{

}

void CanvasController::setDrawShadow(bool drawShadow)
{

}

QSize CanvasController::viewportSize() const
{
    return QSize();
}

void CanvasController::scrollContentsBy(int dx, int dy)
{

}

#include "CanvasController.moc"
