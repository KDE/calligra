/* This file is part of the Calligra project
 * Copyright (C) 2010-2014 Yue Liu <yue.liu@mail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "CollectionTreeWidget.h"

#include "StencilListView.h"
#include "CollectionItemModel.h"
#include "StencilBoxDebug.h"

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

#include <QTreeWidgetItem>
#include <QApplication>
#include <QContextMenuEvent>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QPainter>

SheetDelegate::SheetDelegate(QTreeView* view, QWidget* parent)
    : QItemDelegate(parent),
      m_view(view)
{
}

// style comes from qt designer
// https://qt.gitorious.org/qt/qttools/source/src/designer/src/lib/shared/sheet_delegate.cpp
void SheetDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QAbstractItemModel* model = index.model();
    Q_ASSERT(model);

    if (!model->parent(index).isValid()) {
        // this is a top-level item.
        QStyleOptionButton buttonOption;

        buttonOption.state = option.state;
#ifdef Q_OS_MAC
        buttonOption.state |= QStyle::State_Raised;
#endif
        buttonOption.state &= ~QStyle::State_HasFocus;

        buttonOption.rect = option.rect;
        buttonOption.palette = option.palette;
        buttonOption.features = QStyleOptionButton::None;

        painter->save();
        QColor buttonColor(230, 230, 230);
        QBrush buttonBrush = option.palette.button();
        if (!buttonBrush.gradient() && buttonBrush.texture().isNull())
            buttonColor = buttonBrush.color();
        QColor outlineColor = buttonColor.darker(150);
        QColor highlightColor = buttonColor.lighter(130);

        // Only draw topline if the previous item is expanded
        QModelIndex previousIndex = model->index(index.row() - 1, index.column());
        bool drawTopline = (index.row() > 0 && m_view->isExpanded(previousIndex));
        int highlightOffset = drawTopline ? 1 : 0;

        QLinearGradient gradient(option.rect.topLeft(), option.rect.bottomLeft());
        gradient.setColorAt(0, buttonColor.lighter(102));
        gradient.setColorAt(1, buttonColor.darker(106));

        painter->setPen(Qt::NoPen);
        painter->setBrush(gradient);
        painter->drawRect(option.rect);
        painter->setPen(QPen(highlightColor, 0));
        painter->drawLine(option.rect.topLeft() + QPoint(0, highlightOffset),
                          option.rect.topRight() + QPoint(0, highlightOffset));
        painter->setPen(QPen(outlineColor, 0));
        if (drawTopline)
            painter->drawLine(option.rect.topLeft(), option.rect.topRight());
        painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
        painter->restore();

        QStyleOption branchOption;
        static const int i = 9; // ### hardcoded in qcommonstyle.cpp
        QRect r = option.rect;
        branchOption.rect = QRect(r.left() + i / 2, r.top() + (r.height() - i) / 2, i, i);
        branchOption.palette = option.palette;
        branchOption.state = QStyle::State_Children;

        if (m_view->isExpanded(index))
            branchOption.state |= QStyle::State_Open;

        m_view->style()->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, m_view);

        // draw text
        QRect textrect = QRect(r.left() + i * 2, r.top(), r.width() - ((5 * i) / 2), r.height());
        QString text = elidedText(option.fontMetrics, textrect.width(), Qt::ElideMiddle,
                                  model->data(index, Qt::DisplayRole).toString());
        m_view->style()->drawItemText(painter, textrect, Qt::AlignCenter,
                                      option.palette, m_view->isEnabled(), text);

    } else {
        QItemDelegate::paint(painter, option, index);
    }
}

QSize SheetDelegate::sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const
{
    QSize sz = QItemDelegate::sizeHint(opt, index) + QSize(2, 2);
    return sz;
}


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
    setAcceptDrops(true);
    setItemDelegate(new SheetDelegate(this, this));

    connect(this, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
            this, SLOT(handleMousePress(QTreeWidgetItem*)));

    loadOptions();
}

CollectionTreeWidget::~CollectionTreeWidget()
{
    saveOptions();
}

void CollectionTreeWidget::setFamilyMap(QMap<QString, CollectionItemModel*> map)
{
    m_familyMap = map;
}

void CollectionTreeWidget::regenerateFilteredMap()
{
    QMapIterator<QString, CollectionItemModel*> i(m_familyMap);
    while (i.hasNext()) {
        i.next();
        i.value()->setViewMode(m_viewMode);
        QSortFilterProxyModel* proxy = new QSortFilterProxyModel();
        proxy->setSourceModel(i.value());
        m_filteredMap.insert(i.key(), proxy);
    }

    //regenerate category view
    QMapIterator<QString, QSortFilterProxyModel*> j(m_filteredMap);
    while (j.hasNext()) {
        j.next();
        QTreeWidgetItem* category = new QTreeWidgetItem(this);
        category->setText(0, j.key());
        addStencilListView(category, m_viewMode, j.value());
    }
}

//Link a StencilListView to each TreeWidgetItem
void CollectionTreeWidget::addStencilListView(QTreeWidgetItem* parent,
        QListView::ViewMode viewMode, QSortFilterProxyModel* model)
{
    QTreeWidgetItem* embed_item = new QTreeWidgetItem(parent);
    embed_item->setFlags(Qt::ItemIsEnabled);
    StencilListView* categoryView = new StencilListView();
    categoryView->setViewMode(viewMode);
    categoryView->setModel(model);
    setItemWidget(embed_item, 0, categoryView);
}

StencilListView* CollectionTreeWidget::stencilListViewAt(int idx) const
{
    StencilListView* rc = 0;
    if (QTreeWidgetItem* cat_item = topLevelItem(idx)) {
        if (QTreeWidgetItem* embedItem = cat_item->child(0)) {
            rc = qobject_cast<StencilListView*>(itemWidget(embedItem, 0));
        }
    }
    Q_ASSERT(rc);
    return rc;
}

void CollectionTreeWidget::saveOptions()
{
    KConfigGroup group = KSharedConfig::openConfig()->group("Stencil Box");
    group.writeEntry("viewMode", (int)m_viewMode);
}

void  CollectionTreeWidget::loadOptions()
{
    KConfigGroup group = KSharedConfig::openConfig()->group("Stencil Box");
    int viewMode = group.readEntry("viewMode", (int)QListView::IconMode);
    m_viewMode = (QListView::ViewMode)viewMode;
    updateViewMode();
}

void CollectionTreeWidget::handleMousePress(QTreeWidgetItem* item)
{
    if (item && !item->parent() &&
        QApplication::mouseButtons() == Qt::LeftButton)
        setItemExpanded(item, !isItemExpanded(item));
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
    while (i.hasNext()) {
        i.next();
        i.value()->setViewMode(m_viewMode);
    }
    if (const int numTopLevels = topLevelItemCount()) {
        for (int i = numTopLevels - 1; i >= 0; --i) {
            StencilListView* categoryView = stencilListViewAt(i);

            if (m_viewMode != categoryView->viewMode()) {
                categoryView->setViewMode(m_viewMode);
                categoryView->setMovement(QListView::Static);
                categoryView->setDragDropMode(QAbstractItemView::DragDrop);
                adjustStencilListSize(topLevelItem(i));
            }
        }
    }
    updateGeometries();
}

void CollectionTreeWidget::adjustStencilListSize(QTreeWidgetItem* cat_item)
{
    QTreeWidgetItem* embedItem = cat_item->child(0);
    if (embedItem == 0)
        return;

    StencilListView* list_widget = static_cast<StencilListView*>(itemWidget(embedItem, 0));
    list_widget->setFixedWidth(header()->width());
    list_widget->doItemsLayout();
    const int height = qMax(list_widget->contentsSize().height() , 1);
    list_widget->setFixedHeight(height);
    embedItem->setSizeHint(0, QSize(-1, height - 1));
}

void CollectionTreeWidget::setFilter(QRegExp regExp)
{
    QMapIterator<QString, QSortFilterProxyModel*> j(m_filteredMap);
    while (j.hasNext()) {
        j.next();
        j.value()->setFilterRegExp(regExp);
        j.value()->setFilterRole(Qt::UserRole + 1);
    }
    for (int i = 0; i < topLevelItemCount(); i++) {
        QTreeWidgetItem* tl = topLevelItem(i);
        StencilListView* categoryView = stencilListViewAt(i);
        QAbstractItemModel* model = categoryView->model();
        const bool categoryEnabled = model->rowCount() > 0;
        if (categoryView->model()->rowCount() > 0) {
            categoryView->adjustSize();
            adjustStencilListSize(tl);
        }
        setRowHidden(i, QModelIndex(), !categoryEnabled);
    }
    updateGeometries();
}

void CollectionTreeWidget::resizeEvent(QResizeEvent* e)
{
    QTreeWidget::resizeEvent(e);
    if (const int numTopLevels = topLevelItemCount()) {
        for (int i = numTopLevels - 1; i >= 0; --i) {
            adjustStencilListSize(topLevelItem(i));
        }
    }
}

void CollectionTreeWidget::contextMenuEvent(QContextMenuEvent* e)
{
    QMenu menu;
    menu.addAction(i18n("Expand all"), this, SLOT(expandAll()));
    menu.addAction(i18n("Collapse all"), this, SLOT(collapseAll()));
    menu.addSeparator();

    QAction* listModeAction = menu.addAction(i18n("List View"));
    QAction* iconModeAction = menu.addAction(i18n("Icon View"));
    listModeAction->setCheckable(true);
    iconModeAction->setCheckable(true);
    QActionGroup* viewModeGroup = new QActionGroup(&menu);
    viewModeGroup->addAction(listModeAction);
    viewModeGroup->addAction(iconModeAction);
    if (m_viewMode == QListView::IconMode) {
        iconModeAction->setChecked(true);
    } else {
        listModeAction->setChecked(true);
    }
    connect(listModeAction, SIGNAL(triggered()), SLOT(slotListMode()));
    connect(iconModeAction, SIGNAL(triggered()), SLOT(slotIconMode()));

    e->accept();
    menu.exec(mapToGlobal(e->pos()));
}
