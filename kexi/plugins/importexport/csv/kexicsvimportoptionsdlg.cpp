/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kexicsvimportoptionsdlg.h"
#include <widget/kexicharencodingcombobox.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qtextcodec.h>
#include <qcheckbox.h>
//Added by qt3to4:
#include <Q3GridLayout>

#include <kapplication.h>
#include <kconfig.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kglobal.h>
#include <kcharsets.h>

KexiCSVImportOptionsDialog::KexiCSVImportOptionsDialog( const QString& selectedEncoding, QWidget* parent )
 : KDialogBase( 
	KDialogBase::Plain, 
	i18n( "CSV Import Options" ),
	Ok|Cancel, 
	Ok,
	parent, 
	"KexiCSVImportOptionsDialog", 
	true, 
	false
 )
{
	Q3GridLayout *lyr = new Q3GridLayout( plainPage(), 3, 3, 
		KDialogBase::marginHint(), KDialogBase::spacingHint());

	m_encodingComboBox = new KexiCharacterEncodingComboBox(plainPage(), selectedEncoding);
	lyr->addWidget( m_encodingComboBox, 0, 1 );

	QLabel* lbl = new QLabel( m_encodingComboBox, i18n("Text encoding:"), plainPage());
	lyr->addWidget( lbl, 0, 0 );

	lyr->addItem( new QSpacerItem( 20, 111, QSizePolicy::Minimum, QSizePolicy::Expanding ), 2, 1 );
	lyr->addItem( new QSpacerItem( 121, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 2 );

	m_chkAlwaysUseThisEncoding = new QCheckBox(
		i18n("Always use this encoding when importing CSV data files"), plainPage());
	lyr->addWidget( m_chkAlwaysUseThisEncoding, 1, 1 );

	//read config
	KGlobal::config()->setGroup("ImportExport");
	QString defaultEncodingForImportingCSVFiles 
		= KGlobal::config()->readEntry("DefaultEncodingForImportingCSVFiles");
	if (!defaultEncodingForImportingCSVFiles.isEmpty()) {
		m_encodingComboBox->setSelectedEncoding(defaultEncodingForImportingCSVFiles);
		m_chkAlwaysUseThisEncoding->setChecked(true);
	}

	adjustSize();
	m_encodingComboBox->setFocus();
}

KexiCSVImportOptionsDialog::~KexiCSVImportOptionsDialog()
{
}

KexiCharacterEncodingComboBox* KexiCSVImportOptionsDialog::encodingComboBox() const
{
	return m_encodingComboBox;
}

void KexiCSVImportOptionsDialog::accept()
{
	KGlobal::config()->setGroup("ImportExport");
	if (m_chkAlwaysUseThisEncoding->isChecked())
		KGlobal::config()->writeEntry("defaultEncodingForImportingCSVFiles", 
			m_encodingComboBox->selectedEncoding());
	else
		KGlobal::config()->deleteEntry("defaultEncodingForImportingCSVFiles");

	KDialogBase::accept();
}

#include "kexicsvimportoptionsdlg.moc"
