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

#include <qhbox.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qlayout.h>

#include <kstddirs.h>
#include <klocale.h>

#include "kis_util.h"
#include "kis_doc.h"
#include "kis_channelview.h"

KisChannelView::KisChannelView( KisDoc *_doc, QWidget *_parent, const char *_name )
  : QWidget( _parent, _name )
{
  QVBoxLayout *layout = new QVBoxLayout( this );

  ChannelTable* channeltable = new ChannelTable( _doc, this, "channellist" );
  layout->addWidget( channeltable, 1 );

  QHBox *buttons = new QHBox( this );
  layout->addWidget( buttons );
  
  QPushButton* pbAddChannel = new QPushButton( buttons, "addchannel" );
  pbAddChannel->setPixmap( BarIcon( "newchannel" ) );
  connect( pbAddChannel, SIGNAL( clicked() ), channeltable, SLOT( slotAddChannel() ) );
  
  QPushButton* pbRemoveChannel = new QPushButton( buttons, "removechannel" );
  pbRemoveChannel->setPixmap( BarIcon( "deletechannel" ) );
  connect( pbRemoveChannel, SIGNAL( clicked() ), channeltable, SLOT( slotRemoveChannel() ) );
  
  QPushButton* pbUp = new QPushButton( buttons, "up" );
  pbUp->setPixmap( BarIcon( "raisechannel" ) );
  connect( pbUp, SIGNAL( clicked() ), channeltable, SLOT( slotRaiseChannel() ) );

  QPushButton* pbDown = new QPushButton( buttons, "down" );
  pbDown->setPixmap( BarIcon( "lowerchannel" ) );
  connect( pbDown, SIGNAL( clicked() ), channeltable, SLOT( slotLowerChannel() ) );
}

ChannelTable::ChannelTable( QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( 0 );
}

ChannelTable::ChannelTable( KisDoc* doc, QWidget* _parent, const char* _name )
  : QTableView( _parent, _name )
{
  init( doc );
}

void ChannelTable::init( KisDoc* doc )
{
  setTableFlags( Tbl_autoHScrollBar | Tbl_autoVScrollBar );

  m_doc = doc;

  setBackgroundColor( white );

  // load icon pixmaps
  QString _icon = locate( "kis_pics", "eye.png", KisFactory::global() );
  m_eyeIcon = new QPixmap;
  if( !m_eyeIcon->load( _icon ) )
	QMessageBox::critical( this, "Canvas", "Can't find eye.png" );
  m_eyeRect = QRect( QPoint( 2,( CELLHEIGHT - m_eyeIcon->height() ) / 2 ), m_eyeIcon->size() );

  _icon = locate( "kis_pics", "link.png", KisFactory::global() );
  m_linkIcon = new QPixmap;
  if( !m_linkIcon->load( _icon ) )
	QMessageBox::critical( this, "Canvas", "Can't find link.png" );
  m_linkRect = QRect( QPoint( 25,( CELLHEIGHT - m_linkIcon->height() ) / 2 ), m_linkIcon->size() );
  
  // HACK - the size of the preview image should be configurable somewhere
  m_previewRect = QRect( QPoint( 40, (CELLHEIGHT - m_linkIcon->height() ) /2 ),
			 QSize( 15, 15 ) );

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

void ChannelTable::paintCell( QPainter* /*_painter*/, int /*_row*/, int )
{
/*
  QString tmp;

  switch( _row )
  {
    case 0 :
      tmp = i18n( "Red" );
      break;
    case 1 :
      tmp = i18n( "Green" );
      break;
    case 2 :
      tmp = i18n( "Blue" );
      break;
    default :
      tmp = i18n( "Alpha" );
      break;
  }

  if( _row == m_selected )
  {
    _painter->fillRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1, green );
  }

  if( m_doc->layerList().at( _row )->isVisible() )
  {
    _painter->drawPixmap( m_eyeRect.topLeft(), *m_eyeIcon );
  }

  _painter->drawRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1);
  _painter->drawText( 80, 20, tmp );
*/
}

void ChannelTable::updateTable()
{
  if( m_doc )
  {
    m_items = 1;
    setNumRows( 3 );
    setNumCols( 1 );
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

void ChannelTable::update_contextmenu( int _index )
{
  m_contextmenu->setItemChecked( VISIBLE, m_doc->layerList().at( _index )->isVisible() );
}

void ChannelTable::selectChannel( int/* _index*/ )
{
/*
  unsigned int currentSel = m_selected;
  m_selected = -1;
  updateCell( currentSel, 0 );
  m_selected = _index;
  m_doc->setCurrentLayer( m_selected );
  updateCell( m_selected, 0 );
*/
}

void ChannelTable::slotInverseVisibility( int _index )
{
  m_doc->layerList().at( _index )->setVisible( !m_doc->layerList().at( _index )->isVisible() );
  updateCell( _index, 0 );
  m_doc->compositeImage( m_doc->layerList().at( _index )->imageExtents() );
  //  m_doc->slotUpdateViews( m_doc->layerList().at( _index )->imageExtents() );
}

void ChannelTable::slotMenuAction( int _id )
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

QSize ChannelTable::sizeHint() const
{
  return QSize( CELLWIDTH, CELLHEIGHT * 5 );
}

void ChannelTable::mousePressEvent( QMouseEvent *_event )
{
  int row = findRow( _event->pos().y() );
  QPoint localPoint( _event->pos().x() % cellWidth(), _event->pos().y() % cellHeight() );

  if( _event->button() & LeftButton )
  {
    if( m_eyeRect.contains( localPoint ) )
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
    selectChannel( row );
    update_contextmenu( row );
    m_contextmenu->popup( mapToGlobal( _event->pos() ) );
  }
}

void ChannelTable::slotAddChannel()
{
}

void ChannelTable::slotRemoveChannel()
{
}

void ChannelTable::swapChannels( int a, int b )
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

void ChannelTable::slotRaiseChannel()
{
/*
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
*/
}

void ChannelTable::slotLowerChannel()
{
/*
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
*/
}

void ChannelTable::updateAllCells()
{
  for( unsigned int i = 0; i < m_doc->layerList().count(); i++ )
    updateCell( i, 0 );
}

#include "kis_channelview.moc"

