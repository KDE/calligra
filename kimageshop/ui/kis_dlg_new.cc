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
    : QDialog( parent, name, TRUE )
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
	cmode[cm_Indexed] = new QRadioButton( "&Indexed", cmodeg );
	cmode[cm_Indexed]->setEnabled(false);

	cmode[cm_Greyscale] = new QRadioButton( "&Greyscale", cmodeg );
    cmode[cm_Greyscale]->setEnabled(false);

    cmode[cm_RGB] = new QRadioButton( "&RGB", cmodeg );

	cmode[cm_RGBA] = new QRadioButton( "RGB + &alpha channel", cmodeg );
	cmode[cm_RGBA]->setChecked( true );

	cmode[cm_CMYK] = new QRadioButton( "&CMYK", cmodeg );
    cmode[cm_CMYK]->setEnabled(false);

	cmode[cm_CMYKA] = new QRadioButton( "CMY&K + alpha channel", cmodeg );
    cmode[cm_CMYKA]->setEnabled(false);

    layout->addWidget( cmodeg );

    // Create an exclusive button group
    QButtonGroup *ground = new QButtonGroup( 1, QGroupBox::Horizontal, "Background", this);
    ground->setExclusive( TRUE );

    // insert radiobuttons
    bground[bm_BackgroundColor] = new QRadioButton( "&Background Color", ground );
    bground[bm_ForegroundColor] = new QRadioButton( "&Foreground Color", ground );
    bground[bm_White] = new QRadioButton( "&White", ground );
    bground[bm_White]->setChecked( true );
    bground[bm_Transparent] = new QRadioButton( "&Transparent", ground );

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

    //resize( 1, 1 );
    resize(320,320);
}

bgMode NewDialog::backgroundMode()
{
  if( bground[bm_BackgroundColor]->isChecked() ) return bm_BackgroundColor;
  if( bground[bm_ForegroundColor]->isChecked() ) return bm_ForegroundColor;
  if( bground[bm_Transparent]->isChecked() ) return bm_Transparent;
  if( bground[bm_White]->isChecked() ) return bm_White;

  else return bm_White;
}

cMode NewDialog::colorMode()
{
  if( cmode[cm_Indexed]->isChecked() ) return cm_Indexed;
  if( cmode[cm_Greyscale]->isChecked() ) return cm_Greyscale;
  if( cmode[cm_RGB]->isChecked() ) return cm_RGB;
  if( cmode[cm_RGBA]->isChecked() ) return cm_RGBA;
  if( cmode[cm_CMYK]->isChecked() ) return cm_CMYK;
  if( cmode[cm_CMYKA]->isChecked() ) return cm_CMYKA;

  else return cm_RGBA;
}

#include "kis_dlg_new.moc"
