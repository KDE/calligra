/* This file is part of the KDE project
 * Copyright (C) 2008 Fredy Yanardi <fyanardi@gmail.com>
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

#include "KPrPresenterViewSlidesInterface.h"

#include <KoPAPageBase.h>
#include <KoPAPageThumbnailModel.h>

#include <QVBoxLayout>
#include <QListView>

KPrPresenterViewSlidesInterface::KPrPresenterViewSlidesInterface( const QList<KoPAPageBase *> &pages, QWidget *parent )
    : KPrPresenterViewBaseInterface( pages, parent )
{
    QVBoxLayout *vLayout = new QVBoxLayout;

    m_listView = new QListView;
    m_thumbnailModel = new KoPAPageThumbnailModel( m_pages, this );
    m_listView->setModel( m_thumbnailModel );
    m_listView->setDragDropMode( QListView::NoDragDrop );
    m_listView->setIconSize( QSize( 128, 128 ) );
    m_listView->setViewMode( QListView::IconMode );
    m_listView->setFlow( QListView::LeftToRight );
    m_listView->setWrapping( true );
    m_listView->setResizeMode( QListView::Adjust );
    m_listView->setSelectionMode( QAbstractItemView::SingleSelection );
    m_listView->setMovement( QListView::Static );

    connect( m_listView, SIGNAL(clicked(QModelIndex)), this,
            SLOT(itemClicked(QModelIndex)) );
    connect( m_listView, SIGNAL(doubleClicked(QModelIndex)), this,
            SLOT(itemDoubleClicked(QModelIndex)) );

    vLayout->addWidget( m_listView );

    setLayout( vLayout );
}

void KPrPresenterViewSlidesInterface::itemClicked( const QModelIndex &index )
{
    emit selectedPageChanged( index.row(), false );
}

void KPrPresenterViewSlidesInterface::itemDoubleClicked( const QModelIndex &index )
{
    emit selectedPageChanged( index.row(), true );
}
