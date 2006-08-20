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
#include "KWFactory.h"
#include "KWView.h"
#include "KWCanvas.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWDLoader.h"
#include "frame/KWFrameSet.h"
#include "frame/KWTextFrameSet.h"
#include "frame/KWFrame.h"
#include "frame/KWFrameLayout.h"
#include "dialog/KWFrameDialog.h"

// koffice libs includes
#include <KoShapeManager.h>
#include <KoOasisStyles.h>
#include <KoToolManager.h>
#include <KoTextShape.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactory.h>
#include <KoStyleManager.h>

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
    m_styleManager = new KoStyleManager(this);
    m_zoomMode = KoZoomMode::ZOOM_WIDTH;

    setInstance( KWFactory::instance(), false );
    setTemplateType( "kword_template" );

    connect (&m_frameLayout, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFrameSet(KWFrameSet*)));

    // Init shape Factories with our frame based configuration panels.
    QList<KoShapeConfigFactory *> panels = KWFrameDialog::panels(this);
    foreach(KoID id, KoShapeRegistry::instance()->listKeys())
        KoShapeRegistry::instance()->get(id)->setOptionPanels(panels);

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

    clear();
}

KWDocument::~KWDocument() {
}

void KWDocument::addShape (KoShape *shape) {
    Q_UNUSED(shape);
    // we don't actually add a shape; we assume the FrameDia and friends call a addFrame instead
}

void KWDocument::removeShape (KoShape *shape) {
    shape->repaint();
    foreach(KoView *view, views()) {
        KWCanvas *canvas = static_cast<KWView*>(view)->kwcanvas();
        canvas->shapeManager()->remove(shape);
    }
    m_frameMap.remove(shape);
}

void KWDocument::paintContent(QPainter&, const QRect&, bool, double, double) {
    // TODO
}

bool KWDocument::saveOasis(KoStore*, KoXmlWriter*) {
    // TODO
}

KoView* KWDocument::createViewInstance(QWidget* parent) {
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

    KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
    if(tfs)
        m_styleManager->add( tfs->document() );

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

// *** LOADING
void KWDocument::clear() {
    // document defaults
    m_pageSettings.clear();
    KoColumns columns = m_pageSettings.columns();
    columns.ptColumnSpacing = m_defaultColumnSpacing; // TODO load this value on demand
    m_pageSettings.setColumns(columns);
    m_tabStop = MM_TO_POINT(15);
    m_hasTOC = false;
    foreach(KWFrameSet *fs, m_frameSets) {
        removeFrameSet(fs);
        delete fs;
    }
}

bool KWDocument::loadOasis(const QDomDocument&, KoOasisStyles&, const QDomDocument&, KoStore*) {
    // TODO
}

bool KWDocument::loadXML( QIODevice *, const QDomDocument & doc ) {
    QDomElement root = doc.documentElement();
    clear();
    KWDLoader loader(this);
    bool rc = loader.load(root);
    if (rc)
        endOfLoading();
    return rc;
}

void KWDocument::endOfLoading() // called by both oasis and oldxml
{
    // insert pages
    double maxBottom = 0;
    foreach(KWFrameSet* fs, m_frameSets) {
        foreach(KWFrame *frame, fs->frames())
            maxBottom = qMax(maxBottom, frame->shape()->boundingRect().bottom());
    }
    KWPage *last = pageManager()->page(lastPage());
    double docHeight = last?(last->offsetInDocument() + last->height()):0.0;
    while(docHeight <= maxBottom) {
        kDebug(32001) << "KWDocument::endOfLoading appends a page\n";
        last = appendPage();
        docHeight += last->height();
    }

#if 0
    // do some sanity checking on document.
    for (int i = frameSetCount()-1; i>-1; i--) {
        KWFrameSet *fs = frameSet(i);
        if(!fs) {
            kWarning() << "frameset " << i << " is NULL!!" << endl;
            m_lstFrameSet.remove(i);
            continue;
        }
        if( fs->type()==FT_TABLE) {
            static_cast<KWTableFrameSet *>( fs )->validate();
        } else if (fs->type() == FT_TEXT) {
            for (int f=fs->frameCount()-1; f>=0; f--) {
                KWFrame *frame = fs->frame(f);
                if(frame->left() < 0) {
                    kWarning() << fs->name() << " frame " << f << " pos.x is < 0, moving frame" << endl;
                    frame->moveBy( 0- frame->left(), 0);
                }
                if(frame->right() > m_pageLayout.ptWidth) {
                    kWarning() << fs->name() << " frame " << f << " rightborder outside page ("
                        << frame->right() << ">" << m_pageLayout.ptWidth << "), shrinking" << endl;
                    frame->setRight(m_pageLayout.ptWidth);
                }
                if(fs->isProtectSize())
                    continue; // don't make frames bigger of a protected frameset.
                if(frame->height() < s_minFrameHeight) {
                    kWarning() << fs->name() << " frame " << f << " height is so small no text will fit, adjusting (was: "
                                << frame->height() << " is: " << s_minFrameHeight << ")" << endl;
                    frame->setHeight(s_minFrameHeight);
                }
                if(frame->width() < s_minFrameWidth) {
                    kWarning() << fs->name() << " frame " << f << " width is so small no text will fit, adjusting (was: "
                                << frame->width() << " is: " << s_minFrameWidth  << ")" << endl;
                    frame->setWidth(s_minFrameWidth);
                }
            }
            if(fs->frameCount() == 0) {
                KWPage *page = pageManager()->page(startPage());
                KWFrame *frame = new KWFrame(fs, page->leftMargin(), page->topMargin(),
                        page->width() - page->leftMargin() - page->rightMargin(),
                        page->height() - page->topMargin() - page->bottomMargin());
                //kDebug(32001) << "KWDocument::loadXML main-KWFrame created " << *frame << endl;
                fs->addFrame( frame );
            }
        } else if(fs->frameCount() == 0) {
            kWarning () << "frameset " << i << " " << fs->name() << " has no frames" << endl;
            removeFrameSet(fs);
            if ( fs->type() == FT_PART )
                delete static_cast<KWPartFrameSet *>(fs)->getChild();
            delete fs;
            continue;
        }
        if(fs->frameCount() > 0) {
            KWFrame *frame = fs->frame(0);
            if(frame->isCopy()) {
                kWarning() << "First frame in a frameset[" << fs->name() << "] was set to be a copy; resetting\n";
                frame->setCopy(false);
            }
        }
    }

    // Renumber footnotes
    KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet *>( m_lstFrameSet.getFirst() );
    if ( frameset  )
        frameset->renumberFootNotes( false /*no repaint*/ );

#endif
    // remove header/footer frames that are not visible.
    m_frameLayout.cleanupHeadersFooters();

    foreach(KWPage *page, m_pageManager.pages())
        m_frameLayout.createNewFramesForPage(page->pageNumber());

    emit sigProgress(-1);

    kDebug(32001) << "KWDocument::loadXML done" << endl;

#if 0
    // Connect to notifications from main text-frameset
    if ( frameset ) {
        connect( frameset->textObject(), SIGNAL( chapterParagraphFormatted( KoTextParag * ) ),
                 SLOT( slotChapterParagraphFormatted( KoTextParag * ) ) );
        connect( frameset, SIGNAL( mainTextHeightChanged() ),
                 SIGNAL( mainTextHeightChanged() ) );
    }
#endif

    // Note that more stuff will happen in completeLoading

    setModified(false);
}


#ifndef NDEBUG
void KWDocument::printDebug() {
    class Helper {
        public:
        static QString HFToString(KWord::HeaderFooterType type) {
            switch(type) {
                case KWord::HFTypeEvenOdd: return "evenOdd";
                case KWord::HFTypeUniform: return "Uniform";
                case KWord::HFTypeSameAsFirst: return "SameAsFirst";
                default:
                    return "None";
            }
        }
    };

    kDebug() << "----------------------------------------"<<endl;
    kDebug() << "                 Debug info"<<endl;
    kDebug() << "Document:" << this <<endl;
    kDebug() << "Type of document: " << (m_pageSettings.hasMainTextFrame()?"WP":"DTP") << endl;
    kDebug() << "First Header: " << Helper::HFToString(m_pageSettings.firstHeader()) << endl;
    kDebug() << "First Footer: " << Helper::HFToString(m_pageSettings.firstFooter()) << endl;
    kDebug() << "Other Headers: " << Helper::HFToString(m_pageSettings.headers()) << endl;
    kDebug() << "Other Footers: " << Helper::HFToString(m_pageSettings.footers()) << endl;
    kDebug() << "Units: " << KoUnit::unitName( unit() ) <<endl;
    kDebug() << "# Framesets: " << frameSetCount() <<endl;
    int i=0;
    foreach(KWFrameSet *fs, m_frameSets) {
        kDebug() << "Frameset " << i++ << ": '" <<
            fs->name() << "' (" << fs << ")" << /*(fs->isDeleted()?" Deleted":"")<<*/endl;
//       if ( fs->isVisible())
           fs->printDebug();
//      else
//           kDebug() << "  [hidden] #" << fs->frameCount() << " frames" << endl;
    }
/* TODO
    for ( uint pgNum = 0 ; pgNum < m_sectionTitles.size() ; ++pgNum ) {
        kDebug() << "Page " << pgNum << "  Section: '" << m_sectionTitles[ pgNum ] << "'"<< endl;
    }
*/
    /*
    kDebug() << "# Images: " << getImageCollection()->iterator().count() <<endl;
    QDictIterator<KWImage> it( getImageCollection()->iterator() );
    while ( it.current() ) {
        kDebug() << " + " << it.current()->getFilename() << ": "<<it.current()->refCount() <<endl;
        ++it;
    }
    */

    kDebug() << "PageManager holds "<< pageCount() << " pages in the range: " << startPage() <<
        "-" << lastPage() << endl;
    for (int pgnum = startPage() ; pgnum <= lastPage() ; pgnum++) {
        KWPage *page = pageManager()->page(pgnum);
        pgnum = page->pageNumber(); // in case the last one was a pagespread.
        QString side = "[Left] ";
        if(page->pageSide() == KWPage::Right)
            side = "[Right]";
        else if(page->pageSide() == KWPage::PageSpread)
            side = "[PageSpread]";
        kDebug() << "Page " << pgnum << side << " width=" << page->width() << " height=" << page->height() << endl;
    }
    kDebug() << "  The height of the doc (in pt) is: " << pageManager()->
        bottomOfPage(lastPage()) << endl;
}
#endif


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
