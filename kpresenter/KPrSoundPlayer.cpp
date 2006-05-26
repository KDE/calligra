// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project

base code from kaudioplayer.h, kaudioplayer.cpp
Copyright (C) 2000 Stefan Westerfeld
stefan@space.twc.de

and konq_sound.h konq_sound.cc
Copyright (c) 2001 Malte Starostik <malte@kde.org>

This file's authors :
Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <config.h>

#warning "kde4: readd it"
#define WITHOUT_ARTS

#ifndef WITHOUT_ARTS
#include <kartsdispatcher.h>
#include <kplayobjectfactory.h>
#include <soundserver.h>
#endif

#include <kdebug.h>

#include "KPrSoundPlayer.h"

using namespace std;

class KPresenterSoundPlayerPrivate {
public:
    QString fileName;

    KPresenterSoundPlayerPrivate( QString fileName ) : fileName( fileName ) {};

#ifndef WITHOUT_ARTS
    KArtsDispatcher m_dispatche;
    Arts::SoundServerV2 m_soundServer;
    KPlayObjectFactory *m_factory;
    KPlayObject        *m_player;
#endif
};

KPrSoundPlayer::KPrSoundPlayer( const QString &fileName, QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new KPresenterSoundPlayerPrivate( fileName );

#ifndef WITHOUT_ARTS
    d->m_soundServer = Arts::Reference( "global:Arts_SoundServerV2" );
    d->m_factory = new KPlayObjectFactory( d->m_soundServer );
    d->m_player = 0;
#endif
}

KPrSoundPlayer::~KPrSoundPlayer()
{
#ifndef WITHOUT_ARTS
    delete d->m_player;
    delete d->m_factory;
#endif
    delete d;
}

void KPrSoundPlayer::play( const QString &fileName )
{
    KPrSoundPlayer sp( fileName );
    sp.play();
}

void KPrSoundPlayer::stop()
{
#ifndef WITHOUT_ARTS
    delete d->m_player;
    d->m_player = 0;
#endif
}

void KPrSoundPlayer::play()
{
#ifndef WITHOUT_ARTS
    if ( d->m_soundServer.isNull() )
        return;

    delete d->m_player;

    d->m_player = d->m_factory->createPlayObject( d->fileName, true );
    if ( d->m_player ) {
        if ( d->m_player->object().isNull() )
            stop();
        else
            d->m_player->play();
    }
#endif
}

#include "KPrSoundPlayer.moc"
