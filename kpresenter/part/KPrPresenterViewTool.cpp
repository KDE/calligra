/* This file is part of the KDE project
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
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

#include "KPrPresenterViewTool.h"

#include <QtGui/QKeyEvent>

#include <KDebug>

#include <KoPointerEvent.h>
#include <KoPACanvas.h>

#include "KPrViewModePresentation.h"
#include "KPrViewModePresenterView.h"
#include "KPrPresentationTool.h"

KPrPresenterViewTool::KPrPresenterViewTool( KPrViewModePresenterView &viewMode, KPrPresentationTool *primaryTool, KPrPresentationTool *secondaryTool )
: KoTool( viewMode.canvas() )
, m_viewMode( viewMode )
, m_primaryTool( primaryTool ) // a better name? these two names is quite confusing...
, m_secondaryTool( secondaryTool )
{
}

KPrPresenterViewTool::~KPrPresenterViewTool()
{
}

void KPrPresenterViewTool::paint( QPainter &painter, const KoViewConverter &converter )
{
}

void KPrPresenterViewTool::mousePressEvent( KoPointerEvent *event )
{
    m_primaryTool->mousePressEvent( event );
    m_secondaryTool->mousePressEvent( event );
}

void KPrPresenterViewTool::mouseDoubleClickEvent( KoPointerEvent *event )
{
}

void KPrPresenterViewTool::mouseMoveEvent( KoPointerEvent *event )
{
}

void KPrPresenterViewTool::mouseReleaseEvent( KoPointerEvent *event )
{
}

void KPrPresenterViewTool::keyPressEvent( QKeyEvent *event )
{
    m_primaryTool->keyPressEvent( event );
    if ( event->key() != Qt::Key_Escape) {
        m_secondaryTool->keyPressEvent( event );
    }
}

void KPrPresenterViewTool::keyReleaseEvent( QKeyEvent *event )
{
}

void KPrPresenterViewTool::wheelEvent( KoPointerEvent * event )
{
}

void KPrPresenterViewTool::activate( bool temporary )
{
}

void KPrPresenterViewTool::deactivate()
{
    m_viewMode.deactivate();
}

#include "KPrPresenterViewTool.moc"

