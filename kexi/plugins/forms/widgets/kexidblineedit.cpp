/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidblineedit.h"
#include "kexidbautofield.h"

#include <kdebug.h>
#include <knumvalidator.h>
#include <kdatetable.h>

#include <qpopupmenu.h>

#include <kexiutils/utils.h>
#include <kexidb/queryschema.h>
#include <kexiutils/utils.h>
//Added by qt3to4:
#include <QEvent>
#include <QPaintEvent>

//! @todo reenable as an app aption
//#define USE_KLineEdit_setReadOnly

//! @internal A validator used for read only flag to disable editing
class KexiDBLineEdit_ReadOnlyValidator : public QValidator
{
	public:
		KexiDBLineEdit_ReadOnlyValidator( QObject * parent ) 
		 : QValidator(parent)
		{
		}
		~KexiDBLineEdit_ReadOnlyValidator() {}
		virtual State validate( QString &, int & ) const { return Invalid; }
};

//-----

KexiDBLineEdit::KexiDBLineEdit(QWidget *parent, const char *name)
 : KLineEdit(parent, name)
 , KexiDBTextWidgetInterface()
 , KexiFormDataItemInterface()
 , m_dateFormatter(0)
 , m_timeFormatter(0)
 , m_menuExtender(this, this)
 , m_internalReadOnly(false)
 , m_slotTextChanged_enabled(true)
{
#ifdef USE_KLineEdit_setReadOnly
//! @todo reenable as an app aption
	QPalette p(widget->palette());
	p.setColor( lighterGrayBackgroundColor(palette()) );
	widget->setPalette(p);
#endif

	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotTextChanged(const QString&)));
}

KexiDBLineEdit::~KexiDBLineEdit()
{
	delete m_dateFormatter;
	delete m_timeFormatter;
//	delete m_autonumberDisplayParameters;
}

void KexiDBLineEdit::setInvalidState( const QString& displayText )
{
	KLineEdit::setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & Qt::TabFocus)
		setFocusPolicy(Qt::ClickFocus);
	setText(displayText);
}

void KexiDBLineEdit::setValueInternal(const QVariant& add, bool removeOld)
{
	if (removeOld && m_columnInfo) {
		const KexiDB::Field::Type t = m_columnInfo->field->type();
		if (t == KexiDB::Field::Boolean) {
			//! @todo temporary solution for booleans!
			setText( add.toBool() ? "1" : "0" );
			return;
		}
		else if (t == KexiDB::Field::Date) {
			setText( dateFormatter()->dateToString( m_origValue.toDate() ) );
			setCursorPosition(0); //ok?
			return;
		}
		else if (t == KexiDB::Field::Time) {
//			if (removeOld) {
		//new time entering... just fill the line edit
//		QString add(add_.toString());
//		m_lineedit->setText(add);
//		m_lineedit->setCursorPosition(add.length());
			setText( 
				timeFormatter()->timeToString( 
					//hack to avoid converting null variant to valid QTime(0,0,0)
					m_origValue.isValid() ? m_origValue.toTime() : QTime(99,0,0) 
				)
			);
			setCursorPosition(0); //ok?
			return;
		}
		else if (t == KexiDB::Field::DateTime) {
			if (m_origValue.isValid()) {
				setText(
					dateFormatter()->dateToString( m_origValue.toDateTime().date() ) + " " +
					timeFormatter()->timeToString( m_origValue.toDateTime().time() )
				);
			}
			else {
				setText( QString::null );
			}
			setCursorPosition(0); //ok?
			return;
		}
	}
	
	m_slotTextChanged_enabled = false;

	if (removeOld)
		setText( add.toString() );
	else
		setText( m_origValue.toString() + add.toString() );
	setCursorPosition(0); //ok?

	m_slotTextChanged_enabled = true;
}

QVariant KexiDBLineEdit::value()
{
	const KexiDB::Field::Type t = m_columnInfo->field->type();
	switch (t) {
	case KexiDB::Field::Text:
	case KexiDB::Field::LongText:
		return text();
	case KexiDB::Field::Byte:
	case KexiDB::Field::ShortInteger:
		return text().toShort();
//! @todo uint, etc?
	case KexiDB::Field::Integer:
		return text().toInt();
	case KexiDB::Field::BigInteger:
		return text().toLongLong();
	case KexiDB::Field::Boolean:
		//! @todo temporary solution for booleans!
		return text() == "1" ? QVariant(true,1) : QVariant(false,0);
	case KexiDB::Field::Date:
		return dateFormatter()->stringToVariant( text() );
	case KexiDB::Field::Time:
		return timeFormatter()->stringToVariant( text() );
	case KexiDB::Field::DateTime:
		return stringToDateTime(*dateFormatter(), *timeFormatter(), text());
	case KexiDB::Field::Float:
		return text().toFloat();
	case KexiDB::Field::Double:
		return text().toDouble();
	default:
		return QVariant();
	}
//! @todo more data types!

	return text();
}

void KexiDBLineEdit::slotTextChanged(const QString&)
{
	if (!m_slotTextChanged_enabled)
		return;
	signalValueChanged();
}

bool KexiDBLineEdit::valueIsNull()
{
	return valueIsEmpty(); //ok??? text().isNull();
}

bool KexiDBLineEdit::valueIsEmpty()
{
	if (text().isEmpty())
		return true;

	if (m_columnInfo) {
		const KexiDB::Field::Type t = m_columnInfo->field->type();
		if (t == KexiDB::Field::Date)
			return dateFormatter()->isEmpty( text() );
		else if (t == KexiDB::Field::Time)
			return timeFormatter()->isEmpty( text() );
		else if (t == KexiDB::Field::Time)
			return dateTimeIsEmpty( *dateFormatter(), *timeFormatter(), text() );
	}

//! @todo
	return text().isEmpty();
}

bool KexiDBLineEdit::valueIsValid()
{
	if (!m_columnInfo)
		return true;
//! @todo fix for fields with "required" property = true
	if (valueIsEmpty()/*ok?*/)
		return true;

	const KexiDB::Field::Type t = m_columnInfo->field->type();
	if (t == KexiDB::Field::Date)
		return dateFormatter()->stringToVariant( text() ).isValid();
	else if (t == KexiDB::Field::Time)
		return timeFormatter()->stringToVariant( text() ).isValid();
	else if (t == KexiDB::Field::DateTime)
		return dateTimeIsValid( *dateFormatter(), *timeFormatter(), text() );

//! @todo
	return true;
}

bool KexiDBLineEdit::isReadOnly() const
{
	return m_internalReadOnly;
}

void KexiDBLineEdit::setReadOnly( bool readOnly )
{
#ifdef USE_KLineEdit_setReadOnly
//! @todo reenable as an app aption
	return KLineEdit::setReadOnly( readOnly );
#else
	m_internalReadOnly = readOnly;
	if (m_internalReadOnly) {
		m_readWriteValidator = validator();
		if (!m_readOnlyValidator)
		m_readOnlyValidator = new KexiDBLineEdit_ReadOnlyValidator(this);
		setValidator( m_readOnlyValidator );
	}
	else {
		//revert to r/w validator
		setValidator( m_readWriteValidator );
	}
	m_menuExtender.updatePopupMenuActions();
#endif
}

QPopupMenu * KexiDBLineEdit::createPopupMenu()
{
	QPopupMenu *contextMenu = KLineEdit::createPopupMenu();
	m_menuExtender.createTitle(contextMenu);
	return contextMenu;
}


QWidget* KexiDBLineEdit::widget()
{
	return this;
}

bool KexiDBLineEdit::cursorAtStart()
{
	return cursorPosition()==0;
}

bool KexiDBLineEdit::cursorAtEnd()
{
	return cursorPosition()==(int)text().length();
}

void KexiDBLineEdit::clear()
{
	if (!m_internalReadOnly)
		KLineEdit::clear();
}


void KexiDBLineEdit::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
	KexiFormDataItemInterface::setColumnInfo(cinfo);
	if (!cinfo)
		return;
//! @todo merge this code with KexiTableEdit code!
//! @todo set maximum length validator
//! @todo handle input mask (via QLineEdit::setInputMask()
	const KexiDB::Field::Type t = cinfo->field->type();
	if (cinfo->field->isIntegerType()) {
		QValidator *validator = 0;
		const bool u = cinfo->field->isUnsigned();
		int bottom, top;
		if (t==KexiDB::Field::Byte) {
			bottom = u ? 0 : -0x80;
			top = u ? 0xff : 0x7f;
		}
		else if (t==KexiDB::Field::ShortInteger) {
			bottom = u ? 0 : -0x8000;
			top = u ? 0xffff : 0x7fff;
		}
		else if (t==KexiDB::Field::Integer) {
			bottom = u ? 0 : -0x7fffffff-1;
			top = u ? 0xffffffff : 0x7fffffff;
		}
		else if (t==KexiDB::Field::BigInteger) {
/*! @todo couldn't work with KIntValidator: implement lonlong validator!
			bottom = u ? 0 : -0x7fffffffffffffff;
			top = u ? 0xffffffffffffffff : 127;*/
			validator = new KIntValidator(this);
		}

		if (!validator)
			validator = new KIntValidator(bottom, top, this);
		setValidator( validator );
	}
	else if (cinfo->field->isFPNumericType()) {
		QValidator *validator;
		if (t==KexiDB::Field::Float) {
			if (cinfo->field->isUnsigned()) //ok?
				validator = new KDoubleValidator(0, 3.4e+38, cinfo->field->scale(), this);
			else
				validator = new KDoubleValidator(this);
		}
		else {//double
			if (cinfo->field->isUnsigned()) //ok?
				validator = new KDoubleValidator(0, 1.7e+308, cinfo->field->scale(), this);
			else
				validator = new KDoubleValidator(this);
		}
		setValidator( validator );
	}
	else if (t==KexiDB::Field::Date) {
//! @todo use KDateWidget?
//		QValidator *validator = new KDateValidator(this);
//		setValidator( validator );
		setInputMask( dateFormatter()->inputMask() );
	}
	else if (t==KexiDB::Field::Time) {
//! @todo use KTimeWidget
//		setInputMask("00:00:00");
		setInputMask( timeFormatter()->inputMask() );
	}
	else if (t==KexiDB::Field::DateTime) {
		setInputMask( 
			dateTimeInputMask( *dateFormatter(), *timeFormatter() ) );
	}
	else if (t==KexiDB::Field::Boolean) {
//! @todo temporary solution for booleans!
		QValidator *validator = new KIntValidator(0, 1, this);
		setValidator( validator );
	}

	KexiDBTextWidgetInterface::setColumnInfo(cinfo, this);
}

void KexiDBLineEdit::paintEvent ( QPaintEvent *pe )
{
	KLineEdit::paintEvent( pe );
	KexiDBTextWidgetInterface::paintEvent( this, text().isEmpty(), alignment(), hasFocus() );
}

bool KexiDBLineEdit::event( QEvent * e )
{
	const bool ret = KLineEdit::event( e );
	KexiDBTextWidgetInterface::event(e, this, text().isEmpty());
	if (e->type()==QEvent::FocusOut) {
		QFocusEvent *fe = static_cast<QFocusEvent *>(e);
//		if (fe->reason()!=QFocusEvent::ActiveWindow && fe->reason()!=QFocusEvent::Popup) {
		if (fe->reason()==QFocusEvent::Tab || fe->reason()==QFocusEvent::Backtab) {
		//display aligned to left after loosing the focus (only if this is tab/backtab event)
//! @todo add option to set cursor at the beginning
			setCursorPosition(0); //ok?
		}
	}
	return ret;
}

bool KexiDBLineEdit::appendStretchRequired(KexiDBAutoField* autoField) const
{
	return KexiDBAutoField::Top == autoField->labelPosition();
}

void KexiDBLineEdit::handleAction(const QString& actionName)
{
	if (actionName=="edit_copy") {
		copy();
	}
	else if (actionName=="edit_paste") {
		paste();
	}
	else if (actionName=="edit_cut") {
		cut();
	}
	//! @todo ?
}

#include "kexidblineedit.moc"
