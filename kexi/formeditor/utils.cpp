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

#include <qcursor.h>
#include <qobject.h>
#include <kdebug.h>

#include "form.h"
#include "objecttree.h"
#include "utils.h"

using namespace KFormDesigner;

/// Helper function to clear a list (by removing all children)
void
KFormDesigner::removeChildrenFromList(WidgetList &list)
{
	for(WidgetListIterator it(list); it.current() != 0; ++it)  {
		QWidget *w = it.current();

		// If any widget in the list is a child of this widget, we remove it from the list
		for(WidgetListIterator it2(list); it2.current() != 0; ++it2) {
			QWidget *widg = it2.current();
			if((w != widg) && (w->child(widg->name())))
			{
				kDebug() << "Removing the widget " << widg->name() << "which is a child of " << w->name() << endl;
				list.remove(widg);
			}
		}
	}
}

//// Helper functions for event filtering on composed widgets
void
KFormDesigner::installRecursiveEventFilter(QObject *object, QObject *container)
{
	if(!object || !container|| !object->isWidgetType())
		return;

	kDebug() << "Installing event filter on widget: " << object->name() << " directed to " << container->name() << endl;
	object->installEventFilter(container);
	if(((QWidget*)object)->ownCursor())
		((QWidget*)object)->setCursor(QCursor(Qt::ArrowCursor));

	if(!object->children())
		return;

	QObjectList list = *(object->children());
	for(QObject *obj = list.first(); obj; obj = list.next())
		installRecursiveEventFilter(obj, container);
}

void
KFormDesigner::removeRecursiveEventFilter(QObject *object, QObject *container)
{
	object->removeEventFilter(container);
	if(!object->isWidgetType())
		return;
	if(!object->children())
		return;

	QObjectList list = *(object->children());
	for(QObject *obj = list.first(); obj; obj = list.next())
		removeRecursiveEventFilter(obj, container);
}

void
KFormDesigner::setRecursiveCursor(QWidget *w, Form *form)
{
	ObjectTreeItem *tree = form->objectTree()->lookup(w->name());
	if(tree && ((tree->modifiedProperties()->contains("cursor")) || !tree->children()->isEmpty())
		&& !w->inherits("QLineEdit") && !w->inherits("QTextEdit")
		) //fix weird behaviour
		return; // if the user has set a cursor for this widget or this is a container, don't change it

	if(w->ownCursor())
		w->setCursor(Qt::ArrowCursor);

	QObjectList *l = w->queryList( "QWidget" );
	for(QObject *o = l->first(); o; o = l->next())
		((QWidget*)o)->setCursor(Qt::ArrowCursor);
	delete l;
}

QSize
KFormDesigner::getSizeFromChildren(QWidget *w, const char *inheritClass)
{
	int tmpw = 0, tmph = 0;
	QObjectList *list = w->queryList(inheritClass, 0, false, false);
	for(QObject *o = list->first(); o; o = list->next()) {
		QRect  r = ((QWidget*)o)->geometry();
		tmpw = qMax(tmpw, r.right());
		tmph = qMax(tmph, r.bottom());
	}

	delete list;
	return QSize(tmpw, tmph) + QSize(10, 10);
}

