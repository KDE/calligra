/*
 *  preferencesdlg.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
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

#include <klocale.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

#include "preferencesdlg.h"

PreferencesDialog::PreferencesDialog( QWidget* parent, const char* name, WFlags f )
	: KDialog( parent, name, true, f )
{
  setCaption( i18n( "Preferences" ) );

	// Layout
  QGridLayout* grid = new QGridLayout( this, 5, 5, 7, 15);

	// Inputline
	m_pLineEdit = new QLineEdit( this, "tempDir" );
  grid->addMultiCellWidget( m_pLineEdit, 1, 1, 0, 4 );	

	// Label
	QLabel* label = new QLabel( m_pLineEdit, i18n( "Directory for temporary files" ) , this );
	grid->addWidget( label, 0, 0 );
	
	// OK-Button
  QPushButton *okButton = new QPushButton( this, "OKButton" );
  okButton->setText( i18n( "&Ok" ) );
  okButton->setAutoRepeat( false );
  okButton->setAutoResize( false );
  okButton->setAutoDefault( true );
  okButton->setDefault( true );
  connect( okButton, SIGNAL( clicked() ), this, SLOT( close() ) );
  grid->addWidget( okButton, 3, 3 );
        
	// Cancel-Button
/*
  QPushButton *cancelButton = new QPushButton( this, "CancelButton" );
  cancelButton->setText( i18n( "&Cancel" ) );
  connect( cancelButton, SIGNAL( clicked() ), this, SLOT( close() ) );
  grid->addWidget( cancelButton, 3, 3 );
*/

  // layout
	grid->setRowStretch( 0, 0);
	grid->setRowStretch( 1, 0);
	grid->setColStretch( 0, 0);
	grid->setColStretch( 1, 0);
}

PreferencesDialog::~PreferencesDialog()
{
}

void PreferencesDialog::editPreferences()
{
  PreferencesDialog* dialog;

  dialog = new PreferencesDialog();
  if( dialog->exec() == Accepted )
  {
  }
}

#include "preferencesdlg.moc"






