/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidatetimetableedit.h"
#include "kexidatetimeeditor_p.h"

#include <qapplication.h>
#include <qpainter.h>
#include <qvariant.h>
#include <qrect.h>
#include <qpalette.h>
#include <qcolor.h>
#include <qfontmetrics.h>
#include <qdatetime.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qdatetimeedit.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdatepicker.h>
#include <kdatetbl.h>
#include <klineedit.h>
#include <kpopupmenu.h>
#include <kdatewidget.h>

#include "kexi_utils.h"

KexiDateTimeTableEdit::KexiDateTimeTableEdit(KexiTableViewColumn &column, QScrollView *parent)
 : KexiTableEdit(column, parent,"KexiDateTimeTableEdit")
{
	m_sentEvent = false;
	setViewWidget( new QWidget(this) );
	m_dateEdit = new QDateEdit(widget());
	m_dateEdit->setAutoAdvance(true);
	m_dateEdit->installEventFilter(this);
//	m_dateEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
//TODO: width ok?
	m_dateEdit->setFixedWidth( QFontMetrics(m_dateEdit->font()).width("8888-88-88___") );

	QToolButton* dateBtn = new QToolButton(widget());
	dateBtn->setText("...");
	dateBtn->setFixedWidth( QFontMetrics(dateBtn->font()).width(" ... ") );
	dateBtn->setPopupDelay(1); //1 ms

	m_timeEdit = new QTimeEdit(widget());
	m_timeEdit->setAutoAdvance(true);
	m_timeEdit->installEventFilter(this);
//TODO: use QTimeEdit::display() for better width calculation
	m_timeEdit->setFixedWidth( QFontMetrics(m_dateEdit->font()).width("88:88:88___") );

	m_datePickerPopupMenu = new KPopupMenu(0, "date_popup");
	connect(m_datePickerPopupMenu, SIGNAL(aboutToShow()), this, SLOT(slotShowDatePicker()));
	m_datePicker = new KDatePicker(m_datePickerPopupMenu, QDate::currentDate(), 0);

	KDateTable *dt = Kexi::findFirstChild<KDateTable>(m_datePicker, "KDateTable");
	if (dt)
		connect(dt, SIGNAL(tableClicked()), this, SLOT(acceptDate()));
	m_datePicker->setCloseButton(true);
	m_datePicker->installEventFilter(this);
	m_datePickerPopupMenu->insertItem(m_datePicker);
	dateBtn->setPopup(m_datePickerPopupMenu);
	
#ifdef QDateTimeEditor_HACK
	m_dte_date = Kexi::findFirstChild<QDateTimeEditor>(m_dateEdit, "QDateTimeEditor");
	m_dte_time = Kexi::findFirstChild<QDateTimeEditor>(m_timeEdit, "QDateTimeEditor");
	if (m_dte_date && m_dte_time) {
		m_dte_date->installEventFilter(this);
		m_dte_time->installEventFilter(this);
	}
#else
	m_dte_date = 0;
	m_dte_time = 0;
#endif

//	kdDebug() << m_dte_date->focusSection() << endl;
//	kdDebug() << m_dte_time->focusSection() << endl;

//	dte->setFocusSection(1);

//	connect(btn, SIGNAL(clicked()), this, SLOT(slotShowDatePicker()));
	
	QHBoxLayout* layout = new QHBoxLayout(widget());
	layout->addWidget(m_dateEdit, 0);
	layout->addWidget(dateBtn, 0);
	layout->addWidget(m_timeEdit, 0);
	layout->addStretch(1);

	setFocusProxy(m_dateEdit);

	m_acceptEditorAfterDeleteContents = true;
}

void KexiDateTimeTableEdit::setValueInternal(const QVariant& /*add*/, bool /*removeOld*/)
{
	m_dateEdit->setDate(m_origValue.toDate());
	m_timeEdit->setTime(m_origValue.toTime());

#ifdef QDateTimeEditor_HACK
	if (m_dte_date)
		m_dte_date->setFocusSection(0);
#endif
//	m_oldVal = m_origValue;
}

//! \return true is editor's value is null (not empty)
bool KexiDateTimeTableEdit::valueIsNull()
{
	return m_dateEdit->date().isNull();
}

bool KexiDateTimeTableEdit::valueIsEmpty()
{
	return valueIsNull();//js OK? TODO (nonsense?)
}

void
KexiDateTimeTableEdit::slotDateChanged(QDate date)
{
	m_dateEdit->setDate(date);

	repaint();
}

QVariant
KexiDateTimeTableEdit::value()
{
//	ok = true;
	return QVariant(QDateTime(m_dateEdit->date(), m_timeEdit->time()));
}

void
KexiDateTimeTableEdit::slotShowDatePicker()
{
	QDate date = m_dateEdit->date();
	m_datePicker->setDate(date);
	m_datePicker->setFocus();
	m_datePicker->show();
	m_datePicker->setFocus();
}

/*! filtering some events on a date picker */
bool
KexiDateTimeTableEdit::eventFilter( QObject *o, QEvent *e )
{
	if (o==m_datePicker) {
		kdDebug() << e->type() << endl;
		switch (e->type()) {
		case QEvent::Hide:
			m_datePickerPopupMenu->hide();
			break;
		case QEvent::KeyPress:
		case QEvent::KeyRelease: {
			kdDebug() << "ok!" << endl;
			QKeyEvent *ke = (QKeyEvent *)e;
			if (ke->key()==Key_Enter || ke->key()==Key_Return) {
				//accepting picker
				acceptDate();
				return true;
			}
			else if (ke->key()==Key_Escape) {
				//cancelling picker
				m_datePickerPopupMenu->hide();
				kdDebug() << "reject" << endl;
				return true;
			}
			else m_datePickerPopupMenu->setFocus();
			break;
			}
		default:
			break;
		}
	}
#ifdef QDateTimeEditor_HACK
	else if (e->type()==QEvent::KeyPress && m_dte_date && m_dte_time) {
		bool resendEvent = false;
		QKeyEvent *ke = static_cast<QKeyEvent*>(e);
		if (ke->key()==Qt::Key_Right)
		{
			if (o==m_dte_date && m_dateEdit->hasFocus() 
				&& m_dte_date->focusSection()==int(m_dte_date->sectionCount()-1))
			{
				m_dte_date->setFocusSection(0); //to avoid h-scrolling
				m_timeEdit->setFocus();
				m_dte_time->setFocusSection(0);
			}
			else if (!m_sentEvent && cursorAtEnd())
				resendEvent = true;
		}
		else if (ke->key()==Qt::Key_Left)
		{
			if (o==m_dte_time && m_timeEdit->hasFocus() && m_dte_time->focusSection()==0) {
				m_dateEdit->setFocus();
				m_dte_date->setFocusSection(m_dte_date->sectionCount()-1);
			}
			else if (!m_sentEvent && cursorAtStart())
				resendEvent = true;
		}
		if (resendEvent) {
			//the editor should send this key event:
			m_sentEvent = true; //avoid recursion
			QApplication::sendEvent( this, ke );
			m_sentEvent = false;
			ke->ignore();
			return true;
		}
	}
#endif
	return false;
}

void KexiDateTimeTableEdit::acceptDate()
{
	m_dateEdit->setDate(m_datePicker->date());
	m_datePickerPopupMenu->hide();
	kdDebug() << "accept" << endl;
}

bool KexiDateTimeTableEdit::cursorAtStart()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_date && m_dateEdit->hasFocus() && m_dte_date->focusSection()==0;
#else
	return false;
#endif
}

bool KexiDateTimeTableEdit::cursorAtEnd()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_time && m_timeEdit->hasFocus() 
		&& m_dte_time->focusSection()==int(m_dte_time->sectionCount()-1);
#else
	return false;
#endif
}

void KexiDateTimeTableEdit::clear()
{
	m_dateEdit->setDate(QDate());
}


//======================================================

KexiDateTimeEditorFactoryItem::KexiDateTimeEditorFactoryItem()
{
}

KexiDateTimeEditorFactoryItem::~KexiDateTimeEditorFactoryItem()
{
}

KexiTableEdit* KexiDateTimeEditorFactoryItem::createEditor(
	KexiTableViewColumn &column, QScrollView* parent)
{
	return new KexiDateTimeTableEdit(column, parent);
}

#include "kexidatetimetableedit.moc"
