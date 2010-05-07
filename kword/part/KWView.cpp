/* This file is part of the KDE project
 * Copyright (C) 2001 David Faure <faure@kde.org>
 * Copyright (C) 2005-2007, 2009, 2010 Thomas Zander <zander@kde.org>
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
#include <rdf/KoDocumentRdfBase.h>
#ifdef SHOULD_BUILD_RDF
#include <rdf/KoDocumentRdf.h>
#include <rdf/KoSemanticStylesheetsEditor.h>
#include "dockers/KWRdfDocker.h"
#include "dockers/KWRdfDockerFactory.h"
#endif
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
#include "dockers/KWStatisticsDocker.h"
#include "commands/KWFrameCreateCommand.h"
#include "commands/KWCreateOutlineCommand.h"
#include "commands/KWClipFrameCommand.h"
#include "commands/KWRemoveFrameClipCommand.h"

// koffice libs includes
#include <KoCopyController.h>
#include <KoTextDocument.h>
#include <KoTextShapeData.h>
#include <KoShapeCreateCommand.h>
#include <KoImageSelectionWidget.h>
#include <KoResourceManager.h>
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
#include <KoResourceManager.h>
#include <KoTextAnchor.h>
#include <KoShapeGroupCommand.h>
#include <KoZoomController.h>
#include <KoInlineTextObjectManager.h>
#include <KoBookmark.h>
#include <KoPathShape.h> // for KoPathShapeId
#include <KoCanvasController.h>

// KDE + Qt includes
#include <QHBoxLayout>
#include <QMenu>
#include <QTimer>
#include <klocale.h>
#include <kdebug.h>
#include <kicon.h>
#include <KToggleAction>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kxmlguifactory.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kmessagebox.h>

static KWFrame *frameForShape(KoShape *shape)
{
    while (shape->parent()) {
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

KWView::KWView(const QString &viewMode, KWDocument *document, QWidget *parent)
        : KoView(document, parent)
{
    m_document = document;
    m_snapToGrid = m_document->gridData().snapToGrid();
    m_gui = new KWGui(viewMode, this);
    m_canvas = m_gui->canvas();
    setFocusProxy(m_canvas);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_gui);

    setComponentData(KWFactory::componentData());
    setXMLFile("kword.rc");

    m_currentPage = m_document->pageManager()->begin();

    setupActions();

    connect(m_canvas->shapeManager()->selection(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    new KoFind(this, m_canvas->resourceManager(), actionCollection());

    m_zoomController = new KoZoomController(m_gui->canvasController(), &m_zoomHandler, actionCollection(), 0, this);

    if (shell()) {
        KWStatisticsDockerFactory statisticsFactory(this);
        KWStatisticsDocker *docker = dynamic_cast<KWStatisticsDocker *>(shell()->createDockWidget(&statisticsFactory));
        if (docker && docker->view() != this) docker->setView(this);

#ifdef SHOULD_BUILD_RDF
        KWRdfDockerFactory factory(this);
        shell()->createDockWidget(&factory);
#endif
    }

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
}

KWCanvas *KWView::kwcanvas() const
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
    m_canvas->setReadWrite(readWrite);
    KoToolManager::instance()->updateReadWrite(m_gui->canvasController(), readWrite);
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
    action = actionCollection()->action("insert_picture");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("format_page");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("inline_frame");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("edit_cut");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("edit_copy");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("edit_paste");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("delete_page");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("edit_delete");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("create_linked_frame");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("create_custom_outline");
    if (action) action->setEnabled(readWrite);
    action = actionCollection()->action("showStatusBar");
    if (action) action->setEnabled(readWrite);
}

void KWView::setupActions()
{
    m_actionFormatFrameSet  = new KAction(i18n("Frame/Frameset Properties"), this);
    actionCollection()->addAction("format_frameset", m_actionFormatFrameSet);
    m_actionFormatFrameSet->setToolTip(i18n("Alter frameset properties"));
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
    m_actionViewHeader->setCheckedState(KGuiItem(i18n("Disable Document Headers")));
    m_actionViewHeader->setToolTip(i18n("Shows and hides header display"));
    m_actionViewHeader->setWhatsThis(i18n("Selecting this option toggles the display of headers in KWord.<br/><br/>Headers are special frames at the top of each page which can contain page numbers or other information."));
    if (m_currentPage.isValid())
        m_actionViewHeader->setChecked(m_currentPage.pageStyle().headerPolicy() != KWord::HFTypeNone);
    connect(m_actionViewHeader, SIGNAL(triggered()), this, SLOT(toggleHeader()));

    m_actionViewFooter = new KToggleAction(i18n("Enable Document Footers"), this);
    actionCollection()->addAction("format_footer", m_actionViewFooter);
    m_actionViewFooter->setCheckedState(KGuiItem(i18n("Disable Document Footers")));
    m_actionViewFooter->setToolTip(i18n("Shows and hides footer display"));
    m_actionViewFooter->setWhatsThis(i18n("Selecting this option toggles the display of footers in KWord. <br/><br/>Footers are special frames at the bottom of each page which can contain page numbers or other information."));
    if (m_currentPage.isValid())
        m_actionViewFooter->setChecked(m_currentPage.pageStyle().footerPolicy() != KWord::HFTypeNone);
    connect(m_actionViewFooter, SIGNAL(triggered()), this, SLOT(toggleFooter()));

    m_actionViewSnapToGrid = new KToggleAction(i18n("Snap to Grid"), this);
    actionCollection()->addAction("view_snaptogrid", m_actionViewSnapToGrid);
    m_actionViewSnapToGrid->setChecked(m_snapToGrid);
    connect(m_actionViewSnapToGrid, SIGNAL(triggered()), this, SLOT(toggleSnapToGrid()));

    m_actionRaiseFrame  = new KAction(KIcon("raise"), i18n("Raise Frame"), this);
    actionCollection()->addAction("raiseframe", m_actionRaiseFrame);
    m_actionRaiseFrame->setShortcut(KShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_R));
    m_actionRaiseFrame->setToolTip(i18n("Raise the currently selected frame so that it appears above "
                                        "all the other frames"));
    m_actionRaiseFrame->setWhatsThis(i18n("Raise the currently selected frame so that it appears "
                                          "above all the other frames. This is only useful if frames overlap each other. If multiple "
                                          "frames are selected they are all raised in turn."));
    connect(m_actionRaiseFrame, SIGNAL(triggered()), this, SLOT(raiseFrame()));

    m_actionLowerFrame  = new KAction(KIcon("lower"), i18n("Lower Frame"), this);
    actionCollection()->addAction("lowerframe", m_actionLowerFrame);
    m_actionLowerFrame->setShortcut(KShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_L));
    m_actionLowerFrame->setToolTip(i18n("Lower the currently selected frame so that it disappears under "
                                        "any frame that overlaps it"));
    m_actionLowerFrame->setWhatsThis(i18n("Lower the currently selected frame so that it disappears under "
                                          "any frame that overlaps it. If multiple frames are selected they are all lowered in turn."));
    connect(m_actionLowerFrame, SIGNAL(triggered()), this, SLOT(lowerFrame()));

    m_actionBringToFront = new KAction(KIcon("bring_forward"), i18n("Bring to Front"), this);
    actionCollection()->addAction("bring_tofront_frame", m_actionBringToFront);
    connect(m_actionBringToFront, SIGNAL(triggered()), this, SLOT(bringToFront()));

    m_actionSendBackward = new KAction(KIcon("send_backward"), i18n("Send to Back"), this);
    actionCollection()->addAction("send_toback_frame", m_actionSendBackward);
    connect(m_actionSendBackward, SIGNAL(triggered()), this, SLOT(sendToBack()));

    KActionMenu *actionMenu = new KActionMenu(i18n("Variable"), this);
    foreach (QAction *action, m_document->inlineTextObjectManager()->createInsertVariableActions(kwcanvas()))
        actionMenu->addAction(action);
    actionCollection()->addAction("insert_variable", actionMenu);

#ifdef SHOULD_BUILD_RDF
    if (KoDocumentRdf* rdf = m_document->documentRdf()) {
        KAction* createRef = rdf->createInsertSemanticObjectReferenceAction(kwcanvas());
        actionCollection()->addAction("insert_semanticobject_ref", createRef);
        KActionMenu *subMenu = new KActionMenu(i18n("Create"), this);
        actionCollection()->addAction("insert_semanticobject_new", subMenu);
        foreach(KAction *action, rdf->createInsertSemanticObjectNewActions(kwcanvas())) {
            subMenu->addAction(action);
        }
    }
#endif

    m_actionAddBookmark = new KAction(KIcon("bookmark-new"), i18n("Bookmark..."), this);
    m_actionAddBookmark->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_G);
    actionCollection()->addAction("add_bookmark", m_actionAddBookmark);
    connect(m_actionAddBookmark, SIGNAL(triggered()), this, SLOT(addBookmark()));

    KAction *action = new KAction(i18n("Select Bookmark..."), this);
    action->setIcon(KIcon("bookmarks"));
    action->setShortcut(Qt::CTRL + Qt::Key_G);
    actionCollection()->addAction("select_bookmark", action);
    connect(action, SIGNAL(triggered()), this, SLOT(selectBookmark()));

    action = new KAction(i18n("Insert Picture..."), this);
    action->setToolTip(i18n("Insert a picture into document"));
    actionCollection()->addAction("insert_picture", action);
    connect(action, SIGNAL(triggered()), this, SLOT(insertImage()));

    action = new KAction(i18n("Frame Borders"), this);
    action->setToolTip(i18n("Turns the border display on and off"));
    action->setCheckable(true);
    actionCollection()->addAction("view_frameborders", action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(toggleViewFrameBorders(bool)));
    action->setChecked(m_document->config().viewFrameBorders());
    action->setWhatsThis(i18n("Turns the border display on and off.<br/><br/>The borders are never printed. This option is useful to see how the document will appear on the printed page."));

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

    action = new KAction(i18n("Make inline"), this);
    action->setToolTip(i18n("Convert current frame to an inline frame"));
    action->setWhatsThis(i18n("Convert the current frame to an inline frame.<br><br>Place the inline frame within the text at the point nearest to the frames current position."));
    actionCollection()->addAction("inline_frame", action);
    connect(action, SIGNAL(triggered()), this, SLOT(inlineFrame()));

    action = new KAction(i18n("Previous Page"), this);
    actionCollection()->addAction("page_previous", action);
    connect(action, SIGNAL(triggered()), this, SLOT(goToPreviousPage()));

    action = new KAction(i18n("Next Page"), this);
    actionCollection()->addAction("page_next", action);
    connect(action, SIGNAL(triggered()), this, SLOT(goToNextPage()));

    // -------------- Edit actions
    action = actionCollection()->addAction(KStandardAction::Cut,  "edit_cut", 0, 0);
    new KoCutController(kwcanvas(), action);
    action = actionCollection()->addAction(KStandardAction::Copy,  "edit_copy", 0, 0);
    new KoCopyController(kwcanvas(), action);
    action = actionCollection()->addAction(KStandardAction::Paste,  "edit_paste", 0, 0);
    new KoPasteController(kwcanvas(), action);

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

if (false) { // TODO move this to the text tool as soon as  a) the string freeze is lifted.  b) Qt45 makes this possible
    action = new KAction(i18n("Formatting Characters"), this);
    action->setCheckable(true);
    actionCollection()->addAction("view_formattingchars", action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowFormattingChars(bool)));
    action->setToolTip(i18n("Toggle the display of non-printing characters"));
    action->setWhatsThis(i18n("Toggle the display of non-printing characters.<br/><br/>When this is enabled, KWord shows you tabs, spaces, carriage returns and other non-printing characters."));
}

    action = new KAction(i18n("Select All Frames"), this);

    actionCollection()->addAction("edit_selectallframes", action);
    connect(action, SIGNAL(triggered()), this, SLOT(editSelectAllFrames()));

    action = new KAction(KIcon("edit-delete"), i18n("Delete"), this);
    action->setShortcut(QKeySequence("Del"));
    connect(action, SIGNAL(triggered()), this, SLOT(editDeleteSelection()));
    connect(kwcanvas()->toolProxy(), SIGNAL(selectionChanged(bool)), action, SLOT(setEnabled(bool)));
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

    action = new KAction(i18n("Create Custom Outline"), this);
    actionCollection()->addAction("create_custom_outline", action);
    action->setToolTip(i18n("Create a custom vector outline that text will run around"));
    action->setWhatsThis(i18n("Text normally runs around the content of a shape, when you want a custom outline that is independent of the content you can create one and alter it with the vector tools"));
    connect(action, SIGNAL(triggered()), this, SLOT(createCustomOutline()));

    action = new KAction(i18n("Create Frame-clip"), this);
    actionCollection()->addAction("create_clipped_frame", action);
    connect(action, SIGNAL(triggered()), this, SLOT(createFrameClipping()));

    action = new KAction(i18n("Remove Frame-clip"), this);
    actionCollection()->addAction("remove_clipped_frame", action);
    connect(action, SIGNAL(triggered()), this, SLOT(removeFrameClipping()));

    //------------------------ Settings menu
    action = new KToggleAction(i18n("Status Bar"), this);
    action->setToolTip(i18n("Shows or hides the status bar"));
    actionCollection()->addAction("showStatusBar", action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(showStatusBar(bool)));

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
            m_actionViewTextMode->setWhatsThis(i18n("Do not show any pictures, formatting or layout. KWord will display only the text for editing."));

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

        // -------------- Insert menu


        m_actionInsertLink = new KAction(i18n("Link..."), 0,
                this, SLOT(insertLink()),
                actionCollection(), "insert_link");
        actionInsertLink->setToolTip(i18n("Insert a Web address, email address or hyperlink to a file"));
        m_actionInsertLink->setWhatsThis(i18n("Insert a Web address, email address or hyperlink to a file."));

        m_actionInsertComment = new KAction(i18n("Comment..."), 0,
                this, SLOT(insertComment()),
                actionCollection(), "insert_comment");
        m_actionInsertComment->setToolTip(i18n("Insert a comment about the selected text"));
        m_actionInsertComment->setWhatsThis(i18n("Insert a comment about the selected text. These comments are not designed to appear on the final page."));

        m_actionEditComment = new KAction(i18n("Edit Comment..."), 0,
                this,SLOT(editComment()),
                actionCollection(), "edit_comment");
        m_actionEditComment->setToolTip(i18n("Change the content of a comment"));
        m_actionEditComment->setWhatsThis(i18n("Change the content of a comment."));

        m_actionRemoveComment = new KAction(i18n("Remove Comment"), 0,
                this,SLOT(removeComment()),
                actionCollection(), "remove_comment");
        m_actionRemoveComment->setToolTip(i18n("Remove the selected document comment"));
        m_actionRemoveComment->setWhatsThis(i18n("Remove the selected document comment."));
        m_actionCopyTextOfComment = new KAction(i18n("Copy Text of Comment..."), 0,
                this,SLOT(copyTextOfComment()),
                actionCollection(), "copy_text_comment");


        m_actionInsertFootEndNote = new KAction(i18n("Footnote/Endnote..."), 0,
                this, SLOT(insertFootNote()),
                actionCollection(), "insert_footendnote");
        m_actionInsertFootEndNote->setToolTip(i18n("Insert a footnote referencing the selected text"));
        m_actionInsertFootEndNote->setWhatsThis(i18n("Insert a footnote referencing the selected text."));

        m_actionInsertContents = new KAction(i18n("Table of Contents"), 0,
                this, SLOT(insertContents()),
                actionCollection(), "insert_contents");
        m_actionInsertContents->setToolTip(i18n("Insert table of contents at the current cursor position"));
        m_actionInsertContents->setWhatsThis(i18n("Insert table of contents at the current cursor position."));

        actionInsertVariable = new KActionMenu(i18n("Variable"),
                actionCollection(), "insert_variable");

        // The last argument is only needed if a submenu is to be created
        addVariableActions(VT_FIELD, KoFieldVariable::actionTexts(), actionInsertVariable, i18n("Document Information"));
        addVariableActions(VT_DATE, KoDateVariable::actionTexts(), actionInsertVariable, i18n("Date"));
        addVariableActions(VT_TIME, KoTimeVariable::actionTexts(), actionInsertVariable, i18n("Time"));
        addVariableActions(VT_PGNUM, KoPageVariable::actionTexts(), actionInsertVariable, i18n("Page"));
        addVariableActions(VT_STATISTIC, KWStatisticVariable::actionTexts(), actionInsertVariable, i18n("Statistic"));

        m_actionInsertCustom = new KActionMenu(i18n("Custom"),
                actionCollection(), "insert_custom");
        actionInsertVariable->insert(m_actionInsertCustom);

        //addVariableActions(VT_CUSTOM, KWCustomVariable::actionTexts(), actionInsertVariable, QString::null);

        addVariableActions(VT_MAILMERGE, KoMailMergeVariable::actionTexts(), actionInsertVariable, QString::null);

        actionInsertVariable->popupMenu()->insertSeparator();
        m_actionRefreshAllVariable = new KAction(i18n("Refresh All Variables"), 0,
                this, SLOT(refreshAllVariable()),
                actionCollection(), "refresh_all_variable");
        m_actionRefreshAllVariable->setToolTip(i18n("Update all variables to current values"));
        m_actionRefreshAllVariable->setWhatsThis(i18n("Update all variables in the document to current values.<br><br>This will update page numbers, dates or any other variables that need updating."));

        actionInsertVariable->insert(m_actionRefreshAllVariable);

        m_actionInsertExpression = new KActionMenu(i18n("Expression"),
                actionCollection(), "insert_expression");
        loadexpressionActions(m_actionInsertExpression);

        m_actionInsertFile = new KAction(i18n("File..."), 0,
                this, SLOT(insertFile()),
                actionCollection(), "insert_file");


        // ------------------------- Format menu
        m_actionFormatFrameStylist = new KAction(i18n("Frame Style Manager"), 0,
                this, SLOT(extraFrameStylist()),
                actionCollection(), "frame_stylist");
        m_actionFormatFrameStylist->setToolTip(i18n("Change attributes of framestyles"));
        m_actionFormatFrameStylist->setWhatsThis(i18n("Change background and borders of framestyles.<p>Multiple framestyles can be changed using the dialog box."));

        // ---------------------------- frame toolbar actions

        m_actionFrameStyleMenu = new KActionMenu(i18n("Framestyle"),
                actionCollection(), "frame_stylemenu");
        m_actionFrameStyle = new KSelectAction(i18n("Framestyle"),
                actionCollection(), "frame_style");
        connect(m_actionFrameStyle, SIGNAL(activated(int)),
                this, SLOT(frameStyleSelected(int)));
        updateFrameStyleList();
        m_actionBorderOutline = new KToggleAction(KIcon("borderoutline"), i18n("Border Outline"),
                actionCollection(), "border_outline");
        connect(m_actionBorderOutline, SIGNAL(triggered(bool)), this, SLOT(borderOutline()));
        m_actionBorderLeft = new KToggleAction(KIcon("borderleft"), i18n("Border Left"),
                actionCollection(), "border_left");
        connect(m_actionBorderLeft, SIGNAL(triggered(bool)), this, SLOT(borderLeft()));
        m_actionBorderRight = new KToggleAction(KIcon("borderright"), i18n("Border Right"),
                actionCollection(), "border_right");
        connect(m_actionBorderRight, SIGNAL(triggered(bool)), this, SLOT(borderRight()));
        m_actionBorderTop = new KToggleAction(KIcon("bordertop"), i18n("Border Top"),
                actionCollection(), "border_top");
        connect(m_actionBorderTop, SIGNAL(triggered(bool)), this, SLOT(borderTop()));
        m_actionBorderBottom = new KToggleAction(KIcon("borderbottom"), i18n("Border Bottom"),
                actionCollection(), "border_bottom");
        connect(m_actionBorderBottom, SIGNAL(triggered(bool)), this, SLOT(borderBottom()));
        m_actionBorderStyle = new KSelectAction(i18n("Border Style"),
                actionCollection(), "border_style");

        QStringList lst;
        lst << KoBorder::getStyle(KoBorder::SOLID);
        lst << KoBorder::getStyle(KoBorder::DASH);
        lst << KoBorder::getStyle(KoBorder::DOT);
        lst << KoBorder::getStyle(KoBorder::DASH_DOT);
        lst << KoBorder::getStyle(KoBorder::DASH_DOT_DOT);
        lst << KoBorder::getStyle(KoBorder::DOUBLE_LINE);
        m_actionBorderStyle->setItems(lst);
        m_actionBorderWidth = new KSelectAction(i18n("Border Width"),
                actionCollection(), "border_width");
        lst.clear();
        for (unsigned int i = 1; i < 10; i++)
            lst << QString::number(i);
        m_actionBorderWidth->setItems(lst);
        m_actionBorderWidth->setCurrentItem(0);

        m_actionBorderColor = new TKSelectColorAction(i18n("Border Color"), TKSelectColorAction::LineColor, actionCollection(), "border_color", true);
        m_actionBorderColor->setDefaultColor(QColor());


        // ---------------------- Table menu
        m_actionTablePropertiesMenu = new KAction(i18n("Properties"), 0,
                this, SLOT(tableProperties()),
                actionCollection(), "table_propertiesmenu");
        m_actionTablePropertiesMenu->setToolTip(i18n("Adjust properties of the current table"));
        m_actionTablePropertiesMenu->setWhatsThis(i18n("Adjust properties of the current table."));

        m_actionTableInsertRow = new KAction(i18n("Insert Row..."), "insert_table_row", 0,
                this, SLOT(tableInsertRow()),
                actionCollection(), "table_insrow");
        m_actionTableInsertRow->setToolTip(i18n("Insert one or more rows at cursor location"));
        m_actionTableInsertRow->setWhatsThis(i18n("Insert one or more rows at current cursor location."));

        m_actionTableInsertCol = new KAction(i18n("Insert Column..."), "insert_table_col", 0,
                this, SLOT(tableInsertCol()),
                actionCollection(), "table_inscol");
        m_actionTableInsertCol->setToolTip(i18n("Insert one or more columns into the current table"));
        m_actionTableInsertCol->setWhatsThis(i18n("Insert one or more columns into the current table."));

        m_actionTableDelRow = new KAction(0, "delete_table_row", 0,
                this, SLOT(tableDeleteRow()),
                actionCollection(), "table_delrow");
        m_actionTableDelRow->setToolTip(i18n("Delete selected rows from the current table"));
        m_actionTableDelRow->setWhatsThis(i18n("Delete selected rows from the current table."));

        m_actionTableDelCol = new KAction(0, "delete_table_col", 0,
                this, SLOT(tableDeleteCol()),
                actionCollection(), "table_delcol");
        m_actionTableDelCol->setToolTip(i18n("Delete selected columns from the current table"));
        m_actionTableDelCol->setWhatsThis(i18n("Delete selected columns from the current table."));

        m_actionTableResizeCol = new KAction(i18n("Resize Column..."), 0,
                this, SLOT(tableResizeCol()),
                actionCollection(), "table_resizecol");
        m_actionTableResizeCol->setToolTip(i18n("Change the width of the currently selected column"));
        m_actionTableResizeCol->setWhatsThis(i18n("Change the width of the currently selected column."));


        m_actionTableJoinCells = new KAction(i18n("Join Cells"), 0,
                this, SLOT(tableJoinCells()),
                actionCollection(), "table_joincells");
        m_actionTableJoinCells->setToolTip(i18n("Join two or more cells into one large cell"));
        m_actionTableJoinCells->setWhatsThis(i18n("Join two or more cells into one large cell.<p>This is a good way to create titles and labels within a table."));

        m_actionTableSplitCells= new KAction(i18n("Split Cell..."), 0,
                this, SLOT(tableSplitCells()),
                actionCollection(), "table_splitcells");
        m_actionTableSplitCells->setToolTip(i18n("Split one cell into two or more cells"));
        m_actionTableSplitCells->setWhatsThis(i18n("Split one cell into two or more cells.<p>Cells can be split horizontally, vertically or both directions at once."));

        m_actionTableProtectCells= new KToggleAction(i18n("Protect Cells"), 0, 0, 0,
                actionCollection(), "table_protectcells");
        m_actionTableProtectCells->setToolTip(i18n("Prevent changes to content of selected cells"));
        connect (m_actionTableProtectCells, SIGNAL(toggled(bool)), this,
                SLOT(tableProtectCells(bool)));

        m_actionTableProtectCells->setWhatsThis(i18n("Toggles cell protection on and off.<br><br>When cell protection is on, the user can not alter the content or formatting of the text within the cell."));

        m_actionTableDelete = new KAction(i18n("Delete Table"), 0,
                this, SLOT(tableDelete()),
                actionCollection(), "table_delete");
        m_actionTableDelete->setToolTip(i18n("Delete the entire table"));
        m_actionTableDelete->setWhatsThis(i18n("Deletes all cells and the content within the cells of the currently selected table."));


        m_actionTableStylist = new KAction(i18n("Table Style Manager"), 0,
                this, SLOT(tableStylist()),
                actionCollection(), "table_stylist");
        m_actionTableStylist->setToolTip(i18n("Change attributes of tablestyles"));
        m_actionTableStylist->setWhatsThis(i18n("Change textstyle and framestyle of the tablestyles.<p>Multiple tablestyles can be changed using the dialog box."));

        m_actionTableStyleMenu = new KActionMenu(i18n("Tablestyle"),
                actionCollection(), "table_stylemenu");
        m_actionTableStyle = new KSelectAction(i18n("Tablestyle"),
                actionCollection(), "table_style");
        connect(m_actionTableStyle, SIGNAL(activated(int)),
                this, SLOT(tableStyleSelected(int)));
        updateTableStyleList();

        m_actionConvertTableToText = new KAction(i18n("Convert Table to Text"), 0,
                this, SLOT(convertTableToText()),
                actionCollection(), "convert_table_to_text");
        m_actionSortText= new KAction(i18n("Sort Text..."), 0,
                this, SLOT(sortText()),
                actionCollection(), "sort_text");

        m_actionAddPersonalExpression= new KAction(i18n("Add Expression"), 0,
                this, SLOT(addPersonalExpression()),
                actionCollection(), "add_personal_expression");


        // ---------------------- Tools menu


        m_actionAllowAutoFormat = new KToggleAction(i18n("Enable Autocorrection"), 0,
                this, SLOT(slotAllowAutoFormat()),
                actionCollection(), "enable_autocorrection");
        m_actionAllowAutoFormat->setCheckedState(i18n("Disable Autocorrection"));
        m_actionAllowAutoFormat->setToolTip(i18n("Toggle autocorrection on and off"));
        m_actionAllowAutoFormat->setWhatsThis(i18n("Toggle autocorrection on and off."));

        m_actionAutoFormat = new KAction(i18n("Configure Autocorrection..."), 0,
                this, SLOT(extraAutoFormat()),
                actionCollection(), "configure_autocorrection");
        m_actionAutoFormat->setToolTip(i18n("Change autocorrection options"));
        m_actionAutoFormat->setWhatsThis(i18n("Change autocorrection options including:<p> <UL><LI><P>exceptions to autocorrection</P> <LI><P>add/remove autocorrection replacement text</P> <LI><P>and basic autocorrection options</P>."));

        m_actionEditCustomVarsEdit = new KAction(i18n("Custom Variables..."), 0,
                this, SLOT(editCustomVars()), // TODO: new dialog w add etc.
                actionCollection(), "custom_vars");

        m_actionEditPersonnalExpr=new KAction(i18n("Edit Personal Expressions..."), 0,
                this, SLOT(editPersonalExpr()),
                actionCollection(), "personal_expr");
        m_actionEditPersonnalExpr->setToolTip(i18n("Add or change one or more personal expressions"));
        m_actionEditPersonnalExpr->setWhatsThis(i18n("Add or change one or more personal expressions.<p>Personal expressions are a way to quickly insert commonly used phrases or text into your document."));

        m_actionChangeCase=new KAction(i18n("Change Case..."), 0,
                this, SLOT(changeCaseOfText()),
                actionCollection(), "change_case");
        m_actionChangeCase->setToolTip(i18n("Alter the capitalization of selected text"));
        m_actionChangeCase->setWhatsThis(i18n("Alter the capitalization of selected text to one of five pre-defined patterns.<p>You can also switch all letters from upper case to lower case and from lower case to upper case in one move."));

        //------------------------ Settings menu
        m_actionConfigure = actionCollection()->addAction(KStandardAction::Preferences,  "configure", this, SLOT(configure()));

        //------------------------ Menu frameSet
        QAction *actionChangePicture=new KAction(i18n("Change Picture..."),"frame_image",0,
                this, SLOT(changePicture()),
                actionCollection(), "change_picture");
        actionChangePicture->setToolTip(i18n("Change the picture in the currently selected frame"));
        actionChangePicture->setWhatsThis(i18n("You can specify a different picture in the current frame.<br><br>KWord automatically resizes the new picture to fit within the old frame."));

        m_actionConfigureHeaderFooter=new KAction(i18n("Configure Header/Footer..."), 0,
                this, SLOT(configureHeaderFooter()),
                actionCollection(), "configure_headerfooter");
        m_actionConfigureHeaderFooter->setToolTip(i18n("Configure the currently selected header or footer"));
        m_actionConfigureHeaderFooter->setWhatsThis(i18n("Configure the currently selected header or footer."));

        m_actionOpenLink = new KAction(i18n("Open Link"), 0,
                this, SLOT(openLink()),
                actionCollection(), "open_link");
        m_actionOpenLink->setToolTip(i18n("Open the link with the appropriate application"));
        m_actionOpenLink->setWhatsThis(i18n("Open the link with the appropriate application.<br><br>Web addresses are opened in a browser.<br>Email addresses begin a new message addressed to the link.<br>File links are opened by the appropriate viewer or editor."));

        m_actionChangeLink=new KAction(i18n("Change Link..."), 0,
                this,SLOT(changeLink()),
                actionCollection(), "change_link");
        m_actionChangeLink->setToolTip(i18n("Change the content of the currently selected link"));
        m_actionChangeLink->setWhatsThis(i18n("Change the details of the currently selected link."));

        m_actionCopyLink = new KAction(i18n("Copy Link"), 0,
                this, SLOT(copyLink()),
                actionCollection(), "copy_link");

        m_actionAddLinkToBookmak = new KAction(i18n("Add to Bookmark"), 0,
                this, SLOT(addToBookmark()),
                actionCollection(), "add_to_bookmark");

        m_actionRemoveLink = new KAction(i18n("Remove Link"), 0,
                this, SLOT(removeLink()),
                actionCollection(), "remove_link");

        m_actionShowDocStruct = new KToggleAction(i18n("Show Doc Structure"), 0,
                this, SLOT(showDocStructure()),
                actionCollection(), "show_docstruct");
        m_actionShowDocStruct->setCheckedState(i18n("Hide Doc Structure"));
        m_actionShowDocStruct->setToolTip(i18n("Open document structure sidebar"));
        m_actionShowDocStruct->setWhatsThis(i18n("Open document structure sidebar.<p>This sidebar helps you organize your document and quickly find pictures, tables etc."));

        m_actionConfigureCompletion = new KAction(i18n("Configure Completion..."), 0,
                this, SLOT(configureCompletion()),
                actionCollection(), "configure_completion");
        m_actionConfigureCompletion->setToolTip(i18n("Change the words and options for autocompletion"));
        m_actionConfigureCompletion->setWhatsThis(i18n("Add words or change the options for autocompletion."));


        new KAction(i18n("Completion"), KStdAccel::shortcut(KStdAccel::TextCompletion), this, SLOT(slotCompletion()), actionCollection(), "completion");

        new KAction(i18n("Increase Numbering Level"), Qt::ALT+Qt::Key_Right,
                this, SLOT(slotIncreaseNumberingLevel()), actionCollection(), "increase_numbering_level");
        new KAction(i18n("Decrease Numbering Level"), Qt::ALT+Qt::Key_Left,
                this, SLOT(slotDecreaseNumberingLevel()), actionCollection(), "decrease_numbering_level");


        // --------
        m_actionEditCustomVars = new KAction(i18n("Edit Variable..."), 0,
                this, SLOT(editCustomVariable()),
                actionCollection(), "edit_customvars");
        m_actionApplyAutoFormat= new KAction(i18n("Apply Autocorrection"), 0,
                this, SLOT(applyAutoFormat()),
                actionCollection(), "apply_autoformat");
        m_actionApplyAutoFormat->setToolTip(i18n("Manually force KWord to scan the entire document and apply autocorrection"));
        m_actionApplyAutoFormat->setWhatsThis(i18n("Manually force KWord to scan the entire document and apply autocorrection."));

        m_actionCreateStyleFromSelection = new KAction(i18n("Create Style From Selection..."), 0,
                this, SLOT(createStyleFromSelection()),
                actionCollection(), "create_style");
        m_actionCreateStyleFromSelection->setToolTip(i18n("Create a new style based on the currently selected text"));
        m_actionCreateStyleFromSelection->setWhatsThis(i18n("Create a new style based on the currently selected text.")); // ## "on the current paragraph, taking the formatting from where the cursor is. Selecting text isn't even needed."

        m_actionConfigureFootEndNote = new KAction(i18n("Footnote..."), 0,
                this, SLOT(configureFootEndNote()),
                actionCollection(), "format_footendnote");
        m_actionConfigureFootEndNote->setToolTip(i18n("Change the look of footnotes"));
        m_actionConfigureFootEndNote->setWhatsThis(i18n("Change the look of footnotes."));

        m_actionEditFootEndNote= new KAction(i18n("Edit Footnote"), 0,
                this, SLOT(editFootEndNote()),
                actionCollection(), "edit_footendnote");
        m_actionEditFootEndNote->setToolTip(i18n("Change the content of the currently selected footnote"));
        m_actionEditFootEndNote->setWhatsThis(i18n("Change the content of the currently selected footnote."));


        m_actionChangeFootNoteType = new KAction(i18n("Change Footnote/Endnote Parameter"), 0,
                this, SLOT(changeFootNoteType()),
                actionCollection(), "change_footendtype");

        m_actionSavePicture= new KAction(i18n("Save Picture As..."), 0,
                this, SLOT(savePicture()),
                actionCollection(), "save_picture");
        m_actionSavePicture->setToolTip(i18n("Save the picture in a separate file"));
        m_actionSavePicture->setWhatsThis(i18n("Save the picture in the currently selected frame in a separate file, outside the KWord document."));

        m_actionAllowBgSpellCheck = new KToggleAction(i18n("Autospellcheck"), 0,
                this, SLOT(autoSpellCheck()),
                actionCollection(), "tool_auto_spellcheck");


        m_actionGoToFootEndNote = new KAction(QString::null, //set dynamically
                0, this, SLOT(goToFootEndNote()),
                actionCollection(), "goto_footendnote");

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

        m_actionConvertToTextBox = new KAction(i18n("Convert to Text Box"), 0,
                this, SLOT(convertToTextBox()),
                actionCollection(), "convert_to_text_box");

        m_actionSpellIgnoreAll = new KAction(i18n("Ignore All"), 0,
                this, SLOT(slotAddIgnoreAllWord()),
                actionCollection(), "ignore_all");

        m_actionAddWordToPersonalDictionary=new KAction(i18n("Add Word to Dictionary"),0,
                this, SLOT(addWordToDictionary()),
                actionCollection(), "add_word_to_dictionary");

        m_actionEmbeddedStoreInternal=new KToggleAction(i18n("Store Document Internally"),0,
                this, SLOT(embeddedStoreInternal()),
                actionCollection(), "embedded_store_internal");

    */
}

QList<KWFrame*> KWView::selectedFrames() const
{
    QList<KWFrame*> frames;
    foreach (KoShape *shape, kwcanvas()->shapeManager()->selection()->selectedShapes()) {
        KWFrame *frame = frameForShape(shape);
        Q_ASSERT(frame);
        frames.append(frame);
    }
    return frames;
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
    KWPrintingDialog *dia = new KWPrintingDialog(this);
    dia->printer().setResolution(600);
    dia->printer().setCreator("KWord 2.0");
    dia->printer().setFullPage(true); // ignore printer margins
    return dia;
}

void KWView::insertFrameBreak()
{
    KoTextEditor *handler = qobject_cast<KoTextEditor*> (kwcanvas()->toolProxy()->selection());
    if (handler) {
        // this means we have the text tool selected right now.
        handler->insertFrameBreak();
    } else if (m_document->mainFrameSet()) { // lets just add one to the main text frameset
        KoTextDocument doc(m_document->mainFrameSet()->document());
        doc.textEditor()->insertFrameBreak();
    }
}

void KWView::addBookmark()
{
    QString name, suggestedName;

    KoSelection *selection = kwcanvas()->shapeManager()->selection();
    KoShape *shape = 0;
    shape = selection->firstSelectedShape();
    if (shape == 0) return; // no shape selected

    KWFrame *frame = frameForShape(shape);
    Q_ASSERT(frame);
    KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
    if (fs == 0) return;

    QString tool = KoToolManager::instance()->preferredToolForSelection(selection->selectedShapes());
    KoToolManager::instance()->switchToolRequested(tool);
    KoTextEditor *handler = qobject_cast<KoTextEditor*> (kwcanvas()->toolProxy()->selection());
    Q_ASSERT(handler);

    KoBookmarkManager *manager = m_document->inlineTextObjectManager()->bookmarkManager();
    if (handler->hasSelection())
        suggestedName = handler->selectedText();

    KWCreateBookmarkDialog *dia = new KWCreateBookmarkDialog(manager->bookmarkNameList(), suggestedName, m_canvas->canvasWidget());
    if (dia->exec() == QDialog::Accepted)
        name = dia->newBookmarkName();
    else {
        delete dia;
        return;
    }
    delete dia;

    handler->addBookmark(name);
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

    KoBookmark *bookmark = manager->retrieveBookmark(name);
    KoShape *shape = bookmark->shape();
    KoSelection *selection = kwcanvas()->shapeManager()->selection();
    selection->deselectAll();
    selection->select(shape);

    QString tool = KoToolManager::instance()->preferredToolForSelection(selection->selectedShapes());
    KoToolManager::instance()->switchToolRequested(tool);

    KoResourceManager *rm = m_canvas->resourceManager();
    if (bookmark->hasSelection()) {
        rm->setResource(KoText::CurrentTextPosition, bookmark->position());
        rm->setResource(KoText::CurrentTextAnchor, bookmark->endBookmark()->position() + 1);
        rm->clearResource(KoText::SelectedTextPosition);
        rm->clearResource(KoText::SelectedTextAnchor);
    } else
        rm->setResource(KoText::CurrentTextPosition, bookmark->position() + 1);
}

void KWView::deleteBookmark(const QString &name)
{
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
}

void KWView::editDeleteFrame()
{
    QList<KoShape*> frames;
    foreach (KoShape *shape, kwcanvas()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection)) {
        KWFrame *frame = frameForShape(shape);
        if (frame) {
            KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
            if (fs && fs->textFrameSetType() != KWord::OtherTextFrameSet)
                continue; // can't delete auto-generated frames
        }
        frames.append(shape);
    }
    QUndoCommand *cmd = kwcanvas()->shapeController()->removeShapes(frames);
    m_document->addCommand(cmd);
}

void KWView::toggleHeader()
{
    if (!m_currentPage.isValid())
        return;
    Q_ASSERT(m_currentPage.pageStyle().isValid());
    m_currentPage.pageStyle().setHeaderPolicy(m_actionViewHeader->isChecked() ? KWord::HFTypeEvenOdd : KWord::HFTypeNone);
    m_document->relayout();
}

void KWView::toggleFooter()
{
    if (!m_currentPage.isValid())
        return;
    Q_ASSERT(m_currentPage.pageStyle().isValid());
    m_currentPage.pageStyle().setFooterPolicy(m_actionViewFooter->isChecked() ? KWord::HFTypeEvenOdd : KWord::HFTypeNone);
    m_document->relayout();
}

void KWView::toggleSnapToGrid()
{
    m_snapToGrid = !m_snapToGrid;
    m_document->gridData().setSnapToGrid(m_snapToGrid); // for persistency
}

void KWView::adjustZOrderOfSelectedFrames(KoShapeReorderCommand::MoveShapeType direction)
{
    // TODO we should not allow any shapes to fall behind the main text frame.
    QUndoCommand *cmd = KoShapeReorderCommand::createCommand(kwcanvas()->shapeManager()->selection()->selectedShapes(),
                        kwcanvas()->shapeManager(), direction);
    if (cmd)
        m_document->addCommand(cmd);
}

void KWView::toggleViewFrameBorders(bool on)
{
    kwcanvas()->resourceManager()->setResource(KoText::ShowTextFrames, on);
    kwcanvas()->update();
    m_document->config().setViewFrameBorders(on);
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
    dia->show();
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

void KWView::inlineFrame()
{
    Q_ASSERT(kwdocument()->mainFrameSet());
    KoSelection *selection = kwcanvas()->shapeManager()->selection();

    KoShape *targetShape = 0;
    foreach (KoShape *shape, selection->selectedShapes(KoFlake::TopLevelSelection)) {
        if (shape->isGeometryProtected())
            continue;
        if (shape->parent())
            continue;
        targetShape = shape;
        break; // TODO group before...
    }
    if (targetShape == 0) {
        KMessageBox::error(this, i18n("Please select at least one non-locked shape and try again"));
        return;
    }

    selection->deselectAll();
    KWFrame *frameForAnchor = 0;
    int area = 0;
    QRectF br = targetShape->boundingRect();
    // now find the frame that is closest to the frame we want to inline.
    foreach (KWFrame *frame, kwdocument()->mainFrameSet()->frames()) {
        QRectF intersection = br.intersected(frame->shape()->boundingRect());
        int intersectArea = qRound(intersection.width() * intersection.height());
        if (intersectArea > area) {
            frameForAnchor = frame;
            area = intersectArea;
        } else if (frameForAnchor == 0) {
            // TODO check distance between frames or something.
        }
    }

    if (frameForAnchor == 0) {/* can't happen later on... */
        kDebug(32001) << "bailing out..."; return;
    }

    selection->select(frameForAnchor->shape());

    QPointF absPos = targetShape->absolutePosition();
    targetShape->setParent(static_cast<KoShapeContainer*>(frameForAnchor->shape()));
    targetShape->setAbsolutePosition(absPos);

    QString tool = KoToolManager::instance()->preferredToolForSelection(selection->selectedShapes());
    KoToolManager::instance()->switchToolRequested(tool);
    KoTextEditor *handler = qobject_cast<KoTextEditor*> (kwcanvas()->toolProxy()->selection());
    Q_ASSERT(handler);
    KoTextAnchor *anchor = new KoTextAnchor(targetShape);
    anchor->setOffset(QPointF(0, -targetShape->size().height()));
    // TODO move caret
    handler->insertInlineObject(anchor);
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
    KoSelection *selection = kwcanvas()->shapeManager()->selection();
    QList<KoShape*> oldSelection = selection->selectedShapes(KoFlake::TopLevelSelection);
    if (oldSelection.count() == 0)
        return;
    selection->deselectAll();

    QUndoCommand *cmd = new QUndoCommand(i18n("Create Linked Copy"));
    foreach (KoShape *shape, oldSelection) {
        KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
        Q_ASSERT(frame);
        KWCopyShape *copy = new KWCopyShape(frame->shape(), m_document->pageManager());
        copy->setPosition(frame->shape()->position());
        QPointF offset(40, 40);
        kwcanvas()->clipToDocument(copy, offset);
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
    statusBar()->setVisible(toggled);
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

void KWView::setShowFormattingChars(bool on)
{
    KoResourceManager *rm = m_canvas->resourceManager();
    rm->setResource(KoText::ShowSpaces, on);
    rm->setResource(KoText::ShowTabs, on);
    rm->setResource(KoText::ShowEnters, on);
    rm->setResource(KoText::ShowSpecialCharacters, on);
}

void KWView::editSelectAllFrames()
{
    KoSelection *selection = kwcanvas()->shapeManager()->selection();
    foreach (KWFrameSet *fs, m_document->frameSets()) {
        foreach (KWFrame *frame, fs->frames()) {
            if (frame->shape()->isVisible())
                selection->select(frame->shape());
        }
    }
}

void KWView::editDeleteSelection()
{
    kwcanvas()->toolProxy()->deleteSelection();
}

void KWView::createCustomOutline()
{
    QList<KWFrame *> frames = selectedFrames();
    if (frames.count() == 0)
        return;
    if (frames.count() == 1) {
        m_document->addCommand(new KWCreateOutlineCommand(m_document, frames.at(0)));
    } else {
        QUndoCommand *cmd = new QUndoCommand(i18n("Create outlines"));
        foreach (KWFrame *frame, frames)
            new KWCreateOutlineCommand(m_document, frame, cmd);
        m_document->addCommand(cmd);
    }

    KoSelection *selection = kwcanvas()->shapeManager()->selection();
    selection->deselectAll();
    foreach (KWFrame *frame, frames) {
        KoShapeContainer *group = frame->shape()->parent();
        if (group)
            selection->select(group);
    }

    KoToolManager::instance()->switchToolRequested("PathToolFactoryId");
}

void KWView::createFrameClipping()
{
    QSet<KWFrame *> clipFrames;
    foreach (KoShape *shape, kwcanvas()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection)) {
        KWFrame *frame = frameForShape(shape);
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
    foreach (KoShape *shape, kwcanvas()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection)) {
        KWFrame *frame = frameForShape(shape);
        Q_ASSERT(frame);
        if (frame->shape()->parent())
            unClipFrames << frame;
    }
    if (!unClipFrames.isEmpty()) {
        KWRemoveFrameClipCommand *cmd = new KWRemoveFrameClipCommand(unClipFrames.toList(), m_document);
        m_document->addCommand(cmd);
    }
}

void KWView::insertImage()
{
    KoShape *shape = KoImageSelectionWidget::selectImageShape(m_document->resourceManager(), this);
    if (shape) {
        if (m_currentPage.isValid()) {
            QRectF page = m_currentPage.rect();
            // make the shape be on the current page, and fit inside the current page.
            if (page.width() < shape->size().width() || page.height() < shape->size().height()) {
                QSizeF newSize(page.width() * 0.9, page.height() * 0.9);
                const qreal xRatio = newSize.width() / shape->size().width();
                const qreal yRatio = newSize.height() / shape->size().height();
                if (xRatio > yRatio) // then lets make the vertical set the size.
                    newSize.setWidth(shape->size().width() * yRatio);
                else
                    newSize.setHeight(shape->size().height() * xRatio);
                shape->setSize(newSize);
            }
            shape->setPosition(page.topLeft());

            int zIndex = 0;
            foreach (KoShape *s, m_canvas->shapeManager()->shapesAt(page))
                zIndex = qMax(s->zIndex(), zIndex);
            shape->setZIndex(zIndex+1);
        }

        KoShapeCreateCommand *cmd = new KoShapeCreateCommand(m_document, shape);
        KoSelection *selection = m_canvas->shapeManager()->selection();
        selection->deselectAll();
        selection->select(shape);
        m_document->addCommand(cmd);
    }
}

void KWView::setGuideVisibility(bool on)
{
    m_document->guidesData().setShowGuideLines(on);
    kwcanvas()->update();
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
}

void KWView::selectionChanged()
{
    KoShape *shape = kwcanvas()->shapeManager()->selection()-> firstSelectedShape();
    m_actionFormatFrameSet->setEnabled(shape != 0);
    m_actionAddBookmark->setEnabled(shape != 0);
    if (shape) {
        setCurrentPage(m_document->pageManager()->page(shape));
        KWFrame *frame = frameForShape(shape);
        KWTextFrameSet *fs = frame == 0 ? 0 : dynamic_cast<KWTextFrameSet*>(frame->frameSet());
        if (fs)
            m_actionAddBookmark->setEnabled(true);
        else
            m_actionAddBookmark->setEnabled(false);
    }
    // actions need at least one shape selected
    actionCollection()->action("create_linked_frame")->setEnabled(shape);

    foreach (KoShape *shape, kwcanvas()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection)) {
        KWFrame *frame = frameForShape(shape);
        Q_ASSERT(frame);
        QVariant variant;
        variant.setValue<void*>(frame);
        m_canvas->resourceManager()->setResource(KWord::CurrentFrame, variant);
        variant.setValue<void*>(frame->frameSet());
        m_canvas->resourceManager()->setResource(KWord::CurrentFrameSet, variant);
        break;
    }
}

void KWView::setCurrentPage(const KWPage &currentPage)
{
    Q_ASSERT(currentPage.isValid());
    if (currentPage != m_currentPage) {
        m_currentPage = currentPage;
        m_canvas->resourceManager()->setResource(KoCanvasResource::CurrentPage, m_currentPage.pageNumber());
        m_zoomController->setPageSize(m_currentPage.rect().size());
        m_actionViewHeader->setChecked(m_currentPage.pageStyle().headerPolicy() != KWord::HFTypeNone);
        m_actionViewFooter->setChecked(m_currentPage.pageStyle().footerPolicy() != KWord::HFTypeNone);
    }
}

void KWView::goToNextPage()
{
    KWPage page = currentPage().next();
    if (page.isValid())
        goToPage(page);
}

void KWView::goToPreviousPage()
{
    KWPage page = currentPage().previous();
    if (page.isValid())
        goToPage(page);
}

void KWView::goToPage(const KWPage &page)
{
    KoCanvasController *controller = m_gui->canvasController();
    QPoint origPos = controller->scrollBarValue();
    QPointF pos = m_canvas->viewMode()->documentToView(QPointF(0, page.offsetInDocument()));
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
    KoTextEditor *editor = qobject_cast<KoTextEditor*>(kwcanvas()->toolProxy()->selection());
    if (!editor) {
        kDebug(30015) << "no editor, not reflowing rdf semantic item.";
        return;
    }
    kDebug(30015) << "reflowing rdf semantic item.";
    KoRdfSemanticItemViewSite vs(item, xmlid);
    vs.reflowUsingCurrentStylesheet(editor);
#endif
}

