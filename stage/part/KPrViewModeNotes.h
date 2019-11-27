/* This file is part of the KDE project
 * Copyright (C) 2008 Fredy Yanardi <fyanardi@gmail.com>
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

#ifndef KPRVIEWMODENOTES_H
#define KPRVIEWMODENOTES_H

#include <KoPageApp.h>
#include <KoPAViewMode.h>

class KoPAView;
class KoPAPageBase;

class KPrViewModeNotes : public KoPAViewMode
{
public:
    KPrViewModeNotes(KoPAViewBase *view, KoPACanvasBase *canvas);
    ~KPrViewModeNotes() override;

    void paint(KoPACanvasBase* canvas, QPainter& painter, const QRectF &paintRect) override;
    void tabletEvent(QTabletEvent *event, const QPointF &point) override;
    void mousePressEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseMoveEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseReleaseEvent(QMouseEvent *event, const QPointF &point) override;
    void shortcutOverrideEvent(QKeyEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent * event, const QPointF &point) override;

    void activate(KoPAViewMode *previousViewMode) override;
    void deactivate() override;

    void updateActivePage( KoPAPageBase *page ) override;

    void addShape(KoShape *shape) override;
    void removeShape(KoShape *shape) override;

    const KoPageLayout &activePageLayout() const override;
};

#endif // KPRVIEWMODENOTES_H

