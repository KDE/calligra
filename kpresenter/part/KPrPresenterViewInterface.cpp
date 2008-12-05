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

#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QTextEdit>
#include <QtGui/QToolButton>
#include <QTimeEdit>
#include <QComboBox>

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
    QVBoxLayout *frameLayout = new QVBoxLayout;
    m_currentSlideLabel = new QLabel;
    frameLayout->addWidget( m_currentSlideLabel, 0, Qt::AlignHCenter );
    frameLayout->addWidget( m_canvas );
    frameLayout->addStretch();
    frame->setLayout( frameLayout );

    hLayout->addWidget( frame );
    hLayout->addStretch();

    frame = new QFrame;
    frameNextLayout = new QVBoxLayout;
    m_nextSlideLabel = new QLabel( i18n( "Next Slide" ) );
    m_nextSlidePreview = new QLabel;
    frameNextLayout->addWidget( m_nextSlideLabel, 0, Qt::AlignHCenter );
    frameNextLayout->addWidget( m_nextSlidePreview );
    frameNextLayout->addStretch();
    frame->setLayout( frameNextLayout );
    hLayout->addWidget( frame );

    vLayout->addLayout( hLayout );

    vLayout->addWidget( new QLabel( i18n( "Speaker Notes") ) );
    m_notesTextEdit = new QTextEdit;
    m_notesTextEdit->setReadOnly( true );
    vLayout->addWidget( m_notesTextEdit );

    setLayout( vLayout );
    
    m_timeSlideWidget = NULL;
    m_slides_time = NULL;
    timeEdit = new QTimeEdit();
    slideBox = new QComboBox();
    connect( slideBox,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChanged(int)) );
    
}

void KPrPresenterViewInterface::setActivePage( int pageIndex )
{
    KPrPresenterViewBaseInterface::setActivePage( pageIndex );

    Q_ASSERT(pageIndex >= 0 && pageIndex < m_pages.size());
    KoPAPageBase *page = m_pages.at( pageIndex );
    int pageCount = dynamic_cast<KPrEndOfSlideShowPage *>( m_pages.last() ) ?
            m_pages.count() - 1 : m_pages.count();

    // set the thumbnail for next page preview
    KoPAPageBase *nextPage = 0;
    if ( pageIndex != pageCount ) {
        nextPage = m_pages.at( pageIndex + 1 );
        m_nextSlidePreview->setPixmap( nextPage->thumbnail( m_previewSize ) );
	m_nextSlideLabel->setText(i18n( "Next Slide" ));
	m_nextSlidePreview->setVisible(true);
	if(m_timeSlideWidget != NULL)
	  m_timeSlideWidget->setVisible(false);
    }
    else { // End of presentation, show time for each slide
	m_nextSlideLabel->setText(i18n( "Slides Time" ));
	// first display of slides time
	if(m_timeSlideWidget == NULL)	{
	    createSlideTime();
	    frameNextLayout->insertWidget(1,m_timeSlideWidget,0,Qt::AlignLeft);
	    frameNextLayout->addStretch();
	}
	else
	    m_timeSlideWidget->setVisible(true);
	m_nextSlidePreview->setVisible(false);
    }

    // update the label
    m_currentSlideLabel->setText( pageIndex != pageCount ? 
            i18n( "Current Slide %1 of %2", pageIndex + 1, pageCount ) : 
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
    Q_ASSERT( m_activePage != -1 );
    KoPAPageBase *nextPage = 0;
    if ( m_activePage != m_pages.count() - 1 ) {
        nextPage = m_pages.at( m_activePage + 1 );
    }
    else {
        nextPage = m_pages.at( m_activePage );
    }
    m_nextSlidePreview->setPixmap( nextPage->thumbnail( m_previewSize ) );
}

void KPrPresenterViewInterface::createSlideTime()
{
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(i18n( "Slide :")),0,0);
    
    layout->addWidget(slideBox,0,1);
    
    layout->addWidget(new QLabel(i18n( "Time :")));
    
    timeEdit->setDisplayFormat ( "HH:mm:ss" );
    int index = slideBox->currentIndex ();
    QTime t = QTime(0,0,0);
    t=t.addSecs(m_slides_time->value(index));
    timeEdit->setTime( t );
    layout->addWidget(timeEdit);
        
    m_timeSlideWidget = new QWidget(this);
    m_timeSlideWidget->setLayout(layout);
}

void KPrPresenterViewInterface::setSlidesTime(QMap<int,int> *slides_time)
{
    m_slides_time = slides_time;
    QString chaine;
    int index;
    for (int j = 0;j < m_slides_time->size(); ++j)
    {
	chaine = QString("Slide "+QString::number(j+1));
	index = slideBox->findText(chaine);
	if(index == -1)
	    slideBox->addItem(chaine);
    }
    
    int i = slideBox->currentIndex ();    
    QTime t = QTime(0,0,0);
    t=t.addSecs(m_slides_time->value(i));
    timeEdit->setTime( t );
    kWarning() << "index : " +QString::number(i)+" time : "+t.toString();
}

void KPrPresenterViewInterface::currentIndexChanged(int index)
{
    QTime t = QTime(0,0,0);
    t=t.addSecs(m_slides_time->value(index));
    timeEdit->setTime( t );
}

#include "KPrPresenterViewInterface.moc"

