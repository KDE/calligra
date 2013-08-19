/* This file is part of the KDE libraries
    Copyright (C) 2004 Felix Berger <felixberger@beldesign.de>

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

#include "ktoolbarlabelaction.h"

#include <QtCore/QPointer>
#include <QApplication>
#include <QLabel>
#include <QToolBar>

class KToolBarLabelAction::Private
{
  public:
    QPointer<QAction> buddy;
    QPointer<QLabel> label;
};

KToolBarLabelAction::KToolBarLabelAction(const QString &text, QObject *parent)
  : QWidgetAction(parent),
    d( new Private )
{
  setText(text);
  d->label = 0;
}

KToolBarLabelAction::KToolBarLabelAction(QAction* buddy, const QString &text, QObject *parent)
  : QWidgetAction(parent),
    d( new Private )
{
  setBuddy( buddy );
  setText(text);

  d->label = 0;
}

KToolBarLabelAction::~KToolBarLabelAction()
{
  delete d;
}

void KToolBarLabelAction::setBuddy( QAction* buddy )
{
  d->buddy = buddy;

  QList<QLabel*> labels;
  Q_FOREACH ( QWidget* widget, associatedWidgets() )
    if ( QToolBar* toolBar = qobject_cast<QToolBar*>( widget ) )
      if ( QLabel* label = qobject_cast<QLabel*>( toolBar->widgetForAction( this ) ) )
        labels.append( label );

  Q_FOREACH ( QWidget* widget, buddy->associatedWidgets() )
    if ( QToolBar* toolBar = qobject_cast<QToolBar*>( widget ) ) {
      QWidget* newBuddy = toolBar->widgetForAction( buddy );
      Q_FOREACH ( QLabel* label, labels )
        label->setBuddy( newBuddy );
      return;
    }
}

QAction* KToolBarLabelAction::buddy() const
{
  return d->buddy;
}

bool KToolBarLabelAction::event( QEvent *event )
{
  if ( event->type() == QEvent::ActionChanged ) {
    if ( d->label && text() != d->label->text() ) {
      emit textChanged( text() );
      d->label->setText(text());
    }
  }

  return QWidgetAction::event( event );
}

bool KToolBarLabelAction::eventFilter( QObject *watched, QEvent *event )
{
  if ( d->label && d->buddy && event->type() == QEvent::PolishRequest && watched == d->label) {
    Q_FOREACH ( QWidget* widget, d->buddy->associatedWidgets() ) {
      if ( QToolBar* toolBar = qobject_cast<QToolBar*>( widget ) ) {
        QWidget* newBuddy = toolBar->widgetForAction( d->buddy );
        d->label->setBuddy( newBuddy );
      }
    }
  }
  
  return QWidgetAction::eventFilter( watched, event );
}

QWidget *KToolBarLabelAction::createWidget( QWidget* _parent )
{
  QToolBar *parent = qobject_cast<QToolBar *>(_parent);
  if (!parent)
    return QWidgetAction::createWidget(_parent);
  if (!d->label) {
    d->label = new QLabel( parent );

    /**
     * These lines were copied from Konqueror's KonqDraggableLabel class in
     * konq_misc.cc
     */
    d->label->setBackgroundRole( QPalette::Button );
    d->label->setAlignment( (QApplication::isRightToLeft() ? Qt::AlignRight : Qt::AlignLeft) |
                            Qt::AlignVCenter );
    d->label->adjustSize();
    d->label->setText(text());
    d->label->installEventFilter( this );
  }

  return d->label;
}

