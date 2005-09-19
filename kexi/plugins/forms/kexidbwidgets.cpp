/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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
#include <qfontmetrics.h>
#include <qdatetime.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qstyle.h>
#include <qfiledialog.h>
#include <qclipboard.h>
#include <qtooltip.h>
#include <qimage.h>

#include <kapplication.h>
#include <knumvalidator.h>
#include <kdatetbl.h>
#include <kdatepicker.h>
#include <kpopupmenu.h>
#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kimageio.h>
#include <kfiledialog.h>
#include <kstandarddirs.h>
#include <kstaticdeleter.h>
#include <kimageeffect.h>

#include <kexiutils/utils.h>
#include <kexidb/field.h>
#include <kexidb/queryschema.h>

#ifdef Q_WS_WIN
#define KEXIDATETIMEEDITOR_P_IMPL
#include <win32_utils.h>
#include <krecentdirs.h>
#endif

#include <widget/tableview/kexidatetimeeditor_p.h>

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
	if (m_columnInfo && m_columnInfo->field->type()==KexiDB::Field::Boolean) {
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
	return ret;
}

//////////////////////////////////////////

void KexiDBTextWidgetInterface::setColumnInfo(KexiDB::QueryColumnInfo* cinfo, QWidget *w)
{
	if (cinfo->field->isAutoIncrement()) {
		if (!m_autonumberDisplayParameters)
			m_autonumberDisplayParameters = new KexiDisplayUtils::DisplayParameters();
		KexiDisplayUtils::initDisplayForAutonumberSign(*m_autonumberDisplayParameters, w);
	}
}

void KexiDBTextWidgetInterface::paintEvent( QFrame *w, bool textIsEmpty, int alignment, bool hasFocus  )
{
	KexiFormDataItemInterface *dataItemIface = dynamic_cast<KexiFormDataItemInterface*>(w);
	if (dataItemIface && dataItemIface->columnInfo() && dataItemIface->columnInfo()->field->isAutoIncrement()
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
	if (m_columnInfo && m_columnInfo->field->type()==KexiDB::Field::Boolean) {
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

void KexiDBTextEdit::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
	KexiFormDataItemInterface::setColumnInfo(cinfo);
	if (!cinfo)
		return;
	KexiDBTextWidgetInterface::setColumnInfo(m_columnInfo, this);
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
	setReadOnly(true);
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
	//! @todo: data/time edit API has no readonly flag, 
	//!        so use event filter to avoid changes made by keyboard or mouse when m_readOnly==true
	return m_readOnly; //!isEnabled();
}

void KexiDBTimeEdit::setReadOnly(bool set)
{
	m_readOnly = set;
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
	m_readOnly = false;

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
	setReadOnly(true);
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
	//! @todo: data/time edit API has no readonly flag, 
	//!        so use event filter to avoid changes made by keyboard or mouse when m_readOnly==true
	return m_readOnly; //!isEnabled();
}

void KexiDBDateEdit::setReadOnly(bool set)
{
	m_readOnly = set;
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
	m_readOnly = false;

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

void KexiDBDateTimeEdit::setInvalidState(const QString & /*! @todo paint this text: text*/)
{
	setEnabled(false);
	setReadOnly(true);
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
	//! @todo: data/time edit API has no readonly flag, 
	//!        so use event filter to avoid changes made by keyboard or mouse when m_readOnly==true
	return m_readOnly; //!isEnabled();
}

void KexiDBDateTimeEdit::setReadOnly(bool set)
{
	m_readOnly = set;
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
	setReadOnly(true);
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
	return editor()->isReadOnly();
}

void KexiDBIntSpinBox::setReadOnly(bool set)
{
	editor()->setReadOnly(set);
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
	setReadOnly(true);
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
	return editor()->isReadOnly();
}

void KexiDBDoubleSpinBox::setReadOnly(bool set)
{
	editor()->setReadOnly(set);
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

//////////////////////////////////////////

class KexiImageBox::Button : public QToolButton
{
	public:
		Button(QWidget *parent) : QToolButton(parent, "KexiImageBox::Button")
		{
			setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
			setFixedWidth(QMAX(15, qApp->globalStrut().width()));
			//	setFixedWidth(m_chooser->minimumSizeHint().width()); //! @todo get this from a KStyle
			setAutoRaise(true);
		}
		~Button() {}
		virtual void drawButton( QPainter *p ) {
			QToolButton::drawButton(p);
			QStyle::SFlags arrowFlags = QStyle::Style_Default;
			if (isDown())
				arrowFlags |= QStyle::Style_Down;
			if (isEnabled())
				arrowFlags |= QStyle::Style_Enabled;
			style().drawPrimitive(QStyle::PE_ArrowDown, p,
				QRect((width()-7)/2, height()-9, 7, 7), colorGroup(),
				arrowFlags, QStyleOption() );
		}
};

static KStaticDeleter<QPixmap> KexiImageBox_pmDeleter;
static QPixmap* KexiImageBox_pm = 0;

#if 0
class KexiImageBox::ImageLabel : public QLabel
{
	public:
		ImageLabel::ImageLabel(KexiImageBox *parent) : QLabel(parent, "KexiImageBox::Button")
//			WNoAutoErase)
		{
			setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			setBackgroundMode(Qt::NoBackground);
		}
		virtual void setPixmap ( const QPixmap &pixmap )
		{
			QLabel::setPixmap(pixmap);
			setBackgroundMode(pixmap.isNull() ? Qt::NoBackground : Qt::PaletteBackground);
		}
		virtual ~ImageLabel() {}
	protected:
		virtual void ImageLabel::drawContents ( QPainter *p )
		{
			if (static_cast<KexiImageBox*>(parentWidget())->designMode() && (!pixmap() || pixmap()->isNull()))
			{
				QPixmap pm(size());
				QPainter p2;
				p2.begin(&pm, this);
	//			QLabel::drawContents( p );
				p2.fillRect(0,0,width(),height(), parentWidget()->palette().active().background());

				updatePixmap();
				QImage img(KexiImageBox_pm->convertToImage());
				QColor c = parentWidget()->palette().active().background();
				img = KImageEffect::flatten(img, c.dark(150),
					qGray( c.rgb() ) <= 20 ? Qt::darkGray : c.light(105));
//				m_scalledDown = (pix.width() > (width()/2) || pix.height() > (height()/2));
//				if (m_scalledDown)
//					img = img.smoothScale(width()/2, height()/2, QImage::ScaleMin);
			
//				KexiImageBox_pmDeleter.setObject( KexiImageBox_pm, new QPixmap() );
//				KexiImageBox_pm->convertFromImage(img);

				QPixmap converted;
				converted.convertFromImage(img);
				p2.drawPixmap(2, height()-KexiImageBox_pm->height()-2, converted);
				QFont f(qApp->font());
				f.setPointSize(f.pointSize()*2);
				p2.setFont(f);
				p2.setPen( KexiUtils::contrastColor( parentWidget()->palette().active().background() ) );
				p2.drawText(pm.rect(), Qt::AlignCenter|Qt::WordBreak, i18n("No Image"));
				p2.end();
				bitBlt(this, 0, 0, &pm);
			}
			else
				QLabel::drawContents( p );
		}
		virtual void paletteChange ( const QPalette & oldPalette )
		{
			QLabel::paletteChange(oldPalette);
			if (oldPalette.active().background()!=palette().active().background()) {
				delete KexiImageBox_pm;
				KexiImageBox_pm = 0;
				repaint();
			}
		}
		void updatePixmap() {
			if (!static_cast<KexiImageBox*>(parentWidget())->designMode() && pixmap() && !pixmap()->isNull())
				return;

			if (KexiImageBox_pm) {
//				QSize size = KexiImageBox_pm->size();
				if ((KexiImageBox_pm->width() > (width()/2) || KexiImageBox_pm->height() > (height()/2))) {
//					int maxSize = QMAX(width()/2, height()/2);
//					size = QSize(maxSize,maxSize);
					delete KexiImageBox_pm;
					KexiImageBox_pm = 0;
				}
			}
			if (!KexiImageBox_pm) {
				QString fname( locate("data", QString("kexi/pics/imagebox.png")) );
				KexiImageBox_pmDeleter.setObject( KexiImageBox_pm, new QPixmap(fname, "PNG") );
			}
		}
		virtual void resizeEvent( QResizeEvent *e )
		{
			updatePixmap();
			QWidget::resizeEvent(e);
		}
//		bool m_scalledDown;
};
#endif

KexiImageBox::KexiImageBox( bool designMode, QWidget *parent, const char *name )
	: QWidget( parent, name, WNoAutoErase )
	, KexiFormDataItemInterface()
	, m_actionCollection(this)
	, m_alignment(Qt::AlignAuto|Qt::AlignTop)
	, m_designMode(designMode)
	, m_readOnly(false)
	, m_scaledContents(false)
	, m_keepAspectRatio(true)
{
	setBackgroundMode(Qt::NoBackground);

//	QHBoxLayout *hlyr = new QHBoxLayout(this, 2);
//	m_pixmapLabel = new ImageLabel(this);
//	hlyr->addWidget(m_pixmapLabel);

	if (m_designMode) {
		m_chooser = 0;
	}
	else {
		m_chooser = new Button(this);
//		hlyr->addWidget(m_chooser);
	}
	//setup popup menu
	m_popup = new KPopupMenu(this);
	QString titleString = i18n("Image Box");
	m_titleID = m_popup->insertTitle(SmallIcon("pixmaplabel"), titleString);
	m_insertFromFileAction = new KAction(i18n("Insert From &File..."), SmallIconSet("fileopen"), 0,
			this, SLOT(insertFromFile()), &m_actionCollection, "insert");
	m_insertFromFileAction->plug(m_popup);
	m_saveAsAction = KStdAction::saveAs(this, SLOT(saveAs()), &m_actionCollection);
//	m_saveAsAction->setText(i18n("&Save &As..."));
	m_saveAsAction->plug(m_popup);
	m_popup->insertSeparator();
	m_cutAction = KStdAction::cut(this, SLOT(cut()), &m_actionCollection);
	m_cutAction->plug(m_popup);
	m_copyAction = KStdAction::copy(this, SLOT(copy()), &m_actionCollection);
	m_copyAction->plug(m_popup);
	m_pasteAction = KStdAction::paste(this, SLOT(paste()), &m_actionCollection);
	m_pasteAction->plug(m_popup);
	m_deleteAction = new KAction(i18n("&Clear"), SmallIconSet("editdelete"), Qt::Key_Delete,
		this, SLOT(clear()), &m_actionCollection, "delete");
	m_deleteAction->plug(m_popup);
	m_popup->insertSeparator();
	m_propertiesAction = new KAction(i18n("Properties"), 0, 0,
		this, SLOT(showProperties()), &m_actionCollection, "properties");
	m_propertiesAction->plug(m_popup);
	connect(m_popup, SIGNAL(aboutToShow()), this, SLOT(updateActionsAvailability()));
	connect( m_popup, SIGNAL(aboutToHide()), this, SLOT(slotAboutToHidePopupMenu()));
	if (m_chooser) {
		//we couldn't use m_chooser->setPopup() because of drawing problems
		connect(m_chooser, SIGNAL(pressed()), this, SLOT(slotChooserPressed()));
	}

	updateActionStrings();

//	m_chooser->setPopupDelay(0);
//	m_chooser->setPopup(m_popup);
}

KexiImageBox::~KexiImageBox()
{
}

void KexiImageBox::setValueInternal( const QVariant& add, bool /* irrelevant here: removeOld*/ )
{
	if (isReadOnly())
		return;
	m_pixmap = add.toPixmap();
	repaint();
	emit valueChanged(m_pixmap);
}

QVariant KexiImageBox::value()
{
	return m_pixmap;
//	if (!m_pixmapLabel->pixmap())
//		return QVariant();
//	return *m_pixmapLabel->pixmap();
}

void KexiImageBox::setInvalidState( const QString& displayText )
{
//	m_pixmapLabel->setPixmap(QPixmap());
	m_pixmap = QPixmap();
//TODO	m_pixmapLabel->setText( displayText );
	m_chooser->hide();
	setReadOnly(true);
}

bool KexiImageBox::valueIsNull()
{
	return m_pixmap.isNull();
//	return !m_pixmapLabel->pixmap() || m_pixmapLabel->pixmap()->isNull();
}

bool KexiImageBox::valueIsEmpty()
{
	return false;
}

bool KexiImageBox::isReadOnly() const
{
	return m_readOnly;
}

void KexiImageBox::setReadOnly(bool set)
{
	m_readOnly = set;
}

QPixmap KexiImageBox::pixmap() const
{
	return m_pixmap;
//	return m_pixmapLabel->pixmap() ? *m_pixmapLabel->pixmap() : QPixmap();
}

bool KexiImageBox::hasScaledContents() const
{
	return m_scaledContents;
//	return m_pixmapLabel->hasScaledContents();
}

void KexiImageBox::setPixmap(const QPixmap& pixmap)
{
	setValueInternal(pixmap, true);
//	setBackgroundMode(pixmap.isNull() ? Qt::NoBackground : Qt::PaletteBackground);
}

void KexiImageBox::setScaledContents(bool set)
{
//todo	m_pixmapLabel->setScaledContents(set);
	m_scaledContents = set;
	repaint();
}

void KexiImageBox::setKeepAspectRatio(bool set)
{
	m_keepAspectRatio = set;
	if (m_scaledContents)
		repaint();
}

QWidget* KexiImageBox::widget()
{
	//! @todo
//	return m_pixmapLabel;
	return this;
}

bool KexiImageBox::cursorAtStart()
{
	return true;
}

bool KexiImageBox::cursorAtEnd()
{
	return true;
}

/*void KexiImageBox::clear()
{
	if (isReadOnly())
		return;
	m_pixmap = QPixmap();
	repaint();
//	m_pixmapLabel->setPixmap(QPixmap());
//	m_pixmapLabel->setText(QString::null);
	emit valueChanged(QPixmap());
}*/

void KexiImageBox::insertFromFile()
{
	if (isReadOnly())
		return;

#ifdef Q_WS_WIN
	QString recentDir;
	QString fileName = QFileDialog::getOpenFileName(
		KFileDialog::getStartURL(":lastVisitedImagePath", recentDir).path(), 
		convertKFileDialogFilterToQFileDialogFilter(KImageIO::pattern(KImageIO::Reading)), 
		this, 0, i18n("Insert Image From File"));
#else
	KURL url( KFileDialog::getImageOpenURL(
		":lastVisitedImagePath", this, i18n("Insert Image From File")) );
	QString fileName = url.isLocalFile() ? url.path() : url.prettyURL();

	//! @todo download the file if remote, then set fileName properly
#endif
	if (fileName.isEmpty())
		return;
	kexipluginsdbg << "fname=" << fileName << endl;
	KPixmap pm;
	if (!pm.load(fileName)) {
		//! @todo err msg
		kexipluginswarn << "KexiImageBox::insertFromFile(): err. loading pixmap" << endl;
		return;
	}
	setValueInternal(pm, true);

	//! @todo emit signal for setting "dirty" flag within the design

#ifdef Q_WS_WIN
	//save last visited path
	KURL url(fileName);
	if (url.isLocalFile())
		KRecentDirs::add(":lastVisitedImagePath", url.directory());
#endif
}

void KexiImageBox::saveAs()
{
//	if (!m_pixmapLabel->pixmap() || m_pixmapLabel->pixmap()->isNull()) {
	if (m_pixmap.isNull()) {
		kdWarning() << "KexiImageBox::saveAs(): null pixmap!" << endl;
		return;
	}
#ifdef Q_WS_WIN
	QString recentDir;
	QString fileName = QFileDialog::getSaveFileName(
		KFileDialog::getStartURL(":lastVisitedImagePath", recentDir).path(), 
		convertKFileDialogFilterToQFileDialogFilter(KImageIO::pattern(KImageIO::Writing)), 
		this, 0, i18n("Save Image To File"));
#else
	QString fileName = KFileDialog::getSaveFileName(
		":lastVisitedImagePath", KImageIO::pattern(KImageIO::Writing), this, i18n("Save Image To File"));
#endif
	if (fileName.isEmpty())
		return;
	kexipluginsdbg << fileName << endl;
//	if (!m_pixmapLabel->pixmap()->save(fileName, KImageIO::type(fileName))) {
	if (!m_pixmap.save(fileName, KImageIO::type(fileName).latin1())) {
		//! @todo err msg
		return;
	}

#ifdef Q_WS_WIN
	//save last visited path
	KURL url(fileName);
	if (url.isLocalFile())
		KRecentDirs::add(":lastVisitedImagePath", url.directory());
#endif
}

void KexiImageBox::cut()
{
	if (isReadOnly())
		return;
	copy();
	clear();
}

void KexiImageBox::copy()
{
//	if (m_pixmapLabel->pixmap())
	qApp->clipboard()->setPixmap(m_pixmap, QClipboard::Clipboard);
}

void KexiImageBox::paste()
{
	if (isReadOnly())
		return;
	QPixmap pm( qApp->clipboard()->pixmap(QClipboard::Clipboard) );
	if (!pm.isNull())
		setValueInternal(pm, true);
}

void KexiImageBox::clear()
{
	if (isReadOnly())
		return;
	setValueInternal(QPixmap(), true);

	//! @todo emit signal for setting "dirty" flag within the design
}

void KexiImageBox::showProperties()
{
	//! @todo
}

void KexiImageBox::updateActionsAvailability()
{
	const bool notNull = !valueIsNull();
	m_insertFromFileAction->setEnabled( !isReadOnly() );
	m_saveAsAction->setEnabled( notNull );
	m_cutAction->setEnabled( notNull && !isReadOnly() );
	m_copyAction->setEnabled( notNull );
	m_pasteAction->setEnabled( !isReadOnly() );
	m_deleteAction->setEnabled( notNull && !isReadOnly() );
	m_propertiesAction->setEnabled( notNull );
}

void KexiImageBox::slotAboutToHidePopupMenu()
{
	m_clickTimer.start(50, true);
}

void KexiImageBox::contextMenuEvent( QContextMenuEvent * e )
{
	m_popup->exec( e->globalPos(), -1 );
}

void KexiImageBox::slotChooserPressed()
{
	if (m_clickTimer.isActive())
		return;
	QRect screen = qApp->desktop()->availableGeometry( m_chooser );
	QPoint p;
	if ( QApplication::reverseLayout() ) {
		if ( mapToGlobal( m_chooser->rect().bottomLeft() ).y() + m_popup->sizeHint().height() <= screen.height() )
			p = m_chooser->mapToGlobal( m_chooser->rect().bottomRight() );
		else
			p = m_chooser->mapToGlobal( m_chooser->rect().topRight() - QPoint( 0, m_popup->sizeHint().height() ) );
		p.rx() -= m_popup->sizeHint().width();
	}
	else {
		if ( m_chooser->mapToGlobal( m_chooser->rect().bottomLeft() ).y() + m_popup->sizeHint().height() <= screen.height() )
			p = m_chooser->mapToGlobal( m_chooser->rect().bottomLeft() );
		else
			p = m_chooser->mapToGlobal( m_chooser->rect().topLeft() - QPoint( 0, m_popup->sizeHint().height() ) );
	}
	if (!m_popup->isVisible()) {
		m_popup->exec( p, -1 );
	}
	m_chooser->setDown( false );
}

void KexiImageBox::updateActionStrings()
{
	if (!m_popup)
		return;
	QString titleString = i18n("Image Box");
	if (!dataSource().isEmpty())
		titleString += (": " + dataSource());
	m_popup->changeTitle(m_titleID, m_popup->titlePixmap(m_titleID), titleString);

	if (m_chooser) {
		if (dataSource().isEmpty())
			QToolTip::add(m_chooser, i18n("Click to show actions for this Image Box"));
		else
			QToolTip::add(m_chooser, i18n("Click to show actions for \"%1\" Image Box").arg(dataSource()));
	}
}

void KexiImageBox::setDataSource( const QString &ds )
{
	KexiFormDataItemInterface::setDataSource( ds );
	updateActionStrings();
}

QSize KexiImageBox::sizeHint() const
{
	if (m_pixmap.isNull())
		return QSize(80, 80);
	return m_pixmap.size();
}

//void KexiImageBox::drawContents( QPainter *p )
void KexiImageBox::paintEvent( QPaintEvent*pe )
{
	QPainter p(this);
	p.setClipRect(pe->rect());
	const int m = 0; //todo margin();
//	QPainter ptr(this);
//	ptr.fillRect(0,0,width(),height(), green);
//	p->setClipRect(0, 0, width(), height());
//	QFrame::drawContents( p );
//	QFrame::drawFrame( p );
//	QColor bg(palette().active().background());//parentWidget()->palette().active().background()
	QColor bg(eraseColor());
	if (m_designMode && m_pixmap.isNull()) {
		QPixmap pm(size());
		QPainter p2;
		p2.begin(&pm, this);
//			QLabel::drawContents( p );
		p2.fillRect(0,0,width(),height(), bg);

		updatePixmap();
		QImage img(KexiImageBox_pm->convertToImage());
		img = KImageEffect::flatten(img, bg.dark(150),
			qGray( bg.rgb() ) <= 20 ? Qt::darkGray : bg.light(105));
//				m_scalledDown = (pix.width() > (width()/2) || pix.height() > (height()/2));
//				if (m_scalledDown)
//					img = img.smoothScale(width()/2, height()/2, QImage::ScaleMin);
	
//				KexiImageBox_pmDeleter.setObject( KexiImageBox_pm, new QPixmap() );
//				KexiImageBox_pm->convertFromImage(img);

		QPixmap converted;
		converted.convertFromImage(img);
		p2.drawPixmap(m+2, height()-m-KexiImageBox_pm->height()-2, converted);
		QFont f(qApp->font());
//		f.setPointSize(f.pointSize());
		p2.setFont(f);
		p2.setPen( KexiUtils::contrastColor( bg ) );
		p2.drawText(pm.rect(), Qt::AlignCenter|Qt::WordBreak, i18n("No Image"));
		p2.end();
		bitBlt(this, 0, 0, &pm);
	}
	else {
//		QFrame::drawContents( p );
		if (m_pixmap.isNull())
			p.fillRect(0,0,width(),height(), bg);
		else {
			const bool fast = m_pixmap.width()>1000 && m_pixmap.height()>800; //fast drawing needed
//! @todo we can optimize drawing by drawing rescaled pixmap here 
//! and performing detailed painting later (using QTimer)
			QPixmap pm;
			QPainter p2;
			QPainter *target;
			if (fast) {
				target = &p;
			}
			else {
				pm.resize(size());
				p2.begin(&pm, this);
				target = &p2;
			}
			//clearing needed here because we may need to draw a pixmap with transparency
			target->fillRect(0,0,width(),height(), bg);
			if (m_scaledContents) {
				if (m_keepAspectRatio) {
					QImage img(m_pixmap.convertToImage());
					img = img.smoothScale(width(), height(), QImage::ScaleMin);
					QPoint pos(0,0);
					if (img.width()<width()) {
						int hAlign = QApplication::horizontalAlignment( m_alignment );
						if ( hAlign & Qt::AlignRight )
							pos.setX(width()-img.width());
						else if ( hAlign & Qt::AlignHCenter )
							pos.setX(width()/2-img.width()/2);
					}
					else if (img.height()<height()) {
						if ( m_alignment & Qt::AlignBottom )
							pos.setY(height()-img.height());
						else if ( m_alignment & Qt::AlignVCenter )
							pos.setY(height()/2-img.height()/2);
					}
					QPixmap px;
					px.convertFromImage(img);
					target->drawPixmap(pos, px);
				}
				else {
					target->drawPixmap(QRect(m, m, width()-m*2, height()-m*2), m_pixmap);
				}
			}
			else {
				int hAlign = QApplication::horizontalAlignment( m_alignment );
				QPoint pos;
				if ( hAlign & Qt::AlignRight )
					pos.setX(width()-m_pixmap.width()-m);
				else if ( hAlign & Qt::AlignHCenter )
					pos.setX(width()/2-m_pixmap.width()/2);
				else //left, etc.
					pos.setX(m);

				if ( m_alignment & Qt::AlignBottom )
					pos.setY(height()-m_pixmap.height()-m);
				else if ( m_alignment & Qt::AlignVCenter )
					pos.setY(height()/2-m_pixmap.height()/2);
				else //top, etc. 
					pos.setY(m);
				target->drawPixmap(pos, m_pixmap);
			}
			if (!fast) {
				p2.end();
				bitBlt(this, 0, 0, &pm);
			}
		}
	}
}
/*		virtual void KexiImageBox::paletteChange ( const QPalette & oldPalette )
{
	QFrame::paletteChange(oldPalette);
	if (oldPalette.active().background()!=palette().active().background()) {
		delete KexiImageBox_pm;
		KexiImageBox_pm = 0;
		repaint();
	}
}*/
void KexiImageBox::updatePixmap() {
	if (! (m_designMode && m_pixmap.isNull()) )
		return;

//			if (KexiImageBox_pm) {
//				QSize size = KexiImageBox_pm->size();
//				if ((KexiImageBox_pm->width() > (width()/2) || KexiImageBox_pm->height() > (height()/2))) {
//					int maxSize = QMAX(width()/2, height()/2);
//					size = QSize(maxSize,maxSize);
//					delete KexiImageBox_pm;
//					KexiImageBox_pm = 0;
//				}
//			}
	if (!KexiImageBox_pm) {
		QString fname( locate("data", QString("kexi/pics/imagebox.png")) );
		KexiImageBox_pmDeleter.setObject( KexiImageBox_pm, new QPixmap(fname, "PNG") );
	}
}

void KexiImageBox::setAlignment(int alignment)
{
	m_alignment = alignment;
	if (!m_scaledContents || m_keepAspectRatio)
		repaint();
}

//		virtual void resizeEvent( QResizeEvent *e )
//		{
//			updatePixmap();
//			QWidget::resizeEvent(e);
//		}

/*void KexiImageBox::paintEvent( QPaintEvent* )
{
}
*/
/*
bool KexiImageBox::setProperty( const char * name, const QVariant & value )
{
	const bool ret = QLabel::setProperty(name, value);
	if (p_shadowEnabled) {
		if (0==qstrcmp("indent", name) || 0==qstrcmp("font", name) || 0==qstrcmp("margin", name)
			|| 0==qstrcmp("frameShadow", name) || 0==qstrcmp("frameShape", name)
			|| 0==qstrcmp("frameStyle", name) || 0==qstrcmp("midLineWidth", name)
			|| 0==qstrcmp("lineWidth", name)) {
			p_privateLabel->setProperty(name, value);
			updatePixmap();
		}
	}
	return ret;
}

void KexiImageBox::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
	KexiFormDataItemInterface::setColumnInfo(cinfo);
	KexiDBTextWidgetInterface::setColumnInfo(cinfo, this);
}*/

#include "kexidbwidgets.moc"
