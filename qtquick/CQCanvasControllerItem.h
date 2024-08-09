/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CQCANVASCONTROLLERITEM_H
#define CQCANVASCONTROLLERITEM_H

#include <QQuickPaintedItem>

class CQCanvasControllerItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    Q_PROPERTY(QQuickItem *flickable READ flickable WRITE setFlickable NOTIFY flickableChanged)
    Q_PROPERTY(QSize documentSize READ documentSize NOTIFY documentSizeChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(qreal minimumZoom READ minimumZoom WRITE setMinimumZoom NOTIFY minimumZoomChanged)
    Q_PROPERTY(qreal maximumZoom READ maximumZoom WRITE setMaximumZoom NOTIFY maximumZoomChanged)
public:
    explicit CQCanvasControllerItem(QQuickItem *parent = nullptr);
    ~CQCanvasControllerItem();

    void paint(QPainter *painter) override;

    QQuickItem *canvas() const;
    void setCanvas(QQuickItem *canvas);

    QQuickItem *flickable() const;
    void setFlickable(QQuickItem *item);

    QSize documentSize() const;

    qreal zoom() const;
    void setZoom(qreal newZoom);
    /**
     * Zoom to fit page, but without changing the mode. This is used for
     * reader mode in Calligra Gemini.
     */
    Q_INVOKABLE void zoomToPage();

    qreal minimumZoom() const;
    void setMinimumZoom(qreal newZoom);

    qreal maximumZoom() const;
    void setMaximumZoom(qreal newZoom);

public Q_SLOTS:
    void beginZoomGesture();
    void endZoomGesture();

    void zoomBy(qreal amount, const QPointF &center = QPointF());

    void fitToWidth(qreal width);

    void returnToBounds();

Q_SIGNALS:
    void canvasChanged();
    void flickableChanged();
    void documentSizeChanged();
    void zoomChanged();
    void minimumZoomChanged();
    void maximumZoomChanged();
    void movingFastChanged();

protected:
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private Q_SLOTS:
    void updateDocumentSize(const QSize &size);
    void updateDocumentPosition(const QPoint &pos);
    void canvasControllerChanged();

private:
    class Private;
    Private *const d;
};

#endif // CQCANVASCONTROLLERITEM_H
