/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Michael Koch <koch@kde.org>
 
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpainter.h>
#include <qobjectlist.h>
#include <qscrollview.h>

#include "widgetwrapper.h"

// only for debug
#include <iostream.h>

WidgetWrapper::WidgetWrapper( QWidget* _widget, WFlags f )
  : QWidget( 0, 0 ), m_selectState ( NoSelect ), m_widget( _widget )
{
  setName( m_widget->name() );
  setFixedSize( m_widget->size() );
  setMouseTracking( TRUE );

  m_widget->reparent( this, f, QPoint( 0, 0 ), TRUE );
  m_widget->installEventFilter( this );
}
 
WidgetWrapper::~WidgetWrapper()
{
}

void WidgetWrapper::slotSelectPrimary()
{
  if( m_selectState == NoSelect )
  {
    cerr << "selecting primary : " << m_widget->name() << endl;

    addExtraChilds( red );
    m_selectState = PrimarySelect;
  }

  QWidget::update();
}

void WidgetWrapper::slotSelectSecondary()
{
  if( m_selectState == NoSelect )
  {
    cerr << "selecting secondary : " << m_widget->name() << endl;

    addExtraChilds( green );
    m_selectState = SecondarySelect;
  }

  QWidget::update();
}

void WidgetWrapper::slotUnselect()
{
  if( m_selectState != NoSelect )
  {
    cerr << "unselecting : " << m_widget->name() << endl;

    removeExtraChilds();
    m_selectState = NoSelect;
  }
 
  QWidget::update();
}

WidgetWrapper::SelectState WidgetWrapper::selectState()
{
  return m_selectState;
}

void WidgetWrapper::addExtraChilds( const QColor& _color )
{
  cerr << "adding extra childs" << endl;

  QWidget* w = new QWidget( this );
  w->resize( size() );
  w->setBackgroundColor( _color );
  w->raise();
  w->show();
  w->installEventFilter( this );
  raise();
}

void WidgetWrapper::removeExtraChilds()
{
  cerr << "removing extra childs" << endl;

  QWidget* obj;
  QObjectList *list = QObject::queryList( "QWidget" );
  QObjectListIt it( *list );             // iterate over all childs
 
  while( ( obj = ( (QWidget*) it.current() ) ) != 0 )   // for each found object...
  {
    ++it;
    if( obj != m_widget )
    {
      if( ( red == obj->backgroundColor() ) ||
          ( green == obj->backgroundColor() ) )
      {
        obj->hide();
        removeChild( obj );
      }
    }
  }
  delete list;                           // delete the list, not the objects
}

bool WidgetWrapper::eventFilter( QObject* _obj, QEvent* _event )
{
  if( _event->type() == QEvent::Paint )
  {
    return FALSE;
  }

  QMouseEvent* me;
  static QPoint mpos;

  if( _event->type() == QEvent::MouseButtonPress )
  {
    me = (QMouseEvent*) _event;
    if( me->button() & LeftButton )
    {
      mpos = me->pos();

      if( me->state() & ShiftButton )
      {
        cerr << "LMB clicking + Shift" << endl;

        emit clickedShift( this );
      }
      else
      {
        cerr << "LMB clicking" << endl;

        emit clicked( this );
      }
    }
  }
  if( _event->type() == QEvent::MouseMove )
  {
    me = (QMouseEvent*) _event;
    if( me->state() & LeftButton )
    {
      move( x() - mpos.x() + me->x(), y() - mpos.y() + me->y() );
    }
  }
  if( _event->type() == QEvent::MouseButtonRelease )
  {
    me = (QMouseEvent*) _event;
    if( me->button() & LeftButton )
    {
//    cerr << "Dropping widget" << endl;
    }
  }
  return TRUE;
}

#include "widgetwrapper.moc"

