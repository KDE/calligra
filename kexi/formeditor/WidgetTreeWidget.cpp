/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2008-2010 Jarosław Staniek <staniek@kde.org>

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

#include "WidgetTreeWidget.h"

#include <QPainter>
#include <QHeaderView>
#include <QContextMenuEvent>

#include <KDebug>
#include <KIconLoader>
#include <KLocale>

#include "objecttree.h"
#include "form.h"
#include "container.h"
#include "widgetlibrary.h"

using namespace KFormDesigner;

WidgetTreeWidgetItem::WidgetTreeWidgetItem(WidgetTreeWidgetItem *parent, ObjectTreeItem *data)
        : QTreeWidgetItem(parent, QStringList() << data->name() << data->className())
{
    m_item = data;
}

WidgetTreeWidgetItem::WidgetTreeWidgetItem(QTreeWidget *tree, ObjectTreeItem *data)
        : QTreeWidgetItem(tree, QStringList() << (data ? data->name() : QString())
                                              << (data ? data->className() : QString()))
{
    m_item = data;
}

WidgetTreeWidgetItem::~WidgetTreeWidgetItem()
{
}

QString WidgetTreeWidgetItem::name() const
{
    if (m_item)
        return m_item->name();
    else
        return QString();
}

#warning port WidgetTreeWidgetItem::paintCell 
#if 0
void
WidgetTreeWidgetItem::paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align)
{
    int margin = listView()->itemMargin();
    if (column == 1) {
        if (!m_item)
            return;
        K3ListViewItem::paintCell(p, cg, column, width, align);
    } else {
        if (!m_item)
            return;

        p->fillRect(0, 0, width, height(), QBrush(backgroundColor()));

        if (isSelected()) {
            p->fillRect(0, 0, width, height(), QBrush(cg.highlight()));
            p->setPen(cg.highlightedText());
        }

        QFont f = listView()->font();
        p->save();
        if (isSelected())
            f.setBold(true);
        p->setFont(f);
        if (depth() == 0) { // for edit tab order dialog
            QString iconName
            = static_cast<WidgetTreeWidget*>(listView())->iconNameForClass(m_item->widget()->metaObject()->className());
            p->drawPixmap(margin, (height() - IconSize(KIconLoader::Small)) / 2 , SmallIcon(iconName));
            p->drawText(
                QRect(2*margin + IconSize(KIconLoader::Small), 0, width, height() - 1),
                Qt::AlignVCenter, m_item->name());
        } else
            p->drawText(QRect(margin, 0, width, height() - 1), Qt::AlignVCenter, m_item->name());
        p->restore();

        p->setPen(QColor(200, 200, 200)); //like in t.v.
        p->drawLine(width - 1, 0, width - 1, height() - 1);
    }

    p->setPen(QColor(200, 200, 200)); //like in t.v.
    p->drawLine(-150, height() - 1, width, height() - 1);
}

void
WidgetTreeWidgetItem::paintBranches(QPainter *p, const QColorGroup &cg, int w, int y, int h)
{
    p->eraseRect(0, 0, w, h);
    WidgetTreeWidgetItem *item = (WidgetTreeWidgetItem*)firstChild();
    if (!item || !item->m_item || !item->m_item->widget())
        return;

    p->save();
    p->translate(0, y);
    while (item) {
        p->fillRect(0, 0, w, item->height(), QBrush(item->backgroundColor()));
        p->fillRect(-150, 0, 150, item->height(), QBrush(item->backgroundColor()));
        p->save();
        p->setPen(QColor(200, 200, 200)); //like in t.v.
        p->drawLine(-150, item->height() - 1, w, item->height() - 1);
        p->restore();

        if (item->isSelected()) {
            p->fillRect(0, 0, w, item->height(), QBrush(cg.highlight()));
            p->fillRect(-150, 0, 150, item->height(), QBrush(cg.highlight()));
        }

        QString iconName
        = ((WidgetTreeWidget*)listView())->iconNameForClass(item->m_item->widget()->metaObject()->className());
        p->drawPixmap(
            (w - IconSize(KIconLoader::Small)) / 2, (item->height() - IconSize(KIconLoader::Small)) / 2 ,
            SmallIcon(iconName));

        p->translate(0, item->totalHeight());
        item = (WidgetTreeWidgetItem*)item->nextSibling();
    }
    p->restore();
}
#endif

// 2.0
// void
// WidgetTreeWidgetItem::setup()
// {
//     K3ListViewItem::setup();
//     if (!m_item)
//         setHeight(0);
// }

// 2.0
// void
// WidgetTreeWidgetItem::setOpen(bool set)
// {
//     //don't allow to collapse the node, user may be tricked because we're not displaying [+] marks
//     if (set)
//         K3ListViewItem::setOpen(set);
// }

// WidgetTreeWidget itself ----------------

WidgetTreeWidget::WidgetTreeWidget(QWidget *parent, Options options)
        : QTreeWidget(parent)
        , m_form(0)
        , m_options(options)
{
    setRootIsDecorated(false);
    setHeaderLabels(QStringList() << i18n("Widget name") << i18nc("Widget's type", "Type"));
    installEventFilter(this);

    //2.0 connect((QObject*)header(), SIGNAL(sectionHandleDoubleClicked(int)), this, SLOT(slotColumnSizeChanged(int)));
    if (!(m_options & DisableSelection)) {
        setSelectionMode(ExtendedSelection);
        connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
    }

    header()->setStretchLastSection(true);
    setAllColumnsShowFocus(true);
    //2.0 setItemMargin(3);
    setSortingEnabled(true);
}

WidgetTreeWidget::~WidgetTreeWidget()
{
}

#warning TODO WidgetTreeWidget::sizeHint() const
// QSize
// WidgetTreeWidget::sizeHint() const
// {
//     return QSize(QFontMetrics(font()).width(columnText(0) + columnText(1) + "   "),
//                  K3ListView::sizeHint().height());
// }

WidgetTreeWidgetItem* WidgetTreeWidget::selectedItem() const
{
    if (selectedItems().count() != 1)
        return 0;
    WidgetTreeWidgetItem *item = static_cast<WidgetTreeWidgetItem*>(selectedItems().first());
    return item;
}

QString WidgetTreeWidget::iconNameForClass(const QByteArray& classname) const
{
    return m_form->library()->iconName(classname);
}

// 2.0
// void
// WidgetTreeWidget::slotColumnSizeChanged(int)
// {
//     setColumnWidth(1, viewport()->width() - columnWidth(0));
// }

void WidgetTreeWidget::handleContextMenuEvent(QContextMenuEvent* e)
{
    if (!m_form)
        return;
    WidgetTreeWidgetItem *item = static_cast<WidgetTreeWidgetItem*>(itemAt(e->pos()));
    if (!item)
        return;
    QWidget *w = item->data()->widget();
    if (!w)
        return;
    m_form->createContextMenu(w, m_form->activeContainer(), e->pos());
}

void WidgetTreeWidget::contextMenuEvent(QContextMenuEvent* e)
{
    if (!(m_options & DisableContextMenu)) {
        handleContextMenuEvent(e);
    }
    QAbstractScrollArea::contextMenuEvent(e);
}

WidgetTreeWidgetItem* WidgetTreeWidget::findItem(const QString &name)
{
    QTreeWidgetItemIterator it(this);
    while (*it) {
        WidgetTreeWidgetItem *item = static_cast<WidgetTreeWidgetItem*>(*it);
        if (item->name() == name) {
            return item;
        }
        ++it;
    }
    return 0;
}

void 
WidgetTreeWidget::selectWidget(QWidget *w, bool add)
{
    blockSignals(true); // to avoid recursion

    if (!w) {
        clearSelection();
        blockSignals(false);
        return;
    }

    if (selectedItems().count() == 0)
        add = false;

    if (!add)
        clearSelection();


    QTreeWidgetItem *item = findItem(w->objectName());
    if (!add) {
        setCurrentItem(item);
        //2.0 setSelectionAnchor(item);
        item->setSelected(true);
    } else {
        item->setSelected(true);
    }

    blockSignals(false);
}

void WidgetTreeWidget::slotSelectionChanged()
{
    if (!m_form)
        return;
    const bool hadFocus = hasFocus();
    QList<QTreeWidgetItem*> list = selectedItems();
    m_form->selectFormWidget();
    foreach(QTreeWidgetItem *item, list) {
        WidgetTreeWidgetItem *it = static_cast<WidgetTreeWidgetItem*>(item);
        QWidget *w = it->data()->widget();
        if (w && !m_form->selectedWidgets()->contains(w)) {
            m_form->selectWidget(w, Form::AddToPreviousSelection | Form::DontRaise | Form::LastSelection);
        }
    }
    if (hadFocus)
        setFocus(); //restore focus
}

void WidgetTreeWidget::addItem(ObjectTreeItem *item)
{
    WidgetTreeWidgetItem *parent = findItem(item->parent()->name());
    if (!parent)
        return;

    loadTree(item, parent);
}

void WidgetTreeWidget::removeItem(ObjectTreeItem *item)
{
    if (!item)
        return;
    WidgetTreeWidgetItem *it = findItem(item->name());
    QTreeWidgetItem *root = invisibleRootItem();
    root->takeChild(root->indexOfChild(it));
    delete it;
}

void WidgetTreeWidget::renameItem(const QByteArray &oldname, const QByteArray &newname)
{
    if (findItem(newname)) {
        qWarning() << "item with name" << newname << "already exists, cannot rename";
        return;
    }
    WidgetTreeWidgetItem *item = findItem(oldname);
    if (!item)
        return;
    item->setText(0, newname);
}

void WidgetTreeWidget::setForm(Form *form)
{
    if (m_form)
        disconnect(m_form, SIGNAL(destroying()), this, SLOT(slotBeforeFormDestroyed()));
    m_form = form;
    //2.0 m_topItem = 0;
    clear();

    if (!m_form)
        return;

    connect(m_form, SIGNAL(destroying()), this, SLOT(slotBeforeFormDestroyed()));

    // Creates the hidden top Item
    //2.0 m_topItem = new WidgetTreeWidgetItem(this);
    //2.0 m_topItem->setSelectable(false);
    //2.0 m_topItem->setOpen(true);

    ObjectTree *tree = m_form->objectTree();
    QTreeWidgetItem *root = invisibleRootItem();
    loadTree(tree, static_cast<WidgetTreeWidgetItem*>(root));
    sortItems(0, Qt::AscendingOrder);

    if (!form->selectedWidgets()->isEmpty())
        selectWidget(form->selectedWidgets()->first());
    else
        selectWidget(form->widget());
}

void WidgetTreeWidget::slotBeforeFormDestroyed()
{
    setForm(0);
}

WidgetTreeWidgetItem* WidgetTreeWidget::loadTree(ObjectTreeItem *item, WidgetTreeWidgetItem *parent)
{
    if (!item)
        return 0;
    WidgetTreeWidgetItem *treeItem = new WidgetTreeWidgetItem(parent, item);
    treeItem->setExpanded(true);

//2.0
    // The item is inserted by default at the beginning, but we want it to be at the end, so we move it
//     Q3ListViewItem *last = parent->firstChild();
//     while (last->nextSibling())
//         last = last->nextSibling();
//     treeItem->moveItem(last);

    ObjectTreeList *list = item->children();
    foreach (ObjectTreeItem *titem, *list) {
        loadTree(titem, treeItem);
    }

    return treeItem;
}

#include "WidgetTreeWidget.moc"
