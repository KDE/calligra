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

#include <qlineedit.h>

#include "propertyeditorinput.h"

PropertyEditorInput::PropertyEditorInput(QWidget *parent, QVariant::Type type, QVariant value, const char *name=0)
 : PropertyEditorEditor(parent, type, value, name)
{
	m_lineedit = new QLineEdit(this);
	m_lineedit->resize(width(), height());

	m_lineedit->setText(value.toString());
	m_lineedit->show();
}

QVariant
PropertyEditorInput::getValue()
{
	return QVariant(m_lineedit->text());
}

#include "propertyeditorinput.moc"
