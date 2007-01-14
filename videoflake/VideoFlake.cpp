/* This file is part of the KDE project
   Copyright 2007 Montel Laurent <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QPainter>

#include <kdebug.h>
#include "VideoFlake.h"



VideoFlake::VideoFlake(const KUrl&url)
 : currentUrl(url)
{
  videowidget = new VideoWidget( 0);
  videopath = new VideoPath( 0 );
  audiooutput = new AudioOutput( Phonon::VideoCategory, 0 );
  audiopath = new AudioPath( 0 );
  mediaobject = new MediaObject( 0 );
  mediaobject->addVideoPath( videopath );
  videopath->addOutput( videowidget );
  mediaobject->addAudioPath( audiopath );
  audiopath->addOutput( audiooutput );

  mediaobject->setTickInterval( 350 );
}

VideoFlake::~VideoFlake()
{
}

void VideoFlake::paint( QPainter& painter, const KoViewConverter& converter )
{
}

void VideoFlake::play()
{
  kDebug()<<" VideoFlake::play\n";
  mediaobject->play();
}

void VideoFlake::start()
{ 
  kDebug()<<"VideoFlake::start\n";
  mediaobject->play();
}

void VideoFlake::stop()
{ 
  kDebug()<<"VideoFlake::stop \n";
  mediaobject->stop();
}

void VideoFlake::changeUrl()
{
  kDebug()<<"VideoFlake::changeUrl \n";
  //TODO

}


