/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#include <qcombobox.h>

#include "propertyeditorlist.h"

PropertyEditorList::PropertyEditorList(QWidget *parent, QVariant::Type type, QVariant vlaue, const char *name=0)
 : PropertyEditorEditor(parent, type, QVariant(""), name)
{
	m_combo = new QComboBox(this);
	
	m_combo->setGeometry(frameGeometry());
	m_combo->show();
}

QVariant
PropertyEditorList::getValue()
{
	return QVariant(false);
}

/*********************
 * BOOL-EDITOR       *
 *********************/

PropertyEditorBool::PropertyEditorBool(QWidget *parent, QVariant::Type type, QVariant value, const char *name=0)
 : PropertyEditorList(parent, type, value, name)
{
	m_combo->insertItem("true");
	m_combo->insertItem("false");

	if(value.toBool())
		m_combo->setCurrentItem(0);
	else
		m_combo->setCurrentItem(1);
}

QVariant
PropertyEditorBool::getValue()
{
	return QVariant(false);
}

#include "propertyeditorlist.moc"
