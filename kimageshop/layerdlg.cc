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

#include <qlayout.h>
#include <qpushbutton.h>

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "layerdlg.h"
#include "layerview.h"
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
