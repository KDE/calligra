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

#include "KPrPresenterViewWidget.h"

#include <QtGui/QBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

#include <KDebug>

#include <KoPACanvas.h>

KPrPresenterViewWidget::KPrPresenterViewWidget( KoPACanvas *canvas, QWidget *parent)
    : QWidget( parent )
    , m_canvas( canvas )
{
    // currently this widget only shows the canvas in full screen
    // later will be turned into full presenter view widget
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget( m_canvas->canvasWidget() );
    setLayout(vLayout);
}

#include "KPrPresenterViewWidget.moc"

