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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef FORMEDITORUTILS_H
#define FORMEDITORUTILS_H

#include <qptrlist.h>
#include <qwidget.h>

namespace KFormDesigner {

class Form;
typedef QPtrList<QWidget> WidgetList;
typedef QPtrListIterator<QWidget> WidgetListIterator;

// Helper classes for sorting widgets
class HorWidgetList : public WidgetList
{
	public:
	HorWidgetList() {;}
	virtual int compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2)
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

class VerWidgetList : public WidgetList
{
	public:
	VerWidgetList() {;}
	virtual int compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2)
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

}

#endif

