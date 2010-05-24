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
#include "animations/KPrAnimationLoader.h"

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
    placeholders().init( 0, shapes() );
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
    placeholders().setLayout( layout, document, shapes(), pageSize, master ? master->placeholders().styles() : QMap<QString, KoTextShapeData*>() );
    kDebug(33001) << "master placeholders";
    master->placeholders().debug();
}

KPrPageLayout * KPrPage::layout() const
{
    return placeholders().layout();
}

bool KPrPage::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    if (!KoPAPageBase::loadOdf(element, context)) {
        return false;
    }

    KoXmlElement animation = KoXml::namedItemNS(element, KoXmlNS::anim, "par");

    if (!animation.isNull()) {
        KoXmlElement animationElement;
        forEachElement(animationElement, animation) {
            if (animationElement.tagName() == "seq" && animationElement.namespaceURI() == KoXmlNS::anim) {
                QString nodeType(animationElement.attributeNS(KoXmlNS::presentation, "node-type"));
                if (nodeType == "main-sequence") {
                    KPrAnimationLoader al;
                    al.loadOdf(animationElement, context);
                }
                else {
                    // not yet supported
                }
            }
        }
    }
    return true;
}

void KPrPage::saveOdfPageContent( KoPASavingContext & paContext ) const
{
    KoXmlWriter &writer(paContext.xmlWriter());
    if ( layout() ) {
        KPrPageLayoutSharedSavingData * layouts = dynamic_cast<KPrPageLayoutSharedSavingData *>( paContext.sharedData( KPR_PAGE_LAYOUT_SHARED_SAVING_ID ) );
        Q_ASSERT( layouts );
        if ( layouts ) {
            QString layoutStyle = layouts->pageLayoutStyle( layout() );
            if ( ! layoutStyle.isEmpty() ) {
                writer.addAttribute( "presentation:presentation-page-layout-name", layoutStyle );
            }
        }
    }
    QHash<KPrDeclarations::Type, QString>::const_iterator it(d->usedDeclaration.constBegin());
    for (; it != d->usedDeclaration.constEnd(); ++it) {
        switch (it.key()) {
        case KPrDeclarations::Footer:
            writer.addAttribute("presentation:use-footer-name", it.value());
            break;
        case KPrDeclarations::Header:
            writer.addAttribute("presentation:use-header-name", it.value());
            break;
        case KPrDeclarations::DateTime:
            writer.addAttribute("presentation:use-date-time-name", it.value());
            break;
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
        pageProperties |= DisplayMasterShapes;
    }
    if ( styleStack.property( KoXmlNS::presentation, "background-visible" ) == "true" ) {
        pageProperties |= DisplayMasterBackground;
    }
    if ( styleStack.property( KoXmlNS::presentation, "display-header" ) == "true" ) {
        pageProperties |= DisplayHeader;
    }
    if ( styleStack.property( KoXmlNS::presentation, "display-footer" ) == "true" ) {
        pageProperties |= DisplayFooter;
    }
    if ( styleStack.property( KoXmlNS::presentation, "display-page-number" ) == "true" ) {
        pageProperties |= DisplayPageNumber;
    }
    if ( styleStack.property( KoXmlNS::presentation, "display-date-time" ) == "true" ) {
        pageProperties |= DisplayDateTime;
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
    placeholders().init( layout, shapes() );

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
