/* This file is part of the KDE project
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2010 C. Boemann <cbo@kogmbh.com>
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

#include "KWDocument.h"
#include "KWFactory.h"
#include "KWView.h"
#include "KWCanvas.h"
#include "KWCanvasItem.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWPageStyle.h"
#include "KWOdfLoader.h"
#include "KWOdfWriter.h"
#include "frames/KWFrameSet.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWFrame.h"
#include "frames/KWFrameLayout.h"
#include "frames/KWOutlineShape.h"
#include "dialogs/KWFrameDialog.h"
#include "commands/KWPageInsertCommand.h"
#include "commands/KWPageRemoveCommand.h"
#include "KWRootAreaProvider.h"

// calligra libs includes
#include <changetracker/KoChangeTracker.h>
#include <KoShapeManager.h>
#include <KoTextDocument.h>
#include <KoTextAnchor.h>
#include <KoShapeContainer.h>
#include <KoOdfWriteStore.h>
#include <KoToolManager.h>
#include <KoShapeController.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>
#include <KoStyleManager.h>
#include <KoDocumentResourceManager.h>
#include <KoCanvasResourceManager.h>
#include <KoInlineTextObjectManager.h>
#include <KoDocumentInfo.h>
#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoListStyle.h>
#include <KoListLevelProperties.h>
#include <KoTextShapeData.h>
#include <KoSelection.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>

#include <KoDocumentRdfBase.h>
#ifdef SHOULD_BUILD_RDF
#include <rdf/KoDocumentRdf.h>
#endif

#include <KoProgressUpdater.h>
#include <KoUpdater.h>

// KDE + Qt includes
#include <klocale.h>
#include <kstandardaction.h>
#include <kaction.h>
#include <kdebug.h>
#include <QIODevice>
#include <QTimer>
#include <QThread>
#include <QCoreApplication>
#include <QTextBlock>
#include <QTime>

KWDocument::KWDocument(KoPart *part)
        : KoDocument(part),
        m_frameLayout(&m_pageManager, m_frameSets),
        m_mainFramesetEverFinished(false)
{
    m_frameLayout.setDocument(this);
    resourceManager()->setOdfDocument(this);

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
/* TODO reenable after release
    QVariant variant;
    variant.setValue(new KoChangeTracker(resourceManager()));
    resourceManager()->setResource(KoText::ChangeTracker, variant);
*/
    m_shapeController = new KoShapeController(0, this);

    if (inlineTextObjectManager()) {
        connect(documentInfo(), SIGNAL(infoUpdated(const QString &, const QString &)),
                inlineTextObjectManager(), SLOT(documentInformationUpdated(const QString &, const QString &)));
    }

    clear();
}

KWDocument::~KWDocument()
{
    qDeleteAll(m_panelFactories);
    m_config.setUnit(unit());
    saveConfig();
    qDeleteAll(m_frameSets);
}

// Words adds a couple of dialogs (like KWFrameDialog) which will not call addShape(), but
// will call addFrameSet.  Which will itself call addFrame()
// any call coming in here is due to the undo/redo framework, pasting or for nested frames
void KWDocument::addShape(KoShape *shape)
{
    addShape(shape, 0);
}

void KWDocument::addShape(KoShape* shape, KoTextAnchor* anchor)
{
    KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
    kDebug(32001) << "shape=" << shape << "frame=" << frame;
    if (frame == 0) {
        if (shape->shapeId() == TextShape_SHAPEID) {
            KWTextFrameSet *tfs = new KWTextFrameSet(this);
            tfs->setName("Text");
            frame = new KWFrame(shape, tfs, anchor);
        } else {
            KWFrameSet *fs = new KWFrameSet();
            fs->setName(shape->shapeId());
            frame = new KWFrame(shape, fs, anchor);
        }
    }
    Q_ASSERT(frame->frameSet());
    if (!m_frameSets.contains(frame->frameSet())) {
        addFrameSet(frame->frameSet());
    }

    emit shapeAdded(shape);

    shape->update();
}

void KWDocument::removeShape(KoShape *shape)
{
    KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
    qDebug() << "shape=" << shape << "frame=" << frame << "frameSetType=" << (frame ? Words::frameSetTypeName(frame->frameSet()) : QString());
    if (frame) { // not all shapes have to have a frame. Only top-level ones do.
        KWFrameSet *fs = frame->frameSet();
        Q_ASSERT(fs);
        if (fs->frameCount() == 1) // last frame on FrameSet
            removeFrameSet(fs); // frame and frameset will be deleted when the shape is deleted
        else
            fs->removeFrame(frame);
    } else { // not a frame, but we still have to remove it from views.
        emit shapeRemoved(shape);
    }
}

void KWDocument::shapesRemoved(const QList<KoShape*> &shapes, KUndo2Command *command)
{
    QMap<KoTextEditor *, QList<KoTextAnchor *> > anchors;
    foreach (KoShape *shape, shapes) {
        KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
        if (frame && frame->shape()) {
            KoTextAnchor *anchor = frame->anchor();
            const QTextDocument *document = anchor ? anchor->document(): 0;
            if (document) {
                KoTextEditor *editor = KoTextDocument(document).textEditor();
                anchors[editor].append(anchor);
            }
        }
    }
    QMap<KoTextEditor *, QList<KoTextAnchor *> >::const_iterator it(anchors.constBegin());
    for (; it != anchors.constEnd(); ++it) {
        it.key()->removeAnchors(it.value(), command);
    }
}

QPixmap KWDocument::generatePreview(const QSize &size)
{
    // use first page as preview for all pages
    KWPage firstPage = pageManager()->begin();
    if (! firstPage.isValid()) {
        // TODO: what to return for no page?
        return QPixmap();
    }

    // use shape manager from canvasItem even for QWidget environments
    // if using the shape manager from one of the views there is no guarantee
    // that the view, its canvas and the shapemanager is not destroyed in between
//FIXME    KoShapeManager* shapeManager = static_cast<KWCanvasItem*>(canvasItem())->shapeManager();

//FIXME    return QPixmap::fromImage(firstPage.thumbnail(size, shapeManager));
        return QPixmap();
}

void KWDocument::paintContent(QPainter &, const QRect &)
{
}

KWPage KWDocument::insertPage(int afterPageNum, const QString &masterPageName, bool addUndoRedoCommand)
{
    kDebug(32001) << "afterPageNum=" << afterPageNum << "masterPageName=" << masterPageName;
    KWPageInsertCommand *cmd = new KWPageInsertCommand(this, afterPageNum, masterPageName);
    if (addUndoRedoCommand)
        addCommand(cmd);
    else
        cmd->redo();
    Q_ASSERT(cmd->page().isValid());
    KWPage page = cmd->page();
    if (!addUndoRedoCommand)
        delete cmd;
    return page;
}

KWPage KWDocument::appendPage(const QString &masterPageName, bool addUndoRedoCommand)
{
    int number = 0;
    KWPage last = m_pageManager.last();
    if (last.isValid())
        number = last.pageNumber();
    return insertPage(number, masterPageName, addUndoRedoCommand);
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
    kDebug(32001);
    if (inlineTextObjectManager())
        inlineTextObjectManager()->setProperty(KoInlineObject::PageCount, pageCount());
    emit pageSetupChanged();
}

void KWDocument::removeFrameSet(KWFrameSet *fs)
{
    kDebug(32001) << "frameSet=" << fs;
    m_frameSets.removeAt(m_frameSets.indexOf(fs));
    setModified(true);
    foreach (KWFrame *frame, fs->frames())
        removeFrame(frame);

    emit resourceChanged(Words::CurrentFrameSetCount, m_frameSets.count());

    disconnect(fs, SIGNAL(frameAdded(KWFrame*)), this, SLOT(addFrame(KWFrame*)));
    disconnect(fs, SIGNAL(frameRemoved(KWFrame*)), this, SLOT(removeFrame(KWFrame*)));
}

void KWDocument::relayout(QList<KWFrameSet*> framesets)
{
    if (framesets.isEmpty())
        framesets = m_frameSets;

    kDebug(32001) << "frameSets=" << framesets;

#if 0
    foreach (KWFrameSet *fs, m_frameSets) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs == 0) continue;
        if (tfs->textFrameSetType() != Words::MainTextFrameSet) continue;
        QSet<KWPage> coveredPages;
        QList<int> coveredPageNumbers;
        foreach (KWFrame *frame, tfs->frames()) {
            KWPage page = pageManager()->page(frame->shape());
            if (page.isValid()) {
                if (! coveredPages.contains(page)) {
                    coveredPages += page;
                    coveredPageNumbers << page.pageNumber();
                    continue; // keep one frame per page.
                }
            }
            kDebug(32001) << "Delete frame=" << frame << "pageNumber=" << page.pageNumber();
            foreach (KoView *view, views()) {
                KoCanvasBase *canvas = static_cast<KWView*>(view)->canvasBase();
                canvas->shapeManager()->remove(frame->shape());
            }
            tfs->removeFrame(frame);
            delete frame->shape();
        }
        kDebug(32001) << "coveredPageNumbers=" << coveredPageNumbers;
    }
#endif

    // we switch to the interaction tool to avoid crashes if the tool was editing a frame.
    //KoToolManager::instance()->switchToolRequested(KoInteractionTool_ID);

    // remove header/footer frames that are not visible.
    //m_frameLayout.cleanupHeadersFooters();

    // create new frames and lay them out on the pages
    foreach (const KWPage &page, m_pageManager.pages()) {
        m_frameLayout.createNewFramesForPage(page.pageNumber());
    }

    // re-layout the content displayed within the pages
    foreach (KWFrameSet *fs, framesets) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (!tfs)
            continue;
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(tfs->document()->documentLayout());
        Q_ASSERT(lay);

        if (tfs->textFrameSetType() == Words::MainTextFrameSet && m_layoutProgressUpdater) {
            connect(lay, SIGNAL(layoutProgressChanged(int)), this, SLOT(layoutProgressChanged(int)));
            connect(lay, SIGNAL(finishedLayout()), this, SLOT(layoutFinished()));
        }

        // schedule all calls so multiple layout calls are compressed
        lay->scheduleLayout();
    }

    firePageSetupChanged();
}

void KWDocument::layoutProgressChanged(int percent)
{
    Q_ASSERT(m_layoutProgressUpdater);
    m_layoutProgressUpdater->setProgress(percent);
}

void KWDocument::layoutFinished()
{
    Q_ASSERT(m_layoutProgressUpdater);
    disconnect(QObject::sender(), SIGNAL(layoutProgressChanged(int)), this, SLOT(layoutProgressChanged(int)));
    disconnect(QObject::sender(), SIGNAL(finishedLayout()), this, SLOT(layoutFinished()));
    m_layoutProgressUpdater->setProgress(100);
    m_layoutProgressUpdater = 0; // free the instance
}

void KWDocument::addFrameSet(KWFrameSet *fs)
{
    kDebug(32001) << "frameSet=" << fs;

    Q_ASSERT(!m_frameSets.contains(fs));
    setModified(true);
#if 0
    m_frameSets.append(fs);
#else
    // Be sure we add headers and footers to the beginning of the m_frameSets QList and every other KWFrameTextType
    // after them so future operations iterating over that QList always handle headers and footers first.
    int insertAt = m_frameSets.count();
    KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
    if (tfs && Words::isHeaderFooter(tfs)) {
        insertAt = 0;
        for(int i = 0; i < m_frameSets.count(); ++i) {
            KWTextFrameSet *_tfs = dynamic_cast<KWTextFrameSet*>(m_frameSets[i]);
            if (_tfs && !Words::isHeaderFooter(_tfs)) {
                insertAt = i;
                break;
            }
        }
    }
    m_frameSets.insert(insertAt, fs);
#endif
    foreach (KWFrame *frame, fs->frames())
        addFrame(frame);

    if (KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs)) {
        Q_ASSERT(tfs->pageManager() == pageManager());
        if (tfs->textFrameSetType() == Words::MainTextFrameSet) {
            KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(tfs->document()->documentLayout());
            Q_ASSERT(lay);
            connect(lay, SIGNAL(finishedLayout()), this, SLOT(mainTextFrameSetLayoutDone()));
        }
    }

    connect(fs, SIGNAL(frameAdded(KWFrame*)), this, SLOT(addFrame(KWFrame*)));
    connect(fs, SIGNAL(frameRemoved(KWFrame*)), this, SLOT(removeFrame(KWFrame*)));
}

void KWDocument::addFrame(KWFrame *frame)
{
    kDebug(32001) << "frame=" << frame << "frameSet=" << frame->frameSet();
    //firePageSetupChanged();
    emit resourceChanged(Words::CurrentFrameSetCount, m_frameSets.count());
    emit shapeAdded(frame->shape(), KoShapeManager::AddWithoutRepaint);
}

void KWDocument::removeFrame(KWFrame *frame)
{
    if (frame->shape() == 0) return;
    kDebug(32001) << "frame=" << frame << "frameSet=" << frame->frameSet();

    emit shapeRemoved(frame->shape());
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
    parag->setFontPointSize(20);
    parag->setFontWeight(QFont::Bold);
    styleManager->add(parag);

    parag = new KoParagraphStyle();
    parag->setName(i18n("Head 2"));
    parag->setFontPointSize(16);
    parag->setFontWeight(QFont::Bold);
    styleManager->add(parag);

    parag = new KoParagraphStyle();
    parag->setName(i18n("Head 3"));
    parag->setFontPointSize(12);
    parag->setFontWeight(QFont::Bold);
    styleManager->add(parag);

    parag = new KoParagraphStyle();
    parag->setName(i18n("Bullet List"));
    KoListStyle *list = new KoListStyle(parag);
    KoListLevelProperties llp = list->levelProperties(0);
    llp.setStyle(KoListStyle::Bullet);
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

void KWDocument::setupOpenFileSubProgress()
{
    if (progressUpdater()) {
        m_layoutProgressUpdater = progressUpdater()->startSubtask(1, "Layouting");
    }
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
    Q_UNUSED(doc);
    Q_UNUSED(store);
    return false;
}

void KWDocument::endOfLoading() // called by both oasis and oldxml
{
    kDebug(32001);

    // Get the master page name of the first page.
    QString firstPageMasterName;
    if (mainFrameSet()) {
        QTextBlock block = mainFrameSet()->document()->firstBlock();
        firstPageMasterName = block.blockFormat().stringProperty(KoParagraphStyle::MasterPageName);
    }

    KWPage lastpage = pageManager()->last();
    qreal docHeight = lastpage.isValid() ? (lastpage.offsetInDocument() + lastpage.height()) : 0.0;

    // insert pages
    qreal maxBottom = 0;
    foreach (KWFrameSet *fs, m_frameSets) {
        foreach (KWFrame *frame, fs->frames())
        maxBottom = qMax(maxBottom, frame->shape()->boundingRect().bottom());
    }

    // Here we look at point 'b'. We add pages so at least all frames have a page.
    // btw. the observent reader might notice that cases b and c are not mutually exclusive ;)
    while (docHeight <= maxBottom) {
        kDebug(32001) << "KWDocument::endOfLoading appends a page";
        if (m_pageManager.pageCount() == 0) { // apply the firstPageMasterName only on the first page
            //lastpage = m_pageManager.appendPage(m_pageManager.pageStyle(firstPageMasterName));
            lastpage = appendPage(firstPageMasterName);
        } else { // normally this shouldn't happen cause that loop is only run once...
            //lastpage = m_pageManager.appendPage();
            lastpage = appendPage();
        }
        docHeight += lastpage.height();
    }

#if 0
    // do some sanity checking on document.
    for (int i = frameSetCount() - 1; i > -1; i--) {
        KWFrameSet *fs = frameSet(i);
        if (!fs) {
            kWarning(32001) << "frameset " << i << " is NULL!!";
            m_lstFrameSet.remove(i);
            continue;
        }
        if (fs->type() == FT_TABLE) {
            static_cast<KWTableFrameSet *>(fs)->validate();
        } else if (fs->type() == FT_TEXT) {
            for (int f = fs->frameCount() - 1; f >= 0; f--) {
                KWFrame *frame = fs->frame(f);
                if (frame->left() < 0) {
                    kWarning(32001) << fs->name() << " frame " << f << " pos.x is < 0, moving frame";
                    frame->moveBy(0 - frame->left(), 0);
                }
                if (frame->right() > m_pageLayout.ptWidth) {
                    kWarning(32001) << fs->name() << " frame " << f << " rightborder outside page ("
                    << frame->right() << ">" << m_pageLayout.ptWidth << "), shrinking";
                    frame->setRight(m_pageLayout.ptWidth);
                }
                if (fs->isProtectSize())
                    continue; // don't make frames bigger of a protected frameset.
                if (frame->height() < s_minFrameHeight) {
                    kWarning(32001) << fs->name() << " frame " << f << " height is so small no text will fit, adjusting (was: "
                    << frame->height() << " is: " << s_minFrameHeight << ")";
                    frame->setHeight(s_minFrameHeight);
                }
                if (frame->width() < s_minFrameWidth) {
                    kWarning(32001) << fs->name() << " frame " << f << " width is so small no text will fit, adjusting (was: "
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
            kWarning(32001) << "frameset " << i << " " << fs->name() << " has no frames";
            removeFrameSet(fs);
            if (fs->type() == FT_PART)
                delete static_cast<KWPartFrameSet *>(fs)->getChild();
            delete fs;
            continue;
        }
        if (fs->frameCount() > 0) {
            KWFrame *frame = fs->frame(0);
            if (frame->isCopy()) {
                kWarning(32001) << "First frame in a frameset[" << fs->name() << "] was set to be a copy; resetting";
                frame->setCopy(false);
            }
        }
    }

    // Renumber footnotes
    KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet *>(m_lstFrameSet.getFirst());
    if (frameset)
        frameset->renumberFootNotes(false /*no repaint*/);
#endif

    relayout();

    kDebug(32001) << "KWDocument::endOfLoading done";
#if 0
    // Note that more stuff will happen in completeLoading
    firePageSetupChanged();
#endif
    setModified(false);
}

bool KWDocument::saveOdf(SavingContext &documentContext)
{
    KWOdfWriter writer(this);
    return writer.save(documentContext.odfStore, documentContext.embeddedSaver);
}

void KWDocument::updatePagesForStyle(const KWPageStyle &style)
{
    kDebug(32001) << "pageStyleName=" << style.name();
    QList<KWFrameSet*> framesets;
    foreach(KWFrameSet *fs, frameLayout()->getFrameSets(style)) {
        KWTextFrameSet* tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs)
            framesets.append(tfs);
    }
    int pageNumber = -1;
    foreach (KWPage page, pageManager()->pages()) {
        if (page.pageStyle() == style) {
            pageNumber = page.pageNumber();
            break;
        }
    }
    //Q_ASSERT(pageNumber >= 1);
    if (pageNumber < 1)
        return;
    foreach(KWFrameSet *fs, framesets) {
        static_cast<KWTextFrameSet*>(fs)->rootAreaProvider()->clearPages(pageNumber);
    }
    relayout(framesets);
}

void KWDocument::saveConfig()
{
//   KConfigGroup group(KoGlobal::calligraConfig(), "Spelling");
//   group.writeEntry("PersonalDict", m_spellCheckPersonalDict);

    m_config.save();
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup interface = config->group("Interface");
    interface.writeEntry("ResolutionX", gridData().gridX());
    interface.writeEntry("ResolutionY", gridData().gridY());
}

KWFrame* KWDocument::findClosestFrame(KoShape* shape) const
{
    KWFrame *result = 0;
    int      area   = 0;
    QRectF   br     = shape->boundingRect();

    // now find the frame that is closest to the frame we want to inline.
    foreach (KWFrame *frame, mainFrameSet()->frames()) {
        QRectF intersection  = br.intersected(frame->shape()->boundingRect());
        int    intersectArea = qRound(intersection.width() * intersection.height());

        if (intersectArea > area) {
            result = frame;
            area   = intersectArea;
        } else if (result == 0) {
            // TODO check distance between frames or something.
        }
    }

    return result;
}

KoTextAnchor* KWDocument::anchorOfShape(KoShape *shape) const
{
    Q_ASSERT(mainFrameSet());
    Q_ASSERT(shape);

    // try and find out if shape is already anchored    
    foreach (KoInlineObject *inlineObject, inlineTextObjectManager()->inlineTextObjects()) {
        KoTextAnchor *anchor = dynamic_cast<KoTextAnchor *>(inlineObject);
        if (anchor && anchor->shape() == shape) {
            return anchor;
        }
    }

    KWFrame *frame = frameOfShape(shape);
    KoTextAnchor *anchor = frame->anchor();

    if (!anchor) {
        anchor = new KoTextAnchor(shape);
        anchor->setAnchorType(KoTextAnchor::AnchorPage);
        anchor->setHorizontalPos(KoTextAnchor::HFromLeft);
        anchor->setVerticalPos(KoTextAnchor::VFromTop);
        frame->setAnchor(anchor);
    }

    return anchor;
}


KWFrame *KWDocument::frameOfShape(KoShape* shape) const
{
    while (shape) {
        KWFrame *answer = dynamic_cast<KWFrame*>(shape->applicationData());
        if (answer)
            return answer;
        if (shape->parent() == 0)
            break;
        shape = shape->parent();
    }

    KWFrame *answer = dynamic_cast<KWFrame*>(shape->applicationData());
    if (answer == 0) { // this may be a clipping shape containing the frame-shape
        KoShapeContainer *container = dynamic_cast<KoShapeContainer*>(shape);
        if (container && container->shapeCount() == 1) {
            answer = dynamic_cast<KWFrame*>(container->shapes()[0]->applicationData());
        }
    }

    return answer;
}

