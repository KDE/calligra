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

class KPrPageLayoutWrapper
{
public:
    explicit KPrPageLayoutWrapper( KPrPageLayout * pageLayout )
    : layout( pageLayout )
    {}

    bool operator<( const KPrPageLayoutWrapper & other ) const
    {
        return *layout < *( other.layout );
    }

    KPrPageLayout * layout;
};

KPrPageLayouts::KPrPageLayouts()
{
}

KPrPageLayouts::~KPrPageLayouts()
{
    QMap<KPrPageLayoutWrapper, KPrPageLayout *>::iterator it( m_pageLayouts.begin() );
    for ( ; it != m_pageLayouts.end(); ++it ) {
        delete it.value();
    }
}

bool KPrPageLayouts::completeLoading( KoStore *store )
{
    Q_UNUSED( store );
    return true;
}

bool KPrPageLayouts::completeSaving( KoStore *store, KoXmlWriter * manifestWriter )
{
    Q_UNUSED( store );
    Q_UNUSED( manifestWriter );
    return true;
}

bool KPrPageLayouts::saveOdf( KoPASavingContext & context )
{
    KPrPageLayoutSharedSavingData * sharedData = new KPrPageLayoutSharedSavingData();

    QMap<KPrPageLayoutWrapper, KPrPageLayout *>::iterator it( m_pageLayouts.begin() );
    for ( ; it != m_pageLayouts.end(); ++it ) {
        QString style = it.value()->saveOdf( context );
        sharedData->addPageLayoutStyle( it.value(), style );
    }

    context.addSharedData( KPR_PAGE_LAYOUT_SHARED_SAVING_ID, sharedData );
    return true;
}

KPrPageLayout * KPrPageLayouts::pageLayout( const QString & name, KoPALoadingContext & loadingContext, const QRectF & pageRect )
{
    KPrPageLayout * pageLayout = 0;

    QHash<QString, KoXmlElement*> layouts = loadingContext.odfLoadingContext().stylesReader().presentationPageLayouts();
    QHash<QString, KoXmlElement*>::iterator it( layouts.find( name ) );

    if ( it != layouts.end() ) {
        pageLayout = new KPrPageLayout();
        if ( pageLayout->loadOdf( *( it.value() ), pageRect ) ) {
            QMap<KPrPageLayoutWrapper, KPrPageLayout *>::const_iterator it( m_pageLayouts.find( KPrPageLayoutWrapper( pageLayout ) ) );
            if ( it != m_pageLayouts.end() ) {
                delete pageLayout;
                pageLayout = *it;
            }
            else {
                m_pageLayouts.insert( KPrPageLayoutWrapper( pageLayout ), pageLayout );
            }
        }
        else {
            delete pageLayout;
            pageLayout = 0;
        }
    }
    return pageLayout;
}

const QList<KPrPageLayout *> KPrPageLayouts::layouts() const
{
    return m_pageLayouts.values();
}
