/* This file is part of the KDE project
 * Copyright (C) 2008 Jim Courtiau <jeremy.courtiau@gmail.com>
 * Copyright (C) 2009 Johann Hingue <yoan1703@hotmail.fr>
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

#include "KPrPresentationToolWidget.h"

#include <KoIcon.h>

#include <QPainter>
#include <QPaintEvent>
#include <QDesktopWidget>

KPrPresentationToolWidget::KPrPresentationToolWidget( QWidget *parent )
: QWidget( parent )
{
    // This QWidget will be the setup User Interface
    m_uiWidget.setupUi( this );

    // Set the size of the icon
    QDesktopWidget desktop;
    QRect rect = desktop.screenGeometry( desktop.screenNumber( this ) );
    QSize size = QSize( rect.width() / 40, rect.width() / 40 );
    m_uiWidget.penButton->setIconSize( size );
    m_uiWidget.highLightButton->setIconSize( size );
    m_uiWidget.blackButton->setIconSize( size );
    // Set the buttons Icon
    // TODO add tooltip
    m_uiWidget.penButton->setIcon(koIcon("draw-freehand") );
    m_uiWidget.highLightButton->setIcon(koIcon("highlight-pointer-spot"));
    m_uiWidget.blackButton->setIcon(koIcon("pause-to-black-screen"));
}

void KPrPresentationToolWidget::paintEvent( QPaintEvent *event )
{
    //For the transparent background...
    QPainter painter( this );
    painter.fillRect( event->rect(), Qt::transparent );
}

Ui::KPrPresentationTool KPrPresentationToolWidget::presentationToolUi()
{
    return m_uiWidget;
}
