/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <qprinter.h> // has to be first

// #include <iostream>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <qpushbutton.h>
#include <qprintdialog.h>
#include <qcollection.h>
#include <qkeycode.h>
#include <qmime.h>
#include <qtoolbutton.h>
#include <qtimer.h>
#include <qframe.h>
#include <qscrollbar.h>
#include <qbutton.h>
#include <qpopupmenu.h>
#include <qdir.h>

#include <kapp.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kstdaccel.h>
#include <kstdaction.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <kformulaedit.h>
#include <kcoloractions.h>
#include <kaction.h>

#include <kprocess.h>

#include <dcopclient.h>
#include <dcopref.h>

#include <koFind.h>
#include <koPartSelectDia.h>
#include <koQueryTrader.h>
#include <koReplace.h>
#include <koMainWindow.h>



#include "kspread_map.h"
#include "kspread_table.h"
#include "kspread_dlg_scripts.h"
#include "kspread_doc.h"
#include "kspread_dlg_cons.h"
#include "kspread_util.h"
#include "kspread_canvas.h"
#include "kspread_tabbar.h"
#include "kspread_view.h"
#include "kspread_dlg_formula.h"
#include "kspread_dlg_special.h"
#include "kspread_dlg_sort.h"
#include "kspread_dlg_anchor.h"
#include "kspread_dlg_layout.h"
#include "kspread_dlg_show.h"
#include "kspread_dlg_insert.h"
#include "kspread_handler.h"
#include "kspread_events.h"
#include "kspread_editors.h"
#include "kspread_dlg_format.h"
// #include "kspread_dlg_oszi.h"
#include "kspread_dlg_conditional.h"
#include "kspread_dlg_series.h"
#include "kspread_dlg_reference.h"
#include "kspread_dlg_area.h"
#include "kspread_dlg_resize2.h"
#include "kspread_dlg_preference.h"
#include "kspread_dlg_comment.h"
#include "kspread_dlg_angle.h"
#include "kspread_dlg_goto.h"
#include "kspread_dlg_validity.h"
#include "kspread_dlg_pasteinsert.h"
#include "kspread_dlg_showColRow.h"
#include "kspread_dlg_list.h"
#include "kspread_undo.h"

#include "handler.h"

#include "KSpreadViewIface.h"
#include <kdebug.h>

/*****************************************************************************
 *
 * KSpreadView
 *
 *****************************************************************************/

KSpreadScripts* KSpreadView::m_pGlobalScriptsDialog = 0L;

KSpreadView::KSpreadView( QWidget *_parent, const char *_name, KSpreadDoc* doc ) :
  KoView( doc, _parent, _name )
{
    setInstance( KSpreadFactory::global() );
    setXMLFile( "kspread.rc" );

    m_toolbarLock = FALSE;

    m_pDoc = doc;
    m_pPopupMenu = 0;
    m_pPopupColumn = 0;
    m_pPopupRow = 0;
    m_popupChild = 0;
    m_popupListChoose=0;

    m_dcop = 0;
    m_bLoading =false;

    // Vert. Scroll Bar
    m_pVertScrollBar = new QScrollBar( this, "ScrollBar_2" );
    m_pVertScrollBar->setRange( 0, 4096 );
    m_pVertScrollBar->setOrientation( QScrollBar::Vertical );

    // Horz. Scroll Bar
    m_pHorzScrollBar = new QScrollBar( this, "ScrollBar_1" );
    m_pHorzScrollBar->setRange( 0, 4096 );
    m_pHorzScrollBar->setOrientation( QScrollBar::Horizontal );

    // Tab Bar
    m_pTabBarFirst = newIconButton( "tab_first" );
    QObject::connect( m_pTabBarFirst, SIGNAL( clicked() ), SLOT( slotScrollToFirstTable() ) );
    m_pTabBarLeft = newIconButton( "tab_left" );
    QObject::connect( m_pTabBarLeft, SIGNAL( clicked() ), SLOT( slotScrollToLeftTable() ) );
    m_pTabBarRight = newIconButton( "tab_right" );
    QObject::  connect( m_pTabBarRight, SIGNAL( clicked() ), SLOT( slotScrollToRightTable() ) );
    m_pTabBarLast = newIconButton( "tab_last" );
    QObject::connect( m_pTabBarLast, SIGNAL( clicked() ), SLOT( slotScrollToLastTable() ) );

    m_pTabBar = new KSpreadTabBar( this );
    QObject::connect( m_pTabBar, SIGNAL( tabChanged( const QString& ) ), this, SLOT( changeTable( const QString& ) ) );


    // Paper and Border widgets
    m_pFrame = new QWidget( this );
    m_pFrame->raise();

    // Edit Bar
    m_pToolWidget = new QFrame( this );
    // m_pToolWidget->setFrameStyle( 49 );

    QHBoxLayout* hbox = new QHBoxLayout( m_pToolWidget );
    hbox->addSpacing( 2 );

    //m_pPosWidget = new QLabel( m_pToolWidget );
    //m_pPosWidget->setAlignment( AlignCenter );
    //m_pPosWidget->setFrameStyle( QFrame::WinPanel|QFrame::Sunken );
    m_pPosWidget = new KSpreadLocationEditWidget( m_pToolWidget, this );

    m_pPosWidget->setMinimumWidth( 100 );
    hbox->addWidget( m_pPosWidget );
    hbox->addSpacing( 6 );

    m_pCancelButton = newIconButton( "abort", TRUE, m_pToolWidget );
    hbox->addWidget( m_pCancelButton );
    m_pOkButton = newIconButton( "done", TRUE, m_pToolWidget );
    hbox->addWidget( m_pOkButton );
    hbox->addSpacing( 6 );

    // The widget on which we display the table
    m_pCanvas = new KSpreadCanvas( m_pFrame, this, doc );

    // The line-editor that appears above the table and allows to
    // edit the cells content. It knows about the two buttons.
    m_pEditWidget = new KSpreadEditWidget( m_pToolWidget, m_pCanvas, m_pCancelButton, m_pOkButton );
    m_pEditWidget->setFocusPolicy( QWidget::StrongFocus );
    hbox->addWidget( m_pEditWidget, 2 );
    hbox->addSpacing( 2 );

    m_pCanvas->setEditWidget( m_pEditWidget );

    m_pHBorderWidget = new KSpreadHBorder( m_pFrame, m_pCanvas,this );
    m_pVBorderWidget = new KSpreadVBorder( m_pFrame, m_pCanvas ,this );

    m_pCanvas->setFocusPolicy( QWidget::StrongFocus );
    QWidget::setFocusPolicy( QWidget::StrongFocus );
    setFocusProxy( m_pCanvas );

    connect( this, SIGNAL( invalidated() ), m_pCanvas, SLOT( update() ) );
    connect( this, SIGNAL( regionInvalidated( const QRegion&, bool ) ),
             m_pCanvas, SLOT( repaint( const QRegion&, bool ) ) );

    QObject::connect( m_pVertScrollBar, SIGNAL( valueChanged(int) ), m_pCanvas, SLOT( slotScrollVert(int) ) );
    QObject::connect( m_pHorzScrollBar, SIGNAL( valueChanged(int) ), m_pCanvas, SLOT( slotScrollHorz(int) ) );

    KSpreadTable *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
        addTable( tbl );
    tbl = m_pDoc->map()->initialActiveTable();
    if (tbl)
        setActiveTable(tbl);
    else
        //activate first table which is not hiding
        setActiveTable(m_pDoc->map()->findTable(m_pTabBar->listshow().first()));

    QObject::connect( m_pDoc, SIGNAL( sig_addTable( KSpreadTable* ) ), SLOT( slotAddTable( KSpreadTable* ) ) );


    QObject::connect( m_pDoc, SIGNAL( sig_refreshView(  ) ), this, SLOT( slotRefreshView() ) );

    // Handler for moving and resizing embedded parts
    ContainerHandler* h = new ContainerHandler( this, m_pCanvas );
    connect( h, SIGNAL( popupMenu( KoChild*, const QPoint& ) ), this, SLOT( popupChildMenu( KoChild*, const QPoint& ) ) );

    m_bold = new KToggleAction( i18n("Bold"), "text_bold", CTRL + Key_B, actionCollection(), "bold");
    connect( m_bold, SIGNAL( toggled( bool ) ), this, SLOT( bold( bool ) ) );
    m_italic = new KToggleAction( i18n("Italic"), "text_italic", CTRL + Key_I, actionCollection(), "italic");
    connect( m_italic, SIGNAL( toggled( bool ) ), this, SLOT( italic( bool ) ) );
    m_underline = new KToggleAction( i18n("Underline"), "text_under", CTRL + Key_U, actionCollection(), "underline");
    connect( m_underline, SIGNAL( toggled( bool ) ), this, SLOT( underline( bool ) ) );

    m_strikeOut = new KToggleAction( i18n("Strike out"), "strike_out", 0, actionCollection(), "strikeout");
    connect( m_strikeOut, SIGNAL( toggled( bool ) ), this, SLOT( strikeOut( bool ) ) );

    m_percent = new KToggleAction( i18n("Percent format"), "percent", 0, actionCollection(), "percent");
    connect( m_percent, SIGNAL( toggled( bool ) ), this, SLOT( percent( bool ) ) );
    m_precplus = new KAction( i18n("Increase precision"), "prec_plus", 0, this,
                              SLOT( precisionPlus() ), actionCollection(), "precplus");
    m_precminus = new KAction( i18n("Decrease precision"), "prec_minus", 0, this,
                              SLOT( precisionMinus() ), actionCollection(), "precminus");
    m_money = new KToggleAction( i18n("Money format"), "money", 0, actionCollection(), "money");
    connect( m_money, SIGNAL( toggled( bool ) ), this, SLOT( moneyFormat( bool ) ) );
    m_alignLeft = new KToggleAction( i18n("Align left"), "left", 0, actionCollection(), "left");
    connect( m_alignLeft, SIGNAL( toggled( bool ) ), this, SLOT( alignLeft( bool ) ) );
    m_alignLeft->setExclusiveGroup( "Align" );
    m_alignCenter = new KToggleAction( i18n("Align center"), "center", 0, actionCollection(), "center");
    connect( m_alignCenter, SIGNAL( toggled( bool ) ), this, SLOT( alignCenter( bool ) ) );
    m_alignCenter->setExclusiveGroup( "Align" );
    m_alignRight = new KToggleAction( i18n("Align right"), "right", 0, actionCollection(), "right");
    connect( m_alignRight, SIGNAL( toggled( bool ) ), this, SLOT( alignRight( bool ) ) );
    m_alignRight->setExclusiveGroup( "Align" );

    m_alignTop = new KToggleAction( i18n("Align top"), "text_top", 0, actionCollection(), "top");
    connect( m_alignTop, SIGNAL( toggled( bool ) ), this, SLOT( alignTop( bool ) ) );
    m_alignTop->setExclusiveGroup( "Pos" );
    m_alignMiddle = new KToggleAction( i18n("Align middle"), "middle", 0, actionCollection(), "middle");
    connect( m_alignMiddle, SIGNAL( toggled( bool ) ), this, SLOT( alignMiddle( bool ) ) );
    m_alignMiddle->setExclusiveGroup( "Pos" );
    m_alignBottom = new KToggleAction( i18n("Align bottom"), "text_bottom", 0, actionCollection(), "bottom");
    connect( m_alignBottom, SIGNAL( toggled( bool ) ), this, SLOT( alignBottom( bool ) ) );
    m_alignBottom->setExclusiveGroup( "Pos" );

    m_transform = new KAction( i18n("Transform object..."), "rotate", 0, this, SLOT( transformPart() ),
                               actionCollection(), "transform" );
    m_transform->setEnabled( FALSE );
    // ### This seems to be duplicated ....
    connect( m_transform, SIGNAL( activated() ), this, SLOT( transformPart() ) );
    m_copy = KStdAction::copy( this, SLOT( copySelection() ), actionCollection(), "copy" );
    m_paste = KStdAction::paste( this, SLOT( paste() ), actionCollection(), "paste" );
    m_cut = KStdAction::cut( this, SLOT( cutSelection() ), actionCollection(), "cut" );
    m_specialPaste = new KAction( i18n("Special Paste..."), "special_paste",0, this, SLOT( specialPaste() ), actionCollection(), "specialPaste" );
    m_editCell = new KAction( i18n("Modify Cell"),"cell_edit", CTRL + Key_M, this, SLOT( editCell() ), actionCollection(), "editCell" );
    m_delete = new KAction( i18n("Delete"),"deletecell", 0, this, SLOT( deleteSelection() ), actionCollection(), "delete" );
    m_clearText = new KAction( i18n("Text"), 0, this, SLOT( clearTextSelection() ), actionCollection(), "cleartext" );
    m_clearComment = new KAction( i18n("Comment"), 0, this, SLOT( clearCommentSelection() ), actionCollection(), "clearcomment" );
    m_clearValidity = new KAction( i18n("Validity"), 0, this, SLOT( clearValiditySelection() ), actionCollection(), "clearvalidity" );
    m_clearConditional = new KAction( i18n("Conditional cell attributes"), 0, this, SLOT( clearConditionalSelection() ), actionCollection(), "clearconditional" );
    // ---------------------------- Bernd -----------------------
    //Key_Shift ..

    m_recalc_workbook = new KAction( i18n("Recalculate Workbook"),Key_F9, this, SLOT( RecalcWorkBook() ), actionCollection(), "RecalcWorkBook" );
    m_recalc_worksheet = new KAction( i18n("Recalculate Worksheet"),SHIFT + Key_F9, this, SLOT( RecalcWorkSheet() ), actionCollection(), "RecalcWorkSheet" );

    // ---------------------------- Bernd -----------------------




    m_adjust = new KAction( i18n("Adjust row and column"), 0, this, SLOT( adjust() ), actionCollection(), "adjust" );
    m_default = new KAction( i18n("Default"), 0, this, SLOT( defaultSelection() ), actionCollection(), "default" );
    m_areaName = new KAction( i18n("Area name"), 0, this, SLOT( setAreaName() ), actionCollection(), "areaname" );
    m_showArea = new KAction( i18n("Show area..."), 0, this, SLOT( showAreaName() ), actionCollection(), "showArea" );
    m_resizeRow = new KAction( i18n("Resize row..."), "resizerow", 0, this, SLOT( resizeRow() ), actionCollection(), "resizeRow" );
    m_resizeColumn = new KAction( i18n("Resize column..."), "resizecol", 0, this, SLOT( resizeColumn() ), actionCollection(), "resizeCol" );
    m_equalizeRow = new KAction( i18n("Equalize row"), "adjustrow", 0, this, SLOT( equalizeRow() ), actionCollection(), "equalizeRow" );
    m_equalizeColumn = new KAction( i18n("Equalize column"), "adjustcol", 0, this, SLOT( equalizeColumn() ), actionCollection(), "equalizeCol" );

    m_fontSizeUp = new KAction( i18n("Increase font size"), "fontsizeup", 0, this, SLOT( increaseFontSize() ), actionCollection(), "increaseFontSize" );
    m_fontSizeDown = new KAction( i18n("Decrease font size"), "fontsizedown", 0, this, SLOT( decreaseFontSize() ), actionCollection(), "decreaseFontSize" );
    m_upper = new KAction( i18n("Upper case"), "upper", 0, this, SLOT( upper() ), actionCollection(), "upper" );
    m_lower = new KAction( i18n("Lower case"), "lower", 0, this, SLOT( lower() ), actionCollection(), "lower" );
    m_gotoCell = new KAction( i18n("Goto Cell..."),"goto", 0, this, SLOT( gotoCell() ), actionCollection(), "gotoCell" );
    m_undo = KStdAction::undo( this, SLOT( undo() ), actionCollection(), "undo" );
    m_undo->setEnabled( FALSE );
    m_redo = KStdAction::redo( this, SLOT( redo() ), actionCollection(), "redo" );
    m_redo->setEnabled( FALSE );
    m_paperLayout = new KAction( i18n("Paper Layout..."), 0, this, SLOT( paperLayoutDlg() ), actionCollection(), "paperLayout" );

    m_sortList = new KAction( i18n("Sort lists..."), 0, this, SLOT( sortList() ), actionCollection(), "sortlist" );


    m_insertTable = new KAction( i18n("Insert Table"),"inserttable", 0, this, SLOT( insertTable() ), actionCollection(), "insertTable" );
    m_removeTable = new KAction( i18n("Remove Table"), "delete_table",0,this, SLOT( removeTable() ), actionCollection(), "removeTable" );
    m_showTable = new KAction(i18n("Show Table"),0 ,this,SLOT( showTable()), actionCollection(), "showTable" );
    m_hideTable = new KAction(i18n("Hide Table"),0 ,this,SLOT( hideTable()), actionCollection(), "hideTable" );
    m_preference = new KAction( i18n("Configure Kspread..."),BarIcon("configure"), 0, this, SLOT( preference() ), actionCollection(), "preference" );
    m_firstLetterUpper = new KAction( i18n("Convert first letter to upper case"),"first_letter_upper" ,0, this, SLOT( firstLetterUpper() ), actionCollection(), "firstletterupper" );
    m_verticalText = new KToggleAction( i18n("Vertical text"),"vertical_text" ,0 ,actionCollection(), "verticaltext" );
    connect( m_verticalText, SIGNAL( toggled( bool ) ), this, SLOT( verticalText( bool ) ) );
    m_changeAngle = new KAction( i18n("Change Angle..."), 0, this, SLOT( changeAngle() ), actionCollection(), "changeangle" );
    m_addModifyComment = new KAction( i18n("&Add/modify comment..."),"comment", 0, this, SLOT( addModifyComment() ), actionCollection(), "addmodifycomment" );
    m_removeComment = new KAction( i18n("&Remove comment"),"removecomment", 0, this, SLOT( removeComment() ), actionCollection(), "removecomment" );
    m_editGlobalScripts = new KAction( i18n("Edit Global Scripts..."), 0, this, SLOT( editGlobalScripts() ),
                                       actionCollection(), "editGlobalScripts" );
    m_editLocalScripts = new KAction( i18n("Edit Local Scripts..."), 0, this, SLOT( editLocalScripts() ), actionCollection(), "editLocalScripts" );
    m_reloadScripts = new KAction( i18n("Reload Scripts"), 0, this, SLOT( reloadScripts() ), actionCollection(), "reloadScripts" );
    m_showPageBorders = new KToggleAction( i18n("Show page borders"), 0, actionCollection(), "showPageBorders");
    connect( m_showPageBorders, SIGNAL( toggled( bool ) ), this, SLOT( togglePageBorders( bool ) ) );
    KStdAction::find(this, SLOT(find()), actionCollection());
    KStdAction::replace(this, SLOT(replace()), actionCollection());
    m_conditional = new KAction( i18n("Conditional cell attributes..."), 0, this, SLOT( conditional() ), actionCollection(), "conditional" );
    m_validity = new KAction( i18n("Validity..."), 0, this, SLOT( validity() ), actionCollection(), "validity" );
    m_sort = new KAction( i18n("Sort..."), 0, this, SLOT( sort() ), actionCollection(), "sort" );
    m_consolidate = new KAction( i18n("Consolidate..."), 0, this, SLOT( consolidate() ), actionCollection(), "consolidate" );
    m_mergeCell = new KAction( i18n("Merge cells"),"mergecell" ,0, this, SLOT( mergeCell() ), actionCollection(), "mergecell" );
    m_dissociateCell = new KAction( i18n("Dissociate cells"),"dissociatecell" ,0, this, SLOT( dissociateCell() ), actionCollection(), "dissociatecell" );

    m_increaseIndent = new KAction( i18n("Increase indent"),"increaseindent" ,0, this, SLOT( increaseIndent() ), actionCollection(), "increaseindent" );
    m_decreaseIndent = new KAction( i18n("Decrease indent"),"decreaseindent" ,0, this, SLOT( decreaseIndent() ), actionCollection(),"decreaseindent");

    m_multiRow = new KToggleAction( i18n("Multi Row"), "multirow", 0, actionCollection(), "multiRow" );
    connect( m_multiRow, SIGNAL( toggled( bool ) ), this, SLOT( multiRow( bool ) ) );
    m_selectFont = new KFontAction( i18n("Select Font"), 0, actionCollection(), "selectFont" );
    connect( m_selectFont, SIGNAL( activated( const QString& ) ), this, SLOT( fontSelected( const QString& ) ) );
    m_selectFontSize = new KFontSizeAction( i18n("Select Font Size"), 0, actionCollection(), "selectFontSize" );
    connect( m_selectFontSize, SIGNAL( fontSizeChanged( int ) ), this, SLOT( fontSizeSelected( int ) ) );
    m_deleteColumn = new KAction( i18n("Delete Column(s)"), "delete_table_col", 0, this, SLOT( deleteColumn() ),
                                  actionCollection(), "deleteColumn" );
    m_deleteRow = new KAction( i18n("Delete Row(s)"), "delete_table_row", 0, this, SLOT( deleteRow() ),
                               actionCollection(), "deleteRow" );
    m_insertColumn = new KAction( i18n("Insert Column(s)"), "insert_table_col" , 0, this, SLOT( insertColumn() ),
                                  actionCollection(), "insertColumn" );
    m_insertRow = new KAction( i18n("Insert Row(s)"), "insert_table_row", 0, this, SLOT( insertRow() ),
                               actionCollection(), "insertRow" );
    m_hideRow = new KAction( i18n("Hide Row(s)"), "hide_table_row", 0, this, SLOT( hideRow() ),
                               actionCollection(), "hideRow" );
    m_showRow = new KAction( i18n("Show Row(s)"), "show_table_row", 0, this, SLOT( showRow() ),
                               actionCollection(), "showRow" );
    m_hideColumn = new KAction( i18n("Hide Column(s)"), "hide_table_column", 0, this, SLOT( hideColumn() ),
                               actionCollection(), "hideColumn" );
    m_showColumn = new KAction( i18n("Show Column(s)"), "show_table_column", 0, this, SLOT( showColumn() ),
                               actionCollection(), "showColumn" );
    m_insertCell = new KAction( i18n("Insert Cell(s)..."), "insertcell", 0, this, SLOT( slotInsert() ),
                               actionCollection(), "insertCell" );
    m_removeCell = new KAction( i18n("Remove Cell(s)..."), "removecell", 0, this, SLOT( slotRemove() ),
                               actionCollection(), "removeCell" );
    m_insertCellCopy = new KAction( i18n("Paste with insertion..."), "insertcellcopy", 0, this, SLOT( slotInsertCellCopy() ),
                               actionCollection(), "insertCellCopy" );
    m_cellLayout = new KAction( i18n("Cell Layout..."),"cell_layout", CTRL + Key_L, this, SLOT( layoutDlg() ),
                               actionCollection(), "cellLayout" );
    m_formulaPower = new KAction( i18n("Formula Power"), "rsup", 0, this, SLOT( formulaPower() ),
                                actionCollection(), "formulaPower" );
    m_formulaSubscript = new KAction( i18n("Formula Subscript"), "rsub", 0, this, SLOT( formulaSubscript() ),
                                      actionCollection(), "formulaSubscript" );
    m_formulaParantheses = new KAction( i18n("Formula Parentheses"), "paren", 0, this, SLOT( formulaParentheses() ),
                                        actionCollection(), "formulaParentheses" );
    m_formulaAbsValue = new KAction( i18n("Formula Abs Value"), "abs", 0, this, SLOT( formulaAbsValue() ),
                                     actionCollection(), "formulaAbsValue" );
    m_formulaBrackets = new KAction( i18n("Formula Brackets"), "brackets", 0, this, SLOT( formulaBrackets() ),
                                     actionCollection(), "formulaBrackets" );
    m_formulaFraction = new KAction( i18n("Formula Fraction"), "frac", 0, this, SLOT( formulaFraction() ),
                                     actionCollection(), "formulaFraction" );
    m_formulaRoot = new KAction( i18n("Formula Root"), "sqrt", 0, this, SLOT( formulaRoot() ),
                                 actionCollection(), "formulaRoot" );
    m_formulaIntegral = new KAction( i18n("Formula Integral"), "int", 0, this, SLOT( formulaIntegral() ),
                                     actionCollection(), "formulaIntegral" );
    m_formulaMatrix = new KAction( i18n("Formula Matrix"), "matrix", 0, this, SLOT( formulaMatrix() ),
                                   actionCollection(), "formulaMatrix" );
    m_formulaLeftSuper = new KAction( i18n("Formula Left Super"), "lsup", 0, this, SLOT( formulaLeftSuper() ),
                                      actionCollection(), "formulaLeftSuper" );
    m_formulaLeftSub = new KAction( i18n("Formula Left Sub"), "lsub", 0, this, SLOT( formulaLeftSub() ),
                                    actionCollection(), "formulaLeftSub" );
    m_formulaSum = new KAction( i18n("Formula Sum"), "sum", 0, this, SLOT( formulaSum() ),
                                actionCollection(), "formulaSum" );
    m_formulaProduct = new KAction( i18n("Formula Product"), "prod", 0, this, SLOT( formulaProduct() ),
                                    actionCollection(), "formulaProduct");
    m_formulaSelection = new KSelectAction( i18n("Formula Selection"), 0, actionCollection(), "formulaSelection" );

    QStringList lst;
    lst.append( "sum");
    lst.append( "cos");
    lst.append( "sqrt");
    lst.append( i18n("Others...") );
    ((KSelectAction*) m_formulaSelection)->setItems( lst );
    m_formulaSelection->setCurrentItem(0);
    connect( m_formulaSelection, SIGNAL( activated( const QString& ) ),
             this, SLOT( formulaSelection( const QString& ) ) );

    // Insert menu
    (void) new KAction( i18n("&Math expression..."), "funct", 0, this, SLOT( insertMathExpr() ),
                        actionCollection(), "insertMathExpr" );
    (void) new KAction( i18n("&Formula"), "formula", 0, this, SLOT( insertFormula() ),
                        actionCollection(), "insertFormula" );
    (void) new KAction( i18n("&Series..."),"series", 0, this, SLOT( insertSeries() ), actionCollection(), "series" );
    (void) new KAction( i18n("&Hyperlink..."), 0, this, SLOT( insertHyperlink() ), actionCollection(), "insertHyperlink" );
    (void) new KAction( i18n("&Object..."), "parts", 0, this, SLOT( insertObject() ),
                        actionCollection(), "insertPart");
    (void) new KAction( i18n("&Chart"), "chart", 0, this, SLOT( insertChart() ), actionCollection(), "insertChart" );

    m_autoSum = new KAction( i18n("AutoSum"), "black_sum", 0, this, SLOT( autoSum() ),
                             actionCollection(), "autoSum" );
    m_sortDec = new KAction( i18n("Sort descreasing"), "sort_decrease", 0, this, SLOT( sortDec() ),
                             actionCollection(), "sortDec" );
    m_sortInc = new KAction( i18n("Sort increasing"), "sort_incr", 0, this, SLOT( sortInc() ),
                             actionCollection(), "sortInc" );
    m_textColor = new KSelectColorAction( i18n("Text color"), KColorAction::TextColor, 0, this, SLOT( changeTextColor() ),
                               actionCollection(), "textColor" );
    m_bgColor = new KSelectColorAction( i18n("Background color"), KColorAction::BackgroundColor, 0, this, SLOT( changeBackgroundColor() ),
                               actionCollection(), "backgroundColor" );
    m_borderLeft = new KAction( i18n("Border left"), "border_left", 0, this, SLOT( borderLeft() ), actionCollection(), "borderLeft" );
    m_borderRight = new KAction( i18n("Border Right"), "border_right", 0, this, SLOT( borderRight() ), actionCollection(), "borderRight" );
    m_borderTop = new KAction( i18n("Border Top"), "border_top", 0, this, SLOT( borderTop() ), actionCollection(), "borderTop" );
    m_borderBottom = new KAction( i18n("Border Bottom"), "border_bottom", 0, this, SLOT( borderBottom() ), actionCollection(), "borderBottom" );
    m_borderAll = new KAction( i18n("All borders"), "border_all", 0, this, SLOT( borderAll() ), actionCollection(), "borderAll" );
    m_borderRemove = new KAction( i18n("Remove Borders"), "border_remove", 0, this, SLOT( borderRemove() ), actionCollection(), "borderRemove" );
    m_borderOutline = new KAction( i18n("Border Outline"), ("border_outline"), 0, this, SLOT( borderOutline() ), actionCollection(), "borderOutline" );
    m_borderColor = new KSelectColorAction( i18n("Border Color"), KColorAction:: FrameColor, 0, this, SLOT( changeBorderColor() ),
                               actionCollection(), "borderColor" );
    m_tableFormat = new KAction( i18n("Table Style..."), 0, this, SLOT( tableFormat() ), actionCollection(), "tableFormat" );
    // m_oszi = new KAction( i18n("Osciloscope..."),"oscilloscope", 0, this, SLOT( oszilloscope() ), actionCollection(), "oszi" );

    connect( this, SIGNAL( childSelected( KoDocumentChild* ) ),
             this, SLOT( slotChildSelected( KoDocumentChild* ) ) );
    connect( this, SIGNAL( childUnselected( KoDocumentChild* ) ),
             this, SLOT( slotChildUnselected( KoDocumentChild* ) ) );
    // If a selected part becomes active this is like it is deselected
    // just before.
    connect( this, SIGNAL( childActivated( KoDocumentChild* ) ),
             this, SLOT( slotChildUnselected( KoDocumentChild* ) ) );

    QTimer::singleShot( 0, this, SLOT( initialPosition() ) );
    m_findOptions = 0;
}

KSpreadView::~KSpreadView()
{
    if ( !m_transformToolBox.isNull() )
	delete (&*m_transformToolBox);
    m_pCanvas->endChoose();
    m_pTable = 0; // set the active table to 0L so that when during destruction
    // of embedded child documents possible repaints in KSpreadTable are not
    // performed. The repains can happen if you delete an embedded document,
    // which leads to an regionInvalidated() signal emission in KoView, which calls
    // repaint, etc.etc. :-) (Simon)

    delete m_pPopupColumn;
    delete m_pPopupRow;
    delete m_pPopupMenu;
    delete m_popupChild;
    delete m_popupListChoose;
}


void KSpreadView::initConfig()
{
KConfig *config = KSpreadFactory::global()->config();
if( config->hasGroup("Parameters" ))
        {
        config->setGroup( "Parameters" );
        m_pDoc->setShowHorizontalScrollBar(config->readBoolEntry("Horiz ScrollBar",true));
        m_pDoc->setShowVerticalScrollBar(config->readBoolEntry("Vert ScrollBar",true));
        m_pDoc->setShowColHeader(config->readBoolEntry("Column Header",true));
        m_pDoc->setShowRowHeader(config->readBoolEntry("Row Header",true));
        m_pDoc->setCompletionMode((KGlobalSettings::Completion)config->readNumEntry("Completion Mode",(int)(KGlobalSettings::CompletionAuto)));
        m_pDoc->setMoveToValue((MoveTo)config->readNumEntry("Move",(int)(Bottom)));
        m_pDoc->setIndentValue(config->readNumEntry( "Indent",10 ) );
        m_pDoc->setTypeOfCalc((MethodOfCalc)config->readNumEntry("Method of Calc",(int)(Sum)));
	m_pDoc->setShowTabBar(config->readBoolEntry("Tabbar",true));

	QColor _col(Qt::lightGray);
	_col= config->readColorEntry("GridColor",&_col);
	m_pDoc->changeDefaultGridPenColor(_col);
	m_pDoc->setShowMessageError(config->readBoolEntry( "Msg error" ,false) );
	
	m_pDoc->setShowCommentIndicator(config->readBoolEntry("Comment Indicator",true));

	m_pDoc->setPaperFormat((KoFormat)config->readNumEntry("Default size page",1));
	m_pDoc->setPaperOrientation((KoOrientation)config->readNumEntry("Default orientation page",0));
	
	m_pDoc->setShowFormularBar(config->readBoolEntry("Formula bar",true));
	}
}

void KSpreadView::RecalcWorkBook(){

    KSpreadTable *tbl;

    for ( tbl = m_pDoc->map()->firstTable();
	  tbl != 0L; tbl = m_pDoc->map()->nextTable() ){
      tbl->recalc(true);
    }
    
    //    slotUpdateView( activeTable() );

}

void KSpreadView::RecalcWorkSheet(){

  if (m_pTable!= 0)
    m_pTable->recalc(true);

  //slotUpdateView( activeTable() );

}
void KSpreadView::initialPosition()
{
    // Set the initial position for the marker as store in the XML file,
    // (1,1) otherwise
    int col = m_pDoc->map()->initialMarkerColumn();
    if ( col <= 0 ) col = 1;
    int row = m_pDoc->map()->initialMarkerRow();
    if ( row <= 0 ) row = 1;
    m_pCanvas->gotoLocation( col, row );

    //init toggle button
    m_showPageBorders->setChecked( m_pTable->isShowPageBorders());
    m_tableFormat->setEnabled(false);
    /*recalc all dependent after loading*/
    KSpreadTable *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
        {
        if(tbl->getAutoCalc())
                tbl->recalc(true);
        tbl->refreshMergedCell();
        }

    slotUpdateView( activeTable() );
    m_bLoading =true;

    if ( koDocument()->isReadWrite() )
      initConfig();
}

void KSpreadView::updateEditWidget()
{
    bool active=activeTable()->getShowFormular();
    m_alignLeft->setEnabled(!active);
    m_alignCenter->setEnabled(!active);
    m_alignRight->setEnabled(!active); 
    
    m_toolbarLock = TRUE;

    KSpreadCell* cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
    if ( !cell )
    {
        editWidget()->setText( "" );
        return;
    }

    if ( cell->content() == KSpreadCell::VisualFormula )
        editWidget()->setText( "" );
    else
        editWidget()->setText( cell->text() );

    m_selectFontSize->setFontSize( cell->textFontSize( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    m_selectFont->setFont( cell->textFontFamily( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    m_bold->setChecked( cell->textFontBold( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    m_italic->setChecked( cell->textFontItalic( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    m_underline->setChecked( cell->textFontUnderline( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    m_strikeOut->setChecked( cell->textFontStrike( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );

    if ( cell->align( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) == KSpreadLayout::Left )
        m_alignLeft->setChecked( TRUE );
    else if ( cell->align( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) == KSpreadLayout::Right )
        m_alignRight->setChecked( TRUE );
    else if ( cell->align( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) == KSpreadLayout::Center )
        m_alignCenter->setChecked( TRUE );
    else
    {
        m_alignLeft->setChecked( FALSE );
        m_alignRight->setChecked( FALSE );
        m_alignCenter->setChecked( FALSE );
    }

    if ( cell->alignY( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) == KSpreadLayout::Top )
        m_alignTop->setChecked( TRUE );
    else if ( cell->alignY( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) == KSpreadLayout::Middle )
        m_alignMiddle->setChecked( TRUE );
    else if ( cell->alignY( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) == KSpreadLayout::Bottom )
        m_alignBottom->setChecked( TRUE );

    m_verticalText->setChecked( cell->verticalText( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );

    m_multiRow->setChecked( cell->multiRow( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    if( cell->getFormatNumber( m_pCanvas->markerColumn(), m_pCanvas->markerRow()) == KSpreadCell::Percentage )
        m_percent->setChecked( TRUE );
    else
        m_percent->setChecked( FALSE );

    if( cell->getFormatNumber( m_pCanvas->markerColumn(), m_pCanvas->markerRow()) == KSpreadCell::Money )
        m_money->setChecked( TRUE );
    else
        m_money->setChecked( FALSE );

    if( cell->comment(m_pCanvas->markerColumn(), m_pCanvas->markerRow()).isEmpty() )
        m_removeComment->setEnabled( FALSE );
    else
        m_removeComment->setEnabled( TRUE );

    m_toolbarLock = FALSE;
}

void KSpreadView::activateFormulaEditor()
{
    if ( m_pCanvas->editor() && !m_pCanvas->editor()->inherits("KSpreadFormulaEditor") )
        ASSERT( 0 );

    m_pCanvas->createEditor( KSpreadCanvas::FormulaEditor );
}

void KSpreadView::updateReadWrite( bool readwrite )
{
#ifdef __GNUC_
#warning TODO
#endif
    // m_pCancelButton->setEnabled( readwrite );
    // m_pOkButton->setEnabled( readwrite );
  m_pEditWidget->setEnabled( readwrite );

  QValueList<KAction*> actions = actionCollection()->actions();
  QValueList<KAction*>::ConstIterator aIt = actions.begin();
  QValueList<KAction*>::ConstIterator aEnd = actions.end();
  for (; aIt != aEnd; ++aIt )
    (*aIt)->setEnabled( readwrite );

  m_transform->setEnabled( false );
  m_redo->setEnabled( false );
  m_undo->setEnabled( false );
  m_showTable->setEnabled( true );
  m_hideTable->setEnabled( true );
  m_gotoCell->setEnabled( true );
  //  m_newView->setEnabled( true );
  //m_pDoc->KXMLGUIClient::action( "newView" )->setEnabled( true ); // obsolete (Werner)
  // m_oszi->setEnabled( true );
}

void KSpreadView::formulaPower()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::POWER );
}

void KSpreadView::formulaSubscript()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::SUB );
}

void KSpreadView::formulaParentheses()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::PAREN );
}

void KSpreadView::formulaAbsValue()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::ABS );
}

void KSpreadView::formulaBrackets()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::BRACKET );
}

void KSpreadView::formulaFraction()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::DIVIDE );
}

void KSpreadView::formulaRoot()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::SQRT );
}

void KSpreadView::formulaIntegral()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::INTEGRAL );
}

void KSpreadView::formulaMatrix()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::MATRIX );
}

void KSpreadView::formulaLeftSuper()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::LSUP );
}

void KSpreadView::formulaLeftSub()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::LSUB );
}

void KSpreadView::formulaSum()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::SUM );
}

void KSpreadView::formulaProduct()
{
    activateFormulaEditor();

    canvasWidget()->insertFormulaChar(Box::PRODUCT );
}

void KSpreadView::tableFormat()
{
    KSpreadFormatDlg dlg( this );
    dlg.exec();
}

void KSpreadView::autoSum()
{
    // ######## Torben: Make sure that this can not be called
    // when canvas has a running editor
    if ( m_pCanvas->editor() )
        return;

    m_pCanvas->createEditor( KSpreadCanvas::CellEditor );
    m_pCanvas->editor()->setText( "=sum()" );
    m_pCanvas->editor()->setCursorPosition( 5 );

    // Try to find numbers above
    if ( m_pCanvas->markerRow() > 1 )
    {
        KSpreadCell* cell = 0;
        int r = m_pCanvas->markerRow();
        do
        {
            cell = activeTable()->cellAt( m_pCanvas->markerColumn(), --r );
        }
        while ( cell && cell->isValue() );

        if ( r + 1 < m_pCanvas->markerRow() )
        {
            m_pTable->setChooseRect( QRect( m_pCanvas->markerColumn(), r + 1, 1, m_pCanvas->markerRow() - r - 1 ) );
            return;
        }
    }

    // Try to find numbers left
    if ( m_pCanvas->markerColumn() > 1 )
    {
        KSpreadCell* cell = 0;
        int c = m_pCanvas->markerColumn();
        do
        {
            cell = activeTable()->cellAt( --c, m_pCanvas->markerRow() );
        }
        while ( cell && cell->isValue() );

        if ( c + 1 < m_pCanvas->markerColumn() )
        {
            m_pTable->setChooseRect( QRect( c + 1, m_pCanvas->markerRow(), m_pCanvas->markerColumn() - c - 1, 1 ) );
            return;
        }
    }
}

/*
void KSpreadView::oszilloscope()
{
    QDialog* dlg = new KSpreadOsziDlg( this );
    dlg->show();
}
*/

void KSpreadView::changeTextColor()
{
  if ( m_pTable != 0L )
  {
    m_pTable->setSelectionTextColor( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), m_textColor->color() );
  }
}

void KSpreadView::changeBackgroundColor()
{
  if ( m_pTable != 0L )
  {
    m_pTable->setSelectionbgColor( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), m_bgColor->color() );
  }
}

void KSpreadView::changeBorderColor()
{
  if ( m_pTable != 0L )
  {
    m_pTable->setSelectionBorderColor( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), m_borderColor->color() );
  }
}

void KSpreadView::helpUsing()
{
  kapp->invokeHelp( );
}

QButton * KSpreadView::newIconButton( const char *_file, bool _kbutton, QWidget *_parent )
{
  if ( _parent == 0L )
    _parent = this;

  QButton *pb;
  if ( !_kbutton )
    pb = new QPushButton( _parent );
  else
    pb = new QToolButton( _parent );
  pb->setPixmap( QPixmap( KSBarIcon(_file) ) );

  return pb;
}

void KSpreadView::enableUndo( bool _b )
{
    m_undo->setEnabled( _b );
    m_undo->setText(i18n("Undo: %1").arg(m_pDoc->undoBuffer()->getUndoTitle()));
}

void KSpreadView::enableRedo( bool _b )
{
    m_redo->setEnabled( _b );
    m_redo->setText(i18n("Redo: %1") .arg(m_pDoc->undoBuffer()->getRedoTitle()));
}

void KSpreadView::undo()
{
    m_pDoc->undo();

    updateEditWidget();
    
    resultOfCalc();
}

void KSpreadView::redo()
{
    m_pDoc->redo();

    updateEditWidget();
    resultOfCalc();
}

void KSpreadView::deleteColumn()
{
    if ( !m_pTable )
        return;

    QRect r( activeTable()-> selectionRect() );
    if(r.left()==0 || r.right()==0x7FFF)
        m_pTable->removeColumn( m_pCanvas->markerColumn() );
    else
        m_pTable->removeColumn( r.left(),(r.right()-r.left()) );

    updateEditWidget();
}

void KSpreadView::deleteRow()
{
    if ( !m_pTable )
        return;
    QRect r( activeTable()-> selectionRect() );
    if(r.left()==0 || r.bottom()==0x7FFF)
        m_pTable->removeRow( m_pCanvas->markerRow() );
    else
        m_pTable->removeRow( r.top(),(r.bottom()-r.top()) );

    updateEditWidget();
}

void KSpreadView::insertColumn()
{
    if ( !m_pTable )
        return;
    QRect r( activeTable()-> selectionRect() );
    if(r.left()==0|| r.right()==0x7FFF)
        m_pTable->insertColumn( m_pCanvas->markerColumn() );
    else
        m_pTable->insertColumn( r.left(),(r.right()-r.left()) );


    updateEditWidget();
}

void KSpreadView::hideColumn()
{
    if ( !m_pTable )
        return;
    QRect r( activeTable()-> selectionRect() );
    if(r.left()==0|| r.right()==0x7FFF)
        m_pTable->hideColumn( m_pCanvas->markerColumn() );
    else
        m_pTable->hideColumn( r.left(),(r.right()-r.left()) );
}

void KSpreadView::showColumn()
{
    if ( !m_pTable )
        return;
    KSpreadShowColRow *dlg=new KSpreadShowColRow( this,"showCol",KSpreadShowColRow::Column);
    dlg->show();

}

void KSpreadView::insertRow()
{
    if ( !m_pTable )
        return;
    QRect r( activeTable()-> selectionRect() );
    if(r.left()==0 || r.bottom()==0x7FFF)
        m_pTable->insertRow( m_pCanvas->markerRow() );
    else
        m_pTable->insertRow( r.top(),(r.bottom()-r.top()) );

    updateEditWidget();
}

void KSpreadView::hideRow()
{
    if ( !m_pTable )
        return;
    QRect r( activeTable()-> selectionRect() );
    if(r.left()==0 || r.bottom()==0x7FFF)
        m_pTable->hideRow( m_pCanvas->markerRow() );
    else
        m_pTable->hideRow( r.top(),(r.bottom()-r.top()) );
}

void KSpreadView::showRow()
{
    if ( !m_pTable )
        return;
    KSpreadShowColRow *dlg=new KSpreadShowColRow( this,"showRow",KSpreadShowColRow::Row);
    dlg->show();

}

void KSpreadView::fontSelected( const QString &_font )
{
    if ( m_toolbarLock )
        return;

    if ( m_pTable != 0L )
      m_pTable->setSelectionFont( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), _font.latin1() );

    // Dont leave the focus in the toolbars combo box ...
    if ( m_pCanvas->editor() )
        m_pCanvas->editor()->setFocus();
    else
        m_pCanvas->setFocus();
}

void KSpreadView::decreaseFontSize()
{
    if( !m_pTable  )
        return;
    m_pTable->setSelectionSize( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), -1 );
    updateEditWidget();
}

void KSpreadView::increaseFontSize()
{
    if( !m_pTable  )
        return;
    m_pTable->setSelectionSize( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 1 );
    updateEditWidget();
}

void KSpreadView::lower()
{
    if( !m_pTable  )
        return;
    m_pTable->setSelectionUpperLower( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), -1 );
    updateEditWidget();
}

void KSpreadView::upper()
{
    if( !m_pTable  )
        return;
    m_pTable->setSelectionUpperLower( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 1 );
    updateEditWidget();
}

void KSpreadView::firstLetterUpper()
{
    if( !m_pTable  )
        return;
    m_pTable->setSelectionfirstLetterUpper( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    updateEditWidget();
}

void KSpreadView::verticalText(bool b)
{
    if( !m_pTable  )
        return;
    m_pTable->setSelectionVerticalText( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),b );
    QRect r( activeTable()-> selectionRect() );
    if( r.right() != 0x7FFF && r.bottom() != 0x7FFF )
        m_pCanvas->adjustArea(false);
    updateEditWidget();
}

void KSpreadView::insertFormula()
{
    activateFormulaEditor();
}

void KSpreadView::insertMathExpr()
{
    if ( m_pTable == 0L )
        return;
    KSpreadDlgFormula* dlg = new KSpreadDlgFormula( this, "Formula Editor" );
    dlg->show();
    // #### Is the dialog deleted when it's closed ? (David)
    // Torben thinks that not.
}

void KSpreadView::formulaSelection( const QString &_math )
{
    if ( m_pTable == 0 )
        return;

    if( _math == i18n("Others...") )
    {
        insertMathExpr();
        return;
    }

    KSpreadDlgFormula* dlg = new KSpreadDlgFormula( this, "Formula Editor",_math );
    dlg->show();

    /* if ( !m_pCanvas->editor() )
    {
        m_pCanvas->createEditor( KSpreadCanvas::CellEditor );
        m_pCanvas->editor()->setText( "=" );
    }

    ASSERT( m_pCanvas->editor() && m_pCanvas->editor()->inherits("KSpreadTextEditor") );

    QString function;
    if( _math == "sum" )
        function = _math + "(:)";
    else
        function = _math + "()";

    int cursor = m_pCanvas->editor()->cursorPosition();
    int pos = cursor + _math.length() + 1;
    m_pCanvas->editor()->setText( m_pCanvas->editor()->text().insert( cursor, function ) );
    m_pCanvas->editor()->setCursorPosition( pos );
    m_pCanvas->editor()->setFocus(); */
}

void KSpreadView::fontSizeSelected( int _size )
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable != 0L )
        m_pTable->setSelectionFont( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 0L, _size );

    // Dont leave the focus in the toolbars combo box ...
    if ( m_pCanvas->editor() )
        m_pCanvas->editor()->setFocus();
    else
        m_pCanvas->setFocus();
}

void KSpreadView::bold( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable == 0 )
        return;

    m_pTable->setSelectionFont( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 0L, -1, b );
}

void KSpreadView::underline( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable == 0 )
        return;
    m_pTable->setSelectionFont( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 0L, -1, -1, -1 ,b );
}

void KSpreadView::strikeOut( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable == 0 )
        return;
    m_pTable->setSelectionFont( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 0L, -1, -1, -1 ,-1,b );
}


void KSpreadView::italic( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable == 0 )
        return;

    m_pTable->setSelectionFont( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 0L, -1, -1, b );
}

void KSpreadView::sortInc()
{
    QRect r( activeTable()-> selectionRect() );
    if ( r.left() == 0 || r.top() == 0 ||
         r.right() == 0 || r.bottom() == 0 )
    {
        KMessageBox::error( this, i18n("You must select multiple cells.") );
        return;
    }

    // Entire row(s) selected ? Or just one row ?
    if( r.right() == 0x7FFF || r.top() == r.bottom() )
        activeTable()->sortByRow( r.top() );
    else
        activeTable()->sortByColumn( r.left());
    updateEditWidget();
}

void KSpreadView::sortDec()
{
    QRect r( activeTable()-> selectionRect() );
    if ( r.left() == 0 || r.top() == 0 ||
         r.right() == 0 || r.bottom() == 0 )
    {
        KMessageBox::error( this, i18n("You must select multiple cells.") );
        return;
    }

    // Entire row(s) selected ? Or just one row ?
    if( r.right() == 0x7FFF || r.top() == r.bottom() )
        activeTable()->sortByRow( r.top(),KSpreadTable::Decrease);
    else
        activeTable()->sortByColumn( r.left(),KSpreadTable::Decrease);
    updateEditWidget();
}

void KSpreadView::reloadScripts()
{
  // TODO
}

void KSpreadView::runLocalScript()
{
  // TODO
}

void KSpreadView::editGlobalScripts()
{
  if ( KSpreadView::m_pGlobalScriptsDialog == 0L )
    KSpreadView::m_pGlobalScriptsDialog = new KSpreadScripts();
  KSpreadView::m_pGlobalScriptsDialog->show();
  KSpreadView::m_pGlobalScriptsDialog->raise();
}

void KSpreadView::editLocalScripts()
{
  // TODO
  /* if ( !m_pDoc->editPythonCode() )
  {
    KMessageBox::error( i18n( "Could not start editor" ) );
    return;
    } */
}

void KSpreadView::borderBottom()
{
  if ( m_pTable != 0L )
      m_pTable->borderBottom( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) ,m_borderColor->color());
}

void KSpreadView::borderRight()
{
  if ( m_pTable != 0L )
      m_pTable->borderRight( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) ,m_borderColor->color());
}

void KSpreadView::borderLeft()
{
  if ( m_pTable != 0L )
      m_pTable->borderLeft( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),m_borderColor->color() );
}

void KSpreadView::borderTop()
{
  if ( m_pTable != 0L )
      m_pTable->borderTop( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),m_borderColor->color() );
}

void KSpreadView::borderOutline()
{
  if ( m_pTable != 0L )
      m_pTable->borderOutline( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) ,m_borderColor->color());
}

void KSpreadView::borderAll()
{
  if ( m_pTable != 0L )
      m_pTable->borderAll( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),m_borderColor->color() );
}

void KSpreadView::borderRemove()
{
  if ( m_pTable != 0L )
      m_pTable->borderRemove( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::addTable( KSpreadTable *_t )
{
    if( !_t->isHidden() )
    {
        m_pTabBar->addTab( _t->tableName() );
        setActiveTable( _t );
    }
    else
    {
        m_pTabBar->addHiddenTab(_t->tableName());
    }

    // Connect some signals
    QObject::connect( _t, SIGNAL( sig_updateView( KSpreadTable* ) ), SLOT( slotUpdateView( KSpreadTable* ) ) );
    QObject::connect( _t, SIGNAL( sig_updateView( KSpreadTable *, const QRect& ) ),
                      SLOT( slotUpdateView( KSpreadTable*, const QRect& ) ) );
    QObject::connect( _t, SIGNAL( sig_unselect( KSpreadTable *, const QRect& ) ),
                      SLOT( slotUnselect( KSpreadTable *, const QRect& ) ) );
    QObject::connect( _t, SIGNAL( sig_updateHBorder( KSpreadTable * ) ),
                      SLOT( slotUpdateHBorder( KSpreadTable * ) ) );
    QObject::connect( _t, SIGNAL( sig_updateVBorder( KSpreadTable * ) ),
                      SLOT( slotUpdateVBorder( KSpreadTable * ) ) );
    QObject::connect( _t, SIGNAL( sig_changeSelection( KSpreadTable *, const QRect &, const QRect & ) ),
                      SLOT( slotChangeSelection( KSpreadTable *, const QRect &, const QRect & ) ) );
    QObject::connect( _t, SIGNAL( sig_changeChooseSelection( KSpreadTable *, const QRect &, const QRect & ) ),
                      SLOT( slotChangeChooseSelection( KSpreadTable *, const QRect &, const QRect & ) ) );
    QObject::connect( _t, SIGNAL( sig_nameChanged( KSpreadTable*, const QString& ) ),
                      this, SLOT( slotTableRenamed( KSpreadTable*, const QString& ) ) );
    QObject::connect( _t, SIGNAL( sig_TableHidden( KSpreadTable* ) ),
                      this, SLOT( slotTableHidden( KSpreadTable* ) ) );
    QObject::connect( _t, SIGNAL( sig_TableShown( KSpreadTable* ) ),
                      this, SLOT( slotTableShown( KSpreadTable* ) ) );
    QObject::connect( _t, SIGNAL( sig_TableRemoved( KSpreadTable* ) ),
                      this, SLOT( slotTableRemoved( KSpreadTable* ) ) );
    QObject::connect( _t, SIGNAL( sig_TableActivated( KSpreadTable* ) ),
                      this, SLOT( slotTableActivated( KSpreadTable* ) ) );
    // ########### Why do these signals not send a pointer to the table?
    // This will lead to bugs.
    QObject::connect( _t, SIGNAL( sig_updateChildGeometry( KSpreadChild* ) ),
                      SLOT( slotUpdateChildGeometry( KSpreadChild* ) ) );
    QObject::connect( _t, SIGNAL( sig_removeChild( KSpreadChild* ) ), SLOT( slotRemoveChild( KSpreadChild* ) ) );
    QObject::connect( _t, SIGNAL( sig_maxColumn( int ) ), m_pCanvas, SLOT( slotMaxColumn( int ) ) );
    QObject::connect( _t, SIGNAL( sig_maxRow( int ) ), m_pCanvas, SLOT( slotMaxRow( int ) ) );
    QObject::connect( _t, SIGNAL( sig_polygonInvalidated( const QPointArray& ) ),
                      this, SLOT( repaintPolygon( const QPointArray& ) ) );
    if(m_bLoading)
        m_showPageBorders->setChecked( m_pTable->isShowPageBorders());
}

void KSpreadView::slotTableRemoved( KSpreadTable *_t )
{
  QString m_tableName=_t->tableName();
  m_pTabBar->removeTab( _t->tableName() );
  if(m_pDoc->map()->findTable( m_pTabBar->listshow().first()))
    setActiveTable( m_pDoc->map()->findTable( m_pTabBar->listshow().first() ));
  else
    m_pTable = 0L;

  QValueList<Reference>::Iterator it;
  QValueList<Reference> area=doc()->listArea();
  for ( it = area.begin(); it != area.end(); ++it )
  {
        //remove Area Name when table target is removed
        if((*it).table_name==m_tableName)
	  {
	    doc()->removeArea((*it).ref_name);
	    //now area name is used in formula
	    //so you must recalc tables when remove areaname
	    KSpreadTable *tbl;
	    
	    for ( tbl = doc()->map()->firstTable(); tbl != 0L; tbl = doc()->map()->nextTable() )
	      {
		tbl->refreshRemoveAreaName((*it).ref_name);
	      }
	  }
  }
  m_pHorzScrollBar->setValue(activeTable()->getScrollPosX());
  m_pVertScrollBar->setValue(activeTable()->getScrollPosY());

}

void KSpreadView::removeAllTables()
{
  m_pTabBar->removeAllTabs();

  setActiveTable( 0L );
}

void KSpreadView::setActiveTable( KSpreadTable *_t,bool updateTable )
{
  if ( _t == m_pTable )
    return;

  m_pTable = _t;
  if ( m_pTable == 0L )
    return;
  if(updateTable)
  {
  m_pTabBar->setActiveTab( _t->tableName() );
  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
  m_pCanvas->repaint();
  m_pCanvas->slotMaxColumn( m_pTable->maxColumn() );
  m_pCanvas->slotMaxRow( m_pTable->maxRow() );
  }
}

void KSpreadView::slotRefreshView(  )
{
  refreshView();
}

void KSpreadView::slotTableActivated( KSpreadTable* table )
{
  m_pTabBar->setActiveTab( table->tableName() );
}

void KSpreadView::slotTableRenamed( KSpreadTable* table, const QString& old_name )
{
    m_pTabBar->renameTab( old_name, table->tableName() );
}

void KSpreadView::slotTableHidden( KSpreadTable* table )
{
    m_pTabBar->hideTable( table->tableName() );
}
void KSpreadView::slotTableShown( KSpreadTable* table )
{
    m_pTabBar->displayTable( table->tableName() );
}

void KSpreadView::changeTable( const QString& _name )
{
    if ( activeTable()->tableName() == _name )
        return;

    KSpreadTable *t = m_pDoc->map()->findTable( _name );
    if ( !t )
    {
        kdDebug(36001) << "Unknown table " << _name << endl;
        return;
    }
    m_pCanvas->closeEditor();
    activeTable()->setScrollPosX(m_pHorzScrollBar->value());
    activeTable()->setScrollPosY(m_pVertScrollBar->value());
    setActiveTable( t,false );

    t->setActiveTable();

    updateEditWidget();
    m_pHorzScrollBar->setValue(t->getScrollPosX());
    m_pVertScrollBar->setValue(t->getScrollPosY());
    //refresh toggle button
    m_showPageBorders->setChecked( m_pTable->isShowPageBorders() );
}

void KSpreadView::slotScrollToFirstTable()
{
  m_pTabBar->scrollFirst();
}

void KSpreadView::slotScrollToLeftTable()
{
  m_pTabBar->scrollLeft();
}

void KSpreadView::slotScrollToRightTable()
{

  m_pTabBar->scrollRight();
}

void KSpreadView::slotScrollToLastTable()
{
  m_pTabBar->scrollLast();
}

void KSpreadView::insertTable()
{
  m_pCanvas->closeEditor();
  activeTable()->setScrollPosX(m_pHorzScrollBar->value());
  activeTable()->setScrollPosY(m_pVertScrollBar->value());
  KSpreadTable *t = m_pDoc->createTable();
  m_pDoc->addTable( t );
  m_pHorzScrollBar->setValue(t->getScrollPosX());
  m_pVertScrollBar->setValue(t->getScrollPosY());
  updateEditWidget();
}

void KSpreadView::hideTable()
{
    if ( !m_pTable )
       return;
    m_pDoc->setModified( true );
    m_pTabBar->hideTable();
}

void KSpreadView::showTable()
{
    if ( !m_pTable )
        return;
    KSpreadshow* dlg = new KSpreadshow( this, "Table show");
    dlg->show();
}

void KSpreadView::copySelection()
{
    if ( !m_pTable )
        return;

    m_pTable->copySelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );

    updateEditWidget();
}

void KSpreadView::cutSelection()
{
    if ( !m_pTable )
        return;

    m_pTable->cutSelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    resultOfCalc();
    updateEditWidget();
}

void KSpreadView::paste()
{
    if ( !m_pTable )
        return;

    QRect r( activeTable()-> selectionRect() );
    if(r.left()==0 )
        r.setCoords(m_pCanvas->markerColumn(), m_pCanvas->markerRow() ,
                m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
    m_pTable->paste( QPoint( r.left(),r.top() ) );
    if(m_pTable->getAutoCalc())
        m_pTable->recalc(true);
    resultOfCalc();
    updateEditWidget();
}

void KSpreadView::specialPaste()
{
    if ( !m_pTable )
        return;

    KSpreadspecial dlg( this, "Special Paste" );
    if( dlg.exec() )
    {
    if(m_pTable->getAutoCalc())
        m_pTable->recalc(true);
    resultOfCalc();
    updateEditWidget();
    }
}

void KSpreadView::removeComment()
{
  if ( !m_pTable )
        return;
  m_pTable->setSelectionRemoveComment( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
  updateEditWidget();
}


void KSpreadView::changeAngle()
{
  if ( !m_pTable )
        return;
  KSpreadAngle* dlg = new KSpreadAngle( this, "Angle" ,QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
  if(dlg->exec())
        {
        QRect r( activeTable()-> selectionRect() );
        if( r.right() != 0x7FFF && r.bottom() != 0x7FFF )
                m_pCanvas->adjustArea(false);
        }
}

void KSpreadView::mergeCell()
{
    if ( !m_pTable )
        return;
    QRect rect( activeTable()-> selectionRect() );
    if((rect.right()==0x7FFF) ||(rect.bottom()==0x7FFF))
        {
        KMessageBox::error( this, i18n("Area too large!"));
        }
  else
        {
        m_pTable->mergeCell( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
        }
}

void KSpreadView::dissociateCell()
{
    if ( !m_pTable )
        return;
    m_pTable->dissociateCell( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}


void KSpreadView::increaseIndent()
{
  if ( !m_pTable )
        return;
  m_pTable->increaseIndent( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
  updateEditWidget();
}

void KSpreadView::decreaseIndent()
{
  if ( !m_pTable )
        return;
  m_pTable->decreaseIndent( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::consolidate()
{
    if ( m_pCanvas->editor() )
    {
        m_pCanvas->deleteEditor( true ); // save changes
    }
    KSpreadConsolidate* dlg = new KSpreadConsolidate( this, "Consolidate" );
    dlg->show();
}

void KSpreadView::sortList()
{
  KSpreadList dlg(this,"List selection");
  dlg.exec();
}

void KSpreadView::gotoCell()
{
  
  KSpreadGotoDlg dlg( this, "GotoCell" );
  dlg.exec();
}

void KSpreadView::find()
{
    KoFindDialog dlg( this, "Find", m_findOptions, &m_findStrings );
    if ( KoFindDialog::Accepted != dlg.exec() )
    {
        return;
    }
    m_findOptions = dlg.options();
    m_findStrings = dlg.findHistory();

    // Do the finding!
    QPoint m_marker=QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
    activeTable()->find( m_marker, dlg.pattern(), dlg.options() );
}

void KSpreadView::replace()
{
    KoReplaceDialog dlg( this, "Replace", m_findOptions, &m_findStrings, &m_replaceStrings );
    if ( KoReplaceDialog::Accepted != dlg.exec() )
    {
        return;
    }
    m_findOptions = dlg.options();
    m_findStrings = dlg.findHistory();
    m_replaceStrings = dlg.replacementHistory();

    // Do the replacement.
    QPoint m_marker=QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
    activeTable()->replace( m_marker, dlg.pattern(), dlg.replacement(), dlg.options() );

    // Refresh the editWidget
    KSpreadCell *cell = activeTable()->cellAt( canvasWidget()->markerColumn(),
                                                        canvasWidget()->markerRow() );
    if ( cell->text() != 0L )
        editWidget()->setText( cell->text() );
    else
        editWidget()->setText( "" );
}

void KSpreadView::conditional()
{
//  KSpreadconditional *dlg=new KSpreadconditional( this,"conditional",QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
  QRect rect( activeTable()-> selectionRect() );

  if((rect.right()==0x7FFF) ||(rect.bottom()==0x7FFF))
        {
        KMessageBox::error( this, i18n("Area too large!"));
        }
  else
        {
        if ( rect.left() == 0 || rect.top() == 0 ||
         rect.right() == 0 || rect.bottom() == 0 )
           {
                rect.setCoords(  m_pCanvas->markerColumn(), m_pCanvas->markerRow(),m_pCanvas->markerColumn(), m_pCanvas->markerRow());
           }
         KSpreadconditional *dlg=new KSpreadconditional(this,"conditional",rect);
         dlg->show();
         }

}

void KSpreadView::validity()
{
  QRect rect( activeTable()-> selectionRect() );

  if((rect.right()==0x7FFF) ||(rect.bottom()==0x7FFF))
        {
        KMessageBox::error( this, i18n("Area too large!"));
        }
  else
        {
        if ( rect.left() == 0 || rect.top() == 0 ||
         rect.right() == 0 || rect.bottom() == 0 )
           {
                rect.setCoords(  m_pCanvas->markerColumn(), m_pCanvas->markerRow(),m_pCanvas->markerColumn(), m_pCanvas->markerRow());
           }
         KSpreadDlgValidity *dlg=new KSpreadDlgValidity( this,"validity",rect);
         dlg->show();
         }
}


void KSpreadView::insertSeries()
{
    KSpreadSeriesDlg dlg( this, "Series", QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    dlg.exec();
}

void KSpreadView::sort()
{
    QRect selection( m_pTable->selectionRect() );
    if(selection.left()==0)
    {
        KMessageBox::error( this, i18n("You must select multiple cells") );
        return;
    }

    KSpreadSortDlg dlg( this, "Sort" );
    dlg.exec();
}

void KSpreadView::insertHyperlink()
{
  KSpreadLinkDlg dlg( this, "Create Hyperlink" );
  dlg.exec();
}

void KSpreadView::print( QPrinter &prt )
{
    prt.setFullPage( TRUE );
    QPainter painter;

    //apply page layout parameters
    prt.setOrientation((QPrinter::Orientation)m_pDoc->orientation());
    switch(m_pDoc->paperFormat())
      {
      case  PG_DIN_A3:
	prt.setPageSize(QPrinter::A3);
	break;
      case PG_DIN_A4:
	prt.setPageSize(QPrinter::A4);
	break;
      case PG_DIN_A5:
	prt.setPageSize(QPrinter::A5);
	break;	
      case PG_US_LETTER:
	prt.setPageSize(QPrinter::Letter);
	break;
      case PG_US_LEGAL:
	prt.setPageSize(QPrinter::Legal);
	break;
      case PG_SCREEN :
	//prt.setPageSize(QPrinter::PG_SCREEN);
	kdDebug(36001) <<"We can't define this size of page, so I used A4 format\n";
	prt.setPageSize(QPrinter::A4);
	break;
      case PG_CUSTOM:
	kdDebug(36001) <<"We can't define custom size of page, so I used A4 format \n";
	//prt.setPageSize(QSize(m_pDoc->paperWidth(),m_pDoc->paperHeight()));
	prt.setPageSize(QPrinter::A4);
	break;
      case PG_DIN_B5:
	prt.setPageSize(QPrinter::B5);
	break;
      case PG_US_EXECUTIVE:
	prt.setPageSize(QPrinter::Executive);
	break;
      default:
	kdDebug(36001) <<"Error in m_pDoc->paperFormat() \n";
	break;
      }
    painter.begin( &prt );
    // Print the table and tell that m_pDoc is NOT embedded.
    m_pTable->print( painter, &prt );
    painter.end();
}

void KSpreadView::insertChart( const QRect& _geometry, KoDocumentEntry& _e )
{
    if ( !m_pTable )
        return;

    // Transform the view coordinates to document coordinates
    QWMatrix m = matrix().invert();
    QPoint tl = m.map( _geometry.topLeft() );
    QPoint br = m.map( _geometry.bottomRight() );
    QRect r( activeTable()-> selectionRect() );
    if( r.right() ==0x7FFF ||r.bottom()==0x7FFF)
    {
        KMessageBox::error( this, i18n("Area too large!"));
        m_pTable->insertChart( QRect( tl, br ), _e, QRect( m_pCanvas->markerColumn(), m_pCanvas->markerRow(),1,1) );
    }
    else
    {
    // Insert the new child in the active table.
    m_pTable->insertChart( QRect( tl, br ), _e, m_pTable->selectionRect() );
    }
}

void KSpreadView::insertChild( const QRect& _geometry, KoDocumentEntry& _e )
{
    if ( !m_pTable )
        return;

    // Transform the view coordinates to document coordinates
    QWMatrix m = matrix().invert();
    QPoint tl = m.map( _geometry.topLeft() );
    QPoint br = m.map( _geometry.bottomRight() );

    // Insert the new child in the active table.
    m_pTable->insertChild( QRect( tl, br ), _e );
}

void KSpreadView::slotRemoveChild( KSpreadChild *_child )
{
    if ( _child->table() != m_pTable )
	return;

    // Make shure that this child has no active embedded view -> activate ourselfs
    partManager()->setActivePart( koDocument(), this );
}

void KSpreadView::slotUpdateChildGeometry( KSpreadChild */*_child*/ )
{
    // ##############
    // TODO
    /*
  if ( _child->table() != m_pTable )
    return;

  // Find frame for child
  KSpreadChildFrame *f = 0L;
  QListIterator<KSpreadChildFrame> it( m_lstFrames );
  for ( ; it.current() && !f; ++it )
    if ( it.current()->child() == _child )
      f = it.current();

  assert( f != 0L );

  // Are we already up to date ?
  if ( _child->geometry() == f->partGeometry() )
    return;

  // TODO zooming
  f->setPartGeometry( _child->geometry() );
    */
}

void KSpreadView::togglePageBorders( bool mode )
{
   if ( !m_pTable )
       return;

   m_pTable->setShowPageBorders( mode );
}


void KSpreadView::preference()
{
  if ( !m_pTable )
       return;
  KSpreadpreference* dlg = new KSpreadpreference( this, "Preference");
  if(dlg->exec())
        m_pTable->refreshPreference();
}

void KSpreadView::addModifyComment()
{
  if ( !m_pTable )
       return;

  KSpreadComment* dlg = new  KSpreadComment( this, "comment",QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
  if(dlg->exec())
        updateEditWidget();
}

void KSpreadView::editCell()
{
    if ( m_pCanvas->editor() )
        return;

    m_pCanvas->createEditor();
}

void KSpreadView::nextTable(){

    KSpreadTable *t = m_pDoc->map()->nextTable( activeTable() );
    if ( !t )
    {
        kdDebug(36001) << "Unknown table " <<  endl;
        return;
    }
    m_pCanvas->closeEditor();
    activeTable()->setScrollPosX(m_pHorzScrollBar->value());
    activeTable()->setScrollPosY(m_pVertScrollBar->value());
    setActiveTable( t,false );

    t->setActiveTable();

}

void KSpreadView::previousTable(){

    KSpreadTable *t = m_pDoc->map()->previousTable( activeTable() );
    if ( !t )
    {
        kdDebug(36001) << "Unknown table "  << endl;
        return;
    }
    m_pCanvas->closeEditor();
    activeTable()->setScrollPosX(m_pHorzScrollBar->value());
    activeTable()->setScrollPosY(m_pVertScrollBar->value());
    setActiveTable( t,false );

    t->setActiveTable();


}
void KSpreadView::keyPressEvent ( QKeyEvent* _ev )
{


  // Dont eat accelerators
  if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) ){

    if ( _ev->state() & ( Qt::ControlButton ) ){

      // Universally reserved spreadsheet bavigators known to all professional
      // spreadsheet users around the world -- Bernd

      switch( _ev->key() ){
      case Key_PageDown:

	nextTable();
	return;

      case Key_PageUp:
	previousTable();
	return;

      default:
	  QWidget::keyPressEvent( _ev );
	  return;
      }
    }
    QWidget::keyPressEvent( _ev );
  }
  else
    QApplication::sendEvent( m_pCanvas, _ev );

}

KoDocument* KSpreadView::hitTest( const QPoint &pos )
{
    // Code copied from KoView::hitTest
    KoViewChild *viewChild;

    QWMatrix m = matrix();
    m.translate( m_pCanvas->xOffset(), m_pCanvas->yOffset() );

    KoDocumentChild *docChild = selectedChild();
    if ( docChild )
    {
        if ( ( viewChild = child( docChild->document() ) ) )
        {
            if ( viewChild->frameRegion( m ).contains( pos ) )
                return 0;
        }
        else
            if ( docChild->frameRegion( m ).contains( pos ) )
                return 0;
    }

    docChild = activeChild();
    if ( docChild )
    {
        if ( ( viewChild = child( docChild->document() ) ) )
        {
            if ( viewChild->frameRegion( m ).contains( pos ) )
                return 0;
        }
        else
            if ( docChild->frameRegion( m ).contains( pos ) )
                return 0;
    }

    QPoint pos2( pos.x() / zoom(), pos.y() / zoom() );

    QListIterator<KoDocumentChild> it( m_pDoc->children() );
    for (; it.current(); ++it )
    {
        // Is the child document on the visible table ?
        if ( ((KSpreadChild*)it.current())->table() == m_pTable )
        {
            KoDocument *doc = it.current()->hitTest( pos2, m );
            if ( doc )
                return doc;
        }
    }

    return m_pDoc;
}

int KSpreadView::leftBorder() const
{
    return YBORDER_WIDTH;
}

int KSpreadView::rightBorder() const
{
    return m_pVertScrollBar->width();
}

int KSpreadView::topBorder() const
{
    return m_pToolWidget->height() + XBORDER_HEIGHT;
}

int KSpreadView::bottomBorder() const
{
    return m_pHorzScrollBar->height();
}

void KSpreadView::refreshView()
{
    bool active=activeTable()->getShowFormular();
    m_alignLeft->setEnabled(!active);
    m_alignCenter->setEnabled(!active);
    m_alignRight->setEnabled(!active); 
    active=m_pDoc->getShowFormularBar();
    editWidget()->showEditWidget(active);
    int posFrame=30;
    if(active)
      posWidget()->show();
    else
      {
      posWidget()->hide();
      posFrame=0;
      }
    
    m_pToolWidget->show();
    // If this value (30) is changed then topBorder() needs to
    // be changed, too.
    m_pToolWidget->setGeometry( 0, 0, width(), /*30*/posFrame );
    int top = /*30*/posFrame;

    

    if(m_pDoc->getShowTabBar())
      {
	m_pTabBarFirst->setGeometry( 0, height() - 16, 16, 16 );
	m_pTabBarLeft->setGeometry( 16, height() - 16, 16, 16 );
	m_pTabBarRight->setGeometry( 32, height() - 16, 16, 16 );
	m_pTabBarLast->setGeometry( 48, height() - 16, 16, 16 );
	m_pTabBarFirst->show();
	m_pTabBarLeft->show();
	m_pTabBarRight->show();
	m_pTabBarLast->show();
      }
    else
      {
	m_pTabBarFirst->hide();
	m_pTabBarLeft->hide();
	m_pTabBarRight->hide();
	m_pTabBarLast->hide();
      }



    if(!m_pDoc->getShowHorizontalScrollBar())
        m_pTabBar->setGeometry( 64, height() - 16, width() -64, 16 );
    else
        m_pTabBar->setGeometry( 64, height() - 16, width() / 2 - 64, 16 );
     if(m_pDoc->getShowTabBar())
       m_pTabBar->show();
     else
       m_pTabBar->hide();

    // David's suggestion: move the scrollbars to KSpreadCanvas, but keep those resize statements
    int widthScrollbarVertical=16;
    if(m_pDoc->getShowHorizontalScrollBar())
        m_pHorzScrollBar->show();
    else
        m_pHorzScrollBar->hide();
    if(!m_pDoc->getShowTabBar() && !m_pDoc->getShowHorizontalScrollBar())
      m_pVertScrollBar->setGeometry( width() - 16, top , 16, height() - top );
    else
      m_pVertScrollBar->setGeometry( width() - 16, top , 16, height() - 16 - top );
    m_pVertScrollBar->setSteps( 20 /*linestep*/, m_pVertScrollBar->height() /*pagestep*/);
    if(m_pDoc->getShowVerticalScrollBar())
        m_pVertScrollBar->show();
    else
        {
        widthScrollbarVertical=0;
        m_pVertScrollBar->hide();
        }

    int widthRowHeader=YBORDER_WIDTH;
    if(m_pDoc->getShowRowHeader())
        m_pVBorderWidget->show();
    else
        {
        widthRowHeader=0;
        m_pVBorderWidget->hide();
        }

    int heightColHeader=XBORDER_HEIGHT;
    if(m_pDoc->getShowColHeader())
         m_pHBorderWidget->show();
    else
        {
        heightColHeader=0;
        m_pHBorderWidget->hide();
        }


    m_pHorzScrollBar->setGeometry( width() / 2, height() - 16, width() / 2 - widthScrollbarVertical, 16 );
    m_pHorzScrollBar->setSteps( 20 /*linestep*/, m_pHorzScrollBar->width() /*pagestep*/);

    if(!m_pDoc->getShowTabBar() && !m_pDoc->getShowHorizontalScrollBar())
      m_pFrame->setGeometry( 0, top, width()-widthScrollbarVertical, height()  - top );
    else
      m_pFrame->setGeometry( 0, top, width()-widthScrollbarVertical, height() -16 - top );
    m_pFrame->show();

    m_pCanvas->setGeometry( widthRowHeader, heightColHeader,
                            m_pFrame->width() -widthRowHeader, m_pFrame->height() - heightColHeader );

    m_pHBorderWidget->setGeometry( widthRowHeader, 0, m_pFrame->width() - widthRowHeader, heightColHeader );

    m_pVBorderWidget->setGeometry( 0,heightColHeader, widthRowHeader,
                                   m_pFrame->height() - heightColHeader );
}

void KSpreadView::resizeEvent( QResizeEvent * )
{
 refreshView();
}

void KSpreadView::popupChildMenu( KoChild* child, const QPoint& global_pos )
{
    if ( !child )
	return;

    if ( m_popupChild != 0 )
	delete m_popupChild;

    m_popupChildObject = static_cast<KSpreadChild*>(child);

    m_popupChild = new QPopupMenu( this );

    m_popupChild->insertItem( i18n("Delete embedded document"), this, SLOT( slotPopupDeleteChild() ) );

    m_popupChild->popup( global_pos );
}

void KSpreadView::slotPopupDeleteChild()
{
    if ( !m_popupChildObject || !m_popupChildObject->table() )
	return;
    int ret = KMessageBox::warningYesNo(this,i18n("You are going to remove this embedded document.\nDo you want to continue?"),i18n("Delete embedded document"));
    if ( ret == 3 )
        {
        m_popupChildObject->table()->deleteChild( m_popupChildObject );
        m_popupChildObject = 0;
        }
}

void KSpreadView::popupColumnMenu(const QPoint & _point)
{
    assert( m_pTable );

    if ( !koDocument()->isReadWrite() )
      return;

    if (m_pPopupColumn != 0L )
        delete m_pPopupColumn ;

    m_pPopupColumn = new QPopupMenu( this );

    m_cellLayout->plug( m_pPopupColumn );
    m_cut->plug( m_pPopupColumn );
    m_copy->plug( m_pPopupColumn );
    m_paste->plug( m_pPopupColumn );
    m_specialPaste->plug( m_pPopupColumn );
    m_insertCellCopy->plug( m_pPopupColumn );
    m_pPopupColumn->insertSeparator();
    m_default->plug( m_pPopupColumn );
    // If there is no selection
    QRect selection( m_pTable->selectionRect() );
    if(selection.right()!=0x7FFF && selection.bottom()!=0x7FFF )
    {
      m_areaName->plug( m_pPopupColumn );
    }
    m_resizeColumn->plug( m_pPopupColumn );
    m_pPopupColumn->insertItem( i18n("Adjust Column"), this, SLOT(slotPopupAdjustColumn() ) );
    m_pPopupColumn->insertSeparator();
    m_insertColumn->plug( m_pPopupColumn );
    m_deleteColumn->plug( m_pPopupColumn );
    m_hideColumn->plug( m_pPopupColumn );

    QObject::connect( m_pPopupColumn, SIGNAL(activated( int ) ), this, SLOT(slotActivateTool( int ) ) );

    m_pPopupColumn->popup( _point );
}

void KSpreadView::slotPopupAdjustColumn()
{
    if ( !m_pTable )
       return;
    canvasWidget()->adjustArea();
}

void KSpreadView::popupRowMenu(const QPoint & _point )
{
    assert( m_pTable );

    if ( !koDocument()->isReadWrite() )
      return;

    if (m_pPopupRow != 0L )
        delete m_pPopupRow ;

    m_pPopupRow= new QPopupMenu();

    m_cellLayout->plug( m_pPopupRow );
    m_cut->plug( m_pPopupRow );
    m_copy->plug( m_pPopupRow );
    m_paste->plug( m_pPopupRow );
    m_specialPaste->plug( m_pPopupRow );
    m_insertCellCopy->plug( m_pPopupRow );
    m_pPopupRow->insertSeparator();
    m_default->plug( m_pPopupRow );
    // If there is no selection
    QRect selection( m_pTable->selectionRect() );
    if(selection.right()!=0x7FFF && selection.bottom()!=0x7FFF )
      {
	m_areaName->plug( m_pPopupRow );
      }

    m_resizeRow->plug( m_pPopupRow );
    m_pPopupRow->insertItem( i18n("Adjust Row"), this, SLOT( slotPopupAdjustRow() ) );
    m_pPopupRow->insertSeparator();
    m_insertRow->plug( m_pPopupRow );
    m_deleteRow->plug( m_pPopupRow );
    m_hideRow->plug( m_pPopupRow );

    QObject::connect( m_pPopupRow, SIGNAL( activated( int ) ), this, SLOT( slotActivateTool( int ) ) );
    m_pPopupRow->popup( _point );
}

void KSpreadView::slotPopupAdjustRow()
{
    if ( !m_pTable )
       return;
    canvasWidget()->adjustArea();
}


void KSpreadView::slotListChoosePopupMenu( )
{
 assert( m_pTable );
 if (  m_popupListChoose != 0L )
   delete m_popupListChoose;

 if(!koDocument()->isReadWrite() )
   return;

 m_popupListChoose = new QPopupMenu();
 int id = 0;
 QRect selection( m_pTable->selectionRect() );
 if(selection.left()==0)
   selection.setCoords(m_pCanvas->markerColumn(),m_pCanvas->markerRow(),
		       m_pCanvas->markerColumn(),m_pCanvas->markerRow());
 KSpreadCell *cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
 QString tmp=cell->text();
 QStringList itemList;
 KSpreadCell* c = m_pTable->firstCell();
 for( ;c; c = c->nextCell() )
   {
     int col = c->column();
     if ( selection.left() <= col && selection.right() >= col
	  &&!c->isObscuringForced()&& !(col==m_pCanvas->markerColumn()&& c->row()==m_pCanvas->markerRow()))
       {
	 if(!c->isFormular() && !c->isValue() && !c->valueString().isEmpty()
         && !c->isTime() &&!c->isDate()
	    && c->content() != KSpreadCell::VisualFormula
	    && c->text()!=tmp)
	   {
	     if(itemList.findIndex(c->text())==-1)
	       itemList.append(c->text());
	   }
	   
       }
   }
 for ( QStringList::Iterator it = itemList.begin(); it != itemList.end();++it ) 
   m_popupListChoose->insertItem( (*it), id++ );

 if(id==0)
   return;
 RowLayout *rl = m_pTable->rowLayout( m_pCanvas->markerRow());
 int tx = m_pTable->columnPos( m_pCanvas->markerColumn(), m_pCanvas );
 int ty = m_pTable->rowPos(m_pCanvas->markerRow(), m_pCanvas );
 int h = rl->height( m_pCanvas );
 if ( cell->extraYCells())
   h = cell->extraHeight();
 ty += h;

 QPoint p( tx, ty );
 QPoint p2 = m_pCanvas->mapToGlobal( p );
 m_popupListChoose->popup( p2 );
 QObject::connect( m_popupListChoose, SIGNAL( activated( int ) ),
		  this, SLOT( slotItemSelected( int ) ) );
}


void KSpreadView::slotItemSelected( int id) 
{
  QString tmp=m_popupListChoose->text(id);
  KSpreadCell *cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );  

  if(tmp==cell->text())
    return;

  if ( cell->isDefault() )
    {
      cell = new KSpreadCell(m_pTable , m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
      m_pTable->insertCell(cell);
    }

 
  if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      KSpreadUndoSetText* undo = new KSpreadUndoSetText( m_pDoc, m_pTable, cell->text(), m_pCanvas->markerColumn(), m_pCanvas->markerRow() ,cell->getFormatNumber( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
      m_pDoc->undoBuffer()->appendUndo( undo );
    }
  
  cell->setCellText( tmp, true );
  editWidget()->setText( tmp );
}

void KSpreadView::openPopupMenu( const QPoint & _point )
{
    assert( m_pTable );


    if ( m_pPopupMenu != 0L )
        delete m_pPopupMenu;

    if(!koDocument()->isReadWrite() )
        return;

    m_pPopupMenu = new QPopupMenu();

    m_cellLayout->plug( m_pPopupMenu );
    m_cut->plug( m_pPopupMenu );
    m_copy->plug( m_pPopupMenu );
    m_paste->plug( m_pPopupMenu );
    m_specialPaste->plug( m_pPopupMenu );
    m_insertCellCopy->plug( m_pPopupMenu );
    m_pPopupMenu->insertSeparator();
    m_delete->plug( m_pPopupMenu );
    m_adjust->plug( m_pPopupMenu );
    m_default->plug( m_pPopupMenu );

    // If there is no selection
    QRect selection( m_pTable->selectionRect() );
    if(selection.right()!=0x7FFF && selection.bottom()!=0x7FFF )
    {
      m_areaName->plug( m_pPopupMenu );
      m_pPopupMenu->insertSeparator();
      m_insertCell->plug( m_pPopupMenu );
      m_removeCell->plug( m_pPopupMenu );
    }
    


    KSpreadCell *cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
    m_pPopupMenu->insertSeparator();
    m_addModifyComment->plug( m_pPopupMenu );
    if( !cell->comment(m_pCanvas->markerColumn(), m_pCanvas->markerRow()).isEmpty() )
    {
        m_removeComment->plug( m_pPopupMenu );
    }

    
    if(activeTable()->testListChoose(QPoint(m_pCanvas->markerColumn(), m_pCanvas->markerRow())))
      {
	m_pPopupMenu->insertSeparator();
	m_pPopupMenu->insertItem( i18n("Selection list..."), this, SLOT( slotListChoosePopupMenu() ) );
      }

    // Remove informations about the last tools we offered
    m_lstTools.clear();
    m_lstTools.setAutoDelete( true );

    if ( !cell->isFormular() && !cell->isValue() && !cell->valueString().isEmpty()
         && !cell->isTime() &&!cell->isDate()
         && cell->content() != KSpreadCell::VisualFormula)
    {
      m_popupMenuFirstToolId = 10;
      int i = 0;
      QValueList<KoDataToolInfo> tools = KoDataToolInfo::query( "QString", "text/plain" );
      if( tools.count() > 0 )
      {
        m_pPopupMenu->insertSeparator();
        QValueList<KoDataToolInfo>::Iterator entry = tools.begin();
        for( ; entry != tools.end(); ++entry )
        {
          QStringList lst = (*entry).userCommands();
          QStringList::ConstIterator it = lst.begin();

          // ### Torben: Insert pixmaps here, too
          for (; it != lst.end(); ++it )
            m_pPopupMenu->insertItem( *it, m_popupMenuFirstToolId + i++ );

          lst = (*entry).commands();
          it = lst.begin();
          for (; it != lst.end(); ++it )
          {
            ToolEntry *t = new ToolEntry;
            t->command = *it;
            t->info = *entry;
            m_lstTools.append( t );
          }
        }

        QObject::connect( m_pPopupMenu, SIGNAL( activated( int ) ), this, SLOT( slotActivateTool( int ) ) );
      }
    }

    m_pPopupMenu->popup( _point );
}

void KSpreadView::slotActivateTool( int _id )
{
  ASSERT( m_pTable );

  // Is it the id of a tool in the latest popupmenu ?
  if( _id < m_popupMenuFirstToolId )
    return;

  ToolEntry* entry = m_lstTools.at( _id - m_popupMenuFirstToolId );

  KoDataTool* tool = entry->info.createTool();
  if ( !tool )
  {
      kdDebug(36001) << "Could not create Tool" << endl;
      return;
  }

  KSpreadCell *cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
  ASSERT( !cell->isFormular() && !cell->isValue() );

  QString text = cell->text();
  QString tmpText=cell->text();
  if ( tool->run( entry->command, &text, "QString", "text/plain") )
      {
      if ( !m_pDoc->undoBuffer()->isLocked() )
        {
        KSpreadUndoSetText* undo = new KSpreadUndoSetText( m_pDoc, m_pTable, tmpText, m_pCanvas->markerColumn(), m_pCanvas->markerRow() ,cell->getFormatNumber( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
        m_pDoc->undoBuffer()->appendUndo( undo );
        }
      cell->setCellText( text, true );
      editWidget()->setText( text );
      }
}

void KSpreadView::deleteSelection()
{
    ASSERT( m_pTable );

    m_pTable->deleteSelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    resultOfCalc();
    updateEditWidget();
}

void KSpreadView::adjust()
{
    QRect r( activeTable()-> selectionRect() );
    if( r.right() ==0x7FFF || r.bottom()==0x7FFF)
    {
        KMessageBox::error( this, i18n("Area too large!"));
    }
    else
    {
    canvasWidget()->adjustArea();
    }
}

void KSpreadView::clearTextSelection()
{
    ASSERT( m_pTable );
    m_pTable->clearTextSelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );

    updateEditWidget();
}

void KSpreadView::clearCommentSelection()
{
    ASSERT( m_pTable );
    m_pTable->setSelectionRemoveComment( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );

    updateEditWidget();
}

void KSpreadView::clearValiditySelection()
{
    ASSERT( m_pTable );
    m_pTable->clearValiditySelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );

    updateEditWidget();
}

void KSpreadView::clearConditionalSelection()
{
    ASSERT( m_pTable );
    m_pTable->clearConditionalSelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );

    updateEditWidget();
}


void KSpreadView::defaultSelection()
{
  ASSERT( m_pTable );
  m_pTable->defaultSelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );

  updateEditWidget();
}

void KSpreadView::slotInsert()
{
    QRect r( activeTable()-> selectionRect() );
    if(r.left()==0)
        r.setCoords(m_pCanvas->markerColumn(), m_pCanvas->markerRow()
                ,m_pCanvas->markerColumn(), m_pCanvas->markerRow());
    KSpreadinsert dlg( this, "Insert", r,KSpreadinsert::Insert );
    dlg.exec();
}

void KSpreadView::slotRemove()
{
    QRect r( activeTable()-> selectionRect() );
    if(r.left()==0)
        r.setCoords(m_pCanvas->markerColumn(), m_pCanvas->markerRow()
                ,m_pCanvas->markerColumn(), m_pCanvas->markerRow());

    KSpreadinsert dlg( this, "Remove", r,KSpreadinsert::Remove );
    dlg.exec();
}

void KSpreadView::slotInsertCellCopy()
{
     if ( !m_pTable )
        return;
    QRect r( activeTable()->selectionRect() );
    if(r.left()==0 )
        r.setCoords(m_pCanvas->markerColumn(), m_pCanvas->markerRow() ,
                m_pCanvas->markerColumn(), m_pCanvas->markerRow() );

    if( !m_pTable->testAreaPasteInsert())
        m_pTable->paste( QPoint( r.left(), r.top() ) ,true, Normal,OverWrite,true);
    else
        {
        QRect r( activeTable()-> selectionRect() );
        KSpreadpasteinsert dlg( this, "Remove", r );
        dlg.exec();
        }
    if(m_pTable->getAutoCalc())
        m_pTable->recalc(true);
    updateEditWidget();
}

void KSpreadView::setAreaName()
{
    KSpreadarea* dlg = new KSpreadarea( this, "Area Name",QPoint(m_pCanvas->markerColumn(), m_pCanvas->markerRow()) );
    dlg->show();
}

void KSpreadView::showAreaName()
{
    KSpreadreference* dlg = new KSpreadreference( this, "Show area" );
    dlg->show();
}

void KSpreadView::resizeRow()
{
    QRect selection( activeTable()->selectionRect() );
    if(selection.bottom()==0x7FFF)
        KMessageBox::error( this, i18n("Area too large!"));
    else
        {
        KSpreadresize2* dlg = new KSpreadresize2( this, "Resize row", KSpreadresize2::resize_row );
        dlg->show();
        }
}

void KSpreadView::resizeColumn()
{
    QRect selection( activeTable()->selectionRect() );
    if(selection.right()==0x7FFF)
        KMessageBox::error( this, i18n("Area too large!"));
    else
        {
        KSpreadresize2* dlg = new KSpreadresize2( this, "Resize column", KSpreadresize2::resize_column );
        dlg->show();
        }
}

void KSpreadView::equalizeRow()
{
    QRect selection( activeTable()->selectionRect() );
    if(selection.bottom()==0x7FFF)
        KMessageBox::error( this, i18n("Area too large!"));
    else
        canvasWidget()->equalizeRow();
}

void KSpreadView::equalizeColumn()
{
    QRect selection( activeTable()->selectionRect() );
    if(selection.right()==0x7FFF)
        KMessageBox::error( this, i18n("Area too large!"));
    else
        canvasWidget()->equalizeColumn();
}


void KSpreadView::layoutDlg()
{
  QRect selection( m_pTable->selectionRect() );
  if ( selection.contains( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) ) )
        CellLayoutDlg dlg( this, m_pTable, selection.left(), selection.top(),
                       selection.right(), selection.bottom() );
  else
        CellLayoutDlg dlg( this, m_pTable, m_pCanvas->markerColumn(), m_pCanvas->markerRow(), m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
}

void KSpreadView::paperLayoutDlg()
{
    m_pDoc->paperLayoutDlg();
}

void KSpreadView::multiRow( bool b )
{
    if ( m_toolbarLock )
        return;

    if ( m_pTable != 0L )
        m_pTable->setSelectionMultiRow( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), b );
}

void KSpreadView::alignLeft( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable != 0L )
        {
        if ( !b )
                m_pTable->setSelectionAlign( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Undefined );
        else
                m_pTable->setSelectionAlign( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Left );
        }
}

void KSpreadView::alignRight( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable != 0L )
        {
        if ( !b )
                m_pTable->setSelectionAlign( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Undefined );
        else
                m_pTable->setSelectionAlign( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Right );
        }
}

void KSpreadView::alignCenter( bool b )
{
    if ( m_toolbarLock )
        return;

    if ( m_pTable != 0L )
        {
        if ( !b )
                m_pTable->setSelectionAlign( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Undefined );
        else
                m_pTable->setSelectionAlign( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Center );
        }
}

void KSpreadView::alignTop( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( !b )
        return;

    if ( m_pTable != 0L )
        m_pTable->setSelectionAlignY( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Top );
}

void KSpreadView::alignBottom( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( !b )
        return;

    if ( m_pTable != 0L )
      m_pTable->setSelectionAlignY( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Bottom );
}

void KSpreadView::alignMiddle( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( !b )
        return;

    if ( m_pTable != 0L )
        m_pTable->setSelectionAlignY( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Middle );
}


void KSpreadView::moneyFormat(bool b)
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable != 0L )
        m_pTable->setSelectionMoneyFormat( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),b );
    updateEditWidget();
}

void KSpreadView::precisionPlus()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionPrecision( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 1 );
}

void KSpreadView::precisionMinus()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionPrecision( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), -1 );
}


void KSpreadView::percent( bool b)
{
   if ( m_toolbarLock )
        return;
  if ( m_pTable != 0L )
    m_pTable->setSelectionPercent( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) ,b );
  updateEditWidget();
}

void KSpreadView::insertObject()
{
  KoDocumentEntry e = KoPartSelectDia::selectPart( m_pCanvas );
  if ( e.isEmpty() )
    return;

  (void)new KSpreadInsertHandler( this, m_pCanvas, e );
}

void KSpreadView::insertChart()
{
    QRect r( activeTable()-> selectionRect() );
    if( r.right() ==0x7FFF ||r.bottom()==0x7FFF)
    {
        KMessageBox::error( this, i18n("Area too large!"));
        return;
    }
    QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( "'KOfficeChart' in ServiceTypes" );
    if ( vec.isEmpty() )
    {
        KMessageBox::error( this, i18n("Sorry, no charting component registered") );
        return;
    }

    (void)new KSpreadInsertHandler( this, m_pCanvas, vec[0], TRUE );
}


/*
  // TODO Use KoView setScaling/xScaling/yScaling instead
void KSpreadView::zoomMinus()
{
  if ( m_fZoom <= 0.25 )
    return;

  m_fZoom -= 0.25;

  if ( m_pTable != 0L )
    m_pTable->setLayoutDirtyFlag();

  m_pCanvas->repaint();
  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
}

void KSpreadView::zoomPlus()
{
  if ( m_fZoom >= 3 )
    return;

  m_fZoom += 0.25;

  if ( m_pTable != 0L )
    m_pTable->setLayoutDirtyFlag();

  m_pCanvas->repaint();
  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
}
*/

void KSpreadView::removeTable()
{
   if ( doc()->map()->count() <= 1 )
    {
        KNotifyClient::beep();
        KMessageBox::sorry( this, i18n("You cannot delete the only table of the map."), i18n("Remove table") ); // FIXME bad english? no english!
        return;
    }
    KNotifyClient::beep();
    int ret = KMessageBox::warningYesNo(this,i18n("You are going to remove the active table.\nDo you want to continue?"),i18n("Remove table"));

    if ( ret == 3 )
    {
        if ( m_pCanvas->editor() )
        {
                m_pCanvas->deleteEditor( false );
        }
        m_pDoc->setModified( true );
        KSpreadTable *tbl = activeTable();
        tbl->removeTable();

        doc()->map()->removeTable( tbl );
        delete tbl;
    }
}


void KSpreadView::setText( const QString& _text )
{
  if ( m_pTable == 0L )
    return;

  m_pTable->setText( m_pCanvas->markerRow(), m_pCanvas->markerColumn(), _text );
  KSpreadCell* cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
  if(!cell->isValue() && !cell->isFormular() &&!cell->isBool() && !cell->isDate() && !cell->isTime())
        {
        if(_text.at(0)<'0' || _text.at(0)>'9')
                m_pDoc->addStringCompletion(_text);
        }
}

//------------------------------------------------
//
// Document signals
//
//------------------------------------------------

void KSpreadView::slotAddTable( KSpreadTable *_table )
{
  addTable( _table );
}

void KSpreadView::slotUpdateView( KSpreadTable *_table )
{
    // qDebug("void KSpreadView::slotUdateView( KSpreadTable *_table )\n");

    // Do we display this table ?
    if ( _table != m_pTable )
        return;

    m_pCanvas->update();
}

void KSpreadView::slotUpdateView( KSpreadTable *_table, const QRect& _rect )
{
    // qDebug("void KSpreadView::slotUpdateView( KSpreadTable *_table, const QRect& %i %i|%i %i )\n",_rect.left(),_rect.top(),_rect.right(),_rect.bottom());

    // Do we display this table ?
    if ( _table != m_pTable )
        return;

    m_pCanvas->updateCellRect( _rect );
}

void KSpreadView::slotUpdateHBorder( KSpreadTable *_table )
{
    // qDebug("void KSpreadView::slotUpdateHBorder( KSpreadTable *_table )\n");

    // Do we display this table ?
    if ( _table != m_pTable )
        return;

    m_pHBorderWidget->update();
}

void KSpreadView::slotUpdateVBorder( KSpreadTable *_table )
{
    // qDebug("void KSpreadView::slotUpdateVBorder( KSpreadTable *_table )\n");

    // Do we display this table ?
    if ( _table != m_pTable )
        return;

    m_pVBorderWidget->update();
}

void KSpreadView::slotChangeChooseSelection( KSpreadTable *_table, const QRect &_old, const QRect &_new )
{
    // Do we display this table ?
    if ( _table != m_pTable )
        return;

    m_pCanvas->updateChooseMarker( _old, _new );

    // Emit a signal for internal use
    emit sig_chooseSelectionChanged( _table, _new );
}

void KSpreadView::slotChangeSelection( KSpreadTable *_table, const QRect &_old, const QRect& _old_marker )
{
    QRect n = _table->selectionRect();
    // QRect m = _table->marker();

    // printf ("void KSpreadView::slotChangeSelection( KSpreadTable *_table, const QRect &_old %i %i|%i %i, const QPoint &m %i %i )\n",_old.left(),_old.top(),_old.right(),_old.bottom(),_old_marker.x(),_old_marker.y() );
    // printf ("      const QRect &_new %i %i|%i %i, const QPoint &m %i %i )\n",n.left(),n.top(),n.right(),n.bottom(),m.x(),m.y() );

    // Emit a signal for internal use
    emit sig_selectionChanged( _table, n );

    // Empty selection ?
    // Activate or deactivate some actions. This code is duplicated
    // in KSpreadView::slotUnselect
    if ( (n.left() == 0 && n.top() == 0)
    ||n.right() ==0x7FFF ||n.bottom()==0x7FFF)
        m_tableFormat->setEnabled( FALSE );
    else
        m_tableFormat->setEnabled( TRUE );

    if( n.top()!=0 &&n.bottom()==0x7FFF)
        {
        m_resizeRow->setEnabled(false);
        m_equalizeRow->setEnabled(false);
        }
    else
        {
        m_resizeRow->setEnabled(true);
        m_equalizeRow->setEnabled(true);
        }

    if( n.left()!=0 &&n.right()==0x7FFF)
        {
        m_resizeColumn->setEnabled(false);
        m_equalizeColumn->setEnabled(false);
        }
    else
        {
        m_resizeColumn->setEnabled(true);
        m_equalizeColumn->setEnabled(true);
        }
   
    
    resultOfCalc();
    // Send some event around. This is read for example
    // by the calculator plugin.
    KSpreadSelectionChanged ev( n, activeTable()->name() );
    QApplication::sendEvent( this, &ev );

    // Do we display this table ?
    if ( _table != m_pTable )
        return;

    m_pCanvas->updateSelection( _old, _old_marker );
    m_pVBorderWidget->update();
    m_pHBorderWidget->update();
}

void KSpreadView::resultOfCalc()
{
  double result=0.0;
  int nbCell=0;
QRect n = activeTable()->selectionRect();
 QRect tmpRect(n);
 if(n.left()==0)
   tmpRect.setCoords( m_pCanvas->markerColumn(), m_pCanvas->markerRow(),
		      m_pCanvas->markerColumn(), m_pCanvas->markerRow()); 
 MethodOfCalc tmpMethod=m_pDoc->getTypeOfCalc() ;
if(tmpRect.bottom()==0x7FFF)
   {
     KSpreadCell* c = activeTable()->firstCell();
     for( ;c; c = c->nextCell() )
       {
	 int col = c->column();
	 if ( tmpRect.left() <= col && tmpRect.right() >= col
	      &&!c->isObscuringForced())
	   {
	     if(c->isValue())
	       {
		 switch(tmpMethod)
		   {
		   case Sum:
		     result+=c->valueDouble();		 
		     break;
		   case Average:
		     result+=c->valueDouble();	
		     break;
		   case Min:
		     if(result!=0)
		       result=QMIN(c->valueDouble(),result);
		     else
		       result=c->valueDouble();
		     break;
		   case Max:
		     if(result!=0)
		       result=QMAX(c->valueDouble(),result);
		     else
		       result=c->valueDouble();
		     break;
		   case Count:
		     break;
		   default:
		     break;
		   }
		 nbCell++;
	       }
	   }
       }
   }
 else if(tmpRect.right()==0x7FFF)
   {
     KSpreadCell* c = activeTable()->firstCell();
     for( ; c; c = c->nextCell() )
       {
	 int row = c->row();
	 if ( tmpRect.top() <= row && tmpRect.bottom() >= row
	      &&!c->isObscuringForced())
	   {
	     if(c->isValue())
	       {
		 switch(tmpMethod )
		   {
		   case Sum:
		     result+=c->valueDouble();		 
		     break;
		   case Average:
		     result+=c->valueDouble();	
		     break;
		   case Min:
		     if(result!=0)
		       result=QMIN(c->valueDouble(),result);
		     else
		       result=c->valueDouble();
		     break;
		   case Max:
		     if(result!=0)
		       result=QMAX(c->valueDouble(),result);
		     else
		       result=c->valueDouble();
		     break;
		   case Count:
		     break;
		   default:
		     break;
		   }
		 nbCell++;
	       }	
	   }
       }
   }
 else
   {
     for (int i=tmpRect.left();i<=tmpRect.right();i++)
       for(int j=tmpRect.top();j<=tmpRect.bottom();j++)
	 {
	   KSpreadCell *cell = activeTable()->cellAt( i, j );
	   if(!cell->isDefault() && cell->isValue())
	     {
		 switch(tmpMethod )
		   {
		   case Sum:
		     result+=cell->valueDouble();		 
		     break;
		   case Average:
		     result+=cell->valueDouble();	
		     break;
		   case Min:
		     if(result!=0)
		       result=QMIN(cell->valueDouble(),result);
		     else
		       result=cell->valueDouble();
		     break;
		   case Max:
		     if(result!=0)
		       result=QMAX(cell->valueDouble(),result);
		     else
		       result=cell->valueDouble();
		     break;
		   case Count:
		     break;
		   default:
		     break;
		   }
		 nbCell++;
	       }	      
	 }
   }
 QString tmp;
 switch(tmpMethod )
   {
   case Sum:
     tmp=i18n(" Sum: %1").arg(result);		 
     break;
   case Average:
     result=result/nbCell;
     tmp=i18n("Average: %1").arg(result);	
     break;
   case Min:
     tmp=i18n("Min: %1").arg(result);
     break;
   case Max:
     tmp=i18n("Max: %1").arg(result);
     break;
   case Count:
     tmp=i18n("Count: %1").arg(nbCell);
     break;
   }
	 
	 
 KoMainWindow * tmpKo = m_pDoc->firstShell();
 for ( ; tmpKo ; tmpKo = m_pDoc->nextShell() )
   tmpKo->statusBarLabel()->setText(tmp);
}

void KSpreadView::slotUnselect( KSpreadTable *_table, const QRect& _old )
{
    // Do we display this table ?
    if ( _table != m_pTable )
        return;

    // Unselect the action which only works on a selection
    // with mutiple cells.
    m_tableFormat->setEnabled( FALSE );

    m_pCanvas->updateSelection( _old, _table->marker() );
    m_pVBorderWidget->update();
    m_pHBorderWidget->update();
}

void KSpreadView::repaintPolygon( const QPointArray& polygon )
{
    QPointArray arr = polygon;
    QWMatrix m = matrix()/*.invert()*/;

    for( int i = 0; i < 4; ++i )
        arr.setPoint( i, m.map( arr.point( i ) ) );

    emit regionInvalidated( QRegion( arr ), TRUE );
}

void KSpreadView::paintContent( QPainter& painter, const QRect& rect, bool transparent )
{
    m_pDoc->paintContent( painter, rect, transparent, activeTable() );
}

QWMatrix KSpreadView::matrix() const
{
    QWMatrix m;
    m.translate( -m_pCanvas->xOffset(), -m_pCanvas->yOffset() );
    m.scale( zoom(), zoom() );
    return m;
}

void KSpreadView::transformPart()
{
    ASSERT( selectedChild() );

    if ( m_transformToolBox.isNull() )
    {
        m_transformToolBox = new KoTransformToolBox( selectedChild(), topLevelWidget() );
        m_transformToolBox->show();

        m_transformToolBox->setDocumentChild( selectedChild() );
    }
    else
    {
        m_transformToolBox->show();
        m_transformToolBox->raise();
    }
}

void KSpreadView::slotChildSelected( KoDocumentChild* ch )
{
    m_transform->setEnabled( TRUE );

    if ( !m_transformToolBox.isNull() )
    {
        m_transformToolBox->setEnabled( TRUE );
        m_transformToolBox->setDocumentChild( ch );
    }
}

void KSpreadView::slotChildUnselected( KoDocumentChild* )
{
    m_transform->setEnabled( FALSE );

    if ( !m_transformToolBox.isNull() )
    {
        m_transformToolBox->setEnabled( FALSE );
    }
}

void KSpreadView::enableFormulaToolBar( bool b )
{
    // qDebug("TOOLBARS mode=%s", b ? "TRUE" : "FALSE" );
    m_formulaPower->setEnabled( b );
    m_formulaSubscript->setEnabled( b );
    m_formulaParantheses->setEnabled( b );
    m_formulaAbsValue->setEnabled( b );
    m_formulaBrackets->setEnabled( b );
    m_formulaFraction->setEnabled( b );
    m_formulaRoot->setEnabled( b );
    m_formulaIntegral->setEnabled( b );
    m_formulaMatrix->setEnabled( b );
    m_formulaLeftSuper->setEnabled( b );
    m_formulaLeftSub->setEnabled( b );
    m_formulaSum->setEnabled( b );
    m_formulaProduct->setEnabled( b );
}

void KSpreadView::deleteEditor( bool saveChanges )
{
  m_pCanvas->deleteEditor( saveChanges );
}

DCOPObject* KSpreadView::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KSpreadViewIface( this );

    return m_dcop;
}

QWidget *KSpreadView::canvas()
{
  return canvasWidget();
}

int KSpreadView::canvasXOffset() const
{
  return canvasWidget()->xOffset();
}

int KSpreadView::canvasYOffset() const
{
  return canvasWidget()->yOffset();
}

#include "kspread_view.moc"

