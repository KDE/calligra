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

#include "kexipropertysubeditor.h"
#include "kexipropertyeditor.h"

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
	if(e->type() == QEvent::KeyPress) // || e->type()==QEvent::AccelOverride)
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
		else {
			KexiPropertyEditor *list = dynamic_cast<KexiPropertyEditor*>( parentWidget()->parentWidget() );
			if (!list)
				return false; //for sanity
			return list->handleKeyPress(ev);
/*			if (ev->state()==NoButton 
			&& (ev->key()==Key_Up || ev->key()==Key_Down || ev->key()==Key_Home || ev->key()==Key_End))
		{
			//selection moving
			KListView *list = dynamic_cast<KListView*>( parentWidget()->parentWidget() );
			if (!list)
				return false; //for sanity
			QListViewItem *item = list->itemAt(mapToParent(QPoint(2,2)));
		
			if(ev->key()==Key_Up) {
				//find prev visible
				item = item ? item->itemAbove() : 0;
				while (item && (!item->isSelectable() || !item->isVisible()))
					item = item->itemAbove();
			}
			else if(ev->key()==Key_Down) {
				//find next visible
				item = item ? item->itemBelow() : 0;
				while (item && (!item->isSelectable() || !item->isVisible()))
					item = item->itemBelow();
			}
			else if(ev->key()==Key_Home) {
				//find 1st visible
				item = list->firstChild();
				while (item && (!item->isSelectable() || !item->isVisible()))
					item = item->itemBelow();
			}
			else if(ev->key()==Key_End) {
				//find last visible
				QListViewItem *lastVisible = item;
				while (item) { // && (!item->isSelectable() || !item->isVisible()))
					item = item->itemBelow();
					if (item && item->isSelectable() && item->isVisible())
						lastVisible = item;
				}
				item = lastVisible;
			}
			if(item) {
				list->ensureItemVisible(item);
				list->setSelected(item, true);
			}
			return true;*/
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
