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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidbdateedit.h"
#include <qlayout.h>
#include <qtoolbutton.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QKeyEvent>
#include <QEvent>
#include <kmenu.h>
#include <kdatepicker.h>
#include <kdatetable.h>

#include <kexiutils/utils.h>
#include <kexidb/queryschema.h>

KexiDBDateEdit::KexiDBDateEdit(const QDate &date, QWidget *parent, const char *name)
 : QWidget(parent, name), KexiFormDataItemInterface()
{
	m_invalidState = false;
	m_cleared = false;
	m_readOnly = false;

	m_edit = new Q3DateEdit(date, this);
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

	m_datePickerPopupMenu = new KMenu(0, "date_popup");
	connect(m_datePickerPopupMenu, SIGNAL(aboutToShow()), this, SLOT(slotShowDatePicker()));
	m_datePicker = new KDatePicker(m_datePickerPopupMenu, QDate::currentDate(), 0);

	KDateTable *dt = KexiUtils::findFirstChild<KDateTable>(m_datePicker, "KDateTable");
	if (dt)
		connect(dt, SIGNAL(tableClicked()), this, SLOT(acceptDate()));
	m_datePicker->setCloseButton(true);
	m_datePicker->installEventFilter(this);
	m_datePickerPopupMenu->insertItem(m_datePicker);
	btn->setPopup(m_datePickerPopupMenu);

	Q3HBoxLayout* layout = new Q3HBoxLayout(this);
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
	if (focusPolicy() & Qt::TabFocus)
		setFocusPolicy(Qt::ClickFocus);
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
			if (ke->key()==Qt::Key_Enter || ke->key()==Qt::Key_Return) {
				//accepting picker
				acceptDate();
				return true;
			}
			else if (ke->key()==Qt::Key_Escape) {
				//canceling picker
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

#include "kexidbdateedit.moc"
