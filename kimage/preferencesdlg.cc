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

#include <klocale.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

#include "preferencesdlg.h"

/*****************************************************************************
 *
 * KImagePreferencesDialog
 *
 *****************************************************************************/

KImagePreferencesDialog::KImagePreferencesDialog( QWidget* parent, const char* name, WFlags f )
	: KDialog( parent, name, true, f )
{
	// Layout
    QGridLayout* grid = new QGridLayout( this, 5, 5, 7, 15);

	// Inputline
	m_pLineEdit = new QLineEdit( this, "tempDir" );
    grid->addMultiCellWidget( m_pLineEdit, 1, 1, 0, 4 );	

	// Label
	QLabel* label = new QLabel( m_pLineEdit, i18n( "Directory for temporary files" ) , this );
	grid->addWidget( label, 0, 0 );
	
	// OK-Button
    QPushButton* okButton = new QPushButton( this, "OKButton" );
    okButton->setText( i18n( "&Ok" ) );
    okButton->setAutoRepeat( false );
    okButton->setAutoResize( false );
    okButton->setAutoDefault( true );
    okButton->setDefault( true );
    connect( okButton, SIGNAL( clicked() ), this, SLOT( close() ) );
    grid->addWidget( okButton, 3, 3 );
        
	// Cancel-Button

	grid->setRowStretch( 0, 0);
	grid->setRowStretch( 1, 0);
	grid->setColStretch( 0, 0);
	grid->setColStretch( 1, 0);
}

KImagePreferencesDialog::~KImagePreferencesDialog()
{
}

QString KImagePreferencesDialog::getStr()
{
	QString tmp;

	return tmp;
}

#include "preferencesdlg.moc"
