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

#include "layerdlg.h"
#include "layerview.h"
#include "kimageshop_doc.h"

LayerDialog::LayerDialog(KImageShopDoc* doc, QWidget* _parent, const char* _name, WFlags _flags )
  : QDialog( _parent, _name, _flags )
{
  QGridLayout* layout = new QGridLayout( this, 10, 2, 15, 7 );

  LayerView* layerlist = new LayerView( doc, this, "layerlist" );
  layout->addMultiCellWidget( layerlist, 0, 9, 0, 0 );

  QPushButton* pbAddLayer = new QPushButton( this, "addlayer" );
  pbAddLayer->setText( i18n( "Add layer" ) );
  layout->addWidget( pbAddLayer, 0, 1 );

  QPushButton* pbRemoveLayer = new QPushButton( this, "removelayer" );
  pbRemoveLayer->setText( i18n( "Remove layer" ) );
  layout->addWidget( pbRemoveLayer, 1, 1 );

  QPushButton* pbAddMask = new QPushButton( this, "addmask" );
  pbAddMask->setText( i18n( "Add mask" ) );
  layout->addWidget( pbAddMask, 2, 1 );

  QPushButton* pbRemoveMask = new QPushButton( this, "removemask" );
  pbRemoveMask->setText( i18n( "Remove mask" ) );
  layout->addWidget( pbRemoveMask, 3, 1 );

  QPushButton* pbUp = new QPushButton( this, "up" );
  pbUp->setText( i18n( "Up" ) );
  layout->addWidget( pbUp, 4, 1 );

  QPushButton* pbDown = new QPushButton( this, "down" );
  pbDown->setText( i18n( "Down" ) );
  layout->addWidget( pbDown, 5, 1 );

  QPushButton* pbClose = new QPushButton( this, "closebutton" );
  pbClose->setText( i18n( "Close" ) );
  layout->addWidget( pbClose, 9, 1 );
  QObject::connect( pbClose, SIGNAL( clicked() ), this, SLOT( hide() ) );

  layout->setRowStretch( 8, 1 );
  layout->setColStretch( 0, 1 );

  layout->addColSpacing( 0, layerlist->sizeHint().width() );
}

#include "layerdlg.moc"
