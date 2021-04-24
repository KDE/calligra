/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Shantanu Tushar <shantanu@kde.org>
 * SPDX-FileCopyrightText: 2013 Sujith Haridasan <sujith.h@gmail.com>
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CQCANVASCONTROLLER_H
#define CQCANVASCONTROLLER_H

#include <KoCanvasController.h>

class QSize;
class QPoint;
class KoCanvasBase;
class CQCanvasController : public QObject, public KoCanvasController
{
    Q_OBJECT
public:
    explicit CQCanvasController(KActionCollection* actionCollection);
    virtual ~CQCanvasController();

    virtual void setVastScrolling(qreal factor);
    virtual void setZoomWithWheel(bool zoom);
    virtual void updateDocumentSize(const QSize& sz, bool recalculateCenter);
    virtual void setScrollBarValue(const QPoint& value);
    virtual QPoint scrollBarValue() const;
    virtual void pan(const QPoint& distance);
    virtual QPointF preferredCenter() const;
    virtual void setPreferredCenter(const QPointF& viewPoint);
    virtual void recenterPreferred();
    virtual void zoomTo(const QRect& rect);
    virtual void zoomBy(const QPoint& center, qreal zoom);
    virtual void zoomOut(const QPoint& center);
    virtual void zoomIn(const QPoint& center);
    virtual void ensureVisible(KoShape* shape);
    virtual void ensureVisible(const QRectF& rect, bool smooth);
    virtual int canvasOffsetY() const;
    virtual int canvasOffsetX() const;
    virtual int visibleWidth() const;
    virtual int visibleHeight() const;
    virtual KoCanvasBase* canvas() const;
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void setDrawShadow(bool drawShadow);
    virtual QSize viewportSize() const;
    virtual void scrollContentsBy(int dx, int dy);

    QSize documentSize() const;

Q_SIGNALS:
    void documentSizeChanged(const QSize &sz);
    void documentPositionChanged(const QPoint &pos);

private:
    class Private;
    Private * const d;
};

#endif // CQCANVASCONTROLLER_H


