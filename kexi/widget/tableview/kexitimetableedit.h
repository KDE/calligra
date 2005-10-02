/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXITIMETABLEEDIT_H
#define KEXITIMETABLEEDIT_H

#include "kexitableedit.h"
#include "kexicelleditorfactory.h"

class QTimeEdit;
class QDateTimeEditor;

//! @short Editor class for Time type.
class KEXIDATATABLE_EXPORT KexiTimeTableEdit : public KexiTableEdit
{
	Q_OBJECT

	public:
		KexiTimeTableEdit(KexiTableViewColumn &column, QScrollView *parent=0);
		virtual QVariant value();
		virtual bool valueIsNull();
		virtual bool valueIsEmpty();
		virtual void clear();
		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();

		virtual bool eventFilter( QObject *o, QEvent *e );

	protected slots:
		void slotValueChanged(const QTime& t);

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);
		void moveToFirstSection();

		QTimeEdit* m_edit;
		QDateTimeEditor* m_dte_time;
		QObject *m_dte_time_obj;
		int m_setNumberOnFocus;
		bool m_cleared : 1;
		bool m_sentEvent : 1;
};

class KexiTimeEditorFactoryItem : public KexiCellEditorFactoryItem
{
	public:
		KexiTimeEditorFactoryItem();
		virtual ~KexiTimeEditorFactoryItem();

	protected:
		virtual KexiTableEdit* createEditor(KexiTableViewColumn &column, QScrollView* parent = 0);
};

#endif
