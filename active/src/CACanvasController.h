/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
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


#ifndef CACANVASCONTROLLER_H
#define CACANVASCONTROLLER_H

#include "KoCanvasController.h"
#include "CADocumentInfo.h"

#include <KWPage.h>
#include <KoZoomMode.h>

#include <QDeclarativeItem>

class CAAbstractDocumentHandler;
class CADocumentController;
class CACanvasItem;
class KoCanvasBase;
class KoZoomController;
class KoZoomHandler;

class CACanvasController : public QDeclarativeItem, public KoCanvasController
{
    Q_OBJECT

    Q_PROPERTY (int cameraX READ cameraX WRITE setCameraX NOTIFY cameraXChanged)
    Q_PROPERTY (int cameraY READ cameraY WRITE setCameraY NOTIFY cameraYChanged)
    Q_PROPERTY (qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY (QObject* caCanvasItem READ caCanvasItem WRITE setCACanvasItem NOTIFY caCanvasItemChanged)

public:
    explicit CACanvasController (QDeclarativeItem* parent = 0);
    virtual ~CACanvasController();
    virtual void setVastScrolling (qreal factor);
    virtual void setZoomWithWheel (bool zoom);
    virtual void updateDocumentSize (const QSize& sz, bool recalculateCenter);
    virtual void setScrollBarValue (const QPoint& value);
    virtual QPoint scrollBarValue() const;
    virtual void pan (const QPoint& distance);
    virtual QPointF preferredCenter() const;
    virtual void setPreferredCenter (const QPointF& viewPoint);
    virtual void recenterPreferred();
    virtual void zoomTo (const QRect& rect);
    virtual void zoomBy (const QPoint& center, qreal zoom);
    virtual void zoomOut (const QPoint& center);
    virtual void zoomIn (const QPoint& center);
    virtual void ensureVisible (KoShape* shape);
    virtual void ensureVisible (const QRectF& rect, bool smooth = false);
    virtual int canvasOffsetY() const;
    virtual int canvasOffsetX() const;
    virtual int visibleWidth() const;
    virtual int visibleHeight() const;
    virtual KoCanvasBase* canvas() const;
    virtual void setCanvas (KoCanvasBase* canvas);
    virtual void setDrawShadow (bool drawShadow);
    virtual QSize viewportSize() const;
    virtual void scrollContentsBy (int dx, int dy);
    QObject *caCanvasItem();
    void setCACanvasItem(QObject *caCanvas);

    int cameraX() const;
    int cameraY() const;
    void setCameraX (int cameraX);
    void setCameraY (int cameraY);
    qreal zoom() const;
    void setZoom(qreal zoom);
    void alignTopWith(int y);
    void alignLeftWith(int x);

    KoCanvasControllerProxyObject* canvasControllerProxyObject();
    KoZoomHandler* zoomHandler();
    KoZoomController* zoomController();
    void setZoomHandler (KoZoomHandler* zoomHandler);
    void setDocumentHandler (CAAbstractDocumentHandler *documentHandler);

public slots:
    void zoomToFit();
    void updateCanvas();
    void updateZoomValue(KoZoomMode::Mode mode, qreal zoom);

private:
    KoZoomHandler* m_zoomHandler;
    KoZoomController* m_zoomController;
    QPoint m_currentPoint;
    QSizeF m_documentSize;
    QList<CADocumentInfo*> m_recentFiles;
    qreal m_zoom;
    CACanvasItem *m_caCanvasItem;
    CAAbstractDocumentHandler *m_caDocumentHandler;

protected:
    virtual void geometryChanged (const QRectF& newGeometry, const QRectF& oldGeometry);

signals:
    void cameraXChanged();
    void cameraYChanged();
    void needCanvasUpdate();
    void zoomChanged();
    void caCanvasItemChanged();
};

#endif // CACANVASCONTROLLER_H
