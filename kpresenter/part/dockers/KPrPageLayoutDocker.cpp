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
#include "KPrView.h"
#include "pagelayout/KPrPageLayout.h"
#include "pagelayout/KPrPageLayouts.h"

// this is needed so it can be used in a QVariant
Q_DECLARE_METATYPE( KPrPageLayout* )

KPrPageLayoutDocker::KPrPageLayoutDocker( QWidget* parent, Qt::WindowFlags flags )
: QDockWidget( parent, flags )
, m_view( 0 )
{
    setWindowTitle( i18n( "Page layouts" ) );

    QWidget* base = new QWidget( this );
    m_layoutsView = new QListWidget( base );
    m_layoutsView->setIconSize( QSize( 80, 60 ) );

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( m_layoutsView );
    base->setLayout( layout );
    setWidget( base );
}

void KPrPageLayoutDocker::setView( KPrView* view )
{
    Q_ASSERT( view );
    m_view = view;
    connect( m_view, SIGNAL( activePageChanged() ),
             this, SLOT( slotActivePageChanged() ) );

    KPrPageLayouts * layouts = dynamic_cast<KPrPageLayouts *>( view->kopaDocument()->dataCenterMap()[ PageLayouts ] );
    Q_ASSERT( layouts );

    const QMap<QString, KPrPageLayout *> & layoutMap = layouts->layouts();

    // TODO add empty layout

    foreach( KPrPageLayout * layout, layoutMap ) {
        QListWidgetItem * item = new QListWidgetItem( QIcon( layout->thumbnail() ), "TODO", m_layoutsView );
        item->setData( Qt::UserRole, QVariant::fromValue( layout ) );
        m_layout2item.insert( layout, item );
    }

    slotActivePageChanged();

    connect( m_layoutsView, SIGNAL( itemSelectionChanged() ),
             this, SLOT( slotSelectionChanged() ) );
}

void KPrPageLayoutDocker::slotActivePageChanged()
{
    Q_ASSERT( m_view );

    KPrPage * page = dynamic_cast<KPrPage*>( m_view->activePage() );
    if ( page ) {
        KPrPageLayout * layout = page->layout();
        QListWidgetItem * item = m_layout2item.value( layout, 0 );
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

void KPrPageLayoutDocker::slotSelectionChanged()
{
    Q_ASSERT( m_view );
    KPrPage * page = dynamic_cast<KPrPage*>( m_view->activePage() );
    if ( page ) {
        QList<QListWidgetItem *> items = m_layoutsView->selectedItems();
        if ( !items.isEmpty() ) {
            page->setLayout( items.at( 0 )->data( Qt::UserRole ).value<KPrPageLayout *>(), m_view->kopaDocument() );
        }
    }
}

#include "KPrPageLayoutDocker.moc"
