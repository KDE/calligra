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

#include "newdialog.h"

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
    QButtonGroup *model = new QButtonGroup( 1, QGroupBox::Horizontal, "Color Model", this);
    model->setExclusive( TRUE );

    // insert 4 radiobuttons
    cmodel[RGB] = new QRadioButton( "&RGB", model );
    cmodel[RGB]->setChecked( TRUE );
    cmodel[CMYK] = new QRadioButton( "CM&YK", model );
    cmodel[CMYK]->setEnabled(false);
    cmodel[LAB] = new QRadioButton( "&LAB", model );
    cmodel[LAB]->setEnabled(false);
    cmodel[GREYSCALE] = new QRadioButton( "&Greyscale", model );
    cmodel[GREYSCALE]->setEnabled(false);

    layout->addWidget( model );

    // Create an exclusive button group
    QButtonGroup *ground = new QButtonGroup( 1, QGroupBox::Horizontal, "Background", this);
    ground->setExclusive( TRUE );

    // insert 4 radiobuttons
    bground[BACKGROUND] = new QRadioButton( "&Background Color", ground );
    bground[BACKGROUND]->setChecked( TRUE );
    bground[FOREGROUND] = new QRadioButton( "&Foreground Color", ground );
    bground[WHITE] = new QRadioButton( "&White", ground );
    bground[TRANSPARENT] = new QRadioButton( "&Transparent", ground );

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

int
NewDialog::background()
{
    if( bground[BACKGROUND]->isChecked() ) return BACKGROUND;
    if( bground[FOREGROUND]->isChecked() ) return FOREGROUND;
    if( bground[TRANSPARENT]->isChecked() ) return TRANSPARENT;
    if( bground[WHITE]->isChecked() ) return WHITE;
    else return TRANSPARENT;
}

int
NewDialog::colorModel()
{
    if( cmodel[RGB]->isChecked() ) return RGB;
    if( cmodel[CMYK]->isChecked() ) return CMYK;
    if( cmodel[LAB]->isChecked() ) return LAB;
    if( cmodel[GREYSCALE]->isChecked() ) return GREYSCALE;
    else return RGB;
}

#include "newdialog.moc"
