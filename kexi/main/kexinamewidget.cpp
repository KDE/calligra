/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexinamewidget.h"

#include <qlabel.h>
#include <qlayout.h>

#include <klineedit.h>
#include <kmessagebox.h>
#include <klocale.h>

#include <core/kexi.h>

KexiNameWidget::KexiNameWidget( const QString& message, 
	QWidget* parent, const char* name, WFlags fl )
 : QWidget(parent, name, fl)
{
	init(message, QString::null, QString::null, QString::null, QString::null);
}

KexiNameWidget::KexiNameWidget(const QString& message, 
	const QString& nameLabel, const QString& nameText, 
	const QString& captionLabel, const QString& captionText, 
	QWidget * parent, const char * name, WFlags fl)
{
	init(message, nameLabel, nameText, captionLabel, captionText);
}

void KexiNameWidget::init(
	const QString& message, 
	const QString& nameLabel, const QString& nameText, 
	const QString& captionLabel, const QString& captionText)
{
	m_le_name_txtchanged_disable = false;
	m_le_name_autofill = true;

	lyr = new QGridLayout( this, 1, 1, 0, 6, "lyr");

	lbl_message = new QLabel( this, "message" );
	setMessageText( message );
	lbl_message->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	lbl_message->setAlignment( int( QLabel::AlignTop ) );
	lyr->addMultiCellWidget( lbl_message, 0, 0, 0, 1 );

	lbl_caption = new QLabel( captionLabel.isEmpty() ? i18n( "Caption:" ) : captionLabel,
		this, "lbl_caption" );
	lyr->addWidget( lbl_caption, 1, 0 );

	lbl_name = new QLabel( nameLabel.isEmpty() ? tr( "Name:" ) : nameLabel,
		this, "lbl_name" );
	lyr->addWidget( lbl_name, 2, 0 );

	le_caption = new KLineEdit( nameText, this, "le_caption" );
	le_caption->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred, 1, 0));
//	le_caption->installEventFilter(this);
	lyr->addWidget( le_caption, 1, 1 );

	le_name = new KLineEdit( nameText, this, "le_name" );
	le_name->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred,1,0));
	le_name->setValidator(m_validator=new Kexi::IdentifierValidator(this, "id_val"));
//	le_name->installEventFilter(this);
	lyr->addWidget( le_name, 2, 1 );

	setFocusProxy(le_caption);
	resize( QSize(342, 123).expandedTo(minimumSizeHint()) );

	m_nameWarning = i18n("Please enter the name.");
	m_captionWarning = i18n("Please enter the caption.");

	connect(le_caption, SIGNAL(textChanged(const QString&)),
		this,SLOT(slotCaptionTxtChanged(const QString&)));
	connect(le_name, SIGNAL(textChanged(const QString&)),
		this,SLOT(slotNameTxtChanged(const QString&)));
	connect(le_caption, SIGNAL(returnPressed()),
		this,SIGNAL(returnPressed()));
	connect(le_name, SIGNAL(returnPressed()),
		this,SIGNAL(returnPressed()));
}

KexiNameWidget::~KexiNameWidget()
{
}

void KexiNameWidget::slotCaptionTxtChanged(const QString &capt)
{
	emit textChanged();
	if (le_name->text().isEmpty())
		m_le_name_autofill=true;
	if (m_le_name_autofill) {
		m_le_name_txtchanged_disable = true;
		le_name->setText( Kexi::string2Identifier( capt ) );
		m_le_name_txtchanged_disable = false;
	}
}

void KexiNameWidget::slotNameTxtChanged(const QString &)
{
	emit textChanged();
	if (m_le_name_txtchanged_disable)
		return;
	m_le_name_autofill = false;
}

void KexiNameWidget::clear()
{
	le_name->clear();
	le_caption->clear();
}

bool KexiNameWidget::empty() const
{
	return le_name->text().isEmpty() || le_caption->text().stripWhiteSpace().isEmpty();
}

void KexiNameWidget::setAcceptsEmptyValue( bool set ) 
{ m_validator->setAcceptsEmptyValue(set); }

bool KexiNameWidget::acceptsEmptyValue() const 
{ return m_validator->acceptsEmptyValue(); }

void KexiNameWidget::setCaptionText(const QString& capt)
{
	le_caption->setText(capt);
	m_le_name_autofill = true;
}

void KexiNameWidget::setNameText(const QString& name)
{
	le_name->setText(name);
	m_le_name_autofill = true;
}

void KexiNameWidget::setMessageText(const QString& msg)
{
	lbl_message->setText(msg.stripWhiteSpace());
	if (lbl_message->text().isEmpty())
		lbl_message->hide();
	else
		lbl_message->show();
}

bool KexiNameWidget::checkValidity()
{
	if (acceptsEmptyValue())
		return true;

	if (le_name->text().isEmpty()) {
		KMessageBox::information(0, m_nameWarning);
		le_name->setFocus();
		return false;
	}
	if (le_caption->text().stripWhiteSpace().isEmpty()) {
		KMessageBox::information(0, m_captionWarning);
		le_caption->setFocus();
		return false;
	}
	return true;
}

/*bool KexiNameWidget::eventFilter( QObject *obj, QEvent *ev )
{
	if (ev->type()==QEvent::FocusOut && !acceptsEmptyValue()) {
		if (obj==le_name) {
			if (le_name->text().isEmpty()) {
				KMessageBox::information(0, m_nameWarning);
				le_name->setFocus();
				return true;
			}
		}
		else if (obj==le_caption) {
			if (le_caption->text().isEmpty()) {
				KMessageBox::information(0, m_captionWarning);
				le_caption->setFocus();
				return true;
			}
		}
	}
	return false;
}*/

#include "kexinamewidget.moc"

