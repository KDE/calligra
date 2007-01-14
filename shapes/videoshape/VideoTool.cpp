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
#include <QGridLayout>
#include <QToolButton>

#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

#include "VideoShape.h"

#include "VideoTool.h"
#include "VideoTool.moc"

VideoTool::VideoTool( KoCanvasBase* canvas )
    : KoTool( canvas ),
      m_videoshape(0)
{
}

VideoTool::~VideoTool()
{
}

void VideoTool::activate (bool temporary)
{
    Q_UNUSED( temporary );
    kDebug() << k_funcinfo << endl;

    KoSelection* selection = m_canvas->shapeManager()->selection();
    foreach ( KoShape* shape, selection->selectedShapes() )
    {
        m_videoshape = dynamic_cast<VideoShape*>( shape );
        if ( m_videoshape )
            break;
    }
    if ( !m_videoshape )
    {
        emit sigDone();
        return;
    }
    useCursor( Qt::ArrowCursor, true );
}

void VideoTool::deactivate()
{
  kDebug()<<"VideoTool::deactivate\n";
  m_videoshape = 0;
}

void VideoTool::paint( QPainter& painter, KoViewConverter& viewConverter )
{
    Q_UNUSED( viewConverter );
    QBrush brush( Qt::red );
    painter.fillRect(10,10,10,10, brush);
}

void VideoTool::mousePressEvent( KoPointerEvent* )
{
}

void VideoTool::mouseMoveEvent( KoPointerEvent* )
{
}

void VideoTool::mouseReleaseEvent( KoPointerEvent* )
{
}


QWidget * VideoTool::createOptionWidget()
{
    QWidget *optionWidget = new QWidget();
    QGridLayout *layout = new QGridLayout( optionWidget );

    QToolButton *button = 0;

    button = new QToolButton( optionWidget );
    button->setIcon( SmallIcon("previous") );
    button->setToolTip( i18n( "Previous" ) );
    layout->addWidget( button, 0, 0 );
    connect( button, SIGNAL( clicked( bool ) ), this, SLOT( slotPrevious() ) );

    button = new QToolButton( optionWidget );
    button->setIcon( SmallIcon("start") );
    button->setToolTip( i18n( "Play" ) );
    layout->addWidget( button, 0, 1 );
    connect( button, SIGNAL( clicked( bool ) ), this, SLOT( slotPlay() ) );

    button = new QToolButton( optionWidget );
    button->setIcon( SmallIcon("stop") );
    button->setToolTip( i18n( "Stop" ) );
    layout->addWidget( button, 0, 2 );
    connect( button, SIGNAL( clicked( bool ) ), this, SLOT( slotStop() ) );

    button = new QToolButton( optionWidget );
    button->setIcon( SmallIcon("pause") );
    button->setToolTip( i18n( "Pause" ) );
    layout->addWidget( button, 0, 3 );
    connect( button, SIGNAL( clicked( bool ) ), this, SLOT( slotPause() ) );
 

    button = new QToolButton( optionWidget );
    button->setIcon( SmallIcon("next") );
    button->setToolTip( i18n( "Next" ) );
    layout->addWidget( button, 0, 4 );
    connect( button, SIGNAL( clicked( bool ) ), this, SLOT( slotNext() ) );

    button = new QToolButton( optionWidget );
    button->setIcon( SmallIcon("open") );
    button->setToolTip( i18n( "Open" ) );
    layout->addWidget( button, 0, 5 );
    connect( button, SIGNAL( clicked( bool ) ), this, SLOT( slotOpen() ) );

    return optionWidget;

}

void VideoTool::slotPrevious()
{
  kDebug()<<"VideoTool::slotPrevious\n";
  if(m_videoshape)
    m_videoshape->previous();
}

void VideoTool::slotPlay()
{
  kDebug()<<"VideoTool::slotPlay\n";
  if(m_videoshape) 
    m_videoshape->play();
}

void VideoTool::slotStart()
{
  kDebug()<<" VideoTool::slotStart \n";
  if(m_videoshape)
    m_videoshape->start();
}

void VideoTool::slotStop()
{
  kDebug()<<" VideoTool::slotStop \n";
  if(m_videoshape)
    m_videoshape->stop();
}

void VideoTool::slotChangeUrl()
{
  kDebug()<<" VideoTool::slotChangeUrl \n";
  if(m_videoshape)
    m_videoshape->changeUrl();
}

void VideoTool::slotNext()
{
  kDebug()<<" VideoTool::slotNext \n";
  if(m_videoshape)
    m_videoshape->next();
}

void VideoTool::slotPause()
{
  kDebug()<<" VideoTool::slotPause \n";
  if(m_videoshape)
    m_videoshape->pause();
 
}
