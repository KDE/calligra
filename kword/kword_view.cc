/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   Boston, MA 02111-1307, USA.
*/

#undef Unsorted
#include <qprinter.h>
#include <qpainter.h>
#include <qstring.h>
#include <qkeycode.h>
#include <qpixmap.h>
#include <qevent.h>
#include <kmessagebox.h>
#include <qclipboard.h>
#include <qdropsite.h>
#include <qscrollview.h>
#include <qsplitter.h>
#include <kaction.h>
#include <qfiledialog.h>
#include <qregexp.h>

#include "kword_view.h"
#include "kword_doc.h"
#include "kword_view.moc"
#include "kword_frame.h"
#include "clipbrd_dnd.h"
#include "defs.h"
#include "kword_page.h"
#include "paragdia.h"
#include "parag.h"
#include "stylist.h"
#include "tabledia.h"
#include "insdia.h"
#include "deldia.h"
#include "docstruct.h"
#include "variable.h"
#include "footnotedia.h"
#include "autoformatdia.h"
#include "font.h"
#include "variabledlgs.h"
#include "serialletter.h"

#include <koMainWindow.h>
#include <koDocument.h>
#include <koRuler.h>
#include <koTabChooser.h>
#include <koPartSelectDia.h>
#include <kformulaedit.h>
#include <koTemplateCreateDia.h>
#include <koFrame.h>

#include <kapp.h>
#include <kfiledialog.h>
#include <kstdaction.h>
#include <klocale.h>
#include <qrect.h>
#undef Bool
#include <kspell.h>
#include <kcolordlg.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kaction.h>
#include <kimageio.h>
#include <kcoloractions.h>
#include <kfontdialog.h>
#include <kstddirs.h>
#include <kmessagebox.h>

#include <stdlib.h>
#define DEBUG

#include "preview.h"

/******************************************************************/
/* Class: KWordView                                               */
/******************************************************************/

/*================================================================*/
KWordView::KWordView( QWidget *_parent, const char *_name, KWordDocument* _doc )
    : KoView( _doc, _parent, _name ), format( _doc )
{
    m_pKWordDoc = 0L;
    m_bUnderConstruction = TRUE;
    m_bShowGUI = TRUE;
    gui = 0;
    kspell = 0;
    flow = KWParagLayout::LEFT;
    paragDia = 0L;
    styleManager = 0L;
    vertAlign = KWFormat::VA_NORMAL;
    left.color = white;
    left.style = KWParagLayout::SOLID;
    left.ptWidth = 0;
    right.color = white;
    right.style = KWParagLayout::SOLID;
    right.ptWidth = 0;
    top.color = white;
    top.style = KWParagLayout::SOLID;
    top.ptWidth = 0;
    bottom.color = white;
    bottom.style = KWParagLayout::SOLID;
    bottom.ptWidth = 0;
    tmpBrd.color = black;
    tmpBrd.style = KWParagLayout::SOLID;
    tmpBrd.ptWidth = 1;
    frmBrd.color = black;
    frmBrd.style = KWParagLayout::SOLID;
    frmBrd.ptWidth = 1;
    _viewFormattingChars = FALSE;
    _viewFrameBorders = TRUE;
    _viewTableGrid = TRUE;
    searchEntry = 0L;
    replaceEntry = 0L;
    searchDia = 0L;
    tableDia = 0L;
    m_pKWordDoc = _doc;
    backColor = QBrush( white );
    spc=0;  // default line spacing (Werner)

    setInstance( KWordFactory::global() );
    setXMLFile( "kword.rc" );

    QObject::connect( m_pKWordDoc, SIGNAL( sig_insertObject( KWordChild*, KWPartFrameSet* ) ),
                      this, SLOT( slotInsertObject( KWordChild*, KWPartFrameSet* ) ) );
    QObject::connect( m_pKWordDoc, SIGNAL( sig_updateChildGeometry( KWordChild* ) ),
                      this, SLOT( slotUpdateChildGeometry( KWordChild* ) ) );


    KFontChooser::getFontList(fontList, false);
    setKeyCompression( TRUE );
    setAcceptDrops( TRUE );
    createKWordGUI();
}

/*================================================================*/
void KWordView::showEvent( QShowEvent *e )
{
    KoView::showEvent( e );
    if ( gui && gui->getPaperWidget() )
        gui->getPaperWidget()->setFocus();
}

/*================================================================*/
KWordView::~KWordView()
{
}

/*=============================================================*/
void KWordView::initGui()
{
    updateStyle( "Standard" );
    setFormat( format, FALSE );
    gui->getPaperWidget()->forceFullUpdate();
    gui->getPaperWidget()->init();

    clipboardDataChanged();

    gui->getPaperWidget()->repaintScreen( TRUE );
    if ( gui ) {
        gui->showGUI( TRUE );
        gui->getPaperWidget()->recalcText();
    }
    ( (KToggleAction*)actionToolsEdit )->blockSignals( TRUE );
    ( (KToggleAction*)actionToolsEdit )->setChecked( TRUE );
    ( (KToggleAction*)actionToolsEdit )->blockSignals( FALSE );
    ( (KToggleAction*)actionViewFrameBorders )->blockSignals( TRUE );
    ( (KToggleAction*)actionViewFrameBorders )->setChecked( TRUE );
    ( (KToggleAction*)actionViewFrameBorders )->blockSignals( FALSE );
    ( (KToggleAction*)actionViewTableGrid )->blockSignals( TRUE );
    ( (KToggleAction*)actionViewTableGrid )->setChecked( TRUE );
    ( (KToggleAction*)actionViewTableGrid )->blockSignals( FALSE );
    setNoteType(m_pKWordDoc->getNoteType(), false);

    ( (KColorAction*)actionFormatColor )->blockSignals( TRUE );
    ( (KColorAction*)actionFormatColor )->setColor( Qt::black );
    ( (KColorAction*)actionFormatColor )->blockSignals( FALSE );
    ( (KSelectColorAction*)actionFormatBrdColor )->blockSignals( TRUE );
    ( (KSelectColorAction*)actionFormatBrdColor )->setColor( Qt::black );
    ( (KSelectColorAction*)actionFormatBrdColor )->blockSignals( FALSE );
    ( (KSelectColorAction*)actionFrameBrdColor )->blockSignals( TRUE );
    ( (KSelectColorAction*)actionFrameBrdColor )->setColor( Qt::black );
    ( (KSelectColorAction*)actionFrameBrdColor )->blockSignals( FALSE );
    ( (KSelectColorAction*)actionFrameBackColor )->blockSignals( TRUE );
    ( (KSelectColorAction*)actionFrameBackColor )->setColor( Qt::white );
    ( (KSelectColorAction*)actionFrameBackColor )->blockSignals( FALSE );

    ( (KSelectAction*)actionViewZoom )->setCurrentItem( 1 );

    showFormulaToolbar( FALSE );

    QWidget *tb = 0;
    if ( factory() )
      tb = factory()->container( "frame_toolbar", this );
    if ( tb )
      tb->hide();

    /* ???
    if ( ( (KoMainWindow*)shell() )->fileToolBar() )
        ( (KoMainWindow*)shell() )->fileToolBar()->setBarPos( KToolBar::Left );
    */
}

/*================================================================*/
void KWordView::setupActions()
{
    // -------------- Edit actions

    actionEditUndo = KStdAction::undo( this, SLOT( editUndo() ), actionCollection(), "edit_undo" );
    actionEditUndo->setText( i18n( "No Undo possible" ) );
    actionEditUndo->setEnabled(false);
    actionEditRedo = KStdAction::redo( this, SLOT( editRedo() ), actionCollection(), "edit_redo" );
    actionEditRedo->setText( i18n( "No Redo possible" ) );
    actionEditRedo->setEnabled(false);
    actionEditCut = KStdAction::cut( this, SLOT( editCut() ), actionCollection(), "edit_cut" );
    actionEditCopy = KStdAction::copy( this, SLOT( editCopy() ), actionCollection(), "edit_copy" );
    actionEditPaste = KStdAction::paste( this, SLOT( editPaste() ), actionCollection(), "edit_paste" );
    actionEditFind = KStdAction::find( this, SLOT( editFind() ), actionCollection(), "edit_find" );
    actionEditFind->setText( i18n( "&Find and Replace..." ) );
    actionEditSelectAll = KStdAction::selectAll( this, SLOT( editSelectAll() ), actionCollection(), "edit_selectall" );
    actionEditDelFrame = new KAction( i18n( "&Delete Frame" ), 0,
                                      this, SLOT( editDeleteFrame() ),
                                      actionCollection(), "edit_delframe" );
    actionEditReconnectFrame = new KAction( i18n( "&Reconnect Frame..." ), 0,
                                            this, SLOT( editReconnectFrame() ),
                                            actionCollection(), "edit_reconnect" );
    actionEditCustomVars = new KAction( i18n( "&Custom Variables..." ), 0,
                                        this, SLOT( editCustomVars() ),
                                        actionCollection(), "edit_customvars" );
    actionEditSlDataBase = new KAction( i18n( "Serial &Letter Database..." ), 0,
                                        this, SLOT( editSerialLetterDataBase() ),
                                        actionCollection(), "edit_sldatabase" );

    // -------------- View actions
// DEBUG info
    actionPrintDebug = new KAction( i18n( "Print debug info" ) , 0,
                                       this, SLOT( printDebug() ),
                                       actionCollection(), "printdebug" );
    actionViewFormattingChars = new KToggleAction( i18n( "&Formatting Characters" ), 0,
                                                   this, SLOT( viewFormattingChars() ),
                                                   actionCollection(), "view_formattingchars" );
    actionViewFrameBorders = new KToggleAction( i18n( "Frame &Borders" ), 0,
                                                   this, SLOT( viewFrameBorders() ),
                                                   actionCollection(), "view_frameborders" );
    actionViewTableGrid = new KToggleAction( i18n( "&Table Grid" ), 0,
                                             this, SLOT( viewTableGrid() ),
                                                   actionCollection(), "view_tablegrid" );
    actionViewHeader = new KToggleAction( i18n( "&Header" ), 0,
                                          this, SLOT( viewHeader() ),
                                          actionCollection(), "view_header" );
    actionViewFooter = new KToggleAction( i18n( "&Footer" ), 0,
                                          this, SLOT( viewFooter() ),
                                          actionCollection(), "view_footer" );
    actionViewFootNotes = new KToggleAction( i18n( "&Footnotes" ), 0,
                                             this, SLOT( viewFootNotes() ),
                                          actionCollection(), "view_footnotes" );
    ( (KToggleAction*)actionViewFootNotes )->setExclusiveGroup( "notes" );
    actionViewEndNotes = new KToggleAction( i18n( "&Endnotes" ), 0,
                                             this, SLOT( viewEndNotes() ),
                                          actionCollection(), "view_endnotes" );
    ( (KToggleAction*)actionViewEndNotes )->setExclusiveGroup( "notes" );

    actionViewZoom = new KSelectAction( i18n( "Zoom" ), 0,
                                        actionCollection(), "view_zoom" );
    connect( ( ( KSelectAction* )actionViewZoom ), SIGNAL( activated( const QString & ) ),
             this, SLOT( viewZoom( const QString & ) ) );
    QStringList lst;
    lst << "50%";
    lst << "100%";
    lst << "150%";
    lst << "200%";
    lst << "250%";
    lst << "350%";
    lst << "400%";
    lst << "450%";
    lst << "500%";
    ( (KSelectAction*)actionViewZoom )->setItems( lst );
    // -------------- Insert actions
    actionInsertPicture = new KAction( i18n( "&Picture..." ), "picture", Key_F2,
                                       this, SLOT( insertPicture() ),
                                       actionCollection(), "insert_picture" );
#if 0
    actionInsertClipart = new KAction( i18n( "&Clipart..." ), "clipart", Key_F3,
                                       this, SLOT( insertClipart() ),
                                       actionCollection(), "insert_clipart" );
#endif
#if 0
    actionInsertSpecialChar = new KAction( i18n( "&Special Character..." ), "char", ALT + SHIFT + Key_C,
                                           this, SLOT( insertSpecialChar() ),
                                           actionCollection(), "insert_specialchar" );
#endif
    actionInsertFrameBreak = new KAction( i18n( "&Hard Frame Break" ), CTRL + Key_Return,
                                          this, SLOT( insertFrameBreak() ),
                                          actionCollection(), "insert_framebreak" );
    actionInsertFootEndNote = new KAction( i18n( "&Footnote or Endnote..." ), 0,
                                           this, SLOT( insertFootNoteEndNote() ),
                                           actionCollection(), "insert_footendnote" );
    actionInsertContents = new KAction( i18n( "&Table of Contents" ), 0,
                                        this, SLOT( insertContents() ),
                                        actionCollection(), "insert_contents" );
    actionInsertVarDateFix = new KAction( i18n( "Date (&fix)" ), 0,
                                          this, SLOT( insertVariableDateFix() ),
                                          actionCollection(), "insert_var_datefix" );
    actionInsertVarDate = new KAction( i18n( "&Date (variable)" ), 0,
                                       this, SLOT( insertVariableDateVar() ),
                                       actionCollection(), "insert_var_datevar" );
    actionInsertVarTimeFix = new KAction( i18n( "Time (&fix)" ), 0,
                                          this, SLOT( insertVariableTimeFix() ),
                                          actionCollection(), "insert_var_timefix" );
    actionInsertVarTime = new KAction( i18n( "&Time (variable)" ), 0,
                                       this, SLOT( insertVariableTimeVar() ),
                                       actionCollection(), "insert_var_timevar" );
    actionInsertVarPgNum = new KAction( i18n( "&Page Number" ), 0,
                                        this, SLOT( insertVariablePageNum() ),
                                        actionCollection(), "insert_var_pgnum" );
    actionInsertVarCustom = new KAction( i18n( "&Custom..." ), 0,
                                         this, SLOT( insertVariableCustom() ),
                                         actionCollection(), "insert_var_custom" );
    actionInsertVarSerialLetter = new KAction( i18n( "&Serial Letter..." ), 0,
                                               this, SLOT( insertVariableSerialLetter() ),
                                               actionCollection(), "insert_var_serialletter" );

    // ---------------- Tools actions
    actionToolsEdit = new KToggleAction( i18n( "Edit &Text" ), "edittool", Key_F4,
                                         this, SLOT( toolsEdit() ),
                                         actionCollection(), "tools_edit" );
    ( (KToggleAction*)actionToolsEdit )->setExclusiveGroup( "tools" );
    actionToolsEditFrames = new KToggleAction( i18n( "Edit &Frames" ), "editframetool", Key_F5,
                                               this, SLOT( toolsEditFrame() ),
                                               actionCollection(), "tools_editframes" );
    ( (KToggleAction*)actionToolsEditFrames )->setExclusiveGroup( "tools" );
    actionToolsCreateText = new KToggleAction( i18n( "&Create Text Frame" ), "textframetool", Key_F6,
                                               this, SLOT( toolsCreateText() ),
                                               actionCollection(), "tools_createtext" );
    ( (KToggleAction*)actionToolsCreateText )->setExclusiveGroup( "tools" );
    actionToolsCreatePix = new KToggleAction( i18n( "&Create Picture Frame" ), "picframetool", Key_F7,
                                              this, SLOT( toolsCreatePix() ),
                                              actionCollection(), "tools_createpix" );
    ( (KToggleAction*)actionToolsCreatePix )->setExclusiveGroup( "tools" );
    actionToolsCreateClip = new KToggleAction( i18n( "&Create Clipart Frame" ), "clipart", Key_F8,
                                               this, SLOT( toolsClipart() ),
                                               actionCollection(), "tools_createclip" );
    ( (KToggleAction*)actionToolsCreateClip )->setExclusiveGroup( "tools" );
    actionToolsCreateTable = new KToggleAction( i18n( "&Create Table" ), "table", Key_F9,
                                                this, SLOT( toolsTable() ),
                                                actionCollection(), "tools_table" );
    ( (KToggleAction*)actionToolsCreateTable )->setExclusiveGroup( "tools" );
    actionToolsCreateKSpreadTable = new KToggleAction( i18n( "&Create KSpread Table Frame" ), "table", Key_F10,
                                                       this, SLOT( toolsKSpreadTable() ),
                                                       actionCollection(), "tools_kspreadtable" );
    ( (KToggleAction*)actionToolsCreateKSpreadTable )->setExclusiveGroup( "tools" );
    actionToolsCreateFormula = new KToggleAction( i18n( "&Create Formula Frame" ), "formula", Key_F11,
                                                  this, SLOT( toolsFormula() ),
                                                  actionCollection(), "tools_formula" );
    ( (KToggleAction*)actionToolsCreateFormula )->setExclusiveGroup( "tools" );
    actionToolsCreatePart = new KToggleAction( i18n( "&Create Part Frame" ), "parts", Key_F12,
                                               this, SLOT( toolsPart() ),
                                               actionCollection(), "tools_part" );
    ( (KToggleAction*)actionToolsCreatePart )->setExclusiveGroup( "tools" );


    // ------------------------- Format actions
    actionFormatFont = new KAction( i18n( "&Font..." ), ALT + CTRL + Key_F,
                                    this, SLOT( formatFont() ),
                                    actionCollection(), "format_font" );
    actionFormatColor = new KColorAction( i18n( "&Color..." ), KColorAction::TextColor, ALT + CTRL + Key_C,
                                     this, SLOT( textColor() ),
                                     actionCollection(), "format_color" );
    actionFormatParag = new KAction( i18n( "&Paragraph..." ), 0,
                                     this, SLOT( formatParagraph() ),
                                     actionCollection(), "format_paragraph" );
    actionFormatFrameSet = new KAction( i18n( "&Frame/Frameset..." ), 0,
                                     this, SLOT( formatFrameSet() ),
                                     actionCollection(), "format_frameset" );
    actionFormatPage = new KAction( i18n( "P&age..." ), 0,
                                     this, SLOT( formatPage() ),
                                    actionCollection(), "format_page" );
    actionFormatFontSize = new KFontSizeAction( i18n( "Font Size" ), 0,
                                              actionCollection(), "format_fontsize" );
    connect( ( ( KFontSizeAction* )actionFormatFontSize ), SIGNAL( activated( const QString & ) ),
             this, SLOT( textSizeSelected( const QString & ) ) );
    actionFormatFontFamily = new KFontAction( i18n( "Font Family" ), 0,
                                              actionCollection(), "format_fontfamily" );
    connect( ( ( KFontAction* )actionFormatFontFamily ), SIGNAL( activated( const QString & ) ),
             this, SLOT( textFontSelected( const QString & ) ) );
    actionFormatStyle = new KSelectAction( i18n( "Style" ), 0,
                                           actionCollection(), "format_style" );
    connect( ( ( KSelectAction* )actionFormatStyle ), SIGNAL( activated( const QString & ) ),
             this, SLOT( textStyleSelected( const QString & ) ) );
    lst.clear();
    for ( unsigned int i = 0; i < m_pKWordDoc->paragLayoutList.count(); i++ )
        lst << m_pKWordDoc->paragLayoutList.at( i )->getName();
    styleList = lst;
    ( (KSelectAction*)actionFormatStyle )->setItems( lst );
    actionFormatBold = new KToggleAction( i18n( "&Bold" ), "text_bold", CTRL + Key_B,
                                           this, SLOT( textBold() ),
                                           actionCollection(), "format_bold" );
    actionFormatItalic = new KToggleAction( i18n( "&Italic" ), "text_italic", CTRL + Key_I,
                                           this, SLOT( textItalic() ),
                                           actionCollection(), "format_italic" );
    actionFormatUnderline = new KToggleAction( i18n( "&Underline" ), "text_under", CTRL + Key_U,
                                           this, SLOT( textUnderline() ),
                                           actionCollection(), "format_underline" );
    actionFormatAlignLeft = new KToggleAction( i18n( "Align &Left" ), "alignLeft", ALT + Key_L,
                                       this, SLOT( textAlignLeft() ),
                                       actionCollection(), "format_alignleft" );
    ( (KToggleAction*)actionFormatAlignLeft )->setExclusiveGroup( "align" );
    ( (KToggleAction*)actionFormatAlignLeft )->setChecked( TRUE );
    actionFormatAlignCenter = new KToggleAction( i18n( "Align &Center" ), "alignCenter", ALT + Key_C,
                                         this, SLOT( textAlignCenter() ),
                                         actionCollection(), "format_aligncenter" );
    ( (KToggleAction*)actionFormatAlignCenter )->setExclusiveGroup( "align" );
    actionFormatAlignRight = new KToggleAction( i18n( "Align &Right" ), "alignRight", ALT + Key_R,
                                        this, SLOT( textAlignRight() ),
                                        actionCollection(), "format_alignright" );
    ( (KToggleAction*)actionFormatAlignRight )->setExclusiveGroup( "align" );
    actionFormatAlignBlock = new KToggleAction( i18n( "Align &Block" ), "alignBlock", ALT + Key_B,
                                        this, SLOT( textAlignBlock() ),
                                        actionCollection(), "format_alignblock" );
    ( (KToggleAction*)actionFormatAlignBlock )->setExclusiveGroup( "align" );
    actionFormatLineSpacing = new KSelectAction( i18n( "Linespacing" ), 0,
                                                 actionCollection(), "format_linespacing" );
    connect( ( ( KSelectAction* )actionFormatLineSpacing ), SIGNAL( activated( const QString & ) ),
             this, SLOT( textLineSpacing( const QString & ) ) );
    lst.clear();
    for ( unsigned int i = 0; i <= 10; i++ )
        lst << QString( "%1" ).arg( i );
    ( (KSelectAction*)actionFormatLineSpacing )->setItems( lst );
    actionFormatEnumList = new KToggleAction( i18n( "Enumerated List" ), "enumList", 0,
                                              this, SLOT( textEnumList() ),
                                              actionCollection(), "format_enumlist" );
    ( (KToggleAction*)actionFormatEnumList )->setExclusiveGroup( "style" );
    actionFormatUnsortList = new KToggleAction( i18n( "Bullet List" ), "unsortedList", 0,
                                              this, SLOT( textUnsortList() ),
                                              actionCollection(), "format_unsortlist" );
    ( (KToggleAction*)actionFormatUnsortList )->setExclusiveGroup( "style" );
    actionFormatSuper = new KToggleAction( i18n( "Superscript" ), "super", 0,
                                              this, SLOT( textSuperScript() ),
                                              actionCollection(), "format_super" );
    ( (KToggleAction*)actionFormatSuper )->setExclusiveGroup( "valign" );
    actionFormatSub = new KToggleAction( i18n( "Subscript" ), "sub", 0,
                                              this, SLOT( textSubScript() ),
                                              actionCollection(), "format_sub" );
    ( (KToggleAction*)actionFormatSub )->setExclusiveGroup( "valign" );
    actionFormatBrdLeft = new KToggleAction( i18n( "Paragraph Border Left" ), "borderleft", 0,
                                             this, SLOT( textBorderLeft() ),
                                             actionCollection(), "format_brdleft" );
    actionFormatBrdRight = new KToggleAction( i18n( "Paragraph Border Right" ), "borderright", 0,
                                             this, SLOT( textBorderRight() ),
                                             actionCollection(), "format_brdright" );
    actionFormatBrdTop = new KToggleAction( i18n( "Paragraph Border Top" ), "bordertop", 0,
                                             this, SLOT( textBorderTop() ),
                                             actionCollection(), "format_brdtop" );
    actionFormatBrdBottom = new KToggleAction( i18n( "Paragraph Border Bottom" ), "borderbottom", 0,
                                               this, SLOT( textBorderBottom() ),
                                             actionCollection(), "format_brdbottom" );
    actionFormatBrdColor = new KSelectColorAction( i18n( "Paragraph Border Color" ), KColorAction::FrameColor, 0,
                                             this, SLOT( textBorderColor() ),
                                             actionCollection(), "format_brdcolor" );
    actionFormatBrdWidth = new KSelectAction( i18n( "Paragraph Border Width" ), 0,
                                                 actionCollection(), "format_brdwidth" );
    connect( ( ( KSelectAction* )actionFormatBrdWidth ), SIGNAL( activated( const QString & ) ),
             this, SLOT( textBorderWidth( const QString & ) ) );
    lst.clear();
    for ( unsigned int i = 0; i < 10; i++ )
        lst << QString( "%1" ).arg( i + 1 );
    ( (KSelectAction*)actionFormatBrdWidth )->setItems( lst );
    actionFormatBrdStyle = new KSelectAction( i18n( "Paragraph Border Style" ), 0,
                                                 actionCollection(), "format_brdstyle" );
    connect( ( ( KSelectAction* )actionFormatBrdStyle ), SIGNAL( activated( const QString & ) ),
             this, SLOT( textBorderStyle( const QString & ) ) );
    lst.clear();
    lst.append( i18n( "solid line" ) );
    lst.append( i18n( "dash line ( ---- )" ) );
    lst.append( i18n( "dot line ( **** )" ) );
    lst.append( i18n( "dash dot line ( -*-* )" ) );
    lst.append( i18n( "dash dot dot line ( -**- )" ) );
    ( (KSelectAction*)actionFormatBrdStyle )->setItems( lst );

    // ---------------------------- frame toolbar actions

    actionFrameBrdLeft = new KToggleAction( i18n( "Frame Border Left" ), "borderleft", 0,
                                             this, SLOT( frameBorderLeft() ),
                                             actionCollection(), "frame_brdleft" );
    actionFrameBrdRight = new KToggleAction( i18n( "Frame Border Right" ), "borderright", 0,
                                             this, SLOT( frameBorderRight() ),
                                             actionCollection(), "frame_brdright" );
    actionFrameBrdTop = new KToggleAction( i18n( "Frame Border Top" ), "bordertop", 0,
                                             this, SLOT( frameBorderTop() ),
                                             actionCollection(), "frame_brdtop" );
    actionFrameBrdBottom = new KToggleAction( i18n( "Frame Border Bottom" ), "borderbottom", 0,
                                               this, SLOT( frameBorderBottom() ),
                                             actionCollection(), "frame_brdbottom" );
    actionFrameBrdColor = new KSelectColorAction( i18n( "Frame Border Color" ), KColorAction::FrameColor, 0,
                                             this, SLOT( frameBorderColor() ),
                                             actionCollection(), "frame_brdcolor" );
    actionFrameBrdStyle = new KSelectAction( i18n( "Frame Border Style" ), 0,
                                             actionCollection(), "frame_brdstyle" );
    connect( ( ( KSelectAction* )actionFrameBrdStyle ), SIGNAL( activated( const QString & ) ),
             this, SLOT( frameBorderStyle( const QString & ) ) );
    ( (KSelectAction*)actionFrameBrdStyle )->setItems( lst );
    actionFrameBrdWidth = new KSelectAction( i18n( "Frame Border Width" ), 0,
                                                 actionCollection(), "frame_brdwidth" );
    connect( ( ( KSelectAction* )actionFrameBrdWidth ), SIGNAL( activated( const QString & ) ),
             this, SLOT( frameBorderWidth( const QString & ) ) );
    lst.clear();
    for ( unsigned int i = 0; i < 10; i++ )
        lst << QString( "%1" ).arg( i + 1 );
    ( (KSelectAction*)actionFrameBrdWidth )->setItems( lst );
    actionFrameBackColor = new KSelectColorAction( i18n( "Frame Background Color" ), KColorAction::BackgroundColor, 0,
                                             this, SLOT( frameBackColor() ),
                                             actionCollection(), "frame_backcolor" );

    // ---------------------- formula toolbar actions

    actionFormulaPower = new KAction( i18n( "Power" ), "rsup", 0,
                                      this, SLOT( formulaPower() ),
                                      actionCollection(), "formula_power" );
    actionFormulaSubscript = new KAction( i18n( "Subscript" ), "rsub" , 0,
                                      this, SLOT( formulaSubscript() ),
                                      actionCollection(), "formula_subscript" );
    actionFormulaParentheses = new KAction( i18n( "Parentheses" ), "paren" , 0,
                                      this, SLOT( formulaParentheses() ),
                                      actionCollection(), "formula_parentheses" );
    actionFormulaAbs = new KAction( i18n( "Absolute Value" ), "abs", 0,
                                    this, SLOT( formulaAbsValue() ),
                                      actionCollection(), "formula_abs" );
    actionFormulaBrackets = new KAction( i18n( "Brackets" ), "brackets", 0,
                                      this, SLOT( formulaBrackets() ),
                                      actionCollection(), "formula_brackets" );
    actionFormulaFraction = new KAction( i18n( "Fraction" ), "frac", 0,
                                         this, SLOT( formulaFraction() ),
                                      actionCollection(), "formula_fraction" );
    actionFormulaRoot = new KAction( i18n( "Root" ), "sqrt", 0,
                                         this, SLOT( formulaRoot() ),
                                     actionCollection(), "formula_root" );
    actionFormulaIntegral = new KAction( i18n( "Integral" ), "int", 0,
                                         this, SLOT( formulaIntegral() ),
                                      actionCollection(), "formula_integral" );
    actionFormulaMatrix = new KAction( i18n( "Matrix" ), "matrix", 0,
                                         this, SLOT( formulaMatrix() ),
                                      actionCollection(), "formula_matrix" );
    actionFormulaLeftSuper = new KAction( i18n( "Left Superscript" ), "lsup" , 0,
                                         this, SLOT( formulaLeftSuper() ),
                                      actionCollection(), "formula_leftsup" );
    actionFormulaLeftSub = new KAction( i18n( "Left Subscript" ), "lsub" , 0,
                                         this, SLOT( formulaLeftSub() ),
                                      actionCollection(), "formula_leftsub" );
    actionFormulaProduct = new KAction( i18n( "Formula Product" ), "prod", 0,
                                         this, SLOT( formulaProduct() ),
                                      actionCollection(), "formula_product" );
    actionFormulaSum = new KAction( i18n( "Formula Sum" ), "sum", 0,
                                         this, SLOT( formulaSum() ),
                                      actionCollection(), "formula_sum" );

    // ---------------------- Table actions

    actionTableInsertRow = new KAction( i18n( "&Insert Row..." ), "insert_table_row", 0,
                               this, SLOT( tableInsertRow() ),
                               actionCollection(), "table_insrow" );
    actionTableInsertCol = new KAction( i18n( "&Insert Column..." ), "insert_table_col", 0,
                               this, SLOT( tableInsertCol() ),
                               actionCollection(), "table_inscol" );
    actionTableDelRow = new KAction( i18n( "&Delete Row..." ), "delete_table_row", 0,
                                     this, SLOT( tableDeleteRow() ),
                                     actionCollection(), "table_delrow" );
    actionTableDelCol = new KAction( i18n( "&Delete Column..." ), "delete_table_col", 0,
                               this, SLOT( tableDeleteCol() ),
                                     actionCollection(), "table_delcol" );
    actionTableJoinCells = new KAction( i18n( "&Join Cells" ), 0,
                                        this, SLOT( tableJoinCells() ),
                                        actionCollection(), "table_joincells" );
    actionTableSplitCells = new KAction( i18n( "&Split Cells" ), 0,
                                        this, SLOT( tableSplitCells() ),
                                         actionCollection(), "table_splitcells" );
    actionTableUngroup = new KAction( i18n( "&Ungroup Table" ), 0,
                                        this, SLOT( tableUngroupTable() ),
                                         actionCollection(), "table_ungroup" );
    actionTableDelete = new KAction( i18n( "&Delete Table" ), 0,
                                        this, SLOT( tableDelete() ),
                                         actionCollection(), "table_delete" );

    // ---------------------- Extra actions

    actionExtraSpellCheck = KStdAction::spelling( this, SLOT( extraSpelling() ), actionCollection(), "extra_spellcheck" );
    actionExtraAutocorrection = new KAction( i18n( "&Autocorrection..." ), 0,
                                         this, SLOT( extraAutoFormat() ),
                                         actionCollection(), "extra_autocorrection" );
    actionExtraStylist = new KAction( i18n( "&Stylist..." ), 0,
                                      this, SLOT( extraStylist() ),
                                      actionCollection(), "extra_stylist" );
    actionExtraOptions = new KAction( i18n( "&Options..." ), 0,
                                         this, SLOT( extraOptions() ),
                                      actionCollection(), "extra_options" );
    actionExtraCreateTemplate = new KAction( i18n( "&Create Template from Document..." ), 0,
                                             this, SLOT( extraCreateTemplate() ),
                                      actionCollection(), "extra_template" );
}

/*====================== construct ==============================*/
void KWordView::construct()
{
    if ( m_pKWordDoc == 0L && !m_bUnderConstruction ) return;

    assert( m_pKWordDoc != 0L );

    m_bUnderConstruction = false;

    // We are now in sync with the document
    m_bKWordModified = false;

    resizeEvent( 0L );
}

/*======================== create GUI ==========================*/
void KWordView::createKWordGUI()
{
    // setup GUI
    setupActions();

    gui = new KWordGUI( this, m_bShowGUI, m_pKWordDoc, this );
    gui->setGeometry( 0, 0, width(), height() );
    gui->show();

    gui->getPaperWidget()->formatChanged( format );

    setFormat( format, FALSE );

    if ( gui )
        gui->setDocument( m_pKWordDoc );

    format = m_pKWordDoc->getDefaultParagLayout()->getFormat();
    if ( gui )
        gui->getPaperWidget()->formatChanged( format );

    KWFrameSet *frameset;
    for ( unsigned int i = 0; i < m_pKWordDoc->getNumFrameSets(); i++ ) {
        frameset = m_pKWordDoc->getFrameSet( i );
        if ( frameset->getFrameType() == FT_PART )
            slotInsertObject( dynamic_cast<KWPartFrameSet*>( frameset )->getChild(),
                              dynamic_cast<KWPartFrameSet*>( frameset ) );
        else if ( frameset->getFrameType() == FT_FORMULA )
            dynamic_cast<KWFormulaFrameSet*>( frameset )->create( gui->getPaperWidget() );
    }
}

/*================================================================*/
void KWordView::showFormulaToolbar( bool show )
{
  if ( !factory() )
    return;

  QWidget *tb = factory()->container( "formula_toolbar", this );
  if( !tb )
    return;

  if ( show )
    tb->show();
  else
    tb->hide();
}

/*================================================================*/
void KWordView::clipboardDataChanged()
{
//     if ( kapp->clipboard()->text().isEmpty() ) {
//      m_vMenuEdit->setItemEnabled( m_idMenuEdit_Paste, FALSE );
//      m_vToolBarEdit->setItemEnabled( ID_EDIT_PASTE, FALSE );
//     } else {
//      m_vMenuEdit->setItemEnabled( m_idMenuEdit_Paste, TRUE );
//      m_vToolBarEdit->setItemEnabled( ID_EDIT_PASTE, TRUE );
//     }
}

/*=========================== file print =======================*/
bool KWordView::printDlg()
{
    QPrinter prt;
    prt.setMinMax( 1, m_pKWordDoc->getPages() );
    bool makeLandscape = FALSE;

    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    m_pKWordDoc->getPageLayout( pgLayout, cl, hf );

    switch ( pgLayout.format ) {
    case PG_DIN_A3: prt.setPageSize( QPrinter::A3 );
        break;
    case PG_DIN_A4: prt.setPageSize( QPrinter::A4 );
        break;
    case PG_DIN_A5: prt.setPageSize( QPrinter::A5 );
        break;
    case PG_US_LETTER: prt.setPageSize( QPrinter::Letter );
        break;
    case PG_US_LEGAL: prt.setPageSize( QPrinter::Legal );
        break;
    case PG_US_EXECUTIVE: prt.setPageSize( QPrinter::Executive );
        break;
    case PG_DIN_B5: prt.setPageSize( QPrinter::B5 );
        break;
    case PG_SCREEN: {
        kdWarning() << i18n( "You use the page layout SCREEN. I print it in DIN A4 LANDSCAPE!" ) << endl;
        prt.setPageSize( QPrinter::A4 );
        makeLandscape = TRUE;
    } break;
    default: {
        kdWarning() << i18n( "The used page layout is not supported by QPrinter. I set it to DIN A4." ) << endl;
        prt.setPageSize( QPrinter::A4 );
    } break;
    }

    switch ( pgLayout.orientation ) {
    case PG_PORTRAIT: prt.setOrientation( QPrinter::Portrait );
        break;
    case PG_LANDSCAPE: prt.setOrientation( QPrinter::Landscape );
        break;
    }

    float left_margin = 0.0;
    float top_margin = 0.0;

    if ( makeLandscape ) {
        prt.setOrientation( QPrinter::Landscape );
        left_margin = 28.5;
        top_margin = 15.0;
    }

    if ( prt.setup( this ) ) {
        setCursor( waitCursor );
        gui->getPaperWidget()->viewport()->setCursor( waitCursor );

        QList<KWVariable> *vars = m_pKWordDoc->getVariables();
        KWVariable *v = 0;
        bool serialLetter = FALSE;
        for ( v = vars->first(); v; v = vars->next() ) {
            if ( v->getType() == VT_SERIALLETTER ) {
                serialLetter = TRUE;
                break;
            }
        }

        if ( !m_pKWordDoc->getSerialLetterDataBase() ||
             m_pKWordDoc->getSerialLetterDataBase()->getNumRecords() == 0 )
            serialLetter = FALSE;

        if ( !serialLetter ) {
            QPainter painter;
            painter.begin( &prt );
            m_pKWordDoc->print( &painter, &prt, left_margin, top_margin );
            painter.end();
        } else {
            QPainter painter;
            painter.begin( &prt );
            for ( int i = 0;i < m_pKWordDoc->getSerialLetterDataBase()->getNumRecords(); ++i ) {
                m_pKWordDoc->setSerialLetterRecord( i );
                m_pKWordDoc->print( &painter, &prt, left_margin, top_margin );
                if ( i < m_pKWordDoc->getSerialLetterDataBase()->getNumRecords() - 1 )
                    prt.newPage();
            }
            m_pKWordDoc->setSerialLetterRecord( -1 );
            painter.end();
        }


        setCursor( arrowCursor );
        gui->getPaperWidget()->viewport()->setCursor( ibeamCursor );
    }
    return TRUE;
}

/*================================================================*/
void KWordView::setFormat( const KWFormat &_format, bool _check, bool _update_page, bool _redraw )
{
    if ( _check && _format == format ) return;

    if ( gui && gui->getPaperWidget() && gui->getPaperWidget()->getCursor() &&
         gui->getPaperWidget()->getCursor()->getParag()
         && gui->getPaperWidget()->getCursor()->getTextPos() > 0
         && gui->getPaperWidget()->getCursor()->getParag()->getKWString()->
         data()[ gui->getPaperWidget()->getCursor()->getTextPos() - 1 ].attrib
         && gui->getPaperWidget()->getCursor()->getParag()->
         getKWString()->data()[ gui->getPaperWidget()->getCursor()->getTextPos() - 1 ].attrib->getClassId() == ID_KWCharFootNote )
        return;

    format = _format;

    if ( !_format.getUserFont()->getFontName().isEmpty() ) {
        ( (KFontAction*)actionFormatFontFamily )->blockSignals( TRUE );
        ( (KFontAction*)actionFormatFontFamily )->
            setFont( _format.getUserFont()->getFontName() );
        ( (KFontAction*)actionFormatFontFamily )->blockSignals( FALSE );
    }

    if ( _format.getPTFontSize() != -1 ) {
        ( (KFontSizeAction*)actionFormatFontSize )->blockSignals( TRUE );
        ( (KFontSizeAction*)actionFormatFontSize )->setFontSize( format.getPTFontSize() );
        ( (KFontSizeAction*)actionFormatFontSize )->blockSignals( FALSE );
    }

    if ( _format.getWeight() != -1 ) {
        ( (KToggleAction*)actionFormatBold )->blockSignals( TRUE );
        ( (KToggleAction*)actionFormatBold )->setChecked( _format.getWeight() == QFont::Bold );
        ( (KToggleAction*)actionFormatBold )->blockSignals( FALSE );
        tbFont.setBold( _format.getWeight() == QFont::Bold );
    }
    if ( _format.getItalic() != -1 ) {
        ( (KToggleAction*)actionFormatItalic )->blockSignals( TRUE );
        ( (KToggleAction*)actionFormatItalic )->setChecked( _format.getItalic() );
        ( (KToggleAction*)actionFormatItalic )->blockSignals( FALSE );
        tbFont.setItalic( _format.getItalic() );
    }
    if ( _format.getUnderline() != -1 ) {
        ( (KToggleAction*)actionFormatUnderline )->blockSignals( TRUE );
        ( (KToggleAction*)actionFormatUnderline )->setChecked( _format.getUnderline() );
        ( (KToggleAction*)actionFormatUnderline )->blockSignals( FALSE );
        tbFont.setUnderline( _format.getUnderline() );
    }

    if ( _format.getColor().isValid() ) {
        ( (KColorAction*)actionFormatColor )->blockSignals( TRUE );
        ( (KColorAction*)actionFormatColor )->setColor( _format.getColor() );
        ( (KColorAction*)actionFormatColor )->blockSignals( FALSE );
        tbColor = QColor( _format.getColor() );
    }

    ( (KToggleAction*)actionFormatSuper )->blockSignals( TRUE );
    ( (KToggleAction*)actionFormatSuper )->setChecked( _format.getVertAlign() == KWFormat::VA_SUPER );
    ( (KToggleAction*)actionFormatSuper )->blockSignals( FALSE );
    ( (KToggleAction*)actionFormatSub )->blockSignals( TRUE );
    ( (KToggleAction*)actionFormatSub )->setChecked( _format.getVertAlign() == KWFormat::VA_SUB );
    ( (KToggleAction*)actionFormatSub )->blockSignals( FALSE );

    format = _format;

    if ( _update_page )
        gui->getPaperWidget()->formatChanged( format, _redraw );
}

/*================================================================*/
void KWordView::setFlow( KWParagLayout::Flow _flow )
{
    if ( _flow != flow ) {
        flow = _flow;
        switch ( flow ) {
        case KWParagLayout::LEFT:
            ( (KToggleAction*)actionFormatAlignLeft )->blockSignals( TRUE );
            ( (KToggleAction*)actionFormatAlignLeft )->setChecked( TRUE );
            ( (KToggleAction*)actionFormatAlignLeft )->blockSignals( FALSE );
            break;
        case KWParagLayout::CENTER:
            ( (KToggleAction*)actionFormatAlignCenter )->blockSignals( TRUE );
            ( (KToggleAction*)actionFormatAlignCenter )->setChecked( TRUE );
            ( (KToggleAction*)actionFormatAlignCenter )->blockSignals( FALSE );
            break;
        case KWParagLayout::RIGHT:
            ( (KToggleAction*)actionFormatAlignRight )->blockSignals( TRUE );
            ( (KToggleAction*)actionFormatAlignRight )->setChecked( TRUE );
            ( (KToggleAction*)actionFormatAlignRight )->blockSignals( FALSE );
            break;
        case KWParagLayout::BLOCK:
            ( (KToggleAction*)actionFormatAlignBlock )->blockSignals( TRUE );
            ( (KToggleAction*)actionFormatAlignBlock )->setChecked( TRUE );
            ( (KToggleAction*)actionFormatAlignBlock )->blockSignals( FALSE );
            break;
        }
    }
}

/*================================================================*/
void KWordView::setLineSpacing( int _spc )
{
    if ( _spc != spc ) {
        spc = _spc;
        ( (KSelectAction*)actionFormatLineSpacing )->blockSignals( TRUE );
        ( (KSelectAction*)actionFormatLineSpacing )->setCurrentItem( _spc );
        ( (KSelectAction*)actionFormatLineSpacing )->blockSignals( FALSE );
    }
}

/*================================================================*/
void KWordView::setParagBorders( KWParagLayout::Border _left, KWParagLayout::Border _right,
                                 KWParagLayout::Border _top, KWParagLayout::Border _bottom )
{
    if ( left != _left || right != _right || top != _top || bottom != _bottom ) {
        left = _left;
        right = _right;
        top = _top;
        bottom = _bottom;

        ( (KToggleAction*)actionFormatBrdLeft )->blockSignals( TRUE );
        ( (KToggleAction*)actionFormatBrdLeft )->setChecked( left.ptWidth > 0 );
        ( (KToggleAction*)actionFormatBrdLeft )->blockSignals( FALSE );
        ( (KToggleAction*)actionFormatBrdRight )->blockSignals( TRUE );
        ( (KToggleAction*)actionFormatBrdRight )->setChecked( right.ptWidth > 0 );
        ( (KToggleAction*)actionFormatBrdRight )->blockSignals( FALSE );
        ( (KToggleAction*)actionFormatBrdTop )->blockSignals( TRUE );
        ( (KToggleAction*)actionFormatBrdTop )->setChecked( top.ptWidth > 0 );
        ( (KToggleAction*)actionFormatBrdTop )->blockSignals( FALSE );
        ( (KToggleAction*)actionFormatBrdBottom )->blockSignals( TRUE );
        ( (KToggleAction*)actionFormatBrdBottom )->setChecked( bottom.ptWidth > 0 );
        ( (KToggleAction*)actionFormatBrdBottom )->blockSignals( FALSE );
        if ( left.ptWidth > 0 ) {
            tmpBrd = left;
            setParagBorderValues();
        }
        if ( right.ptWidth > 0 ) {
            tmpBrd = right;
            setParagBorderValues();
        }
        if ( top.ptWidth > 0 ) {
            tmpBrd = top;
            setParagBorderValues();
        }
        if ( bottom.ptWidth > 0 ) {
            tmpBrd = bottom;
            setParagBorderValues();
        }
    }
}

void KWordView::updateReadWrite( bool readwrite )
{
#ifdef __GNUC__
#warning TODO
#endif
  QValueList<KAction*> actions = actionCollection()->actions();
  QValueList<KAction*>::ConstIterator aIt = actions.begin();
  QValueList<KAction*>::ConstIterator aEnd = actions.end();
  for (; aIt != aEnd; ++aIt )
    (*aIt)->setEnabled( readwrite );
}

/*===============================================================*/
void KWordView::setTool( MouseMode _mouseMode )
{
    switch ( _mouseMode ) {
    case MM_EDIT:
        ( (KToggleAction*)actionToolsEdit )->blockSignals( TRUE );
        ( (KToggleAction*)actionToolsEdit )->setChecked( TRUE );
        ( (KToggleAction*)actionToolsEdit )->blockSignals( FALSE );
        break;
    case MM_EDIT_FRAME:
        ( (KToggleAction*)actionToolsEditFrames )->blockSignals( TRUE );
        ( (KToggleAction*)actionToolsEditFrames )->setChecked( TRUE );
        ( (KToggleAction*)actionToolsEditFrames )->blockSignals( FALSE );
        break;
    case MM_CREATE_TEXT:
        ( (KToggleAction*)actionToolsCreateText )->blockSignals( TRUE );
        ( (KToggleAction*)actionToolsCreateText )->setChecked( TRUE );
        ( (KToggleAction*)actionToolsCreateText )->blockSignals( FALSE );
        break;
    case MM_CREATE_PIX:
        ( (KToggleAction*)actionToolsCreatePix )->blockSignals( TRUE );
        ( (KToggleAction*)actionToolsCreatePix )->setChecked( TRUE );
        ( (KToggleAction*)actionToolsCreatePix )->blockSignals( FALSE );
        break;
    case MM_CREATE_CLIPART:
        ( (KToggleAction*)actionToolsCreateClip )->blockSignals( TRUE );
        ( (KToggleAction*)actionToolsCreateClip )->setChecked( TRUE );
        ( (KToggleAction*)actionToolsCreateClip )->blockSignals( FALSE );
        break;
    case MM_CREATE_TABLE:
        ( (KToggleAction*)actionToolsCreateTable )->blockSignals( TRUE );
        ( (KToggleAction*)actionToolsCreateTable )->setChecked( TRUE );
        ( (KToggleAction*)actionToolsCreateTable )->blockSignals( FALSE );
        break;
    case MM_CREATE_KSPREAD_TABLE:
        ( (KToggleAction*)actionToolsCreateKSpreadTable )->blockSignals( TRUE );
        ( (KToggleAction*)actionToolsCreateKSpreadTable )->setChecked( TRUE );
        ( (KToggleAction*)actionToolsCreateKSpreadTable )->blockSignals( FALSE );
        break;
    case MM_CREATE_FORMULA:
        ( (KToggleAction*)actionToolsCreateFormula )->blockSignals( TRUE );
        ( (KToggleAction*)actionToolsCreateFormula )->setChecked( TRUE );
        ( (KToggleAction*)actionToolsCreateFormula )->blockSignals( FALSE );
        break;
    case MM_CREATE_PART:
        ( (KToggleAction*)actionToolsCreatePart )->blockSignals( TRUE );
        ( (KToggleAction*)actionToolsCreatePart )->setChecked( TRUE );
        ( (KToggleAction*)actionToolsCreatePart )->blockSignals( FALSE );
        break;
    }
    QWidget *tbFormat = 0;
    QWidget *tbFrame = 0;

    if ( factory() )
    {
      tbFormat = factory()->container( "border_toolbar", this );
      tbFrame = factory()->container( "frame_toolbar", this );
    }
    if ( tbFrame && tbFormat )
    {
      if ( _mouseMode == MM_EDIT_FRAME )
      {
        tbFormat->hide();
        tbFrame->show();
      }
      else
      {
        tbFormat->show();
        tbFrame->hide();
      }
    }
    actionTableInsertRow->setEnabled( FALSE );
    actionTableInsertCol->setEnabled( FALSE );
    actionTableDelRow->setEnabled( FALSE );
    actionTableDelCol->setEnabled( FALSE );
    actionTableJoinCells->setEnabled( FALSE );
    actionTableSplitCells->setEnabled( FALSE );
    actionTableDelete->setEnabled( FALSE );

    switch ( _mouseMode ) {
    case MM_EDIT: {
        actionTableInsertRow->setEnabled( TRUE );
        actionTableInsertCol->setEnabled( TRUE );
        actionTableDelRow->setEnabled( TRUE );
        actionTableDelCol->setEnabled( TRUE );
        actionTableJoinCells->setEnabled( TRUE );
        actionTableSplitCells->setEnabled( TRUE );
        actionTableDelete->setEnabled( TRUE );
    } break;
    case MM_EDIT_FRAME: {
        actionTableJoinCells->setEnabled( TRUE );
        actionTableSplitCells->setEnabled( TRUE );
    } break;
    default: break;
    }
}

/*===============================================================*/
void KWordView::updateStyle( QString _styleName, bool _updateFormat )
{
    int pos = styleList.findIndex( _styleName );

    if ( pos == -1 )
        return;

    ( (KSelectAction*)actionFormatStyle )->blockSignals( TRUE );
    ( (KSelectAction*)actionFormatStyle )->setCurrentItem( pos );
    ( (KSelectAction*)actionFormatStyle )->blockSignals( FALSE );

    ( (KToggleAction*)actionFormatEnumList )->blockSignals( TRUE );
    ( (KToggleAction*)actionFormatEnumList )->setChecked( _styleName == "Enumerated List" );
    ( (KToggleAction*)actionFormatEnumList )->blockSignals( FALSE );

    ( (KToggleAction*)actionFormatUnsortList )->blockSignals( TRUE );
    ( (KToggleAction*)actionFormatUnsortList )->setChecked( _styleName == "Bullet List" );
    ( (KToggleAction*)actionFormatUnsortList )->blockSignals( FALSE );

    setFormat( m_pKWordDoc->findParagLayout( _styleName )->getFormat(), FALSE, _updateFormat, FALSE );

    if ( gui )
        gui->getHorzRuler()->setTabList( m_pKWordDoc->findParagLayout( _styleName )->getTabList() );
}

/*===============================================================*/
void KWordView::updateStyleList()
{
    styleList.clear();
    for ( unsigned int i = 0; i < m_pKWordDoc->paragLayoutList.count(); i++ ) {
        styleList.append( m_pKWordDoc->paragLayoutList.at( i )->getName() );
    }
    ( (KSelectAction*)actionFormatStyle )->setItems( styleList );
    updateStyle( gui->getPaperWidget()->getParagLayout()->getName() );
}

/*===============================================================*/
void KWordView::editUndo()
{
    m_pKWordDoc->undo();
    gui->getPaperWidget()->recalcWholeText( TRUE );
    if ( gui->getPaperWidget()->formulaIsActive() )
        gui->getPaperWidget()->insertFormulaChar( UNDO_CHAR );
}

/*===============================================================*/
void KWordView::editRedo()
{
    m_pKWordDoc->redo();
    gui->getPaperWidget()->recalcWholeText( TRUE );
    if ( gui->getPaperWidget()->formulaIsActive() )
        gui->getPaperWidget()->insertFormulaChar( REDO_CHAR );
}

/*===============================================================*/
void KWordView::editCut()
{
    if ( gui->getPaperWidget()->formulaIsActive() )
        gui->getPaperWidget()->insertFormulaChar( CUT_CHAR );
    else
        gui->getPaperWidget()->editCut();
}

/*===============================================================*/
void KWordView::editCopy()
{
    if ( gui->getPaperWidget()->formulaIsActive() )
        gui->getPaperWidget()->insertFormulaChar( COPY_CHAR );
    else
        gui->getPaperWidget()->editCopy();
}

/*===============================================================*/
void KWordView::editPaste()
{
    if ( gui->getPaperWidget()->formulaIsActive() )
        gui->getPaperWidget()->insertFormulaChar( PASTE_CHAR );
    else {
        QClipboard *cb = QApplication::clipboard();

        if ( cb->data()->provides( MIME_TYPE ) ) {
            if ( cb->data()->encodedData( MIME_TYPE ).size() )
                gui->getPaperWidget()->editPaste( cb->data()->encodedData( MIME_TYPE ), MIME_TYPE );
        } else if ( cb->data()->provides( "text/plain" ) ) {
            if ( cb->data()->encodedData( "text/plain" ).size() )
                gui->getPaperWidget()->editPaste( cb->data()->encodedData( "text/plain" ) );
        } else if ( !cb->text().isEmpty() )
            gui->getPaperWidget()->editPaste( cb->text() );
    }
}

/*===============================================================*/
void KWordView::editSelectAll()
{
    gui->getPaperWidget()->selectAll();
}

/*===============================================================*/
void KWordView::editFind()
{
    if ( searchDia ) return;

    searchDia = new KWSearchDia( this, "", m_pKWordDoc, gui->getPaperWidget(), this, searchEntry, replaceEntry, fontList );
    searchDia->setCaption( i18n( "KWord - Search & Replace" ) );
    connect( searchDia, SIGNAL( closeClicked() ), this, SLOT( searchDiaClosed() ) );
    searchDia->show();
}

/*================================================================*/
void KWordView::editDeleteFrame()
{
    gui->getPaperWidget()->editDeleteFrame();
}

/*================================================================*/
void KWordView::editReconnectFrame()
{
    gui->getPaperWidget()->editReconnectFrame();
}

/*===============================================================*/
void KWordView::editCustomVars()
{
    KWVariableValueDia *dia = new KWVariableValueDia( this, m_pKWordDoc->getVariables() );
    dia->exec();
    gui->getPaperWidget()->recalcWholeText();
    gui->getPaperWidget()->repaintScreen( FALSE );
    delete dia;
}

/*===============================================================*/
void KWordView::editSerialLetterDataBase()
{
    KWSerialLetterEditor *dia = new KWSerialLetterEditor( this, m_pKWordDoc->getSerialLetterDataBase() );
    dia->exec();
    gui->getPaperWidget()->recalcWholeText();
    gui->getPaperWidget()->repaintScreen( FALSE );
    delete dia;
}

/*===============================================================*/
void KWordView::viewFormattingChars()
{
    _viewFormattingChars = ( (KToggleAction*)actionViewFormattingChars )->isChecked();
    gui->getPaperWidget()->repaintScreen( !_viewFormattingChars );
}

/*===============================================================*/
void KWordView::viewFrameBorders()
{
    _viewFrameBorders = ( (KToggleAction*)actionViewFrameBorders )->isChecked();
    gui->getPaperWidget()->repaintScreen( FALSE );
}

/*===============================================================*/
void KWordView::viewTableGrid()
{
    _viewTableGrid = ( (KToggleAction*)actionViewTableGrid )->isChecked();
    gui->getPaperWidget()->repaintScreen( !_viewTableGrid );
}

/*===============================================================*/
void KWordView::viewHeader()
{
    m_pKWordDoc->setHeader( ( (KToggleAction*)actionViewHeader )->isChecked() );
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    m_pKWordDoc->getPageLayout( pgLayout, cl, hf );
    m_pKWordDoc->setPageLayout( pgLayout, cl, hf );
}

/*===============================================================*/
void KWordView::viewFooter()
{
    m_pKWordDoc->setFooter( ( (KToggleAction*)actionViewFooter )->isChecked() );
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    m_pKWordDoc->getPageLayout( pgLayout, cl, hf );
    m_pKWordDoc->setPageLayout( pgLayout, cl, hf );
}

/*===============================================================*/
void KWordView::viewFootNotes()
{
    if ( !( (KToggleAction*)actionViewFootNotes )->isChecked() )
        return;
    setNoteType( KWFootNoteManager::FootNotes);
}

/*===============================================================*/
void KWordView::viewEndNotes()
{
    if ( !( (KToggleAction*)actionViewEndNotes )->isChecked() )
        return;
    setNoteType( KWFootNoteManager::EndNotes);
}

void KWordView::setNoteType( KWFootNoteManager::NoteType nt, bool change)
{
    if (change)
        m_pKWordDoc->setNoteType( nt );
    switch (nt)
    {
      case KWFootNoteManager::FootNotes:
      ( (KToggleAction*)actionViewFootNotes )->blockSignals( TRUE );
      ( (KToggleAction*)actionViewFootNotes )->setChecked( TRUE );
      ( (KToggleAction*)actionViewFootNotes )->blockSignals( FALSE );
      actionInsertFootEndNote->setText(i18n("&Footnote"));
      break;
    case KWFootNoteManager::EndNotes:
      default:
      ( (KToggleAction*)actionViewEndNotes )->blockSignals( TRUE );
      ( (KToggleAction*)actionViewEndNotes )->setChecked( TRUE );
      ( (KToggleAction*)actionViewEndNotes )->blockSignals( FALSE );
      actionInsertFootEndNote->setText(i18n("&Endnote"));
      break;
    }
}

void KWordView::viewZoom( const QString &s )
{
    QString z( s );
    z = z.replace( QRegExp( "%" ), "" );
    z = z.simplifyWhiteSpace();
    int zoom = z.toInt();
    int oldZoom = m_pKWordDoc->getZoom();
    if ( zoom != m_pKWordDoc->getZoom() ) {
        KoPageLayout pgLayout;
        KoColumns cl;
        KoKWHeaderFooter hf;
        m_pKWordDoc->setZoom( 100 );
        m_pKWordDoc->getPageLayout( pgLayout, cl, hf );
        m_pKWordDoc->setZoom( zoom );
        m_pKWordDoc->updateFrameSizes( oldZoom );
        newPageLayout( pgLayout );
        gui->getVertRuler()->setZoom(static_cast<double>(zoom)/100.0);
        gui->getHorzRuler()->setZoom(static_cast<double>(zoom)/100.0);
    }
    gui->getPaperWidget()->setFocus();
}

/*===============================================================*/
void KWordView::insertPicture()
{
    QString file;
#ifdef USE_QFD
    QFileDialog fd( QString::null, i18n( "Pictures (*.gif *.png *.jpg *.jpeg *.xpm *.bmp)\nAll files (*)" ), 0, 0, TRUE );
    fd.setPreviewMode( FALSE, TRUE );
    fd.setContentsPreviewWidget( new Preview( &fd ) );
    fd.setViewMode( QFileDialog::ListView | QFileDialog::PreviewContents );
    if ( fd.exec() == QDialog::Accepted )
        file = fd.selectedFile();
#else
    KFileDialog fd( QString::null, KImageIO::pattern(KImageIO::Reading), 0, 0, TRUE );
    fd.setCaption(i18n("Insert Picture"));
    //fd.setPreviewMode( FALSE, TRUE );
    fd.setPreviewWidget( new Preview( &fd ) );
    //fd.setViewMode( QFileDialog::ListView | QFileDialog::PreviewContents );
    KURL url;
    if ( fd.exec() == QDialog::Accepted )
        url = fd.selectedURL();

    if( url.isEmpty() )
      return;

    if( !url.isLocalFile() )
    {
      KMessageBox::sorry( 0L, i18n( "Only local files supported yet." ) );
      return;
    }

    file = url.path();
#endif
    if ( !file.isEmpty() ) m_pKWordDoc->insertPicture( file, gui->getPaperWidget() );
}

/*===============================================================*/
void KWordView::insertClipart()
{
#ifdef __GNUC__
#warning "Inserting cliparts is currently not implemented!"
#endif
}

/*===============================================================*/
void KWordView::insertSpecialChar()
{
#ifdef __GNUC__
#warning "Inserting special characters is currently not implemented!"
#endif
}

/*===============================================================*/
void KWordView::insertFrameBreak()
{
    if ( gui->getPaperWidget()->getTable() ) return;

    QKeyEvent e(static_cast<QEvent::Type>(6) /*QEvent::KeyPress*/ ,Key_Return,0,ControlButton);
    gui->getPaperWidget()->keyPressEvent( &e );
}

/*===============================================================*/
void KWordView::insertVariableDateFix()
{
    gui->getPaperWidget()->insertVariable( VT_DATE_FIX );
}

/*===============================================================*/
void KWordView::insertVariableDateVar()
{
    gui->getPaperWidget()->insertVariable( VT_DATE_VAR );
}

/*===============================================================*/
void KWordView::insertVariableTimeFix()
{
    gui->getPaperWidget()->insertVariable( VT_TIME_FIX );
}

/*===============================================================*/
void KWordView::insertVariableTimeVar()
{
    gui->getPaperWidget()->insertVariable( VT_TIME_VAR );
}

/*===============================================================*/
void KWordView::insertVariablePageNum()
{
    gui->getPaperWidget()->insertVariable( VT_PGNUM );
}

/*===============================================================*/
void KWordView::insertVariableCustom()
{
    gui->getPaperWidget()->insertVariable( VT_CUSTOM );
}

/*===============================================================*/
void KWordView::insertVariableSerialLetter()
{
    gui->getPaperWidget()->insertVariable( VT_SERIALLETTER );
}

/*===============================================================*/
void KWordView::insertFootNoteEndNote()
{
    int start = m_pKWordDoc->getFootNoteManager().findStart( gui->getPaperWidget()->getCursor() );

    if ( start == -1 )
    {
        KMessageBox::sorry( this,
                            i18n( "Sorry, you can only insert footnotes or\n"
                                  "endnotes into the first frameset."),
                            i18n("Insert Footnote/Endnote"));
    } else {
        KWFootNoteDia dia( 0L, "", m_pKWordDoc, gui->getPaperWidget(), start,
                 m_pKWordDoc->getNoteType() == KWFootNoteManager::FootNotes );
        dia.show();
    }
}

/*===============================================================*/
void KWordView::insertContents()
{
    m_pKWordDoc->createContents();
    gui->getPaperWidget()->recalcWholeText();
    gui->getPaperWidget()->repaintScreen( FALSE );
}

/*===============================================================*/
void KWordView::formatFont()
{
    QFont tmpFont = tbFont;

    if ( KFontDialog::getFont( tmpFont ) ) {
        tbFont = tmpFont;
        format.setUserFont( m_pKWordDoc->findUserFont( tbFont.family() ) );
        format.setPTFontSize( tbFont.pointSize() );
        format.setWeight( tbFont.weight() );
        format.setItalic( tbFont.italic() );
        format.setUnderline( tbFont.underline() );
        ( (KFontAction*)actionFormatFontFamily )->blockSignals( TRUE );
        ( (KFontAction*)actionFormatFontFamily )->setFont( tbFont.family() );
        ( (KFontAction*)actionFormatFontFamily )->blockSignals( FALSE );
        ( (KFontSizeAction*)actionFormatFontSize )->blockSignals( TRUE );
        ( (KFontSizeAction*)actionFormatFontSize )->setFontSize( tbFont.pointSize() );
        ( (KFontSizeAction*)actionFormatFontSize )->blockSignals( FALSE );
        ( (KToggleAction*)actionFormatBold )->blockSignals( TRUE );
        ( (KToggleAction*)actionFormatBold )->setChecked( tbFont.bold() );
        ( (KToggleAction*)actionFormatBold )->blockSignals( FALSE );
        ( (KToggleAction*)actionFormatItalic )->blockSignals( TRUE );
        ( (KToggleAction*)actionFormatItalic )->setChecked( tbFont.italic() );
        ( (KToggleAction*)actionFormatItalic )->blockSignals( FALSE );
        ( (KToggleAction*)actionFormatUnderline )->blockSignals( TRUE );
        ( (KToggleAction*)actionFormatUnderline )->setChecked( tbFont.underline() );
        ( (KToggleAction*)actionFormatUnderline )->blockSignals( FALSE );
        if ( gui ) {
            gui->getPaperWidget()->formatChanged( format );
            gui->getPaperWidget()->setFocus();
        }
    }
}

/*===============================================================*/
void KWordView::formatParagraph()
{
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_SPACING | KWParagDia::PD_FLOW |
                               KWParagDia::PD_BORDERS |
                               KWParagDia::PD_NUMBERING | KWParagDia::PD_TABS, m_pKWordDoc );
    paragDia->setCaption( i18n( "KWord - Paragraph settings" ) );
    connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );
    paragDia->setLeftIndent( gui->getPaperWidget()->getLeftIndent() );
    paragDia->setFirstLineIndent( gui->getPaperWidget()->getFirstLineIndent() );
    paragDia->setSpaceBeforeParag( gui->getPaperWidget()->getSpaceBeforeParag() );
    paragDia->setSpaceAfterParag( gui->getPaperWidget()->getSpaceAfterParag() );
    paragDia->setLineSpacing( gui->getPaperWidget()->getLineSpacing() );
    paragDia->setFlow( gui->getPaperWidget()->getFlow() );
    paragDia->setLeftBorder( gui->getPaperWidget()->getLeftBorder() );
    paragDia->setRightBorder( gui->getPaperWidget()->getRightBorder() );
    paragDia->setTopBorder( gui->getPaperWidget()->getTopBorder() );
    paragDia->setBottomBorder( gui->getPaperWidget()->getBottomBorder() );
    paragDia->setCounter( gui->getPaperWidget()->getCounter() );
    paragDia->setTabList( gui->getPaperWidget()->getParagLayout()->getTabList() );
    paragDia->show();
    delete paragDia;
    paragDia = 0;
}

/*===============================================================*/
void KWordView::formatPage()
{
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter kwhf;
    m_pKWordDoc->getPageLayout( pgLayout, cl, kwhf );

    KoHeadFoot hf;
    int flags = FORMAT_AND_BORDERS | KW_HEADER_AND_FOOTER | DISABLE_UNIT;
    if ( m_pKWordDoc->getProcessingType() == KWordDocument::WP )
        flags = flags | COLUMNS;
    else
        flags = flags | DISABLE_BORDERS;

    if ( KoPageLayoutDia::pageLayout( pgLayout, hf, cl, kwhf, flags ) ) {
        m_pKWordDoc->setPageLayout( pgLayout, cl, kwhf );
        gui->getVertRuler()->setPageLayout( pgLayout );
        gui->getHorzRuler()->setPageLayout( pgLayout );
        gui->getPaperWidget()->frameSizeChanged( pgLayout );
    }
}

/*===============================================================*/
void KWordView::formatFrameSet()
{
    if ( m_pKWordDoc->getFirstSelectedFrame() )
    {
        gui->getPaperWidget()->femProps();
    } else {
        KMessageBox::sorry( this,
                            i18n("Sorry, you have to select a frame first."),
                            i18n("Format Frameset"));
    }
}

/*===============================================================*/
void KWordView::extraSpelling()
{
    if (kspell) return; // Already in progress
    currParag = 0L;
    currFrameSetNum = -1;
    kspell = new KSpell( this, i18n( "Spell Checking" ), this, SLOT( spellCheckerReady() ) );

    QObject::connect( kspell, SIGNAL( death() ),
                      this, SLOT( spellCheckerFinished() ) );
    QObject::connect( kspell, SIGNAL( misspelling( QString, QStringList*, unsigned ) ),
                      this, SLOT( spellCheckerMisspelling( QString, QStringList*, unsigned ) ) );
    QObject::connect( kspell, SIGNAL( corrected( QString, QString, unsigned ) ),
                      this, SLOT( spellCheckerCorrected( QString, QString, unsigned ) ) );
    QObject::connect( kspell, SIGNAL( done( const QString & ) ), this, SLOT( spellCheckerDone( const QString & ) ) );
}

/*===============================================================*/
void KWordView::extraAutoFormat()
{
    KWAutoFormatDia dia( this, "", m_pKWordDoc, gui->getPaperWidget() );
    dia.show();
}

/*===============================================================*/
void KWordView::extraStylist()
{
    if ( styleManager ) {
        styleManager->close();
        delete styleManager;
        styleManager = 0;
    }
    styleManager = new KWStyleManager( this, m_pKWordDoc, fontList );
    connect( styleManager, SIGNAL( okClicked() ), this, SLOT( styleManagerOk() ) );
    styleManager->setCaption( i18n( "KWord - Stylist" ) );
    styleManager->show();
}

/*===============================================================*/
void KWordView::extraCreateTemplate()
{
    QPixmap pix( 45, 60 );
    pix.fill( Qt::white );

    QString file = "/tmp/kwt.kwt";
    m_pKWordDoc->saveNativeFormat( file );

    KoTemplateCreateDia::createTemplate( "kword_template", KWordFactory::global(),
                                         file, pix, this );
    system( QString( "rm %1" ).arg( file ).latin1() );
    KWordFactory::global()->dirs()->addResourceType("kword_template",
                                                    KStandardDirs::kde_default( "data" ) +
                                                    "kword/templates/");
}

/*===============================================================*/
void KWordView::extraOptions()
{
}

/*===============================================================*/
void KWordView::toolsEdit()
{
    if ( !( (KToggleAction*)actionToolsEdit )->isChecked() )
        return;
    gui->getPaperWidget()->mmEdit();
}

/*===============================================================*/
void KWordView::toolsEditFrame()
{
    if ( !( (KToggleAction*)actionToolsEditFrames )->isChecked() )
        return;
    gui->getPaperWidget()->mmEditFrame();
}

/*===============================================================*/
void KWordView::toolsCreateText()
{
    if ( !( (KToggleAction*)actionToolsCreateText )->isChecked() )
        return;
    gui->getPaperWidget()->mmCreateText();
}

/*===============================================================*/
void KWordView::toolsCreatePix()
{
    if ( !( (KToggleAction*)actionToolsCreatePix )->isChecked() )
        return;
    gui->getPaperWidget()->mmEdit();

    QString file;
#ifdef USE_QFD
    QFileDialog fd( QString::null, i18n( "Pictures (*.gif *.png *.jpg *.jpeg *.xpm *.bmp)\nAll files (*)" ), 0, 0, TRUE );
    fd.setPreviewMode( FALSE, TRUE );
    fd.setContentsPreviewWidget( new Preview( &fd ) );
    fd.setViewMode( QFileDialog::ListView | QFileDialog::PreviewContents );
    if ( fd.exec() == QDialog::Accepted )
        file = fd.selectedFile();
#else
    KFileDialog fd( QString::null, KImageIO::pattern(KImageIO::Writing), 0, 0, TRUE );
    //fd.setPreviewMode( FALSE, TRUE );
    fd.setCaption(i18n("Create Picture"));
    fd.setPreviewWidget( new Preview( &fd ) );
    //fd.setViewMode( QFileDialog::ListView | QFileDialog::PreviewContents );
    KURL url;
    if ( fd.exec() == QDialog::Accepted )
        url = fd.selectedURL();

    if( url.isEmpty() )
      return;

    if( !url.isLocalFile() )
    {
      KMessageBox::sorry( 0L, i18n( "Only local files supported yet." ) );
      return;
    }

    file = url.path();
#endif

    if ( !file.isEmpty() ) {
        gui->getPaperWidget()->mmCreatePix();
        gui->getPaperWidget()->setPixmapFilename( file );
    } else
        gui->getPaperWidget()->mmEdit();
}

/*===============================================================*/
void KWordView::toolsClipart()
{
    if ( !( (KToggleAction*)actionToolsCreateClip )->isChecked() )
        return;
    gui->getPaperWidget()->mmClipart();
}

/*===============================================================*/
void KWordView::toolsTable()
{
    if ( !( (KToggleAction*)actionToolsCreateTable )->isChecked() )
        return;
    if ( tableDia ) {
        tableDia->close();
        delete tableDia;
        tableDia = 0L;
    }

    tableDia = new KWTableDia( this, "", gui->getPaperWidget(), m_pKWordDoc,
                               gui->getPaperWidget()->tableRows(),
                               gui->getPaperWidget()->tableCols(),
                               gui->getPaperWidget()->tableWidthMode(),
                               gui->getPaperWidget()->tableHeightMode() );
    tableDia->setCaption( i18n( "KWord - Insert Table" ) );
    tableDia->show();
}

/*===============================================================*/
void KWordView::toolsKSpreadTable()
{
    if ( !( (KToggleAction*)actionToolsCreateKSpreadTable )->isChecked() )
        return;
    gui->getPaperWidget()->mmKSpreadTable();

    KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( "application/x-kspread" );
    if (entry.isEmpty())
    {
      KMessageBox::sorry( 0, i18n( "Sorry, no table component registered" ) );
      gui->getPaperWidget()->mmEdit();
    }
    else
        gui->getPaperWidget()->setPartEntry( entry );
}

/*===============================================================*/
void KWordView::toolsFormula()
{
    if ( !( (KToggleAction*)actionToolsCreateFormula )->isChecked() )
        return;
    gui->getPaperWidget()->mmFormula();
}

/*===============================================================*/
void KWordView::toolsPart()
{
    if ( !( (KToggleAction*)actionToolsCreatePart )->isChecked() )
        return;
    gui->getPaperWidget()->mmEdit();

    KoDocumentEntry pe = KoPartSelectDia::selectPart( this );
    if ( pe.isEmpty() )
        return;

    gui->getPaperWidget()->mmPart();
    gui->getPaperWidget()->setPartEntry( pe );
}

/*===============================================================*/
void KWordView::tableInsertRow()
{
    gui->getPaperWidget()->mmEdit();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before inserting a new row." ),
                            i18n( "Insert Row" ) );
    } else {
        KWInsertDia dia( this, "", grpMgr, m_pKWordDoc, KWInsertDia::ROW, gui->getPaperWidget() );
        dia.setCaption( i18n( "Insert Row" ) );
        dia.show();
    }
}

/*===============================================================*/
void KWordView::tableInsertCol()
{
    gui->getPaperWidget()->mmEdit();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before inserting a new column." ),
                            i18n( "Insert Column" ) );
    } else {
        if ( grpMgr->getBoundingRect().right() + 62 > static_cast<int>( m_pKWordDoc->getPTPaperWidth() ) )
        {
            KMessageBox::sorry( this,
                            i18n( "There is not enough space at the right of the table\n"
                                  "to insert a new column." ),
                            i18n( "Insert Column" ) );
        }
        else
        {
            KWInsertDia dia( this, "", grpMgr, m_pKWordDoc, KWInsertDia::COL, gui->getPaperWidget() );
            dia.setCaption( i18n( "Insert Column" ) );
            dia.show();
        }
    }
}

/*===============================================================*/
void KWordView::tableDeleteRow()
{
    gui->getPaperWidget()->mmEdit();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before deleting a row." ),
                            i18n( "Delete Row" ) );
    } else {
        if ( grpMgr->getRows() == 1 )
        {
            int result;
            result = KMessageBox::warningContinueCancel(this,
                        i18n("The table has only one row.\n"
                             "Deleting this row will delete the table.\n\n"
                             "Do you want to delete the table?"),
                        i18n("Delete Row"),
                        i18n("&Delete"));
            if (result == KMessageBox::Continue)
            {
                gui->getPaperWidget()->deleteTable( grpMgr );
            }
        } else {
            KWDeleteDia dia( this, "", grpMgr, m_pKWordDoc, KWDeleteDia::ROW, gui->getPaperWidget() );
            dia.setCaption( i18n( "Delete Row" ) );
            dia.show();
        }
    }
}

/*===============================================================*/
void KWordView::tableDeleteCol()
{
    gui->getPaperWidget()->mmEdit();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before deleting a column." ),
                            i18n( "Delete Column" ) );
    } else {
        if ( grpMgr->getCols() == 1 )
        {
            int result;
            result = KMessageBox::warningContinueCancel(this,
                        i18n("The table has only one column.\n"
                             "Deleting this column will delete the table.\n\n"
                             "Do you want to delete the table?"),
                        i18n("Delete Column"),
                        i18n("&Delete"));
            if (result == KMessageBox::Continue)
            {
                gui->getPaperWidget()->deleteTable( grpMgr );
            }
        } else {
            KWDeleteDia dia( this, "", grpMgr, m_pKWordDoc, KWDeleteDia::COL, gui->getPaperWidget() );
            dia.setCaption( i18n( "Delete Column" ) );
            dia.show();
        }
    }
}

/*===============================================================*/
void KWordView::tableJoinCells()
{
    gui->getPaperWidget()->mmEditFrame();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getCurrentTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before joining cells." ),
                            i18n( "Join Cells" ) );
    } else {
        QPainter painter;
        painter.begin( gui->getPaperWidget() );
        if ( !grpMgr->joinCells() )
        {
            KMessageBox::sorry( this,
                                i18n( "You have to select some cells which are next to each other\n"
                                      "and are not already joined." ),
                                i18n( "Join Cells" ) );
        }
        painter.end();
        QRect r = grpMgr->getBoundingRect();
        r = QRect( r.x() - gui->getPaperWidget()->contentsX(),
                   r.y() - gui->getPaperWidget()->contentsY(),
                   r.width(), r.height() );
        gui->getPaperWidget()->repaintScreen( r, TRUE );
    }
}

/*===============================================================*/
void KWordView::tableSplitCells()
{
    gui->getPaperWidget()->mmEditFrame();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getCurrentTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before splitting cells." ),
                            i18n( "Split Cells" ) );
    } else {
        QPainter painter;
        painter.begin( gui->getPaperWidget() );
        if ( !grpMgr->splitCell() )
        {
            KMessageBox::sorry( this,
                                i18n("You have to select a joined cell."),
                                i18n("Split Cells") );
        }
        painter.end();
        QRect r = grpMgr->getBoundingRect();
        r = QRect( r.x() - gui->getPaperWidget()->contentsX(),
                   r.y() - gui->getPaperWidget()->contentsY(),
                   r.width(), r.height() );
        gui->getPaperWidget()->repaintScreen( r, TRUE );
    }
}

/*===============================================================*/
void KWordView::tableUngroupTable()
{
    gui->getPaperWidget()->mmEdit();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before ungrouping a table." ),
                            i18n( "Ungroup Table" ) );
    } else {
        int result;
        result = KMessageBox::warningContinueCancel(this,
                        i18n("Ungrouping a table can not be undone.\n"
                             "Are you sure you want to ungroup the table?"),
                        i18n("Ungroup Table"), i18n("&Ungroup"));
        if (result == KMessageBox::Continue)
        {
            QRect r = grpMgr->getBoundingRect();
            grpMgr->ungroup();
            r = QRect( r.x() - gui->getPaperWidget()->contentsX(),
                       r.y() - gui->getPaperWidget()->contentsY(),
                       r.width(), r.height() );
            gui->getPaperWidget()->repaintScreen( r, TRUE );
            m_pKWordDoc->delGroupManager(grpMgr);
        }
    }
}

/*===============================================================*/
void KWordView::tableDelete()
{
    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table \n"
                                  "or select it to delete it!" ),
                            i18n( "Delete Table" ) );
    }
    else
    {
        gui->getPaperWidget()->deleteTable( grpMgr );
    }
}

/*====================== text style selected  ===================*/
void KWordView::textStyleSelected( const QString &_style )
{
    QString style = _style;
    if ( gui )
        gui->getPaperWidget()->applyStyle( style );
    format = m_pKWordDoc->findParagLayout( style )->getFormat();
    if ( gui ) {
        gui->getPaperWidget()->formatChanged( format, FALSE );
        gui->getPaperWidget()->setFocus();
    }
    updateStyle( style, FALSE );
}

/*======================= text size selected  ===================*/
void KWordView::textSizeSelected( const QString &_size)
{
    QString size = _size;
    tbFont.setPointSize( size.toInt() );
    format.setPTFontSize( size.toInt() );
    if ( gui ) {
        gui->getPaperWidget()->formatChanged( format, TRUE, KWFormat::FontSize );
        gui->getPaperWidget()->setFocus();
    }
}

/*======================= text font selected  ===================*/
void KWordView::textFontSelected( const QString &_font )
{
    QString font = _font;
    tbFont.setFamily( font );
    format.setUserFont( m_pKWordDoc->findUserFont( font ) );
    if ( gui ) {
        gui->getPaperWidget()->formatChanged( format, TRUE, KWFormat::FontFamily );
        gui->getPaperWidget()->setFocus();
    }
}

/*========================= text bold ===========================*/
void KWordView::textBold()
{
    tbFont.setBold( !tbFont.bold() );
    format.setWeight( tbFont.bold() ? QFont::Bold : QFont::Normal );
    if ( gui )
        gui->getPaperWidget()->formatChanged( format, TRUE, KWFormat::Weight );
}

/*========================== text italic ========================*/
void KWordView::textItalic()
{
    tbFont.setItalic( !tbFont.italic() );
    format.setItalic( tbFont.italic() ? 1 : 0 );
    if ( gui )
        gui->getPaperWidget()->formatChanged( format, TRUE, KWFormat::Italic );
}

/*======================== text underline =======================*/
void KWordView::textUnderline()
{
    tbFont.setUnderline( !tbFont.underline() );
    format.setUnderline( tbFont.underline() ? 1 : 0 );
    if ( gui )
        gui->getPaperWidget()->formatChanged( format, TRUE, KWFormat::Underline );
}

/*=========================== text color ========================*/
void KWordView::textColor()
{
    if ( KColorDialog::getColor( tbColor ) ) {
        ( (KColorAction*)actionFormatColor )->blockSignals( TRUE );
        ( (KColorAction*)actionFormatColor )->setColor( tbColor );
        ( (KColorAction*)actionFormatColor )->blockSignals( FALSE );
        format.setColor( tbColor );
        if ( gui )
            gui->getPaperWidget()->formatChanged( format, TRUE, KWFormat::Color );
    }
}

/*======================= text align left =======================*/
void KWordView::textAlignLeft()
{
    if ( !( (KToggleAction*)actionFormatAlignLeft )->isChecked() )
        return;
    flow = KWParagLayout::LEFT;
    if ( gui )
        gui->getPaperWidget()->setFlow( KWParagLayout::LEFT );
}

/*======================= text align center =====================*/
void KWordView::textAlignCenter()
{
    if ( !( (KToggleAction*)actionFormatAlignCenter )->isChecked() )
        return;
    flow = KWParagLayout::CENTER;
    if ( gui )
        gui->getPaperWidget()->setFlow( KWParagLayout::CENTER );
}

/*======================= text align right ======================*/
void KWordView::textAlignRight()
{
    if ( !( (KToggleAction*)actionFormatAlignRight )->isChecked() )
        return;
    flow = KWParagLayout::RIGHT;
    if ( gui )
        gui->getPaperWidget()->setFlow( KWParagLayout::RIGHT );
}

/*======================= text align block ======================*/
void KWordView::textAlignBlock()
{
    if ( !( (KToggleAction*)actionFormatAlignBlock )->isChecked() )
        return;
    flow = KWParagLayout::BLOCK;
    if ( gui )
        gui->getPaperWidget()->setFlow( KWParagLayout::BLOCK );
}

/*===============================================================*/
void KWordView::textLineSpacing( const QString &spc)
{
    KWUnit u;
    u.setPT( spc.toInt() );
    if ( gui )
        gui->getPaperWidget()->setLineSpacing( u );
}

/*====================== enumerated list ========================*/
void KWordView::textEnumList()
{
    bool b = ( (KToggleAction*)actionFormatEnumList )->isChecked();
    if ( b ) {
        if ( gui )
            gui->getPaperWidget()->setEnumList();
    } else {
        if ( gui )
            gui->getPaperWidget()->setNormalText();
    }
}

/*====================== unsorted list ==========================*/
void KWordView::textUnsortList()
{
    bool b = ( (KToggleAction*)actionFormatUnsortList )->isChecked();
    if ( b ) {
        if ( gui )
            gui->getPaperWidget()->setBulletList();
    } else {
        if ( gui )
            gui->getPaperWidget()->setNormalText();
    }
}

/*===============================================================*/
void KWordView::textSuperScript()
{
    bool b = ( (KToggleAction*)actionFormatSuper )->isChecked();
    if ( b )
        vertAlign = KWFormat::VA_SUPER;
    else
        vertAlign = KWFormat::VA_NORMAL;
    format.setVertAlign( vertAlign );
    if ( gui )
        gui->getPaperWidget()->formatChanged( format, TRUE, KWFormat::Vertalign );
}

/*===============================================================*/
void KWordView::textSubScript()
{
    bool b = ( (KToggleAction*)actionFormatSub )->isChecked();
    if ( b )
        vertAlign = KWFormat::VA_SUB;
    else
        vertAlign = KWFormat::VA_NORMAL;
    format.setVertAlign( vertAlign );
    if ( gui )
        gui->getPaperWidget()->formatChanged( format, TRUE, KWFormat::Vertalign );
}

/*===============================================================*/
void KWordView::textBorderLeft()
{
    bool b = ( (KToggleAction*)actionFormatBrdLeft )->isChecked();
    if ( b )
        left = tmpBrd;
    else
        left.ptWidth = 0;

    if ( gui )
        gui->getPaperWidget()->setParagLeftBorder( left );
}

/*===============================================================*/
void KWordView::textBorderRight()
{
    bool b = ( (KToggleAction*)actionFormatBrdRight )->isChecked();
    if ( b )
        right = tmpBrd;
    else
        right.ptWidth = 0;

    if ( gui )
        gui->getPaperWidget()->setParagRightBorder( right );
}

/*===============================================================*/
void KWordView::textBorderTop()
{
    bool b = ( (KToggleAction*)actionFormatBrdTop )->isChecked();
    if ( b )
        top = tmpBrd;
    else
        top.ptWidth = 0;

    if ( gui )
        gui->getPaperWidget()->setParagTopBorder( top );
}

/*===============================================================*/
void KWordView::textBorderBottom()
{
    bool b = ( (KToggleAction*)actionFormatBrdBottom )->isChecked();
    if ( b )
        bottom = tmpBrd;
    else
        bottom.ptWidth = 0;

    if ( gui )
        gui->getPaperWidget()->setParagBottomBorder( bottom );
}

/*================================================================*/
void KWordView::textBorderColor()
{
  tmpBrd.color=( (KColorAction*)actionFormatBrdColor )->color();
}

/*================================================================*/
void KWordView::textBorderWidth( const QString &width )
{
    tmpBrd.ptWidth = width.toInt();
}

/*================================================================*/
void KWordView::textBorderStyle( const QString &style )
{
    QString stl = style;

    if ( stl == i18n( "solid line" ) )
        tmpBrd.style = KWParagLayout::SOLID;
    else if ( stl == i18n( "dash line ( ---- )" ) )
        tmpBrd.style = KWParagLayout::DASH;
    else if ( stl == i18n( "dot line ( **** )" ) )
        tmpBrd.style = KWParagLayout::DOT;
    else if ( stl == i18n( "dash dot line ( -*-* )" ) )
        tmpBrd.style = KWParagLayout::DASH_DOT;
    else if ( stl == i18n( "dash dot dot line ( -**- )" ) )
        tmpBrd.style = KWParagLayout::DASH_DOT_DOT;
}

/*================================================================*/
void KWordView::frameBorderLeft()
{
    bool b = ( (KToggleAction*)actionFrameBrdLeft )->isChecked();
    if ( gui )
        gui->getPaperWidget()->setLeftFrameBorder( frmBrd, b );
}

/*================================================================*/
void KWordView::frameBorderRight()
{
    bool b = ( (KToggleAction*)actionFrameBrdRight )->isChecked();
    if ( gui )
        gui->getPaperWidget()->setRightFrameBorder( frmBrd, b );
}

/*================================================================*/
void KWordView::frameBorderTop()
{
    bool b = ( (KToggleAction*)actionFrameBrdTop )->isChecked();
    if ( gui )
        gui->getPaperWidget()->setTopFrameBorder( frmBrd, b );
}

/*================================================================*/
void KWordView::frameBorderBottom()
{
    bool b = ( (KToggleAction*)actionFrameBrdBottom )->isChecked();
    if ( gui )
        gui->getPaperWidget()->setBottomFrameBorder( frmBrd, b );
}

/*================================================================*/
void KWordView::frameBorderColor()
{

        frmBrd.color=( (KColorAction*)actionFrameBrdColor )->color();
        if ( gui )
            gui->getPaperWidget()->setFrameBorderColor( frmBrd.color );
}

/*================================================================*/
void KWordView::frameBorderWidth( const QString &width )
{
    frmBrd.ptWidth = width.toInt();
}

/*================================================================*/
void KWordView::frameBorderStyle( const QString &style )
{
    QString stl = style;

    if ( stl == i18n( "solid line" ) )
        frmBrd.style = KWParagLayout::SOLID;
    else if ( stl == i18n( "dash line ( ---- )" ) )
        frmBrd.style = KWParagLayout::DASH;
    else if ( stl == i18n( "dot line ( **** )" ) )
        frmBrd.style = KWParagLayout::DOT;
    else if ( stl == i18n( "dash dot line ( -*-* )" ) )
        frmBrd.style = KWParagLayout::DASH_DOT;
    else if ( stl == i18n( "dash dot dot line ( -**- )" ) )
        frmBrd.style = KWParagLayout::DASH_DOT_DOT;
}

/*================================================================*/
void KWordView::frameBackColor()
{

        backColor=( (KSelectColorAction*)actionFrameBackColor )->color();
        if ( gui )
            gui->getPaperWidget()->setFrameBackgroundColor( backColor );
}

/*================================================================*/
void KWordView::formulaPower()
{
    gui->getPaperWidget()->insertFormulaChar( Box::POWER );
}

/*================================================================*/
void KWordView::formulaSubscript()
{
    gui->getPaperWidget()->insertFormulaChar( Box::SUB );
}

/*================================================================*/
void KWordView::formulaParentheses()
{
    gui->getPaperWidget()->insertFormulaChar( Box::PAREN );
}

/*================================================================*/
void KWordView::formulaAbsValue()
{
    gui->getPaperWidget()->insertFormulaChar( Box::ABS );
}

/*================================================================*/
void KWordView::formulaBrackets()
{
    gui->getPaperWidget()->insertFormulaChar( Box::BRACKET );
}

/*================================================================*/
void KWordView::formulaFraction()
{
    gui->getPaperWidget()->insertFormulaChar( Box::DIVIDE );
}

/*================================================================*/
void KWordView::formulaRoot()
{
    gui->getPaperWidget()->insertFormulaChar( Box::SQRT );
}

/*================================================================*/
void KWordView::formulaIntegral()
{
    gui->getPaperWidget()->insertFormulaChar( Box::INTEGRAL );
}

/*================================================================*/
void KWordView::formulaMatrix()
{
    gui->getPaperWidget()->insertFormulaChar( Box::MATRIX );
}

/*================================================================*/
void KWordView::formulaLeftSuper()
{
    gui->getPaperWidget()->insertFormulaChar( Box::LSUP );
}

/*================================================================*/
void KWordView::formulaLeftSub()
{
    gui->getPaperWidget()->insertFormulaChar( Box::LSUB );
}

/*================================================================*/
void KWordView::formulaProduct()
{
    gui->getPaperWidget()->insertFormulaChar( Box::PRODUCT );
}

/*================================================================*/
void KWordView::formulaSum()
{
    gui->getPaperWidget()->insertFormulaChar( Box::SUM );
}

/*================================================================*/
void KWordView::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    if ( gui ) gui->resize( width(), height() );
}

/*================================================================*/
void KWordView::keyPressEvent( QKeyEvent *e )
{
    QApplication::sendEvent( gui, e );
}

/*================================================================*/
void KWordView::keyReleaseEvent( QKeyEvent *e )
{
    QApplication::sendEvent( gui, e );
}

/*================================================================*/
void KWordView::mousePressEvent( QMouseEvent *e )
{
    if ( gui )
        QApplication::sendEvent( gui->getPaperWidget(), e );
}

/*================================================================*/
void KWordView::mouseMoveEvent( QMouseEvent *e )
{
    if ( gui )
        QApplication::sendEvent( gui->getPaperWidget(), e );
}

/*================================================================*/
void KWordView::mouseReleaseEvent( QMouseEvent *e )
{
    if ( gui )
        QApplication::sendEvent( gui->getPaperWidget(), e );
}

/*================================================================*/
void KWordView::focusInEvent( QFocusEvent *e )
{
    if ( gui )
        QApplication::sendEvent( gui->getPaperWidget(), e );
}

/*================================================================*/
void KWordView::dragEnterEvent( QDragEnterEvent *e )
{
    QApplication::sendEvent( gui, e );
}

/*================================================================*/
void KWordView::dragMoveEvent( QDragMoveEvent *e )
{
    QApplication::sendEvent( gui, e );
}

/*================================================================*/
void KWordView::dragLeaveEvent( QDragLeaveEvent *e )
{
    QApplication::sendEvent( gui, e );
}

/*================================================================*/
void KWordView::dropEvent( QDropEvent *e )
{
    QApplication::sendEvent( gui, e );
}

/*================================================================*/
void KWordView::setParagBorderValues()
{
    ( (KSelectAction*)actionFormatBrdWidth )->blockSignals( TRUE );
    ( (KSelectAction*)actionFormatBrdWidth )->setCurrentItem( tmpBrd.ptWidth - 1 );
    ( (KSelectAction*)actionFormatBrdWidth )->blockSignals( FALSE );
    ( (KSelectAction*)actionFormatBrdStyle )->blockSignals( TRUE );
    ( (KSelectAction*)actionFormatBrdStyle )->setCurrentItem( (int)tmpBrd.style );
    ( (KSelectAction*)actionFormatBrdStyle )->blockSignals( FALSE );
}

/*================================================================*/
void KWordView::slotInsertObject( KWordChild *, KWPartFrameSet * )
{
}

/*================================================================*/
void KWordView::slotUpdateChildGeometry( KWordChild */*_child*/ )
{
}

/*================================================================*/
void KWordView::paragDiaOk()
{
    gui->getPaperWidget()->setLeftIndent( paragDia->getLeftIndent() );
    gui->getPaperWidget()->setFirstLineIndent( paragDia->getFirstLineIndent() );
    gui->getPaperWidget()->setSpaceBeforeParag( paragDia->getSpaceBeforeParag() );
    gui->getPaperWidget()->setSpaceAfterParag( paragDia->getSpaceAfterParag() );
    gui->getPaperWidget()->setLineSpacing( paragDia->getLineSpacing() );

    switch ( KWUnit::unitType( m_pKWordDoc->getUnit() ) )
    {
    case U_MM:
    {
        gui->getHorzRuler()->setLeftIndent( paragDia->getLeftIndent().mm() );
        gui->getHorzRuler()->setFirstIndent( paragDia->getFirstLineIndent().mm() );
    } break;
    case U_INCH:
    {
        gui->getHorzRuler()->setLeftIndent( paragDia->getLeftIndent().inch() );
        gui->getHorzRuler()->setFirstIndent( paragDia->getFirstLineIndent().inch() );
    } break;
    case U_PT:
    {
        gui->getHorzRuler()->setLeftIndent( paragDia->getLeftIndent().pt() );
        gui->getHorzRuler()->setFirstIndent( paragDia->getFirstLineIndent().pt() );
    } break;
    }

    gui->getPaperWidget()->setFlow( paragDia->getFlow() );
    gui->getPaperWidget()->setParagLeftBorder( paragDia->getLeftBorder() );
    gui->getPaperWidget()->setParagRightBorder( paragDia->getRightBorder() );
    gui->getPaperWidget()->setParagTopBorder( paragDia->getTopBorder() );
    gui->getPaperWidget()->setParagBottomBorder( paragDia->getBottomBorder() );
    gui->getPaperWidget()->setCounter( paragDia->getCounter() );
    setFlow( paragDia->getFlow() );
    setLineSpacing( paragDia->getLineSpacing().pt() );
}

/*================================================================*/
void KWordView::styleManagerOk()
{
    m_pKWordDoc->updateAllStyles();
}

/*================================================================*/
void KWordView::newPageLayout( KoPageLayout _layout )
{
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    m_pKWordDoc->getPageLayout( pgLayout, cl, hf );

    m_pKWordDoc->setPageLayout( _layout, cl, hf );
    gui->getHorzRuler()->setPageLayout( _layout );
    gui->getVertRuler()->setPageLayout( _layout );

    gui->getPaperWidget()->frameSizeChanged( _layout );
    gui->getPaperWidget()->forceFullUpdate();
}

/*================================================================*/
void KWordView::spellCheckerReady()
{
    // #### currently only the first available textframeset is checked!!!

    currParag = 0;
    for ( unsigned int i = 0; i < m_pKWordDoc->getNumFrameSets(); i++ ) {
        KWFrameSet *frameset = m_pKWordDoc->getFrameSet( i );
        if ( frameset->getFrameType() != FT_TEXT )
            continue;
        currFrameSetNum = i;
        currParag = dynamic_cast<KWTextFrameSet*>( frameset )->getFirstParag();
        break;
    }

    if ( !currParag ) {
        kspell->cleanUp();
        return;
    }

    QString text;
    KWParag *p = currParag;
    while ( currParag ) {
        text += currParag->getKWString()->toString() + "\n";
        currParag = currParag->getNext();
    }
    text += "\n";
    currParag = p;
    lastTextPos = 0;
    kspell->check( text );
}

/*================================================================*/
void KWordView::spellCheckerMisspelling( QString , QStringList* , unsigned )
{
}

/*================================================================*/
void KWordView::spellCheckerCorrected( QString old, QString corr, unsigned )
{
    if ( !currParag )
        return;

    QString text;
    while ( currParag ) {
        text = currParag->getKWString()->toString();
        int pos = text.find( old, lastTextPos );
        if ( pos != -1 ) {
            KWFormat f( m_pKWordDoc );
            f = *( ( (KWCharFormat*)currParag->getKWString()->data()[ pos ].attrib )->getFormat() );
            currParag->getKWString()->remove( pos, old.length() );
            currParag->insertText( pos, corr );
            currParag->setFormat( pos, corr.length(), f );
            lastTextPos = pos + corr.length();
            break;
        }
        currParag = currParag->getNext();
        lastTextPos = 0;
    }
}

/*================================================================*/
void KWordView::spellCheckerDone( const QString & )
{
    gui->getPaperWidget()->recalcWholeText();
    gui->getPaperWidget()->recalcCursor( TRUE );
    kspell->cleanUp();
    spellCheckerFinished();
}

/*================================================================*/
void KWordView::spellCheckerFinished( )
{
    KSpell::spellStatus status = kspell->status();
    delete kspell;
    kspell = 0;
    if (status == KSpell::Error)
    {
        KMessageBox::sorry(this, i18n("ISpell could not be started.\n"
        "Please make sure you have ISpell properly configured and in your PATH."));
    }
    else if (status == KSpell::Crashed)
    {
        KMessageBox::sorry(this, i18n("ISpell seems to have crashed."));
        gui->getPaperWidget()->recalcWholeText();
        gui->getPaperWidget()->recalcCursor( TRUE );
    }
}


/*================================================================*/
void KWordView::searchDiaClosed()
{
    searchDia->delayedDestruct(); // This will delete the dialog.
    searchDia = 0L;
}

/*================================================================*/
void KWordView::changeUndo( QString _text, bool _enable )
{
    if ( _enable ) {
        actionEditUndo->setEnabled( TRUE );
        actionEditUndo->setText(i18n( "Undo: %1" ).arg(_text));
    } else {
        actionEditUndo->setEnabled( FALSE );
        actionEditUndo->setText( i18n( "No Undo possible" ) );
    }
}

/*================================================================*/
void KWordView::changeRedo( QString _text, bool _enable )
{
    if ( _enable ) {
        actionEditRedo->setEnabled( TRUE );
        actionEditRedo->setText(i18n( "Redo: %1" ).arg(_text));
    } else {
        actionEditRedo->setEnabled( FALSE );
        actionEditRedo->setText( i18n( "No Redo possible" ) );
    }
}

/******************************************************************/
/* Class: KWLayoutWidget                                          */
/******************************************************************/

KWLayoutWidget::KWLayoutWidget( QWidget *parent, KWordGUI *g )
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
/* Class: KWordGUI                                                */
/******************************************************************/
KWordGUI::KWordGUI( QWidget *parent, bool, KWordDocument *_doc, KWordView *_view )
    : QWidget( parent, "" )
{
    doc = _doc;
    view = _view;
    _showStruct = FALSE;

    r_horz = r_vert = 0;

    panner = new QSplitter( Qt::Horizontal, this );
    docStruct = new KWDocStruct( panner, doc, this );
    docStruct->setMinimumWidth( 0 );
    left = new KWLayoutWidget( panner, this );
    left->show();
    paperWidget = new KWPage( left, doc, this );

    QValueList<int> l;
    l << 0;
    panner->setSizes( l );

    KoPageLayout layout;
    KoColumns cols;
    KoKWHeaderFooter hf;
    doc->getPageLayout( layout, cols, hf );

    tabChooser = new KoTabChooser( left, KoTabChooser::TAB_ALL );

    r_horz = new KoRuler( left, paperWidget->viewport(), Qt::Horizontal, layout,
                          KoRuler::F_INDENTS | KoRuler::F_TABS, tabChooser );
    r_vert = new KoRuler( left, paperWidget->viewport(), Qt::Vertical, layout, 0 );
    connect( r_horz, SIGNAL( newPageLayout( KoPageLayout ) ), view, SLOT( newPageLayout( KoPageLayout ) ) );
    connect( r_horz, SIGNAL( newLeftIndent( double ) ), paperWidget, SLOT( newLeftIndent( double ) ) );
    connect( r_horz, SIGNAL( newFirstIndent( double ) ), paperWidget, SLOT( newFirstIndent( double ) ) );
    connect( r_horz, SIGNAL( openPageLayoutDia() ), view, SLOT( openPageLayoutDia() ) );
    connect( r_horz, SIGNAL( unitChanged( QString ) ), this, SLOT( unitChanged( QString ) ) );
    connect( r_vert, SIGNAL( newPageLayout( KoPageLayout ) ), view, SLOT( newPageLayout( KoPageLayout ) ) );
    connect( r_vert, SIGNAL( openPageLayoutDia() ), view, SLOT( openPageLayoutDia() ) );
    connect( r_vert, SIGNAL( unitChanged( QString ) ), this, SLOT( unitChanged( QString ) ) );

    r_horz->setUnit( doc->getUnit() );
    r_vert->setUnit( doc->getUnit() );

    switch ( KWUnit::unitType( doc->getUnit() ) ) {
    case U_MM:
        r_horz->setLeftIndent( paperWidget->getLeftIndent().mm() );
        r_horz->setFirstIndent( paperWidget->getFirstLineIndent().mm() );
        break;
    case U_INCH:
        r_horz->setLeftIndent( paperWidget->getLeftIndent().inch() );
        r_horz->setFirstIndent( paperWidget->getFirstLineIndent().inch() );
        break;
    case U_PT:
        r_horz->setLeftIndent( paperWidget->getLeftIndent().pt() );
        r_horz->setFirstIndent( paperWidget->getFirstLineIndent().pt() );
        break;
    }

    r_horz->hide();
    r_vert->hide();

    paperWidget->show();
    docStruct->show();

    reorganize();

    if ( doc->getProcessingType() == KWordDocument::DTP )
        paperWidget->setRuler2Frame( 0, 0 );

    connect( r_horz, SIGNAL( tabListChanged( QList<KoTabulator>* ) ), paperWidget,
             SLOT( tabListChanged( QList<KoTabulator>* ) ) );

    paperWidget->forceFullUpdate();

    setKeyCompression( TRUE );
    setAcceptDrops( TRUE );
    setFocusPolicy( QWidget::StrongFocus );

    scrollTo( 0, 0 );
}

/*================================================================*/
void KWordGUI::showGUI( bool __show )
{
    _show = __show;
    reorganize();
}

/*================================================================*/
void KWordGUI::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    reorganize();
}

/*================================================================*/
void KWordGUI::keyPressEvent( QKeyEvent* e )
{
    QApplication::sendEvent( paperWidget, e );
}

/*================================================================*/
void KWordGUI::keyReleaseEvent( QKeyEvent* e )
{
    QApplication::sendEvent( paperWidget, e );
}

/*================================================================*/
void KWordGUI::dragEnterEvent( QDragEnterEvent *e )
{
    QApplication::sendEvent( paperWidget, e );
}

/*================================================================*/
void KWordGUI::dragMoveEvent( QDragMoveEvent *e )
{
    QApplication::sendEvent( paperWidget, e );
}

/*================================================================*/
void KWordGUI::dragLeaveEvent( QDragLeaveEvent *e )
{
    QApplication::sendEvent( paperWidget, e );
}

/*================================================================*/
void KWordGUI::dropEvent( QDropEvent *e )
{
    QApplication::sendEvent( paperWidget, e );
}

/*================================================================*/
void KWordGUI::reorganize()
{
    r_vert->show();
    r_horz->show();
    tabChooser->show();

    tabChooser->setGeometry( 0, 0, 20, 20 );

    panner->setGeometry( 0, 0, width(), height() );
    paperWidget->setGeometry( 20, 20, left->width() - 20, left->height() - 20 );
    r_horz->setGeometry( 20, 0, left->width() - 20, 20 );
    r_vert->setGeometry( 0, 20, 20, left->height() - 20 );
}

/*================================================================*/
void KWordGUI::unitChanged( QString u )
{
    doc->setUnit( u );
    doc->setUnitToAll();
}

/*================================================================*/
void KWordGUI::setDocument( KWordDocument *_doc )
{
    doc = _doc;
    paperWidget->setDocument( doc );
}

/*================================================================*/
void KWordGUI::scrollTo( int _x, int _y )
{
    paperWidget->setContentsPos( _x, _y );
}

/*================================================================*/
bool KWordView::doubleClickActivation() const
{
    return TRUE;
}

/*================================================================*/
QWidget* KWordView::canvas()
{
    return gui->getPaperWidget()->viewport();
}

/*================================================================*/
int KWordView::canvasXOffset() const
{
    return gui->getPaperWidget()->contentsX();
}
/*================================================================*/
int KWordView::canvasYOffset() const
{
    return gui->getPaperWidget()->contentsY();
}
/*================================================================*/
void KWordView::canvasAddChild( KoViewChild *child )
{
    gui->getPaperWidget()->addChild( child->frame() );
}

/*===============================================================*/
void KWordView::printDebug() {
    KWordDocument *doc = m_pKWordDoc ;
    kdDebug() << "----------------------------------------"<<endl;
    kdDebug() << "                 Debug info"<<endl;
    kdDebug() << "Document:" << doc <<endl;
    kdDebug() << "Type of document: (0=WP, 1=DTP) " << doc->getProcessingType() <<endl;
    kdDebug() << "size: x:" << doc->getPTLeftBorder()<< ", y:"<<doc->getPTTopBorder() << ", w:"<< doc->getPTPaperWidth() << ", h:"<<doc->getPTPaperHeight()<<endl;
    kdDebug() << "Has header: " << doc->hasHeader() << " visible"<<endl;
    kdDebug() << "Has footer: " << doc->hasFooter() << " visible"<<endl;
    kdDebug() << "units: " << doc->getUnit() <<endl;
    kdDebug() << "Legend: types 0=base, 1=txt, 2=pic, 3=part, 4=formula." <<endl;
    kdDebug() << "        info  0=body, headers: 1=first, 2=odd, 3=even footers: 4=first, 5=odd, 6=even, 7=footnote" <<endl;
    kdDebug() << "  newFrameBh  0=Reconnect, 1=NoFollowup, 2=Copy" <<endl;
    kdDebug() << "# Framesets: " << doc->getNumFrameSets() <<endl;
    for (unsigned int iFrameset = 0; iFrameset < doc->getNumFrameSets(); iFrameset++ ) {
        kdDebug() << "Frameset " << iFrameset << ": '" <<
            doc->getFrameSet(iFrameset)->getName() << "' (" << doc->getFrameSet(iFrameset) << ")" <<endl;
        kdDebug() << " |  Type:" << doc->getFrameSet(iFrameset)->getFrameType() << endl;
        kdDebug() << " |  Info:" << doc->getFrameSet(iFrameset)->getFrameInfo() << endl;
        if(doc->getFrameSet(iFrameset)->getGroupManager()) {
            kdDebug() << " |  Groupmanager:" << doc->getFrameSet(iFrameset)->getGroupManager() << endl;
            KWGroupManager::Cell *cell = doc->getFrameSet(iFrameset)->getGroupManager()->getCell(doc->getFrameSet(iFrameset));
            kdDebug() << " |  |- row :" << cell->row << endl;
            kdDebug() << " |  |- col :" << cell->col << endl;
            kdDebug() << " |  |- rows:" << cell->rows << endl;
            kdDebug() << " |  +- cols:" << cell->cols << endl;
        }
        for ( unsigned int j = 0; j < doc->getFrameSet(iFrameset)->getNumFrames(); j++ ) {
            kdDebug() << " +-- Frame " << j << " of "<< doc->getFrameSet(iFrameset)->getNumFrames() <<  endl;
                kdDebug() << "     FrameBehaviour: "<< doc->getFrameSet(iFrameset)->getFrame(j)->getFrameBehaviour() << endl;
                kdDebug() << "     NewFrameBehaviour: "<< doc->getFrameSet(iFrameset)->getFrame(j)->getNewFrameBehaviour() << endl;
            kdDebug() << "     SheetSide "<< doc->getFrameSet(iFrameset)->getFrame(j)->getSheetSide() << endl;
            if(doc->getFrameSet(iFrameset)->getFrame( j )->isSelected())
                kdDebug() << " *   Page "<< doc->getFrameSet(iFrameset)->getFrame(j)->getPageNum() << endl;
            else
                kdDebug() << "     Page "<< doc->getFrameSet(iFrameset)->getFrame(j)->getPageNum() << endl;
        }
    }
    kdDebug() << "# Images: " << doc->getImageCollection()->iterator().count() <<endl;
    QDictIterator<KWImage> it( doc->getImageCollection()->iterator() );
    while ( it.current() ) {
        kdDebug() << " + " << it.current()->getFilename() << ": "<<it.current()->refCount() <<endl;
        ++it;
    }
}

