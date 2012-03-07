/*
    Part of KDE project.
    Copyright (C) 2010-2012 Yue Liu <yue.liu@mail.com>

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

#include "StencilBoxTreeWidget.h"
#include "StencilListView.h"
#include "StencilListModel.h"

#include <kdebug.h>
#include <klocale.h>

#include <QTreeWidgetItem>
#include <QApplication>
#include <QTreeWidgetItem>
#include <QContextMenuEvent>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QHeaderView>
#include <QDir>

SheetDelegate::SheetDelegate(QTreeView *view, QWidget *parent)
    : QStyledItemDelegate(parent),
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
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize SheetDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
    QSize sz = QStyledItemDelegate::sizeHint(opt, index) + QSize(2, 2);
    return sz;
}

StencilBoxTreeWidget::StencilBoxTreeWidget(QWidget* parent): QTreeWidget(parent)
{
    header()->hide();
    header()->setResizeMode(QHeaderView::Stretch);
    setRootIsDecorated(false);
    setItemsExpandable(true);
    setFocusPolicy(Qt::NoFocus);
    setIndentation(0);
    setColumnCount(1);
    setVerticalScrollMode(ScrollPerPixel);

    setItemDelegate(new SheetDelegate(this, this));

    loadLastStencils();

    connect(this, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
            this, SLOT(handleMousePress(QTreeWidgetItem*)));
}

StencilBoxTreeWidget::~StencilBoxTreeWidget()
{
    saveLastStencils();
    qDeleteAll(m_stencilHash->begin(), m_stencilHash->end());
    m_stencilHash->clear();
    delete *m_stencilHash;
}

bool StencilBoxTreeWidget::addStencil(const QStringList &parameters)
{
    QString stencilId = parameters.at(0);
    if(!m_stencilHash.contains(stencilId)) {
        QDir dir(stencilId);
        StencilListModel* model = new StencilListModel(this);
        QStringList shapes = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach(const QString& shape, shapes) {
            QString id = QString(shape + QDir::separator () + shape);
            QString confPath = QString(id + ".desktop");
            KDesktopFile shapeConf = dir.absoluteFilePath(confPath);
            KConfigGroup dg = shapeConf.desktopGroup();
            QString name = dg.readEntry("Name");
            QString type = dg.readEntry("Type");

            // load various properties
            QString keepAspectRatio = dg.readEntry("CS-KeepAspectRatio", "0");
            KoProperties* props = new KoProperties();
            props->setProperty("keepAspectRatio", keepAspectRatio.toInt());

            QString icon = QString();
            if(dir.exists(id + ".png")) {
                icon = id + ".png";
            } else {
                icon = "";
            }
            StencilShapeFactory* factory = new StencilShapeFactory(id, type, name, props);
            KoShapeRegistry::instance()->add(id, factory);
            ShapeItem item;
            item.id = id;
            item.name = name;
            item.icon = QIcon(icon);
            item.properties = props;
            model->appendRow(item);
        }
        m_stencilHash->insert(stencilId, model);
    }

    if (0) {
        //if already opened, do nothing
        return false;
    } else {
        StencilListModel* model = m_stencilHash->value(stencilId);
        QTreeWidgetItem* item = new QTreeWidget(this);

        insertTopLevelItem(0, item);

        return true;
    }
}

void StencilBoxTreeWidget::handleMousePress(QTreeWidgetItem *item)
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

void StencilBoxTreeWidget::adjustSubListSize(QTreeWidgetItem *cat_item)
{
    QTreeWidgetItem *embedItem = cat_item->child(0);
    if (embedItem == 0)
        return;

    StencilListView *list_widget = static_cast<StencilListView*>(itemWidget(embedItem, 0));
    list_widget->setFixedWidth(header()->width());
    list_widget->doItemsLayout();
    const int height = qMax(list_widget->contentsSize().height() ,1);
    list_widget->setFixedHeight(height);
    embedItem->setSizeHint(0, QSize(-1, height - 1));
}

void StencilBoxTreeWidget::resizeEvent(QResizeEvent *e)
{
    QTreeWidget::resizeEvent(e);
    if (const int numTopLevels = topLevelItemCount()) {
        for (int i = numTopLevels - 1; i >= 0; --i)
        {
            adjustSubListSize(topLevelItem(i));
        }
    }
}

void StencilBoxTreeWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;
    menu.addAction(i18n("Expand all"), this, SLOT(expandAll()));
    menu.addAction(i18n("Collapse all"), this, SLOT(collapseAll()));
    menu.addSeparator();

    e->accept();
    menu.exec(mapToGlobal(e->pos()));
}
