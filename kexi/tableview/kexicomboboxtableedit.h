/* This file is part of the KDE project
   Copyright (C) 2002   Peter Simonsson <psn@linux.se>
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

#ifndef _KEXICOMBOBOXTABLEEDIT_H_
#define _KEXICOMBOBOXTABLEEDIT_H_

#include <kcombobox.h>

#include "kexidb/field.h"
#include "kexitableedit.h"
#include "kexicelleditorfactory.h"

/**
 * 
 * Peter Simonsson
 **/
class KexiComboBoxTableEdit : public KexiTableEdit
{
	public:
		KexiComboBoxTableEdit(KexiDB::Field &f, QWidget *parent=0);
//		KexiComboBoxTableEdit(KexiDB::Field::Type t, const QStringList list,
//			QWidget *parent=0, const char *name=0);


		virtual QVariant value(bool &ok);

		virtual void clear();
		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();

		virtual bool valueIsNull();
		virtual bool valueIsEmpty();

	private:
		virtual void init(const QString& /*add*/);

		//pointer for to reduces casting (typing :)
		KComboBox *m_combo;
};

class KexiComboBoxEditorFactoryItem : public KexiCellEditorFactoryItem
{
	public:
		KexiComboBoxEditorFactoryItem();
		virtual ~KexiComboBoxEditorFactoryItem();

	protected:
		virtual KexiTableEdit* createEditor(KexiDB::Field &f, QWidget* parent = 0);
};

#endif
