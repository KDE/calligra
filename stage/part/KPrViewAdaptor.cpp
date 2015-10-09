/*  This file is part of the KDE project
    Copyright (C) 2008 James Hogan <james@albanarts.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA
*/

#include "KPrViewAdaptor.h"
#include "KPrView.h"
#include "KPrViewModePresentation.h"
#include "KPrAnimationDirector.h"
#include "KPrDocument.h"
#include "KPrNotes.h"
#include "KPrPage.h"
#include <KoTextShapeData.h>

#include <QUrl>
#include <QTextDocument>

KPrViewAdaptor::KPrViewAdaptor( KPrView* view )
: KoViewAdaptor( view )
, m_view( view )
{
    KPrDocument *doc = m_view->kprDocument();
    connect( doc, SIGNAL(activeCustomSlideShowChanged(QString)), this, SIGNAL(activeCustomSlideShowChanged(QString)) );
    connect( doc, SIGNAL(customSlideShowsModified()), this, SIGNAL(customSlideShowsModified()) );

    // We need to know when the presentation is started and stopped, and when it is navigated
    connect( m_view->presentationMode(), SIGNAL(activated()), this, SLOT(presentationActivated()) );
    connect( m_view->presentationMode(), SIGNAL(deactivated()), this, SIGNAL(presentationStopped()) );
    connect( m_view->presentationMode(), SIGNAL(pageChanged(int,int)), this, SIGNAL(presentationPageChanged(int,int)) );
    connect( m_view->presentationMode(), SIGNAL(stepChanged(int)), this, SIGNAL(presentationStepChanged(int)) );
}

KPrViewAdaptor::~KPrViewAdaptor()
{
}

// custom slideshows

QStringList KPrViewAdaptor::customSlideShows() const
{
    KPrDocument *doc = m_view->kprDocument();
    return doc->customSlideShows()->names();
}

QString KPrViewAdaptor::activeCustomSlideShow() const
{
    KPrDocument *doc = m_view->kprDocument();
    return doc->activeCustomSlideShow();
}

bool KPrViewAdaptor::setActiveCustomSlideShow( const QString &name )
{
    // Check that the custom slideshow exists
    if ( name.isEmpty() || customSlideShows().contains( name ) ) {
        KPrDocument *doc = m_view->kprDocument();
        doc->setActiveCustomSlideShow( name );
        return true;
    }
    else {
        return false;
    }
}

// slides in the custom slideshow

int KPrViewAdaptor::numCustomSlideShowSlides() const
{
    KPrDocument *doc = m_view->kprDocument();
    return doc->slideShow().size();
}

QString KPrViewAdaptor::pageName( int page ) const
{
    KPrDocument *doc = m_view->kprDocument();

    QList<KoPAPageBase *> slideShow = doc->slideShow();
    if ( page >= 0 && page < slideShow.size() ) {
        return slideShow[page]->name();
    }
    return QString();
}

QString KPrViewAdaptor::pageNotes( int page, const QString &format ) const
{
    KPrDocument *doc = m_view->kprDocument();

    QList<KoPAPageBase *> slideShow = doc->slideShow();
    if ( page >= 0 && page < slideShow.size() ) {
        KPrPage *prPage = dynamic_cast<KPrPage *>( slideShow[page] );
        Q_ASSERT( 0 != prPage );
        if ( 0 != prPage ) {
            KPrNotes *pageNotes = prPage->pageNotes();
            KoShape *textShape = pageNotes->textShape();
            KoTextShapeData *textShapeData = qobject_cast<KoTextShapeData *>( textShape->userData() );
            Q_ASSERT( 0 != textShapeData );
            if ( 0 != textShapeData ) {
                if ( format == "plain" ) {
                    return textShapeData->document()->toPlainText();
                }
                else if ( format == "html" ) {
                    return textShapeData->document()->toHtml();
                }
            }
        }
    }
    return QString();
}

bool KPrViewAdaptor::exportPageThumbnail( int page, int width, int height,
                                          const QString &filename, const QString &format, int quality )
{
    KPrDocument *doc = m_view->kprDocument();

    QList<KoPAPageBase *> slideShow = doc->slideShow();
    if ( page >= 0 && page < slideShow.size() ) {
        KoPAPageBase *pageObject = slideShow[page];
        Q_ASSERT( pageObject );
        return m_view->exportPageThumbnail( pageObject, QUrl::fromLocalFile( filename ),
                                            QSize( qMax( 0, width ), qMax( 0, height ) ),
                                            format.isEmpty() ? "PNG" : format.toLatin1(),
                                            qBound( -1, quality, 100 ) );
    }
    else {
        return false;
    }
}
    
// Presentation control

void KPrViewAdaptor::presentationStart()
{
    m_view->startPresentation();
}

void KPrViewAdaptor::presentationStartFromFirst()
{
    m_view->startPresentationFromBeginning();
}

void KPrViewAdaptor::presentationStop()
{
    m_view->stopPresentation();
}

void KPrViewAdaptor::presentationPrev()
{
    if ( m_view->isPresentationRunning() ) {
        m_view->presentationMode()->navigate( KPrAnimationDirector::PreviousStep );
    }
}

void KPrViewAdaptor::presentationNext()
{
    if ( m_view->isPresentationRunning() ) {
        m_view->presentationMode()->navigate( KPrAnimationDirector::NextStep );
    }
}

void KPrViewAdaptor::presentationPrevSlide()
{
    if ( m_view->isPresentationRunning() ) {
        m_view->presentationMode()->navigate( KPrAnimationDirector::PreviousPage );
    }
}

void KPrViewAdaptor::presentationNextSlide()
{
    if ( m_view->isPresentationRunning() ) {
        m_view->presentationMode()->navigate( KPrAnimationDirector::NextPage );
    }
}

void KPrViewAdaptor::presentationFirst()
{
    if ( m_view->isPresentationRunning() ) {
        m_view->presentationMode()->navigate( KPrAnimationDirector::FirstPage );
    }
}

void KPrViewAdaptor::presentationLast()
{
    if ( m_view->isPresentationRunning() ) {
        m_view->presentationMode()->navigate( KPrAnimationDirector::LastPage );
    }
}

void KPrViewAdaptor::gotoPresentationPage( int pg )
{
    if ( m_view->isPresentationRunning() ) {
        m_view->presentationMode()->navigateToPage( pg );
    }
}

// Presentation accessors

bool KPrViewAdaptor::isPresentationRunning() const
{
    return m_view->isPresentationRunning();
}

int KPrViewAdaptor::currentPresentationPage() const
{
    if ( m_view->isPresentationRunning() ) {
        return m_view->presentationMode()->currentPage();
    }
    else {
        return -1;
    }
}

int KPrViewAdaptor::currentPresentationStep() const
{
    if ( m_view->isPresentationRunning() ) {
        return m_view->presentationMode()->currentStep();
    }
    else {
        return -1;
    }
}

int KPrViewAdaptor::numStepsInPresentationPage() const
{
    if ( m_view->isPresentationRunning() ) {
        return m_view->presentationMode()->numStepsInPage();
    }
    else {
        return -1;
    }
}

int KPrViewAdaptor::numPresentationPages() const
{
    if ( m_view->isPresentationRunning() ) {
        return m_view->presentationMode()->numPages();
    }
    else {
        return -1;
    }
}

/**
 * Fired when the presentation is activated.
 */
void KPrViewAdaptor::presentationActivated()
{
    emit presentationStarted( numPresentationPages() );
}

