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
#include "dialog/KWFrameDialog.h"

// koffice libs includes
#include <KoShapeManager.h>
#include <KoOasisStyles.h>
#include <KoToolManager.h>
#include <KoTextShape.h>
#include <KoShapeRegistry.h>

// KDE + Qt includes
#include <klocale.h>
#include <kmessagebox.h>
#include <QIODevice>
#include <QTimer>
#include <QDomDocument>
#include <QCoreApplication>


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

    connect (&m_frameLayout, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFrameSet(KWFrameSet*)));

    // Init shape Factories with our frame based configuration panels.
    QList<KoShapeConfigFactory *> panels = KWFrameDialog::panels(this);
    foreach(KoID id, KoShapeRegistry::instance()->listKeys())
        KoShapeRegistry::instance()->get(id)->setOptionPanels(panels);

    KoToolManager::instance()->toolBox()->show();

    // print error if kotext not available
    if( KoShapeRegistry::instance()->get(KoTextShape_SHAPEID) == 0 ) {
         KMessageBox::error(parentWidget,
                 i18n("Can not find needed text component, KWord will quit now"),
                 i18n("Installation Error"));
        QCoreApplication::exit(10);
        QTimer::singleShot(0, QCoreApplication::instance(), SLOT(quit));
        // TODO actually quit if I can figure out how to do that from here :(
        return;
    }

appendPage();
appendPage()->setPageSide(KWPage::PageSpread);
appendPage();
appendPage();
}

KWDocument::~KWDocument() {
}

void KWDocument::addShape (KoShape *shape) {
#if 0
    if(shape->shapeId() == KoTextShape_SHAPEID) {
        KWTextFrameSet *fs = new KWTextFrameSet();
        KWTextFrame *frame = new KWTextFrame(shape, fs);
        addFrameSet(fs);
    } else {
        KWFrameSet *fs = new KWFrameSet();
        KWFrame *frame = new KWFrame(shape, fs);
        addFrameSet(fs);
    }
#endif
}

void KWDocument::removeShape (KoShape *shape) {
#if 0
    foreach(KoView *view, views()) {
        KWCanvas *canvas = static_cast<KWView*>(view)->kwcanvas();
        canvas->shapeManager()->remove(shape);
        canvas->update();
    }
#endif
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
    connect(fs, SIGNAL(frameRemoved(KWFrame*)), this, SLOT(removeFrame(KWFrame*)));
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
    m_frameMap.insert(frame->shape(), frame);
    foreach(KoView *view, views()) {
        KWCanvas *canvas = static_cast<KWView*>(view)->kwcanvas();
        canvas->shapeManager()->add(frame->shape());
    }
    frame->shape()->repaint();
}

void KWDocument::removeFrame(KWFrame *frame) {
    m_frameMap.remove(frame->shape());
}

KWFrame *KWDocument::frameForShape(KoShape *shape) const {
    return m_frameMap.value(shape);
}

KWFrameSet *KWDocument::frameSetByName( const QString & name )
{
    foreach(KWFrameSet *fs, m_frameSets) {
        if(fs->name() == name)
            return fs;
    }
    return 0;
}


QString KWDocument::uniqueFrameSetName( const QString& suggestion ) {
    // make up a new name for the frameset, use "[base] [digits]" as template.
    // Fully translatable naturally :)
    return renameFrameSet("", suggestion);
}

QString KWDocument::suggestFrameSetNameForCopy( const QString& base ) {
    // make up a new name for the frameset, use Copy[digits]-[base] as template.
    // Fully translatable naturally :)
    return renameFrameSet(i18n("Copy"), base);
}

QString KWDocument::renameFrameSet( const QString &prefix, const QString& base ) {
    if(! frameSetByName(base))
        return base;
    QString before, after;
    QRegExp findDigits("\\d+");
    int pos = findDigits.indexIn(base);
    if(pos >= 0) {
        before=base.left(pos);
        after = base.mid(pos + findDigits.matchedLength());
    }
    else if(prefix.isEmpty())
        before = base +" ";
    else {
        before = prefix;
        after = " "+ base;
    }

    if(! before.startsWith(prefix)) {
        before = prefix + before;
    }

    int count=0;
    while(true) {
        QString name = (before + (count==0?"":QString::number(count)) + after).trimmed();
        if(! frameSetByName(name))
            return name;
        count++;
    }
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
