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
#include "dialogs/KWFrameDialog.h"
#include "KWRootAreaProvider.h"
#include "WordsDebug.h"

// calligra libs includes
#include <changetracker/KoChangeTracker.h>
#include <KoShapeManager.h>
#include <KoTextDocument.h>
#include <KoAnnotation.h>
#include <KoGridData.h>
#include <KoShapeAnchor.h>
#include <KoShapeContainer.h>
#include <KoToolManager.h>
#include <KoShapeController.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>
#include <KoStyleManager.h>
#include <KoDocumentResourceManager.h>
#include <KoCanvasResourceManager.h>
#include <KoTextRangeManager.h>
#include <KoInlineTextObjectManager.h>
#include <KoDocumentInfo.h>
#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoListStyle.h>
#include <KoListLevelProperties.h>
#include <KoSelection.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>
#include <KoTextEditor.h>
#include <KoPart.h>
#include <KoDocumentInfoDlg.h>
#include <KoDocumentRdfBase.h>
#include <KoAnnotationLayoutManager.h>
#include <KoPageWidgetItem.h>
#include <KoUnit.h>

#ifdef SHOULD_BUILD_RDF
#include <KoDocumentRdf.h>
#include <KoDocumentRdfEditWidget.h>
#endif

#include <KoProgressUpdater.h>
#include <KoUpdater.h>

// KF5
#include <klocalizedstring.h>
#include <kconfiggroup.h>
#include <KSharedConfig>

// Qt
#include <QIODevice>
#include <QTimer>
#include <QThread>
#include <QCoreApplication>
#include <QTextBlock>
#include <QTime>

KWDocument::KWDocument(KoPart *part)
        : KoDocument(part)
        , m_isMasterDocument(false)
        , m_frameLayout(&m_pageManager, m_frameSets)
        , m_mainFramesetEverFinished(false)
        , m_annotationManager(0)
{
    Q_ASSERT(part);
    m_frameLayout.setDocument(this);
    resourceManager()->setOdfDocument(this);

    connect(&m_frameLayout, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFrameSet(KWFrameSet*)));
    connect(&m_frameLayout, SIGNAL(removedFrameSet(KWFrameSet*)), this, SLOT(removeFrameSet(KWFrameSet*)));

    // Init shape Factories with our frame based configuration panels.
    m_panelFactories = KWFrameDialog::panels(this);
    foreach (const QString &id, KoShapeRegistry::instance()->keys()) {
        KoShapeFactoryBase *shapeFactory = KoShapeRegistry::instance()->value(id);
        if (shapeFactory) {
            shapeFactory->setOptionPanels(m_panelFactories);
        }
    }

    resourceManager()->setUndoStack(undoStack());
    if (documentRdf()) {
        documentRdf()->linkToResourceManager(resourceManager());
    }

#ifdef SHOULD_BUILD_RDF
    {
        KoDocumentRdf *rdf = new KoDocumentRdf(this);
        setDocumentRdf(rdf);
    }

#endif



/* TODO reenable after release
    QVariant variant;
    variant.setValue(new KoChangeTracker(resourceManager()));
    resourceManager()->setResource(KoText::ChangeTracker, variant);
*/
    m_shapeController = new KoShapeController(0, this);

    if (inlineTextObjectManager()) {
        connect(documentInfo(), SIGNAL(infoUpdated(QString,QString)),
                inlineTextObjectManager(), SLOT(documentInformationUpdated(QString,QString)));
    }

    m_annotationManager = new KoAnnotationLayoutManager(this);

    clear();
}

KWDocument::~KWDocument()
{
    qDeleteAll(m_panelFactories);
    m_config.setUnit(unit());
    saveConfig();
    qDeleteAll(m_frameSets);
}

bool KWDocument::isMasterDocument() const
{
    return m_isMasterDocument;
}

void KWDocument::setIsMasterDocument(bool isMasterDocument)
{
    m_isMasterDocument = isMasterDocument;
}



// Words adds a couple of dialogs (like KWFrameDialog) which will not call addShape(), but
// will call addFrameSet.  Which will itself call addSequencedShape()
// any call coming in here is due to the undo/redo framework, pasting or for nested frames
void KWDocument::addShape(KoShape *shape)
{
    KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
    debugWords << "shape=" << shape << "frame=" << frame;
    if (frame == 0) {
        if (shape->shapeId() == TextShape_SHAPEID) {
            KWTextFrameSet *tfs = new KWTextFrameSet(this);
            tfs->setName("Text");
            frame = new KWFrame(shape, tfs);
        } else {
            KWFrameSet *fs = new KWFrameSet();
            fs->setName(shape->shapeId());
            frame = new KWFrame(shape, fs);
        }
    }
    Q_ASSERT(KWFrameSet::from(shape));
    if (!m_frameSets.contains(KWFrameSet::from(shape))) {
        addFrameSet(KWFrameSet::from(shape));
    }

    if (!(shape->shapeId() == "AnnotationTextShapeID")) {
        emit shapeAdded(shape, KoShapeManager::PaintShapeOnAdd);
    }

    shape->update();
}

void KWDocument::removeShape(KoShape *shape)
{
    debugWords << "shape=" << shape;
    KWFrameSet *fs = KWFrameSet::from(shape);
    if (fs) { // not all shapes have to have to be in a frameset
        if (fs->shapeCount() == 1) // last shape on FrameSet
            removeFrameSet(fs); // shape and frameset will be deleted when the shape is deleted
        else
            fs->removeShape(shape);
    } else { // not in a frameset, but we still have to remove it from views.
        emit shapeRemoved(shape);
    }
    if (shape->shapeId() == "AnnotationTextShapeID") {
        annotationLayoutManager()->removeAnnotationShape(shape);
    }
}

void KWDocument::shapesRemoved(const QList<KoShape*> &shapes, KUndo2Command *command)
{
    QMap<KoTextEditor *, QList<KoShapeAnchor *> > anchors;
    QMap<KoTextEditor *, QList<KoAnnotation *> > annotations;
    const KoAnnotationManager *annotationManager = textRangeManager()->annotationManager();
    foreach (KoShape *shape, shapes) {
        KoShapeAnchor *anchor = shape->anchor();
        if (anchor && anchor->textLocation()) {
            const QTextDocument *document = anchor->textLocation()->document();
            if (document) {
                KoTextEditor *editor = KoTextDocument(document).textEditor();
                anchors[editor].append(anchor);
            }
            break;
        }
        foreach (const QString &name, annotationManager->annotationNameList()) {
            KoAnnotation *annotation = annotationManager->annotation(name);
            if (annotation->annotationShape() == shape) {
                // Remove From annotation layout manager.
                KoTextEditor *editor = KoTextDocument(annotation->document()).textEditor();
                annotations[editor].append(annotation);
                break;
            }
        }
    }

    QMap<KoTextEditor *, QList<KoShapeAnchor *> >::const_iterator anchorIter(anchors.constBegin());
    for (; anchorIter != anchors.constEnd(); ++anchorIter) {
        anchorIter.key()->removeAnchors(anchorIter.value(), command);
    }

    QMap<KoTextEditor *, QList<KoAnnotation *> >::const_iterator annotationIter(annotations.constBegin());
    for (; annotationIter != annotations.constEnd(); ++annotationIter) {
        annotationIter.key()->removeAnnotations(annotationIter.value(), command);
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
    KoShapeManager* shapeManager = static_cast<KWCanvasItem*>(documentPart()->canvasItem(this))->shapeManager();

    return QPixmap::fromImage(firstPage.thumbnail(size, shapeManager, true));
}

void KWDocument::paintContent(QPainter &, const QRect &)
{
}

KWPage KWDocument::insertPage(int afterPageNum, const QString &masterPageName)
{
    debugWords << "afterPageNum=" << afterPageNum << "masterPageName=" << masterPageName;

    //KWPage prevPage = m_document->pageManager().page(m_afterPageNum);
    KWPageStyle pageStyle = pageManager()->pageStyle(masterPageName);
    KWPage page = pageManager()->insertPage(afterPageNum + 1, pageStyle);
    Q_ASSERT(page.isValid());
    Q_ASSERT(page.pageNumber() >= 1 && page.pageNumber() <= pageManager()->pageCount());

    // Set the y-offset of the new page.
    KWPage prevPage = page.previous();
    if (prevPage.isValid()) {
        KoInsets padding = pageManager()->padding();    //TODO Shouldn't this be style dependent ?
        page.setOffsetInDocument(prevPage.offsetInDocument() + prevPage.height() + padding.top + padding.bottom);
    } else {
        page.setOffsetInDocument(0.0);
    }

    debugWords << "pageNumber=" << page.pageNumber();

    // Create the KWTextFrame's for the new KWPage
    KWFrameLayout *framelayout = frameLayout();
    framelayout->createNewFramesForPage(page.pageNumber());

    // make sure we have updated the view before we do anything else
    firePageSetupChanged();

    return page;
}

KWPage KWDocument::appendPage(const QString &masterPageName)
{
    int number = 0;
    KWPage last = m_pageManager.last();
    if (last.isValid())
        number = last.pageNumber();
    return insertPage(number, masterPageName);
}

void KWDocument::firePageSetupChanged()
{
    debugWords;
    if (inlineTextObjectManager())
        inlineTextObjectManager()->setProperty(KoInlineObject::PageCount, pageCount());
    emit pageSetupChanged();
}

void KWDocument::removeFrameSet(KWFrameSet *fs)
{
    debugWords << "frameSet=" << fs;
    m_frameSets.removeAt(m_frameSets.indexOf(fs));
    setModified(true);
    foreach (KoShape *shape, fs->shapes())
        removeSequencedShape(shape);

    disconnect(fs, SIGNAL(shapeAdded(KoShape*)), this, SLOT(addSequencedShape(KoShape*)));
    disconnect(fs, SIGNAL(shapeRemoved(KoShape*)), this, SLOT(removeSequencedShape(KoShape*)));
}

void KWDocument::relayout(QList<KWFrameSet*> framesets)
{
    if (framesets.isEmpty())
        framesets = m_frameSets;

    debugWords << "frameSets=" << framesets;


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
    debugWords << "frameSet=" << fs;

    Q_ASSERT(!m_frameSets.contains(fs));
    setModified(true);

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

    foreach (KoShape *shape, fs->shapes())
        addSequencedShape(shape);

    if (KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs)) {
        Q_ASSERT(tfs->pageManager() == pageManager());
        if (tfs->textFrameSetType() == Words::MainTextFrameSet) {
            KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(tfs->document()->documentLayout());
            Q_ASSERT(lay);
            connect(lay, SIGNAL(finishedLayout()), this, SLOT(mainTextFrameSetLayoutDone()));
        }
    }

    connect(fs, SIGNAL(shapeAdded(KoShape*)), this, SLOT(addSequencedShape(KoShape*)));
    connect(fs, SIGNAL(shapeRemoved(KoShape*)), this, SLOT(removeSequencedShape(KoShape*)));
}

void KWDocument::addSequencedShape(KoShape *shape)
{
    debugWords << "shape=" << shape << "frameSet=" << KWFrameSet::from(shape);
    //firePageSetupChanged();
    emit shapeAdded(shape, KoShapeManager::AddWithoutRepaint);
}

void KWDocument::removeSequencedShape(KoShape *shape)
{
    debugWords << "shape=" << shape << "frameSet=" << KWFrameSet::from(shape);

    emit shapeRemoved(shape);
    KWPage page = pageManager()->page(shape);
    if (!page.isValid()) return;
    if (!page.isAutoGenerated()) return;
    if (page != pageManager()->last() || page == pageManager()->begin())
        return; // can only delete last page.
    foreach (KWFrameSet *fs, m_frameSets) {
        foreach (KoShape *s, fs->shapes()) {
            if (page == pageManager()->page(s))
                return;
        }
    }
    //KWPageRemoveCommand *cmd = new KWPageRemoveCommand(this, page);
    //cmd->redo();
    //delete cmd;
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

KoTextRangeManager *KWDocument::textRangeManager() const
{
    QVariant var = resourceManager()->resource(KoText::TextRangeManager);
    return var.value<KoTextRangeManager*>();
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
        QString name = QString(before + (count == 0 ? QString() : QString::number(count)) + after).trimmed();
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
    parag->setName(i18n("Standard"));
    parag->setFontPointSize(12);
    parag->setFontWeight(QFont::Normal);
    styleManager->add(parag);

    parag = new KoParagraphStyle();
    parag->setName(i18n("Document Title"));
    parag->setFontPointSize(24);
    parag->setFontWeight(QFont::Bold);
    parag->setAlignment(Qt::AlignCenter);
    styleManager->add(parag);

    parag = new KoParagraphStyle();
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
    llp.setLabelType(KoListStyle::BulletCharLabelType);
    llp.setBulletCharacter(QChar(0x2022)); // Bullet
    list->setLevelProperties(llp);
    parag->setListStyle(list);
    styleManager->add(parag);

    setMimeTypeAfterLoading("application/vnd.oasis.opendocument.text");
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
    debugWords;

    // Get the master page name of the first page.
    QString firstPageMasterName;
    if (mainFrameSet()) {
        QTextBlock block = mainFrameSet()->document()->firstBlock();
        firstPageMasterName = block.blockFormat().stringProperty(KoParagraphStyle::MasterPageName);
    }

    appendPage(firstPageMasterName);

    relayout();

    debugWords << "KWDocument::endOfLoading done";
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
    debugWords << "pageStyleName=" << style.name();
    QList<KWFrameSet*> framesets;
    foreach(KWFrameSet *fs, frameLayout()->getFrameSets(style)) {
        KWTextFrameSet* tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs)
            framesets.append(tfs);
    }
    int pageNumber = -1;
    foreach (const KWPage &page, pageManager()->pages()) {
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
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup interface = config->group("Interface");
    interface.writeEntry("ResolutionX", gridData().gridX());
    interface.writeEntry("ResolutionY", gridData().gridY());
}

KoShape *KWDocument::findTargetTextShape(KoShape *shape) const
{
    KoShape *result = 0;
    int      area   = 0;
    QRectF   br     = shape->boundingRect();

    // now find the frame that is closest to the frame we want to inline.
    foreach (KoShape *shape, mainFrameSet()->shapes()) {
        QRectF intersection  = br.intersected(shape->boundingRect());
        int    intersectArea = qRound(intersection.width() * intersection.height());

        if (intersectArea > area) {
            result = shape;
            area   = intersectArea;
        } else if (result == 0) {
            // TODO check distance between frames or something.
        }
    }

    return result;
}

KoShapeAnchor* KWDocument::anchorOfShape(KoShape *shape) const
{
    Q_ASSERT(mainFrameSet());
    Q_ASSERT(shape);

    KoShapeAnchor *anchor = shape->anchor();

    if (!anchor) {
        anchor = new KoShapeAnchor(shape);
        anchor->setAnchorType(KoShapeAnchor::AnchorPage);
        anchor->setHorizontalPos(KoShapeAnchor::HFromLeft);
        anchor->setVerticalPos(KoShapeAnchor::VFromTop);
        shape->setAnchor(anchor);
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

KoDocumentInfoDlg *KWDocument::createDocumentInfoDialog(QWidget *parent, KoDocumentInfo *docInfo) const
{

    KoDocumentInfoDlg *dlg = new KoDocumentInfoDlg(parent, docInfo);
    KoMainWindow *mainwin = dynamic_cast<KoMainWindow*>(parent);
    if (mainwin) {
        connect(dlg, SIGNAL(saveRequested()), mainwin, SLOT(slotFileSave()));
    }

#ifdef SHOULD_BUILD_RDF
    KoPageWidgetItem *rdfEditWidget = new KoDocumentRdfEditWidget(static_cast<KoDocumentRdf*>(documentRdf()));
    dlg->addPageItem(rdfEditWidget);
#endif
    return dlg;
}
