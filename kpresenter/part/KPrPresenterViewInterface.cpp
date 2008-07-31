/* This file is part of the KDE project
 * Copyright (C) 2008 Fredy Yanardi <fyanardi@gmail.com>
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

#include "KPrPresenterViewInterface.h"

#include <QtGui/QBoxLayout>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QTextEdit>
#include <QtGui/QToolButton>

#include <KDebug>
#include <KLocale>
#include <KIcon>

#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoPAPageThumbnailModel.h>
#include <KoPAView.h>
#include <KoPAViewMode.h>
#include <KoShape.h>
#include <KoTextShapeData.h>

#include "KPrEndOfSlideShowPage.h"
#include "KPrNotes.h"
#include "KPrPage.h"

KPrPresenterViewInterface::KPrPresenterViewInterface( const QList<KoPAPageBase *> &pages, KoPACanvas *canvas, QWidget *parent )
    : KPrPresenterViewBaseInterface( pages, parent )
    , m_canvas( canvas )
{
    QVBoxLayout *vLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;

    QFrame *frame = new QFrame;
    QVBoxLayout *frameLayout1 = new QVBoxLayout;
    m_currentSlideLabel = new QLabel;
    frameLayout1->addWidget( m_currentSlideLabel );
    frameLayout1->addWidget( m_canvas );
    frame->setLayout( frameLayout1 );
    // frame->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );

    hLayout->addWidget( frame );
    // I don't know why this always messes up the canvas position inside layout
    // hLayout->addStretch();

    frame = new QFrame;
    QVBoxLayout *frameLayout2 = new QVBoxLayout;
    m_nextSlideLabel = new QLabel( i18n( "Next Slide" ) );
    m_nextSlidePreview = new QLabel;
    frameLayout2->addWidget( m_nextSlideLabel );
    frameLayout2->addWidget( m_nextSlidePreview );
    frameLayout2->addStretch();
    frame->setLayout( frameLayout2 );
    // frame->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
    hLayout->addWidget( frame );

    vLayout->addLayout( hLayout );

    // vLayout->addSpacing( 20 );
    vLayout->addWidget( new QLabel( i18n( "Speaker Notes") ) );
    m_notesTextEdit = new QTextEdit;
    m_notesTextEdit->setReadOnly( true );
    vLayout->addWidget( m_notesTextEdit );

    setLayout( vLayout );
}

void KPrPresenterViewInterface::setActivePage( KoPAPageBase *page )
{
    KPrPresenterViewBaseInterface::setActivePage( page );

    int currentIndex = m_pages.indexOf( page );
    int pageCount = dynamic_cast<KPrEndOfSlideShowPage *>( m_pages.last() ) ?
            m_pages.count() - 1 : m_pages.count();

    // set the thumbnail for next page preview
    KoPAPageBase *nextPage = 0;
    if ( currentIndex != m_pages.count() - 1 && currentIndex != -1 ) {
        nextPage = m_pages.at( currentIndex + 1 );
        m_nextSlidePreview->setPixmap( nextPage->thumbnail( m_previewSize ) );
    }
    else { // End of presentation, just a black pixmap for the next slide preview
        QPixmap pixmap( m_previewSize );
        pixmap.fill( Qt::black );
        m_nextSlidePreview->setPixmap( pixmap );
    }

    // update the label
    m_currentSlideLabel->setText( currentIndex != -1 ? 
            i18n( "Current Slide %1 of %2", currentIndex + 1, pageCount ) : 
            i18n( "End of Slide Show" ) );

    // set the presentation notes
    KPrPage *prPage = dynamic_cast<KPrPage *>( page );
    Q_ASSERT( prPage );
    KPrNotes *pageNotes = prPage->pageNotes();
    KoShape *textShape = pageNotes->textShape();
    KoTextShapeData *textShapeData = dynamic_cast<KoTextShapeData *>( textShape->userData() );
    Q_ASSERT( textShapeData );
    QTextDocument *document = textShapeData->document()->clone( m_notesTextEdit );
    m_notesTextEdit->setDocument( document );
}

void KPrPresenterViewInterface::setPreviewSize( const QSize &size )
{
    m_previewSize = size;
    m_canvas->setFixedSize( size );

    // set the thumbnail for next page preview
    int currentIndex = m_pages.indexOf( m_activePage );

    Q_ASSERT( currentIndex != -1 );
    KoPAPageBase *nextPage = 0;
    if ( currentIndex != m_pages.count() - 1 ) {
        nextPage = m_pages.at( currentIndex + 1 );
    }
    else {
        nextPage = m_activePage;
    }
    m_nextSlidePreview->setPixmap( nextPage->thumbnail( m_previewSize ) );
}

#include "KPrPresenterViewInterface.moc"

