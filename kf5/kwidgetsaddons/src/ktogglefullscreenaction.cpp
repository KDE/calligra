/* This file is part of the KDE libraries
    Copyright (C) 1999 Reginald Stadlbauer <reggie@kde.org>
              (C) 1999 Simon Hausmann <hausmann@kde.org>
              (C) 2000 Nicolas Hadacek <haadcek@kde.org>
              (C) 2000 Kurt Granroth <granroth@kde.org>
              (C) 2000 Michael Koch <koch@kde.org>
              (C) 2001 Holger Freyther <freyther@kde.org>
              (C) 2002 Ellis Whitehead <ellis@kde.org>
              (C) 2002 Joseph Wenninger <jowenn@kde.org>
              (C) 2003 Andras Mantia <amantia@kde.org>
              (C) 2005-2006 Hamish Rodda <rodda@kde.org>

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

#include "ktogglefullscreenaction.h"

#include <QtCore/QEvent>

class KToggleFullScreenAction::Private
{
public:
    Private( KToggleFullScreenAction* action )
        : q( action )
        , window( 0 )
    {
    }

    void updateTextsAndIcon()
    {
        if ( q->isChecked() ) {
            q->setText( KToggleFullScreenAction::tr( "Exit F&ull Screen Mode", "@action:inmenu" ) );
            q->setIconText( KToggleFullScreenAction::tr( "Exit Full Screen", "@action:intoolbar" ) );
            q->setToolTip( KToggleFullScreenAction::tr( "Exit full screen mode", "@info:tooltip" ) );
            q->setIcon( QIcon::fromTheme( QStringLiteral("view-restore") ) );
        } else {
            q->setText( KToggleFullScreenAction::tr( "F&ull Screen Mode", "@action:inmenu" ) );
            q->setIconText( KToggleFullScreenAction::tr( "Full Screen", "@action:intoolbar" ) );
            q->setToolTip( KToggleFullScreenAction::tr( "Display the window in full screen", "@info:tooltip" ) );
            q->setIcon( QIcon::fromTheme( QStringLiteral("view-fullscreen") ) );
        }
    }

    KToggleFullScreenAction* q;
    QWidget* window;
};

KToggleFullScreenAction::KToggleFullScreenAction( QObject *parent )
  : KToggleAction( parent ),
    d( new Private( this ) )
{
    d->updateTextsAndIcon();
}

KToggleFullScreenAction::KToggleFullScreenAction( QWidget *window, QObject *parent )
  : KToggleAction( parent ),
    d( new Private( this ) )
{
    d->updateTextsAndIcon();
    setWindow( window );
}

KToggleFullScreenAction::~KToggleFullScreenAction()
{
  delete d;
}

void KToggleFullScreenAction::setWindow( QWidget* window )
{
  if ( d->window )
    d->window->removeEventFilter( this );

  d->window = window;

  if ( d->window )
    d->window->installEventFilter( this );
}

void KToggleFullScreenAction::slotToggled( bool checked )
{
  KToggleAction::slotToggled( checked );
  d->updateTextsAndIcon();
}

bool KToggleFullScreenAction::eventFilter( QObject* object, QEvent* event )
{
  if ( object == d->window )
    if ( event->type() == QEvent::WindowStateChange ) {
      if ( d->window->isFullScreen() != isChecked() )
        activate( QAction::Trigger );
    }

  return false;
}

void KToggleFullScreenAction::setFullScreen( QWidget* window, bool set )
{
  if( set )
    window->setWindowState( window->windowState() | Qt::WindowFullScreen );
  else
    window->setWindowState( window->windowState() & ~Qt::WindowFullScreen );
}

