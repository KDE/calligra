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

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <klocale.h>

#include "kis_dlg_preferences.h"

GeneralTab::GeneralTab( QWidget *_parent, const char *_name )
  : QWidget( _parent, _name )
{
	// Layout
  QGridLayout* grid = new QGridLayout( this, 2, 1, 7, 15);

  // checkbutton
  m_saveOnExit = new QCheckBox( i18n( "Save and restore dialog geometries" ), this );
  grid->addWidget( m_saveOnExit, 0, 0 );
}

bool GeneralTab::saveOnExit()
{
  return m_saveOnExit->isChecked();
}

DirectoriesTab::DirectoriesTab( QWidget *_parent, const char *_name )
  : QWidget( _parent, _name )
{
  QLabel* label;

	// Layout
  QGridLayout* grid = new QGridLayout( this, 4, 1, 7, 15);

	// Inputline
	m_pLineEdit = new QLineEdit( this, "tempDir" );
  grid->addWidget( m_pLineEdit, 1, 0 );	

	// Label
	label = new QLabel( m_pLineEdit, i18n( "Directory for temporary files" ) , this );
	grid->addWidget( label, 0, 0 );

	// Inputline
	m_pLineEdit = new QLineEdit( this, "gimpGradientDir" );
  grid->addWidget( m_pLineEdit, 3, 0 );	

	// Label
	label = new QLabel( m_pLineEdit, i18n( "Directory of GIMP gradients" ) , this );
	grid->addWidget( label, 2, 0 );
}

UndoRedoTab::UndoRedoTab( QWidget *_parent, const char *_name  )
  : QWidget( _parent, _name )
{
	// Layout
  QGridLayout* grid = new QGridLayout( this, 3, 1, 7, 15);

  QLabel *label;

  label = new QLabel( i18n( "Undo depth totally" ), this );
  grid->addWidget( label, 0, 0 );

  label = new QLabel( i18n( "Undo depth in memory" ), this );
  grid->addWidget( label, 1, 0 );

  grid->setRowStretch( 2, 1 );
}

PreferencesDialog::PreferencesDialog( QWidget* parent, const char* name, WFlags f )
	: QTabDialog( parent, name, true )
{
  setCaption( i18n( "Preferences" ) );

  m_general = new GeneralTab( this );
  addTab( m_general, i18n( "General") );

  m_directories = new DirectoriesTab( this );
  addTab( m_directories, i18n( "Directories") );

  m_undoRedo = new UndoRedoTab( this );
  addTab( m_undoRedo, i18n( "Undo/redo") );

  setOkButton();
  setCancelButton();
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

#include "kis_dlg_preferences.moc"







