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
  QGridLayout* layout = new QGridLayout( this, 4, 2 );
 
  LayerView* layerview = new LayerView( _doc, this, "layerlist" );
  layout->addMultiCellWidget( layerview, 0, 0, 0, 1 );
 
  QPushButton* pbAddLayer = new QPushButton( this, "addlayer" );
  pbAddLayer->setPixmap( ICON( "newlayer.xpm" ) );
  layout->addWidget( pbAddLayer, 2, 0 );
 
  QPushButton* pbRemoveLayer = new QPushButton( this, "removelayer" );
  pbRemoveLayer->setPixmap( ICON( "deletelayer.xpm" ) );
  layout->addWidget( pbRemoveLayer, 2, 1 );

/* 
  QPushButton* pbAddMask = new QPushButton( this, "addmask" );
  pbAddMask->setPixmap( ICON( "newlayer.xpm" ) );
  layout->addWidget( pbAddMask, 2, 0 );
 
  QPushButton* pbRemoveMask = new QPushButton( this, "removemask" );
  pbRemoveMask->setPixmap( ICON( "removelayer.xpm" ) );
  layout->addWidget( pbRemoveMask, 2, 1 );
*/
 
  QPushButton* pbUp = new QPushButton( this, "up" );
  pbUp->setPixmap( ICON( "raiselayer.xpm" ) );
  layout->addWidget( pbUp, 3, 0 );
  connect( pbUp, SIGNAL( clicked() ), layerview, SLOT( slotUpperLayer() ) );

  QPushButton* pbDown = new QPushButton( this, "down" );
  pbDown->setPixmap( ICON( "lowerlayer.xpm" ) );
  layout->addWidget( pbDown, 3, 1 );
  connect( pbDown, SIGNAL( clicked() ), layerview, SLOT( slotLowerLayer() ) );

  layout->setRowStretch( 0, 1 );
//layout->setColStretch( 0, 1 );

//layout->addColSpacing( 0, layerview->sizeHint().width() );
  layout->addRowSpacing( 0, layerview->sizeHint().height() );
}

ChannelTab::ChannelTab( KImageShopDoc *_doc, QWidget *_parent, const char *_name , WFlags _flags )
  : QWidget( _parent, _name, _flags )
{
}

LayerDialog::LayerDialog( KImageShopDoc *_doc, QWidget *_parent, const char *_name, WFlags _flags )
  : QTabDialog( _parent, _name, _flags )
{
  addTab( new LayerTab( _doc, _parent, _name ), i18n( "Layers" ) );
  addTab( new ChannelTab( _doc, _parent, _name ), i18n( "Channels" ) );

  setOkButton( i18n( "Close" ) );
}

#include "layerdlg.moc"
