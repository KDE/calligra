/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrPageLayoutDocker.h"

#include <QListWidget>
#include <QSize>
#include <QVBoxLayout>

#include <klocale.h>

#include <KoPADocument.h>

#include "KPrPage.h"
#include "KPresenter.h"
#include "KPrView.h"
#include "pagelayout/KPrPageLayout.h"
#include "pagelayout/KPrPageLayouts.h"

#include <kdebug.h>

// this is needed so it can be used in a QVariant
Q_DECLARE_METATYPE( KPrPageLayout* )

KPrPageLayoutDocker::KPrPageLayoutDocker( QWidget* parent, Qt::WindowFlags flags )
: QDockWidget( parent, flags )
, m_view( 0 )
, m_previousItem( 0 )
{
    setWindowTitle( i18n( "Slide Layouts" ) );

    QWidget* base = new QWidget( this );
    m_layoutsView = new QListWidget( base );
    m_layoutsView->setIconSize( QSize( 80, 60 ) );
    m_layoutsView->setGridSize( QSize( 80, 60 ) );
    m_layoutsView->setViewMode( QListView::IconMode );
    m_layoutsView->setResizeMode( QListView::Adjust );
    m_layoutsView->setMovement( QListView::Static );
    m_layoutsView->setSelectionRectVisible(false);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( m_layoutsView );
    base->setLayout( layout );
    setWidget( base );
}

void KPrPageLayoutDocker::setView( KPrView* view )
{
    Q_ASSERT( view );
    m_view = view;
    connect( m_view->proxyObject, SIGNAL( activePageChanged() ),
             this, SLOT( slotActivePageChanged() ) );

    // remove the layouts from the last view
    m_layoutsView->clear();

    KPrPageLayouts *layouts = view->kopaDocument()->resourceManager()->resource(KPresenter::PageLayouts).value<KPrPageLayouts*>();

    Q_ASSERT( layouts );

    const QList<KPrPageLayout *> layoutMap = layouts->layouts();

    // TODO add empty layout

    foreach( KPrPageLayout * layout, layoutMap ) {
        if ( layout->type() == KPrPageLayout::Page ) {
            addLayout( layout );
        }
    }

    slotActivePageChanged();

    connect( m_layoutsView, SIGNAL( itemPressed( QListWidgetItem * ) ),
             this, SLOT( slotItemPressed( QListWidgetItem * ) ) );
    connect( m_layoutsView, SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem * ) ),
             this, SLOT( slotCurrentItemChanged( QListWidgetItem *, QListWidgetItem * ) ) );
}

void KPrPageLayoutDocker::slotActivePageChanged()
{
    Q_ASSERT( m_view );

    KPrPage * page = dynamic_cast<KPrPage*>( m_view->activePage() );
    if ( page ) {
        KPrPageLayout * layout = page->layout();
        QListWidgetItem * item = m_layout2item.value( layout, 0 );
        if ( item == 0 && layout != 0 && layout->type() == KPrPageLayout::Page ) {
            item = addLayout( layout );
        }

        if ( item ) {
            m_layoutsView->blockSignals( true );
            item->setSelected( true );
            m_layoutsView->blockSignals( false );
            m_layoutsView->scrollToItem( item );
        }
        else {
            QList<QListWidgetItem*> items = m_layoutsView->selectedItems();
            foreach ( QListWidgetItem * i, items ) {
                m_layoutsView->blockSignals( true );
                i->setSelected( false );
                m_layoutsView->blockSignals( false );
            }
        }
    }
}

void KPrPageLayoutDocker::slotItemPressed( QListWidgetItem * item )
{
    if ( item == m_previousItem ) {
        applyLayout( item );
    }
    else {
        m_previousItem = item;
    }
}

void KPrPageLayoutDocker::slotCurrentItemChanged( QListWidgetItem * item, QListWidgetItem * previous )
{
    applyLayout( item );
    m_previousItem = previous;
}


QListWidgetItem * KPrPageLayoutDocker::addLayout( KPrPageLayout * layout )
{
    QListWidgetItem * item = new QListWidgetItem( QIcon( layout->thumbnail() ), "", m_layoutsView );
    item->setData( Qt::UserRole, QVariant::fromValue( layout ) );
    m_layout2item.insert( layout, item );
    return item;
}

void KPrPageLayoutDocker::applyLayout( QListWidgetItem * item )
{
    // don't crash when all items are replaced
    if ( item ) {
        Q_ASSERT( m_view );
        KPrPage * page = dynamic_cast<KPrPage*>( m_view->activePage() );
        if ( page ) {
            page->setLayout( item->data( Qt::UserRole ).value<KPrPageLayout *>(), m_view->kopaDocument() );
        }
    }
}

#include "KPrPageLayoutDocker.moc"
