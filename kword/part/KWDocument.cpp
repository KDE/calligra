/* This file is part of the KDE project
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
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
 * Boston, MA 02110-1301, USA.
 */

// kword includes
#include "KWDocument.h"
#include "KWView.h"
#include "KWCanvas.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "frame/KWTextFrameSet.h"
#include "frame/KWTextFrame.h"
#include "frame/KWFrameLayout.h"

// koffice libs includes
#include <KoShapeManager.h>
#include <KoOasisStyles.h>
#include <KoToolManager.h>
#include <KoTextShape.h>
#include <KoTextShapeFactory.h>
#include <KoShapeRegistry.h>

// KDE + Qt includes
#include <QIODevice>
#include <QTimer>
#include <QDomDocument>


KWDocument::KWDocument( QWidget *parentWidget, QObject* parent, bool singleViewMode )
    : KoDocument(parentWidget, parent, singleViewMode),
    m_snapToGrid(false),
    m_gridX(MM_TO_POINT(5.0)),
    m_gridY(MM_TO_POINT(5.0)),
    m_zoom(100),
    m_frameLayout(pageManager(), m_frameSets, pageSettings())
{
    m_pageManager.setDefaultPage(*pageSettings()->pageLayout());
    m_zoomMode = KoZoomMode::ZOOM_WIDTH;
    KoToolManager::instance()->toolBox()->show();

    // replace plugin version with dynamic-loadable library version
    KoTextShapeFactory *factory = new KoTextShapeFactory(parent, QStringList());
    KoShapeRegistry::instance()->remove(factory->shapeId());
    KoShapeRegistry::instance()->add(factory);

    connect (&m_frameLayout, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFrameSet(KWFrameSet*)));

appendPage();
appendPage()->setPageSide(KWPage::PageSpread);
appendPage();
appendPage();
}

KWDocument::~KWDocument() {
}

void KWDocument::addShape (KoShape *shape) {
    KoTextShape *text = dynamic_cast<KoTextShape*>(shape);
    if(text) {
        KWTextFrameSet *fs = new KWTextFrameSet();
        KWTextFrame *frame = new KWTextFrame(text, fs);
        addFrameSet(fs);
    } else {
        KWFrameSet *fs = new KWFrameSet();
        KWFrame *frame = new KWFrame(shape, fs);
        addFrameSet(fs);
    }
}

void KWDocument::removeShape (KoShape *shape) {
    foreach(KoView *view, views()) {
        KWCanvas *canvas = static_cast<KWView*>(view)->kwcanvas();
        canvas->shapeManager()->remove(shape);
        canvas->update();
    }
}

void KWDocument::paintContent(QPainter&, const QRect&, bool, double, double) {
    // TODO
}

bool KWDocument::loadXML(QIODevice*, const QDomDocument&) {
    // TODO
}

bool KWDocument::loadOasis(const QDomDocument&, KoOasisStyles&, const QDomDocument&, KoStore*) {
    // TODO
}

bool KWDocument::saveOasis(KoStore*, KoXmlWriter*) {
    // TODO
}

KoView* KWDocument::createViewInstance(QWidget* parent, const char* name) {
    KWView *view = new KWView(m_viewMode, this, parent);
    foreach(KWFrameSet *fs, m_frameSets) {
        foreach(KWFrame *frame, fs->frames()) {
            view->kwcanvas()->shapeManager()->add(frame->shape());
        }
    }
    return view;
}

KWPage* KWDocument::insertPage( int afterPageNum ) {
    KWPage *page = m_pageManager.insertPage(afterPageNum+1);
    PageProcessingQueue *ppq = new PageProcessingQueue(this);
    ppq->addPage(page);
    return page;
}

KWPage* KWDocument::appendPage() {
    return insertPage( m_pageManager.lastPageNumber() );
}


void KWDocument::removeFrameSet( KWFrameSet *fs ) {
    m_frameSets.removeAt( m_frameSets.indexOf(fs) );
    setModified( true );
    foreach(KWFrame *frame, fs->frames()) {
        frame->shape()->repaint();
        foreach(KoView *view, views()) {
            KWCanvas *canvas = static_cast<KWView*>(view)->kwcanvas();
            canvas->shapeManager()->remove(frame->shape());
        }
    }
    emit frameSetRemoved(fs);
}

void KWDocument::addFrameSet(KWFrameSet *fs) {
    if(m_frameSets.contains(fs)) return;
    setModified( true );
    m_frameSets.append(fs);
    foreach(KWFrame *frame, fs->frames())
        addFrame(frame);
    connect(fs, SIGNAL(frameAdded(KWFrame*)), this, SLOT(addFrame(KWFrame*)));
    emit frameSetAdded(fs);
}

int KWDocument::pageCount() const {
    return pageManager()->pageCount();
}

int KWDocument::startPage() const {
    return pageManager()->startPage();
}

int KWDocument::lastPage() const {
    return pageManager()->lastPageNumber();
}

void KWDocument::addFrame(KWFrame *frame) {
    foreach(KoView *view, views()) {
        KWCanvas *canvas = static_cast<KWView*>(view)->kwcanvas();
        canvas->shapeManager()->add(frame->shape());
    }
    frame->shape()->repaint();
}


// ************* PageProcessingQueue ************
PageProcessingQueue::PageProcessingQueue(KWDocument *parent) {
    m_document = parent;
    m_triggered = false;
}

void PageProcessingQueue::addPage(KWPage *page) {
    m_pages.append(page);
    if(! m_triggered)
        QTimer::singleShot(0, this, SLOT(process()));
    m_triggered = true;
}

void PageProcessingQueue::process() {
    foreach(KWPage *page, m_pages) {
        m_document->m_frameLayout.createNewFramesForPage(page->pageNumber());
        emit m_document->pageAdded(page);
    }
    m_pages.clear();
    deleteLater();
}


#include "KWDocument.moc"
