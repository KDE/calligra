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

KexiDateTableEdit::KexiDateTableEdit(KexiDB::Field &f, QScrollView *parent)
 : KexiTableEdit(f, parent,"KexiDateTableEdit")
{
//	kdDebug() << "KexiDateTableEdit: Date = " << value.toString() << endl;
	setView( new QWidget(this) );
//	m_edit = new KLineEdit(m_view);
	m_edit = new QDateEdit(view());
	m_edit->setAutoAdvance(true);
//js	m_edit->setFrame(false);
//js	m_edit->setValidator(new KDateValidator(m_edit, "DateValidator"));
	QToolButton* btn = new QToolButton(view());
	btn->setText("...");
	btn->setFixedWidth( QFontMetrics(btn->font()).width(" ... ") );
	btn->setPopupDelay(1); //1 ms
	
	m_datePickerPopupMenu = new KPopupMenu(0, "date_popup");
	connect(m_datePickerPopupMenu, SIGNAL(aboutToShow()), this, SLOT(slotShowDatePicker()));
	m_datePicker = new KDatePicker(m_datePickerPopupMenu, QDate(), 0);

	KDateTable *dt =Kexi::findFirstChild<KDateTable>(m_datePicker, "KDateTable");
	if (dt)
		connect(dt, SIGNAL(tableClicked()), this, SLOT(acceptDate()));
	m_datePicker->setCloseButton(true);
	m_datePicker->installEventFilter(this);
//	, WType_TopLevel | WDestructiveClose | WStyle_Customize
		//| WStyle_StaysOnTop | WStyle_NoBorder);
	m_datePickerPopupMenu->insertItem(m_datePicker);
	btn->setPopup(m_datePickerPopupMenu);
	
//	connect(btn, SIGNAL(clicked()), this, SLOT(slotShowDatePicker()));
	
	QHBoxLayout* layout = new QHBoxLayout(view());
	layout->addWidget(m_edit, 1);
	layout->addWidget(btn, 0);

	setFocusProxy(m_edit);
}

void KexiDateTableEdit::init(const QString& add, bool /*removeOld*/)
{
	bool ok;
	QDate date = KGlobal::locale()->readDate(m_origValue.toString(), &ok);

	if(!ok)
	{
		date = QDate::currentDate();
	}

	m_edit->setDate(date);
	
	m_oldVal = date;
}

//! \return true is editor's value is null (not empty)
bool KexiDateTableEdit::valueIsNull()
{
	return m_edit->date().isNull();
}

bool KexiDateTableEdit::valueIsEmpty()
{
	return false;//js OK? TODO (nonsense?)
}

void
KexiDateTableEdit::slotDateChanged(QDate date)
{
//js	m_edit->setText(KGlobal::locale()->formatDate(date, true));
	m_edit->setDate(date);

	repaint();
}

QVariant
KexiDateTableEdit::value(bool &ok)
{
	ok = true;
//js	QDate date = KGlobal::locale()->readDate(m_edit->text(), &ok);
//	QDate date = m_edit->date();
	return QVariant(m_edit->date());

/*	if(!ok)
	{
		date = m_oldVal;
	}*/

//	return QVariant(date.toString(Qt::ISODate));
}

void
KexiDateTableEdit::slotShowDatePicker()
{
	bool ok = true;
//js	QDate date = KGlobal::locale()->readDate(m_edit->text(), &ok);
	QDate date = m_edit->date();

	if(!ok)
		date = QDate::currentDate();
	
	m_datePicker->setDate(date);
	m_datePicker->setFocus();
	m_datePicker->show();
	m_datePicker->setFocus();
	/*
	bool ok;
	QDate date = KGlobal::locale()->readDate(m_edit->text(), &ok);

	if(!ok)
	{
		date = QDate::currentDate();
	}

	m_datePicker = new KexiDatePicker(0, date, 0, WType_TopLevel | WDestructiveClose | WStyle_Customize
		| WStyle_StaysOnTop | WStyle_NoBorder);
	QPoint global = mapToGlobal(QPoint(width() - height(), height()));
	m_datePicker->move(global);
	m_datePicker->show();

	connect(m_datePicker, SIGNAL(dateChanged(QDate)), this, SLOT(slotDateChanged(QDate)));
	*/
}

/*! filtering some events on date picket */
bool
KexiDateTableEdit::eventFilter( QObject *o, QEvent *e )
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
//js				m_edit->setText( KGlobal::locale()->formatDate(m_datePicker->date(), true) );
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
	//TODO?
	return false;
}

bool KexiDateTableEdit::cursorAtEnd()
{
	//TODO?
	return false;
}

void KexiDateTableEdit::clear()
{
	//TODO??
}



// we need the date thing

KexiDatePicker::KexiDatePicker(QWidget *parent, QDate date, const char *name, WFlags f)
 : KDatePicker(parent, date, name)
{
	setWFlags(f);
}

KexiDatePicker::~KexiDatePicker()
{
}

//======================================================

KexiDateEditorFactoryItem::KexiDateEditorFactoryItem()
{
}

KexiDateEditorFactoryItem::~KexiDateEditorFactoryItem()
{
}

KexiTableEdit* KexiDateEditorFactoryItem::createEditor(
	KexiDB::Field &f, QScrollView* parent)
{
	return new KexiDateTableEdit(f, parent);
}

#include "kexidatetableedit.moc"
