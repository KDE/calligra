/* This file is part of the KDE project
   Copyright (C) 2002   Peter Simonsson <psn@linux.se>

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

#include <qlayout.h>

#include "kexicomboboxtableedit.h"


KexiComboBoxTableEdit::KexiComboBoxTableEdit(KexiDBField::ColumnType t, const QStringList list, QWidget *parent,
 const char *name) : KexiTableEdit(parent, name)
{
	m_view = new KComboBox(this, "tableCombo");
	m_view->clear();
	m_view->insertStringList(list);
	m_view->setCurrentItem(static_cast<int>(t));
	(new QVBoxLayout(this))->addWidget(m_view);
}


QVariant KexiComboBoxTableEdit::value()
{
	return QVariant(m_view->currentItem());
}
