/*
 *  layerdlg.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch    <mkoch@kde.org>
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

#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kstddirs.h>

#include "layerdlg.h"
#include "layerview.h"
#include "channelview.h"
#include "kimageshop_doc.h"

QPixmap *LayerDialog::m_eyeIcon, *LayerDialog::m_linkIcon;
QRect LayerDialog::m_eyeRect, LayerDialog::m_linkRect;

LayerTab::LayerTab( KImageShopDoc *_doc, QWidget *_parent, const char *_name , WFlags _flags )
  : QWidget( _parent, _name, _flags )
{
  QVBoxLayout *layout = new QVBoxLayout( this );

  LayerView* layerview = new LayerView( _doc, this, "layerlist" );
  layout->addWidget( layerview );

  QHBoxLayout *buttonlayout = new QHBoxLayout( layout );
 
  QPushButton* pbAddLayer = new QPushButton( this, "addlayer" );
  pbAddLayer->setPixmap( BarIcon( "newlayer" ) );
  buttonlayout->addWidget( pbAddLayer );
 
  QPushButton* pbRemoveLayer = new QPushButton( this, "removelayer" );
  pbRemoveLayer->setPixmap( BarIcon( "deletelayer" ) );
  buttonlayout->addWidget( pbRemoveLayer );

/* 
  QPushButton* pbAddMask = new QPushButton( this, "addmask" );
  pbAddMask->setPixmap( BarIcon( "newlayer" ) );
  buttonlayout->addWidget( pbAddMask );
 
  QPushButton* pbRemoveMask = new QPushButton( this, "removemask" );
  pbRemoveMask->setPixmap( BarIcon( "removelayer" ) );
  buttonlayout->addWidget( pbRemoveMask );
*/
 
  QPushButton* pbUp = new QPushButton( this, "up" );
  pbUp->setPixmap( BarIcon( "raiselayer" ) );
  buttonlayout->addWidget( pbUp );
  connect( pbUp, SIGNAL( clicked() ), layerview, SLOT( slotUpperLayer() ) );

  QPushButton* pbDown = new QPushButton( this, "down" );
  pbDown->setPixmap( BarIcon( "lowerlayer" ) );
  buttonlayout->addWidget( pbDown );
  connect( pbDown, SIGNAL( clicked() ), layerview, SLOT( slotLowerLayer() ) );

  // Matthias: Never use fixed sizes with kfloatingdialog.
  //setMinimumSize( sizeHint() );
}

ChannelTab::ChannelTab( KImageShopDoc *_doc, QWidget *_parent, const char *_name , WFlags _flags )
  : QWidget( _parent, _name, _flags )
{
  QVBoxLayout *layout = new QVBoxLayout( this );
 
  ChannelView* channelview = new ChannelView( _doc, this, "channellist" );
  layout->addWidget( channelview );
 
  QHBoxLayout *buttonlayout = new QHBoxLayout( layout );
 
  QPushButton* pbAddLayer = new QPushButton( this, "addchannel" );
  pbAddLayer->setPixmap( BarIcon( "newlayer" ) );
  buttonlayout->addWidget( pbAddLayer );
 
  QPushButton* pbRemoveLayer = new QPushButton( this, "removechannel" );
  pbRemoveLayer->setPixmap( BarIcon( "deletelayer" ) );
  buttonlayout->addWidget( pbRemoveLayer );
 
  QPushButton* pbUp = new QPushButton( this, "raise" );
  pbUp->setPixmap( BarIcon( "raiselayer" ) );
  buttonlayout->addWidget( pbUp );
  connect( pbUp, SIGNAL( clicked() ), channelview, SLOT( slotRaiseChannel() ) );
 
  QPushButton* pbDown = new QPushButton( this, "lower" );
  pbDown->setPixmap( BarIcon( "lowerlayer" ) );
  buttonlayout->addWidget( pbDown );
  connect( pbDown, SIGNAL( clicked() ), channelview, SLOT( slotLowerChannel() ) );
}

LayerDialog::LayerDialog( KImageShopDoc *_doc, QWidget *_parent, const char *_name, WFlags _flags )
  : KFloatingTabDialog(_parent, _name)
{
  setCaption( i18n( "Layers&Channels" ) );
  if( !m_eyeIcon )
  {
    QString _icon = locate( "appdata", "pics/eye.png" );
    m_eyeIcon = new QPixmap;
    if( !m_eyeIcon->load( _icon ) )
      QMessageBox::critical( this, "Canvas", "Can't find eye.png" );
    m_eyeRect = QRect( QPoint( 2,( CELLHEIGHT - m_eyeIcon->height() ) / 2 ), m_eyeIcon->size() );
  }
  if( !m_linkIcon )
  {
    QString _icon = locate( "appdata", "pics/link.png" );
    m_linkIcon = new QPixmap;
    if( !m_linkIcon->load( _icon ) )
      QMessageBox::critical( this, "Canvas", "Can't find link.png" );
    m_linkRect = QRect( QPoint( 25,( CELLHEIGHT - m_linkIcon->height() ) / 2 ), m_linkIcon->size() );
  }

  m_pLayerTab = new LayerTab(_doc, this, _name);
  m_pChannelTab =  new ChannelTab(_doc, this, _name);
  
  addTab(m_pLayerTab, i18n("Layers"));
  addTab(m_pChannelTab, i18n("Channels"));
}

#include "layerdlg.moc"


