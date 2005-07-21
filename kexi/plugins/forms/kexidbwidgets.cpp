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
#include "kexidatetimeeditor_p.h"

#include <qpainter.h>
#include <qfontmetrics.h>
#include <qdatetime.h>
#include <qlayout.h>
#include <qtoolbutton.h>

#include <knumvalidator.h>
#include <kdatetbl.h>
#include <kdatepicker.h>
#include <kpopupmenu.h>
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

void KexiDBCheckBox::setValueInternal(const QVariant &add, bool )
{
	setState( add.isNull() ? NoChange : (add.toBool() ? On : Off) );
}

QVariant
KexiDBCheckBox::value()
{
	return QVariant( isChecked(), 3 );
}

void KexiDBCheckBox::slotStateChanged(int )
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
	return !isEnabled();
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

KexiDBTimeEdit::KexiDBTimeEdit(const QTime &time, QWidget *parent, const char *name)
 : QTimeEdit(time, parent, name), KexiFormDataItemInterface()
{
	m_invalidState = false;
	setAutoAdvance(true);
	m_cleared = false;

#ifdef QDateTimeEditor_HACK
	m_dte_time = KexiUtils::findFirstChild<QDateTimeEditor>(this, "QDateTimeEditor");
#else
	m_dte_time = 0;
#endif

	connect(this, SIGNAL(valueChanged(const QTime&)), this, SLOT(slotValueChanged(const QTime&)));
}

KexiDBTimeEdit::~KexiDBTimeEdit()
{
}

void KexiDBTimeEdit::setInvalidState( const QString&)
{
	setEnabled(false);
	m_invalidState = true;
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & TabFocus)
		setFocusPolicy(QWidget::ClickFocus);
}

void
KexiDBTimeEdit::setEnabled(bool enabled)
{
	 // prevent the user from reenabling the widget when it is in invalid state
	if(enabled && m_invalidState)
		return;
	QTimeEdit::setEnabled(enabled);
}

void KexiDBTimeEdit::setValueInternal(const QVariant &add, bool removeOld)
{
	m_cleared = !m_origValue.isValid();

	int setNumberOnFocus = -1;
	QTime t;
	QString addString(add.toString());
	if (removeOld) {
		if (!addString.isEmpty() && addString[0].latin1()>='0' && addString[0].latin1() <='9') {
			setNumberOnFocus = addString[0].latin1()-'0';
			t = QTime(setNumberOnFocus, 0, 0);
		}
	}
	else
		t = m_origValue.toTime();

	setTime(t);
}

QVariant
KexiDBTimeEdit::value()
{
	//QDateTime - a hack needed because QVariant(QTime) has broken isNull()
	return QVariant(QDateTime( m_cleared ? QDate() : QDate(0,1,2)/*nevermind*/, time()));
}

bool KexiDBTimeEdit::valueIsNull()
{
	return !time().isValid() || time().isNull();
}

bool KexiDBTimeEdit::valueIsEmpty()
{
	return m_cleared;
}

bool KexiDBTimeEdit::isReadOnly() const
{
	return !isEnabled();
}

QWidget*
KexiDBTimeEdit::widget()
{
	return this;
}

bool KexiDBTimeEdit::cursorAtStart()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_time && hasFocus() && m_dte_time->focusSection()==0;
#else
	return false;
#endif
}

bool KexiDBTimeEdit::cursorAtEnd()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_time && hasFocus()
		&& m_dte_time->focusSection()==int(m_dte_time->sectionCount()-1);
#else
	return false;
#endif
}

void KexiDBTimeEdit::clear()
{
	setTime(QTime());
	m_cleared = true;
}

void
KexiDBTimeEdit::slotValueChanged(const QTime&)
{
	m_cleared = false;
}

//////////////////////////////////////////

KexiDBDateEdit::KexiDBDateEdit(const QDate &date, QWidget *parent, const char *name)
 : QWidget(parent, name), KexiFormDataItemInterface()
{
	m_invalidState = false;
	m_cleared = false;

	m_edit = new QDateEdit(date, this);
	m_edit->setAutoAdvance(true);
	m_edit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	connect( m_edit, SIGNAL(valueChanged(const QDate&)), this, SLOT(slotValueChanged(const QDate&)) );
	connect( m_edit, SIGNAL(valueChanged(const QDate&)), this, SIGNAL(dateChanged(const QDate&)) );

	QToolButton* btn = new QToolButton(this);
	btn->setText("...");
	btn->setFixedWidth( QFontMetrics(btn->font()).width(" ... ") );
	btn->setPopupDelay(1); //1 ms

#ifdef QDateTimeEditor_HACK
	m_dte_date = KexiUtils::findFirstChild<QDateTimeEditor>(m_edit, "QDateTimeEditor");
#else
	m_dte_date = 0;
#endif

	m_datePickerPopupMenu = new KPopupMenu(0, "date_popup");
	connect(m_datePickerPopupMenu, SIGNAL(aboutToShow()), this, SLOT(slotShowDatePicker()));
	m_datePicker = new KDatePicker(m_datePickerPopupMenu, QDate::currentDate(), 0);

	KDateTable *dt = KexiUtils::findFirstChild<KDateTable>(m_datePicker, "KDateTable");
	if (dt)
		connect(dt, SIGNAL(tableClicked()), this, SLOT(acceptDate()));
	m_datePicker->setCloseButton(true);
	m_datePicker->installEventFilter(this);
	m_datePickerPopupMenu->insertItem(m_datePicker);
	btn->setPopup(m_datePickerPopupMenu);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(m_edit, 1);
	layout->addWidget(btn, 0);

	setFocusProxy(m_edit);
}

KexiDBDateEdit::~KexiDBDateEdit()
{
}

void KexiDBDateEdit::setInvalidState( const QString& )
{
	setEnabled(false);
	m_invalidState = true;
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & TabFocus)
		setFocusPolicy(QWidget::ClickFocus);
}

void
KexiDBDateEdit::setEnabled(bool enabled)
{
	 // prevent the user from reenabling the widget when it is in invalid state
	if(enabled && m_invalidState)
		return;
	QWidget::setEnabled(enabled);
}

void KexiDBDateEdit::setValueInternal(const QVariant &add, bool removeOld)
{
	int setNumberOnFocus = -1;
	QDate d;
	QString addString(add.toString());
	if (removeOld) {
		if (!addString.isEmpty() && addString[0].latin1()>='0' && addString[0].latin1() <='9') {
			setNumberOnFocus = addString[0].latin1()-'0';
			d = QDate(setNumberOnFocus*1000, 1, 1);
		}
	}
	else
		d = m_origValue.toDate();

	m_edit->setDate(d);
}

QVariant
KexiDBDateEdit::value()
{
	return QVariant(m_edit->date());
}

bool KexiDBDateEdit::valueIsNull()
{
	return !m_edit->date().isValid() || m_edit->date().isNull();
}

bool KexiDBDateEdit::valueIsEmpty()
{
	return m_cleared;
}

bool KexiDBDateEdit::isReadOnly() const
{
	return !isEnabled();
}

QWidget*
KexiDBDateEdit::widget()
{
	return this;
}

bool KexiDBDateEdit::cursorAtStart()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_date && m_edit->hasFocus() && m_dte_date->focusSection()==0;
#else
	return false;
#endif
}

bool KexiDBDateEdit::cursorAtEnd()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_date && m_edit->hasFocus()
		&& m_dte_date->focusSection()==int(m_dte_date->sectionCount()-1);
#else
	return false;
#endif
}

void KexiDBDateEdit::clear()
{
	m_edit->setDate(QDate());
	m_cleared = true;
}

void
KexiDBDateEdit::slotValueChanged(const QDate&)
{
	m_cleared = false;
}

void
KexiDBDateEdit::slotShowDatePicker()
{
	QDate date = m_edit->date();

	m_datePicker->setDate(date);
	m_datePicker->setFocus();
	m_datePicker->show();
	m_datePicker->setFocus();
}

void
KexiDBDateEdit::acceptDate()
{
	m_edit->setDate(m_datePicker->date());
	m_datePickerPopupMenu->hide();
}

bool
KexiDBDateEdit::eventFilter(QObject *o, QEvent *e)
{
	if (o != m_datePicker)
		return false;

	switch (e->type()) {
		case QEvent::Hide:
			m_datePickerPopupMenu->hide();
			break;
		case QEvent::KeyPress:
		case QEvent::KeyRelease: {
			QKeyEvent *ke = (QKeyEvent *)e;
			if (ke->key()==Key_Enter || ke->key()==Key_Return) {
				//accepting picker
				acceptDate();
				return true;
			}
			else if (ke->key()==Key_Escape) {
				//cancelling picker
				m_datePickerPopupMenu->hide();
				return true;
			}
			else
				 m_datePickerPopupMenu->setFocus();
			break;
		}
		default:
			break;
	}
	return false;
}

//////////////////////////////////////////

KexiDBDateTimeEdit::KexiDBDateTimeEdit(const QDateTime &datetime, QWidget *parent, const char *name)
 : QWidget(parent, name), KexiFormDataItemInterface()
{
	m_invalidState = false;
	m_cleared = false;

	m_dateEdit = new QDateEdit(datetime.date(), this);
	m_dateEdit->setAutoAdvance(true);
	m_dateEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
//	m_dateEdit->setFixedWidth( QFontMetrics(m_dateEdit->font()).width("8888-88-88___") );
	connect(m_dateEdit, SIGNAL(valueChanged(const QDate&)), this, SLOT(slotValueChanged()));
	connect(m_dateEdit, SIGNAL(valueChanged(const QDate&)), this, SIGNAL(dateTimeChanged()));

	QToolButton* btn = new QToolButton(this);
	btn->setText("...");
	btn->setFixedWidth( QFontMetrics(btn->font()).width(" ... ") );
	btn->setPopupDelay(1); //1 ms

	m_timeEdit = new QTimeEdit(datetime.time(), this);;
	m_timeEdit->setAutoAdvance(true);
	m_timeEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	connect(m_timeEdit, SIGNAL(valueChanged(const QTime&)), this, SLOT(slotValueChanged()));
	connect(m_timeEdit, SIGNAL(valueChanged(const QTime&)), this, SIGNAL(dateTimeChanged()));

#ifdef QDateTimeEditor_HACK
	m_dte_date = KexiUtils::findFirstChild<QDateTimeEditor>(m_dateEdit, "QDateTimeEditor");
	m_dte_time = KexiUtils::findFirstChild<QDateTimeEditor>(m_timeEdit, "QDateTimeEditor");
#else
	m_dte_date = 0;
#endif

	m_datePickerPopupMenu = new KPopupMenu(0, "date_popup");
	connect(m_datePickerPopupMenu, SIGNAL(aboutToShow()), this, SLOT(slotShowDatePicker()));
	m_datePicker = new KDatePicker(m_datePickerPopupMenu, QDate::currentDate(), 0);

	KDateTable *dt = KexiUtils::findFirstChild<KDateTable>(m_datePicker, "KDateTable");
	if (dt)
		connect(dt, SIGNAL(tableClicked()), this, SLOT(acceptDate()));
	m_datePicker->setCloseButton(true);
	m_datePicker->installEventFilter(this);
	m_datePickerPopupMenu->insertItem(m_datePicker);
	btn->setPopup(m_datePickerPopupMenu);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(m_dateEdit, 0);
	layout->addWidget(btn, 0);
	layout->addWidget(m_timeEdit, 0);
	//layout->addStretch(1);

	setFocusProxy(m_dateEdit);
}

KexiDBDateTimeEdit::~KexiDBDateTimeEdit()
{
}

void KexiDBDateTimeEdit::setInvalidState(const QString &text)
{
	setEnabled(false);
	m_invalidState = true;
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & TabFocus)
		setFocusPolicy(QWidget::ClickFocus);
}

void
KexiDBDateTimeEdit::setEnabled(bool enabled)
{
	 // prevent the user from reenabling the widget when it is in invalid state
	if(enabled && m_invalidState)
		return;
	QWidget::setEnabled(enabled);
}

void KexiDBDateTimeEdit::setValueInternal(const QVariant &, bool )
{
	m_dateEdit->setDate(m_origValue.toDate());
	m_timeEdit->setTime(m_origValue.toTime());
}

QVariant
KexiDBDateTimeEdit::value()
{
	return QDateTime(m_dateEdit->date(), m_timeEdit->time());
}

bool KexiDBDateTimeEdit::valueIsNull()
{
	return !m_dateEdit->date().isValid() || m_dateEdit->date().isNull()
		|| !m_timeEdit->time().isValid() || m_timeEdit->time().isNull();
}

bool KexiDBDateTimeEdit::valueIsEmpty()
{
	return m_cleared;
}

bool KexiDBDateTimeEdit::isReadOnly() const
{
	return !isEnabled();
}

QWidget*
KexiDBDateTimeEdit::widget()
{
	return m_dateEdit;
}

bool KexiDBDateTimeEdit::cursorAtStart()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_date && m_dateEdit->hasFocus() && m_dte_date->focusSection()==0;
#else
	return false;
#endif
}

bool KexiDBDateTimeEdit::cursorAtEnd()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_time && m_timeEdit->hasFocus()
		&& m_dte_time->focusSection()==int(m_dte_time->sectionCount()-1);
#else
	return false;
#endif
}

void KexiDBDateTimeEdit::clear()
{
	m_dateEdit->setDate(QDate());
	m_timeEdit->setTime(QTime());
	m_cleared = true;
}

void
KexiDBDateTimeEdit::slotValueChanged()
{
	m_cleared = false;
}

void
KexiDBDateTimeEdit::slotShowDatePicker()
{
	QDate date = m_dateEdit->date();

	m_datePicker->setDate(date);
	m_datePicker->setFocus();
	m_datePicker->show();
	m_datePicker->setFocus();
}

void
KexiDBDateTimeEdit::acceptDate()
{
	m_dateEdit->setDate(m_datePicker->date());
	m_datePickerPopupMenu->hide();
}

bool
KexiDBDateTimeEdit::eventFilter(QObject *o, QEvent *e)
{
	if (o != m_datePicker)
		return false;

	switch (e->type()) {
		case QEvent::Hide:
			m_datePickerPopupMenu->hide();
			break;
		case QEvent::KeyPress:
		case QEvent::KeyRelease: {
			QKeyEvent *ke = (QKeyEvent *)e;
			if (ke->key()==Key_Enter || ke->key()==Key_Return) {
				//accepting picker
				acceptDate();
				return true;
			}
			else if (ke->key()==Key_Escape) {
				//cancelling picker
				m_datePickerPopupMenu->hide();
				return true;
			}
			else
				 m_datePickerPopupMenu->setFocus();
			break;
		}
		default:
			break;
	}
	return false;
}

QDateTime
KexiDBDateTimeEdit::dateTime() const
{
	return QDateTime(m_dateEdit->date(), m_timeEdit->time());
}

void
KexiDBDateTimeEdit::setDateTime(const QDateTime &dt)
{
	m_dateEdit->setDate(dt.date());
	m_timeEdit->setTime(dt.time());
}

//////////////////////////////////////////

KexiDBIntSpinBox::KexiDBIntSpinBox(QWidget *parent, const char *name)
 : KIntSpinBox(parent, name) , KexiFormDataItemInterface()
{
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
}

KexiDBIntSpinBox::~KexiDBIntSpinBox()
{
}

void KexiDBIntSpinBox::setInvalidState( const QString& displayText )
{
	m_invalidState = true;
	setEnabled(false);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & TabFocus)
		setFocusPolicy(QWidget::ClickFocus);
	setSpecialValueText(displayText);
	KIntSpinBox::setValue(minValue());
}

void
KexiDBIntSpinBox::setEnabled(bool enabled)
{
	 // prevent the user from reenabling the widget when it is in invalid state
	if(enabled && m_invalidState)
		return;
	KIntSpinBox::setEnabled(enabled);
}

void KexiDBIntSpinBox::setValueInternal(const QVariant&, bool)
{
	KIntSpinBox::setValue(m_origValue.toInt());
}

QVariant
KexiDBIntSpinBox::value()
{
	return KIntSpinBox::value();
}

void KexiDBIntSpinBox::slotValueChanged()
{
	signalValueChanged();
}

bool KexiDBIntSpinBox::valueIsNull()
{
	return cleanText().isEmpty();
}

bool KexiDBIntSpinBox::valueIsEmpty()
{
	return false;
}

bool KexiDBIntSpinBox::isReadOnly() const
{
	return !isEnabled();
}

QWidget*
KexiDBIntSpinBox::widget()
{
	return this;
}

bool KexiDBIntSpinBox::cursorAtStart()
{
	return false; //! \todo ?
}

bool KexiDBIntSpinBox::cursorAtEnd()
{
	return false; //! \todo ?
}

void KexiDBIntSpinBox::clear()
{
	KIntSpinBox::setValue(minValue()); //! \todo ?
}

//////////////////////////////////////////

KexiDBDoubleSpinBox::KexiDBDoubleSpinBox(QWidget *parent, const char *name)
 : KDoubleSpinBox(parent, name) , KexiFormDataItemInterface()
{
	connect(this, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged()));
}

KexiDBDoubleSpinBox::~KexiDBDoubleSpinBox()
{
}

void KexiDBDoubleSpinBox::setInvalidState( const QString& displayText )
{
	m_invalidState = true;
	setEnabled(false);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & TabFocus)
		setFocusPolicy(QWidget::ClickFocus);
	setSpecialValueText(displayText);
	KDoubleSpinBox::setValue(minValue());
}

void
KexiDBDoubleSpinBox::setEnabled(bool enabled)
{
	 // prevent the user from reenabling the widget when it is in invalid state
	if(enabled && m_invalidState)
		return;
	KDoubleSpinBox::setEnabled(enabled);
}

void KexiDBDoubleSpinBox::setValueInternal(const QVariant&, bool )
{
	KDoubleSpinBox::setValue(m_origValue.toDouble());
}

QVariant
KexiDBDoubleSpinBox::value()
{
	return KDoubleSpinBox::value();
}

void KexiDBDoubleSpinBox::slotValueChanged()
{
	signalValueChanged();
}

bool KexiDBDoubleSpinBox::valueIsNull()
{
	return cleanText().isEmpty();
}

bool KexiDBDoubleSpinBox::valueIsEmpty()
{
	return false;
}

bool KexiDBDoubleSpinBox::isReadOnly() const
{
	return !isEnabled();
}

QWidget*
KexiDBDoubleSpinBox::widget()
{
	return this;
}

bool KexiDBDoubleSpinBox::cursorAtStart()
{
	return false; //! \todo ?
}

bool KexiDBDoubleSpinBox::cursorAtEnd()
{
	return false; //! \todo ?
}

void KexiDBDoubleSpinBox::clear()
{
	KDoubleSpinBox::setValue(minValue());
}

//////////////////////////////////////////

KexiPushButton::KexiPushButton( const QString & text, QWidget * parent, const char * name )
: KPushButton(text, parent, name)
{
}

KexiPushButton::~KexiPushButton()
{
}


#include "kexidbwidgets.moc"
