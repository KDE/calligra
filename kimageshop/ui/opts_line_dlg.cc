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

#include "opts_line_dlg.h"



LineOptionsDialog::LineOptionsDialog( QWidget *parent, const char *name )
    : KDialog( parent, name, true )
{
    setCaption( "Current Tool Options" );

    QVBoxLayout* layout = new QVBoxLayout( this, 3 );
    QGridLayout* grid = new QGridLayout( layout, 2, 2);

    m_thickness = new QSpinBox( 1, 16, 10, this );
    m_thickness->setValue( 1 );
    QLabel* tlabel = new QLabel( m_thickness, "T&hickness", this );

    grid->addWidget( tlabel, 0, 0 );
    grid->addWidget( m_thickness, 0, 1 );

    m_opacity = new QSpinBox( 0, 255, 10, this );
    m_opacity->setValue( 255 );
    QLabel* olabel = new QLabel( m_opacity, "&Opacity", this );

    grid->addWidget( olabel, 1, 0 );
    grid->addWidget( m_opacity, 1, 1 );

    QHBoxLayout* buttons = new QHBoxLayout( layout, 3 );
    buttons->addStretch( 3 );

    QPushButton *ok, *cancel, *save;
    ok = new QPushButton( "&OK", this );
    ok->setDefault( true );
    ok->setMinimumSize( ok->sizeHint() );
    connect( ok, SIGNAL(clicked()), SLOT(accept()) );
    buttons->addWidget( ok );

    cancel = new QPushButton( "&Cancel", this );
    cancel->setMinimumSize( cancel->sizeHint() );
    connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
    buttons->addWidget( cancel );

    save = new QPushButton( "&Save", this );
    save->setMinimumSize( save->sizeHint() );
    //connect( save, SIGNAL(clicked()), SLOT(reject()) );
    buttons->addWidget( save );
 
    resize( 1, 1 );
}

#include "opts_line_dlg.moc"
