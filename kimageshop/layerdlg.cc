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
#include "layerlist.h"
#include "canvas.h"

/*
LayerDialog::LayerDialog( QWidget* _parent, const char* _name, WFlags _flags )
  : QDialog( _parent, _name, _flags)
{
}
*/

LayerDialog::LayerDialog( Canvas* _canvas, QWidget* _parent, const char* _name, WFlags _flags )
  : QDialog( _parent, _name, _flags )
{
  QGridLayout* layout = new QGridLayout( this, 6, 2, 15, 7 );

  LayerList* layerlist = new LayerList( _canvas, this, "layerlist" );
  layerlist->resize( layerlist->sizeHint() );
  layout->addMultiCellWidget( layerlist, 0, 5, 0, 0 );

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

  QPushButton* pbClose = new QPushButton( this, "closebutton" );
  pbClose->setText( i18n( "Close" ) );
  layout->addWidget( pbClose, 5, 1 );
  QObject::connect( pbClose, SIGNAL( clicked() ), this, SLOT( reject() ) );

  layout->setRowStretch( 4, 1 );
  layout->setColStretch( 0, 1 );

  layout->addColSpacing( 0, layerlist->sizeHint().width() );
}

#include "layerdlg.moc"
