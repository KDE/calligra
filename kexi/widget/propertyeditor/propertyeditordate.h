/* This file is part of the KDE project
   Copyright (C) 2003 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef PROPERTYEDITORDATE_H
#define PROPERTYEDITORDATE_H

#include "kexipropertysubeditor.h"

class KexiProperty;
class QDateEdit;
class QTimeEdit;
class QDateTimeEdit;
class QDate;
class QTime;
class QDateTime;

class KEXIPROPERTYEDITOR_EXPORT PropertyEditorDate : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorDate(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorDate() {;}

		virtual QVariant	value();
		virtual	void 		setValue(const QVariant &value);

	protected slots:
		void			valueChanged(const QDate&);

	protected:
		QDateEdit	*m_dateedit;
};


class KEXIPROPERTYEDITOR_EXPORT PropertyEditorTime : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorTime(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorTime() {;}

		virtual QVariant	value();
		virtual	void 		setValue(const QVariant &value);

	protected slots:
		void			valueChanged(const QTime&);

	protected:
		QTimeEdit	*m_timeedit;
};


class KEXIPROPERTYEDITOR_EXPORT PropertyEditorDateTime : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorDateTime(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorDateTime() {;}

		virtual QVariant	value();
		virtual	void 		setValue(const QVariant &value);

	protected slots:
		void			valueChanged(const QDateTime&);

	protected:
		QDateTimeEdit	*m_datetime;
};

#endif

