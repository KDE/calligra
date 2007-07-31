/* This file is part of the KDE project
   Copyright 2007 Montel Laurent <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

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
#include <QFrame>
#include <QVBoxLayout>
#include <kdebug.h>
#include "VideoShape.h"



VideoShape::VideoShape(const KUrl&url)
{
  m_widget = new QFrame(0);
  QVBoxLayout *box = new QVBoxLayout( m_widget );
  m_videowidget = new VideoWidget(m_widget );
  box->addWidget(  m_videowidget );
  m_videopath = new VideoPath( m_widget );
  m_audiooutput = new AudioOutput( Phonon::VideoCategory, m_widget );
  m_audiopath = new AudioPath( m_widget );
  m_mediaobject = new MediaObject( m_widget );
  m_mediaobject->addVideoPath( m_videopath );
  m_videopath->addOutput( m_videowidget );
  m_mediaobject->addAudioPath( m_audiopath );
  m_audiopath->addOutput( m_audiooutput );

  m_mediaobject->setTickInterval( 350 );
  m_mediaobject->setCurrentSource(url);
}

VideoShape::~VideoShape()
{
}

void VideoShape::changePosition(int pos)
{
  //TODO
  //m_mediaobject->setPosition(position);
}

void VideoShape::setCurrentUrl(const KUrl&url)
{
  m_mediaobject->setCurrentSource(url);
}

KUrl VideoShape::currentUrl() const
{
  return m_mediaobject->currentSource().url();
}

void VideoShape::resize( const QSizeF &newSize )
{
  kDebug()<<" VideoShape::resize( const QSizeF &newSize )";
  QSize tmpSize(newSize.toSize());
  m_widget->resize(tmpSize);
  KoShape::resize(newSize);
}

void VideoShape::paint( QPainter& painter, const KoViewConverter& converter )
{
  kDebug()<<" VideoShape::paint( QPainter& painter, const KoViewConverter& converter )";
  m_widget->repaint();
  m_videowidget->repaint();
}

void VideoShape::play()
{
  kDebug()<<" VideoShape::play";
  m_mediaobject->play();
}

void VideoShape::start()
{ 
  kDebug()<<"VideoShape::start";
  m_mediaobject->play();
}

void VideoShape::stop()
{ 
  kDebug()<<"VideoShape::stop";
  m_mediaobject->stop();
}

void VideoShape::changeUrl()
{
  kDebug()<<"VideoShape::changeUrl";
  //TODO

}

void VideoShape::next()
{
  kDebug()<<"VideoShape::next";
  //TODO
}

void VideoShape::previous()
{
  kDebug()<<"VideoShape::previous";
  //TODO
}

void VideoShape::pause()
{
 kDebug()<<" VideoShape::pause";
 m_mediaobject->pause();
}

void VideoShape::saveOdf( KoShapeSavingContext & context ) const
{
    // TODO
}

bool VideoShape::loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context ) {
    return false; // TODO
}
