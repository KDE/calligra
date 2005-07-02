/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>

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

#include "widget.h"
#include "property.h"
#include "editoritem.h"

#include <qpainter.h>
#include <qvariant.h>

#ifdef QT_ONLY
#include <qlistview.h>
#else
#include <klistview.h>
#include <kdebug.h>
#endif

using namespace KoProperty;

namespace KoProperty {
class WidgetPrivate
{
	public:
		WidgetPrivate()
		: property(0)
		, leaveTheSpaceForRevertButton(false)
		, hasBorders(true)
		{}
		~WidgetPrivate() {}

		Property    *property;
		bool  leaveTheSpaceForRevertButton;
		bool  hasBorders;
};
}

Widget::Widget(Property *property, QWidget *parent, const char *name)
 : QWidget(parent, name)
{
	d = new WidgetPrivate();
	d->property = property;
}

Widget::~Widget()
{
	delete d;
}

Property*
Widget::property() const
{
	return d->property;
}

void
Widget::setProperty(Property *property)
{
	d->property = property;
	if(property)
		setValue(property->value(), false);
	//if(property->type() == ValueFromList)
	//	setValueList(property->valueList());
}

void
Widget::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
	p->eraseRect(r);
	QRect rect(r);
	rect.setLeft(rect.left()+KPROPEDITOR_ITEM_MARGIN);
//	if (d->hasBorders)
//		rect.setTop(rect.top()+1); //+1 to have the same vertical position as editor
//	else
//		rect.setHeight(rect.height()-1); //don't place over listviews's border
	p->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine, value.toString());
}

void
Widget::undo()
{
	if(d->property)
		d->property->resetValue();
}

bool
Widget::eventFilter(QObject*, QEvent* e)
{
	if(e->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = static_cast<QKeyEvent*>(e);
		if(ev->key() == Key_Escape)
		{
			emit rejectInput(this);
			return true;
		}
		else if((ev->key() == Key_Return) || (ev->key() == Key_Enter))
		{
			// should apply when autosync == false
			emit acceptInput(this);
			return true;
		}
		KListView *list = static_cast<KListView*>(parentWidget()->parentWidget());
		KListViewItem *item = static_cast<KListViewItem*>(list->itemAt(mapToParent(QPoint(2,2))));

		if (item) {
			if(ev->key() == Key_Up && ev->state() != ControlButton)
			{
				if(item->itemAbove())
					list->setCurrentItem(item->itemAbove());
				return true;
			}
			else if(ev->key() == Key_Down && ev->state() != ControlButton)
			{
				if(item->itemBelow())
					list->setCurrentItem(item->itemBelow());
				return true;
			}
		}
	}

	return false;
}

void
Widget::setFocusWidget(QWidget*focusProxy)
{
	if (focusProxy && focusProxy->focusPolicy() != NoFocus) {
		setFocusProxy(focusProxy);
		focusProxy->installEventFilter(this);
	}
}

bool
Widget::leavesTheSpaceForRevertButton() const
{
	return d->leaveTheSpaceForRevertButton;
}

void
Widget::setLeavesTheSpaceForRevertButton(bool set)
{
	d->leaveTheSpaceForRevertButton = set;
}

void
Widget::setHasBorders(bool set)
{
	d->hasBorders = set;
}

bool
Widget::hasBorders() const
{
	return d->hasBorders;
}

#include "widget.moc"
