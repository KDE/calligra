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

#include <KoPageLayout.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoPALoadingContext.h>
#include <KoPASavingContext.h>
#include <KoPAMasterPage.h>

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

KPrPageLayouts::KPrPageLayouts(QObject *parent)
    : QObject(parent)
{
}

KPrPageLayouts::~KPrPageLayouts()
{
    QMap<KPrPageLayoutWrapper, KPrPageLayout *>::iterator it( m_pageLayouts.begin() );
    for ( ; it != m_pageLayouts.end(); ++it ) {
        delete it.value();
    }
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

bool compareLayouts( const KPrPageLayout * p1, const KPrPageLayout * p2 )
{
    return KPrPageLayout::compareByContent( *p1,* p2 );
}

bool KPrPageLayouts::loadOdf( KoPALoadingContext & context )
{
    QHash<QString, KoXmlElement*> layouts = context.odfLoadingContext().stylesReader().presentationPageLayouts();
    QHash<QString, KoXmlElement*>::iterator it( layouts.begin() );

    // TODO need to use the correct page size
    // we should check which layouts are already loaded
    const QMap<QString, KoPAMasterPage *> & masterPages = context.masterPages();
    if ( ! masterPages.isEmpty() ) {
        KoPageLayout & layout = masterPages.begin().value()->pageLayout();
        QRectF pageRect( 0, 0, layout.width, layout.height );
        for ( ; it != layouts.end(); ++it ) {
            KPrPageLayout * pageLayout = new KPrPageLayout();
            if ( pageLayout->loadOdf( *( it.value() ), pageRect ) ) {
                QMap<KPrPageLayoutWrapper, KPrPageLayout *>::const_iterator it( m_pageLayouts.constFind( KPrPageLayoutWrapper( pageLayout ) ) );
                if ( it != m_pageLayouts.constEnd() ) {
                    delete pageLayout;
                }
                else {
                    m_pageLayouts.insert( KPrPageLayoutWrapper( pageLayout ), pageLayout );
                }
            }
            else {
                delete pageLayout;
            }
        }
    }

    // handel default styles
    layouts = context.odfLoadingContext().defaultStylesReader().presentationPageLayouts();
    it = layouts.begin();
    QList<KPrPageLayout *> defaultLayouts;
    for ( ; it != layouts.end(); ++it ) {
        KPrPageLayout * pageLayout = new KPrPageLayout();
        // this is not used but needed
        QRectF pageRect( 0, 0, 800, 600 );
        if ( pageLayout->loadOdf( *( it.value() ), pageRect ) ) {
            defaultLayouts.append( pageLayout );
        }
        else {
            delete pageLayout;
        }
    }
    QList<KPrPageLayout *> documentLayouts = m_pageLayouts.values();

    qSort( documentLayouts.begin(), documentLayouts.end(), compareLayouts );
    qSort( defaultLayouts.begin(), defaultLayouts.end(), compareLayouts );

    QList<KPrPageLayout *>::const_iterator docIt = documentLayouts.constBegin();
    QList<KPrPageLayout *>::const_iterator defaultIt = defaultLayouts.constBegin();
    while ( defaultIt != defaultLayouts.constEnd() ) {
        if ( docIt == documentLayouts.constEnd() || compareLayouts( *defaultIt, *docIt ) ) {
            m_pageLayouts.insert( KPrPageLayoutWrapper( *defaultIt ), *defaultIt );
            ++defaultIt;
        }
        else if ( compareLayouts( *docIt, *defaultIt ) ) {
            ++docIt;
        }
        else {
            // it already exist
            ++docIt;
            ++defaultIt;
        }
    }

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
            QMap<KPrPageLayoutWrapper, KPrPageLayout *>::const_iterator it( m_pageLayouts.constFind( KPrPageLayoutWrapper( pageLayout ) ) );
            if ( it != m_pageLayouts.constEnd() ) {
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
