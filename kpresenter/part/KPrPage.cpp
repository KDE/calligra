/* This file is part of the KDE project
 * Copyright (C) 2007-2009 Thorsten Zachmann <zachmann@kde.org>
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

#include "KPrPage.h"

#include <QString>

#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoStyleStack.h>
#include <KoGenStyle.h>
#include <KoShapeLayer.h>
#include <KoPALoadingContext.h>
#include <KoPASavingContext.h>

#include "KPrDocument.h"
#include "KPrDeclarations.h"
#include "KPresenter.h"
#include "KPrPageApplicationData.h"
#include "KPrMasterPage.h"
#include "KPrNotes.h"
#include "KPrPlaceholderShape.h"
#include "KPrShapeManagerDisplayMasterStrategy.h"
#include "pagelayout/KPrPageLayout.h"
#include "pagelayout/KPrPageLayouts.h"
#include "pagelayout/KPrPageLayoutSharedSavingData.h"
#include "pagelayout/KPrPlaceholder.h"
#include "pageeffects/KPrPageEffectRegistry.h"
#include "pageeffects/KPrPageEffect.h"

#include <kdebug.h>

class KPrPage::Private
{
public:
    Private( KPrPage * page, KPrDocument * document )
    : pageNotes( new KPrNotes( page, document ) )
    , declarations( document->declarations() )
    {}

    ~Private()
    {
        delete pageNotes;
    }
    KPrNotes * pageNotes;
    QHash<KPrDeclarations::Type, QString> usedDeclaration;
    KPrDeclarations *declarations;

};

KPrPage::KPrPage( KoPAMasterPage * masterPage, KPrDocument * document )
: KoPAPage( masterPage )
, d( new Private( this, document ) )
{
    setApplicationData( new KPrPageApplicationData() );
    placeholders().init( 0, childShapes() );
}

KPrPage::~KPrPage()
{
    delete d;
}

KPrPageApplicationData * KPrPage::pageData( KoPAPageBase * page )
{
    KPrPageApplicationData * data = dynamic_cast<KPrPageApplicationData *>( page->applicationData() );
    Q_ASSERT( data );
    return data;
}

KPrNotes *KPrPage::pageNotes()
{
    return d->pageNotes;
}

void KPrPage::shapeAdded( KoShape * shape )
{
    Q_ASSERT( shape );
    placeholders().shapeAdded( shape );
}

void KPrPage::shapeRemoved( KoShape * shape )
{
    Q_ASSERT( shape );
    placeholders().shapeRemoved( shape );
}

void KPrPage::setLayout( KPrPageLayout * layout, KoPADocument * document )
{
    QSizeF pageSize( pageLayout().width, pageLayout().height );
    KPrMasterPage * master = dynamic_cast<KPrMasterPage *>( masterPage() );
    Q_ASSERT( master );
    placeholders().setLayout( layout, document, childShapes(), pageSize, master ? master->placeholders().styles() : QMap<QString, KoTextShapeData*>() );
    kDebug(33001) << "master placeholders";
    master->placeholders().debug();
}

KPrPageLayout * KPrPage::layout() const
{
    return placeholders().layout();
}

void KPrPage::saveOdfPageContent( KoPASavingContext & paContext ) const
{
    if ( layout() ) {
        KPrPageLayoutSharedSavingData * layouts = dynamic_cast<KPrPageLayoutSharedSavingData *>( paContext.sharedData( KPR_PAGE_LAYOUT_SHARED_SAVING_ID ) );
        Q_ASSERT( layouts );
        if ( layouts ) {
            QString layoutStyle = layouts->pageLayoutStyle( layout() );
            if ( ! layoutStyle.isEmpty() ) {
                paContext.xmlWriter().addAttribute( "presentation:presentation-page-layout-name", layoutStyle );
            }
        }
    }
    KoPAPageBase::saveOdfPageContent( paContext );
}

void KPrPage::saveOdfPageStyleData( KoGenStyle &style, KoPASavingContext &paContext ) const
{
    KoPAPage::saveOdfPageStyleData( style, paContext );
    style.addProperty( "presentation:background-visible", ( m_pageProperties & DisplayMasterBackground ) == DisplayMasterBackground );
    style.addProperty( "presentation:background-objects-visible", ( m_pageProperties & DisplayMasterShapes ) == DisplayMasterShapes );

    KPrPageApplicationData * data = dynamic_cast<KPrPageApplicationData *>( applicationData() );
    Q_ASSERT( data );
    KPrPageEffect * pageEffect = data->pageEffect();

    if ( pageEffect ) {
        pageEffect->saveOdfSmilAttributes( style );
    }
}

void KPrPage::loadOdfPageTag( const KoXmlElement &element, KoPALoadingContext &loadingContext )
{
    KoPAPage::loadOdfPageTag( element, loadingContext );

    KoStyleStack& styleStack = loadingContext.odfLoadingContext().styleStack();

    int pageProperties = m_pageProperties & UseMasterBackground;
    if ( styleStack.property( KoXmlNS::presentation, "background-objects-visible" ) == "true" ) {
        pageProperties = pageProperties | DisplayMasterShapes;
    }
    if ( styleStack.property( KoXmlNS::presentation, "background-visible" ) == "true" ) {
        pageProperties = pageProperties | DisplayMasterBackground;
    }
    m_pageProperties = pageProperties;

    KPrPageApplicationData * data = dynamic_cast<KPrPageApplicationData *>( applicationData() );
    Q_ASSERT( data );

    KoOdfStylesReader& stylesReader = loadingContext.odfLoadingContext().stylesReader();
    const KoXmlElement * styleElement = stylesReader.findContentAutoStyle( element.attributeNS( KoXmlNS::draw, "style-name" ), "drawing-page" );
    if ( styleElement ) {
#ifndef KOXML_USE_QDOM
        KoXmlNode node = styleElement->namedItemNS( KoXmlNS::style, "drawing-page-properties" );
#else
	KoXmlNode node; // XXX!!!
#endif
        if ( node.isElement() ) {
            data->setPageEffect( KPrPageEffectRegistry::instance()->createPageEffect( node.toElement() ) );
        }
    }
#ifndef KOXML_USE_QDOM
    KoXmlNode node = element.namedItemNS(KoXmlNS::presentation, "notes");
#else
    KoXmlNode node; //XXX!!!
#endif
    if ( node.isElement() ) {
        d->pageNotes->loadOdf(node.toElement(), loadingContext);
    }
}

void KPrPage::loadOdfPageExtra( const KoXmlElement &element, KoPALoadingContext & loadingContext )
{
    // the layout needs to be loaded after the shapes are already loaded so the initialization of the data works
    KPrPageLayout * layout = 0;
    if ( element.hasAttributeNS( KoXmlNS::presentation, "presentation-page-layout-name" ) ) {
        KPrPageLayouts *layouts = loadingContext.documentResourceManager()->resource(KPresenter::PageLayouts).value<KPrPageLayouts*>();

        Q_ASSERT( layouts );
        if ( layouts ) {
            QString layoutName = element.attributeNS( KoXmlNS::presentation, "presentation-page-layout-name" );
            QRectF pageRect( 0, 0, pageLayout().width, pageLayout().height );
            layout = layouts->pageLayout( layoutName, loadingContext, pageRect );
            kDebug(33001) << "page layout" << layoutName << layout;
        }
    }
    placeholders().init( layout, childShapes() );

    if (element.hasAttributeNS(KoXmlNS::presentation, "use-footer-name")) {
        QString name = element.attributeNS (KoXmlNS::presentation, "use-footer-name");
        d->usedDeclaration.insert(KPrDeclarations::Footer, name);
    }
    if (element.hasAttributeNS( KoXmlNS::presentation, "use-header-name")) {
        QString name = element.attributeNS (KoXmlNS::presentation, "use-header-name");
        d->usedDeclaration.insert(KPrDeclarations::Header, name);
    }
    if (element.hasAttributeNS( KoXmlNS::presentation, "use-date-time-name")) {
        QString name = element.attributeNS (KoXmlNS::presentation, "use-date-time-name");
        d->usedDeclaration.insert(KPrDeclarations::DateTime, name);
    }
}

bool KPrPage::saveOdfPresentationNotes(KoPASavingContext &paContext) const
{
    d->pageNotes->saveOdf(paContext);
    return true;
}

KoPageApp::PageType KPrPage::pageType() const
{
    return KoPageApp::Slide;
}

QString KPrPage::declaration(KPrDeclarations::Type type) const
{
    return d->declarations->declaration(type, d->usedDeclaration.value(type));
}

KoShapeManagerPaintingStrategy * KPrPage::getPaintingStrategy() const
{
    return new KPrShapeManagerDisplayMasterStrategy(0);
}
