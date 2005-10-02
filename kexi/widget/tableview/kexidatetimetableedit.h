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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXIDATETIMETABLEEDIT_H
#define KEXIDATETIMETABLEEDIT_H

#include "kexitableedit.h"
#include <kdatepicker.h>
#include "kexicelleditorfactory.h"

class QDateEdit;
class QTimeEdit;
class QDateTimeEditor;

class KDatePicker;
class KPopupMenu;

//! @short Editor class for DateTime type.
class KEXIDATATABLE_EXPORT KexiDateTimeTableEdit : public KexiTableEdit
{
	Q_OBJECT

	public:
		KexiDateTimeTableEdit(KexiTableViewColumn &column, QScrollView *parent=0);
		virtual QVariant value();
		virtual bool valueIsNull();
		virtual bool valueIsEmpty();
		virtual void clear();
		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();

		virtual bool eventFilter( QObject *o, QEvent *e );

	protected slots:
		void slotDateChanged(QDate);
		void slotShowDatePicker();
		void acceptDate();

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);

		KDatePicker *m_datePicker;
		QDateEdit* m_dateEdit;
		QTimeEdit* m_timeEdit;
		QDateTimeEditor *m_dte_date, *m_dte_time;
		KPopupMenu *m_datePickerPopupMenu;

		bool m_sentEvent : 1;
};

class KexiDateTimeEditorFactoryItem : public KexiCellEditorFactoryItem
{
	public:
		KexiDateTimeEditorFactoryItem();
		virtual ~KexiDateTimeEditorFactoryItem();

	protected:
		virtual KexiTableEdit* createEditor(KexiTableViewColumn &column, QScrollView* parent = 0);
};

#endif
