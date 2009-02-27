/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2007-2009 Jarosław Staniek <staniek@kde.org>

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

#include <QHash>
#include <QTabBar>
#include <QTabWidget>
#include <QActionGroup>
#include <kexi_export.h>

//! @todo replace QTabWidget by KTabWidget after the bug with & is fixed:
#define TabWidgetBase QTabWidget
//#define USE_KTabWidget //todo: uncomment

class QMimeData;
class QDomDocument;

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

//! @short A common interface for HorizontalWidgetList and VerticalWidgetList
class CustomSortableWidgetList : public QWidgetList
{
public:
    CustomSortableWidgetList() : QWidgetList() {}
    virtual ~CustomSortableWidgetList() {}
    virtual void sort() {}
};

//! @short A helper for sorting widgets horizontally
class HorizontalWidgetList : public CustomSortableWidgetList
{
public:
    HorizontalWidgetList(QWidget *topLevelWidget);
    virtual ~HorizontalWidgetList();
    virtual void sort();
protected:
    class LessThan;
    LessThan *m_lessThan;
};

//! @short A helper for sorting widgets vertically
class VerticalWidgetList : public CustomSortableWidgetList
{
public:
    VerticalWidgetList(QWidget *topLevelWidget);
    virtual ~VerticalWidgetList();
    virtual void sort();
protected:
    class LessThan;
    LessThan *m_lessThan;
};

/*! This function is used to remove all the child widgets from a list, and
  keep only the "toplevel" ones. */
KFORMEDITOR_EXPORT void removeChildrenFromList(QWidgetList &list);

/*! This helper function install an event filter on \a object and all of its
  children, directed to \a container.
  This is necessary to filter events for composed widgets. */
KFORMEDITOR_EXPORT void installRecursiveEventFilter(QObject *object, QObject *container);

/*! This helper function removes an event filter installed before
  on \a object and all of its children.
  This is necessary to filter events for composed widgets. */
KFORMEDITOR_EXPORT void removeRecursiveEventFilter(QObject *object, QObject *container);

KFORMEDITOR_EXPORT void setRecursiveCursor(QWidget *w, Form *form);

//! \return the size of \a w children
/*! This can be used eg to get widget's sizeHint. */
KFORMEDITOR_EXPORT QSize getSizeFromChildren(QWidget *widget, const char *inheritClass = "QWidget");

//! @return mimetype for the forms XML format
inline QString mimeType() { return "application/x-kexi-form"; }

//! @returns deep copy of the current clipboard contents (for all formats)
KFORMEDITOR_EXPORT QMimeData *deepCopyOfClipboardData();

//! Copies @a xml data to the clipboard both in the plain text format and forms XML format
KFORMEDITOR_EXPORT void copyToClipboard(const QString& xml);

//! Recursively saves widget list @a list and form @a form to @a doc XML document
/*! @a containers hash is filled with containers found within the widget list,
 and @a parents is filled with the parent widgets found within the widget list. 
 USed in DeleteWidgetCommand ctor. */
KFORMEDITOR_EXPORT void widgetsToXML(QDomDocument& doc, 
    QHash<QByteArray, QByteArray>& containers,
    QHash<QByteArray, QByteArray>& parents,
    const Form& form, const QWidgetList &list);

//! QActionGroup extended by action() method.
class KFORMEDITOR_EXPORT ActionGroup : public QActionGroup
{
    public:
        ActionGroup( QObject * parent );
        ~ActionGroup();
        //! Reimplemented.
        void addAction(QAction* action);
        QAction *action(const QString& name) const;
    private:
        class Private;
        Private * const d;
};

}

#endif

