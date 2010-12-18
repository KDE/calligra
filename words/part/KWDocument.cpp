/* This file is part of the KDE project
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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
#include "KWDocument_p.h"
#include "KWFactory.h"
#include "KWView.h"
#include "KWCanvas.h"
#include "KWCanvasItem.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWPageStyle.h"
#include "KWOdfLoader.h"
#include "KWDLoader.h"
#include "KWOdfWriter.h"
#include "frames/KWFrameSet.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWFrame.h"
#include "frames/KWFrameLayout.h"
#include "frames/KWOutlineShape.h"
#include "dialogs/KWFrameDialog.h"
#include "dialogs/KWStartupWidget.h"
#include "commands/KWPageInsertCommand.h"
#include "commands/KWPageRemoveCommand.h"

// koffice libs includes
#include <KoShapeManager.h>
#include <KoTextDocument.h>
#include <KoTextAnchor.h>
#include <KoShapeContainer.h>
#include <KoOdfWriteStore.h>
#include <KoToolManager.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>
#include <KoStyleManager.h>
#include <KoResourceManager.h>
#include <KoInteractionTool.h>
#include <KoInlineTextObjectManager.h>
#include <KoDocumentInfo.h>
#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoListStyle.h>
#include <KoListLevelProperties.h>
#include <KoTextShapeData.h>

#include <rdf/KoDocumentRdfBase.h>
#ifdef SHOULD_BUILD_RDF
#include <rdf/KoDocumentRdf.h>
#endif

#include <KoProgressUpdater.h>
#include <KoUpdater.h>

// KDE + Qt includes
#include <klocale.h>
#include <kstandardaction.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kdebug.h>
#include <QIODevice>
#include <QTimer>
#include <QThread>
#include <QCoreApplication>
#include <QTextBlock>

/// \internal
// this class will be added to all views and be hidden by default.
// during loading any frames we find will be added here and only when they are positioned properly will
// those frames be re-shown.
class MagicCurtain : public KoShapeContainer
{
public:
    // reimplemented pure virtual calls
    bool loadOdf(const KoXmlElement&, KoShapeLoadingContext&) { return false; }
    void saveOdf(KoShapeSavingContext&) const { }
    void paintComponent(QPainter&, const KoViewConverter&) { }

    /// add the frame to be hidden
    void addFrame(KWFrame *frame);
    /// add the shape to be hidden
    void addShape(KoShape *shape);
    // reveal all the frames that were added before
    void revealFramesForPage(int pageNumber, qreal moveFrames);

private:
    QHash<int, QList<KWFrame*> > m_data;
};

void MagicCurtain::addFrame(KWFrame *frame)
{
    Q_ASSERT(frame->loadingPageNumber() > 0);
    QList<KWFrame*> frames = m_data.value(frame->loadingPageNumber());
    frames << frame;
    m_data.insert(frame->loadingPageNumber(), frames);
    frame->shape()->setParent(this);
}

void MagicCurtain::addShape(KoShape *shape)
{
    shape->setParent(this);
}

void MagicCurtain::revealFramesForPage(int pageNumber, qreal moveFrames)
{
    QPointF offset(0, moveFrames);
    foreach (KWFrame *frame, m_data.value(pageNumber)) {
        frame->shape()->setPosition(frame->shape()->position() + offset);
        frame->shape()->setParent(0);
        frame->clearLoadingData();
    }
    m_data.remove(pageNumber);
}


// KWDocument
KWDocument::KWDocument(QWidget *parentWidget, QObject *parent, bool singleViewMode)
        : KoDocument(parentWidget, parent, singleViewMode),
        m_frameLayout(&m_pageManager, m_frameSets),
        m_magicCurtain(0),
        m_mainFramesetEverFinished(false)
{
    m_frameLayout.setDocument(this);
    resourceManager()->setOdfDocument(this);

    setComponentData(KWFactory::componentData(), false);
    setTemplateType("kword_template");

    connect(&m_frameLayout, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFrameSet(KWFrameSet*)));
    connect(&m_frameLayout, SIGNAL(removedFrameSet(KWFrameSet*)), this, SLOT(removeFrameSet(KWFrameSet*)));

    // Init shape Factories with our frame based configuration panels.
    m_panelFactories = KWFrameDialog::panels(this);
    foreach (const QString &id, KoShapeRegistry::instance()->keys()) {
        KoShapeFactoryBase *shapeFactory = KoShapeRegistry::instance()->value(id);
        shapeFactory->setOptionPanels(m_panelFactories);
    }

    resourceManager()->setUndoStack(undoStack());
    if (documentRdfBase()) {
        documentRdfBase()->linkToResourceManager(resourceManager());
    }

    QVariant variant;
    variant.setValue(new KoChangeTracker(resourceManager()));
    resourceManager()->setResource(KoText::ChangeTracker, variant);

    connect(documentInfo(), SIGNAL(infoUpdated(const QString &, const QString &)),
            inlineTextObjectManager(), SLOT(documentInformationUpdated(const QString &, const QString &)));

    m_config.load(this);
    clear();
}

KWDocument::~KWDocument()
{
    qDeleteAll(m_panelFactories);
    delete m_magicCurtain;
    m_config.setUnit(unit());
    saveConfig();
    qDeleteAll(m_frameSets);
}

void KWDocument::addShape(KoShape *shape)
{
    // KWord adds a couple of dialogs (like KWFrameDialog) which will not call addShape(), but
    // will call addFrameSet.  Which will itself call addFrame()
    // any call coming in here is due to the undo/redo framework or for nested frames

    KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
    if (frame == 0) {
        KWFrameSet *fs = new KWFrameSet();
        fs->setName(shape->shapeId());
        frame = new KWFrame(shape, fs);
    }
    Q_ASSERT(frame->frameSet());
    addFrameSet(frame->frameSet());

    foreach (KoView *view, views()) {
        KoCanvasBase *canvas = static_cast<KWView*>(view)->canvasBase();
        canvas->shapeManager()->addShape(shape);
    }
}

void KWDocument::removeShape(KoShape *shape)
{
    KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
    if (frame) { // not all shapes have to have a frame. Only top-level ones do.
        KWFrameSet *fs = frame->frameSet();
        Q_ASSERT(fs);
        if (fs->frameCount() == 1) // last frame on FrameSet
            removeFrameSet(fs); // frame and frameset will be deleted when the shape is deleted
        else
            fs->removeFrame(frame);
    } else { // not a frame, but we still have to remove it from views.
        foreach (KoView *view, views()) {
            KoCanvasBase *canvas = static_cast<KWView*>(view)->canvasBase();
            canvas->shapeManager()->remove(shape);
        }
    }
}

void KWDocument::paintContent(QPainter&, const QRect &rect)
{
    Q_UNUSED(rect);
#ifdef __GNUC__
    #warning TODO: implement KWDocument::paintContent
#endif

}

KoView *KWDocument::createViewInstance(QWidget *parent)
{
    KWView *view = new KWView(m_viewMode, this, parent);
    if (m_magicCurtain)
        view->canvasBase()->shapeManager()->addShape(m_magicCurtain, KoShapeManager::AddWithoutRepaint);

    bool switchToolCalled = false;
    foreach (KWFrameSet *fs, m_frameSets) {
        if (fs->frameCount() == 0)
            continue;
        foreach (KWFrame *frame, fs->frames())
            view->canvasBase()->shapeManager()->addShape(frame->shape(), KoShapeManager::AddWithoutRepaint);
        if (switchToolCalled)
            continue;
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs && tfs->textFrameSetType() == KWord::MainTextFrameSet) {
            KoSelection *selection = view->canvasBase()->shapeManager()->selection();
            selection->select(fs->frames().first()->shape());

            KoToolManager::instance()->switchToolRequested(
                KoToolManager::instance()->preferredToolForSelection(selection->selectedShapes()));
            switchToolCalled = true;
        }
    }
    if (!switchToolCalled)
        KoToolManager::instance()->switchToolRequested(KoInteractionTool_ID);

    return view;
}

QGraphicsItem *KWDocument::createCanvasItem()
{
    // caller owns the canvas item
    KWCanvasItem *item = new KWCanvasItem(m_viewMode, this);

    if (m_magicCurtain) {
        item->shapeManager()->addShape(m_magicCurtain, KoShapeManager::AddWithoutRepaint);
    }
    foreach (KWFrameSet *fs, m_frameSets) {
        if (fs->frameCount() == 0) {
            continue;
        }
        foreach (KWFrame *frame, fs->frames()) {
            item->shapeManager()->addShape(frame->shape(), KoShapeManager::AddWithoutRepaint);
        }
    }
    return item;
}

KWPage KWDocument::insertPage(int afterPageNum, const QString &masterPageName)
{
    KWPageInsertCommand *cmd = new KWPageInsertCommand(this, afterPageNum, masterPageName);
    addCommand(cmd);
    Q_ASSERT(cmd->page().isValid());
    return cmd->page();
}

KWPage KWDocument::appendPage(const QString &masterPageName)
{
    int number = 0;
    KWPage last = m_pageManager.last();
    if (last.isValid())
        number = last.pageNumber();
    return insertPage(number, masterPageName);
}

void KWDocument::removePage(int pageNumber)
{
    if (pageCount() <= 1)
        return;

    KWPage page = m_pageManager.page(pageNumber);
    if (! page.isValid()) {
        kWarning(32001) << "remove page requested for a non exiting page!" << pageNumber;
        return;
    }
    addCommand(new KWPageRemoveCommand(this, page));
}

void KWDocument::firePageSetupChanged()
{
    if (inlineTextObjectManager())
        inlineTextObjectManager()->setProperty(KoInlineObject::PageCount, pageCount());
    emit pageSetupChanged();
}

void KWDocument::removeFrameSet(KWFrameSet *fs)
{
    m_frameSets.removeAt(m_frameSets.indexOf(fs));
    setModified(true);
    foreach (KWFrame *frame, fs->frames())
        removeFrame(frame);
    foreach (KoView *view, views()) {
        KoCanvasBase *canvas = static_cast<KWView*>(view)->canvasBase();
        canvas->resourceManager()->setResource(KWord::CurrentFrameSetCount, m_frameSets.count());
    }
}

void KWDocument::relayout()
{
    foreach (KWFrameSet *fs, m_frameSets) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs == 0) continue;
        if (tfs->textFrameSetType() != KWord::MainTextFrameSet) continue;
        // we switch to the interaction tool to avoid crashes if the tool was editing a frame.
        KoToolManager::instance()->switchToolRequested(KoInteractionTool_ID);
        QSet<KWPage> coveredPages;
        foreach (KWFrame *frame, tfs->frames()) {
            KWPage page = pageManager()->page(frame->shape());
            if (page.isValid()) {
                if (! coveredPages.contains(page)) {
                    coveredPages += page;
                    continue; // keep one frame per page.
                }
            }

            foreach (KoView *view, views()) {
                KoCanvasBase *canvas = static_cast<KWView*>(view)->canvasBase();
                canvas->shapeManager()->remove(frame->shape());
            }
            tfs->removeFrame(frame);
            delete frame->shape();
        }
    }
    PageProcessingQueue *ppq = new PageProcessingQueue(this);
    foreach (const KWPage &page, pageManager()->pages())
        ppq->addPage(page);
}

void KWDocument::addFrameSet(KWFrameSet *fs)
{
    if (m_frameSets.contains(fs)) return;
    setModified(true);
    m_frameSets.append(fs);
    foreach (KWFrame *frame, fs->frames())
        addFrame(frame);

    KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
    if (tfs) {
        tfs->setPageManager(pageManager());
        if (tfs->textFrameSetType() == KWord::MainTextFrameSet ||
                tfs->textFrameSetType() == KWord::OtherTextFrameSet) {
            connect(tfs, SIGNAL(moreFramesNeeded(KWTextFrameSet*)),
                    this, SLOT(requestMoreSpace(KWTextFrameSet*)));
            connect(tfs, SIGNAL(layoutDone()), this, SLOT(mainTextFrameSetLayoutDone()));
        }
        else {
            connect(tfs, SIGNAL(decorationFrameResize(KWTextFrameSet*)),
                    this, SLOT(updateHeaderFooter(KWTextFrameSet*)));
        }
    }

    connect(fs, SIGNAL(frameAdded(KWFrame*)), this, SLOT(addFrame(KWFrame*)));
    connect(fs, SIGNAL(frameRemoved(KWFrame*)), this, SLOT(removeFrame(KWFrame*)));
}

void KWDocument::addFrame(KWFrame *frame)
{
    foreach (KoView *view, views()) {
        KoCanvasBase *canvas = static_cast<KWView*>(view)->canvasBase();
        if (frame->outlineShape())
            canvas->shapeManager()->addShape(frame->outlineShape()->parent());
        else
            canvas->shapeManager()->addShape(frame->shape());
        canvas->resourceManager()->setResource(KWord::CurrentFrameSetCount, m_frameSets.count());
    }
    if (frame->loadingPageNumber() > 0) {
        if (m_magicCurtain == 0) {
            m_magicCurtain = new MagicCurtain();
            m_magicCurtain->setVisible(false);
            foreach (KoView *view, views())
                static_cast<KWView*>(view)->canvasBase()->shapeManager()->addShape(m_magicCurtain);
        }
        m_magicCurtain->addFrame(frame);
    }
    else
        frame->shape()->update();
}

void KWDocument::removeFrame(KWFrame *frame)
{
    if (frame->shape() == 0) return;
    removeFrameFromViews(frame);
    KWPage page = pageManager()->page(frame->shape());
    if (!page.isValid()) return;
    if (!page.isAutoGenerated()) return;
    if (page != pageManager()->last() || page == pageManager()->begin())
        return; // can only delete last page.
    foreach (KWFrameSet *fs, m_frameSets) {
        foreach (KWFrame *f, fs->frames()) {
            if (page == pageManager()->page(f->shape()))
                return;
        }
    }

    KWPageRemoveCommand *cmd = new KWPageRemoveCommand(this, page);
    cmd->redo();
    delete cmd;
}

void KWDocument::mainTextFrameSetLayoutDone()
{
    m_mainFramesetEverFinished = true;
}

KWFrameSet *KWDocument::frameSetByName(const QString &name)
{
    foreach (KWFrameSet *fs, m_frameSets) {
        if (fs->name() == name)
            return fs;
    }
    return 0;
}

KWTextFrameSet *KWDocument::mainFrameSet() const
{
    return m_frameLayout.mainFrameSet();
}

KoInlineTextObjectManager *KWDocument::inlineTextObjectManager() const
{
    QVariant var = resourceManager()->resource(KoText::InlineTextObjectManager);
    return var.value<KoInlineTextObjectManager*>();
}

QString KWDocument::uniqueFrameSetName(const QString &suggestion)
{
    // make up a new name for the frameset, use "[base] [digits]" as template.
    // Fully translatable naturally :)
    return renameFrameSet("", suggestion);
}

QString KWDocument::suggestFrameSetNameForCopy(const QString &base)
{
    // make up a new name for the frameset, use Copy[digits]-[base] as template.
    // Fully translatable naturally :)
    return renameFrameSet(i18n("Copy"), base);
}

QString KWDocument::renameFrameSet(const QString &prefix, const QString &base)
{
    if (! frameSetByName(base))
        return base;
    QString before, after;
    QRegExp findDigits("\\d+");
    int pos = findDigits.indexIn(base);
    if (pos >= 0) {
        before = base.left(pos);
        after = base.mid(pos + findDigits.matchedLength());
    } else if (prefix.isEmpty())
        before = base + ' ';
    else {
        before = prefix;
        after = ' ' + base;
    }

    if (! before.startsWith(prefix)) {
        before = prefix + before;
    }

    int count = 0;
    while (true) {
        QString name = (before + (count == 0 ? "" : QString::number(count)) + after).trimmed();
        if (! frameSetByName(name))
            return name;
        count++;
    }
}

// *** LOADING

void KWDocument::initEmpty()
{
    clear();

    appendPage("Standard");

    Q_ASSERT(resourceManager()->hasResource(KoText::StyleManager));
    KoStyleManager *styleManager = resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
    Q_ASSERT(styleManager);
    KoParagraphStyle *parag = new KoParagraphStyle();
    parag->setName(i18n("Head 1"));
    KoCharacterStyle *character = parag->characterStyle();
    character->setFontPointSize(20);
    character->setFontWeight(QFont::Bold);
    styleManager->add(parag);

    parag = new KoParagraphStyle();
    parag->setName(i18n("Head 2"));
    character = parag->characterStyle();
    character->setFontPointSize(16);
    character->setFontWeight(QFont::Bold);
    styleManager->add(parag);

    parag = new KoParagraphStyle();
    parag->setName(i18n("Head 3"));
    character = parag->characterStyle();
    character->setFontPointSize(12);
    character->setFontWeight(QFont::Bold);
    styleManager->add(parag);

    parag = new KoParagraphStyle();
    parag->setName(i18n("Bullet List"));
    KoListStyle *list = new KoListStyle(parag);
    KoListLevelProperties llp = list->levelProperties(0);
    llp.setStyle(KoListStyle::DiscItem);
    list->setLevelProperties(llp);
    parag->setListStyle(list);
    styleManager->add(parag);

    KoDocument::initEmpty();
    clearUndoHistory();
}

void KWDocument::clear()
{
    // document defaults
    foreach (const KWPage &page, m_pageManager.pages())
        m_pageManager.removePage(page);
    m_pageManager.clearPageStyles();

    m_config.load(this); // re-load values
    foreach (KWFrameSet *fs, m_frameSets) {
        removeFrameSet(fs);
        delete fs;
    }

    // industry standard for bleed
    KoInsets padding;
    padding.top = MM_TO_POINT(3);
    padding.bottom = MM_TO_POINT(3);
    padding.left = MM_TO_POINT(3);
    padding.right = MM_TO_POINT(3);
    m_pageManager.setPadding(padding);

    if (inlineTextObjectManager())
        inlineTextObjectManager()->setProperty(KoInlineObject::PageCount, pageCount());
}

bool KWDocument::loadOdf(KoOdfReadStore &odfStore)
{
    clear();
    KWOdfLoader loader(this);
    bool rc = loader.load(odfStore);
    if (rc)
        endOfLoading();
    return rc;
}

bool KWDocument::loadXML(const KoXmlDocument &doc, KoStore *store)
{
    clear();
    KoXmlElement root = doc.documentElement();
    KWDLoader loader(this, store);
    bool rc = loader.load(root);
    if (rc)
        endOfLoading();
    return rc;
}

void KWDocument::endOfLoading() // called by both oasis and oldxml
{
    QPointer<KoUpdater> updater;
    if (progressUpdater()) {
        updater = progressUpdater()->startSubtask(1, "KWDocument::endOfLoading");
        updater->setProgress(0);
    }

    // Get the master page name of the first page.
    QString firstPageMasterName;
    if (mainFrameSet()) {
        QTextBlock block = mainFrameSet()->document()->firstBlock();
        firstPageMasterName = block.blockFormat().stringProperty(KoParagraphStyle::MasterPageName);
    }

    KWPage lastpage = pageManager()->last();
    qreal docHeight = lastpage.isValid() ? (lastpage.offsetInDocument() + lastpage.height()) : 0.0;
    PageProcessingQueue *ppq = new PageProcessingQueue(this);

    // insert pages
    qreal maxBottom = 0;
    foreach (KWFrameSet *fs, m_frameSets) {
        foreach (KWFrame *frame, fs->frames())
        maxBottom = qMax(maxBottom, frame->shape()->boundingRect().bottom());
    }
    // The Document we loaded could have specified
    //  1) a number of pages
    //  2) a number of frames
    // At the end of loading we then end up in one of 3 situations.
    // a) we have exactly the amount of pages that the document needs.
    // b) we have absolute frames positioned on pages that don't exist.
    // c) we have so much text in any of our text-framesets that new pages
    //    may have to be generated at some time after loading is completed.

    if (m_magicCurtain) { // pages defined in the loaded-document
        foreach (KWPage page, m_pageManager.pages())
            m_magicCurtain->revealFramesForPage(page.pageNumber(), page.offsetInDocument());
    }

    // Here we look at point 'b'. We add pages so at least all frames have a page.
    // btw. the observent reader might notice that cases b and c are not mutually exclusive ;)
    while (docHeight <= maxBottom) {
        kDebug(32001) << "KWDocument::endOfLoading appends a page";
        if (m_pageManager.pageCount() == 0) // apply the firstPageMasterName only on the first page
            lastpage = m_pageManager.appendPage(m_pageManager.pageStyle(firstPageMasterName));
        else // normally this shouldn't happen cause that loop is only run once...
            lastpage = m_pageManager.appendPage();
        ppq->addPage(lastpage);
        docHeight += lastpage.height();
        if (m_magicCurtain) {
            m_magicCurtain->revealFramesForPage(lastpage.pageNumber(), lastpage.offsetInDocument());
        }
    }

    if (updater) updater->setProgress(50);

#if 0
    // do some sanity checking on document.
    for (int i = frameSetCount() - 1; i > -1; i--) {
        KWFrameSet *fs = frameSet(i);
        if (!fs) {
            kWarning() << "frameset " << i << " is NULL!!";
            m_lstFrameSet.remove(i);
            continue;
        }
        if (fs->type() == FT_TABLE) {
            static_cast<KWTableFrameSet *>(fs)->validate();
        } else if (fs->type() == FT_TEXT) {
            for (int f = fs->frameCount() - 1; f >= 0; f--) {
                KWFrame *frame = fs->frame(f);
                if (frame->left() < 0) {
                    kWarning() << fs->name() << " frame " << f << " pos.x is < 0, moving frame";
                    frame->moveBy(0 - frame->left(), 0);
                }
                if (frame->right() > m_pageLayout.ptWidth) {
                    kWarning() << fs->name() << " frame " << f << " rightborder outside page ("
                    << frame->right() << ">" << m_pageLayout.ptWidth << "), shrinking";
                    frame->setRight(m_pageLayout.ptWidth);
                }
                if (fs->isProtectSize())
                    continue; // don't make frames bigger of a protected frameset.
                if (frame->height() < s_minFrameHeight) {
                    kWarning() << fs->name() << " frame " << f << " height is so small no text will fit, adjusting (was: "
                    << frame->height() << " is: " << s_minFrameHeight << ")";
                    frame->setHeight(s_minFrameHeight);
                }
                if (frame->width() < s_minFrameWidth) {
                    kWarning() << fs->name() << " frame " << f << " width is so small no text will fit, adjusting (was: "
                    << frame->width() << " is: " << s_minFrameWidth  << ")";
                    frame->setWidth(s_minFrameWidth);
                }
            }
            if (fs->frameCount() == 0) {
                KWPage *page = pageManager()->page(startPage());
                KWFrame *frame = new KWFrame(fs, page->leftMargin(), page->topMargin(),
                                             page->width() - page->leftMargin() - page->rightMargin(),
                                             page->height() - page->topMargin() - page->bottomMargin());
                //kDebug(32001) <<"KWDocument::loadXML main-KWFrame created" << *frame;
                fs->addFrame(frame);
            }
        } else if (fs->frameCount() == 0) {
            kWarning() << "frameset " << i << " " << fs->name() << " has no frames";
            removeFrameSet(fs);
            if (fs->type() == FT_PART)
                delete static_cast<KWPartFrameSet *>(fs)->getChild();
            delete fs;
            continue;
        }
        if (fs->frameCount() > 0) {
            KWFrame *frame = fs->frame(0);
            if (frame->isCopy()) {
                kWarning() << "First frame in a frameset[" << fs->name() << "] was set to be a copy; resetting";
                frame->setCopy(false);
            }
        }
    }

    // Renumber footnotes
    KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet *>(m_lstFrameSet.getFirst());
    if (frameset)
        frameset->renumberFootNotes(false /*no repaint*/);

#endif
    // remove header/footer frames that are not visible.
    m_frameLayout.cleanupHeadersFooters();

    foreach (const KWPage &page, m_pageManager.pages())
        m_frameLayout.createNewFramesForPage(page.pageNumber());

    foreach (KWFrameSet *fs, m_frameSets) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (!tfs)
            continue;
        KoTextDocument textDoc(tfs->document());
        foreach (KoInlineObject *inlineObject, textDoc.inlineTextObjectManager()->inlineTextObjects()) {
            KoTextAnchor *anchor = dynamic_cast<KoTextAnchor*>(inlineObject);
            if (anchor) {
                if (m_magicCurtain == 0) {
                    m_magicCurtain = new MagicCurtain();
                    m_magicCurtain->setVisible(false);
                    foreach (KoView *view, views())
                        static_cast<KWView*>(view)->canvasBase()->shapeManager()->addShape(m_magicCurtain);
                }
                m_magicCurtain->addShape(anchor->shape());
            }
        }
        tfs->setAllowLayout(true);
    }

    if (updater) updater->setProgress(100);

    kDebug(32001) << "KWDocument::endOfLoading done";

    // Note that more stuff will happen in completeLoading
    firePageSetupChanged();
    setModified(false);
}

bool KWDocument::saveOdf(SavingContext &documentContext)
{
    KWOdfWriter writer(this);
    return writer.save(documentContext.odfStore, documentContext.embeddedSaver);
}

QStringList KWDocument::extraNativeMimeTypes(ImportExportType importExportType) const
{
    QStringList answer = KoDocument::extraNativeMimeTypes(importExportType);
    if (importExportType == KoDocument::ForExport)
        answer.removeAll("application/x-kword"); // we can't save this, only load.
    return answer;
}

void KWDocument::requestMoreSpace(KWTextFrameSet *fs)
{
    // kDebug(32002) << fs;
    Q_ASSERT(fs);
    Q_ASSERT(fs->frameCount() > 0);
    Q_ASSERT(QThread::currentThread() == thread());

    KWFrame *lastFrame = fs->frames().last();

    QString masterPageName;
    if (fs == mainFrameSet()) {
        KoShape *shape = lastFrame->shape();
        if (shape) {
            KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
            if (data) {
                QTextBlock block = fs->document()->findBlock(data->endPosition() + 1);
                if (block.isValid()) {
                    masterPageName = block.blockFormat().stringProperty(KoParagraphStyle::MasterPageName);
                }
            }
        }
    }

    KWPage page = m_pageManager.page(lastFrame->shape());
    int pageDiff = m_pageManager.pageCount() - page.pageNumber();
    if (page.pageSide() == KWPage::PageSpread)
        pageDiff--;
    if (pageDiff >= (lastFrame->frameOnBothSheets() ? 1 : 2)) {
        // its enough to just create a new frame.
        m_frameLayout.createNewFrameForPage(fs, page.pageNumber()
                + (lastFrame->frameOnBothSheets() ? 1 : 2));
    } else {
        int afterPageNum = 0;
        KWPage last = m_pageManager.last();
        if (last.isValid())
            afterPageNum = last.pageNumber();
        KWPageInsertCommand cmd(this, afterPageNum, masterPageName);
        cmd.redo();
        KWPage newPage = cmd.page();
        Q_ASSERT(newPage.isValid());
        newPage.setAutoGenerated(true);
        if (m_magicCurtain)
            m_magicCurtain->revealFramesForPage(newPage.pageNumber(), newPage.offsetInDocument());
    }
}

void KWDocument::updateHeaderFooter(KWTextFrameSet *tfs)
{
    // find all pages that have the page style set and re-layout them.
    Q_ASSERT(tfs->pageStyle().isValid());
    PageProcessingQueue *ppq = 0;
    const KWPageStyle style = tfs->pageStyle();
    foreach (KWPage page, pageManager()->pages()) {
        if (page.pageStyle() == style) {
            if (ppq == 0)
                ppq = new PageProcessingQueue(this);
            ppq->addPage(page);
        }
    }
}

void KWDocument::showStartUpWidget(KoMainWindow *parent, bool alwaysShow)
{
    // print error if kotext not available
    if (KoShapeRegistry::instance()->value(TextShape_SHAPEID) == 0)
        // need to wait 1 event since exiting here would not work.
        QTimer::singleShot(0, this, SLOT(showErrorAndDie()));
    else
        KoDocument::showStartUpWidget(parent, alwaysShow);
}

void KWDocument::showErrorAndDie()
{
    KMessageBox::error(widget(),
                       i18n("Can not find needed text component, KWord will quit now"),
                       i18n("Installation Error"));
    QCoreApplication::exit(10);
}

void KWDocument::removeFrameFromViews(KWFrame *frame)
{
    Q_ASSERT(frame);
    foreach (KoView *view, views()) {
        KoCanvasBase *canvas = static_cast<KWView*>(view)->canvasBase();
        canvas->shapeManager()->remove(frame->shape());
    }
}

#ifndef NDEBUG
void KWDocument::printDebug()
{
    static const char * headerFooterType[] = { "None", "EvenOdd", "Uniform", "ERROR" };

    kDebug(32001) << "----------------------------------------";
    kDebug(32001) << "                 Debug info";
    kDebug(32001) << "Document:" << this;
    /*kDebug(32001) <<"Type of document:" << (m_pageStyle.hasMainTextFrame()?"WP":"DTP"); */
    kDebug(32001) << "Units:" << KoUnit::unitName(unit());
    kDebug(32001) << "# Framesets:" << frameSetCount();
    int i = 0;
    foreach (KWFrameSet *fs, m_frameSets) {
        kDebug(32001) << "Frameset" << i++ << ":" <<
        fs->name() << '(' << fs << ')';
        fs->printDebug();
    }

    kDebug(32001) << "PageManager holds" << pageCount() << " pages";
    KWPage page = m_pageManager.begin();
    while (page.isValid()) {
        int pgnum = page.pageNumber();
        QString side = "[Left] ";
        QString num = QString::number(pgnum);
        if (page.pageSide() == KWPage::Right)
            side = "[Right]";
        else if (page.pageSide() == KWPage::PageSpread) {
            side = "[PageSpread]";
            pgnum++;
            num += '-' + QString::number(pgnum);
        }
        kDebug(32001) << "Page" << num << side << " width:" << page.width() << " height:" << page.height() << "following" << page.pageStyle().name();
        if (page.isAutoGenerated())
            kDebug(32001) << "     Auto-generated page (inserted for text layout)";
        page = page.next();
    }

    foreach (const KWPageStyle &style, m_pageManager.pageStyles()) {
        kDebug(32001) << "PageStyle" << style.name();
        kDebug(32001) << "     Header:" << headerFooterType[style.headerPolicy()];
        kDebug(32001) << "     Footer:" << headerFooterType[style.footerPolicy()];
        KoColumns columns = style.columns();
        if (columns.columns != 1)
            kDebug(32001) << " +-- Columns:" << columns.columns << columns.columnSpacing << "pt spacing";
        KoPageLayout layout = style.pageLayout();
        kDebug(32001) << "     PageSize: " << layout.width << "x" << layout.height;
        kDebug(32001) << "     Indents: (tlbr, edge, binding)"
                      << layout.topMargin << layout.leftMargin
                      << layout.bottomMargin << layout.rightMargin
                      << layout.pageEdge << layout.bindingSide;
    }

    kDebug(32001) << "  The height of the doc (in pt) is:" << pageManager()->bottomOfPage(pageManager()->pageCount() - 1);
}
#endif

QList<KoDocument::CustomDocumentWidgetItem> KWDocument::createCustomDocumentWidgets(QWidget *parent)
{
    KoColumns columns;
    columns.columns = 1;
    columns.columnSpacing = m_config.defaultColumnSpacing();

    QList<KoDocument::CustomDocumentWidgetItem> widgetList;
    KoDocument::CustomDocumentWidgetItem item;
    item.widget = new KWStartupWidget(parent, this, columns);
    widgetList << item;
    return widgetList;
}

void KWDocument::saveConfig()
{
    if (!isReadWrite())
        return;
//   KConfigGroup group(KoGlobal::kofficeConfig(), "Spelling");
//   group.writeEntry("PersonalDict", m_spellCheckPersonalDict);

    if (isEmbedded())
        return;
    m_config.save();
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup interface = config->group("Interface");
    interface.writeEntry("ResolutionX", gridData().gridX());
    interface.writeEntry("ResolutionY", gridData().gridY());
}


// ************* PageProcessingQueue ************
PageProcessingQueue::PageProcessingQueue(KWDocument *parent)
    : QObject(parent)
{
    m_document = parent;
    m_triggered = false;
}

void PageProcessingQueue::addPage(KWPage page)
{
    m_pages.append(page);
    if (! m_triggered)
        QTimer::singleShot(0, this, SLOT(process()));
    m_triggered = true;
}

void PageProcessingQueue::process()
{
    m_triggered = false;
    const bool docIsEmpty = m_document->isEmpty();
    const bool docIsModified = m_document->isModified();
    foreach (const KWPage &page, m_pages) {
        if (! page.isValid())
            continue;
        m_document->m_frameLayout.createNewFramesForPage(page.pageNumber());
    }
    if (docIsEmpty)
        m_document->setEmpty();
    if (!docIsModified)
        m_document->setModified(false);
    m_pages.clear();
    deleteLater();
    emit m_document->pageSetupChanged();
}
