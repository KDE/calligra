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
#include <qlayout.h>

#include <kstddirs.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include "kis_util.h"
#include "kis_doc.h"
#include "kis_channelview.h"
#include "kis_framebutton.h"
#include "kis_factory.h"

//#define KISBarIcon( x ) BarIcon( x, KisFactory::global() )

KisChannelView::KisChannelView( KisDoc *_doc, QWidget *_parent, const char *_name )
  : QWidget( _parent, _name )
{
  QVBoxLayout *layout = new QVBoxLayout( this );

  ChannelTable* channeltable = new ChannelTable( _doc, this, "channellist" );
  layout->addWidget( channeltable, 1 );

  QHBox *buttons = new QHBox( this );
  layout->addWidget( buttons );

  KisFrameButton* pbAddChannel = new KisFrameButton( buttons );
  pbAddChannel->setPixmap( BarIcon( "newlayer" ) );
  connect( pbAddChannel, SIGNAL( clicked() ), channeltable, SLOT( slotAddChannel() ) );

  KisFrameButton* pbRemoveChannel = new KisFrameButton( buttons );
  pbRemoveChannel->setPixmap( BarIcon( "deletelayer" ) );
  connect( pbRemoveChannel, SIGNAL( clicked() ), channeltable, SLOT( slotRemoveChannel() ) );
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
	KMessageBox::error( this, "Can't find eye.png", "Canvas" );
  m_eyeRect = QRect( QPoint( 2,( CELLHEIGHT - m_eyeIcon->height() ) / 2 ), m_eyeIcon->size() );

  _icon = locate( "kis_pics", "link.png", KisFactory::global() );
  m_linkIcon = new QPixmap;
  if( !m_linkIcon->load( _icon ) )
	KMessageBox::error( this, "Can't find link.png", "Canvas" );
  m_linkRect = QRect( QPoint( 25,( CELLHEIGHT - m_linkIcon->height() ) / 2 ), m_linkIcon->size() );

  // HACK - the size of the preview image should be configurable somewhere
  m_previewRect = QRect( QPoint( 40, (CELLHEIGHT - m_linkIcon->height() ) /2 ),
			 QSize( 15, 15 ) );

  updateTable();

  setCellWidth( CELLWIDTH );
  setCellHeight( CELLHEIGHT );
  //  m_selected = m_doc->layerList().count() - 1;
  
  updateAllCells();
    
  QPopupMenu *submenu = new QPopupMenu();

  m_contextmenu = new QPopupMenu();

  m_contextmenu->setCheckable(TRUE);

  m_contextmenu->insertItem( i18n( "Visible" ), VISIBLE );
  m_contextmenu->insertItem( i18n( "Level" ), submenu );

  m_contextmenu->insertSeparator();

  m_contextmenu->insertItem( i18n( "Add Channel" ), ADDCHANNEL );
  m_contextmenu->insertItem( i18n( "Remove Channel"), REMOVECHANNEL );

  connect( m_contextmenu, SIGNAL( activated( int ) ), SLOT( slotMenuAction( int ) ) );
  connect( submenu, SIGNAL( activated( int ) ), SLOT( slotMenuAction( int ) ) );
}

void ChannelTable::paintCell( QPainter* _painter, int _row, int /* _col */)
{

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

  if( /* m_doc->layerList().at( _row )->isVisible()*/ true )
  {
    _painter->drawPixmap( m_eyeRect.topLeft(), *m_eyeIcon );
  }

  _painter->drawRect( 0, 0, cellWidth( 0 ) - 1, cellHeight() - 1);
  _painter->drawText( 80, 20, tmp );
}

void ChannelTable::updateTable()
{
  m_items = 4;
  setNumRows( 4 );
  setNumCols( 1 );
  resize( sizeHint() );
}

void ChannelTable::update_contextmenu( int  )
{
}

void ChannelTable::selectChannel( int/* _index*/ )
{

}

void ChannelTable::slotInverseVisibility( int  )
{
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

void ChannelTable::updateAllCells()
{
  for( unsigned int i = 0; i < /* m_doc->layerList().count()*/ 3;  i++ )
    updateCell( i, 0 );
}

#include "kis_channelview.moc"

