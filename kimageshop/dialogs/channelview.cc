/*
 *  channelview.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
 *                1999 Michael Koch    <koch@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

// only for debug
#include <iostream.h>

#include <qpainter.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>

#include <klocale.h>

#include "misc.h"
#include "kimageshop_doc.h"
#include "channelview.h"
#include "layerdlg.h"

ChannelView::ChannelView( QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( 0 );
}

ChannelView::ChannelView( KImageShopDoc* doc, QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( doc );
}

void ChannelView::init( KImageShopDoc* doc )
{
  setTableFlags( Tbl_autoHScrollBar | Tbl_autoVScrollBar );

  m_doc = doc;

  setBackgroundColor( white );
  updateTable();

  setCellWidth( CELLWIDTH );
  setCellHeight( CELLHEIGHT );
  m_selected = m_doc->layerList().count() - 1;

  QPopupMenu *submenu = new QPopupMenu();
 
  submenu->insertItem( i18n( "Upper" ), RAISECHANNEL );
  submenu->insertItem( i18n( "Lower" ), LOWERCHANNEL );

  m_contextmenu = new QPopupMenu();

  m_contextmenu->setCheckable(TRUE);

  m_contextmenu->insertItem( i18n( "Visible" ), VISIBLE );
  m_contextmenu->insertItem( i18n( "Level" ), submenu );

  m_contextmenu->insertSeparator();

  m_contextmenu->insertItem( i18n( "Add Layer" ), ADDCHANNEL );
  m_contextmenu->insertItem( i18n( "Remove Layer"), REMOVECHANNEL );

  connect( m_contextmenu, SIGNAL( activated( int ) ), SLOT( slotMenuAction( int ) ) );
  connect( submenu, SIGNAL( activated( int ) ), SLOT( slotMenuAction( int ) ) );
}

void ChannelView::paintCell( QPainter* _painter, int _row, int )
{
  if( _row == m_selected )
  {
    _painter->fillRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1, green );
  }

  if( m_doc->layerList().at( _row )->isVisible() )
  {
    _painter->drawPixmap( LayerDialog::m_eyeRect.topLeft(), *LayerDialog::m_eyeIcon );
  }

  _painter->drawRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1);
  _painter->drawText( 80, 20, m_doc->layerList().at( _row )->name() );
}

void ChannelView::updateTable()
{
  if( m_doc )
  {
    m_items = 0;
    setNumRows( 0 );
    setNumCols( 0 );
/*
    m_items = m_doc->layerList().count();
    setNumRows( m_items );
    setNumCols( 1 );
*/
  }
  else
  {
    m_items = 0;
    setNumRows( 0 );
    setNumCols( 0 );
  }
  resize( sizeHint() );
}

void ChannelView::update_contextmenu( int _index )
{
  m_contextmenu->setItemChecked( VISIBLE, m_doc->layerList().at( _index )->isVisible() );
}

void ChannelView::selectChannel( int _index )
{
  unsigned int currentSel = m_selected;
  m_selected = -1;
  updateCell( currentSel, 0 );
  m_selected = _index;
  m_doc->setCurrentLayer( m_selected );
  updateCell( m_selected, 0 );
}

void ChannelView::slotInverseVisibility( int _index )
{
  m_doc->layerList().at( _index )->setVisible( !m_doc->layerList().at( _index )->isVisible() );
  updateCell( _index, 0 );
  m_doc->compositeImage( m_doc->layerList().at( _index )->imageExtents() );
  //  m_doc->slotUpdateViews( m_doc->layerList().at( _index )->imageExtents() );
}

void ChannelView::slotMenuAction( int _id )
{
  switch( _id )
  {
    case VISIBLE:
      slotInverseVisibility( m_selected );
      break;
    case ADDCHANNEL:
      slotAddChannel();
      break;
    case REMOVECHANNEL:
      slotRemoveChannel();
      break;
    case RAISECHANNEL:
      slotRaiseChannel();
      break;
    case LOWERCHANNEL:
      slotLowerChannel();
      break;
    default:
      cerr << "Michael : unknown context menu action" << endl;
      break;
  }
}

QSize ChannelView::sizeHint() const
{
  return QSize( CELLWIDTH, CELLHEIGHT * 5 );
}

void ChannelView::mousePressEvent( QMouseEvent *_event )
{
  int row = findRow( _event->pos().y() );
  QPoint localPoint( _event->pos().x() % cellWidth(), _event->pos().y() % cellHeight() );

  if( _event->button() & LeftButton )
  {
    if( LayerDialog::m_eyeRect.contains( localPoint ) )
    {
      slotInverseVisibility( row );
    }
    else if( row != -1 )
    {
      selectChannel( row );
    }
  }
  else if( _event->button() & RightButton )
  {
    // TODO: Should the Layer under the cursor selected when clicking RMB ?

    selectChannel( row );
    update_contextmenu( row );
    m_contextmenu->popup( mapToGlobal( _event->pos() ) );
  }
}

void ChannelView::slotAddChannel()
{
}

void ChannelView::slotRemoveChannel()
{
}

void ChannelView::swapChannels( int a, int b )
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
      //      m_doc->slotUpdateViews( rect );
    }
  }
}

void ChannelView::slotRaiseChannel()
{
  unsigned int newpos = m_selected > 0 ? m_selected - 1 : 0;

  if( m_selected != newpos )
  {
    m_doc->upperLayer( m_selected );
    repaint();
    swapChannels( m_selected, newpos );
    m_selected = newpos;
    updateCell( m_selected + 1, 0 );
    updateCell( m_selected, 0 );
  }
}

void ChannelView::slotLowerChannel()
{
  unsigned int newpos = ( m_selected + 1 ) < m_doc->layerList().count() ? m_selected + 1 : m_selected;

  if( m_selected != newpos )
  {
    m_doc->lowerLayer( m_selected );
    repaint();
    swapChannels( m_selected, newpos );
    m_selected = newpos;
    updateCell( m_selected - 1, 0 );
    updateCell( m_selected, 0 );
  }
}

void ChannelView::updateAllCells()
{
  for( unsigned int i = 0; i < m_doc->layerList().count(); i++ )
    updateCell( i, 0 );
}

#include "channelview.moc"

