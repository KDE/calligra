/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

/*
   this file contains all editor-classes using comboboxes
*/

#ifndef PROPERTYEDITORLIST_H
#define PROPERTYEDITORLIST_H

#include <kcombobox.h>

#include "kexipropertysubeditor.h"

class KComboBox;
class QStringList;
class KexiProperty;
class KListBox;
class QToolButton;

class KEXIPROPERTYEDITOR_EXPORT PropComboBox : public KComboBox
{
	Q_OBJECT

	public:
		PropComboBox(QWidget *parent, bool multi);
		~PropComboBox() {;}

		virtual bool eventFilter(QObject *o, QEvent *e);
		void setSelected(const QStringList &list);
		QStringList getSelected();

	public slots:
		void updateEdit();
		void hideList();

	protected:
		KListBox *m_listbox;
		bool m_eventFilterEnabled : 1;
};

class KEXIPROPERTYEDITOR_EXPORT PropertyEditorList : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorList(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorList() {;}

		virtual QVariant	value();
		virtual	void 		setValue(const QVariant &value);

		void setList(QStringList l);

	protected slots:
		void valueChanged();
		void itemExecuted();

	protected:
		PropComboBox		*m_combo;
		QToolButton		*m_button;
};

class KEXIPROPERTYEDITOR_EXPORT PropertyEditorMultiList : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorMultiList(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorMultiList() {;}

		virtual QVariant	value();
		virtual	void 		setValue(const QVariant &value);

		void setList(QStringList l);

	protected slots:
		void valueChanged();

	protected:
		PropComboBox		*m_combo;
};

class KEXIPROPERTYEDITOR_EXPORT PropertyEditorCursor : public PropertyEditorList
{
	Q_OBJECT

	public:
		PropertyEditorCursor(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorCursor() {;}

		virtual QVariant	value();
		virtual	void 		setValue(const QVariant &value);
};


#endif
