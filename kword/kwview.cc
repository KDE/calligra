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

#include "kwcanvas.h"
#undef Unsorted
#include <qpainter.h>
#include <qstring.h>
#include <qkeycode.h>
#include <qpixmap.h>
#include <qevent.h>
#include <kmessagebox.h>
#include <qclipboard.h>
#include <qsplitter.h>
#include <kaction.h>
//#include <qfiledialog.h>
#include <qregexp.h>
#include <qobjectlist.h>
#include <qpaintdevicemetrics.h>

#include "kwview.h"
#include "kwtextframeset.h"
#include "kwdoc.h"
#include "kwview.moc"
#include "kwframe.h"
#include "kwgroupmanager.h"
#include "defs.h"
#include "paragdia.h"
#include "searchdia.h"
#include "stylist.h"
#include "tabledia.h"
#include "insdia.h"
#include "deldia.h"
#include "docstruct.h"
#include "variable.h"
#include "footnotedia.h"
#include "autoformatdia.h"
#include "variabledlgs.h"
#include "serialletter.h"
#include "kwconfig.h"
#include "kcharselectdia.h"
#include "kwcommand.h"

#include <koMainWindow.h>
#include <koDocument.h>
#include <koRuler.h>
#include <koTabChooser.h>
#include <koPartSelectDia.h>
#include <kformulaedit.h>
#include <koTemplateCreateDia.h>
#include <koFrame.h>

#include <ktempfile.h>
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
#include <kimageio.h>
#include <kcoloractions.h>
#include <kfontdialog.h>
#include <kstddirs.h>
#include <kparts/event.h>

#include <stdlib.h>

#include "preview.h"

/******************************************************************/
/* Class: KWView                                               */
/******************************************************************/

/*================================================================*/
KWView::KWView( QWidget *_parent, const char *_name, KWDocument* _doc )
    : KoView( _doc, _parent, _name )
{
    doc = 0L;
    gui = 0;
    kspell = 0;
    //vertAlign = KWFormat::VA_NORMAL;
    left.color = white;
    left.style = Border::SOLID;
    left.ptWidth = 0;
    right.color = white;
    right.style = Border::SOLID;
    right.ptWidth = 0;
    top.color = white;
    top.style = Border::SOLID;
    top.ptWidth = 0;
    bottom.color = white;
    bottom.style = Border::SOLID;
    bottom.ptWidth = 0;
    tmpBrd.color = black;
    tmpBrd.style = Border::SOLID;
    tmpBrd.ptWidth = 1;
    frmBrd.color = black;
    frmBrd.style = Border::SOLID;
    frmBrd.ptWidth = 1;

    searchEntry = new KWSearchContext();
    replaceEntry = new KWSearchContext();
    doc = _doc;
    backColor = QBrush( white );

    setInstance( KWFactory::global() );
    setXMLFile( "kword.rc" );

    QObject::connect( doc, SIGNAL( sig_insertObject( KWChild*, KWPartFrameSet* ) ),
                      this, SLOT( slotInsertObject( KWChild*, KWPartFrameSet* ) ) );
    QObject::connect( this, SIGNAL( embeddImage( const QString & ) ),
                      this, SLOT( insertPicture( const QString & ) ) );
    QObject::connect( doc, SIGNAL( sig_updateChildGeometry( KWChild* ) ),
                      this, SLOT( slotUpdateChildGeometry( KWChild* ) ) );
    QObject::connect( doc, SIGNAL( pageNumChanged() ),
                      this, SLOT( updatePageInfo() ) );

    KFontChooser::getFontList(fontList, false); // Shouldn't this be in the doc, or not at all ?
    setKeyCompression( TRUE );
    setAcceptDrops( TRUE );
    createKWGUI();
    initConfig();
    gui->canvasWidget()->updateCurrentFormat();
    setFocusProxy( gui->canvasWidget() );
}

/*================================================================*/
KWView::~KWView()
{
}

/*=============================================================*/
void KWView::initConfig()
{
  KConfig *config = KWFactory::global()->config();
  KSpellConfig ksconfig;
  if( config->hasGroup("KSpell kword" ) )
    {
      config->setGroup( "KSpell kword" );
      ksconfig.setNoRootAffix(config->readNumEntry ("KSpell_NoRootAffix", 0));
      ksconfig.setRunTogether(config->readNumEntry ("KSpell_RunTogether", 0));
      ksconfig.setDictionary(config->readEntry ("KSpell_Dictionary", ""));
      ksconfig.setDictFromList(config->readNumEntry ("KSpell_DictFromList", FALSE));
      ksconfig.setEncoding(config->readNumEntry ("KSpell_Encoding", KS_E_ASCII));
      ksconfig.setClient(config->readNumEntry ("KSpell_Client", KS_CLIENT_ISPELL));
      getGUI()->getDocument()->setKSpellConfig(ksconfig);
    }
  if(config->hasGroup("Interface" ) )
    {
      config->setGroup( "Interface" );
      getGUI()->getDocument()->setGridY(config->readNumEntry("GridY",10));
      getGUI()->getDocument()->setGridX(config->readNumEntry("GridX",10));
    }
}

/*=============================================================*/
void KWView::initGui()
{
    clipboardDataChanged();

    if ( gui )
        gui->showGUI();
    actionToolsEdit->setChecked( TRUE );
    actionViewFrameBorders->setChecked( doc->getViewFrameBorders() );
    actionViewTableGrid->setChecked( doc->getViewTableGrid());
    actionViewFormattingChars->setChecked( doc->getViewFormattingChars());

    actionViewHeader->setChecked(doc->isHeaderVisible());
    actionViewFooter->setChecked(doc->isFooterVisible());

    //setNoteType(doc->getNoteType(), false);

    actionFormatColor->setColor( Qt::black );
    actionFormatBrdColor->setColor( Qt::black );
    actionFrameBrdColor->setColor( Qt::black );
    actionFrameBackColor->setColor( Qt::white );

    actionViewZoom->setCurrentItem( 1 );

    showFormulaToolbar( FALSE );

    /*
      The XML file does this for us now
      QWidget *tb = 0;
      if ( factory() )
      tb = factory()->container( "frame_toolbar", this );
      if ( tb )
      tb->hide();
    */

    statusBar()->insertItem( QString(" ")+i18n("Page %1/%2").arg(1).arg(1)+' ', statusPage );
    // Workaround for bug in KDE-2.1[.1]'s KStatusBar (show() not called in insertItem)
    QObjectList *l = statusBar()->queryList( "QLabel" );
    QObjectListIt it( *l );
    for ( ; it.current() ; ++it )
        static_cast<QLabel *>(it.current())->show();
    delete l;
    updatePageInfo();
}

/*================================================================*/
void KWView::setupActions()
{
    // -------------- Edit actions
    actionEditCut = KStdAction::cut( this, SLOT( editCut() ), actionCollection(), "edit_cut" );
    actionEditCopy = KStdAction::copy( this, SLOT( editCopy() ), actionCollection(), "edit_copy" );
    actionEditPaste = KStdAction::paste( this, SLOT( editPaste() ), actionCollection(), "edit_paste" );
    KStdAction::find( this, SLOT( editFind() ), actionCollection(), "edit_find" );
    KStdAction::replace( this, SLOT( editReplace() ), actionCollection(), "edit_replace" );
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
    (void) new KAction( "Print debug info" , 0,
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
    actionViewFootNotes->setExclusiveGroup( "notes" );
    actionViewEndNotes = new KToggleAction( i18n( "&Endnotes" ), 0,
                                             this, SLOT( viewEndNotes() ),
                                          actionCollection(), "view_endnotes" );
    actionViewEndNotes->setExclusiveGroup( "notes" );

    actionViewZoom = new KSelectAction( i18n( "Zoom" ), 0,
                                        actionCollection(), "view_zoom" );
    connect( actionViewZoom, SIGNAL( activated( const QString & ) ),
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
    actionViewZoom->setItems( lst );
    // -------------- Insert actions
    actionInsertPicture = new KAction( i18n( "&Picture..." ), "picture", Key_F2,
                                       this, SLOT( insertPicture() ),
                                       actionCollection(), "insert_picture" );

    actionInsertSpecialChar = new KAction( i18n( "&Special Character..." ), "char", ALT + SHIFT + Key_C,
                                           this, SLOT( insertSpecialChar() ),
                                           actionCollection(), "insert_specialchar" );

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
    actionToolsEdit->setExclusiveGroup( "tools" );
    actionToolsEditFrames = new KToggleAction( i18n( "Edit &Frames" ), "editframetool", Key_F5,
                                               this, SLOT( toolsEditFrame() ),
                                               actionCollection(), "tools_editframes" );
    actionToolsEditFrames->setExclusiveGroup( "tools" );
    actionToolsCreateText = new KToggleAction( i18n( "&Create Text Frame" ), "textframetool", Key_F6,
                                               this, SLOT( toolsCreateText() ),
                                               actionCollection(), "tools_createtext" );
    actionToolsCreateText->setExclusiveGroup( "tools" );
    actionToolsCreatePix = new KToggleAction( i18n( "&Create Picture Frame" ), "picframetool", Key_F7,
                                              this, SLOT( toolsCreatePix() ),
                                              actionCollection(), "tools_createpix" );
    actionToolsCreatePix->setExclusiveGroup( "tools" );
    actionToolsCreateTable = new KToggleAction( i18n( "&Create Table" ), "table", Key_F9,
                                                this, SLOT( toolsTable() ),
                                                actionCollection(), "tools_table" );
    actionToolsCreateTable->setExclusiveGroup( "tools" );
    actionToolsCreateFormula = new KToggleAction( i18n( "&Create Formula Frame" ), "formula", Key_F11,
                                                  this, SLOT( toolsFormula() ),
                                                  actionCollection(), "tools_formula" );
    actionToolsCreateFormula->setExclusiveGroup( "tools" );
    actionToolsCreatePart = new KToggleAction( i18n( "&Create Part Frame" ), "parts", Key_F12,
                                               this, SLOT( toolsPart() ),
                                               actionCollection(), "tools_part" );
    actionToolsCreatePart->setExclusiveGroup( "tools" );


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

    actionFormatStylist = new KAction( i18n( "&Stylist..." ), 0,
                                      this, SLOT( extraStylist() ),
                                      actionCollection(), "format_stylist" );

    actionFormatFontSize = new KFontSizeAction( i18n( "Font Size" ), 0,
                                              actionCollection(), "format_fontsize" );
    connect( actionFormatFontSize, SIGNAL( fontSizeChanged( int ) ),
             this, SLOT( textSizeSelected( int ) ) );
    actionFormatFontFamily = new KFontAction( i18n( "Font Family" ), 0,
                                              actionCollection(), "format_fontfamily" );
    connect( actionFormatFontFamily, SIGNAL( activated( const QString & ) ),
             this, SLOT( textFontSelected( const QString & ) ) );

    actionFormatStyle = new KSelectAction( i18n( "Style" ), 0,
                                           actionCollection(), "format_style" );
    connect( actionFormatStyle, SIGNAL( activated( int ) ),
             this, SLOT( textStyleSelected( int ) ) );
    updateStyleList();

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
    actionFormatAlignLeft->setExclusiveGroup( "align" );
    actionFormatAlignLeft->setChecked( TRUE );
    actionFormatAlignCenter = new KToggleAction( i18n( "Align &Center" ), "alignCenter", ALT + Key_C,
                                         this, SLOT( textAlignCenter() ),
                                         actionCollection(), "format_aligncenter" );
    actionFormatAlignCenter->setExclusiveGroup( "align" );
    actionFormatAlignRight = new KToggleAction( i18n( "Align &Right" ), "alignRight", ALT + Key_R,
                                        this, SLOT( textAlignRight() ),
                                        actionCollection(), "format_alignright" );
    actionFormatAlignRight->setExclusiveGroup( "align" );
    actionFormatAlignBlock = new KToggleAction( i18n( "Align &Block" ), "alignBlock", ALT + Key_B,
                                        this, SLOT( textAlignBlock() ),
                                        actionCollection(), "format_alignblock" );
    actionFormatAlignBlock->setExclusiveGroup( "align" );
    actionFormatEnumList = new KToggleAction( i18n( "Enumerated List" ), "enumList", 0,
                                              this, SLOT( textEnumList() ),
                                              actionCollection(), "format_enumlist" );
    actionFormatEnumList->setExclusiveGroup( "style" );
    actionFormatUnsortList = new KToggleAction( i18n( "Bullet List" ), "unsortedList", 0,
                                              this, SLOT( textUnsortList() ),
                                              actionCollection(), "format_unsortlist" );
    actionFormatUnsortList->setExclusiveGroup( "style" );
    actionFormatSuper = new KToggleAction( i18n( "Superscript" ), "super", 0,
                                              this, SLOT( textSuperScript() ),
                                              actionCollection(), "format_super" );
    actionFormatSuper->setExclusiveGroup( "valign" );
    actionFormatSub = new KToggleAction( i18n( "Subscript" ), "sub", 0,
                                              this, SLOT( textSubScript() ),
                                              actionCollection(), "format_sub" );
    actionFormatSub->setExclusiveGroup( "valign" );
    actionFormatBrdOutline = new KToggleAction( i18n( "Paragraph Border Outline" ), "borderoutline", 0,this, SLOT( textBorderOutline() ),
			actionCollection(), "format_brdoutline" );

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
    connect( actionFormatBrdWidth, SIGNAL( activated( const QString & ) ),
             this, SLOT( textBorderWidth( const QString & ) ) );
    lst.clear();
    for ( unsigned int i = 0; i < 10; i++ )
        lst << QString( "%1" ).arg( i + 1 );
    actionFormatBrdWidth->setItems( lst );
    actionFormatBrdStyle = new KSelectAction( i18n( "Paragraph Border Style" ), 0,
                                                 actionCollection(), "format_brdstyle" );
    connect( actionFormatBrdStyle, SIGNAL( activated( const QString & ) ),
             this, SLOT( textBorderStyle( const QString & ) ) );
    lst.clear();
    lst.append( i18n( "solid line" ) );
    lst.append( i18n( "dash line ( ---- )" ) );
    lst.append( i18n( "dot line ( **** )" ) );
    lst.append( i18n( "dash dot line ( -*-* )" ) );
    lst.append( i18n( "dash dot dot line ( -**- )" ) );
    actionFormatBrdStyle->setItems( lst );

    // ---------------------------- frame toolbar actions

    actionFrameBrdOutline = new KToggleAction( i18n( "Frame Border Outline" ), "borderoutline", 0,this, SLOT( frameBorderOutline() ),
			actionCollection(), "frame_brdoutline" );

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
    connect( actionFrameBrdStyle, SIGNAL( activated( const QString & ) ),
             this, SLOT( frameBorderStyle( const QString & ) ) );
    actionFrameBrdStyle->setItems( lst );
    actionFrameBrdWidth = new KSelectAction( i18n( "Frame Border Width" ), 0,
                                                 actionCollection(), "frame_brdwidth" );
    connect( actionFrameBrdWidth, SIGNAL( activated( const QString & ) ),
             this, SLOT( frameBorderWidth( const QString & ) ) );
    lst.clear();
    for ( unsigned int i = 0; i < 10; i++ )
        lst << QString( "%1" ).arg( i + 1 );
    actionFrameBrdWidth->setItems( lst );
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

    actionExtraOptions = new KAction( i18n( "&Options..." ), 0,
                                      this, SLOT( extraOptions() ),
                                      actionCollection(), "extra_options" );
    actionExtraCreateTemplate = new KAction( i18n( "&Create Template from Document..." ), 0,
                                             this, SLOT( extraCreateTemplate() ),
                                             actionCollection(), "extra_template" );
    //------------------------ Configure
    actionExtraOptions = new KAction( i18n( "&Configure..." ), 0,
                                      this, SLOT( configure() ),
                                      actionCollection(), "configure" );

}

/*======================== create GUI ==========================*/
void KWView::createKWGUI()
{
    // setup GUI
    setupActions();

    gui = new KWGUI( this, true, doc, this );
    gui->setGeometry( 0, 0, width(), height() );
    gui->show();

    KWFrameSet *frameset;
    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        frameset = doc->getFrameSet( i );
        if ( frameset->getFrameType() == FT_PART )
            slotInsertObject( dynamic_cast<KWPartFrameSet*>( frameset )->getChild(),
                              dynamic_cast<KWPartFrameSet*>( frameset ) );
        else if ( frameset->getFrameType() == FT_FORMULA )
            dynamic_cast<KWFormulaFrameSet*>( frameset )->create( gui->canvasWidget() );
    }
}

/*================================================================*/
void KWView::showFormulaToolbar( bool show )
{
  if ( !factory() )
    return;

  // This might not be exactly the right place. But these actions
  // must be enabled when a formula is active...
  doc->getFormulaDocument()->getMakeGreekAction()->setEnabled( show );
  doc->getFormulaDocument()->getAddGenericUpperAction()->setEnabled( show );
  doc->getFormulaDocument()->getAddGenericLowerAction()->setEnabled( show );
  doc->getFormulaDocument()->getRemoveEnclosingAction()->setEnabled( show );

  QWidget *tb = factory()->container( "formula_toolbar", this );
  if( !tb )
    return;

  if ( show )
    tb->show();
  else
    tb->hide();
}

void KWView::updatePageInfo()
{
    KWFrameSetEdit * edit = gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
    {
        int pgNum = edit->currentFrame()->pageNum() + 1;
        statusBar()->changeItem( QString(" ")+i18n("Page %1/%2").arg(pgNum).arg(doc->getPages())+' ', statusPage );
        gui->getVertRuler()->setOffset( 0, -gui->canvasWidget()->getVertRulerPos() );
    }
}

/*================================================================*/
void KWView::clipboardDataChanged()
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
void KWView::setupPrinter( KPrinter &prt )
{
    prt.setMinMax( 1, doc->getPages() );
    bool makeLandscape = FALSE;

    KoPageLayout pgLayout = doc->pageLayout();

    switch ( pgLayout.format ) {
    case PG_DIN_A3: prt.setPageSize( KPrinter::A3 );
        break;
    case PG_DIN_A4: prt.setPageSize( KPrinter::A4 );
        break;
    case PG_DIN_A5: prt.setPageSize( KPrinter::A5 );
        break;
    case PG_US_LETTER: prt.setPageSize( KPrinter::Letter );
        break;
    case PG_US_LEGAL: prt.setPageSize( KPrinter::Legal );
        break;
    case PG_US_EXECUTIVE: prt.setPageSize( KPrinter::Executive );
        break;
    case PG_DIN_B5: prt.setPageSize( KPrinter::B5 );
        break;
    case PG_SCREEN: {
        kdWarning() << "You use the page layout SCREEN. Printing in DIN A4 LANDSCAPE." << endl;
        prt.setPageSize( KPrinter::A4 );
        makeLandscape = TRUE;
    } break;
    default: {
        kdWarning() << "The used page layout is not supported by KPrinter. Printing in A4." << endl;
        prt.setPageSize( KPrinter::A4 );
    } break;
    }

    if ( pgLayout.orientation == PG_LANDSCAPE || makeLandscape )
        prt.setOrientation( KPrinter::Landscape );
    else
        prt.setOrientation( KPrinter::Portrait );
}

void KWView::print( KPrinter &prt )
{
    gui->canvasWidget()->viewport()->setCursor( waitCursor );

    prt.setFullPage( true );

    int oldZoom = doc->zoom();
    QPaintDeviceMetrics metrics( &prt );
    double hackFactor = 1.0; // 2.0 for the hack (improving print-preview's quality)
    // The real solution is Qt-3.0's KPrinter::setResolution.
    doc->setZoomAndResolution( 100, metrics.logicalDpiX()*hackFactor, metrics.logicalDpiY()*hackFactor, false );

    bool serialLetter = FALSE;
#if 0
    QList<KWVariable> *vars = doc->getVariables();
    KWVariable *v = 0;
    for ( v = vars->first(); v; v = vars->next() ) {
        if ( v->getType() == VT_SERIALLETTER ) {
            serialLetter = TRUE;
            break;
        }
    }

    if ( !doc->getSerialLetterDataBase() ||
         doc->getSerialLetterDataBase()->getNumRecords() == 0 )
        serialLetter = FALSE;
#endif
    //float left_margin = 0.0;
    //float top_margin = 0.0;

    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    doc->getPageLayout( pgLayout, cl, hf );
    KoPageLayout oldPGLayout = pgLayout;

    if ( pgLayout.format == PG_SCREEN )
    {
        //left_margin = 25.8;
        //top_margin = 15.0;
        pgLayout.ptLeft += 25.8;         // Not sure why we need this....
        pgLayout.ptRight += 15.0;
        // TODO the other units. Well, better get rid of the multiple-unit thing.
        doc->setPageLayout( pgLayout, cl, hf );
    }

    if ( !serialLetter ) {
        QPainter painter;
        painter.begin( &prt );
        //painter.scale( 1.0/hackFactor, 1.0/hackFactor );
        gui->canvasWidget()->print( &painter, &prt );
        painter.end();
    } else {
#if 0
        QPainter painter;
        painter.begin( &prt );
        for ( int i = 0; i < doc->getSerialLetterDataBase()->getNumRecords(); ++i ) {
            doc->setSerialLetterRecord( i );
            gui->canvasWidget()->print( &painter, &prt, left_margin, top_margin );
            if ( i < doc->getSerialLetterDataBase()->getNumRecords() - 1 )
                prt.newPage();
        }
        doc->setSerialLetterRecord( -1 );
        painter.end();
#endif
    }

    if ( pgLayout.format == PG_SCREEN )
        doc->setPageLayout( oldPGLayout, cl, hf );

    doc->setZoomAndResolution( oldZoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY(), false );

    gui->canvasWidget()->viewport()->setCursor( ibeamCursor );
}

/*================================================================*/
void KWView::showFormat( const QTextFormat &currentFormat )
{
    // update the gui with the current format.
    //kdDebug() << "KWView::setFormat font family=" << currentFormat.font().family() << endl;
    actionFormatFontFamily->setFont( currentFormat.font().family() );
    actionFormatFontSize->setFontSize( currentFormat.font().pointSize() );
    actionFormatBold->setChecked( currentFormat.font().bold());
    actionFormatItalic->setChecked( currentFormat.font().italic() );
    actionFormatUnderline->setChecked( currentFormat.font().underline());
    actionFormatColor->setColor( currentFormat.color() );

    switch(currentFormat.vAlign())
      {
      case QTextFormat::AlignSuperScript:
	{
	  actionFormatSub->setChecked( false );
	  actionFormatSuper->setChecked( true );
	  break;
	}
      case QTextFormat::AlignSubScript:
	{
	  actionFormatSub->setChecked( true );
	  actionFormatSuper->setChecked( false );
	  break;
	}
      case QTextFormat::AlignNormal:
      default:
	{
	  actionFormatSub->setChecked( false );
	  actionFormatSuper->setChecked( false );
	  break;
	}
      }

}


void KWView::showRulerIndent( KWUnit _leftMargin, KWUnit _firstLine )
{
  KWUnits unit = KWUnit::unitType( doc->getUnit() );
  KWUnit relativeValue = KWUnit::createUnit((_firstLine.value( unit )+_leftMargin.value( unit )) , unit );
  gui->getHorzRuler()->setFirstIndent( relativeValue.value( unit ) );
  gui->getHorzRuler()->setLeftIndent( _leftMargin.value( unit ) );
}

/*================================================================*/
void KWView::showAlign( int align ) {
    switch ( align ) {
        case Qt3::AlignAuto: // In left-to-right mode it's align left. TODO: alignright if text->isRightToLeft()
        case Qt::AlignLeft:
            actionFormatAlignLeft->setChecked( TRUE );
            break;
        case Qt::AlignCenter:
            actionFormatAlignCenter->setChecked( TRUE );
            break;
        case Qt::AlignRight:
            actionFormatAlignRight->setChecked( TRUE );
            break;
        case Qt3::AlignJustify: // Make this Qt:AlignJustify after the merge with Qt3
            actionFormatAlignBlock->setChecked( TRUE );
            break;
    }
}

/*================================================================*/
void KWView::showCounter( Counter &c )
{
    actionFormatEnumList->setChecked( ( c.numbering() == Counter::NUM_LIST ) && ( c.style() == Counter::STYLE_NUM ) );
    actionFormatUnsortList->setChecked( ( c.numbering() == Counter::NUM_LIST ) && ( c.style() == Counter::STYLE_DISCBULLET ) );
}

/*================================================================*/
void KWView::showFrameBorders( Border _left, Border _right,
                               Border _top, Border _bottom )
{
   actionFrameBrdLeft->setChecked( _left.ptWidth > 0 );
   actionFrameBrdRight->setChecked( _right.ptWidth > 0 );
   actionFrameBrdTop->setChecked( _top.ptWidth > 0 );
   actionFrameBrdBottom->setChecked( _bottom.ptWidth > 0 );
   verifyFrameOutline();
}

/*================================================================*/
void KWView::showParagBorders( Border _left, Border _right,
                               Border _top, Border _bottom )
{
    if ( left != _left || right != _right || top != _top || bottom != _bottom ) {
        left = _left;
        right = _right;
        top = _top;
        bottom = _bottom;

        actionFormatBrdLeft->setChecked( left.ptWidth > 0 );
        actionFormatBrdRight->setChecked( right.ptWidth > 0 );
        actionFormatBrdTop->setChecked( top.ptWidth > 0 );
        actionFormatBrdBottom->setChecked( bottom.ptWidth > 0 );
	actionFormatBrdOutline->setChecked( actionFormatBrdBottom->isChecked() &&
                                            actionFormatBrdTop->isChecked() &&
					    actionFormatBrdRight->isChecked() &&
					    actionFormatBrdLeft->isChecked() );

        if ( left.ptWidth > 0 ) {
            tmpBrd = left;
            showParagBorderValues();
        }
        if ( right.ptWidth > 0 ) {
            tmpBrd = right;
            showParagBorderValues();
        }
        if ( top.ptWidth > 0 ) {
            tmpBrd = top;
            showParagBorderValues();
        }
        if ( bottom.ptWidth > 0 ) {
            tmpBrd = bottom;
            showParagBorderValues();
        }
    }
}

void KWView::updateReadWrite( bool readwrite )
{
    // Disable everything if readonly.
    // But don't enable everything if readwrite. E.g. "undo" must be initially disabled.
    if ( !readwrite )
    {
        QValueList<KAction*> actions = actionCollection()->actions();
        QValueList<KAction*>::ConstIterator aIt = actions.begin();
        QValueList<KAction*>::ConstIterator aEnd = actions.end();
        for (; aIt != aEnd; ++aIt )
            (*aIt)->setEnabled( readwrite );
        // A few harmless actions
        actionEditCopy->setEnabled( true );
        actionViewFormattingChars->setEnabled( true );
        actionViewFrameBorders->setEnabled( true );
        actionViewTableGrid->setEnabled( true );
        actionViewHeader->setEnabled( true );
        actionViewFooter->setEnabled( true );
        actionViewFootNotes->setEnabled( true );
        actionViewEndNotes->setEnabled( true );
        actionViewZoom->setEnabled( true );
    }
}

/*===============================================================*/
void KWView::setTool( MouseMode _mouseMode )
{
    switch ( _mouseMode ) {
    case MM_EDIT:
        actionToolsEdit->setChecked( TRUE );
        break;
    case MM_EDIT_FRAME:
        actionToolsEditFrames->setChecked( TRUE );
        break;
    case MM_CREATE_TEXT:
        actionToolsCreateText->setChecked( TRUE );
        break;
    case MM_CREATE_PIX:
        actionToolsCreatePix->setChecked( TRUE );
        break;
    case MM_CREATE_TABLE:
        actionToolsCreateTable->setChecked( TRUE );
        break;
    case MM_CREATE_FORMULA:
        actionToolsCreateFormula->setChecked( TRUE );
        break;
    case MM_CREATE_PART:
        actionToolsCreatePart->setChecked( TRUE );
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
        //checked false all frame border button
        //because when you change mode to edit frame
        // there isn't any frame selected => border button
        //should be unselect.
        actionFrameBrdOutline->setChecked(false);
        actionFrameBrdLeft->setChecked(false);
        actionFrameBrdRight->setChecked(false);
        actionFrameBrdTop->setChecked(false);
        actionFrameBrdBottom->setChecked(false);
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
    actionTableUngroup->setEnabled( FALSE );

    switch ( _mouseMode ) {
    case MM_EDIT: {
        actionTableInsertRow->setEnabled( TRUE );
        actionTableInsertCol->setEnabled( TRUE );
        actionTableDelRow->setEnabled( TRUE );
        actionTableDelCol->setEnabled( TRUE );
        actionTableJoinCells->setEnabled( TRUE );
        actionTableSplitCells->setEnabled( TRUE );
        actionTableDelete->setEnabled( TRUE );
        actionTableUngroup->setEnabled( TRUE );
    } break;
    case MM_EDIT_FRAME: {
        actionTableJoinCells->setEnabled( TRUE );
        actionTableSplitCells->setEnabled( TRUE );
        actionTableUngroup->setEnabled( TRUE );
    } break;
    default: break;
    }
}

/*===============================================================*/
void KWView::showStyle( const QString & styleName )
{
    QListIterator<KWStyle> styleIt( doc->styleList() );
    for ( int pos = 0 ; styleIt.current(); ++styleIt, ++pos )
    {
        if ( styleIt.current()->name() == styleName ) {
            actionFormatStyle->setCurrentItem( pos );
            return;
        }
    }
}

/*===============================================================*/
void KWView::updateStyleList()
{
    QString currentStyle = actionFormatStyle->currentText();
    QStringList lst;
    QListIterator<KWStyle> styleIt( doc->styleList() );
    for (; styleIt.current(); ++styleIt )
        lst << i18n("KWord Style", styleIt.current()->name().local8Bit()); // try to translate the name, if standard
    actionFormatStyle->setItems( lst );
    showStyle( currentStyle );
}

/*===============================================================*/
void KWView::editCut()
{
#if 0
    if ( gui->canvasWidget()->formulaIsActive() )
        gui->canvasWidget()->insertFormulaChar( CUT_CHAR );
#endif
    KWFrameSetEdit * edit = gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
        edit->cut();
}

/*===============================================================*/
void KWView::editCopy()
{
#if 0
    if ( gui->canvasWidget()->formulaIsActive() )
        gui->canvasWidget()->insertFormulaChar( COPY_CHAR );
    else
#endif
    KWFrameSetEdit * edit = gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
        edit->copy();
}

/*===============================================================*/
void KWView::editPaste()
{
    KWFrameSetEdit * edit = gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
        edit->paste();
}

/*===============================================================*/
void KWView::editSelectAll()
{
    KWFrameSetEdit * edit = gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
        edit->selectAll();
}

/*===============================================================*/
void KWView::editFind()
{
    KWSearchDia *dialog;

    dialog = new KWSearchDia( this, "Find", searchEntry );
    if (dialog->exec() == QDialog::Accepted)
    {
    }
    delete dialog;
}

/*===============================================================*/
void KWView::editReplace()
{
    KWReplaceDia *dialog;

    dialog = new KWReplaceDia( this, "Replace", searchEntry, replaceEntry );
    if (dialog->exec() == QDialog::Accepted)
    {
    }
    delete dialog;
}

/*================================================================*/
void KWView::editDeleteFrame()
{
    int result;
    result = KMessageBox::warningContinueCancel(this,                                                i18n("Do you want to delete this frame?"),
                                                i18n("Delete Frame"),
                                                i18n("&Delete"));
    if (result == KMessageBox::Continue)
        gui->canvasWidget()->deleteFrame();
}

/*================================================================*/
void KWView::editReconnectFrame()
{
#if 0
    gui->canvasWidget()->editReconnectFrame();
#endif
}

/*===============================================================*/
void KWView::editCustomVars()
{
#if 0
    KWVariableValueDia *dia = new KWVariableValueDia( this, doc->getVariables() );
    dia->exec();
    gui->canvasWidget()->recalcWholeText();
    gui->canvasWidget()->repaintScreen( FALSE );
    delete dia;
#endif
}

/*===============================================================*/
void KWView::editSerialLetterDataBase()
{
#if 0
    KWSerialLetterEditor *dia = new KWSerialLetterEditor( this, doc->getSerialLetterDataBase() );
    dia->exec();
    gui->canvasWidget()->recalcWholeText();
    gui->canvasWidget()->repaintScreen( FALSE );
    delete dia;
#endif
}

/*===============================================================*/
void KWView::viewFormattingChars()
{

    doc->setViewFormattingChars(actionViewFormattingChars->isChecked());
    gui->canvasWidget()->repaintAll();
}

/*===============================================================*/
void KWView::viewFrameBorders()
{
    doc->setViewFrameBorders(actionViewFrameBorders->isChecked());
    gui->canvasWidget()->repaintAll();
}

/*===============================================================*/
void KWView::viewTableGrid()
{
    doc->setViewTableGrid( actionViewTableGrid->isChecked());
    gui->canvasWidget()->repaintAll();
}

/*===============================================================*/
void KWView::viewHeader()
{
    doc->setHeaderVisible( actionViewHeader->isChecked() );
    /*
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    doc->getPaeLayout( pgLayout, cl, hf );
    doc->setPageLayout( pgLayout, cl, hf );
    */
}

/*===============================================================*/
void KWView::viewFooter()
{
    doc->setFooterVisible( actionViewFooter->isChecked() );
    /*
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    doc->getPageLayout( pgLayout, cl, hf );
    doc->setPageLayout( pgLayout, cl, hf );
    */
}

/*===============================================================*/
void KWView::viewFootNotes()
{
#if 0
    if ( !actionViewFootNotes->isChecked() )
        return;
    setNoteType( KWFootNoteManager::FootNotes);
#endif
}

/*===============================================================*/
void KWView::viewEndNotes()
{
#if 0
    if ( !actionViewEndNotes->isChecked() )
        return;
    setNoteType( KWFootNoteManager::EndNotes);
#endif
}

#if 0
void KWView::setNoteType( KWFootNoteManager::NoteType nt, bool change)
{
    if (change)
        doc->setNoteType( nt );
    switch (nt)
    {
      case KWFootNoteManager::FootNotes:
      actionViewFootNotes->setChecked( TRUE );
      actionInsertFootEndNote->setText(i18n("&Footnote"));
      break;
    case KWFootNoteManager::EndNotes:
      default:
      actionViewEndNotes->setChecked( TRUE );
      actionInsertFootEndNote->setText(i18n("&Endnote"));
      break;
    }
}
#endif

void KWView::viewZoom( const QString &s )
{
    QString z( s );
    z = z.replace( QRegExp( "%" ), "" );
    z = z.simplifyWhiteSpace();
    int zoom = z.toInt();
    if ( zoom != doc->zoom() ) {
        doc->setZoomAndResolution( zoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY(), true );
        gui->getHorzRuler()->setZoom( doc->zoomedResolutionX() );
        gui->getVertRuler()->setZoom( doc->zoomedResolutionY() );

        // What's this for ?
        KWFrame *frame=doc->getFirstSelectedFrame();
        if(frame && frame->getFrameSet() && frame->getFrameSet()->getFrameType() == FT_TEXT)
        {
            if(doc->processingType() == KWDocument::WP && frame->getFrameSet() == doc->getFrameSet(0) && frame->isSelected())
                frame->setSelected(true);
        }
    }
    gui->canvasWidget()->repaintAll();
    gui->canvasWidget()->setFocus();

}

/*===============================================================*/
void KWView::insertPicture()
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
    // TODO disable this action if currentframeset not a text frameset
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->insertPicture( file );
}

/*===============================================================*/
void KWView::insertPicture(const QString &filename)
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->insertPicture( filename );
}

/*===============================================================*/
void KWView::insertSpecialChar()
{
    KWTextFrameSetEdit *edit=dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( !edit )
        return;
    QString f = edit->textFontFamily();
    QChar c=' ';
    KCharSelectDia *dlg=new KCharSelectDia( this, "insert special char", f,c );
    connect(dlg,SIGNAL(insertChar(QChar,const QString &)),this,SLOT(slotSpecialChar(QChar,const QString &)));
    dlg->show();
    delete dlg;
}

/*===============================================================*/
void KWView::slotSpecialChar(QChar c, const QString &_font)
{
   KWTextFrameSetEdit *edit=dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( !edit )
        return;
    edit->setFamily( _font );
    edit->insertSpecialChar(c);
}

/*===============================================================*/
void KWView::insertFrameBreak()
{
#if 0
    if ( gui->canvasWidget()->getTable() )
	return;
#endif

    QKeyEvent e(static_cast<QEvent::Type>(6) /*QEvent::KeyPress*/ ,Key_Return,0,ControlButton);
#if 0
    gui->canvasWidget()->keyPressEvent( &e );
#endif
}

/*===============================================================*/
void KWView::insertVariableDateFix()
{
#if 0
    gui->canvasWidget()->insertVariable( VT_DATE_FIX );
#endif
}

/*===============================================================*/
void KWView::insertVariableDateVar()
{
#if 0
    gui->canvasWidget()->insertVariable( VT_DATE_VAR );
#endif
}

/*===============================================================*/
void KWView::insertVariableTimeFix()
{
#if 0
    gui->canvasWidget()->insertVariable( VT_TIME_FIX );
#endif
}

/*===============================================================*/
void KWView::insertVariableTimeVar()
{
#if 0
    gui->canvasWidget()->insertVariable( VT_TIME_VAR );
#endif
}

/*===============================================================*/
void KWView::insertVariablePageNum()
{
#if 0
    gui->canvasWidget()->insertVariable( VT_PGNUM );
#endif
}

/*===============================================================*/
void KWView::insertVariableCustom()
{
#if 0
    gui->canvasWidget()->insertVariable( VT_CUSTOM );
#endif
}

/*===============================================================*/
void KWView::insertVariableSerialLetter()
{
#if 0
    gui->canvasWidget()->insertVariable( VT_SERIALLETTER );
#endif
}

/*===============================================================*/
void KWView::insertFootNoteEndNote()
{
#if 0
    int start = doc->getFootNoteManager().findStart( gui->canvasWidget()->getCursor() );

    if ( start == -1 )
    {
        KMessageBox::sorry( this,
                            i18n( "Sorry, you can only insert footnotes or\n"
                                  "endnotes into the first frameset."),
                            i18n("Insert Footnote/Endnote"));
    } else {
        KWFootNoteDia dia( 0L, "", doc, gui->canvasWidget(), start,
                 doc->getNoteType() == KWFootNoteManager::FootNotes );
        dia.show();
    }
#endif
}

/*===============================================================*/
void KWView::insertContents()
{
    doc->createContents();
    gui->canvasWidget()->repaintAll();
}

/*===============================================================*/
void KWView::formatFont()
{
    kdDebug() << "KWView::formatFont" << endl;
    QFont tmpFont = tbFont;

    if ( KFontDialog::getFont( tmpFont ) ) {
        tbFont = tmpFont;
        KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
        if ( edit )
            edit->setFont(tbFont);
    }

#if 0
    QFont tmpFont = tbFont;

    if ( KFontDialog::getFont( tmpFont ) ) {
        tbFont = tmpFont;
        format.setUserFont( doc->findUserFont( tbFont.family() ) );
        format.setPTFontSize( tbFont.pointSize() );
        format.setWeight( tbFont.weight() );
        format.setItalic( tbFont.italic() );
        format.setUnderline( tbFont.underline() );
        actionFormatFontFamily->setFont( tbFont.family() );
        actionFormatFontSize->setFontSize( tbFont.pointSize() );
        actionFormatBold->setChecked( tbFont.bold() );
        actionFormatItalic->setChecked( tbFont.italic() );
        actionFormatUnderline->setChecked( tbFont.underline() );
        if ( gui ) {
            gui->canvasWidget()->formatChanged( format );
#endif
            gui->canvasWidget()->setFocus();
#if 0
        }
    }
#endif
}

/*===============================================================*/
void KWView::formatParagraph()
{
    KWTextFrameSetEdit *edit = dynamic_cast<KWTextFrameSetEdit *>( gui->canvasWidget()->currentFrameSetEdit() );
    if (edit) // TODO disable action if not text frameset
    {
        KWParagDia *paragDia = new KWParagDia( this, "", fontList,
                                               KWParagDia::PD_SPACING | KWParagDia::PD_ALIGN |
                                               KWParagDia::PD_BORDERS |
                                               KWParagDia::PD_NUMBERING | KWParagDia::PD_TABS, doc );
        paragDia->setCaption( i18n( "Paragraph settings" ) );
        connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );

        // Initialize the dialog from the current paragraph's settings
        KWParagLayout lay = static_cast<KWTextParag *>(edit->getCursor()->parag())->createParagLayout();
        paragDia->setParagLayout( lay );
        paragDia->show();
        delete paragDia;
    }
}

/*===============================================================*/
void KWView::formatPage()
{
    if( !doc->isReadWrite())
        return;
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter kwhf;
    doc->getPageLayout( pgLayout, cl, kwhf );

    pageLayout tmpOldLayout;
    tmpOldLayout._pgLayout=pgLayout;
    tmpOldLayout._cl=cl;
    tmpOldLayout._hf=kwhf;

    KoHeadFoot hf;
    int flags = FORMAT_AND_BORDERS | KW_HEADER_AND_FOOTER | DISABLE_UNIT;
    if ( doc->processingType() == KWDocument::WP )
        flags = flags | COLUMNS;
    else
        flags = flags | DISABLE_BORDERS;

    if ( KoPageLayoutDia::pageLayout( pgLayout, hf, cl, kwhf, flags ) ) {

        pageLayout tmpNewLayout;
        tmpNewLayout._pgLayout=pgLayout;
        tmpNewLayout._cl=cl;
        tmpNewLayout._hf=kwhf;

        KWPageLayoutCommand *cmd =new KWPageLayoutCommand( i18n("Change Layout"),doc,gui,tmpOldLayout,tmpNewLayout ) ;
        doc->addCommand(cmd);

        doc->setPageLayout( pgLayout, cl, kwhf );
        gui->getVertRuler()->setPageLayout( pgLayout );
        gui->getHorzRuler()->setPageLayout( pgLayout );
        gui->canvasWidget()->repaintAll(true);
        KWFrame *frame=doc->getFirstSelectedFrame();
        if(frame!=0L && frame->getFrameSet() && frame->getFrameSet()->getFrameType() == FT_TEXT)
        {
            if(doc->processingType()  == KWDocument::WP && frame->getFrameSet() == doc->getFrameSet(0)&& frame->isSelected())
                frame->setSelected(true);
        }
#if 0
        gui->canvasWidget()->frameSizeChanged( pgLayout );
#endif
    }
}

/*===============================================================*/
void KWView::formatFrameSet()
{
    if ( doc->getFirstSelectedFrame() )
      gui->canvasWidget()->editFrameProperties();
    else // TODO enable/disable the action depending on whether a frame is selected, instead
        KMessageBox::sorry( this,
                            i18n("Sorry, you have to select a frame first."),
                            i18n("Format Frameset"));
}

/*===============================================================*/
void KWView::extraSpelling()
{
    if (kspell) return; // Already in progress
    //currParag = 0L;
    currFrameSetNum = -1;
    kspell = new KSpell( this, i18n( "Spell Checking" ), this, SLOT( spellCheckerReady() ), getGUI()->getDocument()->getKSpellConfig());

    QObject::connect( kspell, SIGNAL( death() ),
                      this, SLOT( spellCheckerFinished() ) );
    QObject::connect( kspell, SIGNAL( misspelling( QString, QStringList*, unsigned ) ),
                      this, SLOT( spellCheckerMisspelling( QString, QStringList*, unsigned ) ) );
    QObject::connect( kspell, SIGNAL( corrected( QString, QString, unsigned ) ),
                      this, SLOT( spellCheckerCorrected( QString, QString, unsigned ) ) );
    QObject::connect( kspell, SIGNAL( done( const QString & ) ), this, SLOT( spellCheckerDone( const QString & ) ) );
}

/*===============================================================*/
void KWView::extraAutoFormat()
{

    KWAutoFormatDia dia( this, "", doc, gui->canvasWidget() );
    dia.show();
}

/*===============================================================*/
void KWView::extraStylist()
{
  //doc->setSelection(false);
    KWStyleManager * styleManager = new KWStyleManager( this, doc, fontList );
    connect( styleManager, SIGNAL( okClicked() ), this, SLOT( styleManagerOk() ) );
    styleManager->setCaption( i18n( "Stylist" ) );
    styleManager->show();
    delete styleManager;
}

/*===============================================================*/
void KWView::extraCreateTemplate()
{
    QPixmap pix( 45, 60 );
    pix.fill( Qt::white );

    KTempFile tempFile;
    tempFile.setAutoDelete(true);

    doc->saveNativeFormat( tempFile.name() );

    KoTemplateCreateDia::createTemplate( "kword_template", KWFactory::global(),
                                         tempFile.name(), pix, this );

    KWFactory::global()->dirs()->addResourceType("kword_template",
                                                    KStandardDirs::kde_default( "data" ) +
                                                    "kword/templates/");
}

/*===============================================================*/
void KWView::extraOptions()
{
}

/*===============================================================*/
void KWView::toolsEdit()
{
    if ( actionToolsEdit->isChecked() )
        gui->canvasWidget()->setMouseMode( MM_EDIT );
}

/*===============================================================*/
void KWView::toolsEditFrame()
{
    if ( actionToolsEditFrames->isChecked() )
        gui->canvasWidget()->setMouseMode( MM_EDIT_FRAME );
}

/*===============================================================*/
void KWView::toolsCreateText()
{
    if ( actionToolsCreateText->isChecked() )
        gui->canvasWidget()->setMouseMode( MM_CREATE_TEXT );
}

/*===============================================================*/
void KWView::toolsCreatePix()
{
    if ( !actionToolsCreatePix->isChecked() )
        return;
    gui->canvasWidget()->setMouseMode( MM_EDIT );

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
        gui->canvasWidget()->setMouseMode( MM_CREATE_PIX );
        gui->canvasWidget()->setPixmapFilename( file );
    } else
        gui->canvasWidget()->setMouseMode( MM_EDIT );
}

/*===============================================================*/
void KWView::toolsTable()
{
    if ( !actionToolsCreateTable->isChecked() )
        return;
    KWTableDia *tableDia = new KWTableDia( this, "", gui->canvasWidget(), doc,
                               gui->canvasWidget()->tableRows(),
                               gui->canvasWidget()->tableCols(),
                               gui->canvasWidget()->tableWidthMode(),
                               gui->canvasWidget()->tableHeightMode() );
    tableDia->setCaption( i18n( "Insert Table" ) );
    tableDia->show();
    delete tableDia;
}

/*===============================================================*/
void KWView::toolsFormula()
{
    if ( !actionToolsCreateFormula->isChecked() )
        return;
    gui->canvasWidget()->setMouseMode( MM_CREATE_FORMULA );
}

/*===============================================================*/
void KWView::toolsPart()
{
    if ( !actionToolsCreatePart->isChecked() )
        return;
    gui->canvasWidget()->setMouseMode( MM_EDIT );

    KoDocumentEntry pe = KoPartSelectDia::selectPart( this );
    if ( pe.isEmpty() )
        return;

    gui->canvasWidget()->setMouseMode( MM_CREATE_PART );
    gui->canvasWidget()->setPartEntry( pe );
}

/*===============================================================*/
void KWView::tableInsertRow()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT );
    KWGroupManager *grpMgr = gui->canvasWidget()->getTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before inserting a new row." ),
                            i18n( "Insert Row" ) );
    } else {
        KWInsertDia dia( this, "", grpMgr, doc, KWInsertDia::ROW, gui->canvasWidget() );
        dia.setCaption( i18n( "Insert Row" ) );
        dia.show();
    }
}

/*===============================================================*/
void KWView::tableInsertCol()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT );
    KWGroupManager *grpMgr = gui->canvasWidget()->getTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before inserting a new column." ),
                            i18n( "Insert Column" ) );
    } else {
        // value = 62 because a insert column = 60 +2 (border )see kwgroupmanager.cc
        if ( grpMgr->getBoundingRect().right() + 62 > static_cast<int>( doc->ptPaperWidth() ) )
        {
            KMessageBox::sorry( this,
                            i18n( "There is not enough space at the right of the table\n"
                                  "to insert a new column." ),
                            i18n( "Insert Column" ) );
        }
        else
        {
            KWInsertDia dia( this, "", grpMgr, doc, KWInsertDia::COL, gui->canvasWidget() );
            dia.setCaption( i18n( "Insert Column" ) );
            dia.show();
        }
    }
}

/*===============================================================*/
void KWView::tableDeleteRow()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT );

    KWGroupManager *grpMgr = gui->canvasWidget()->getTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before deleting a row." ),
                            i18n( "Delete Row" ) );
    }
    else
    {
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
                gui->canvasWidget()->deleteTable( grpMgr );
            }
        }
        else
        {
            KWDeleteDia dia( this, "", grpMgr, doc, KWDeleteDia::ROW, gui->canvasWidget() );
            dia.setCaption( i18n( "Delete Row" ) );
            dia.show();
        }
    }

}

/*===============================================================*/
void KWView::tableDeleteCol()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT );

    KWGroupManager *grpMgr = gui->canvasWidget()->getTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before deleting a column." ),
                            i18n( "Delete Column" ) );
    }
    else
    {
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
                gui->canvasWidget()->deleteTable( grpMgr );
            }
        }
        else
        {
            KWDeleteDia dia( this, "", grpMgr, doc, KWDeleteDia::COL, gui->canvasWidget() );
            dia.setCaption( i18n( "Delete Column" ) );
            dia.show();
        }
    }
}

/*===============================================================*/
void KWView::tableJoinCells()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT_FRAME );

    KWGroupManager *grpMgr = gui->canvasWidget()->getCurrentTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before joining cells." ),
                            i18n( "Join Cells" ) );
    } else {
        QPainter painter;
        painter.begin( gui->canvasWidget() );
        if ( !grpMgr->joinCells() )
        {
            KMessageBox::sorry( this,
                                i18n( "You have to select some cells which are next to each other\n"
                                      "and are not already joined." ),
                                i18n( "Join Cells" ) );
        }
        painter.end();
        QRect r = grpMgr->getBoundingRect();
        r = QRect( r.x() - gui->canvasWidget()->contentsX(),
                   r.y() - gui->canvasWidget()->contentsY(),
                   r.width(), r.height() );
        //gui->canvasWidget()->repaintScreen( r, TRUE );
        gui->canvasWidget()->repaintAll();
    }
}

/*===============================================================*/
void KWView::tableSplitCells()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT_FRAME );

    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    KWGroupManager *grpMgr = gui->canvasWidget()->getCurrentTable();
    if ( !grpMgr && selectedFrames.count() > 0) {
        grpMgr=selectedFrames.at(0)->getFrameSet()->getGroupManager();
    }

    if(selectedFrames.count() >1 || grpMgr == 0) {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                            //i18n( "You have to select one table cell\n"
                                  "before splitting cells." ),
                            i18n( "Split Cells" ) );
        return;
    }

    QPainter painter;
    painter.begin( gui->canvasWidget() );
    int rows=1, cols=2;
    if ( !grpMgr->splitCell(rows,cols) ) {
        KMessageBox::sorry( this,
                            i18n("You have to select a joined cell."),
                            //i18n("There is not enough space to split the cell into that many parts"),
                            i18n("Split Cells") );
    }
    painter.end();
    QRect r = grpMgr->getBoundingRect();
    r = QRect( r.x() - gui->canvasWidget()->contentsX(),
               r.y() - gui->canvasWidget()->contentsY(),
               r.width(), r.height() );
    //gui->canvasWidget()->repaintScreen( r, TRUE );
     gui->canvasWidget()->repaintAll();

}

/*===============================================================*/
void KWView::tableUngroupTable()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT );

    KWGroupManager *grpMgr = gui->canvasWidget()->getTable();
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
            r = QRect( r.x() - gui->canvasWidget()->contentsX(),
                       r.y() - gui->canvasWidget()->contentsY(),
                       r.width(), r.height() );

            //gui->canvasWidget()->repaintScreen( r, TRUE );
            doc->delGroupManager(grpMgr);
        }
        gui->canvasWidget()->repaintAll();
    }
}

/*===============================================================*/
void KWView::tableDelete()
{
    KWGroupManager *grpMgr = gui->canvasWidget()->getTable();
    if ( !grpMgr )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table \n"
                                  "or select it to delete it!" ),
                            i18n( "Delete Table" ) );
    }
    else
    {
        gui->canvasWidget()->deleteTable( grpMgr );
    }
}

/*====================== text style selected  ===================*/
void KWView::textStyleSelected( int index )
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->applyStyle( doc->styleAt( index ) );
    gui->canvasWidget()->setFocus(); // the combo keeps focus...
}

/*======================= text size selected  ===================*/
void KWView::textSizeSelected( int size )
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setPointSize( size );
    gui->canvasWidget()->setFocus(); // the combo keeps focus...
}

/*======================= text font selected  ===================*/
void KWView::textFontSelected( const QString & font )
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setFamily( font );
    gui->canvasWidget()->setFocus(); // the combo keeps focus...
}

/*========================= text bold ===========================*/
void KWView::textBold()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setBold(actionFormatBold->isChecked());
}

/*========================== text italic ========================*/
void KWView::textItalic()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setItalic(actionFormatItalic->isChecked());
}

/*======================== text underline =======================*/
void KWView::textUnderline()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setUnderline(actionFormatUnderline->isChecked());
}

/*=========================== text color ========================*/
void KWView::textColor()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
    {
        QColor color = edit->textColor();
        if ( KColorDialog::getColor( color ) ) {
            actionFormatColor->setColor( color );
            edit->setTextColor( color );
        }
    }
}

/*======================= text align left =======================*/
void KWView::textAlignLeft()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setAlign(Qt::AlignLeft);
}

/*======================= text align center =====================*/
void KWView::textAlignCenter()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setAlign(Qt::AlignCenter);
}

/*======================= text align right ======================*/
void KWView::textAlignRight()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setAlign(Qt::AlignRight);
}

/*======================= text align block ======================*/
void KWView::textAlignBlock()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setAlign(Qt3::AlignJustify);
}

/*====================== enumerated list ========================*/
void KWView::textEnumList()
{
    Counter c;
    if(actionFormatEnumList->isChecked())
    {
        c.setNumbering( Counter::NUM_LIST );
        c.setStyle( Counter::STYLE_NUM );
    }
    else
    {
        c.setNumbering( Counter::NUM_NONE );
    }
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    ASSERT(edit);
    if ( edit )
        edit->setCounter( c );
}

/*====================== unsorted list ==========================*/
void KWView::textUnsortList()
{
    Counter c;
    if(actionFormatUnsortList->isChecked())
    {
        c.setNumbering( Counter::NUM_LIST );
        c.setStyle( Counter::STYLE_DISCBULLET );
    }
    else
    {
        c.setNumbering( Counter::NUM_NONE );
    }
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
    ASSERT(edit);
        edit->setCounter( c );
}

/*===============================================================*/
void KWView::textSuperScript()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setTextSuperScript(actionFormatSuper->isChecked());
}

/*===============================================================*/
void KWView::textSubScript()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setTextSubScript(actionFormatSub->isChecked());
}

void KWView::textBorderOutline()
{
    if ( actionFormatBrdOutline->isChecked() )
    {
        left = tmpBrd;
        right = tmpBrd;
        top =tmpBrd;
        bottom = tmpBrd;
    }
    else
    {
        left.ptWidth = 0;
        right.ptWidth = 0;
        top.ptWidth = 0;
        bottom.ptWidth = 0;
    }
    actionFormatBrdRight->setChecked(actionFormatBrdOutline->isChecked());
    actionFormatBrdLeft->setChecked(actionFormatBrdOutline->isChecked());
    actionFormatBrdTop->setChecked(actionFormatBrdOutline->isChecked());
    actionFormatBrdBottom->setChecked(actionFormatBrdOutline->isChecked());
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setBorders( left, right, bottom, top );
}

/*===============================================================*/
void KWView::textBorderLeft()
{
    if ( actionFormatBrdLeft->isChecked() )
        left = tmpBrd;
    else
        left.ptWidth = 0;
    actionFormatBrdOutline->setChecked(actionFormatBrdRight->isChecked()&&
				       actionFormatBrdLeft->isChecked()&&
				       actionFormatBrdTop->isChecked()&&
				       actionFormatBrdBottom->isChecked());

    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setBorders( left, right, bottom, top );
}

/*===============================================================*/
void KWView::textBorderRight()
{

    if ( actionFormatBrdRight->isChecked() )
        right = tmpBrd;
    else
        right.ptWidth = 0;
    actionFormatBrdOutline->setChecked(actionFormatBrdRight->isChecked()&&
				       actionFormatBrdLeft->isChecked()&&
				       actionFormatBrdTop->isChecked()&&
				       actionFormatBrdBottom->isChecked());

    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setBorders( left, right, bottom, top );
}

/*===============================================================*/
void KWView::textBorderTop()
{

    if ( actionFormatBrdTop->isChecked() )
        top = tmpBrd;
    else
        top.ptWidth = 0;
    actionFormatBrdOutline->setChecked(actionFormatBrdRight->isChecked()&&
				       actionFormatBrdLeft->isChecked()&&
				       actionFormatBrdTop->isChecked()&&
				       actionFormatBrdBottom->isChecked());

    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setBorders( left, right, bottom, top );
}

/*===============================================================*/
void KWView::textBorderBottom()
{

    if ( actionFormatBrdBottom->isChecked() )
        bottom = tmpBrd;
    else
        bottom.ptWidth = 0;
    actionFormatBrdOutline->setChecked(actionFormatBrdRight->isChecked()&&
				       actionFormatBrdLeft->isChecked()&&
				       actionFormatBrdTop->isChecked()&&
				       actionFormatBrdBottom->isChecked());

    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if ( edit )
        edit->setBorders( left, right, bottom, top );
}

/*================================================================*/
void KWView::textBorderColor()
{
    tmpBrd.color = actionFormatBrdColor->color();
}

/*================================================================*/
void KWView::textBorderWidth( const QString &width )
{
    tmpBrd.ptWidth = width.toInt();
}

/*================================================================*/
void KWView::textBorderStyle( const QString &style )
{
    QString stl = style;

    if ( stl == i18n( "solid line" ) )
        tmpBrd.style = Border::SOLID;
    else if ( stl == i18n( "dash line ( ---- )" ) )
        tmpBrd.style = Border::DASH;
    else if ( stl == i18n( "dot line ( **** )" ) )
        tmpBrd.style = Border::DOT;
    else if ( stl == i18n( "dash dot line ( -*-* )" ) )
        tmpBrd.style = Border::DASH_DOT;
    else if ( stl == i18n( "dash dot dot line ( -**- )" ) )
        tmpBrd.style = Border::DASH_DOT_DOT;
}

/*================================================================*/
void KWView::frameBorderOutline()
{
    bool b = actionFrameBrdOutline->isChecked();
    gui->canvasWidget()->setOutlineFrameBorder( frmBrd, b );
    actionFrameBrdLeft->setChecked(b);
    actionFrameBrdRight->setChecked(b);
    actionFrameBrdTop->setChecked(b);
    actionFrameBrdBottom->setChecked(b);
}

/*================================================================*/
void KWView::frameBorderLeft()
{
    bool b = actionFrameBrdLeft->isChecked();
    gui->canvasWidget()->setLeftFrameBorder( frmBrd, b );
}

/*================================================================*/
void KWView::frameBorderRight()
{
    bool b = actionFrameBrdRight->isChecked();
    gui->canvasWidget()->setRightFrameBorder( frmBrd, b );
    verifyFrameOutline();
}

/*================================================================*/
void KWView::frameBorderTop()
{
    bool b = actionFrameBrdTop->isChecked();
    gui->canvasWidget()->setTopFrameBorder( frmBrd, b );
    verifyFrameOutline();
}

/*================================================================*/
void KWView::frameBorderBottom()
{
    bool b = actionFrameBrdBottom->isChecked();
    gui->canvasWidget()->setBottomFrameBorder( frmBrd, b );
    verifyFrameOutline();
}

/*================================================================*/
void KWView::frameBorderColor()
{
    frmBrd.color=actionFrameBrdColor->color();
    //gui->canvasWidget()->setFrameBorderColor( frmBrd.color );
}

/*================================================================*/
void KWView::frameBorderWidth( const QString &width )
{
    frmBrd.ptWidth = width.toInt();
}

/*================================================================*/
void KWView::frameBorderStyle( const QString &style )
{
    QString stl = style;
    if ( stl == i18n( "solid line" ) )
        frmBrd.style = Border::SOLID;
    else if ( stl == i18n( "dash line ( ---- )" ) )
        frmBrd.style = Border::DASH;
    else if ( stl == i18n( "dot line ( **** )" ) )
        frmBrd.style = Border::DOT;
    else if ( stl == i18n( "dash dot line ( -*-* )" ) )
        frmBrd.style = Border::DASH_DOT;
    else if ( stl == i18n( "dash dot dot line ( -**- )" ) )
        frmBrd.style = Border::DASH_DOT_DOT;
}

/*================================================================*/
void KWView::frameBackColor()
{
    backColor=actionFrameBackColor->color();
    if ( gui )
        gui->canvasWidget()->setFrameBackgroundColor( backColor );
}

/*================================================================*/
void KWView::verifyFrameOutline()
{
    actionFrameBrdOutline->setChecked(actionFrameBrdBottom->isChecked()&&
                                      actionFrameBrdTop->isChecked()&&
                                      actionFrameBrdLeft->isChecked()&&
                                      actionFrameBrdRight->isChecked());
}

/*================================================================*/
void KWView::formulaPower()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::POWER );
#endif
}

/*================================================================*/
void KWView::formulaSubscript()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::SUB );
#endif
}

/*================================================================*/
void KWView::formulaParentheses()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::PAREN );
#endif
}

/*================================================================*/
void KWView::formulaAbsValue()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::ABS );
#endif
}

/*================================================================*/
void KWView::formulaBrackets()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::BRACKET );
#endif
}

/*================================================================*/
void KWView::formulaFraction()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::DIVIDE );
#endif
}

/*================================================================*/
void KWView::formulaRoot()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::SQRT );
#endif
}

/*================================================================*/
void KWView::formulaIntegral()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::INTEGRAL );
#endif
}

/*================================================================*/
void KWView::formulaMatrix()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::MATRIX );
#endif
}

/*================================================================*/
void KWView::formulaLeftSuper()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::LSUP );
#endif
}

/*================================================================*/
void KWView::formulaLeftSub()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::LSUB );
#endif
}

/*================================================================*/
void KWView::formulaProduct()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::PRODUCT );
#endif
}

/*================================================================*/
void KWView::formulaSum()
{
#if 0
    gui->canvasWidget()->insertFormulaChar( Box::SUM );
#endif
}

/*================================================================*/
void KWView::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    if ( gui ) gui->resize( width(), height() );
}

#if 0
/*================================================================*/
void KWView::keyPressEvent( QKeyEvent *e )
{
    QApplication::sendEvent( gui, e );
}

/*================================================================*/
void KWView::keyReleaseEvent( QKeyEvent */*e*/ )
{
    //    QApplication::sendEvent( gui, e );
}

/*================================================================*/
void KWView::mousePressEvent( QMouseEvent *e )
{
    // My guess is that this is NEVER called...
    kdDebug() << "KWView::mousePressEvent" << endl;
    if ( gui )
        QApplication::sendEvent( gui->canvasWidget(), e );
}

/*================================================================*/
void KWView::mouseMoveEvent( QMouseEvent *e )
{
    if ( gui )
        QApplication::sendEvent( gui->canvasWidget(), e );
}

/*================================================================*/
void KWView::mouseReleaseEvent( QMouseEvent *e )
{
    if ( gui )
        QApplication::sendEvent( gui->canvasWidget(), e );
}

/*================================================================*/
void KWView::focusInEvent( QFocusEvent *e )
{
    if ( gui )
        QApplication::sendEvent( gui->canvasWidget(), e );
}

/*================================================================*/
void KWView::dragEnterEvent( QDragEnterEvent *e )
{
    QApplication::sendEvent( gui, e );
}

/*================================================================*/
void KWView::dragMoveEvent( QDragMoveEvent *e )
{
    QApplication::sendEvent( gui, e );
}

/*================================================================*/
void KWView::dragLeaveEvent( QDragLeaveEvent *e )
{
    QApplication::sendEvent( gui, e );
}

/*================================================================*/
void KWView::dropEvent( QDropEvent *e )
{
    QApplication::sendEvent( gui, e );
}
#endif

void KWView::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    if ( ev->activated() )
    {
        initGui();
    }
    else
    {
        //remove item when you desactivate gui
        statusBar()->removeItem(statusPage);
    }
    KoView::guiActivateEvent( ev );
}

/*================================================================*/
void KWView::showParagBorderValues()
{
    actionFormatBrdWidth->setCurrentItem( tmpBrd.ptWidth - 1 );
    actionFormatBrdStyle->setCurrentItem( (int)tmpBrd.style );
}

/*================================================================*/
void KWView::slotInsertObject( KWChild *, KWPartFrameSet * )
{
}

/*================================================================*/
void KWView::slotUpdateChildGeometry( KWChild */*_child*/ )
{
}

/*================================================================*/
void KWView::paragDiaOk()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if (!edit) return;
    KWUnits unit = KWUnit::unitType( doc->getUnit() );
    const KWParagDia * paragDia = static_cast<const KWParagDia*>(sender());
    // TODO a macro command with all the changes in it !
    // undo should do all in one step.
    if(paragDia->isLeftMarginChanged())
    {
	edit->setMargin( QStyleSheetItem::MarginLeft, paragDia->leftIndent() );
	gui->getHorzRuler()->setLeftIndent( paragDia->leftIndent().value( unit ) );
    }

    if(paragDia->isRightMarginChanged())
    {
	edit->setMargin( QStyleSheetItem::MarginRight, paragDia->rightIndent() );
	//koRuler doesn't support setRightIndent
	//gui->getHorzRuler()->setRightIndent( paragDia->rightIndent().value( unit ) );
    }
    if(paragDia->isSpaceBeforeChanged())
        edit->setMargin( QStyleSheetItem::MarginTop, paragDia->spaceBeforeParag() );

    if(paragDia->isSpaceAfterChanged())
        edit->setMargin( QStyleSheetItem::MarginBottom, paragDia->spaceAfterParag() );

    if(paragDia->isFirstLineChanged())
    {
	edit->setMargin( QStyleSheetItem::MarginFirstLine, paragDia->firstLineIndent());
	KWUnit relativeValue = KWUnit::createUnit((paragDia->leftIndent().value( unit )+paragDia->firstLineIndent().value( unit )) , unit );
	gui->getHorzRuler()->setFirstIndent( relativeValue.value( unit ) );
    }

    if(paragDia->isAlignChanged())
        edit->setAlign( paragDia->align() );

    if(paragDia->isBulletChanged())
        edit->setCounter( paragDia->counter() );

    if(paragDia->listTabulatorChanged())
        edit->setTabList(paragDia->tabListTabulator());


    if(paragDia->isLineSpacingChanged())
        edit->setLineSpacing( paragDia->lineSpacing() );

    if(paragDia->isBorderChanged())
        edit->setBorders( paragDia->leftBorder(), paragDia->rightBorder(),
                          paragDia->bottomBorder(), paragDia->topBorder() );
}

/*================================================================*/
void KWView::tabListChanged( QList<KoTabulator>*_tabList )
{
    if(!doc->isReadWrite())
        return;
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if (!edit)
        return;
    edit->setTabList(_tabList);
}

/*================================================================*/
void KWView::styleManagerOk()
{
}

/*================================================================*/
void KWView::newPageLayout( KoPageLayout _layout )
{
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    doc->getPageLayout( pgLayout, cl, hf );

    pageLayout tmpOldLayout;
    tmpOldLayout._pgLayout=pgLayout;
    tmpOldLayout._cl=cl;
    tmpOldLayout._hf=hf;

    doc->setPageLayout( _layout, cl, hf );

    pageLayout tmpNewLayout;
    tmpNewLayout._pgLayout=_layout;
    tmpNewLayout._cl=cl;
    tmpNewLayout._hf=hf;

    KWPageLayoutCommand *cmd =new KWPageLayoutCommand( i18n("Change Layout"),doc,gui,tmpOldLayout,tmpNewLayout ) ;
    doc->addCommand(cmd);


    gui->getHorzRuler()->setPageLayout( _layout );
    gui->getVertRuler()->setPageLayout( _layout );
    gui->canvasWidget()->repaintAll(true);

    KWFrame *frame=doc->getFirstSelectedFrame();
    if(frame!=0L && frame->getFrameSet() && frame->getFrameSet()->getFrameType() == FT_TEXT)
        {
            if(doc->processingType()  == KWDocument::WP && frame->getFrameSet() == doc->getFrameSet(0)&& frame->isSelected())
                frame->setSelected(true);
        }
#if 0
    gui->canvasWidget()->frameSizeChanged( _layout );
    gui->canvasWidget()->forceFullUpdate();
#endif

}

/*================================================================*/
void KWView::newFirstIndent( double _firstIndent )
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if (!edit) return;
    KWUnit u;
    double val = _firstIndent - edit->currentParagLayout().margins[QStyleSheetItem::MarginLeft].pt();
    u.setPT( val );
    edit->setMargin( QStyleSheetItem::MarginFirstLine, u);
}

/*================================================================*/
void KWView::newLeftIndent( double _leftIndent)
{
    KWUnit u;
    u.setPT( _leftIndent );
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if (edit)
        edit->setMargin( QStyleSheetItem::MarginLeft, u );
}

/*================================================================*/
void KWView::openPopupMenuEditText( const QPoint & _point )
{
  if(!koDocument()->isReadWrite() )
    return;
   KWTextFrameSetEdit *edit = dynamic_cast<KWTextFrameSetEdit *>( gui->canvasWidget()->currentFrameSetEdit() );
    if (edit)
        ((QPopupMenu*)factory()->container("text_popup",this))->popup(_point);
}

/*================================================================*/
void KWView::openPopupMenuChangeAction( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() )
        return;
    ((QPopupMenu*)factory()->container("action_popup",this))->popup(_point);
}

/*================================================================*/
void KWView::updatePopupMenuChangeAction()
{
    KWFrame *frame=doc->getFirstSelectedFrame();
    // if a header/footer etc. Dont show the popup.
    if(frame->getFrameSet() && frame->getFrameSet()->getFrameInfo() != FI_BODY)
        return;

    // enable delete
    actionEditDelFrame->setEnabled(true);

    // if text frame,
    if(frame->getFrameSet() && frame->getFrameSet()->getFrameType() == FT_TEXT)
        {
            // if frameset 0 disable delete
            if(doc->processingType()  == KWDocument::WP && frame->getFrameSet() == doc->getFrameSet(0))
                {
                    actionEditReconnectFrame->setEnabled(false);
                    actionEditDelFrame->setEnabled(false);
                }
            else
                {
                    actionEditReconnectFrame->setEnabled(true);
                }
        }
    else
        actionEditReconnectFrame->setEnabled(false);

    if(gui->canvasWidget()->getCurrentTable())
        actionEditReconnectFrame->setEnabled(false);
}

/*================================================================*/
void KWView::openPopupMenuEditFrame( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() )
        return;
    updatePopupMenuChangeAction();
    ((QPopupMenu*)factory()->container("frame_popup",this))->popup(_point);
}

/*================================================================*/
void KWView::spellCheckerReady()
{
    // #### currently only the first available textframeset is checked!!
    currParag = 0;
    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        KWFrameSet *frameset = doc->getFrameSet( i );
        if ( frameset->getFrameType() != FT_TEXT )
            continue;
        currFrameSetNum = i;
        KWTextFrameSet *tmpParag = dynamic_cast<KWTextFrameSet*> (frameset) ;
        currParag = dynamic_cast<KWTextParag*> (tmpParag->textDocument()->firstParag()) ;
        break;
    }

    if ( !currParag ) {
        kspell->cleanUp();
        return;
    }

    QString text;
    KWTextParag *p = currParag;
    while ( currParag ) {
        text += currParag->string()->toString() + "\n";
        currParag = static_cast<KWTextParag *>(currParag->next());
    }
    text += "\n";
    currParag = p;
    lastTextPos = 0;
    kspell->check( text );
}

/*================================================================*/
void KWView::spellCheckerMisspelling( QString , QStringList* , unsigned )
{
}

/*================================================================*/
void KWView::spellCheckerCorrected( QString old, QString corr, unsigned )
{
    if ( !currParag )
        return;

    QString text;
    while ( currParag ) {
        text = currParag->string()->toString();
        int pos = text.find( old, lastTextPos );
        if ( pos != -1 ) {
            QTextFormat *f = currParag->paragFormat();
            currParag->remove( pos, old.length() );
            currParag->insert( pos, corr );
            currParag->setFormat( pos,old.length(),f,true );
            lastTextPos = pos + corr.length();
            break;
        }
        currParag = static_cast<KWTextParag *>(currParag->next());
        lastTextPos = 0;
    }
}

/*================================================================*/
void KWView::spellCheckerDone( const QString & )
{
    //gui->canvasWidget()->recalcWholeText();
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if (edit)
        edit->drawCursor( TRUE );
    kspell->cleanUp();
    spellCheckerFinished();
}

/*================================================================*/
void KWView::spellCheckerFinished( )
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
        //gui->canvasWidget()->recalcWholeText();
    }
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit());
    if (edit)
        edit->drawCursor( TRUE );
}

/*================================================================*/
void KWView::configure( )
{
    KWConfig *configDia = new KWConfig( this, "");
    configDia->show();
}

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
KWGUI::KWGUI( QWidget *parent, bool, KWDocument *_doc, KWView *_view )
    : QWidget( parent, "" )
{
    doc = _doc;
    view = _view;

    r_horz = r_vert = 0;

    panner = new QSplitter( Qt::Horizontal, this );
    docStruct = new KWDocStruct( panner, doc, this );
    docStruct->setMinimumWidth( 0 );
    left = new KWLayoutWidget( panner, this );
    left->show();
    canvas = new KWCanvas( left, doc, this );

    QValueList<int> l;
    l << 0;
    panner->setSizes( l );

    KoPageLayout layout = doc->pageLayout();

    tabChooser = new KoTabChooser( left, KoTabChooser::TAB_ALL );

    r_horz = new KoRuler( left, canvas->viewport(), Qt::Horizontal, layout,
                          KoRuler::F_INDENTS | KoRuler::F_TABS, tabChooser );
    r_horz->setReadWrite(doc->isReadWrite());
    r_vert = new KoRuler( left, canvas->viewport(), Qt::Vertical, layout, 0 );
    connect( r_horz, SIGNAL( newPageLayout( KoPageLayout ) ), view, SLOT( newPageLayout( KoPageLayout ) ) );
    r_vert->setReadWrite(doc->isReadWrite());

    r_horz->setZoom( doc->zoomedResolutionX() );
    r_vert->setZoom( doc->zoomedResolutionY() );

    connect( r_horz, SIGNAL( newLeftIndent( double ) ), view, SLOT( newLeftIndent( double ) ) );
    connect( r_horz, SIGNAL( newFirstIndent( double ) ), view, SLOT( newFirstIndent( double ) ) );

    connect( r_horz, SIGNAL( openPageLayoutDia() ), view, SLOT( openPageLayoutDia() ) );
    connect( r_horz, SIGNAL( unitChanged( QString ) ), this, SLOT( unitChanged( QString ) ) );
    connect( r_vert, SIGNAL( newPageLayout( KoPageLayout ) ), view, SLOT( newPageLayout( KoPageLayout ) ) );
    connect( r_vert, SIGNAL( openPageLayoutDia() ), view, SLOT( openPageLayoutDia() ) );
    connect( r_vert, SIGNAL( unitChanged( QString ) ), this, SLOT( unitChanged( QString ) ) );

    r_horz->setUnit( doc->getUnit() );
    r_vert->setUnit( doc->getUnit() );

#if 0
    switch ( KWUnit::unitType( doc->getUnit() ) ) {
    case U_MM:
        r_horz->setLeftIndent( canvas->getLeftIndent().mm() );
        r_horz->setFirstIndent( canvas->getFirstLineIndent().mm() );
        break;
    case U_INCH:
        r_horz->setLeftIndent( canvas->getLeftIndent().inch() );
        r_horz->setFirstIndent( canvas->getFirstLineIndent().inch() );
        break;
    case U_PT:
        r_horz->setLeftIndent( canvas->getLeftIndent().pt() );
        r_horz->setFirstIndent( canvas->getFirstLineIndent().pt() );
        break;
    }
#endif

    r_horz->hide();
    r_vert->hide();


    canvas->show();
    docStruct->show();

    reorganize();

#if 0
    if ( doc->processingType() == KWDocument::DTP )
        canvas->setRuler2Frame( 0, 0 );
#endif

    connect( r_horz, SIGNAL( tabListChanged( QList<KoTabulator>* ) ), view,
             SLOT( tabListChanged( QList<KoTabulator>* ) ) );

#if 0
    canvas->forceFullUpdate();
#endif

    setKeyCompression( TRUE );
    setAcceptDrops( TRUE );
    setFocusPolicy( QWidget::NoFocus );

    canvas->setContentsPos( 0, 0 );
}

/*================================================================*/
void KWGUI::showGUI()
{
    reorganize();
}

/*================================================================*/
void KWGUI::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    reorganize();
}

/*================================================================*/
void KWGUI::reorganize()
{
    r_vert->show();
    r_horz->show();
    tabChooser->show();

    tabChooser->setGeometry( 0, 0, 20, 20 );

    panner->setGeometry( 0, 0, width(), height() );
    canvas->setGeometry( 20, 20, left->width() - 20, left->height() - 20 );
    r_horz->setGeometry( 20, 0, left->width() - 20, 20 );
    r_vert->setGeometry( 0, 20, 20, left->height() - 20 );
}

/*================================================================*/
void KWGUI::unitChanged( QString u )
{
    doc->setUnit( u );
    doc->setUnitToAll();
}

/*================================================================*/
bool KWView::doubleClickActivation() const
{
    return TRUE;
}

/*================================================================*/
QWidget* KWView::canvas()
{
    return gui->canvasWidget()->viewport();
}

/*================================================================*/
int KWView::canvasXOffset() const
{
    return gui->canvasWidget()->contentsX();
}
/*================================================================*/
int KWView::canvasYOffset() const
{
    return gui->canvasWidget()->contentsY();
}
/*================================================================*/
void KWView::canvasAddChild( KoViewChild *child )
{
    gui->canvasWidget()->addChild( child->frame() );
}

/*===============================================================*/
void KWView::printDebug() {
    doc->printDebug();
}

