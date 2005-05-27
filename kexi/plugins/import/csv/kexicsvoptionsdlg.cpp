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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include "kexicsvoptionsdlg.h"
#include "kexicsvoptions.h"

#include <qlayout.h>
#include <qtextcodec.h>

#include <kcombobox.h>
#include <klocale.h>
#include <kglobal.h>
#include <kcharsets.h>

KexiCSVOptionsDialog::KexiCSVOptionsDialog( const QString& selectedEncoding, QWidget* parent )
 : KDialogBase( 
	KDialogBase::Plain, 
	i18n( "CSV Import Options" ),
	Ok|Cancel, 
	Ok,
	parent, 
	"KexiCSVOptionsDialog", 
	true, 
	false
 )
 , m_defaultEncodingAdded(false)
{
	QVBoxLayout *lyr = new QVBoxLayout( plainPage(), 0, spacingHint() );

	m_widget = new KexiCSVOptionsWidget(plainPage(), "KexiCSVOptionsWidget");
	lyr->addWidget(m_widget);

	QString defaultEncoding(QString::fromLatin1(KGlobal::locale()->encoding()));
	QString defaultEncodingDescriptiveName;

	QStringList descEncodings(KGlobal::charsets()->descriptiveEncodingNames());
	QStringList::ConstIterator it = descEncodings.constBegin();
	for (uint id = 0; it!=descEncodings.constEnd(); ++it) {
		bool found = false;
		QString name( KGlobal::charsets()->encodingForName( *it ) );
		QTextCodec *codecForEnc = KGlobal::charsets()->codecForName(name, found);
		if (found) {
			m_widget->cbEncodings->insertItem(*it);
			if (codecForEnc->name() == defaultEncoding || name == defaultEncoding) {
				defaultEncodingDescriptiveName = *it;
			}
			if (codecForEnc->name() == selectedEncoding || name == selectedEncoding) {
				m_widget->cbEncodings->setCurrentItem(id);
			}
			id++;
		}
	}

	if (!defaultEncodingDescriptiveName.isEmpty()) {
		m_defaultEncodingAdded = true;
		m_widget->cbEncodings->insertItem( i18n("Text encoding: Default", "Default: %1")
			.arg(defaultEncodingDescriptiveName), 0 );
		if (selectedEncoding==defaultEncoding) {
			m_widget->cbEncodings->setCurrentItem(0);
		}
		else
			m_widget->cbEncodings->setCurrentItem(m_widget->cbEncodings->currentItem()+1);
	}

	adjustSize();
}

KexiCSVOptionsDialog::~KexiCSVOptionsDialog()
{
}

void KexiCSVOptionsDialog::accept()
{
	if (m_defaultEncodingAdded && 0==m_widget->cbEncodings->currentItem()) {
		m_selectedEncoding = QString::fromLatin1(KGlobal::locale()->encoding());
	}
	else {
		m_selectedEncoding = KGlobal::charsets()->encodingForName( m_widget->cbEncodings->currentText() );
	}

	KDialogBase::accept();
}

#include "kexicsvoptionsdlg.moc"
