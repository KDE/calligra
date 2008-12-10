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

#include "KPrPresentationToolWidget.h"

#include <KLocale>
#include <KDebug>

#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

#include <KIcon>

KPrPresentationToolWidget::KPrPresentationToolWidget( QWidget *parent )
    : QWidget( parent )
{
    // This QWidget will be the setup User Interface
    m_uiWidget.setupUi( this );
    
    // Set the size of the icon
    m_uiWidget.penButton->setIconSize( QSize( 22,22 ) );
    m_uiWidget.highLightButton->setIconSize( QSize( 22,22 ) );
    // Set the buttons Icon
    m_uiWidget.penButton->setIcon(KIcon("pen.png"));
    m_uiWidget.highLightButton->setIcon(KIcon("highlight.png"));
    
}

void KPrPresentationToolWidget::paintEvent(QPaintEvent *event)
{
    //For the transparent background...
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::transparent );
}

Ui::KPrPresentationTool KPrPresentationToolWidget::presentationToolUi()
{
    return m_uiWidget;
}

#include "KPrPresentationToolWidget.moc"

