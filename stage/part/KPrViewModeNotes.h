/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRVIEWMODENOTES_H
#define KPRVIEWMODENOTES_H

#include <KoPAViewMode.h>
#include <KoPageApp.h>

class KoPAView;
class KoPAPageBase;

class KPrViewModeNotes : public KoPAViewMode
{
public:
    KPrViewModeNotes(KoPAViewBase *view, KoPACanvasBase *canvas);
    ~KPrViewModeNotes() override;

    void paint(KoPACanvasBase *canvas, QPainter &painter, const QRectF &paintRect) override;
    void tabletEvent(QTabletEvent *event, const QPointF &point) override;
    void mousePressEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseMoveEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseReleaseEvent(QMouseEvent *event, const QPointF &point) override;
    void shortcutOverrideEvent(QKeyEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event, const QPointF &point) override;

    void activate(KoPAViewMode *previousViewMode) override;
    void deactivate() override;

    void updateActivePage(KoPAPageBase *page) override;

    void addShape(KoShape *shape) override;
    void removeShape(KoShape *shape) override;

    const KoPageLayout &activePageLayout() const override;
};

#endif // KPRVIEWMODENOTES_H
