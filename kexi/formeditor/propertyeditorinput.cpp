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
#include <qspinbox.h>

#include "propertyeditorinput.h"

PropertyEditorInput::PropertyEditorInput(QWidget *parent, QVariant::Type type, QVariant value, const char *name=0)
 : PropertyEditorEditor(parent, type, value, name)
{
	m_lineedit = new QLineEdit(this);
	m_lineedit->resize(width(), height());

	m_lineedit->setText(value.toString());
	m_lineedit->show();

	setWidget(m_lineedit);

	connect(m_lineedit, SIGNAL(textChanged(const QString &)), this, SLOT(slotTextChanged(const QString &)));
}

void
PropertyEditorInput::slotTextChanged(const QString &text)
{
	emit changed(this);
}

QVariant
PropertyEditorInput::getValue()
{
	return QVariant(m_lineedit->text());
}

//INT

PropertyEditorSpin::PropertyEditorSpin(QWidget *parent, QVariant::Type type, QVariant value, const char *name=0)
 : PropertyEditorEditor(parent, type, value, name)
{
	m_spinBox = new QSpinBox(this);
	m_spinBox->resize(width(), height());

	m_spinBox->setValue(value.toInt());
	m_spinBox->show();

	setWidget(m_spinBox);
}

QVariant
PropertyEditorSpin::getValue()
{
	return QVariant(m_spinBox->value());
}

#include "propertyeditorinput.moc"
