/* This file is part of the KDE project
  Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include "kexidbwidgets.h"

#include <qpainter.h>

#include <knumvalidator.h>
#include <kdatetbl.h>
#include <kdebug.h>

#include <kexiutils/utils.h>
#include <kexidb/field.h>

KexiDBLineEdit::KexiDBLineEdit(QWidget *parent, const char *name)
 : KLineEdit(parent, name)
 , KexiDBTextWidgetInterface()
 , KexiFormDataItemInterface()
// , m_autonumberDisplayParameters(0)
{
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotTextChanged(const QString&)));
}

KexiDBLineEdit::~KexiDBLineEdit()
{
//	delete m_autonumberDisplayParameters;
}

void KexiDBLineEdit::setInvalidState( const QString& displayText )
{
	setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & TabFocus)
		setFocusPolicy(QWidget::ClickFocus);
	setText(displayText);
}

void KexiDBLineEdit::setValueInternal(const QVariant& add, bool removeOld)
{
	if (m_field && m_field->type()==KexiDB::Field::Boolean) {
//! @todo temporary solution for booleans!
		setText( add.toBool() ? "1" : "0" );
	}
	else {
		if (removeOld)
			setText( add.toString() );
		else
			setText( m_origValue.toString() + add.toString() );
	}
}

QVariant KexiDBLineEdit::value()
{
	return text();
}

void KexiDBLineEdit::slotTextChanged(const QString&)
{
	signalValueChanged();
}

bool KexiDBLineEdit::valueIsNull()
{
	return text().isNull();
}

bool KexiDBLineEdit::valueIsEmpty()
{
	return text().isEmpty();
}

bool KexiDBLineEdit::isReadOnly() const
{
	return KLineEdit::isReadOnly();
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
	setText(QString::null);
}

void KexiDBLineEdit::setField(KexiDB::Field* field)
{
	KexiFormDataItemInterface::setField(field);
	if (!field)
		return;
//! @todo merge this code with KexiTableEdit code!
//! @todo set maximum length validator
//! @todo handle input mask (via QLineEdit::setInputMask()
	const KexiDB::Field::Type t = field->type();
	if (field->isIntegerType()) {
		QValidator *validator = 0;
		const bool u = field->isUnsigned();
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
	else if (field->isFPNumericType()) {
		QValidator *validator;
		if (t==KexiDB::Field::Float) {
			if (field->isUnsigned()) //ok?
				validator = new KDoubleValidator(0, 3.4e+38, field->scale(), this);
			else
				validator = new KDoubleValidator(this);
		}
		else {//double
			if (field->isUnsigned()) //ok?
				validator = new KDoubleValidator(0, 1.7e+308, field->scale(), this);
			else
				validator = new KDoubleValidator(this);
		}
		setValidator( validator );
	}
	else if (t==KexiDB::Field::Date) {
//! @todo use KDateWidget
		QValidator *validator = new KDateValidator(this);
		setValidator( validator );
	}
	else if (t==KexiDB::Field::Time) {
//! @todo use KTimeWidget
		setInputMask("00:00:00");
	}
	else if (t==KexiDB::Field::Boolean) {
//! @todo temporary solution for booleans!
		QValidator *validator = new KIntValidator(0, 1, this);
		setValidator( validator );
	}

	KexiDBTextWidgetInterface::setField(m_field, this);
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
	return ret;
}

//////////////////////////////////////////

void KexiDBTextWidgetInterface::setField(KexiDB::Field* field, QWidget *w)
{
	if (field->isAutoIncrement()) {
		if (!m_autonumberDisplayParameters)
			m_autonumberDisplayParameters = new KexiDisplayUtils::DisplayParameters();
		KexiDisplayUtils::initDisplayForAutonumberSign(*m_autonumberDisplayParameters, w);
	}
}

void KexiDBTextWidgetInterface::paintEvent( QFrame *w, bool textIsEmpty, int alignment, bool hasFocus  )
{
	KexiFormDataItemInterface *dataItemIface = dynamic_cast<KexiFormDataItemInterface*>(w);
	if (dataItemIface && dataItemIface->field() && dataItemIface->field()->isAutoIncrement()
		&& m_autonumberDisplayParameters && dataItemIface->cursorAtNewRow() && textIsEmpty)
	{
		QPainter p(w);
		if (w->hasFocus()) {
			p.setPen(KexiUtils::blendColors(m_autonumberDisplayParameters->textColor, w->palette().active().base(), 1, 3));
		}
		const int m = w->lineWidth()+w->midLineWidth();
		KexiDisplayUtils::drawAutonumberSign(*m_autonumberDisplayParameters, &p,
			2+m+w->margin(), m, w->width()-m*2 -2-2, w->height()-m*2 -2, alignment, hasFocus);
	}
}

void KexiDBTextWidgetInterface::event( QEvent * e, QWidget *w, bool textIsEmpty )
{
	if (e->type()==QEvent::FocusIn || e->type()==QEvent::FocusOut) {
		if (m_autonumberDisplayParameters && textIsEmpty)
			w->repaint();
	}
}

//////////////////////////////////////////

KexiDBTextEdit::KexiDBTextEdit(QWidget *parent, const char *name)
 : KTextEdit(parent, name)
 , KexiDBTextWidgetInterface()
 , KexiFormDataItemInterface()
{
	connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
}

KexiDBTextEdit::~KexiDBTextEdit()
{
}

void KexiDBTextEdit::setInvalidState( const QString& displayText )
{
	setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & TabFocus)
		setFocusPolicy(QWidget::ClickFocus);
	setText(displayText);
}

void KexiDBTextEdit::setValueInternal(const QVariant& add, bool removeOld)
{
	if (m_field && m_field->type()==KexiDB::Field::Boolean) {
//! @todo temporary solution for booleans!
		setText( add.toBool() ? "1" : "0" );
	}
	else {
		if (removeOld)
			setText( add.toString() );
		else
			setText( m_origValue.toString() + add.toString() );
	}
}

QVariant KexiDBTextEdit::value()
{
	return text();
}

void KexiDBTextEdit::slotTextChanged()
{
	signalValueChanged();
}

bool KexiDBTextEdit::valueIsNull()
{
	return text().isNull();
}

bool KexiDBTextEdit::valueIsEmpty()
{
	return text().isEmpty();
}

bool KexiDBTextEdit::isReadOnly() const
{
	return KTextEdit::isReadOnly();
}

QWidget* KexiDBTextEdit::widget()
{
	return this;
}

bool KexiDBTextEdit::cursorAtStart()
{
	int para, index;
	getCursorPosition ( &para, &index );
	return para==0 && index==0;
}

bool KexiDBTextEdit::cursorAtEnd()
{
	int para, index;
	getCursorPosition ( &para, &index );
	return (paragraphs()-1)==para && (paragraphLength(paragraphs()-1)-1)==index;
}

void KexiDBTextEdit::clear()
{
	setText(QString::null);
}

void KexiDBTextEdit::setField(KexiDB::Field* field)
{
	KexiFormDataItemInterface::setField(field);
	if (!field)
		return;
	KexiDBTextWidgetInterface::setField(m_field, this);
}

void KexiDBTextEdit::paintEvent ( QPaintEvent *pe )
{
	KTextEdit::paintEvent( pe );
	KexiDBTextWidgetInterface::paintEvent( this, text().isEmpty(), alignment(), hasFocus() );
}

//////////////////////////////////////////

KexiDBCheckBox::KexiDBCheckBox(const QString &text, QWidget *parent, const char *name)
 : QCheckBox(text, parent, name), KexiFormDataItemInterface()
{
	m_invalidState = false;
	setTristate(true);
	connect(this, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)));
}

KexiDBCheckBox::~KexiDBCheckBox()
{
}

void KexiDBCheckBox::setInvalidState( const QString& displayText )
{
	setEnabled(false);
	setState(NoChange);
	m_invalidState = true;
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & TabFocus)
		setFocusPolicy(QWidget::ClickFocus);
	setText(displayText);
}

void
KexiDBCheckBox::setEnabled(bool enabled)
{
	if(enabled && m_invalidState)
		return;
	QCheckBox::setEnabled(enabled);
}

void KexiDBCheckBox::setValueInternal(const QVariant &add, bool removeOld)
{
	setState( add.isNull() ? NoChange : (add.toBool() ? On : Off) );
}

QVariant
KexiDBCheckBox::value()
{
	return QVariant( isChecked(), 3 );
}

void KexiDBCheckBox::slotStateChanged(int state)
{
	signalValueChanged();
}

bool KexiDBCheckBox::valueIsNull()
{
	return state() == NoChange;
}

bool KexiDBCheckBox::valueIsEmpty()
{
	return false;
}

bool KexiDBCheckBox::isReadOnly() const
{
	return isEnabled();
}

QWidget*
KexiDBCheckBox::widget()
{
	return this;
}

bool KexiDBCheckBox::cursorAtStart()
{
	return false; //! \todo ?
}

bool KexiDBCheckBox::cursorAtEnd()
{
	return false; //! \todo ?
}

void KexiDBCheckBox::clear()
{
	setState(NoChange);
}

//////////////////////////////////////////

/*
KexiFormTableEdit::KexiFormTableEdit(QWidget *parent, const char *name)
 : QWidget(parent, name), KexiFormDataItemInterface()
{
	m_editor = 0;
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setAutoAdd(true);
}

KexiFormTableEdit::~KexiFormTableEdit()
{
}

void
KexiFormTableEdit::setTableEdit(KexiTableEdit *editor)
{
	m_editor = editor;
}

void KexiFormTableEdit::setInvalidState( const QString& displayText )
{
	if(m_editor)
		m_editor->setInvalidState(displayText);
}

void KexiFormTableEdit::setValueInternal(const QVariant &add, bool removeOld)
{
	if(m_editor)
		m_editor->setValueInternal(add, removeOld);
}

QVariant
KexiFormTableEdit::value()
{
	if(m_editor)
		return m_editor->value();
	else
		return QVariant();
}

bool KexiFormTableEdit::valueIsNull()
{
	if(m_editor)
		return m_editor->valueIsNull();
	else
		return false;
}

bool KexiFormTableEdit::valueIsEmpty()
{
	if(m_editor)
		return m_editor->valueIsEmpty();
	else
		return false;
}

bool KexiFormTableEdit::isReadOnly() const
{
	if(m_editor)
		return m_editor->isReadOnly();
	else
		return false;
}

QWidget*
KexiFormTableEdit::widget()
{
	if(m_editor)
		return m_editor->widget();
	else
		return this;
}

bool KexiFormTableEdit::cursorAtStart()
{
	if(m_editor)
		return m_editor->cursorAtStart();
	else
		return false;
}

bool KexiFormTableEdit::cursorAtEnd()
{
	if(m_editor)
		return m_editor->cursorAtEnd();
	else
		return false;
}

void KexiFormTableEdit::clear()
{
	if(m_editor)
		m_editor->clear();
}
*/
//////////////////////////////////////////

KexiPushButton::KexiPushButton( const QString & text, QWidget * parent, const char * name )
: KPushButton(text, parent, name)
{
}

KexiPushButton::~KexiPushButton()
{
}


#include "kexidbwidgets.moc"
