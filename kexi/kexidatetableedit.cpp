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

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdatepicker.h>

//#include "datepicker.h"

#include "kexidatetableedit.h"

KexiDateTableEdit::KexiDateTableEdit(QVariant v, QWidget *parent, const char *name)
  : KexiInputTableEdit(v, QVariant::Date, QString::null, false, parent, name)
{
	m_data = v;
	
	if(!m_data.toDate().isValid())
	{
		m_data = QVariant(QDate::currentDate());
	}
	else
	{
		m_data = v;
	}

	m_text = KGlobal::_locale->formatDate(m_data.toDate(), true);
	m_view->setText(m_text);
	setCursor(QCursor(ArrowCursor));

	m_mouseDown = false;
	m_3dDecore = true;
	m_datePicker = 0;
}

void
KexiDateTableEdit::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	QColorGroup cg(colorGroup());
//	kdDebug() << "color: " << back.red() << ", " << back.green() << ", " << back.blue() << endl;

	QFontMetrics f = fontMetrics();

	p.drawText(2, f.height() - 2 , m_text);

	QRect r(width() - height(), 0, height(), height());

	if(!m_mouseDown)
	{
		p.setPen(QPen(cg.light()));
	}
	else
	{
		p.setPen(QPen(cg.dark()));
	}

	p.setBrush(QBrush(cg.button()));

	if(!m_3dDecore)
	{
		p.drawRect(r);
	}
	else
	{
		QColor top = cg.light();
		int c = top.red();
		if(m_mouseDown)
			c = c - 30;


		for(int i=0; i < r.height(); i++)
		{
			p.setPen(QPen(QColor(c, c, c)));
			p.drawLine(width() - height(), i, width(), i);
			c = c - 5;
		}
	}

	if(m_mouseDown)
	{
		p.setPen(QPen(cg.light()));
	}
	else
	{
		p.setPen(QPen(cg.dark()));
	}
	p.drawLine(width() - 1, 1, width() - 1, height()); //vertical line
	p.drawLine(r.x(), height() - 1, width() - 1, height() - 1); //horizontal line

	p.setPen(cg.foreground());
	if(m_mouseDown)
		p.drawText(width() - r.width() + 3, f.height() - 1, "...");
	else
		p.drawText(width() - r.width() + 2, f.height() - 2, "...");


	p.end();
}

void
KexiDateTableEdit::mousePressEvent(QMouseEvent *ev)
{
	if(ev->x() > width() - height())
	{
		m_mouseDown = true;
		repaint();
	}
}

void
KexiDateTableEdit::mouseMoveEvent(QMouseEvent *)
{
	setCursor(QCursor(ArrowCursor));
}

void
KexiDateTableEdit::mouseReleaseEvent(QMouseEvent *ev)
{
	if(m_mouseDown)
	{
		m_mouseDown = false;
		repaint();
		
		QDate date;
		if(!m_data.toDate().isValid())
		{
			date = QDate::currentDate();
		}
		else
		{
			date = m_data.toDate();
		}
		m_datePicker = new KexiDatePicker(0, date, 0, WType_TopLevel | WDestructiveClose | WStyle_Customize | WStyle_StaysOnTop | WStyle_NoBorder);
//		uh man, how i hate backwords compatibility
//		m_datePicker->setCloseButton(true);
		QPoint global = mapToGlobal(QPoint(width() - height(), height()));
		m_datePicker->move(global);
		m_datePicker->show();

		connect(m_datePicker, SIGNAL(dateChanged(QDate)), this, SLOT(slotDateChanged(QDate)));
	}
}

void
KexiDateTableEdit::setDecorated(bool decorated)
{
	m_3dDecore = decorated;
}

void
KexiDateTableEdit::slotDateChanged(QDate date)
{
	m_data = date;
	m_text = KGlobal::_locale->formatDate(m_data.toDate(), true);
	m_view->setText(m_text);

	repaint();
}

QVariant
KexiDateTableEdit::value()
{
	return QVariant(m_data);
}

KexiDateTableEdit::~KexiDateTableEdit()
{
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
