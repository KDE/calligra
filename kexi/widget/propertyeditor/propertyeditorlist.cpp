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

#include <klocale.h>

#include "propertyeditorlist.h"
#include "kexiproperty.h"

PropComboBox::PropComboBox(QWidget *parent)
   : KComboBox(parent)
  {}

bool
PropComboBox::eventFilter(QObject *o, QEvent *e)
{
	if(o == lineEdit())
	{
	if(e->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = static_cast<QKeyEvent*>(e);
		if((ev->key()==Key_Up || ev->key()==Key_Down) && ev->state()!=ControlButton)
		{
			parentWidget()->eventFilter(o, e);
			return true;
		}
	}
	}
	
	return KComboBox::eventFilter(o, e);
}

PropertyEditorList::PropertyEditorList(QWidget *parent, KexiProperty *property, const char *name)
 : KexiPropertySubEditor(parent, property, name)
{
	m_combo = new PropComboBox(this);

	m_combo->setFocusPolicy(QWidget::StrongFocus);
	m_combo->setGeometry(frameGeometry());
	m_combo->setEditable(true);
	m_combo->setAutoCompletion(true);
	if(property->list())
	{
	m_combo->insertStringList(*(property->list()));
	m_combo->setCurrentText(property->value().asString());
	}
	m_combo->show();

	setWidget(m_combo);
	connect(m_combo, SIGNAL(activated(int)), SLOT(valueChanged()));
}

QVariant
PropertyEditorList::getValue()
{
	return QVariant(m_combo->currentText());
}

void
PropertyEditorList::setValue(const QVariant &value)
{
	m_combo->setCurrentText(value.toString());
	emit changed(this);
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

#include "propertyeditorlist.moc"
