/* This file is part of the KDE project
   Copyright (C) 2011 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrViewModeOutline.h"

#include "KoPAView.h"
#include "KoPACanvas.h"

#include "KPrOutlineEditor.h"

KPrViewModeOutline::KPrViewModeOutline(KoPAView *view, KoPACanvas *canvas)
    : KoPAViewMode( view, canvas )
    , m_outlineEditor(new KPrOutlineEditor(this, view->parentWidget()))
{
    m_outlineEditor->hide();
}

void KPrViewModeOutline::paint(KoPACanvasBase* canvas, QPainter& painter, const QRectF &paintRect)
{
    Q_UNUSED(canvas);
    Q_UNUSED(painter);
    Q_UNUSED(paintRect);
}

void KPrViewModeOutline::tabletEvent(QTabletEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mousePressEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseMoveEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseReleaseEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModeOutline::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModeOutline::wheelEvent(QWheelEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::activate(KoPAViewMode *previousViewMode)
{
    Q_UNUSED(previousViewMode);
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->hide();
    }
    m_outlineEditor->show();
    m_outlineEditor->setFocus(Qt::ActiveWindowFocusReason);
}


void KPrViewModeOutline::deactivate()
{
    m_outlineEditor->hide();
     // Active the view as a basic but active one
    m_view->setActionEnabled(KoPAView::AllActions, true);
    m_view->doUpdateActivePage(m_view->activePage());
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->show();
    }
}
