/* This file is part of the KDE project
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
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

#include <QtGui/QVBoxLayout>

#include <KoPACanvas.h>

#include "KPrPresentationHighlightWidget.h"

KPrPresentationHighlightWidget::KPrPresentationHighlightWidget(KoPACanvas * canvas) : QWidget(canvas)
{
    QSize size = canvas->size();
    
    QPixmap newPage( size );

    QColor c(Qt::black); c.setAlphaF(0.5); newPage.fill(c);

    resize(size);
    m_blackBackgroundframe = new QFrame(this);
    QVBoxLayout *frameLayout2 = new QVBoxLayout();
    QLabel *label = new QLabel();
    
    label->setPixmap(newPage);
    
    frameLayout2->addWidget( label, 0, Qt::AlignCenter );
    m_blackBackgroundframe->setLayout( frameLayout2 );
    m_blackBackgroundframe->move( -4,-4 );

    // change the visibility
    if ( !m_blackBackgroundVisibility )
    {
	m_blackBackgroundVisibility = true;
	setVisible(false);
    }
    else
    {
	m_blackBackgroundVisibility = false;
	setVisible(true);
    }
}

KPrPresentationHighlightWidget::~KPrPresentationHighlightWidget()
{
    delete m_blackBackgroundframe;  
}