/* This file is part of the KDE project
   Copyright (C) 2002   Peter Simonsson <psn@linux.se>

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

#include <qlayout.h>

#include "kexicomboboxtableedit.h"


//KexiComboBoxTableEdit::KexiComboBoxTableEdit(KexiDB::Field::Type t, const QStringList list, QWidget *parent,
// const char *name) : KexiTableEdit(parent, name)
KexiComboBoxTableEdit::KexiComboBoxTableEdit(
	QVariant value, KexiDB::Field &f, const QString& add, QWidget *parent)
 : KexiTableEdit(value, f, parent,"KexiComboBoxTableEdit")
{
	m_view = new KComboBox(this, "tableCombo");
	m_combo = static_cast<KComboBox*>(m_view);
	m_combo->clear();
//	m_combo->insertStringList(f.enumHints());
	QValueVector<QString> hints = f.enumHints();
	for(int i=0; i < hints.size(); i++)
	{
		if(!hints.at(i).isEmpty())
			m_combo->insertItem(hints.at(i));
	}
	m_combo->setCurrentItem(value.toInt() - 1);

//js:	TODO
//js	static_cast<KComboBox*>(m_view)->insertStringList(list);
//js	static_cast<KComboBox*>(m_view)->setCurrentItem(static_cast<int>(t));
}


QVariant KexiComboBoxTableEdit::value(bool &ok)
{
	ok = true;
	return m_combo->currentItem() + 1;
}

bool KexiComboBoxTableEdit::cursorAtStart()
{
	//TODO?
	return false;
}

bool KexiComboBoxTableEdit::cursorAtEnd()
{
	//TODO?
	return false;
}

void KexiComboBoxTableEdit::clear()
{
	m_combo->clear();
}

bool KexiComboBoxTableEdit::valueIsNull()
{
	return false;
}

bool KexiComboBoxTableEdit::valueIsEmpty()
{
	return false;
}

