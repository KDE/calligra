/* This file is part of the KDE project
   Copyright (C) 2002 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include "formulastring.h"


FormulaString::FormulaString( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "FormulaString" );
    resize( 511, 282 );
    setCaption( trUtf8( "Formula String" ) );
    setSizeGripEnabled( TRUE );
    QVBoxLayout* FormulaStringLayout = new QVBoxLayout( this, 11, 6, "FormulaStringLayout");

    textWidget = new QTextEdit( this, "textWidget" );
    FormulaStringLayout->addWidget( textWidget );

    QHBoxLayout* Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1");

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setText( trUtf8( "Help" ) );
    buttonHelp->setAccel( 4144 );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( trUtf8( "OK" ) );
    buttonOk->setAccel( 0 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setText( trUtf8( "Cancel" ) );
    buttonCancel->setAccel( 0 );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );
    FormulaStringLayout->addLayout( Layout1 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
FormulaString::~FormulaString()
{
    // no need to delete child widgets, Qt does it all for us
}

#include "formulastring.moc"
