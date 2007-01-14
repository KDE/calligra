/* This file is part of the KDE project
 * Copyright (C) 2001 David Faure <faure@kde.org>
 * Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
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
#include "KWFactory.h"
#include "frame/KWFrame.h"
#include "dialog/KWFrameDialog.h"

// koffice libs includes
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoShapeController.h>
#include <KoZoomAction.h>
#include <KoToolManager.h>
#include <KoMainWindow.h>
#include <KoToolBox.h>
#include <KoTextShapeData.h>
#include <KoShapeSelector.h>
#include <KoToolBoxFactory.h>
#include <KoToolDockerFactory.h>
#include <KoShapeSelectorFactory.h>
#include <KoTextSelectionHandler.h>
#include <KoInlineObjectRegistry.h>
#include <KoToolProxy.h>

// KDE + Qt includes
#include <QHBoxLayout>
#include <QTextDocument>
#include <QTimer>
#include <QPrinter>
#include <klocale.h>
#include <kdebug.h>
#include <kicon.h>
#include <kactioncollection.h>
#include <kactionmenu.h>

KWView::KWView( const QString& viewMode, KWDocument* document, QWidget *parent )
    : KoView( document, parent )
{
    m_document = document;
    m_snapToGrid = m_document->gridData().snapToGrid();
    m_gui = new KWGui( viewMode, this );
    m_canvas = m_gui->canvas();
    actionCollection()->setAssociatedWidget(m_canvas);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_gui);

    setInstance( KWFactory::instance() );
    setXMLFile( "kword.rc" );

    m_currentPage = m_document->pageManager()->page(m_document->startPage());

    setupActions();

    KoToolBoxFactory toolBoxFactory( "KWord" );
    createDockWidget( &toolBoxFactory );
    KoShapeSelectorFactory shapeSelectorFactory;
    createDockWidget( &shapeSelectorFactory );
    KoToolDockerFactory toolDockerFactory;
    KoToolDocker *d =  dynamic_cast<KoToolDocker*>( createDockWidget( &toolDockerFactory ) );
    m_gui->setToolOptionDocker( d );

    connect( kwcanvas()->shapeManager()->selection(), SIGNAL( selectionChanged() ), this, SLOT( selectionChanged() ) );
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

    m_actionViewZoom = new KoZoomAction( modes, i18n("Zoom"), this );
    actionCollection()->addAction("view_zoom", m_actionViewZoom);
    actionCollection()->addAction(KStandardAction::ZoomIn,  "zoom_in", m_actionViewZoom, SLOT(zoomIn()));
    actionCollection()->addAction(KStandardAction::ZoomOut,  "zoom_out", m_actionViewZoom, SLOT(zoomOut()));
    m_zoomHandler.setZoomAndResolution( 100, KoGlobal::dpiX(), KoGlobal::dpiY() );
    m_zoomHandler.setZoomMode( m_document->zoomMode() );
    m_zoomHandler.setZoom( m_document->zoom() );
    updateZoomControls();
    connect( m_actionViewZoom, SIGNAL( zoomChanged( KoZoomMode::Mode, int ) ),
            this, SLOT( viewZoom( KoZoomMode::Mode, int ) ) );

    m_actionFormatFrameSet  = new KAction(i18n("Frame/Frameset Properties"), this);
    actionCollection()->addAction("format_frameset", m_actionFormatFrameSet );
    m_actionFormatFrameSet->setToolTip( i18n( "Alter frameset properties" ) );
    m_actionFormatFrameSet->setEnabled( false );
    connect(m_actionFormatFrameSet, SIGNAL(triggered()), this, SLOT(editFrameProperties()));

    QAction *print  = new KAction("MyPrint", this);
    actionCollection()->addAction("file_my_print", print );
    connect(print, SIGNAL(triggered()), this, SLOT(print()));

    m_actionFormatBold  = new KToggleAction(KIcon("text_bold"), i18n("Bold"), this);
    actionCollection()->addAction("format_bold", m_actionFormatBold );
    m_actionFormatBold->setShortcut(KShortcut(Qt::CTRL + Qt::Key_B));
    connect( m_actionFormatBold, SIGNAL(toggled(bool)), this, SLOT(textBold(bool)) );

    m_actionFormatItalic  = new KToggleAction(KIcon("text_italic"), i18n("Italic"), this);
    actionCollection()->addAction("format_italic", m_actionFormatItalic );
    m_actionFormatItalic->setShortcut(KShortcut( Qt::CTRL + Qt::Key_I));
    connect( m_actionFormatBold, SIGNAL(toggled(bool)), this, SLOT(textItalic(bool)) );

    m_actionFormatUnderline  = new KToggleAction(KIcon("text_under"), i18n("Underline"), this);
    actionCollection()->addAction("format_underline", m_actionFormatUnderline );
    m_actionFormatUnderline->setShortcut(KShortcut( Qt::CTRL + Qt::Key_U));
    connect( m_actionFormatUnderline, SIGNAL(toggled(bool)), this, SLOT(textUnderline(bool)) );

    m_actionFormatStrikeOut  = new KToggleAction(KIcon("text_strike"), i18n("Strike Out"), this);
    actionCollection()->addAction("format_strike", m_actionFormatStrikeOut );
    connect( m_actionFormatStrikeOut, SIGNAL(toggled(bool)), this, SLOT(textStrikeOut(bool)) );

    // ------------------- Actions with a key binding and no GUI item
    KAction *action  = new KAction(i18n("Insert Non-Breaking Space"), this);
    actionCollection()->addAction("nonbreaking_space", action );
    action->setShortcut( KShortcut( Qt::CTRL+Qt::Key_Space));
    connect(action, SIGNAL(triggered()), this, SLOT( slotNonbreakingSpace() ));

    action  = new KAction(i18n("Insert Non-Breaking Hyphen"), this);
    actionCollection()->addAction("nonbreaking_hyphen", action );
    action->setShortcut( KShortcut( Qt::CTRL+Qt::SHIFT+Qt::Key_Minus));
    connect(action, SIGNAL(triggered()), this, SLOT( slotNonbreakingHyphen() ));

    action  = new KAction(i18n("Insert Soft Hyphen"), this);
    actionCollection()->addAction("soft_hyphen", action );
    action->setShortcut( KShortcut( Qt::CTRL+Qt::Key_Minus));
    connect(action, SIGNAL(triggered()), this, SLOT( slotSoftHyphen() ));

    action  = new KAction(i18n("Line Break"), this);
    actionCollection()->addAction("line_break", action );
    action->setShortcut( KShortcut( Qt::SHIFT+Qt::Key_Return));
    connect(action, SIGNAL(triggered()), this, SLOT( slotLineBreak() ));

    m_actionInsertFrameBreak  = new KAction(QString::null, this);
    actionCollection()->addAction("insert_framebreak", m_actionInsertFrameBreak );
    m_actionInsertFrameBreak->setShortcut( KShortcut( Qt::CTRL + Qt::Key_Return));
    connect(m_actionInsertFrameBreak, SIGNAL(triggered()), this, SLOT( insertFrameBreak() ));
    //if ( m_document->processingType() == KWDocument::WP ) {
        m_actionInsertFrameBreak->setText( i18n( "Page Break" ) );
        m_actionInsertFrameBreak->setToolTip( i18n( "Force the remainder of the text into the next page" ) );
        m_actionInsertFrameBreak->setWhatsThis( i18n( "This inserts a non-printing character at the current cursor position. All text after this point will be moved into the next page." ) );
    /*} else {
        m_actionInsertFrameBreak->setText( i18n( "Hard Frame Break" ) );
        m_actionInsertFrameBreak->setToolTip( i18n( "Force the remainder of the text into the next frame" ) );
        m_actionInsertFrameBreak->setWhatsThis( i18n( "This inserts a non-printing character at the current cursor position. All text after this point will be moved into the next frame in the frameset." ) );
    } */

    m_actionFormatFont  = new KAction(i18n("Font..."), this);
    actionCollection()->addAction("format_font", m_actionFormatFont );
    m_actionInsertFrameBreak->setShortcut( KShortcut( Qt::ALT + Qt::CTRL + Qt::Key_F));
    m_actionFormatFont->setToolTip( i18n( "Change character size, font, boldface, italics etc." ) );
    m_actionFormatFont->setWhatsThis( i18n( "Change the attributes of the currently selected characters." ) );
    connect(m_actionFormatFont, SIGNAL(triggered()), this, SLOT( formatFont() ));

    m_actionEditDelFrame  = new KAction(i18n("Delete Frame"), this);
    actionCollection()->addAction("edit_delframe", m_actionEditDelFrame );
    m_actionEditDelFrame->setToolTip( i18n( "Delete the currently selected frame(s)" ) );
    m_actionEditDelFrame->setWhatsThis( i18n( "Delete the currently selected frame(s)." ) );
    connect(m_actionEditDelFrame, SIGNAL(triggered()), this, SLOT( editDeleteFrame() ));

    m_actionViewHeader  = new KToggleAction(i18n("Enable Document Headers"), this);
    actionCollection()->addAction("format_header", m_actionViewHeader );
    m_actionViewHeader->setCheckedState(KGuiItem(i18n("Disable Document Headers")));
    m_actionViewHeader->setToolTip( i18n( "Shows and hides header display" ) );
    m_actionViewHeader->setWhatsThis( i18n( "Selecting this option toggles the display of headers in KWord.<br><br>Headers are special frames at the top of each page which can contain page numbers or other information." ) );
    connect(m_actionViewHeader, SIGNAL(triggered()), this, SLOT( toggleHeader() ));

    m_actionViewFooter  = new KToggleAction(i18n("Enable Document Footers"), this);
    actionCollection()->addAction("format_footer", m_actionViewFooter );
    m_actionViewFooter->setCheckedState(KGuiItem(i18n("Disable Document Footers")));
    m_actionViewFooter->setToolTip( i18n( "Shows and hides footer display" ) );
    m_actionViewFooter->setWhatsThis( i18n( "Selecting this option toggles the display of footers in KWord. <br><br>Footers are special frames at the bottom of each page which can contain page numbers or other information." ) );
    connect(m_actionViewFooter, SIGNAL(triggered()), this, SLOT( toggleFooter() ));

    m_actionViewSnapToGrid = new KToggleAction(i18n("Snap to Grid"), this);
    actionCollection()->addAction("view_snaptogrid", m_actionViewSnapToGrid);
    m_actionViewSnapToGrid->setChecked(m_snapToGrid);
    connect(m_actionViewSnapToGrid, SIGNAL(triggered()), this, SLOT( toggleSnapToGrid() ));

    m_actionRaiseFrame  = new KAction(KIcon("raise"), i18n("Raise Frame"), this);
    actionCollection()->addAction("raiseframe", m_actionRaiseFrame );
    m_actionRaiseFrame->setShortcut( KShortcut(Qt::CTRL +Qt::SHIFT+ Qt::Key_R) );
    m_actionRaiseFrame->setToolTip( i18n( "Raise the currently selected frame so that it appears above "
        "all the other frames" ) );
    m_actionRaiseFrame->setWhatsThis( i18n( "Raise the currently selected frame so that it appears "
        "above all the other frames. This is only useful if frames overlap each other. If multiple "
        "frames are selected they are all raised in turn." ) );
    connect(m_actionRaiseFrame, SIGNAL(triggered()), this, SLOT( raiseFrame() ));

    m_actionLowerFrame  = new KAction(KIcon("lower"), i18n("Lower Frame"), this);
    actionCollection()->addAction("lowerframe", m_actionLowerFrame );
    m_actionLowerFrame->setShortcut(KShortcut(Qt::CTRL +Qt::SHIFT+ Qt::Key_L));
    m_actionLowerFrame->setToolTip( i18n( "Lower the currently selected frame so that it disappears under "
        "any frame that overlaps it" ) );
    m_actionLowerFrame->setWhatsThis( i18n( "Lower the currently selected frame so that it disappears under "
        "any frame that overlaps it. If multiple frames are selected they are all lowered in turn." ) );
    connect(m_actionLowerFrame, SIGNAL(triggered()), this, SLOT( lowerFrame() ));

    m_actionBringToFront = new KAction(KIcon("bring_forward"), i18n("Bring to Front"), this);
    actionCollection()->addAction("bring_tofront_frame", m_actionBringToFront);
    connect(m_actionBringToFront, SIGNAL(triggered()), this, SLOT( bringToFront() ));

    m_actionSendBackward = new KAction(KIcon("send_backward"), i18n("Send to Back"), this);
    actionCollection()->addAction("send_toback_frame", m_actionSendBackward);
    connect(m_actionSendBackward, SIGNAL(triggered()), this, SLOT( sendToBack() ));

    KActionMenu *actionMenu = new KActionMenu(i18n("Variable"), this);
    foreach(QAction *action, KoInlineObjectRegistry::instance()->createInsertVariableActions(kwcanvas()))
        actionMenu->addAction(action);
    actionCollection()->addAction("insert_variable", actionMenu);

/* ********** From old kwview ****
We probably want to have each of these again, so just move them when you want to implement it
This saves problems with finding out which we missed near the end.

    // -------------- File menu
    m_actionExtraCreateTemplate = new KAction( i18n( "Create Template From Document..." ), 0,
    this, SLOT( extraCreateTemplate() ),
    actionCollection(), "extra_template" );
    m_actionExtraCreateTemplate->setToolTip( i18n( "Save this document and use it later as a template" ) );
    m_actionExtraCreateTemplate->setWhatsThis( i18n( "You can save this document as a template.<br><br>You can use this new template as a starting point for another document." ) );

    m_actionFileStatistics  = new KAction(i18n("Statistics"), this);
    actionCollection()->addAction("file_statistics", m_actionFileStatistics );
    m_actionFileStatistics->setToolTip( i18n( "Sentence, word and letter counts for this document" ) );
    m_actionFileStatistics->setWhatsThis( i18n( "Information on the number of letters, words, syllables and sentences for this document.<p>Evaluates readability using the Flesch reading score." ) );
    // -------------- Edit actions
    m_actionEditCut = actionCollection()->addAction(KStandardAction::Cut,  "edit_cut", this, SLOT( editCut() ));
    m_actionEditCopy = actionCollection()->addAction(KStandardAction::Copy,  "edit_copy", this, SLOT( editCopy() ));
    m_actionEditPaste = actionCollection()->addAction(KStandardAction::Paste,  "edit_paste", this, SLOT( editPaste() ));
    m_actionEditFind = actionCollection()->addAction(KStandardAction::Find,  "edit_find", this, SLOT( editFind() ));
    m_actionEditFindNext = actionCollection()->addAction(KStandardAction::FindNext,  "edit_findnext", this, SLOT( editFindNext() ));
    m_actionEditFindPrevious = actionCollection()->addAction(KStandardAction::FindPrev,  "edit_findprevious", this, SLOT( editFindPrevious() ));
    m_actionEditReplace = actionCollection()->addAction(KStandardAction::Replace,  "edit_replace", this, SLOT( editReplace() ));
    m_actionEditSelectAll = actionCollection()->addAction(KStandardAction::SelectAll,  "edit_selectall", this, SLOT( editSelectAll() ));
    new KAction( i18n( "Select All Frames" ), 0, this, SLOT( editSelectAllFrames() ), actionCollection(), "edit_selectallframes" );
    m_actionEditSelectCurrentFrame = new KAction( i18n( "Select Frame" ), 0,
    0, this, SLOT( editSelectCurrentFrame() ),
    actionCollection(), "edit_selectcurrentframe" );
    m_actionSpellCheck = actionCollection()->addAction(KStandardAction::Spelling,  "extra_spellcheck", this, SLOT( slotSpellCheck() ));
    m_actionDeletePage = new KAction( i18n( "Delete Page" ), "delslide", 0,
    this, SLOT( deletePage() ),
    actionCollection(), "delete_page" );
    kDebug(32003) <<  m_doc->pageCount() <<  " " << (m_doc->processingType() == KWDocument::DTP) << endl;

    (void) new KAction( i18n( "Configure Mail Merge..." ), "configure",0,
    this, SLOT( editMailMergeDataBase() ),
    actionCollection(), "edit_sldatabase" );


    QAction * mailMergeLabelAction = new KWMailMergeLabelAction::KWMailMergeLabelAction(
    i18n("Drag Mail Merge Variable"), actionCollection(), "mailmerge_draglabel" );
    connect( mailMergeLabelAction, SIGNAL( triggered(bool) ), this, SLOT(editMailMergeDataBase()) );

    //    (void) new KWMailMergeComboAction::KWMailMergeComboAction(i18n("Insert Mailmerge Var"),0,this,SLOT(JWJWJW()),actionCollection(),"mailmerge_varchooser");

    // -------------- Frame menu
    m_actionCreateLinkedFrame  = new KAction(i18n("Create Linked Copy"), this);
    actionCollection()->addAction("create_linked_frame", m_actionCreateLinkedFrame );
    m_actionCreateLinkedFrame->setToolTip( i18n( "Create a copy of the current frame, always showing the same contents" ) );
    m_actionCreateLinkedFrame->setWhatsThis( i18n("Create a copy of the current frame, that remains linked to it. This means they always show the same contents: modifying the contents in such a frame will update all its linked copies.") );


    // -------------- View menu

    if ( !m_doc->isEmbedded() ) {

        QActionGroup* viewModeActionGroup = new QActionGroup( this );
        viewModeActionGroup->setExclusive( true );
        m_actionViewTextMode = new KToggleAction( i18n( "Text Mode" ), 0,
                this, SLOT( viewTextMode() ),
                actionCollection(), "view_textmode" );
        m_actionViewTextMode->setToolTip( i18n( "Only show the text of the document" ) );
        m_actionViewTextMode->setWhatsThis( i18n( "Do not show any pictures, formatting or layout. KWord will display only the text for editing." ) );

        m_actionViewTextMode->setActionGroup( viewModeActionGroup );
        m_actionViewPageMode = new KToggleAction( i18n( "Page Mode" ), 0,
                this, SLOT( viewPageMode() ),
                actionCollection(), "view_pagemode" );
        m_actionViewPageMode->setWhatsThis( i18n( "Switch to page mode.<br><br> Page mode is designed to make editing your text easy.<br><br>This function is most frequently used to return to text editing after switching to preview mode." ) );
        m_actionViewPageMode->setToolTip( i18n( "Switch to page editing mode" ) );

        m_actionViewPageMode->setActionGroup( viewModeActionGroup );
        m_actionViewPageMode->setChecked( true );
        m_actionViewPreviewMode = new KToggleAction( i18n( "Preview Mode" ), 0,
                this, SLOT( viewPreviewMode() ),
                actionCollection(), "view_previewmode" );
        m_actionViewPreviewMode->setWhatsThis( i18n( "Zoom out from your document to get a look at several pages of your document.<br><br>The number of pages per line can be customized." ) );
        m_actionViewPreviewMode->setToolTip( i18n( "Zoom out to a multiple page view" ) );

        m_actionViewPreviewMode->setActionGroup( viewModeActionGroup );
    }
    else // no viewmode switching when embedded; at least "Page" makes no sense
    {
        m_actionViewTextMode = 0;
        m_actionViewPageMode = 0;
        m_actionViewPreviewMode = 0;
    }

    m_actionViewFormattingChars = new KToggleAction( i18n( "Formatting Characters" ), 0,
            this, SLOT( slotViewFormattingChars() ),
            actionCollection(), "view_formattingchars" );
    m_actionViewFormattingChars->setToolTip( i18n( "Toggle the display of non-printing characters" ) );
    m_actionViewFormattingChars->setWhatsThis( i18n( "Toggle the display of non-printing characters.<br><br>When this is enabled, KWord shows you tabs, spaces, carriage returns and other non-printing characters." ) );

    m_actionViewFrameBorders = new KToggleAction( i18n( "Frame Borders" ), 0,
            this, SLOT( slotViewFrameBorders() ),
            actionCollection(), "view_frameborders" );
    m_actionViewFrameBorders->setToolTip( i18n( "Turns the border display on and off" ) );
    m_actionViewFrameBorders->setWhatsThis( i18n( "Turns the border display on and off.<br><br>The borders are never printed. This option is useful to see how the document will appear on the printed page." ) );

    // -------------- Insert menu
    m_actionInsertSpecialChar = new KAction( i18n( "Special Character..." ), "char",
            Qt::ALT + Qt::SHIFT + Qt::Key_C,
            this, SLOT( insertSpecialChar() ),
            actionCollection(), "insert_specialchar" );
    m_actionInsertSpecialChar->setToolTip( i18n( "Insert one or more symbols or letters not found on the keyboard" ) );
    m_actionInsertSpecialChar->setWhatsThis( i18n( "Insert one or more symbols or letters not found on the keyboard." ) );

    new KAction( m_doc->processingType() == KWDocument::WP ? i18n( "Page" ) : i18n( "Page..." ), "page", 0,
            this, SLOT( insertPage() ),
            actionCollection(), "insert_page" );

    m_actionInsertLink = new KAction( i18n( "Link..." ), 0,
            this, SLOT( insertLink() ),
            actionCollection(), "insert_link" );
    actionInsertLink->setToolTip( i18n( "Insert a Web address, email address or hyperlink to a file" ) );
    m_actionInsertLink->setWhatsThis( i18n( "Insert a Web address, email address or hyperlink to a file." ) );

    m_actionInsertComment = new KAction( i18n( "Comment..." ), 0,
            this, SLOT( insertComment() ),
            actionCollection(), "insert_comment" );
    m_actionInsertComment->setToolTip( i18n( "Insert a comment about the selected text" ) );
    m_actionInsertComment->setWhatsThis( i18n( "Insert a comment about the selected text. These comments are not designed to appear on the final page." ) );

    m_actionEditComment = new KAction( i18n("Edit Comment..."), 0,
            this,SLOT(editComment()),
            actionCollection(), "edit_comment");
    m_actionEditComment->setToolTip( i18n( "Change the content of a comment" ) );
    m_actionEditComment->setWhatsThis( i18n( "Change the content of a comment." ) );

    m_actionRemoveComment = new KAction( i18n("Remove Comment"), 0,
            this,SLOT(removeComment()),
            actionCollection(), "remove_comment");
    m_actionRemoveComment->setToolTip( i18n( "Remove the selected document comment" ) );
    m_actionRemoveComment->setWhatsThis( i18n( "Remove the selected document comment." ) );
    m_actionCopyTextOfComment = new KAction( i18n("Copy Text of Comment..."), 0,
            this,SLOT(copyTextOfComment()),
            actionCollection(), "copy_text_comment");


    m_actionInsertFootEndNote = new KAction( i18n( "Footnote/Endnote..." ), 0,
            this, SLOT( insertFootNote() ),
            actionCollection(), "insert_footendnote" );
    m_actionInsertFootEndNote->setToolTip( i18n( "Insert a footnote referencing the selected text" ) );
    m_actionInsertFootEndNote->setWhatsThis( i18n( "Insert a footnote referencing the selected text." ) );

    m_actionInsertContents = new KAction( i18n( "Table of Contents" ), 0,
            this, SLOT( insertContents() ),
            actionCollection(), "insert_contents" );
    m_actionInsertContents->setToolTip( i18n( "Insert table of contents at the current cursor position" ) );
    m_actionInsertContents->setWhatsThis( i18n( "Insert table of contents at the current cursor position." ) );

    actionInsertVariable = new KActionMenu( i18n( "Variable" ),
            actionCollection(), "insert_variable" );

    // The last argument is only needed if a submenu is to be created
    addVariableActions( VT_FIELD, KoFieldVariable::actionTexts(), actionInsertVariable, i18n("Document Information") );
    addVariableActions( VT_DATE, KoDateVariable::actionTexts(), actionInsertVariable, i18n("Date") );
    addVariableActions( VT_TIME, KoTimeVariable::actionTexts(), actionInsertVariable, i18n("Time") );
    addVariableActions( VT_PGNUM, KoPageVariable::actionTexts(), actionInsertVariable, i18n("Page") );
    addVariableActions( VT_STATISTIC, KWStatisticVariable::actionTexts(), actionInsertVariable, i18n("Statistic") );

    m_actionInsertCustom = new KActionMenu( i18n( "Custom" ),
            actionCollection(), "insert_custom" );
    actionInsertVariable->insert(m_actionInsertCustom);

    //addVariableActions( VT_CUSTOM, KWCustomVariable::actionTexts(), actionInsertVariable, QString::null );

    addVariableActions( VT_MAILMERGE, KoMailMergeVariable::actionTexts(), actionInsertVariable, QString::null );

    actionInsertVariable->popupMenu()->insertSeparator();
    m_actionRefreshAllVariable = new KAction( i18n( "Refresh All Variables" ), 0,
            this, SLOT( refreshAllVariable() ),
            actionCollection(), "refresh_all_variable" );
    m_actionRefreshAllVariable->setToolTip( i18n( "Update all variables to current values" ) );
    m_actionRefreshAllVariable->setWhatsThis( i18n( "Update all variables in the document to current values.<br><br>This will update page numbers, dates or any other variables that need updating." ) );

    actionInsertVariable->insert(m_actionRefreshAllVariable);

    m_actionInsertExpression = new KActionMenu( i18n( "Expression" ),
            actionCollection(), "insert_expression" );
    loadexpressionActions( m_actionInsertExpression);

    m_actionInsertFile = new KAction( i18n( "File..." ), 0,
            this, SLOT( insertFile() ),
            actionCollection(), "insert_file" );


    // ------------------------- Format menu
    m_actionFormatParag = new KAction( i18n( "Paragraph..." ), Qt::ALT + Qt::CTRL + Qt::Key_P,
            this, SLOT( formatParagraph() ),
            actionCollection(), "format_paragraph" );
    m_actionFormatParag->setToolTip( i18n( "Change paragraph margins, text flow, borders, bullets, numbering etc." ) );
    m_actionFormatParag->setWhatsThis( i18n( "Change paragraph margins, text flow, borders, bullets, numbering etc.<p>Select text in multiple paragraphs to change the formatting of all selected paragraphs.<p>If no text is selected, the paragraph where the cursor is located will be changed." ) );

    m_actionFormatFrameSet = new KAction( i18n( "Frame/Frameset Properties" ), 0,
            this, SLOT( formatFrameSet() ),
            actionCollection(), "format_frameset" );
    m_actionFormatFrameSet->setToolTip( i18n( "Alter frameset properties" ) );
    m_actionFormatFrameSet->setWhatsThis( i18n( "Alter frameset properties.<p>Currently you can change the frame background." ) );

    m_actionFormatPage = new KAction( i18n( "Page Layout..." ), 0,
            this, SLOT( formatPage() ),
            actionCollection(), "format_page" );
    m_actionFormatPage->setToolTip( i18n( "Change properties of entire page" ) );
    m_actionFormatPage->setWhatsThis( i18n( "Change properties of the entire page.<p>Currently you can change paper size, paper orientation, header and footer sizes, and column settings." ) );


    m_actionFormatFrameStylist = new KAction( i18n( "Frame Style Manager" ), 0,
            this, SLOT( extraFrameStylist() ),
            actionCollection(), "frame_stylist" );
    m_actionFormatFrameStylist->setToolTip( i18n( "Change attributes of framestyles" ) );
    m_actionFormatFrameStylist->setWhatsThis( i18n( "Change background and borders of framestyles.<p>Multiple framestyles can be changed using the dialog box." ) );


    m_actionFormatStylist = new KAction( i18n( "Style Manager" ), Qt::ALT + Qt::CTRL + Qt::Key_S,
            this, SLOT( extraStylist() ),
            actionCollection(), "format_stylist" );
    m_actionFormatStylist->setToolTip( i18n( "Change attributes of styles" ) );
    m_actionFormatStylist->setWhatsThis( i18n( "Change font and paragraph attributes of styles.<p>Multiple styles can be changed using the dialog box." ) );

    m_actionFormatFontSize = new KFontSizeAction( i18n( "Font Size" ), actionCollection(), "format_fontsize" );
    connect( m_actionFormatFontSize, SIGNAL( fontSizeChanged( int ) ),
            this, SLOT( textSizeSelected( int ) ) );

    m_actionFontSizeIncrease  = new KAction(i18n("Increase Font Size"), "fontsizeup"), this);
    actionCollection()->addAction("increase_fontsize", m_actionFontSizeIncrease );
    m_actionFontSizeDecrease  = new KAction(i18n("Decrease Font Size"), "fontsizedown"), this);
    actionCollection()->addAction("decrease_fontsize", m_actionFontSizeDecrease );

    m_actionFormatFontFamily = new KFontAction( KFontChooser::SmoothScalableFonts,
            //i18n( "Font Family" ),
            actionCollection(), "format_fontfamily" );
    connect( m_actionFormatFontFamily, SIGNAL( triggered( const QString & ) ),
            this, SLOT( textFontSelected( const QString & ) ) );

    m_actionFormatStyleMenu  = new KActionMenu(i18n("Style"), this);
    actionCollection()->addAction("format_stylemenu", m_actionFormatStyleMenu );
    m_actionFormatStyle  = new KSelectAction(i18n("Style"), this);
    actionCollection()->addAction("format_style", m_actionFormatStyle );
    // In fact, binding a key to this action will simply re-apply the current style. Why not.
    //m_actionFormatStyle->setShortcutConfigurable( false );
    connect( m_actionFormatStyle, SIGNAL( activated( int ) ),
            this, SLOT( textStyleSelected( int ) ) );
    updateStyleList();

    m_actionFormatDefault=new KAction( i18n( "Default Format" ), 0,
            this, SLOT( textDefaultFormat() ),
            actionCollection(), "text_default" );
    m_actionFormatDefault->setToolTip( i18n( "Change font and paragraph attributes to their default values" ) );
    m_actionFormatDefault->setWhatsThis( i18n( "Change font and paragraph attributes to their default values." ) );

    // ----------------------- More format actions, for the toolbar only

    QActionGroup* alignActionGroup = new QActionGroup( this );
    alignActionGroup->setExclusive( true );
    m_actionFormatAlignLeft = new KToggleAction( i18n( "Align Left" ), "text_left", Qt::CTRL + Qt::Key_L,
            this, SLOT( textAlignLeft() ),
            actionCollection(), "format_alignleft" );
    m_actionFormatAlignLeft->setActionGroup( alignActionGroup );
    m_actionFormatAlignLeft->setChecked( true );
    m_actionFormatAlignCenter = new KToggleAction( i18n( "Align Center" ), "text_center", Qt::CTRL + Qt::ALT + Qt::Key_C,
            this, SLOT( textAlignCenter() ),
            actionCollection(), "format_aligncenter" );
    m_actionFormatAlignCenter->setActionGroup( alignActionGroup );
    m_actionFormatAlignRight = new KToggleAction( i18n( "Align Right" ), "text_right", Qt::CTRL + Qt::ALT + Qt::Key_R,
            this, SLOT( textAlignRight() ),
            actionCollection(), "format_alignright" );
    m_actionFormatAlignRight->setActionGroup( alignActionGroup );
    m_actionFormatAlignBlock = new KToggleAction( i18n( "Align Block" ), "text_block", Qt::CTRL + Qt::Key_J,
            this, SLOT( textAlignBlock() ),
            actionCollection(), "format_alignblock" );
    m_actionFormatAlignBlock->setActionGroup( alignActionGroup );

    QActionGroup* spacingActionGroup = new QActionGroup( this );
    spacingActionGroup->setExclusive( true );
    m_actionFormatSpacingSingle = new KToggleAction( i18n( "Line Spacing 1" ), "spacesimple", Qt::CTRL + Qt::Key_1,
            this, SLOT( textSpacingSingle() ),
            actionCollection(), "format_spacingsingle" );
    m_actionFormatSpacingSingle->setActionGroup( spacingActionGroup );
    m_actionFormatSpacingOneAndHalf = new KToggleAction( i18n( "Line Spacing 1.5" ), "spacedouble", Qt::CTRL + Qt::Key_5,
            this, SLOT( textSpacingOneAndHalf() ),
            actionCollection(), "format_spacing15" );
    m_actionFormatSpacingOneAndHalf->setActionGroup( spacingActionGroup );
    m_actionFormatSpacingDouble = new KToggleAction( i18n( "Line Spacing 2" ), "spacetriple", Qt::CTRL + Qt::Key_2,
            this, SLOT( textSpacingDouble() ),
            actionCollection(), "format_spacingdouble" );
    m_actionFormatSpacingDouble->setActionGroup( spacingActionGroup );

    m_actionFormatSuper = new KToggleAction( i18n( "Superscript" ), "super", 0,
            this, SLOT( textSuperScript() ),
            actionCollection(), "format_super" );
    m_actionFormatSub = new KToggleAction( i18n( "Subscript" ), "sub", 0,
            this, SLOT( textSubScript() ),
            actionCollection(), "format_sub" );

    m_actionFormatIncreaseIndent= new KAction( i18n( "Increase Indent" ),
            QApplication::isRightToLeft() ? "format_decreaseindent" : "format_increaseindent", 0,
            this, SLOT( textIncreaseIndent() ),
            actionCollection(), "format_increaseindent" );

    m_actionFormatDecreaseIndent= new KAction( i18n( "Decrease Indent" ),
            QApplication::isRightToLeft() ? "format_increaseindent" :"format_decreaseindent", 0,
            this, SLOT( textDecreaseIndent() ),
            actionCollection(), "format_decreaseindent" );

    m_actionFormatColor = new TKSelectColorAction( i18n( "Text Color..." ), TKSelectColorAction::TextColor,
            this, SLOT( textColor() ),
            actionCollection(), "format_color", true );
    m_actionFormatColor->setDefaultColor(QColor());


    m_actionFormatNumber  = new KActionMenu(KIcon( "enumList" ), i18n("Number"), this);
    actionCollection()->addAction("format_number", m_actionFormatNumber );
    m_actionFormatNumber->setDelayed( false );
    m_actionFormatBullet  = new KActionMenu(KIcon( "unsortedList" ), i18n("Bullet"), this);
    actionCollection()->addAction("format_bullet", m_actionFormatBullet );
    m_actionFormatBullet->setDelayed( false );
    QActionGroup* counterStyleActionGroup = new QActionGroup( this );
    counterStyleActionGroup->setExclusive( true );
    Q3PtrList<KoCounterStyleWidget::StyleRepresenter> stylesList;
    KoCounterStyleWidget::makeCounterRepresenterList( stylesList );
    Q3PtrListIterator<KoCounterStyleWidget::StyleRepresenter> styleIt( stylesList );
    for ( ; styleIt.current() ; ++styleIt ) {
        // Dynamically create toggle-actions for each list style.
        // This approach allows to edit toolbars and extract separate actions from this menu
        KToggleAction* act = new KToggleAction( styleIt.current()->name(), // TODO icon
                actionCollection(),
                QString("counterstyle_%1").arg( styleIt.current()->style() ) );
        connect( act, SIGNAL( triggered(bool) ), this, SLOT( slotCounterStyleSelected() ) );
        act->setActionGroup( counterStyleActionGroup );
        // Add to the right menu: both for "none", bullet for bullets, numbers otherwise
        if ( styleIt.current()->style() == KoParagCounter::STYLE_NONE ) {
            m_actionFormatBullet->insert( act );
            m_actionFormatNumber->insert( act );
        } else if ( styleIt.current()->isBullet() )
            m_actionFormatBullet->insert( act );
        else
            m_actionFormatNumber->insert( act );
    }

    // ---------------------------- frame toolbar actions

    m_actionFrameStyleMenu = new KActionMenu( i18n( "Framestyle" ),
            actionCollection(), "frame_stylemenu" );
    m_actionFrameStyle = new KSelectAction( i18n( "Framestyle" ),
            actionCollection(), "frame_style" );
    connect( m_actionFrameStyle, SIGNAL( activated( int ) ),
            this, SLOT( frameStyleSelected( int ) ) );
    updateFrameStyleList();
    m_actionBorderOutline = new KToggleAction( KIcon("borderoutline"), i18n( "Border Outline" ),
            actionCollection(), "border_outline" );
    connect( m_actionBorderOutline, SIGNAL( triggered(bool) ), this, SLOT( borderOutline() ) );
    m_actionBorderLeft = new KToggleAction( KIcon("borderleft"), i18n( "Border Left" ),
            actionCollection(), "border_left" );
    connect( m_actionBorderLeft, SIGNAL( triggered(bool) ), this, SLOT( borderLeft() ) );
    m_actionBorderRight = new KToggleAction( KIcon("borderright"), i18n( "Border Right" ),
            actionCollection(), "border_right" );
    connect( m_actionBorderRight, SIGNAL( triggered(bool) ), this, SLOT( borderRight() ) );
    m_actionBorderTop = new KToggleAction( KIcon("bordertop"), i18n( "Border Top" ),
            actionCollection(), "border_top" );
    connect( m_actionBorderTop, SIGNAL( triggered(bool) ), this, SLOT( borderTop() ) );
    m_actionBorderBottom = new KToggleAction( KIcon("borderbottom"), i18n( "Border Bottom" ),
            actionCollection(), "border_bottom" );
    connect( m_actionBorderBottom, SIGNAL( triggered(bool) ), this, SLOT( borderBottom() ) );
    m_actionBorderStyle = new KSelectAction( i18n( "Border Style" ),
            actionCollection(), "border_style" );

    QStringList lst;
    lst << KoBorder::getStyle( KoBorder::SOLID );
    lst << KoBorder::getStyle( KoBorder::DASH );
    lst << KoBorder::getStyle( KoBorder::DOT );
    lst << KoBorder::getStyle( KoBorder::DASH_DOT );
    lst << KoBorder::getStyle( KoBorder::DASH_DOT_DOT );
    lst << KoBorder::getStyle( KoBorder::DOUBLE_LINE );
    m_actionBorderStyle->setItems( lst );
    m_actionBorderWidth = new KSelectAction( i18n( "Border Width" ),
            actionCollection(), "border_width" );
    lst.clear();
    for ( unsigned int i = 1; i < 10; i++ )
        lst << QString::number( i );
    m_actionBorderWidth->setItems( lst );
    m_actionBorderWidth->setCurrentItem( 0 );

    m_actionBorderColor = new TKSelectColorAction( i18n("Border Color"), TKSelectColorAction::LineColor, actionCollection(), "border_color", true );
    m_actionBorderColor->setDefaultColor(QColor());


    m_actionBackgroundColor = new TKSelectColorAction( i18n( "Text Background Color..." ), TKSelectColorAction::FillColor, actionCollection(),"border_backgroundcolor", true);
    m_actionBackgroundColor->setToolTip( i18n( "Change background color for currently selected text" ) );
    m_actionBackgroundColor->setWhatsThis( i18n( "Change background color for currently selected text." ) );

    connect(m_actionBackgroundColor,SIGNAL(activated()),SLOT(backgroundColor() ));
    m_actionBackgroundColor->setDefaultColor(QColor());

    // ---------------------- Table menu
    m_actionTablePropertiesMenu = new KAction( i18n( "Properties" ), 0,
            this, SLOT( tableProperties() ),
            actionCollection(), "table_propertiesmenu" );
    m_actionTablePropertiesMenu->setToolTip( i18n( "Adjust properties of the current table" ) );
    m_actionTablePropertiesMenu->setWhatsThis( i18n( "Adjust properties of the current table." ) );

    m_actionTableInsertRow = new KAction( i18n( "Insert Row..." ), "insert_table_row", 0,
            this, SLOT( tableInsertRow() ),
            actionCollection(), "table_insrow" );
    m_actionTableInsertRow->setToolTip( i18n( "Insert one or more rows at cursor location" ) );
    m_actionTableInsertRow->setWhatsThis( i18n( "Insert one or more rows at current cursor location." ) );

    m_actionTableInsertCol = new KAction( i18n( "Insert Column..." ), "insert_table_col", 0,
            this, SLOT( tableInsertCol() ),
            actionCollection(), "table_inscol" );
    m_actionTableInsertCol->setToolTip( i18n( "Insert one or more columns into the current table" ) );
    m_actionTableInsertCol->setWhatsThis( i18n( "Insert one or more columns into the current table." ) );

    m_actionTableDelRow = new KAction( 0, "delete_table_row", 0,
            this, SLOT( tableDeleteRow() ),
            actionCollection(), "table_delrow" );
    m_actionTableDelRow->setToolTip( i18n( "Delete selected rows from the current table" ) );
    m_actionTableDelRow->setWhatsThis( i18n( "Delete selected rows from the current table." ) );

    m_actionTableDelCol = new KAction( 0, "delete_table_col", 0,
            this, SLOT( tableDeleteCol() ),
            actionCollection(), "table_delcol" );
    m_actionTableDelCol->setToolTip( i18n( "Delete selected columns from the current table" ) );
    m_actionTableDelCol->setWhatsThis( i18n( "Delete selected columns from the current table." ) );

    m_actionTableResizeCol = new KAction( i18n( "Resize Column..." ), 0,
            this, SLOT( tableResizeCol() ),
            actionCollection(), "table_resizecol" );
    m_actionTableResizeCol->setToolTip( i18n( "Change the width of the currently selected column" ) );
    m_actionTableResizeCol->setWhatsThis( i18n( "Change the width of the currently selected column." ) );


    m_actionTableJoinCells = new KAction( i18n( "Join Cells" ), 0,
            this, SLOT( tableJoinCells() ),
            actionCollection(), "table_joincells" );
    m_actionTableJoinCells->setToolTip( i18n( "Join two or more cells into one large cell" ) );
    m_actionTableJoinCells->setWhatsThis( i18n( "Join two or more cells into one large cell.<p>This is a good way to create titles and labels within a table." ) );

    m_actionTableSplitCells= new KAction( i18n( "Split Cell..." ), 0,
            this, SLOT( tableSplitCells() ),
            actionCollection(), "table_splitcells" );
    m_actionTableSplitCells->setToolTip( i18n( "Split one cell into two or more cells" ) );
    m_actionTableSplitCells->setWhatsThis( i18n( "Split one cell into two or more cells.<p>Cells can be split horizontally, vertically or both directions at once." ) );

    m_actionTableProtectCells= new KToggleAction( i18n( "Protect Cells" ), 0, 0, 0,
            actionCollection(), "table_protectcells" );
    m_actionTableProtectCells->setToolTip( i18n( "Prevent changes to content of selected cells" ) );
    connect (m_actionTableProtectCells, SIGNAL( toggled(bool) ), this,
            SLOT( tableProtectCells(bool) ));

    m_actionTableProtectCells->setWhatsThis( i18n( "Toggles cell protection on and off.<br><br>When cell protection is on, the user can not alter the content or formatting of the text within the cell." ) );

    m_actionTableDelete = new KAction( i18n( "Delete Table" ), 0,
            this, SLOT( tableDelete() ),
            actionCollection(), "table_delete" );
    m_actionTableDelete->setToolTip( i18n( "Delete the entire table" ) );
    m_actionTableDelete->setWhatsThis( i18n( "Deletes all cells and the content within the cells of the currently selected table." ) );


    m_actionTableStylist = new KAction( i18n( "Table Style Manager" ), 0,
            this, SLOT( tableStylist() ),
            actionCollection(), "table_stylist" );
    m_actionTableStylist->setToolTip( i18n( "Change attributes of tablestyles" ) );
    m_actionTableStylist->setWhatsThis( i18n( "Change textstyle and framestyle of the tablestyles.<p>Multiple tablestyles can be changed using the dialog box." ) );

    m_actionTableStyleMenu = new KActionMenu( i18n( "Tablestyle" ),
            actionCollection(), "table_stylemenu" );
    m_actionTableStyle = new KSelectAction( i18n( "Tablestyle" ),
            actionCollection(), "table_style" );
    connect( m_actionTableStyle, SIGNAL( activated( int ) ),
            this, SLOT( tableStyleSelected( int ) ) );
    updateTableStyleList();

    m_actionConvertTableToText = new KAction( i18n( "Convert Table to Text" ), 0,
            this, SLOT( convertTableToText() ),
            actionCollection(), "convert_table_to_text" );
    m_actionSortText= new KAction( i18n( "Sort Text..." ), 0,
            this, SLOT( sortText() ),
            actionCollection(), "sort_text" );

    m_actionAddPersonalExpression= new KAction( i18n( "Add Expression" ), 0,
            this, SLOT( addPersonalExpression() ),
            actionCollection(), "add_personal_expression" );


    // ---------------------- Tools menu


    m_actionAllowAutoFormat = new KToggleAction( i18n( "Enable Autocorrection" ), 0,
            this, SLOT( slotAllowAutoFormat() ),
            actionCollection(), "enable_autocorrection" );
    m_actionAllowAutoFormat->setCheckedState(i18n("Disable Autocorrection"));
    m_actionAllowAutoFormat->setToolTip( i18n( "Toggle autocorrection on and off" ) );
    m_actionAllowAutoFormat->setWhatsThis( i18n( "Toggle autocorrection on and off." ) );

    m_actionAutoFormat = new KAction( i18n( "Configure Autocorrection..." ), 0,
            this, SLOT( extraAutoFormat() ),
            actionCollection(), "configure_autocorrection" );
    m_actionAutoFormat->setToolTip( i18n( "Change autocorrection options" ) );
    m_actionAutoFormat->setWhatsThis( i18n( "Change autocorrection options including:<p> <UL><LI><P>exceptions to autocorrection</P> <LI><P>add/remove autocorrection replacement text</P> <LI><P>and basic autocorrection options</P>." ) );

    m_actionEditCustomVarsEdit = new KAction( i18n( "Custom Variables..." ), 0,
            this, SLOT( editCustomVars() ), // TODO: new dialog w add etc.
            actionCollection(), "custom_vars" );

    m_actionEditPersonnalExpr=new KAction( i18n( "Edit Personal Expressions..." ), 0,
            this, SLOT( editPersonalExpr() ),
            actionCollection(), "personal_expr" );
    m_actionEditPersonnalExpr->setToolTip( i18n( "Add or change one or more personal expressions" ) );
    m_actionEditPersonnalExpr->setWhatsThis( i18n( "Add or change one or more personal expressions.<p>Personal expressions are a way to quickly insert commonly used phrases or text into your document." ) );

    m_actionChangeCase=new KAction( i18n( "Change Case..." ), 0,
            this, SLOT( changeCaseOfText() ),
            actionCollection(), "change_case" );
    m_actionChangeCase->setToolTip( i18n( "Alter the capitalization of selected text" ) );
    m_actionChangeCase->setWhatsThis( i18n( "Alter the capitalization of selected text to one of five pre-defined patterns.<p>You can also switch all letters from upper case to lower case and from lower case to upper case in one move." ) );

    //------------------------ Settings menu
    m_actionConfigure = actionCollection()->addAction(KStandardAction::Preferences,  "configure", this, SLOT(configure()));

    //------------------------ Menu frameSet
    QAction *actionChangePicture=new KAction( i18n( "Change Picture..." ),"frame_image",0,
            this, SLOT( changePicture() ),
            actionCollection(), "change_picture" );
    actionChangePicture->setToolTip( i18n( "Change the picture in the currently selected frame" ) );
    actionChangePicture->setWhatsThis( i18n( "You can specify a different picture in the current frame.<br><br>KWord automatically resizes the new picture to fit within the old frame." ) );

    m_actionConfigureHeaderFooter=new KAction( i18n( "Configure Header/Footer..." ), 0,
            this, SLOT( configureHeaderFooter() ),
            actionCollection(), "configure_headerfooter" );
    m_actionConfigureHeaderFooter->setToolTip( i18n( "Configure the currently selected header or footer" ) );
    m_actionConfigureHeaderFooter->setWhatsThis( i18n( "Configure the currently selected header or footer." ) );

    m_actionInlineFrame = new KToggleAction( i18n( "Inline Frame" ), 0,
            this, SLOT( inlineFrame() ),
            actionCollection(), "inline_frame" );
    m_actionInlineFrame->setToolTip( i18n( "Convert current frame to an inline frame" ) );
    m_actionInlineFrame->setWhatsThis( i18n( "Convert the current frame to an inline frame.<br><br>Place the inline frame within the text at the point nearest to the frames current position." ) );

    m_actionOpenLink = new KAction( i18n( "Open Link" ), 0,
            this, SLOT( openLink() ),
            actionCollection(), "open_link" );
    m_actionOpenLink->setToolTip( i18n( "Open the link with the appropriate application" ) );
    m_actionOpenLink->setWhatsThis( i18n( "Open the link with the appropriate application.<br><br>Web addresses are opened in a browser.<br>Email addresses begin a new message addressed to the link.<br>File links are opened by the appropriate viewer or editor." ) );

    m_actionChangeLink=new KAction( i18n("Change Link..."), 0,
            this,SLOT(changeLink()),
            actionCollection(), "change_link");
    m_actionChangeLink->setToolTip( i18n( "Change the content of the currently selected link" ) );
    m_actionChangeLink->setWhatsThis( i18n( "Change the details of the currently selected link." ) );

    m_actionCopyLink = new KAction( i18n( "Copy Link" ), 0,
            this, SLOT( copyLink() ),
            actionCollection(), "copy_link" );

    m_actionAddLinkToBookmak = new KAction( i18n( "Add to Bookmark" ), 0,
            this, SLOT( addToBookmark() ),
            actionCollection(), "add_to_bookmark" );

    m_actionRemoveLink = new KAction( i18n( "Remove Link" ), 0,
            this, SLOT( removeLink() ),
            actionCollection(), "remove_link" );

    m_actionShowDocStruct = new KToggleAction( i18n( "Show Doc Structure" ), 0,
            this, SLOT( showDocStructure() ),
            actionCollection(), "show_docstruct" );
    m_actionShowDocStruct->setCheckedState(i18n("Hide Doc Structure"));
    m_actionShowDocStruct->setToolTip( i18n( "Open document structure sidebar" ) );
    m_actionShowDocStruct->setWhatsThis( i18n( "Open document structure sidebar.<p>This sidebar helps you organize your document and quickly find pictures, tables etc." ) );

    m_actionShowRuler = new KToggleAction( i18n( "Show Rulers" ), 0,
            this, SLOT( showRuler() ),
            actionCollection(), "show_ruler" );
    m_actionShowRuler->setCheckedState(i18n("Hide Rulers"));
    m_actionShowRuler->setToolTip( i18n( "Shows or hides rulers" ) );
    m_actionShowRuler->setWhatsThis( i18n("The rulers are the white measuring spaces top and left of the "
                "document. The rulers show the position and width of pages and of frames and can "
                "be used to position tabulators among others.<p>Uncheck this to disable "
                "the rulers from being displayed." ) );

    m_actionViewShowGrid = new KToggleAction( i18n( "Show Grid" ), 0,
            this, SLOT( viewGrid() ),
            actionCollection(), "view_grid" );
    m_actionViewShowGrid->setCheckedState(i18n("Hide Grid"));

    m_actionConfigureCompletion = new KAction( i18n( "Configure Completion..." ), 0,
            this, SLOT( configureCompletion() ),
            actionCollection(), "configure_completion" );
    m_actionConfigureCompletion->setToolTip( i18n( "Change the words and options for autocompletion" ) );
    m_actionConfigureCompletion->setWhatsThis( i18n( "Add words or change the options for autocompletion." ) );


    new KAction( i18n( "Completion" ), KStdAccel::shortcut(KStdAccel::TextCompletion), this, SLOT( slotCompletion() ), actionCollection(), "completion" );

    new KAction( i18n( "Increase Numbering Level" ), Qt::ALT+Qt::Key_Right,
            this, SLOT( slotIncreaseNumberingLevel() ), actionCollection(), "increase_numbering_level" );
    new KAction( i18n( "Decrease Numbering Level" ), Qt::ALT+Qt::Key_Left,
            this, SLOT( slotDecreaseNumberingLevel() ), actionCollection(), "decrease_numbering_level" );


    // --------
    m_actionEditCustomVars = new KAction( i18n( "Edit Variable..." ), 0,
            this, SLOT( editCustomVariable() ),
            actionCollection(), "edit_customvars" );
    m_actionApplyAutoFormat= new KAction( i18n( "Apply Autocorrection" ), 0,
            this, SLOT( applyAutoFormat() ),
            actionCollection(), "apply_autoformat" );
    m_actionApplyAutoFormat->setToolTip( i18n( "Manually force KWord to scan the entire document and apply autocorrection" ) );
    m_actionApplyAutoFormat->setWhatsThis( i18n( "Manually force KWord to scan the entire document and apply autocorrection." ) );

    m_actionCreateStyleFromSelection = new KAction( i18n( "Create Style From Selection..." ), 0,
            this, SLOT( createStyleFromSelection()),
            actionCollection(), "create_style" );
    m_actionCreateStyleFromSelection->setToolTip( i18n( "Create a new style based on the currently selected text" ) );
    m_actionCreateStyleFromSelection->setWhatsThis( i18n( "Create a new style based on the currently selected text." ) ); // ## "on the current paragraph, taking the formatting from where the cursor is. Selecting text isn't even needed."

    m_actionConfigureFootEndNote = new KAction( i18n( "Footnote..." ), 0,
            this, SLOT( configureFootEndNote()),
            actionCollection(), "format_footendnote" );
    m_actionConfigureFootEndNote->setToolTip( i18n( "Change the look of footnotes" ) );
    m_actionConfigureFootEndNote->setWhatsThis( i18n( "Change the look of footnotes." ) );

    m_actionEditFootEndNote= new KAction( i18n("Edit Footnote"), 0,
            this, SLOT( editFootEndNote()),
            actionCollection(), "edit_footendnote" );
    m_actionEditFootEndNote->setToolTip( i18n( "Change the content of the currently selected footnote" ) );
    m_actionEditFootEndNote->setWhatsThis( i18n( "Change the content of the currently selected footnote." ) );


    m_actionChangeFootNoteType = new KAction( i18n("Change Footnote/Endnote Parameter"), 0,
            this, SLOT( changeFootNoteType() ),
            actionCollection(), "change_footendtype");

    m_actionSavePicture= new KAction( i18n("Save Picture As..."), 0,
            this, SLOT( savePicture() ),
            actionCollection(), "save_picture");
    m_actionSavePicture->setToolTip( i18n( "Save the picture in a separate file" ) );
    m_actionSavePicture->setWhatsThis( i18n( "Save the picture in the currently selected frame in a separate file, outside the KWord document." ) );

    m_actionAllowBgSpellCheck = new KToggleAction( i18n( "Autospellcheck" ), 0,
            this, SLOT( autoSpellCheck() ),
            actionCollection(), "tool_auto_spellcheck" );


    m_actionGoToFootEndNote = new KAction( QString::null, //set dynamically
            0, this, SLOT( goToFootEndNote() ),
            actionCollection(), "goto_footendnote" );

    m_actionAddBookmark= new KAction( i18n( "Bookmark..." ), 0,
            this, SLOT( addBookmark() ),
            actionCollection(), "add_bookmark" );
    m_actionSelectBookmark= new KAction( i18n( "Select Bookmark..." ), 0,
            this, SLOT( selectBookmark() ),
            actionCollection(), "select_bookmark" );

    m_actionImportStyle= new KAction( i18n( "Import Styles..." ), 0,
            this, SLOT( importStyle() ),
            actionCollection(), "import_style" );

    m_actionCreateFrameStyle = new KAction( i18n( "Create Framestyle From Frame..." ), 0,
            this, SLOT( createFrameStyle()),
            actionCollection(), "create_framestyle" );
    m_actionCreateFrameStyle->setToolTip( i18n( "Create a new style based on the currently selected frame" ) );
    m_actionCreateFrameStyle->setWhatsThis( i18n( "Create a new framestyle based on the currently selected frame." ) );

    m_actionConvertToTextBox = new KAction( i18n( "Convert to Text Box" ), 0,
            this, SLOT( convertToTextBox() ),
            actionCollection(), "convert_to_text_box" );

    m_actionSpellIgnoreAll = new KAction( i18n( "Ignore All" ), 0,
            this, SLOT( slotAddIgnoreAllWord() ),
            actionCollection(), "ignore_all" );

    m_actionAddWordToPersonalDictionary=new KAction( i18n( "Add Word to Dictionary" ),0,
            this, SLOT( addWordToDictionary() ),
            actionCollection(), "add_word_to_dictionary" );

    m_actionEmbeddedStoreInternal=new KToggleAction( i18n( "Store Document Internally" ),0,
            this, SLOT( embeddedStoreInternal() ),
            actionCollection(), "embedded_store_internal" );

*/
}

void KWView::setZoom( int zoom ) {
    m_zoomHandler.setZoom( zoom );
    m_document->setZoom( zoom ); // for persistency reasons

    if( m_actionViewZoom ) {
        m_actionViewZoom->setEffectiveZoom( zoom );
    }

    //if ( statusBar() )
    //    m_sbZoomLabel->setText( ' ' + QString::number( zoom ) + "% " );

    // Also set the zoom in KoView (for embedded views)
    //kDebug(32003) << "KWView::setZoom " << zoom << " setting koview zoom to " << m_zoomHandler.zoomedResolutionY() << endl;
    kwcanvas()->updateSize();
    m_gui->updateRulers();
}

void KWView::viewZoom( KoZoomMode::Mode mode, int zoom )
{
    //kDebug(32003) << " viewZoom '" << KoZoomMode::toString( mode ) << ", " << zoom << "'" << endl;

    if ( !m_currentPage )
        return;

    int newZoom = zoom;

    if ( mode == KoZoomMode::ZOOM_WIDTH ) {
        m_zoomHandler.setZoomMode(KoZoomMode::ZOOM_WIDTH);
        newZoom = qRound( static_cast<double>(m_gui->viewportSize().width() * 100 ) /
                (m_zoomHandler.resolutionX() * m_currentPage->width() ) ) - 1;

        if(newZoom != m_zoomHandler.zoomInPercent() || m_gui->horizontalScrollBarVisible()) {
            // we have to do this twice to take into account a possibly appearing vertical scrollbar
            QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
        }
    }
    else if ( mode == KoZoomMode::ZOOM_PAGE ) {
        m_zoomHandler.setZoomMode(KoZoomMode::ZOOM_PAGE);
        double height = m_zoomHandler.resolutionY() * m_currentPage->height();
        double width = m_zoomHandler.resolutionX() * m_currentPage->width();
        QSize viewport = m_gui->viewportSize();
        newZoom = qMin( qRound( static_cast<double>(viewport.height() * 100 ) / height ),
                     qRound( static_cast<double>(viewport.width()* 100 ) / width ) ) - 1;
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
        case KoZoomMode::ZOOM_PIXELS:
            kWarning(32003) << "Illegal zoommode!\n";
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

void KWView::showEvent(QShowEvent *event) {
    Q_UNUSED(event);
    QTimer::singleShot( 1000, this, SLOT( updateZoom() ) );
}

void KWView::editFrameProperties() {
    QList<KWFrame*> frames;
    foreach(KoShape *shape, kwcanvas()->shapeManager()->selection()->selectedShapes()) {
        KoShape *parent = shape;
        while(parent->parent())
            parent = parent->parent();
        KWFrame *frame = dynamic_cast<KWFrame*> (parent->applicationData());
        Q_ASSERT(frame);
        frames.append(frame);
    }
    KWFrameDialog *frameDialog = new KWFrameDialog(frames, m_document, this);
    frameDialog->exec();
    delete frameDialog;
}

// -------------------- Actions -----------------------
void KWView::print() {
// options;
//   DPI
//   pages
//   fontEmbeddingEnabled();
//   duplex
const bool clipToPage=false; // should become a setting in the GUI

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
    KoInsets bleed = m_document->pageManager()->padding();
    const int bleedOffset = (int) (clipToPage?0:POINT_TO_INCH(-bleed.left * resolution));
    const int bleedWidth = (int) (clipToPage?0:POINT_TO_INCH((bleed.left + bleed.right) * resolution));
    const int bleedHeigt = (int) (clipToPage?0:POINT_TO_INCH((bleed.top + bleed.bottom) * resolution));
    for(int pageNum=m_document->startPage(); pageNum <= lastPage; pageNum++) {
        KWPage *page = m_document->pageManager()->page(pageNum);
        // Note that Qt does not at this time allow us to alter the page size to an arbitairy size
        const int pageOffset = qRound(POINT_TO_INCH( resolution * page->offsetInDocument()));
        painter.save();

        painter.translate(0, -pageOffset);
        double width = page->width();
        int clipHeight = (int) POINT_TO_INCH( resolution * page->height());
        int clipWidth = (int) POINT_TO_INCH( resolution * page->width());
        int offset = bleedOffset;
        if(page->pageSide() == KWPage::PageSpread) { // left part
            width /= 2;
            clipWidth /= 2;
            painter.setClipRect(offset, pageOffset, clipWidth + bleedWidth, clipHeight + bleedHeigt);
            m_canvas->shapeManager()->paint( painter, zoomer, true );
            printer.newPage();
            painter.translate(-clipWidth, 0);
            pageNum++;
            offset += clipWidth;
        }
        painter.setClipRect(offset, pageOffset, clipWidth + bleedWidth, clipHeight + bleedHeigt);
        m_canvas->shapeManager()->paint( painter, zoomer, true );

        painter.restore();

        if(pageNum != lastPage)
            printer.newPage();
    }

    painter.end();
}

void KWView::textBold(bool bold) {
    KoTextSelectionHandler *handler = qobject_cast<KoTextSelectionHandler*> (kwcanvas()->toolProxy()->selection());
    if(handler)
        handler->bold(bold);
}

void KWView::textItalic(bool italic) {
    KoTextSelectionHandler *handler = qobject_cast<KoTextSelectionHandler*> (kwcanvas()->toolProxy()->selection());
    if(handler)
        handler->italic(italic);
}

void KWView::textUnderline(bool underline) {
    KoTextSelectionHandler *handler = qobject_cast<KoTextSelectionHandler*> (kwcanvas()->toolProxy()->selection());
    if(handler)
        handler->underline(underline);
}

void KWView::textStrikeOut(bool strikeout) {
    KoTextSelectionHandler *handler = qobject_cast<KoTextSelectionHandler*> (kwcanvas()->toolProxy()->selection());
    if(handler)
        handler->strikeOut(strikeout);
}

void KWView::slotNonbreakingSpace() {
    KoTextSelectionHandler *handler = qobject_cast<KoTextSelectionHandler*> (kwcanvas()->toolProxy()->selection());
    if(handler)
        handler->insert(QString(QChar(0xa0)));
}

void KWView::slotNonbreakingHyphen() {
    KoTextSelectionHandler *handler = qobject_cast<KoTextSelectionHandler*> (kwcanvas()->toolProxy()->selection());
    if(handler)
        handler->insert(QString(QChar(0x2013)));
}

void KWView::slotSoftHyphen() {
    KoTextSelectionHandler *handler = qobject_cast<KoTextSelectionHandler*> (kwcanvas()->toolProxy()->selection());
    if(handler)
        handler->insert(QString(QChar(0xad)));
}

void KWView::slotLineBreak() {
    KoTextSelectionHandler *handler = qobject_cast<KoTextSelectionHandler*> (kwcanvas()->toolProxy()->selection());
    if(handler)
        handler->insert(QString(QChar('\n')));
}

void KWView::insertFrameBreak() {
    KoTextSelectionHandler *handler = qobject_cast<KoTextSelectionHandler*> (kwcanvas()->toolProxy()->selection());
    if(handler)
        handler->insertFrameBreak();
}

void KWView::formatFont() {
    KoTextSelectionHandler *handler = qobject_cast<KoTextSelectionHandler*> (kwcanvas()->toolProxy()->selection());
    if(handler)
        handler->selectFont(this);
}

void KWView::editDeleteFrame() {
    QUndoCommand *cmd = kwcanvas()->shapeController()->removeShapes(
            kwcanvas()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection));
    m_document->addCommand(cmd);
}

void KWView::toggleHeader() {
    KWPageSettings pageSettings = m_document->pageSettings();
    if(m_currentPage->pageNumber() == m_document->startPage()) { // first page
        if(pageSettings.firstHeader() == KWord::HFTypeNone)
            pageSettings.setFirstHeaderPolicy(KWord::HFTypeEvenOdd);
        else
            pageSettings.setFirstHeaderPolicy(KWord::HFTypeNone);
    }
    else {
        if(pageSettings.headers() == KWord::HFTypeNone)
            pageSettings.setHeaderPolicy(KWord::HFTypeEvenOdd);
        else
            pageSettings.setHeaderPolicy(KWord::HFTypeNone);
    }
    m_document->setPageSettings(pageSettings);
}

void KWView::toggleFooter() {
    KWPageSettings pageSettings = m_document->pageSettings();
    if(m_currentPage->pageNumber() == m_document->startPage()) { // first page
        if(pageSettings.firstFooter() == KWord::HFTypeNone)
            pageSettings.setFirstFooterPolicy(KWord::HFTypeEvenOdd);
        else
            pageSettings.setFirstFooterPolicy(KWord::HFTypeNone);
    }
    else {
        if(pageSettings.footers() == KWord::HFTypeNone)
            pageSettings.setFooterPolicy(KWord::HFTypeEvenOdd);
        else
            pageSettings.setFooterPolicy(KWord::HFTypeNone);
    }
    m_document->setPageSettings(pageSettings);
}

void KWView::toggleSnapToGrid() {
    m_snapToGrid = !m_snapToGrid;
    m_document->gridData().setSnapToGrid(m_snapToGrid); // for persistency
}

void KWView::adjustZOrderOfSelectedFrames(KoShapeReorderCommand::MoveShapeType direction) {
    QUndoCommand *cmd = KoShapeReorderCommand::createCommand(kwcanvas()->shapeManager()->selection()->selectedShapes(),
        kwcanvas()->shapeManager(), direction);
    if(cmd)
        m_document->addCommand(cmd);
}

// end of actions


void KWView::selectionChanged()
{
  bool shapeSelected = kwcanvas()->shapeManager()->selection()->count() > 0;

  m_actionFormatFrameSet->setEnabled( shapeSelected );
}

void KWView::editUndo() {
    KoShape *shape = m_canvas->shapeManager()->selection()->firstSelectedShape();
    if(shape) {
        // first see if we have a text shape that we are editing and direct undo there.
        KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (shape->userData());
        if(data && data->document()->isUndoAvailable()) {
            data->document()->undo();
            return;
        }
    }
    emit undo();
}

void KWView::editRedo() {
    KoShape *shape = m_canvas->shapeManager()->selection()->firstSelectedShape();
    if(shape) {
        // first see if we have a text shape that we are editing and direct undo there.
        KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (shape->userData());
        if(data && data->document()->isRedoAvailable()) {
            data->document()->redo();
            return;
        }
    }
    emit redo();
}

#include "KWView.moc"
