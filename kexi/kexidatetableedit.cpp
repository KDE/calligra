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

#include "datepicker.h"

#include "kexidatetableedit.h"

KexiDateTableEdit::KexiDateTableEdit(QVariant v, QWidget *parent, const char *name)
  : QLineEdit("", parent, name)
{
	m_data = v;
	m_text = KGlobal::_locale->formatDate(m_data.toDate(), true);
	setText(m_text);
	setCursor(QCursor(ArrowCursor));
}

void
KexiDateTableEdit::paintEvent(QPaintEvent *ev)
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
	p.drawRect(r);

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
KexiDateTableEdit::mouseMoveEvent(QMouseEvent *ev)
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
		DatePicker *d = new DatePicker(this);
		QPoint global = mapToGlobal(QPoint(width() - height(), height()));
		d->move(global);
		d->show();
	}
}

KexiDateTableEdit::~KexiDateTableEdit()
{
}

#include "kexidatetableedit.moc"
