/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA
*/

#undef Unsorted

#include "kwview.h"

#include "KWordViewIface.h"
#include "configfootnotedia.h"
#include "defs.h"
#include "deldia.h"
#include "docstruct.h"
#include "footnotedia.h"
#include "insdia.h"
#include "kwanchor.h"
#include "kwbookmark.h"
#include "kwcanvas.h"
#include "kwcommand.h"
#include "kwconfig.h"
#include "kwcreatebookmarkdia.h"
#include "kwdoc.h"
#include "kwdrag.h"
#include "kweditpersonnalexpressiondia.h"
#include "kwformulaframe.h"
#include "kwframe.h"
#include "kwframestyle.h"
#include "kwframestylemanager.h"
#include "kwimportstyledia.h"
#include "kwinserthorizontallinedia.h"
#include "kwinsertpagedia.h"
#include "kwinsertpicdia.h"
#include "kwpartframeset.h"
#include "kwstylemanager.h"
#include "kwtableframeset.h"
#include "kwtablestyle.h"
#include "kwtablestylemanager.h"
#include "kwtextdocument.h"
#include "kwvariable.h"
#include "kwviewmode.h"
#include "mailmerge.h"
#include "mailmerge_actions.h"
#include "resizetabledia.h"
#include "searchdia.h"
#include "sortdia.h"
#include "splitcellsdia.h"
#include "tabledia.h"
#include "paragvisitors.h"
#include "kwoasisloader.h"

#include <kformuladocument.h>
#include <kformulamimesource.h>

#include <korichtext.h>
#include <koAutoFormat.h>
#include <koAutoFormatDia.h>
#include <koChangeCaseDia.h>
#include <koCharSelectDia.h>
#include <koCommentDia.h>
#include <koCreateStyleDia.h>
#include <koDocumentInfo.h>
#include <koFontDia.h>
#include <koFrame.h>
#include <koInsertLink.h>
#include <koMainWindow.h>
#include <koParagDia.h>
#include <koPartSelectAction.h>
#include <koPictureFilePreview.h>
#include <koSearchDia.h>
#include <koStore.h>
#include <koStoreDrag.h>
#include <koTemplateCreateDia.h>
#include <kovariable.h>
#include <koVariableDlgs.h>
#include <kotextobject.h>
#include <tkcoloractions.h>

#include <kaccelgen.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kparts/event.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include <kstdaction.h>
#include <ktempfile.h>
#include <kurldrag.h>
#include <kdeversion.h>

#include <qcheckbox.h>
#include <qclipboard.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpaintdevicemetrics.h>
#include <qprogressdialog.h>
#include <qregexp.h>
#include <qtabwidget.h>
#include <qtimer.h>
#include <qvbox.h>
#include <qbuffer.h>

#include <stdlib.h>

#ifdef HAVE_LIBKSPELL2
#include <kspell2/dialog.h>
#include <kspell2/defaultdictionary.h>
#include "kospell.h"
using namespace KSpell2;
#endif

KWView::KWView( KWViewMode* viewMode, QWidget *_parent, const char *_name, KWDocument* _doc )
    : KoView( _doc, _parent, _name )
{
    m_doc = _doc;
    m_gui = 0;

    dcop = 0;
    dcopObject(); // build it
    fsInline=0L;
    m_spell.kospell = 0;
#ifdef HAVE_LIBKSPELL2
    m_spell.dlg = 0;
    m_broker = Broker::openBroker( KSharedConfig::openConfig( "kwordrc" ) );
#endif
    m_spell.macroCmdSpellCheck=0L;
    m_spell.textIterator = 0L;
    m_border.left.color = white;
    m_border.left.setStyle (KoBorder::SOLID);
    m_border.left.setPenWidth( 0);
    m_border.right = m_border.left;
    m_border.top = m_border.left;
    m_border.bottom = m_border.left;
    m_border.common.color = black;
    m_border.common.setStyle(KoBorder::SOLID);
    m_border.common.setPenWidth( 1);
    m_currentPage = 0;
    m_specialCharDlg=0L;
    m_searchEntry = 0L;
    m_replaceEntry = 0L;
    m_findReplace = 0L;
    m_fontDlg = 0L;
    m_paragDlg = 0L;

    m_actionList.setAutoDelete( true );
    m_variableActionList.setAutoDelete( true );
    // Default values.
    m_zoomViewModeNormal = m_doc->zoom();
    m_zoomViewModePreview = 33;
    m_viewFrameBorders = m_doc->viewFrameBorders();
    KoView::setZoom( m_doc->zoomedResolutionY() /* KoView only supports one zoom */ ); // initial value
    //m_viewTableGrid = true;

    setInstance( KWFactory::global() );
    if ( !m_doc->isReadWrite() )
        setXMLFile( "kword_readonly.rc" );
    else
        setXMLFile( "kword.rc" );


    QObject::connect( this, SIGNAL( embeddImage( const QString & ) ),
                      this, SLOT( slotEmbedImage( const QString & ) ) );

    setKeyCompression( TRUE );
    setAcceptDrops( TRUE );

    setupActions();

    m_gui = new KWGUI( viewMode, this, this );
    m_gui->setGeometry( 0, 0, width(), height() );
    m_gui->show();

    KStatusBar * sb = statusBar();
    m_sbPageLabel = 0L;
    if ( sb ) // No statusbar in e.g. konqueror
    {
        m_sbPageLabel = new KStatusBarLabel( QString::null, 0, sb );
        addStatusBarItem( m_sbPageLabel, 0 );
    }
    m_sbFramesLabel = 0L; // Only added when frames are selected

    connect( m_doc, SIGNAL( pageNumChanged() ),
             this, SLOT( pageNumChanged()) );

    connect( m_doc, SIGNAL( pageLayoutChanged( const KoPageLayout& ) ),
             this, SLOT( slotPageLayoutChanged( const KoPageLayout& )) );

    connect( m_doc, SIGNAL( docStructureChanged(int) ),
             this, SLOT( docStructChanged(int)) );

    connect( m_doc, SIGNAL( sig_refreshMenuCustomVariable()),
             this, SLOT( refreshCustomMenu()));

    connect( m_doc, SIGNAL(sig_frameSelectedChanged()),
             this, SLOT( frameSelectedChanged()));

    connect( QApplication::clipboard(), SIGNAL( dataChanged() ),
             this, SLOT( clipboardDataChanged() ) );

    connect( m_gui->canvasWidget(), SIGNAL(currentFrameSetEditChanged()),
             this, SLOT(slotFrameSetEditChanged()) );

    connect( m_gui->canvasWidget(), SIGNAL( currentMouseModeChanged(int) ),
             this, SLOT( showMouseMode(int) ) );

    // Cut and copy are directly connected to the selectionChanged signal
    if ( m_doc->isReadWrite() )
    {
        connect( m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
                 this, SLOT(slotChangeCutState(bool )) );
        connect (m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
                 this, SLOT(slotChangeCaseState(bool )));
    }
    else
    {
        actionEditCut->setEnabled( false );
        actionChangeCase->setEnabled( false );
    }

    connect( m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
             actionEditCopy, SLOT(setEnabled(bool)) );

    //connect (m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
    //         actionCreateStyleFromSelection, SLOT(setEnabled(bool)));

    connect (m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
             actionConvertToTextBox, SLOT(setEnabled(bool)));
    connect (m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
             actionAddPersonalExpression, SLOT(setEnabled(bool )));
    connect (m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
             actionSortText, SLOT(setEnabled(bool )));

    connect( m_gui->canvasWidget(), SIGNAL(frameSelectedChanged()),
             this, SLOT(frameSelectedChanged()));

    connect( m_gui->canvasWidget(), SIGNAL(docStructChanged(int)),
             this, SLOT(docStructChanged(int)));

    connect( m_gui->canvasWidget(), SIGNAL(updateRuler()),
             this, SLOT(slotUpdateRuler()));

    if ( shell() )
    {
        connect( shell(), SIGNAL( documentSaved()), m_doc,SLOT(slotDocumentInfoModifed() ) );
        changeNbOfRecentFiles( m_doc->maxRecentFiles() );
    }

    m_gui->canvasWidget()->updateCurrentFormat();
    setFocusProxy( m_gui->canvasWidget() );

    //when kword is embedded into konqueror apply a zoom=100
    //in konqueror we can't change zoom -- ### TODO ?
    if(!m_doc->isReadWrite())
    {
        setZoom( 100, true );
        slotUpdateRuler();
        initGui();
    }

    // Determine initial scroll position
    // We do this delayed, so that the GUI has been fully constructed
    // (and e.g. the statusbar can repaint).
    QTimer::singleShot( 0, this, SLOT( slotSetInitialPosition() ) );
}

KWView::~KWView()
{
    delete m_tableActionList.first(); // the first one is the separator.
    clearSpellChecker();

    delete m_searchEntry;
    m_searchEntry = 0L;
    delete m_replaceEntry;
    m_replaceEntry = 0L;
    if ( m_specialCharDlg )
        m_specialCharDlg->closeDialog(); // will call slotSpecialCharDlgClosed

    // Abort any find/replace
    delete m_findReplace;
    deselectAllFrames(); // don't let resizehandles hang around
    // Delete gui while we still exist ( it needs documentDeleted() )
    delete m_gui;
    delete m_sbPageLabel;
    delete fsInline;
    delete dcop;
    delete m_fontDlg;
    delete m_paragDlg;
}

DCOPObject* KWView::dcopObject()
{
    if ( !dcop )
	dcop = new KWordViewIface( this );

    return dcop;
}

void KWView::slotChangeCutState(bool b)
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit && edit->textFrameSet()->protectContent())
        actionEditCut->setEnabled( false );
    else
        actionEditCut->setEnabled( b );
}

void KWView::slotChangeCaseState(bool b)
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit && edit->textFrameSet()->protectContent())
        actionChangeCase->setEnabled( false );
    else
        actionChangeCase->setEnabled( b );
}

void KWView::slotSetInitialPosition()
{
    KWTextFrameSetEdit* textedit = dynamic_cast<KWTextFrameSetEdit *>(m_gui->canvasWidget()->currentFrameSetEdit());
    if ( textedit )
        textedit->ensureCursorVisible();
    else
        m_gui->canvasWidget()->setContentsPos( 0, 0 );
}

void KWView::changeNbOfRecentFiles(int _nb)
{
    if ( shell() ) // 0 when embedded into konq !
        shell()->setMaxRecentItems( _nb );
}

void KWView::initGui()
{
    clipboardDataChanged();
    if ( m_gui )
        m_gui->showGUI();
    showMouseMode( KWCanvas::MM_EDIT );
    initGUIButton();
    actionFormatDecreaseIndent->setEnabled(false);
    //setNoteType(m_doc->getNoteType(), false);

    actionFormatColor->setCurrentColor( Qt::black );

    //refresh zoom combobox
    changeZoomMenu( m_doc->zoom() );
    showZoom( m_doc->zoom() );

    // This is probably to emit currentMouseModeChanged and set the cursor
    m_gui->canvasWidget()->setMouseMode( m_gui->canvasWidget()->mouseMode() );

    bool editingFormula = dynamic_cast<KWFormulaFrameSetEdit *>( m_gui->canvasWidget()->currentFrameSetEdit() ) != 0;
    //showFormulaToolbar( FALSE ); // not called, to avoid creating the formula-document if not necessary
    if(shell())
        shell()->showToolbar( "formula_toolbar", editingFormula );

    // Prevention against applyMainWindowSettings hiding the statusbar
    KStatusBar * sb = statusBar();
    if ( sb )
        sb->show();

    updatePageInfo();
    slotFrameSetEditChanged();
    frameSelectedChanged();
    renameButtonTOC(m_doc->isTOC());
    //at the beginning actionBackgroundColor should be active
    actionBackgroundColor->setEnabled(true);
    updateBgSpellCheckingState();
    updateDirectCursorButton();
    actionCreateFrameStyle->setEnabled(false);
}


void KWView::updateBgSpellCheckingState()
{
    actionAllowBgSpellCheck->setChecked( m_doc->backgroundSpellCheckEnabled() );
}


void KWView::initGUIButton()
{
    actionViewFrameBorders->setChecked( viewFrameBorders() );
    actionViewFormattingChars->setChecked( m_doc->viewFormattingChars() );
    actionShowDocStruct->setChecked(m_doc->showdocStruct());
    actionShowRuler->setChecked(m_doc->showRuler());

    updateHeaderFooterButton();
    actionAllowAutoFormat->setChecked( m_doc->allowAutoFormat() );

    QString mode=m_gui->canvasWidget()->viewMode()->type();
    if(mode=="ModePreview")
        actionViewPreviewMode->setChecked(true);
    else if(mode=="ModeText")
        actionViewTextMode->setChecked(true);
    else if(mode=="ModeNormal")
        actionViewPageMode->setChecked(true);
    else
        actionViewPageMode->setChecked(true);
    switchModeView();
}

void KWView::setupActions()
{
    // The actions here are grouped by menu, because this helps noticing
    // accelerator clashes.

    // -------------- File menu
    actionExtraCreateTemplate = new KAction( i18n( "&Create Template From Document..." ), 0,
                                             this, SLOT( extraCreateTemplate() ),
                                             actionCollection(), "extra_template" );
    actionExtraCreateTemplate->setToolTip( i18n( "Save this document and use it later as a template" ) );
    actionExtraCreateTemplate->setWhatsThis( i18n( "You can save this document as a template.<br><br>You can use this new template as a starting point for another document." ) );

    actionFileStatistics = new KAction( i18n( "Statistics" ), 0, this, SLOT( fileStatistics() ), actionCollection(), "file_statistics" );
    actionFileStatistics->setToolTip( i18n( "Sentence, word and letter counts for this document" ) );
    actionFileStatistics->setWhatsThis( i18n( "Information on the number of letters, words, syllables and sentences for this document.<p>Evaluates readability using the Flesch reading score." ) );
    // -------------- Edit actions
    actionEditCut = KStdAction::cut( this, SLOT( editCut() ), actionCollection(), "edit_cut" );
    actionEditCopy = KStdAction::copy( this, SLOT( editCopy() ), actionCollection(), "edit_copy" );
    actionEditPaste = KStdAction::paste( this, SLOT( editPaste() ), actionCollection(), "edit_paste" );
    actionEditFind = KStdAction::find( this, SLOT( editFind() ), actionCollection(), "edit_find" );
    actionEditFindNext = KStdAction::findNext( this, SLOT( editFindNext() ), actionCollection(), "edit_findnext" );
    actionEditFindPrevious = KStdAction::findPrev( this, SLOT( editFindPrevious() ), actionCollection(), "edit_findprevious" );
    actionEditReplace = KStdAction::replace( this, SLOT( editReplace() ), actionCollection(), "edit_replace" );
    actionEditSelectAll = KStdAction::selectAll( this, SLOT( editSelectAll() ), actionCollection(), "edit_selectall" );
    new KAction( i18n( "Select All Frames" ), 0, this, SLOT( editSelectAllFrames() ), actionCollection(), "edit_selectallframes" );
    actionSpellCheck = KStdAction::spelling( this, SLOT( slotSpellCheck() ), actionCollection(), "extra_spellcheck" );
    actionDeletePage = new KAction( i18n( "Delete Page" ), "delslide", 0,
                                    this, SLOT( deletePage() ),
                                    actionCollection(), "delete_page" );
    kdDebug() <<  m_doc->numPages() <<  " " << (m_doc->processingType() == KWDocument::DTP) << endl;

    (void) new KAction( i18n( "Configure Mai&l Merge..." ), "configure",0,
                        this, SLOT( editMailMergeDataBase() ),
                        actionCollection(), "edit_sldatabase" );


    (void) new KWMailMergeLabelAction::KWMailMergeLabelAction( i18n("Drag Mail Merge Variable"), 0,
                    this, SLOT(editMailMergeDataBase()), actionCollection(), "mailmerge_draglabel" );

//    (void) new KWMailMergeComboAction::KWMailMergeComboAction(i18n("Insert Mailmerge Var"),0,this,SLOT(JWJWJW()),actionCollection(),"mailmerge_varchooser");

    // -------------- Frame menu
    actionEditDelFrame = new KAction( i18n( "&Delete Frame" ), 0,
                                      this, SLOT( editDeleteFrame() ),
                                      actionCollection(), "edit_delframe" );
    actionEditDelFrame->setToolTip( i18n( "Delete the currently selected frame(s)." ) );
    actionEditDelFrame->setWhatsThis( i18n( "Delete the currently selected frame(s)." ) );

    actionCreateLinkedFrame = new KAction( i18n( "Create Linked Copy" ), 0, this, SLOT( createLinkedFrame() ), actionCollection(), "create_linked_frame" );
    actionCreateLinkedFrame->setToolTip( i18n( "Create a copy of the current frame, always showing the same contents" ) );
    actionCreateLinkedFrame->setWhatsThis( i18n("Create a copy of the current frame, that remains linked to it. This means they always show the same contents: modifying the contents in such a frame will update all its linked copies.") );

    actionRaiseFrame = new KAction( i18n( "Ra&ise Frame" ), "raise",
                                    CTRL +SHIFT+ Key_R, this, SLOT( raiseFrame() ),
                                    actionCollection(), "raiseframe" );
    actionRaiseFrame->setToolTip( i18n( "Raise the currently selected frame so that it appears above all the other frames" ) );
    actionRaiseFrame->setWhatsThis( i18n( "Raise the currently selected frame so that it appears above all the other frames. This is only useful if frames overlap each other. If multiple frames are selected they are all raised in turn." ) );

    actionLowerFrame = new KAction( i18n( "&Lower Frame" ), "lower",
                                    CTRL +SHIFT+ Key_L, this, SLOT( lowerFrame() ),
                                    actionCollection(), "lowerframe" );
    actionLowerFrame->setToolTip( i18n( "Lower the currently selected frame so that it disappears under any frame that overlaps it" ) );
    actionLowerFrame->setWhatsThis( i18n( "Lower the currently selected frame so that it disappears under any frame that overlaps it. If multiple frames are selected they are all lowered in turn." ) );

    actionBringToFront= new KAction( i18n( "Bring to Front" ), "bring_forward",
                                          0, this, SLOT( bringToFront() ),
                                          actionCollection(), "bring_tofront_frame" );

    actionSendBackward= new KAction( i18n( "Send to Back" ), "send_backward",
                                          0, this, SLOT( sendToBack() ),
                                          actionCollection(), "send_toback_frame" );


    // -------------- View menu
    actionViewTextMode = new KToggleAction( i18n( "Text Mode" ), 0,
                                            this, SLOT( viewTextMode() ),
                                            actionCollection(), "view_textmode" );
    actionViewTextMode->setToolTip( i18n( "Only show the text of the document." ) );
    actionViewTextMode->setWhatsThis( i18n( "Do not show any pictures, formatting or layout. KWord will display only the text for editing." ) );

    actionViewTextMode->setExclusiveGroup( "viewmodes" );
    actionViewPageMode = new KToggleAction( i18n( "&Page Mode" ), 0,
                                            this, SLOT( viewPageMode() ),
                                            actionCollection(), "view_pagemode" );
    actionViewPageMode->setWhatsThis( i18n( "Switch to page mode.<br><br> Page mode is designed to make editing your text easy.<br><br>This function is most frequently used to return to text editing after switching to preview mode." ) );
    actionViewPageMode->setToolTip( i18n( "Switch to page editing mode." ) );

    actionViewPageMode->setExclusiveGroup( "viewmodes" );
    actionViewPageMode->setChecked( true );
    actionViewPreviewMode = new KToggleAction( i18n( "Pre&view Mode" ), 0,
                                            this, SLOT( viewPreviewMode() ),
                                            actionCollection(), "view_previewmode" );
    actionViewPreviewMode->setWhatsThis( i18n( "Zoom out from your document to get a look at several pages of your document.<br><br>The number of pages per line can be customized." ) );
    actionViewPreviewMode->setToolTip( i18n( "Zoom out to a multiple page view." ) );

    actionViewPreviewMode->setExclusiveGroup( "viewmodes" );

    actionViewFormattingChars = new KToggleAction( i18n( "&Formatting Characters" ), 0,
                                                   this, SLOT( slotViewFormattingChars() ),
                                                   actionCollection(), "view_formattingchars" );
    actionViewFormattingChars->setToolTip( i18n( "Toggle the display of non-printing characters." ) );
    actionViewFormattingChars->setWhatsThis( i18n( "Toggle the display of non-printing characters.<br><br>When this is enabled, KWord shows you tabs, spaces, carriage returns and other non-printing characters." ) );

    actionViewFrameBorders = new KToggleAction( i18n( "Frame &Borders" ), 0,
                                                   this, SLOT( slotViewFrameBorders() ),
                                                   actionCollection(), "view_frameborders" );
    actionViewFrameBorders->setToolTip( i18n( "Turns the border display on and off." ) );
    actionViewFrameBorders->setWhatsThis( i18n( "Turns the border display on and off.<br><br>The borders are never printed. This option is useful to see how the document will appear on the printed page." ) );

    actionViewHeader = new KToggleAction( i18n( "Show &Header" ), 0,
                                          this, SLOT( viewHeader() ),
                                          actionCollection(), "format_header" );
 #if KDE_IS_VERSION(3,2,90)
    actionViewHeader->setCheckedState(i18n("Hide &Header"));
 #endif
    actionViewHeader->setToolTip( i18n( "Shows and hides header display." ) );
    actionViewHeader->setWhatsThis( i18n( "Selecting this option toggles the display of headers in KWord.<br><br>Headers are special frames at the top of each page which can contain page numbers or other information." ) );

    actionViewFooter = new KToggleAction( i18n( "Show Foo&ter" ), 0,
                                          this, SLOT( viewFooter() ),
                                          actionCollection(), "format_footer" );
 #if KDE_IS_VERSION(3,2,90)
    actionViewFooter->setCheckedState(i18n("Hide Foo&ter"));
 #endif
    actionViewFooter->setToolTip( i18n( "Shows and hides footer display." ) );
    actionViewFooter->setWhatsThis( i18n( "Selecting this option toggles the display of footers in KWord. <br><br>Footers are special frames at the bottom of each page which can contain page numbers or other information." ) );

    actionViewZoom = new KSelectAction( i18n( "Zoom" ), "viewmag", 0,
                                        actionCollection(), "view_zoom" );

    connect( actionViewZoom, SIGNAL( activated( const QString & ) ),
             this, SLOT( viewZoom( const QString & ) ) );
    actionViewZoom->setEditable(true);
    changeZoomMenu( );

    // -------------- Insert menu
    actionInsertSpecialChar = new KAction( i18n( "Sp&ecial Character..." ), "char",
                        ALT + SHIFT + Key_C,
                        this, SLOT( insertSpecialChar() ),
                        actionCollection(), "insert_specialchar" );
    actionInsertSpecialChar->setToolTip( i18n( "Insert one or more symbols or letters not found on the keyboard." ) );
    actionInsertSpecialChar->setWhatsThis( i18n( "Insert one or more symbols or letters not found on the keyboard." ) );

    actionInsertFrameBreak = new KAction( QString::null, CTRL + Key_Return,
                                          this, SLOT( insertFrameBreak() ),
                                          actionCollection(), "insert_framebreak" );
    if ( m_doc->processingType() == KWDocument::WP ) {
        actionInsertFrameBreak->setText( i18n( "Page Break" ) );
        actionInsertFrameBreak->setToolTip( i18n( "Force the remainder of the text into the next page." ) );
        actionInsertFrameBreak->setWhatsThis( i18n( "This inserts a non-printing character at the current cursor position. All text after this point will be moved into the next page." ) );
    } else {
        actionInsertFrameBreak->setText( i18n( "&Hard Frame Break" ) );
        actionInsertFrameBreak->setToolTip( i18n( "Force the remainder of the text into the next frame." ) );
        actionInsertFrameBreak->setWhatsThis( i18n( "This inserts a non-printing character at the current cursor position. All text after this point will be moved into the next frame in the frameset." ) );
    }

    /*actionInsertPage =*/ new KAction( m_doc->processingType() == KWDocument::WP ? i18n( "Page" ) : i18n( "Page..." ), "page", 0,
                                    this, SLOT( insertPage() ),
                                    actionCollection(), "insert_page" );

    actionInsertLink = new KAction( i18n( "Link..." ), 0,
                                    this, SLOT( insertLink() ),
                                    actionCollection(), "insert_link" );
    actionInsertLink->setToolTip( i18n( "Insert a Web address, email address or hyperlink to a file." ) );
    actionInsertLink->setWhatsThis( i18n( "Insert a Web address, email address or hyperlink to a file." ) );

    actionInsertComment = new KAction( i18n( "Comment..." ), 0,
                                    this, SLOT( insertComment() ),
                                    actionCollection(), "insert_comment" );
    actionInsertComment->setToolTip( i18n( "Insert a comment about the selected text." ) );
    actionInsertComment->setWhatsThis( i18n( "Insert a comment about the selected text. These comments are not designed to appear on the final page." ) );

    actionEditComment = new KAction( i18n("Edit Comment..."), 0,
                                  this,SLOT(editComment()),
                                  actionCollection(), "edit_comment");
    actionEditComment->setToolTip( i18n( "Change the content of a comment." ) );
    actionEditComment->setWhatsThis( i18n( "Change the content of a comment." ) );

    actionRemoveComment = new KAction( i18n("Remove Comment"), 0,
                                     this,SLOT(removeComment()),
                                     actionCollection(), "remove_comment");
    actionRemoveComment->setToolTip( i18n( "Remove the selected document comment." ) );
    actionRemoveComment->setWhatsThis( i18n( "Remove the selected document comment." ) );
    actionCopyTextOfComment = new KAction( i18n("Copy Text of Comment..."), 0,
                                  this,SLOT(copyTextOfComment()),
                                  actionCollection(), "copy_text_comment");


    actionInsertFootEndNote = new KAction( i18n( "&Footnote/Endnote..." ), 0,
                                           this, SLOT( insertFootNote() ),
                                           actionCollection(), "insert_footendnote" );
    actionInsertFootEndNote->setToolTip( i18n( "Insert a footnote referencing the selected text." ) );
    actionInsertFootEndNote->setWhatsThis( i18n( "Insert a footnote referencing the selected text." ) );

    actionInsertContents = new KAction( i18n( "Table of &Contents" ), 0,
                                        this, SLOT( insertContents() ),
                                        actionCollection(), "insert_contents" );
    actionInsertContents->setToolTip( i18n( "Insert table of contents at the current cursor position." ) );
    actionInsertContents->setWhatsThis( i18n( "Insert table of contents at the current cursor position." ) );

    m_variableDefMap.clear();
    actionInsertVariable = new KActionMenu( i18n( "&Variable" ),
                                            actionCollection(), "insert_variable" );

    // The last argument is only needed if a submenu is to be created
    addVariableActions( VT_FIELD, KoFieldVariable::actionTexts(), actionInsertVariable, i18n("Document &Information") );
    addVariableActions( VT_DATE, KoDateVariable::actionTexts(), actionInsertVariable, i18n("&Date") );
    addVariableActions( VT_TIME, KoTimeVariable::actionTexts(), actionInsertVariable, i18n("&Time") );
    addVariableActions( VT_PGNUM, KoPageVariable::actionTexts(), actionInsertVariable, i18n("&Page") );
    addVariableActions( VT_STATISTIC, KWStatisticVariable::actionTexts(), actionInsertVariable, i18n("&Statistic") );

    actionInsertCustom = new KActionMenu( i18n( "&Custom" ),
                                            actionCollection(), "insert_custom" );
    actionInsertVariable->insert(actionInsertCustom);

    //addVariableActions( VT_CUSTOM, KWCustomVariable::actionTexts(), actionInsertVariable, QString::null );

    addVariableActions( VT_MAILMERGE, KoMailMergeVariable::actionTexts(), actionInsertVariable, QString::null );

    actionInsertVariable->popupMenu()->insertSeparator();
    actionRefreshAllVariable = new KAction( i18n( "&Refresh All Variables" ), 0,
                                    this, SLOT( refreshAllVariable() ),
                                    actionCollection(), "refresh_all_variable" );
    actionRefreshAllVariable->setToolTip( i18n( "Update all variables to current values." ) );
    actionRefreshAllVariable->setWhatsThis( i18n( "Update all variables in the document to current values.<br><br>This will update page numbers, dates or any other variables that need updating." ) );

    actionInsertVariable->insert(actionRefreshAllVariable);

    actionInsertExpression = new KActionMenu( i18n( "&Expression" ),
                                            actionCollection(), "insert_expression" );
    loadexpressionActions( actionInsertExpression);

    actionToolsCreateText = new KToggleAction( i18n( "Te&xt Frame" ), "frame_text", Key_F10 /*same as kpr*/,
                                               this, SLOT( toolsCreateText() ),
                                               actionCollection(), "tools_createtext" );
    actionToolsCreateText->setToolTip( i18n( "Create a new text frame." ) );
    actionToolsCreateText->setWhatsThis( i18n( "Create a new text frame." ) );

    actionToolsCreateText->setExclusiveGroup( "tools" );
    actionInsertFormula = new KAction( i18n( "For&mula" ), "frame_formula", Key_F4,
                                       this, SLOT( insertFormula() ),
                                       actionCollection(), "tools_formula" );
    actionInsertFormula->setToolTip( i18n( "Insert a formula into a new frame." ) );
    actionInsertFormula->setWhatsThis( i18n( "Insert a formula into a new frame." ) );

    actionInsertTable = new KAction( i18n( "&Table..." ), "inline_table",
                        Key_F5,
                        this, SLOT( insertTable() ),
                        actionCollection(), "insert_table" );
    actionInsertTable->setToolTip( i18n( "Create a table." ) );
    actionInsertTable->setWhatsThis( i18n( "Create a table.<br><br>The table can either exist in a frame of its own or inline." ) );

    actionToolsCreatePix = new KToggleAction( i18n( "P&icture..." ), "frame_image", // or inline_image ?
                                              SHIFT + Key_F5 /*same as kpr*/,
                                              this, SLOT( insertPicture() ),
                                              actionCollection(), "insert_picture" );
    actionToolsCreatePix->setToolTip( i18n( "Create a new frame for a picture." ) );
    actionToolsCreatePix->setWhatsThis( i18n( "Create a new frame for a picture or diagram." ) );
    actionToolsCreatePix->setExclusiveGroup( "tools" );

    actionToolsCreatePart = new KoPartSelectAction( i18n( "&Object Frame" ), "frame_query",
                                                    this, SLOT( toolsPart() ),
                                                    actionCollection(), "tools_part" );
    actionToolsCreatePart->setToolTip( i18n( "Insert an object into a new frame." ) );
    actionToolsCreatePart->setWhatsThis( i18n( "Insert an object into a new frame." ) );

    actionInsertFile = new KAction( i18n( "Fi&le..." ), 0,
                                   this, SLOT( insertFile() ),
                                   actionCollection(), "insert_file" );


    // ------------------------- Format menu
    actionFormatFont = new KAction( i18n( "&Font..." ), ALT + CTRL + Key_F,
                                    this, SLOT( formatFont() ),
                                    actionCollection(), "format_font" );
    actionFormatFont->setToolTip( i18n( "Change character size, font, boldface, italics etc." ) );
    actionFormatFont->setWhatsThis( i18n( "Change the attributes of the currently selected characters." ) );

    actionFormatParag = new KAction( i18n( "&Paragraph..." ), ALT + CTRL + Key_P,
                                     this, SLOT( formatParagraph() ),
                                     actionCollection(), "format_paragraph" );
    actionFormatParag->setToolTip( i18n( "Change paragraph margins, text flow, borders, bullets, numbering etc." ) );
    actionFormatParag->setWhatsThis( i18n( "Change paragraph margins, text flow, borders, bullets, numbering etc.<p>Select text in multiple paragraphs to change the formatting of all selected paragraphs.<p>If no text is selected, the paragraph where the cursor is located will be changed." ) );

    actionFormatFrameSet = new KAction( i18n( "F&rame/Frameset Properties..." ), 0,
                                     this, SLOT( formatFrameSet() ),
                                     actionCollection(), "format_frameset" );
    actionFormatFrameSet->setToolTip( i18n( "Alter frameset properties." ) );
    actionFormatFrameSet->setWhatsThis( i18n( "Alter frameset properties.<p>Currently you can change the frame background." ) );

    actionFormatPage = new KAction( i18n( "Page &Layout..." ), 0,
                        this, SLOT( formatPage() ),
                        actionCollection(), "format_page" );
    actionFormatPage->setToolTip( i18n( "Change properties of entire page." ) );
    actionFormatPage->setWhatsThis( i18n( "Change properties of the entire page.<p>Currently you can change paper size, paper orientation, header and footer sizes, and column settings." ) );


    actionFormatFrameStylist = new KAction( i18n( "&Frame Style Manager" ), 0 /*shortcut?*/,
                                this, SLOT( extraFrameStylist() ),
                                actionCollection(), "frame_stylist" );
    actionFormatFrameStylist->setToolTip( i18n( "Change attributes of framestyles." ) );
    actionFormatFrameStylist->setWhatsThis( i18n( "Change background and borders of framestyles.<p>Multiple framestyles can be changed using the dialog box." ) );


    actionFormatStylist = new KAction( i18n( "&Style Manager" ), ALT + CTRL + Key_S,
                        this, SLOT( extraStylist() ),
                        actionCollection(), "format_stylist" );
    actionFormatStylist->setToolTip( i18n( "Change attributes of styles." ) );
    actionFormatStylist->setWhatsThis( i18n( "Change font and paragraph attributes of styles.<p>Multiple styles can be changed using the dialog box." ) );

    actionFormatFontSize = new KFontSizeAction( i18n( "Font Size" ), 0,
                                              actionCollection(), "format_fontsize" );
    connect( actionFormatFontSize, SIGNAL( fontSizeChanged( int ) ),
             this, SLOT( textSizeSelected( int ) ) );

    actionFontSizeIncrease = new KAction( i18n("Increase Font Size"), "fontsizeup", CTRL + Key_Greater, this, SLOT( increaseFontSize() ), actionCollection(), "increase_fontsize" );
    actionFontSizeDecrease = new KAction( i18n("Decrease Font Size"), "fontsizedown", CTRL + Key_Less, this, SLOT( decreaseFontSize() ), actionCollection(), "decrease_fontsize" );

#ifdef KFONTACTION_HAS_CRITERIA_ARG
    actionFormatFontFamily = new KFontAction( KFontChooser::SmoothScalableFonts,
                                              i18n( "Font Family" ), 0,
                                              actionCollection(), "format_fontfamily" );
#else
    actionFormatFontFamily = new KFontAction( i18n( "Font Family" ), 0,
                                              actionCollection(), "format_fontfamily" );
#endif
    connect( actionFormatFontFamily, SIGNAL( activated( const QString & ) ),
             this, SLOT( textFontSelected( const QString & ) ) );

    actionFormatStyleMenu = new KActionMenu( i18n( "St&yle" ), 0,
                                           actionCollection(), "format_stylemenu" );
    actionFormatStyle = new KSelectAction( i18n( "St&yle" ), 0,
                                           actionCollection(), "format_style" );
    // In fact, binding a key to this action will simply re-apply the current style. Why not.
    //actionFormatStyle->setShortcutConfigurable( false );
    connect( actionFormatStyle, SIGNAL( activated( int ) ),
             this, SLOT( textStyleSelected( int ) ) );
    updateStyleList();

    actionFormatDefault=new KAction( i18n( "Default Format" ), 0,
                                          this, SLOT( textDefaultFormat() ),
                                          actionCollection(), "text_default" );
    actionFormatDefault->setToolTip( i18n( "Change font and paragraph attributes to their default values." ) );
    actionFormatDefault->setWhatsThis( i18n( "Change font and paragraph attributes to their default values." ) );

    // ----------------------- More format actions, for the toolbar only

    actionFormatBold = new KToggleAction( i18n( "&Bold" ), "text_bold", CTRL + Key_B,
                                           this, SLOT( textBold() ),
                                           actionCollection(), "format_bold" );
    actionFormatItalic = new KToggleAction( i18n( "&Italic" ), "text_italic", CTRL + Key_I,
                                           this, SLOT( textItalic() ),
                                           actionCollection(), "format_italic" );
    actionFormatUnderline = new KToggleAction( i18n( "&Underline" ), "text_under", CTRL + Key_U,
                                           this, SLOT( textUnderline() ),
                                           actionCollection(), "format_underline" );
    actionFormatStrikeOut = new KToggleAction( i18n( "&Strike Out" ), "text_strike", 0 ,
                                           this, SLOT( textStrikeOut() ),
                                           actionCollection(), "format_strike" );

    actionFormatAlignLeft = new KToggleAction( i18n( "Align &Left" ), "text_left", CTRL + Key_L,
                                       this, SLOT( textAlignLeft() ),
                                       actionCollection(), "format_alignleft" );
    actionFormatAlignLeft->setExclusiveGroup( "align" );
    actionFormatAlignLeft->setChecked( TRUE );
    actionFormatAlignCenter = new KToggleAction( i18n( "Align &Center" ), "text_center", CTRL + ALT + Key_C,
                                         this, SLOT( textAlignCenter() ),
                                         actionCollection(), "format_aligncenter" );
    actionFormatAlignCenter->setExclusiveGroup( "align" );
    actionFormatAlignRight = new KToggleAction( i18n( "Align &Right" ), "text_right", CTRL + ALT + Key_R,
                                        this, SLOT( textAlignRight() ),
                                        actionCollection(), "format_alignright" );
    actionFormatAlignRight->setExclusiveGroup( "align" );
    actionFormatAlignBlock = new KToggleAction( i18n( "Align &Block" ), "text_block", CTRL + Key_J,
                                        this, SLOT( textAlignBlock() ),
                                        actionCollection(), "format_alignblock" );
    actionFormatAlignBlock->setExclusiveGroup( "align" );

    actionFormatSuper = new KToggleAction( i18n( "Superscript" ), "super", 0,
                                              this, SLOT( textSuperScript() ),
                                              actionCollection(), "format_super" );
    actionFormatSuper->setExclusiveGroup( "valign" );
    actionFormatSub = new KToggleAction( i18n( "Subscript" ), "sub", 0,
                                              this, SLOT( textSubScript() ),
                                              actionCollection(), "format_sub" );
    actionFormatSub->setExclusiveGroup( "valign" );

    actionFormatIncreaseIndent= new KAction( i18n( "Increase Indent" ),
					     QApplication::reverseLayout() ? "format_decreaseindent" : "format_increaseindent", 0,
                                             this, SLOT( textIncreaseIndent() ),
                                             actionCollection(), "format_increaseindent" );

    actionFormatDecreaseIndent= new KAction( i18n( "Decrease Indent" ),
                                             QApplication::reverseLayout() ? "format_increaseindent" :"format_decreaseindent", 0,
                                             this, SLOT( textDecreaseIndent() ),
                                             actionCollection(), "format_decreaseindent" );

    actionFormatColor = new TKSelectColorAction( i18n( "Text Color..." ), TKSelectColorAction::TextColor,
                                     this, SLOT( textColor() ),
                                     actionCollection(), "format_color", true );
    actionFormatColor->setDefaultColor(QColor());


    //actionFormatList = new KToggleAction( i18n( "List" ), "enumList", 0,
    //                                          this, SLOT( textList() ),
    //                                          actionCollection(), "format_list" );
    //actionFormatList->setExclusiveGroup( "style" );

    actionFormatNumber = new KActionMenu( i18n( "Number" ),
                                          "enumList", actionCollection(), "format_number" );
    actionFormatNumber->setDelayed( false );
    actionFormatBullet = new KActionMenu( i18n( "Bullet" ),
                                          "unsortedList", actionCollection(), "format_bullet" );
    actionFormatBullet->setDelayed( false );
    QPtrList<KoCounterStyleWidget::StyleRepresenter> stylesList;
    KoCounterStyleWidget::makeCounterRepresenterList( stylesList );
    QPtrListIterator<KoCounterStyleWidget::StyleRepresenter> styleIt( stylesList );
    for ( ; styleIt.current() ; ++styleIt ) {
        // Dynamically create toggle-actions for each list style.
        // This approach allows to edit toolbars and extract separate actions from this menu
        KToggleAction* act = new KToggleAction( styleIt.current()->name(), /*TODO icon,*/
                                                0, this, SLOT( slotCounterStyleSelected() ),
                                                actionCollection(), QString("counterstyle_%1").arg( styleIt.current()->style() ).latin1() );
        act->setExclusiveGroup( "counterstyle" );
        // Add to the right menu: both for "none", bullet for bullets, numbers otherwise
        if ( styleIt.current()->style() == KoParagCounter::STYLE_NONE ) {
            actionFormatBullet->insert( act );
            actionFormatNumber->insert( act );
        } else if ( styleIt.current()->isBullet() )
            actionFormatBullet->insert( act );
        else
            actionFormatNumber->insert( act );
    }

    // ---------------------------- frame toolbar actions

    actionFrameStyleMenu = new KActionMenu( i18n( "Fra&mestyle" ), 0,
                                           actionCollection(), "frame_stylemenu" );
    actionFrameStyle = new KSelectAction( i18n( "Framest&yle" ), 0,
                                           actionCollection(), "frame_style" );
    connect( actionFrameStyle, SIGNAL( activated( int ) ),
             this, SLOT( frameStyleSelected( int ) ) );
    updateFrameStyleList();
    actionBorderOutline = new KToggleAction( i18n( "Border Outline" ), "borderoutline",
                            0, this, SLOT( borderOutline() ), actionCollection(), "border_outline" );
    actionBorderLeft = new KToggleAction( i18n( "Border Left" ), "borderleft",
                            0, this, SLOT( borderLeft() ), actionCollection(), "border_left" );
    actionBorderRight = new KToggleAction( i18n( "Border Right" ), "borderright",
                            0, this, SLOT( borderRight() ), actionCollection(), "border_right" );
    actionBorderTop = new KToggleAction( i18n( "Border Top" ), "bordertop",
                            0, this, SLOT( borderTop() ), actionCollection(), "border_top" );
    actionBorderBottom = new KToggleAction( i18n( "Border Bottom" ), "borderbottom",
                            0, this, SLOT( borderBottom() ),  actionCollection(), "border_bottom" );
    actionBorderStyle = new KSelectAction( i18n( "Border Style" ),
                            0,  actionCollection(), "border_style" );
    connect( actionBorderStyle, SIGNAL( activated( const QString & ) ),
             this, SLOT( borderStyle( const QString & ) ) );

    QStringList lst;
    lst << KoBorder::getStyle( KoBorder::SOLID );
    lst << KoBorder::getStyle( KoBorder::DASH );
    lst << KoBorder::getStyle( KoBorder::DOT );
    lst << KoBorder::getStyle( KoBorder::DASH_DOT );
    lst << KoBorder::getStyle( KoBorder::DASH_DOT_DOT );
    lst << KoBorder::getStyle( KoBorder::DOUBLE_LINE );
    actionBorderStyle->setItems( lst );
    actionBorderWidth = new KSelectAction( i18n( "Border Width" ), 0,
                                                 actionCollection(), "border_width" );
    connect( actionBorderWidth, SIGNAL( activated( const QString & ) ),
             this, SLOT( borderWidth( const QString & ) ) );
    lst.clear();
    for ( unsigned int i = 1; i < 10; i++ )
        lst << QString::number( i );
    actionBorderWidth->setItems( lst );

    actionBorderColor = new TKSelectColorAction( i18n("Border Color"), TKSelectColorAction::LineColor, actionCollection(), "border_color", true );
    actionBorderColor->setDefaultColor(QColor());
    connect(actionBorderColor,SIGNAL(activated()),SLOT(borderColor()));


    actionBackgroundColor = new TKSelectColorAction( i18n( "Text Background Color..." ), TKSelectColorAction::FillColor, actionCollection(),"border_backgroundcolor", true);
    actionBackgroundColor->setToolTip( i18n( "Change background color for currently selected text." ) );
    actionBackgroundColor->setWhatsThis( i18n( "Change background color for currently selected text." ) );

    connect(actionBackgroundColor,SIGNAL(activated()),SLOT(backgroundColor() ));
    actionBackgroundColor->setDefaultColor(QColor());

    // ---------------------- Table menu
    actionTablePropertiesMenu = new KAction( i18n( "&Properties..." ), 0,
                               this, SLOT( tableProperties() ),
                               actionCollection(), "table_propertiesmenu" );
    actionTablePropertiesMenu->setToolTip( i18n( "Adjust properties of the current table." ) );
    actionTablePropertiesMenu->setWhatsThis( i18n( "Adjust properties of the current table." ) );

/* TODO: disabled for the moment because I first want a nice icon :-) (09-06-2002)

    actionTableProperties = new KAction( i18n( "&Properties..." ), 0,
                               this, SLOT( tableProperties() ),
                               actionCollection(), "table_properties" );
    actionTableProperties->setToolTip( i18n( "Adjust properties of the current table." ) );
    actionTableProperties->setWhatsThis( i18n( "Adjust properties of the current table." ) );

*/
    actionTableInsertRow = new KAction( i18n( "&Insert Row..." ), "insert_table_row", 0,
                               this, SLOT( tableInsertRow() ),
                               actionCollection(), "table_insrow" );
    actionTableInsertRow->setToolTip( i18n( "Insert one or more rows at cursor location." ) );
    actionTableInsertRow->setWhatsThis( i18n( "Insert one or more rows at current cursor location." ) );

    actionTableInsertCol = new KAction( i18n( "I&nsert Column..." ), "insert_table_col", 0,
                               this, SLOT( tableInsertCol() ),
                               actionCollection(), "table_inscol" );
    actionTableInsertCol->setToolTip( i18n( "Insert one or more columns into the current table." ) );
    actionTableInsertCol->setWhatsThis( i18n( "Insert one or more columns into the current table." ) );

    actionTableDelRow = new KAction( i18n( "&Delete Selected Rows..." ), "delete_table_row", 0,
                                     this, SLOT( tableDeleteRow() ),
                                     actionCollection(), "table_delrow" );
    actionTableDelRow->setToolTip( i18n( "Delete selected rows from the current table." ) );
    actionTableDelRow->setWhatsThis( i18n( "Delete selected rows from the current table." ) );

    actionTableDelCol = new KAction( i18n( "D&elete Selected Columns..." ), "delete_table_col", 0,
                                     this, SLOT( tableDeleteCol() ),
                                     actionCollection(), "table_delcol" );
    actionTableDelCol->setToolTip( i18n( "Delete selected columns from the current table." ) );
    actionTableDelCol->setWhatsThis( i18n( "Delete selected columns from the current table." ) );

    actionTableResizeCol = new KAction( i18n( "Resize Column..." ), 0,
                               this, SLOT( tableResizeCol() ),
                               actionCollection(), "table_resizecol" );
    actionTableResizeCol->setToolTip( i18n( "Change the width of the currently selected column." ) );
    actionTableResizeCol->setWhatsThis( i18n( "Change the width of the currently selected column." ) );


    actionTableJoinCells = new KAction( i18n( "&Join Cells" ), 0,
                                        this, SLOT( tableJoinCells() ),
                                        actionCollection(), "table_joincells" );
    actionTableJoinCells->setToolTip( i18n( "Join two or more cells into one large cell." ) );
    actionTableJoinCells->setWhatsThis( i18n( "Join two or more cells into one large cell.<p>This is a good way to create titles and labels within a table." ) );

    actionTableSplitCells= new KAction( i18n( "&Split Cells..." ), 0,
                                         this, SLOT( tableSplitCells() ),
                                         actionCollection(), "table_splitcells" );
    actionTableSplitCells->setToolTip( i18n( "Split one cell into two or more cells." ) );
    actionTableSplitCells->setWhatsThis( i18n( "Split one cell into two or more cells.<p>Cells can be split horizontally, vertically or both directions at once." ) );

    actionTableProtectCells= new KToggleAction( i18n( "Protect Cells" ), 0,
                                         this, SLOT( tableProtectCells() ),
                                         actionCollection(), "table_protectcells" );
    actionTableProtectCells->setToolTip( i18n( "Prevent changes to content of selected cells." ) );
    actionTableProtectCells->setWhatsThis( i18n( "Toggles cell protection on and off.<br><br>When cell protection is on, the user can not alter the content or formatting of the text within the cell." ) );

    actionTableUngroup = new KAction( i18n( "&Ungroup Table" ), 0,
                                      this, SLOT( tableUngroupTable() ),
                                      actionCollection(), "table_ungroup" );
    actionTableUngroup->setToolTip( i18n( "Break a table into individual frames." ) );
    actionTableUngroup->setWhatsThis( i18n( "Break a table into individual frames<p>Each frame can be moved independently around the page." ) );

    actionTableDelete = new KAction( i18n( "Delete &Table" ), 0,
                                     this, SLOT( tableDelete() ),
                                     actionCollection(), "table_delete" );
    actionTableDelete->setToolTip( i18n( "Delete the entire table." ) );
    actionTableDelete->setWhatsThis( i18n( "Deletes all cells and the content within the cells of the currently selected table." ) );


    actionTableStylist = new KAction( i18n( "T&able Style Manager..." ), 0,
                        this, SLOT( tableStylist() ),
                        actionCollection(), "table_stylist" );
    actionTableStylist->setToolTip( i18n( "Change attributes of tablestyles." ) );
    actionTableStylist->setWhatsThis( i18n( "Change textstyle and framestyle of the tablestyles.<p>Multiple tablestyles can be changed using the dialog box." ) );

    actionTableStyleMenu = new KActionMenu( i18n( "Table&style" ), 0,
                                           actionCollection(), "table_stylemenu" );
    actionTableStyle = new KSelectAction( i18n( "Table&style" ), 0,
                                           actionCollection(), "table_style" );
    connect( actionTableStyle, SIGNAL( activated( int ) ),
             this, SLOT( tableStyleSelected( int ) ) );
    updateTableStyleList();

    actionConvertTableToText = new KAction( i18n( "Convert Table to Text" ), 0,
                                            this, SLOT( convertTableToText() ),
                                            actionCollection(), "convert_table_to_text" );
    actionSortText= new KAction( i18n( "Sort Text..." ), 0,
                                 this, SLOT( sortText() ),
                                 actionCollection(), "sort_text" );

    actionAddPersonalExpression= new KAction( i18n( "Add Expression" ), 0,
                                              this, SLOT( addPersonalExpression() ),
                                              actionCollection(), "add_personal_expression" );


    // ---------------------- Tools menu


    actionAllowAutoFormat = new KToggleAction( i18n( "Enable Autocorrection" ), 0,
                                             this, SLOT( slotAllowAutoFormat() ),
                                          actionCollection(), "enable_autocorrection" );
#if KDE_IS_VERSION(3,2,90)
    actionAllowAutoFormat->setCheckedState(i18n("Disable Autocorrection"));
#endif
    actionAllowAutoFormat->setToolTip( i18n( "Toggle autocorrection on and off." ) );
    actionAllowAutoFormat->setWhatsThis( i18n( "Toggle autocorrection on and off." ) );

    actionAutoFormat = new KAction( i18n( "Configure &Autocorrection..." ), 0,
                        this, SLOT( extraAutoFormat() ),
                        actionCollection(), "configure_autocorrection" );
    actionAutoFormat->setToolTip( i18n( "Change autocorrection options." ) );
    actionAutoFormat->setWhatsThis( i18n( "Change autocorrection options including:<p> <UL><LI><P>exceptions to autocorrection</P> <LI><P>add/remove autocorrection replacement text</P> <LI><P>and basic autocorrection options</P>." ) );

    actionEditCustomVarsEdit = new KAction( i18n( "Custom &Variables..." ), 0,
                                        this, SLOT( editCustomVars() ), // TODO: new dialog w add etc.
                                        actionCollection(), "custom_vars" );

    actionEditPersonnalExpr=new KAction( i18n( "Edit &Personal Expressions..." ), 0,
                                         this, SLOT( editPersonalExpr() ),
                                     actionCollection(), "personal_expr" );
    actionEditPersonnalExpr->setToolTip( i18n( "Add or change one or more personal expressions." ) );
    actionEditPersonnalExpr->setWhatsThis( i18n( "Add or change one or more personal expressions.<p>Personal expressions are a way to quickly insert commonly used phrases or text into your document." ) );

    actionChangeCase=new KAction( i18n( "Change Case..." ), 0,
                                     this, SLOT( changeCaseOfText() ),
                                     actionCollection(), "change_case" );
    actionChangeCase->setToolTip( i18n( "Alter the capitalization of selected text." ) );
    actionChangeCase->setWhatsThis( i18n( "Alter the capitalization of selected text to one of five pre-defined patterns.<p>You can also switch all letters from upper case to lower case and from lower case to upper case in one move." ) );

    //------------------------ Settings menu
    actionConfigure = KStdAction::preferences(this, SLOT(configure()), actionCollection(), "configure" );

    //------------------------ Menu frameSet
    actionChangePicture=new KAction( i18n( "Change Picture..." ),"frame_image",0,
                                     this, SLOT( changePicture() ),
                                     actionCollection(), "change_picture" );
    actionChangePicture->setToolTip( i18n( "Change the picture in the currently selected frame." ) );
    actionChangePicture->setWhatsThis( i18n( "You can specify a different picture in the current frame.<br><br>KWord automatically resizes the new picture to fit within the old frame." ) );

    actionConfigureHeaderFooter=new KAction( i18n( "Configure Header/Footer..." ), 0,
                                     this, SLOT( configureHeaderFooter() ),
                                     actionCollection(), "configure_headerfooter" );
    actionConfigureHeaderFooter->setToolTip( i18n( "Configure the currently selected header or footer." ) );
    actionConfigureHeaderFooter->setWhatsThis( i18n( "Configure the currently selected header or footer." ) );

    actionInlineFrame = new KToggleAction( i18n( "Inline Frame" ), 0,
                                            this, SLOT( inlineFrame() ),
                                            actionCollection(), "inline_frame" );
    actionInlineFrame->setToolTip( i18n( "Convert current frame to an inline frame." ) );
    actionInlineFrame->setWhatsThis( i18n( "Convert the current frame to an inline frame.<br><br>Place the inline frame within the text at the point nearest to the frames current position." ) );

    actionOpenLink = new KAction( i18n( "Open Link" ), 0,
                                     this, SLOT( openLink() ),
                                     actionCollection(), "open_link" );
    actionOpenLink->setToolTip( i18n( "Open the link with the appropriate application." ) );
    actionOpenLink->setWhatsThis( i18n( "Open the link with the appropriate application.<br><br>Web addresses are opened in a browser.<br>Email addresses begin a new message addressed to the link.<br>File links are opened by the appropriate viewer or editor." ) );

    actionChangeLink=new KAction( i18n("Change Link..."), 0,
                                  this,SLOT(changeLink()),
                                  actionCollection(), "change_link");
    actionChangeLink->setToolTip( i18n( "Change the content of the currently selected link." ) );
    actionChangeLink->setWhatsThis( i18n( "Change the details of the currently selected link." ) );

    actionCopyLink = new KAction( i18n( "Copy Link" ), 0,
                                     this, SLOT( copyLink() ),
                                     actionCollection(), "copy_link" );

    actionAddLinkToBookmak = new KAction( i18n( "Add to Bookmark" ), 0,
                                     this, SLOT( addToBookmark() ),
                                     actionCollection(), "add_to_bookmark" );

    actionRemoveLink = new KAction( i18n( "Remove Link" ), 0,
                                     this, SLOT( removeLink() ),
                                     actionCollection(), "remove_link" );

    actionShowDocStruct = new KToggleAction( i18n( "Show Doc Structure" ), 0,
                                            this, SLOT( showDocStructure() ),
                                            actionCollection(), "show_docstruct" );
#if KDE_IS_VERSION(3,2,90)
    actionShowDocStruct->setCheckedState(i18n("Hide Doc Structure"));
#endif
    actionShowDocStruct->setToolTip( i18n( "Open document structure sidebar." ) );
    actionShowDocStruct->setWhatsThis( i18n( "Open document structure sidebar.<p>This sidebar helps you organize your document and quickly find pictures, tables etc." ) );

	actionShowRuler = new KToggleAction( i18n( "Show Rulers" ), 0,
										this, SLOT( showRuler() ),
										actionCollection(), "show_ruler" );
#if KDE_IS_VERSION(3,2,90)
	actionShowRuler->setCheckedState(i18n("Hide Rulers"));
#endif
	actionShowRuler->setToolTip( i18n( "Shows or hides rulers." ) );
	actionShowRuler->setWhatsThis( i18n("The rulers are the white measuring spaces top and left of the "
                    "document. The rulers show the position and width of pages and of frames and can "
                    "be used to position tabulators among others.<p>Uncheck this to disable "
                    "the rulers from being displayed." ) );

    actionConfigureCompletion = new KAction( i18n( "Configure C&ompletion..." ), 0,
                        this, SLOT( configureCompletion() ),
                        actionCollection(), "configure_completion" );
    actionConfigureCompletion->setToolTip( i18n( "Change the words and options for autocompletion." ) );
    actionConfigureCompletion->setWhatsThis( i18n( "Add words or change the options for autocompletion." ) );


    // ------------------- Actions with a key binding and no GUI item
    new KAction( i18n( "Insert Non-Breaking Space" ), CTRL+Key_Space,
                 this, SLOT( slotNonbreakingSpace() ), actionCollection(), "nonbreaking_space" );
    new KAction( i18n( "Insert Non-Breaking Hyphen" ), CTRL+SHIFT+Key_Minus,
                 this, SLOT( slotNonbreakingHyphen() ), actionCollection(), "nonbreaking_hyphen" );
    new KAction( i18n( "Insert Soft Hyphen" ), CTRL+Key_Minus,
                 this, SLOT( slotSoftHyphen() ), actionCollection(), "soft_hyphen" );
    new KAction( i18n( "Line Break" ), SHIFT+Key_Return,
                 this, SLOT( slotLineBreak() ), actionCollection(), "line_break" );

    new KAction( i18n( "Completion" ), KStdAccel::shortcut(KStdAccel::TextCompletion), this, SLOT( slotCompletion() ), actionCollection(), "completion" );

    new KAction( i18n( "Increase Numbering Level" ), ALT+Key_Right,
                 this, SLOT( slotIncreaseNumberingLevel() ), actionCollection(), "increase_numbering_level" );
    new KAction( i18n( "Decrease Numbering Level" ), ALT+Key_Left,
                 this, SLOT( slotDecreaseNumberingLevel() ), actionCollection(), "decrease_numbering_level" );


    // --------
    actionEditCustomVars = new KAction( i18n( "Edit Variable..." ), 0,
                                        this, SLOT( editCustomVariable() ),
                                        actionCollection(), "edit_customvars" );
    actionApplyAutoFormat= new KAction( i18n( "Apply Autocorrection" ), 0,
                                        this, SLOT( applyAutoFormat() ),
                                        actionCollection(), "apply_autoformat" );
    actionApplyAutoFormat->setToolTip( i18n( "Manually force KWord to scan the entire document and apply autocorrection." ) );
    actionApplyAutoFormat->setWhatsThis( i18n( "Manually force KWord to scan the entire document and apply autocorrection." ) );

    actionCreateStyleFromSelection = new KAction( i18n( "Create Style From Selection..." ), 0,
                                        this, SLOT( createStyleFromSelection()),
                                        actionCollection(), "create_style" );
    actionCreateStyleFromSelection->setToolTip( i18n( "Create a new style based on the currently selected text." ) );
    actionCreateStyleFromSelection->setWhatsThis( i18n( "Create a new style based on the currently selected text." ) ); // ## "on the current paragraph, taking the formatting from where the cursor is. Selecting text isn't even needed."

    actionConfigureFootEndNote = new KAction( i18n( "&Footnote..." ), 0,
                                        this, SLOT( configureFootEndNote()),
                                        actionCollection(), "format_footendnote" );
    actionConfigureFootEndNote->setToolTip( i18n( "Change the look of footnotes." ) );
    actionConfigureFootEndNote->setWhatsThis( i18n( "Change the look of footnotes." ) );

    actionEditFootEndNote= new KAction( i18n("Edit Footnote"), 0,
                                        this, SLOT( editFootEndNote()),
                                        actionCollection(), "edit_footendnote" );
    actionEditFootEndNote->setToolTip( i18n( "Change the content of the currently selected footnote." ) );
    actionEditFootEndNote->setWhatsThis( i18n( "Change the content of the currently selected footnote." ) );


    actionChangeFootNoteType = new KAction( i18n("Change Footnote/Endnote Parameter"), 0,
                                        this, SLOT( changeFootNoteType() ),
                                            actionCollection(), "change_footendtype");

    actionSavePicture= new KAction( i18n("Save Picture..."), 0,
                                    this, SLOT( savePicture() ),
                                    actionCollection(), "save_picture");
    actionSavePicture->setToolTip( i18n( "Save the picture in a separate file." ) );
    actionSavePicture->setWhatsThis( i18n( "Save the picture in the currently selected frame in a separate file, outside the KWord document." ) );

    actionAllowBgSpellCheck = new KToggleAction( i18n( "Autospellcheck" ), 0,
                                            this, SLOT( autoSpellCheck() ),
                                            actionCollection(), "tool_auto_spellcheck" );


    actionGoToFootEndNote = new KAction( QString::null /*set dynamically*/, 0,
                                            this, SLOT( goToFootEndNote() ),
                                            actionCollection(), "goto_footendnote" );

    actionEditFrameSet = new KAction( i18n( "Edit Text Frameset" ), 0,
                                            this, SLOT( editFrameSet() ),
                                            actionCollection(), "edit_frameset" );

    actionDeleteFrameSet = new KAction( i18n( "Delete Frameset" ), 0,
                                        this, SLOT( deleteFrameSet() ),
                                        actionCollection(), "delete_frameset" );

    actionEditFrameSetProperties = new KAction( i18n( "Edit Frameset Properties" ), 0,
                                        this, SLOT( editFrameSetProperties() ),
                                        actionCollection(), "edit_frameset_properties" );


    actionSelectedFrameSet = new KAction( i18n( "Select Frameset" ), 0,
                                            this, SLOT( selectFrameSet() ),
                                            actionCollection(), "select_frameset" );


    actionAddBookmark= new KAction( i18n( "&Bookmark..." ), 0,
                                            this, SLOT( addBookmark() ),
                                            actionCollection(), "add_bookmark" );
    actionSelectBookmark= new KAction( i18n( "Select &Bookmark..." ), 0,
                                            this, SLOT( selectBookmark() ),
                                            actionCollection(), "select_bookmark" );

    actionImportStyle= new KAction( i18n( "Import Styles..." ), 0,
                                            this, SLOT( importStyle() ),
                                            actionCollection(), "import_style" );

    actionCreateFrameStyle = new KAction( i18n( "&Create Framestyle From Frame..." ), 0,
                                        this, SLOT( createFrameStyle()),
                                        actionCollection(), "create_framestyle" );
    actionCreateFrameStyle->setToolTip( i18n( "Create a new style based on the currently selected frame." ) );
    actionCreateFrameStyle->setWhatsThis( i18n( "Create a new framestyle based on the currently selected frame." ) );

#if 0 // re-enable after fixing
    actionInsertDirectCursor = new KToggleAction( i18n( "Type Anywhere Cursor" ), 0,
                                                  this, SLOT( insertDirectCursor() ),
                                                  actionCollection(), "direct_cursor" );
#endif

    actionConvertToTextBox = new KAction( i18n( "Convert to Text Box" ), 0,
                                                  this, SLOT( convertToTextBox() ),
                                                  actionCollection(), "convert_to_text_box" );


    actionSpellIgnoreAll = new KAction( i18n( "Ignore All" ), 0,
                                        this, SLOT( slotAddIgnoreAllWord() ),
                                        actionCollection(), "ignore_all" );

#if 0 // KWORD_HORIZONTAL_LINE
    actionInsertHorizontalLine = new KAction( i18n( "Horizontal Line..." ), 0,
                                        this, SLOT( insertHorizontalLine() ),
                                        actionCollection(), "insert_horizontal_line" );

    actionChangeHorizontalLine=new KAction( i18n( "Change Horizontal Line..." ),0,
                                            this, SLOT( changeHorizontalLine() ),
                                            actionCollection(), "change_horizontal_line" );
#endif

    actionAddWordToPersonalDictionary=new KAction( i18n( "Add Word to Dictionary" ),0,
                                                   this, SLOT( addWordToDictionary() ),
                                                   actionCollection(), "add_word_to_dictionary" );

    actionEmbeddedStoreInternal=new KToggleAction( i18n( "Store Document Internally" ),0,
                                            this, SLOT( embeddedStoreInternal() ),
                                            actionCollection(), "embedded_store_internal" );

    // For RMB inside a cell, see KWTableFrameSetEdit::showPopup
    // This isn't a dynamic list; it's only plugged/unplugged depending on the context.
    // If you change the contents of that list, check ~KWView.
    m_tableActionList.append( new KActionSeparator(actionCollection()) );
    m_tableActionList.append( actionTableInsertRow );
    m_tableActionList.append( actionTableDelRow );
    m_tableActionList.append( actionTableInsertCol );
    m_tableActionList.append( actionTableDelCol );
}

void KWView::refreshMenuExpression()
{
    loadexpressionActions( actionInsertExpression);
}

void KWView::loadexpressionActions( KActionMenu * parentMenu)
{
    KActionPtrList lst = actionCollection()->actions("expression-action");
    QValueList<KAction *> actions = lst;
    QValueList<KAction *>::ConstIterator it = lst.begin();
    QValueList<KAction *>::ConstIterator end = lst.end();
    // Delete all actions but keep their shortcuts in mind
    QMap<QString, KShortcut> personalShortCut;
    for (; it != end; ++it )
    {
        if ( !(*it)->shortcut().isNull() )
        {
            personalShortCut.insert( (*it)->text(), (*it)->shortcut() );
        }
        delete *it;
    }

    parentMenu->popupMenu()->clear();
    QStringList path = m_doc->personalExpressionPath();
    QStringList files;
    for ( QStringList::Iterator it = path.begin(); it != path.end(); ++it )
    {
        QDir dir( *it );
        if ( dir.exists() )
        {
            QStringList tmp = dir.entryList("*.xml");
            for ( QStringList::Iterator it2 = tmp.begin(); it2 != tmp.end(); ++it2 )
                files.append( QString( (*it) + (*it2)));
        }
    }

    //QStringList files = KWFactory::global()->dirs()->findAllResources( "expression", "*.xml", TRUE );
    int i = 0;
    int nbFile = 0;
    for( QStringList::Iterator it = files.begin(); it != files.end(); ++it,nbFile++ )
        createExpressionActions( parentMenu,*it, i,(nbFile<(int)files.count()-1), personalShortCut );
}

void KWView::createExpressionActions( KActionMenu * parentMenu,const QString& filename,int &i, bool insertSepar, const QMap<QString, KShortcut>& personalShortCut )
{
    QFile file( filename );
    if ( !file.exists() || !file.open( IO_ReadOnly ) )
        return;

    QDomDocument doc;
    doc.setContent( &file );
    file.close();

    bool expressionExist =false;
    QDomNode n = doc.documentElement().firstChild();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
        if ( n.isElement() )
        {
            QDomElement e = n.toElement();
            if ( e.tagName() == "Type" )
            {
                expressionExist =true;
                QString group = i18n( e.namedItem( "TypeName" ).toElement().text().utf8() );
                KActionMenu * subMenu = new KActionMenu( group, actionCollection() );
                parentMenu->insert( subMenu );

                QDomNode n2 = e.firstChild();
                for( ; !n2.isNull(); n2 = n2.nextSibling() )
                {

                    if ( n2.isElement() )
                    {
                        QDomElement e2 = n2.toElement();
                        if ( e2.tagName() == "Expression" )
                        {
                            QString text = i18n( e2.namedItem( "Text" ).toElement().text().utf8() );
                            KAction * act = new KAction( text, 0, this, SLOT( insertExpression() ),
                                                         actionCollection(),
                                                         QString("expression-action_%1").arg(i).latin1() );
                            if ( personalShortCut.contains(text) )
                                act->setShortcut( personalShortCut[text] );
                            i++;
                            act->setGroup("expression-action");
                            subMenu->insert( act );
                        }
                    }
                }
            }
        }
    }
    if(expressionExist && insertSepar)
        parentMenu->popupMenu()->insertSeparator();
}

void KWView::insertExpression()
{
 KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KAction * act = (KAction *)(sender());
        edit->insertExpression(act->text());
    }
}


void KWView::addVariableActions( int type, const QStringList & texts,
                                 KActionMenu * parentMenu, const QString & menuText )
{
    // Single items go directly into parentMenu.
    // For multiple items we create a submenu.
    if ( texts.count() > 1 && !menuText.isEmpty() )
    {
        KActionMenu * subMenu = new KActionMenu( menuText, actionCollection() );
        parentMenu->insert( subMenu );
        parentMenu = subMenu;
    }
    QStringList::ConstIterator it = texts.begin();
    for ( int i = 0; it != texts.end() ; ++it, ++i )
    {
        if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
        {
            VariableDef v;
            v.type = type;
            v.subtype = i;
            QCString actionName;
            actionName.sprintf( "var-action-%d-%d", type, i );
            KAction * act = new KAction( (*it), 0, this, SLOT( insertVariable() ),
                                         actionCollection(), actionName );
            // Mainly for KEditToolbar
            act->setToolTip( i18n( "Insert variable \"%1\" into the text" ).arg( *it ) );
            m_variableDefMap.insert( act, v );
            parentMenu->insert( act );
        }
    }
}

void KWView::refreshCustomMenu()
{
    KActionPtrList lst2 = actionCollection()->actions("custom-variable-action");
    QValueList<KAction *> actions = lst2;
    QValueList<KAction *>::ConstIterator it2 = lst2.begin();
    QValueList<KAction *>::ConstIterator end = lst2.end();
    QMap<QString, KShortcut> shortCut;

    for (; it2 != end; ++it2 )
    {
        if ( !(*it2)->shortcut().toString().isEmpty())
        {
            shortCut.insert((*it2)->text(), KShortcut( (*it2)->shortcut()));
        }
        delete *it2;
    }

    delete actionInsertCustom;
    actionInsertCustom = new KActionMenu( i18n( "&Custom" ),
                                            actionCollection(), "insert_custom" );
    actionInsertVariable->insert(actionInsertCustom, 0);

    actionInsertCustom->popupMenu()->clear();
    QPtrListIterator<KoVariable> it( m_doc->variableCollection()->getVariables() );
    KAction * act=0;
    QStringList lst;
    QString varName;
    int i = 0;
    for ( ; it.current() ; ++it )
    {
        KoVariable *var = it.current();
        if ( var->type() == VT_CUSTOM )
        {
            varName=( (KoCustomVariable*) var )->name();
            if ( !lst.contains( varName) )
            {
                 lst.append( varName );
                 QCString name = QString("custom-action_%1").arg(i).latin1();
                 if ( shortCut.contains( varName ))
                 {
                     act = new KAction( varName, (shortCut)[varName], this, SLOT( insertCustomVariable() ),actionCollection(), name );
                 }
                 else
                     act = new KAction( varName, 0, this, SLOT( insertCustomVariable() ),
                                        actionCollection(), name );
                 act->setGroup( "custom-variable-action" );
                 actionInsertCustom->insert( act );
                 i++;
            }
        }
    }
    bool state=!lst.isEmpty();
    if(state)
        actionInsertCustom->popupMenu()->insertSeparator();

    act = new KAction( i18n("New..."), 0, this, SLOT( insertNewCustomVariable() ), actionCollection(),QString("custom-action_%1").arg(i).latin1());
    act->setGroup( "custom-variable-action" );


    actionEditCustomVarsEdit->setEnabled( state );

    actionInsertCustom->insert( act );

}


void KWView::insertCustomVariable()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KAction * act = (KAction *)(sender());
        edit->insertCustomVariable(act->text());
    }
}

void KWView::insertNewCustomVariable()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertVariable( VT_CUSTOM, 0 );
}

void KWView::showFormulaToolbar( bool show )
{
    m_doc->formulaDocument()->setEnabled( show );
    if(shell())
      shell()->showToolbar( "formula_toolbar", show );
}

void KWView::updatePageInfo()
{
    if ( m_sbPageLabel )
    {
        KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
        if ( edit && edit->currentFrame() )
        {
            m_currentPage = edit->currentFrame()->pageNum();
        } else {
            KWFrame* f = m_doc->getFirstSelectedFrame();
            if ( f )
                m_currentPage = f->pageNum();
        }
        /*kdDebug() << (void*)this << " KWView::updatePageInfo "
                  << " edit: " << edit << " " << ( edit?edit->frameSet()->getName():QString::null)
                  << " currentFrame: " << (edit?edit->currentFrame():0L)
                  << " m_currentPage=" << m_currentPage << " m_sbPageLabel=" << m_sbPageLabel
                  << endl;*/

        // To avoid bugs, apply max page number in case a page was removed.
        m_currentPage = QMIN( m_currentPage, m_doc->numPages()-1 );

        QString oldText = m_sbPageLabel->text();
        QString newText;

        newText= (m_gui->canvasWidget()->viewMode()->type()!="ModeText")? QString(" ")+i18n("Page %1/%2").arg(m_currentPage+1).arg(m_doc->numPages())+' ' : QString::null;
        if ( newText != oldText )
        {
            m_sbPageLabel->setText( newText );
            // Need to repaint immediately. Otherwise when deleting 100 pages
            // at once, there's no feedback.
            m_sbPageLabel->repaint();
        }
    }
    slotUpdateRuler();
}

void KWView::pageNumChanged()
{
     docStructChanged(TextFrames);
     updatePageInfo();
     int pages = m_doc->numPages();
     kdDebug() <<  pages <<  " " << (m_doc->processingType() == KWDocument::DTP) << endl;
     refreshDeletePageAction();
}

void KWView::updateFrameStatusBarItem()
{
    KStatusBar * sb = statusBar();
    int nbFrame=m_doc->getSelectedFrames().count();
    if ( m_doc->showStatusBar() && sb && nbFrame > 0 )
    {
        if ( !m_sbFramesLabel )
        {
            m_sbFramesLabel = sb ? new KStatusBarLabel( QString::null, 0, sb ) : 0;
            addStatusBarItem( m_sbFramesLabel );
        }
        if ( nbFrame == 1 )
        {
            KoUnit::Unit unit = m_doc->unit();
            QString unitName = m_doc->unitName();
            KWFrame * frame = m_doc->getFirstSelectedFrame();
            m_sbFramesLabel->setText( i18n( "Statusbar info", "%1. Frame: %2, %3  -  %4, %5 (width: %6, height: %7) (%8)" )
                                      .arg( frame->frameSet()->getName() )
                                      .arg( KoUnit::toUserStringValue( frame->left(), unit ) )
                                      .arg( KoUnit::toUserStringValue((frame->top() - (frame->pageNum() * m_doc->ptPaperHeight())), unit ) )
                                      .arg( KoUnit::toUserStringValue( frame->right(), unit ) )
                                      .arg( KoUnit::toUserStringValue( frame->bottom(), unit ) )
                                      .arg( KoUnit::toUserStringValue( frame->width(), unit ) )
                                      .arg( KoUnit::toUserStringValue( frame->height(), unit ) )
                                      .arg( unitName ) );
        } else
            m_sbFramesLabel->setText( i18n( "%1 frames selected" ).arg( nbFrame ) );
    }
    else if ( sb && m_sbFramesLabel )
    {
        removeStatusBarItem( m_sbFramesLabel );
        delete m_sbFramesLabel;
        m_sbFramesLabel = 0L;
    }
}

void KWView::clipboardDataChanged()
{
    if ( !m_gui || !m_doc->isReadWrite() )
    {
        actionEditPaste->setEnabled(false);
        return;
    }
    KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
    // Is there plain text in the clipboard ?
    if ( edit && !QApplication::clipboard()->text().isEmpty() )
    {
        actionEditPaste->setEnabled(true);
        return;
    }
    QMimeSource *data = QApplication::clipboard()->data();
    const int provides = checkClipboard( data );
    if ( provides & ( ProvidesImage | ProvidesOasis | ProvidesFormula ) )
        actionEditPaste->setEnabled( true );
    else
    {
        // Plain text requires a framesetedit
        actionEditPaste->setEnabled( edit && ( provides & ProvidesPlainText ) );
    }
}

int KWView::checkClipboard( QMimeSource *data )
{
    int provides = 0;
    QValueList<QCString> formats;
    const char* fmt;
    for (int i=0; (fmt = data->format(i)); i++)
        formats.append( QCString( fmt ) );

#if 0 // not needed anymore
    // QImageDrag::canDecode( data ) is very very slow in Qt 2 (n*m roundtrips)
    // Workaround....
    QStrList fileFormats = QImageIO::inputFormats();
    for ( fileFormats.first() ; fileFormats.current() && !provides ; fileFormats.next() )
    {
        QCString format = fileFormats.current();
        QCString type = "image/" + format.lower();
        if ( ( formats.findIndex( type ) != -1 ) )
            provides |= ProvidesImage;
    }
#endif
    if ( QImageDrag::canDecode( data ) )
        provides |= ProvidesImage;
    if ( formats.findIndex( KFormula::MimeSource::selectionMimeType() ) != -1 )
        provides |= ProvidesFormula;
    if ( formats.findIndex( "text/plain" ) != -1 )
        provides |= ProvidesPlainText;
    QCString returnedTypeMime;
    if ( KWTextDrag::provides( data, KoTextObject::acceptSelectionMimeType(), returnedTypeMime ) )
        provides |= ProvidesOasis;
    //kdDebug(32001) << "KWView::checkClipboard provides=" << provides << endl;
    return provides;
}

/*=========================== file print =======================*/
void KWView::setupPrinter( KPrinter &prt )
{
    //recalc time and date variable before to print
    //it's necessary otherwise we print a document
    //with a bad date and time
    //TODO call once this function
    m_doc->recalcVariables(  VT_TIME );
    m_doc->recalcVariables(  VT_DATE );
    m_doc->recalcVariables(  VT_STATISTIC );

    prt.setPageSelection( KPrinter::ApplicationSide );
    prt.setCurrentPage( currentPage() + 1 );
    prt.setMinMax( 1, m_doc->numPages() );

    KoPageLayout pgLayout = m_doc->pageLayout();

    prt.setPageSize( static_cast<KPrinter::PageSize>( KoPageFormat::printerPageSize( pgLayout.format ) ) );

    if ( pgLayout.orientation == PG_LANDSCAPE || pgLayout.format == PG_SCREEN )
        prt.setOrientation( KPrinter::Landscape );
    else
        prt.setOrientation( KPrinter::Portrait );
}

void KWView::print( KPrinter &prt )
{
    bool displayFieldCode = m_doc->variableCollection()->variableSetting()->displayFieldCode();
    if ( displayFieldCode )
    {
        m_doc->variableCollection()->variableSetting()->setDisplayFieldCode(false);
        m_doc->recalcVariables(  VT_ALL );
    }

// Don't repaint behind the print dialog until we're done zooming/unzooming the doc
    m_gui->canvasWidget()->setUpdatesEnabled(false);
    m_gui->canvasWidget()->viewport()->setCursor( waitCursor );

    prt.setFullPage( true );

    // ### HACK: disable zooming-when-printing if embedded parts are used.
    // No koffice app supports zooming in paintContent currently.
    // Disable in ALL cases now
    bool doZoom = false;
    /*QPtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() && doZoom ; ++fit )
        if ( fit.current()->type() == FT_PART )
            doZoom = false;*/

    int oldZoom = m_doc->zoom();
    // We don't get valid metrics from the printer - and we want a better resolution
    // anyway (it's the PS driver that takes care of the printer resolution).
    QPaintDeviceMetrics metrics( &prt );

    //int dpiX = metrics.logicalDpiX();
    //int dpiY = metrics.logicalDpiY();
    int dpiX = doZoom ? 300 : KoGlobal::dpiX();
    int dpiY = doZoom ? 300 : KoGlobal::dpiY();
    ///////// Changing the dpiX/dpiY is very wrong nowadays. This has no effect on the font size
    ///////// that we give Qt, anymore, so it leads to minuscule fonts in the printout => doZoom==false.
    m_doc->setZoomAndResolution( 100, dpiX, dpiY );
    m_doc->newZoomAndResolution( false, true /* for printing*/ );
    //kdDebug() << "KWView::print metrics: " << metrics.logicalDpiX() << "," << metrics.logicalDpiY() << endl;
    //kdDebug() << "x11AppDPI: " << KoGlobal::dpiX() << "," << KoGlobal::dpiY() << endl;

    bool serialLetter = FALSE;

    QPtrList<KoVariable> vars = m_doc->variableCollection()->getVariables();
    KoVariable *v = 0;
    for ( v = vars.first(); v; v = vars.next() ) {
        if ( v->type() == VT_MAILMERGE ) {
            serialLetter = TRUE;
            break;
        }
    }

    if ( !m_doc->mailMergeDataBase() ) serialLetter=FALSE;
	else
	{
		m_doc->mailMergeDataBase()->refresh(false);
                if (m_doc->mailMergeDataBase()->getNumRecords() == 0 )  serialLetter = FALSE;
	}

    //float left_margin = 0.0;
    //float top_margin = 0.0;

    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    m_doc->getPageLayout( pgLayout, cl, hf );
    KoPageLayout oldPGLayout = pgLayout;

    if ( pgLayout.format == PG_SCREEN )
    {
        //left_margin = 25.8;
        //top_margin = 15.0;
        pgLayout.ptLeft += 25.8;         // Not sure why we need this....
        pgLayout.ptRight += 15.0;
        m_doc->setPageLayout( pgLayout, cl, hf, false );
    }

    QPainter painter;
    painter.begin( &prt );

    kdDebug(32001) << "KWView::print scaling by " << (double)metrics.logicalDpiX() / (double)dpiX
                   << "," << (double)metrics.logicalDpiY() / (double)dpiY << endl;
    painter.scale( (double)metrics.logicalDpiX() / (double)dpiX,
                   (double)metrics.logicalDpiY() / (double)dpiY );

    bool canceled = false;
    // Breaks wysiwyg, obviously - trying without
//#define KW_PASS_PAINTER_TO_QRT
#ifdef KW_PASS_PAINTER_TO_QRT
    int paragraphs = 0;
    fit.toFirst();
    for ( ; fit.current() ; ++fit )
        if ( fit.current()->isVisible() )
            paragraphs += fit.current()->paragraphs();
    kdDebug() << "KWView::print total paragraphs: " << paragraphs << endl;

    // This can take a lot of time (reformatting everything), so a progress dialog is needed
    QProgressDialog progress( i18n( "Printing..." ), i18n( "Cancel" ), paragraphs, this );
    progress.setProgress( 0 );
    int processedParags = 0;
    fit.toFirst();
    for ( ; fit.current() ; ++fit )
        if ( fit.current()->isVisible() )
        {
            kapp->processEvents();
            if ( progress.wasCancelled() ) {
                canceled = true;
                break;
            }

            kdDebug() << "KWView::print preparePrinting " << fit.current()->getName() << endl;
            fit.current()->preparePrinting( &painter, &progress, processedParags );
        }
#endif

    if ( !canceled )
    {
        if ( !serialLetter )
            m_gui->canvasWidget()->print( &painter, &prt );
        else
        {
            for ( int i = 0; i < m_doc->mailMergeDataBase()->getNumRecords(); ++i ) {
                m_doc->setMailMergeRecord( i );
		m_doc->variableCollection()->recalcVariables(VT_MAILMERGE);
                m_gui->canvasWidget()->print( &painter, &prt );
                if ( i < m_doc->mailMergeDataBase()->getNumRecords() - 1 )
                    prt.newPage();
            }
            m_doc->setMailMergeRecord( -1 );
        }
    }

    if ( pgLayout.format == PG_SCREEN )
        m_doc->setPageLayout( oldPGLayout, cl, hf, false );

#ifdef KW_PASS_PAINTER_TO_QRT
    fit.toFirst();
    for ( ; fit.current() ; ++fit )
        if ( fit.current()->isVisible() )
            fit.current()->preparePrinting( 0L, 0L, processedParags );
#endif

    m_doc->setZoomAndResolution( oldZoom, KoGlobal::dpiX(), KoGlobal::dpiY() );
    m_doc->newZoomAndResolution( false, false );
    kdDebug() << "KWView::print zoom&res reset" << endl;

    m_gui->canvasWidget()->setUpdatesEnabled(true);
    m_gui->canvasWidget()->viewport()->setCursor( ibeamCursor );
    m_doc->repaintAllViews();

    if ( displayFieldCode )
    {
        m_doc->variableCollection()->variableSetting()->setDisplayFieldCode(true);
        m_doc->recalcVariables(  VT_ALL );
    }
    else
        m_doc->variableCollection()->recalcVariables(VT_MAILMERGE);

    painter.end(); // this is what triggers the printing
    m_doc->variableCollection()->variableSetting()->setLastPrintingDate(QDateTime::currentDateTime());
    m_doc->recalcVariables( VT_DATE );
}

void KWView::showFormat( const KoTextFormat &currentFormat )
{
    // update the gui with the current format.
    //kdDebug() << "KWView::setFormat font family=" << currentFormat.font().family() << endl;
    actionFormatFontFamily->setFont( currentFormat.font().family() );
    actionFormatFontSize->setFontSize( currentFormat.pointSize() );
    actionFormatBold->setChecked( currentFormat.font().bold());
    actionFormatItalic->setChecked( currentFormat.font().italic() );
    actionFormatUnderline->setChecked( currentFormat.underline());
    actionFormatStrikeOut->setChecked( currentFormat.strikeOut());
    QColor col=currentFormat.textBackgroundColor();
    //actionBackgroundColor->setEnabled(true);
    actionBackgroundColor->setCurrentColor( col.isValid() ? col : QApplication::palette().color( QPalette::Active, QColorGroup::Base ));

    if ( m_doc->getSelectedFrames().count() > 0)
        actionBackgroundColor->setText(i18n("Frame Background Color..."));
    else
        actionBackgroundColor->setText(i18n("Text Background Color..."));
    switch(currentFormat.vAlign())
      {
      case KoTextFormat::AlignSuperScript:
        {
          actionFormatSub->setChecked( false );
          actionFormatSuper->setChecked( true );
          break;
        }
      case KoTextFormat::AlignSubScript:
        {
          actionFormatSub->setChecked( true );
          actionFormatSuper->setChecked( false );
          break;
        }
      case KoTextFormat::AlignNormal:
      default:
        {
          actionFormatSub->setChecked( false );
          actionFormatSuper->setChecked( false );
          break;
        }
      }

}

void KWView::showRulerIndent( double _leftMargin, double _firstLine, double _rightMargin, bool rtl )
{
  KoRuler * hRuler = m_gui ? m_gui->getHorzRuler() : 0;
  if ( hRuler )
  {
      hRuler->setFirstIndent( KoUnit::toUserValue( _firstLine, m_doc->unit() ) );
      hRuler->setLeftIndent( KoUnit::toUserValue( _leftMargin, m_doc->unit() ) );
      hRuler->setRightIndent( KoUnit::toUserValue( _rightMargin, m_doc->unit() ) );
      hRuler->setDirection( rtl );
      actionFormatDecreaseIndent->setEnabled( _leftMargin>0);
  }
}

void KWView::showAlign( int align ) {
    switch ( align ) {
        case Qt::AlignAuto: // In left-to-right mode it's align left. TODO: alignright if text->isRightToLeft()
            kdWarning() << k_funcinfo << "shouldn't be called with AlignAuto" << endl;
            // fallthrough
        case Qt::AlignLeft:
            actionFormatAlignLeft->setChecked( TRUE );
            break;
        case Qt::AlignHCenter:
            actionFormatAlignCenter->setChecked( TRUE );
            break;
        case Qt::AlignRight:
            actionFormatAlignRight->setChecked( TRUE );
            break;
        case Qt::AlignJustify:
            actionFormatAlignBlock->setChecked( TRUE );
            break;
    }
}

void KWView::showCounter( KoParagCounter &c )
{
    QString styleStr("counterstyle_");
    styleStr += QString::number( c.style() );
    //kdDebug() << "KWView::showCounter styleStr=" << styleStr << endl;
    KToggleAction* act = static_cast<KToggleAction *>( actionCollection()->action( styleStr.latin1() ) );
    Q_ASSERT( act );
    if ( act )
        act->setChecked( true );
}

void KWView::showFrameBorders( const KoBorder& _left, const KoBorder& _right,
                               const KoBorder& _top, const KoBorder& _bottom )
{
    showParagBorders( _left, _right, _top, _bottom );
}

void KWView::showParagBorders( const KoBorder& left, const KoBorder& right,
                               const KoBorder& top, const KoBorder& bottom )
{
    if ( m_border.left != left || m_border.right != right || m_border.top != top || m_border.bottom != bottom )
    {
        m_border.left = left;
        m_border.right = right;
        m_border.top = top;
        m_border.bottom = bottom;

        actionBorderLeft->setChecked( left.penWidth() > 0 );
        actionBorderRight->setChecked( right.penWidth() > 0 );
        actionBorderTop->setChecked( top.penWidth() > 0 );
        actionBorderBottom->setChecked( bottom.penWidth() > 0 );
        actionBorderOutline->setChecked(
            actionBorderLeft->isChecked() &&
            actionBorderRight->isChecked() &&
            actionBorderTop->isChecked() &&
            actionBorderBottom->isChecked());

        if ( left.penWidth() > 0 ) {
            m_border.common = left;
            borderShowValues();
        }
        if ( right.penWidth() > 0 ) {
            m_border.common = right;
            borderShowValues();
        }
        if ( top.penWidth() > 0 ) {
            m_border.common = top;
            borderShowValues();
        }
        if ( bottom.penWidth() > 0 ) {
            m_border.common = bottom;
            borderShowValues();
        }
    }
}

void KWView::updateReadWrite( bool readwrite )
{
    // First disable or enable everything
    QValueList<KAction*> actions = actionCollection()->actions();
    // Also grab actions from the document
    actions += m_doc->actionCollection()->actions();
    QValueList<KAction*>::ConstIterator aIt = actions.begin();
    QValueList<KAction*>::ConstIterator aEnd = actions.end();
    for (; aIt != aEnd; ++aIt )
        (*aIt)->setEnabled( readwrite );

    if ( !readwrite )
    {
        // Readonly -> re-enable a few harmless actions
        actionFileStatistics->setEnabled( true );
        actionExtraCreateTemplate->setEnabled( true );
        actionViewPageMode->setEnabled( true );
        actionViewPreviewMode->setEnabled( true );

        actionViewTextMode->setEnabled( true );
        actionShowRuler->setEnabled( true );
        actionEditFind->setEnabled( true );
        actionViewFormattingChars->setEnabled( true );
        actionViewFrameBorders->setEnabled( true );
        // that's not readonly, in fact, it modifies the doc
        //actionViewHeader->setEnabled( true );
        //actionViewFooter->setEnabled( true );
        actionViewZoom->setEnabled( true );
        actionInsertComment->setEnabled( true );
        actionAllowAutoFormat->setEnabled( true );
        actionShowDocStruct->setEnabled( true );
        actionConfigureCompletion->setEnabled( true );
        actionFormatBullet->setEnabled(true);
        actionFormatNumber->setEnabled( true);
        actionSelectBookmark->setEnabled( true );
        KAction* act = actionCollection()->action("edit_sldatabase");
        if (act)
            act->setEnabled( true );

        // In fact the new view could be readwrite, so this is too dangerous
        // (e.g. during spellchecking or during search-n-replace)
        //act = actionCollection()->action("view_newview");
        //if (act)
        //    act->setEnabled( true );
    }
    else
    {
        frameSelectedChanged();
        slotFrameSetEditChanged();
        refreshCustomMenu();
        refreshDeletePageAction();
        // Correctly enable or disable undo/redo actions again
        m_doc->commandHistory()->updateActions();
    }
}

void KWView::refreshDeletePageAction()
{
    actionDeletePage->setEnabled( m_doc->numPages() > 1 && m_doc->processingType() == KWDocument::DTP );
}

void KWView::showMouseMode( int _mouseMode )
{
    switch ( _mouseMode ) {
    case KWCanvas::MM_EDIT:
    case KWCanvas::MM_CREATE_TABLE:
    case KWCanvas::MM_CREATE_FORMULA:
    case KWCanvas::MM_CREATE_PART:
        // No tool to activate for this mode -> deselect all the others
        actionToolsCreateText->setChecked( false );
        actionToolsCreatePix->setChecked( false );
        //actionToolsCreatePart->setChecked( false );
        break;
    case KWCanvas::MM_CREATE_TEXT:
        actionToolsCreateText->setChecked( true );
        break;
    case KWCanvas::MM_CREATE_PIX:
        actionToolsCreatePix->setChecked( true );
        break;
        //case KWCanvas::MM_CREATE_PART:
        //actionToolsCreatePart->setChecked( true );
        break;
    }

    actionTableJoinCells->setEnabled( FALSE );
    actionTableSplitCells->setEnabled( FALSE );
    actionTableProtectCells->setEnabled( false );
    actionFormatFrameSet->setEnabled(FALSE);
    actionTablePropertiesMenu->setEnabled( false );
    actionConvertTableToText->setEnabled( false );
}

void KWView::showStyle( const QString & styleName )
{
    QPtrListIterator<KWStyle> styleIt( m_doc->styleCollection()->styleList() );
    for ( int pos = 0 ; styleIt.current(); ++styleIt, ++pos )
    {
        if ( styleIt.current()->name() == styleName ) {
            // Select style in combo
            actionFormatStyle->setCurrentItem( pos );
            // Check the appropriate action among the actionFormatStyleMenu actions
            KToggleAction* act = dynamic_cast<KToggleAction *>(actionCollection()->action( /*QString("style_%1").arg(pos).latin1()*/ styleIt.current()->shortCutName().latin1()));
            if ( act )
                act->setChecked( true );
            return;
        }
    }
}

void KWView::updateStyleList()
{
    QString currentStyle = actionFormatStyle->currentText();
    // Generate list of styles
    QStringList lst;
    QPtrListIterator<KWStyle> styleIt( m_doc->styleCollection()->styleList() );
    int pos = -1;
    for ( int i = 0; styleIt.current(); ++styleIt, ++i ) {
        QString name = styleIt.current()->displayName();
        lst << name;
        if ( pos == -1 && name == currentStyle )
            pos = i;
    }
    // Fill the combo - using a KSelectAction
    actionFormatStyle->setItems( lst );
    if ( pos > -1 )
        actionFormatStyle->setCurrentItem( pos );

    // Fill the menu - using a KActionMenu, so that it's possible to bind keys
    // to individual actions
    QStringList lstWithAccels;
    // Generate unique accelerators for the menu items
    KAccelGen::generate( lst, lstWithAccels );
    QMap<QString, KShortcut> shortCut;

    KActionPtrList lst2 = actionCollection()->actions("styleList");
    QValueList<KAction *> actions = lst2;
    QValueList<KAction *>::ConstIterator it = lst2.begin();
    QValueList<KAction *>::ConstIterator end = lst2.end();
    for (; it != end; ++it )
    {
        if ( !(*it)->shortcut().toString().isEmpty())
        {
            KoParagStyle* tmp = m_doc->styleCollection()->findStyleShortCut( (*it)->name() );
            if ( tmp )
                shortCut.insert( tmp->shortCutName(), KShortcut( (*it)->shortcut()));
        }
        actionFormatStyleMenu->remove( *it );
        delete *it;
    }
    uint i = 0;
    for ( QStringList::Iterator it = lstWithAccels.begin(); it != lstWithAccels.end(); ++it, ++i )
    {
        KToggleAction* act = 0L;
        // The list lst was created (unsorted) from the style collection, so we have still the same order.
        KoParagStyle *tmp = m_doc->styleCollection()->styleAt( i );
        if ( tmp )
        {
            QCString name = tmp->shortCutName().latin1();
            if ( shortCut.contains(name))
            {
                act = new KToggleAction( (*it),
                                         (shortCut)[name], this, SLOT( slotStyleSelected() ),
                                         actionCollection(), name );

            }
            else
                act = new KToggleAction( (*it),
                                         0, this, SLOT( slotStyleSelected() ),
                                         actionCollection(),name );
            act->setGroup( "styleList" );
            act->setExclusiveGroup( "styleListAction" );
            actionFormatStyleMenu->insert( act );
        }
        else
            kdWarning() << "No style found for " << *it << endl;
    }
}

void KWView::updateFrameStyleList()
{
    QString currentStyle = actionFrameStyle->currentText();
    // Generate list of styles
    QStringList lst;
    QPtrListIterator<KWFrameStyle> styleIt( m_doc->frameStyleCollection()->frameStyleList() );
    int pos = -1;
    for ( int i = 0; styleIt.current(); ++styleIt, ++i ) {
        QString name = styleIt.current()->displayName();
        lst << name;
        if ( pos == -1 && name == currentStyle )
            pos = i;
    }
    // Fill the combo - using a KSelectAction
    actionFrameStyle->setItems( lst );
    if ( pos > -1 )
        actionFrameStyle->setCurrentItem( pos );

    // Fill the menu - using a KActionMenu, so that it's possible to bind keys
    // to individual actions
    QStringList lstWithAccels;
    // Generate unique accelerators for the menu items
    KAccelGen::generate( lst, lstWithAccels );
    QMap<QString, KShortcut> shortCut;


    KActionPtrList lst2 = actionCollection()->actions("frameStyleList");
    QValueList<KAction *> actions = lst2;
    QValueList<KAction *>::ConstIterator it = lst2.begin();
    QValueList<KAction *>::ConstIterator end = lst2.end();
    for (; it != end; ++it )
    {
        if ( !(*it)->shortcut().toString().isEmpty())
        {
            KWFrameStyle* tmp = m_doc->frameStyleCollection()->findStyleShortCut( (*it)->name() );
            if ( tmp )
                shortCut.insert( tmp->shortCutName(), KShortcut( (*it)->shortcut()));
        }
        actionFrameStyleMenu->remove(*it );
        delete *it;
    }

    uint i = 0;
    for ( QStringList::Iterator it = lstWithAccels.begin(); it != lstWithAccels.end(); ++it, ++i )
    {
        KToggleAction* act =0L;

        // The list lst was created (unsorted) from the frame style collection, so we have still the same order.
        KWFrameStyle *tmp = m_doc->frameStyleCollection()->frameStyleAt( i );
        if ( tmp )
        {
            QCString name = tmp->shortCutName().latin1();

            if ( shortCut.contains(name))
            {
                act = new KToggleAction( (*it),
                                         (shortCut)[name], this, SLOT( slotFrameStyleSelected() ),
                                         actionCollection(), name );

            }
            else
                act = new KToggleAction( (*it),
                                         0, this, SLOT( slotFrameStyleSelected() ),
                                         actionCollection(), name );
            act->setGroup( "frameStyleList" );
            act->setExclusiveGroup( "frameStyleList" );
            actionFrameStyleMenu->insert( act );
        }
        else
            kdWarning() << "No frame style found for " << *it << endl;
    }
}



void KWView::updateTableStyleList()
{
    QString currentStyle = actionTableStyle->currentText();
    // Generate list of styles
    QStringList lst;
    QPtrListIterator<KWTableStyle> styleIt( m_doc->tableStyleCollection()->tableStyleList() );
    int pos = -1;
    for ( int i = 0; styleIt.current(); ++styleIt, ++i ) {
        QString name = styleIt.current()->displayName();
        lst << name;
        if ( pos == -1 && name == currentStyle )
            pos = i;
    }
    // Fill the combo - using a KSelectAction
    actionTableStyle->setItems( lst );
    if ( pos > -1 )
        actionTableStyle->setCurrentItem( pos );

    // Fill the menu - using a KActionMenu, so that it's possible to bind keys
    // to individual actions
    QStringList lstWithAccels;
    // Generate unique accelerators for the menu items
    KAccelGen::generate( lst, lstWithAccels );
    QMap<QString, KShortcut> shortCut;

    KActionPtrList lst2 = actionCollection()->actions("tableStyleList");
    QValueList<KAction *> actions = lst2;
    QValueList<KAction *>::ConstIterator it = lst2.begin();
    QValueList<KAction *>::ConstIterator end = lst2.end();
    for (; it != end; ++it )
    {
        if ( !(*it)->shortcut().toString().isEmpty())
        {
            KWTableStyle* tmp = m_doc->tableStyleCollection()->findStyleShortCut( (*it)->name() );
            if ( tmp )
                shortCut.insert( tmp->shortCutName(), KShortcut( (*it)->shortcut()));
        }
        actionTableStyleMenu->remove( *it );
        delete *it;
    }

    uint i = 0;
    for ( QStringList::Iterator it = lstWithAccels.begin(); it != lstWithAccels.end(); ++it, ++i )
    {
        KToggleAction* act =0L;

        // The list lst was created (unsorted) from the table style collection, so we have still the same order.
        KWTableStyle *tmp = m_doc->tableStyleCollection()->tableStyleAt( i );
        if ( tmp)
        {
            QCString name = tmp->shortCutName().latin1();
            if ( shortCut.contains(name))
            {

                act = new KToggleAction( (*it),
                                         (shortCut)[name], this, SLOT( slotTableStyleSelected() ),
                                         actionCollection(), name );
            }
            else
                act = new KToggleAction( (*it),
                                         0, this, SLOT( slotTableStyleSelected() ),
                                         actionCollection(), name );

            act->setExclusiveGroup( "tableStyleList" );
            act->setGroup( "tableStyleList" );
            actionTableStyleMenu->insert( act );
        }
        else
            kdWarning() << "No table style found for " << *it << endl;
    }
}

void KWView::editCut()
{
    KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
        edit->cut();
    else
    {
        m_gui->canvasWidget()->cutSelectedFrames();
    }
}

void KWView::editCopy()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->copy();
    else {
        QDragObject *drag = m_doc->dragSelected( 0 );
        QApplication::clipboard()->setData( drag );
    }
}

void KWView::editPaste()
{
    QMimeSource *data = QApplication::clipboard()->data();
    pasteData( data );
}

// paste or drop
void KWView::pasteData( QMimeSource* data )
{
    int provides = checkClipboard( data );
    Q_ASSERT( provides != 0 );

    // formula must be the first as a formula is also available as image
    if ( provides & ProvidesFormula ) {
        KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
        if ( edit && edit->frameSet()->type() == FT_FORMULA ) {
            edit->pasteData( data, ProvidesFormula );
        }
        else {
            insertFormula( data );
        }
    }
    else // pasting text and/or frames
    {
        deselectAllFrames();
        KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
        if ( edit && ( ( provides & ProvidesOasis ) || ( provides & ProvidesPlainText ) ) )
            edit->pasteData( data, provides );
        else if ( ( provides & ProvidesOasis ) ) {
            QCString returnedTypeMime;
            // Not editing a frameset? We can't paste plain text then... only entire frames.
            if ( KWTextDrag::provides( data, KoTextObject::acceptSelectionMimeType(), returnedTypeMime) )
            {
                const QByteArray arr = data->encodedData( returnedTypeMime );
                if( !arr.isEmpty() )
                {
                    QBuffer buffer( arr );
                    KoStore * store = KoStore::createStore( &buffer, KoStore::Read );
                    KWOasisLoader oasisLoader( m_doc );
                    QValueList<KWFrame *> frames = oasisLoader.insertOasisData( store, 0 /* no cursor */ );
                    delete store;
                    QValueList<KWFrame *>::ConstIterator it = frames.begin();
                    for ( ; it != frames.end() ; ++it )
                        (*it)->setSelected( true );
                    // TODO undo/redo command for the frames created above
                }
            }
        }
        else { // providesImage, must be after providesOasis
            KoPoint docPoint( m_doc->ptLeftBorder(), m_doc->ptPageTop( m_currentPage ) + m_doc->ptTopBorder() );
            m_gui->canvasWidget()->pasteImage( data, docPoint );
        }
    }
}

void KWView::editSelectAll()
{
    KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
        edit->selectAll();
}

void KWView::editSelectAllFrames()
{
    m_gui->canvasWidget()->selectAllFrames( true );
    frameSelectedChanged();
}

void KWView::editFind()
{
    if (!m_searchEntry)
        m_searchEntry = new KoSearchContext();
    KWTextFrameSetEdit * edit = currentTextEdit();
    bool hasSelection = edit && edit->textFrameSet()->hasSelection();
    bool hasCursor = edit != 0L;

    KoSearchDia dialog( m_gui->canvasWidget(), "find", m_searchEntry, hasSelection, hasCursor );
    if ( dialog.exec() == QDialog::Accepted )
    {
        delete m_findReplace;
        m_findReplace = new KWFindReplace( m_gui->canvasWidget(), &dialog, m_gui->canvasWidget()->kWordDocument()->visibleTextObjects(m_gui->canvasWidget()->viewMode()), edit);
        editFindNext();
    }
}

void KWView::editReplace()
{
    if (!m_searchEntry)
        m_searchEntry = new KoSearchContext();
    if (!m_replaceEntry)
        m_replaceEntry = new KoSearchContext();

    KWTextFrameSetEdit * edit = currentTextEdit();
    bool hasSelection = edit && edit->textFrameSet()->hasSelection();
    bool hasCursor = edit != 0L;

    KoReplaceDia dialog( m_gui->canvasWidget(), "replace", m_searchEntry, m_replaceEntry, hasSelection, hasCursor );
    if ( dialog.exec() == QDialog::Accepted )
    {
        delete m_findReplace;
        m_findReplace = new KWFindReplace( m_gui->canvasWidget(), &dialog, m_gui->canvasWidget()->kWordDocument()->visibleTextObjects(m_gui->canvasWidget()->viewMode()), edit);
        editFindNext();
    }
}

void KWView::editFindNext()
{
    if ( !m_findReplace ) // shouldn't be called before find or replace is activated
    {
        editFind();
        return;
    }
    (void) m_findReplace->findNext();
}

void KWView::editFindPrevious()
{
    if ( !m_findReplace ) // shouldn't be called before find or replace is activated
    {
        editFind();
        return;
    }
    (void) m_findReplace->findPrevious();
}

void KWView::adjustZOrderOfSelectedFrames(moveFrameType moveType) {
    KMacroCommand* macroCmd = 0L;
    // For each selected frame...
    QPtrList<KWFrame> frames = m_doc->getSelectedFrames();
    if(frames.count()==0) return;

    int pageNum= frames.at(0)->pageNum();
    for (QPtrListIterator<KWFrame> fIt( frames ); fIt.current() ; ++fIt ) {
        // include all frames in case of table.
        KWFrameSet *table = fIt.current()->frameSet()->getGroupManager();
        if(table) {
            for (QPtrListIterator<KWFrame> cellIt(table->frameIterator()  ); cellIt.current() ; ++cellIt ) {
                if(frames.contains(cellIt.current() ) ==0 && cellIt.current()->pageNum()==pageNum)
                    frames.append(cellIt.current());
            }
        }
    }

    int lowestZOrder=10000;
    QString actionName;
    for (QPtrListIterator<KWFrame> fIt( frames ); fIt.current() ; ++fIt ) {
        KWFrame* frame = fIt.current();
        int newZOrder=0;
        switch(moveType) {
            case RaiseFrame:
                newZOrder=raiseFrame(frames,frame);
                actionName=i18n("Raise Frame");
                break;
            case LowerFrame:
                newZOrder=lowerFrame(frames,frame);
                actionName=i18n("Lower Frame");
                break;
            case BringToFront:
                newZOrder=bringToFront(frames,frame);
                actionName=i18n("Bring to Front");
                break;
            case SendToBack:
                newZOrder=sendToBack(frames,frame);
                actionName=i18n("Send to Back");
                break;
        }

        if ( newZOrder != frame->zOrder() ) { // only if changed.
            lowestZOrder=QMIN(lowestZOrder, newZOrder);

            KWFrame* frameCopy = frame->getCopy();
            frame->setZOrder( newZOrder );

            KWFramePropertiesCommand* cmd = new KWFramePropertiesCommand( QString::null, frameCopy, frame);
            if(!macroCmd)
                macroCmd = new KMacroCommand( actionName );
            macroCmd->addCommand(cmd);

            frameCopy = frame->getCopy();
            frame->setZOrder( newZOrder );

            cmd = new KWFramePropertiesCommand( QString::null, frameCopy, frame );
            if(!macroCmd)
                macroCmd = new KMacroCommand( actionName );
            macroCmd->addCommand(cmd);
        }
    }

    if ( macroCmd )
    {
        m_doc->addCommand(macroCmd);
        // Calling updateFrames() on the selected frames' framesets isn't enough,
        // we also need other framesets to notice the new frames on top.
        m_doc->updateAllFrames();
        m_doc->layout();
        m_doc->repaintAllViews();
    }

    if(lowestZOrder != 10000 && m_doc->processingType() == KWDocument::WP) {
        m_doc->lowerMainFrames( pageNum, lowestZOrder );
    }
}

// Make room for refZOrder, by raising all z-orders above it by 1
void KWView::increaseAllZOrdersAbove(int refZOrder, int pageNum, const QPtrList<KWFrame> frameSelection) {
    QPtrList<KWFrame> framesInPage = m_doc->framesInPage( pageNum, false );
    for ( QPtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore frames we selected.
        if(frameIt.current()->zOrder() >= refZOrder) {
            frameIt.current()->setZOrder( frameIt.current()->zOrder() + 1 );
        }
    }
}

// Make room for refZOrder, by lowering all z-orders below it by 1
void KWView::decreaseAllZOrdersUnder(int refZOrder, int pageNum, const QPtrList<KWFrame> frameSelection) {
    QPtrList<KWFrame> framesInPage = m_doc->framesInPage( pageNum, false );
    for ( QPtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore frames we selected.
        if(frameIt.current()->zOrder() <= refZOrder) {
            frameIt.current()->setZOrder( frameIt.current()->zOrder() - 1 );
        }
    }
}

int KWView::raiseFrame(const QPtrList<KWFrame> frameSelection, const KWFrame *frame) {
    int newZOrder = 10000;
    QValueList<int> zorders;
    QPtrList<KWFrame> framesInPage = m_doc->framesInPage( frame->pageNum(), false );
    for ( QPtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore other frames we selected.
        if(! frameIt.current()->intersects(*frame)) continue; // only frames that I intersect with.
        int z = frameIt.current()->zOrder();
        if(z > frame->zOrder()) {
            newZOrder=QMIN(newZOrder, z + 1);
        }
        zorders.append( z );
    }
    if(newZOrder==10000) return frame->zOrder();
    // Ensure that newZOrder is "free"
    if ( zorders.find( newZOrder ) != zorders.end() )
        increaseAllZOrdersAbove( newZOrder, frame->pageNum(), frameSelection );
    return newZOrder;
}

int KWView::lowerFrame(const QPtrList<KWFrame> frameSelection, const KWFrame *frame) {
    int newZOrder = -10000;
    QValueList<int> zorders;
    QPtrList<KWFrame> framesInPage = m_doc->framesInPage( frame->pageNum(), false );
    for ( QPtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore other frames we selected.
        if(frameIt.current()->frameSet()->isMainFrameset()) continue; // ignore main frameset.
        if(! frameIt.current()->intersects(*frame)) continue; // only frames that I intersect with.
        int z = frameIt.current()->zOrder();
        if(z < frame->zOrder()) {
            newZOrder=QMAX(newZOrder, z -1);
        }
        zorders.append( z );
    }
    if(newZOrder==-10000) return frame->zOrder();
    // Ensure that newZOrder is "free"
    if ( zorders.find( newZOrder ) != zorders.end() )
        decreaseAllZOrdersUnder( newZOrder, frame->pageNum(), frameSelection );
    return newZOrder;
}

int KWView::bringToFront(const QPtrList<KWFrame> frameSelection, const KWFrame *frame) {
    int newZOrder = frame->zOrder();
    QPtrList<KWFrame> framesInPage = m_doc->framesInPage( frame->pageNum(), false );
    for ( QPtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore other frames we selected.
        if(! frameIt.current()->intersects(*frame)) continue; // only frames that I intersect with.
        newZOrder=QMAX(newZOrder, frameIt.current()->zOrder()+1);
    }
    return newZOrder;
}

int KWView::sendToBack(const QPtrList<KWFrame> frameSelection, const KWFrame *frame) {
    int newZOrder = frame->zOrder();
    QPtrList<KWFrame> framesInPage = m_doc->framesInPage( frame->pageNum(), false );
    for ( QPtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore other frames we selected.
        if(frameIt.current()->frameSet()->isMainFrameset()) continue; // ignore main frameset.
        if(! frameIt.current()->intersects(*frame)) continue; // only frames that I intersect with.
        newZOrder=QMIN(newZOrder, frameIt.current()->zOrder()-1);
    }
    return newZOrder;
}

void KWView::editDeleteFrame()
{
    deleteFrame();
}

void KWView::deleteFrame( bool _warning )
{
    if ( !m_doc->isReadWrite() )
        return;
    QPtrList<KWFrame> frames=m_doc->getSelectedFrames();
    Q_ASSERT( frames.count() >= 1 );
    if( frames.count() < 1)
        return;
    if(frames.count()==1)
    {
        KWFrame *theFrame = frames.at(0);
        KWFrameSet *fs = theFrame->frameSet();

        Q_ASSERT( !fs->isAHeader() ); // the action is disabled for such cases
        Q_ASSERT( !fs->isAFooter() );
        if ( fs->isMainFrameset() || fs->isAFooter() || fs->isAHeader() || fs->isFootEndNote())
            return;

        // frame is part of a table?
        if ( fs->getGroupManager() )
        {
            int result = KMessageBox::warningContinueCancel(
                this,
                i18n( "You are about to delete a table.\n"
                      "Doing so will delete all the text in the table.\n"
                      "Are you sure you want to do that?"),
                i18n("Delete Table"), i18n("&Delete"),
                "DeleteTableConfirmation",
                true );
            if (result != KMessageBox::Continue)
                return;
            m_doc->deleteTable( fs->getGroupManager() );
            m_gui->canvasWidget()->emitFrameSelectedChanged();
            return;
        }

        if ( fs->getNumFrames() == 1 && fs->type() == FT_TEXT) {
            if ( m_doc->processingType() == KWDocument::WP && m_doc->frameSetNum( fs ) == 0 )
                return; // if primairy FS, we can't delete it :)

            KWTextFrameSet * textfs = dynamic_cast<KWTextFrameSet *>(fs);
            if ( !textfs ) // assertion...
                return;

            KoTextDocument * textdoc = textfs->textDocument();
            if ( textdoc->length() > 0 )
            {
                int result = KMessageBox::warningContinueCancel(
                    this,
                    i18n( "You are about to delete the last Frame of the "
                          "Frameset '%1'. "
                          "The contents of this Frameset will not appear "
                          "anymore!\n"
                          "Are you sure you want to do that?").arg(fs->getName()),
                    i18n("Delete Frame"), i18n("&Delete"));

                if (result != KMessageBox::Continue)
                    return;

                m_doc->deleteFrame( theFrame );
                m_gui->canvasWidget()->emitFrameSelectedChanged();
                return;
            }

        }

        if(_warning)
        {
            int result = KMessageBox::warningContinueCancel(
                this,
                i18n("Do you want to delete this frame?"),
                i18n("Delete Frame"),
                i18n("&Delete"),
                "DeleteLastFrameConfirmation",
                true );
            if (result != KMessageBox::Continue)
                return;
        }
        m_doc->deleteFrame( theFrame );
        m_gui->canvasWidget()->emitFrameSelectedChanged();
    }
    else
    {
        //several frame
        if(_warning)
        {
            int result = KMessageBox::warningContinueCancel(
                this,
                i18n("Do you want to delete this frame?"),
                i18n("Delete Frame"),
                i18n("&Delete"),
                "DeleteLastFrameConfirmation",
                true );
            if (result != KMessageBox::Continue)
                return;
        }

        m_doc->deleteSelectedFrames();

        m_gui->canvasWidget()->emitFrameSelectedChanged();
    }
}

void KWView::createLinkedFrame()
{
    QPtrList<KWFrame> selectedFrames = m_doc->getSelectedFrames();
    if (selectedFrames.count() != 1)
        return; // action is disabled in such a case
    KWFrame* frame = selectedFrames.getFirst();
    KWFrame* newFrame = new KWFrame(0L, frame->x()+20, frame->y()+20, frame->width(), frame->height() );
    newFrame->setZOrder( m_doc->maxZOrder( newFrame->pageNum(m_doc) ) + 1 ); // make sure it's on top
    newFrame->setCopy(true);
    newFrame->setNewFrameBehavior( KWFrame::Copy );
    frame->frameSet()->addFrame( newFrame );

    frame->setSelected(false);
    newFrame->setSelected(true);

    KWCreateFrameCommand *cmd = new KWCreateFrameCommand( i18n("Create Linked Copy"), newFrame );
    m_doc->addCommand( cmd );

    m_doc->frameChanged( newFrame );
}

void KWView::editCustomVariable()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if (edit)
    {
        KoCustomVariable *var = static_cast<KoCustomVariable *>(edit->variable());
        if (var)
        {
            QString oldvalue = var->value();
            KoCustomVarDialog dia( this, var );
            if ( dia.exec() )
            {
                m_doc->recalcVariables( VT_CUSTOM );
                if( var->value() != oldvalue )
                {
                    KWChangeCustomVariableValue *cmd=new KWChangeCustomVariableValue(i18n( "Change Custom Variable" ),m_doc, oldvalue, var->value(), var );
                    m_doc->addCommand(cmd);
                }
            }
        }
    }
}

void KWView::editCustomVars()
{
    KoCustomVariablesDia dia( this, m_doc->variableCollection()->getVariables() );
    QStringList listOldCustomValue;
    QPtrListIterator<KoVariable> oldIt( m_doc->variableCollection()->getVariables() );
    for ( ; oldIt.current() ; ++oldIt )
    {
        if(oldIt.current()->type()==VT_CUSTOM)
            listOldCustomValue.append(((KoCustomVariable*)oldIt.current())->value());
    }
    if(dia.exec())
    {
        m_doc->recalcVariables( VT_CUSTOM );
        //temporaly hack, for the moment we can't undo/redo change custom variables
        QPtrListIterator<KoVariable> it( m_doc->variableCollection()->getVariables() );
        KMacroCommand * macroCommand = 0L;
        int i=0;
        for ( ; it.current() ; ++it )
        {
            if(it.current()->type() == VT_CUSTOM )
            {
                if(((KoCustomVariable*)it.current())->value()!=*(listOldCustomValue.at(i)))
                {
                    if(!macroCommand)
                        macroCommand = new KMacroCommand( i18n( "Change Custom Variable" ) );
                    KWChangeCustomVariableValue *cmd=new KWChangeCustomVariableValue(i18n( "Change Custom Variable" ),m_doc,*(listOldCustomValue.at(i)), ((KoCustomVariable*)it.current())->value() ,((KoCustomVariable*)it.current()));
                    macroCommand->addCommand(cmd);
                }
                i++;
            }
        }
        if(macroCommand)
            m_doc->addCommand(macroCommand);
    }
}

void KWView::editMailMergeDataBase()
{
	m_doc->mailMergeDataBase()->showConfigDialog(this);
#if 0
    KWMailMergeEditor *dia = new KWMailMergeEditor( this, m_doc->mailMergeDataBase() );
    dia->exec();
    // Don't know if we really need this so it's commented out (SL)
    // m_gui->canvasWidget()->repaintAll( FALSE );
    delete dia;
#endif
}

void KWView::viewTextMode()
{
    if ( actionViewTextMode->isChecked() )
    {
        KWTextFrameSet* fs = KWViewModeText::determineTextFrameSet( m_doc );
        if ( fs ) { // TODO: disable the action when there is no text frameset available
            if ( dynamic_cast<KWViewModePreview *>(m_gui->canvasWidget()->viewMode()) )
                m_zoomViewModePreview = m_doc->zoom();
            showZoom( m_zoomViewModeNormal ); // share the same zoom
            setZoom( m_zoomViewModeNormal, false );
            m_doc->switchViewMode( new KWViewModeText( m_doc, fs ) );
        } else
            initGUIButton(); // ensure we show the current viewmode
    }
    else
        actionViewTextMode->setChecked( true ); // always one has to be checked !
}

void KWView::viewPageMode()
{
    if ( actionViewPageMode->isChecked() )
    {
        if ( dynamic_cast<KWViewModePreview *>(m_gui->canvasWidget()->viewMode()) )
            m_zoomViewModePreview = m_doc->zoom();
        showZoom( m_zoomViewModeNormal );
        setZoom( m_zoomViewModeNormal, false );
        m_doc->switchViewMode( new KWViewModeNormal( m_doc, viewFrameBorders() ) );
    }
    else
        actionViewPageMode->setChecked( true ); // always one has to be checked !
}

void KWView::viewPreviewMode()
{
    if ( actionViewPreviewMode->isChecked() )
    {
        m_zoomViewModeNormal = m_doc->zoom();
        showZoom( m_zoomViewModePreview );
        setZoom( m_zoomViewModePreview, false );
        m_doc->switchViewMode( new KWViewModePreview( m_doc, viewFrameBorders(), m_doc->nbPagePerRow() ) );
    }
    else
        actionViewPreviewMode->setChecked( true ); // always one has to be checked !
}

void KWView::changeZoomMenu( int zoom )
{
    QString mode;
    if ( m_gui && m_gui->canvasWidget() && m_gui->canvasWidget()->viewMode())
        mode = m_gui->canvasWidget()->viewMode()->type();

    QStringList lst;
    lst << i18n( "Zoom to Width" );
    if ( mode!="ModeText" )
    {
        lst << i18n( "Zoom to Whole Page" );
    }

    if(zoom>0)
    {
        QValueList<int> list;
        bool ok;
        const QStringList itemsList ( actionViewZoom->items() );
        QRegExp regexp("(\\d+)"); // "Captured" non-empty sequence of digits

        for (QStringList::ConstIterator it = itemsList.begin() ; it != itemsList.end() ; ++it)
        {
            regexp.search(*it);
            const int val=regexp.cap(1).toInt(&ok);
            //zoom : limit inferior=10
            if(ok && val>9 && list.contains(val)==0)
                list.append( val );
        }
        //necessary at the beginning when we read config
        //this value is not in combo list
        if(list.contains(zoom)==0)
            list.append( zoom );

        qHeapSort( list );

        for (QValueList<int>::Iterator it = list.begin() ; it != list.end() ; ++it)
            lst.append( i18n("%1%").arg(*it) );
    }
    else
    {
        lst << i18n("%1%").arg("33");
        lst << i18n("%1%").arg("50");
        lst << i18n("%1%").arg("75");
        lst << i18n("%1%").arg("100");
        lst << i18n("%1%").arg("125");
        lst << i18n("%1%").arg("150");
        lst << i18n("%1%").arg("200");
        lst << i18n("%1%").arg("250");
        lst << i18n("%1%").arg("350");
        lst << i18n("%1%").arg("400");
        lst << i18n("%1%").arg("450");
        lst << i18n("%1%").arg("500");
    }
    actionViewZoom->setItems( lst );
}

void KWView::showZoom( int zoom )
{
    QStringList list = actionViewZoom->items();
    QString zoomStr( i18n("%1%").arg( zoom ) );
    actionViewZoom->setCurrentItem( list.findIndex(zoomStr)  );
}

void KWView::slotViewFormattingChars()
{
    m_doc->setViewFormattingChars(actionViewFormattingChars->isChecked());
    m_doc->layout(); // Due to the different formatting when this option is activated
    m_doc->repaintAllViews();
}

void KWView::slotViewFrameBorders()
{
    setViewFrameBorders(actionViewFrameBorders->isChecked());
    m_gui->canvasWidget()->repaintAll();
}

void KWView::viewHeader()
{
    bool state = actionViewHeader->isChecked();
    m_doc->setHeaderVisible( state );
    KWHideShowHeader *cmd=new KWHideShowHeader( state ? i18n("Show Header"):i18n("Hide Header"), m_doc, state);
    m_doc->addCommand(cmd);
    updateHeader();
}

void KWView::updateHeader()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    bool state = actionViewHeader->isChecked();
    if(!state )
    {
        KWFrameSet *frameSet=0L;
        if(edit)
        {
            frameSet=edit->frameSet();
            if (frameSet->isAHeader())
                m_doc->terminateEditing( frameSet );
            else
            {
                KWTableFrameSet *table = frameSet->frame(0)->frameSet()->getGroupManager();
                if (table)
                {
                    if (table->isFloating() && table->anchorFrameset()->isAHeader())
                        m_doc->terminateEditing( table );
                }
            }

        }
        else
        {
            KWFormulaFrameSetEdit * editFormula = dynamic_cast<KWFormulaFrameSetEdit *>(m_gui->canvasWidget()->currentFrameSetEdit());
            if(editFormula)
            {
                frameSet= editFormula->frameSet();
                if(frameSet->type()==FT_FORMULA && frameSet->isFloating())
                    m_doc->terminateEditing( frameSet );
            }

        }
    }
    m_doc->updateResizeHandles( );
}


void KWView::viewFooter()
{
    bool state=actionViewFooter->isChecked();
    m_doc->setFooterVisible( state );
    KWHideShowFooter *cmd=new KWHideShowFooter( state ? i18n("Show Footer"):i18n("Hide Footer"), m_doc, state);
    m_doc->addCommand(cmd);
    updateFooter();
}

void KWView::updateFooter()
{
    bool state=actionViewFooter->isChecked();
    KWTextFrameSetEdit * edit = currentTextEdit();
    if(!state )
    {
        KWFrameSet *frameSet=0L;
        if(edit)
        {
            frameSet=edit->frameSet();
            if (frameSet->isAFooter())
                m_doc->terminateEditing( frameSet );
            else
            {
                KWTableFrameSet *table = frameSet->frame(0)->frameSet()->getGroupManager();
                if (table)
                {
                    if (table->isFloating() && table->anchorFrameset()->isAFooter())
                        m_doc->terminateEditing( table );
                }
            }
        }
        else
        {
            KWFormulaFrameSetEdit * editFormula = dynamic_cast<KWFormulaFrameSetEdit *>(m_gui->canvasWidget()->currentFrameSetEdit());
            if(editFormula)
            {
                frameSet= editFormula->frameSet();
                if(frameSet->type()==FT_FORMULA && frameSet->isFloating())
                    m_doc->terminateEditing( frameSet );

            }

        }
    }
    m_doc->updateResizeHandles( );

}

void KWView::viewZoom( const QString &s )
{
    bool ok=false;
    KWCanvas * canvas = m_gui->canvasWidget();
    int zoom = 0;

    if ( s == i18n("Zoom to Width") )
    {
        zoom = qRound( static_cast<double>(canvas->visibleWidth() * 100 ) / (m_doc->resolutionX() * m_doc->ptPaperWidth() ) );
        ok = true;
    }
    else if ( s == i18n("Zoom to Whole Page") )
    {
        double height = m_doc->resolutionY() * m_doc->ptPaperHeight();
        double width = m_doc->resolutionX() * m_doc->ptPaperWidth();
        zoom = QMIN( qRound( static_cast<double>(canvas->visibleHeight() * 100 ) / height ),
                     qRound( static_cast<double>(canvas->visibleWidth() * 100 ) / width ) );
        ok = true;
    }
    else
    {
        QRegExp regexp("(\\d+)"); // "Captured" non-empty sequence of digits
        regexp.search(s);
        zoom=regexp.cap(1).toInt(&ok);
    }
    if( !ok || zoom<10 ) //zoom should be valid and >10
        zoom = m_doc->zoom();
    //refresh menu
    changeZoomMenu( zoom );
    //refresh menu item
    showZoom(zoom);
    //apply zoom if zoom!=m_doc->zoom()
    if( zoom != m_doc->zoom() )
    {
        setZoom( zoom, true );

        m_doc->updateResizeHandles();
        KWTextFrameSetEdit * edit = currentTextEdit();
        if ( edit )
            edit->ensureCursorVisible();
    }

    m_gui->canvasWidget()->setFocus();

}

void KWView::setZoom( int zoom, bool updateViews )
{
    m_doc->setZoomAndResolution( zoom, KoGlobal::dpiX(), KoGlobal::dpiY());
    m_doc->newZoomAndResolution( updateViews, false );
    m_doc->updateZoomRuler();

    // Also set the zoom in KoView (for embedded views)
    //kdDebug() << "KWView::showZoom setting koview zoom to " << m_doc->zoomedResolutionY() << endl;
    KoView::setZoom( m_doc->zoomedResolutionY() /* KoView only supports one zoom */ );
}

void KWView::insertPicture()
{
    if ( actionToolsCreatePix->isChecked() )
    {
        KWInsertPicDia dia( this,m_gui->canvasWidget()->pictureInline(),m_gui->canvasWidget()->pictureKeepRatio(),m_doc );
        if ( dia.exec() == QDialog::Accepted && !dia.picture().isNull() )
        {
            insertPicture( dia.picture(), dia.makeInline(), dia.keepRatio() );
            m_gui->canvasWidget()->setPictureInline( dia.makeInline());
            m_gui->canvasWidget()->setPictureKeepRatio( dia.keepRatio() );
        }
        else
            m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );
    }
    else
    {
        // clicked on the already active tool -> abort
        m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );
    }
}


void KWView::slotEmbedImage( const QString &filename )
{
    KoPicture picture;
    KoPictureKey key;
    key.setKeyFromFile( filename );
    picture.setKey( key );
    picture.loadFromFile( filename );
    insertPicture( picture, false, true );
}

void KWView::insertPicture( const KoPicture& picture, const bool makeInline, const bool _keepRatio )
{
    if ( makeInline )
    {
        const double widthLimit = m_doc->unzoomItX( m_doc->paperWidth() - m_doc->leftBorder() - m_doc->rightBorder() - m_doc->zoomItX( 10 ) );
        const double heightLimit = m_doc->unzoomItY( m_doc->paperHeight() - m_doc->topBorder() - m_doc->bottomBorder() - m_doc->zoomItY( 10 ) );
        fsInline = 0L;
        KWPictureFrameSet *frameset = new KWPictureFrameSet( m_doc, QString::null );

        frameset->insertPicture( picture );

        QSize pixmapSize ( frameset->picture().getOriginalSize() );
        // This ensures 1-1 at 100% on screen, but allows zooming and printing with correct DPI values
        // ### TODO/FIXME: is the qRound really necessary?
        double width = m_doc->unzoomItX( qRound( (double)pixmapSize.width() * m_doc->zoomedResolutionX() / POINT_TO_INCH( KoGlobal::dpiX() ) ) );
        double height = m_doc->unzoomItY( qRound( (double)pixmapSize.height() * m_doc->zoomedResolutionY() / POINT_TO_INCH( KoGlobal::dpiY() ) ) );

        frameset->setKeepAspectRatio( _keepRatio);


        if ( _keepRatio && ((width > widthLimit) || (height > heightLimit)) )
        {
            // size too big => adjust the size and keep ratio
            const double ratioX = width / widthLimit;
            const double ratioY = height / heightLimit;
            const double ratioPicture = width / height;

            if ( ratioPicture == 0 )  // unlikely
            {
                width = widthLimit;
                height = heightLimit;
            }
            else
                if ( ratioX > ratioY )  // restrict width and calculate height
                {
                    width = widthLimit;
                    height = widthLimit/ratioPicture;
                }
                else   // restrict height and calculate width
                {
                    width = heightLimit*ratioPicture;
                    height = heightLimit;
                }
        }
        else
        {
            // Apply reasonable limits
            width = kMin( width, widthLimit );
            height = kMin( height, heightLimit );
        }

        fsInline = frameset;
        KWFrame *frame = new KWFrame ( fsInline, 0, 0, width, height );
        fsInline->addFrame( frame, false );
        m_gui->canvasWidget()->inlinePictureStarted();
        showMouseMode( KWCanvas::MM_EDIT );

        displayFrameInlineInfo();

#if 0
        edit->insertFloatingFrameSet( fs, i18n("Insert Picture Inline") );
        fs->finalize(); // done last since it triggers a redraw
        showMouseMode( KWCanvas::MM_EDIT );
        m_doc->refreshDocStructure(Pictures);
#endif
    }
    else
    {
        m_gui->canvasWidget()->insertPicture( picture , picture.getOriginalSize(), _keepRatio );
    }
}

bool KWView::insertInlinePicture()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if(edit)
    {
        if ( edit->textFrameSet()->textObject()->protectContent() )
            return false;

        m_doc->addFrameSet( fsInline, false ); // done first since the frame number is stored in the undo/redo
#if 0
        KWFrame *frame = new KWFrame( fsInline, 0, 0, m_doc->unzoomItX( width ), m_doc->unzoomItY( height ) );
        fsInline->addFrame( frame, false );
#endif
        edit->insertFloatingFrameSet( fsInline, i18n("Insert Picture Inline") );
        fsInline->finalize(); // done last since it triggers a redraw
        showMouseMode( KWCanvas::MM_EDIT );
        m_doc->refreshDocStructure(Pictures);
        fsInline=0L;
        updateFrameStatusBarItem();
    }
    else
    {
        delete fsInline;
        fsInline=0L;
        updateFrameStatusBarItem();
    }
    return true;
}

void KWView::displayFrameInlineInfo()
{
    KMessageBox::information(this,
                             i18n("Set cursor where you want to insert inline frame."),
                             i18n("Insert Inline Frame"),
                             "SetCursorInsertInlineFrame",true);

    KStatusBar * sb = statusBar();
    if (sb )
    {
        if ( !m_sbFramesLabel )
        {
            m_sbFramesLabel = sb ? new KStatusBarLabel( QString::null, 0, sb ) : 0;
            addStatusBarItem( m_sbFramesLabel );
        }
        if(m_sbFramesLabel)
            m_sbFramesLabel->setText( i18n("Set cursor where you want to insert inline frame.") );
    }
}



void KWView::insertSpecialChar()
{
    KWTextFrameSetEdit *edit=currentTextEdit();
    if ( !edit )
        return;
    QString f = edit->textFontFamily();
    QChar c=' ';
    if (m_specialCharDlg==0)
    {
        m_specialCharDlg = new KoCharSelectDia( this, "insert special char", f, c, false );
        connect( m_specialCharDlg, SIGNAL(insertChar(QChar,const QString &)),
                 this, SLOT(slotSpecialChar(QChar,const QString &)));
        connect( m_specialCharDlg, SIGNAL( finished() ),
                 this, SLOT( slotSpecialCharDlgClosed() ) );
    }
    m_specialCharDlg->show();
}

void KWView::slotSpecialCharDlgClosed()
{
    if ( m_specialCharDlg )
    {
        disconnect( m_specialCharDlg, SIGNAL(insertChar(QChar,const QString &)),
                    this, SLOT(slotSpecialChar(QChar,const QString &)));
        disconnect( m_specialCharDlg, SIGNAL( finished() ),
                    this, SLOT( slotSpecialCharDlgClosed() ) );
        m_specialCharDlg->deleteLater();
        m_specialCharDlg = 0L;
    }
}

void KWView::slotSpecialChar(QChar c, const QString &_font)
{
    KWTextFrameSetEdit *edit=currentTextEdit();
    if ( !edit )
        return;
    edit->insertSpecialChar(c, _font);
}

void KWView::insertFrameBreak()
{
    KWTextFrameSetEdit *edit=currentTextEdit();
    if ( !edit )
        return;
    edit->insertFrameBreak();
}

void KWView::insertPage()
{
    if ( m_doc->processingType() == KWDocument::WP )
    {
        m_gui->canvasWidget()->editFrameSet( m_doc->frameSet(0) );
        KWTextFrameSetEdit *edit = currentTextEdit();
        Q_ASSERT(edit);
        if ( edit )
            edit->insertWPPage();
    } else {
        KWInsertPageDia dlg( this, "insertpage");
        if ( dlg.exec())
        {
            // If 'before', subtract 1 to the page number
            KCommand* cmd = new KWInsertRemovePageCommand( m_doc, KWInsertRemovePageCommand::Insert, dlg.getInsertPagePos()==KW_INSERTPAGEAFTER ?  m_currentPage : (m_currentPage -1));
            cmd->execute();
            m_doc->addCommand( cmd );
        }
    }
}

void KWView::deletePage()
{
    if ( m_doc->processingType() == KWDocument::WP )
    {
        // TODO we have to remove text so that the page can be removed
        // (e.g. everything between two (auto or manual) frame breaks)
        // Note: This must also be done in DTP mode, in some cases.
    } else {
        KCommand* cmd = new KWInsertRemovePageCommand( m_doc, KWInsertRemovePageCommand::Remove, m_currentPage );
        cmd->execute();
        m_doc->addCommand( cmd );
    }
}

void KWView::insertLink()
{
    KWTextFrameSetEdit *edit=currentTextEdit();
    if ( !edit )
        return;
    QString link;
    QString ref;
    if ( edit->textFrameSet()->hasSelection() )
    {
        QString selectedText = edit->textFrameSet()->textObject()->selectedText();
        if ( selectedText.startsWith( "mailto:/" ) ||
             selectedText.startsWith( "ftp:/" ) ||
             selectedText.startsWith( "http:/" ) )
        {
            link=selectedText;
            ref = selectedText;
        }
        else
        {
            //Just add text as link name and not url
            link = selectedText;
        }
    }

    if(KoInsertLinkDia::createLinkDia(link, ref, m_doc->listOfBookmarkName(0), this))
    {
        if(!link.isEmpty() && !ref.isEmpty())
            edit->insertLink(link, ref);
    }
}

void KWView::insertComment()
{
    KWTextFrameSetEdit *edit=currentTextEdit();
    if ( !edit )
        return;
    QString authorName;
    KoDocumentInfo * info = m_doc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
        kdWarning() << "Author information not found in documentInfo !" << endl;
    else
        authorName = authorPage->fullName();

    KoCommentDia *commentDia = new KoCommentDia( this, QString::null,authorName );
    if( commentDia->exec() )
    {
        edit->insertComment(commentDia->commentText());
    }
    delete commentDia;
}


void KWView::insertVariable()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KAction * act = (KAction *)(sender());
        VariableDefMap::Iterator it = m_variableDefMap.find( act );
        if ( it == m_variableDefMap.end() )
            kdWarning() << "Action not found in m_variableDefMap." << endl;
        else
        {
            if ( (*it).type == VT_FIELD )
                edit->insertVariable( (*it).type, KoFieldVariable::fieldSubType( (*it).subtype ) );
            else
                edit->insertVariable( (*it).type, (*it).subtype );
        }
    }
}

void KWView::insertFootNote()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    //Q_ASSERT( edit ); // the action should be disabled if we're not editing a textframeset...
    if ( edit ) // test for dcop call !
    {
        if ( edit->frameSet() != m_doc->frameSet(0) )
        {
            KMessageBox::sorry( this,
                                i18n( "You can only insert footnotes or "
                                      "endnotes into the first frameset."),
                                i18n("Insert Footnote"));
        } else {
            KWFootNoteDia dia( m_gui->canvasWidget()->footNoteType(), m_gui->canvasWidget()->numberingFootNoteType(), QString::null, this, m_doc, 0 );
            if ( dia.exec() ) {
                edit->insertFootNote( dia.noteType(), dia.numberingType(), dia.manualString() );
                m_gui->canvasWidget()->setFootNoteType( dia.noteType() );
                m_gui->canvasWidget()->setNumberingFootNoteType( dia.numberingType() );
            }
        }
    }
}

void KWView::renameButtonTOC(bool b)
{
   KActionCollection * coll = actionCollection();
   QString name= b ? i18n("Update Table of &Contents"):i18n("Table of &Contents");
   coll->action("insert_contents")->setText(name);
}

void KWView::insertContents()
{
    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
        edit->insertTOC();
}

void KWView::formatFont()
{
    KoTextFormatInterface* textIface = applicableTextInterfaces().first();

    if ( !textIface || !textIface->currentFormat() )
        return;

    delete m_fontDlg;
    m_fontDlg = new KoFontDia( *textIface->currentFormat()
#ifdef HAVE_LIBKSPELL2
                               , m_broker
#endif
                               , this, "" );

    connect( m_fontDlg, SIGNAL( applyFont() ),
                 this, SLOT( slotApplyFont() ) );

    m_fontDlg->exec();
    delete m_fontDlg;
    m_fontDlg=0L;

    //m_gui->canvasWidget()->setFocus();
}

void KWView::slotApplyFont()
{
    int flags = m_fontDlg->changedFlags();
    if ( flags )
    {
        KMacroCommand *globalCmd = new KMacroCommand(i18n("Change Font"));
        QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
        QPtrListIterator<KoTextFormatInterface> it( lst );
        for ( ; it.current() ; ++it )
        {
            KoTextFormat newFormat = m_fontDlg->newFormat();
            KCommand *cmd = it.current()->setFormatCommand( &newFormat, flags, true);
            if (cmd)
                globalCmd->addCommand(cmd);
        }
        m_doc->addCommand(globalCmd);
        m_gui->canvasWidget()->setFocus(); // the combo keeps focus...
    }

}

void KWView::formatParagraph()
{
    showParagraphDialog();
}

void KWView::showParagraphDialog( int initialPage, double initialTabPos )
{
    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
    {
        delete m_paragDlg;
        m_paragDlg = new KoParagDia( this, "",
                                     KoParagDia::PD_SPACING | KoParagDia::PD_ALIGN |
                                     KoParagDia::PD_BORDERS |
                                     KoParagDia::PD_NUMBERING | KoParagDia::PD_TABS, m_doc->unit(),edit->textFrameSet()->frame(0)->width() ,(!edit->frameSet()->isHeaderOrFooter() && !edit->frameSet()->getGroupManager()), edit->frameSet()->isFootEndNote());
        m_paragDlg->setCaption( i18n( "Paragraph Settings" ) );

        // Initialize the dialog from the current paragraph's settings
        m_paragDlg->setParagLayout( edit->cursor()->parag()->paragLayout() );

        // Set initial page and initial tabpos if necessary
        if ( initialPage != -1 )
        {
            m_paragDlg->setCurrentPage( initialPage );
            if ( initialPage == KoParagDia::PD_TABS )
                m_paragDlg->tabulatorsWidget()->setCurrentTab( initialTabPos );
        }
        connect( m_paragDlg, SIGNAL( applyParagStyle() ), this, SLOT( slotApplyParag()));

        m_paragDlg->exec();
        delete m_paragDlg;
        m_paragDlg=0L;
    }

}

void KWView::slotApplyParag()
{
    KWTextFrameSetEdit *edit = currentTextEdit();
    if( !edit)
        return;
    KMacroCommand * macroCommand = 0L;
    KCommand *cmd=0L;
    if(m_paragDlg->isLeftMarginChanged())
    {
        cmd=edit->setMarginCommand( QStyleSheetItem::MarginLeft, m_paragDlg->leftIndent() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
        m_gui->getHorzRuler()->setLeftIndent( KoUnit::toUserValue( m_paragDlg->leftIndent(), m_doc->unit() ) );

    }

    if(m_paragDlg->isRightMarginChanged())
    {
        cmd=edit->setMarginCommand( QStyleSheetItem::MarginRight, m_paragDlg->rightIndent() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
        m_gui->getHorzRuler()->setRightIndent( KoUnit::toUserValue( m_paragDlg->rightIndent(), m_doc->unit() ) );
    }
    if(m_paragDlg->isSpaceBeforeChanged())
    {
        cmd=edit->setMarginCommand( QStyleSheetItem::MarginTop, m_paragDlg->spaceBeforeParag() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
    }
    if(m_paragDlg->isSpaceAfterChanged())
    {
        cmd=edit->setMarginCommand( QStyleSheetItem::MarginBottom, m_paragDlg->spaceAfterParag() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
    }
    if(m_paragDlg->isFirstLineChanged())
    {
        cmd=edit->setMarginCommand( QStyleSheetItem::MarginFirstLine, m_paragDlg->firstLineIndent());
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
        m_gui->getHorzRuler()->setFirstIndent(
            KoUnit::toUserValue( m_paragDlg->firstLineIndent(), m_doc->unit() ) );
    }

    if(m_paragDlg->isAlignChanged())
    {
        cmd=edit->setAlignCommand( m_paragDlg->align() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
    }
    if(m_paragDlg->isCounterChanged())
    {
        cmd=edit->setCounterCommand( m_paragDlg->counter() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
    }
    if(m_paragDlg->listTabulatorChanged())
    {
        cmd=edit->setTabListCommand( m_paragDlg->tabListTabulator() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
    }

    if(m_paragDlg->isLineSpacingChanged())
    {
        cmd=edit->setLineSpacingCommand( m_paragDlg->lineSpacing(),m_paragDlg->lineSpacingType() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );

            macroCommand->addCommand(cmd);
        }
    }
    if(m_paragDlg->isBorderChanged())
    {
        cmd=edit->setBordersCommand( m_paragDlg->leftBorder(), m_paragDlg->rightBorder(),
                                     m_paragDlg->topBorder(), m_paragDlg->bottomBorder() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );

            macroCommand->addCommand(cmd);
        }
    }
    if ( m_paragDlg->isPageBreakingChanged() )
    {
        cmd=edit->setPageBreakingCommand( m_paragDlg->pageBreaking() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );

            macroCommand->addCommand(cmd);
        }
    }

    if(macroCommand)
        m_doc->addCommand(macroCommand);
    // Set "oldLayout" in KoParagDia from the current paragraph's settings
    // Otherwise "isBlahChanged" will return wrong things when doing A -> B -> A
    m_paragDlg->setParagLayout( edit->cursor()->parag()->paragLayout() );
}

// This handles Tabulators _only_
void KWView::slotHRulerDoubleClicked( double ptpos )
{
    showParagraphDialog( KoParagDia::PD_TABS, ptpos );
}

// This handles either:
// - Indents
// - Page Layout
//
// This does _not_ handle Tabulators!
void KWView::slotHRulerDoubleClicked()
{
    QString mode = m_gui->canvasWidget()->viewMode()->type();
    bool state = (mode!="ModeText");
    if ( !state )
        return;

    KoRuler *ruler = m_gui->getHorzRuler ();
    if ( (ruler->flags() & KoRuler::F_INDENTS) && currentTextEdit() ) {
        if ( ruler->doubleClickedIndent () ) {
            formatParagraph();
            return;
        }
    }

    formatPage();
}

void KWView::formatPage()
{
    if( !m_doc->isReadWrite())
        return;
    QString mode = m_gui->canvasWidget()->viewMode()->type();
    bool state = (mode!="ModeText");
    if ( !state )
        return;

    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter kwhf;
    m_doc->getPageLayout( pgLayout, cl, kwhf );

    KWPageLayoutStruct oldLayout( pgLayout, cl, kwhf );

    KoHeadFoot hf;
    int flags = FORMAT_AND_BORDERS | DISABLE_UNIT;
    if ( m_doc->processingType() == KWDocument::WP )
        flags |= KW_HEADER_AND_FOOTER | COLUMNS;
    else
        flags |= DISABLE_BORDERS;

    KoUnit::Unit unit = m_doc->unit();
    KoUnit::Unit oldUnit = unit;

    if ( KoPageLayoutDia::pageLayout( pgLayout, hf, cl, kwhf, flags, unit, this ) )
    {
        if( !(oldLayout._pgLayout==pgLayout) ||
            oldLayout._cl != cl ||
            oldLayout._hf != kwhf )
        {
            KWPageLayoutStruct newLayout( pgLayout, cl, kwhf );

            KWTextFrameSetEdit *edit = currentTextEdit();
            if (edit)
                edit->textFrameSet()->clearUndoRedoInfo();
            KCommand *cmd =new KWPageLayoutCommand( i18n("Change Layout"),
                                                    m_doc, oldLayout, newLayout );
            m_doc->addCommand(cmd);

            m_doc->setPageLayout( pgLayout, cl, kwhf );
        }
        if ( unit != oldUnit )
            m_doc->setUnit( unit ); // ##### needs undo/redo support
    }
}

void KWView::formatFrameSet()
{
    if ( m_doc->getFirstSelectedFrame() )
    {
        m_gui->canvasWidget()->editFrameProperties();
    }
    else // Should never happen, the action is disabled
        KMessageBox::sorry( this,
                            i18n("You must select a frame first."),
                            i18n("Format Frameset"));
}

void KWView::slotSpellCheck()
{
#ifdef HAVE_LIBKSPELL2
    if (m_spell.kospell) return; // Already in progress
    //m_doc->setReadWrite(false); // prevent editing text - not anymore
    m_spell.macroCmdSpellCheck = 0L;
    m_spell.replaceAll.clear();
    QValueList<KoTextObject *> objects;
    KWTextFrameSetEdit * edit = currentTextEdit();
    if (!edit)
        return;
    int options = 0;
    if ( edit && edit->textFrameSet()->hasSelection() )
    {
        objects.append(edit->textFrameSet()->textObject());
        options = KFindDialog::SelectedText;
    }
    else
    {
        objects = m_gui->canvasWidget()->kWordDocument()->visibleTextObjects(m_gui->canvasWidget()->viewMode());
    }
    m_spell.textIterator = new KoTextIterator( objects, edit, options );
    kdDebug()<<"Created iterator with "<< objects.count() <<endl;
    startKSpell();
#endif
}

void KWView::extraAutoFormat()
{
    m_doc->autoFormat()->readConfig();
    KoAutoFormatDia dia( this, 0, m_doc->autoFormat() );
    dia.exec();
    m_doc->startBackgroundSpellCheck(); // will do so if enabled
}

void KWView::extraFrameStylist()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->hideCursor();
    KWFrameStyleManager * frameStyleManager = new KWFrameStyleManager( this, m_doc, m_doc->frameStyleCollection()->frameStyleList() );
    frameStyleManager->exec();
    delete frameStyleManager;
    if ( edit )
        edit->showCursor();
}

void KWView::createFrameStyle()
{
    KWFrame* frame = 0L;

    QPtrList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
    if (selectedFrames.count()== 1)
        frame = selectedFrames.first();

    if (frame)
    {
        QStringList list;
        QPtrListIterator<KWFrameStyle> styleIt( m_doc->frameStyleCollection()->frameStyleList() );
        for ( ; styleIt.current(); ++styleIt )
        {
            list.append( styleIt.current()->name() );
        }
        KoCreateStyleDia *dia = new KoCreateStyleDia( list , this, 0 );
        if ( dia->exec() )
        {
            KWFrameStyle *style= new KWFrameStyle( dia->nameOfNewStyle(), frame );
            m_doc->frameStyleCollection()->addFrameStyleTemplate( style );
            m_doc->updateAllFrameStyleLists();
        }
        delete dia;
    }
}

void KWView::extraStylist()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    QString activeStyleName  = QString::null;
    if ( edit )
    {
        edit->hideCursor();
        if (edit->cursor() && edit->cursor()->parag() && edit->cursor()->parag()->style())
            activeStyleName = edit->cursor()->parag()->style()->displayName();
    }
    KWStyleManager * styleManager = new KWStyleManager( this, m_doc->unit(),m_doc, m_doc->styleCollection()->styleList(), activeStyleName );
    styleManager->exec();
    delete styleManager;
    if ( edit )
        edit->showCursor();
}

void KWView::extraCreateTemplate()
{
    int width = 60;
    int height = 60;
    QPixmap pix = m_doc->generatePreview(QSize(width, height));

    KTempFile tempFile( QString::null, ".kwt" );
    tempFile.setAutoDelete(true);

    m_doc->saveNativeFormat( tempFile.name() );

    KoTemplateCreateDia::createTemplate( "kword_template", KWFactory::global(),
                                         tempFile.name(), pix, this );

    KWFactory::global()->dirs()->addResourceType("kword_template",
                                                    KStandardDirs::kde_default( "data" ) +
                                                    "kword/templates/");
}

void KWView::toolsCreateText()
{
    if ( actionToolsCreateText->isChecked() )
        m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_CREATE_TEXT );
    else
    {
        // clicked on the already active tool -> abort
        m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );
    }
}

void KWView::insertTable()
{
    KWCanvas * canvas = m_gui->canvasWidget();
    canvas->setMouseMode( KWCanvas::MM_EDIT );
    KWTableDia *tableDia = new KWTableDia( this, 0, KWTableDia::NEW, canvas, m_doc,
                                           canvas->tableRows(),
                                           canvas->tableCols(),
                                           canvas->tableWidthMode(),
                                           canvas->tableHeightMode(),
                                           canvas->tableIsFloating(),
                                           canvas->tableTemplateName(),
                                           canvas->tableFormat());
    tableDia->setCaption( i18n( "Insert Table" ) );
    if ( tableDia->exec() == QDialog::Rejected )
        canvas->setMouseMode( KWCanvas::MM_EDIT );
    delete tableDia;
}

void KWView::insertFormula( QMimeSource* source )
{
    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
    {
        KWFormulaFrameSet *frameset = new KWFormulaFrameSet( m_doc, QString::null );
        m_doc->addFrameSet( frameset, false ); // done first since the frame number is stored in the undo/redo
        if ( source ) {
            QByteArray data = source->encodedData( KFormula::MimeSource::selectionMimeType() );
            QDomDocument formula;
            formula.setContent( data );
            QDomElement formulaElem = formula.namedItem("KFORMULA").toElement();
            frameset->paste( formulaElem );
        }
        KWFrame *frame = new KWFrame(frameset, 0, 0, 10, 10 );
        frame->setZOrder( m_doc->maxZOrder( frame->pageNum(m_doc) ) + 1 ); // make sure it's on top
        frameset->addFrame( frame, false );
        edit->insertFloatingFrameSet( frameset, i18n("Insert Formula") );
        frameset->finalize(); // done last since it triggers a redraw
        m_doc->refreshDocStructure(FT_FORMULA);

        m_gui->canvasWidget()->editFrameSet( frameset );
        frameset->setChanged();
        m_gui->canvasWidget()->repaintChanged( frameset, true );
    }
}

void KWView::toolsPart()
{
    m_gui->canvasWidget()->insertPart( actionToolsCreatePart->documentEntry() );
}


int KWView::tableSelectCell(const QString &tableName, uint row, uint col)
{
    if(!m_doc || !m_gui)
        return -1;
    KWFrameSet *fs = m_doc->frameSetByName(tableName);
    if(!fs)
        return -1;
    KWTableFrameSet *table = dynamic_cast<KWTableFrameSet*>(fs);
    if(!table)
        return -1;
    if (row >= table->getRows() || col >= table->getCols())
        return -1;

    KWTableFrameSet::Cell *cell = table->getCell(row, col);

    KWCanvas *canvas = m_gui->canvasWidget();
    if(!canvas)
        return -1;
    canvas->tableSelectCell(table, cell);
    return 0;
}

int KWView::tableDeleteRow(const QValueList<uint>& rows, KWTableFrameSet *table )
{
    if(!table)
        table = m_gui->canvasWidget()->getCurrentTable();

    if (!m_doc || !table)
        return -1;

    if(rows.count() >= table->getRows()) {
        m_doc->deleteTable(table);
        return 0;
    }

    KMacroCommand *macro = new KMacroCommand(i18n("Remove Rows"));

    for (int i = rows.count() - 1; i >= 0 ; i--) {
        KWRemoveRowCommand *cmd = new KWRemoveRowCommand( i18n("Remove Row"),
            table, rows[i] );
        macro->addCommand(cmd);
    }

    macro->execute();
    m_doc->addCommand(macro);
    return 0;
}

int KWView::tableDeleteCol(const QValueList<uint>& cols, KWTableFrameSet *table)
{
    if(!table)
        table = m_gui->canvasWidget()->getCurrentTable();

    if (!m_doc || !table)
        return -1;

    if(cols.count() >= table->getCols()) {
        m_doc->deleteTable(table);
        return 0;
    }

    KMacroCommand *macro = new KMacroCommand(i18n("Remove Columns"));

    for (int i = cols.count() - 1;  i >= 0; i--) {
        KWRemoveColumnCommand *cmd = new KWRemoveColumnCommand( i18n("Remove Column"),
            table, cols[i] );
        macro->addCommand(cmd);
    }

    macro->execute();
    m_doc->addCommand(macro);
    return 0;
}

void KWView::tableProperties()
{
    KWCanvas * canvas = m_gui->canvasWidget();
    KWTableFrameSet *table = canvas->getCurrentTable();
    if (table)
    {
        canvas->setMouseMode( KWCanvas::MM_EDIT );
        KWTableDia *tableDia = new KWTableDia( this, 0, KWTableDia::EDIT, canvas, m_doc,
                                               table->getRows(),
                                               table->getCols(),
                                               canvas->tableWidthMode(),
                                               canvas->tableHeightMode(),
                                               canvas->tableIsFloating(),
                                               canvas->tableTemplateName(),
                                               canvas->tableFormat());
        tableDia->setCaption( i18n( "Adjust Table" ) );
        if ( tableDia->exec() == QDialog::Rejected )
            canvas->setMouseMode( KWCanvas::MM_EDIT );
        delete tableDia;
    }
}

void KWView::tableInsertRow()
{
    m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );
    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    Q_ASSERT(table);
    if (!table)
        return;
    KWInsertDia dia( this, "insert_row_dialog", table, m_doc, KWInsertDia::ROW, m_gui->canvasWidget() );
    dia.setCaption( i18n( "Insert Row" ) );
    dia.exec();
}

void KWView::tableInsertRow(uint row, KWTableFrameSet *table)
{
    if(!table)
        table = m_gui->canvasWidget()->getCurrentTable();

    if (!m_doc || !table)
        return;

    if(row > table->getRows())
        return;

    KWInsertRowCommand *cmd = new KWInsertRowCommand( i18n("Insert Row"), table, row);
    cmd->execute();
    m_doc->addCommand(cmd);
}


void KWView::tableInsertCol()
{
    m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );
    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    Q_ASSERT(table);
    if (!table)
        return;
/*
    // value = 62 because a insert column = 60 +2 (border )see kwtableframeset.cc
    if ( table->boundingRect().right() + 62 > static_cast<int>( m_doc->ptPaperWidth() ) )
    {
        KMessageBox::sorry( this,
                            i18n( "There is not enough space at the right of the table "
                                  "to insert a new column." ),
                            i18n( "Insert Column" ) );
    }
    else
    {
        KWInsertDia dia( this, "", table, m_doc, KWInsertDia::COL, m_gui->canvasWidget() );
        dia.setCaption( i18n( "Insert Column" ) );
        dia.exec();
    } */
    KWInsertDia dia( this, "insert_column_dialog", table, m_doc, KWInsertDia::COL, m_gui->canvasWidget() );
    dia.setCaption( i18n( "Insert Column" ) );
    dia.exec();
}

void KWView::tableInsertCol(uint col,  KWTableFrameSet *table  )
{
    if(!table)
        table = m_gui->canvasWidget()->getCurrentTable();

    if (!m_doc || !table)
        return;

    if(col > table->getCols())
        return;

    // we pass as last parameter the maximum offset that the table can use.
    // this offset is the max right offset of the containing frame in the case
    // of an inline (floating) table, the size of the page for other tables.
    double maxRightOffset;
    if (table->isFloating())    // inline table: max offset of containing frame
         maxRightOffset = table->anchorFrameset()->frame(0)->right();
    else                        // non inline table: max offset of the page
         maxRightOffset = m_doc->ptPaperWidth() - m_doc->ptRightBorder();

    KWInsertColumnCommand *cmd = new KWInsertColumnCommand( i18n("Insert Column"),
        table, col,  maxRightOffset);
    cmd->execute();
    m_doc->addCommand(cmd);
}

void KWView::tableDeleteRow()
{
    m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );

    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    Q_ASSERT(table);
    if (!table)
        return;
    if ( table->getRows() == 1 )
    {
        int result;
        result = KMessageBox::warningContinueCancel(this,
                                                    i18n("The table has only one row. "
                                                         "Deleting this row will delete the table.\n"
                                                         "Do you want to delete the table?"),
                                                    i18n("Delete Row"),
                                                    i18n("&Delete"));
        if (result == KMessageBox::Continue)
        {
            m_doc->deleteTable( table );
            m_gui->canvasWidget()->emitFrameSelectedChanged();
        }
    }
    else
    {
        KWDeleteDia dia( this, "", table, m_doc, KWDeleteDia::ROW, m_gui->canvasWidget() );
        dia.setCaption( i18n( "Delete Row" ) );
        dia.exec();
    }

}

void KWView::tableDeleteCol()
{
    m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );

    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    Q_ASSERT(table);
    if (!table)
        return;
    if ( table->getCols() == 1 )
    {
        int result;
        result = KMessageBox::warningContinueCancel(this,
                                                    i18n("The table has only one column. "
                                                         "Deleting this column will delete the table.\n"
                                                         "Do you want to delete the table?"),
                                                    i18n("Delete Column"),
                                                    i18n("&Delete"));
        if (result == KMessageBox::Continue)
        {
            m_doc->deleteTable( table );
            m_gui->canvasWidget()->emitFrameSelectedChanged();
        }
    }
    else
    {
        KWDeleteDia dia( this, "", table, m_doc, KWDeleteDia::COL, m_gui->canvasWidget() );
        dia.setCaption( i18n( "Delete Column" ) );
        dia.exec();
    }
}

void KWView::tableResizeCol()
{
    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    Q_ASSERT(table);
    if (!table)
        return;
    KWResizeTableDia dia( this, "", table, m_doc, KWResizeTableDia::COL, m_gui->canvasWidget() );
    dia.setCaption( i18n( "Resize Column" ) );
    dia.exec();
}

void KWView::tableJoinCells()
{
    //m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT_FRAME );

    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    Q_ASSERT(table);
    if (!table)
        return;
    KCommand * cmd=table->joinCells();
    if ( !cmd )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to select some cells which are next to each other "
                                  "and are not already joined." ),
                            i18n( "Join Cells" ) );
        return;
    }
    m_doc->addCommand(cmd);
    m_doc->layout();
    //KoRect r = m_doc->zoomRect( table->boundingRect() );
    //m_gui->canvasWidget()->repaintScreen( r, TRUE );
    m_gui->canvasWidget()->repaintAll();
    m_gui->canvasWidget()->emitFrameSelectedChanged();
}

void KWView::tableSplitCells() {
    KWSplitCellDia *splitDia=new KWSplitCellDia( this,"split cell",m_gui->canvasWidget()->splitCellRows(),m_gui->canvasWidget()->splitCellCols() );
    if(splitDia->exec()) {
        unsigned int nbCols=splitDia->cols();
        unsigned int nbRows=splitDia->rows();
        m_gui->canvasWidget()->setSplitCellRows(nbRows);
        m_gui->canvasWidget()->setSplitCellCols(nbCols);
        tableSplitCells(nbCols, nbRows);
    }
    delete splitDia;
}

void KWView::tableSplitCells(int cols, int rows)
{
    //m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT_FRAME );

    QPtrList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    if ( !table && selectedFrames.count() > 0) {
        table=selectedFrames.at(0)->frameSet()->getGroupManager();
    }

    if(selectedFrames.count() >1 || table == 0) {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table "
                                  "before splitting cells." ),
                            i18n( "Split Cells" ) );
        return;
    }

    //int rows=1, cols=2;
    KCommand *cmd=table->splitCell(rows,cols);
    if ( !cmd ) {
        KMessageBox::sorry( this,
                            i18n("There is not enough space to split the cell into that many parts, make it bigger first"),
                            i18n("Split Cells") );
        return;
    }
    m_doc->addCommand(cmd);
    //KoRect r = m_doc->zoomRect( table->boundingRect() );
    //m_gui->canvasWidget()->repaintScreen( r, TRUE );
    m_doc->updateAllFrames();
    m_doc->layout();
    m_gui->canvasWidget()->repaintAll();
    m_doc->frameSelectedChanged();
}

void KWView::tableUngroupTable()
{
    m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );

    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    Q_ASSERT(table);
    if (!table)
        return;

    // Use a macro command because we may have to make the table non-floating first
    KMacroCommand * macroCmd = new KMacroCommand( i18n( "Ungroup Table" ) );

    if ( table->isFloating() )
    {
        KWFrameSetInlineCommand *cmd = new KWFrameSetInlineCommand( QString::null, table, false );
        macroCmd->addCommand(cmd);
    }

    KWUngroupTableCommand *cmd = new KWUngroupTableCommand( QString::null, table );
    macroCmd->addCommand( cmd );
    m_doc->addCommand( macroCmd );
    macroCmd->execute(); // do it all
}

void KWView::tableDelete()
{
    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    Q_ASSERT(table);
    if (!table)
        return;
    m_doc->deleteTable( table );
    m_gui->canvasWidget()->emitFrameSelectedChanged();
}

void KWView::tableStylist()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->hideCursor();
    KWTableStyleManager * tableStyleManager = new KWTableStyleManager( this, m_doc, m_doc->tableStyleCollection()->tableStyleList() );
    tableStyleManager->exec();
    delete tableStyleManager;
    if ( edit )
        edit->showCursor();
}

void KWView::tableProtectCells()
{
    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    Q_ASSERT(table);
    if (!table)
        return;
    KCommand *cmd = table->setProtectContentCommand( actionTableProtectCells->isChecked() );
    if ( cmd )
        m_doc->addCommand( cmd );
}

// Called when selecting a style in the Format / Style menu
void KWView::slotStyleSelected()
{
    QString actionName = QString::fromLatin1(sender()->name());
    if ( actionName.startsWith( "shortcut_style_" ) )//see lib/kotext/kostyle.cc
    {
        kdDebug() << "KWView::slotStyleSelected " << actionName << endl;
        textStyleSelected( m_doc->styleCollection()->findStyleShortCut( actionName) );
    }
}

void KWView::textStyleSelected( KoParagStyle *_sty )
{
    if ( !_sty )
        return;

    if ( m_gui->canvasWidget()->currentFrameSetEdit() )
    {
        KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(m_gui->canvasWidget()->currentFrameSetEdit()->currentTextEdit());
        if ( edit )
            edit->applyStyle( _sty );
    }
    else
    { // it might be that a frame (or several frames) are selected
        QPtrList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
        if (selectedFrames.count() <= 0)
            return; // nope, no frames are selected.
        // yes, indeed frames are selected.
        QPtrListIterator<KWFrame> it( selectedFrames );
        KMacroCommand *globalCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KWFrame *curFrame = it.current();
            KWFrameSet *curFrameSet = curFrame->frameSet();
            if (curFrameSet->type() == FT_TEXT)
            {
                KoTextObject *textObject = ((KWTextFrameSet*)curFrameSet)->textObject();
                textObject->textDocument()->selectAll( KoTextDocument::Temp );
                KCommand *cmd = textObject->applyStyleCommand( 0L, _sty , KoTextDocument::Temp, KoParagLayout::All, KoTextFormat::Format, true, true );
                textObject->textDocument()->removeSelection( KoTextDocument::Temp );
                if (cmd)
                {
                    if ( !globalCmd )
                        globalCmd = new KMacroCommand( selectedFrames.count() == 1 ? i18n("Apply Style to Frame") : i18n("Apply Style to Frames"));
                    globalCmd->addCommand( cmd );
                }
            }
        }
        if ( globalCmd )
            m_doc->addCommand( globalCmd );
    }
    m_gui->canvasWidget()->setFocus(); // the combo keeps focus...*/

}

// Called by the above, and when selecting a style in the style combobox
void KWView::textStyleSelected( int index )
{
    textStyleSelected( m_doc->styleCollection()->styleAt( index ) );
}

// Slot is called when selecting a framestyle in the Frames / Framestyle menu
void KWView::slotFrameStyleSelected()
{
    QString actionName = QString::fromLatin1(sender()->name());
    if ( actionName.startsWith( "shortcut_framestyle_" ) )//see kwframestyle.cc
    {
        //kdDebug() << "KWView::slotFrameStyleSelected " << styleStr << endl;
        frameStyleSelected( m_doc->frameStyleCollection()->findStyleShortCut( actionName) );
    }
}

void KWView::frameStyleSelected( int index )
{
    frameStyleSelected( m_doc->frameStyleCollection()->frameStyleAt( index ) );
}


// Called by the above, and when selecting a style in the framestyle combobox
void KWView::frameStyleSelected( KWFrameStyle *_sty )
{
    if ( !_sty )
        return;

    if ( m_gui->canvasWidget()->currentFrameSetEdit() )
    {
        KWFrame * single = m_gui->canvasWidget()->currentFrameSetEdit()->currentFrame();
        if ( single ) {

            KCommand *cmd = new KWFrameStyleCommand( i18n("Apply Framestyle to Frame"), single, _sty );
            if (cmd) {
                m_doc->addCommand( cmd );
                cmd->execute();
            }
        }
    }
    else
    { // it might be that a frame (or several frames) are selected
        QPtrList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
        if (selectedFrames.count() <= 0)
            return; // nope, no frames are selected.
        // yes, indeed frames are selected.
        QPtrListIterator<KWFrame> it( selectedFrames );

        KMacroCommand *globalCmd = new KMacroCommand( selectedFrames.count() == 1 ? i18n("Apply Framestyle to Frame") : i18n("Apply Framestyle to Frames"));

        for ( ; it.current() ; ++it )
        {
            KWFrame *curFrame = it.current();
            KCommand *cmd = new KWFrameStyleCommand( i18n("Apply Framestyle"), curFrame, _sty );
            if (cmd)
                globalCmd->addCommand( cmd );
        }
        m_doc->addCommand( globalCmd );
        globalCmd->execute();
    }

    m_gui->canvasWidget()->repaintAll();
    m_gui->canvasWidget()->setFocus(); // the combo keeps focus...*/

    // Adjust GUI
    QPtrListIterator<KWFrameStyle> styleIt( m_doc->frameStyleCollection()->frameStyleList() );
    for ( int pos = 0 ; styleIt.current(); ++styleIt, ++pos )
    {
        if ( styleIt.current()->name() == _sty->name() ) {
            actionFrameStyle->setCurrentItem( pos );
            KToggleAction* act = dynamic_cast<KToggleAction *>(actionCollection()->action( styleIt.current()->shortCutName().latin1() ));
            if ( act )
                act->setChecked( true );
            return;
        }
    }
}


// Called when selecting a tablestyle in the Table / Tablestyle menu
void KWView::slotTableStyleSelected()
{
    QString actionName = QString::fromLatin1(sender()->name());
    if ( actionName.startsWith( "shortcut_tablestyle_" ) )
    {
        tableStyleSelected( m_doc->tableStyleCollection()->findStyleShortCut( actionName) );
    }
}

void KWView::tableStyleSelected( int index )
{
    tableStyleSelected( m_doc->tableStyleCollection()->tableStyleAt( index ) );
}

// Called by the above, and when selecting a style in the framestyle combobox
void KWView::tableStyleSelected( KWTableStyle *_sty )
{
    if ( !_sty )
        return;

    if ( m_gui->canvasWidget()->currentFrameSetEdit() )
    {
        KWFrame * single = m_gui->canvasWidget()->currentFrameSetEdit()->currentFrame();
        if ( (single) && ( single->frameSet()->type() == FT_TEXT ) )
        {
            KCommand *cmd = new KWTableStyleCommand( i18n("Apply Tablestyle to Frame"), single, _sty );
            if (cmd) {
                m_doc->addCommand( cmd );
                cmd->execute();
            }
        }
    }
    else
    {
        QPtrList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
        if (selectedFrames.count() <= 0)
            return; // nope, no frames are selected.

        QPtrListIterator<KWFrame> it( selectedFrames );

        KMacroCommand *globalCmd = new KMacroCommand( selectedFrames.count() == 1 ? i18n("Apply Tablestyle to Frame") : i18n("Apply Tablestyle to Frames"));

        for ( ; ( ( it.current() ) && ( it.current()->frameSet()->type() == FT_TEXT ) ); ++it )
        {
            KWFrame *curFrame = it.current();
            KCommand *cmd = new KWTableStyleCommand( i18n("Apply Tablestyle to Frame"), curFrame, _sty );
            if (cmd)
                globalCmd->addCommand( cmd );
        }
        m_doc->addCommand( globalCmd );
        globalCmd->execute();
    }

    m_gui->canvasWidget()->repaintAll();
    m_gui->canvasWidget()->setFocus(); // the combo keeps focus...*/

    // Adjust GUI
    QPtrListIterator<KWTableStyle> styleIt( m_doc->tableStyleCollection()->tableStyleList() );
    for ( int pos = 0 ; styleIt.current(); ++styleIt, ++pos )
    {
        if ( styleIt.current()->name() == _sty->name() ) {
            actionTableStyle->setCurrentItem( pos );
            KToggleAction* act = dynamic_cast<KToggleAction *>(actionCollection()->action( styleIt.current()->shortCutName().latin1() ));
            if ( act )
                act->setChecked( true );
            return;
        }
    }
}

void KWView::increaseFontSize()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    KoTextFormat *format = edit->currentFormat();
    if ( edit )
        textSizeSelected( edit->textFrameSet()->textObject()->docFontSize( format ) + 1 );
}

void KWView::decreaseFontSize()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    KoTextFormat *format = edit->currentFormat();
    if ( edit )
        textSizeSelected( edit->textFrameSet()->textObject()->docFontSize( format ) - 1 );
}

void KWView::textSizeSelected( int size )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand *globalCmd = new KMacroCommand(i18n("Change Text Size"));
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setPointSizeCommand( size );
        if (cmd)
            globalCmd->addCommand(cmd);
    }
    m_doc->addCommand(globalCmd);
    m_gui->canvasWidget()->setFocus(); // the combo keeps focus...
}

void KWView::textFontSelected( const QString & font )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setFamilyCommand( font );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Change Text Font") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_doc->addCommand( macroCmd );
    m_gui->canvasWidget()->setFocus(); // the combo keeps focus...
}

QPtrList<KoTextFormatInterface> KWView::applicableTextInterfaces() const
{
    QPtrList<KoTextFormatInterface> lst;
    if (currentTextEdit())
    {
      if ( !currentTextEdit()->textObject()->protectContent())
      {
	// simply return the current textEdit
	lst.append( currentTextEdit() );
	kdDebug() << "text frame name: " << currentTextEdit()->textFrameSet()->name() << endl;
	KWCollectFramesetsVisitor visitor;
	currentTextEdit()->textDocument()->visitSelection( KoTextDocument::Standard, &visitor ); //find all framesets in the selection
	const QValueList<KWFrameSet *>& frameset = visitor.frameSets();
	for ( QValueList<KWFrameSet *>::ConstIterator it = frameset.begin(); it != frameset.end(); ++it )
	{
	  if ( (*it)->type() == FT_TABLE )
	  {
	    KWTableFrameSet* kwtableframeset = static_cast<KWTableFrameSet *>( *it );
	    //kdDebug() << "table found: " << kwtableframeset->getNumFrames() << endl;
	    int const rows  = kwtableframeset->getRows();
	    int const cols = kwtableframeset->getCols();
	    //finding all cells and add them to the interface list
	    for (int r=0; r<rows; ++r)
	    {
	      for (int c=0; c<cols; ++c)
	      {
		KWTableFrameSet::Cell *cell = kwtableframeset->getCell(r,c);
		if (cell)
		{
		  kdDebug() << "adding (" << r << "," << c << ")" << endl;
		  lst.append(cell);
		}
	      }
	    }
	  }
	}
      }
    }
    else
    {   // it might be that a frame (or several frames) are selected
        // in that case, list the text framesets behind them
      QPtrList<KWFrame> selectedFrames = m_doc->getSelectedFrames();
      QPtrListIterator<KWFrame> it( selectedFrames );
      for ( ; it.current() ; ++it )
      {
	if ( it.current()->frameSet()->type() == FT_TEXT ) {
	  KWTextFrameSet* fs = static_cast<KWTextFrameSet *>( it.current()->frameSet() );
	  if ( !lst.contains( fs )&& !fs->protectContent() )
	    lst.append( fs );
	}
      }
    }
    return lst;
}

void KWView::textBold()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setBoldCommand( actionFormatBold->isChecked() );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Make Text Bold") );
            macroCmd->addCommand(cmd);
        }
    }
    if(macroCmd)
        m_doc->addCommand(macroCmd);

}

void KWView::textItalic()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setItalicCommand( actionFormatItalic->isChecked() );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Make Text Italic") );
            macroCmd->addCommand( cmd );
        }
    }
    if( macroCmd)
        m_doc->addCommand( macroCmd );
}

void KWView::textUnderline()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;

    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setUnderlineCommand( actionFormatUnderline->isChecked() );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Underline Text") );
            macroCmd->addCommand( cmd );
        }
    }
    if(macroCmd)
        m_doc->addCommand( macroCmd );
}

void KWView::textStrikeOut()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setStrikeOutCommand( actionFormatStrikeOut->isChecked() );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Strike Out Text") );
            macroCmd->addCommand( cmd );
        }
    }
    if( macroCmd)
        m_doc->addCommand( macroCmd );
}

void KWView::textColor()
{
    /*        QColor color = edit->textColor();
              if ( KColorDialog::getColor( color ) ) {
              actionFormatColor->setColor( color );
              edit->setTextColor( color );
              }
    */
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setTextColorCommand( actionFormatColor->color() );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Set Text Color") );
            macroCmd->addCommand( cmd );
        }
    }
    if( macroCmd)
        m_doc->addCommand( macroCmd );
}

void KWView::textAlignLeft()
{
    if ( actionFormatAlignLeft->isChecked() )
    {
        QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
        QPtrListIterator<KoTextFormatInterface> it( lst );
        KMacroCommand* macroCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KCommand *cmd = it.current()->setAlignCommand( Qt::AlignLeft );
            if (cmd)
            {
                if ( !macroCmd)
                    macroCmd = new KMacroCommand( i18n("Left-Align Text") );
                macroCmd->addCommand( cmd );
            }
        }
        if( macroCmd )
            m_doc->addCommand( macroCmd );
    }
    else
        actionFormatAlignLeft->setChecked( true );
}

void KWView::textAlignCenter()
{
    if ( actionFormatAlignCenter->isChecked() )
    {
        QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
        QPtrListIterator<KoTextFormatInterface> it( lst );
        KMacroCommand* macroCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KCommand *cmd = it.current()->setAlignCommand( Qt::AlignHCenter );
            if (cmd)
            {
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Center Text") );
                macroCmd->addCommand( cmd );
            }
        }
        if( macroCmd )
            m_doc->addCommand( macroCmd );
    }
    else
        actionFormatAlignCenter->setChecked( true );
}

void KWView::textAlignRight()
{
    if ( actionFormatAlignRight->isChecked() )
    {
        QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
        QPtrListIterator<KoTextFormatInterface> it( lst );
        KMacroCommand* macroCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KCommand *cmd = it.current()->setAlignCommand( Qt::AlignRight );
            if ( cmd )
            {
                if (!macroCmd )
                    macroCmd = new KMacroCommand( i18n("Right-Align Text") );
                macroCmd->addCommand( cmd );
            }
        }
        if( macroCmd)
            m_doc->addCommand( macroCmd );
    }
    else
        actionFormatAlignRight->setChecked( true );
}

void KWView::textAlignBlock()
{
    if ( actionFormatAlignBlock->isChecked() )
    {
        QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
        QPtrListIterator<KoTextFormatInterface> it( lst );
        KMacroCommand* macroCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KCommand *cmd = it.current()->setAlignCommand( Qt::AlignJustify );
            if ( cmd )
            {
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Justify Text") );
                macroCmd->addCommand( cmd );
            }
        }
        if( macroCmd)
            m_doc->addCommand( macroCmd );
    }
    else
        actionFormatAlignBlock->setChecked( true );
}

void KWView::slotCounterStyleSelected()
{
    QString actionName = QString::fromLatin1(sender()->name());
    if ( actionName.startsWith( "counterstyle_" ) )
    {
        QString styleStr = actionName.mid(13);
        //kdDebug() << "KWView::slotCounterStyleSelected styleStr=" << styleStr << endl;
        KoParagCounter::Style style = (KoParagCounter::Style)(styleStr.toInt());
        KoParagCounter c;
        if ( style == KoParagCounter::STYLE_NONE )
            c.setNumbering( KoParagCounter::NUM_NONE );
        else {
            c.setNumbering( KoParagCounter::NUM_LIST );
            c.setStyle( style );
            if ( c.isBullet() )
                c.setSuffix( QString::null );
            // else the suffix remains the default, '.'
            // TODO save this setting, to use the last one selected in the dialog?
            // (same for custom bullet char etc.)

            // 68927: restart numbering, by default, if last parag wasn't numbered
            // (and if we're not applying this to a selection)
            if ( currentTextEdit() && !currentTextEdit()->textFrameSet()->hasSelection() ) {
                KoTextParag* parag = currentTextEdit()->cursor()->parag();
                if ( parag->prev() && !parag->prev()->counter() )
                    c.setRestartCounter(true);
            }
        }

        QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
        QPtrListIterator<KoTextFormatInterface> it( lst );
        KMacroCommand* macroCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KCommand *cmd = it.current()->setCounterCommand( c );
            if ( cmd )
            {
                if ( !macroCmd )
                    macroCmd=new KMacroCommand( i18n("Change List Type") );
                macroCmd->addCommand( cmd );
            }
        }
        if( macroCmd)
            m_doc->addCommand( macroCmd );
    }
}

void KWView::textSuperScript()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setTextSuperScriptCommand(actionFormatSuper->isChecked());
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Make Text Superscript") );
            macroCmd->addCommand(cmd);
        }
    }
    if( macroCmd)
        m_doc->addCommand(macroCmd);
}

void KWView::textSubScript()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setTextSubScriptCommand(actionFormatSub->isChecked());
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Make Text Subscript") );
            macroCmd->addCommand(cmd);
        }
    }
    if( macroCmd )
        m_doc->addCommand(macroCmd);
}

void KWView::changeCaseOfText()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KoChangeCaseDia *caseDia=new KoChangeCaseDia( this,"change case" );
    if(caseDia->exec())
    {
        KMacroCommand* macroCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KCommand *cmd = it.current()->setChangeCaseOfTextCommand(caseDia->getTypeOfCase());
            if (cmd)
            {
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Change Case of Text") );
                macroCmd->addCommand(cmd);
            }
        }
        if( macroCmd )
            m_doc->addCommand(macroCmd);
    }
    delete caseDia;
}

void KWView::editPersonalExpr()
{
   KWEditPersonnalExpression *personalDia=new KWEditPersonnalExpression( this );
   if(personalDia->exec())
       m_doc->refreshMenuExpression();
   delete personalDia;
}


void KWView::textIncreaseIndent()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    double leftMargin=0.0;
    if(!lst.isEmpty())
        leftMargin=lst.first()->currentParagLayoutFormat()->margins[QStyleSheetItem::MarginLeft];
    double indent = m_doc->indentValue();
    double newVal = leftMargin + indent;
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setMarginCommand( QStyleSheetItem::MarginLeft, newVal );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Increase Paragraph Depth") );
            macroCmd->addCommand(cmd);
        }
    }
    if( macroCmd)
        m_doc->addCommand(macroCmd);
    if(!lst.isEmpty())
    {
        const KoParagLayout *layout=lst.first()->currentParagLayoutFormat();
        showRulerIndent( layout->margins[QStyleSheetItem::MarginLeft], layout->margins[QStyleSheetItem::MarginFirstLine], layout->margins[QStyleSheetItem::MarginRight], lst.first()->rtl());
    }
#if 0


    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        double leftMargin = edit->currentLeftMargin();
        double indent = m_doc->indentValue();
        double newVal = leftMargin + indent;
        // Test commented out. This breaks with the DTP case... The user can put
        // a frame anywhere, even closer to the edges than left/right border allows (DF).
        //if( newVal <= (m_doc->ptPaperWidth()-m_doc->ptRightBorder()-m_doc->ptLeftBorder()))
        {
            KCommand *cmd=edit->setMarginCommand( QStyleSheetItem::MarginLeft, newVal );
            if(cmd)
                m_doc->addCommand(cmd);
        }
    }
#endif
}

void KWView::textDecreaseIndent()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    double leftMargin=0.0;
    if(!lst.isEmpty())
        leftMargin=lst.first()->currentParagLayoutFormat()->margins[QStyleSheetItem::MarginLeft];
    double indent = m_doc->indentValue();
    double newVal = leftMargin - indent;
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setMarginCommand( QStyleSheetItem::MarginLeft, QMAX( newVal, 0 ) );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Decrease Paragraph Depth") );
            macroCmd->addCommand(cmd);
        }
    }
    if( macroCmd)
        m_doc->addCommand(macroCmd);
    if(!lst.isEmpty())
    {
        const KoParagLayout *layout=lst.first()->currentParagLayoutFormat();
        showRulerIndent( layout->margins[QStyleSheetItem::MarginLeft], layout->margins[QStyleSheetItem::MarginFirstLine], layout->margins[QStyleSheetItem::MarginRight], lst.first()->rtl());
    }


#if 0
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        double leftMargin = edit->currentLeftMargin();
        if ( leftMargin > 0 )
        {
            double indent = m_doc->indentValue();
            double newVal = leftMargin - indent;
            KCommand *cmd=edit->setMarginCommand( QStyleSheetItem::MarginLeft, QMAX( newVal, 0 ) );
            if(cmd)
                m_doc->addCommand(cmd);
        }
    }
#endif
}


void KWView::textDefaultFormat()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setDefaultFormatCommand();
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Default Format") );
            macroCmd->addCommand(cmd);
        }
    }
    if( macroCmd)
        m_doc->addCommand(macroCmd);
}


void KWView::borderOutline()
{
    bool b = actionBorderOutline->isChecked();

    actionBorderLeft->setChecked(b);
    actionBorderRight->setChecked(b);
    actionBorderTop->setChecked(b);
    actionBorderBottom->setChecked(b);

    borderSet();
}

void KWView::borderLeft()
{
    actionBorderOutline->setChecked(
        actionBorderLeft->isChecked() &&
        actionBorderRight->isChecked() &&
        actionBorderTop->isChecked() &&
        actionBorderBottom->isChecked());

    borderSet();
}

void KWView::borderRight()
{
    actionBorderOutline->setChecked(
        actionBorderLeft->isChecked() &&
        actionBorderRight->isChecked() &&
        actionBorderTop->isChecked() &&
        actionBorderBottom->isChecked());

    borderSet();
}

void KWView::borderTop()
{
    actionBorderOutline->setChecked(
        actionBorderLeft->isChecked() &&
        actionBorderRight->isChecked() &&
        actionBorderTop->isChecked() &&
        actionBorderBottom->isChecked());

    borderSet();
}

void KWView::borderBottom()
{
    actionBorderOutline->setChecked(
        actionBorderLeft->isChecked() &&
        actionBorderRight->isChecked() &&
        actionBorderTop->isChecked() &&
        actionBorderBottom->isChecked());

    borderSet();
}

void KWView::borderColor()
{
    m_border.common.color = actionBorderColor->color();
    m_border.left.color = m_border.common.color;
    m_border.right.color = m_border.common.color;
    m_border.top.color = m_border.common.color;
    m_border.bottom.color = m_border.common.color;
    borderSet();
}

void KWView::borderWidth( const QString &width )
{
    m_border.common.setPenWidth( width.toInt());
    m_border.left.setPenWidth(m_border.common.penWidth());
    m_border.right.setPenWidth(m_border.common.penWidth());
    m_border.top.setPenWidth(m_border.common.penWidth());
    m_border.bottom.setPenWidth(m_border.common.penWidth());
    borderSet();
    m_gui->canvasWidget()->setFocus();
}

void KWView::borderStyle( const QString &style )
{
    m_border.common.setStyle(KoBorder::getStyle( style ));
    m_border.left.setStyle( m_border.common.getStyle());
    m_border.right.setStyle( m_border.common.getStyle());
    m_border.top.setStyle( m_border.common.getStyle());
    m_border.bottom.setStyle( m_border.common.getStyle());
    borderSet();
    m_gui->canvasWidget()->setFocus();
}

void KWView::backgroundColor()
{
    QColor backColor = actionBackgroundColor->color();
    // ### TODO port to applicableTextInterfaces ?
    KWTextFrameSetEdit *edit = currentTextEdit();
    if ( m_gui)
    {
        if(edit)
        {
            KCommand *cmd=edit->setTextBackgroundColorCommand(backColor);
            if( cmd)
                m_doc->addCommand( cmd );
        }
        else
            m_gui->canvasWidget()->setFrameBackgroundColor( backColor );
    }
}


void KWView::borderSet()
{
    // The effect of this action depends on if we are in Edit Text or Edit Frame mode.
    m_border.left = m_border.common;
    m_border.right = m_border.common;
    m_border.top = m_border.common;
    m_border.bottom = m_border.common;
    if ( !actionBorderLeft->isChecked() )
    {
        m_border.left.setPenWidth( 0);
    }
    if ( !actionBorderRight->isChecked() )
    {
        m_border.right.setPenWidth( 0);
    }
    if ( !actionBorderTop->isChecked() )
    {
        m_border.top.setPenWidth(0);
    }
    if ( !actionBorderBottom->isChecked() )
    {
        m_border.bottom.setPenWidth(0);
    }
    KWTextFrameSetEdit *edit = currentTextEdit();
    if ( edit )
    {
        KCommand *cmd=edit->setBordersCommand( m_border.left, m_border.right, m_border.top, m_border.bottom );
        if(cmd)
            m_doc->addCommand(cmd);
    }
    else
    {
        KMacroCommand *macro = 0L;
        KCommand*cmd=m_gui->canvasWidget()->setLeftFrameBorder( m_border.common, actionBorderLeft->isChecked() );
        if ( cmd )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Change Border"));
            macro->addCommand( cmd);
        }
        cmd = m_gui->canvasWidget()->setRightFrameBorder( m_border.common, actionBorderRight->isChecked() );
        if ( cmd )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Change Border"));
            macro->addCommand( cmd);
        }

        cmd = m_gui->canvasWidget()->setTopFrameBorder( m_border.common, actionBorderTop->isChecked() );
        if ( cmd )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Change Border"));
            macro->addCommand( cmd);
        }

        cmd = m_gui->canvasWidget()->setBottomFrameBorder( m_border.common, actionBorderBottom->isChecked() );
        if ( cmd )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Change Border"));
            macro->addCommand( cmd);
        }
        if ( macro )
            m_doc->addCommand( macro );
    }
}

void KWView::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    if ( m_gui ) m_gui->resize( width(), height() );
}

void KWView::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    if ( ev->activated() )
    {
        initGui();
        if (m_doc->isEmbedded() )
            setZoom( m_doc->zoom(), true );
    }
    KoView::guiActivateEvent( ev );
}

void KWView::borderShowValues()
{
    actionBorderWidth->setCurrentItem( (int)m_border.common.penWidth() - 1 );
    actionBorderStyle->setCurrentItem( (int)m_border.common.getStyle() );
}

void KWView::tabListChanged( const KoTabulatorList & tabList )
{
    if(!m_doc->isReadWrite())
        return;
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setTabListCommand( tabList );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand(i18n("Change Tabulator") );
            macroCmd->addCommand(cmd);
        }
    }
    if(macroCmd)
        m_doc->addCommand(macroCmd);
}

void KWView::newPageLayout( const KoPageLayout &_layout )
{
    QString mode = m_gui->canvasWidget()->viewMode()->type();
    bool state = (mode!="ModeText");
    if ( !state )
        return;

    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    m_doc->getPageLayout( pgLayout, cl, hf );

    if(_layout==pgLayout)
        return;

    KWPageLayoutStruct oldLayout( pgLayout, cl, hf );

    m_doc->setPageLayout( _layout, cl, hf );

    KWPageLayoutStruct newLayout( _layout, cl, hf );

    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
        edit->textFrameSet()->clearUndoRedoInfo();
    KCommand *cmd = new KWPageLayoutCommand( i18n("Change Layout"), m_doc, oldLayout, newLayout );
    m_doc->addCommand(cmd);
}

void KWView::slotPageLayoutChanged( const KoPageLayout& layout )
{
    // This is connected to a signal of KWDocument, so that when the
    // above method, or any other way of changing the page layout happens,
    // the rulers are updated in all views.
    m_gui->canvasWidget()->viewMode()->setPageLayout( m_gui->getHorzRuler(), m_gui->getVertRuler(), layout );
    m_gui->canvasWidget()->repaintAll( true );
}

void KWView::newFirstIndent( double _firstIndent )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setMarginCommand( QStyleSheetItem::MarginFirstLine, _firstIndent );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand(i18n("Change First Line Indent"));
            macroCmd->addCommand(cmd);
        }
    }
    if(macroCmd)
        m_doc->addCommand(macroCmd);
}

void KWView::newLeftIndent( double _leftIndent )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setMarginCommand( QStyleSheetItem::MarginLeft, _leftIndent );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand(i18n("Change Indent") );
            macroCmd->addCommand(cmd);
        }
    }
    if(macroCmd)
        m_doc->addCommand(macroCmd);

}

void KWView::newRightIndent( double _rightIndent)
{

    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setMarginCommand( QStyleSheetItem::MarginRight, _rightIndent );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand(i18n("Change Indent") );
            macroCmd->addCommand(cmd);
        }
    }
    if(macroCmd)
        m_doc->addCommand(macroCmd);
}

QPopupMenu * KWView::popupMenu( const QString& name )
{
    Q_ASSERT(factory());
    if ( factory() )
        return ((QPopupMenu*)factory()->container( name, this ));
    return 0L;
}

void KWView::openPopupMenuInsideFrame( KWFrame* frame, const QPoint & _point )
{
    KWFrameSetEdit *fse = m_gui->canvasWidget()->currentFrameSetEdit();
    kdDebug() << (void*) fse << " in KWView::openPopupMenuInsideFrame" << endl;
    unplugActionList( "tableactions" ); // will be plugged again by KWTableFrameSetEdit
    if (fse)
        fse->showPopup( frame, this, _point );
    else
        frame->frameSet()->showPopup( frame, this, _point );
}

void KWView::openPopupMenuChangeAction( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() || !factory())
        return;
    QPopupMenu* popup = static_cast<QPopupMenu *>(factory()->container("action_popup",this));
    if ( popup )
        popup->popup(_point);
}

void KWView::openPopupMenuEditFrame( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() || !factory())
        return;
    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    if(!table)
    {
        QPtrList<KAction> actionList;

        int nbFrame=m_doc->getSelectedFrames().count();
        KActionSeparator *separator=new KActionSeparator();
        KActionSeparator *separator2=new KActionSeparator();
        if(nbFrame ==1)
        {
            KWFrame *frame=m_doc->getFirstSelectedFrame();
            KWFrameSet *frameSet=frame->frameSet();
            // ## TODO design: move this to KWFrameSet and derived classes (virtual method)
            if( frameSet->type()==FT_PICTURE )
            {
                actionList.append(separator);
                if ( !frameSet->protectContent() )
                    actionList.append(actionChangePicture);
                actionList.append(actionSavePicture);
            }
#if 0 // KWORD_HORIZONTAL_LINE
            else if ( frameSet->type() == FT_HORZLINE )
            {
                actionList.append(separator);
                actionList.append(actionChangeHorizontalLine);
                actionList.append(actionSavePicture);
            }
#endif
            else if ( frameSet->type() == FT_PART )
            {
                KWPartFrameSet *part = static_cast<KWPartFrameSet *>(frameSet);
                actionEmbeddedStoreInternal->setChecked(part->getChild()->document()->storeInternal());
                actionEmbeddedStoreInternal->setEnabled(part->getChild()->document()->hasExternURL());
                actionList.append(separator);
                actionList.append(actionEmbeddedStoreInternal);
            }
            else if(frameSet->isHeaderOrFooter())
            {
                actionList.append(separator);
                actionList.append(actionConfigureHeaderFooter);
            }
            else if (frameSet->isFootEndNote())
            {
                actionList.append(separator);
                actionGoToFootEndNote->setText( frameSet->isFootNote() ?
                    i18n( "Go to Footnote" ) : i18n( "Go to Endnote" ) );
                actionList.append(actionGoToFootEndNote);
            }
            bool state = !frameSet->isHeaderOrFooter() && !frameSet->isFootEndNote();
            state = state && (m_doc->processingType() == KWDocument::WP &&frameSet!=m_doc->frameSet( 0 ));
            if(state)
            {
                actionList.append(separator2);
                KWFrameSet * parentFs = frameSet->getGroupManager() ? frameSet->getGroupManager() : frameSet;
                actionInlineFrame->setChecked(parentFs->isFloating());
                actionList.append(actionInlineFrame);
            }
        }
        plugActionList( "frameset_type_action", actionList );
        ((QPopupMenu*)factory()->container("frame_popup",this))->exec(_point);
        unplugActionList( "frameset_type_action" );
        delete separator;
        delete separator2;
    }
    else
        ((QPopupMenu*)factory()->container("frame_popup_table",this))->popup(_point);
}

void KWView::startKSpell()
{
#ifdef HAVE_LIBKSPELL2
    if ( !m_spell.kospell )
        m_spell.kospell = new KoSpell( m_broker, this  );

    // Spell-check the next paragraph
    Q_ASSERT( m_spell.textIterator );

    m_spell.kospell->check( m_spell.textIterator, true );
    delete m_spell.dlg;
    m_spell.dlg = new KSpell2::Dialog( m_spell.kospell, this );
    m_spell.dlg->activeAutoCorrect( true );

    QObject::connect( m_spell.dlg, SIGNAL(misspelling(const QString&, int)),
                      this, SLOT(spellCheckerMisspelling(const QString&, int)) );
    QObject::connect( m_spell.dlg, SIGNAL(replace(const QString&, int, const QString&)),
                      this, SLOT(spellCheckerCorrected(const QString&, int, const QString&)) );
    QObject::connect( m_spell.dlg, SIGNAL(done(const QString&) ),
                      this, SLOT(spellCheckerDone(const QString&)) );

    QObject::connect( m_spell.dlg, SIGNAL( stop() ),
                      this, SLOT( spellCheckerFinished( ) ) );
    QObject::connect( m_spell.dlg, SIGNAL(cancel() ),
                      this, SLOT( spellCheckerCancel() ) );
    QObject::connect( m_spell.dlg, SIGNAL(autoCorrect(const QString &, const QString & ) ),
                      this, SLOT( spellAddAutoCorrect (const QString &, const QString &) ) );

    m_spell.dlg->show();
#endif
    //clearSpellChecker();
}

void KWView::spellCheckerMisspelling( const QString &old, int pos )
{
   #ifdef HAVE_LIBKSPELL2
    //kdDebug(32001) << "KWView::spellCheckerMisspelling old=" << old << " pos=" << pos << endl;
    KoTextObject* textobj = m_spell.kospell->currentTextObject();
    KoTextParag* parag = m_spell.kospell->currentParag();
    Q_ASSERT( textobj );
    Q_ASSERT( parag );
    if ( !textobj || !parag ) return;
    KWTextDocument *textdoc=static_cast<KWTextDocument *>( textobj->textDocument() );
    Q_ASSERT( textdoc );
    if ( !textdoc ) return;
    pos += m_spell.kospell->currentStartIndex();
    kdDebug(32001) << "KWView::spellCheckerMisspelling parag=" << parag->paragId() << " pos=" << pos << " length=" << old.length() << endl;
    textdoc->textFrameSet()->highlightPortion( parag, pos, old.length(), m_gui->canvasWidget() );
#endif
}

void KWView::spellCheckerCorrected( const QString &old, int pos , const QString &corr )
{
#ifdef HAVE_LIBKSPELL2
    //kdDebug(32001) << "KWView::spellCheckerCorrected old=" << old << " corr=" << corr << " pos=" << pos << endl;
    KoTextObject* textobj = m_spell.kospell->currentTextObject();
    KoTextParag* parag = m_spell.kospell->currentParag();
    Q_ASSERT( textobj );
    Q_ASSERT( parag );
    if ( !textobj || !parag ) return;
    KWTextDocument *textdoc=static_cast<KWTextDocument *>( textobj->textDocument() );
    Q_ASSERT( textdoc );
    if ( !textdoc ) return;
    pos += m_spell.kospell->currentStartIndex();
    textdoc->textFrameSet()->highlightPortion( parag, pos, old.length(), m_gui->canvasWidget() );

    KoTextCursor cursor( textdoc );
    cursor.setParag( parag );
    cursor.setIndex( pos );
    if(!m_spell.macroCmdSpellCheck)
        m_spell.macroCmdSpellCheck=new KMacroCommand(i18n("Correct Misspelled Word"));
    m_spell.macroCmdSpellCheck->addCommand(textobj->replaceSelectionCommand(
        &cursor, corr, KoTextObject::HighlightSelection, QString::null ));
#endif
}

void KWView::spellCheckerDone( const QString & )
{
#ifdef HAVE_LIBKSPELL2
    //kdDebug(32001) << "KWView::spellCheckerDone" << endl;
    KWTextDocument *textdoc=static_cast<KWTextDocument *>( m_spell.kospell->textDocument() );
    Q_ASSERT( textdoc );
    if ( textdoc )
        textdoc->textFrameSet()->removeHighlight();

    clearSpellChecker();
#endif
}

void KWView::clearSpellChecker(bool cancelSpellCheck)
{
#ifdef HAVE_LIBKSPELL2
    kdDebug(32001) << "KWView::clearSpellChecker" << endl;

    delete m_spell.textIterator;
    m_spell.textIterator = 0L;
    if ( m_spell.macroCmdSpellCheck )
    {
        if ( !cancelSpellCheck )
            m_doc->addCommand(m_spell.macroCmdSpellCheck);
        else
        {
            //reverte all changes
            m_spell.macroCmdSpellCheck->unexecute();
            delete m_spell.macroCmdSpellCheck;
        }
    }
    m_spell.macroCmdSpellCheck=0L;
    m_spell.replaceAll.clear();
    //m_doc->setReadWrite(true);
#endif
}

void KWView::spellCheckerCancel()
{
#ifdef HAVE_LIBKSPELL2
    kdDebug()<<"void KWView::spellCheckerCancel() \n";
    spellCheckerRemoveHighlight();
    //we add command :( => don't add command and reverte changes
    clearSpellChecker(true);
#endif
}


void KWView::spellCheckerRemoveHighlight()
{
#ifdef HAVE_LIBKSPELL2
    KoTextObject* textobj = m_spell.kospell->currentTextObject();
    if ( textobj ) {
        KWTextDocument *textdoc=static_cast<KWTextDocument *>( textobj->textDocument() );
        if ( textdoc )
            textdoc->textFrameSet()->removeHighlight();
    }
    KWTextFrameSetEdit * edit = currentTextEdit();
    if (edit)
        edit->drawCursor( TRUE );
#endif
}

void KWView::spellCheckerFinished()
{
#ifdef HAVE_LIBKSPELL2
    kdDebug(32001) << "KWView::spellCheckerFinished (death)" << endl;
    spellCheckerRemoveHighlight();
    clearSpellChecker();

#endif
}

void KWView::spellAddAutoCorrect (const QString & originalword, const QString & newword)
{
    m_doc->autoFormat()->addAutoFormatEntry( originalword, newword );
}

void KWView::configure()
{
    KWConfig configDia( this );
    configDia.exec();
}

KWTextFrameSetEdit *KWView::currentTextEdit() const
{
    if (!m_gui)
        return 0L;
    KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
        return dynamic_cast<KWTextFrameSetEdit *>(edit->currentTextEdit());
    return 0L;
}

void KWView::slotFrameSetEditChanged()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    bool rw = koDocument()->isReadWrite();
    bool hasSelection = false;
    if ( edit ) {
        hasSelection = edit->textFrameSet()->hasSelection();
        if ( edit->textFrameSet()->textObject()->protectContent())
            rw = false;
    }
    else {
        KWFrameSetEdit * e = m_gui->canvasWidget()->currentFrameSetEdit();
        if ( e && e->frameSet()->type() == FT_FORMULA ) {
            hasSelection = true;
        }
    }
    actionEditCut->setEnabled( hasSelection && rw );

    actionEditCopy->setEnabled( hasSelection );
    actionEditReplace->setEnabled( /*edit &&*/ rw );
    clipboardDataChanged(); // for paste

    bool state = (edit != 0L) && rw;
    actionEditSelectAll->setEnabled(state);
    actionInsertComment->setEnabled( state );
    actionFormatDefault->setEnabled( rw);
    actionFormatFont->setEnabled( rw );
    actionFormatFontSize->setEnabled( rw );
    actionFormatFontFamily->setEnabled( rw );
    actionAddBookmark->setEnabled(state);
    actionBackgroundColor->setEnabled( rw );
    actionFormatStyleMenu->setEnabled( rw );
    actionFormatBold->setEnabled( rw );
    actionFormatItalic->setEnabled( rw );
    actionFormatUnderline->setEnabled( rw );
    actionFormatStrikeOut->setEnabled( rw );
    actionFormatColor->setEnabled( rw );
    actionFormatAlignLeft->setEnabled( rw );
    actionFormatAlignCenter->setEnabled( rw );
    actionFormatAlignRight->setEnabled( rw );
    actionFormatAlignBlock->setEnabled( rw );

    actionBorderLeft->setEnabled( rw );
    actionBorderRight->setEnabled( rw );
    actionBorderTop->setEnabled( rw );
    actionBorderBottom->setEnabled( rw );
    actionBorderOutline->setEnabled( rw );
    actionBorderColor->setEnabled( rw );
    actionBorderWidth->setEnabled( rw );
    actionBorderStyle->setEnabled( rw );


    //actionFormatIncreaseIndent->setEnabled(state);
    actionInsertLink->setEnabled(state);
#if 0 // KWORD_HORIZONTAL_LINE
    actionInsertHorizontalLine->setEnabled( state);
#endif
    actionCreateStyleFromSelection->setEnabled( state /*&& hasSelection*/);
    actionConvertToTextBox->setEnabled( state && hasSelection);
    actionAddPersonalExpression->setEnabled( state && hasSelection);
    actionSortText->setEnabled( state && hasSelection);
    bool goodleftMargin=false;
    if(state)
        goodleftMargin=(edit->currentLeftMargin()>0);

    actionFormatDecreaseIndent->setEnabled(goodleftMargin /*&& state*/);
    bool isFootNoteSelected = ((rw && edit && !edit->textFrameSet()->isFootEndNote())||(!edit&& rw));
    actionFormatBullet->setEnabled(isFootNoteSelected);
    actionFormatNumber->setEnabled(isFootNoteSelected);
    actionFormatStyle->setEnabled(isFootNoteSelected);
    actionFormatSuper->setEnabled(rw);
    actionFormatSub->setEnabled(rw);
    actionFormatParag->setEnabled(state);
    actionInsertSpecialChar->setEnabled(state);
    actionSpellCheck->setEnabled(state);

    actionChangeCase->setEnabled( (rw && !edit)|| (state && hasSelection) );

    if ( edit && edit->textFrameSet()->protectContent())
    {
        actionChangeCase->setEnabled( false );
        actionEditCut->setEnabled( false );
    }
    else
        actionChangeCase->setEnabled( true );

    updateTableActions( -1 );

    actionInsertFormula->setEnabled(state && (m_gui->canvasWidget()->viewMode()->type()!="ModeText"));
    actionInsertVariable->setEnabled(state);
    actionInsertExpression->setEnabled(state);

    changeFootEndNoteState();
    //frameset different of header/footer
    state= state && edit && edit->frameSet() && !edit->frameSet()->isHeaderOrFooter() && !edit->frameSet()->getGroupManager() && !edit->frameSet()->isFootEndNote();
    actionInsertContents->setEnabled(state);
    actionInsertFrameBreak->setEnabled( state );
    updatePageInfo();
}

void KWView::changeFootEndNoteState()
{
    bool rw = koDocument()->isReadWrite();
    KWTextFrameSetEdit * edit = currentTextEdit();
    QString mode = m_gui->canvasWidget()->viewMode()->type();

    bool isEditableFrameset = edit && edit->frameSet() && edit->frameSet()->isMainFrameset();
    bool ok = rw && isEditableFrameset && (mode!="ModeText");
    actionInsertFootEndNote->setEnabled( ok );
    actionEditFootEndNote->setEnabled( ok );
}

void KWView::changeFootNoteMenuItem( bool _footnote)
{
    actionEditFootEndNote->setText( _footnote? i18n("Edit Footnote"): i18n("Edit Endnote"));
    actionChangeFootNoteType->setText( _footnote? i18n("Change Footnote Parameter"):i18n("Change Endnote Parameter"));
}

void KWView::slotUpdateRuler()
{
    KWCanvas* canvas = m_gui->canvasWidget();
    QRect r( canvas->viewMode()->rulerFrameRect( canvas ) );
    if ( !r.isNull() )
    {
        m_gui->getHorzRuler()->setFrameStartEnd( r.left(), r.right() );
        m_gui->getVertRuler()->setFrameStartEnd( r.top(), r.bottom() );
    }
    canvas->updateRulerOffsets();
}

void KWView::frameSelectedChanged()
{
    bool rw = koDocument()->isReadWrite();
    QPtrList<KWFrame> selectedFrames = m_doc->getSelectedFrames();
    int nbFrame = selectedFrames.count();

    actionFormatFrameSet->setEnabled( nbFrame>=1 );
    if ( rw && nbFrame >= 1 )
    {
        bool okForDelete = true;
        bool okForLowerRaise = false;
        bool okForChangeParagStyle = true;
        bool okForChangeInline = true;
        QPtrListIterator<KWFrame> it( selectedFrames );
        for ( ; it.current() && ( okForDelete || okForLowerRaise || okForChangeParagStyle || okForChangeInline) ; ++it )
        {
            // Check we selected no footer nor header
            bool isFootNote = it.current()->frameSet()->isFootEndNote();
            bool headerFooterFootNote = it.current()->frameSet()->isHeaderOrFooter() || isFootNote;
            bool isMainWPFrame = it.current()->frameSet()->isMainFrameset();
            okForChangeParagStyle &= !isFootNote;

            okForDelete &= !headerFooterFootNote;
            okForDelete &= !isMainWPFrame;

            // Check we selected a frame we can lower raise.
            // The header, footer, main frameset, footnotes and inline frames can't be raised.
            // As soon as we find one who we can lower/raise open the option.
            okForLowerRaise |= !(isMainWPFrame || headerFooterFootNote || it.current()->frameSet()->isFloating());
            okForChangeInline &= !(isMainWPFrame || headerFooterFootNote );
        }
        actionEditDelFrame->setEnabled( okForDelete );
        actionEditCut->setEnabled( okForDelete );

        actionLowerFrame->setEnabled( okForLowerRaise );
        actionRaiseFrame->setEnabled( okForLowerRaise );
        actionSendBackward->setEnabled( okForLowerRaise );
        actionBringToFront->setEnabled( okForLowerRaise );
        actionFormatBullet->setEnabled( okForChangeParagStyle );
        actionFormatNumber->setEnabled( okForChangeParagStyle );
        actionFormatStyle->setEnabled( okForChangeParagStyle);
        actionInlineFrame->setEnabled( okForChangeInline);

    } else
    {   // readonly document, or no frame selected -> disable
        actionEditDelFrame->setEnabled( false );
        actionInlineFrame->setEnabled(false);
        actionEditCut->setEnabled( false );
        actionLowerFrame->setEnabled( false );
        actionRaiseFrame->setEnabled( false );
        actionSendBackward->setEnabled( false );
        actionBringToFront->setEnabled( false );

    }
    bool frameDifferentOfPart=false;
    if(nbFrame >= 1)
    {
        QPtrListIterator<KWFrame> it( selectedFrames );
        for ( ; it.current(); ++it )
        {
            if ( it.current()->frameSet()->type()!=FT_PART && it.current()->frameSet()->type()!= FT_PICTURE)
            {
                frameDifferentOfPart=true;
                break;
            }
        }
    }

    actionBackgroundColor->setEnabled( (nbFrame >= 1) && frameDifferentOfPart);
    actionBackgroundColor->setText(i18n("Frame Background Color..."));

    if ( frameDifferentOfPart ) {
        KWFrame *frame = m_doc->getFirstSelectedFrame();

        if ( frame )
        {
            QColor frameCol=frame->backgroundColor().color();
            //actionBackgroundColor->setText(i18n("Frame Background Color..."));
            actionBackgroundColor->setCurrentColor( frameCol.isValid()? frame->backgroundColor().color() :  QApplication::palette().color( QPalette::Active, QColorGroup::Base ));
        }
    }

    actionCreateFrameStyle->setEnabled( nbFrame==1 );
    actionCreateLinkedFrame->setEnabled( nbFrame==1 );

    actionEditCopy->setEnabled( nbFrame >= 1 );

    updateTableActions( nbFrame );
    m_doc->refreshFrameBorderButton();

    updatePageInfo(); // takes care of slotUpdateRuler()
    updateFrameStatusBarItem();

    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( !lst.isEmpty() )
    {
        QPtrListIterator<KoTextFormatInterface> it( lst );
        KoTextFormat format=*(lst.first()->currentFormat());
        showFormat( format );

        const KoParagLayout * paragLayout=lst.first()->currentParagLayoutFormat();
        KoParagCounter counter;
        if(paragLayout->counter)
            counter = *(paragLayout->counter);
        showCounter( counter );
        int align = paragLayout->alignment;
        if ( align == Qt::AlignAuto )
            align = Qt::AlignLeft; // ## seems hard to detect RTL here
        showAlign( align );
    }
}


void KWView::updateTableActions( int nbFramesSelected )
{
    if ( nbFramesSelected == -1 ) // not calculated by caller
    {
        QPtrList<KWFrame> selectedFrames = m_doc->getSelectedFrames();
        nbFramesSelected = selectedFrames.count();
    }

    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    actionTableJoinCells->setEnabled( table && (nbFramesSelected>1));
    actionConvertTableToText->setEnabled( table && table->isFloating() );

    bool oneCellSelected = (table && nbFramesSelected==1);
    actionTableSplitCells->setEnabled( oneCellSelected ); // TODO also allow to split current cell

    // cellEdited: true if the cursor is in a table cell
    bool cellEdited = table && ( m_gui->canvasWidget()->currentTableRow() > -1 );
    // rowKnown: true if cellEdited or if entire row(s) selected
    bool rowKnown = table && ( cellEdited || table->isRowsSelected() );
    actionTableInsertRow->setEnabled( rowKnown );
    actionTableDelRow->setEnabled( rowKnown );
    // colKnown: true if cellEdited or if entire col(s) selected
    bool colKnown = table && ( cellEdited || table->isColsSelected() );
    actionTableInsertCol->setEnabled( colKnown );
    actionTableDelCol->setEnabled( colKnown );
    // TODO (after msg freeze) : update text of actionTableDelCol to
    // either "Delete Current Column" or "Delete Selected Columns".
    // Same with actionTableDelRow.

    actionTableResizeCol->setEnabled( table );
    actionTableDelete->setEnabled( table );
    actionTablePropertiesMenu->setEnabled( table );

    bool cellsSelected = (table && nbFramesSelected>0);
    actionTableUngroup->setEnabled( cellsSelected );
    actionTableProtectCells->setEnabled( cellsSelected );
    if ( cellsSelected )
    {
        unsigned int row = 0;
        unsigned int col = 0;
        table->getFirstSelected(row, col );
        bool _protect = table->getCell( row, col )->protectContent();
        actionTableProtectCells->setChecked(_protect);
    }
}

void KWView::docStructChanged(int _type)
{
    KWDocStruct *m_pDocStruct=m_gui->getDocStruct();
    if(m_pDocStruct)
        m_pDocStruct->getDocStructTree()->refreshTree(_type);
    m_doc->recalcVariables(  VT_STATISTIC );
}

void KWView::setViewFrameBorders(bool b)
{
    m_viewFrameBorders = b;
    // Store setting in doc, for further views and for saving
    m_doc->setViewFrameBorders( b );
    m_doc->viewMode()->setDrawFrameBorders( b );
}

bool KWView::doubleClickActivation() const
{
    return TRUE;
}

QWidget* KWView::canvas()
{
    return m_gui->canvasWidget()->viewport();
}

int KWView::canvasXOffset() const
{
    return m_gui->canvasWidget()->contentsX();
}

int KWView::canvasYOffset() const
{
    return m_gui->canvasWidget()->contentsY();
}

void KWView::canvasAddChild( KoViewChild * /*child*/ )
{
    // Not necessary anymore
    //m_gui->canvasWidget()->addChild( child->frame() );
}

void KWView::changePicture()
{
    KWFrame * frame = m_doc->getFirstSelectedFrame();
    KWPictureFrameSet *frameset = static_cast<KWPictureFrameSet *>(frame->frameSet());
    if ( frameset->protectContent() )
        return;
    KoPictureKey oldKey ( frameset->picture().getKey() );
    QString oldFile ( oldKey.filename() );
    KURL url;
    url.setPath( oldFile );
    if (!QDir(url.directory()).exists())
        oldFile = url.fileName();

    KoPicture picture ( KWInsertPicDia::selectPictureDia( oldFile, this ) );
    if ( !picture.isNull() )
    {
        KWFrameChangePictureCommand *cmd= new KWFrameChangePictureCommand( i18n("Change Picture"), FrameIndex(frame), oldKey, picture.getKey() ) ;

        frameset->insertPicture( picture );
        m_doc->frameChanged( frame );
        m_doc->refreshDocStructure( FT_PICTURE );
        m_doc->addCommand(cmd);
    }
    else
        kdDebug() << "KWView::changePicture cancelled" << endl;
}

void KWView::savePicture()
{
    KWFrame * frame = m_doc->getFirstSelectedFrame();
    if ( frame )//test for dcop call
    {
        KWPictureFrameSet *frameset = static_cast<KWPictureFrameSet *>(frame->frameSet());
        QString oldFile = frameset->picture().getKey().filename();
        KURL url;
        url.setPath( oldFile );
        if ( !QDir(url.directory()).exists() )
            oldFile = url.fileName();

        KoPicture picture( frameset->picture() );
        QString mimetype = picture.getMimeType();
        kdDebug() << "Picture has mime type: " << mimetype << endl;
        QStringList mimetypes;
        mimetypes << mimetype;
        KFileDialog fd( oldFile, QString::null, this, 0, TRUE );
        fd.setMimeFilter( mimetypes );
        fd.setCaption(i18n("Save Picture"));
        fd.setOperationMode(KFileDialog::Saving);
        if ( fd.exec() == QDialog::Accepted )
        {
            url = fd.selectedURL();
            if ( url.isValid() )
            {
                if ( url.isLocalFile() )
                {
                    QFile file( url.path() );
                    if ( file.open( IO_ReadWrite ) )
                    {
                        picture.save( &file );
                        file.close();
                    }
                    else
                    {
                        KMessageBox::error(this,
                                   i18n("Error during saving. Could not open '%1' for writing").arg ( url.path() ),
                                   i18n("Save Picture"));
                    }
                }
                else
                {
                    KTempFile tempFile;
                    tempFile.setAutoDelete( true );
                    if ( tempFile.status() == 0 )
                    {
                        QFile file( tempFile.name() );
                        if ( file.open( IO_ReadWrite ) )
                        {
                            picture.save( &file );
                            file.close();
                            if ( !KIO::NetAccess::upload( tempFile.name(), url, this ) )
                            {
                              KMessageBox::sorry( this, i18n(
                                  "Unable to save the file to '%1'. %2.").arg( url.prettyURL() ).arg( KIO::NetAccess::lastErrorString() ),
                                  i18n("Save Failed") );
                            }
                        }
                        else
                            KMessageBox::error(this,
                                i18n("Error during saving. Could not open '%1' temporary file for writing").arg ( file.name() ),
                                i18n("Save Picture"));
                    }
                    else
                        KMessageBox::sorry( this, i18n(
                            "Error during saving. Could not create temporary file: %1.").arg( strerror( tempFile.status() ) ),
                            i18n("Save Picture") );
                }
            }
            else
                KMessageBox::sorry( this, i18n("URL %1 is invalid.").arg( url.prettyURL() ), i18n("Save Picture") );
        }
    }
}

void KWView::configureHeaderFooter()
{
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter kwhf;
    m_doc->getPageLayout( pgLayout, cl, kwhf );

    KWPageLayoutStruct oldLayout( pgLayout, cl, kwhf );

    KoHeadFoot hf;
    int flags = KW_HEADER_AND_FOOTER;
    KoUnit::Unit unit = m_doc->unit();
    KoUnit::Unit oldUnit = unit;

    if ( KoPageLayoutDia::pageLayout( pgLayout, hf, cl, kwhf, flags, unit ) ) {
        if( oldLayout._hf != kwhf )
        {
            KWPageLayoutStruct newLayout( pgLayout, cl, kwhf );

            KCommand *cmd = new KWPageLayoutCommand( i18n("Change Layout"), m_doc,
                                                     oldLayout, newLayout );
            m_doc->addCommand(cmd);

            m_doc->setPageLayout( pgLayout, cl, kwhf );
        }
        if ( unit != oldUnit )
            m_doc->setUnit( unit ); // needs undo/redo support
    }

}

void KWView::inlineFrame()
{
    KWFrame * frame = m_doc->getFirstSelectedFrame();
    KWFrameSet * fs = frame->frameSet();
    KWFrameSet * parentFs = fs->getGroupManager() ? fs->getGroupManager() : fs;

    if(actionInlineFrame->isChecked())
    {

        KMacroCommand* macroCmd = new KMacroCommand( i18n("Make Frameset Inline") );
        QValueList<FrameIndex> frameindexList;
        QValueList<FrameMoveStruct> frameindexMove;

        KoPoint initialPos = frame->topLeft();
        // turn non-floating frame into floating frame
        KWFrameSetInlineCommand *cmd = new KWFrameSetInlineCommand( i18n("Make Frameset Inline"), parentFs, true );
        cmd->execute();

        frameindexList.append( FrameIndex( frame ) );
        frameindexMove.append( FrameMoveStruct( initialPos, frame->topLeft() ) );

        KWFrameMoveCommand *cmdMoveFrame = new KWFrameMoveCommand( i18n("Move Frame"), frameindexList, frameindexMove );

        macroCmd->addCommand(cmdMoveFrame);
        macroCmd->addCommand(cmd);
        m_doc->addCommand(macroCmd);
    }
    else
    {
        KWFrameSetInlineCommand *cmd = new KWFrameSetInlineCommand( i18n("Make Frameset Non-Inline"), parentFs, false );
        m_doc->addCommand(cmd);
        cmd->execute();
    }
}

void KWView::openLink()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->openLink();
}

void KWView::changeLink()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KoLinkVariable * var=edit->linkVariable();
        if(var)
        {
            QString oldhref= var->url();
            QString oldLinkName=var->value();
            QString link=oldLinkName;
            QString ref=oldhref;
            if(KoInsertLinkDia::createLinkDia(link, ref, m_doc->listOfBookmarkName(0), this))
            {
                if(!link.isEmpty() && !ref.isEmpty())
                {
                    if( ref != oldhref || link!=oldLinkName)
                    {
                        KWChangeLinkVariable*cmd=new KWChangeLinkVariable( i18n("Change Link"), m_doc,oldhref, ref, oldLinkName,link, var);
                        cmd->execute();
                        m_doc->addCommand(cmd);
                    }
                }
            }
        }
    }
}

void KWView::copyLink()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->copyLink();
}

void KWView::removeLink()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->removeLink();
}

void KWView::addToBookmark()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KoLinkVariable * var=edit->linkVariable();
        if(var)
        {
            edit->addBookmarks(var->url());
        }
    }
}

void KWView::showDocStructure()
{
    m_doc->setShowDocStruct(actionShowDocStruct->isChecked());
    m_doc->reorganizeGUI();
}

void KWView::showRuler()
{
    m_doc->setShowRuler( actionShowRuler->isChecked());
    m_doc->reorganizeGUI();
}

void KWView::slotSoftHyphen()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertSoftHyphen();
}

void KWView::slotNonbreakingSpace()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertNonbreakingSpace();
}

void KWView::slotNonbreakingHyphen()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertNonbreakingHyphen();
}

void KWView::slotLineBreak()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertLineBreak();
}

void KWView::slotIncreaseNumberingLevel()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->increaseNumberingLevel( m_doc->styleCollection() );
}

void KWView::slotDecreaseNumberingLevel()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->decreaseNumberingLevel( m_doc->styleCollection() );
}

void KWView::refreshAllVariable()
{
    m_doc->recalcVariables( VT_ALL );
}

void KWView::slotAllowAutoFormat()
{
    bool state = actionAllowAutoFormat->isChecked();
    m_doc->setAllowAutoFormat( state );
}

void KWView::slotCompletion()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->completion();
}

void KWView::updateHeaderFooterButton()
{
    actionViewHeader->setChecked(m_doc->isHeaderVisible());
    actionViewFooter->setChecked(m_doc->isFooterVisible());
}

void KWView::editComment()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KoVariable * tmpVar=edit->variable();
        KoNoteVariable * var = dynamic_cast<KoNoteVariable *>(tmpVar);
        if(var)
        {
            QString authorName;
            KoDocumentInfo * info = m_doc->documentInfo();
            KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
            if ( !authorPage )
                kdWarning() << "Author information not found in documentInfo !" << endl;
            else
                authorName = authorPage->fullName();
            QString oldValue = var->note();
            QString createDate = var->createdNote();
            KoCommentDia *commentDia = new KoCommentDia( this, oldValue, authorName, createDate);
            if( commentDia->exec() )
            {
                if ( oldValue != commentDia->commentText())
                {
                    KWChangeVariableNoteText *cmd = new KWChangeVariableNoteText( i18n("Change Note Text"), m_doc, oldValue,commentDia->commentText(), var);
                    m_doc->addCommand( cmd );
                    cmd->execute();
                }
            }
            delete commentDia;
        }
    }
}

void KWView::fileStatistics()
{
    KWStatisticsDialog *statisticsDialog = new KWStatisticsDialog( this, m_doc );
    if ( !statisticsDialog->wasCanceled() )
        statisticsDialog->exec();
    delete statisticsDialog;
}

void KWView::removeComment()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        edit->removeComment();
    }
}

void KWView::copyTextOfComment()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        edit->copyTextOfComment();
    }
}

void KWView::configureCompletion()
{
    m_doc->autoFormat()->readConfig();
    KoCompletionDia dia( this, 0, m_doc->autoFormat() );
    dia.exec();
}

void KWView::applyAutoFormat()
{
    m_doc->autoFormat()->readConfig();
    KMacroCommand *macro = 0L;
    QValueList<KoTextObject *> list(m_doc->visibleTextObjects(m_gui->canvasWidget()->viewMode()));
    QValueList<KoTextObject *>::Iterator fit = list.begin();
    for ( ; fit != list.end() ; ++fit )
    {
        KCommand *cmd = m_doc->autoFormat()->applyAutoFormat( *fit );
        if ( cmd )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Apply Autoformat"));
            macro->addCommand( cmd );
        }
    }
    if ( macro )
        m_doc->addCommand( macro );
}

void KWView::createStyleFromSelection()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        QStringList list;
        QPtrListIterator<KWStyle> styleIt( m_doc->styleCollection()->styleList() );
        for ( ; styleIt.current(); ++styleIt )
        {
            list.append( styleIt.current()->name() );
        }
        KoCreateStyleDia *dia = new KoCreateStyleDia( QStringList(), this, 0 );
        if ( dia->exec() )
        {
            QString name = dia->nameOfNewStyle();
            if ( list.contains( name ) ) // update existing style
            {
                // TODO confirmation message box
                KoParagStyle* style = m_doc->styleCollection()->findStyle( name );
                Q_ASSERT( style );
                if ( style )
                    edit->updateStyleFromSelection( style );
            }
            else // create new style
            {
                KoParagStyle *style = edit->createStyleFromSelection( name );
                m_doc->styleCollection()->addStyleTemplate( style );
                m_doc->updateAllStyleLists();
            }
            showStyle( name );
        }
        delete dia;
    }
}

// Initially called by initGUIButton
void KWView::switchModeView()
{
    // Apply the same viewmode to all views (due to limitations in the text formatter)
    // So we get the viewmode to use from the document.
    m_gui->canvasWidget()->switchViewMode( m_doc->viewMode() );
    slotUpdateRuler();

    // Now update the actions appropriately
    QString mode = m_gui->canvasWidget()->viewMode()->type();
    bool isTextMode = mode == "ModeText";
    bool state = !isTextMode;
    actionToolsCreateText->setEnabled(state);
    actionToolsCreatePix->setEnabled(state);
    actionToolsCreatePart->setEnabled(state);
    actionInsertFormula->setEnabled(state);
    actionInsertTable->setEnabled(state);
    changeFootEndNoteState();
    actionViewFooter->setEnabled( state && m_doc->processingType() == KWDocument::WP );
    actionViewHeader->setEnabled( state && m_doc->processingType() == KWDocument::WP );
    //actionViewTextMode->setEnabled(m_doc->processingType()==KWDocument::WP);
    actionShowDocStruct->setEnabled(state);
    actionFormatPage->setEnabled(state);
    actionInsertContents->setEnabled( state );
    actionFrameStyle->setEnabled( state );
    actionTableStyle->setEnabled ( state );
    if ( m_gui->getHorzRuler())
    {
        m_gui->getHorzRuler()->setPageLayoutMenuItemEnabled( state );
        if ( !koDocument()->isReadWrite())
            m_gui->getHorzRuler()->changeFlags( KoRuler::F_NORESIZE );
        else
        {
            if ( state )
                m_gui->getHorzRuler()->changeFlags( m_gui->getHorzRuler()->flags() & ~(KoRuler::F_NORESIZE) );
            else
                m_gui->getHorzRuler()->changeFlags( m_gui->getHorzRuler()->flags() | KoRuler::F_NORESIZE );
        }
    }
    if ( m_gui->getVertRuler() )
    {
        m_gui->getVertRuler()->setPageLayoutMenuItemEnabled( state );
        if ( !koDocument()->isReadWrite())
            m_gui->getVertRuler()->changeFlags( KoRuler::F_NORESIZE );
        else
        {
            if ( state )
                m_gui->getVertRuler()->changeFlags( m_gui->getVertRuler()->flags() & ~(KoRuler::F_NORESIZE) );
            else
                m_gui->getVertRuler()->changeFlags( m_gui->getVertRuler()->flags() | KoRuler::F_NORESIZE );

        }
    }

    if ( isTextMode )
    {
        if ( m_doc->showdocStruct() )
        {
            m_doc->setShowDocStruct(false);
            m_doc->reorganizeGUI();
        }
    }
    else
    {
        m_doc->setShowDocStruct(actionShowDocStruct->isChecked());
        m_doc->reorganizeGUI();
    }
    //recalc pgnum variable when we swith viewmode
    //because in text mode view we display field code and not value
    //normal because we don't have real page in this mode
    m_doc->recalcVariables( VT_PGNUM );
    if ( isTextMode )
    {
        // Make sure we edit the same frameset as the one shown in the textview ;-)
        m_gui->canvasWidget()->editFrameSet( static_cast<KWViewModeText* >(m_doc->viewMode())->textFrameSet() );

    }
    //remove/add "zoom to page". Not necessary in text mode view.
    changeZoomMenu( m_doc->zoom() );
    showZoom( m_doc->zoom() );
    updatePageInfo();
    m_doc->viewMode()->setPageLayout( m_gui->getHorzRuler(), m_gui->getVertRuler(), m_doc->pageLayout() );
}

void KWView::configureFootEndNote()
{
    KWConfigFootNoteDia *dia = new KWConfigFootNoteDia( this, "configfootnote", m_doc );
    dia->exec();
    delete dia;
}

void KWView::editFootEndNote()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KWFootNoteVariable * var = dynamic_cast<KWFootNoteVariable *>( edit->variable() );
        if ( var && var->frameSet() )
        {
            m_gui->canvasWidget()->editFrameSet( var->frameSet() );
            KWTextFrameSetEdit* textedit = currentTextEdit();
            if ( textedit )
                textedit->ensureCursorVisible();
        }
    }
}

void KWView::changeFootNoteType()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KoVariable * tmpVar=edit->variable();
        KWFootNoteVariable * var = dynamic_cast<KWFootNoteVariable *>(tmpVar);
        if(var && var->frameSet())
        {
            KWFootNoteDia dia( var->noteType(), var->numberingType(), (var->numberingType()==KWFootNoteVariable::Auto) ? QString::null : var->manualString(), this, m_doc, 0 );
            if ( dia.exec() )
            {
                FootNoteParameter oldParam( var );
                FootNoteParameter newParam(dia.noteType(), dia.numberingType(), dia.manualString() );
                KWChangeFootNoteParametersCommand * cmd = new KWChangeFootNoteParametersCommand( i18n("Change Footnote Parameters"), var , oldParam, newParam, m_doc);
                cmd->execute();
                m_doc->addCommand ( cmd );
            }
        }
    }
}


void KWView::autoSpellCheck()
{
    autoSpellCheck( actionAllowBgSpellCheck->isChecked() );
}

void KWView::autoSpellCheck(bool b)
{
    m_doc->changeBgSpellCheckingState( b );
}

void KWView::goToFootEndNote()
{
    KWFrame * frm = m_doc->getFirstSelectedFrame();
    if ( frm )
    {
        KWFootNoteFrameSet *footNoteFrameSet = dynamic_cast<KWFootNoteFrameSet *>(frm->frameSet());
        if ( footNoteFrameSet )
        {
            KWFootNoteVariable* var=footNoteFrameSet->footNoteVariable();
            KoTextParag *parag = var->paragraph();
            int index = var->index();
            KWTextDocument *textDoc = static_cast<KWTextDocument *>(var->textDocument());
            KWTextFrameSet *frameSet =textDoc->textFrameSet();
            m_gui->canvasWidget()->editTextFrameSet( frameSet, parag, index );
        }
    }
}

void KWView::openDocStructurePopupMenu( const QPoint &_p, KWFrameSet *frameset)
{
    if(!koDocument()->isReadWrite() )
        return;
    QPtrList<KAction> actionList;

    bool state = (frameset->type()==FT_TEXT || frameset->type()==FT_TABLE || frameset->type()==FT_FORMULA );
    if ( state )
        actionList.append(actionEditFrameSet);
    actionDeleteFrameSet->setEnabled( (!frameset->isMainFrameset() && !frameset->isFootEndNote() && !frameset->isHeaderOrFooter()) );

    plugActionList( "edit_action", actionList );

    QPopupMenu* popup = static_cast<QPopupMenu *>(factory()->container("docstruct_popup",this));
    if ( popup )
        popup->exec(_p);
    unplugActionList( "edit_action" );
}

void KWView::selectFrameSet()
{
    if ( m_gui->getDocStruct() )
    {
        m_gui->getDocStruct()->selectFrameSet();
    }
}

void KWView::editFrameSet()
{
    if ( m_gui->getDocStruct() )
    {
        m_gui->getDocStruct()->editFrameSet();
        //return focus to canvas.
        m_gui->canvasWidget()->setFocus();
    }
}

void KWView::editFrameSetProperties()
{
    if ( m_gui->getDocStruct() )
    {
        m_gui->getDocStruct()->editProperties();
    }
}

void KWView::deleteFrameSet()
{
    if ( m_gui->getDocStruct() )
    {
        m_gui->getDocStruct()->deleteFrameSet();
    }
}

void KWView::insertFile()
{
    KFileDialog fd( QString::null, QString::null, this, 0, TRUE );
    fd.setMimeFilter( "application/x-kword" );
    fd.setCaption(i18n("Insert File"));
    KURL url;
    if ( fd.exec() != QDialog::Accepted )
        return;
    url = fd.selectedURL();
    if( url.isEmpty() )
    {
        KMessageBox::sorry( this,
                            i18n("File name is empty."),
                            i18n("Insert File"));
        return;
    }
    insertFile( url );
}

void KWView::insertFile(const KURL& _url)
{
    KMacroCommand* macroCmd = 0L;
    bool hasFixedFramesets = false;
    KoStore* store=KoStore::createStore( this, _url, KoStore::Read );

    // ###### All of this should be ported to use the pasting code instead.

    QString maindoc = "maindoc.xml";
    if ( store )
    {
        // We need to load the pictures before we treat framesets
        // because KWDocument::pasteFrames() calls processPictureRequests().
        bool b = store->open(maindoc);
        if ( !b )
        {
            KMessageBox::sorry( this,
                                i18n("This file is not a KWord file!"),
                                i18n("Insert File"));
            delete store;
            return;
        }

        QDomDocument doc;
        doc.setContent( store->device() );
        QDomElement word = doc.documentElement();

        m_doc->loadPictureMap( word );
        store->close();
        m_doc->loadImagesFromStore( store );
    }
    if ( store )
    {
        bool b = store->open(maindoc);
        if ( !b )
        {
            KMessageBox::sorry( this,
                                i18n("File name is not a KWord file!"),
                                i18n("Insert File"));
            delete store;
            return;
        }
        QDomDocument doc;
        doc.setContent( store->device() );
        QDomElement word = doc.documentElement();

        QDomElement framesets = word.namedItem( "FRAMESETS" ).toElement();
        if ( !framesets.isNull() )
        {
            QDomElement framesetElem = framesets.firstChild().toElement();
            // just in case
            while ( !framesetElem.isNull() && framesetElem.tagName() != "FRAMESET" )
                framesetElem = framesetElem.nextSibling().toElement();

            if ( !framesetElem.isNull() )
            {
                KWTextFrameSet *textFrameSet = dynamic_cast<KWTextFrameSet *>( m_doc->frameSet(0) );
                KoTextCursor insertionCursor( textFrameSet->textDocument() );
                KWTextFrameSetEdit* edit = currentTextEdit();
                if ( edit && !edit->textFrameSet()->protectContent()) {
                    textFrameSet = edit->textFrameSet();
                    insertionCursor = *edit->cursor();
                }
                // Handle the main textframeset special - concatenate the text
                QDomDocument domDoc( "PARAGRAPHS" );
                QDomElement paragsElem = domDoc.createElement( "PARAGRAPHS" );
                domDoc.appendChild( paragsElem );

                // Need an intermediate list otherwise nextSibling doesn't work after moving the node
                // to the other DOM tree ;)
                QValueList<QDomElement> paragList;
                QValueList<QString> inlineFsNames;
                QDomElement fsElem;

                QDomNode n = framesetElem.firstChild().toElement();
                while( !n.isNull() )
                {
                    QDomElement e = n.toElement(); // try to convert the node to an element.
                    if  ( !e.isNull() && e.tagName() == "PARAGRAPH" )
                    {
                        paragList.append( e );
                        // Handle inline framesets
                        QDomElement formatsElem = e.namedItem( "FORMATS" ).toElement();
                        if ( !formatsElem.isNull() )
                        {
                            // Get references to inline framesets
                            QDomElement formatElem = formatsElem.firstChild().toElement();
                            for ( ; !formatElem.isNull() ; formatElem = formatElem.nextSibling().toElement() )
                            {
                                QDomElement anchorElem = formatElem.namedItem( "ANCHOR" ).toElement();
                                if ( !anchorElem.isNull() )
                                {
                                    QString type = anchorElem.attribute( "type" );
                                    if ( type == "grpMgr" /* old syntax */ || type == "frameset" )
                                    {
                                        QString iName = anchorElem.attribute( "instance" );
                                        inlineFsNames.append( iName );
                                        // inline framsets shall appear after the paragraph
                                        QString tableName;
                                        QDomElement table;
                                        fsElem = framesetElem.nextSibling().toElement();
                                        for ( ; !fsElem.isNull() ; fsElem = fsElem.nextSibling().toElement() )
                                        {
                                            if ( fsElem.tagName() == "FRAMESET" )
                                            {
                                                QString name = fsElem.attribute( "name" );
                                                QString grpMgr = fsElem.attribute( "grpMgr" );
                                                if ( name == iName )
                                                {
                                                    paragList.append( fsElem );
                                                    //kdDebug()<<k_funcinfo<<" Inline frameset: "<<name<<" added"<<endl;
                                                }
                                                else if ( grpMgr == iName )
                                                {   // Table so we need to create table framset if it is new
                                                    if ( grpMgr != tableName )
                                                    {
                                                        tableName = grpMgr;
                                                        table = domDoc.createElement("FRAMESET");
                                                        table.setAttribute("frameType", FT_TABLE);
                                                        table.setAttribute("frameInfo", 0);
                                                        table.setAttribute("protectSize", fsElem.attribute("protectSize","0"));
                                                        table.setAttribute("name", tableName);
                                                        table.setAttribute("visible", fsElem.attribute("visible","1"));
                                                        paragList.append( table ); // same level as paragraphs, so it goes into the paragList
                                                        //kdDebug()<<k_funcinfo<<" paragList Added new table: "<<grpMgr<<endl;
                                                    }

                                                    table.appendChild( fsElem.cloneNode() ); // add the cell as child to the table frameset
                                                    //kdDebug()<<k_funcinfo<<" Inline table: "<<grpMgr<<" Added new cell: "<<name<<endl;
                                                }
                                                //else kdDebug()<<k_funcinfo<<" Fixed frameset: "<<name<<endl;
                                            }
                                            //else kdDebug()<<k_funcinfo<<" Not frameset: "<<fsElem.tagName()<<endl;
                                        }
                                        //kdDebug()<<k_funcinfo<<" Treated "<<i<<" frameset elements"<<endl;
                                    }
                                }
                            }
                        }
                    }
                    n = n.nextSibling();
                }

                QValueList<QDomElement>::Iterator it = paragList.begin();
                QValueList<QDomElement>::Iterator end = paragList.end();
                for ( ; it!= end ; ++it )
                {
                    //kdDebug()<<k_funcinfo<<" paragList tag: "<<(*it).tagName()<<" name: "<<(*it).attribute( "name" )<<" grpMgr: "<<(*it).attribute( "grpMgr" )<<endl;
                    paragsElem.appendChild( *it );
                }
                //kdDebug() << k_funcinfo << "Paragraphs:\n" << domDoc.toCString() << endl;

                // The fixed framesets
                // doctype SELECTION is used for fixed framesets
                QDomDocument domDocFrames( "SELECTION" ); // see KWCanvas::copySelectedFrames
                QDomElement topElem = domDocFrames.createElement( "SELECTION" );
                domDocFrames.appendChild( topElem );
                QString tableName;
                QDomElement table;
                QValueList<QString> fsInHeader;
                QValueList<QDomElement> framesetsList;

                framesetElem = framesetElem.nextSibling().toElement();
                for ( ; !framesetElem.isNull() ; framesetElem = framesetElem.nextSibling().toElement() )
                {
                    if ( framesetElem.tagName() == "FRAMESET" )
                    {
                        FrameSetType frameSetType = static_cast<FrameSetType>( KWDocument::getAttribute( framesetElem, "frameType", FT_BASE ) );
                        KWFrameSet::Info info = static_cast<KWFrameSet::Info>( framesetElem.attribute("frameInfo").toInt() );
                        if ( frameSetType == FT_TEXT &&
                             (info == KWFrameSet::FI_FIRST_HEADER ||
                              info == KWFrameSet::FI_EVEN_HEADER   ||
                              info == KWFrameSet::FI_ODD_HEADER  ||
                              info == KWFrameSet::FI_FIRST_FOOTER ||
                              info == KWFrameSet::FI_EVEN_FOOTER   ||
                              info == KWFrameSet::FI_ODD_FOOTER)
                            )
                            fsInHeader += getInlineFramesets( framesetElem );

                        QString name = framesetElem.attribute( "name" );
                        QString grpMgr = framesetElem.attribute( "grpMgr" );
                        if ( !inlineFsNames.contains(name) && !inlineFsNames.contains(grpMgr) )
                        {   // fixed frameset
                            if ( !grpMgr.isEmpty() )
                            {   // Table cell
                                if ( grpMgr != tableName )
                                {   // New table (first cell)
                                    tableName = grpMgr;
                                    table = domDocFrames.createElement("FRAMESET");
                                    table.setAttribute("frameType", FT_TABLE);
                                    table.setAttribute("frameInfo", 0);
                                    table.setAttribute("protectSize", framesetElem.attribute("protectSize","0"));
                                    table.setAttribute("name", tableName);
                                    table.setAttribute("visible", framesetElem.attribute("visible","1"));
                                    framesetsList.append( table );
                                    //kdDebug()<<k_funcinfo<<" framesetsList Added new table: "<<grpMgr<<endl;
                                }
                                table.appendChild( framesetElem.cloneNode() ); // add the cell as child to the table frameset
                                //kdDebug()<<k_funcinfo<<" Fixed table '"<<grpMgr<<"': Added new cell: '"<<name<<"'"<<endl;
                            }
                            else // other frameset type
                            {
                                framesetsList.append( framesetElem );
                                //kdDebug()<<k_funcinfo<<" Fixed frameset: '"<<name<<"' added"<<endl;
                            }
                        }
                        //else kdDebug()<<k_funcinfo<<" Inline frameset, skipped: "<<name<<endl;
                    }
                    //else kdDebug()<<k_funcinfo<<" Not frameset element, skipped: "<<framesetElem.tagName()<<endl;
                }
                it = framesetsList.begin();
                end = framesetsList.end();
                for ( ; it != end ; ++it )
                {
                    framesetElem = (*it);
                    FrameSetType frameSetType = static_cast<FrameSetType>( KWDocument::getAttribute( framesetElem, "frameType", FT_BASE ) );
                    KWFrameSet::Info info = static_cast<KWFrameSet::Info>( framesetElem.attribute("frameInfo").toInt() );
                    QString name = framesetElem.attribute("name");
                    QString grpMgr = framesetElem.attribute( "grpMgr" );
                    // We skip headers, footers and framsets/tables inside theese
                    if ( !fsInHeader.contains(name) && !fsInHeader.contains(grpMgr) &&
                         !( frameSetType == FT_TEXT &&
                            (info == KWFrameSet::FI_FIRST_HEADER ||
                             info == KWFrameSet::FI_EVEN_HEADER   ||
                             info == KWFrameSet::FI_ODD_HEADER  ||
                             info == KWFrameSet::FI_FIRST_FOOTER ||
                             info == KWFrameSet::FI_EVEN_FOOTER   ||
                             info == KWFrameSet::FI_ODD_FOOTER) )
                        )
                    {
                        hasFixedFramesets = true;
                        topElem.appendChild( framesetElem );
                    }
                }
                // Embedded documents
                QDomDocument embeddedDoc( "SELECTION" );
                QDomElement selElem = embeddedDoc.createElement( "SELECTION" );
                embeddedDoc.appendChild( selElem );

                QValueList<QDomElement> embeddedList;
                QDomElement embeddedElem = word.namedItem( "EMBEDDED" ).toElement();
                for ( ; !embeddedElem.isNull() ; embeddedElem = embeddedElem.nextSibling().toElement() )
                {
                    if ( embeddedElem.tagName() == "EMBEDDED" )
                    {
                        embeddedList.append( embeddedElem );
                    }
                }
                it = embeddedList.begin();
                end = embeddedList.end();
                bool hasEmbedded = false;
                for ( ; it!= end ; ++it )
                {
                    selElem.appendChild( *it );
                    hasEmbedded = true;
                }
                store->close();


                // Now we do the insertion
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Insert File") );

                if ( hasFixedFramesets )
                {
                    // insert fixed framesets
                    //kdDebug() << k_funcinfo << domDocFrames.toCString() << endl;
                    m_doc->pasteFrames( topElem, macroCmd, false, false, false /* don't select frames */ );
                }
                if ( hasEmbedded )
                {
                    //kdDebug()<<k_funcinfo<<" Embedded: \n"<<embeddedDoc.toCString()<<endl;
                    if ( !macroCmd )
                        macroCmd = new KMacroCommand( i18n("Insert File") );
                    m_doc->insertEmbedded( store, selElem, macroCmd, 0 );
                }

                // insert paragraphs and inline framesets (we always have at least one paragraph)
                KCommand *cmd = textFrameSet->pasteOasis( &insertionCursor, domDoc.toCString(), true );

                if ( cmd ) {
                    macroCmd->addCommand( cmd );
                }
                m_doc->addCommand( macroCmd );

            }
        }

        m_doc->loadImagesFromStore( store );
        m_doc->completePasting();

    }
    delete store;
}

QValueList<QString> KWView::getInlineFramesets( const QDomNode &framesetElem)
{
    //kdDebug()<<k_funcinfo<<" Frameset: "<<framesetElem.toElement().attribute("name")<<endl;
    QValueList<QString> list;
    QDomNode n = framesetElem.firstChild().toElement();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if  ( !e.isNull() && e.tagName() == "PARAGRAPH" )
        {
            QDomElement formatsElem = e.namedItem( "FORMATS" ).toElement();
            if ( !formatsElem.isNull() )
            {
                // Get references to inline framesets
                QDomElement formatElem = formatsElem.firstChild().toElement();
                for ( ; !formatElem.isNull() ; formatElem = formatElem.nextSibling().toElement() )
                {
                    QDomElement anchorElem = formatElem.namedItem( "ANCHOR" ).toElement();
                    if ( !anchorElem.isNull() )
                    {
                        QString type = anchorElem.attribute( "type" );
                        if ( type == "grpMgr" /* old syntax */ || type == "frameset" )
                        {
                            QString iName = anchorElem.attribute( "instance" );
                            list.append( iName );
                            //kdDebug()<<k_funcinfo<<" added: "<<iName<<endl;
                        }
                    }
                }
            }
        }
    }
    return list;
}

void KWView::addBookmark()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        //don't create list against viewmode. We must list all bookmarks.
        KWCreateBookmarkDia dia( m_doc->listOfBookmarkName(0L), this, 0 );
        if ( dia.exec() ) {
            QString bookName = dia.bookmarkName();
            KoTextCursor start, end;
            if ( edit->textDocument()->hasSelection( KoTextDocument::Standard ) ) {
                start = edit->textDocument()->selectionStartCursor( KoTextDocument::Standard );
                end = edit->textDocument()->selectionEndCursor( KoTextDocument::Standard );
            } else {
                start = *edit->cursor();
                end = start;
            }
            int startSel = start.index();
            int endSel = end.index();
            m_doc->insertBookMark(bookName, static_cast<KWTextParag*>(start.parag()),static_cast<KWTextParag*>(end.parag()), edit->textFrameSet(), startSel, endSel);
        }
    }
}

void KWView::selectBookmark()
{
    KWSelectBookmarkDia dia( m_doc->listOfBookmarkName(m_gui->canvasWidget()->viewMode()), m_doc, this, 0 );
    if ( dia.exec() ) {
        QString bookName = dia.bookmarkSelected();
        KWBookMark * book = m_doc->bookMarkByName( bookName );
        Q_ASSERT( book );
        if ( book )
        {
            Q_ASSERT( book->startParag() );
            Q_ASSERT( book->endParag() );
            if ( !book->startParag() || !book->endParag() )
                return;
            m_gui->canvasWidget()->editTextFrameSet( book->frameSet(), book->startParag(), book->bookmarkStartIndex() );
            KWTextFrameSetEdit * edit = currentTextEdit();
            Q_ASSERT( edit );
            if ( edit )
            {
                edit->textDocument()->removeSelection( KoTextDocument::Standard );
                edit->textDocument()->setSelectionStart( KoTextDocument::Standard, edit->cursor() );
                edit->cursor()->setParag( book->endParag());
                edit->cursor()->setIndex(book->bookmarkEndIndex() );
                edit->textDocument()->setSelectionEnd( KoTextDocument::Standard, edit->cursor() );
                book->startParag()->setChanged( true );
                book->endParag()->setChanged( true );
                m_doc->slotRepaintChanged( edit->frameSet() );
            }
        }
    }
}

void KWView::importStyle()
{
    KWImportStyleDia dia( m_doc, m_doc->styleCollection(), this );
    if ( dia.exec() && !dia.importedStyles().isEmpty() ) {
        m_doc->styleCollection()->importStyles( dia.importedStyles() );
        m_doc->setModified( true );
        m_doc->updateAllStyleLists();
    }
}

void KWView::testAndCloseAllFrameSetProtectedContent()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( edit && edit->textFrameSet()->protectContent()) {
        m_doc->terminateEditing( edit->frameSet());
    }
}

void KWView::updateRulerInProtectContentMode()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    KoRuler * hRuler = m_gui ? m_gui->getHorzRuler() : 0;

    if ( edit && hRuler) {
        if ( !edit->textFrameSet()->protectContent() )
            hRuler->changeFlags(KoRuler::F_INDENTS | KoRuler::F_TABS);
        else
            hRuler->changeFlags(0);
        hRuler->repaint();
    }
}

void KWView::deselectAllFrames()
{
    m_gui->canvasWidget()->selectAllFrames( false );
}

void KWView::insertDirectCursor()
{
#if 0
    insertDirectCursor( actionInsertDirectCursor->isChecked());
#endif
}

void KWView::insertDirectCursor(bool b)
{
    m_doc->setInsertDirectCursor(b);
}

void KWView::updateDirectCursorButton()
{
#if 0
    actionInsertDirectCursor->setChecked(m_doc->insertDirectCursor());
#endif
}

void KWView::convertTableToText()
{
    KWCanvas * canvas = m_gui->canvasWidget();
    KWTableFrameSet *table = canvas->getCurrentTable();
    if (table && table->isFloating())
    {
        table->convertTableToText();
        KWAnchor * anchor = table->findAnchor( 0 );
        if ( anchor )
        {
            KWTextFrameSet *frameset= table->anchorFrameset();
            KoTextParag *parag = anchor->paragraph();
            int pos = anchor->index();
            KMacroCommand *macro = new KMacroCommand(i18n("Convert Table to Text"));
            KCommand *cmd =table->anchorFrameset()->deleteAnchoredFrame( anchor );
            if ( cmd )
                macro->addCommand( cmd);

            m_gui->canvasWidget()->emitFrameSelectedChanged();
            deleteFrame( false );
            m_gui->canvasWidget()->editTextFrameSet( frameset, parag, pos );
            QMimeSource *data = QApplication::clipboard()->data();
            if ( data->provides( KWTextDrag::selectionMimeType() ) )
            {
                QByteArray arr = data->encodedData( KWTextDrag::selectionMimeType() );
                if ( arr.size() )
                {
                    KWTextFrameSetEdit* edit = currentTextEdit();
                    if ( edit && edit->textFrameSet())
                    {
                        KCommand *cmd =edit->textFrameSet()->pasteOasis( edit->cursor(), QCString( arr , arr.count()), true );
                        if ( cmd )
                            macro->addCommand( cmd);
                        m_doc->addCommand(cmd);
                    }
                }
            }
            m_doc->addCommand(macro);
            QApplication::clipboard()->clear();
        }
    }
}

void KWView::convertToTextBox()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if( edit && !edit->textFrameSet()->protectContent() && edit->textFrameSet()->textObject()->hasSelection())
    {
        edit->copy();
        KMacroCommand *macro = 0L;

        KCommand *cmd = edit->textFrameSet()->textObject()->removeSelectedTextCommand( edit->textView()->cursor(), KoTextDocument::Standard );
        if ( cmd )
        {
            if ( ! macro )
                macro = new KMacroCommand( i18n("Convert to Text Box"));
            macro->addCommand( cmd );
        }
        cmd = m_gui->canvasWidget()->createTextBox(KoRect(30,30,30,30) );
        if ( cmd )
        {
            if ( ! macro )
                macro = new KMacroCommand( i18n("Convert to Text Box"));
            macro->addCommand( cmd );
        }

        edit = currentTextEdit();
        if ( edit )
        {
            QMimeSource *data = QApplication::clipboard()->data();
            if ( data->provides( KWTextDrag::selectionMimeType() ) )
            {
                QByteArray arr = data->encodedData( KWTextDrag::selectionMimeType() );
                if ( arr.size() )
                {
                    cmd =edit->textFrameSet()->pasteOasis( edit->textView()->cursor(), QCString( arr, arr.size()+1 ), true );
                    if ( cmd )
                    {
                        if ( ! macro )
                            macro = new KMacroCommand( i18n("Convert to Text Box"));
                        macro->addCommand( cmd );
                    }
                }
            }
        }
        if ( macro )
            m_doc->addCommand(macro);
    }
}

void KWView::slotAddIgnoreAllWord()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( edit )
        m_doc->addSpellCheckIgnoreWord( edit->currentWordOrSelection() );
}

void KWView::sortText()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( edit && edit->textFrameSet()->hasSelection())
    {
        KWSortDia * dlg = new KWSortDia( this, "sort dia" );
        if ( dlg->exec())
        {
            if ( edit->textFrameSet()->sortText(dlg->getSortType()) )
            {
                QMimeSource *data = QApplication::clipboard()->data();
                if ( data->provides( KWTextDrag::selectionMimeType() ) )
                {
                    QByteArray arr = data->encodedData( KWTextDrag::selectionMimeType() );
                    if ( arr.size() )
                    {
                        KCommand *cmd =edit->textFrameSet()->pasteOasis( edit->cursor(), QCString( arr , arr.size()+1), true );
                        if ( cmd )
                            m_doc->addCommand(cmd);
                    }
                }
                QApplication::clipboard()->clear();
            }
        }
        delete dlg;
    }
}

void KWView::addPersonalExpression()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( !(edit && edit->textFrameSet()->hasSelection()))
        return;

    QString newExpression = edit->textFrameSet()->textObject()->selectedText();
    //load file !!!
    QString tmp=locateLocal("data","kword/expression/perso.xml");
    QFile file( tmp );
    if ( !file.open( IO_ReadOnly ) )
	return;
    QDomDocument doc;
    doc.setContent( &file );
    file.close();

    QString group;
    QMap<QString, QStringList>lstOfPersonalExp;
    QStringList list;
    QDomNode n = doc.documentElement().firstChild();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
        if ( n.isElement() )
        {
            QDomElement e = n.toElement();
            if ( e.tagName() == "Type" )
            {
                list.clear();
                group = i18n( e.namedItem( "TypeName" ).toElement().text().utf8() );

                QDomNode n2 = e.firstChild();
                for( ; !n2.isNull(); n2 = n2.nextSibling() )
                {

                    if ( n2.isElement() )
                    {
                        QDomElement e2 = n2.toElement();
                        if ( e2.tagName() == "Expression" )
                        {
                            QString text = i18n( e2.namedItem( "Text" ).toElement().text().utf8() );
                            list<<text;
                        }
                    }
                }
                lstOfPersonalExp.insert(group,list);
                group = "";
            }
        }
    }
    //save
    doc = QDomDocument( "KWordExpression" );
    QDomElement begin = doc.createElement( "KWordExpression" );
    doc.appendChild( begin );
    QMapIterator<QString, QStringList> itPersonalExp = lstOfPersonalExp.find(i18n("Normal"));
    if ( itPersonalExp != lstOfPersonalExp.end())
    {
        list = itPersonalExp.data();
        list<<newExpression;
        lstOfPersonalExp.replace( i18n("Normal"), list);
    }
    else
    {
        list.clear();
        list<<newExpression;
        lstOfPersonalExp.insert( i18n("Normal"), list);
    }


    itPersonalExp = lstOfPersonalExp.begin();
    for ( ; itPersonalExp != lstOfPersonalExp.end(); ++itPersonalExp )
    {
        QDomElement type = doc.createElement( "Type" );
        begin.appendChild( type );
        QDomElement typeName = doc.createElement( "TypeName" );
        type.appendChild( typeName );
        typeName.appendChild( doc.createTextNode(itPersonalExp.key()  ) );
        list=itPersonalExp.data();
        for( uint i=0;i<list.count();i++ )
        {
            QDomElement expr = doc.createElement( "Expression" );
            type.appendChild( expr );
            QDomElement text = doc.createElement( "Text" );
            expr.appendChild( text );
            text.appendChild( doc.createTextNode(list[i] ) );
        }
    }
    QCString s = doc.toCString();

    if ( !file.open( IO_WriteOnly ) )
    {
        kdDebug()<<"Error in addPersonalExpression()\n";
	return;
    }
    file.writeBlock(s,s.length());
    file.close();
    m_doc->refreshMenuExpression();
}

#if 0 // KWORD_HORIZONTAL_LINE
void KWView::insertHorizontalLine()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( edit && edit->textFrameSet() && !edit->textFrameSet()->textObject()->protectContent() )
    {
        KWinsertHorizontalLineDia *dia = new KWinsertHorizontalLineDia( m_doc, this);
        if ( dia->exec() )
        {
            KWHorzLineFrameSet *horizontalLine = new KWHorzLineFrameSet( m_doc, QString::null /*automatic name*/ );

            KWFrame *frame = new KWFrame(horizontalLine, 50, 50, edit->textFrameSet()->frame(0)->width(), 10 );
            horizontalLine->addFrame( frame );
            frame->setZOrder( m_doc->maxZOrder( frame->pageNum(m_doc) ) + 1 ); // make sure it's on top
            m_doc->addFrameSet( horizontalLine, false );
            horizontalLine->loadPicture( dia->horizontalLineName() );
            edit->insertFloatingFrameSet( horizontalLine, i18n("Insert Horizontal Line") );

            m_doc->updateAllFrames();
        }
        delete dia;
    }
}

void KWView::changeHorizontalLine()
{

    KWFrame * frame = m_doc->getFirstSelectedFrame();
    KWHorzLineFrameSet *frameset = static_cast<KWHorzLineFrameSet *>(frame->frameSet());
    KoPictureKey oldKey ( frameset->picture().getKey() );
    QString oldFile ( oldKey.filename() );
    KWinsertHorizontalLineDia *dia = new KWinsertHorizontalLineDia( m_doc, this);
    if ( dia->exec() )
    {
        QString file( dia->horizontalLineName() );
        KoPictureKey key;
        key.setKeyFromFile( file );
        KoPicture picture;
        picture.setKey( key );
        picture.loadFromFile( file );
        KWFrameChangePictureCommand *cmd= new KWFrameChangePictureCommand( i18n("Change HorizontalLine"), FrameIndex(frame), oldFile, file) ;

        frameset->insertPicture( picture );
        m_doc->frameChanged( frame );
        m_doc->addCommand(cmd);
    }
    delete dia;
}
#endif

void KWView::addWordToDictionary()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( edit && m_doc->backgroundSpellCheckEnabled() )
    {
        QString word = edit->wordUnderCursor( *edit->cursor() );
        if ( !word.isEmpty()) {
            m_doc->addWordToDictionary( word);
            // Re-check everything to make this word normal again
            m_doc->reactivateBgSpellChecking();
        }
    }
}

void KWView::embeddedStoreInternal()
{
    kdDebug()<<k_funcinfo<<endl;
    KWFrame * frame = m_doc->getFirstSelectedFrame();
    KWPartFrameSet *part = static_cast<KWPartFrameSet *>(frame->frameSet());
    part->storeInternal();
}

void KWView::deleteFrameSet( KWFrameSet * frameset)
{
    if ( frameset  && frameset->frame(0))
    {
        frameset->frame(0)->setSelected( true );
        deleteFrame();
    }
}

QPtrList<KAction> KWView::listOfResultOfCheckWord( const QString &word )
{
    QPtrList<KAction> listAction;
#ifdef HAVE_LIBKSPELL2
//not perfect, improve API!!!!
    DefaultDictionary *dict = m_broker->defaultDictionary();
    QStringList lst = dict->suggest( word );
    if ( !lst.contains( word ) )
    {
        QStringList::ConstIterator it = lst.begin();
        for ( ; it != lst.end() ; ++it )
        {
            if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
            {
                KAction * act = new KAction( *it );
                connect( act, SIGNAL(activated()), this, SLOT(slotCorrectWord()) );
                listAction.append( act );
            }
        }
    }
#endif
    return listAction;
}

void KWView::slotCorrectWord()
{
    KAction * act = (KAction *)(sender());
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( edit )
    {
        edit->selectWordUnderCursor( *(edit->cursor()) );
        m_doc->addCommand( edit->textObject()->replaceSelectionCommand( edit->cursor(), act->text(), KoTextDocument::Standard, i18n("Replace Word") ));
    }
}

void KWView::slotChildActivated( bool a )
{
  // Same hack as in KoView
  KoViewChild* ch = child( (KoView*)sender() );
  if ( !ch )
    return;
  KWChild* kwchild = static_cast<KWChild *>( ch->documentChild() );
  KWPartFrameSet* fs = kwchild->partFrameSet();
  //kdDebug() << "KWView::slotChildActivated fs=" << fs << endl;
  Q_ASSERT( fs );
  if ( fs ) {
      if ( a )
          fs->startEditing();
      else
          fs->endEditing();
  }
  KoView::slotChildActivated( a );
}

#ifdef HAVE_LIBKSPELL2
Broker *KWView::broker() const
{
    return m_broker;
}
#endif

/******************************************************************/
/* Class: KWLayoutWidget                                          */
/******************************************************************/

KWLayoutWidget::KWLayoutWidget( QWidget *parent, KWGUI *g )
    : QWidget( parent )
{
    gui = g;
}

void KWLayoutWidget::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    gui->reorganize();
}

/******************************************************************/
/* Class: KWGUI                                                */
/******************************************************************/
KWGUI::KWGUI( KWViewMode* viewMode, QWidget *parent, KWView *_view )
    : QWidget( parent, "" )
{
    view = _view;

    r_horz = r_vert = 0;
    KWDocument * doc = view->kWordDocument();

    panner = new QSplitter( Qt::Horizontal, this );
    docStruct = new KWDocStruct( panner, doc, this );
    docStruct->setMinimumWidth( 0 );
    left = new KWLayoutWidget( panner, this );
    left->show();
    canvas = new KWCanvas( viewMode, left, doc, this );

    QValueList<int> l;
    l << 10;
    l << 90;
    panner->setSizes( l );

    KoPageLayout layout = doc->pageLayout();

    tabChooser = new KoTabChooser( left, KoTabChooser::TAB_ALL );
    tabChooser->setReadWrite(doc->isReadWrite());

    r_horz = new KoRuler( left, canvas->viewport(), Qt::Horizontal, layout,
                          KoRuler::F_INDENTS | KoRuler::F_TABS, doc->unit(), tabChooser );
    r_horz->setReadWrite(doc->isReadWrite());
    r_vert = new KoRuler( left, canvas->viewport(), Qt::Vertical, layout, 0, doc->unit() );
    connect( r_horz, SIGNAL( newPageLayout( const KoPageLayout & ) ), view, SLOT( newPageLayout( const KoPageLayout & ) ) );
    r_vert->setReadWrite(doc->isReadWrite());

    r_horz->setZoom( doc->zoomedResolutionX() );
    r_vert->setZoom( doc->zoomedResolutionY() );

    r_horz->setGridSize(doc->gridX());

    connect( r_horz, SIGNAL( newLeftIndent( double ) ), view, SLOT( newLeftIndent( double ) ) );
    connect( r_horz, SIGNAL( newFirstIndent( double ) ), view, SLOT( newFirstIndent( double ) ) );
    connect( r_horz, SIGNAL( newRightIndent( double ) ), view, SLOT( newRightIndent( double ) ) );

    connect( r_horz, SIGNAL( doubleClicked() ), view, SLOT( slotHRulerDoubleClicked() ) );
    connect( r_horz, SIGNAL( doubleClicked(double) ), view, SLOT( slotHRulerDoubleClicked(double) ) );
    connect( r_horz, SIGNAL( unitChanged( KoUnit::Unit ) ), this, SLOT( unitChanged( KoUnit::Unit ) ) );
    connect( r_vert, SIGNAL( newPageLayout( const KoPageLayout & ) ), view, SLOT( newPageLayout( const KoPageLayout & ) ) );
    connect( r_vert, SIGNAL( doubleClicked() ), view, SLOT( formatPage() ) );
    connect( r_vert, SIGNAL( unitChanged( KoUnit::Unit ) ), this, SLOT( unitChanged( KoUnit::Unit ) ) );

    r_horz->hide();
    r_vert->hide();

    canvas->show();

    reorganize();

    connect( r_horz, SIGNAL( tabListChanged( const KoTabulatorList & ) ), view,
             SLOT( tabListChanged( const KoTabulatorList & ) ) );

    setKeyCompression( TRUE );
    setAcceptDrops( TRUE );
    setFocusPolicy( QWidget::NoFocus );
}

void KWGUI::showGUI()
{
    reorganize();
}

void KWGUI::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    reorganize();
}

void KWGUI::reorganize()
{
    int space=20;
    if(view->kWordDocument()->showRuler())
    {
        r_vert->show();
        r_horz->show();
        tabChooser->show();
        tabChooser->setGeometry( 0, 0, 20, 20 );
    }
    else
    {
        r_vert->hide();
        r_horz->hide();
        tabChooser->hide();
        space=0;
    }

    if(view->kWordDocument()->showdocStruct()) {
        if(docStruct->isHidden()) {
            docStruct->show();
            if(panner->sizes()[0] < 50) {
                QValueList<int> l;
                l << 100;
                l << width()-100;
                panner->setSizes( l );
            }
        }
    } else
        docStruct->hide();

    if( view->statusBar())
    {
        if(view->kWordDocument()->showStatusBar())
            view->statusBar()->show();
        else
            view->statusBar()->hide();
    }

    if ( view->kWordDocument()->showScrollBar())
    {
        canvas->setVScrollBarMode(QScrollView::Auto);
        canvas->setHScrollBarMode(QScrollView::Auto);
    }
    else
    {
        canvas->setVScrollBarMode(QScrollView::AlwaysOff);
        canvas->setHScrollBarMode(QScrollView::AlwaysOff);
    }

    panner->setGeometry( 0, 0, width(), height() );
    canvas->setGeometry( space, space, left->width() - space, left->height() - space );
    r_horz->setGeometry( space, 0, left->width() - space, space );
    r_vert->setGeometry( 0, space, space, left->height() - space );
}

void KWGUI::unitChanged( KoUnit::Unit u )
{
    view->kWordDocument()->setUnit( u );
}

// Implementation of KWStatisticsDialog
KWStatisticsDialog::KWStatisticsDialog( QWidget *_parent, KWDocument *_doc )
    : KDialogBase(_parent, "statistics", true, i18n("Statistics"),KDialogBase::Ok, KDialogBase::Ok, false )
{
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );

    QTabWidget *tab = new QTabWidget( page );
    QFrame *pageAll = 0;
    QFrame *pageGeneral = 0;
    QFrame *pageSelected = 0;
    for (int i=0; i < 7; ++i) {
        resultLabelAll[i] = 0;
        resultLabelSelected[i] = 0;
        if ( i < 6 )
            resultGeneralLabel[i]=0;
    }
    m_doc = _doc;
    m_parent = _parent;
    m_canceled = true;


    // add Tab "General"
    pageGeneral = new QFrame( this );
    tab->addTab( pageGeneral,  i18n( "General" ) );

    addBoxGeneral( pageGeneral, resultGeneralLabel );
    calcGeneral( resultGeneralLabel );

    // add Tab "All"
    pageAll = new QFrame( this );
    tab->addTab( pageAll,  i18n( "Text" ) );

    addBox( pageAll, resultLabelAll, true );

    m_canceled = true;
    pageSelected = new QFrame( this );
    tab->addTab( pageSelected,  i18n( "Selected Text" ) );
    // let's see if there's selected text
    bool b = docHasSelection();
    tab->setTabEnabled(pageSelected, b);
    if ( b ) {
        addBox( pageSelected, resultLabelSelected,  false);
        // assign results to 'selected' tab.
        if ( !calcStats( resultLabelSelected, true,true ) )
            return;
        if ( !calcStats( resultLabelAll, false,false ) )
            return;
        showPage( 2 );
    } else {
        // assign results
        if ( !calcStats( resultLabelAll, false, false ) )
            return;
        showPage( 1 );
    }
    topLayout->addWidget( tab );
    m_canceled = false;

}

void KWStatisticsDialog::slotRefreshValue(bool state)
{
    m_canceled = true;
    // let's see if there's selected text
    bool b = docHasSelection();
    if ( b )
    {
        if ( !calcStats( resultLabelSelected, true, true ) )
            return;
        if ( !calcStats( resultLabelAll, false, state ) )
            return;
    }
    else
    {
        // assign results
        if ( !calcStats( resultLabelAll, false, state ) )
            return;
    }
    m_canceled = false;
}


void KWStatisticsDialog::calcGeneral( QLabel **resultLabel )
{
    KLocale *locale = KGlobal::locale();

    resultLabel[0]->setText( locale->formatNumber( m_doc->numPages(), 0) );
    int table =0;
    int picture = 0;
    int part = 0;
    int nbFrameset = 0;
    int nbFormula = 0;
    QPtrListIterator<KWFrameSet> framesetIt( m_doc->framesetsIterator() );
    for ( framesetIt.toFirst(); framesetIt.current(); ++framesetIt ) {
        KWFrameSet *frameSet = framesetIt.current();
        if ( frameSet && frameSet->isVisible())
        {
            if ( frameSet->type() == FT_TABLE)
                table++;
            else if ( frameSet->type() == FT_PICTURE)
                picture++;
            else if ( frameSet->type() == FT_PART )
                part++;
            else if ( frameSet->type() == FT_FORMULA )
                nbFormula++;
            nbFrameset++;
        }
    }

    resultLabel[1]->setText( locale->formatNumber( nbFrameset, 0 ) );
    resultLabel[2]->setText( locale->formatNumber( picture, 0 ) );
    resultLabel[3]->setText( locale->formatNumber( table, 0 ) );
    resultLabel[4]->setText( locale->formatNumber( part, 0 ) );
    resultLabel[5]->setText( locale->formatNumber( nbFormula, 0 ) );
}

bool KWStatisticsDialog::calcStats( QLabel **resultLabel, bool selection, bool useFootEndNote  )
{
    ulong charsWithSpace = 0L;
    ulong charsWithoutSpace = 0L;
    ulong words = 0L;
    ulong sentences = 0L;
    ulong lines = 0L;
    ulong syllables = 0L;

    // safety check result labels
    for (int i=0; i < 7; ++i) {
        if ( !resultLabel[i] ) {
            kdDebug() << "Warning: KWStatisticsDiaolog::calcStats result table not initialized." << endl;
            return false;
        }
    }

    // count paragraphs for progress dialog:
    ulong paragraphs = 0L;
    QPtrListIterator<KWFrameSet> framesetIt( m_doc->framesetsIterator() );
    for ( framesetIt.toFirst(); framesetIt.current(); ++framesetIt ) {
        KWFrameSet *frameSet = framesetIt.current();
        if ( (frameSet->frameSetInfo() == KWFrameSet::FI_FOOTNOTE || frameSet->frameSetInfo() == KWFrameSet::FI_BODY) && frameSet->isVisible() )
        {
            if ( (useFootEndNote && frameSet->frameSetInfo() == KWFrameSet::FI_FOOTNOTE) || frameSet->frameSetInfo() == KWFrameSet::FI_BODY )
            {
                if ( selection && false )
                    paragraphs += frameSet->paragraphsSelected();
                else
                    paragraphs += frameSet->paragraphs();
            }

        }
    }
    QProgressDialog progress( i18n( "Counting..." ), i18n( "Cancel" ), paragraphs, this, "count", true );
    progress.setMinimumDuration( 1000 );
    progress.setProgress( 0 );

    // do the actual counting
    for ( framesetIt.toFirst(); framesetIt.current(); ++framesetIt ) {
        KWFrameSet *frameSet = framesetIt.current();
        // Exclude headers and footers
        if ( (frameSet->frameSetInfo() == KWFrameSet::FI_FOOTNOTE || frameSet->frameSetInfo() == KWFrameSet::FI_BODY) && frameSet->isVisible() ) {
            if ( (useFootEndNote && frameSet->frameSetInfo() == KWFrameSet::FI_FOOTNOTE) || frameSet->frameSetInfo() == KWFrameSet::FI_BODY )
            {

                if( ! frameSet->statistics( &progress, charsWithSpace, charsWithoutSpace,
                                            words, sentences, syllables, lines, selection ) ) {
                    // someone pressed "Cancel"
                    return false;
                }
            }
        }
    }

    // assign results
    KLocale *locale = KGlobal::locale();
    resultLabel[0]->setText( locale->formatNumber( charsWithSpace, 0) );
    resultLabel[1]->setText( locale->formatNumber( charsWithoutSpace, 0 ) );
    resultLabel[2]->setText( locale->formatNumber( syllables, 0 ) );
    resultLabel[3]->setText( locale->formatNumber( words, 0 ) );
    resultLabel[4]->setText( locale->formatNumber( sentences, 0 ) );
    resultLabel[5]->setText( locale->formatNumber( lines, 0 ) );
    // add flesch
    double f = calcFlesch( sentences, words, syllables );
    QString flesch = locale->formatNumber( f , 1 );
    if( words < 200 ) {
        // a kind of warning if too few words:
        flesch = i18n("approximately %1").arg( flesch );
    }
    resultLabel[6]->setText( flesch );
    return true;
}

double KWStatisticsDialog::calcFlesch( ulong sentences, ulong words, ulong syllables )
{
    // calculate Flesch reading ease score:
    float flesch_score = 0;
    if( words > 0 && sentences > 0 )
        flesch_score = 206.835 - (1.015 * (words / sentences)) - (84.6 * syllables / words);
    return flesch_score;
}

void KWStatisticsDialog::addBoxGeneral( QFrame *page, QLabel **resultLabel )
{
    // Layout Managers
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, 7 );
    QGroupBox *box = new QGroupBox( i18n( "Statistics" ), page );
    QGridLayout *grid = new QGridLayout( box, 9, 3, KDialog::marginHint(), KDialog::spacingHint() );
    grid->setRowStretch (9, 1);
    // margins
    int fHeight = box->fontMetrics().height();
    grid->setMargin( fHeight );
    grid->addColSpacing( 1, fHeight );
    grid->addRowSpacing( 0, fHeight );

    // insert labels
    QLabel *label1 = new QLabel( i18n( "Number of pages:" ), box );
    grid->addWidget( label1, 1, 0, 1 );
    resultLabel[0] = new QLabel( "", box );
    grid->addWidget( resultLabel[0], 1, 2, 2 );

    QLabel *label2 = new QLabel( i18n( "Number of frames:" ), box );
    grid->addWidget( label2, 2, 0, 1 );
    resultLabel[1] = new QLabel( "", box );
    grid->addWidget( resultLabel[1], 2, 2, 2 );

    QLabel *label3 = new QLabel( i18n( "Number of pictures:" ), box );
    grid->addWidget( label3, 3, 0, 1 );
    resultLabel[2] = new QLabel( "", box );
    grid->addWidget( resultLabel[2], 3, 2, 2 );


    QLabel *label4 = new QLabel( i18n( "Number of tables:" ), box );
    grid->addWidget( label4, 4, 0, 1 );
    resultLabel[3] = new QLabel( "", box );
    grid->addWidget( resultLabel[3], 4, 2, 2 );

    QLabel *label5 = new QLabel( i18n( "Number of embedded objects:" ), box );
    grid->addWidget( label5, 5, 0, 1 );
    resultLabel[4] = new QLabel( "", box );
    grid->addWidget( resultLabel[4], 5, 2, 2 );

    QLabel *label6 = new QLabel( i18n( "Number of formula frameset:" ), box );
    grid->addWidget( label6, 6, 0, 1 );
    resultLabel[5] = new QLabel( "", box );
    grid->addWidget( resultLabel[5], 6, 2, 2 );

    topLayout->addWidget( box );
}

void KWStatisticsDialog::addBox( QFrame *page, QLabel **resultLabel, bool calcWithFootNoteCheckbox )
{
    // Layout Managers
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, 7 );
    if ( calcWithFootNoteCheckbox )
    {
        QWidget *w = new QWidget(page);
        topLayout->addWidget( w );
        QVBoxLayout *noteLayout = new QVBoxLayout( w, KDialog::marginHint(), 0 );
        QCheckBox *calcWithFootNote = new QCheckBox( i18n("&Include text from foot- and endnotes"), w);
        noteLayout->addWidget( calcWithFootNote );
        connect( calcWithFootNote, SIGNAL(toggled ( bool )), this, SLOT( slotRefreshValue(bool)));
    }


    QGroupBox *box = new QGroupBox( i18n( "Statistics" ), page );
    QGridLayout *grid = new QGridLayout( box, 9, 3, KDialog::marginHint(), KDialog::spacingHint() );
    grid->setRowStretch (9, 1);

    // margins
    int fHeight = box->fontMetrics().height();
    grid->setMargin( fHeight );
    grid->addColSpacing( 1, fHeight );
    grid->addRowSpacing( 0, fHeight );

    //maximum size for result column (don't know how to do this better..)
    QString init = i18n("approximately %1").arg( "00000000" );

    // insert labels
    QLabel *label1 = new QLabel( i18n( "Characters including spaces:" ), box );
    grid->addWidget( label1, 1, 0, 1 );
    resultLabel[0] = new QLabel( "", box );
    grid->addWidget( resultLabel[0], 1, 2, 2 );

    QLabel *label2 = new QLabel( i18n( "Characters without spaces:" ), box );
    grid->addWidget( label2, 2, 0, 1 );
    resultLabel[1] = new QLabel( "", box );
    grid->addWidget( resultLabel[1], 2, 2, 2 );

    QLabel *label3 = new QLabel( i18n( "Syllables:" ), box );
    grid->addWidget( label3, 3, 0, 1 );
    resultLabel[2] = new QLabel( "", box );
    grid->addWidget( resultLabel[2], 3, 2, 2 );

    QLabel *label4 = new QLabel( i18n( "Words:" ), box );
    grid->addWidget( label4, 4, 0, 1 );
    resultLabel[3] = new QLabel( "", box );
    grid->addWidget( resultLabel[3], 4, 2, 2 );

    QLabel *label5 = new QLabel( i18n( "Sentences:" ), box );
    grid->addWidget( label5, 5, 0, 1 );
    resultLabel[4] = new QLabel( "", box );
    grid->addWidget( resultLabel[4], 5, 2, 2 );

    QLabel *label6 = new QLabel( i18n( "Lines:" ), box );
    grid->addWidget( label6, 6, 0, 1 );
    resultLabel[5] = new QLabel( "", box );
    grid->addWidget( resultLabel[5], 6, 2, 2 );


    QLabel *label7 = new QLabel( i18n( "Flesch reading ease:" ), box );
    grid->addWidget( label7, 7, 0, 1 );
    resultLabel[6] = new QLabel( init, box );
    grid->addWidget( resultLabel[6], 7, 2, 2 );

    topLayout->addWidget( box );
}

bool KWStatisticsDialog::docHasSelection()const
{
    QPtrListIterator<KWFrameSet> fsIt( m_doc->framesetsIterator() );

    for ( ; fsIt.current(); ++fsIt ) {
        KWFrameSet *fs = fsIt.current();
        if ( fs->paragraphsSelected() ) {
            return true;
        }
    }
    return false;
}

#include "kwview.moc"
