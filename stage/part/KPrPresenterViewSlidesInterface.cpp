/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPresenterViewSlidesInterface.h"

#include <KoPAPageBase.h>
#include <KoPAPageThumbnailModel.h>

#include <QListView>
#include <QVBoxLayout>

KPrPresenterViewSlidesInterface::KPrPresenterViewSlidesInterface(const QList<KoPAPageBase *> &pages, QWidget *parent)
    : KPrPresenterViewBaseInterface(pages, parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout;

    m_listView = new QListView;
    m_thumbnailModel = new KoPAPageThumbnailModel(m_pages, this);
    m_listView->setModel(m_thumbnailModel);
    m_listView->setDragDropMode(QListView::NoDragDrop);
    m_listView->setIconSize(QSize(128, 128));
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setFlow(QListView::LeftToRight);
    m_listView->setWrapping(true);
    m_listView->setResizeMode(QListView::Adjust);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setMovement(QListView::Static);

    connect(m_listView, &QAbstractItemView::clicked, this, &KPrPresenterViewSlidesInterface::itemClicked);
    connect(m_listView, &QAbstractItemView::doubleClicked, this, &KPrPresenterViewSlidesInterface::itemDoubleClicked);

    vLayout->addWidget(m_listView);

    setLayout(vLayout);
}

void KPrPresenterViewSlidesInterface::itemClicked(const QModelIndex &index)
{
    Q_EMIT selectedPageChanged(index.row(), false);
}

void KPrPresenterViewSlidesInterface::itemDoubleClicked(const QModelIndex &index)
{
    Q_EMIT selectedPageChanged(index.row(), true);
}
