/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include "kinputdialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <klocale.h>

/*****************************************************************************
 *
 * KInputDialog
 *
 *****************************************************************************/

KInputDialog::KInputDialog(QWidget *parent, const char *name, const char *inputtitle, WFlags f )
	: KDialog( parent, name, TRUE, f )
{
	// Layout
    QGridLayout* grid = new QGridLayout( this, 4, 4, 15, 7 );

	// Inputline
	m_pLineEdit = new QLineEdit( this, "inputtext" );
	grid->addMultiCellWidget( m_pLineEdit, 1, 1, 0, 3 );
	
	// Label
	QLabel* label = new QLabel( m_pLineEdit, inputtitle, this );
	grid->addWidget( label, 0, 0 );

	// OK-Button
	QPushButton* buttonOK = new QPushButton( this, "ButtonOK" );
	buttonOK->setText( i18n( "OK" ) );
	buttonOK->setAutoRepeat(false);
	buttonOK->setAutoResize(false);
	buttonOK->setAutoDefault(true);
	buttonOK->setDefault(true);
	buttonOK->resize( buttonOK->sizeHint() );
	connect( buttonOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
	grid->addWidget( buttonOK, 3, 2 );
	
	// Cancel-Button
	QPushButton* buttonCancel = new QPushButton( this, "ButtonCancel" );
	buttonCancel->setText( i18n( "Cancel" ) );
	buttonCancel->resize( buttonOK->sizeHint() );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	grid->addWidget( buttonCancel, 3, 3 ); 

	// Dialog
	grid->setRowStretch( 0, 0 );
	grid->setRowStretch( 1, 0 );
	grid->setRowStretch( 2, 1 );
	grid->setRowStretch( 3, 0 );
	grid->setColStretch( 0, 0 );
	grid->setColStretch( 1, 1 );
	grid->setColStretch( 2, 0 );
	grid->setColStretch( 3, 0 );

	m_pLineEdit->setFocus();

	setMinimumSize( label->sizeHint().width() + m_pLineEdit->sizeHint().width() + buttonOK->sizeHint().width() + 30, 
	                label->sizeHint().height() + buttonOK->sizeHint().height() + buttonCancel->sizeHint().height() + 40);
}

KInputDialog::~KInputDialog()
{
}

void KInputDialog::setStr( const QString& theString )
{
  m_pLineEdit->setText( theString );
}

int KInputDialog::getStr( QString& theString)
{
  int result;
  QString tmp;

  setStr( theString );
  result = exec();
  if( result == Accepted )
  {
    theString = m_pLineEdit->text();
  }
  return result;
}

#include "kinputdialog.moc"
