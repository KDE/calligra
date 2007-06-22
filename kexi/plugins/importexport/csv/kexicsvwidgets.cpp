/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexicsvwidgets.h"

#include <qdir.h>
#include <qlabel.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3Frame>

#include <klocale.h>
#include <klineedit.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <kio/global.h>

#define KEXICSV_OTHER_DELIMITER_INDEX 4

KexiCSVDelimiterWidget::KexiCSVDelimiterWidget( bool lineEditOnBottom, QWidget * parent )
 : QWidget(parent)
 , m_availableDelimiters(KEXICSV_OTHER_DELIMITER_INDEX)
{
	Q3BoxLayout *lyr = 
		lineEditOnBottom ? 
		(Q3BoxLayout *)new Q3VBoxLayout( this, 0, KDialog::spacingHint() )
		: (Q3BoxLayout *)new Q3HBoxLayout( this, 0, KDialog::spacingHint() );

	m_availableDelimiters[0]=KEXICSV_DEFAULT_FILE_DELIMITER;
	m_availableDelimiters[1]=";";
	m_availableDelimiters[2]="\t";
	m_availableDelimiters[3]=" ";

	m_combo = new KComboBox(this);
	m_combo->setObjectName("KexiCSVDelimiterComboBox");
	m_combo->addItem( i18n( "Comma \",\"") ); //<-- KEXICSV_DEFAULT_FILE_DELIMITER
	m_combo->addItem( i18n( "Semicolon \";\"" ) );
	m_combo->addItem( i18n( "Tabulator" ) );
	m_combo->addItem( i18n( "Space \" \"" ) );
	m_combo->addItem( i18n( "Other" ) );
	lyr->addWidget(m_combo);
	setFocusProxy(m_combo);

	m_delimiterEdit = new KLineEdit(this);
	m_delimiterEdit->setObjectName("m_delimiterEdit");
//  m_delimiterEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_delimiterEdit->sizePolicy().hasHeightForWidth() ) );
	m_delimiterEdit->setMaximumSize( QSize( 30, 32767 ) );
	m_delimiterEdit->setMaxLength(1);
	lyr->addWidget( m_delimiterEdit );
	if (!lineEditOnBottom)
		lyr->addStretch(2);

	slotDelimiterChangedInternal(KEXICSV_DEFAULT_FILE_DELIMITER_INDEX); //this will init m_delimiter
	connect(m_combo, SIGNAL(activated(int)),
	  this, SLOT(slotDelimiterChanged(int)));
	connect(m_delimiterEdit, SIGNAL(returnPressed()),
	  this, SLOT(slotDelimiterLineEditReturnPressed()));
	connect(m_delimiterEdit, SIGNAL(textChanged( const QString & )),
	  this, SLOT(slotDelimiterLineEditTextChanged( const QString & ) ));
}

void KexiCSVDelimiterWidget::slotDelimiterChanged(int index)
{
	slotDelimiterChangedInternal(index);
	if (index==KEXICSV_OTHER_DELIMITER_INDEX)
		m_delimiterEdit->setFocus();
}

void KexiCSVDelimiterWidget::slotDelimiterChangedInternal(int index)
{
	bool changed = false;
	if (index > KEXICSV_OTHER_DELIMITER_INDEX)
		return;
	else if (index == KEXICSV_OTHER_DELIMITER_INDEX) {
		changed = m_delimiter != m_delimiterEdit->text();
		m_delimiter = m_delimiterEdit->text();
	}
	else {
		changed = m_delimiter != m_availableDelimiters[index];
		m_delimiter = m_availableDelimiters[index];
	}
	m_delimiterEdit->setEnabled(index == KEXICSV_OTHER_DELIMITER_INDEX);
	if (changed)
		emit delimiterChanged(m_delimiter);
}

void KexiCSVDelimiterWidget::slotDelimiterLineEditReturnPressed()
{
	if (m_combo->currentIndex() != KEXICSV_OTHER_DELIMITER_INDEX)
		return;
	slotDelimiterChangedInternal(KEXICSV_OTHER_DELIMITER_INDEX);
}

void KexiCSVDelimiterWidget::slotDelimiterLineEditTextChanged( const QString & )
{
	slotDelimiterChangedInternal(KEXICSV_OTHER_DELIMITER_INDEX);
}

void KexiCSVDelimiterWidget::setDelimiter(const QString& delimiter)
{
	QVector<QString>::ConstIterator it = m_availableDelimiters.constBegin();
	int index = 0;
	for (; it != m_availableDelimiters.constEnd(); ++it, index++) {
		if (*it == delimiter) {
			m_combo->setCurrentIndex(index);
			slotDelimiterChangedInternal(index);
			return;
		}
	}
	//else: set other (custom) delimiter
	m_delimiterEdit->setText(delimiter);
	m_combo->setCurrentIndex(KEXICSV_OTHER_DELIMITER_INDEX);
	slotDelimiterChangedInternal(KEXICSV_OTHER_DELIMITER_INDEX);
}

//----------------------------------------------------

KexiCSVTextQuoteComboBox::KexiCSVTextQuoteComboBox( QWidget * parent )
 : KComboBox(parent)
{
	addItem( "\"" );
	addItem( "'" );
	addItem( i18n( "None" ) );
}

QString KexiCSVTextQuoteComboBox::textQuote() const
{
	if (currentIndex()==2)
		return QString();
	return currentText();
}

void KexiCSVTextQuoteComboBox::setTextQuote(const QString& textQuote)
{
	if (textQuote=="\"" || textQuote=="'")
		setEditText(textQuote);
	else if (textQuote.isEmpty())
		setEditText(i18n( "None" ));
}

//----------------------------------------------------

KexiCSVInfoLabel::KexiCSVInfoLabel( const QString& labelText, QWidget* parent )
 : QWidget(parent)
{
	Q3VBoxLayout *vbox = new Q3VBoxLayout( this, 0, KDialog::spacingHint() );
	Q3HBoxLayout *hbox = new Q3HBoxLayout( this );
	vbox->addLayout(hbox);
	m_leftLabel = new QLabel(labelText, this);
	m_leftLabel->setMinimumWidth(130);
	m_leftLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	m_leftLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	m_leftLabel->setWordWrap(true);
	hbox->addWidget(m_leftLabel);
	m_iconLbl = new QLabel(this);
	m_iconLbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	m_iconLbl->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	m_fnameLbl = new QLabel(this);
	m_fnameLbl->setOpenExternalLinks(true);
	m_fnameLbl->setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard);
	m_fnameLbl->setFocusPolicy(Qt::NoFocus);
	m_fnameLbl->setTextFormat(Qt::PlainText);
	m_fnameLbl->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding,1,0));
	m_fnameLbl->setLineWidth(1);
	m_fnameLbl->setFrameStyle(Q3Frame::Box);
	m_fnameLbl->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	m_fnameLbl->setWordWrap(true);
	hbox->addSpacing(5);
	hbox->addWidget(m_iconLbl);
	hbox->addWidget(m_fnameLbl, 1, Qt::AlignVCenter | Qt::AlignLeft
#ifdef __GNUC__
#warning TODO | Qt::TextWordWrap
#else
#pragma WARNING( TODO | Qt::TextWordWrap )
#endif
	);
	hbox->addSpacing(10);
	m_commentLbl = new QLabel(this);
	m_commentLbl->setOpenExternalLinks(true);
	m_commentLbl->setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard);
	m_commentLbl->setFocusPolicy(Qt::NoFocus);
	m_commentLbl->setTextFormat(Qt::PlainText);
	m_commentLbl->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	m_commentLbl->setLineWidth(1);
	m_commentLbl->setFrameStyle(QFrame::Box);
	m_commentLbl->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	m_commentLbl->setWordWrap(true);
	hbox->addWidget(m_commentLbl, 0, Qt::AlignVCenter | Qt::AlignRight
#ifdef __GNUC__
#warning TODO | Qt::TextWordWrap
#else
#pragma WARNING( TODO | Qt::TextWordWrap )
#endif
	);

	m_separator = new Q3Frame(this);
	m_separator->setFrameShape(Q3Frame::HLine);
	m_separator->setFrameShadow(Q3Frame::Sunken);
	vbox->addWidget(m_separator);
}

void KexiCSVInfoLabel::setFileName( const QString& fileName )
{
	m_fnameLbl->setText( QDir::convertSeparators(fileName) );
	if (!fileName.isEmpty()) {
		m_iconLbl->setPixmap( 
			KIO::pixmapForUrl(KUrl(fileName), 0, K3Icon::Desktop) );
	}
}

void KexiCSVInfoLabel::setLabelText( const QString& text )
{
	m_fnameLbl->setText( text );
//	int lines = m_fnameLbl->lines();
//	m_fnameLbl->setFixedHeight( 
//		QFontMetrics(m_fnameLbl->currentFont()).height() * lines );
}

void KexiCSVInfoLabel::setIcon(const QString& iconName)
{
	m_iconLbl->setPixmap( DesktopIcon(iconName) );
}

void KexiCSVInfoLabel::setCommentText( const QString& text )
{
	m_commentLbl->setText(text);
}

//----------------------------------------------------

QStringList csvMimeTypes()
{
	QStringList mimetypes;
	mimetypes << "text/csv" << "text/plain" /*<< "all/allfiles"*/; // use application/octet-stream if you want all files, but then the others are not necessary
	return mimetypes;
}

#include "kexicsvwidgets.moc"
