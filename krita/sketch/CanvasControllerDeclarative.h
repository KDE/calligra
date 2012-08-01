/* This file is part of the KDE project
 * Copyright (C) 2012 KO GmbH. Contact: Boudewijn Rempt <boud@kogmbh.com>
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
#ifndef KRITA_SKETCH_CANVASCONTROLLER_DECLARATIVE_H
#define KRITA_SKETCH_CANVASCONTROLLER_DECLARATIVE_H

#include <QtDeclarative/QDeclarativeItem>
#include <KoCanvasController.h>

class KoZoomHandler;
class KoZoomController;
class KoViewConverter;

class CanvasControllerDeclarative : public QDeclarativeItem, public KoCanvasController
{
    Q_OBJECT
    Q_PROPERTY(QString file READ file WRITE setFile)
    Q_PROPERTY(int visibleWidth READ visibleWidth)
    Q_PROPERTY(int visibleHeight READ visibleHeight)

    Q_PROPERTY(QSize documentSize READ documentSize NOTIFY documentSizeChanged)
    Q_PROPERTY(QPoint documentOffset READ getDocumentOffset)

    Q_PROPERTY(QDeclarativeItem* verticalScrollHandle READ verticalScrollHandle WRITE setVerticalScrollHandle)
    Q_PROPERTY(QDeclarativeItem* horizontalScrollHandle READ horizontalScrollHandle WRITE setHorizontalScrollHandle)

    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)

public:
    CanvasControllerDeclarative(QDeclarativeItem* parent = 0);
    virtual ~CanvasControllerDeclarative();

    QString file() const;
    void setFile(const QString &f);

    virtual void scrollContentsBy(int dx, int dy);
    virtual QSize viewportSize() const;

    virtual void resetDocumentOffset(const QPoint& offset = QPoint());

    virtual void setDrawShadow(bool drawShadow);

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
    virtual void ensureVisible(KoShape* shape);
    virtual void ensureVisible(const QRectF& rect, bool smooth = false);
    virtual int canvasOffsetY() const;
    virtual int canvasOffsetX() const;
    virtual int visibleWidth() const;
    virtual int visibleHeight() const;
    virtual KoCanvasBase* canvas() const;
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void setMargin(int margin);

    KoZoomHandler* zoomHandler() const;
    void setZoomMax(qreal newZoomMax);
    qreal zoomMax() const;
    void setZoomMin(qreal newZoomMin);
    qreal zoomMin() const;

    QPoint getDocumentOffset();

    Q_SLOT void setForce(const QVector2D& newForce);
    QVector2D force() const;

    QDeclarativeItem *verticalScrollHandle();
    QDeclarativeItem *horizontalScrollHandle();

    int zoomLevel() const;

public Q_SLOTS:
    virtual void setVerticalScrollHandle(QDeclarativeItem *handle);
    virtual void setHorizontalScrollHandle(QDeclarativeItem *handle);

    virtual void setZoomLevel(int zoomPercentage);
    virtual void zoomOut(const QPoint& center = QPoint());
    virtual void zoomIn(const QPoint& center = QPoint());
    virtual void zoomBy(const QPoint& center, qreal zoom);
    virtual void resetZoom();

Q_SIGNALS:
    void docMoved();

    void nextPage();
    void previousPage();

    void progress(int progress);
    void completed();

    void documentSizeChanged();

    void showVerticalScrollHandle();
    void hideVerticalScrollHandle();
    void showHorizontalScrollHandle();
    void hideHorizontalScrollHandle();

    void zoomLevelChanged();

protected:
    virtual bool eventFilter(QObject* target, QEvent* event );
    KoZoomController* zoomController(KoViewConverter* viewConverter = 0, bool recreate = false);

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void updateCanvasSize());
    Q_PRIVATE_SLOT(d, void documentOffsetMoved(const QPoint& point));
    Q_PRIVATE_SLOT(d, void timerUpdate());
};

#endif //KRITA_SKETCH_CANVASCONTROLLERDECLARATIVE_H
