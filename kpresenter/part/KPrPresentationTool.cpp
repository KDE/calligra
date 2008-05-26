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
#include <KoPACanvas.h>
#include <KPrShapeApplicationData.h>
#include <KPrSoundData.h>
#include <phonon/phononnamespace.h>
#include<Phonon/MediaObject>
#include "KPrViewModePresentation.h"

KPrPresentationTool::KPrPresentationTool( KPrViewModePresentation & viewMode )
: KoTool( viewMode.canvas() )
, m_viewMode( viewMode )
,m_music(0)
{
}

KPrPresentationTool::~KPrPresentationTool()
{
    if(m_music)
        m_music->stop();
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
        KoShape * shapeClicked = m_canvas->shapeManager()->shapeAt( event->point );
        if(shapeClicked) {
            if(KPrShapeApplicationData *appData
                                = dynamic_cast<KPrShapeApplicationData *>( shapeClicked->applicationData())) {
                switch(appData->m_invokeResponse) {
                    case KPrShapeApplicationData::DoNavigate:
                        m_viewMode.navigate( KPrAnimationDirector::PreviousStep );
                        break;
                    case KPrShapeApplicationData::DoNone:
                    default:
                        break;
                }

                if(m_music)
                    m_music->stop();
                if(appData->m_soundData)
                {
                    m_music = Phonon::createPlayer(Phonon::MusicCategory,
                                                Phonon::MediaSource(appData->m_soundData->nameOfTempFile()));
                    connect(m_music, SIGNAL(finished()), m_music, SLOT(deleteLater()));
                    m_music->play();
                }
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
    if(m_music)
        m_music->stop();
}

#include "KPrPresentationTool.moc"
