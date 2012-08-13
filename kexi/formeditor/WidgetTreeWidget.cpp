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
#include <KIconEffect>
#include <KLocale>

#include <KoIcon.h>

#include "objecttree.h"
#include "form.h"
#include "container.h"
#include "widgetlibrary.h"
#include "commands.h"

using namespace KFormDesigner;

WidgetTreeWidgetItem::WidgetTreeWidgetItem(WidgetTreeWidgetItem *parent, ObjectTreeItem *data,
    LoadTreeFlags loadTreeFlags, int forcedTabPageIndex, const QString& forcedTabPageName)
        : QTreeWidgetItem(parent)
        , m_data(data)
        , m_loadTreeFlags(loadTreeFlags)
{
    init(forcedTabPageIndex, forcedTabPageName);
}

WidgetTreeWidgetItem::WidgetTreeWidgetItem(QTreeWidget *tree, ObjectTreeItem *data,
    LoadTreeFlags loadTreeFlags, int forcedTabPageIndex, const QString& forcedTabPageName)
        : QTreeWidgetItem(tree)
        , m_data(data)
        , m_loadTreeFlags(loadTreeFlags)
{
    init(forcedTabPageIndex, forcedTabPageName);
}

WidgetTreeWidgetItem::~WidgetTreeWidgetItem()
{
}

void WidgetTreeWidgetItem::init(int forcedTabPageIndex, const QString& forcedTabPageName)
{
    if (m_data) {
        initTextAndIcon(forcedTabPageIndex, forcedTabPageName);
    }
}

void WidgetTreeWidgetItem::initTextAndIcon(int forcedTabPageIndex, const QString& forcedTabPageName)
{
    QString itemName;
    QString itemClass;
    QString itemIconName;
    Qt::ItemFlags itemFlags = flags();
    WidgetTreeWidget *widgetTreeWidget = qobject_cast<WidgetTreeWidget*>(treeWidget());
    ObjectTreeItem* selectable = widgetTreeWidget ? widgetTreeWidget->selectableItem(m_data) : m_data;
    if (selectable != m_data) {
        kDebug() << "****" << (m_loadTreeFlags & LoadTreeForAddedTabPage) << selectable->widget();
        if (qobject_cast<QTabWidget*>(selectable->widget())) {
            // tab widget's page
            const QTabWidget* tabWidget = qobject_cast<QTabWidget*>(selectable->widget());
            int tabIndex = tabWidget->indexOf(m_data->widget());
            if (tabIndex == -1 && (m_loadTreeFlags & LoadTreeForAddedTabPage)) { // tab appended
                if (forcedTabPageIndex >= 0)
                    tabIndex = forcedTabPageIndex;
                else
                    tabIndex = tabWidget->count();
            }
            kDebug() << tabIndex;
            if (tabIndex >= 0) {
                if (forcedTabPageName.isEmpty()) {
                    itemName = tabWidget->tabText(tabIndex);
                    if (itemName.isEmpty()) {
                        itemName = forcedTabPageName;
                        if (itemName.isEmpty())
                            itemName = i18n("Page %1", tabIndex + 1);
                    }
                    else {
                        itemName.replace('&', "");
                    }
                }
                else
                    itemName = forcedTabPageName;
                itemClass = i18nc("Tab widget's page", "Tab Page");
                m_customSortingKey = QString("tab%1").arg(tabIndex);
                kDebug() << "m_customSortingKey" << m_customSortingKey;
                itemFlags |= Qt::ItemIsSelectable;
                itemFlags ^= Qt::ItemIsSelectable;
                itemIconName = koIconName("tabwidget_tab");
            }
        }
    }
    // defaults:
    if (itemName.isEmpty()) {
        itemName = m_data->name();
    }
    if (itemClass.isEmpty()) {
        itemClass = m_data->className();
    }
    if (itemIconName.isEmpty()) {
        if (widgetTreeWidget) {
            itemIconName = widgetTreeWidget->iconNameForClass(m_data->widget()->metaObject()->className());
        }
    }
    // set:
    if (itemFlags != flags()) {
        setFlags(itemFlags);
    }
    setText(0, itemName);
    setText(1, itemClass);
    if (!itemIconName.isEmpty()) {
        QPixmap icon(SmallIcon(itemIconName));
        if (!(itemFlags & Qt::ItemIsSelectable)) {
            KIconEffect::semiTransparent(icon);
        }
        setIcon(0, icon);
    }
    if (!(itemFlags & Qt::ItemIsSelectable)) {
        setForeground(0, treeWidget()->palette().color(QPalette::Disabled, QPalette::Text));
        setForeground(1, treeWidget()->palette().color(QPalette::Disabled, QPalette::Text));
    }
}

QString WidgetTreeWidgetItem::name() const
{
    if (m_data)
        return m_data->name();
    else
        return QString();
}

bool WidgetTreeWidgetItem::operator<( const QTreeWidgetItem & other ) const
{
    const WidgetTreeWidgetItem *otherItem = dynamic_cast<const WidgetTreeWidgetItem*>(&other);
    if (!otherItem)
        return QTreeWidgetItem::operator<(other);
    return m_customSortingKey < otherItem->customSortingKey();
}

#ifdef __GNUC__
#warning port WidgetTreeWidgetItem::paintCell
#else
#pragma WARNING( port WidgetTreeWidgetItem::paintCell )
#endif
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
        , m_slotSelectionChanged_enabled(true)
        , m_selectWidget_enabled(true)
{
    setRootIsDecorated(false);
    setHeaderLabels(QStringList() << i18n("Widget name") << i18nc("Widget's type", "Type"));
    installEventFilter(this);

    if (!(m_options & DisableSelection)) {
        setSelectionMode(ExtendedSelection);
        connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
    }

    setAllColumnsShowFocus(true);
    setExpandsOnDoubleClick(false);
    setIndentation(indentation() / 2);
}

WidgetTreeWidget::~WidgetTreeWidget()
{
}

#ifdef __GNUC__
#warning TODO WidgetTreeWidget::sizeHint() const
#else
#pragma WARNING( TODO WidgetTreeWidget::sizeHint() const )
#endif
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

ObjectTreeItem* WidgetTreeWidget::selectableItem(ObjectTreeItem* item)
{
    return m_form->library()->selectableItem(item);
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
    WidgetTreeWidgetItem *newItem = static_cast<WidgetTreeWidgetItem*>(tryToAlterSelection(item));
    QWidget *w = newItem->data()->widget();
    if (!w)
        return;
    m_form->createContextMenu(w, m_form->activeContainer(), e->pos(), Form::WidgetTreeContextMenuTarget);
}

void WidgetTreeWidget::contextMenuEvent(QContextMenuEvent* e)
{
    if (!(m_options & DisableContextMenu)) {
        handleContextMenuEvent(e);
    }
    QAbstractScrollArea::contextMenuEvent(e);
}

WidgetTreeWidgetItem* WidgetTreeWidget::findItem(const QString& name)
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

WidgetTreeWidgetItem* WidgetTreeWidget::findItemByFirstColumn(const QString& text)
{
    QTreeWidgetItemIterator it(this);
    while (*it) {
        WidgetTreeWidgetItem *item = static_cast<WidgetTreeWidgetItem*>(*it);
        if (item->text(0) == text) {
            return item;
        }
        ++it;
    }
    return 0;
}

void WidgetTreeWidget::selectWidget(QWidget *w, KFormDesigner::Form::WidgetSelectionFlags flags)
{
    if (!m_selectWidget_enabled)
        return;
    blockSignals(true); // to avoid recursion

    if (!w) {
        clearSelection();
        blockSignals(false);
        return;
    }

    if (selectedItems().count() == 0) {
        flags |= Form::ReplacePreviousSelection;
    }

    if ((flags & Form::ReplacePreviousSelection))
        clearSelection();

    QTreeWidgetItem *item = findItem(w->objectName());
    if ((flags & Form::ReplacePreviousSelection)) {
        setCurrentItem(item);
        //2.0 setSelectionAnchor(item);
        item->setSelected(true);
    } else {
        item->setSelected(true);
    }

    blockSignals(false);
}

void WidgetTreeWidget::selectWidgetForItem(QTreeWidgetItem *item)
{
    WidgetTreeWidgetItem *it = dynamic_cast<WidgetTreeWidgetItem*>(item);
    if (!it)
        return;
    QWidget *w = it->data()->widget();
    if (w && !m_form->selectedWidgets()->contains(w)) {
        m_form->selectWidget(w, Form::AddToPreviousSelection | Form::DontRaise | Form::LastSelection);
    }
}

void WidgetTreeWidget::activateTabPageIfNeeded(QTreeWidgetItem* item)
{
    WidgetTreeWidgetItem *childItem = dynamic_cast<WidgetTreeWidgetItem*>(item);
    if (!childItem)
        return;
    WidgetTreeWidgetItem *parentItem = dynamic_cast<WidgetTreeWidgetItem*>(item->parent());
    while (childItem && parentItem) {
        if (parentItem && qobject_cast<QTabWidget*>(parentItem->data()->widget())) {
            qobject_cast<QTabWidget*>(parentItem->data()->widget())->setCurrentWidget(
                childItem->data()->widget());
        }
        childItem = parentItem;
        parentItem = dynamic_cast<WidgetTreeWidgetItem*>(parentItem->parent());
    }
}

QTreeWidgetItem* WidgetTreeWidget::tryToAlterSelection(QTreeWidgetItem* current)
{
    activateTabPageIfNeeded(current);

    if (   current
        && !(current->flags() & Qt::ItemIsSelectable)
        && current->parent()
        && (current->parent()->flags() & Qt::ItemIsSelectable)
       )
    {
        m_slotSelectionChanged_enabled = false;
        foreach (QTreeWidgetItem *selectedItem, selectedItems()) {
            selectedItem->setSelected(false);
        }
        selectWidgetForItem(current->parent());
        setCurrentItem(current->parent());
        current->parent()->setSelected(true);
        m_slotSelectionChanged_enabled = true;
        return current->parent();
    }
    return current;
}

void WidgetTreeWidget::slotSelectionChanged()
{
    if (!m_form || !m_slotSelectionChanged_enabled)
        return;
    const bool hadFocus = hasFocus();
    const QList<QTreeWidgetItem*> list( selectedItems() );
    m_selectWidget_enabled = false; // to avoid execution seleting form's item
                                    // on the tree when selectFormWidget() is called
    m_form->selectFormWidget();
    m_selectWidget_enabled = true;
    foreach (QTreeWidgetItem *item, list) {
        selectWidgetForItem(item);
    }
    tryToAlterSelection(currentItem());
    if (hadFocus)
        setFocus(); //restore focus
}

void WidgetTreeWidget::addItem(KFormDesigner::ObjectTreeItem *item)
{
    WidgetTreeWidgetItem *parent = findItem(item->parent()->name());
    if (!parent)
        return;

    WidgetTreeWidgetItem::LoadTreeFlags flags;
    if (dynamic_cast<const InsertPageCommand*>(m_form->executingCommand())) {
        kDebug() << "InsertPageCommand";
        flags |= WidgetTreeWidgetItem::LoadTreeForAddedTabPage;
    }
    if (dynamic_cast<const RemovePageCommand*>(m_form->executingCommand())) {
        kDebug() << "undoing RemovePageCommand";
        flags |= WidgetTreeWidgetItem::LoadTreeForAddedTabPage;
    }
    loadTree(item, parent, flags);
}

void WidgetTreeWidget::removeItem(KFormDesigner::ObjectTreeItem *item)
{
    if (!item)
        return;
    if (dynamic_cast<const RemovePageCommand*>(m_form->executingCommand())) {
        kDebug() << "RemovePageCommand";
    }
    WidgetTreeWidgetItem *it = findItem(item->name());
    QTreeWidgetItem *root = invisibleRootItem();
    root->takeChild(root->indexOfChild(it));
    delete it;
}

void WidgetTreeWidget::renameItem(const QByteArray &oldname, const QByteArray &newname)
{
    if (findItemByFirstColumn(newname)) {
        kWarning() << "item with name" << newname << "already exists, cannot rename";
        return;
    }
    WidgetTreeWidgetItem *item = findItemByFirstColumn(oldname);
    if (!item)
        return;
    item->setText(0, newname);
}

void WidgetTreeWidget::setForm(Form *form)
{
    m_slotSelectionChanged_enabled = false;
    if (m_form) {
        disconnect(m_form, SIGNAL(destroying()), this, SLOT(slotBeforeFormDestroyed()));
        disconnect(m_form, SIGNAL(selectionChanged(QWidget*,KFormDesigner::Form::WidgetSelectionFlags)),
            this, SLOT(selectWidget(QWidget*,KFormDesigner::Form::WidgetSelectionFlags)));
        disconnect(m_form, SIGNAL(childRemoved(KFormDesigner::ObjectTreeItem*)),
            this, SLOT(removeItem(KFormDesigner::ObjectTreeItem*)));
        disconnect(m_form, SIGNAL(childAdded(KFormDesigner::ObjectTreeItem*)),
            this, SLOT(addItem(KFormDesigner::ObjectTreeItem*)));
        disconnect(m_form, SIGNAL(widgetNameChanged(const QByteArray&, const QByteArray&)),
            this, SLOT(renameItem(const QByteArray&, const QByteArray&)));

    }
    m_form = form;
    //2.0 m_topItem = 0;
    clear();

    if (!m_form)
        return;

    connect(m_form, SIGNAL(destroying()), this, SLOT(slotBeforeFormDestroyed()));
    connect(m_form, SIGNAL(selectionChanged(QWidget*,KFormDesigner::Form::WidgetSelectionFlags)),
        this, SLOT(selectWidget(QWidget*,KFormDesigner::Form::WidgetSelectionFlags)));
    connect(m_form, SIGNAL(childRemoved(KFormDesigner::ObjectTreeItem*)),
        this, SLOT(removeItem(KFormDesigner::ObjectTreeItem*)));
    connect(m_form, SIGNAL(childAdded(KFormDesigner::ObjectTreeItem*)),
        this, SLOT(addItem(KFormDesigner::ObjectTreeItem*)));
    connect(m_form, SIGNAL(widgetNameChanged(const QByteArray&, const QByteArray&)),
        this, SLOT(renameItem(const QByteArray&, const QByteArray&)));

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
    m_slotSelectionChanged_enabled = true;
    resizeColumnToContents(0);
}

void WidgetTreeWidget::slotBeforeFormDestroyed()
{
    setForm(0);
}

void WidgetTreeWidget::loadTree(ObjectTreeItem *item, WidgetTreeWidgetItem *parent,
    WidgetTreeWidgetItem::LoadTreeFlags flags)
{
    if (!item)
        return;

    const RemovePageCommand* removePageCommand
        = dynamic_cast<const RemovePageCommand*>(m_form->executingCommand());
    int forcedTabPageIndex;
    QString forcedTabPageName;
    if (removePageCommand) {
        kDebug() << "undoing RemovePageCommand - fixing item name and index";
        forcedTabPageIndex = removePageCommand->pageIndex();
        forcedTabPageName = removePageCommand->pageName();
    }
    else
        forcedTabPageIndex = -1;

    WidgetTreeWidgetItem *treeItem = new WidgetTreeWidgetItem(parent, item, flags,
                                                              forcedTabPageIndex, forcedTabPageName);
    treeItem->setExpanded(true);

//2.0
    // The item is inserted by default at the beginning, but we want it to be at the end, so we move it
//     Q3ListViewItem *last = parent->firstChild();
//     while (last->nextSibling())
//         last = last->nextSibling();
//     treeItem->moveItem(last);

    ObjectTreeList *list = item->children();
    if (flags & WidgetTreeWidgetItem::LoadTreeForAddedTabPage)
        flags ^= WidgetTreeWidgetItem::LoadTreeForAddedTabPage; // this flag does not propagate to children
    foreach (ObjectTreeItem *titem, *list) {
        loadTree(titem, treeItem);
    }
}

#include "WidgetTreeWidget.moc"
