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

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "layerdlg.h"
#include "layerview.h"
#include "channelview.h"
#include "kimageshop_doc.h"

LayerTab::LayerTab( KImageShopDoc *_doc, QWidget *_parent, const char *_name , WFlags _flags )
  : QWidget( _parent, _name, _flags )
{
  QVBoxLayout *layout = new QVBoxLayout( this );

  LayerView* layerview = new LayerView( _doc, this, "layerlist" );
  layout->addWidget( layerview );

  QHBoxLayout *buttonlayout = new QHBoxLayout( layout );
 
  QPushButton* pbAddLayer = new QPushButton( this, "addlayer" );
  pbAddLayer->setPixmap( ICON( "newlayer.xpm" ) );
  buttonlayout->addWidget( pbAddLayer );
 
  QPushButton* pbRemoveLayer = new QPushButton( this, "removelayer" );
  pbRemoveLayer->setPixmap( ICON( "deletelayer.xpm" ) );
  buttonlayout->addWidget( pbRemoveLayer );

/* 
  QPushButton* pbAddMask = new QPushButton( this, "addmask" );
  pbAddMask->setPixmap( ICON( "newlayer.xpm" ) );
  buttonlayout->addWidget( pbAddMask );
 
  QPushButton* pbRemoveMask = new QPushButton( this, "removemask" );
  pbRemoveMask->setPixmap( ICON( "removelayer.xpm" ) );
  buttonlayout->addWidget( pbRemoveMask );
*/
 
  QPushButton* pbUp = new QPushButton( this, "up" );
  pbUp->setPixmap( ICON( "raiselayer.xpm" ) );
  buttonlayout->addWidget( pbUp );
  connect( pbUp, SIGNAL( clicked() ), layerview, SLOT( slotUpperLayer() ) );

  QPushButton* pbDown = new QPushButton( this, "down" );
  pbDown->setPixmap( ICON( "lowerlayer.xpm" ) );
  buttonlayout->addWidget( pbDown );
  connect( pbDown, SIGNAL( clicked() ), layerview, SLOT( slotLowerLayer() ) );

  setMinimumSize( sizeHint() );
}

ChannelTab::ChannelTab( KImageShopDoc *_doc, QWidget *_parent, const char *_name , WFlags _flags )
  : QWidget( _parent, _name, _flags )
{
  QVBoxLayout *layout = new QVBoxLayout( this );
 
  ChannelView* channelview = new ChannelView( _doc, this, "channellist" );
  layout->addWidget( channelview );
 
  QHBoxLayout *buttonlayout = new QHBoxLayout( layout );
 
  QPushButton* pbAddLayer = new QPushButton( this, "addchannel" );
  pbAddLayer->setPixmap( ICON( "newlayer.xpm" ) );
  buttonlayout->addWidget( pbAddLayer );
 
  QPushButton* pbRemoveLayer = new QPushButton( this, "removechannel" );
  pbRemoveLayer->setPixmap( ICON( "deletelayer.xpm" ) );
  buttonlayout->addWidget( pbRemoveLayer );
 
  QPushButton* pbUp = new QPushButton( this, "raise" );
  pbUp->setPixmap( ICON( "raiselayer.xpm" ) );
  buttonlayout->addWidget( pbUp );
  connect( pbUp, SIGNAL( clicked() ), channelview, SLOT( slotRaiseChannel() ) );
 
  QPushButton* pbDown = new QPushButton( this, "lower" );
  pbDown->setPixmap( ICON( "lowerlayer.xpm" ) );
  buttonlayout->addWidget( pbDown );
  connect( pbDown, SIGNAL( clicked() ), channelview, SLOT( slotLowerChannel() ) );
}

LayerDialog::LayerDialog( KImageShopDoc *_doc, QWidget *_parent, const char *_name, WFlags _flags )
  : KTabCtl( _parent, _name )
{
  LayerTab *layerTab = new LayerTab( _doc, this, _name );
  ChannelTab *channelTab =  new ChannelTab( _doc, this, _name );

  addTab( layerTab, i18n( "Layers" ) );
  addTab( channelTab, i18n( "Channels" ) );

  setMinimumSize( layerTab->sizeHint() );
}

#include "layerdlg.moc"
