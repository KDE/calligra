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

#include <klocale.h>

#include "propertyeditorlist.h"

PropertyEditorList::PropertyEditorList(QWidget *parent, QVariant::Type type, QVariant value, const char *name)
 : PropertyEditorEditor(parent, type, value, name)
{
	m_combo = new QComboBox(this);

	m_combo->setFocusPolicy(QWidget::StrongFocus);
	m_combo->setGeometry(frameGeometry());
	m_combo->show();

	setWidget(m_combo);
	connect(m_combo, SIGNAL(activated(int)), SLOT(valueChanged()));
}

PropertyEditorList::PropertyEditorList(QWidget *parent, QVariant::Type type, QVariant value, QStringList list, const char *name)
 : PropertyEditorEditor(parent, type, value, name)
{
	m_combo = new QComboBox(this);

	m_combo->setFocusPolicy(QWidget::StrongFocus);
	m_combo->setGeometry(frameGeometry());
	m_combo->insertStringList(list);
	m_combo->setCurrentItem(value.toInt());
	m_combo->show();

	setWidget(m_combo);
	connect(m_combo, SIGNAL(activated(int)), SLOT(valueChanged()));
}

QVariant
PropertyEditorList::getValue()
{
	return QVariant(m_combo->currentItem());
}

void
PropertyEditorList::setList(QStringList l)
{
	m_combo->insertStringList(l);
}

void
PropertyEditorList::valueChanged()
{
	emit changed(this);
}

/*********************
 * BOOL-EDITOR       *
 *********************/

PropertyEditorBool::PropertyEditorBool(QWidget *parent, QVariant::Type type, QVariant value, const char *name)
 : PropertyEditorList(parent, type, value, name)
{
	m_combo->insertItem(i18n("true"));
	m_combo->insertItem(i18n("false"));

	if(value.toBool())
		m_combo->setCurrentItem(0);
	else
		m_combo->setCurrentItem(1);
}

QVariant
PropertyEditorBool::getValue()
{
	if(m_combo->currentItem() == 0)
		return QVariant(true, 1);
	else
		return QVariant(false, 1);
}

#include "propertyeditorlist.moc"
