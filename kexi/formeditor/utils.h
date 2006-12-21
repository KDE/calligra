/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef FORMEDITORUTILS_H
#define FORMEDITORUTILS_H

#include <q3ptrlist.h>
#include <qwidget.h>
#include <kexi_export.h>

namespace KFormDesigner {

class Form;

//! @short A list of widget pointers.
typedef Q3PtrList<QWidget> WidgetList;

//! @short An iterator for WidgetList.
typedef Q3PtrListIterator<QWidget> WidgetListIterator;

//! Helper classes for sorting widgets horizontally
class HorWidgetList : public WidgetList
{
	public:
	HorWidgetList() {;}
	virtual int compareItems(Q3PtrCollection::Item item1, Q3PtrCollection::Item item2)
	{
		QWidget *w1 = static_cast<QWidget*>(item1);
		QWidget *w2 = static_cast<QWidget*>(item2);

		if(w1->x() < w2->x())
			return -1;
		if(w1->x() > w2->x())
			return 1;
		return 0; // item1 == item2
	}
};

//! Helper classes for sorting widgets vertically
class VerWidgetList : public WidgetList
{
	public:
	VerWidgetList() {;}
	virtual int compareItems(Q3PtrCollection::Item item1, Q3PtrCollection::Item item2)
	{
		QWidget *w1 = static_cast<QWidget*>(item1);
		QWidget *w2 = static_cast<QWidget*>(item2);

		if(w1->y() < w2->y())
			return -10;
		if(w1->y() > w2->y())
			return 1;
		return 0; // item1 == item2
	}
};

/*! This function is used to remove all the child widgets from a list, and
  keep only the "toplevel" ones. */
KFORMEDITOR_EXPORT void removeChildrenFromList(WidgetList &list);

/*! This helper function install an event filter on \a object and all of its
  children, directed to \a container.
  This is necessary to filter events for composed widgets. */
KFORMEDITOR_EXPORT void installRecursiveEventFilter(QObject *object, QObject *container);

/*! This helper function removes an event filter installed before
  on \a object and all of its children.
  This is necessary to filter events for composed widgets. */
KFORMEDITOR_EXPORT void removeRecursiveEventFilter(QObject *object, QObject *container);

KFORMEDITOR_EXPORT void setRecursiveCursor(QWidget *w, Form *form);

/*! \return the size of \a w children. This can be used eg to get widget's sizeHint. */
KFORMEDITOR_EXPORT QSize getSizeFromChildren(QWidget *widget, const char *inheritClass="QWidget");

}

#endif

