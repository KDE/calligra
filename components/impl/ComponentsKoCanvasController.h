/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * Copyright (C) 2013 Sujith Haridasan <sujith.h@gmail.com>
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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
 *
 */

#ifndef CALLIGRA_COMPONENTS_COMPONENTSKOCANVASCONTROLLER_H
#define CALLIGRA_COMPONENTS_COMPONENTSKOCANVASCONTROLLER_H

#include <KoCanvasController.h>

#include <QSize>
#include <QPoint>

class KoCanvasBase;
namespace Calligra {
namespace Components {

class ComponentsKoCanvasController : public QObject, public KoCanvasController
{
    Q_OBJECT
public:
    explicit ComponentsKoCanvasController(KActionCollection* actionCollection);
    ~ComponentsKoCanvasController() override;
 
    void setVastScrolling(qreal factor) override;
    void setZoomWithWheel(bool zoom) override;
    void updateDocumentSize(const QSize& sz, bool recalculateCenter) override;
    void setScrollBarValue(const QPoint& value) override;
    QPoint scrollBarValue() const override;
    void pan(const QPoint& distance) override;
    QPointF preferredCenter() const override;
    void setPreferredCenter(const QPointF& viewPoint) override;
    void recenterPreferred() override;
    void zoomTo(const QRect& rect) override;
    void zoomBy(const QPoint& center, qreal zoom) override;
    void zoomOut(const QPoint& center) override;
    void zoomIn(const QPoint& center) override;
    void ensureVisible(KoShape* shape) override;
    void ensureVisible(const QRectF& rect, bool smooth) override;
    int canvasOffsetY() const override;
    int canvasOffsetX() const override;
    int visibleWidth() const override;
    int visibleHeight() const override;
    KoCanvasBase* canvas() const override;
    void setCanvas(KoCanvasBase* canvas) override;
    void setDrawShadow(bool drawShadow) override;
    QSize viewportSize() const override;
    void scrollContentsBy(int dx, int dy) override;

    QSize documentSize();

Q_SIGNALS:
    void documentSizeChanged(const QSize &sz);
    void documentPositionChanged(const QPoint &pos);

private:
    class Private;
    Private * const d;
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_COMPONENTSKOCANVASCONTROLLER_H


