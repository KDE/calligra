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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qlabel.h>
#include <qlayout.h>
#include <qstringlist.h>
#include <qtextedit.h>
#include <qtooltip.h>
#include <qvariant.h>
#include <qwhatsthis.h>

#include <kapplication.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kstdguiitem.h>
#include <kpushbutton.h>

#include "formulastring.h"
#include "kformula_view.h"


FormulaString::FormulaString( KFormulaPartView* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ), view( parent )
{
    if ( !name )
	setName( "FormulaString" );
    resize( 511, 282 );
    setCaption( i18n( "Formula String" ) );
    setSizeGripEnabled( TRUE );
    QVBoxLayout* FormulaStringLayout = new QVBoxLayout( this, 11, 6, "FormulaStringLayout");

    textWidget = new QTextEdit( this, "textWidget" );
    FormulaStringLayout->addWidget( textWidget );

    QHBoxLayout* Layout2 = new QHBoxLayout( 0, 0, 6, "Layout2");
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer );

    position = new QLabel( this, "position" );
    position->setText( trUtf8( "1:1" ) );
    Layout2->addWidget( position );
    FormulaStringLayout->addLayout( Layout2 );

    QHBoxLayout* Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1");

    buttonHelp = new KPushButton( KStdGuiItem::help(), this, "buttonHelp" );
    buttonHelp->setAccel( 4144 );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    buttonOk = new KPushButton( KStdGuiItem::ok(), this, "buttonOk" );
    buttonOk->setAccel( 0 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new KPushButton( KStdGuiItem::cancel(), this, "buttonCancel" );
    buttonCancel->setAccel( 0 );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );
    FormulaStringLayout->addLayout( Layout1 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonHelp, SIGNAL(clicked() ), this, SLOT( helpButtonClicked() ) );
    connect( textWidget, SIGNAL( cursorPositionChanged( int, int ) ),
             this, SLOT( cursorPositionChanged( int, int ) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
FormulaString::~FormulaString()
{
    // no need to delete child widgets, Qt does it all for us
}

void FormulaString::accept()
{
    QStringList errorList = view->readFormulaString( textWidget->text() );
    if ( errorList.count() == 0 ) {
        QDialog::accept();
    }
    else {
        KMessageBox::sorry( this, errorList.join( "\n" ), i18n( "Parser Error" ) );
    }
}

void FormulaString::helpButtonClicked()
{
  kapp->invokeHelp( "formula-strings", "kformula" );
}

void FormulaString::cursorPositionChanged( int para, int pos )
{
    position->setText( QString( "%1:%2" ).arg( para+1 ).arg( pos+1 ) );
}

#include "formulastring.moc"
