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

#ifndef KEXIDATETABLEEDIT_H
#define KEXIDATETABLEEDIT_H

#include <kexiinputtableedit.h>

#include <kdatepicker.h>

/*
  this class represents an editor for QVariant::Date

  uhm, thanks to tronical, who added me a second constructor
  for KDatePicker i dont't have to type so much JIPPIE
  update (2002-09-02 (00:30))
*/

//class KDatePicker;

class KexiDatePicker;

class KexiDateTableEdit : public KexiInputTableEdit
{

	Q_OBJECT

	public:
		KexiDateTableEdit(QVariant v=0, QWidget *parent=0, const char *name=0);
		~KexiDateTableEdit();

		void setDecorated(bool decore);
		QVariant value();

	protected:
		void paintEvent(QPaintEvent *ev);
		void mousePressEvent(QMouseEvent *ev);
		void mouseReleaseEvent(QMouseEvent *ev);
		void mouseMoveEvent(QMouseEvent *ev);
		/* gruml, we have to overwrite the cursor */

		KexiDatePicker	*m_datePicker;

		QVariant	m_data;
		QString		m_text;

		bool		m_mouseDown;
		bool		m_3dDecore;

	protected slots:
		void		slotDateChanged(QDate);
};

class KexiDatePicker : public KDatePicker
{
	Q_OBJECT

	public:
		KexiDatePicker(QWidget *parent, QDate date, const char *name, WFlags f);
		~KexiDatePicker();
};

#endif
