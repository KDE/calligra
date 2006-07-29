/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include <kapplication.h>
#include <kconfig.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kglobal.h>
#include <kcharsets.h>

KexiCSVImportOptions::KexiCSVImportOptions()
{
	kapp->config()->setGroup("ImportExport");
	encoding = kapp->config()->readEntry("DefaultEncodingForImportingCSVFiles");
	if (encoding.isEmpty()) {
		encoding = QString::fromLatin1(KGlobal::locale()->encoding());
		defaultEncodingExplicitySet = false;
	}
	else
		defaultEncodingExplicitySet = true;

	stripWhiteSpaceInTextValuesChecked 
		= kapp->config()->readBoolEntry("StripBlanksOffOfTextValuesWhenImportingCSVFiles", true);
}

KexiCSVImportOptions::~KexiCSVImportOptions()
{
}

bool KexiCSVImportOptions::operator== ( const KexiCSVImportOptions & opt ) const
{
	return defaultEncodingExplicitySet==opt.defaultEncodingExplicitySet
		&& stripWhiteSpaceInTextValuesChecked==opt.stripWhiteSpaceInTextValuesChecked
		&& encoding==opt.encoding;
}

bool KexiCSVImportOptions::operator!= ( const KexiCSVImportOptions & opt ) const
{
	return !( *this==opt );
}

//----------------------------------

KexiCSVImportOptionsDialog::KexiCSVImportOptionsDialog( 
	const KexiCSVImportOptions& options, QWidget* parent )
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
	QGridLayout *lyr = new QGridLayout( plainPage(), 5, 3, 
		KDialogBase::marginHint(), KDialogBase::spacingHint());

	m_encodingComboBox = new KexiCharacterEncodingComboBox(plainPage(), options.encoding);
	lyr->addWidget( m_encodingComboBox, 0, 1 );

	QLabel* lbl = new QLabel( m_encodingComboBox, i18n("Text encoding:"), plainPage());
	lyr->addWidget( lbl, 0, 0 );

	lyr->addItem( new QSpacerItem( 20, KDialogBase::spacingHint(), QSizePolicy::Fixed, QSizePolicy::Fixed ), 2, 1 );
	lyr->addItem( new QSpacerItem( 121, KDialogBase::spacingHint(), QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 2 );

	m_chkAlwaysUseThisEncoding = new QCheckBox(
		i18n("Always use this encoding when importing CSV data files"), plainPage());
	lyr->addWidget( m_chkAlwaysUseThisEncoding, 1, 1 );

	m_chkStripWhiteSpaceInTextValues = new QCheckBox(
		i18n("Strip leading and trailing blanks off of text values"), plainPage());
	lyr->addWidget( m_chkStripWhiteSpaceInTextValues, 3, 1 );
	lyr->addItem( new QSpacerItem( 20, KDialogBase::spacingHint(), QSizePolicy::Minimum, QSizePolicy::Expanding ), 4, 1 );

	//update widgets
	if (options.defaultEncodingExplicitySet) {
		m_encodingComboBox->setSelectedEncoding(options.encoding);
		m_chkAlwaysUseThisEncoding->setChecked(true);
	}
	m_chkStripWhiteSpaceInTextValues->setChecked(options.stripWhiteSpaceInTextValuesChecked);

	adjustSize();
	m_encodingComboBox->setFocus();
}

KexiCSVImportOptionsDialog::~KexiCSVImportOptionsDialog()
{
}

KexiCSVImportOptions KexiCSVImportOptionsDialog::options() const
{
	KexiCSVImportOptions opt;
	opt.encoding = m_encodingComboBox->selectedEncoding();
	opt.stripWhiteSpaceInTextValuesChecked = m_chkStripWhiteSpaceInTextValues->isChecked();
	return opt;
}

void KexiCSVImportOptionsDialog::accept()
{
	kapp->config()->setGroup("ImportExport");
	if (m_chkAlwaysUseThisEncoding->isChecked())
		kapp->config()->writeEntry("DefaultEncodingForImportingCSVFiles", 
			m_encodingComboBox->selectedEncoding());
	else
		kapp->config()->deleteEntry("DefaultEncodingForImportingCSVFiles");

	kapp->config()->writeEntry("StripBlanksOffOfTextValuesWhenImportingCSVFiles", 
		m_chkStripWhiteSpaceInTextValues->isChecked());

	KDialogBase::accept();
}

#include "kexicsvimportoptionsdlg.moc"
