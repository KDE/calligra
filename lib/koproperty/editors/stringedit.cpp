/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "stringedit.h"

#include <qlayout.h>
#include <qlineedit.h>
#include <qvariant.h>

namespace KOProperty {

StringEdit::StringEdit(Property *property, QWidget *parent, const char *name)
 : Widget(property, parent, name)
{
	QHBoxLayout *l = new QHBoxLayout(this, 0, 0);
	m_edit = new QLineEdit(this);
	m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_edit->setMinimumHeight(5);
	l->addWidget(m_edit);
	setFocusWidget(m_edit);

	connect(m_edit, SIGNAL(textChanged(const QString&)), this, SLOT(slotValueChanged(const QString&)));
}

StringEdit::~StringEdit()
{}

QVariant
StringEdit::value() const
{
	return m_edit->text();
}

void
StringEdit::setValue(const QVariant &value, bool emitChange)
{
	m_edit->blockSignals(true);
	m_edit->setText(value.toString());
	m_edit->blockSignals(false);
	if (emitChange)
		emit valueChanged(this);
}

void
StringEdit::slotValueChanged(const QString &)
{
	emit valueChanged(this);
}

}

#include "stringedit.moc"

