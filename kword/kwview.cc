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
   Boston, MA 02111-1307, USA
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
#include "kwtableframeset.h"
#include "kwdoc.h"
#include "kwframe.h"
#include "kwstyle.h"
#include "kwformat.h"
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
#include "fontdia.h"
#include "counter.h"


#include <koMainWindow.h>
#include <koDocument.h>
#include <koRuler.h>
#include <koTabChooser.h>
#include <koPartSelectDia.h>
#include <koTemplateCreateDia.h>
#include <koFrame.h>

#include <ktempfile.h>
#include <kapp.h>
#include <kdebug.h>
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
#include <tkcoloractions.h>
#include <kstddirs.h>
#include <kparts/event.h>
#include <kformuladocument.h>

#include <stdlib.h>

#include "preview.h"

KWView::KWView( QWidget *_parent, const char *_name, KWDocument* _doc )
    : KoView( _doc, _parent, _name )
{
    doc = 0L;
    gui = 0;
    kspell = 0;
    //vertAlign = KWFormat::VA_NORMAL;
    m_border.left.color = white;
    m_border.left.style = Border::SOLID;
    m_border.left.ptWidth = 0;
    m_border.right = m_border.left;
    m_border.top = m_border.left;
    m_border.bottom = m_border.left;
    m_border.common.color = black;
    m_border.common.style = Border::SOLID;
    m_border.common.ptWidth = 1;
    m_currentPage = 0;

    searchEntry = 0L;
    replaceEntry = 0L;
    doc = _doc;
    backColor = QBrush( white );

    setInstance( KWFactory::global() );
    setXMLFile( "kword.rc" );

    QObject::connect( this, SIGNAL( embeddImage( const QString & ) ),
                      this, SLOT( insertPicture( const QString & ) ) ); /////// # wrong one ! should create a picture frame (TODO)

    setKeyCompression( TRUE );
    setAcceptDrops( TRUE );
    createKWGUI();
    initConfig();

    connect( doc, SIGNAL( pageNumChanged() ),
             this, SLOT( pageNumChanged()) );

    connect( doc, SIGNAL( docStructureChanged(TypeStructDocItem) ),
             this, SLOT( docStructChanged(TypeStructDocItem)) );

    connect( QApplication::clipboard(), SIGNAL( dataChanged() ),
             this, SLOT( clipboardDataChanged() ) );

    connect( gui->canvasWidget(), SIGNAL(currentFrameSetEditChanged()),
             this, SLOT(updateButtons()) );

    connect( gui->canvasWidget(), SIGNAL(currentMouseModeChanged(MouseMode)),
             this, SLOT(setTool(MouseMode)));

    // Cut and copy are directly connected to the selectionChanged signal
    connect( gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
             actionEditCut, SLOT(setEnabled(bool)) );
    connect( gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
             actionEditCopy, SLOT(setEnabled(bool)) );

    connect( gui->canvasWidget(), SIGNAL(frameSelectedChanged()),
             this, SLOT(frameSelectedChanged()));

    connect( gui->canvasWidget(), SIGNAL(docStructChanged(TypeStructDocItem)),
             this, SLOT(docStructChanged(TypeStructDocItem)));

    gui->canvasWidget()->updateCurrentFormat();
    setFocusProxy( gui->canvasWidget() );
}

KWView::~KWView()
{
    // Delete gui while we still exist ( it needs documentDeleted() )
    delete gui;
    if(statusBar())
        statusBar()->removeItem(statusPage);
}

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
      // Config-file value in mm, default 10 pt
      double indent = MM_TO_POINT( config->readDoubleNumEntry("Indent", POINT_TO_MM(10.0) ) );
      doc->setIndentValue(indent);
      changeNbOfRecentFiles(config->readNumEntry("NbRecentFile",10));

      doc->setShowRuler(config->readBoolEntry("Rulers",true));
      doc->setAutoSave((config->readNumEntry("AutoSave",1))*60);
    }
}

void KWView::changeNbOfRecentFiles(int _nb)
{
    shell()->setMaxRecentItems( _nb );
}

void KWView::initGui()
{
    clipboardDataChanged();

    if ( gui )
        gui->showGUI();
    setTool( MM_EDIT );
    actionViewFrameBorders->setChecked( doc->getViewFrameBorders() );
    actionViewFormattingChars->setChecked( doc->getViewFormattingChars());

    actionViewHeader->setChecked(doc->isHeaderVisible());
    actionViewFooter->setChecked(doc->isFooterVisible());
    actionFormatDecreaseIndent->setEnabled(false);
    //setNoteType(doc->getNoteType(), false);

    actionFormatColor->setColor( Qt::black );

    //refresh zoom combobox
    QStringList list=actionViewZoom->items();
    QString zoomStr=QString::number(doc->zoom())+'%';
    actionViewZoom->setCurrentItem(list.findIndex(zoomStr)  );


    MouseMode mouseMode=gui->canvasWidget()->getMouseMode();
    gui->canvasWidget()->setMouseMode( mouseMode );
    //setTool(mouseMode);
    if(mouseMode!=MM_EDIT_FRAME)
        doc->deSelectAllFrames();


    showFormulaToolbar( FALSE );

    if(statusBar())
    {
        statusBar()->insertItem( QString(" ")+i18n("Page %1/%2").arg(1).arg(1)+' ', statusPage );
        // Workaround for bug in KDE-2.1[.1]'s KStatusBar (show() not called in insertItem)
        QObjectList *l = statusBar()->queryList( "QLabel" );
        QObjectListIt it( *l );
        for ( ; it.current() ; ++it )
            static_cast<QLabel *>(it.current())->show();
        delete l;
        statusBar()->show();
        //kdDebug() << "KWView::setupActions statusbar is now " << statusBar() << endl;
    }

    updatePageInfo();
    updateButtons();
}

void KWView::setupActions()
{
    // The actions here are grouped by menu, because this helps noticing
    // accelerator clashes.

    // -------------- File menu
    actionExtraCreateTemplate = new KAction( i18n( "&Create Template from Document..." ), 0,
                                             this, SLOT( extraCreateTemplate() ),
                                             actionCollection(), "extra_template" );
    (void) new KAction( i18n( "Statistics" ), 0, this, SLOT( fileStatistics() ), actionCollection(), "file_statistics" );

    // -------------- Edit actions
    actionEditCut = KStdAction::cut( this, SLOT( editCut() ), actionCollection(), "edit_cut" );
    actionEditCopy = KStdAction::copy( this, SLOT( editCopy() ), actionCollection(), "edit_copy" );
    actionEditPaste = KStdAction::paste( this, SLOT( editPaste() ), actionCollection(), "edit_paste" );
    KStdAction::find( this, SLOT( editFind() ), actionCollection(), "edit_find" );
    KStdAction::replace( this, SLOT( editReplace() ), actionCollection(), "edit_replace" );
    actionEditSelectAll = KStdAction::selectAll( this, SLOT( editSelectAll() ), actionCollection(), "edit_selectall" );
    actionExtraSpellCheck = KStdAction::spelling( this, SLOT( extraSpelling() ), actionCollection(), "extra_spellcheck" );


    (void) new KAction( i18n( "Serial &Letter Database..." ), 0,
                        this, SLOT( editSerialLetterDataBase() ),
                        actionCollection(), "edit_sldatabase" );

    // -------------- Frame menu
    actionEditDelFrame = new KAction( i18n( "&Delete Frame" ), 0,
                                      this, SLOT( editDeleteFrame() ),
                                      actionCollection(), "edit_delframe" );
    actionEditReconnectFrame = new KAction( i18n( "&Reconnect Frame..." ), 0,
                                            this, SLOT( editReconnectFrame() ),
                                            actionCollection(), "edit_reconnect" );
    actionToolsEdit = new KToggleAction( i18n( "Edit &Text" ), "edittool", Key_F4,
                                         this, SLOT( toolsEdit() ),
                                         actionCollection(), "tools_edit" );
    actionToolsEdit->setExclusiveGroup( "tools" );
    actionToolsEditFrames = new KToggleAction( i18n( "Edit &Frames" ),"frame_edit", Key_F5,
                                               this, SLOT( toolsEditFrame() ),
                                               actionCollection(), "tools_editframes" );
    actionToolsEditFrames->setExclusiveGroup( "tools" );


    // -------------- View menu
    actionViewFormattingChars = new KToggleAction( i18n( "&Formatting Characters" ), 0,
                                                   this, SLOT( viewFormattingChars() ),
                                                   actionCollection(), "view_formattingchars" );
    actionViewFormattingChars->setEnabled( false ); // ## not implemented
    actionViewFrameBorders = new KToggleAction( i18n( "Frame &Borders" ), 0,
                                                   this, SLOT( viewFrameBorders() ),
                                                   actionCollection(), "view_frameborders" );
    actionViewHeader = new KToggleAction( i18n( "&Header" ), 0,
                                          this, SLOT( viewHeader() ),
                                          actionCollection(), "view_header" );
    actionViewFooter = new KToggleAction( i18n( "Foo&ter" ), 0,
                                          this, SLOT( viewFooter() ),
                                          actionCollection(), "view_footer" );
    actionViewFootNotes = new KToggleAction( i18n( "Foot&notes" ), 0,
                                             this, SLOT( viewFootNotes() ),
                                          actionCollection(), "view_footnotes" );
    actionViewFootNotes->setExclusiveGroup( "notes" );
    actionViewEndNotes = new KToggleAction( i18n( "&Endnotes" ), 0,
                                             this, SLOT( viewEndNotes() ),
                                          actionCollection(), "view_endnotes" );
    actionViewEndNotes->setExclusiveGroup( "notes" );

    actionViewZoom = new KSelectAction( i18n( "Zoom" ), "viewmag", 0,
                                        actionCollection(), "view_zoom" );
    connect( actionViewZoom, SIGNAL( activated( const QString & ) ),
             this, SLOT( viewZoom( const QString & ) ) );
    QStringList lst;
    lst << "50%";
    lst << "75%";
    lst << "100%";
    lst << "125%";
    lst << "150%";
    lst << "200%";
    lst << "250%";
    lst << "350%";
    lst << "400%";
    lst << "450%";
    lst << "500%";
    actionViewZoom->setItems( lst );

    // -------------- Insert menu
    (void) new KAction( i18n( "&Table" ), "inline_table", 0,
                        this, SLOT( insertTable() ),
                        actionCollection(), "insert_table" );

    actionInsertPicture = new KAction( i18n( "&Picture Inline..." ),"inline_image", Key_F2,
                        this, SLOT( insertPicture() ),
                        actionCollection(), "insert_picture" );

    actionInsertSpecialChar = new KAction( i18n( "Sp&ecial Character..." ), "char",
                        ALT + SHIFT + Key_C,
                        this, SLOT( insertSpecialChar() ),
                        actionCollection(), "insert_specialchar" );

    actionInsertFormula = new KAction( i18n( "For&mula" ), "frame_formula", 0,
                                       this, SLOT( toolsFormula() ),
                                       actionCollection(), "tools_formula" );

    actionInsertFrameBreak = new KAction( i18n( "&Hard Frame Break" ), CTRL + Key_Return,
                                          this, SLOT( insertFrameBreak() ),
                                          actionCollection(), "insert_framebreak" );
    actionInsertFrameBreak->setEnabled( false ); // ### TODO

    actionInsertFootEndNote = new KAction( i18n( "&Footnote or Endnote..." ), 0,
                                           this, SLOT( insertFootNoteEndNote() ),
                                           actionCollection(), "insert_footendnote" );
    actionInsertFootEndNote->setEnabled( false ); // ### TODO

    actionInsertContents = new KAction( i18n( "Table of &Contents" ), 0,
                                        this, SLOT( insertContents() ),
                                        actionCollection(), "insert_contents" );

    actionToolsCreateText = new KToggleAction( i18n( "Te&xt Frame" ), "frame_text", Key_F6,
                                               this, SLOT( toolsCreateText() ),
                                               actionCollection(), "tools_createtext" );
    actionToolsCreateText->setExclusiveGroup( "tools" );
    actionToolsCreatePix = new KToggleAction( i18n( "P&icture Frame" ), "frame_image", Key_F7,
                                              this, SLOT( toolsCreatePix() ),
                                              actionCollection(), "tools_createpix" );
    actionToolsCreatePix->setExclusiveGroup( "tools" );
    actionToolsCreatePart = new KToggleAction( i18n( "&Object Frame" ), "frame_query", Key_F12,
                                               this, SLOT( toolsPart() ),
                                               actionCollection(), "tools_part" );
    actionToolsCreatePart->setExclusiveGroup( "tools" );

    m_variableDefMap.clear();
    actionInsertVariable = new KActionMenu( i18n( "&Variable" ),
                                            actionCollection(), "insert_variable" );
    // The last argument is only needed if a submenu is to be created
    addVariableActions( VT_FIELD, KWFieldVariable::actionTexts(), actionInsertVariable, i18n("&Property") );
    addVariableActions( VT_DATE, KWDateVariable::actionTexts(), actionInsertVariable, i18n("&Date") );
    addVariableActions( VT_TIME, KWTimeVariable::actionTexts(), actionInsertVariable, i18n("&Time") );
    addVariableActions( VT_PGNUM, KWPgNumVariable::actionTexts(), actionInsertVariable, QString::null );
    addVariableActions( VT_CUSTOM, KWCustomVariable::actionTexts(), actionInsertVariable, QString::null );
    addVariableActions( VT_SERIALLETTER, KWSerialLetterVariable::actionTexts(), actionInsertVariable, QString::null );

    // ------------------------- Format menu
    actionFormatFont = new KAction( i18n( "&Font..." ), ALT + CTRL + Key_F,
                                    this, SLOT( formatFont() ),
                                    actionCollection(), "format_font" );
    actionFormatColor = new KColorAction( i18n( "Text Color..." ), KColorAction::TextColor, ALT + CTRL + Key_C,
                                     this, SLOT( textColor() ),
                                     actionCollection(), "format_color" );
    actionFormatParag = new KAction( i18n( "&Paragraph..." ), ALT + CTRL + Key_P,
                                     this, SLOT( formatParagraph() ),
                                     actionCollection(), "format_paragraph" );
    actionFormatFrameSet = new KAction( i18n( "F&rame/Frameset..." ), 0,
                                     this, SLOT( formatFrameSet() ),
                                     actionCollection(), "format_frameset" );
    (void) new KAction( i18n( "P&age..." ), 0,
                        this, SLOT( formatPage() ),
                        actionCollection(), "format_page" );

    (void) new KAction( i18n( "&Stylist..." ), ALT + CTRL + Key_S,
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

    actionFormatStyle = new KSelectAction( i18n( "St&yle" ), 0,
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
    actionFormatStrikeOut = new KToggleAction( i18n( "&Strike out" ), "text_strike", 0 ,
                                           this, SLOT( textStrikeOut() ),
                                           actionCollection(), "format_strike" );

    actionFormatAlignLeft = new KToggleAction( i18n( "Align &Left" ), "text_left", ALT + Key_L,
                                       this, SLOT( textAlignLeft() ),
                                       actionCollection(), "format_alignleft" );
    actionFormatAlignLeft->setExclusiveGroup( "align" );
    actionFormatAlignLeft->setChecked( TRUE );
    actionFormatAlignCenter = new KToggleAction( i18n( "Align &Center" ), "text_center", ALT + Key_C,
                                         this, SLOT( textAlignCenter() ),
                                         actionCollection(), "format_aligncenter" );
    actionFormatAlignCenter->setExclusiveGroup( "align" );
    actionFormatAlignRight = new KToggleAction( i18n( "Align &Right" ), "text_right", ALT + Key_R,
                                        this, SLOT( textAlignRight() ),
                                        actionCollection(), "format_alignright" );
    actionFormatAlignRight->setExclusiveGroup( "align" );
    actionFormatAlignBlock = new KToggleAction( i18n( "Align &Block" ), "text_block", ALT + Key_B,
                                        this, SLOT( textAlignBlock() ),
                                        actionCollection(), "format_alignblock" );
    actionFormatAlignBlock->setExclusiveGroup( "align" );
    actionFormatList = new KToggleAction( i18n( "List" ), "enumList", 0,
                                              this, SLOT( textList() ),
                                              actionCollection(), "format_list" );
    actionFormatList->setExclusiveGroup( "style" );
    actionFormatSuper = new KToggleAction( i18n( "Superscript" ), "super", 0,
                                              this, SLOT( textSuperScript() ),
                                              actionCollection(), "format_super" );
    actionFormatSuper->setExclusiveGroup( "valign" );
    actionFormatSub = new KToggleAction( i18n( "Subscript" ), "sub", 0,
                                              this, SLOT( textSubScript() ),
                                              actionCollection(), "format_sub" );
    actionFormatSub->setExclusiveGroup( "valign" );

    actionFormatIncreaseIndent= new KAction( i18n( "Increase Indent" ), "format_increaseindent", 0,
                                      this, SLOT( textIncreaseIndent() ),
                                      actionCollection(), "format_increaseindent" );

    actionFormatDecreaseIndent= new KAction( i18n( "Decrease Indent" ),"format_decreaseindent", 0,
                                      this, SLOT( textDecreaseIndent() ),
                                      actionCollection(), "format_decreaseindent" );

    // ---------------------------- frame toolbar actions

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
    lst.clear();
    lst << Border::getStyle( Border::SOLID );
    lst << Border::getStyle( Border::DASH );
    lst << Border::getStyle( Border::DOT );
    lst << Border::getStyle( Border::DASH_DOT );
    lst << Border::getStyle( Border::DASH_DOT_DOT );
    actionBorderStyle->setItems( lst );
    actionBorderWidth = new KSelectAction( i18n( "Border Width" ), 0,
                                                 actionCollection(), "border_width" );
    connect( actionBorderWidth, SIGNAL( activated( const QString & ) ),
             this, SLOT( borderWidth( const QString & ) ) );
    lst.clear();
    for ( unsigned int i = 1; i < 10; i++ )
        lst << QString::number( i );
    actionBorderWidth->setItems( lst );


     actionBorderColor = new TKSelectColorAction( i18n("Border Color"), TKSelectColorAction::LineColor, actionCollection(), "border_color" );
    connect(actionBorderColor,SIGNAL(activated()),SLOT(borderColor()));


    actionBackgroundColor = new TKSelectColorAction( i18n( "Background Color" ), TKSelectColorAction::FillColor, actionCollection(),"border_backgroundcolor");
    connect(actionBackgroundColor,SIGNAL(activated()),SLOT(backgroundColor() ));

    // ---------------------- Table menu

    actionTableInsertRow = new KAction( i18n( "&Insert Row..." ), "insert_table_row", 0,
                               this, SLOT( tableInsertRow() ),
                               actionCollection(), "table_insrow" );
    actionTableInsertCol = new KAction( i18n( "I&nsert Column..." ), "insert_table_col", 0,
                               this, SLOT( tableInsertCol() ),
                               actionCollection(), "table_inscol" );
    actionTableDelRow = new KAction( i18n( "&Delete Row..." ), "delete_table_row", 0,
                                     this, SLOT( tableDeleteRow() ),
                                     actionCollection(), "table_delrow" );
    actionTableDelCol = new KAction( i18n( "D&elete Column..." ), "delete_table_col", 0,
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
    actionTableDelete = new KAction( i18n( "Delete &Table" ), 0,
                                     this, SLOT( tableDelete() ),
                                     actionCollection(), "table_delete" );

    // ---------------------- Tools menu

    actionEditCustomVars = new KAction( i18n( "&Custom Variables..." ), 0,
                                        this, SLOT( editCustomVars() ),
                                        actionCollection(), "edit_customvars" );
    (void) new KAction( i18n( "&Autocorrection..." ), 0,
                        this, SLOT( extraAutoFormat() ),
                        actionCollection(), "extra_autocorrection" );

    //------------------------ Settings menu
    KStdAction::preferences(this, SLOT(configure()), actionCollection(), "configure" );
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
            KAction * act = new KAction( (*it), 0, this, SLOT( insertVariable() ),
                                         actionCollection(), "var-action" );
            m_variableDefMap.insert( act, v );
            parentMenu->insert( act );
        }
    }
}

void KWView::fileStatistics()
{
    ulong charsWithSpace = 0L;
    ulong charsWithoutSpace = 0L;
    ulong words = 0L;
    ulong sentences = 0L;
    QListIterator<KWFrameSet> framesetIt( doc->framesetsIterator() );
    for ( ; framesetIt.current(); ++framesetIt )
    {
        KWFrameSet *frameSet = framesetIt.current();
        // Exclude headers and footers
        if ( frameSet->getFrameInfo() == FI_BODY && frameSet->isVisible() )
        {
            frameSet->statistics( charsWithSpace, charsWithoutSpace, words, sentences );
        }
    }

    KDialogBase dlg( KDialogBase::Plain, i18n( "Document Statistics" ),
                     KDialogBase::Ok, KDialogBase::Ok, this, 0, true,
                     true /* separator */ );

    QVBoxLayout * vlayout = new QVBoxLayout( dlg.plainPage() );
    vlayout->addWidget( new QLabel(
        i18n( "<qt><center>Document Statistics</center><hr><p>"
              "Characters (total count including spaces) : <b>%1</b><br/>"
              "Characters without spaces: <b>%2</b><br/>"
              "Words: <b>%3</b><br/>"
              "Sentences: <b>%4</b></p>" ).
        arg( charsWithSpace ).arg( charsWithoutSpace ).arg( words ).arg( sentences ),
                        dlg.plainPage() ) );
    dlg.setInitialSize( QSize( 400, 200 ) ); // not too good for long translations... -> use a real layout and 5 labels
    dlg.show();
}

/*======================== create GUI ==========================*/
void KWView::createKWGUI()
{
    // setup GUI
    setupActions();

    gui = new KWGUI( this, doc, this );
    gui->setGeometry( 0, 0, width(), height() );
    gui->show();
}

/*================================================================*/
void KWView::showFormulaToolbar( bool show )
{
  // This might not be exactly the right place. But these actions
  // must be enabled when a formula is active...
  doc->getFormulaDocument()->getMakeGreekAction()->setEnabled( show );
  doc->getFormulaDocument()->getAddGenericUpperAction()->setEnabled( show );
  doc->getFormulaDocument()->getAddGenericLowerAction()->setEnabled( show );
  doc->getFormulaDocument()->getRemoveEnclosingAction()->setEnabled( show );
  doc->getFormulaDocument()->getInsertSymbolAction()->setEnabled( show );

  shell()->showToolbar( "formula_toolbar", show );
}

void KWView::updatePageInfo()
{
    KWFrameSetEdit * edit = gui->canvasWidget()->currentFrameSetEdit();
    if ( edit && statusBar())
    {
        m_currentPage = edit->currentFrame()->pageNum();
        statusBar()->changeItem( QString(" ")+i18n("Page %1/%2").arg(m_currentPage+1).arg(doc->getPages())+' ', statusPage );
        gui->getVertRuler()->setOffset( 0, -gui->canvasWidget()->getVertRulerPos() );
    }
}

void KWView::pageNumChanged()
{
     docStructChanged(TextFrames);
     updatePageInfo();
}

/*================================================================*/
void KWView::clipboardDataChanged()
{
    // Can we paste into something ?
    if ( !gui || !gui->canvasWidget()->currentFrameSetEdit() )
    {
        actionEditPaste->setEnabled(false);
        return;
    }
    // Is there plain text in the clipboard ?
    if ( !QApplication::clipboard()->text().isEmpty() )
    {
        actionEditPaste->setEnabled(true);
        return;
    }
    // Is there kword XML in the clipboard ?
    QMimeSource *data = QApplication::clipboard()->data();
    actionEditPaste->setEnabled( data->provides( MIME_TYPE ) );

}

/*=========================== file print =======================*/
void KWView::setupPrinter( KPrinter &prt )
{
#ifdef HAVE_KDEPRINT
    prt.setPageSelection( KPrinter::ApplicationSide );
    prt.setCurrentPage( currentPage() + 1 );
#endif
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
    // Don't repaint behind the print dialog until we're done zooming/unzooming the doc
    gui->canvasWidget()->setUpdatesEnabled(false);
    gui->canvasWidget()->viewport()->setCursor( waitCursor );

    prt.setFullPage( true );

    // ### HACK: disable zooming-when-printing if embedded parts are used.
    // No koffice app supports zooming in paintContent currently.
    bool doZoom = true;
    QListIterator<KWFrameSet> fit = doc->framesetsIterator();
    for ( ; fit.current() && doZoom ; ++fit )
        if ( fit.current()->getFrameType() == FT_PART )
            doZoom = false;

    int oldZoom = doc->zoom();
    // We don't get valid metrics from the printer - and we want a better resolution
    // anyway (it's the PS driver that takes care of the printer resolution).
    QPaintDeviceMetrics metrics( &prt );
    //doc->setZoomAndResolution( 100, metrics.logicalDpiX(), metrics.logicalDpiY(), false );
    if ( doZoom )
        doc->setZoomAndResolution( 100, 300, 300, false );
    //kdDebug() << "KWView::print zoom&res set" << endl;

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
        if ( doZoom )
            painter.scale( metrics.logicalDpiX() / 300.0, metrics.logicalDpiY() / 300.0 );
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
    //kdDebug() << "KWView::print zoom&res reset" << endl;

    gui->canvasWidget()->setUpdatesEnabled(true);
    gui->canvasWidget()->viewport()->setCursor( ibeamCursor );
    doc->repaintAllViews();
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
    actionFormatStrikeOut->setChecked( currentFormat.font().strikeOut());

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


void KWView::showRulerIndent( double _leftMargin, double _firstLine )
{
  KoRuler * hRuler = gui ? gui->getHorzRuler() : 0;
  if ( hRuler )
  {
      hRuler->setFirstIndent( KWUnit::userValue( _firstLine + _leftMargin, doc->getUnit() ) );
      hRuler->setLeftIndent( KWUnit::userValue( _leftMargin, doc->getUnit() ) );
      actionFormatDecreaseIndent->setEnabled( _leftMargin>0);
  }
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
    actionFormatList->setChecked( c.numbering() == Counter::NUM_LIST );
}

/*================================================================*/
void KWView::showFrameBorders( Border _left, Border _right,
                               Border _top, Border _bottom )
{
    showParagBorders( _left, _right, _top, _bottom );
}

/*================================================================*/
void KWView::showParagBorders( Border left, Border right,
                               Border top, Border bottom )
{
    if ( m_border.left != left || m_border.right != right || m_border.top != top || m_border.bottom != bottom )
    {
        m_border.left = left;
        m_border.right = right;
        m_border.top = top;
        m_border.bottom = bottom;

        actionBorderLeft->setChecked( left.ptWidth > 0 );
        actionBorderRight->setChecked( right.ptWidth > 0 );
        actionBorderTop->setChecked( top.ptWidth > 0 );
        actionBorderBottom->setChecked( bottom.ptWidth > 0 );
        actionBorderOutline->setChecked(
            actionBorderLeft->isChecked() &&
            actionBorderRight->isChecked() &&
            actionBorderTop->isChecked() &&
            actionBorderBottom->isChecked());

        if ( left.ptWidth > 0 ) {
            m_border.common = left;
            borderShowValues();
        }
        if ( right.ptWidth > 0 ) {
            m_border.common = right;
            borderShowValues();
        }
        if ( top.ptWidth > 0 ) {
            m_border.common = top;
            borderShowValues();
        }
        if ( bottom.ptWidth > 0 ) {
            m_border.common = bottom;
            borderShowValues();
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
    case MM_CREATE_FORMULA:
        // no such tool
        break;
    case MM_CREATE_PART:
        actionToolsCreatePart->setChecked( TRUE );
        break;
    }

    if ( _mouseMode == MM_EDIT_FRAME )
    {
        //checked false all frame border button
        //because when you change mode to edit frame
        // there isn't any frame selected => border button
        //should be unselect.

        /// ### But this is a hack (what happens when selecting a frame and then unselecting it again ?)
        // The same code should be called, but it isn't. And the same applied to Delete Frame, etc.
        // --> TODO: a signal (from canvas preferrably), when the selected frames change.
        actionBorderOutline->setChecked(false);
        actionBorderLeft->setChecked(false);
        actionBorderRight->setChecked(false);
        actionBorderTop->setChecked(false);
        actionBorderBottom->setChecked(false);
    }

    actionTableInsertRow->setEnabled( FALSE );
    actionTableInsertCol->setEnabled( FALSE );
    actionTableDelRow->setEnabled( FALSE );
    actionTableDelCol->setEnabled( FALSE );
    actionTableJoinCells->setEnabled( FALSE );
    actionTableSplitCells->setEnabled( FALSE );
    actionTableDelete->setEnabled( FALSE );
    actionTableUngroup->setEnabled( FALSE );
    actionBackgroundColor->setEnabled(FALSE);
    KWTableFrameSet *table = gui->canvasWidget()->getTable();

    switch ( _mouseMode ) {
    case MM_EDIT: {
        actionTableInsertRow->setEnabled( table );
        actionTableInsertCol->setEnabled( table );
        actionTableDelRow->setEnabled( table );
        actionTableDelCol->setEnabled( table );
        actionTableDelete->setEnabled( table );
        actionTableUngroup->setEnabled( table );
    } break;
    case MM_EDIT_FRAME: {
        frameSelectedChanged();
        actionBackgroundColor->setEnabled(TRUE);
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
    for (; styleIt.current(); ++styleIt ) {
        lst << i18n("KWord style", styleIt.current()->name().utf8()); // try to translate the name, if standard
    }
    actionFormatStyle->setItems( lst );
    showStyle( currentStyle );
}

/*===============================================================*/
void KWView::editCut()
{
    KWFrameSetEdit * edit = gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
        edit->cut();
}

/*===============================================================*/
void KWView::editCopy()
{
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
    if (!searchEntry)
        searchEntry = new KWSearchContext();
    KWSearchDia dialog( gui->canvasWidget(), "find", searchEntry );
    if ( dialog.exec() == QDialog::Accepted )
    {
        KWFindReplace find( gui->canvasWidget(), &dialog );
        find.proceed();
    }
}

/*===============================================================*/
void KWView::editReplace()
{
    if (!searchEntry)
        searchEntry = new KWSearchContext();
    if (!replaceEntry)
        replaceEntry = new KWSearchContext();
    KWReplaceDia dialog( gui->canvasWidget(), "replace", searchEntry, replaceEntry );
    if ( dialog.exec() == QDialog::Accepted )
    {
        KWFindReplace replace( gui->canvasWidget(), &dialog );
        replace.proceed();
    }
}

/*================================================================*/
void KWView::editDeleteFrame()
{
    QList<KWFrame> frames=doc->getSelectedFrames();
    if(frames.count()>1)  {
        KMessageBox::sorry( this, i18n( "You have selected multiple frames.\n"
                                        "You can only delete one frame at the time." ),
                            i18n( "Delete Frame" ) );
        return;
    }
    if(frames.count()<1)  {
        KMessageBox::sorry( this, i18n( "You have not selected a frame.\n"
                                        "You need to select a frame first in order to delete it."),
                            i18n( "Delete Frame" ) );
        return;
    }
    KWFrame *theFrame = frames.at(0);
    KWFrameSet *fs = theFrame->getFrameSet();

    if ( fs->isAHeader() ) {
        KMessageBox::sorry( this, i18n( "This is a Header frame, it can not be deleted."),
                            i18n( "Delete Frame"  ) );
        return;
    }
    if ( fs->isAFooter() ) {
        KMessageBox::sorry( this, i18n( "This is a Footer frame, it can not be deleted."),
                            i18n( "Delete Frame"  ) );
        return;
    }

    // frame is part of a table?
    if ( fs->getGroupManager() ) {
        int result;
        result = KMessageBox::warningContinueCancel(this,
                                                    i18n( "You are about to delete a table\n"
                                                          "Doing so will delete all the text in the table\n"
                                                          "Are you sure you want to do that?"), i18n("Delete Table"), i18n("&Delete"));
        if (result != KMessageBox::Continue)
            return;
        gui->canvasWidget()->deleteTable( fs->getGroupManager() );
        return;
    }

    if ( fs->getNumFrames() == 1 && fs->getFrameType() == FT_TEXT) {
        if ( doc->processingType() == KWDocument::WP && doc->getFrameSetNum( fs ) == 0 )
            return;

        KWTextFrameSet * textfs = dynamic_cast<KWTextFrameSet *>(doc->getFrameSet( 0 ) );
        if ( !textfs )
            return;

        QTextDocument * textdoc = textfs->textDocument();
        QTextParag * parag = textdoc->firstParag();
        if ( parag && parag->string()->length() > 0 ) {
            int result = KMessageBox::warningContinueCancel(this,
                                                        i18n( "You are about to delete the last Frame of the\n"
                                                              "Frameset '%1'.\n"
                                                              "The contents of this Frameset will not appear\n"
                                                              "anymore!\n\n"
                                                              "Are you sure you want to do that?").arg(fs->getName()),
                                                        i18n("Delete Frame"), i18n("&Delete"));

            if (result == KMessageBox::Continue)
                gui->canvasWidget()->deleteFrame( theFrame );
            return;
        }

    }

    int result = KMessageBox::warningContinueCancel(this,
                                                    i18n("Do you want to delete this frame?"),
                                                    i18n("Delete Frame"),
                                                    i18n("&Delete"));
    if (result == KMessageBox::Continue)
        gui->canvasWidget()->deleteFrame( theFrame );
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
    KWCustomVariablesDia dia( this, doc->getVariables() );
    dia.exec();
    doc->layout();
    doc->repaintAllViews();
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
void KWView::viewHeader()
{
    doc->setHeaderVisible( actionViewHeader->isChecked() );
    doc->updateResizeHandles( );
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
    doc->updateResizeHandles( );
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

        doc->updateResizeHandles( );
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
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertPicture( file );

}

/*===============================================================*/
void KWView::insertPicture(const QString &filename)
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertPicture( filename );
}

/*===============================================================*/
void KWView::insertSpecialChar()
{
    KWTextFrameSetEdit *edit=currentTextEdit();
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
    KWTextFrameSetEdit *edit=currentTextEdit();
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

    // boy what a hack
    QKeyEvent e(static_cast<QEvent::Type>(6) /*QEvent::KeyPress*/ ,Key_Return,0,ControlButton);
    gui->canvasWidget()->keyPressEvent( &e );
#endif
}

/*===============================================================*/
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
            edit->insertVariable( (*it).type, (*it).subtype );
        }
    }
}

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

void KWView::insertContents()
{
    doc->createContents();
    gui->canvasWidget()->repaintAll();
}

void KWView::formatFont()
{
    //kdDebug(32002) << "KWView::formatFont" << endl;
    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
    {
        KWFontDia *fontDia = new KWFontDia( this, "",edit->textFont(),actionFormatSub->isChecked(), actionFormatSuper->isChecked());
        fontDia->show();
        edit->setFont(fontDia->getNewFont(),fontDia->getSubScript(),fontDia->getSuperScript());
        delete fontDia;
    }
    gui->canvasWidget()->setFocus();
}

void KWView::formatParagraph()
{
    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
    {
        KWParagDia *paragDia = new KWParagDia( this, "",
                                               KWParagDia::PD_SPACING | KWParagDia::PD_ALIGN |
                                               KWParagDia::PD_BORDERS |
                                               KWParagDia::PD_NUMBERING | KWParagDia::PD_TABS, doc );
        paragDia->setCaption( i18n( "Paragraph settings" ) );

        // Initialize the dialog from the current paragraph's settings
        KWParagLayout lay = static_cast<KWTextParag *>(edit->getCursor()->parag())->paragLayout();
        paragDia->setParagLayout( lay );
        if(!paragDia->exec())
            return;

        // TODO a macro command with all the changes in it !
        // undo should do all in one step.
        if(paragDia->isLeftMarginChanged())
        {
            edit->setMargin( QStyleSheetItem::MarginLeft, paragDia->leftIndent() );
            gui->getHorzRuler()->setLeftIndent( KWUnit::userValue( paragDia->leftIndent(), doc->getUnit() ) );
        }

        if(paragDia->isRightMarginChanged())
        {
            edit->setMargin( QStyleSheetItem::MarginRight, paragDia->rightIndent() );
            //koRuler doesn't support setRightIndent
            //gui->getHorzRuler()->setRightIndent( KWUnit::userValue( paragDia->rightIndent(), doc->getUnit() ) );
        }
        if(paragDia->isSpaceBeforeChanged())
            edit->setMargin( QStyleSheetItem::MarginTop, paragDia->spaceBeforeParag() );

        if(paragDia->isSpaceAfterChanged())
            edit->setMargin( QStyleSheetItem::MarginBottom, paragDia->spaceAfterParag() );

        if(paragDia->isFirstLineChanged())
        {
            edit->setMargin( QStyleSheetItem::MarginFirstLine, paragDia->firstLineIndent());
            gui->getHorzRuler()->setFirstIndent(
                KWUnit::userValue( paragDia->leftIndent() + paragDia->firstLineIndent(), doc->getUnit() ) );
        }

        if(paragDia->isAlignChanged())
            edit->setAlign( paragDia->align() );

        if(paragDia->isBulletChanged())
            edit->setCounter( paragDia->counter() );

        if(paragDia->listTabulatorChanged())
            edit->setTabList( paragDia->tabListTabulator() );

        if(paragDia->isLineSpacingChanged())
            edit->setLineSpacing( paragDia->lineSpacing() );

        if(paragDia->isBorderChanged())
            edit->setBorders( paragDia->leftBorder(), paragDia->rightBorder(),
                              paragDia->topBorder(), paragDia->bottomBorder() );

        if ( paragDia->isPageBreakingChanged() )
            edit->setPageBreaking( paragDia->linesTogether() );

        delete paragDia;
    }

}

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

        KWTextFrameSetEdit *edit = currentTextEdit();
        if (edit)
            edit->textFrameSet()->clearUndoRedoInfo();
        KWPageLayoutCommand *cmd =new KWPageLayoutCommand( i18n("Change Layout"),doc,tmpOldLayout,tmpNewLayout ) ;
        doc->addCommand(cmd);

        doc->setPageLayout( pgLayout, cl, kwhf );
        doc->updateRuler();

        doc->updateResizeHandles();
#if 0
        gui->canvasWidget()->frameSizeChanged( pgLayout );
#endif
    }
}

void KWView::formatFrameSet()
{
    if ( doc->getFirstSelectedFrame() )
      gui->canvasWidget()->editFrameProperties();
    else // TODO enable/disable the action depending on whether a frame is selected, instead
        KMessageBox::sorry( this,
                            i18n("Sorry, you have to select a frame first."),
                            i18n("Format Frameset"));
}

void KWView::extraSpelling()
{
    if (kspell) return; // Already in progress
    m_spellCurrFrameSetNum = -1;
    kspell = new KSpell( this, i18n( "Spell Checking" ), this, SLOT( spellCheckerReady() ), doc->getKSpellConfig() );

    QObject::connect( kspell, SIGNAL( death() ),
                      this, SLOT( spellCheckerFinished() ) );
    QObject::connect( kspell, SIGNAL( misspelling( QString, QStringList*, unsigned ) ),
                      this, SLOT( spellCheckerMisspelling( QString, QStringList*, unsigned ) ) );
    QObject::connect( kspell, SIGNAL( corrected( QString, QString, unsigned ) ),
                      this, SLOT( spellCheckerCorrected( QString, QString, unsigned ) ) );
    QObject::connect( kspell, SIGNAL( done( const QString & ) ),
                      this, SLOT( spellCheckerDone( const QString & ) ) );
}

void KWView::extraAutoFormat()
{
    KWAutoFormatDia dia( this, 0, doc, gui->canvasWidget() );
    dia.show();
}

void KWView::extraStylist()
{
    KWStyleManager * styleManager = new KWStyleManager( this, doc );
    styleManager->setCaption( i18n( "Stylist" ) );
    styleManager->show();
    delete styleManager;
}

void KWView::extraCreateTemplate()
{
    QPixmap pix( 45, 60 );
    pix.fill( Qt::white );

    KTempFile tempFile( QString::null, ".kwt" );
    tempFile.setAutoDelete(true);

    doc->saveNativeFormat( tempFile.name() );

    KoTemplateCreateDia::createTemplate( "kword_template", KWFactory::global(),
                                         tempFile.name(), pix, this );

    KWFactory::global()->dirs()->addResourceType("kword_template",
                                                    KStandardDirs::kde_default( "data" ) +
                                                    "kword/templates/");
}

void KWView::toolsEdit()
{
    if ( actionToolsEdit->isChecked() )
        gui->canvasWidget()->setMouseMode( MM_EDIT );
    else
        actionToolsEdit->setChecked( true ); // always one has to be checked !
}

void KWView::toolsEditFrame()
{
    if ( actionToolsEditFrames->isChecked() )
        gui->canvasWidget()->setMouseMode( MM_EDIT_FRAME );
    else
        actionToolsEditFrames->setChecked( true ); // always one has to be checked !
}

void KWView::toolsCreateText()
{
    if ( actionToolsCreateText->isChecked() )
        gui->canvasWidget()->setMouseMode( MM_CREATE_TEXT );
    else
        actionToolsCreateText->setChecked( true ); // always one has to be checked !
}

void KWView::toolsCreatePix()
{
    if ( !actionToolsCreatePix->isChecked() )
    {
        actionToolsCreatePix->setChecked( true ); // always one has to be checked !
        return;
    }
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

void KWView::insertTable()
{
    KWCanvas * canvas = gui->canvasWidget();
    KWTableDia *tableDia = new KWTableDia( this, 0, canvas, doc,
                                           canvas->tableRows(),
                                           canvas->tableCols(),
                                           canvas->tableWidthMode(),
                                           canvas->tableHeightMode(),
                                           canvas->tableIsFloating() );
    tableDia->setCaption( i18n( "Insert Table" ) );
    tableDia->show();
    delete tableDia;
}

void KWView::toolsFormula()
{
    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
    {
        KWFormulaFrameSet *frameset = new KWFormulaFrameSet( doc, QString::null );
        doc->addFrameSet( frameset, false ); // done first since the frame number is stored in the undo/redo
        KWFrame *frame = new KWFrame(frameset, 0, 0, 10, 10 );
        frameset->addFrame( frame, false );
        edit->insertFloatingFrameSet( frameset, i18n("Insert Formula") );
        frameset->finalize(); // done last since it triggers a redraw

        // Strange, seems we need this - hmm, do we, still ?
        edit->getCursor()->parag()->invalidate( 0 ); // and that's done by KWTextParag::setCustomItem. Hmm.
        edit->getCursor()->parag()->setChanged( true );
        doc->slotRepaintChanged( edit->frameSet() );
    }
}

void KWView::toolsPart()
{
    if ( !actionToolsCreatePart->isChecked() )
    {
        actionToolsCreatePart->setChecked( true ); // always one has to be checked !
        return;
    }
    gui->canvasWidget()->setMouseMode( MM_EDIT );

    KoDocumentEntry pe = KoPartSelectDia::selectPart( this );
    if ( pe.isEmpty() )
        return;

    gui->canvasWidget()->setMouseMode( MM_CREATE_PART );
    gui->canvasWidget()->setPartEntry( pe );
}

void KWView::tableInsertRow()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT );
    KWTableFrameSet *table = gui->canvasWidget()->getTable();
    if ( !table )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before inserting a new row." ),
                            i18n( "Insert Row" ) );
    } else {
        KWInsertDia dia( this, "", table, doc, KWInsertDia::ROW, gui->canvasWidget() );
        dia.setCaption( i18n( "Insert Row" ) );
        dia.show();
    }
}

void KWView::tableInsertCol()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT );
    KWTableFrameSet *table = gui->canvasWidget()->getTable();
    if ( !table )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before inserting a new column." ),
                            i18n( "Insert Column" ) );
    } else {
        // value = 62 because a insert column = 60 +2 (border )see kwtableframeset.cc
        if ( table->boundingRect().right() + 62 > static_cast<int>( doc->ptPaperWidth() ) )
        {
            KMessageBox::sorry( this,
                            i18n( "There is not enough space at the right of the table\n"
                                  "to insert a new column." ),
                            i18n( "Insert Column" ) );
        }
        else
        {
            KWInsertDia dia( this, "", table, doc, KWInsertDia::COL, gui->canvasWidget() );
            dia.setCaption( i18n( "Insert Column" ) );
            dia.show();
        }
    }
}

void KWView::tableDeleteRow()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT );

    KWTableFrameSet *table = gui->canvasWidget()->getTable();
    if ( !table )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before deleting a row." ),
                            i18n( "Delete Row" ) );
    }
    else
    {
        if ( table->getRows() == 1 )
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
                gui->canvasWidget()->deleteTable( table );
            }
        }
        else
        {
            KWDeleteDia dia( this, "", table, doc, KWDeleteDia::ROW, gui->canvasWidget() );
            dia.setCaption( i18n( "Delete Row" ) );
            dia.show();
        }
    }

}

void KWView::tableDeleteCol()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT );

    KWTableFrameSet *table = gui->canvasWidget()->getTable();
    if ( !table )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before deleting a column." ),
                            i18n( "Delete Column" ) );
    }
    else
    {
        if ( table->getCols() == 1 )
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
                gui->canvasWidget()->deleteTable( table );
            }
        }
        else
        {
            KWDeleteDia dia( this, "", table, doc, KWDeleteDia::COL, gui->canvasWidget() );
            dia.setCaption( i18n( "Delete Column" ) );
            dia.show();
        }
    }
}

void KWView::tableJoinCells()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT_FRAME );

    KWTableFrameSet *table = gui->canvasWidget()->getCurrentTable();
    if ( !table )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before joining cells." ),
                            i18n( "Join Cells" ) );
    } else {
        if ( !table->joinCells() )
        {
            KMessageBox::sorry( this,
                                i18n( "You have to select some cells which are next to each other\n"
                                      "and are not already joined." ),
                                i18n( "Join Cells" ) );
        }
        //KoRect r = doc->zoomRect( table->boundingRect() );
        //gui->canvasWidget()->repaintScreen( r, TRUE );
        gui->canvasWidget()->repaintAll();
        doc->layout();
    }
}

void KWView::tableSplitCells()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT_FRAME );

    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    KWTableFrameSet *table = gui->canvasWidget()->getCurrentTable();
    if ( !table && selectedFrames.count() > 0) {
        table=selectedFrames.at(0)->getFrameSet()->getGroupManager();
    }

    if(selectedFrames.count() >1 || table == 0) {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                            //i18n( "You have to select one table cell\n"
                                  "before splitting cells." ),
                            i18n( "Split Cells" ) );
        return;
    }

    int rows=1, cols=2;
    if ( !table->splitCell(rows,cols) ) {
        KMessageBox::sorry( this,
                            i18n("You have to select a joined cell."),
                            //i18n("There is not enough space to split the cell into that many parts"),
                            i18n("Split Cells") );
    }
    //KoRect r = doc->zoomRect( table->boundingRect() );
    //gui->canvasWidget()->repaintScreen( r, TRUE );
    gui->canvasWidget()->repaintAll();
    doc->updateAllFrames();
    doc->layout();
}

void KWView::tableUngroupTable()
{
    gui->canvasWidget()->setMouseMode( MM_EDIT );

    KWTableFrameSet *table = gui->canvasWidget()->getTable();
    if ( !table )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table\n"
                                  "before ungrouping a table." ),
                            i18n( "Ungroup Table" ) );
    }
    else
    {
        int result;
        result = KMessageBox::warningContinueCancel(this,
                                                    i18n("Ungrouping a table can not be undone.\n"
                                                         "Are you sure you want to ungroup the table?"),
                                                    i18n("Ungroup Table"), i18n("&Ungroup"));
        if (result == KMessageBox::Continue)
            gui->canvasWidget()->ungroupTable(table);
    }
}

void KWView::tableDelete()
{
    KWTableFrameSet *table = gui->canvasWidget()->getTable();
    if ( !table )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table \n"
                                  "or select it to delete it!" ),
                            i18n( "Delete Table" ) );
    }
    else
    {
        gui->canvasWidget()->deleteTable( table );
    }
}

void KWView::textStyleSelected( int index )
{
    if(gui->canvasWidget()->currentFrameSetEdit())
    {
        KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(gui->canvasWidget()->currentFrameSetEdit()->currentTextEdit());
        if ( edit )
            edit->applyStyle( doc->styleAt( index ) );
        gui->canvasWidget()->setFocus(); // the combo keeps focus...
    }
}

void KWView::textSizeSelected( int size )
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->setPointSize( size );
    gui->canvasWidget()->setFocus(); // the combo keeps focus...
}

void KWView::textFontSelected( const QString & font )
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->setFamily( font );
    gui->canvasWidget()->setFocus(); // the combo keeps focus...
}

void KWView::textBold()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->setBold(actionFormatBold->isChecked());
}

void KWView::textItalic()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->setItalic(actionFormatItalic->isChecked());

}

void KWView::textUnderline()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->setUnderline(actionFormatUnderline->isChecked());
}

void KWView::textStrikeOut()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->setStrikeOut(actionFormatStrikeOut->isChecked());
}

void KWView::textColor()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        QColor color = edit->textColor();
        if ( KColorDialog::getColor( color ) ) {
            actionFormatColor->setColor( color );
            edit->setTextColor( color );
        }
    }
}

void KWView::textAlignLeft()
{
    if ( actionFormatAlignLeft->isChecked() )
    {
        KWTextFrameSetEdit * edit = currentTextEdit();
        if ( edit )
            edit->setAlign(Qt::AlignLeft);
    } else
        actionFormatAlignLeft->setChecked( true );
}

void KWView::textAlignCenter()
{
    if ( actionFormatAlignCenter->isChecked() )
    {
        KWTextFrameSetEdit * edit = currentTextEdit();
        if ( edit )
            edit->setAlign(Qt::AlignCenter);
    } else
        actionFormatAlignCenter->setChecked( true );
}

void KWView::textAlignRight()
{
    if ( actionFormatAlignRight->isChecked() )
    {
        KWTextFrameSetEdit * edit = currentTextEdit();
        if ( edit )
        edit->setAlign(Qt::AlignRight);
    } else
        actionFormatAlignRight->setChecked( true );
}

void KWView::textAlignBlock()
{
    if ( actionFormatAlignBlock->isChecked() )
    {
        KWTextFrameSetEdit * edit = currentTextEdit();
        if ( edit )
            edit->setAlign(Qt3::AlignJustify);
    } else
        actionFormatAlignBlock->setChecked( true );
}

void KWView::textList()
{
    Counter c;
    if ( actionFormatList->isChecked() )
    {
        c.setNumbering( Counter::NUM_LIST );
        c.setStyle( Counter::STYLE_NUM );
    }
    else
    {
        c.setNumbering( Counter::NUM_NONE );
    }
    KWTextFrameSetEdit * edit = currentTextEdit();
    ASSERT(edit);
    if ( edit )
        edit->setCounter( c );
}

void KWView::textSuperScript()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->setTextSuperScript(actionFormatSuper->isChecked());
}

void KWView::textSubScript()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->setTextSubScript(actionFormatSub->isChecked());
}

void KWView::textIncreaseIndent()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        double leftMargin = edit->currentParagLayout().margins[QStyleSheetItem::MarginLeft];
        double indent = doc->getIndentValue();
        double newVal = leftMargin + indent;
        // Test commented out. This breaks with the DTP case... The user can put
        // a frame anywhere, even closed to the edges than left/right border allows (DF).
        //if( newVal <= (doc->ptPaperWidth()-doc->ptRightBorder()-doc->ptLeftBorder()))
        {
            edit->setMargin( QStyleSheetItem::MarginLeft, newVal );
        }
    }
}

void KWView::textDecreaseIndent()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        double leftMargin = edit->currentParagLayout().margins[QStyleSheetItem::MarginLeft];
        if ( leftMargin > 0 )
        {
            double indent = doc->getIndentValue();
            double newVal = leftMargin - indent;
            edit->setMargin( QStyleSheetItem::MarginLeft, QMAX( newVal, 0 ) );
        }
    }
}

/*================================================================*/
void KWView::borderOutline()
{
    bool b = actionBorderOutline->isChecked();

    actionBorderLeft->setChecked(b);
    actionBorderRight->setChecked(b);
    actionBorderTop->setChecked(b);
    actionBorderBottom->setChecked(b);

    borderSet();
}

/*================================================================*/
void KWView::borderLeft()
{
    actionBorderOutline->setChecked(
        actionBorderLeft->isChecked() &&
        actionBorderRight->isChecked() &&
        actionBorderTop->isChecked() &&
        actionBorderBottom->isChecked());

    borderSet();
}

/*================================================================*/
void KWView::borderRight()
{
    actionBorderOutline->setChecked(
        actionBorderLeft->isChecked() &&
        actionBorderRight->isChecked() &&
        actionBorderTop->isChecked() &&
        actionBorderBottom->isChecked());

    borderSet();
}

/*================================================================*/
void KWView::borderTop()
{
    actionBorderOutline->setChecked(
        actionBorderLeft->isChecked() &&
        actionBorderRight->isChecked() &&
        actionBorderTop->isChecked() &&
        actionBorderBottom->isChecked());

    borderSet();
}

/*================================================================*/
void KWView::borderBottom()
{
    actionBorderOutline->setChecked(
        actionBorderLeft->isChecked() &&
        actionBorderRight->isChecked() &&
        actionBorderTop->isChecked() &&
        actionBorderBottom->isChecked());

    borderSet();
}

/*================================================================*/
void KWView::borderColor()
{
    m_border.common.color = actionBorderColor->color();
    m_border.left.color = m_border.common.color;
    m_border.right.color = m_border.common.color;
    m_border.top.color = m_border.common.color;
    m_border.bottom.color = m_border.common.color;
    borderSet();
}

/*================================================================*/
void KWView::borderWidth( const QString &width )
{
    m_border.common.ptWidth = width.toInt();
    m_border.left.ptWidth = m_border.common.ptWidth;
    m_border.right.ptWidth = m_border.common.ptWidth;
    m_border.top.ptWidth = m_border.common.ptWidth;
    m_border.bottom.ptWidth = m_border.common.ptWidth;
    borderSet();
    gui->canvasWidget()->setFocus();
}

/*================================================================*/
void KWView::borderStyle( const QString &style )
{
    m_border.common.style = Border::getStyle( style );
    m_border.left.style = m_border.common.style;
    m_border.right.style = m_border.common.style;
    m_border.top.style = m_border.common.style;
    m_border.bottom.style = m_border.common.style;
    borderSet();
    gui->canvasWidget()->setFocus();
}

/*================================================================*/
void KWView::backgroundColor()
{
    // The effect of this action depends on if we are in Edit Text or Edit Frame mode.
    if ( actionToolsEditFrames->isChecked() )
    {
        backColor = actionBackgroundColor->color();
        if ( gui )
            gui->canvasWidget()->setFrameBackgroundColor( backColor );
    }
    //borderSet();
}

/*================================================================*/
void KWView::borderSet()
{
    // The effect of this action depends on if we are in Edit Text or Edit Frame mode.

    m_border.left = m_border.common;
    m_border.right = m_border.common;
    m_border.top = m_border.common;
    m_border.bottom = m_border.common;
    if ( !actionBorderLeft->isChecked() )
    {
        m_border.left.ptWidth = 0;
    }
    if ( !actionBorderRight->isChecked() )
    {
        m_border.right.ptWidth = 0;
    }
    if ( !actionBorderTop->isChecked() )
    {
        m_border.top.ptWidth = 0;
    }
    if ( !actionBorderBottom->isChecked() )
    {
        m_border.bottom.ptWidth = 0;
    }
    if ( actionToolsEditFrames->isChecked() )
    {
        if ( (actionBorderLeft->isChecked() && actionBorderRight->isChecked()
            && actionBorderBottom->isChecked() && actionBorderTop->isChecked())
            || (!actionBorderLeft->isChecked() && !actionBorderRight->isChecked()
            && !actionBorderBottom->isChecked() && !actionBorderTop->isChecked()))
        {
            gui->canvasWidget()->setOutlineFrameBorder( m_border.common, actionBorderLeft->isChecked() );
        }
        else
        {
            gui->canvasWidget()->setLeftFrameBorder( m_border.common, actionBorderLeft->isChecked() );
            gui->canvasWidget()->setRightFrameBorder( m_border.common, actionBorderRight->isChecked() );
            gui->canvasWidget()->setTopFrameBorder( m_border.common, actionBorderTop->isChecked() );
            gui->canvasWidget()->setBottomFrameBorder( m_border.common, actionBorderBottom->isChecked() );
        }
    }
    else
    {
        KWTextFrameSetEdit *edit = currentTextEdit();
        if ( edit )
        {
            edit->setBorders( m_border.left, m_border.right, m_border.top, m_border.bottom );
        }
    }
}

void KWView::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    if ( gui ) gui->resize( width(), height() );
}

void KWView::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    if ( ev->activated() )
    {
        initGui();
    }
    else
    {
        if(statusBar())
        {
            //remove item when GUI is deactivated
            statusBar()->removeItem(statusPage);
        }
    }
    KoView::guiActivateEvent( ev );
}

void KWView::borderShowValues()
{
    actionBorderWidth->setCurrentItem( (int)m_border.common.ptWidth - 1 );
    actionBorderStyle->setCurrentItem( (int)m_border.common.style );
}

void KWView::tabListChanged( const KoTabulatorList & tabList )
{
    if(!doc->isReadWrite())
        return;
    KWTextFrameSetEdit * edit = currentTextEdit();
    if (!edit)
        return;
    edit->setTabList( tabList );
}

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

    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
        edit->textFrameSet()->clearUndoRedoInfo();
    KWPageLayoutCommand *cmd = new KWPageLayoutCommand( i18n("Change Layout"),doc,tmpOldLayout,tmpNewLayout ) ;
    doc->addCommand(cmd);

    doc->updateRuler();

    doc->updateResizeHandles();
}

void KWView::newFirstIndent( double _firstIndent )
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if (!edit) return;
    double val = _firstIndent - edit->currentParagLayout().margins[QStyleSheetItem::MarginLeft];
    edit->setMargin( QStyleSheetItem::MarginFirstLine, val );
}

void KWView::newLeftIndent( double _leftIndent)
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if (edit)
        edit->setMargin( QStyleSheetItem::MarginLeft, _leftIndent );
}

void KWView::openPopupMenuEditText( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() )
        return;

    KWFrameSetEdit * edit = gui->canvasWidget()->currentFrameSetEdit();
    QString menuName;
    if (edit)
        menuName=edit->getPopupName();
    if(!menuName.isEmpty())
        ((QPopupMenu*)factory()->container(menuName,this))->popup(_point);
}

void KWView::openPopupMenuChangeAction( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() )
        return;
    ((QPopupMenu*)factory()->container("action_popup",this))->popup(_point);
}

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

void KWView::openPopupMenuEditFrame( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() )
        return;
    updatePopupMenuChangeAction();
    KWTableFrameSet *table = gui->canvasWidget()->getCurrentTable();
    if(!table)
        ((QPopupMenu*)factory()->container("frame_popup",this))->popup(_point);
    else
        ((QPopupMenu*)factory()->container("frame_popup_table",this))->popup(_point);
}

void KWView::spellCheckerReady()
{
    for ( unsigned int i = m_spellCurrFrameSetNum + 1; i < doc->getNumFrameSets(); i++ ) {
        KWFrameSet *frameset = doc->getFrameSet( i );
        if ( !frameset->isVisible() || frameset->getFrameType() != FT_TEXT )
            continue;
        m_spellCurrFrameSetNum = i; // store as number, not as pointer, to implement "go to next frameset" when done
        kdDebug() << "KWView::spellCheckerReady spell-checking frameset " << m_spellCurrFrameSetNum << endl;
        KWTextFrameSet * textfs = dynamic_cast<KWTextFrameSet*>( frameset );
        QTextParag * p = textfs->textDocument()->firstParag();
        QString text;
        while ( p ) {
            QString str = p->string()->toString();
            str.truncate( str.length() - 1 ); // damn trailing space
            text += str + '\n';
            p = p->next();
        }
        text += '\n';
        kspell->check( text );
        return;
    }
    kdDebug() << "KWView::spellCheckerReady done" << endl;

    // Done
    kspell->cleanUp();
    delete kspell;
    kspell = 0;
}

void KWView::spellCheckerMisspelling( QString old, QStringList* , unsigned pos )
{
    kdDebug() << "KWView::spellCheckerMisspelling old=" << old << " pos=" << pos << endl;
    KWTextFrameSet * fs = dynamic_cast<KWTextFrameSet *>( doc->getFrameSet( m_spellCurrFrameSetNum ) );
    ASSERT( fs );
    if ( !fs ) return;
    QTextParag * p = fs->textDocument()->firstParag();
    while ( p && (int)pos >= p->length() )
    {
        pos -= p->length();
        p = p->next();
    }
    ASSERT( p );
    if ( !p ) return;
    kdDebug() << "KWView::spellCheckerMisspelling p=" << p->paragId() << " pos=" << pos << " length=" << old.length() << endl;
    fs->highlightPortion( p, pos, old.length(), gui->canvasWidget() );
}

void KWView::spellCheckerCorrected( QString old, QString corr, unsigned pos )
{
    kdDebug() << "KWView::spellCheckerCorrected old=" << old << " corr=" << corr << " pos=" << pos << endl;

    KWTextFrameSet * fs = dynamic_cast<KWTextFrameSet *>( doc->getFrameSet( m_spellCurrFrameSetNum ) );
    ASSERT( fs );
    if ( !fs ) return;
    QTextParag * p = fs->textDocument()->firstParag();
    while ( p && (int)pos >= p->length() )
    {
        pos -= p->length();
        p = p->next();
    }
    ASSERT( p );
    if ( !p ) return;
    fs->highlightPortion( p, pos, old.length(), gui->canvasWidget() );

    QTextCursor cursor( fs->textDocument() );
    cursor.setParag( p );
    cursor.setIndex( pos );
    // Remove the match
    fs->removeSelectedText( &cursor, KWTextFrameSet::HighlightSelection );
    // Insert the replacement
    QTextFormat * format = p->at( pos )->format();
    // TODO a macro command somehow
    fs->insert( &cursor, static_cast<KWTextFormat *>(format),
                corr, true, false,
                i18n("Insert Replacement") );
}

void KWView::spellCheckerDone( const QString & )
{
    KWTextFrameSet * fs = dynamic_cast<KWTextFrameSet *>( doc->getFrameSet( m_spellCurrFrameSetNum ) );
    ASSERT( fs );
    if ( fs )
        fs->removeHighlight();

    if ( kspell->dlgResult() != KS_CANCEL && kspell->dlgResult() != KS_STOP )
    {
        // Try to check another frameset
        spellCheckerReady();
    }
    else
    {
        kspell->cleanUp();
        delete kspell;
        kspell = 0;
    }
}

void KWView::spellCheckerFinished()
{
    KSpell::spellStatus status = kspell->status();
    delete kspell;
    kspell = 0;
    if (status == KSpell::Error)
    {
        KMessageBox::sorry(this, i18n("ISpell could not be started.\n"
                                      "Please make sure you have ISpell properly configured and in your PATH.\nUsed settings->configure."));
    }
    else if (status == KSpell::Crashed)
    {
        KMessageBox::sorry(this, i18n("ISpell seems to have crashed."));
    }
    KWTextFrameSet * fs = dynamic_cast<KWTextFrameSet *>( doc->getFrameSet( m_spellCurrFrameSetNum ) );
    ASSERT( fs );
    if ( fs )
        fs->removeHighlight();

    KWTextFrameSetEdit * edit = currentTextEdit();
    if (edit)
        edit->drawCursor( TRUE );
}

void KWView::configure()
{
    KWConfig configDia( this );
    configDia.show();
}

KWTextFrameSetEdit *KWView::currentTextEdit()
{
    if (!gui)
        return 0L;
    KWFrameSetEdit * edit = gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
        return dynamic_cast<KWTextFrameSetEdit *>(edit->currentTextEdit());
    return 0L;
}

void KWView::updateButtons()
{
    KWTextFrameSetEdit * edit = currentTextEdit();

    bool hasSelection = edit && edit->textFrameSet()->hasSelection();
    actionEditCut->setEnabled( hasSelection );
    actionEditCopy->setEnabled( hasSelection );
    clipboardDataChanged(); // for paste

    bool state = (edit != 0L);
    actionEditSelectAll->setEnabled(state);
    actionFormatFont->setEnabled(state);
    actionFormatFontSize->setEnabled(state);
    actionFormatFontFamily->setEnabled(state);
    actionFormatStyle->setEnabled(state);
    actionFormatBold->setEnabled(state);
    actionFormatItalic->setEnabled(state);
    actionFormatUnderline->setEnabled(state);
    actionFormatStrikeOut->setEnabled(state);
    actionFormatColor->setEnabled(state);
    actionFormatAlignLeft->setEnabled(state);
    actionFormatAlignCenter->setEnabled(state);
    actionFormatAlignRight->setEnabled(state);
    actionFormatAlignBlock->setEnabled(state);
    actionFormatIncreaseIndent->setEnabled(state);


    bool goodleftMargin=false;
    if(state)
        goodleftMargin=(edit->currentParagLayout().margins[QStyleSheetItem::MarginLeft]>0);

    actionFormatDecreaseIndent->setEnabled(goodleftMargin && state);

    actionFormatList->setEnabled(state);
    actionFormatSuper->setEnabled(state);
    actionFormatSub->setEnabled(state);
    actionFormatParag->setEnabled(state);
    actionInsertSpecialChar->setEnabled(state);
    actionInsertPicture->setEnabled(state);
    actionInsertFormula->setEnabled(state);
    actionInsertContents->setEnabled(state);
    actionInsertVariable->setEnabled(state);
    actionBackgroundColor->setEnabled(!state && gui->canvasWidget()->getMouseMode()==MM_EDIT_FRAME);

    KWTableFrameSet *table = gui->canvasWidget()->getTable();
    actionTableInsertRow->setEnabled( table );
    actionTableInsertCol->setEnabled( table );
    actionTableDelRow->setEnabled( table );
    actionTableDelCol->setEnabled( table );
    actionTableDelete->setEnabled( table );
    actionTableUngroup->setEnabled( table );

}

void KWView::frameSelectedChanged()
{
    KWTableFrameSet *table = gui->canvasWidget()->getCurrentTable();
    actionTableJoinCells->setEnabled( table );
    actionTableSplitCells->setEnabled( table );
}

void KWView::docStructChanged(TypeStructDocItem _type)
{
    KWDocStruct *m_pDocStruct=gui->getDocStruct();
    if(m_pDocStruct)
        m_pDocStruct->getDocStructTree()->refreshTree(_type);
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
KWGUI::KWGUI( QWidget *parent, KWDocument *_doc, KWView *_view )
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

    connect( r_horz, SIGNAL( openPageLayoutDia() ), view, SLOT( formatPage() ) );
    connect( r_horz, SIGNAL( unitChanged( QString ) ), this, SLOT( unitChanged( QString ) ) );
    connect( r_vert, SIGNAL( newPageLayout( KoPageLayout ) ), view, SLOT( newPageLayout( KoPageLayout ) ) );
    connect( r_vert, SIGNAL( openPageLayoutDia() ), view, SLOT( formatPage() ) );
    connect( r_vert, SIGNAL( unitChanged( QString ) ), this, SLOT( unitChanged( QString ) ) );

    r_horz->setUnit( doc->getUnitName() );
    r_vert->setUnit( doc->getUnitName() );

    r_horz->hide();
    r_vert->hide();

    canvas->show();
    docStruct->show();

    reorganize();

#if 0
    if ( doc->processingType() == KWDocument::DTP )   // ???
        canvas->setRuler2Frame( 0, 0 );
#endif

    connect( r_horz, SIGNAL( tabListChanged( const KoTabulatorList & ) ), view,
             SLOT( tabListChanged( const KoTabulatorList & ) ) );

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
    int space=20;
    if(doc->getShowRuler())
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

    panner->setGeometry( 0, 0, width(), height() );
    canvas->setGeometry( space, space, left->width() - space, left->height() - space );
    r_horz->setGeometry( space, 0, left->width() - space, space );
    r_vert->setGeometry( 0, space, space, left->height() - space );
}

/*================================================================*/
void KWGUI::unitChanged( QString u )
{
    doc->setUnit( KWUnit::unit( u ) );
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

#include "kwview.moc"
