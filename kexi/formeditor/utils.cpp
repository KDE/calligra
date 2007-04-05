/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2007 Jaroslaw Staniek <js@iidea.pl>

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
#include <qobjectlist.h>
#include <qtabwidget.h>
#include <qtabbar.h>

#include <kdebug.h>
#include <kexiutils/utils.h>

#include "form.h"
#include "objecttree.h"
#include "utils.h"

using namespace KFormDesigner;

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
				kdDebug() << "Removing the widget " << widg->name() << "which is a child of " << w->name() << endl;
				list.remove(widg);
			}
		}
	}
}

void
KFormDesigner::installRecursiveEventFilter(QObject *object, QObject *container)
{
	if(!object || !container|| !object->isWidgetType())
		return;

	kdDebug() << "Installing event filter on widget: " << object->name() << " directed to " << container->name() << endl;
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
		tmpw = QMAX(tmpw, r.right());
		tmph = QMAX(tmph, r.bottom());
	}

	delete list;
	return QSize(tmpw, tmph) + QSize(10, 10);
}

// -----------------

HorWidgetList::HorWidgetList(QWidget *topLevelWidget)
	: WidgetList()
	, m_topLevelWidget(topLevelWidget)
{
}

HorWidgetList::~HorWidgetList()
{
}

int HorWidgetList::compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2)
{
	QWidget *w1 = static_cast<QWidget*>(item1);
	QWidget *w2 = static_cast<QWidget*>(item2);
	return w1->mapTo(m_topLevelWidget, QPoint(0,0)).x() - w2->mapTo(m_topLevelWidget, QPoint(0,0)).x();
}

// -----------------

VerWidgetList::VerWidgetList(QWidget *topLevelWidget)
	: WidgetList()
	, m_topLevelWidget(topLevelWidget)
{
}

VerWidgetList::~VerWidgetList()
{
}

int VerWidgetList::compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2)
{
	QWidget *w1 = static_cast<QWidget*>(item1);
	QWidget *w2 = static_cast<QWidget*>(item2);

	int y1, y2;
	QObject *page1 = 0;
	TabWidget *tw1 = KFormDesigner::findParent<KFormDesigner::TabWidget>(w1, "KFormDesigner::TabWidget", page1);
	if (tw1) // special case
		y1 = w1->mapTo(m_topLevelWidget, QPoint(0,0)).y() + tw1->tabBarHeight() -2 -2;
	else
		y1 = w1->mapTo(m_topLevelWidget, QPoint(0,0)).y();

	QObject *page2 = 0;
	TabWidget *tw2 = KFormDesigner::findParent<KFormDesigner::TabWidget>(w2, "KFormDesigner::TabWidget", page2);
	if (tw1 && tw2 && tw1 == tw2 && page1 != page2) {
		// this sorts widgets by tabs there're put in
		return tw1->indexOf(static_cast<QWidget*>(page1)) - tw2->indexOf(static_cast<QWidget*>(page2));
	}

	if (tw2) // special case
		y2 = w2->mapTo(m_topLevelWidget, QPoint(0,0)).y() + tw2->tabBarHeight() -2 -2;
	else
		y2 = w2->mapTo(m_topLevelWidget, QPoint(0,0)).y();
	
	kdDebug() << w1->name() << ": " << y1 << " " 
		<< " | " << w2->name() << ": " << y2 << endl;


	//kdDebug() << w1->name() << ": " << w1->mapTo(m_topLevelWidget, QPoint(0,0)) << " " << w1->y()
		//<< " | " << w2->name() << ":" /*<< w2->mapFrom(m_topLevelWidget, QPoint(0,w2->y()))*/ << " " << w2->y() << endl;
	return y1 - y2;
}

#include "utils.moc"
