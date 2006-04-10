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

#include <QLabel>
#include <QStringList>
#include <QTextEdit>
#include <QToolTip>
#include <QVariant>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <q3whatsthis.h>

#include <kapplication.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kstdguiitem.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

#include "formulastring.h"
#include "kformula_view.h"


FormulaString::FormulaString( KFormulaPartView* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl ), view( parent )
{
    if ( !name )
	setName( "FormulaString" );
    resize( 511, 282 );
    setCaption( i18n( "Formula String" ) );
    setSizeGripEnabled( true );
    
    m_widgetLayout = new QVBoxLayout( this );
    m_buttonLayout = new QHBoxLayout( this );
    m_textEdit = new QTextEdit( this, "textEdit" );
    m_position = new QLabel( this, "position" );
    m_btnHelp = new KPushButton( KStdGuiItem::help(), this, "btnHelp" );
    m_btnOk = new KPushButton( KStdGuiItem::ok(), this, "btnOk" );
    m_btnCancel = new KPushButton( KStdGuiItem::cancel(), this, "btnCancel" );
    
    m_buttonLayout->addWidget( m_btnHelp );
    m_buttonLayout->addSpacing( 100 );
    m_buttonLayout->addWidget( m_btnOk );
    m_buttonLayout->addWidget( m_btnCancel );
    
    m_widgetLayout->addWidget( m_textEdit );
    m_widgetLayout->addWidget( m_position );
    m_widgetLayout->addLayout( m_buttonLayout );
    setLayout( m_widgetLayout );

    m_position->setText( trUtf8( "1:1" ) );
    m_btnHelp->setAccel( 4144 );
    m_btnHelp->setAutoDefault( true );
    m_btnOk->setAccel( 0 );
    m_btnOk->setAutoDefault( true );
    m_btnOk->setDefault( true );
    m_btnCancel->setAccel( 0 );
    m_btnCancel->setAutoDefault( true );

    // signals and slots connections
    connect( m_btnOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( m_btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( m_btnHelp, SIGNAL(clicked() ), this, SLOT( helpButtonClicked() ) );
    connect( m_textEdit, SIGNAL( cursorPositionChanged() ),
             this, SLOT( cursorPositionChanged() ) );
}

FormulaString::~FormulaString()
{
  delete m_textEdit;
  delete m_btnHelp;
  delete m_btnOk;
  delete m_btnCancel;
  delete m_position;
  delete m_buttonLayout;
  delete m_widgetLayout;
}

void FormulaString::accept()
{
  QStringList errorList = view->readFormulaString( m_textEdit->toPlainText() );
  if ( errorList.count() == 0 )
  {
    QDialog::accept();
  }
  else
  {
    KMessageBox::sorry( this, errorList.join( "\n" ), i18n( "Parser Error" ) );
  }
}

void FormulaString::helpButtonClicked()
{
    KToolInvocation::invokeHelp( "formula-strings", "kformula" );
}

void FormulaString::cursorPositionChanged()
{
//    m_textEdit->	
//    m_position->setText( QString( "%1:%2" ).arg( para+1 ).arg( pos+1 ) );
}

void FormulaString::setEditText( const QString& text )
{
    m_textEdit->setPlainText( text );
}

#include "formulastring.moc"
