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
#include <kdebug.h>
#include <klistview.h>

#include "kexipropertysubeditor.h"

KexiPropertySubEditor::KexiPropertySubEditor(QWidget *parent, KexiProperty *property, const char *name)
 : QWidget(parent, name)
{
	m_childWidget = 0;
	m_property = property;
	m_leaveTheSpaceForRevertButton = false;
}

bool
KexiPropertySubEditor::eventFilter(QObject* /*watched*/, QEvent* e)
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
		else if (ev->state()!=ControlButton && (ev->key()==Key_Up || ev->key()==Key_Down)) {
			KListView *list = (KListView*) parentWidget()->parentWidget();
			KListViewItem *item = (KListViewItem*)list->itemAt(mapToParent(QPoint(2,2)));
		
			if(ev->key()==Key_Up) {
				if(item->itemAbove())
					list->setCurrentItem(item->itemAbove());
				return true;
			}
			else if(ev->key()==Key_Down) {
				if(item->itemBelow())
					list->setCurrentItem(item->itemBelow());
				return true;
			}
		}
	}
	return false;
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
KexiPropertySubEditor::setWidget(QWidget *w, QWidget* focusProxy)
{
	if (m_childWidget)
		m_childWidget->removeEventFilter(this);

	m_childWidget = w;

	if(!m_childWidget)
		return;
	if (focusProxy && focusProxy->focusPolicy()!=NoFocus) {
		setFocusProxy(focusProxy);
		focusProxy->installEventFilter(this);
	}
	else if (m_childWidget->focusPolicy()!=NoFocus)
		setFocusProxy(m_childWidget);

	m_childWidget->installEventFilter(this);
//	if (m_childWidget->inherits("QFrame")) {
//		static_cast<QFrame*>(m_childWidget)->setFrameStyle( QFrame::Box | QFrame::Plain );
//	}
}

QVariant
KexiPropertySubEditor::value()
{
	return QVariant("");
}

void
KexiPropertySubEditor::setValue(const QVariant &/*value*/)
{
	return;
}

KexiPropertySubEditor::~KexiPropertySubEditor()
{
}


#include "kexipropertysubeditor.moc"
