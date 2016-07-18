/* This file is part of the KDE project
   Copyright (C) 2006-2010 Thorsten Zachmann <zachmann@kde.org>
  Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrDocument.h"


#include "KPrView.h"
#include "KPresenter.h"
#include "KPrPage.h"
#include "KPrMasterPage.h"
#include "KPrShapeApplicationData.h"
#include "KPrFactory.h"
#include "KPrViewModeNotes.h"
#include "KPrPlaceholderShapeFactory.h"
#include "KPrSoundCollection.h"
#include "KPrDeclarations.h"
#include "pagelayout/KPrPageLayouts.h"
#include "tools/KPrPlaceholderToolFactory.h"
#include "commands/KPrDeleteSlidesCommand.h"
#include <KoPASavingContext.h>
#include <KoPALoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <KSharedConfig>

#include <QTimer>
#include <QCoreApplication>
#include <QGlobalStatic>
#include <QStandardPaths>

class InitOnce
{
public:
    InitOnce()
    {
        KoToolRegistry * toolRegistry = KoToolRegistry::instance();
        toolRegistry->add( new KPrPlaceholderToolFactory() );
        KoShapeRegistry * registry = KoShapeRegistry::instance();
        registry->addFactory( new KPrPlaceholderShapeFactory() );
    }
};

Q_GLOBAL_STATIC( InitOnce, s_initOnce );

KPrDocument::KPrDocument(KoPart *part)
: KoPADocument(part)
, m_customSlideShows(new KPrCustomSlideShows())
, m_presentationMonitor( 0 )
, m_presenterViewEnabled( false )
, m_declarations( new KPrDeclarations() )
{
    // TODO: revisit if this is the place where that initialization should happen
    InitOnce * initOnce = s_initOnce;
    // have this is as otherwise we get a warning from the compiler
    // the variable is used and the way it is done is to only call it once
    Q_UNUSED( initOnce );

    KoShapeLoadingContext::addAdditionalAttributeData( KoShapeLoadingContext::AdditionalAttributeData(
                                                       KoXmlNS::presentation, "placeholder",
                                                       "presentation:placeholder" ) );

    KoShapeLoadingContext::addAdditionalAttributeData( KoShapeLoadingContext::AdditionalAttributeData(
                                                       KoXmlNS::presentation, "class",
                                                       "presentation:class" ) );

    QVariant variant;
    variant.setValue(new KPrSoundCollection(this));
    resourceManager()->setResource(KPresenter::SoundCollection, variant);

    variant.setValue(new KPrPageLayouts(this));
    resourceManager()->setResource(KPresenter::PageLayouts, variant);

    loadKPrConfig();
}

KPrDocument::~KPrDocument()
{
    saveKPrConfig();
    delete m_customSlideShows;
    delete m_declarations;
}

const char * KPrDocument::odfTagName( bool withNamespace )
{
    return withNamespace ? "office:presentation": "presentation";
}

bool KPrDocument::saveOdfProlog( KoPASavingContext & context )
{
    if ( !KoPADocument::saveOdfProlog( context ) )
        return false;
    m_declarations->saveOdf( context );
    return true;
}

bool KPrDocument::saveOdfEpilogue( KoPASavingContext & context )
{
    context.xmlWriter().startElement( "presentation:settings" );
    if ( !m_activeCustomSlideShow.isEmpty() && m_customSlideShows->names().contains(  m_activeCustomSlideShow ) ) {
        context.xmlWriter().addAttribute( "presentation:show", m_activeCustomSlideShow );
    }
    m_customSlideShows->saveOdf( context );
    context.xmlWriter().endElement();//presentation:settings
    return true;
}

void KPrDocument::saveOdfDocumentStyles( KoPASavingContext & context )
{
    KoPADocument::saveOdfDocumentStyles( context );
    KPrPageLayouts *layouts = resourceManager()->resource(KPresenter::PageLayouts).value<KPrPageLayouts*>();

    Q_ASSERT( layouts );
    if ( layouts ) {
        layouts->saveOdf( context );
    }
}

bool KPrDocument::loadOdfEpilogue( const KoXmlElement & body, KoPALoadingContext & context )
{
    const KoXmlElement & presentationSettings( KoXml::namedItemNS( body, KoXmlNS::presentation, "settings" ) );
    if ( !presentationSettings.isNull() ) {
        m_customSlideShows->loadOdf( presentationSettings, context );
    }

    m_activeCustomSlideShow = QString( "" );
    if ( presentationSettings.hasAttributeNS( KoXmlNS::presentation, "show" ) ) {
        QString show = presentationSettings.attributeNS( KoXmlNS::presentation, "show" );
        if ( m_customSlideShows->names().contains( show ) ) {
            m_activeCustomSlideShow = show;
        }
    }

    return true;
}

bool KPrDocument::loadOdfDocumentStyles( KoPALoadingContext & context )
{
    KPrPageLayouts *layouts = resourceManager()->resource(KPresenter::PageLayouts).value<KPrPageLayouts*>();
    if ( layouts ) {
        layouts->loadOdf( context );
    }
    return true;
}

KoPAPage * KPrDocument::newPage( KoPAMasterPage * masterPage )
{
    Q_ASSERT(masterPage);
    return new KPrPage( masterPage, this );
}

KoPAMasterPage * KPrDocument::newMasterPage()
{
    return new KPrMasterPage(this);
}

KoOdf::DocumentType KPrDocument::documentType() const
{
    return KoOdf::Presentation;
}

void KPrDocument::addAnimation( KPrShapeAnimation * animation )
{
    KoShape * shape = animation->shape();

    KPrShapeAnimations & animations( animationsByPage( pageByShape( shape ) ) );

    // add animation to the list of animations
    animations.add( animation );

    // add animation to the shape animation data so that it can be regenerated on delete shape and undo
    KPrShapeApplicationData * applicationData = dynamic_cast<KPrShapeApplicationData*>( shape->applicationData() );
    if ( applicationData == 0 ) {
        applicationData = new KPrShapeApplicationData();
        shape->setApplicationData( applicationData );
    }
    applicationData->animations().insert( animation );
    applicationData->setDeleteAnimations(false);
}

void KPrDocument::removeAnimation( KPrShapeAnimation * animation, bool removeFromApplicationData )
{
    KoShape * shape = animation->shape();

    KPrShapeAnimations & animations( animationsByPage( pageByShape( shape ) ) );

    // remove animation from the list of animations
    animations.remove( animation );

    KPrShapeApplicationData *applicationData = dynamic_cast<KPrShapeApplicationData*>(shape->applicationData());
    Q_ASSERT(applicationData);
    if (removeFromApplicationData) {
        // remove animation from the shape animation data
        applicationData->animations().remove(animation);
    } else {
        applicationData->setDeleteAnimations(true);
    }
}

void KPrDocument::replaceAnimation(KPrShapeAnimation *oldAnimation, KPrShapeAnimation *newAnimation)
{
    KoShape *shape = oldAnimation->shape();
    KPrShapeAnimations &animations(animationsByPage(pageByShape(shape)));

    // remove animation from the list of animations
    animations.replaceAnimation(oldAnimation, newAnimation);
}

void KPrDocument::postAddShape( KoPAPageBase * page, KoShape * shape )
{
    Q_UNUSED( page );
    KPrShapeApplicationData * applicationData = dynamic_cast<KPrShapeApplicationData*>( shape->applicationData() );
    if ( applicationData ) {
        // reinsert animations. this is needed on undo of a delete shape that had a animations
        QSet<KPrShapeAnimation *> animations = applicationData->animations();
        for ( QSet<KPrShapeAnimation *>::const_iterator it( animations.begin() ); it != animations.end(); ++it ) {
            addAnimation( *it );
        }
    }
}

void KPrDocument::postRemoveShape( KoPAPageBase * page, KoShape * shape )
{
    Q_UNUSED( page );
    KPrShapeApplicationData * applicationData = dynamic_cast<KPrShapeApplicationData*>( shape->applicationData() );
    if ( applicationData ) {
        QSet<KPrShapeAnimation *> animations = applicationData->animations();
        for ( QSet<KPrShapeAnimation *>::const_iterator it( animations.begin() ); it != animations.end(); ++it ) {
            // remove animations, don't remove from shape application data so that it can be reinserted on undo.
            removeAnimation( *it, false );
        }
    }
}

void KPrDocument::removePages(QList<KoPAPageBase *> &pages)
{
    KPrDeleteSlidesCommand *command = new KPrDeleteSlidesCommand(this, pages);
    addCommand(command);
}

void KPrDocument::loadKPrConfig()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();

    if ( config->hasGroup( "SlideShow" ) ) {
        KConfigGroup configGroup = config->group( "SlideShow" );
        m_presentationMonitor = configGroup.readEntry<int>( "PresentationMonitor", 0 );
        m_presenterViewEnabled = configGroup.readEntry<bool>( "PresenterViewEnabled", false );
    }
}

void KPrDocument::saveKPrConfig()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup configGroup = config->group( "SlideShow" );

    configGroup.writeEntry( "PresentationMonitor", m_presentationMonitor );
    configGroup.writeEntry( "PresenterViewEnabled", m_presenterViewEnabled );
}

KoPageApp::PageType KPrDocument::pageType() const
{
    return KoPageApp::Slide;
}

void KPrDocument::initEmpty()
{
    QString fileName(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("calligrastage/templates/Screen/.source/emptyLandscape.otp")));
    setModified( true );
    bool ok = loadNativeFormat( fileName );
    if ( !ok ) {
        // use initEmpty from  kopageapp
        showLoadingErrorDialog();
        KoPADocument::initEmpty();
    }
    resetURL();
}

KPrShapeAnimations & KPrDocument::animationsByPage( KoPAPageBase * page )
{
    KPrPageData * pageData = dynamic_cast<KPrPageData *>( page );
    Q_ASSERT( pageData );
    return pageData->animations();
}

KPrCustomSlideShows* KPrDocument::customSlideShows()
{
    return m_customSlideShows;
}

void KPrDocument::setCustomSlideShows( KPrCustomSlideShows* replacement )
{
    m_customSlideShows = replacement;
    emit customSlideShowsModified();
}

int KPrDocument::presentationMonitor()
{
    return m_presentationMonitor;
}

void KPrDocument::setPresentationMonitor( int monitor )
{
    m_presentationMonitor = monitor;
}

bool KPrDocument::isPresenterViewEnabled()
{
    return m_presenterViewEnabled;
}

void KPrDocument::setPresenterViewEnabled( bool enabled )
{
    m_presenterViewEnabled = enabled;
}

QList<KoPAPageBase*> KPrDocument::slideShow() const
{
    if ( !m_activeCustomSlideShow.isEmpty() &&
            m_customSlideShows->names().contains( m_activeCustomSlideShow ) ) {
        return m_customSlideShows->getByName( m_activeCustomSlideShow );
    }

    return pages();
}

QString KPrDocument::activeCustomSlideShow() const
{
    return m_activeCustomSlideShow;
}

void KPrDocument::setActiveCustomSlideShow( const QString &customSlideShow )
{
    if ( customSlideShow != m_activeCustomSlideShow ) {
        m_activeCustomSlideShow = customSlideShow;
        emit activeCustomSlideShowChanged( customSlideShow );
    }
}

bool KPrDocument::loadOdfProlog( const KoXmlElement & body, KoPALoadingContext & context )
{
    if ( !KoPADocument::loadOdfProlog( body, context ) )
        return false;
    return m_declarations->loadOdf( body, context );
}

KPrDeclarations * KPrDocument::declarations() const
{
    return m_declarations;
}
