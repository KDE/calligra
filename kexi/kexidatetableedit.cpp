/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

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
#include <qlineedit.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdatepicker.h>
#include <kdatetbl.h>

//#include "datepicker.h"

#include "kexidatetableedit.h"

KexiDateTableEdit::KexiDateTableEdit(QVariant v, QWidget *parent, const char *name)
  : KexiTableEdit(parent, name)
{
	kdDebug() << "KexiDateTableEdit: Date = " << v.toString() << endl;
	m_datePicker = 0;
	m_view = new QWidget(this);
	m_edit = new QLineEdit(m_view);
	m_edit->setValidator(new KDateValidator(m_edit, "DateValidator"));
	QToolButton* btn = new QToolButton(m_view);
	btn->setText("...");
	connect(btn, SIGNAL(clicked()), this, SLOT(slotShowDatePicker()));
	QHBoxLayout* layout = new QHBoxLayout(m_view);
	layout->addWidget(m_edit);
	layout->addWidget(btn);
	
	bool ok;
	QDate date = KGlobal::_locale->readDate(v.toString(), &ok);
	
	if(!ok)
	{
		date = QDate::currentDate();
	}

	m_edit->setText(KGlobal::_locale->formatDate(date, true));
	m_oldVal = date;
	setFocusProxy(m_edit);
}

void
KexiDateTableEdit::slotDateChanged(QDate date)
{
	m_edit->setText(KGlobal::_locale->formatDate(date, true));
	repaint();
}

QVariant
KexiDateTableEdit::value()
{
	bool ok;
	QDate date = KGlobal::_locale->readDate(m_edit->text(), &ok);
	
	if(!ok)
	{
		date = m_oldVal;
	}
	
	return QVariant(date.toString(Qt::ISODate));
}

void
KexiDateTableEdit::slotShowDatePicker()
{
	bool ok;
	QDate date = KGlobal::_locale->readDate(m_edit->text(), &ok);
	
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

#include "kexidatetableedit.moc"
