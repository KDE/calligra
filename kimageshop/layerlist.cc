//   layer list widget to be incorporated into a layer control widget
//
//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

// only for debug
#include <iostream.h>

#include <qpainter.h>
#include <qpixmap.h>
#include <qmessagebox.h>

#include <kstddirs.h>
#include <kglobal.h>
#include <klocale.h>

#include "misc.h"
#include "layerlist.h"

#define WIDTH   150
#define HEIGHT  40
#define MAXROWS 8

QPixmap *LayerList::m_eyeIcon, *LayerList::m_linkIcon;
QRect LayerList::m_eyeRect, LayerList::m_linkRect;

LayerList::LayerList( QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( 0 );
}

LayerList::LayerList( Canvas* _canvas, QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( _canvas );
}

void LayerList::init( Canvas* _canvas )
{
  setTableFlags( Tbl_autoHScrollBar | Tbl_autoVScrollBar );

  m_canvas = _canvas;

  setBackgroundColor( white );
  updateTable();

  setCellWidth( WIDTH );
  setCellHeight( HEIGHT );
  m_selected = m_canvas->layerList().count() - 1;
  if( !m_eyeIcon )
  {
    QString _icon = locate( "data", "kimageshop/pics/eye.xpm" );
    m_eyeIcon = new QPixmap;
    if( !m_eyeIcon->load( _icon ) )
      QMessageBox::critical( this, "Canvas", "Can't find eye.xpm" );
    m_eyeRect = QRect( QPoint( 5,( cellHeight() - m_eyeIcon->height() ) / 2 ), m_eyeIcon->size() );
  }
  if( !m_linkIcon )
  {
    QString _icon = locate( "data", "kimageshop/pics/link.xpm" );
    m_linkIcon = new QPixmap;
    if( !m_linkIcon->load( _icon ) )
      QMessageBox::critical( this, "Canvas", "Can't find link.xpm" );
    m_linkRect = QRect( QPoint( 25,( cellHeight() - m_linkIcon->height() ) / 2 ), m_linkIcon->size() );
  }
}

void LayerList::paintCell( QPainter* p, int _row, int )
{
  if( _row == m_selected )
  {
    p->fillRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1, QColor( 15, 175, 50 ) );
  }
  if( m_canvas->layerList().at( _row )->isVisible() )
    p->drawPixmap( m_eyeRect.topLeft(), *m_eyeIcon );
  if( m_canvas->layerList().at( _row )->isLinked() )
    p->drawPixmap( m_linkRect.topLeft(), *m_linkIcon );

  p->drawRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1);
  p->drawText( 80, 20, m_canvas->layerList().at( _row )->name() );
}

void LayerList::updateList()
{
}

void LayerList::updateTable()
{
  if( m_canvas )
  {
    m_items = m_canvas->layerList().count();
    setNumRows( m_items );
    setNumCols( 1 );
  }
  else
  {
    m_items = 0;
    setNumRows( 0 );
    setNumCols( 0 );
  }
  resize( sizeHint() );
}

QSize LayerList::sizeHint() const
{
  return QSize( WIDTH, HEIGHT * MAXROWS );
}

void LayerList::mousePressEvent( QMouseEvent* _event )
{
  QPoint localPoint( _event->pos().x() % cellWidth(), _event->pos().y() % cellHeight() );

//SHOW_POINT( localPoint );

  int row = findRow( _event->pos().y() );

  if( m_eyeRect.contains( localPoint ) )
  {
    m_canvas->layerList().at( row )->setVisible( !m_canvas->layerList().at( row )->isVisible() );
    updateCell( row, 0 );
    m_canvas->compositeImage( m_canvas->layerList().at( row )->imageExtents() );
    m_canvas->repaint( m_canvas->layerList().at( row )->imageExtents(), false );
    return;
  }
  if( m_linkRect.contains( localPoint ) )
  {
    m_canvas->layerList().at( row )->setLinked( !m_canvas->layerList().at( row )->isLinked() );
    updateCell( row, 0 );
    return;
  }
  if( row != -1 )
  {
    int currentSel = m_selected;
    m_selected = -1;
    updateCell( currentSel, 0 );
    m_selected = row;
    m_canvas->setCurrentLayer( m_selected );
    updateCell( m_selected, 0 );
  }
}

#include "layerlist.moc"

