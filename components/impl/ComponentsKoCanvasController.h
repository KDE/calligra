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

#ifndef CALLIGRA_COMPONENTS_COMPONENTSKOCANVASCONTROLLER_H
#define CALLIGRA_COMPONENTS_COMPONENTSKOCANVASCONTROLLER_H

#include <KoCanvasController.h>

#include <QPoint>
#include <QSize>

class KoCanvasBase;
namespace Calligra
{
namespace Components
{

class ComponentsKoCanvasController : public QObject, public KoCanvasController
{
    Q_OBJECT
public:
    explicit ComponentsKoCanvasController(KActionCollection *actionCollection);
    ~ComponentsKoCanvasController() override;

    void setVastScrolling(qreal factor) override;
    void setZoomWithWheel(bool zoom) override;
    void updateDocumentSize(const QSize &sz, bool recalculateCenter) override;
    void setScrollBarValue(const QPoint &value) override;
    QPoint scrollBarValue() const override;
    void pan(const QPoint &distance) override;
    QPointF preferredCenter() const override;
    void setPreferredCenter(const QPointF &viewPoint) override;
    void recenterPreferred() override;
    void zoomTo(const QRect &rect) override;
    void zoomBy(const QPoint &center, qreal zoom) override;
    void zoomOut(const QPoint &center) override;
    void zoomIn(const QPoint &center) override;
    void ensureVisible(KoShape *shape) override;
    void ensureVisible(const QRectF &rect, bool smooth) override;
    int canvasOffsetY() const override;
    int canvasOffsetX() const override;
    int visibleWidth() const override;
    int visibleHeight() const override;
    KoCanvasBase *canvas() const override;
    void setCanvas(KoCanvasBase *canvas) override;
    void setDrawShadow(bool drawShadow) override;
    QSize viewportSize() const override;
    void scrollContentsBy(int dx, int dy) override;

    QSize documentSize();

Q_SIGNALS:
    void documentSizeChanged(const QSize &sz);
    void documentPositionChanged(const QPoint &pos);

private:
    class Private;
    Private *const d;
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_COMPONENTSKOCANVASCONTROLLER_H
