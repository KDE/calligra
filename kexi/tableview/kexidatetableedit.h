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

#ifndef KEXIDATETABLEEDIT_H
#define KEXIDATETABLEEDIT_H

#include <kexitableedit.h>
#include <kdatepicker.h>

#include "kexicelleditorfactory.h"

class QDateEdit;
class QDateTimeEditor;
class KDatePicker;
class KPopupMenu;

//! @short Editor class for Date type.
class KEXIDATATABLE_EXPORT KexiDateTableEdit : public KexiTableEdit
{
	Q_OBJECT

	public:
		KexiDateTableEdit(KexiTableViewColumn &column, QScrollView *parent=0);
		virtual QVariant value(bool &ok);
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
		virtual void init(const QString& add, bool removeOld);
		void moveToFirstSection();

		KDatePicker	*m_datePicker;
		QDateEdit* m_edit;
		
		KPopupMenu *m_datePickerPopupMenu;

		QDateTimeEditor* m_dte_date;
		QObject *m_dte_date_obj;
		int m_setNumberOnFocus;
		bool m_sentEvent : 1;
};

class KexiDateEditorFactoryItem : public KexiCellEditorFactoryItem
{
	public:
		KexiDateEditorFactoryItem();
		virtual ~KexiDateEditorFactoryItem();

	protected:
		virtual KexiTableEdit* createEditor(KexiTableViewColumn &column, QScrollView* parent = 0);
};

#endif
