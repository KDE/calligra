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

#ifndef KPRVIEWMODEPRESENTERVIEW_H
#define KPRVIEWMODEPRESENTERVIEW_H

#include "KPrViewModePresentation.h"
#include "KPrAnimationDirector.h"

class KoPAView;
class KoPACanvas;
class KoPAPageBase;
class KoPAViewMode;

class KPrPresenterViewWidget;
class KPrPresentationCanvas;
class KPrPresenterViewTool;

class KPrViewModePresenterView : public KPrViewModePresentation
{
public:
    KPrViewModePresenterView(KoPAView *view, KoPACanvas *canvas, KPrViewModePresentation *presentationMode);
    ~KPrViewModePresenterView();

    void tabletEvent(QTabletEvent *event, const QPointF &point);
    void mousePressEvent(QMouseEvent *event, const QPointF &point);
    void mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point);
    void mouseMoveEvent(QMouseEvent *event, const QPointF &point);
    void mouseReleaseEvent(QMouseEvent *event, const QPointF &point);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent * event, const QPointF &point);

    void activate(KoPAViewMode *previousViewMode);
    void deactivate();

    /// reimplemented
    void updateActivePage( KoPAPageBase *page );

private:
    KPrPresenterViewWidget *m_presenterViewWidget;
    KPrViewModePresentation *m_presentationMode;
};

#endif // KPRVIEWMODEPRESENTERVIEW_H

