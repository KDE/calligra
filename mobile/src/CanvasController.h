/*
 * This file is part of the KDE project
 *
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


#ifndef CANVASCONTROLLER_H
#define CANVASCONTROLLER_H

#include "KoCanvasController.h"
#include <QDeclarativeItem>

class CanvasController : public QDeclarativeItem, KoCanvasController
{
    Q_OBJECT
public:
    virtual void setVastScrolling(qreal factor);
    virtual void setZoomWithWheel(bool zoom);
    virtual void updateDocumentSize(const QSize& sz, bool recalculateCenter);
    virtual void setScrollBarValue(const QPoint& value);
    virtual QPoint scrollBarValue() const;
    virtual void pan(const QPoint& distance);
    virtual QPoint preferredCenter() const;
    virtual void setPreferredCenter(const QPoint& viewPoint);
    virtual void recenterPreferred();
    virtual void zoomTo(const QRect& rect);
    virtual void zoomBy(const QPoint& center, qreal zoom);
    virtual void zoomOut(const QPoint& center);
    virtual void zoomIn(const QPoint& center);
    virtual void ensureVisible(KoShape* shape);
    virtual void ensureVisible(const QRectF& rect, bool smooth = false);
    virtual int canvasOffsetY() const;
    virtual int canvasOffsetX() const;
    virtual int visibleWidth() const;
    virtual int visibleHeight() const;
    virtual KoCanvasBase* canvas() const;
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void setDrawShadow(bool drawShadow);
    virtual QSize viewportSize() const;
    virtual void scrollContentsBy(int dx, int dy);

    explicit CanvasController(KActionCollection* actionCollection = 0);
};

#endif // CANVASCONTROLLER_H
