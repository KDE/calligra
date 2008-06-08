/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrSoundEventAction.h"

#include <Phonon/MediaObject>

#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoShapeSavingContext.h>
#include <KPrSoundData.h>

KPrSoundEventAction::KPrSoundEventAction()
: QObject()
, m_media( 0 )
, m_soundData( 0 )
{
}

KPrSoundEventAction::~KPrSoundEventAction()
{
    delete m_media;
    delete m_soundData;
}

bool KPrSoundEventAction::loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context )
{
}

void KPrSoundEventAction::saveOdf( KoShapeSavingContext & context ) const
{
    context.xmlWriter().startElement( "presentation:event-listener" );
    context.xmlWriter().addAttribute( "script:event-name", "dom:click" );
    context.xmlWriter().addAttribute( "presentation:action", "sound" );
    // TODO save sound
    context.xmlWriter().endElement();
}

void KPrSoundEventAction::execute( KoTool * tool )
{
    Q_UNUSED( tool );
    if ( m_soundData ) {
        m_media = Phonon::createPlayer( Phonon::MusicCategory,
                                        Phonon::MediaSource( m_soundData->nameOfTempFile() ) );
        connect( m_media, SIGNAL( finished() ), this, SLOT( finished() ) );
        m_media->play();
    }
}

void KPrSoundEventAction::finish( KoTool * tool )
{
    Q_UNUSED( tool );
    if ( m_media ) {
        m_media->stop();
        finished();
    }
}

void KPrSoundEventAction::setSoundData( KPrSoundData * soundData )
{
    delete m_soundData;
    m_soundData = soundData;
}

void KPrSoundEventAction::finished()
{
    delete m_media;
    m_media = 0;
}

#include "KPrSoundEventAction.moc"
