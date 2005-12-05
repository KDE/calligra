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

#ifndef kpresenter_sound_player_h
#define kpresenter_sound_player_h

#include <qobject.h>


/*
  namespace Arts
  {
  class SoundServerV2;
  class KArtsDispatcher;
  calss KPlayObjectFactory;
  calss KPlayObject;
  }
*/
class KPrSoundPlayer : public QObject
{
    Q_OBJECT
public:
    KPrSoundPlayer( const QString &fileName, QObject *parent = 0, const char *name = 0 );

    ~KPrSoundPlayer();

    void play( const QString &fileName );
    void stop();

public slots:
    void play();

private:
    class KPresenterSoundPlayerPrivate *d;
};

#endif // kpresenter_sound_player_h
