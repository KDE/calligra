/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998	  */
/* Version: 0.0.1						  */
/* Author: Reginald Stadlbauer, Torben Weis			  */
/* E-Mail: reggie@kde.org, weis@kde.org				  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: View							  */
/******************************************************************/

#include <qprinter.h>
#include <qpainter.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qkeycode.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qclipboard.h>
#include <qdropsite.h>
#include <qscrollview.h>
#include <qsplitter.h>
#include <qaction.h>

#include "kword_view.h"
#include "kword_doc.h"
#include "kword_view.moc"
#include "kword_shell.h"
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

#include <koPartSelectDia.h>
#include <koMainWindow.h>
#include <koDocument.h>
#include <koRuler.h>
#include <koTabChooser.h>
#include <koPartSelectDia.h>
#include <kformulaedit.h>

#include <kapp.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kimgio.h>
#include <qrect.h>
#include <kspell.h>
#include <kcolordlg.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kaction.h>
#include <kcoloractions.h>

#include <stdlib.h>
#include <X11/Xlib.h>
#define DEBUG

// /******************************************************************/
// /* Class: KWordFrame						  */
// /******************************************************************/
// KWordFrame::KWordFrame( KWordView* _view, KWordChild* _child )
//     : KoFrame( _view )
// {
//     m_pKWordView = _view;
//     m_pKWordChild = _child;
// }

/******************************************************************/
/* Class: KWordView						  */
/******************************************************************/

/*================================================================*/
KWordView::KWordView( QWidget *_parent, const char *_name, KWordDocument* _doc )
    : ContainerView( _doc, _parent, _name ), format( _doc )
{
    m_pKWordDoc = 0L;
    m_bUnderConstruction = TRUE;
    m_bShowGUI = TRUE;
    gui = 0;
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

    QObject::connect( m_pKWordDoc, SIGNAL( sig_insertObject( KWordChild*, KWPartFrameSet* ) ),
		      this, SLOT( slotInsertObject( KWordChild*, KWPartFrameSet* ) ) );
    QObject::connect( m_pKWordDoc, SIGNAL( sig_updateChildGeometry( KWordChild* ) ),
		      this, SLOT( slotUpdateChildGeometry( KWordChild* ) ) );


    getFonts();
    setKeyCompression( TRUE );
    setAcceptDrops( TRUE );
    createGUI();
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
    ( (KToggleAction*)actionViewEndNotes )->blockSignals( TRUE );
    ( (KToggleAction*)actionViewEndNotes )->setChecked( TRUE );
    ( (KToggleAction*)actionViewEndNotes )->blockSignals( FALSE );

    ( (KColorAction*)actionFormatColor )->blockSignals( TRUE );
    ( (KColorAction*)actionFormatColor )->setColor( Qt::black );
    ( (KColorAction*)actionFormatColor )->blockSignals( FALSE );
    ( (KColorAction*)actionFormatBrdColor )->blockSignals( TRUE );
    ( (KColorAction*)actionFormatBrdColor )->setColor( Qt::black );
    ( (KColorAction*)actionFormatBrdColor )->blockSignals( FALSE );
    ( (KColorAction*)actionFrameBrdColor )->blockSignals( TRUE );
    ( (KColorAction*)actionFrameBrdColor )->setColor( Qt::black );
    ( (KColorAction*)actionFrameBrdColor )->blockSignals( FALSE );
    ( (KColorAction*)actionFrameBackColor )->blockSignals( TRUE );
    ( (KColorAction*)actionFrameBackColor )->setColor( Qt::white );
    ( (KColorAction*)actionFrameBackColor )->blockSignals( FALSE );

    showFormulaToolbar( FALSE );
    KToolBar *tb = shell()->viewToolBar( "frame_toolbar" );
    if ( tb )
	tb->hide();
}

/*================================================================*/
void KWordView::setupActions()
{
    // -------------- Edit actions

    actionEditUndo = new KAction( i18n( "No Undo possible" ), KWBarIcon( "undo" ), ALT + Key_Backspace,
				  this, SLOT( editUndo() ),
				  actionCollection(), "edit_undo" );
    actionEditRedo = new KAction( i18n( "No Redo possible" ), KWBarIcon( "redo" ), 0,
				  this, SLOT( editRedo() ),
				  actionCollection(), "edit_redo" );
    actionEditCut = new KAction( i18n( "&Cut" ), KWBarIcon( "editcut" ), CTRL + Key_X,
				 this, SLOT( editCut() ),
				 actionCollection(), "edit_cut" );
    actionEditCopy = new KAction( i18n( "C&opy" ), KWBarIcon( "editcopy" ), CTRL + Key_C,
				  this, SLOT( editCopy() ),
				  actionCollection(), "edit_copy" );
    actionEditPaste = new KAction( i18n( "&Paste" ), KWBarIcon( "editpaste" ), CTRL + Key_V,
				   this, SLOT( editPaste() ),
				   actionCollection(), "edit_paste" );
    actionEditFind = new KAction( i18n( "&Find and Replace..." ), KWBarIcon( "kwsearch" ), CTRL + Key_F,
				  this, SLOT( editFind() ),
				  actionCollection(), "edit_find" );
    actionEditSelectAll = new KAction( i18n( "&Select all" ), CTRL + Key_A,
				       this, SLOT( editSelectAll() ),
				       actionCollection(), "edit_selectall" );
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
    actionViewNewView = new KAction( i18n( "&New View" ), 0,
				     this, SLOT( newView() ),
				     actionCollection(), "view_newview" );
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

    // -------------- Insert actions
    actionInsertPicture = new KAction( i18n( "&Picture..." ), KWBarIcon( "picture" ), Key_F2,
				       this, SLOT( insertPicture() ),
				       actionCollection(), "insert_picture" );
    actionInsertClipart = new KAction( i18n( "&Clipart..." ), KWBarIcon( "clipart" ), Key_F3,
				       this, SLOT( insertClipart() ),
				       actionCollection(), "insert_clipart" );
    actionInsertSpecialChar = new KAction( i18n( "&Special Character..." ), KWBarIcon( "char" ), ALT + Key_C,
					   this, SLOT( insertSpecialChar() ),
					   actionCollection(), "insert_specialchar" );
    actionInsertFrameBreak = new KAction( i18n( "&Hard Frame Break" ), CTRL + Key_Return,
					  this, SLOT( insertFrameBreak() ),
					  actionCollection(), "insert_framebreak" );
    actionInsertFootEndNote = new KAction( i18n( "&Footnote or Endnote..." ), 0,
					   this, SLOT( insertFootNoteEndNote() ),
					   actionCollection(), "insert_footendnote" );
    actionInsertContents = new KAction( i18n( "&Table Of Contents..." ), 0,
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
    actionToolsEdit = new KToggleAction( i18n( "Edit &Text" ), KWBarIcon( "edittool" ), Key_F4,
					 this, SLOT( toolsEdit() ),
					 actionCollection(), "tools_edit" );
    ( (KToggleAction*)actionToolsEdit )->setExclusiveGroup( "tools" );
    actionToolsEditFrames = new KToggleAction( i18n( "Edit &Frames" ), KWBarIcon( "editframetool" ), Key_F5,
					       this, SLOT( toolsEditFrame() ),
					       actionCollection(), "tools_editframes" );
    ( (KToggleAction*)actionToolsEditFrames )->setExclusiveGroup( "tools" );
    actionToolsCreateText = new KToggleAction( i18n( "&Create Text Frame" ), KWBarIcon( "textframetool" ), Key_F6,
					       this, SLOT( toolsCreateText() ),
					       actionCollection(), "tools_createtext" );
    ( (KToggleAction*)actionToolsCreateText )->setExclusiveGroup( "tools" );
    actionToolsCreatePix = new KToggleAction( i18n( "&Create Picture Frame" ), KWBarIcon( "picframetool" ), Key_F7,
					      this, SLOT( toolsCreatePix() ),
					      actionCollection(), "tools_createpix" );
    ( (KToggleAction*)actionToolsCreatePix )->setExclusiveGroup( "tools" );
    actionToolsCreateClip = new KToggleAction( i18n( "&Create Clipart Frame" ), KWBarIcon( "clipart" ), Key_F8,
					       this, SLOT( toolsClipart() ),
					       actionCollection(), "tools_createclip" );
    ( (KToggleAction*)actionToolsCreateClip )->setExclusiveGroup( "tools" );
    actionToolsCreateTable = new KToggleAction( i18n( "&Create Table" ), KWBarIcon( "table" ), Key_F9,
						this, SLOT( toolsTable() ),
						actionCollection(), "tools_table" );
    ( (KToggleAction*)actionToolsCreateTable )->setExclusiveGroup( "tools" );
    actionToolsCreateKSpreadTable = new KToggleAction( i18n( "&Create KSpread Table Frame" ), KWBarIcon( "table" ), Key_F10,
						       this, SLOT( toolsKSpreadTable() ),
						       actionCollection(), "tools_kspreadtable" );
    ( (KToggleAction*)actionToolsCreateKSpreadTable )->setExclusiveGroup( "tools" );
    actionToolsCreateFormula = new KToggleAction( i18n( "&Create Formula Frame" ), KWBarIcon( "formula" ), Key_F11,
						  this, SLOT( toolsFormula() ),
						  actionCollection(), "tools_formula" );
    ( (KToggleAction*)actionToolsCreateFormula )->setExclusiveGroup( "tools" );
    actionToolsCreatePart = new KToggleAction( i18n( "&Create Part Frame" ), KWBarIcon( "parts" ), Key_F12,
					       this, SLOT( toolsPart() ),
					       actionCollection(), "tools_part" );
    ( (KToggleAction*)actionToolsCreatePart )->setExclusiveGroup( "tools" );


    // ------------------------- Format actions
    actionFormatFont = new KAction( i18n( "&Font..." ), ALT + Key_F,
				    this, SLOT( formatFont() ),
				    actionCollection(), "format_font" );
    actionFormatColor = new KColorAction( i18n( "&Color..." ), KColorAction::TextColor, ALT + Key_C,
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
    QStringList lst;
    for ( unsigned int i = 0; i < m_pKWordDoc->paragLayoutList.count(); i++ )
 	lst << m_pKWordDoc->paragLayoutList.at( i )->getName();
    styleList = lst;
    ( (KSelectAction*)actionFormatStyle )->setItems( lst );
    actionFormatBold = new KToggleAction( i18n( "&Bold" ), KWBarIcon( "bold" ), CTRL + Key_B,
					   this, SLOT( textBold() ),
					   actionCollection(), "format_bold" );
    actionFormatItalic = new KToggleAction( i18n( "&Italic" ), KWBarIcon( "italic" ), CTRL + Key_I,
					   this, SLOT( textItalic() ),
					   actionCollection(), "format_italic" );
    actionFormatUnderline = new KToggleAction( i18n( "&Underline" ), KWBarIcon( "underl" ), CTRL + Key_U,
					   this, SLOT( textUnderline() ),
					   actionCollection(), "format_underline" );
    actionFormatAlignLeft = new KToggleAction( i18n( "Align &Left" ), KWBarIcon( "alignLeft" ), ALT + Key_L,
				       this, SLOT( textAlignLeft() ),
				       actionCollection(), "format_alignleft" );
    ( (KToggleAction*)actionFormatAlignLeft )->setExclusiveGroup( "align" );
    ( (KToggleAction*)actionFormatAlignLeft )->setChecked( TRUE );
    actionFormatAlignCenter = new KToggleAction( i18n( "Align &Center" ), KWBarIcon( "alignCenter" ), ALT + Key_C,
					 this, SLOT( textAlignCenter() ),
					 actionCollection(), "format_aligncenter" );
    ( (KToggleAction*)actionFormatAlignCenter )->setExclusiveGroup( "align" );
    actionFormatAlignRight = new KToggleAction( i18n( "Align &Right" ), KWBarIcon( "alignRight" ), ALT + Key_R,
					this, SLOT( textAlignRight() ),
					actionCollection(), "format_alignright" );
    ( (KToggleAction*)actionFormatAlignRight )->setExclusiveGroup( "align" );
    actionFormatAlignBlock = new KToggleAction( i18n( "Align &Block" ), KWBarIcon( "alignBlock" ), ALT + Key_B,
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
    actionFormatEnumList = new KToggleAction( i18n( "Enumerated List" ), KWBarIcon( "enumList" ), 0,
					      this, SLOT( textEnumList() ),
					      actionCollection(), "format_enumlist" );
    ( (KToggleAction*)actionFormatEnumList )->setExclusiveGroup( "style" );
    actionFormatUnsortList = new KToggleAction( i18n( "Bullet List" ), KWBarIcon( "unsortedList" ), 0,
					      this, SLOT( textUnsortList() ),
					      actionCollection(), "format_unsortlist" );
    ( (KToggleAction*)actionFormatUnsortList )->setExclusiveGroup( "style" );
    actionFormatSuper = new KToggleAction( i18n( "Superscript" ), KWBarIcon( "super" ), 0,
					      this, SLOT( textSuperScript() ),
					      actionCollection(), "format_super" );
    ( (KToggleAction*)actionFormatSuper )->setExclusiveGroup( "valign" );
    actionFormatSub = new KToggleAction( i18n( "Subscript" ), KWBarIcon( "sub" ), 0,
					      this, SLOT( textSubScript() ),
					      actionCollection(), "format_sub" );
    ( (KToggleAction*)actionFormatSub )->setExclusiveGroup( "valign" );
    actionFormatBrdLeft = new KToggleAction( i18n( "Paragraph Border Left" ), KWBarIcon( "borderleft" ), 0,
					     this, SLOT( textBorderLeft() ),
					     actionCollection(), "format_brdleft" );
    actionFormatBrdRight = new KToggleAction( i18n( "Paragraph Border Right" ), KWBarIcon( "borderright" ), 0,
					     this, SLOT( textBorderRight() ),
					     actionCollection(), "format_brdright" );
    actionFormatBrdTop = new KToggleAction( i18n( "Paragraph Border Top" ), KWBarIcon( "bordertop" ), 0,
					     this, SLOT( textBorderTop() ),
					     actionCollection(), "format_brdtop" );
    actionFormatBrdBottom = new KToggleAction( i18n( "Paragraph Border Bottom" ), KWBarIcon( "borderbottom" ), 0,
					       this, SLOT( textBorderBottom() ),
					     actionCollection(), "format_brdbottom" );
    actionFormatBrdColor = new KColorAction( i18n( "Paragraph Border Color" ), KColorAction::FrameColor, 0,
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

    actionFrameBrdLeft = new KToggleAction( i18n( "Frame Border Left" ), KWBarIcon( "borderleft" ), 0,
					     this, SLOT( frameBorderLeft() ),
					     actionCollection(), "frame_brdleft" );
    actionFrameBrdRight = new KToggleAction( i18n( "Frame Border Right" ), KWBarIcon( "borderright" ), 0,
					     this, SLOT( frameBorderRight() ),
					     actionCollection(), "frame_brdright" );
    actionFrameBrdTop = new KToggleAction( i18n( "Frame Border Top" ), KWBarIcon( "bordertop" ), 0,
					     this, SLOT( frameBorderTop() ),
					     actionCollection(), "frame_brdtop" );
    actionFrameBrdBottom = new KToggleAction( i18n( "Frame Border Bottom" ), KWBarIcon( "borderbottom" ), 0,
					       this, SLOT( frameBorderBottom() ),
					     actionCollection(), "frame_brdbottom" );
    actionFrameBrdColor = new KColorAction( i18n( "Frame Border Color" ), KColorAction::FrameColor, 0,
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
    actionFrameBackColor = new KColorAction( i18n( "Frame Background Color" ), KColorAction::BackgroundColor, 0,
					     this, SLOT( frameBackColor() ),
					     actionCollection(), "frame_backcolor" );

    // ---------------------- formula toolbar actions

    actionFormulaPower = new KAction( i18n( "Power" ), KWBarIcon( "index2" ), 0,
				      this, SLOT( formulaPower() ),
				      actionCollection(), "formula_power" );
    actionFormulaSubscript = new KAction( i18n( "Subscript" ), KWBarIcon( "index3" ), 0,
				      this, SLOT( formulaSubscript() ),
				      actionCollection(), "formula_subscript" );
    actionFormulaParentheses = new KAction( i18n( "Parentheses" ), KWBarIcon( "bra" ), 0,
				      this, SLOT( formulaParentheses() ),
				      actionCollection(), "formula_parentheses" );
    actionFormulaAbs = new KAction( i18n( "Absolute Value" ), KWBarIcon( "abs" ), 0,
				    this, SLOT( formulaAbsValue() ),
				      actionCollection(), "formula_abs" );
    actionFormulaBrackets = new KAction( i18n( "Brackets" ), KWBarIcon( "brackets" ), 0,
				      this, SLOT( formulaBrackets() ),
				      actionCollection(), "formula_brackets" );
    actionFormulaFraction = new KAction( i18n( "Fraction" ), KWBarIcon( "frac" ), 0,
					 this, SLOT( formulaFraction() ),
				      actionCollection(), "formula_fraction" );
    actionFormulaRoot = new KAction( i18n( "Root" ), KWBarIcon( "root" ), 0,
					 this, SLOT( formulaRoot() ),
				     actionCollection(), "formula_root" );
    actionFormulaIntegral = new KAction( i18n( "Integral" ), KWBarIcon( "integral" ), 0,
					 this, SLOT( formulaIntegral() ),
				      actionCollection(), "formula_integral" );
    actionFormulaMatrix = new KAction( i18n( "Matrix" ), KWBarIcon( "matrix" ), 0,
					 this, SLOT( formulaMatrix() ),
				      actionCollection(), "formula_matrix" );
    actionFormulaLeftSuper = new KAction( i18n( "Left Superscript" ), KWBarIcon( "index0" ), 0,
					 this, SLOT( formulaLeftSuper() ),
				      actionCollection(), "formula_leftsup" );
    actionFormulaLeftSub = new KAction( i18n( "Left Subscript" ), KWBarIcon( "index1" ), 0,
					 this, SLOT( formulaLeftSub() ),
				      actionCollection(), "formula_leftsub" );

    // ---------------------- Table actions

    actionTableInsertRow = new KAction( i18n( "&Insert Row..." ), KWBarIcon( "rowin" ), 0,
			       this, SLOT( tableInsertRow() ),
			       actionCollection(), "table_insrow" );
    actionTableInsertCol = new KAction( i18n( "&Insert Column..." ), KWBarIcon( "colin" ), 0,
			       this, SLOT( tableInsertCol() ),
			       actionCollection(), "table_inscol" );
    actionTableDelRow = new KAction( i18n( "&Delete Row..." ), KWBarIcon( "rowout" ), 0,
				     this, SLOT( tableDeleteRow() ),
				     actionCollection(), "table_delrow" );
    actionTableDelCol = new KAction( i18n( "&Delete Column..." ), KWBarIcon( "colout" ), 0,
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

    actionExtraSpellCheck = new KAction( i18n( "&Spell Checking..." ), KWBarIcon( "spellcheck" ), 0,
					 this, SLOT( extraSpelling() ),
					 actionCollection(), "extra_spellcheck" );
    actionExtraAutocorrection = new KAction( i18n( "&Autocorrection..." ), 0,
					 this, SLOT( extraAutoFormat() ),
					 actionCollection(), "extra_autocorrection" );
    actionExtraStylist = new KAction( i18n( "&Stylist..." ), 0,
				      this, SLOT( extraStylist() ),
				      actionCollection(), "extra_stylist" );
    actionExtraOptions = new KAction( i18n( "&Options..." ), 0,
					 this, SLOT( extraOptions() ),
				      actionCollection(), "extra_options" );
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
void KWordView::createGUI()
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
    KToolBar *tb = shell()->viewToolBar( "formula_toolbar" );
    if ( !tb )
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
// 	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Paste, FALSE );
// 	m_vToolBarEdit->setItemEnabled( ID_EDIT_PASTE, FALSE );
//     } else {
// 	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Paste, TRUE );
// 	m_vToolBarEdit->setItemEnabled( ID_EDIT_PASTE, TRUE );
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
	qWarning( i18n( "You use the page layout SCREEN. I print it in DIN A4 LANDSCAPE!" ) );
	prt.setPageSize( QPrinter::A4 );
	makeLandscape = TRUE;
    } break;
    default: {
	warning( i18n( "The used page layout is not supported by QPrinter. I set it to DIN A4." ) );
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
	 getKWString()->data()[ gui->getPaperWidget()->getCursor()->getTextPos() - 1 ].attrib->getClassId() ==
	 ID_KWCharFootNote )
	return;

    format = _format;

    if ( _format.getUserFont()->getFontName() ) {
	( (KFontAction*)actionFormatFontFamily )->blockSignals( TRUE );
	( (KFontAction*)actionFormatFontFamily )->
	    setCurrentItem( fontList.findIndex( _format.getUserFont()->getFontName() ) );
	( (KFontAction*)actionFormatFontFamily )->blockSignals( FALSE );
    }

    if ( _format.getPTFontSize() != -1 ) {
	( (KFontSizeAction*)actionFormatFontSize )->blockSignals( TRUE );
	( (KFontSizeAction*)actionFormatFontSize )->setCurrentItem( _format.getPTFontSize() - 1 );
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

    KToolBar *tbFormat = shell()->viewToolBar( "format_toolbar" );
    KToolBar *tbFrame = shell()->viewToolBar( "frame_toolbar" );
    if ( tbFrame && tbFormat ) {
	if ( _mouseMode == MM_EDIT_FRAME ) {
	    tbFormat->hide();
	    tbFrame->show();
	} else {
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
    QObject::connect( searchDia, SIGNAL( cancelButtonPressed() ), this, SLOT( searchDiaClosed() ) );
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

/*================================================================*/
void KWordView::newView()
{
    assert( ( m_pKWordDoc != 0L ) );

    KWordShell* shell = new KWordShell;
    shell->show();
    // ################
    //shell->setDocument( m_pKWordDoc );
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
    m_pKWordDoc->setNoteType( KWFootNoteManager::FootNotes );
}

/*===============================================================*/
void KWordView::viewEndNotes()
{
    if ( !( (KToggleAction*)actionViewEndNotes )->isChecked() )
	return;
    m_pKWordDoc->setNoteType( KWFootNoteManager::EndNotes );
}

/*===============================================================*/
void KWordView::insertPicture()
{
    QString file = KFilePreviewDialog::getOpenFileName( QString::null,
							KImageIO::pattern(KImageIO::Reading), 0);

    if ( !file.isEmpty() ) m_pKWordDoc->insertPicture( file, gui->getPaperWidget() );
}

/*===============================================================*/
void KWordView::insertClipart()
{
}

/*===============================================================*/
void KWordView::insertSpecialChar()
{
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
	QMessageBox::critical( this, i18n( "Error" ), i18n( "Currently you can only insert footnotes or\n"
							    "endotes into the first frameset!" ), i18n( "OK" ) );
    else {
	KWFootNoteDia dia( 0L, "", m_pKWordDoc, gui->getPaperWidget(), start );
	dia.setCaption( i18n( "Insert Footnote/Endnote" ) );
	dia.show();
    }
}

/*===============================================================*/
void KWordView::insertContents()
{
    m_pKWordDoc->createContents();
}

/*===============================================================*/
void KWordView::formatFont()
{
}

/*===============================================================*/
void KWordView::formatParagraph()
{
    if ( paragDia ) {
	QObject::disconnect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
	paragDia->close();
	delete paragDia;
	paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_SPACING | KWParagDia::PD_FLOW |
			       KWParagDia::PD_BORDERS |
			       KWParagDia::PD_NUMBERING | KWParagDia::PD_TABS, m_pKWordDoc );
    paragDia->setCaption( i18n( "KWord - Paragraph settings" ) );
    QObject::connect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
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
}

/*===============================================================*/
void KWordView::formatPage()
{
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter kwhf;
    m_pKWordDoc->getPageLayout( pgLayout, cl, kwhf );

    KoHeadFoot hf;
    int flags = FORMAT_AND_BORDERS | KW_HEADER_AND_FOOTER | USE_NEW_STUFF | DISABLE_UNIT;
    if ( m_pKWordDoc->getProcessingType() == KWordDocument::WP )
	flags = flags | COLUMNS;
    else
	flags = flags | DISABLE_BORDERS;

    if ( KoPageLayoutDia::pageLayout( pgLayout, hf, cl, kwhf, flags ) )
    {
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
	gui->getPaperWidget()->femProps();
    else
	QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to select at least one frame!" ), i18n( "OK" ) );
}

/*===============================================================*/
void KWordView::extraSpelling()
{
    currParag = 0L;
    currFrameSetNum = -1;
    kspell = new KSpell( this, i18n( "Spell Checking" ), this, SLOT( spellCheckerReady() ) );
}

/*===============================================================*/
void KWordView::extraAutoFormat()
{
    KWAutoFormatDia dia( this, "", m_pKWordDoc, gui->getPaperWidget() );
    dia.setCaption( i18n( "Autocorrection" ) );
    dia.show();
}

/*===============================================================*/
void KWordView::extraStylist()
{
    if ( styleManager ) {
	QObject::disconnect( styleManager, SIGNAL( applyButtonPressed() ), this, SLOT( styleManagerOk() ) );
	styleManager->close();
	delete styleManager;
	styleManager = 0;
    }
    styleManager = new KWStyleManager( this, m_pKWordDoc, fontList );
    QObject::connect( styleManager, SIGNAL( applyButtonPressed() ), this, SLOT( styleManagerOk() ) );
    styleManager->setCaption( i18n( "KWord - Stylist" ) );
    styleManager->show();
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
    QString file = KFilePreviewDialog::getOpenFileName( QString::null,
							KImageIO::pattern(KImageIO::Reading), 0);

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

    QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( "'IDL:KSpread/DocumentFactory:1.0#KSpread' in RepoIds", 1 );
    if ( vec.isEmpty() ) {
	cout << "Got no results" << endl;
	QMessageBox::critical( this, i18n( "Error" ), i18n( "Sorry, no table component registered" ), i18n( "OK" ) );
	return;
    }

    cerr << "USING component " << vec[ 0 ].name.ascii() << endl;
    gui->getPaperWidget()->setPartEntry( vec[ 0 ] );
}

/*===============================================================*/
void KWordView::toolsFormula()
{
    if ( !( (KToggleAction*)actionToolsCreateFormula )->isChecked() )
	return;
    gui->getPaperWidget()->mmFormula();

//     QValueList<KoDocumentEntry>
// 	vec = KoDocumentEntry::query( "'IDL:KFormula/DocumentFactory:1.0#KFormula' in RepoIds", 1 );
//     if ( vec.isEmpty() )
//     {
// 	cout << "Got no results" << endl;
// 	QMessageBox::critical( this, i18n( "Error" ), i18n( "Sorry, no formula component registered" ), i18n( "OK" ) );
// 	return;
//     }
//     gui->getPaperWidget()->setPartEntry( vec[ 0 ] );
}

/*===============================================================*/
void KWordView::toolsPart()
{
    if ( !( (KToggleAction*)actionToolsCreatePart )->isChecked() )
	return;
    gui->getPaperWidget()->mmEdit();

    KoDocumentEntry pe = KoPartSelectDia::selectPart();
    if ( pe.name.isEmpty() )
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
	QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to put the cursor into a table to edit it!" ),
			       i18n( "OK" ) );
    else
    {
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
	QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to put the cursor into a table to edit it!" ),
			       i18n( "OK" ) );
    else
    {
	if ( grpMgr->getBoundingRect().right() + 62 > static_cast<int>( m_pKWordDoc->getPTPaperWidth() ) )
	    QMessageBox::critical( this, i18n( "Error" ),
				   i18n( "There is not enough space at the right of the table\nto insert a new column." ),
				   i18n( "OK" ) );
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
	QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to put the cursor into a table to edit it!" ),
			       i18n( "OK" ) );
    else {
	if ( grpMgr->getRows() == 1 )
	    QMessageBox::critical( this, i18n( "Error" ),
				   i18n( "The table has only one row. You can't delete the last one!" ), i18n( "OK" ) );
	else {
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
	QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to put the cursor into a table to edit it!" ),
			       i18n( "OK" ) );
    else {
	if ( grpMgr->getCols() == 1 )
	    QMessageBox::critical( this, i18n( "Error" ),
				   i18n( "The table has only one column. You can't delete the last one!" ), i18n( "OK" ) );
	else {
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
	QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to select some cells in a table to join them!" ),
			       i18n( "OK" ) );
    else {
	QPainter painter;
	painter.begin( gui->getPaperWidget() );
	if ( !grpMgr->joinCells() )
	    QMessageBox::critical( this, i18n( "Error" ),
				   i18n( "You have to select some cells which are next to each other\n"
					 "and are not already joined." ), i18n( "OK" ) );
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
	QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to select a cell in a table to split it!" ),
			       i18n( "OK" ) );
    else {
	QPainter painter;
	painter.begin( gui->getPaperWidget() );
	if ( !grpMgr->splitCell() )
	    QMessageBox::critical( this, i18n( "Error" ), i18n( "Currently it's only possible to split a joined cell.\n"
								"So, you have to selecte a joined cell." ), i18n( "OK" ) );
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
	QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to put the cursor into a table to edit it!" ),
			       i18n( "OK" ) );
    else {
	if ( QMessageBox::warning( this, i18n( "Warning" ), i18n( "Ungrouping a table is an irrevesible action!\n"
								  "Do you really want to do that?" ), i18n( "Yes" ),
				   i18n( "No" ) ) == 0 ) {
	    grpMgr->ungroup();
	    QRect r = grpMgr->getBoundingRect();
	    r = QRect( r.x() - gui->getPaperWidget()->contentsX(),
		       r.y() - gui->getPaperWidget()->contentsY(),
		       r.width(), r.height() );
	    gui->getPaperWidget()->repaintScreen( r, TRUE );
	}
    }
}

/*===============================================================*/
void KWordView::tableDelete()
{
    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
	QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to put the cursor into a table \n"
							    "or select it to delete it!" ), i18n( "OK" ) );
    else
	gui->getPaperWidget()->deleteTable( grpMgr );

}

/*===============================================================*/
void KWordView::helpContents()
{
}

/*===============================================================*/
void KWordView::helpAbout()
{
    QMessageBox::information( this, "KWord",
			      i18n( "KWord 0.0.1 alpha\n\n"
				    "( c ) by Torben Weis <weis@kde.org> and \n"
				    "Reginald Stadlbauer <reggie@kde.org> 1998\n\n"
				    "KWord is under GNU GPL" ),
                              i18n( "OK"));
}

/*===============================================================*/
void KWordView::helpAboutKOffice()
{
}

/*===============================================================*/
void KWordView::helpAboutKDE()
{
}

/*====================== text style selected  ===================*/
void KWordView::textStyleSelected( const QString &_style )
{
    QString style = _style;
    if ( gui )
	gui->getPaperWidget()->applyStyle( style );
    format = m_pKWordDoc->findParagLayout( style )->getFormat();
    if ( gui )
	gui->getPaperWidget()->formatChanged( format, FALSE );
    updateStyle( style, FALSE );
}

/*======================= text size selected  ===================*/
void KWordView::textSizeSelected( const QString &_size)
{
    QString size = _size;
    tbFont.setPointSize( size.toInt() );
    format.setPTFontSize( size.toInt() );
    if ( gui )
	gui->getPaperWidget()->formatChanged( format );
}

/*======================= text font selected  ===================*/
void KWordView::textFontSelected( const QString &_font )
{
    QString font = _font;
    tbFont.setFamily( font );
    format.setUserFont( m_pKWordDoc->findUserFont( font ) );
    if ( gui )
	gui->getPaperWidget()->formatChanged( format );
}

/*========================= text bold ===========================*/
void KWordView::textBold()
{
    tbFont.setBold( !tbFont.bold() );
    format.setWeight( tbFont.bold() ? QFont::Bold : QFont::Normal );
    if ( gui )
	gui->getPaperWidget()->formatChanged( format );
}

/*========================== text italic ========================*/
void KWordView::textItalic()
{
    tbFont.setItalic( !tbFont.italic() );
    format.setItalic( tbFont.italic() ? 1 : 0 );
    if ( gui )
	gui->getPaperWidget()->formatChanged( format );
}

/*======================== text underline =======================*/
void KWordView::textUnderline()
{
    tbFont.setUnderline( !tbFont.underline() );
    format.setUnderline( tbFont.underline() ? 1 : 0 );
    if ( gui )
	gui->getPaperWidget()->formatChanged( format );
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
	    gui->getPaperWidget()->formatChanged( format );
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
	gui->getPaperWidget()->formatChanged( format );
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
	gui->getPaperWidget()->formatChanged( format );
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
    if ( KColorDialog::getColor( tmpBrd.color ) ) {
	( (KColorAction*)actionFormatBrdColor )->blockSignals( TRUE );
	( (KColorAction*)actionFormatBrdColor )->setColor( tmpBrd.color );
	( (KColorAction*)actionFormatBrdColor )->blockSignals( FALSE );
    }
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
    if ( KColorDialog::getColor( frmBrd.color ) ) {
	( (KColorAction*)actionFrameBrdColor )->blockSignals( TRUE );
	( (KColorAction*)actionFrameBrdColor )->setColor( frmBrd.color );
	( (KColorAction*)actionFrameBrdColor )->blockSignals( FALSE );
    }
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
    QColor c = backColor.color();
    if ( KColorDialog::getColor( c ) ) {
	backColor.setColor( c );
	( (KColorAction*)actionFrameBackColor )->blockSignals( TRUE );
	( (KColorAction*)actionFrameBackColor )->setColor( c );
	( (KColorAction*)actionFrameBackColor )->blockSignals( FALSE );
	if ( gui )
	    gui->getPaperWidget()->setFrameBackgroundColor( backColor );
    }
}

/*================================================================*/
void KWordView::formulaPower()
{
    gui->getPaperWidget()->insertFormulaChar( POWER );
}

/*================================================================*/
void KWordView::formulaSubscript()
{
    gui->getPaperWidget()->insertFormulaChar( SUB );
}

/*================================================================*/
void KWordView::formulaParentheses()
{
    gui->getPaperWidget()->insertFormulaChar( PAREN );
}

/*================================================================*/
void KWordView::formulaAbsValue()
{
    gui->getPaperWidget()->insertFormulaChar( ABS );
}

/*================================================================*/
void KWordView::formulaBrackets()
{
    gui->getPaperWidget()->insertFormulaChar( BRACKET );
}

/*================================================================*/
void KWordView::formulaFraction()
{
    gui->getPaperWidget()->insertFormulaChar( DIVIDE );
}

/*================================================================*/
void KWordView::formulaRoot()
{
    gui->getPaperWidget()->insertFormulaChar( SQRT );
}

/*================================================================*/
void KWordView::formulaIntegral()
{
    gui->getPaperWidget()->insertFormulaChar( INTEGRAL );
}

/*================================================================*/
void KWordView::formulaMatrix()
{
    gui->getPaperWidget()->insertFormulaChar( MATRIX );
}

/*================================================================*/
void KWordView::formulaLeftSuper()
{
    gui->getPaperWidget()->insertFormulaChar( LSUP );
}

/*================================================================*/
void KWordView::formulaLeftSub()
{
    gui->getPaperWidget()->insertFormulaChar( LSUB );
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
// bool KWordView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
// {
//     if ( CORBA::is_nil( _menubar ) ) {
// 	m_vMenuEdit = 0L;
// 	m_vMenuView = 0L;
// 	m_vMenuInsert = 0L;
// 	m_vMenuFormat = 0L;
// 	m_vMenuExtra = 0L;
// 	m_vMenuHelp = 0L;
// 	return TRUE;
//     }

//     QString text;

//     // edit menu
//     text = i18n( "&Edit" ) ;
//     _menubar->insertMenu( text, m_vMenuEdit, -1, -1 );

//     OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( BarIcon( "undo" ) );
//     text = i18n( "No Undo possible" ) ;
//     m_idMenuEdit_Undo = m_vMenuEdit->insertItem6( pix, text, this, "editUndo", CTRL + Key_Z, -1, -1 );
//     m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, FALSE );

//     pix = OPUIUtils::convertPixmap( BarIcon( "redo" ) );
//     text = i18n( "No Redo possible" ) ;
//     m_idMenuEdit_Redo = m_vMenuEdit->insertItem6( pix, text, this, "editRedo", 0, -1, -1 );
//     m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, FALSE );
//     m_vMenuEdit->insertSeparator( -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "editcut" ) );
//     text = i18n( "&Cut" ) ;
//     m_idMenuEdit_Cut = m_vMenuEdit->insertItem6( pix, text, this, "editCut", CTRL + Key_X, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "editcopy" ) );
//     text = i18n( "&Copy" ) ;
//     m_idMenuEdit_Copy = m_vMenuEdit->insertItem6( pix, text, this, "editCopy", CTRL + Key_C, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "editpaste" ) );
//     text = i18n( "&Paste" ) ;
//     m_idMenuEdit_Paste = m_vMenuEdit->insertItem6( pix, text, this, "editPaste", CTRL + Key_V, -1, -1 );

//     m_vMenuEdit->insertSeparator( -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "kwsearch" ) );
//     text = i18n( "&Find and Replace..." ) ;
//     m_idMenuEdit_Find = m_vMenuEdit->insertItem6( pix, text, this, "editFind", CTRL + Key_F, -1, -1 );

//     m_vMenuEdit->insertSeparator( -1 );
//     text = i18n( "&Select all" ) ;
//     m_idMenuEdit_SelectAll = m_vMenuEdit->insertItem4( text, this, "editSelectAll", 0, -1, -1 );

//     m_vMenuEdit->insertSeparator( -1 );
//     text = i18n( "&Delete Frame" ) ;
//     m_idMenuEdit_DeleteFrame = m_vMenuEdit->insertItem4( text, this, "editDeleteFrame", 0, -1, -1 );

//     text = i18n( "&Reconnect Frame..." ) ;
//     m_idMenuEdit_ReconnectFrame = m_vMenuEdit->insertItem4( text, this, "editReconnectFrame", 0, -1, -1 );

//     m_vMenuEdit->insertSeparator( -1 );

//     text = i18n( "&Custom Variables..." ) ;
//     m_idMenuEdit_CustomVars = m_vMenuEdit->insertItem4( text, this, "editCustomVars", 0, -1, -1 );

//     m_vMenuEdit->insertSeparator( -1 );

//     text = i18n( "Serial &Letter Database..." ) ;
//     m_idMenuEdit_SerialLetterDataBase = m_vMenuEdit->insertItem4( text, this,
// 								  "editSerialLetterDataBase", 0, -1, -1 );

//     // View
//     text = i18n( "&View" ) ;
//     _menubar->insertMenu( text, m_vMenuView, -1, -1 );

//     text = i18n( "&New View" ) ;
//     m_idMenuView_NewView = m_vMenuView->insertItem4( text, this, "newView", 0, -1, -1 );
//     m_vMenuView->insertSeparator( -1 );
//     text = i18n( "&Formatting Chars" ) ;
//     m_idMenuView_FormattingChars = m_vMenuView->insertItem4( text, this, "viewFormattingChars", 0, -1, -1 );
//     text = i18n( "Frame &Borders" ) ;
//     m_idMenuView_FrameBorders = m_vMenuView->insertItem4( text, this, "viewFrameBorders", 0, -1, -1 );
//     text = i18n( "Table &Grid" ) ;
//     m_idMenuView_TableGrid = m_vMenuView->insertItem4( text, this, "viewTableGrid", 0, -1, -1 );
//     m_vMenuView->insertSeparator( -1 );
//     text = i18n( "&Header" ) ;
//     m_idMenuView_Header = m_vMenuView->insertItem4( text, this, "viewHeader", 0, -1, -1 );
//     text = i18n( "F&ooter" ) ;
//     m_idMenuView_Footer = m_vMenuView->insertItem4( text, this, "viewFooter", 0, -1, -1 );
//     m_vMenuView->insertSeparator( -1 );
//     text = i18n( "&Footnotes" ) ;
//     m_idMenuView_FootNotes = m_vMenuView->insertItem4( text, this, "viewFootNotes", 0, -1, -1 );
//     text = i18n( "&Endnotes" ) ;
//     m_idMenuView_EndNotes = m_vMenuView->insertItem4( text, this, "viewEndNotes", 0, -1, -1 );

//     m_vMenuView->setCheckable( TRUE );
//     m_vMenuView->setItemChecked( m_idMenuView_FrameBorders, TRUE );
//     m_vMenuView->setItemChecked( m_idMenuView_TableGrid, TRUE );
//     m_vMenuView->setItemChecked( m_idMenuView_Header, m_pKWordDoc->hasHeader() );
//     m_vMenuView->setItemChecked( m_idMenuView_Footer, m_pKWordDoc->hasFooter() );
//     m_vMenuView->setItemChecked( m_idMenuView_FootNotes, FALSE );
//     m_vMenuView->setItemChecked( m_idMenuView_EndNotes, TRUE );

//     // insert menu
//     text = i18n( "&Insert" ) ;
//     _menubar->insertMenu( text, m_vMenuInsert, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "picture" ) );
//     text = i18n( "&Picture..." ) ;
//     m_idMenuInsert_Picture = m_vMenuInsert->insertItem6( pix, text, this, "insertPicture", Key_F2, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "clipart" ) );
//     text = i18n( "&Clipart..." ) ;
//     m_idMenuInsert_Clipart = m_vMenuInsert->insertItem6( pix, text, this, "insertClipart", Key_F3, -1, -1 );

//     m_vMenuInsert->insertSeparator( -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "char" ) );
//     text = i18n( "&Special Character..." ) ;
//     m_idMenuInsert_SpecialChar = m_vMenuInsert->insertItem6( pix, text, this,
// 							     "insertSpecialChar", ALT + Key_C, -1, -1 );
//     m_vMenuInsert->insertSeparator( -1 );
//     text = i18n( "&Hard frame break" ) ;
//     m_idMenuInsert_FrameBreak = m_vMenuInsert->insertItem4( text, this, "insertFrameBreak", 0, -1, -1 );

//     m_vMenuInsert->insertSeparator( -1 );

//     text = i18n( "&Variable" ) ;
//     m_vMenuInsert->insertItem8( text, m_vMenuInsert_Variable, -1, -1 );

//     m_vMenuInsert->insertSeparator( -1 );

//     text = i18n( "&Footnote or Endnote..." ) ;
//     m_idMenuInsert_FootNoteEndNote = m_vMenuInsert->insertItem4( text, this, "insertFootNoteEndNote", 0, -1, -1 );

//     m_vMenuInsert->insertSeparator( -1 );

//     text = i18n( "&Table of Contents..." ) ;
//     m_idMenuInsert_Contents = m_vMenuInsert->insertItem4( text, this, "insertContents", 0, -1, -1 );

//     text = i18n( "Date ( fix )" ) ;
//     m_idMenuInsert_VariableDateFix = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableDateFix", 0, -1, -1 );
//     text = i18n( "Date ( variable )" ) ;
//     m_idMenuInsert_VariableDateVar = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableDateVar", 0, -1, -1 );
//     text = i18n( "Time ( fix )" ) ;
//     m_idMenuInsert_VariableTimeFix = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableTimeFix", 0, -1, -1 );
//     text = i18n( "Time ( variable )" ) ;
//     m_idMenuInsert_VariableTimeVar = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableTimeVar", 0, -1, -1 );
//     text = i18n( "Page Number" ) ;
//     m_idMenuInsert_VariablePageNum = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariablePageNum", 0, -1, -1 );
//     m_vMenuInsert_Variable->insertSeparator( -1 );
//     text = i18n( "Custom..." ) ;
//     m_idMenuInsert_VariableCustom = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableCustom", 0, -1, -1 );
//     m_vMenuInsert_Variable->insertSeparator( -1 );
//     text = i18n( "Serial Letter..." ) ;
//     m_idMenuInsert_VariableSerialLetter = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableSerialLetter",
// 									       0, -1, -1 );

//     // tools menu
//     text = i18n( "&Tools" ) ;
//     _menubar->insertMenu( text, m_vMenuTools, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "edittool" ) );
//     text = i18n( "&Edit Text" ) ;
//     m_idMenuTools_Edit = m_vMenuTools->insertItem6( pix, text, this, "toolsEdit", Key_F4, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "editframetool" ) );
//     text = i18n( "&Edit Frames" ) ;
//     m_idMenuTools_EditFrame = m_vMenuTools->insertItem6( pix, text, this, "toolsEditFrame", Key_F5, -1, -1 );

//     m_vMenuTools->insertSeparator( -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "textframetool" ) );
//     text = i18n( "&Create Text Frame" ) ;
//     m_idMenuTools_CreateText = m_vMenuTools->insertItem6( pix, text, this, "toolsCreateText", Key_F6, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "picframetool" ) );
//     text = i18n( "&Create Picture Frame" ) ;
//     m_idMenuTools_CreatePix = m_vMenuTools->insertItem6( pix, text, this, "toolsCreatePix", Key_F7, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "clipart" ) );
//     text = i18n( "&Create Clipart Frame" ) ;
//     m_idMenuTools_Clipart = m_vMenuTools->insertItem6( pix, text, this, "toolsClipart", Key_F8, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "table" ) );
//     text = i18n( "&Create Table Frame" ) ;
//     m_idMenuTools_Table = m_vMenuTools->insertItem6( pix, text, this, "toolsTable", Key_F9, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "table" ) );
//     text = i18n( "&Create KSpread Table Frame" ) ;
//     m_idMenuTools_KSpreadTable = m_vMenuTools->insertItem6( pix, text, this, "toolsKSpreadTable", Key_F10, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "formula" ) );
//     text = i18n( "&Create Formula Frame" ) ;
//     m_idMenuTools_Formula = m_vMenuTools->insertItem6( pix, text, this, "toolsFormula", Key_F11, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "parts" ) );
//     text = i18n( "&Create Part Frame" ) ;
//     m_idMenuTools_Part = m_vMenuTools->insertItem6( pix, text, this, "toolsPart", Key_F12, -1, -1 );

//     m_vMenuTools->setCheckable( TRUE );
//     m_vMenuTools->setItemChecked( m_idMenuTools_Edit, TRUE );

//     // format menu
//     text = i18n( "&Format" ) ;
//     _menubar->insertMenu( text, m_vMenuFormat, -1, -1 );

//     text = i18n( "&Font..." ) ;
//     m_idMenuFormat_Font = m_vMenuFormat->insertItem4( text, this, "formatFont", ALT + Key_F, -1, -1 );
//     text = i18n( "&Color..." ) ;
//     m_idMenuFormat_Color = m_vMenuFormat->insertItem4( text, this, "formatColor", ALT + Key_C, -1, -1 );
//     text = i18n( "Paragraph..." ) ;
//     m_idMenuFormat_Paragraph = m_vMenuFormat->insertItem4( text, this, "formatParagraph", ALT + Key_Q, -1, -1 );
//     text = i18n( "Frame/Frameset..." ) ;
//     m_idMenuFormat_FrameSet = m_vMenuFormat->insertItem4( text, this, "formatFrameSet", 0, -1, -1 );
//     text = i18n( "Page..." ) ;
//     m_idMenuFormat_Page = m_vMenuFormat->insertItem4( text, this, "formatPage", ALT + Key_P, -1, -1 );

//     m_vMenuFormat->insertSeparator( -1 );

//     text = i18n( "&Style..." ) ;
//     m_idMenuFormat_Style = m_vMenuFormat->insertItem4( text, this, "formatStyle", ALT + Key_A, -1, -1 );

//     // table menu
//     text = i18n( "&Table" ) ;
//     _menubar->insertMenu( text, m_vMenuTable, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "rowin" ) );
//     text = i18n( "&Insert Row..." ) ;
//     m_idMenuTable_InsertRow = m_vMenuTable->insertItem6( pix, text, this, "tableInsertRow", 0, -1, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "colin" ) );
//     text = i18n( "I&nsert Column..." ) ;
//     m_idMenuTable_InsertCol = m_vMenuTable->insertItem6( pix, text, this, "tableInsertCol", 0, -1, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "rowout" ) );
//     text = i18n( "&Delete Row..." ) ;
//     m_idMenuTable_DeleteRow = m_vMenuTable->insertItem6( pix, text, this, "tableDeleteRow", 0, -1, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "colout" ) );
//     text = i18n( "&Delete Column..." ) ;
//     m_idMenuTable_DeleteCol = m_vMenuTable->insertItem6( pix, text, this, "tableDeleteCol", 0, -1, -1 );

//     m_vMenuTable->insertSeparator( -1 );

//     text = i18n( "&Join Cells" ) ;
//     m_idMenuTable_JoinCells = m_vMenuTable->insertItem4( text, this, "tableJoinCells", 0, -1, -1 );
//     text = i18n( "&Split Cells" ) ;
//     m_idMenuTable_SplitCells = m_vMenuTable->insertItem4( text, this, "tableSplitCells", 0, -1, -1 );
//     text = i18n( "&Ungroup Table" ) ;
//     m_idMenuTable_UngroupTable = m_vMenuTable->insertItem4( text, this, "tableUngroupTable", 0, -1, -1 );

//     m_vMenuTable->insertSeparator( -1 );

//     text = i18n( "&Delete Table" ) ;
//     m_idMenuTable_Delete = m_vMenuTable->insertItem4( text, this, "tableDelete", 0, -1, -1 );

//     // extra menu
//     text = i18n( "&Extra" ) ;
//     _menubar->insertMenu( text, m_vMenuExtra, -1, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "spellcheck" ) );
//     text = i18n( "&Spell Cheking..." ) ;
//     m_idMenuExtra_Spelling = m_vMenuExtra->insertItem6( pix, text, this, "extraSpelling", ALT + Key_C, -1, -1 );
//     text = i18n( "&Autocorrection..." ) ;
//     m_idMenuExtra_AutoFormat = m_vMenuExtra->insertItem4( text, this, "extraAutoFormat", 0, -1, -1 );
//     text = i18n( "&Stylist..." ) ;
//     m_idMenuExtra_Stylist = m_vMenuExtra->insertItem4( text, this, "extraStylist", ALT + Key_S, -1, -1 );

//     m_vMenuExtra->insertSeparator( -1 );

//     text = i18n( "&Options..." ) ;
//     m_idMenuExtra_Options = m_vMenuExtra->insertItem4( text, this, "extraOptions", ALT + Key_O, -1, -1 );

//     // help menu
//     m_vMenuHelp = _menubar->helpMenu();
//     if ( CORBA::is_nil( m_vMenuHelp ) ) {
// 	_menubar->insertSeparator( -1 );
// 	text = i18n( "&Help" ) ;
// 	_menubar->setHelpMenu( _menubar->insertMenu( text, m_vMenuHelp, -1, -1 ) );
//     }
//     else
// 	m_vMenuHelp->insertSeparator( -1 );

//     text = i18n( "&Contents" ) ;
//     m_idMenuHelp_Contents = m_vMenuHelp->insertItem4( text, this, "helpContents", 0, -1, -1 );
//     /* m_rMenuBar->insertSeparator(m_idMenuHelp);
//        m_idMenuHelp_About = m_rMenuBar->insertItem( CORBA::string_dup( i18n( "&About KWord..." ) ), m_idMenuHelp,
//        this, CORBA::string_dup( "helpAbout" ) );
//        m_idMenuHelp_AboutKOffice = m_rMenuBar->insertItem( CORBA::string_dup( i18n( "About K&Office..." ) ), m_idMenuHelp,
//        this, CORBA::string_dup( "helpAboutKOffice" ) );
//        m_idMenuHelp_AboutKDE = m_rMenuBar->insertItem( CORBA::string_dup( i18n( "&About KDE..." ) ), m_idMenuHelp,
//        this, CORBA::string_dup( "helpAboutKDE" ) ); */

//     QObject::connect( kapp->clipboard(), SIGNAL( dataChanged() ), this, SLOT( clipboardDataChanged() ) );

//     return TRUE;
// }

// /*======================= setup edit toolbar ===================*/
// bool KWordView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
// {
//     if ( CORBA::is_nil( _factory ) ) {
// 	m_vToolBarEdit = 0L;
// 	m_vToolBarText = 0L;
// 	m_vToolBarInsert = 0L;
// 	return TRUE;
//     }

//     m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

//     m_vToolBarEdit->setFullWidth( FALSE );

//     // undo
//     OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( BarIcon( "undo" ) );
//     QString toolTip = i18n( "Undo" ) ;
//     m_idButtonEdit_Undo = m_vToolBarEdit->insertButton2( pix, ID_UNDO, SIGNAL( clicked() ), this, "editUndo",
// 							 TRUE, toolTip, -1 );
//     //m_vToolBarEdit->setItemEnabled( ID_UNDO, FALSE );

//     // redo
//     pix = OPUIUtils::convertPixmap( BarIcon( "redo" ) );
//     toolTip = i18n( "Redo" ) ;
//     m_idButtonEdit_Redo = m_vToolBarEdit->insertButton2( pix, ID_REDO, SIGNAL( clicked() ), this, "editRedo",
// 							 TRUE, toolTip, -1 );
//     //m_vToolBarEdit->setItemEnabled( ID_REDO, FALSE );

//     m_vToolBarEdit->insertSeparator( -1 );

//     // cut
//     pix = OPUIUtils::convertPixmap( BarIcon( "editcut" ) );
//     toolTip = i18n( "Cut" ) ;
//     m_idButtonEdit_Cut = m_vToolBarEdit->insertButton2( pix, ID_EDIT_CUT, SIGNAL( clicked() ), this,
// 							"editCut", TRUE, toolTip, -1 );

//     // copy
//     pix = OPUIUtils::convertPixmap( BarIcon( "editcopy" ) );
//     toolTip = i18n( "Copy" ) ;
//     m_idButtonEdit_Copy = m_vToolBarEdit->insertButton2( pix, ID_EDIT_COPY, SIGNAL( clicked() ), this,
// 							 "editCopy", TRUE, toolTip, -1 );

//     // paste
//     pix = OPUIUtils::convertPixmap( BarIcon( "editpaste" ) );
//     toolTip = i18n( "Paste" ) ;
//     m_idButtonEdit_Paste = m_vToolBarEdit->insertButton2( pix, ID_EDIT_PASTE, SIGNAL( clicked() ), this,
// 							  "editPaste", TRUE, toolTip, -1 );

//     m_vToolBarEdit->insertSeparator( -1 );

//     // spelling
//     pix = OPUIUtils::convertPixmap( BarIcon( "spellcheck" ) );
//     toolTip = i18n( "Spell Checking" ) ;
//     m_idButtonEdit_Spelling = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraSpelling",
// 							     TRUE, toolTip, -1 );

//     m_vToolBarEdit->insertSeparator( -1 );

//     // find
//     pix = OPUIUtils::convertPixmap( BarIcon( "kwsearch" ) );
//     toolTip = i18n( "Find & Replace" ) ;
//     m_idButtonEdit_Find = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editFind",
// 							 TRUE, toolTip, -1 );

//     m_vToolBarEdit->enable( OpenPartsUI::Show );

//     // TOOLBAR Insert
//     m_vToolBarInsert = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
//     m_vToolBarInsert->setFullWidth( FALSE );

//     // picture
//     pix = OPUIUtils::convertPixmap( BarIcon( "picture" ) );
//     toolTip = i18n( "Insert Picture" ) ;
//     m_idButtonInsert_Picture = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertPicture",
// 								TRUE, toolTip, -1 );

//     // clipart
//     pix = OPUIUtils::convertPixmap( BarIcon( "clipart" ) );
//     toolTip = i18n( "Insert Clipart" ) ;
//     m_idButtonInsert_Clipart = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertClipart",
// 								TRUE, toolTip, -1 );

//     m_vToolBarInsert->insertSeparator( -1 );

//     // special char
//     pix = OPUIUtils::convertPixmap( BarIcon( "char" ) );
//     toolTip = i18n( "Insert Special Character" ) ;
//     m_idButtonInsert_SpecialChar = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertSpecialChar",
// 								    TRUE, toolTip, -1 );

//     m_vToolBarInsert->enable( OpenPartsUI::Show );

//     // TOOLBAR table
//     m_vToolBarTable = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
//     m_vToolBarTable->setFullWidth( FALSE );

//     pix = OPUIUtils::convertPixmap( BarIcon( "rowin" ) );
//     toolTip = i18n( "Insert Row" ) ;
//     m_idButtonTable_InsertRow = m_vToolBarTable->insertButton2( pix, ID_TABLE_INSROW, SIGNAL( clicked() ), this,
// 								"tableInsertRow",
// 								TRUE, toolTip, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "colin" ) );
//     toolTip = i18n( "Insert Column" ) ;
//     m_idButtonTable_InsertCol = m_vToolBarTable->insertButton2( pix, ID_TABLE_INSCOL, SIGNAL( clicked() ), this,
// 								"tableInsertCol",
// 								TRUE, toolTip, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "rowout" ) );
//     toolTip = i18n( "Delete Row" ) ;
//     m_idButtonTable_DeleteRow = m_vToolBarTable->insertButton2( pix, ID_TABLE_DELROW, SIGNAL( clicked() ), this,
// 								"tableDeleteRow",
// 								TRUE, toolTip, -1 );

//     pix = OPUIUtils::convertPixmap( BarIcon( "colout" ) );
//     toolTip = i18n( "Delete Column" ) ;
//     m_idButtonTable_DeleteCol = m_vToolBarTable->insertButton2( pix, ID_TABLE_DELCOL, SIGNAL( clicked() ), this,
// 								"tableDeleteCol",
// 								TRUE, toolTip, -1 );

//     m_vToolBarTable->enable( OpenPartsUI::Show );

//     // TOOLBAR Tools
//     m_vToolBarTools = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
//     m_vToolBarTools->setFullWidth( FALSE );

//     // edit
//     pix = OPUIUtils::convertPixmap( BarIcon( "edittool" ) );
//     toolTip = i18n( "Edit Text Tool" ) ;
//     m_idButtonTools_Edit = m_vToolBarTools->insertButton2( pix, ID_TOOL_EDIT, SIGNAL( clicked() ), this,
// 							   "toolsEdit",
// 							   TRUE, toolTip, -1 );
//     m_vToolBarTools->setToggle( ID_TOOL_EDIT, TRUE );
//     m_vToolBarTools->setButton( ID_TOOL_EDIT, TRUE );

//     // edit frame
//     pix = OPUIUtils::convertPixmap( BarIcon( "editframetool" ) );
//     toolTip = i18n( "Edit Frames Tool" ) ;
//     m_idButtonTools_EditFrame = m_vToolBarTools->insertButton2( pix, ID_TOOL_EDIT_FRAME, SIGNAL( clicked() ),
// 								this, "toolsEditFrame",
// 								TRUE, toolTip, -1 );
//     m_vToolBarTools->setToggle( ID_TOOL_EDIT_FRAME, TRUE );

//     // create text frame
//     pix = OPUIUtils::convertPixmap( BarIcon( "textframetool" ) );
//     toolTip = i18n( "Create Text Frame" ) ;
//     m_idButtonTools_CreateText = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_TEXT, SIGNAL( clicked() ),
// 								 this, "toolsCreateText",
// 								 TRUE, toolTip, -1 );
//     m_vToolBarTools->setToggle( ID_TOOL_CREATE_TEXT, TRUE );

//     // create pix frame
//     pix = OPUIUtils::convertPixmap( BarIcon( "picframetool" ) );
//     toolTip = i18n( "Create Picture Frame" ) ;
//     m_idButtonTools_CreatePix = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_PIX, SIGNAL( clicked() ),
// 								this, "toolsCreatePix",
// 								TRUE, toolTip, -1 );
//     m_vToolBarTools->setToggle( ID_TOOL_CREATE_PIX, TRUE );

//     // create clip frame
//     pix = OPUIUtils::convertPixmap( BarIcon( "clipart" ) );
//     toolTip = i18n( "Create Clipart Frame" ) ;
//     m_idButtonTools_Clipart = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_CLIPART, SIGNAL( clicked() ),
// 							      this, "toolsClipart",
// 							      TRUE, toolTip, -1 );
//     m_vToolBarTools->setToggle( ID_TOOL_CREATE_CLIPART, TRUE );

//     // create table frame
//     pix = OPUIUtils::convertPixmap( BarIcon( "table" ) );
//     toolTip = i18n( "Create Table Frame" ) ;
//     m_idButtonTools_Table = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_TABLE, SIGNAL( clicked() ),
// 							    this, "toolsTable",
// 							    TRUE, toolTip, -1 );
//     m_vToolBarTools->setToggle( ID_TOOL_CREATE_TABLE, TRUE );

//     // create table frame
//     pix = OPUIUtils::convertPixmap( BarIcon( "table" ) );
//     toolTip = i18n( "Create KSPread Table Frame" ) ;
//     m_idButtonTools_KSpreadTable = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_KSPREAD_TABLE, SIGNAL( clicked() ),
// 								   this,
// 								   "toolsKSpreadTable", TRUE, toolTip, -1 );
//     m_vToolBarTools->setToggle( ID_TOOL_CREATE_KSPREAD_TABLE, TRUE );

//     // create formula frame
//     pix = OPUIUtils::convertPixmap( BarIcon( "formula" ) );
//     toolTip = i18n( "Create Formula Frame" ) ;
//     m_idButtonTools_Formula = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_FORMULA, SIGNAL( clicked() ),
// 							      this, "toolsFormula",
// 							      TRUE, toolTip, -1 );
//     m_vToolBarTools->setToggle( ID_TOOL_CREATE_FORMULA, TRUE );

//     // create part frame
//     pix = OPUIUtils::convertPixmap( BarIcon( "parts" ) );
//     toolTip = i18n( "Create Part Frame" ) ;
//     m_idButtonTools_Part = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_PART, SIGNAL( clicked() ),
// 							   this, "toolsPart",
// 							   TRUE, toolTip, -1 );
//     m_vToolBarTools->setToggle( ID_TOOL_CREATE_PART, TRUE );

//     m_vToolBarTools->enable( OpenPartsUI::Show );

//     // TOOLBAR Text
//     m_vToolBarText = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
//     m_vToolBarText->setFullWidth( FALSE );

//     // style combobox
//     styleList.clear();
//     for ( unsigned int i = 0; i < m_pKWordDoc->paragLayoutList.count(); i++ ) {
// 	styleList.append( m_pKWordDoc->paragLayoutList.at( i )->getName() );
//     }
//     toolTip = i18n( "Style" ) ;
//     m_idComboText_Style = m_vToolBarText->insertCombo( styleList, ID_STYLE_LIST,
// 						       FALSE, SIGNAL( activated( const QString & ) ),
// 						       this, "textStyleSelected", TRUE, toolTip,
// 						       200, -1, OpenPartsUI::AtBottom );

//     // size combobox
//     OpenPartsUI::WStrList sizelist;
//     for ( int i = 0; i <= 97; i++ ) {
//         QString buffer;
//         buffer.setNum( i+4 );
// 	sizelist.append ( buffer );
//     }
//     toolTip = i18n( "Font Size" ) ;
//     m_idComboText_FontSize = m_vToolBarText->insertCombo( sizelist, ID_FONT_SIZE, TRUE,
// 							  SIGNAL( activated( const QString & ) ),
// 							  this, "textSizeSelected", TRUE,
// 							  toolTip, 50, -1, OpenPartsUI::AtBottom );
//     m_vToolBarText->setCurrentComboItem( ID_FONT_SIZE, 8 );
//     tbFont.setPointSize( 12 );

//     // fonts combobox
//     getFonts();
//     toolTip = i18n( "Font List" ) ;
//     m_idComboText_FontList = m_vToolBarText->insertCombo( fontList, ID_FONT_LIST, TRUE,
// 							  SIGNAL( activated( const QString & ) ), this,
// 							  "textFontSelected", TRUE, toolTip,
// 							  200, -1, OpenPartsUI::AtBottom );
//     tbFont.setFamily( fontList[ 0 ] );
//     m_vToolBarText->setCurrentComboItem( ID_FONT_LIST, 0 );

//     m_vToolBarText->insertSeparator( -1 );

//     // bold
//     pix = OPUIUtils::convertPixmap( BarIcon( "bold" ) );
//     toolTip = i18n( "Bold" ) ;
//     m_idButtonText_Bold = m_vToolBarText->insertButton2( pix, ID_BOLD, SIGNAL( clicked() ),
// 							 this, "textBold", TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_BOLD, TRUE );
//     m_vToolBarText->setButton( ID_BOLD, FALSE );
//     tbFont.setBold( FALSE );

//     // italic
//     pix = OPUIUtils::convertPixmap( BarIcon( "italic" ) );
//     toolTip = i18n( "Italic" ) ;
//     m_idButtonText_Italic = m_vToolBarText->insertButton2( pix, ID_ITALIC, SIGNAL( clicked() ),
// 							   this, "textItalic", TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_ITALIC, TRUE );
//     m_vToolBarText->setButton( ID_ITALIC, FALSE );
//     tbFont.setItalic( FALSE );

//     // underline
//     pix = OPUIUtils::convertPixmap( BarIcon( "underl" ) );
//     toolTip = i18n( "Underline" ) ;
//     m_idButtonText_Underline = m_vToolBarText->insertButton2( pix, ID_UNDERLINE, SIGNAL( clicked() ),
// 							      this, "textUnderline", TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_UNDERLINE, TRUE );
//     m_vToolBarText->setButton( ID_UNDERLINE, FALSE );
//     tbFont.setUnderline( FALSE );

//     // color
//     tbColor = black;
//     pix = KOUIUtils::colorPixmap( tbColor, KOUIUtils::TXT_COLOR );
//     toolTip = i18n( "Text Color" ) ;
//     m_idButtonText_Color = m_vToolBarText->insertButton2( pix, ID_TEXT_COLOR, SIGNAL( clicked() ),
// 							  this, "textColor",
// 							  TRUE, toolTip, -1 );

//     m_vToolBarText->insertSeparator( -1 );

//     // align left
//     pix = OPUIUtils::convertPixmap( BarIcon( "alignLeft" ) );
//     toolTip = i18n( "Align Left" ) ;
//     m_idButtonText_ALeft = m_vToolBarText->insertButton2( pix, ID_ALEFT, SIGNAL( clicked() ),
// 							  this, "textAlignLeft",
// 							  TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_ALEFT, TRUE );
//     m_vToolBarText->setButton( ID_ALEFT, TRUE );

//     // align center
//     pix = OPUIUtils::convertPixmap( BarIcon( "alignCenter" ) );
//     toolTip = i18n( "Align Center" ) ;
//     m_idButtonText_ACenter = m_vToolBarText->insertButton2( pix, ID_ACENTER, SIGNAL( clicked() ),
// 							    this, "textAlignCenter",
// 							    TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_ACENTER, TRUE );
//     m_vToolBarText->setButton( ID_ACENTER, FALSE );

//     // align right
//     pix = OPUIUtils::convertPixmap( BarIcon( "alignRight" ) );
//     toolTip = i18n( "Align Right" ) ;
//     m_idButtonText_ARight = m_vToolBarText->insertButton2( pix, ID_ARIGHT, SIGNAL( clicked() ), this, "textAlignRight",
// 							   TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_ARIGHT, TRUE );
//     m_vToolBarText->setButton( ID_ARIGHT, FALSE );

//     QString tmp;
//     // align block
//     pix = OPUIUtils::convertPixmap( BarIcon( "alignBlock" ) );
//     toolTip = i18n( "Align Block" ) ;
//     m_idButtonText_ABlock = m_vToolBarText->insertButton2( pix, ID_ABLOCK, SIGNAL( clicked() ), this, "textAlignBlock",
// 							   TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_ABLOCK, TRUE );
//     m_vToolBarText->setButton( ID_ABLOCK, FALSE );

//     // line spacing
//     OpenPartsUI::WStrList spclist;
//     for( unsigned int i = 0; i <= 10; i++ ) {
//         QString buffer;
//         buffer.setNum( i );
// 	spclist.append( buffer );
//     }
//     toolTip = i18n( "Line Spacing ( in pt )" ) ;
//     m_idComboText_LineSpacing = m_vToolBarText->insertCombo( spclist, ID_LINE_SPC, FALSE,
// 							     SIGNAL( activated( const QString & ) ),
// 							     this, "textLineSpacing", TRUE, toolTip,
// 							     60, -1, OpenPartsUI::AtBottom );
//     spc = 0;
//     m_vToolBarText->insertSeparator( -1 );

//     // enum list
//     pix = OPUIUtils::convertPixmap( BarIcon( "enumList" ) );
//     toolTip = i18n( "Enumerated List" ) ;
//     m_idButtonText_EnumList = m_vToolBarText->insertButton2( pix, ID_ENUM_LIST, SIGNAL( clicked() ),
// 							     this, "textEnumList",
// 							     TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_ENUM_LIST, TRUE );
//     m_vToolBarText->setButton( ID_ENUM_LIST, FALSE );

//     // unsorted list
//     pix = OPUIUtils::convertPixmap( BarIcon( "unsortedList" ) );
//     toolTip = i18n( "Unsorted List" ) ;
//     m_idButtonText_EnumList = m_vToolBarText->insertButton2( pix, ID_USORT_LIST, SIGNAL( clicked() ),
// 							     this, "textUnsortList",
// 							     TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_USORT_LIST, TRUE );
//     m_vToolBarText->setButton( ID_USORT_LIST, FALSE );

//     m_vToolBarText->insertSeparator( -1 );

//     // superscript
//     pix = OPUIUtils::convertPixmap( BarIcon( "super" ) );
//     toolTip = i18n( "Superscript" ) ;
//     m_idButtonText_SuperScript = m_vToolBarText->insertButton2( pix, ID_SUPERSCRIPT, SIGNAL( clicked() ),
// 								this, "textSuperScript",
// 								TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_SUPERSCRIPT, TRUE );
//     m_vToolBarText->setButton( ID_SUPERSCRIPT, FALSE );

//     // subscript
//     pix = OPUIUtils::convertPixmap( BarIcon( "sub" ) );
//     toolTip = i18n( "Subscript" ) ;
//     m_idButtonText_SubScript = m_vToolBarText->insertButton2( pix, ID_SUBSCRIPT, SIGNAL( clicked() ),
// 							      this, "textSubScript",
// 							      TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_SUBSCRIPT, TRUE );
//     m_vToolBarText->setButton( ID_SUBSCRIPT, FALSE );

//     m_vToolBarText->insertSeparator( -1 );

//     // border left
//     pix = OPUIUtils::convertPixmap( BarIcon( "borderleft" ) );
//     toolTip = i18n( "Paragraph Border Left" ) ;
//     m_idButtonText_BorderLeft = m_vToolBarText->insertButton2( pix, ID_BRD_LEFT, SIGNAL( clicked() ),
// 							       this, "textBorderLeft",
// 							       TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_BRD_LEFT, TRUE );
//     m_vToolBarText->setButton( ID_BRD_LEFT, FALSE );

//     // border right
//     pix = OPUIUtils::convertPixmap( BarIcon( "borderright" ) );
//     toolTip = i18n( "Paragraph Border Right" ) ;
//     m_idButtonText_BorderRight = m_vToolBarText->insertButton2( pix, ID_BRD_RIGHT, SIGNAL( clicked() ),
// 								this, "textBorderRight",
// 								TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_BRD_RIGHT, TRUE );
//     m_vToolBarText->setButton( ID_BRD_RIGHT, FALSE );

//     // border top
//     pix = OPUIUtils::convertPixmap( BarIcon( "bordertop" ) );
//     toolTip = i18n( "Paragraph Border Top" ) ;
//     m_idButtonText_BorderTop = m_vToolBarText->insertButton2( pix, ID_BRD_TOP, SIGNAL( clicked() ),
// 							      this, "textBorderTop",
// 							      TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_BRD_TOP, TRUE );
//     m_vToolBarText->setButton( ID_BRD_TOP, FALSE );

//     // border bottom
//     pix = OPUIUtils::convertPixmap( BarIcon( "borderbottom" ) );
//     toolTip = i18n( "Paragraph Border Bottom" ) ;
//     m_idButtonText_BorderBottom = m_vToolBarText->insertButton2( pix, ID_BRD_BOTTOM, SIGNAL( clicked() ),
// 								 this, "textBorderBottom",
// 								 TRUE, toolTip, -1 );
//     m_vToolBarText->setToggle( ID_BRD_BOTTOM, TRUE );
//     m_vToolBarText->setButton( ID_BRD_BOTTOM, FALSE );

//     // border color
//     tmpBrd.color = black;
//     pix = KOUIUtils::colorPixmap( tmpBrd.color, KOUIUtils::FRAME_COLOR );
//     toolTip = i18n( "Paragraph Border Color" ) ;
//     m_idButtonText_BorderColor = m_vToolBarText->insertButton2( pix, ID_BORDER_COLOR, SIGNAL( clicked() ),
// 								this, "textBorderColor",
// 								TRUE, toolTip, -1 );

//     // border width combobox
//     OpenPartsUI::WStrList widthlist;
//     for( unsigned int i = 1; i <= 10; i++ ) {
//         QString buffer;
//         buffer.setNum( i );
// 	widthlist.append( buffer );
//     }
//     tmpBrd.ptWidth = 1;
//     toolTip = i18n( "Paragraph Border Width" ) ;
//     m_idComboText_BorderWidth = m_vToolBarText->insertCombo( widthlist, ID_BRD_WIDTH, FALSE,
// 							     SIGNAL( activated( const QString & ) ),
// 							     this, "textBorderWidth", TRUE, toolTip,
// 							     60, -1, OpenPartsUI::AtBottom );

//     // border style combobox
//     OpenPartsUI::WStrList linestylelist;
//     linestylelist.append( i18n( "solid line" ) );
//     linestylelist.append( i18n( "dash line ( ---- )" ) );
//     linestylelist.append( i18n( "dot line ( **** )" ) );
//     linestylelist.append( i18n( "dash dot line ( -*-* )" ) );
//     linestylelist.append( i18n( "dash dot dot line ( -**- )" ) );
//     toolTip = i18n( "Paragraph Border Style" ) ;
//     m_idComboText_BorderStyle = m_vToolBarText->insertCombo( linestylelist, ID_BRD_STYLE, FALSE,
// 							     SIGNAL( activated( const QString & ) ),
// 							     this, "textBorderStyle", TRUE, toolTip,
// 							     150, -1, OpenPartsUI::AtBottom );
//     tmpBrd.style = KWParagLayout::SOLID;

//     m_vToolBarText->enable( OpenPartsUI::Show );

//     // TOOLBAR Frame
//     m_vToolBarFrame = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
//     m_vToolBarFrame->setFullWidth( FALSE );

//     // border left
//     pix = OPUIUtils::convertPixmap( BarIcon( "borderleft" ) );
//     toolTip = i18n( "Frame Border Left" ) ;
//     m_idButtonFrame_BorderLeft = m_vToolBarFrame->insertButton2( pix, ID_FBRD_LEFT, SIGNAL( clicked() ),
// 								 this, "frameBorderLeft",
// 								 TRUE, toolTip, -1 );
//     m_vToolBarFrame->setToggle( ID_FBRD_LEFT, TRUE );
//     m_vToolBarFrame->setButton( ID_FBRD_LEFT, FALSE );

//     // border right
//     pix = OPUIUtils::convertPixmap( BarIcon( "borderright" ) );
//     toolTip = i18n( "Frame Border Right" ) ;
//     m_idButtonFrame_BorderRight = m_vToolBarFrame->insertButton2( pix, ID_FBRD_RIGHT, SIGNAL( clicked() ),
// 								  this, "frameBorderRight",
// 								  TRUE, toolTip, -1 );
//     m_vToolBarFrame->setToggle( ID_FBRD_RIGHT, TRUE );
//     m_vToolBarFrame->setButton( ID_FBRD_RIGHT, FALSE );

//     // border top
//     pix = OPUIUtils::convertPixmap( BarIcon( "bordertop" ) );
//     toolTip = i18n( "Frame Border Top" ) ;
//     m_idButtonFrame_BorderTop = m_vToolBarFrame->insertButton2( pix, ID_FBRD_TOP, SIGNAL( clicked() ),
// 								this, "frameBorderTop",
// 								TRUE, toolTip, -1 );
//     m_vToolBarFrame->setToggle( ID_FBRD_TOP, TRUE );
//     m_vToolBarFrame->setButton( ID_FBRD_TOP, FALSE );

//     // border bottom
//     pix = OPUIUtils::convertPixmap( BarIcon( "borderbottom" ) );
//     toolTip = i18n( "Frame Border Bottom" ) ;
//     m_idButtonFrame_BorderBottom = m_vToolBarFrame->insertButton2( pix, ID_FBRD_BOTTOM, SIGNAL( clicked() ),
// 								   this, "frameBorderBottom",
// 								   TRUE, toolTip, -1 );
//     m_vToolBarFrame->setToggle( ID_FBRD_BOTTOM, TRUE );
//     m_vToolBarFrame->setButton( ID_FBRD_BOTTOM, FALSE );

//     // border color
//     pix = KOUIUtils::colorPixmap( frmBrd.color, KOUIUtils::FRAME_COLOR );
//     toolTip = i18n( "Frame Border Color" ) ;
//     m_idButtonFrame_BorderColor = m_vToolBarFrame->insertButton2( pix, ID_FBORDER_COLOR, SIGNAL( clicked() ),
// 								  this, "frameBorderColor",
// 								  TRUE, toolTip, -1 );

//     toolTip = i18n( "Frame Border Width" ) ;
//     m_idComboFrame_BorderWidth = m_vToolBarFrame->insertCombo( widthlist, ID_FBRD_WIDTH, FALSE,
// 							       SIGNAL( activated( const QString & ) ),
// 							       this, "frameBorderWidth", TRUE, toolTip,
// 							       60, -1, OpenPartsUI::AtBottom );

//     toolTip = i18n( "Frame Border Style" ) ;
//     m_idComboFrame_BorderStyle = m_vToolBarFrame->insertCombo( linestylelist, ID_FBRD_STYLE, FALSE,
// 							       SIGNAL( activated( const QString & ) ),
// 							       this, "frameBorderStyle", TRUE, toolTip,
// 							       150, -1, OpenPartsUI::AtBottom );

//     // frame back color
//     backColor.setColor( white );
//     pix = KOUIUtils::colorPixmap( backColor.color(), KOUIUtils::BACK_COLOR );
//     toolTip = i18n( "Frame Background Color" ) ;
//     m_idButtonFrame_BackColor = m_vToolBarFrame->insertButton2( pix, ID_FBACK_COLOR, SIGNAL( clicked() ),
// 								this, "frameBackColor",
// 								TRUE, toolTip, -1 );

//     m_vToolBarFrame->enable( OpenPartsUI::Hide );


//     // TOOLBAR Formula
//     m_vToolBarFormula = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
//     m_vToolBarFormula->setFullWidth( FALSE );

//     pix = OPUIUtils::convertPixmap( BarIcon( "index2" ) );
//     toolTip = i18n( "Power" ) ;
//     m_idButtonFormula_Power = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
// 								this, "formulaPower",
// 								TRUE, toolTip, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "index3" ) );
//     toolTip = i18n( "Subscript" ) ;
//     m_idButtonFormula_Subscript = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
// 								    this, "formulaSubscript",
// 								    TRUE, toolTip, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "bra" ) );
//     toolTip = i18n( "Parentheses" ) ;
//     m_idButtonFormula_Parentheses = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
// 								      this, "formulaParentheses",
// 								      TRUE, toolTip, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "abs" ) );
//     toolTip = i18n( "Absolute Value" ) ;
//     m_idButtonFormula_AbsValue = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
// 								   this, "formulaAbsValue",
// 								   TRUE, toolTip, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "brackets" ) );
//     toolTip = i18n( "Brackets" ) ;
//     m_idButtonFormula_Brackets = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
// 								   this, "formulaBrackets",
// 								   TRUE, toolTip, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "frac" ) );
//     toolTip = i18n( "Fraction" ) ;
//     m_idButtonFormula_Fraction = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
// 								   this, "formulaFraction",
// 								   TRUE, toolTip, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "root" ) );
//     toolTip = i18n( "Root" ) ;
//     m_idButtonFormula_Root = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
// 							       this, "formulaRoot",
// 							       TRUE, toolTip, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "integral" ) );
//     toolTip = i18n( "Integral" ) ;
//     m_idButtonFormula_Integral = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
// 								   this, "formulaIntegral",
// 								   TRUE, toolTip, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "matrix" ) );
//     toolTip = i18n( "Matrix" ) ;
//     m_idButtonFormula_Matrix = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
// 								 this, "formulaMatrix",
// 								 TRUE, toolTip, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "index0" ) );
//     toolTip = i18n( "Left Superscript" ) ;
//     m_idButtonFormula_LeftSuper = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
// 								    this, "formulaLeftSuper",
// 								    TRUE, toolTip, -1 );
//     pix = OPUIUtils::convertPixmap( BarIcon( "index1" ) );
//     toolTip = i18n( "Left Subscript" ) ;
//     m_idButtonFormula_LeftSub = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
// 								  this, "formulaLeftSub",
// 								  TRUE, toolTip, -1 );

//     m_vToolBarFormula->enable( OpenPartsUI::Hide );
// //     m_vToolBarFormula->setBarPos( OpenPartsUI::Floating );

//     updateStyle( "Standard" );
//     setFormat( format, FALSE );
//     gui->getPaperWidget()->forceFullUpdate();
//     gui->getPaperWidget()->init();

//     clipboardDataChanged();

//     gui->getPaperWidget()->repaintScreen( TRUE );

//     return TRUE;
// }

/*===================== load not KDE installed fonts =============*/
void KWordView::getFonts()
{
    int numFonts;
    Display *kde_display;
    char** fontNames;
    char** fontNames_copy;
    QString qfontname;

    kde_display = kapp->getDisplay();

    bool have_installed = kapp->kdeFonts( fontList );

    if ( have_installed )
	return;

    fontNames = XListFonts( kde_display, "*", 32767, &numFonts );
    fontNames_copy = fontNames;

    for ( int i = 0; i < numFonts; i++ ) {
	if ( **fontNames != '-' ) {
	    fontNames ++;
	    continue;
	}

	qfontname = "";
	qfontname = *fontNames;
	int dash = qfontname.find ( '-', 1, TRUE );

	if ( dash == -1 ) {
	    fontNames ++;
	    continue;
	}

	int dash_two = qfontname.find ( '-', dash + 1 , TRUE );

	if ( dash == -1 ) {
	    fontNames ++;
	    continue;
	}


	qfontname = qfontname.mid( dash +1, dash_two - dash -1 );

	if( !qfontname.contains( "open look", TRUE ) ) {
	    if( qfontname != "nil" ) {
		if( fontList.find( qfontname ) == fontList.end() )
		    fontList.append( qfontname );
	    }
	}

	fontNames ++;

    }

    XFreeFontNames( fontNames_copy );
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

    ( (KColorAction*)actionFormatBrdColor )->blockSignals( TRUE );
    ( (KColorAction*)actionFormatBrdColor )->setColor( tmpBrd.color );
    ( (KColorAction*)actionFormatBrdColor )->blockSignals( FALSE );
}

/*================================================================*/
void KWordView::slotInsertObject( KWordChild *, KWPartFrameSet * )
{
//     OpenParts::View_var v;

//     try
//     {
// 	v = _child->createView( m_vKoMainWindow );
//     }
//     catch ( OpenParts::Document::MultipleViewsNotSupported &_ex )
//     {
// 	// HACK
// 	printf( "void KWordView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n" );
// 	printf( "Could not create view\n" );
// 	exit( 1 );
//     }

//     if ( CORBA::is_nil( v ) )
//     {
// 	printf( "void KWordView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n" );
// 	printf( "return value is 0L\n" );
// 	exit( 1 );
//     }

//     KOffice::View_var kv = KOffice::View::_narrow( v );
//     kv->setMode( KOffice::View::ChildMode );
//     assert( !CORBA::is_nil( kv ) );
//     _kwpf->setView( kv );
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
    gui->getPaperWidget()->repaintScreen( TRUE );
}

/*================================================================*/
void KWordView::spellCheckerReady()
{
    // #### currently only the first available textframeset is checked!!!

    if ( !kspell->isOk() ) {
	QMessageBox::critical( this, i18n( "Error" ), i18n( "Error when spellchecking! Make sure\n"
							    "ISpell is installed!" ), i18n( "OK" ) );
	return;
    }

    QObject::connect( kspell, SIGNAL( misspelling( QString, QStrList*, unsigned ) ),
		      this, SLOT( spellCheckerMisspelling( QString, QStrList*, unsigned ) ) );
    QObject::connect( kspell, SIGNAL( corrected( QString, QString, unsigned ) ),
		      this, SLOT( spellCheckerCorrected( QString, QString, unsigned ) ) );
    QObject::connect( kspell, SIGNAL( done( const char* ) ), this, SLOT( spellCheckerDone( const char* ) ) );
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
	QObject::disconnect( kspell, SIGNAL( misspelling( QString, QStrList*, unsigned ) ), this,
			     SLOT( spellCheckerMisspelling( QString, QStrList*, unsigned ) ) );
	QObject::disconnect( kspell, SIGNAL( corrected( QString, QString, unsigned ) ),
			     this, SLOT( spellCheckerCorrected( QString, QString, unsigned ) ) );
	QObject::disconnect( kspell, SIGNAL( done( const char* ) ), this, SLOT( spellCheckerDone( const char* ) ) );
	delete kspell;
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
void KWordView::spellCheckerMisspelling( QString , QStrList* , unsigned )
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
void KWordView::spellCheckerDone( const char* )
{
    kspell->cleanUp();
    QObject::disconnect( kspell, SIGNAL( misspelling( QString, QStrList*, unsigned ) ), this,
			 SLOT( spellCheckerMisspelling( QString, QStrList*, unsigned ) ) );
    QObject::disconnect( kspell, SIGNAL( corrected( QString, QString, unsigned ) ),
			 this, SLOT( spellCheckerCorrected( QString, QString, unsigned ) ) );
    QObject::disconnect( kspell, SIGNAL( done( const char* ) ), this, SLOT( spellCheckerDone( const char* ) ) );
    delete kspell;
    gui->getPaperWidget()->recalcWholeText();
    gui->getPaperWidget()->recalcCursor( TRUE );
}

/*================================================================*/
void KWordView::searchDiaClosed()
{
    QObject::disconnect( searchDia, SIGNAL( cancelButtonPressed() ), this, SLOT( searchDiaClosed() ) );
    searchDia->close();
    delete searchDia;
    searchDia = 0L;
}

/*================================================================*/
void KWordView::changeUndo( QString _text, bool _enable )
{
    if ( _enable ) {
	actionEditUndo->setEnabled( TRUE );
	QString str;
	str.sprintf( i18n( "Undo: %s" ), _text.data() );
	actionEditUndo->setText( str );
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
	QString str;
	str.sprintf( i18n( "Redo: %s" ), _text.data() );
	actionEditRedo->setText( str );
    } else {
	actionEditRedo->setEnabled( FALSE );
	actionEditRedo->setText( i18n( "No Redo possible" ) );
    }
}

/******************************************************************/
/* Class: KWordGUI						  */
/******************************************************************/

/*================================================================*/
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
    left = new QWidget( panner );
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

    r_horz = new KoRuler( left, paperWidget->viewport(), KoRuler::HORIZONTAL, layout,
			  KoRuler::F_INDENTS | KoRuler::F_TABS, tabChooser );
    r_vert = new KoRuler( left, paperWidget->viewport(), KoRuler::VERTICAL, layout, 0 );
    connect( r_horz, SIGNAL( newPageLayout( KoPageLayout ) ), view, SLOT( newPageLayout( KoPageLayout ) ) );
    connect( r_horz, SIGNAL( newLeftIndent( int ) ), paperWidget, SLOT( newLeftIndent( int ) ) );
    connect( r_horz, SIGNAL( newFirstIndent( int ) ), paperWidget, SLOT( newFirstIndent( int ) ) );
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

    // HACK
    if ( doc->viewCount() == 1 && !doc->loaded() ) {
	QKeyEvent e( static_cast<QEvent::Type>( 6 ) /*QEvent::KeyPress*/ , Key_Delete, 0 ,0 );
	paperWidget->keyPressEvent( &e );
    }

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
    if ( _show ) {
	r_vert->show();
	r_horz->show();
	tabChooser->show();

	tabChooser->setGeometry( 0, 0, 20, 20 );

	panner->setGeometry( 0, 0, width(), height() );
	paperWidget->setGeometry( 20, 20, left->width() - 20, left->height() - 20 );
	r_horz->setGeometry( 20, 0, left->width() - 20, 20 );
	r_vert->setGeometry( 0, 20, 20, left->height() - 20 );
    } else {
	r_vert->hide();
	r_horz->hide();
	tabChooser->hide();

	QValueList<int> l;
	l << 0;
	panner->setSizes( l );
	panner->setGeometry( 0, 0, width(), height() );
	paperWidget->setGeometry( 20, 20, left->width() - 20, left->height() - 20 );

    }
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
    return gui->getPaperWidget();
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
