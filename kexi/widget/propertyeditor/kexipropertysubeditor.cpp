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
#include <qvariant.h>

#include "kexipropertysubeditor.h"

KexiPropertySubEditor::KexiPropertySubEditor(QWidget *parent, KexiProperty *property, const char *name)
 : QWidget(parent, name)
{
	m_childWidget = 0;
}

bool
KexiPropertySubEditor::eventFilter(QObject* watched, QEvent* e)
{
	if(e->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = static_cast<QKeyEvent*>(e);
		if(ev->key() == Key_Escape)
		{
			emit reject(this);
			return true;
		}
		else if((ev->key() == Key_Return) || (ev->key() == Key_Enter))
		{
			emit accept(this);
			return true;
		}
	}

	return QWidget::eventFilter(watched, e);
}

void
KexiPropertySubEditor::resizeEvent(QResizeEvent *ev)
{
	if(m_childWidget)
	{
		m_childWidget->resize(ev->size());
	}
}

void
KexiPropertySubEditor::setWidget(QWidget *w)
{
	m_childWidget = w;
	setFocusProxy(m_childWidget);
	m_childWidget->installEventFilter(this);
}

QVariant
KexiPropertySubEditor::getValue()
{
	return QVariant("");
}

void
KexiPropertySubEditor::setValue(const QVariant &value)
{
	return;
}

KexiPropertySubEditor::~KexiPropertySubEditor()
{
}


#include "kexipropertysubeditor.moc"
