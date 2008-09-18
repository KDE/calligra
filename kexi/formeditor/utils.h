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

#ifndef FORMEDITORUTILS_H
#define FORMEDITORUTILS_H

#include <q3ptrlist.h>
#include <qtabbar.h>
#include <qtabwidget.h>
#include <kexi_export.h>

//! @todo replace QTabWidget by KTabWidget after the bug with & is fixed:
#define TabWidgetBase QTabWidget
//#define USE_KTabWidget //todo: uncomment

namespace KFormDesigner
{

class Form;

/*! \return parent object of \a o that inherits \a className or NULL if no such parent
 If the parent is found, \a prevPrev is set to a child of child of the parent,
 what for TabWidget means the page widget. */
template<class type>
type* findParent(QObject* o, const char* className, QObject* &prevPrev)
{
    if (!o || !className || className[0] == '\0')
        return 0;
    QObject *prev = o;
    while (((o = o->parent())) && !o->inherits(className)) {
        prevPrev = prev;
        prev = o;
    }
    return static_cast<type*>(o);
}

//! A tab widget providing information about height of the tab bar.
class KFORMEDITOR_EXPORT TabWidget : public TabWidgetBase
{
    Q_OBJECT
public:
    TabWidget(QWidget *parent)
            : TabWidgetBase(parent) {}
    virtual ~TabWidget() {}
    int tabBarHeight() const {
        return tabBar()->height();
    }
};

//! @short A list of widget pointers.
typedef Q3PtrList<QWidget> WidgetList;

//! @short An iterator for WidgetList.
typedef Q3PtrListIterator<QWidget> WidgetListIterator;

//! @short A helper for sorting widgets horizontally
class HorWidgetList : public WidgetList
{
public:
    HorWidgetList(QWidget *topLevelWidget);
    virtual ~HorWidgetList();
protected:
    virtual int compareItems(Q3PtrCollection::Item item1, Q3PtrCollection::Item item2);
    QWidget *m_topLevelWidget;
};

//! @short A helper for sorting widgets vertically
class VerWidgetList : public WidgetList
{
public:
    VerWidgetList(QWidget *topLevelWidget);
    virtual ~VerWidgetList();
protected:
    virtual int compareItems(Q3PtrCollection::Item item1, Q3PtrCollection::Item item2);
    QWidget *m_topLevelWidget;
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
KFORMEDITOR_EXPORT QSize getSizeFromChildren(QWidget *widget, const char *inheritClass = "QWidget");

}

#endif

