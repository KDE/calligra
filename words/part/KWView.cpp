/* This file is part of the KDE project
 * Copyright (C) 2001 David Faure <faure@kde.org>
 * Copyright (C) 2005-2007, 2009, 2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010-2011 Boudewijn Rempt <boud@kogmbh.com>
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

// words includes
#include "KWView.h"
#include "KWGui.h"
#include "KWDocument.h"
#include "KWCanvas.h"
#include "KWViewMode.h"
#include "KWFactory.h"
#include "KWStatusBar.h"
#include "KWPageManager.h"
#include "KWPageStyle.h"
#include "frames/KWFrame.h"
#include "frames/KWCopyShape.h"
#include "frames/KWTextFrameSet.h"
#include "dialogs/KWFrameDialog.h"
#include "dialogs/KWPageSettingsDialog.h"
#include "dialogs/KWStatisticsDialog.h"
#include "dialogs/KWPrintingDialog.h"
#include "dialogs/KWCreateBookmarkDialog.h"
#include "dialogs/KWSelectBookmarkDialog.h"
#include "dialogs/KWInsertPageDialog.h"
#include "commands/KWFrameCreateCommand.h"
#include "commands/KWClipFrameCommand.h"
#include "commands/KWRemoveFrameClipCommand.h"
#include "commands/KWShapeCreateCommand.h"
#include <KoShapeReorderCommand.h>
#include "ui_KWInsertImage.h"

// calligra libs includes
#include <calligraversion.h>
#include <KoCanvasController.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>
#include <KoProperties.h>
#include <KoCopyController.h>
#include <KoTextDocument.h>
#include <KoTextShapeData.h>
#include <KoCanvasResourceManager.h>
#include <KoCutController.h>
#include <KoStandardAction.h>
#include <KoPasteController.h>
#include <KoShape.h>
#include <KoText.h>
#include <KoFind.h>
#include <KoShapeContainer.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoShapeController.h>
#include <KoZoomAction.h>
#include <KoToolManager.h>
#include <KoMainWindow.h>
#include <KoTextEditor.h>
#include <KoToolProxy.h>
#include <KoTextAnchor.h>
#include <KoShapeGroupCommand.h>
#include <KoZoomController.h>
#include <KoInlineTextObjectManager.h>
#include <KoBookmark.h>
#include <KoPathShape.h> // for KoPathShapeId
#include <KoCanvasController.h>
#include <KoDocumentRdfBase.h>
#ifdef SHOULD_BUILD_RDF
#include <rdf/KoDocumentRdf.h>
#include <rdf/KoSemanticStylesheetsEditor.h>
#endif
#include <KoFindText.h>
#include <KoFindToolbar.h>
#include <KoTextLayoutRootArea.h>

// KDE + Qt includes
#include <QHBoxLayout>
#include <KMenu>
#include <QTimer>
#include <klocale.h>
#include <kdebug.h>
#include <kicon.h>
#include <kdialog.h>
#include <KToggleAction>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kxmlguifactory.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <KParts/PartManager>

KWView::KWView(const QString &viewMode, KWDocument *document, QWidget *parent)
        : KoView(document, parent)
        , m_canvas(0), m_actionMenu(0)
{
    setAcceptDrops(true);

    m_document = document;
    m_snapToGrid = m_document->gridData().snapToGrid();
    m_gui = new KWGui(viewMode, this);
    m_canvas = m_gui->canvas();
    setFocusProxy(m_canvas);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_gui);

    setComponentData(KWFactory::componentData());
    setXMLFile("words.rc");

    m_currentPage = m_document->pageManager()->begin();

    setupActions();

    connect(m_canvas->shapeManager()->selection(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    QList<QTextDocument*> texts;
    KoFindText::findTextInShapes(m_canvas->shapeManager()->shapes(), texts);
    KoMainWindow *win = qobject_cast<KoMainWindow*>(window());
    if(win) {
        connect(win->partManager(), SIGNAL(activePartChanged(KParts::Part*)), this, SLOT(loadingCompleted()));
    }

    m_find = new KoFindText(texts, this);
    KoFindToolbar *toolbar = new KoFindToolbar(m_find, actionCollection(), this);
    toolbar->setVisible(false);
    connect(m_find, SIGNAL(matchFound(KoFindMatch)), this, SLOT(findMatchFound(KoFindMatch)));
    connect(m_find, SIGNAL(updateCanvas()), m_canvas, SLOT(update()));

    layout->addWidget(toolbar);

    m_zoomController = new KoZoomController(m_gui->canvasController(), &m_zoomHandler, actionCollection(), 0, this);
    if (statusBar())
        KWStatusBar::addViewControls(statusBar(), this);

    // the zoom controller needs to be initialized after the status bar gets initialized as
    // that resulted in bug 180759
    m_zoomController->setPageSize(m_currentPage.rect().size());
    KoZoomMode::Modes modes = KoZoomMode::ZOOM_WIDTH;
    if (m_canvas->viewMode()->hasPages())
        modes |= KoZoomMode::ZOOM_PAGE;
    m_zoomController->zoomAction()->setZoomModes(modes);
    connect(m_canvas, SIGNAL(documentSize(const QSizeF &)), m_zoomController, SLOT(setDocumentSize(const QSizeF&)));
    m_canvas->updateSize(); // to emit the doc size at least once
    m_zoomController->setZoom(m_document->config().zoomMode(), m_document->config().zoom() / 100.);
    connect(m_zoomController, SIGNAL(zoomChanged(KoZoomMode::Mode, qreal)), this, SLOT(zoomChanged(KoZoomMode::Mode, qreal)));

#ifdef SHOULD_BUILD_RDF
    if (KoDocumentRdf *rdf = m_document->documentRdf()) {
        connect(rdf, SIGNAL(semanticObjectViewSiteUpdated(KoRdfSemanticItem*, const QString&)),
                this, SLOT(semanticObjectViewSiteUpdated(KoRdfSemanticItem*, const QString&)));
    }
#endif
}

KWView::~KWView()
{
    m_canvas = 0;
}

KoCanvasBase *KWView::canvasBase() const
{
    return m_canvas;
}

const KWPage KWView::currentPage() const
{
    return m_currentPage;
}

QWidget *KWView::canvas() const
{
    return m_canvas;
}

void KWView::updateReadWrite(bool readWrite)
{
    m_actionFormatFrameSet->setEnabled(readWrite);
    m_actionInsertFrameBreak->setEnabled(readWrite);
    m_actionViewHeader->setEnabled(readWrite);
    m_actionViewFooter->setEnabled(readWrite);
    m_actionViewSnapToGrid->setEnabled(readWrite);
    m_actionRaiseFrame->setEnabled(readWrite);
    m_actionLowerFrame->setEnabled(readWrite);
    m_actionBringToFront->setEnabled(readWrite);
    m_actionSendBackward->setEnabled(readWrite);
    m_actionAddBookmark->setEnabled(readWrite);
    QAction *action = actionCollection()->action("insert_variable");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("select_bookmark"); // TODO fix the dialog to honor read-only instead
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("format_page");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("anchor");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("edit_cut");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("edit_copy");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("edit_paste");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("edit_paste_text");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("delete_page");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("edit_delete");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("create_linked_frame");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("create_custom_outline");
    if (action) action->setEnabled(readWrite);
}

void KWView::setupActions()
{
    m_actionFormatFrameSet  = new KAction(i18n("Shape Properties..."), this);
    actionCollection()->addAction("format_frameset", m_actionFormatFrameSet);
    m_actionFormatFrameSet->setToolTip(i18n("Change how the shape behave"));
    m_actionFormatFrameSet->setEnabled(false);
    connect(m_actionFormatFrameSet, SIGNAL(triggered()), this, SLOT(editFrameProperties()));

    m_actionInsertFrameBreak  = new KAction(QString(), this);
    actionCollection()->addAction("insert_framebreak", m_actionInsertFrameBreak);
    m_actionInsertFrameBreak->setShortcut(KShortcut(Qt::CTRL + Qt::Key_Return));
    connect(m_actionInsertFrameBreak, SIGNAL(triggered()), this, SLOT(insertFrameBreak()));
    m_actionInsertFrameBreak->setText(i18n("Page Break"));
    m_actionInsertFrameBreak->setToolTip(i18n("Force the remainder of the text into the next page"));
    m_actionInsertFrameBreak->setWhatsThis(i18n("All text after this point will be moved into the next page."));

    m_actionViewHeader = new KToggleAction(i18n("Enable Document Headers"), this);
    actionCollection()->addAction("format_header", m_actionViewHeader);
    m_actionViewHeader->setToolTip(i18n("Shows and hides header display"));
    m_actionViewHeader->setWhatsThis(i18n("Selecting this option toggles the display of headers in Words.<br/><br/>Headers are special frames at the top of each page which can contain page numbers or other information."));
    if (m_currentPage.isValid())
        m_actionViewHeader->setChecked(m_currentPage.pageStyle().headerPolicy() != Words::HFTypeNone);
    connect(m_actionViewHeader, SIGNAL(triggered()), this, SLOT(toggleHeader()));

    m_actionViewFooter = new KToggleAction(i18n("Enable Document Footers"), this);
    actionCollection()->addAction("format_footer", m_actionViewFooter);
    m_actionViewFooter->setToolTip(i18n("Shows and hides footer display"));
    m_actionViewFooter->setWhatsThis(i18n("Selecting this option toggles the display of footers in Words. <br/><br/>Footers are special shapes at the bottom of each page which can contain page numbers or other information."));
    if (m_currentPage.isValid())
        m_actionViewFooter->setChecked(m_currentPage.pageStyle().footerPolicy() != Words::HFTypeNone);
    connect(m_actionViewFooter, SIGNAL(triggered()), this, SLOT(toggleFooter()));

    m_actionViewSnapToGrid = new KToggleAction(i18n("Snap to Grid"), this);
    actionCollection()->addAction("view_snaptogrid", m_actionViewSnapToGrid);
    m_actionViewSnapToGrid->setChecked(m_snapToGrid);
    connect(m_actionViewSnapToGrid, SIGNAL(triggered()), this, SLOT(toggleSnapToGrid()));

    m_actionRaiseFrame  = new KAction(KIcon("raise"), i18n("Raise Shape"), this);
    actionCollection()->addAction("raiseframe", m_actionRaiseFrame);
    m_actionRaiseFrame->setShortcut(KShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_R));
    m_actionRaiseFrame->setToolTip(i18n("Raise the currently selected shape so that it appears above "
                                        "all the other shape"));
    m_actionRaiseFrame->setWhatsThis(i18n("Raise the currently selected shape so that it appears "
                                          "above all the other shapes. This is only useful if shapes overlap each other. If multiple "
                                          "shapes are selected they are all raised in turn."));
    connect(m_actionRaiseFrame, SIGNAL(triggered()), this, SLOT(raiseFrame()));

    m_actionLowerFrame  = new KAction(KIcon("lower"), i18n("Lower Shape"), this);
    actionCollection()->addAction("lowerframe", m_actionLowerFrame);
    m_actionLowerFrame->setShortcut(KShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_L));
    m_actionLowerFrame->setToolTip(i18n("Lower the currently selected shape so that it disappears under "
                                        "any shape that overlaps it"));
    m_actionLowerFrame->setWhatsThis(i18n("Lower the currently selected shape so that it disappears under "
                                          "any shape that overlaps it. If multiple shape are selected they are all lowered in turn."));
    connect(m_actionLowerFrame, SIGNAL(triggered()), this, SLOT(lowerFrame()));

    m_actionBringToFront = new KAction(KIcon("bring_forward"), i18n("Bring to Front"), this);
    actionCollection()->addAction("bring_tofront_frame", m_actionBringToFront);
    connect(m_actionBringToFront, SIGNAL(triggered()), this, SLOT(bringToFront()));

    m_actionSendBackward = new KAction(KIcon("send_backward"), i18n("Send to Back"), this);
    actionCollection()->addAction("send_toback_frame", m_actionSendBackward);
    connect(m_actionSendBackward, SIGNAL(triggered()), this, SLOT(sendToBack()));

    m_actionMenu = new KActionMenu(i18n("Variable"), this);
    foreach (QAction *action, m_document->inlineTextObjectManager()->createInsertVariableActions(canvasBase()))
        m_actionMenu->addAction(action);
    actionCollection()->addAction("insert_variable", m_actionMenu);
    connect(m_document->inlineTextObjectManager()->variableManager(), SIGNAL(valueChanged()), this, SLOT(variableChanged()));

#ifdef SHOULD_BUILD_RDF
    if (KoDocumentRdf* rdf = m_document->documentRdf()) {
        KAction* createRef = rdf->createInsertSemanticObjectReferenceAction(canvasBase());
        actionCollection()->addAction("insert_semanticobject_ref", createRef);
        KActionMenu *subMenu = new KActionMenu(i18n("Create"), this);
        actionCollection()->addAction("insert_semanticobject_new", subMenu);
        foreach(KAction *action, rdf->createInsertSemanticObjectNewActions(canvasBase())) {
            subMenu->addAction(action);
        }
    }
#endif

    m_actionAddBookmark = new KAction(KIcon("bookmark-new"), i18n("Bookmark..."), this);
    m_actionAddBookmark->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_G);
    actionCollection()->addAction("add_bookmark", m_actionAddBookmark);
    connect(m_actionAddBookmark, SIGNAL(triggered()), this, SLOT(addBookmark()));

    KAction *action = new KAction(i18n("Select Bookmark..."), this);
    action->setIconText(i18n("Bookmark"));
    action->setIcon(KIcon("bookmarks"));
    action->setShortcut(Qt::CTRL + Qt::Key_G);
    actionCollection()->addAction("select_bookmark", action);
    connect(action, SIGNAL(triggered()), this, SLOT(selectBookmark()));

    action = new KAction(i18n("Show Text Shape Borders"), this);
    action->setToolTip(i18n("Turns the border display on and off"));
    action->setCheckable(true);
    actionCollection()->addAction("view_frameborders", action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(toggleViewFrameBorders(bool)));
    m_canvas->resourceManager()->setResource(KoCanvasResourceManager::ShowTextShapeOutlines, QVariant(false));
    action->setChecked(m_document->config().viewFrameBorders()); // will change resource if true
    action->setWhatsThis(i18n("Turns the border display on and off.<br/><br/>The borders are never printed. This option is useful to see how the document will appear on the printed page."));

    action = new KAction(i18n("Show Formatting Characters"), this);
    action->setCheckable(true);
    actionCollection()->addAction("view_formattingchars", action);

    connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowFormattingChars(bool)));
    m_canvas->resourceManager()->setResource(KoCanvasResourceManager::ShowFormattingCharacters, QVariant(false));
    action->setChecked(m_document->config().showFormattingChars()); // will change resource if true
    action->setToolTip(i18n("Toggle the display of non-printing characters"));
    action->setWhatsThis(i18n("Toggle the display of non-printing characters.<br/><br/>When this is enabled, Words shows you tabs, spaces, carriage returns and other non-printing characters."));

    action = new KAction(i18n("Show Table Borders"), this);
    action->setCheckable(true);
    actionCollection()->addAction("view_tableborders", action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowTableBorders(bool)));
    m_canvas->resourceManager()->setResource(KoCanvasResourceManager::ShowTableBorders, QVariant(false));
    action->setChecked(m_document->config().showTableBorders()); // will change resource if true
    action->setToolTip(i18n("Toggle the display of table borders"));
    action->setWhatsThis(i18n("Toggle the display of table borders.<br/><br/>When this is enabled, Words shows you any invisible table borders with a thin gray line."));

    action = new KAction(i18n("Page Layout..."), this);
    actionCollection()->addAction("format_page", action);
    action->setToolTip(i18n("Change properties of entire page"));
    action->setWhatsThis(i18n("Change properties of the entire page.<p>Currently you can change paper size, paper orientation, header and footer sizes, and column settings.</p>"));
    connect(action, SIGNAL(triggered()), this, SLOT(formatPage()));

#ifdef SHOULD_BUILD_RDF
    action = new KAction(i18n("Semantic Stylesheets..."), this);
    actionCollection()->addAction("edit_semantic_stylesheets", action);
    action->setToolTip(i18n("Modify and add semantic stylesheets"));
    action->setWhatsThis(i18n("Stylesheets are used to format contact, event, and location information which is stored in Rdf"));
    connect(action, SIGNAL(triggered()), this, SLOT(editSemanticStylesheets()));
#endif

    KActionMenu *anchorActionMenu = new KActionMenu(i18n("Anchor"), this);
    QActionGroup *anchoringActionGroup = new QActionGroup(this);
    anchoringActionGroup->setExclusive(true);
    action = new KToggleAction(i18n("As Character"), this);
    action->setToolTip(i18n("Anchor the current shape as a character in the text"));
    anchorActionMenu->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(anchorAsChar()));
    action->setActionGroup(anchoringActionGroup);

    action = new KToggleAction(i18n("To Character"), this);
    action->setToolTip(i18n("Anchor the current shape to the character at the current position"));
    anchorActionMenu->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(anchorToChar()));
    action->setActionGroup(anchoringActionGroup);

    action = new KToggleAction(i18n("To Paragraph"), this);
    action->setToolTip(i18n("Anchor the current shape to current paragraph"));
    anchorActionMenu->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(anchorToParagraph()));
    action->setActionGroup(anchoringActionGroup);

    action = new KToggleAction(i18n("To Page"), this);
    action->setToolTip(i18n("Anchor the current shape to current page"));
    anchorActionMenu->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(anchorToPage()));
    action->setActionGroup(anchoringActionGroup);
    actionCollection()->addAction("anchor", anchorActionMenu);

    action = actionCollection()->addAction(KStandardAction::Prior,  "page_previous", this, SLOT(goToPreviousPage()));

    action = actionCollection()->addAction(KStandardAction::Next,  "page_next", this, SLOT(goToNextPage()));

    // -------------- Edit actions
    action = actionCollection()->addAction(KStandardAction::Cut,  "edit_cut", 0, 0);
    new KoCutController(canvasBase(), action);
    action = actionCollection()->addAction(KStandardAction::Copy,  "edit_copy", 0, 0);
    new KoCopyController(canvasBase(), action);
    action = actionCollection()->addAction(KStandardAction::Paste,  "edit_paste", 0, 0);
    new KoPasteController(canvasBase(), action);

    action  = new KAction(i18n("Statistics"), this);
    actionCollection()->addAction("file_statistics", action);
    action->setToolTip(i18n("Sentence, word and letter counts for this document"));
    action->setWhatsThis(i18n("Information on the number of letters, words, syllables and sentences for this document.<p>Evaluates readability using the Flesch reading score.</p>"));
    connect(action, SIGNAL(triggered()), this, SLOT(showStatisticsDialog()));

    action = new KAction(i18n("Show Rulers"), this);
    action->setCheckable(true);
    action->setToolTip(i18n("Shows or hides rulers"));
    action->setWhatsThis(i18n("The rulers are the white measuring spaces top and left of the "
                              "document. The rulers show the position and width of pages and of frames and can "
                              "be used to position tabulators among others.<p>Uncheck this to disable "
                              "the rulers from being displayed.</p>"));
    action->setChecked(m_document->config().viewRulers());
    actionCollection()->addAction("show_ruler", action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(showRulers(bool)));

    action = new KAction(i18n("Page..."), this);
    actionCollection()->addAction("insert_page", action);
    connect(action, SIGNAL(triggered()), this, SLOT(insertPage()));

    action = new KAction(i18n("Delete Page"), this);
    actionCollection()->addAction("delete_page", action);
    connect(action, SIGNAL(triggered()), this, SLOT(deletePage()));
    handleDeletePageAction(); //decide if we enable or disable this action
    connect(m_document, SIGNAL(pageSetupChanged()), this, SLOT(handleDeletePageAction()));

    action = new KAction(i18n("Select All Shapes"), this);
    actionCollection()->addAction("edit_selectallframes", action);
    connect(action, SIGNAL(triggered()), this, SLOT(editSelectAllFrames()));

    action = new KAction(KIcon("edit-delete"), i18n("Delete"), this);
    action->setShortcut(QKeySequence("Del"));
    connect(action, SIGNAL(triggered()), this, SLOT(editDeleteSelection()));
    connect(canvasBase()->toolProxy(), SIGNAL(selectionChanged(bool)), action, SLOT(setEnabled(bool)));
    actionCollection()->addAction("edit_delete", action);

    action = m_document->gridData().gridToggleAction(m_canvas);
    actionCollection()->addAction("view_grid", action);

    KToggleAction *guides = KoStandardAction::showGuides(this, SLOT(setGuideVisibility(bool)), actionCollection());
    guides->setChecked(m_document->guidesData().showGuideLines());

    // -------------- Frame menu
    action  = new KAction(i18n("Create Linked Copy"), this);
    actionCollection()->addAction("create_linked_frame", action);
    action->setToolTip(i18n("Create a copy of the current frame, always showing the same contents"));
    action->setWhatsThis(i18n("Create a copy of the current frame, that remains linked to it. This means they always show the same contents: modifying the contents in such a frame will update all its linked copies."));
    connect(action, SIGNAL(triggered()), this, SLOT(createLinkedFrame()));

    action = new KAction(i18n("Create Frame-clip"), this);
    actionCollection()->addAction("create_clipped_frame", action);
    connect(action, SIGNAL(triggered()), this, SLOT(createFrameClipping()));

    action = new KAction(i18n("Remove Frame-clip"), this);
    actionCollection()->addAction("remove_clipped_frame", action);
    connect(action, SIGNAL(triggered()), this, SLOT(removeFrameClipping()));

    KToggleAction *tAction = new KToggleAction(i18n("Show Status Bar"), this);
    tAction->setToolTip(i18n("Shows or hides the status bar"));
    actionCollection()->addAction("showStatusBar", tAction);
    connect(tAction, SIGNAL(toggled(bool)), this, SLOT(showStatusBar(bool)));

    // -------------- Insert menu
    /* ********** From old kwview ****
    We probably want to have each of these again, so just move them when you want to implement it
    This saves problems with finding out which we missed near the end.

        // -------------- File menu
        m_actionExtraCreateTemplate = new KAction(i18n("Create Template From Document..."), 0,
        this, SLOT(extraCreateTemplate()),
        actionCollection(), "extra_template");
        m_actionExtraCreateTemplate->setToolTip(i18n("Save this document and use it later as a template"));
        m_actionExtraCreateTemplate->setWhatsThis(i18n("You can save this document as a template.<br><br>You can use this new template as a starting point for another document."));

        (void) new KAction(i18n("Configure Mail Merge..."), "configure",0,
        this, SLOT(editMailMergeDataBase()),
        actionCollection(), "edit_sldatabase");


        QAction *mailMergeLabelAction = new KWMailMergeLabelAction::KWMailMergeLabelAction(
        i18n("Drag Mail Merge Variable"), actionCollection(), "mailmerge_draglabel");
        connect(mailMergeLabelAction, SIGNAL(triggered(bool)), this, SLOT(editMailMergeDataBase()));

        //    (void) new KWMailMergeComboAction::KWMailMergeComboAction(i18n("Insert Mailmerge Var"),0,this,SLOT(JWJWJW()),actionCollection(),"mailmerge_varchooser");

        // -------------- View menu

        if (!m_doc->isEmbedded()) {

            QActionGroup *viewModeActionGroup = new QActionGroup(this);
            viewModeActionGroup->setExclusive(true);
            m_actionViewTextMode = new KToggleAction(i18n("Text Mode"), 0,
                    this, SLOT(viewTextMode()),
                    actionCollection(), "view_textmode");
            m_actionViewTextMode->setToolTip(i18n("Only show the text of the document"));
            m_actionViewTextMode->setWhatsThis(i18n("Do not show any pictures, formatting or layout. Words will display only the text for editing."));

            m_actionViewTextMode->setActionGroup(viewModeActionGroup);
            m_actionViewPageMode = new KToggleAction(i18n("Page Mode"), 0,
                    this, SLOT(viewPageMode()),
                    actionCollection(), "view_pagemode");
            m_actionViewPageMode->setWhatsThis(i18n("Switch to page mode.<br><br> Page mode is designed to make editing your text easy.<br><br>This function is most frequently used to return to text editing after switching to preview mode."));
            m_actionViewPageMode->setToolTip(i18n("Switch to page editing mode"));

            m_actionViewPageMode->setActionGroup(viewModeActionGroup);
            m_actionViewPageMode->setChecked(true);
            m_actionViewPreviewMode = new KToggleAction(i18n("Preview Mode"), 0,
                    this, SLOT(viewPreviewMode()),
                    actionCollection(), "view_previewmode");
            m_actionViewPreviewMode->setWhatsThis(i18n("Zoom out from your document to get a look at several pages of your document.<br><br>The number of pages per line can be customized."));
            m_actionViewPreviewMode->setToolTip(i18n("Zoom out to a multiple page view"));

            m_actionViewPreviewMode->setActionGroup(viewModeActionGroup);
        }
        else // no viewmode switching when embedded; at least "Page" makes no sense
        {
            m_actionViewTextMode = 0;
            m_actionViewPageMode = 0;
            m_actionViewPreviewMode = 0;
        }

        m_actionEditCustomVarsEdit = new KAction(i18n("Custom Variables..."), 0,
                this, SLOT(editCustomVars()), // TODO: new dialog w add etc.
                actionCollection(), "custom_vars");

        m_actionEditPersonnalExpr=new KAction(i18n("Edit Personal Expressions..."), 0,
                this, SLOT(editPersonalExpr()),
                actionCollection(), "personal_expr");
        m_actionEditPersonnalExpr->setToolTip(i18n("Add or change one or more personal expressions"));
        m_actionEditPersonnalExpr->setWhatsThis(i18n("Add or change one or more personal expressions.<p>Personal expressions are a way to quickly insert commonly used phrases or text into your document."));


        m_actionConfigureHeaderFooter=new KAction(i18n("Configure Header/Footer..."), 0,
                this, SLOT(configureHeaderFooter()),
                actionCollection(), "configure_headerfooter");
        m_actionConfigureHeaderFooter->setToolTip(i18n("Configure the currently selected header or footer"));
        m_actionConfigureHeaderFooter->setWhatsThis(i18n("Configure the currently selected header or footer."));

        m_actionAddLinkToBookmak = new KAction(i18n("Add to Bookmark"), 0,
                this, SLOT(addToBookmark()),
                actionCollection(), "add_to_bookmark");

        m_actionConfigureCompletion = new KAction(i18n("Configure Completion..."), 0,
                this, SLOT(configureCompletion()),
                actionCollection(), "configure_completion");
        m_actionConfigureCompletion->setToolTip(i18n("Change the words and options for autocompletion"));
        m_actionConfigureCompletion->setWhatsThis(i18n("Add words or change the options for autocompletion."));

        new KAction(i18n("Completion"), KStdAccel::shortcut(KStdAccel::TextCompletion), this, SLOT(slotCompletion()), actionCollection(), "completion");

        // --------
        m_actionEditCustomVars = new KAction(i18n("Edit Variable..."), 0,
                this, SLOT(editCustomVariable()),
                actionCollection(), "edit_customvars");

        m_actionCreateStyleFromSelection = new KAction(i18n("Create Style From Selection..."), 0,
                this, SLOT(createStyleFromSelection()),
                actionCollection(), "create_style");
        m_actionCreateStyleFromSelection->setToolTip(i18n("Create a new style based on the currently selected text"));
        m_actionCreateStyleFromSelection->setWhatsThis(i18n("Create a new style based on the currently selected text.")); // ## "on the current paragraph, taking the formatting from where the cursor is. Selecting text isn't even needed."

        m_actionSavePicture= new KAction(i18n("Save Picture As..."), 0,
                this, SLOT(savePicture()),
                actionCollection(), "save_picture");
        m_actionSavePicture->setToolTip(i18n("Save the picture in a separate file"));
        m_actionSavePicture->setWhatsThis(i18n("Save the picture in the currently selected frame in a separate file, outside the Words document."));

        m_actionAddBookmark= new KAction(i18n("Bookmark..."), 0,
                this, SLOT(addBookmark()),
                actionCollection(), "add_bookmark");
        m_actionSelectBookmark= new KAction(i18n("Select Bookmark..."), 0,
                this, SLOT(selectBookmark()),
                actionCollection(), "select_bookmark");

        m_actionImportStyle= new KAction(i18n("Import Styles..."), 0,
                this, SLOT(importStyle()),
                actionCollection(), "import_style");

        m_actionCreateFrameStyle = new KAction(i18n("Create Framestyle From Frame..."), 0,
                this, SLOT(createFrameStyle()),
                actionCollection(), "create_framestyle");
        m_actionCreateFrameStyle->setToolTip(i18n("Create a new style based on the currently selected frame"));
        m_actionCreateFrameStyle->setWhatsThis(i18n("Create a new framestyle based on the currently selected frame."));
    */
}

QList<KWFrame*> KWView::selectedFrames() const
{
    QList<KWFrame*> frames;
    foreach (KoShape *shape, canvasBase()->shapeManager()->selection()->selectedShapes()) {
        KWFrame *frame = kwdocument()->getFrameOfShape(shape);
        Q_ASSERT(frame);
        frames.append(frame);
    }
    return frames;
}

KoShape* KWView::getSelectedShape() const
{
    KoSelection *selection = canvasBase()->shapeManager()->selection();
    
    foreach (KoShape *s, selection->selectedShapes(KoFlake::TopLevelSelection)) {
        if (s->isGeometryProtected())
            continue;
        return s;
    }

    return 0;
}

// -------------------- Actions -----------------------

void KWView::editFrameProperties()
{
    KWFrameDialog *frameDialog = new KWFrameDialog(selectedFrames(), m_document, this);
    frameDialog->exec();
    delete frameDialog;
}

KoPrintJob *KWView::createPrintJob()
{
    KWPrintingDialog *dia = new KWPrintingDialog(m_document, m_canvas->shapeManager(), this);
    dia->printer().setResolution(600);
    dia->printer().setCreator(QString("Words %1.%2.%3").arg(Calligra::versionMajor())
                              .arg(Calligra::versionMinor()).arg(Calligra::versionRelease()));
    dia->printer().setFullPage(true); // ignore printer margins
    return dia;
}

void KWView::insertFrameBreak()
{
    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(canvasBase());
    if (editor) {
        // this means we have the text tool selected right now.
        editor->insertFrameBreak();
    } else if (m_document->mainFrameSet()) { // lets just add one to the main text frameset
        KoTextDocument doc(m_document->mainFrameSet()->document());
        doc.textEditor()->insertFrameBreak();
    }
}

void KWView::addBookmark()
{
    QString name, suggestedName;

    KoSelection *selection = canvasBase()->shapeManager()->selection();
    KoShape *shape = 0;
    shape = selection->firstSelectedShape();
    if (shape == 0) return; // no shape selected

    KWFrame *frame = kwdocument()->getFrameOfShape(shape);
    Q_ASSERT(frame);
    KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
    if (fs == 0) return;

    QString tool = KoToolManager::instance()->preferredToolForSelection(selection->selectedShapes());
    KoToolManager::instance()->switchToolRequested(tool);
    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(canvasBase());
    Q_ASSERT(editor);

    KoBookmarkManager *manager = m_document->inlineTextObjectManager()->bookmarkManager();
    if (editor->hasSelection()) {
        suggestedName = editor->selectedText();
    }

    KWCreateBookmarkDialog *dia = new KWCreateBookmarkDialog(manager->bookmarkNameList(), suggestedName, m_canvas->canvasWidget());
    if (dia->exec() == QDialog::Accepted) {
        name = dia->newBookmarkName();
    }
    else {
        delete dia;
        return;
    }
    delete dia;

    editor->addBookmark(name);
}

void KWView::selectBookmark()
{
    QString name;
    KoBookmarkManager *manager = m_document->inlineTextObjectManager()->bookmarkManager();

    KWSelectBookmarkDialog *dia = new KWSelectBookmarkDialog(manager->bookmarkNameList(), m_canvas->canvasWidget());
    connect(dia, SIGNAL(nameChanged(const QString &, const QString &)),
            manager, SLOT(rename(const QString &, const QString &)));
    connect(dia, SIGNAL(bookmarkDeleted(const QString &)),
            this, SLOT(deleteBookmark(const QString &)));
    if (dia->exec() == QDialog::Accepted)
        name = dia->selectedBookmarkName();
    else {
        delete dia;
        return;
    }
    delete dia;
#if 0
    KoBookmark *bookmark = manager->retrieveBookmark(name);
    KoShape *shape = bookmark->shape();
    KoSelection *selection = canvasBase()->shapeManager()->selection();
    selection->deselectAll();
    selection->select(shape);

    QString tool = KoToolManager::instance()->preferredToolForSelection(selection->selectedShapes());
    KoToolManager::instance()->switchToolRequested(tool);

    KoCanvasResourceManager *rm = m_canvas->resourceManager();
    if (bookmark->hasSelection()) {
        rm->setResource(KoText::CurrentTextPosition, bookmark->position());
        rm->setResource(KoText::CurrentTextAnchor, bookmark->endBookmark()->position() + 1);
        rm->clearResource(KoText::SelectedTextPosition);
        rm->clearResource(KoText::SelectedTextAnchor);
    } else
        rm->setResource(KoText::CurrentTextPosition, bookmark->position() + 1);
#else
#ifdef __GNUC__
    #warning FIXME: port to textlayout-rework
#endif
#endif
}

void KWView::deleteBookmark(const QString &name)
{
    Q_UNUSED(name);
#if 0
    KoInlineTextObjectManager*manager = m_document->inlineTextObjectManager();
    KoBookmark *bookmark = manager->bookmarkManager()->retrieveBookmark(name);
    if (!bookmark || !bookmark->shape())
        return;

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(bookmark->shape()->userData());
    if (!data)
        return;
    QTextCursor cursor(data->document());
    if (bookmark->hasSelection()) {
        cursor.setPosition(bookmark->endBookmark()->position() - 1);
        manager->removeInlineObject(cursor);
    }
    cursor.setPosition(bookmark->position());
    manager->removeInlineObject(cursor);
#else
#ifdef __GNUC__
    #warning FIXME: port to textlayout-rework
#endif
#endif
}

void KWView::editDeleteFrame()
{
    QList<KoShape*> frames;
    foreach (KoShape *shape, canvasBase()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection)) {
        KWFrame *frame = kwdocument()->getFrameOfShape(shape);
        if (frame) {
            KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
            if (fs && fs->textFrameSetType() != Words::OtherTextFrameSet)
                continue; // can't delete auto-generated frames
        }
        frames.append(shape);
    }
    KUndo2Command *cmd = canvasBase()->shapeController()->removeShapes(frames);
    m_document->addCommand(cmd);
}

void KWView::toggleHeader()
{
    if (!m_currentPage.isValid())
        return;
    Q_ASSERT(m_currentPage.pageStyle().isValid());
    m_currentPage.pageStyle().setHeaderPolicy(m_actionViewHeader->isChecked() ? Words::HFTypeEvenOdd : Words::HFTypeNone);
    m_document->relayout();
}

void KWView::toggleFooter()
{
    if (!m_currentPage.isValid())
        return;
    Q_ASSERT(m_currentPage.pageStyle().isValid());
    m_currentPage.pageStyle().setFooterPolicy(m_actionViewFooter->isChecked() ? Words::HFTypeEvenOdd : Words::HFTypeNone);
    m_document->relayout();
}

void KWView::toggleSnapToGrid()
{
    m_snapToGrid = !m_snapToGrid;
    m_document->gridData().setSnapToGrid(m_snapToGrid); // for persistency
}


void KWView::toggleViewFrameBorders(bool on)
{
    m_canvas->resourceManager()->setResource(KoCanvasResourceManager::ShowTextShapeOutlines, on);
    m_canvas->update();
    m_document->config().setViewFrameBorders(on);
}

void KWView::setShowFormattingChars(bool on)
{
    m_canvas->resourceManager()->setResource(KoCanvasResourceManager::ShowFormattingCharacters, QVariant(on));
    m_canvas->update();
    m_document->config().setShowFormattingChars(on);
}

void KWView::setShowTableBorders(bool on)
{
    m_canvas->resourceManager()->setResource(KoCanvasResourceManager::ShowTableBorders, QVariant(on));
    m_canvas->update();
    m_document->config().setShowTableBorders(on);
}

void KWView::formatPage()
{
    if (! m_currentPage.isValid())
        return;
    KWPageSettingsDialog *dia = new KWPageSettingsDialog(this, m_document, m_currentPage);
    if (m_document->mainFrameSet()) {
        KoTextDocument doc(m_document->mainFrameSet()->document());
        KoTextEditor *editor = doc.textEditor();
        if (editor)
            dia->showTextDirection(editor->isBidiDocument());
    }
    if (!m_lastPageSettingsTab.isEmpty()) {
        KPageWidgetItem *item = dia->pageItem(m_lastPageSettingsTab);
        if (item)
            dia->setCurrentPage(item);
    }
    connect(dia, SIGNAL(finished()), this, SLOT(pageSettingsDialogFinished()));
    dia->show();
}

void KWView::pageSettingsDialogFinished()
{
    KWPageSettingsDialog *dia = qobject_cast<KWPageSettingsDialog*>(QObject::sender());
    m_lastPageSettingsTab = dia && dia->currentPage() ? dia->currentPage()->name() : QString();
}

void KWView::editSemanticStylesheets()
{
#ifdef SHOULD_BUILD_RDF
    if (KoDocumentRdf *rdf = m_document->documentRdf()) {
        KoSemanticStylesheetsEditor *dia = new KoSemanticStylesheetsEditor(this, rdf);
        dia->show();
        // TODO this leaks memory
    }
#endif
}

void KWView::anchorAsChar()
{
    KoShape *shape = getSelectedShape();

    if (shape) {
        KoTextAnchor *anchor = kwdocument()->getAnchorOfShape(shape, true);

        if (anchor) {
            anchor->setAnchorType(KoTextAnchor::AnchorAsCharacter);
            anchor->shape()->notifyChanged();
        }
    }
}

void KWView::anchorToChar()
{
    KoShape *shape = getSelectedShape();
    
    if (shape) {
        KoTextAnchor *anchor = kwdocument()->getAnchorOfShape(shape, true);
        
        if (anchor) {
            anchor->setAnchorType(KoTextAnchor::AnchorToCharacter);
            anchor->shape()->notifyChanged();
        }
    }
}

void KWView::anchorToParagraph()
{
    KoShape *shape = getSelectedShape();
    
    if (shape) {
        KoTextAnchor *anchor = kwdocument()->getAnchorOfShape(shape, true);
        
        if (anchor) {
            anchor->setAnchorType(KoTextAnchor::AnchorParagraph);
            anchor->shape()->notifyChanged();
        }
    }
}

void KWView::anchorToPage()
{
    KoShape *shape = getSelectedShape();
    
    if (shape) {
        KoTextAnchor *anchor = kwdocument()->getAnchorOfShape(shape, false);
        
        if (anchor) {
            m_document->inlineTextObjectManager()->removeInlineObject(anchor);
            anchor->setAnchorType(KoTextAnchor::AnchorPage);
            anchor->shape()->notifyChanged();
        }
    }
}

void KWView::showStatisticsDialog()
{
    KWStatisticsDialog *dia = new KWStatisticsDialog(this);
    dia->exec();
    delete dia;
}

void KWView::showRulers(bool visible)
{
    m_document->config().setViewRulers(visible);
    m_gui->updateRulers();
}

void KWView::createLinkedFrame()
{
    KoSelection *selection = canvasBase()->shapeManager()->selection();
    QList<KoShape*> oldSelection = selection->selectedShapes(KoFlake::TopLevelSelection);
    if (oldSelection.count() == 0)
        return;
    selection->deselectAll();

    KUndo2Command *cmd = new KUndo2Command(i18nc("(qtundo-format)", "Create Linked Copy"));
    foreach (KoShape *shape, oldSelection) {
        KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
        Q_ASSERT(frame);
        KWCopyShape *copy = new KWCopyShape(frame->shape(), m_document->pageManager());
        copy->setPosition(frame->shape()->position());
        QPointF offset(40, 40);
        canvasBase()->clipToDocument(copy, offset);
        copy->setPosition(frame->shape()->position() + offset);
        copy->setZIndex(frame->shape()->zIndex() + 1);
        KWFrame *newFrame = new KWFrame(copy, frame->frameSet());
        new KWFrameCreateCommand(m_document, newFrame, cmd);
        selection->select(copy);
    }
    m_document->addCommand(cmd);
}

void KWView::showStatusBar(bool toggled)
{
    if (statusBar()) statusBar()->setVisible(toggled);
}

void KWView::deletePage()
{
    Q_ASSERT(m_currentPage.isValid() && !m_currentPage.isAutoGenerated());
    m_document->removePage(m_currentPage.pageNumber());
}

void KWView::insertPage()
{
    KWInsertPageDialog *diag = new KWInsertPageDialog(m_document, this);
    connect(diag, SIGNAL(finished(int)), diag, SLOT(deleteLater()));
    diag->show();
}

void KWView::handleDeletePageAction()
{
    Q_ASSERT(m_document);

    QAction *action = actionCollection()->action("delete_page");
    if (action) {
        action->setEnabled(m_document->pageCount() > 1
                && m_currentPage.isValid() && !m_currentPage.isAutoGenerated());
    }
}

void KWView::editSelectAllFrames()
{
    KoSelection *selection = canvasBase()->shapeManager()->selection();
    foreach (KWFrameSet *fs, m_document->frameSets()) {
        foreach (KWFrame *frame, fs->frames()) {
            if (frame->shape()->isVisible())
                selection->select(frame->shape());
        }
    }
}

void KWView::editDeleteSelection()
{
    canvasBase()->toolProxy()->deleteSelection();
}

void KWView::createFrameClipping()
{
    QSet<KWFrame *> clipFrames;
    foreach (KoShape *shape, canvasBase()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection)) {
        KWFrame *frame = kwdocument()->getFrameOfShape(shape);
        Q_ASSERT(frame);
        if (frame->shape()->parent() == 0)
            clipFrames << frame;
    }
    if (!clipFrames.isEmpty()) {
        KWClipFrameCommand *cmd = new KWClipFrameCommand(clipFrames.toList(), m_document);
        m_document->addCommand(cmd);
    }
}

void KWView::removeFrameClipping()
{
    QSet<KWFrame *> unClipFrames;
    foreach (KoShape *shape, canvasBase()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection)) {
        KWFrame *frame = kwdocument()->getFrameOfShape(shape);
        Q_ASSERT(frame);
        if (frame->shape()->parent())
            unClipFrames << frame;
    }
    if (!unClipFrames.isEmpty()) {
        KWRemoveFrameClipCommand *cmd = new KWRemoveFrameClipCommand(unClipFrames.toList(), m_document);
        m_document->addCommand(cmd);
    }
}

void KWView::setGuideVisibility(bool on)
{
    m_document->guidesData().setShowGuideLines(on);
    m_canvas->update();
}

// end of actions

void KWView::popupContextMenu(const QPoint &globalPosition, const QList<QAction*> &actions)
{
    unplugActionList("frameset_type_action");
    plugActionList("frameset_type_action", actions);
    if (factory() == 0) // we are a kpart, the factory is only set on the active component.
        return;
    QMenu *menu = dynamic_cast<QMenu*>(factory()->container("frame_popup", this));
    if (menu)
        menu->exec(globalPosition);
}

void KWView::zoomChanged(KoZoomMode::Mode mode, qreal zoom)
{
    m_document->config().setZoom(qRound(zoom * 100.0));
    m_document->config().setZoomMode(mode);
    m_canvas->update();
}

void KWView::selectionChanged()
{
    KoShape *shape = canvasBase()->shapeManager()->selection()-> firstSelectedShape();
    m_actionFormatFrameSet->setEnabled(shape != 0);
    m_actionAddBookmark->setEnabled(shape != 0);
    if (shape) {
        setCurrentPage(m_document->pageManager()->page(shape));
        KWFrame *frame = kwdocument()->getFrameOfShape(shape);
        KWTextFrameSet *fs = frame == 0 ? 0 : dynamic_cast<KWTextFrameSet*>(frame->frameSet());
        if (fs)
            m_actionAddBookmark->setEnabled(true);
        else
            m_actionAddBookmark->setEnabled(false);
    }
    // actions that need at least one shape selected
    QAction *action = actionCollection()->action("create_linked_frame");
    if (action) action->setEnabled(shape);
    action = actionCollection()->action("anchor");
    if (action) action->setEnabled(shape && kwdocument()->mainFrameSet());

    foreach (KoShape *shape, canvasBase()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection)) {
        KWFrame *frame = kwdocument()->getFrameOfShape(shape);
        Q_ASSERT(frame);
        QVariant variant;
        variant.setValue<void*>(frame);
        m_canvas->resourceManager()->setResource(Words::CurrentFrame, variant);
        variant.setValue<void*>(frame->frameSet());
        m_canvas->resourceManager()->setResource(Words::CurrentFrameSet, variant);
        break;
    }
}

void KWView::setCurrentPage(const KWPage &currentPage)
{
    Q_ASSERT(currentPage.isValid());
    if (currentPage != m_currentPage) {
        m_currentPage = currentPage;
        m_canvas->resourceManager()->setResource(KoCanvasResourceManager::CurrentPage, m_currentPage.pageNumber());

        QSizeF newPageSize = m_currentPage.rect().size();
        QSizeF newMaxPageSize = QSize(qMax(m_maxPageSize.width(), newPageSize.width()),
                                     qMax(m_maxPageSize.height(), newPageSize.height()));
        if (newMaxPageSize != m_maxPageSize) {
            m_maxPageSize = newMaxPageSize;
            m_zoomController->setPageSize(m_maxPageSize);
        }

        m_actionViewHeader->setChecked(m_currentPage.pageStyle().headerPolicy() != Words::HFTypeNone);
        m_actionViewFooter->setChecked(m_currentPage.pageStyle().footerPolicy() != Words::HFTypeNone);
    }
}

void KWView::goToPreviousPage(Qt::KeyboardModifiers modifiers)
{
    // Scroll display
    qreal moveDistance = m_canvas->canvasController()->visibleHeight() * 0.8;
    m_canvas->canvasController()->pan(QPoint(0, -moveDistance));

    // Find current frameset, FIXME for now assume main
    KWTextFrameSet *currentFrameSet = kwdocument()->mainFrameSet();

    // Since we move _up_ calculate the position where a frame would _start_ if
    // we were scrolled to the _first_ page
    QPointF pos = currentFrameSet->frames().first()->shape()->absoluteTransformation(0).map(QPointF(0, 5));

    pos += m_canvas->viewMode()->viewToDocument(m_canvas->documentOffset(), viewConverter());

    // Find textshape under that position and from current frameset
    QList<KoShape*> possibleTextShapes = m_canvas->shapeManager()->shapesAt(QRectF(pos.x() - 20, pos.y() -20, 40, 40));
    KoTextShapeData *textShapeData = 0;
    foreach (KoShape* shape, possibleTextShapes) {
        KoShapeUserData *userData = shape->userData();
        if ((textShapeData = dynamic_cast<KoTextShapeData*>(userData))) {
            foreach (KWFrame *frame, currentFrameSet->frames()) {
                if (frame->shape() == shape) {
                    pos = shape->absoluteTransformation(0).inverted().map(pos);
                     pos += QPointF(0.0, textShapeData->documentOffset());

                    int cursorPos = textShapeData->rootArea()->hitTest(pos, Qt::FuzzyHit).position;
                    KoTextDocument(textShapeData->document()).textEditor()->setPosition(cursorPos, (modifiers & Qt::ShiftModifier) ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
                    return;
                }
            }
        }
    }
}

void KWView::goToNextPage(Qt::KeyboardModifiers modifiers)
{
    // Scroll display
    qreal moveDistance = m_canvas->canvasController()->visibleHeight() * 0.8;
    m_canvas->canvasController()->pan(QPoint(0, moveDistance));

    // Find current frameset, FIXME for now assume main
    KWTextFrameSet *currentFrameSet = kwdocument()->mainFrameSet();

    // Since we move _down_ calculate the position where a frame would _end_ if
    // we were scrolled to the _lasst_ page
    KoShape *shape = currentFrameSet->frames().last()->shape();
    QPointF pos = shape->absoluteTransformation(0).map(QPointF(0, shape->size().height() - 5));
    pos.setY(pos.y() - m_document->pageManager()->page(qreal(pos.y())).rect().bottom());

    pos += m_canvas->viewMode()->viewToDocument(m_canvas->documentOffset() + QPointF(0, m_canvas->canvasController()->visibleHeight()), viewConverter());

    // Find textshape under that position and from current frameset
    QList<KoShape*> possibleTextShapes = m_canvas->shapeManager()->shapesAt(QRectF(pos.x() - 20, pos.y() -20, 40, 40));
    KoTextShapeData *textShapeData = 0;
    foreach (KoShape* shape, possibleTextShapes) {
        KoShapeUserData *userData = shape->userData();
        if ((textShapeData = dynamic_cast<KoTextShapeData*>(userData))) {
            foreach (KWFrame *frame, currentFrameSet->frames()) {
                if (frame->shape() == shape) {
                    pos = shape->absoluteTransformation(0).inverted().map(pos);
                    pos += QPointF(0.0, textShapeData->documentOffset());

                    int cursorPos = textShapeData->rootArea()->hitTest(pos, Qt::FuzzyHit).position;
                    KoTextDocument(textShapeData->document()).textEditor()->setPosition(cursorPos, (modifiers & Qt::ShiftModifier) ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
                    return;
                }
            }
        }
    }
}

void KWView::goToPage(const KWPage &page)
{
    KoCanvasController *controller = m_gui->canvasController();
    QPoint origPos = controller->scrollBarValue();
    QPointF pos = m_canvas->viewMode()->documentToView(QPointF(0,
                            page.offsetInDocument()), m_canvas->viewConverter());
    origPos.setY((int)pos.y());
    controller->setScrollBarValue(origPos);
}

void KWView::showEvent(QShowEvent *e)
{
    KoView::showEvent(e);
    QTimer::singleShot(0, this, SLOT(updateStatusBarAction()));
}

void KWView::updateStatusBarAction()
{
    KToggleAction *action = (KToggleAction*) actionCollection()->action("showStatusBar");
    if (action && statusBar())
        action->setChecked(! statusBar()->isHidden());
}

void KWView::offsetInDocumentMoved(int yOffset)
{
    const qreal offset = -m_zoomHandler.viewToDocumentY(yOffset);
    const qreal height = m_zoomHandler.viewToDocumentY(m_gui->viewportSize().height());
    if (m_currentPage.isValid()) { // most of the time the current will not change.
        const qreal pageTop = m_currentPage.offsetInDocument();
        const qreal pageBottom = pageTop + m_currentPage.height();
        const qreal visibleArea = qMin(offset + height, pageBottom) - qMax(pageTop, offset);
        if (visibleArea / height >= 0.45) // current page is just fine if > 45% is shown
            return;

        // using 'next'/'prev' is much cheaper than using a documentOffset, so try that first.
        if (pageTop > offset && pageTop < offset + height) { // check if the page above is a candidate.
            KWPage page = m_currentPage.previous();
            if (page.isValid() && pageTop - offset > visibleArea) {
                setCurrentPage(page);
                return;
            }
        }
        if (pageBottom > offset && pageBottom < offset + height) { // check if the page above is a candidate.
            KWPage page = m_currentPage.next();
            if (page.isValid() && m_currentPage.height() - height > visibleArea) {
                setCurrentPage(page);
                return;
            }
        }
    }
    KWPage page = m_document->pageManager()->page(qreal(offset + height / 2.0));
    if (page.isValid())
        setCurrentPage(page);
}

void KWView::semanticObjectViewSiteUpdated(KoRdfSemanticItem* item, const QString &xmlid)
{
#ifdef SHOULD_BUILD_RDF
    kDebug(30015) << "xmlid:" << xmlid << " reflow item:" << item->name();
    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(canvasBase());
    if (!editor) {
        kDebug(30015) << "no editor, not reflowing rdf semantic item.";
        return;
    }
    kDebug(30015) << "reflowing rdf semantic item.";
    KoRdfSemanticItemViewSite vs(item, xmlid);
    vs.reflowUsingCurrentStylesheet(editor);
#endif
}

void KWView::variableChanged(){
    m_actionMenu->menu()->clear();
    foreach (QAction *action, m_document->inlineTextObjectManager()->createInsertVariableActions(canvasBase()))
        m_actionMenu->addAction(action);
}

void adjustZOrderOfSelectedFrames(KoCanvasBase *canvasBase, KWDocument *document, KoShapeReorderCommand::MoveShapeType direction)
{
    // TODO we should not allow any shapes to fall behind the main text frame.
    KUndo2Command *cmd = KoShapeReorderCommand::createCommand(canvasBase->shapeManager()->selection()->selectedShapes(),
                        canvasBase->shapeManager(), direction);
    if (cmd)
        document->addCommand(cmd);
}


void KWView::raiseFrame() {
    adjustZOrderOfSelectedFrames(canvasBase(), m_document, KoShapeReorderCommand::RaiseShape);
}

void KWView::lowerFrame() {
    adjustZOrderOfSelectedFrames(canvasBase(), m_document, KoShapeReorderCommand::LowerShape);
}

void KWView::bringToFront() {
    adjustZOrderOfSelectedFrames(canvasBase(), m_document, KoShapeReorderCommand::BringToFront);
}

void KWView::sendToBack() {
    adjustZOrderOfSelectedFrames(canvasBase(), m_document, KoShapeReorderCommand::SendToBack);
}

void KWView::findMatchFound(KoFindMatch match)
{
    if(!match.isValid() || !match.location().canConvert<QTextCursor>() || !match.container().canConvert<QTextDocument*>()) {
        return;
    }

    QTextCursor cursor = match.location().value<QTextCursor>();

    m_canvas->resourceManager()->setResource(KoText::CurrentTextAnchor, cursor.anchor());
    m_canvas->resourceManager()->setResource(KoText::CurrentTextPosition, cursor.position());
}

void KWView::loadingCompleted()
{
    QList<QTextDocument*> texts;
    KoFindText::findTextInShapes(m_canvas->shapeManager()->shapes(), texts);
    m_find->addDocuments(texts);
}

void KWView::addImages(const QList<QImage> &imageList, const QPoint &insertAt)
{
    if (!m_canvas) {
        // now canvas because we're not on the desktop?
        return;
    }
    // get position from event and convert to document coordinates
    QPointF pos = m_canvas->viewConverter()->viewToDocument(insertAt)
            + m_canvas->documentOffset()- m_canvas->documentOrigin();

    // create a factory
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value("PictureShape");
    if (!factory) {
        kWarning(30003) << "No picture shape found, cannot drop images.";
        return;
    }

    // get the textshape at this point
    QList<KoShape*> possibleTextShapes = canvasBase()->shapeManager()->shapesAt(QRectF(pos.x() - 10, pos.y() -10, 20, 20));
    KoTextShapeData *textShapeData = 0;
    foreach (KoShape* shape, possibleTextShapes) {
        KoShapeUserData *userData = shape->userData();
        if ((textShapeData = dynamic_cast<KoTextShapeData*>(userData))) {
            // We've found the top-level text shape.
            break;
        }
    }

    KDialog dlg;
    dlg.setCaption(i18n("Insert Image Options"));
    QWidget *page = new QWidget(&dlg);
    dlg.setMainWidget(page);
    Ui_KWInsertImage uiInsertImage;
    uiInsertImage.setupUi(page);

    if (textShapeData) {
        // ask the user what kind of anchoring and run-around to use
        if (dlg.exec() == QDialog::Rejected) {
            return;
        }
    }
    else {
        // ask the user what kind of run-around to use
        uiInsertImage.grpAnchor->setVisible(false);
        uiInsertImage.grpHAlign->setVisible(false);
        uiInsertImage.grpVAlign->setVisible(false);

        if (dlg.exec() == KDialog::QDialog::Rejected) {
            return;
        }
    }

    foreach(const QImage image, imageList) {

        KoProperties params;
        QVariant v;
        v.setValue<QImage>(image);
        params.setProperty("qimage", v);

        KoShape *shape = factory->createShape(&params, kwdocument()->resourceManager());

        // resize the shape so it will fit in the document, with some nice
        // hard-coded constants.
        qreal pageWidth = currentPage().width();
        qreal pageHeight = currentPage().height();
        if (shape->size().width() > pageWidth * 0.8 ||
                shape->size().height() > pageHeight)
        {
            QSizeF sz = shape->size();
            sz.scale(QSizeF(pageWidth * 0.6, pageHeight *.6), Qt::KeepAspectRatio);
            shape->setSize(sz);
        }

        if (!shape) {
            kWarning(30003) << "Could not create a shape from the image";
            return;
        }

        // Set the wraparound
        if (uiInsertImage.noRunaround->isChecked()) {
            shape->setTextRunAroundSide(KoShape::NoRunAround);
        }
        else if (uiInsertImage.left->isChecked()) {
            shape->setTextRunAroundSide(KoShape::LeftRunAroundSide);
        }
        else if (uiInsertImage.right->isChecked()) {
            shape->setTextRunAroundSide(KoShape::RightRunAroundSide);
        }
        else if (uiInsertImage.longest->isChecked()) {
            shape->setTextRunAroundSide(KoShape::BiggestRunAroundSide);
        }
        else if (uiInsertImage.both->isChecked()) {
            shape->setTextRunAroundSide(KoShape::BothRunAroundSide);
        }
        else if (uiInsertImage.enough->isChecked()) {
            shape->setTextRunAroundSide(KoShape::EnoughRunAroundSide);
            double threshold = uiInsertImage.threshold->value();
            shape->setTextRunAroundThreshold(threshold);
        }

        KoTextAnchor *anchor   = 0;
        double        distance = uiInsertImage.distance->value();
        shape->setTextRunAroundDistance(distance);

        // only if we have a text shape, we will anchor to the text inside.
        if (textShapeData) {

            // Create the anchor
            QTextDocument *qdoc = textShapeData->document();
            anchor = new KoTextAnchor(shape);

            // anchor
            // XXX: What about: HFrame, HFrameContent, HFrameEndMargin, HFrameStartMargin?
            anchor->setAnchorType(KoTextAnchor::AnchorToCharacter);
            if (uiInsertImage.rAnchorPage->isChecked()) {
                // XXX: or: VPageContent?
                anchor->setVerticalRel(KoTextAnchor::VPage);
                // XXX: or: HPageContent or HPageStartMargin or HPageEndMargin?
                anchor->setHorizontalRel(KoTextAnchor::HPage);

            }
            else if (uiInsertImage.rAnchorParagraph->isChecked()) {
                // XXX: or: VParagraphContent?
                anchor->setVerticalRel(KoTextAnchor::VParagraph);
                // XXX: or HParagraphContent, HParagraphEndMargin, HParagraphStartMargin?
                anchor->setHorizontalRel(KoTextAnchor::HParagraph);

            }
            else if (uiInsertImage.rAnchorToCharacter->isChecked()) {
                // XXX: or VBaseline, VLine?
                anchor->setVerticalRel(KoTextAnchor::VChar);
                anchor->setHorizontalRel(KoTextAnchor::HChar);

            }
            else if (uiInsertImage.rAnchorAsCharacter->isChecked()) {
                anchor->setVerticalRel(KoTextAnchor::VBaseline);
                anchor->setHorizontalRel(KoTextAnchor::HChar);
                anchor->setAnchorType(KoTextAnchor::AnchorAsCharacter);
            }

            // horizontal alignment
            // XXX: what about HFromInside, HFromLeft, HInside, HOutside?
            if (uiInsertImage.rAlignLeft->isChecked()) {
                anchor->setHorizontalPos(KoTextAnchor::HLeft);
            }
            else if (uiInsertImage.rAlignCenter->isChecked()) {
                anchor->setHorizontalPos(KoTextAnchor::HCenter);
            }
            else if (uiInsertImage.rAlignRight->isChecked()) {
                anchor->setHorizontalPos(KoTextAnchor::HRight);
            }

            // vertical alignment
            if (uiInsertImage.rAlignTop->isChecked()) {
                // XXX: or VFromTop?
                anchor->setVerticalPos(KoTextAnchor::VTop);
            }
            else if (uiInsertImage.rAlignMiddle->isChecked()) {
                anchor->setVerticalPos(KoTextAnchor::VMiddle);
            }
            else if (uiInsertImage.rAlignBottom->isChecked()) {
                anchor->setVerticalPos(KoTextAnchor::VBottom);
            }

            anchor->setOffset(QPointF(0, -shape->size().height()));
            // insert the anchor into the text document
            KoTextEditor editor(qdoc);
            editor.insertInlineObject(anchor);
        }
        else {
            shape->setPosition(pos);
            pos += QPointF(25,25); // increase the position for each shape we insert so the
                                   // user can see them all.
        }
        
        // create the undo step.
        KWShapeCreateCommand *cmd = new KWShapeCreateCommand(kwdocument(), shape, anchor);
        KoSelection *selection = m_canvas->shapeManager()->selection();
        selection->deselectAll();
        selection->select(shape);
        m_canvas->addCommand(cmd);
    }
}

