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
#include <qpopupmenu.h>

#include <kstddirs.h>
#include <kglobal.h>
#include <klocale.h>

#include "misc.h"
#include "kimageshop_doc.h"
#include "layerlist.h"

#define WIDTH   200
#define HEIGHT  40
#define MAXROWS 8

QPixmap *LayerList::m_eyeIcon, *LayerList::m_linkIcon;
QRect LayerList::m_eyeRect, LayerList::m_linkRect;

LayerList::LayerList( QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( 0 );
}

LayerList::LayerList( KImageShopDoc* doc, QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( doc );
}

void LayerList::init( KImageShopDoc* doc )
{
  setTableFlags( Tbl_autoHScrollBar | Tbl_autoVScrollBar );

  m_doc = doc;

  setBackgroundColor( white );
  updateTable();

  setCellWidth( WIDTH );
  setCellHeight( HEIGHT );
  m_selected = m_doc->layerList().count() - 1;
  if( !m_eyeIcon )
  {
    QString _icon = locate( "appdata", "pics/eye.xpm" );
    m_eyeIcon = new QPixmap;
    if( !m_eyeIcon->load( _icon ) )
      QMessageBox::critical( this, "Canvas", "Can't find eye.xpm" );
    m_eyeRect = QRect( QPoint( 5,( cellHeight() - m_eyeIcon->height() ) / 2 ), m_eyeIcon->size() );
  }
  if( !m_linkIcon )
  {
    QString _icon = locate( "appdata", "pics/link.xpm" );
    m_linkIcon = new QPixmap;
    if( !m_linkIcon->load( _icon ) )
      QMessageBox::critical( this, "Canvas", "Can't find link.xpm" );
    m_linkRect = QRect( QPoint( 25,( cellHeight() - m_linkIcon->height() ) / 2 ), m_linkIcon->size() );
  }

  m_contextmenu = new QPopupMenu();

  m_contextmenu->setCheckable(TRUE);

  m_contextmenu->insertItem( i18n( "Visible" ), 1 );
  m_contextmenu->insertItem( i18n( "Linked"), 2 );
  m_contextmenu->insertItem( i18n( "Opacity"), 3 );
  m_contextmenu->insertItem( i18n( "Rename"), 4 );

  m_contextmenu->insertSeparator();

  m_contextmenu->insertItem( i18n( "Add Layer" ), 11 );
  m_contextmenu->insertItem( i18n( "Remove Layer"), 12 );
  m_contextmenu->insertItem( i18n( "Add Mask" ), 13 );
  m_contextmenu->insertItem( i18n( "Remove Mask"), 14 );

  connect( m_contextmenu, SIGNAL( activated( int ) ), SLOT( slotMenuAction( int ) ) );
}

void LayerList::paintCell( QPainter* p, int _row, int )
{
  if( _row == m_selected )
  {
    p->fillRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1, QColor( 15, 175, 50 ) );
  }
  if( m_doc->layerList().at( _row )->isVisible() )
    p->drawPixmap( m_eyeRect.topLeft(), *m_eyeIcon );
  if( m_doc->layerList().at( _row )->isLinked() )
    p->drawPixmap( m_linkRect.topLeft(), *m_linkIcon );

  p->drawRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1);
  p->drawText( 80, 20, m_doc->layerList().at( _row )->name() );
}

void LayerList::updateList()
{
}

void LayerList::updateTable()
{
  if( m_doc )
  {
    m_items = m_doc->layerList().count();
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

void LayerList::update_contextmenu( int _index )
{
  m_contextmenu->setItemChecked( 1, m_doc->layerList().at( _index )->isVisible() );
  m_contextmenu->setItemChecked( 2, m_doc->layerList().at( _index )->isLinked() );
}

void LayerList::selectLayer( int _index )
{
  int currentSel = m_selected;
  m_selected = -1;
  updateCell( currentSel, 0 );
  m_selected = _index;
  m_doc->setCurrentLayer( m_selected );
  updateCell( m_selected, 0 );
}

void LayerList::inverseVisibility( int _index )
{
  m_doc->layerList().at( _index )->setVisible( !m_doc->layerList().at( _index )->isVisible() );
  updateCell( _index, 0 );
  m_doc->compositeImage( m_doc->layerList().at( _index )->imageExtents() );
  m_doc->slotUpdateViews(m_doc->layerList().at( _index )->imageExtents());
}

void LayerList::inverseLinking( int _index )
{
  m_doc->layerList().at( _index )->setLinked( !m_doc->layerList().at( _index )->isLinked() );
  updateCell( _index, 0 );
}

void LayerList::renameLayer( int _index )
{
  QString layername = m_doc->layerList().at( _index )->name();

  if( layername.left( 3 ) == "___" )
    layername = layername.right( layername.length() - 3 );
  else
    layername = "___" + layername;

  m_doc->layerList().at( _index )->setName( layername );
  updateCell( _index, 0 );
}

void LayerList::addLayer( int _index )
{
  cerr << "Michael : Add Layer" << endl;
}

void LayerList::removeLayer( int _index )
{
  cerr << "Michael : Remove Layer" << endl;
}

void LayerList::slotMenuAction( int _id )
{
  switch( _id )
  {
    case 1:
      inverseVisibility( m_selected );
      break;
    case 2:
      inverseLinking( m_selected );
      break;
    case 3:
      cerr << "Michael : Opacity" << endl;
      break;
    case 4:
      renameLayer( m_selected );
      break;
    case 11:
      addLayer( m_selected );
      break;
    case 12:
      removeLayer( m_selected );
      break;
    default:
      cerr << "Michael : unknown context menu action" << endl;
      break;
  }
}

QSize LayerList::sizeHint() const
{
  return QSize( WIDTH, HEIGHT * MAXROWS );
}

void LayerList::mousePressEvent( QMouseEvent* _event )
{
  int row = findRow( _event->pos().y() );
  QPoint localPoint( _event->pos().x() % cellWidth(), _event->pos().y() % cellHeight() );

  if( _event->button() & LeftButton )
  {
    if( m_eyeRect.contains( localPoint ) )
    {
      inverseVisibility( row );
    }
    else if( m_linkRect.contains( localPoint ) )
    {
      inverseLinking( row );
    }
    else if( row != -1 )
    {
      selectLayer( row );
    }
  }
  else if( _event->button() & RightButton )
  {
    // TODO: Should the Layer under the cursor selected when clicking RMB ?

    selectLayer( row );
    update_contextmenu( row );
    m_contextmenu->popup( mapToGlobal( _event->pos() ) );
  }
}

#include "layerlist.moc"

