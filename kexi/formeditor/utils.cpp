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

#include <QApplication>
#include <QClipboard>
#include <QCursor>
#include <QDomDocument>
#include <QMimeData>
#include <QTabWidget>
#include <QTabBar>
#include <QPen>
#include <QPainter>

#include <kdebug.h>
#include <kexiutils/utils.h>

#include "form.h"
#include "formIO.h"
#include "objecttree.h"
#include "utils.h"

using namespace KFormDesigner;

void
KFormDesigner::removeChildrenFromList(QWidgetList &list)
{
    QSet<QWidget*> toRemove;
    foreach (QWidget *w, list) {
        // If any widget in the list is a child of this widget, we remove it from the list
        foreach (QWidget *widg, list) {
            if ((w != widg) && (w->findChild<QWidget*>(widg->objectName()))) {
                kDebug() << "Removing the widget " << widg->objectName()
                    << "which is a child of " << w->objectName();
                toRemove.insert(widg);
            }
        }
    }
    QSet<QWidget*> all(list.toSet());
    all.subtract(toRemove);
    list = all.toList();
}

void
KFormDesigner::setRecursiveCursor(QWidget *w, Form *form)
{
    ObjectTreeItem *tree = form->objectTree()->lookup(w->objectName());
    if (tree && ((tree->modifiedProperties()->contains("cursor")) || !tree->children()->isEmpty())
            && !w->inherits("QLineEdit") && !w->inherits("QTextEdit")
       ) //fix weird behaviour
        return; // if the user has set a cursor for this widget or this is a container, don't change it

//2.0    if (w->testAttribute(Qt::WA_SetCursor))
    w->setCursor(Qt::ArrowCursor);

    const QList<QWidget*> list(w->findChildren<QWidget*>());
    foreach(QWidget *widget, list) {
        widget->setCursor(Qt::ArrowCursor);
    }
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

class HorizontalWidgetList::LessThan
{
public:
    LessThan(QWidget *topLevelWidget)
     : m_topLevelWidget(topLevelWidget)
    {
    }
    bool operator()(QWidget *w1, QWidget *w2) {
        return w1->mapTo(m_topLevelWidget, QPoint(0, 0)).x()
             - w2->mapTo(m_topLevelWidget, QPoint(0, 0)).x();
    }
    QWidget *m_topLevelWidget;
};

HorizontalWidgetList::HorizontalWidgetList(QWidget *topLevelWidget)
        : CustomSortableWidgetList()
        , m_lessThan(new LessThan(topLevelWidget))
{
}

HorizontalWidgetList::HorizontalWidgetList(const HorizontalWidgetList& list)
    : CustomSortableWidgetList(list)
    , m_lessThan(new LessThan(list.m_lessThan->m_topLevelWidget))
{
}


HorizontalWidgetList::~HorizontalWidgetList()
{
    delete m_lessThan;
}

void HorizontalWidgetList::sort()
{
    qSort(begin(), end(), *m_lessThan);
}

// -----------------

class VerticalWidgetList::LessThan
{
public:
    LessThan(QWidget *topLevelWidget)
     : m_topLevelWidget(topLevelWidget)
    {
    }
    bool operator()(QWidget *w1, QWidget *w2)
    {
        int y1, y2;
        QObject *page1 = 0;
        TabWidget *tw1 = KFormDesigner::findParent<KFormDesigner::TabWidget>(
            w1, "KFormDesigner::TabWidget", page1);
        if (tw1) // special case
            y1 = w1->mapTo(m_topLevelWidget, QPoint(0, 0)).y() + tw1->tabBarHeight() - 2 - 2;
        else
            y1 = w1->mapTo(m_topLevelWidget, QPoint(0, 0)).y();

        QObject *page2 = 0;
        TabWidget *tw2 = KFormDesigner::findParent<KFormDesigner::TabWidget>(
            w2, "KFormDesigner::TabWidget", page2);
        if (tw1 && tw2 && tw1 == tw2 && page1 != page2) {
            // this sorts widgets by tabs there're put in
            return tw1->indexOf(static_cast<QWidget*>(page1)) < tw2->indexOf(static_cast<QWidget*>(page2));
        }

        if (tw2) // special case
            y2 = w2->mapTo(m_topLevelWidget, QPoint(0, 0)).y() + tw2->tabBarHeight() - 2 - 2;
        else
            y2 = w2->mapTo(m_topLevelWidget, QPoint(0, 0)).y();

        kDebug() << w1->objectName() << ": " << y1 << " "
            << " | " << w2->objectName() << ": " << y2;


        //kDebug() << w1->name() << ": " << w1->mapTo(m_topLevelWidget, QPoint(0,0)) << " " << w1->y()
        //<< " | " << w2->name() << ":" /*<< w2->mapFrom(m_topLevelWidget, QPoint(0,w2->y()))*/ << " " << w2->y();
        return y1 < y2;
    }
    QWidget *m_topLevelWidget;
};

VerticalWidgetList::VerticalWidgetList(QWidget *topLevelWidget)
        : CustomSortableWidgetList()
        , m_lessThan(new LessThan(topLevelWidget))
{
}

VerticalWidgetList::VerticalWidgetList(const VerticalWidgetList& list)
    : CustomSortableWidgetList(list)
    , m_lessThan(new LessThan(list.m_lessThan->m_topLevelWidget))
{
}

VerticalWidgetList::~VerticalWidgetList()
{
    delete m_lessThan;
}

void VerticalWidgetList::sort()
{
    qSort(begin(), end(), *m_lessThan);
}

// ----

QMimeData *KFormDesigner::deepCopyOfClipboardData()
{
    //QClipboard *cb = QApplication::clipboard();
    QMimeData *data = new QMimeData();
    foreach(const QString& format, data->formats()) {
        data->setData(format, data->data(format));
    }
    return data;
}

void KFormDesigner::copyToClipboard(const QString& xml)
{
    kDebug() << xml;
    QMimeData *data = new QMimeData();
    data->setText(xml);
    data->setData(KFormDesigner::mimeType(), xml.toUtf8());
    QClipboard *cb = QApplication::clipboard();
    cb->setMimeData(data);
}

void KFormDesigner::widgetsToXML(QDomDocument& doc, 
    QHash<QByteArray, QByteArray>& containers,
    QHash<QByteArray, QByteArray>& parents,
    const Form& form, const QWidgetList &list)
{
    containers.clear();
    parents.clear();
    doc = QDomDocument("UI");
    doc.appendChild(doc.createElement("UI"));
    QDomElement parent = doc.firstChildElement("UI");

    QWidgetList topLevelList(list);
    KFormDesigner::removeChildrenFromList(topLevelList);

    foreach (QWidget *w, topLevelList) {
        ObjectTreeItem *item = form.objectTree()->lookup(w->objectName());
        if (!item)
            return;
        Container *c = form.parentContainer(item->widget());
        if (!c)
            return;

        // We need to store both parentContainer and parentWidget as they may be different (eg for TabWidget page)
        containers.insert(
            item->name().toLatin1(),
            c->widget()->objectName().toLatin1().constData()
        );
        parents.insert(
            item->name().toLatin1(),
            item->parent()->name().toLatin1()
        );
        FormIO::saveWidget(item, parent, doc);
#ifdef KFD_SIGSLOTS
        form.connectionBuffer()->saveAllConnectionsForWidget(
            item->widget()->objectName(), doc);
#endif
    }

    FormIO::cleanClipboard(parent);
}

//-----------------------------

class ActionGroup::Private {
    public:
        Private() {}
        QHash<QString, QAction*> actions;
};

ActionGroup::ActionGroup( QObject * parent )
    : QActionGroup(parent)
    , d( new Private )
{
}

ActionGroup::~ActionGroup()
{
    delete d;
}

void ActionGroup::addAction(QAction* action)
{
    QActionGroup::addAction(action);
    d->actions.insert(action->objectName(), action);
}

QAction *ActionGroup::action(const QString& name) const
{
    return d->actions.value(name);
}

//-----------------------------

int KFormDesigner::alignValueToGrid(int value, int gridSize)
{
    if ((value % gridSize * 2) < gridSize)
        return value / gridSize * gridSize;
    return (value / gridSize + 1) * gridSize;
}

//-----------------------------

void KFormDesigner::paintWidgetFrame(QPainter& p, const QRect& geometry)
{
    QColor c1(Qt::white);
    c1.setAlpha(100);
    QColor c2(Qt::black);
    c2.setAlpha(100);
    QRect r(geometry);
    r.setWidth(r.width()-1);
    r.setHeight(r.height()-1);
    QPen pen1(c1, 1, Qt::DashLine);
    QPen pen2(c2, 1, Qt::DashLine);
    p.setPen(pen1);
    p.drawRect(r);
    p.setPen(pen2);
    p.drawRect(r);
}

#include "utils.moc"
