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
#include "layerview.h"

#define WIDTH   200
#define HEIGHT  40
#define MAXROWS 8

QPixmap *LayerView::m_eyeIcon, *LayerView::m_linkIcon;
QRect LayerView::m_eyeRect, LayerView::m_linkRect;

LayerView::LayerView( QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( 0 );
}

LayerView::LayerView( KImageShopDoc* doc, QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( doc );
}

void LayerView::init( KImageShopDoc* doc )
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

  QPopupMenu *submenu = new QPopupMenu();
 
  submenu->insertItem( i18n( "Upper" ), UPPERLAYER );
  submenu->insertItem( i18n( "Lower" ), LOWERLAYER );
  submenu->insertItem( i18n( "Most front" ), FRONTLAYER );
  submenu->insertItem( i18n( "Most back" ), BACKLAYER );

  m_contextmenu = new QPopupMenu();

  m_contextmenu->setCheckable(TRUE);

  m_contextmenu->insertItem( i18n( "Visible" ), VISIBLE );
  m_contextmenu->insertItem( i18n( "Selection"), SELECTION );
  m_contextmenu->insertItem( i18n( "Level" ), submenu );
  m_contextmenu->insertItem( i18n( "Linked"), LINKING );
  m_contextmenu->insertItem( i18n( "Opacity"), OPACITY );
  m_contextmenu->insertItem( i18n( "Rename"), RENAME );

  m_contextmenu->insertSeparator();

  m_contextmenu->insertItem( i18n( "Add Layer" ), ADDLAYER );
  m_contextmenu->insertItem( i18n( "Remove Layer"), REMOVELAYER );
  m_contextmenu->insertItem( i18n( "Add Mask" ), ADDMASK );
  m_contextmenu->insertItem( i18n( "Remove Mask"), REMOVEMASK );

  connect( m_contextmenu, SIGNAL( activated( int ) ), SLOT( slotMenuAction( int ) ) );
  connect( submenu, SIGNAL( activated( int ) ), SLOT( slotMenuAction( int ) ) );
}

void LayerView::paintCell( QPainter* _painter, int _row, int )
{
  if( _row == m_selected )
  {
    _painter->fillRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1, green );
  }

  if( m_doc->layerList().at( _row )->isVisible() )
  {
    _painter->drawPixmap( m_eyeRect.topLeft(), *m_eyeIcon );
  }

  if( m_doc->layerList().at( _row )->isLinked() )
  {
    _painter->drawPixmap( m_linkRect.topLeft(), *m_linkIcon );
  }

  _painter->drawRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1);
  _painter->drawText( 80, 20, m_doc->layerList().at( _row )->name() );
}

void LayerView::updateTable()
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

void LayerView::update_contextmenu( int _index )
{
  m_contextmenu->setItemChecked( VISIBLE, m_doc->layerList().at( _index )->isVisible() );
  m_contextmenu->setItemChecked( LINKING, m_doc->layerList().at( _index )->isLinked() );
}

void LayerView::selectLayer( int _index )
{
  int currentSel = m_selected;
  m_selected = -1;
  updateCell( currentSel, 0 );
  m_selected = _index;
  m_doc->setCurrentLayer( m_selected );
  updateCell( m_selected, 0 );
}

void LayerView::slotInverseVisibility( int _index )
{
  m_doc->layerList().at( _index )->setVisible( !m_doc->layerList().at( _index )->isVisible() );
  updateCell( _index, 0 );
  m_doc->compositeImage( m_doc->layerList().at( _index )->imageExtents() );
  m_doc->slotUpdateViews( m_doc->layerList().at( _index )->imageExtents() );
}

void LayerView::slotInverseLinking( int _index )
{
  m_doc->layerList().at( _index )->setLinked( !m_doc->layerList().at( _index )->isLinked() );
  updateCell( _index, 0 );
}

void LayerView::slotRenameLayer( int _index )
{
  QString layername = m_doc->layerList().at( _index )->name();

  if( layername.left( 3 ) == "___" )
    layername = layername.right( layername.length() - 3 );
  else
    layername = "___" + layername;

  m_doc->layerList().at( _index )->setName( layername );
  updateCell( _index, 0 );
}

void LayerView::slotMenuAction( int _id )
{
  switch( _id )
  {
    case VISIBLE:
      slotInverseVisibility( m_selected );
      break;
    case LINKING:
      slotInverseLinking( m_selected );
      break;
    case OPACITY:
      cerr << "Michael : Opacity" << endl;
      break;
    case RENAME:
      slotRenameLayer( m_selected );
      break;
    case ADDLAYER:
      slotAddLayer();
      break;
    case REMOVELAYER:
      slotRemoveLayer();
      break;
    case UPPERLAYER:
      slotUpperLayer();
      break;
    case LOWERLAYER:
      slotLowerLayer();
      break;
    case FRONTLAYER:
      slotFrontLayer();
      break;
    case BACKLAYER:
      slotBackgroundLayer();
      break;
    default:
      cerr << "Michael : unknown context menu action" << endl;
      break;
  }
}

QSize LayerView::sizeHint() const
{
  return QSize( WIDTH, HEIGHT * 5 );
}

void LayerView::mousePressEvent( QMouseEvent* _event )
{
  int row = findRow( _event->pos().y() );
  QPoint localPoint( _event->pos().x() % cellWidth(), _event->pos().y() % cellHeight() );

  if( _event->button() & LeftButton )
  {
    if( m_eyeRect.contains( localPoint ) )
    {
      slotInverseVisibility( row );
    }
    else if( m_linkRect.contains( localPoint ) )
    {
      slotInverseLinking( row );
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

void LayerView::slotAddLayer()
{
}

void LayerView::slotRemoveLayer()
{
}

void LayerView::swapLayers( int a, int b )
{
  if( ( m_doc->layerList().at( a )->isVisible() ) &&
      ( m_doc->layerList().at( b )->isVisible() ) )
  {
    QRect l1 = m_doc->layerList().at( a )->imageExtents();
    QRect l2 = m_doc->layerList().at( b )->imageExtents();

    if( l1.intersects( l2 ) )
    {
      QRect rect = l1.intersect( l2 );
 
      m_doc->compositeImage( rect );
      m_doc->slotUpdateViews( rect );
    }
  }
}

void LayerView::slotUpperLayer()
{
  int newpos = m_selected > 0 ? m_selected - 1 : 0;

  if( m_selected != newpos )
  {
    m_doc->upperLayer( m_selected );
    repaint();
    swapLayers( m_selected, newpos );
    m_selected = newpos;
    updateCell( m_selected + 1, 0 );
    updateCell( m_selected, 0 );
  }
}

void LayerView::slotLowerLayer()
{
  int newpos = ( m_selected + 1 ) < m_doc->layerList().count() ? m_selected + 1 : m_selected;

  if( m_selected != newpos )
  {
    m_doc->lowerLayer( m_selected );
    repaint();
    swapLayers( m_selected, newpos );
    m_selected = newpos;
    updateCell( m_selected - 1, 0 );
    updateCell( m_selected, 0 );
  }
}

void LayerView::slotFrontLayer()
{
  cout << "LayerView::slotFrontLayer" << endl;

  if( m_selected != ( m_doc->layerList().count() - 1 ) )
  { 
    m_doc->setFrontLayer( m_selected );
    m_selected = m_doc->layerList().count() - 1;

    QRect updateRect = m_doc->layerList().at( m_selected )->imageExtents();
    m_doc->compositeImage( updateRect );
    m_doc->slotUpdateViews( updateRect );

    updateAllCells();
  }
}

void LayerView::slotBackgroundLayer()
{
  cout << "LayerView::slotBackgroundLayer" << endl;

  if( m_selected != 0 )
  {
    m_doc->setBackgroundLayer( m_selected );
    m_selected = 0;

    QRect updateRect = m_doc->layerList().at( m_selected )->imageExtents();
    m_doc->compositeImage( updateRect );
    m_doc->slotUpdateViews( updateRect );

    updateAllCells(); 
  }
}

void LayerView::updateAllCells()
{
  for( int i = 0; i < m_doc->layerList().count(); i++ )
    updateCell( i, 0 );
}

#include "layerview.moc"

