/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPresentationHighlightWidget.h"

#include <KoPACanvasBase.h>

#include <QColor>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

KPrPresentationHighlightWidget::KPrPresentationHighlightWidget(KoPACanvasBase *canvas)
    : KPrPresentationToolEventForwarder(canvas)
    , m_size(canvas->canvasWidget()->size())
{
    // The focus and the track for have the mouse position every time
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    // Size of the canvas is saved because it's used in the paintEvent
    resize(m_size);

    m_center = QCursor::pos();
    update();
}

KPrPresentationHighlightWidget::~KPrPresentationHighlightWidget() = default;

/** paintEvent call with the update in the mouseMoveEvent */
void KPrPresentationHighlightWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPen myPen(Qt::black, 0);
    QColor c(Qt::black);
    // TODO make alpha configurable
    c.setAlphaF(0.5);
    // The circle we want
    QPainterPath ellipse;
    // TODO make radius configurable
    ellipse.addEllipse(m_center.x() - 75, m_center.y() - 75, 150, 150);
    // All the 'background'
    QPainterPath myPath;
    myPath.addRect(0, 0, m_size.rwidth(), m_size.rheight());
    // We draw the difference
    painter.setPen(myPen);
    painter.fillPath(myPath.subtracted(ellipse), c);
}

/** Take the mouse position every time the mouse is moving */
void KPrPresentationHighlightWidget::mouseMoveEvent(QMouseEvent *e)
{
    // Save the position of the mouse
    m_center = e->pos();
    // Update the screen : move the circle with a paint event
    // TODO maybe only update what has changed
    update();
}
