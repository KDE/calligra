/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KPrPresentationBlackWidget.h"

#include <KoPACanvasBase.h>

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QRect>

KPrPresentationBlackWidget::KPrPresentationBlackWidget(KoPACanvasBase *canvas)
    : KPrPresentationToolEventForwarder(canvas)
    , m_size(canvas->canvasWidget()->size())
{
    // Size of the canvas is saved because it's used in the paintEvent
    resize(m_size);
    update();
}

KPrPresentationBlackWidget::~KPrPresentationBlackWidget() = default;

void KPrPresentationBlackWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(0, 0, m_size.width(), m_size.height(), Qt::black);
}
