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

#include "kexidatetableedit.h"

#define KEXIDATETIMEEDITOR_P_IMPL
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

KexiDateTableEdit::KexiDateTableEdit(KexiTableViewColumn &column, QScrollView *parent)
 : KexiTableEdit(column, parent,"KexiDateTableEdit")
{
	m_sentEvent = false;
	setView( new QWidget(this) );
	m_edit = new QDateEdit(view());
	m_edit->setAutoAdvance(true);
	m_edit->installEventFilter(this);
	m_setNumberOnFocus = -1;

	QToolButton* btn = new QToolButton(view());
	btn->setText("...");
	btn->setFixedWidth( QFontMetrics(btn->font()).width(" ... ") );
	btn->setPopupDelay(1); //1 ms

	m_dte_date_obj = Kexi::findFirstChild<QObject>(m_edit, "QDateTimeEditor");
	if (m_dte_date_obj)
		m_dte_date_obj->installEventFilter(this);
	
#if QDateTimeEditor_HACK
	m_dte_date = dynamic_cast<QDateTimeEditor*>(m_dte_date_obj);
#else
	m_dte_date = 0;
#endif

	m_datePickerPopupMenu = new KPopupMenu(0, "date_popup");
	connect(m_datePickerPopupMenu, SIGNAL(aboutToShow()), this, SLOT(slotShowDatePicker()));
	m_datePicker = new KDatePicker(m_datePickerPopupMenu, QDate::currentDate(), 0);

	KDateTable *dt =Kexi::findFirstChild<KDateTable>(m_datePicker, "KDateTable");
	if (dt)
		connect(dt, SIGNAL(tableClicked()), this, SLOT(acceptDate()));
	m_datePicker->setCloseButton(true);
	m_datePicker->installEventFilter(this);
	m_datePickerPopupMenu->insertItem(m_datePicker);
	btn->setPopup(m_datePickerPopupMenu);
	
	QHBoxLayout* layout = new QHBoxLayout(view());
	layout->addWidget(m_edit, 1);
	layout->addWidget(btn, 0);

	setFocusProxy(m_edit);

	m_acceptEditorAfterDeleteContents = true;
}

void KexiDateTableEdit::init(const QString& add, bool removeOld)
{
	m_setNumberOnFocus = -1;
	QDate d;
	if (removeOld) {
		if (!add.isEmpty() && add[0].latin1()>='0' && add[0].latin1() <='9') {
			m_setNumberOnFocus = add[0].latin1()-'0';
			d = QDate((m_setNumberOnFocus)*1000, 1, 1);
		}
	}
	else {
		d = m_origValue.toDate();
	}
	m_edit->setDate(d);
	moveToFirstSection();
}

bool KexiDateTableEdit::valueIsNull()
{
	return m_edit->date().isNull();
}

bool KexiDateTableEdit::valueIsEmpty()
{
	return valueIsNull();//js OK? TODO (nonsense?)
}

void
KexiDateTableEdit::slotDateChanged(QDate date)
{
	m_edit->setDate(date);
	repaint();
}

QVariant 
KexiDateTableEdit::value(bool &ok)
{
	ok = true;
	return QVariant(m_edit->date());
}

void
KexiDateTableEdit::slotShowDatePicker()
{
	QDate date = m_edit->date();

	m_datePicker->setDate(date);
	m_datePicker->setFocus();
	m_datePicker->show();
	m_datePicker->setFocus();
}

//! @internal helper
void KexiDateTableEdit::moveToFirstSection()
{
	if (!m_dte_date_obj)
		return;
#if QDateTimeEditor_HACK
	if (m_dte_date)
		m_dte_date->setFocusSection(0);
#else
	QKeyEvent ke_left(QEvent::KeyPress, Qt::Key_Left, 0, 0);
	for (int i=0; i<8; i++)
		QApplication::sendEvent( m_dte_date_obj, &ke_left );
#endif
}

bool KexiDateTableEdit::eventFilter( QObject *o, QEvent *e )
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
	else if (e->type()==QEvent::FocusIn && o->parent() && o->parent()->parent()==m_edit
		&& m_setNumberOnFocus >= 0 && m_dte_date_obj)
	{
		// there was a number character passed as 'add' parameter in init():
		moveToFirstSection();
		QKeyEvent ke(QEvent::KeyPress, int(Qt::Key_0)+m_setNumberOnFocus, 
			'0'+m_setNumberOnFocus, 0, QString::number(m_setNumberOnFocus));
		QApplication::sendEvent( m_dte_date_obj, &ke );
		m_setNumberOnFocus = -1;
	}
#if QDateTimeEditor_HACK
	else if (e->type()==QEvent::KeyPress && m_dte_date) {
		bool resendEvent = false;
		QKeyEvent *ke = static_cast<QKeyEvent*>(e);
		if ((ke->key()==Qt::Key_Right && !m_sentEvent && cursorAtEnd())
			|| (ke->key()==Qt::Key_Left && !m_sentEvent && cursorAtStart()))
		{
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

void KexiDateTableEdit::acceptDate()
{
	m_edit->setDate(m_datePicker->date());
	m_datePickerPopupMenu->hide();
	kdDebug() << "accept" << endl;
}

bool KexiDateTableEdit::cursorAtStart()
{
#if QDateTimeEditor_HACK
	return m_dte_date && m_edit->hasFocus() && m_dte_date->focusSection()==0;
#else
	return false;
#endif
}

bool KexiDateTableEdit::cursorAtEnd()
{
#if QDateTimeEditor_HACK
	return m_dte_date && m_edit->hasFocus() 
		&& m_dte_date->focusSection()==(m_dte_date->sectionCount()-1);
#else
	return false;
#endif
}

void KexiDateTableEdit::clear()
{
	m_edit->setDate(QDate());
}

//======================================================

KexiDateEditorFactoryItem::KexiDateEditorFactoryItem()
{
}

KexiDateEditorFactoryItem::~KexiDateEditorFactoryItem()
{
}

KexiTableEdit* KexiDateEditorFactoryItem::createEditor(
	KexiTableViewColumn &column, QScrollView* parent)
{
	return new KexiDateTableEdit(column, parent);
}

#include "kexidatetableedit.moc"
