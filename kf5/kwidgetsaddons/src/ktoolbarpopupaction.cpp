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

#include "ktoolbarpopupaction.h"

#include <QMenu>
#include <QToolBar>
#include <QToolButton>

class KToolBarPopupAction::Private
{
  public:
    Private()
      : delayed( true ), stickyMenu( true )
    {
    }

    bool delayed:1;
    bool stickyMenu:1;
};

KToolBarPopupAction::KToolBarPopupAction(const QIcon& icon, const QString& text, QObject *parent)
  : QWidgetAction(parent),
    d( new Private )
{
  setIcon(icon);
  setText(text);
  setMenu( new QMenu );
}

KToolBarPopupAction::~KToolBarPopupAction()
{
  delete d;
  delete menu();
}

#ifndef KDE_NO_DEPRECATED
QMenu* KToolBarPopupAction::popupMenu() const
{
  return menu();
}
#endif

QWidget * KToolBarPopupAction::createWidget( QWidget * _parent )
{
  QToolBar *parent = qobject_cast<QToolBar *>(_parent);
  if (!parent)
    return QWidgetAction::createWidget(_parent);
  QToolButton* button = new QToolButton( parent );
  button->setAutoRaise( true );
  button->setFocusPolicy( Qt::NoFocus );
  button->setIconSize( parent->iconSize() );
  button->setToolButtonStyle( parent->toolButtonStyle() );
  button->setDefaultAction( this );

  connect( parent, SIGNAL(iconSizeChanged(QSize)),
           button, SLOT(setIconSize(QSize)) );
  connect( parent, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)),
           button, SLOT(setToolButtonStyle(Qt::ToolButtonStyle)) );
  connect( button, SIGNAL(triggered(QAction*)),
           parent, SIGNAL(actionTriggered(QAction*)) );

  if ( d->delayed )
    if ( d->stickyMenu )
      button->setPopupMode( QToolButton::MenuButtonPopup );
    else
      button->setPopupMode( QToolButton::DelayedPopup );
  else
    button->setPopupMode( QToolButton::InstantPopup );

  return button;
}

bool KToolBarPopupAction::delayed() const
{
  return d->delayed;
}

void KToolBarPopupAction::setDelayed( bool delayed )
{
  d->delayed = delayed;
}

bool KToolBarPopupAction::stickyMenu() const
{
  return d->stickyMenu;
}

void KToolBarPopupAction::setStickyMenu( bool sticky )
{
  d->stickyMenu = sticky;
}

