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

  // TODO: Do we need mouse tracking ?
  // setMouseTracking( TRUE );

  // TODO: sollte entfern werden, parent (widgetwrapper) gleich beim create festlegen
  if( m_widget )
    m_widget->reparent( this, f, QPoint( 0, 0 ), TRUE );

  // fuer das Widgte und alle seine Kimder Eventfilter installieren
  installChildEventFilter( m_widget );
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

void WidgetWrapper::slotResizing( const QRect& _rect )
{
  cerr << "WidgetWrapper::slotResizing() : " << _rect.width() << "," << _rect.height() << endl;

  resize( _rect.width(), _rect.height() );

   cerr << "real size : " << width() << "," << height() << endl;
}

WidgetWrapper::SelectState WidgetWrapper::selectState()
{
  return m_selectState;
}

void WidgetWrapper::addExtraChilds( const QColor& _color )
{
  ResizeWidget* rw1 = new ResizeWidget( this, ResizeWidget::TopLeft, _color );
  rw1->size().width();
  rw1->move( 0, 0 );
  rw1->raise();
  rw1->show();

  ResizeWidget* rw2 = new ResizeWidget( this, ResizeWidget::TopRight, _color );
  rw2->move( size().width() - RESIZEWIDGET_SIZE , 0 );
  rw2->raise();
  rw2->show();

  ResizeWidget* rw3 = new ResizeWidget( this, ResizeWidget::BottomLeft, _color );
  rw3->move( 0, size().height() - RESIZEWIDGET_SIZE );
  rw3->raise();
  rw3->show();

  ResizeWidget* rw4 = new ResizeWidget( this, ResizeWidget::BottomRight, _color );
  rw4->move( size().width() - RESIZEWIDGET_SIZE , size().height() - RESIZEWIDGET_SIZE );
  rw4->raise();
  rw4->show();


  QObject::connect( rw1, SIGNAL( resizing( const QRect& ) ), 
                    this, SLOT( slotResizing ( const QRect& ) ) );
  QObject::connect( rw2, SIGNAL( resizing( const QRect& ) ),
                    this, SLOT( slotResizing ( const QRect& ) ) );
  QObject::connect( rw3, SIGNAL( resizing( const QRect& ) ),
                    this, SLOT( slotResizing ( const QRect& ) ) );
  QObject::connect( rw4, SIGNAL( resizing( const QRect& ) ),
                    this, SLOT( slotResizing ( const QRect& ) ) );

  if( width() > RESIZEWIDGET_SPACE )
  {
    ResizeWidget* rw5 = new ResizeWidget( this, ResizeWidget::Top, _color );
    rw5->move( ( size().width() - RESIZEWIDGET_SIZE ) / 2 , 0 );
    rw5->raise();
    rw5->show();

    ResizeWidget* rw6 = new ResizeWidget( this, ResizeWidget::Bottom, _color );
    rw6->move( ( size().width() - RESIZEWIDGET_SIZE ) / 2 , size().height() - RESIZEWIDGET_SIZE );
    rw6->raise();
    rw6->show();
  }

  if( height() > RESIZEWIDGET_SPACE )
  {
    ResizeWidget* rw7 = new ResizeWidget( this, ResizeWidget::Left, _color );
    rw7->move( 0, ( size().height() - RESIZEWIDGET_SIZE ) / 2 );
    rw7->raise();
    rw7->show();
 
    ResizeWidget* rw8 = new ResizeWidget( this, ResizeWidget::Right, _color );
    rw8->move( size().width() - RESIZEWIDGET_SIZE, ( size().height() - RESIZEWIDGET_SIZE ) / 2 );
    rw8->raise();
    rw8->show();
  }

  raise();

  QWidget::update();
}

void WidgetWrapper::removeExtraChilds()
{
  QWidget* obj;
  QObjectList *list = QObject::queryList( "ResizeWidget" );
  QObjectListIt it( *list );             // iterate over all childs

  if( list )
  {
    while( ( obj = ( (QWidget*) it.current() ) ) != 0 )   // for each found object...
    {
      ++it;
      if( obj != m_widget )
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

      xpos = ( xpos + size().width() ) <= parentWidget()->size().width() ?
               xpos : parentWidget()->size().width() - size().width();
      ypos = ( ypos + size().height() ) <= parentWidget()->size().height() ?
               ypos : parentWidget()->size().height() - size().height();

      move( xpos, ypos );
    }
  }
  if( _event->type() == QEvent::MouseButtonRelease )
  {
    me = (QMouseEvent*) _event;
    if( me->button() & LeftButton )
    {
      cerr << "Dropping widget" << endl;

      QPoint pos = QWidget::pos();

      emit moveWidget( this, pos );
    }
  }
  return TRUE;
}

/*
void WidgetWrapper::resizeEvent( QResizeEvent* _event )
{
  cerr << "WidgetWrapper::resizeEvent()" << endl;

  if( m_selectState == PrimarySelect )
  {
    // resize widget
    m_widget->resize( _event->size() );

    // re-position resizers
    removeExtraChilds();
    addExtraChilds( black );

    // TODO: make this more general
  }
}
*/

void WidgetWrapper::installChildEventFilter( QWidget* _widget )
{
  cerr << "WidgetWrapper::installChildEventFilter() : " << _widget->name() << endl;

  _widget->installEventFilter( this );

  // find child widgets
  QObjectList *list = QObject::queryList( "QWidget" );

  // child widgets found ?
  if( ( list ) && ( !list->isEmpty() ) )
  {
    QWidget* child;
    QObjectListIt it( *list );

    // iterate over all childs
    while( ( child = ( (QWidget*) it.current() ) ) != 0 )
    {
      // for each found object...
      ++it;
      child->installEventFilter( this );
    }
    // delete the list, not the objects
    delete list;
  }
}

#include "widgetwrapper.moc"

