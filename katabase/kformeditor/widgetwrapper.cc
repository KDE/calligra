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
#include "resizewidget.h"

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

    addExtraChilds( black );
    m_selectState = PrimarySelect;
  }

  QWidget::update();
}

void WidgetWrapper::slotSelectSecondary()
{
  if( m_selectState == NoSelect )
  {
    cerr << "selecting secondary : " << m_widget->name() << endl;

    addExtraChilds( darkGray );
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
  // TODO: Remove this ugly SIZE construct here and add a constant to ResizeWidget

  int SIZE;

  ResizeWidget* rw1 = new ResizeWidget( ResizeWidget::TopLeft, _color, this );
  SIZE = rw1->size().width();
  rw1->move( 0, 0 );
  rw1->raise();
  rw1->show();
  //rw1->installEventFilter( this );

  ResizeWidget* rw2 = new ResizeWidget( ResizeWidget::TopRight, _color, this );
  rw2->move( size().width() - SIZE , 0 );
  rw2->raise();
  rw2->show();
  //rw2->installEventFilter( this );

  ResizeWidget* rw3 = new ResizeWidget( ResizeWidget::BottomLeft, _color, this );
  rw3->move( 0, size().height() - SIZE );
  rw3->raise();
  rw3->show();
  //rw3->installEventFilter( this );

  ResizeWidget* rw4 = new ResizeWidget( ResizeWidget::BottomRight, _color, this );
  rw4->move( size().width() - SIZE , size().height() - SIZE );
  rw4->raise();
  rw4->show();
  //rw4->installEventFilter( this );

  raise();

  QWidget::update();
}

void WidgetWrapper::removeExtraChilds()
{
  QWidget* obj;
  QObjectList *list = QObject::queryList( "ResizeWidget" );
  QObjectListIt it( *list );             // iterate over all childs
 
  while( ( obj = ( (QWidget*) it.current() ) ) != 0 )   // for each found object...
  {
    ++it;
    if( obj != m_widget )
    {
      obj->hide();
      removeChild( obj );
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
        emit clickedShift( this );
      else
        emit clicked( this );
    }
  }
  if( ( _event->type() == QEvent::MouseMove ) &&
      ( m_selectState == PrimarySelect ) )
  {
    me = (QMouseEvent*) _event;
    if( me->state() & LeftButton )
    {
      int xpos = x() - mpos.x() + me->x();
      int ypos = y() - mpos.y() + me->y();

      xpos = xpos > 0 ? xpos : 0;
      ypos = ypos > 0 ? ypos : 0;
 
      move( xpos, ypos );
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

