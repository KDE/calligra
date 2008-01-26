/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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

#include "KarbonGradientChooser.h"
#include "KarbonGradientItem.h"

#include <KoResourceServerProvider.h>

#include <kfiledialog.h>
#include <klocale.h>

#include <QtGui/QResizeEvent>

KarbonGradientChooser::KarbonGradientChooser( QWidget *parent )
    : KoResourceItemChooser( parent ), m_checkerPainter( 4 )
{
    setIconSize( QSize(100, 20) );
    m_resourceAdapter = new KoResourceServerAdapter<KoAbstractGradient>(KoResourceServerProvider::instance()->gradientServer());

    connect( m_resourceAdapter, SIGNAL(resourceAdded(KoResource*)),
             this, SLOT(addGradient(KoResource*)));
    connect( m_resourceAdapter, SIGNAL(removingResource(KoResource*)), 
             this, SLOT(removeGradient(KoResource*)));

    m_resourceAdapter->connectToResourceServer();

    connect( this, SIGNAL( importClicked() ), this, SLOT( importGradient() ) );
    connect( this, SIGNAL( deleteClicked() ), this, SLOT( deleteGradient() ) );
}

KarbonGradientChooser::~KarbonGradientChooser()
{
    delete m_resourceAdapter;
}

void KarbonGradientChooser::addGradient(KoResource* resource)
{
    KoAbstractGradient * gradient = dynamic_cast<KoAbstractGradient*>( resource );
    if( gradient && gradient->valid() ) {
        KarbonGradientItem* item = new KarbonGradientItem( gradient, &m_checkerPainter );
        m_itemMap[resource] = item;
        addItem( item );
    }
}

void KarbonGradientChooser::removeGradient(KoResource* resource)
{
    KoResourceItem *item = m_itemMap[resource];

    if(item) {
        m_itemMap.remove(item->resource());
        removeItem( item );
    }
}

void KarbonGradientChooser::importGradient()
{
    QString filter( "*.svg *.kgr *.ggr" );
    QString filename = KFileDialog::getOpenFileName( KUrl(), filter, 0, i18n( "Choose Gradient to Add" ) );

    if(m_resourceAdapter)
        m_resourceAdapter->importResource(filename);
}

void KarbonGradientChooser::deleteGradient()
{
    if(! currentItem() )
        return;

    KoAbstractGradient * gradient = static_cast<KarbonGradientItem*>( currentItem() )->gradient();
    m_resourceAdapter->removeResource( gradient );
}

void KarbonGradientChooser::resizeEvent ( QResizeEvent * event )
{
//     KoResourceItemChooser::resizeEvent( event );

    QSize newSize( viewportSize().width(), iconSize().height() );
    setIconSize( newSize );
}

void KarbonGradientChooser::showEvent( QShowEvent * event )
{
    QSize newSize( viewportSize().width(), iconSize().height() );
    setIconSize( newSize );
}

#include "KarbonGradientChooser.moc"
