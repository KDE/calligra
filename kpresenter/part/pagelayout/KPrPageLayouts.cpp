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

#include "KPrPageLayouts.h"

#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoPALoadingContext.h>
#include <KoPASavingContext.h>

#include "KPrPageLayout.h"
#include "KPrPageLayoutSharedSavingData.h"

KPrPageLayouts::KPrPageLayouts()
{
}

KPrPageLayouts::~KPrPageLayouts()
{
    QMap<QString, KPrPageLayout *>::iterator it( m_pageLayouts.begin() );
    for ( ; it != m_pageLayouts.end(); ++it ) {
        delete it.value();
    }
}

bool KPrPageLayouts::saveOdf( KoPASavingContext & context )
{
    KPrPageLayoutSharedSavingData * sharedData = new KPrPageLayoutSharedSavingData();

    QMap<QString, KPrPageLayout *>::iterator it( m_pageLayouts.begin() );
    for ( ; it != m_pageLayouts.end(); ++it ) {
        QString style = it.value()->saveOdf( context );
        sharedData->addPageLayoutStyle( it.value(), style );
    }

    context.addSharedData( KPR_PAGE_LAYOUT_SHARED_SAVING_ID, sharedData );
    return true;
}

void KPrPageLayouts::loadOdf( KoPALoadingContext &loadingContext )
{
    QHash<QString, KoXmlElement*> layouts = loadingContext.odfLoadingContext().stylesReader().presentationPageLayouts();

    QHash<QString, KoXmlElement*>::iterator it( layouts.begin() );

    for ( ; it != layouts.end(); ++it ) {
        KPrPageLayout * pageLayout = new KPrPageLayout();
        if ( pageLayout->loadOdf( *( it.value() ), loadingContext ) ) {
            m_pageLayouts.insert( it.key(), pageLayout );
        }
        else {
            delete pageLayout;
        }
    }
}

KPrPageLayout * KPrPageLayouts::pageLayout( const QString & name )
{
    KPrPageLayout * pageLayout = 0;
    QMap<QString, KPrPageLayout *>::iterator it( m_pageLayouts.find( name ) );
    if ( it != m_pageLayouts.end() ) {
        pageLayout = it.value();
    }
    return pageLayout;
}
