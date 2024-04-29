/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoResourceSelector.h"
#include <KoResourceItemDelegate.h>
#include <KoResourceItemView.h>
#include <KoResourceModel.h>
#include <KoResourceServerAdapter.h>
#include <QHeaderView>
#include <QListView>
#include <QMouseEvent>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTableView>

#include <WidgetsDebug.h>

class Q_DECL_HIDDEN KoResourceSelector::Private
{
public:
    Private()
        : displayMode(ImageMode)
    {
    }
    DisplayMode displayMode;

    void updateIndex(KoResourceSelector *me)
    {
        KoResourceModel *resourceModel = qobject_cast<KoResourceModel *>(me->model());
        if (!resourceModel)
            return;
        if (!resourceModel->rowCount())
            return;

        int currentIndex = me->currentIndex();
        QModelIndex currentModelIndex = me->view()->currentIndex();

        if (currentIndex < 0 || !currentModelIndex.isValid()) {
            me->blockSignals(true);
            me->view()->setCurrentIndex(resourceModel->index(0, 0));
            me->setCurrentIndex(0);
            me->blockSignals(false);
            me->update();
        }
    }
};

KoResourceSelector::KoResourceSelector(QWidget *parent)
    : QComboBox(parent)
    , d(new Private())
{
    connect(this, QOverload<int>::of(&KoResourceSelector::currentIndexChanged), this, &KoResourceSelector::indexChanged);

    setMouseTracking(true);
}

KoResourceSelector::KoResourceSelector(QSharedPointer<KoAbstractResourceServerAdapter> resourceAdapter, QWidget *parent)
    : QComboBox(parent)
    , d(new Private())
{
    Q_ASSERT(resourceAdapter);

    setView(new KoResourceItemView(this));
    setModel(new KoResourceModel(resourceAdapter, this));
    setItemDelegate(new KoResourceItemDelegate(this));
    setMouseTracking(true);
    d->updateIndex(this);

    connect(this, QOverload<int>::of(&KoResourceSelector::currentIndexChanged), this, &KoResourceSelector::indexChanged);

    connect(resourceAdapter.data(), &KoAbstractResourceServerAdapter::resourceAdded, this, &KoResourceSelector::resourceAdded);
    connect(resourceAdapter.data(), &KoAbstractResourceServerAdapter::removingResource, this, &KoResourceSelector::resourceRemoved);
}

KoResourceSelector::~KoResourceSelector()
{
    delete d;
}

void KoResourceSelector::paintEvent(QPaintEvent *pe)
{
    QComboBox::paintEvent(pe);

    if (d->displayMode == ImageMode) {
        QStyleOptionComboBox option;
        option.initFrom(this);
        QRect r = style()->subControlRect(QStyle::CC_ComboBox, &option, QStyle::SC_ComboBoxEditField, this);

        QStyleOptionViewItem viewOption;
        viewOption.initFrom(this);
        viewOption.rect = r;

        QPainter painter(this);
        itemDelegate()->paint(&painter, viewOption, view()->currentIndex());
    }
}

void KoResourceSelector::mousePressEvent(QMouseEvent *event)
{
    QStyleOptionComboBox opt;
    opt.initFrom(this);
    opt.subControls = QStyle::SC_All;
    opt.activeSubControls = QStyle::SC_ComboBoxArrow;
    QStyle::SubControl sc = style()->hitTestComplexControl(QStyle::CC_ComboBox, &opt, mapFromGlobal(event->globalPos()), this);
    // only clicking on combobox arrow shows popup,
    // otherwise the resourceApplied signal is send with the current resource
    if (sc == QStyle::SC_ComboBoxArrow)
        QComboBox::mousePressEvent(event);
    else {
        QModelIndex index = view()->currentIndex();
        if (!index.isValid())
            return;

        KoResource *resource = static_cast<KoResource *>(index.internalPointer());
        if (resource)
            Q_EMIT resourceApplied(resource);
    }
}

void KoResourceSelector::mouseMoveEvent(QMouseEvent *event)
{
    QStyleOptionComboBox option;
    option.initFrom(this);
    QRect r = style()->subControlRect(QStyle::CC_ComboBox, &option, QStyle::SC_ComboBoxEditField, this);
    if (r.contains(event->pos()))
        setCursor(Qt::PointingHandCursor);
    else
        unsetCursor();
}

void KoResourceSelector::setResourceAdapter(QSharedPointer<KoAbstractResourceServerAdapter> resourceAdapter)
{
    Q_ASSERT(resourceAdapter);
    setModel(new KoResourceModel(resourceAdapter, this));
    d->updateIndex(this);

    connect(resourceAdapter.data(), &KoAbstractResourceServerAdapter::resourceAdded, this, &KoResourceSelector::resourceAdded);
    connect(resourceAdapter.data(), &KoAbstractResourceServerAdapter::removingResource, this, &KoResourceSelector::resourceRemoved);
}

void KoResourceSelector::setDisplayMode(DisplayMode mode)
{
    if (mode == d->displayMode)
        return;

    switch (mode) {
    case ImageMode:
        setItemDelegate(new KoResourceItemDelegate(this));
        setView(new KoResourceItemView(this));
        break;
    case TextMode:
        setItemDelegate(new QStyledItemDelegate(this));
        setView(new QListView(this));
        break;
    }

    d->displayMode = mode;
    d->updateIndex(this);
}

void KoResourceSelector::setColumnCount(int columnCount)
{
    KoResourceModel *resourceModel = qobject_cast<KoResourceModel *>(model());
    if (resourceModel)
        resourceModel->setColumnCount(columnCount);
}

void KoResourceSelector::setRowHeight(int rowHeight)
{
    QTableView *tableView = qobject_cast<QTableView *>(view());
    if (tableView)
        tableView->verticalHeader()->setDefaultSectionSize(rowHeight);
}

void KoResourceSelector::indexChanged(int)
{
    QModelIndex index = view()->currentIndex();
    if (!index.isValid())
        return;

    KoResource *resource = static_cast<KoResource *>(index.internalPointer());
    if (resource)
        Q_EMIT resourceSelected(resource);
}

void KoResourceSelector::resourceAdded(KoResource *)
{
    d->updateIndex(this);
}

void KoResourceSelector::resourceRemoved(KoResource *)
{
    d->updateIndex(this);
}
