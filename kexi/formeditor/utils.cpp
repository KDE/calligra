/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2007 Jarosław Staniek <staniek@kde.org>

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
    for (WidgetListIterator it(list); it.current() != 0; ++it)  {
        QWidget *w = it.current();

        // If any widget in the list is a child of this widget, we remove it from the list
        for (WidgetListIterator it2(list); it2.current() != 0; ++it2) {
            QWidget *widg = it2.current();
            if ((w != widg) && (w->findChild<QWidget*>(widg->objectName()))) {
                kDebug() << "Removing the widget " << widg->objectName() << "which is a child of " << w->objectName();
                list.remove(widg);
            }
        }
    }
}

void
KFormDesigner::installRecursiveEventFilter(QObject *object, QObject *container)
{
    if (!object || !container || !object->isWidgetType())
        return;

    kDebug() << "Installing event filter on widget: " << object->objectName() << " directed to " << container->objectName();
    object->installEventFilter(container);
    if (((QWidget*)object)->testAttribute(Qt::WA_SetCursor))
        ((QWidget*)object)->setCursor(QCursor(Qt::ArrowCursor));

    const QObjectList list(object->children());
    foreach(QObject *obj, list)
    installRecursiveEventFilter(obj, container);
}

void
KFormDesigner::removeRecursiveEventFilter(QObject *object, QObject *container)
{
    object->removeEventFilter(container);
    if (!object->isWidgetType())
        return;

    const QObjectList list(object->children());
    foreach(QObject *obj, list)
    removeRecursiveEventFilter(obj, container);
}

void
KFormDesigner::setRecursiveCursor(QWidget *w, Form *form)
{
    ObjectTreeItem *tree = form->objectTree()->lookup(w->objectName());
    if (tree && ((tree->modifiedProperties()->contains("cursor")) || !tree->children()->isEmpty())
            && !w->inherits("QLineEdit") && !w->inherits("QTextEdit")
       ) //fix weird behaviour
        return; // if the user has set a cursor for this widget or this is a container, don't change it

    if (w->testAttribute(Qt::WA_SetCursor))
        w->setCursor(Qt::ArrowCursor);

    const QList<QWidget*> list(w->findChildren<QWidget*>());
    foreach(QWidget *widget, list)
    widget->setCursor(Qt::ArrowCursor);
}

QSize
KFormDesigner::getSizeFromChildren(QWidget *w, const char *inheritClass)
{
    int tmpw = 0, tmph = 0;
    const QList<QWidget*> list(w->findChildren<QWidget*>());
    foreach(QWidget *widget, list) {
        if (widget->inherits(inheritClass)) {
            tmpw = qMax(tmpw, widget->geometry().right());
            tmph = qMax(tmph, widget->geometry().bottom());
        }
    }
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

int HorWidgetList::compareItems(Q3PtrCollection::Item item1, Q3PtrCollection::Item item2)
{
    QWidget *w1 = static_cast<QWidget*>(item1);
    QWidget *w2 = static_cast<QWidget*>(item2);
    return w1->mapTo(m_topLevelWidget, QPoint(0, 0)).x() - w2->mapTo(m_topLevelWidget, QPoint(0, 0)).x();
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

int VerWidgetList::compareItems(Q3PtrCollection::Item item1, Q3PtrCollection::Item item2)
{
    QWidget *w1 = static_cast<QWidget*>(item1);
    QWidget *w2 = static_cast<QWidget*>(item2);

    int y1, y2;
    QObject *page1 = 0;
    TabWidget *tw1 = KFormDesigner::findParent<KFormDesigner::TabWidget>(w1, "KFormDesigner::TabWidget", page1);
    if (tw1) // special case
        y1 = w1->mapTo(m_topLevelWidget, QPoint(0, 0)).y() + tw1->tabBarHeight() - 2 - 2;
    else
        y1 = w1->mapTo(m_topLevelWidget, QPoint(0, 0)).y();

    QObject *page2 = 0;
    TabWidget *tw2 = KFormDesigner::findParent<KFormDesigner::TabWidget>(w2, "KFormDesigner::TabWidget", page2);
    if (tw1 && tw2 && tw1 == tw2 && page1 != page2) {
        // this sorts widgets by tabs there're put in
        return tw1->indexOf(static_cast<QWidget*>(page1)) - tw2->indexOf(static_cast<QWidget*>(page2));
    }

    if (tw2) // special case
        y2 = w2->mapTo(m_topLevelWidget, QPoint(0, 0)).y() + tw2->tabBarHeight() - 2 - 2;
    else
        y2 = w2->mapTo(m_topLevelWidget, QPoint(0, 0)).y();

    kDebug() << w1->objectName() << ": " << y1 << " "
    << " | " << w2->objectName() << ": " << y2;


    //kDebug() << w1->name() << ": " << w1->mapTo(m_topLevelWidget, QPoint(0,0)) << " " << w1->y()
    //<< " | " << w2->name() << ":" /*<< w2->mapFrom(m_topLevelWidget, QPoint(0,w2->y()))*/ << " " << w2->y();
    return y1 - y2;
}

#include "utils.moc"
