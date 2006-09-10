/* This file is part of the KDE project
 * Copyright (C) 2001 David Faure <faure@kde.org>
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
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
 * Boston, MA 02110-1301, USA
 */

// kword includes
#include "KWView.h"
#include "KWGui.h"
#include "KWDocument.h"
#include "KWCanvas.h"
#include "KWPage.h"
#include "KWViewMode.h"
#include "dialog/KWFrameDialog.h"

// koffice libs includes
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoZoomAction.h>
#include <KoToolManager.h>
#include <KoMainWindow.h>
#include <KoToolBox.h>

// KDE + Qt includes
#include <QHBoxLayout>
#include <QTimer>
#include <QPrinter>
#include <klocale.h>
#include <kdebug.h>

KWView::KWView( const QString& viewMode, KWDocument* document, QWidget *parent )
    : KoView( document, parent )
{
    m_document = document;
    m_gui = new KWGui( viewMode, this );
    m_canvas = m_gui->canvas();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_gui);

    if ( !m_document->isReadWrite() )
        setXMLFile( "kword_readonly.rc" );
    else
        setXMLFile( "kword.rc" );

    m_currentPage = m_document->pageManager()->page(m_document->startPage());

    setupActions();
}

KWView::~KWView() {
}

KWCanvas *KWView::kwcanvas() const {
    return m_canvas;
}

QWidget *KWView::canvas() const {
    return m_canvas;
}

void KWView::updateReadWrite(bool readWrite) {
    // TODO
}

void KWView::setupActions() {
    KoZoomMode::Modes modes = KoZoomMode::ZOOM_WIDTH;

    if ( kwcanvas() && kwcanvas()->viewMode()->hasPages() )
        modes |= KoZoomMode::ZOOM_PAGE;

    m_actionViewZoom = new KoZoomAction( modes, "viewmag", i18n( "Zoom" ),
                                         0, actionCollection(), "view_zoom" );
    m_zoomHandler.setZoomAndResolution( 100, KoGlobal::dpiX(), KoGlobal::dpiY() );
    m_zoomHandler.setZoomMode( m_document->zoomMode() );
    m_zoomHandler.setZoom( m_document->zoom() );
    updateZoomControls();
    QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
    connect( m_actionViewZoom, SIGNAL( zoomChanged( KoZoomMode::Mode, int ) ),
            this, SLOT( viewZoom( KoZoomMode::Mode, int ) ) );

    m_actionFormatFrameSet = new KAction( i18n( "Frame/Frameset Properties" ),
            actionCollection(), "format_frameset");
    m_actionFormatFrameSet->setToolTip( i18n( "Alter frameset properties" ) );
    connect(m_actionFormatFrameSet, SIGNAL(triggered()), this, SLOT(editFrameProperties()));

    KAction *print = new KAction("MyPrint", actionCollection(), "file_my_print");
    connect(print, SIGNAL(triggered()), this, SLOT(print()));
}

void KWView::setZoom( int zoom ) {
    m_zoomHandler.setZoom( zoom );
    m_document->setZoom( zoom ); // for persistency reasons
    //getGUI()->getHorzRuler()->setZoom( m_zoomHandler.zoomedResolutionX() );
    //getGUI()->getVertRuler()->setZoom( m_zoomHandler.zoomedResolutionY() );

    //if ( statusBar() )
    //    m_sbZoomLabel->setText( ' ' + QString::number( zoom ) + "% " );

    // Also set the zoom in KoView (for embedded views)
    //kDebug() << "KWView::setZoom " << zoom << " setting koview zoom to " << m_zoomHandler.zoomedResolutionY() << endl;
    kwcanvas()->updateSize();
}

void KWView::viewZoom( KoZoomMode::Mode mode, int zoom )
{
    //kDebug() << " viewZoom '" << KoZoomMode::toString( mode ) << ", " << zoom << "'" << endl;

    if ( !m_currentPage )
        return;

    int newZoom = zoom;

    if ( mode == KoZoomMode::ZOOM_WIDTH ) {
        m_zoomHandler.setZoomMode(KoZoomMode::ZOOM_WIDTH);
        newZoom = qRound( static_cast<double>(m_gui->visibleWidth() * 100 ) / (m_zoomHandler.resolutionX() * m_currentPage->width() ) ) - 1;

        if(newZoom != m_zoomHandler.zoomInPercent() && !m_gui->verticalScrollBarVisible()) {
            // we have to do this twice to take into account a possibly appearing vertical scrollbar
            QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
        }
    }
    else if ( mode == KoZoomMode::ZOOM_PAGE ) {
        m_zoomHandler.setZoomMode(KoZoomMode::ZOOM_PAGE);
        double height = m_zoomHandler.resolutionY() * m_currentPage->height();
        double width = m_zoomHandler.resolutionX() * m_currentPage->width();
        newZoom = qMin( qRound( static_cast<double>(m_gui->visibleHeight() * 100 ) / height ),
                     qRound( static_cast<double>(m_gui->visibleWidth() * 100 ) / width ) ) - 1;
    }
    else {
        m_zoomHandler.setZoomMode(KoZoomMode::ZOOM_CONSTANT);
    }

    if( newZoom < 10 || newZoom == m_zoomHandler.zoomInPercent()) //zoom should be valid and >10
        return;

    setZoom( newZoom );
    updateZoomControls();
    canvas()->setFocus();
}

void KWView::changeZoomMenu() {
    KoZoomMode::Modes modes = KoZoomMode::ZOOM_WIDTH;

    if ( kwcanvas() && kwcanvas()->viewMode()->hasPages() )
        modes |= KoZoomMode::ZOOM_PAGE;

    m_actionViewZoom->setZoomModes( modes );
}

void KWView::updateZoomControls()
{
    switch(m_zoomHandler.zoomMode())
    {
        case KoZoomMode::ZOOM_WIDTH:
        case KoZoomMode::ZOOM_PAGE:
            m_actionViewZoom->setZoom( KoZoomMode::toString( m_zoomHandler.zoomMode() ) );
            break;
        case KoZoomMode::ZOOM_CONSTANT:
            m_actionViewZoom->setZoom( m_zoomHandler.zoomInPercent() );
            break;
    }
}

void KWView::updateZoom( ) {
    viewZoom( m_zoomHandler.zoomMode(), m_zoomHandler.zoomInPercent() );
}

void KWView::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );

    if ( m_zoomHandler.zoomMode() != KoZoomMode::ZOOM_CONSTANT )
        updateZoom();
}

void KWView::editFrameProperties() {
    QList<KWFrame*> frames;
    foreach(KoShape *shape, kwcanvas()->shapeManager()->selection()->selectedShapes()) {
        KWFrame *frame = m_document->frameForShape(shape);
        if(frame)
            frames.append(frame);
    }
    KWFrameDialog *frameDialog = new KWFrameDialog(frames, m_document, this);
    frameDialog->exec();
    delete frameDialog;
}

QDockWidget *KWView::createToolBox() {
    return KoToolManager::instance()->toolBox("KWord");
}

// Actions
void KWView::print() {
// options;
//   DPI
//   clip to page
//   pages
//   fontEmbeddingEnabled();
//   duplex

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setCreator("KWord 2.0alpha");
    printer.setDocName ("Demo canvas");
    printer.setOutputFileName("output.pdf");
    int resolution = 600;
    printer.setResolution(resolution);
    printer.setFullPage(true); // ignore printer margins

    QPainter painter(&printer);
    KoZoomHandler zoomer;
    zoomer.setZoomAndResolution(100, resolution, resolution);
    const int lastPage = m_document->lastPage();
    for(int pageNum=m_document->startPage(); pageNum <= lastPage; pageNum++) {
        KWPage *page = m_document->pageManager()->page(pageNum);
        // Note that Qt does not at this time allow us to alter the page size to an arbitairy size
        const int pageOffset = qRound(POINT_TO_INCH( resolution * page->offsetInDocument()));
        painter.save();

        painter.translate(0, -pageOffset);
        double width = page->width();
        int clipHeight = (int) POINT_TO_INCH( resolution * page->height() );
        int clipWidth = (int) POINT_TO_INCH( resolution * page->width() );
        int offset = 0;
        if(page->pageSide() == KWPage::PageSpread) { // left part
            width /= 2;
            clipWidth /= 2;
            offset = clipWidth;
            painter.setClipRect(0, pageOffset, clipWidth, clipHeight);
            m_canvas->shapeManager()->paint( painter, zoomer, true );
            printer.newPage();
            painter.translate(-clipWidth, 0);
            pageNum++;
        }
        painter.setClipRect(offset, pageOffset, clipWidth, clipHeight);
        m_canvas->shapeManager()->paint( painter, zoomer, true );

        painter.restore();

        if(pageNum != lastPage)
            printer.newPage();
    }

    painter.end();
}

#include "KWView.moc"
