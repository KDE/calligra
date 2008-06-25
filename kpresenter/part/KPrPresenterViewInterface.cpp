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

#include <QtCore/QTimer>
#include <QtGui/QBoxLayout>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QListView>
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

#include "KPrNotes.h"
#include "KPrPage.h"
#include "KPrViewModePresenterView.h"

/* KPrPresenterViewBaseInterface */
KPrPresenterViewBaseInterface::KPrPresenterViewBaseInterface( KoPADocument *document, QWidget *parent )
    : QWidget( parent )
    , m_document( document )
{
}

void KPrPresenterViewBaseInterface::setActivePage( KoPAPageBase *page )
{
    m_activePage = page;
}

/* KPrPresenterViewInterface
 * This widget is the main interface, this widget shows current slide, next slide
 * and the presenter's notes
 */
KPrPresenterViewInterface::KPrPresenterViewInterface( KoPADocument *document, KoPACanvas *canvas, QWidget *parent )
    : KPrPresenterViewBaseInterface( document, parent )
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
    frameLayout2 = new QVBoxLayout;
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

    // set the thumbnail for next page preview
    KoPAPageBase *nextPage = m_document->pageByNavigation( page, KoPageApp::PageNext );
    if ( nextPage ) {
        m_nextSlidePreview->setPixmap( nextPage->thumbnail( m_previewSize ) );
    }

    // update the label
    int slideIndex = m_document->pageIndex( page ) + 1;
    m_currentSlideLabel->setText( i18n( "Current Slide %1 of %2", slideIndex, m_document->pageCount() )  );

    // set the presentation notes
    KPrPage *prPage = dynamic_cast<KPrPage *>( page );
    Q_ASSERT( prPage );
    KPrNotes *pageNotes = prPage->pageNotes();
    KoShape *textShape = pageNotes->textShape();
    KoTextShapeData *textShapeData = dynamic_cast<KoTextShapeData *>( textShape->userData() );
    Q_ASSERT( textShapeData );
    m_notesTextEdit->setDocument( textShapeData->document() );
}

void KPrPresenterViewInterface::setPreviewSize( const QSize &size )
{
    m_previewSize = size;
    m_canvas->setFixedSize( size );

    KoPAPageBase *nextPage = m_document->pageByNavigation( m_activePage, KoPageApp::PageNext );
    if ( nextPage ) {
        m_nextSlidePreview->setPixmap( nextPage->thumbnail( m_previewSize ) );
    }
}

/* KPrPresenterViewSlidesInterface
 * This widget shows all slides in the presentation
 */
KPrPresenterViewSlidesInterface::KPrPresenterViewSlidesInterface( KoPADocument *document, QWidget *parent )
    : KPrPresenterViewBaseInterface( document, parent )
{
    QVBoxLayout *vLayout = new QVBoxLayout;

    QListView *listView = new QListView;
    m_thumbnailModel = new KoPAPageThumbnailModel( m_document->pages( false ) );
    listView->setModel( m_thumbnailModel );
    listView->setDragDropMode( QListView::NoDragDrop );
    listView->setIconSize( QSize( 128, 128 ) );
    listView->setViewMode( QListView::IconMode );
    listView->setFlow( QListView::LeftToRight );
    listView->setWrapping( true );
    listView->setResizeMode( QListView::Adjust );
    listView->setSelectionMode( QAbstractItemView::SingleSelection );
    listView->setMovement( QListView::Static );

    vLayout->addWidget( listView );

    setLayout( vLayout );
}

void KPrPresenterViewSlidesInterface::setActivePage( KoPAPageBase *page )
{
    KPrPresenterViewBaseInterface::setActivePage( page );
}

/* KPrPresenterViewToolWidget
 * This widget shows all navigation functions (previous and next slide) together
 * with clock and timer
 */
KPrPresenterViewToolWidget::KPrPresenterViewToolWidget(QWidget *parent)
    : QFrame(parent)
{
    QSize iconSize( 32, 32 );
    QHBoxLayout *mainLayout = new QHBoxLayout;

    QHBoxLayout *hLayout = new QHBoxLayout;
    QToolButton *toolButton = new QToolButton;
    toolButton->setIcon( KIcon( "go-previous" ) );
    toolButton->setIconSize( iconSize );
    connect( toolButton, SIGNAL( clicked() ), this, SIGNAL( previousSlideClicked() ) );
    hLayout->addWidget(toolButton);
    toolButton = new QToolButton;
    toolButton->setIcon( KIcon( "go-next" ) );
    toolButton->setIconSize( iconSize );
    connect( toolButton, SIGNAL( clicked() ), this, SIGNAL( nextSlideClicked() ) );
    hLayout->addWidget( toolButton );

    mainLayout->addLayout(hLayout);
    mainLayout->addSpacing( 5 );
    QFrame *frame = new QFrame;
    frame->setFrameStyle(QFrame::VLine | QFrame::Sunken);
    mainLayout->addWidget(frame);
    mainLayout->addSpacing( 5 );

    toolButton = new QToolButton;
    toolButton->setCheckable( true );
    toolButton->setIcon( KIcon( "view-list-icons.png" ) );
    toolButton->setIconSize( iconSize );
    connect( toolButton, SIGNAL( toggled( bool ) ), this, SIGNAL( slideThumbnailsToggled( bool ) ) );
    mainLayout->addWidget( toolButton );

    mainLayout->addSpacing( 5 );
    frame = new QFrame;
    frame->setFrameStyle( QFrame::VLine | QFrame::Raised );
    mainLayout->addWidget( frame );
    mainLayout->addSpacing( 5 );

    hLayout = new QHBoxLayout;
    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap( KIcon( "user-away").pixmap( iconSize ) );
    hLayout->addWidget( iconLabel );
    m_clockLabel = new QLabel( QTime::currentTime().toString( "hh:mm:ss ap" ) );
    m_clockLabel->setStyleSheet("QLabel { font-size: 24px }");
    hLayout->addWidget( m_clockLabel );
    mainLayout->addLayout( hLayout );

    mainLayout->addSpacing( 5 );
    frame = new QFrame;
    frame->setFrameStyle( QFrame::VLine | QFrame::Plain );
    mainLayout->addWidget(frame);
    mainLayout->addSpacing(5);

    hLayout = new QHBoxLayout;
    iconLabel = new QLabel;
    iconLabel->setPixmap( KIcon( "chronometer" ).pixmap( iconSize ) );
    hLayout->addWidget(iconLabel);
    m_timerLabel = new QLabel( "00:00:00");
    m_timerLabel->setStyleSheet("QLabel { font-size: 24px }");
    hLayout->addWidget( m_timerLabel );
    mainLayout->addLayout(hLayout);

    setLayout(mainLayout);
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    m_clockTimer = new QTimer( this );
    connect( m_clockTimer, SIGNAL( timeout() ), this, SLOT( updateClock() ) );
    m_clockTimer->start( 1000 );

    hour = 0;
    min = 0;
    sec = 0;
}

void KPrPresenterViewToolWidget::updateClock()
{
    QTime time = QTime::currentTime();
    m_clockLabel->setText( time.toString( "hh:mm:ss a" ) );

    sec++;
    if ( sec == 60 ) {
        min++;
        sec = 0;
    }
    if ( min == 60 ) {
        hour++;
        min = 0;
    }

    // display the timer, with 0 appended if only 1 digit
    m_timerLabel->setText( QString( "%1:%2:%3").arg( hour, 2, 10, QChar( '0' ) )
            .arg( min, 2, 10, QChar( '0' ) ).arg( sec, 2, 10, QChar( '0' ) ) );
}

#include "KPrPresenterViewInterface.moc"

