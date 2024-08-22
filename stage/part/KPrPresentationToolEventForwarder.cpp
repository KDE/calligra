/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPresentationToolEventForwarder.h"

#include <KoPACanvas.h>

KPrPresentationToolEventForwarder::KPrPresentationToolEventForwarder(KoPACanvasBase *canvas)
    : QWidget(canvas->canvasWidget())
{
}

KPrPresentationToolEventForwarder::~KPrPresentationToolEventForwarder() = default;

void KPrPresentationToolEventForwarder::receiveMousePressEvent(QMouseEvent *event)
{
    mousePressEvent(event);
}

void KPrPresentationToolEventForwarder::receiveMouseMoveEvent(QMouseEvent *event)
{
    mouseMoveEvent(event);
}

void KPrPresentationToolEventForwarder::receiveMouseReleaseEvent(QMouseEvent *event)
{
    mouseReleaseEvent(event);
}
