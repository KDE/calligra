/* This file is part of the KDE project
 * Copyright (C) 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
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
#include "KPrPresentationStrategyBase.h"

#include "KPrPresentationTool.h"
#include "KPrViewModePresentation.h"
#include "KPrPresentationStrategy.h"
#include "ui/KPrPresentationToolWidget.h"
#include "KPrPresentationToolEventForwarder.h"

KPrPresentationStrategyBase::KPrPresentationStrategyBase( KPrPresentationTool * tool )
: m_tool( tool )
, m_widget( 0 )
{
}

KPrPresentationStrategyBase::~KPrPresentationStrategyBase()
{
    delete m_widget;
}

void KPrPresentationStrategyBase::setToolWidgetParent( QWidget * widget )
{
    return m_tool->m_presentationToolWidget->setParent( widget );
}

KoPACanvasBase * KPrPresentationStrategyBase::canvas()
{
    return m_tool->m_viewMode.canvas();
}

void KPrPresentationStrategyBase::activateDefaultStrategy()
{
    m_tool->switchStrategy( new KPrPresentationStrategy( m_tool ) );
}

KPrPresentationToolEventForwarder* KPrPresentationStrategyBase::widget()
{
    return m_widget;
}
