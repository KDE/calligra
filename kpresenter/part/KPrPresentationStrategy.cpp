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

#include "KPrPresentationStrategy.h"

#include "KPrPresentationHighlightStrategy.h"
#include "KPrPresentationDrawStrategy.h"
#include "KPrPresentationViewModeStrategy.h"

KPrPresentationStrategy::KPrPresentationStrategy( KPrPresentationTool * tool )
{
    m_tool = tool;
}

KPrPresentationStrategy::~KPrPresentationStrategy()
{
}

void KPrPresentationStrategy::handleEscape()
{
    if ( m_tool->getDrawMode() ) {
        KPrPresentationDrawStrategy m_drawStrategy( m_tool );
        m_drawStrategy.handleEscape();
    }
    else if ( m_tool->getHighlightMode() ) {
        KPrPresentationHighlightStrategy m_highlightStrategy( m_tool );
        m_highlightStrategy.handleEscape();
    }
    else {
        KPrPresentationViewModeStrategy m_viewModeStrategy( m_tool );
        m_viewModeStrategy.handleEscape();
    }
}

