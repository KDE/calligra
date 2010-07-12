/* This file is part of the KDE project
 * Copyright (C) 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * Copyright (C) 2009 Thorsten Zachmann <zachmann@kde.org>
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
#include "KPrPresentationBlackWidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QPainter>
#include <QRect>

KPrPresentationBlackWidget::KPrPresentationBlackWidget( KoPACanvas * canvas )
: KPrPresentationToolEventForwarder(canvas)
, m_size( canvas->size() )
{
    // Size of the canvas is saved because it's used in the paintEvent
    resize( m_size );
    update();
}

KPrPresentationBlackWidget::~KPrPresentationBlackWidget()
{
}

void KPrPresentationBlackWidget::paintEvent( QPaintEvent * event )
{
    Q_UNUSED(event);
    QPainter painter( this );
    painter.fillRect(0,0,m_size.width(), m_size.height(), Qt::black );
}
