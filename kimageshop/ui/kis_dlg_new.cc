/*
 *  newdialog.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Sven Fischer    <herpes@kawo2.rwth-aachen.de>
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

const int MAXIMAGEWIDTH = 32767;
const int INITIALWIDTH = 512;
const int MAXIMAGEHEIGHT = 32767;
const int INITIALHEIGHT = 512;

#include "kis_dlg_new.h"

#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qspinbox.h>


NewDialog::NewDialog( QWidget *parent, const char *name )
    : KDialog( parent, name, TRUE )
{
    setCaption( "New Image" );

    QVBoxLayout* layout = new QVBoxLayout( this, 3 );

    QGridLayout* grid = new QGridLayout( layout, 2, 2);

    iwidth = new QSpinBox( 1, MAXIMAGEWIDTH, 10, this );
    iwidth->setValue( INITIALWIDTH );
    QLabel* wlabel = new QLabel( iwidth, "W&idth", this );

    grid->addWidget( wlabel, 0, 0 );
    grid->addWidget( iwidth, 0, 1 );

    iheight = new QSpinBox( 1, MAXIMAGEHEIGHT, 10, this );
    iheight->setValue( INITIALHEIGHT );
    QLabel* hlabel = new QLabel( iheight, "&Height", this );

    grid->addWidget( hlabel, 1, 0 );
    grid->addWidget( iheight, 1, 1 );

    // Create an exclusive button group
    QButtonGroup *cmodeg = new QButtonGroup( 1, QGroupBox::Horizontal, "Color Mode", this);
    cmodeg->setExclusive( TRUE );

    // insert radiobuttons
	cmode[CM_INDEXED] = new QRadioButton( "&Indexed", cmodeg );
	cmode[CM_INDEXED]->setEnabled(false);
	cmode[CM_GREYSCALE] = new QRadioButton( "&Greyscale", cmodeg );
    cmode[CM_GREYSCALE]->setEnabled(false);
    cmode[CM_RGB] = new QRadioButton( "&RGB", cmodeg );
    cmode[CM_RGB]->setEnabled(false);
	cmode[CM_RGBA] = new QRadioButton( "RGB + &alpha channel", cmodeg );
	cmode[CM_RGBA]->setChecked( true );
	cmode[CM_CMYK] = new QRadioButton( "&CMYK", cmodeg );
    cmode[CM_CMYK]->setEnabled(false);
	cmode[CM_CMYKA] = new QRadioButton( "CMY&K + alpha channel", cmodeg );
    cmode[CM_CMYKA]->setEnabled(false);

    layout->addWidget( cmodeg );

    // Create an exclusive button group
    QButtonGroup *ground = new QButtonGroup( 1, QGroupBox::Horizontal, "Background", this);
    ground->setExclusive( TRUE );

    // insert radiobuttons
    bground[BM_BACKGROUNDCOLOR] = new QRadioButton( "&Background Color", ground );
    bground[BM_FOREGROUNDCOLOR] = new QRadioButton( "&Foreground Color", ground );
    bground[BM_WHITE] = new QRadioButton( "&White", ground );
    bground[BM_WHITE]->setChecked( true );
    bground[BM_TRANSPARENT] = new QRadioButton( "&Transparent", ground );

    layout->addWidget( ground );

    QHBoxLayout* buttons = new QHBoxLayout( layout );

    buttons->addStretch( 3 );

    QPushButton *ok, *cancel;
    ok = new QPushButton( "&OK", this );
    ok->setDefault( true );
    ok->setMinimumSize( ok->sizeHint() );
    connect( ok, SIGNAL(clicked()), SLOT(accept()) );
    buttons->addWidget( ok );

    cancel = new QPushButton( "&Cancel", this );
    cancel->setMinimumSize( cancel->sizeHint() );
    connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
    buttons->addWidget( cancel );

    resize( 1, 1 );
}

bgMode NewDialog::backgroundMode()
{
  if( bground[BM_BACKGROUNDCOLOR]->isChecked() ) return BM_BACKGROUNDCOLOR;
  if( bground[BM_FOREGROUNDCOLOR]->isChecked() ) return BM_FOREGROUNDCOLOR;
  if( bground[BM_TRANSPARENT]->isChecked() ) return BM_TRANSPARENT;
  if( bground[BM_WHITE]->isChecked() ) return BM_WHITE;

  else return BM_WHITE;
}

cMode NewDialog::colorMode()
{
  if( cmode[CM_INDEXED]->isChecked() ) return CM_INDEXED;
  if( cmode[CM_GREYSCALE]->isChecked() ) return CM_GREYSCALE;
  if( cmode[CM_RGB]->isChecked() ) return CM_RGB;
  if( cmode[CM_RGBA]->isChecked() ) return CM_RGBA;
  if( cmode[CM_CMYK]->isChecked() ) return CM_CMYK;
  if( cmode[CM_CMYKA]->isChecked() ) return CM_CMYKA;

  else return CM_RGBA;
}

#include "kis_dlg_new.moc"
