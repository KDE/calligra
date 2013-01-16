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

class WidgetTreeWidgetItem::Private
{
public:
    Private(ObjectTreeItem *data_,
            LoadTreeFlags loadTreeFlags_);
    ~Private();

    ObjectTreeItem *data;
    QString customSortingKey;
    LoadTreeFlags loadTreeFlags;
};

WidgetTreeWidgetItem::Private::Private(ObjectTreeItem *data_, LoadTreeFlags loadTreeFlags_)
    :data(data_), loadTreeFlags(loadTreeFlags_)
{

}

WidgetTreeWidgetItem::Private::~Private()
{

}

WidgetTreeWidgetItem::WidgetTreeWidgetItem(WidgetTreeWidgetItem *parent, ObjectTreeItem *data,
    LoadTreeFlags loadTreeFlags, int forcedTabPageIndex, const QString& forcedTabPageName)
        : QTreeWidgetItem(parent)
        ,d(new Private(data, loadTreeFlags))
{
    init(forcedTabPageIndex, forcedTabPageName);
}

WidgetTreeWidgetItem::WidgetTreeWidgetItem(QTreeWidget *tree, ObjectTreeItem *data,
    LoadTreeFlags loadTreeFlags, int forcedTabPageIndex, const QString& forcedTabPageName)
        : QTreeWidgetItem(tree)
        , d(new Private(data, loadTreeFlags))
{
    init(forcedTabPageIndex, forcedTabPageName);
}

WidgetTreeWidgetItem::~WidgetTreeWidgetItem()
{
    delete d;
}

void WidgetTreeWidgetItem::init(int forcedTabPageIndex, const QString& forcedTabPageName)
{
    if (d->data) {
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
    ObjectTreeItem* selectable = widgetTreeWidget ? widgetTreeWidget->selectableItem(d->data) : d->data;
    if (selectable != d->data) {
        kDebug() << "****" << (d->loadTreeFlags & LoadTreeForAddedTabPage) << selectable->widget();
        if (qobject_cast<QTabWidget*>(selectable->widget())) {
            // tab widget's page
            const QTabWidget* tabWidget = qobject_cast<QTabWidget*>(selectable->widget());
            int tabIndex = tabWidget->indexOf(d->data->widget());
            if (tabIndex == -1 && (d->loadTreeFlags & LoadTreeForAddedTabPage)) { // tab appended
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
                d->customSortingKey = QString("tab%1").arg(tabIndex);
                kDebug() << "d->customSortingKey" << d->customSortingKey;
                itemFlags |= Qt::ItemIsSelectable;
                itemFlags ^= Qt::ItemIsSelectable;
                itemIconName = koIconName("tabwidget_tab");
            }
        }
    }
    // defaults:
    if (itemName.isEmpty()) {
        itemName = d->data->name();
    }
    if (itemClass.isEmpty()) {
        itemClass = d->data->className();
    }
    if (itemIconName.isEmpty()) {
        if (widgetTreeWidget) {
            itemIconName = widgetTreeWidget->iconNameForClass(d->data->widget()->metaObject()->className());
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
    if (d->data)
        return d->data->name();
    else
        return QString();
}

bool WidgetTreeWidgetItem::operator<( const QTreeWidgetItem & other ) const
{
    const WidgetTreeWidgetItem *otherItem = dynamic_cast<const WidgetTreeWidgetItem*>(&other);
    if (!otherItem)
        return QTreeWidgetItem::operator<(other);
    return d->customSortingKey < otherItem->customSortingKey();
}

ObjectTreeItem* WidgetTreeWidgetItem::data() const
{
    return d->data;
}

QString WidgetTreeWidgetItem::customSortingKey() const
{
    return d->customSortingKey;
}

#ifdef __GNUC__
#warning port WidgetTreeWidgetItem::paintCell
#else
#pragma WARNING( port WidgetTreeWidgetItem::paintCell )
#endif

// WidgetTreeWidget itself -----------------------------------------------------------------------------------------------

class WidgetTreeWidget::Private
{
public:
    Private(Options o);
    ~Private();

    Form *form;
    Options options;

    //! Used to temporarily disable slotSelectionChanged() when reloading contents in setForm().
    bool slotSelectionChanged_enabled;
    //! Used to temporarily disable selectWidget().
    bool selectWidget_enabled;
};

WidgetTreeWidget::Private::Private(Options o)
    :form(0), options(o), slotSelectionChanged_enabled(true), selectWidget_enabled(true)
{

}

WidgetTreeWidget::Private::~Private()
{

}

WidgetTreeWidget::WidgetTreeWidget(QWidget *parent, Options options)
    : QTreeWidget(parent), d(new Private(options))
{
    setRootIsDecorated(false);
    setHeaderLabels(QStringList() << i18n("Widget name") << i18nc("Widget's type", "Type"));
    installEventFilter(this);

    if (!(d->options & DisableSelection)) {
        setSelectionMode(ExtendedSelection);
        connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
    }

    setAllColumnsShowFocus(true);
    setExpandsOnDoubleClick(false);
    setIndentation(indentation() / 2);
}

WidgetTreeWidget::~WidgetTreeWidget()
{
    delete d;
}

WidgetTreeWidgetItem* WidgetTreeWidget::selectedItem() const
{
    if (selectedItems().count() != 1)
        return 0;
    WidgetTreeWidgetItem *item = static_cast<WidgetTreeWidgetItem*>(selectedItems().first());
    return item;
}

QString WidgetTreeWidget::iconNameForClass(const QByteArray& classname) const
{
    return d->form->library()->iconName(classname);
}

ObjectTreeItem* WidgetTreeWidget::selectableItem(ObjectTreeItem* item)
{
    return d->form->library()->selectableItem(item);
}

void WidgetTreeWidget::handleContextMenuEvent(QContextMenuEvent* e)
{
    if (!d->form)
        return;
    WidgetTreeWidgetItem *item = static_cast<WidgetTreeWidgetItem*>(itemAt(e->pos()));
    if (!item)
        return;
    WidgetTreeWidgetItem *newItem = static_cast<WidgetTreeWidgetItem*>(tryToAlterSelection(item));
    QWidget *w = newItem->data()->widget();
    if (!w)
        return;
    d->form->createContextMenu(w, d->form->activeContainer(), e->pos(), Form::WidgetTreeContextMenuTarget);
}

void WidgetTreeWidget::contextMenuEvent(QContextMenuEvent* e)
{
    if (!(d->options & DisableContextMenu)) {
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
    if (!d->selectWidget_enabled)
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
    if (w && !d->form->selectedWidgets()->contains(w)) {
        d->form->selectWidget(w, Form::AddToPreviousSelection | Form::DontRaise | Form::LastSelection);
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
        d->slotSelectionChanged_enabled = false;
        foreach (QTreeWidgetItem *selectedItem, selectedItems()) {
            selectedItem->setSelected(false);
        }
        selectWidgetForItem(current->parent());
        setCurrentItem(current->parent());
        current->parent()->setSelected(true);
        d->slotSelectionChanged_enabled = true;
        return current->parent();
    }
    return current;
}

void WidgetTreeWidget::slotSelectionChanged()
{
    if (!d->form || !d->slotSelectionChanged_enabled)
        return;
    const bool hadFocus = hasFocus();
    const QList<QTreeWidgetItem*> list( selectedItems() );
    d->selectWidget_enabled = false; // to avoid execution seleting form's item
                                    // on the tree when selectFormWidget() is called
    d->form->selectFormWidget();
    d->selectWidget_enabled = true;
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
    if (dynamic_cast<const InsertPageCommand*>(d->form->executingCommand())) {
        kDebug() << "InsertPageCommand";
        flags |= WidgetTreeWidgetItem::LoadTreeForAddedTabPage;
    }
    if (dynamic_cast<const RemovePageCommand*>(d->form->executingCommand())) {
        kDebug() << "undoing RemovePageCommand";
        flags |= WidgetTreeWidgetItem::LoadTreeForAddedTabPage;
    }
    loadTree(item, parent, flags);
}

void WidgetTreeWidget::removeItem(KFormDesigner::ObjectTreeItem *item)
{
    if (!item)
        return;
    if (dynamic_cast<const RemovePageCommand*>(d->form->executingCommand())) {
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
    d->slotSelectionChanged_enabled = false;
    if (d->form) {
        disconnect(d->form, SIGNAL(destroying()), this, SLOT(slotBeforeFormDestroyed()));
        disconnect(d->form, SIGNAL(selectionChanged(QWidget*,KFormDesigner::Form::WidgetSelectionFlags)),
            this, SLOT(selectWidget(QWidget*,KFormDesigner::Form::WidgetSelectionFlags)));
        disconnect(d->form, SIGNAL(childRemoved(KFormDesigner::ObjectTreeItem*)),
            this, SLOT(removeItem(KFormDesigner::ObjectTreeItem*)));
        disconnect(d->form, SIGNAL(childAdded(KFormDesigner::ObjectTreeItem*)),
            this, SLOT(addItem(KFormDesigner::ObjectTreeItem*)));
        disconnect(d->form, SIGNAL(widgetNameChanged(const QByteArray&, const QByteArray&)),
            this, SLOT(renameItem(const QByteArray&, const QByteArray&)));

    }
    d->form = form;
    clear();

    if (!d->form)
        return;

    connect(d->form, SIGNAL(destroying()), this, SLOT(slotBeforeFormDestroyed()));
    connect(d->form, SIGNAL(selectionChanged(QWidget*,KFormDesigner::Form::WidgetSelectionFlags)),
        this, SLOT(selectWidget(QWidget*,KFormDesigner::Form::WidgetSelectionFlags)));
    connect(d->form, SIGNAL(childRemoved(KFormDesigner::ObjectTreeItem*)),
        this, SLOT(removeItem(KFormDesigner::ObjectTreeItem*)));
    connect(d->form, SIGNAL(childAdded(KFormDesigner::ObjectTreeItem*)),
        this, SLOT(addItem(KFormDesigner::ObjectTreeItem*)));
    connect(d->form, SIGNAL(widgetNameChanged(const QByteArray&, const QByteArray&)),
        this, SLOT(renameItem(const QByteArray&, const QByteArray&)));

    ObjectTree *tree = d->form->objectTree();
    QTreeWidgetItem *root = invisibleRootItem();
    loadTree(tree, static_cast<WidgetTreeWidgetItem*>(root));
    sortItems(0, Qt::AscendingOrder);

    if (!form->selectedWidgets()->isEmpty())
        selectWidget(form->selectedWidgets()->first());
    else
        selectWidget(form->widget());
    d->slotSelectionChanged_enabled = true;
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
        = dynamic_cast<const RemovePageCommand*>(d->form->executingCommand());
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

    ObjectTreeList *list = item->children();
    if (flags & WidgetTreeWidgetItem::LoadTreeForAddedTabPage)
        flags ^= WidgetTreeWidgetItem::LoadTreeForAddedTabPage; // this flag does not propagate to children
    foreach (ObjectTreeItem *titem, *list) {
        loadTree(titem, treeItem);
    }
}

#include "WidgetTreeWidget.moc"
