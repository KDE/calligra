/*
 *  opt_line_dialog.cc - part of Krayon
 *
 *  Copyright (c) 2001 John Califf <jcaliff@compuzone.net>
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

#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>

#include <klocale.h>

#include "opts_line_dlg.h"

LineOptionsDialog::LineOptionsDialog( bool _solid, bool _pattern,
    int _thickness, int _opacity, QWidget *parent, const char *name )
    : KDialog( parent, name, true )
{
    setCaption( i18n("Current Tool Options") );

    QVBoxLayout* layout = new QVBoxLayout( this, 4 );
    QGridLayout* grid = new QGridLayout( layout, 2, 4);

    mpThickness = new QSpinBox( 1, 16, 1, this );
    mpThickness->setValue( _thickness );
    QLabel* tlabel = new QLabel( mpThickness, i18n("T&hickness"), this );

    grid->addWidget( tlabel, 0, 0 );
    grid->addWidget( mpThickness, 0, 1 );

    mpOpacity = new QSpinBox( 0, 255, 32, this );
    mpOpacity->setValue( _opacity );
    QLabel* olabel = new QLabel( mpOpacity, i18n("&Opacity"), this );

    grid->addWidget( olabel, 1, 0 );
    grid->addWidget( mpOpacity, 1, 1 );

    mpSolid = new QCheckBox( this );
    mpSolid->setChecked( _solid );
    QLabel* slabel = new QLabel( mpSolid, i18n("Fill Interior Regions"), this );

    grid->addWidget( slabel, 2, 0 );
    grid->addWidget( mpSolid, 2, 1 );

    mpUsePattern = new QCheckBox( this );
    mpUsePattern->setChecked( _pattern );
    QLabel* plabel = new QLabel( mpUsePattern, i18n("Use Current Pattern"), this );

    grid->addWidget( plabel, 3, 0 );
    grid->addWidget( mpUsePattern, 3, 1 );

    QHBoxLayout* buttons = new QHBoxLayout( layout, 3 );
    buttons->addStretch( 3 );

    QPushButton *ok, *cancel, *save;
    ok = new QPushButton( i18n("&OK"), this );
    ok->setDefault( true );
    ok->setMinimumSize( ok->sizeHint() );
    connect( ok, SIGNAL(clicked()), SLOT(accept()) );
    buttons->addWidget( ok );

    cancel = new QPushButton( i18n("&Cancel"), this );
    cancel->setMinimumSize( cancel->sizeHint() );
    connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
    buttons->addWidget( cancel );

    save = new QPushButton( i18n("&Save"), this );
    save->setMinimumSize( save->sizeHint() );
    //connect( save, SIGNAL(clicked()), SLOT(reject()) );
    buttons->addWidget( save );
 
    resize( 1, 1 );
}

#include "opts_line_dlg.moc"
