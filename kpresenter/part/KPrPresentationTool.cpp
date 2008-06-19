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

#include "KPrPresentationTool.h"

#include <QKeyEvent>

#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>
#include <KoEventAction.h>
#include <KoPACanvas.h>
#include "KPrViewModePresentation.h"

KPrPresentationTool::KPrPresentationTool( KPrViewModePresentation & viewMode )
: KoTool( viewMode.canvas() )
, m_viewMode( viewMode )
{
}

KPrPresentationTool::~KPrPresentationTool()
{
}

bool KPrPresentationTool::wantsAutoScroll()
{
    return false;
}
void KPrPresentationTool::paint( QPainter &painter, const KoViewConverter &converter )
{
}

void KPrPresentationTool::mousePressEvent( KoPointerEvent *event )
{
    if ( event->button() & Qt::LeftButton ) {
        finishEventActions();
        KoShape * shapeClicked = m_canvas->shapeManager()->shapeAt( event->point );
        if (shapeClicked) {
            m_eventActions = shapeClicked->eventActions();
            if ( m_eventActions.size() ) {
                foreach ( KoEventAction * eventAction, m_eventActions ) {
                    eventAction->execute( this );
                }
                // don't do next step if a action was executed
                return;
            }
        }
        m_viewMode.navigate( KPrAnimationDirector::NextStep );
    }
}

void KPrPresentationTool::mouseDoubleClickEvent( KoPointerEvent *event )
{
}

void KPrPresentationTool::mouseMoveEvent( KoPointerEvent *event )
{
}

void KPrPresentationTool::mouseReleaseEvent( KoPointerEvent *event )
{
}

void KPrPresentationTool::keyPressEvent( QKeyEvent *event )
{
    finishEventActions();
    event->accept();

    switch ( event->key() )
    {
        case Qt::Key_Escape:
            m_viewMode.activateSavedViewMode();
            break;
        case Qt::Key_Home:
            m_viewMode.navigate( KPrAnimationDirector::FirstPage );
            break;
        case Qt::Key_Up:
        case Qt::Key_PageUp:
            m_viewMode.navigate( KPrAnimationDirector::PreviousPage );
            break;
        case Qt::Key_Backspace:
        case Qt::Key_Left:
            m_viewMode.navigate( KPrAnimationDirector::PreviousStep );
            break;
        case Qt::Key_Right:
        case Qt::Key_Space:
            m_viewMode.navigate( KPrAnimationDirector::NextStep );
            break;
        case Qt::Key_Down:
        case Qt::Key_PageDown:
            m_viewMode.navigate( KPrAnimationDirector::NextPage );
            break;
        case Qt::Key_End:
            m_viewMode.navigate( KPrAnimationDirector::LastPage );
            break;
        default:
            event->ignore();
            break;
    }
}

void KPrPresentationTool::keyReleaseEvent( QKeyEvent *event )
{
}

void KPrPresentationTool::wheelEvent( KoPointerEvent * event )
{
}

void KPrPresentationTool::activate( bool temporary )
{
}

void KPrPresentationTool::deactivate()
{
    finishEventActions();
}

void KPrPresentationTool::finishEventActions()
{
    foreach ( KoEventAction * eventAction, m_eventActions ) {
        eventAction->finish( this );
    }
}

#include "KPrPresentationTool.moc"
