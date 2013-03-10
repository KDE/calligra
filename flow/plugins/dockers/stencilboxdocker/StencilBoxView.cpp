/*
    Part of KDE project.
    Copyright (C) 2010 Yue Liu <opuspace@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "StencilBoxView.h"
#include "CollectionItemModel.h"

#include <kdebug.h>
#include <klocale.h>

#include <QTreeWidgetItem>
#include <QItemDelegate>
#include <QApplication>
#include <QContextMenuEvent>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QHeaderView>
#include <QSortFilterProxyModel>

SheetDelegate::SheetDelegate(QTreeView *view, QWidget *parent)
    : QItemDelegate(parent),
      m_view(view)
{
}

void SheetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QAbstractItemModel *model = index.model();
    Q_ASSERT(model);

    if (!model->parent(index).isValid())
    {
        // this is a top-level item.
        QStyleOptionButton buttonOption;

        buttonOption.state = option.state;
        #ifdef Q_WS_MAC
        buttonOption.state |= QStyle::State_Raised;
        #endif
        buttonOption.state &= ~QStyle::State_HasFocus;

        buttonOption.rect = option.rect;
        buttonOption.palette = option.palette;
        buttonOption.features = QStyleOptionButton::None;
        m_view->style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter, m_view);

        QStyleOption branchOption;
        static const int i = 9; // ### hardcoded in qcommonstyle.cpp
        QRect r = option.rect;
        branchOption.rect = QRect(r.left() + i/2, r.top() + (r.height() - i)/2, i, i);
        branchOption.palette = option.palette;
        branchOption.state = QStyle::State_Children;

        if (m_view->isExpanded(index))
            branchOption.state |= QStyle::State_Open;

        m_view->style()->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, m_view);

        // draw text
        QRect textrect = QRect(r.left() + i*2, r.top(), r.width() - ((5*i)/2), r.height());
        QString text = elidedText(option.fontMetrics, textrect.width(), Qt::ElideMiddle,
            model->data(index, Qt::DisplayRole).toString());
        m_view->style()->drawItemText(painter, textrect, Qt::AlignCenter,
            option.palette, m_view->isEnabled(), text);

    }
    else
    {
        QItemDelegate::paint(painter, option, index);
    }
}

QSize SheetDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
    QSize sz = QItemDelegate::sizeHint(opt, index) + QSize(2, 2);
    return sz;
}

ShapeListView::ShapeListView(QWidget* parent) :
    QListView(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setFrameShape(QFrame::NoFrame);
    setIconSize(QSize(22, 22));
    setMovement(QListView::Static);
    setSpacing(1);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeMode(QListView::Adjust);
    setUniformItemSizes(true);
    
    //setSelectionMode(QAbstractItemView::NoSelection);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDropIndicatorShown(true);

    //connect(this, SIGNAL(pressed(QModelIndex)), this, SLOT(slotPressed(QModelIndex)));
    setEditTriggers(QAbstractItemView::AnyKeyPressed);
}

/*void ShapeListView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_startPos = event->pos();
    }
    QListView::mousePressEvent(event);
}

void ShapeListView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        int distance = (event->pos() - m_startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
        {
            ;//performDrag();
        }
    }
    QListView::mouseMoveEvent(event);
}

void ShapeListView::performDrag()
{
    QListViewItem *item = currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->text());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap(":/images/person.png"));
        if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
            delete item;
    }
}*/

CollectionTreeWidget::CollectionTreeWidget(QWidget* parent): QTreeWidget(parent)
{
    header()->hide();
    header()->setResizeMode(QHeaderView::Stretch);
    setRootIsDecorated(false);
    setItemsExpandable(true);
    setFocusPolicy(Qt::NoFocus);
    setIndentation(0);
    setColumnCount(1);
    setVerticalScrollMode(ScrollPerPixel);
    restoreViewMode();

    setItemDelegate(new SheetDelegate(this, this));

    //regenerateFilteredMap();

    connect(this, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
            this, SLOT(handleMousePress(QTreeWidgetItem*)));
}

CollectionTreeWidget::~CollectionTreeWidget()
{
    saveViewMode();
}

void CollectionTreeWidget::setFamilyMap(QMap<QString, CollectionItemModel*> map)
{
    m_familyMap = map;
}

void CollectionTreeWidget::regenerateFilteredMap()
{
    QMapIterator<QString, CollectionItemModel*> i(m_familyMap);
    while(i.hasNext())
    {
        i.next();
        i.value()->setViewMode(m_viewMode);
        QSortFilterProxyModel* proxy = new QSortFilterProxyModel();
        proxy->setSourceModel(i.value());
        m_filteredMap.insert(i.key(), proxy);
    }

    //delete category view

    //regenerate category view
    QMapIterator<QString, QSortFilterProxyModel*> j(m_filteredMap);
    while (j.hasNext())
    {
         j.next();
         QTreeWidgetItem *category = new QTreeWidgetItem(this);
         category->setText(0, j.key());
         addCategoryView(category, m_viewMode, j.value());
    }
}

//Link a ShapeListView to each TreeWidgetItem
void CollectionTreeWidget::addCategoryView(QTreeWidgetItem *parent,
                                                     bool iconMode, QSortFilterProxyModel *model)
{
    QTreeWidgetItem *embed_item = new QTreeWidgetItem(parent);
    embed_item->setFlags(Qt::ItemIsEnabled);
    ShapeListView *categoryView = new ShapeListView();
    categoryView->setViewMode(iconMode ? QListView::IconMode : QListView::ListMode);
    setItemWidget(embed_item, 0, categoryView);
    categoryView->setModel(model);
}

ShapeListView* CollectionTreeWidget::categoryViewAt(int idx) const
{
    ShapeListView *rc = 0;
    if (QTreeWidgetItem *cat_item = topLevelItem(idx))
    {
        if (QTreeWidgetItem *embedItem = cat_item->child(0))
        {
            rc = qobject_cast<ShapeListView*>(itemWidget(embedItem, 0));
        }
    }
    Q_ASSERT(rc);
    return rc;
}

void CollectionTreeWidget::saveViewMode()
{
    //FIXME
}

void  CollectionTreeWidget::restoreViewMode()
{
    //FIXME
    m_viewMode = QListView::ListMode;
}

void CollectionTreeWidget::handleMousePress(QTreeWidgetItem *item)
{
    if (item == 0)
        return;

    if (QApplication::mouseButtons() != Qt::LeftButton)
        return;

    if (item->parent() == 0) {
        setItemExpanded(item, !isItemExpanded(item));
        return;
    }
}

void CollectionTreeWidget::slotListMode()
{
    m_viewMode = QListView::ListMode;
    updateViewMode();
}

void CollectionTreeWidget::slotIconMode()
{
    m_viewMode = QListView::IconMode;
    updateViewMode();
}

void CollectionTreeWidget::updateViewMode()
{
    QMapIterator<QString, CollectionItemModel*> i(m_familyMap);
    while(i.hasNext())
    {
        i.next();
        i.value()->setViewMode(m_viewMode);
    }
    if (const int numTopLevels = topLevelItemCount())
    {
        for (int i = numTopLevels - 1; i >= 0; --i)
        {
            const QListView::ViewMode viewMode  = m_viewMode ? QListView::IconMode : QListView::ListMode;
            ShapeListView *categoryView = categoryViewAt(i);

            if (viewMode != categoryView->viewMode())
            {
                categoryView->setViewMode(viewMode);
                categoryView->setMovement(QListView::Static);
                categoryView->setDragDropMode(QAbstractItemView::DragDrop);
                adjustSubListSize(topLevelItem(i));
            }
        }
    }
    updateGeometries();
}

void CollectionTreeWidget::adjustSubListSize(QTreeWidgetItem *cat_item)
{
    QTreeWidgetItem *embedItem = cat_item->child(0);
    if (embedItem == 0)
        return;

    ShapeListView *list_widget = static_cast<ShapeListView*>(itemWidget(embedItem, 0));
    list_widget->setFixedWidth(header()->width());
    list_widget->doItemsLayout();
    const int height = qMax(list_widget->contentsSize().height() ,1);
    list_widget->setFixedHeight(height);
    embedItem->setSizeHint(0, QSize(-1, height - 1));
}

void CollectionTreeWidget::setFilter(QRegExp regExp)
{
    QMapIterator<QString, QSortFilterProxyModel*> j(m_filteredMap);
    while (j.hasNext())
    {
         j.next();
         j.value()->setFilterRegExp(regExp);
         j.value()->setFilterRole(Qt::UserRole+1);
    }
    const int numTopLevels = topLevelItemCount();
    for (int i = 0; i < numTopLevels; i++)
    {
        QTreeWidgetItem *tl = topLevelItem(i);
        ShapeListView *categoryView = categoryViewAt(i);
        QAbstractItemModel *model = categoryView->model();
        const bool categoryEnabled = model->rowCount() > 0;
        if (categoryEnabled) {
            categoryView->adjustSize();
            adjustSubListSize(tl);
        }
        setRowHidden(i, QModelIndex(), !categoryEnabled);
    }
    updateGeometries();
}

void CollectionTreeWidget::resizeEvent(QResizeEvent *e)
{
    QTreeWidget::resizeEvent(e);
    if (const int numTopLevels = topLevelItemCount()) {
        for (int i = numTopLevels - 1; i >= 0; --i)
        {
            adjustSubListSize(topLevelItem(i));
        }
    }
}

void CollectionTreeWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;
    menu.addAction(i18n("Expand all"), this, SLOT(expandAll()));
    menu.addAction(i18n("Collapse all"), this, SLOT(collapseAll()));
    menu.addSeparator();

    QAction *listModeAction = menu.addAction(i18n("List View"));
    QAction *iconModeAction = menu.addAction(i18n("Icon View"));
    listModeAction->setCheckable(true);
    iconModeAction->setCheckable(true);
    QActionGroup *viewModeGroup = new QActionGroup(&menu);
    viewModeGroup->addAction(listModeAction);
    viewModeGroup->addAction(iconModeAction);
    if (m_viewMode) {
        iconModeAction->setChecked(true);
    }
    else {
        listModeAction->setChecked(true);
    }
    connect(listModeAction, SIGNAL(triggered()), SLOT(slotListMode()));
    connect(iconModeAction, SIGNAL(triggered()), SLOT(slotIconMode()));

    e->accept();
    menu.exec(mapToGlobal(e->pos()));
}
