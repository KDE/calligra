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

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>

#include <qpushbutton.h>
#include <qprintdialog.h>
#include <qcollection.h>
#include <qkeycode.h>
#include <kaction.h>
#include <qmime.h>
#include <qtoolbutton.h>
#include <qtimer.h>

#include <kapp.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kstdaccel.h>
#include <kstdaction.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kformulaedit.h>
#include <kcoloractions.h>

#include <dcopclient.h>
#include <dcopref.h>

#include <koPartSelectDia.h>
#include <koQueryTrader.h>

#include "kspread_map.h"
#include "kspread_table.h"
#include "kspread_dlg_scripts.h"
#include "kspread_doc.h"
#include "kspread_shell.h"
#include "kspread_dlg_cons.h"
#include "kspread_util.h"
#include "kspread_canvas.h"
#include "kspread_tabbar.h"
#include "kspread_view.h"
#include "kspread_dlg_formula.h"
#include "kspread_dlg_special.h"
#include "kspread_dlg_replace.h"
#include "kspread_dlg_sort.h"
#include "kspread_dlg_anchor.h"
#include "kspread_dlg_resize.h"
#include "kspread_dlg_show.h"
#include "kspread_dlg_insert.h"
#include "kspread_handler.h"
#include "kspread_events.h"
#include "kspread_editors.h"
#include "kspread_dlg_format.h"
#include "kspread_dlg_oszi.h"
#include "kspread_dlg_conditional.h"
#include "kspread_dlg_series.h"
#include "kspread_dlg_reference.h"
#include "kspread_dlg_area.h"
#include "kspread_dlg_resize2.h"
#include "kspread_dlg_preference.h"
#include "kspread_dlg_comment.h"
#include "kspread_dlg_angle.h"
#include <kscript_scriptmenu.h>

#include "handler.h"
#include "toolbox.h"

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

    // Handler for moving and resizing embedded parts
    (void)new ContainerHandler( this, m_pCanvas );

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

    m_alignTop = new KToggleAction( i18n("Align top"), "top", 0, actionCollection(), "top");
    connect( m_alignTop, SIGNAL( toggled( bool ) ), this, SLOT( alignTop( bool ) ) );
    m_alignTop->setExclusiveGroup( "Pos" );
    m_alignMiddle = new KToggleAction( i18n("Align middle"), "middle", 0, actionCollection(), "middle");
    connect( m_alignMiddle, SIGNAL( toggled( bool ) ), this, SLOT( alignMiddle( bool ) ) );
    m_alignMiddle->setExclusiveGroup( "Pos" );
    m_alignBottom = new KToggleAction( i18n("Align bottom"), "bottom", 0, actionCollection(), "bottom");
    connect( m_alignBottom, SIGNAL( toggled( bool ) ), this, SLOT( alignBottom( bool ) ) );
    m_alignBottom->setExclusiveGroup( "Pos" );

    m_transform = new KAction( i18n("Transform object..."), "rotate", 0, this, SLOT( transformPart() ),
			       actionCollection(), "transform" );
    m_transform->setEnabled( FALSE );
    connect( m_transform, SIGNAL( activated() ), this, SLOT( transformPart() ) );
    m_copy = KStdAction::copy( this, SLOT( copySelection() ), actionCollection(), "copy" );
    m_paste = KStdAction::paste( this, SLOT( paste() ), actionCollection(), "paste" );
    m_cut = KStdAction::cut( this, SLOT( cutSelection() ), actionCollection(), "cut" );
    m_specialPaste = new KAction( i18n("Special Paste..."), "special_paste",0, this, SLOT( specialPaste() ), actionCollection(), "specialPaste" );
    m_editCell = new KAction( i18n("Edit Cell"),"cell_edit", CTRL + Key_E, this, SLOT( editCell() ), actionCollection(), "editCell" );
    m_delete = new KAction( i18n("Delete"),"deletecell", 0, this, SLOT( deleteSelection() ), actionCollection(), "delete" );
    m_clear = new KAction( i18n("Clear"), 0, this, SLOT( clearSelection() ), actionCollection(), "clear" );
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

    m_undo = KStdAction::undo( this, SLOT( undo() ), actionCollection(), "undo" );
    m_undo->setEnabled( FALSE );
    m_redo = KStdAction::redo( this, SLOT( redo() ), actionCollection(), "redo" );
    m_redo->setEnabled( FALSE );
    m_paperLayout = new KAction( i18n("Paper Layout..."), 0, this, SLOT( paperLayoutDlg() ), actionCollection(), "paperLayout" );
    m_insertTable = new KAction( i18n("Insert Table"),"inserttable", 0, this, SLOT( insertTable() ), actionCollection(), "insertTable" );
    m_removeTable = new KAction( i18n("Remove Table"), "delete_table",0,this, SLOT( removeTable() ), actionCollection(), "removeTable" );
    m_showTable = new KAction(i18n("Show Table"),0 ,this,SLOT( showTable()), actionCollection(), "showTable" );
    m_hideTable = new KAction(i18n("Hide Table"),0 ,this,SLOT( hideTable()), actionCollection(), "hideTable" );
    m_removeTable = new KAction( i18n("Preference..."), 0, this, SLOT( preference() ), actionCollection(), "preference" );
    m_firstLetterUpper = new KAction( i18n("Convert first letter to upper case"),"first_letter_upper" ,0, this, SLOT( firstLetterUpper() ), actionCollection(), "firstletterupper" );
    m_verticalText = new KToggleAction( i18n("Vertical text"),"vertical_text" ,0 ,actionCollection(), "verticaltext" );
    connect( m_verticalText, SIGNAL( toggled( bool ) ), this, SLOT( verticalText( bool ) ) );
    m_changeAngle = new KAction( i18n("Change Angle..."), 0, this, SLOT( changeAngle() ), actionCollection(), "changeangle" );
    m_addModifyComment = new KAction( i18n("Add/modify comment..."),"comment", 0, this, SLOT( addModifyComment() ), actionCollection(), "addmodifycomment" );
    //m_showComment = new KAction( i18n("Show comment"), 0, this, SLOT( showComment() ), actionCollection(), "showcomment" );
    m_removeComment = new KAction( i18n("Remove comment"),"removecomment", 0, this, SLOT( removeComment() ), actionCollection(), "removecomment" );
    m_editGlobalScripts = new KAction( i18n("Edit Global Scripts..."), 0, this, SLOT( editGlobalScripts() ),
				       actionCollection(), "editGlobalScripts" );
    m_editLocalScripts = new KAction( i18n("Edit Local Scripts..."), 0, this, SLOT( editLocalScripts() ), actionCollection(), "editLocalScripts" );
    m_reloadScripts = new KAction( i18n("Reload Scripts"), 0, this, SLOT( reloadScripts() ), actionCollection(), "reloadScripts" );
    m_showPageBorders = new KToggleAction( i18n("Show page borders"), 0, actionCollection(), "showPageBorders");
    connect( m_showPageBorders, SIGNAL( toggled( bool ) ), this, SLOT( togglePageBorders( bool ) ) );
    m_replace = new KAction( i18n("Find and Replace..."), "find",CTRL + Key_F, this, SLOT( replace() ), actionCollection(), "replace" );
    m_conditional = new KAction( i18n("Relational cell attributes..."), 0, this, SLOT( conditional() ), actionCollection(), "conditional" );
    m_sort = new KAction( i18n("Sort"), 0, this, SLOT( sort() ), actionCollection(), "sort" );
    m_consolidate = new KAction( i18n("Consolidate..."), 0, this, SLOT( consolidate() ), actionCollection(), "consolidate" );

    m_multiRow = new KToggleAction( i18n("Multi Row"), "multirow", 0, actionCollection(), "multiRow" );
    connect( m_multiRow, SIGNAL( toggled( bool ) ), this, SLOT( multiRow( bool ) ) );
    m_selectFont = new KFontAction( i18n("Select Font"), 0, actionCollection(), "selectFont" );
    connect( m_selectFont, SIGNAL( activated( const QString& ) ), this, SLOT( fontSelected( const QString& ) ) );
    m_selectFontSize = new KFontSizeAction( i18n("Select Font Size"), 0, actionCollection(), "selectFontSize" );
    connect( m_selectFontSize, SIGNAL( fontSizeChanged( int ) ), this, SLOT( fontSizeSelected( int ) ) );
    m_deleteColumn = new KAction( i18n("Delete Column"), "delete_table_col", 0, this, SLOT( deleteColumn() ),
				  actionCollection(), "deleteColumn" );
    m_deleteRow = new KAction( i18n("Delete Row"), "delete_table_row", 0, this, SLOT( deleteRow() ),
			       actionCollection(), "deleteRow" );
    m_insertColumn = new KAction( i18n("Insert Column"), "insert_table_col" , 0, this, SLOT( insertColumn() ),
				  actionCollection(), "insertColumn" );
    m_insertRow = new KAction( i18n("Insert Row"), "insert_table_row", 0, this, SLOT( insertRow() ),
			       actionCollection(), "insertRow" );
    m_insertCell = new KAction( i18n("Insert Cell"), "insertcell", 0, this, SLOT( slotInsert() ),
			       actionCollection(), "insertCell" );
    m_removeCell = new KAction( i18n("Remove Cell"), "removecell", 0, this, SLOT( slotRemove() ),
			       actionCollection(), "removeCell" );
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
    connect( m_formulaSelection, SIGNAL( activated( const QString& ) ),
	     this, SLOT( formulaSelection( const QString& ) ) );

    // Insert menu
    (void) new KAction( i18n("Formula..."), "funct", 0, this, SLOT( insertFormula() ),
                        actionCollection(), "insertFormula" );
    (void) new KAction( i18n("Series..."),"series", 0, this, SLOT( series() ), actionCollection(), "series" );
    (void) new KAction( i18n("Anchor..."), 0, this, SLOT( createAnchor() ), actionCollection(), "createAnchor" );
    (void) new KAction( i18n("Object..."), "parts", 0, this, SLOT( insertObject() ),
                        actionCollection(), "insertPart");
    (void) new KAction( i18n("Chart"), "chart", 0, this, SLOT( insertChart() ), actionCollection(), "insertChart" );

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
    m_oszi = new KAction( i18n("Osciloscope..."),"oscilloscope", 0, this, SLOT( oszilloscope() ), actionCollection(), "oszi" );
    m_scripts = new KScriptMenu( DCOPRef( kapp->dcopClient()->appId(), dcopObject()->objId() ), KSpreadFactory::global(),
				 i18n("Scripts"), actionCollection(), "scripts" );

    connect( this, SIGNAL( childSelected( KoDocumentChild* ) ),
	     this, SLOT( slotChildSelected( KoDocumentChild* ) ) );
    connect( this, SIGNAL( childUnselected( KoDocumentChild* ) ),
	     this, SLOT( slotChildUnselected( KoDocumentChild* ) ) );
    // If a selected part becomes active this is like it is deselected
    // just before.
    connect( this, SIGNAL( childActivated( KoDocumentChild* ) ),
	     this, SLOT( slotChildUnselected( KoDocumentChild* ) ) );

    QTimer::singleShot( 0, this, SLOT( initialPosition() ) );

}

KSpreadView::~KSpreadView()
{
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
	tbl->recalc(true);
    m_bLoading =true;
}

/*
// Moved that to KSpreadCanvas
bool KSpreadView::eventKeyPressed( QKeyEvent* _event, bool choose )
{
    if ( m_pTable == 0L )
	return true;

    int x, y;
    RowLayout *rl;
    ColumnLayout *cl;
    KSpreadCell *cell;
    // Flag that indicates whether we make a selection right now
    bool make_select = FALSE;

    QRect selection;
    int marker_column;
    int marker_row;
    void (KSpreadCanvas::*hideMarker)();
    void (KSpreadCanvas::*showMarker)();
    void (KSpreadCanvas::*setMarkerColumn)(int);
    void (KSpreadCanvas::*setMarkerRow)(int);
    int (KSpreadCanvas::*markerColumn)() const;
    int (KSpreadCanvas::*markerRow)() const;
    if ( choose )
    {
	selection = m_pTable->chooseRect();
	// No selection until now ? => select the current cell
	if ( selection.left() == 0 )
	    selection = QRect( m_pCanvas->chooseMarkerColumn(), m_pCanvas->chooseMarkerRow(), 1, 1 );
	marker_column = m_pCanvas->chooseMarkerColumn();
	marker_row = m_pCanvas->chooseMarkerRow();
	hideMarker = &KSpreadCanvas::hideChooseMarker;
	showMarker = &KSpreadCanvas::showChooseMarker;
	setMarkerRow = &KSpreadCanvas::setChooseMarkerRow;
	setMarkerColumn = &KSpreadCanvas::setChooseMarkerColumn;
	markerRow = &KSpreadCanvas::chooseMarkerRow;
	markerColumn = &KSpreadCanvas::chooseMarkerColumn;
    }
    else
    {
	selection = m_pTable->selectionRect();
	marker_column = m_pCanvas->markerColumn();
	marker_row = m_pCanvas->markerRow();
	hideMarker = &KSpreadCanvas::hideMarker;
	showMarker = &KSpreadCanvas::showMarker;
	setMarkerRow = &KSpreadCanvas::setMarkerRow;
	setMarkerColumn = &KSpreadCanvas::setMarkerColumn;
	markerRow = &KSpreadCanvas::markerRow;
	markerColumn = &KSpreadCanvas::markerColumn;
    }

    // Done in KSpreadCanvas::keyPressEvent

    // Are we making a selection right now ? Go thru this only if no selection is made
    // or if we neither selected complete rows nor columns.
    if ( ( _event->state() & ShiftButton ) == ShiftButton &&
	 ( selection.left() == 0 || ( selection.right() != 0 && selection.bottom() != 0 ) ) &&
	 ( _event->key() == Key_Down || _event->key() == Key_Up || _event->key() == Key_Left || _event->key() == Key_Right ) )
	make_select = TRUE;

    // Do we have an old selection ? If yes, unselect everything
    if ( selection.left() != 0 && !make_select )
    {
	if ( choose )
	    m_pTable->setChooseRect( QRect( marker_column, marker_row, 1, 1 ) );
	else
	    m_pTable->unselect();
    }


    switch( _event->key() )
    {
    case Key_Return:
    case Key_Enter:
    case Key_Down:
	// Note: choose is only TRUE, if we get Key_Down
	
	(m_pCanvas->*hideMarker)();

	if ( selection.left() == 0 && make_select )
	    selection = QRect( marker_column, marker_row, 1, 1 );

	cell = m_pTable->cellAt( marker_column, marker_row );
	// Are we leaving a cell with extra size ?
	if ( cell->isForceExtraCells() )
	  (m_pCanvas->*setMarkerRow)( marker_row + 1 + cell->extraYCells() );
	else
  	  (m_pCanvas->*setMarkerRow)( marker_row + 1 );

	cell = m_pTable->cellAt( marker_column, marker_row );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	  (m_pCanvas->*setMarkerRow)( cell->obscuringCellsRow() );
	  (m_pCanvas->*setMarkerColumn)( cell->obscuringCellsColumn() );
	}

	y = m_pTable->rowPos( (m_pCanvas->*markerRow)(), m_pCanvas );
	rl = m_pTable->rowLayout( (m_pCanvas->*markerRow)() );
	if ( y + rl->height( m_pCanvas ) > m_pCanvas->height() )
	  vertScrollBar()->setValue( m_pCanvas->yOffset() + ( y + rl->height( m_pCanvas )
							      - m_pCanvas->height() ) );

	if ( make_select )
        {
	    // If we have been at the top of the selection ...
	    if ( selection.bottom() == marker_row )
		selection.setBottom( (m_pCanvas->*markerRow)() );
	    else if ( make_select )
		selection.setTop( (m_pCanvas->*markerRow)() );

	    if ( choose )
		m_pTable->setChooseRect( selection );
	    else
		m_pTable->setSelection( selection, m_pCanvas );
	}
	else if ( choose )
	    m_pTable->setChooseRect( QRect( m_pCanvas->chooseMarkerColumn(), m_pCanvas->chooseMarkerRow(), 1, 1 ) );

	(m_pCanvas->*showMarker)();

	if ( !choose )
	    updateEditWidget();
	break;

    case Key_Up:
	(m_pCanvas->*hideMarker)();

	if ( selection.left() == 0 && make_select )
	    selection = QRect( marker_column, marker_row, 1, 1 );

	(m_pCanvas->*setMarkerRow)( marker_row - 1 );
	cell = m_pTable->cellAt( marker_column, marker_row );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	  (m_pCanvas->*setMarkerRow)( cell->obscuringCellsRow() );
	  (m_pCanvas->*setMarkerColumn)( cell->obscuringCellsColumn() );
	}

	y = m_pTable->rowPos( (m_pCanvas->*markerRow)(), m_pCanvas );
	rl = m_pTable->rowLayout( (m_pCanvas->*markerRow)() );
	if ( y < 0 )
	    vertScrollBar()->setValue( m_pCanvas->yOffset() + y );

	if ( make_select )
        {
	    // If we have been at the top of the selection ...
	    if ( selection.top() == marker_row )
		selection.setTop( (m_pCanvas->*markerRow)() );
	    else if ( make_select )
		selection.setBottom( (m_pCanvas->*markerRow)() );

	    if ( choose )
		m_pTable->setChooseRect( selection );
	    else
		m_pTable->setSelection( selection, m_pCanvas );
	}
	else if ( choose )
	    m_pTable->setChooseRect( QRect( m_pCanvas->chooseMarkerColumn(), m_pCanvas->chooseMarkerRow(), 1, 1 ) );

	(m_pCanvas->*showMarker)();

	if ( !choose )
	    updateEditWidget();
	
	break;

    case Key_Left:
	(m_pCanvas->*hideMarker)();

	if ( selection.left() == 0 && make_select )
	    selection = QRect( marker_column, marker_row, 1, 1 );

	(m_pCanvas->*setMarkerColumn)( marker_column - 1 );
	cell = m_pTable->cellAt( marker_column, marker_row );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	  (m_pCanvas->*setMarkerRow)( cell->obscuringCellsRow() );
	  (m_pCanvas->*setMarkerColumn)( cell->obscuringCellsColumn() );
	}

	x = m_pTable->columnPos( (m_pCanvas->*markerColumn)(), m_pCanvas );
	cl = m_pTable->columnLayout( (m_pCanvas->*markerColumn)() );
	if ( x < 0 )
	  horzScrollBar()->setValue( m_pCanvas->xOffset() + x );

	if ( make_select )
        {
	    // If we have been at the left side of the selection ...
	    if ( selection.left() == marker_column )
		selection.setLeft( (m_pCanvas->*markerColumn)() );
	    else
		selection.setRight( (m_pCanvas->*markerColumn)() );

	    if ( choose )
		m_pTable->setChooseRect( selection );
	    else
		m_pTable->setSelection( selection, m_pCanvas );
	}
	else if ( choose )
	    m_pTable->setChooseRect( QRect( m_pCanvas->chooseMarkerColumn(), m_pCanvas->chooseMarkerRow(), 1, 1 ) );

	(m_pCanvas->*showMarker)();

	if ( !choose )
	    updateEditWidget();

	break;

    case Key_Right:
	(m_pCanvas->*hideMarker)();

	if ( selection.left() == 0 && make_select )
	    selection = QRect( marker_column, marker_row, 1, 1 );

	cell = m_pTable->cellAt( marker_column, marker_row );
	// Are we leaving a cell with extra size ?
	if ( cell->isForceExtraCells() )
	  (m_pCanvas->*setMarkerColumn)( marker_column + 1 + cell->extraXCells() );
	else
	  (m_pCanvas->*setMarkerColumn)( marker_column + 1 );

	cell = m_pTable->cellAt( marker_column, marker_row );
	// Go to the upper left corner of the obscuring object ( if there is one )
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	  (m_pCanvas->*setMarkerRow)( cell->obscuringCellsRow() );
	  (m_pCanvas->*setMarkerColumn)( cell->obscuringCellsColumn() );
	}

	x = m_pTable->columnPos( (m_pCanvas->*markerColumn)(), m_pCanvas );
	cl = m_pTable->columnLayout( (m_pCanvas->*markerColumn)() );
	if ( x + cl->width( m_pCanvas ) > m_pCanvas->width() )
	    horzScrollBar()->setValue( m_pCanvas->xOffset() + ( x + cl->width( m_pCanvas )
								- m_pCanvas->width() ) );

	if ( make_select )
        {
	    // If we have been at the right side of the selection ...
	    if ( selection.right() == marker_column )
		selection.setRight( (m_pCanvas->*markerColumn)() );
	    // We have been on the left side ....
	    else
		selection.setLeft( (m_pCanvas->*markerColumn)() );

	    if ( choose )
		m_pTable->setChooseRect( selection );
	    else
		m_pTable->setSelection( selection, m_pCanvas );
	}
	else if ( choose )
	    m_pTable->setChooseRect( QRect( m_pCanvas->chooseMarkerColumn(), m_pCanvas->chooseMarkerRow(), 1, 1 ) );

	(m_pCanvas->*showMarker)();

	if ( !choose )
	    updateEditWidget();
	break;
    }

  return true;
}
*/

void KSpreadView::updateEditWidget()
{
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

    m_selectFontSize->setFontSize( cell->textFontSize() );
    m_selectFont->setFont( cell->textFontFamily() );
    m_bold->setChecked( cell->textFontBold() );
    m_italic->setChecked( cell->textFontItalic() );
    m_underline->setChecked( cell->textFontUnderline() );
    m_strikeOut->setChecked( cell->textFontStrike() );

    if ( cell->align() == KSpreadLayout::Left )
	m_alignLeft->setChecked( TRUE );
    else if ( cell->align() == KSpreadLayout::Right )
	m_alignRight->setChecked( TRUE );
    else if ( cell->align() == KSpreadLayout::Center )
	m_alignCenter->setChecked( TRUE );
    else
    {
	m_alignLeft->setChecked( FALSE );
	m_alignRight->setChecked( FALSE );
	m_alignCenter->setChecked( FALSE );
    }

    if ( cell->alignY() == KSpreadLayout::Top )
	m_alignTop->setChecked( TRUE );
    else if ( cell->alignY() == KSpreadLayout::Middle )
	m_alignMiddle->setChecked( TRUE );
    else if ( cell->alignY() == KSpreadLayout::Bottom )
	m_alignBottom->setChecked( TRUE );

    m_verticalText->setChecked( cell->verticalText() );

    m_multiRow->setChecked( cell->multiRow() );
    /*if( cell->faktor()==100.0 && cell->postfix()=="%")
    	m_percent->setChecked( TRUE );
    else
    	m_percent->setChecked( FALSE );
    */
    if(cell->getFormatNumber()==KSpreadCell::Percentage)
        m_percent->setChecked( TRUE );
    else
    	m_percent->setChecked( FALSE );

    if(cell->getFormatNumber()==KSpreadCell::Money)
        m_money->setChecked( TRUE );
    else
    	m_money->setChecked( FALSE );

    if( cell->getComment().isEmpty())
        {
        //m_showComment->setEnabled(FALSE);
        m_removeComment->setEnabled(FALSE);
        }
    else
        {
        //m_showComment->setEnabled(TRUE);
        m_removeComment->setEnabled(TRUE);
        }
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
  //  m_newView->setEnabled( true );
  //m_pDoc->KXMLGUIClient::action( "newView" )->setEnabled( true ); // obsolete (Werner)
  m_oszi->setEnabled( true );
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
    QRect r( activeTable()-> selectionRect() );
    if( r.right() ==0x7FFF)
    {
	KMessageBox::error( this, i18n("Area too large!"));
    }
    else if(r.bottom()==0x7FFF)
    {
	KMessageBox::error( this, i18n("Area too large!"));
    }
    else
    {
	KSpreadFormatDlg dlg( this );
	dlg.exec();
    }
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

void KSpreadView::oszilloscope()
{
    QDialog* dlg = new KSpreadOsziDlg( this );
    dlg->show();
}

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

  QPixmap *pixmap = 0L;

  pixmap = new QPixmap( KSBarIcon(_file) );

  QButton *pb;
  if ( !_kbutton )
    pb = new QPushButton( _parent );
  else
    pb = new QToolButton( _parent );
  if ( pixmap )
    pb->setPixmap( *pixmap );

  return pb;
}

void KSpreadView::enableUndo( bool _b )
{
    m_undo->setEnabled( _b );
}

void KSpreadView::enableRedo( bool _b )
{
    m_redo->setEnabled( _b );
}

void KSpreadView::undo()
{
    m_pDoc->undo();

    updateEditWidget();
}

void KSpreadView::redo()
{
    m_pDoc->redo();

    updateEditWidget();
}

void KSpreadView::deleteColumn()
{
    if ( !m_pTable )
	return;
    m_pTable->deleteColumn( m_pCanvas->markerColumn() );
    KSpreadTable *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
	tbl->recalc(true);

    QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
    for( ; it.current(); ++it )
	it.current()->changeNameCellRef(m_pCanvas->marker(),true,KSpreadTable::ColumnRemove,m_pTable->name());

    updateEditWidget();
}

void KSpreadView::deleteRow()
{
    if ( !m_pTable )
	return;
    m_pTable->deleteRow( m_pCanvas->markerRow() );
    KSpreadTable *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
	tbl->recalc(true);

    QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
    for( ; it.current(); ++it )
	it.current()->changeNameCellRef(m_pCanvas->marker(),true,KSpreadTable::RowRemove,m_pTable->name());

    updateEditWidget();
}

void KSpreadView::insertColumn()
{
    if ( !m_pTable )
	return;
    m_pTable->insertColumn( m_pCanvas->markerColumn() );
    KSpreadTable *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
	tbl->recalc(true);
    QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
    for( ; it.current(); ++it )
	it.current()->changeNameCellRef(m_pCanvas->marker(),true,KSpreadTable::ColumnInsert, m_pTable->name());

    updateEditWidget();
}

void KSpreadView::insertRow()
{
    if ( !m_pTable )
	return;
    m_pTable->insertRow( m_pCanvas->markerRow() );
    KSpreadTable *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
	tbl->recalc(true);
    QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
    for( ; it.current(); ++it )
	it.current()->changeNameCellRef(m_pCanvas->marker(),true,KSpreadTable::RowInsert,m_pTable->name());

    updateEditWidget();
}

void KSpreadView::fontSelected( const QString &_font )
{
    if ( m_toolbarLock )
	return;

    if ( m_pTable != 0L )
      m_pTable->setSelectionFont( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), _font );

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
    if( r.right() !=0x7FFF && r.bottom() !=0x7FFF)
        adjust();
    updateEditWidget();
}

void KSpreadView::insertFormula()
{
  if ( m_pTable == 0L )
	return;
  KSpreadDlgFormula* dlg = new KSpreadDlgFormula( this, "Formula Editor" );
  dlg->show();
  // Is the dialog deleted when it's closed ? (David)
}

void KSpreadView::formulaSelection( const QString &_math )
{
    if ( m_pTable == 0 )
	return;

    if( _math == i18n("Others...") )
    {
        insertFormula();
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
    KMessageBox::error( this, i18n("One cell was selected!") );
  }
  else if( r.right() ==0x7FFF)
  {
    KMessageBox::error( this, i18n("Area too large!") );
  }
  else if(r.bottom()==0x7FFF)
  {
    KMessageBox::error( this, i18n("Area too large!") );
  }
  else
  {
    activeTable()->sortByColumn( r.left());
  }
}


void KSpreadView::sortDec()
{
    QRect r( activeTable()-> selectionRect() );
    if ( r.left() == 0 || r.top() == 0 ||
	 r.right() == 0 || r.bottom() == 0 )
    {
  	KMessageBox::error( this, i18n("One cell was selected!") );
    }
    else if( r.right() ==0x7FFF)
    {
	KMessageBox::error( this, i18n("Area too large!") );
    }
    else if(r.bottom()==0x7FFF)
    {
	KMessageBox::error( this, i18n("Area too large!") );
    }
    else
    {
	activeTable()->sortByColumn( r.left(),KSpreadTable::Decrease);
    }	

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
  {
    QRect selection( m_pTable->selectionRect() );
    if(selection.right()==0x7FFF ||selection.bottom()==0x7FFF)
    {
      KMessageBox::error( this, i18n("Area too large!") );
    }
    else
    {
      m_pTable->borderBottom( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) ,m_borderColor->color());
    }
  }
}
void KSpreadView::borderRight()
{
  if ( m_pTable != 0L )
  {
    QRect selection( m_pTable->selectionRect() );
    if(selection.right()==0x7FFF ||selection.bottom()==0x7FFF)
    {
      KMessageBox::error( this, i18n("Area too large!") );
    }
    else
    {
      m_pTable->borderRight( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) ,m_borderColor->color());
    }
  }
}
void KSpreadView::borderLeft()
{
  if ( m_pTable != 0L )
  {
    QRect selection( m_pTable->selectionRect() );
    if(selection.right()==0x7FFF ||selection.bottom()==0x7FFF)
    {
      KMessageBox::error( this, i18n("Area too large!") );
    }
    else
    {
      m_pTable->borderLeft( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),m_borderColor->color() );
    }
  }
}
void KSpreadView::borderTop()
{
  if ( m_pTable != 0L )
  {
    QRect selection( m_pTable->selectionRect() );
    if(selection.right()==0x7FFF ||selection.bottom()==0x7FFF)
    {
      KMessageBox::error( this, i18n("Area too large!") );
    }
    else
    {
      m_pTable->borderTop( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),m_borderColor->color() );
    }
  }
}
void KSpreadView::borderOutline()
{
  if ( m_pTable != 0L )
  {
    QRect selection( m_pTable->selectionRect() );
    if(selection.right()==0x7FFF ||selection.bottom()==0x7FFF)
    {
      KMessageBox::error( this, i18n("Area too large!") );
    }
    else
    {
      m_pTable->borderOutline( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) ,m_borderColor->color());
    }
  }
}
void KSpreadView::borderAll()
{
  if ( m_pTable != 0L )
  {
    QRect selection( m_pTable->selectionRect() );
    if(selection.right()==0x7FFF ||selection.bottom()==0x7FFF)
    {
      KMessageBox::error( this, i18n("Area too large!") );
    }
    else
    {
      m_pTable->borderAll( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),m_borderColor->color() );
    }
  }
}

void KSpreadView::borderRemove()
{
  if ( m_pTable != 0L )
  {
    QRect selection( m_pTable->selectionRect() );
    if(selection.right()==0x7FFF ||selection.bottom()==0x7FFF)
    {
      KMessageBox::error( this, i18n("Area too large!") );
    }
    else
    {
      m_pTable->borderRemove( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    }
  }

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

void KSpreadView::removeTable( KSpreadTable *_t )
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
 	   	doc()->removeArea((*it).ref_name);
  }

}

void KSpreadView::removeAllTables()
{
  m_pTabBar->removeAllTabs();

  setActiveTable( 0L );
}

void KSpreadView::setActiveTable( KSpreadTable *_t )
{
  if ( _t == m_pTable )
    return;

  m_pTable = _t;
  if ( m_pTable == 0L )
    return;

  m_pTabBar->setActiveTab( _t->tableName() );

  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
  m_pCanvas->repaint();

  m_pCanvas->slotMaxColumn( m_pTable->maxColumn() );
  m_pCanvas->slotMaxRow( m_pTable->maxRow() );
}

void KSpreadView::slotTableRenamed( KSpreadTable* table, const QString& old_name )
{
    m_pTabBar->renameTab( old_name, table->tableName() );
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

    setActiveTable( t );

    updateEditWidget();
    //refresh toggle button
    m_showPageBorders->setChecked( m_pTable->isShowPageBorders());

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
  KSpreadTable *t = m_pDoc->createTable();
  m_pDoc->addTable( t );
  updateEditWidget();
}

void KSpreadView::hideTable()
{
    if ( !m_pTable )
       return;
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
  if ( m_pTable )
  	{
  	QRect rect( activeTable()-> selectionRect() );

  	if((rect.right()==0x7FFF) ||(rect.bottom()==0x7FFF))
  		{
  		KMessageBox::error( this, i18n("Not supported"));
		}
  	else
   	 	m_pTable->copySelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
   	}
    	
}

void KSpreadView::cutSelection()
{
    if ( !m_pTable )
	return;
    QRect rect( activeTable()-> selectionRect() );

    if((rect.right()==0x7FFF) ||(rect.bottom()==0x7FFF))
  		{
  		KMessageBox::error( this, i18n("Not supported"));
		}
  	else
  		{
    		m_pTable->cutSelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    		updateEditWidget();
    		}
}

void KSpreadView::paste()
{
    if ( !m_pTable )
	return;

    m_pTable->paste( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    m_pTable->recalc(true);
    updateEditWidget();
}

void KSpreadView::specialPaste()
{
    if ( !m_pTable )
	return;

    KSpreadspecial dlg( this, "Special Paste" );
    if(dlg.exec())
        {
        m_pTable->recalc(true);
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

/*void KSpreadView::showComment()
{
  if ( !m_pTable )
	return;
  m_pCanvas->showComment();
}*/

void KSpreadView::changeAngle()
{
  if ( !m_pTable )
        return;
  KSpreadAngle* dlg = new KSpreadAngle( this, "Angle" ,QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
  dlg->show();
}

void KSpreadView::consolidate()
{
    KSpreadConsolidate* dlg = new KSpreadConsolidate( this, "Consolidate" );
    dlg->show();
}

void KSpreadView::replace()
{
  KSpreadreplace* dlg = new KSpreadreplace( this, "Replace" ,QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
  dlg->show();
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

void KSpreadView::series()
{
  KSpreadseries* dlg = new KSpreadseries( this, "Series",QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
  dlg->show();

}

void KSpreadView::sort()
{
  QRect selection( m_pTable->selectionRect() );
  if(selection.left()==0)
    {
    KMessageBox::error( this, i18n("One cell was selected!") );
    }
  else if((selection.right()==0x7FFF) ||(selection.bottom()==0x7FFF))
  	{
  	KMessageBox::error( this, i18n("Area too large!"));
	}
   else
        {
        KSpreadsort* dlg = new KSpreadsort( this, "Sort" );
        dlg->show();
        }
}

void KSpreadView::createAnchor()
{
  KSpreadanchor* dlg = new KSpreadanchor( this, "Create Anchor" ,QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
  dlg->show();
}

bool KSpreadView::printDlg()
{
    QPrinter prt;
    if ( QPrintDialog::getPrinterSetup( &prt ) )
    {
	prt.setFullPage( TRUE );
	QPainter painter;
	painter.begin( &prt );
	// Print the table and tell that m_pDoc is NOT embedded.
	m_pTable->print( painter, &prt );
	painter.end();
    }

    return true;
}

void KSpreadView::insertChart( const QRect& _geometry, KoDocumentEntry& _e )
{
    // Transform the view coordinates to document coordinates
    QWMatrix m = matrix().invert();
    QPoint tl = m.map( _geometry.topLeft() );
    QPoint br = m.map( _geometry.bottomRight() );

    // Insert the new child in the active table.
    m_pTable->insertChart( QRect( tl, br ), _e, m_pTable->selectionRect() );
}

void KSpreadView::insertChild( const QRect& _geometry, KoDocumentEntry& _e )
{
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

  // TODO
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
        {
        m_pTable->recalc();
        m_pCanvas->repaint();
        slotUpdateHBorder(activeTable());
        }
}

void KSpreadView::addModifyComment()
{
  if ( !m_pTable )
       return;

  KSpreadcomment* dlg = new  KSpreadcomment( this, "comment",QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
  if(dlg->exec())
        updateEditWidget();
}

void KSpreadView::editCell()
{
    if ( m_pCanvas->editor() )
	return;

    m_pCanvas->createEditor();
}

// ############## Torben: Do we need that ?
void KSpreadView::keyPressEvent ( QKeyEvent* _ev )
{
  // Dont eat accelerators
  if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) )
    QWidget::keyPressEvent( _ev );
  else
    QApplication::sendEvent( m_pCanvas, _ev );
}

int KSpreadView::leftBorder() const
{
  return YBORDER_WIDTH;
}

int KSpreadView::rightBorder() const
{
 return /*20*/ 16;
}

int KSpreadView::topBorder() const
{
 return /*30 + */XBORDER_HEIGHT;
}

int KSpreadView::bottomBorder() const
{
 return /*20*/16;
}

void KSpreadView::resizeEvent( QResizeEvent * )
{
    m_pToolWidget->show();
    m_pToolWidget->setGeometry( 0, 0, width(), 30 );
    int top = 30;
    // m_pPosWidget->setGeometry( 2,2,50,26 );
    // m_pCancelButton->setGeometry( 60, 2, 26, 26 );
    // m_pOkButton->setGeometry( 90, 2, 26, 26 );
    // m_pEditWidget->setGeometry( 125, 2, 200, 26 );
	
    m_pTabBarFirst->setGeometry( 0, height() - 16, 16, 16 );
    m_pTabBarFirst->show();
    m_pTabBarLeft->setGeometry( 16, height() - 16, 16, 16 );
    m_pTabBarLeft->show();
    m_pTabBarRight->setGeometry( 32, height() - 16, 16, 16 );
    m_pTabBarRight->show();
    m_pTabBarLast->setGeometry( 48, height() - 16, 16, 16 );
    m_pTabBarLast->show();
    m_pTabBar->setGeometry( 64, height() - 16, width() / 2 - 64, 16 );
    m_pTabBar->show();

    // David's suggestion: move the scrollbars to KSpreadCanvas, but keep those resize statements
    m_pHorzScrollBar->setGeometry( width() / 2, height() - 16, width() / 2 - 16, 16 );
    m_pHorzScrollBar->setSteps( 20 /*linestep*/, m_pHorzScrollBar->width() /*pagestep*/);
    m_pHorzScrollBar->show();
    m_pVertScrollBar->setGeometry( width() - 16, top , 16, height() - 16 - top );
    m_pVertScrollBar->setSteps( 20 /*linestep*/, m_pVertScrollBar->height() /*pagestep*/);
    m_pVertScrollBar->show();

    m_pFrame->setGeometry( 0, top, width() - 16, height() - 16 - top );
    m_pFrame->show();

    m_pCanvas->setGeometry( YBORDER_WIDTH, XBORDER_HEIGHT,
                            m_pFrame->width() - YBORDER_WIDTH, m_pFrame->height() - XBORDER_HEIGHT );

    m_pHBorderWidget->setGeometry( YBORDER_WIDTH, 0, m_pFrame->width() - YBORDER_WIDTH, XBORDER_HEIGHT );
    m_pHBorderWidget->show();

    m_pVBorderWidget->setGeometry( 0, XBORDER_HEIGHT, YBORDER_WIDTH,
                                   m_pFrame->height() - XBORDER_HEIGHT );
    m_pVBorderWidget->show();
}


void KSpreadView::popupColumnMenu(const QPoint & _point)
{
    assert( m_pTable );

    if ( !koDocument()->isReadWrite() )
      return;

    if (m_pPopupColumn != 0L )
	delete m_pPopupColumn ;

    m_pPopupColumn= new QPopupMenu();

    m_pPopupColumn->insertItem( KSBarIcon("insert_table_col"),i18n("Insert Column"), this, SLOT( slotPopupInsertColumn() ) );
    m_pPopupColumn->insertItem( KSBarIcon("delete_table_col"),i18n("Remove Column"), this, SLOT( slotPopupRemoveColumn() ) );
    m_pPopupColumn->insertItem( KSBarIcon("resizecol"),i18n("Resize..."), this, SLOT( slotPopupResizeColumn() ) );
    m_pPopupColumn->insertItem( i18n("Adjust Column"), this, SLOT(slotPopupAdjustColumn() ) );
    QObject::connect( m_pPopupColumn, SIGNAL(activated( int ) ), this, SLOT(slotActivateTool( int ) ) );

    m_pPopupColumn->popup( _point );
}

void KSpreadView::slotPopupAdjustColumn()
{
    if ( !m_pTable )
       return;
    canvasWidget()->hBorderWidget()->adjustColumn();
}

void KSpreadView::slotPopupResizeColumn()
{
    if ( !m_pTable )
	return;
    KSpreadresize* dlg = new KSpreadresize( this, "Resize column",KSpreadresize::resize_column );
    dlg->show();
}

void KSpreadView::slotPopupInsertColumn()
{
    m_pTable->insertColumn( m_pHBorderWidget->markerColumn() );
    KSpreadTable *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
	tbl->recalc(true);
    QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
    for( ; it.current(); ++it )
	it.current()->changeNameCellRef( QPoint(m_pHBorderWidget->markerColumn(),0),true,KSpreadTable::ColumnInsert, m_pTable->name());

    updateEditWidget();
}

void KSpreadView::slotPopupRemoveColumn()
{
    m_pTable->deleteColumn( m_pHBorderWidget->markerColumn() );
    KSpreadTable *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
	tbl->recalc(true);
    QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
    for( ; it.current(); ++it )
	it.current()->changeNameCellRef( QPoint(m_pHBorderWidget->markerColumn(),0),true,KSpreadTable::ColumnRemove,m_pTable->name());

    updateEditWidget();
}

void KSpreadView::popupRowMenu(const QPoint & _point )
{
    assert( m_pTable );

    if ( !koDocument()->isReadWrite() )
      return;

    if (m_pPopupRow != 0L )
	delete m_pPopupRow ;

    m_pPopupRow= new QPopupMenu();

    m_pPopupRow->insertItem( KSBarIcon("insert_table_row"),i18n("Insert Row"), this, SLOT( slotPopupInsertRow() ) );
    m_pPopupRow->insertItem( KSBarIcon("delete_table_row"),i18n("Remove Row"), this, SLOT( slotPopupRemoveRow() ) );
    m_pPopupRow->insertItem( KSBarIcon("resizerow"),i18n("Resize..."), this, SLOT( slotPopupResizeRow() ) );
    m_pPopupRow->insertItem( i18n("Adjust Row"), this, SLOT( slotPopupAdjustRow() ) );

    QObject::connect( m_pPopupRow, SIGNAL( activated( int ) ), this, SLOT( slotActivateTool( int ) ) );
    m_pPopupRow->popup( _point );
}

void KSpreadView::slotPopupAdjustRow()
{
    if ( !m_pTable )
       return;
    canvasWidget()->vBorderWidget()->adjustRow();
}

void KSpreadView::slotPopupResizeRow()
{
    if ( !m_pTable )
       return;
    KSpreadresize* dlg = new KSpreadresize( this, "Resize row",KSpreadresize::resize_row );
    dlg->show();
}

void KSpreadView::slotPopupInsertRow()
{
    //kdDebug(36001) << "KSpreadView::slotPopupInsertRow()" << endl;
    m_pTable->insertRow( m_pVBorderWidget->markerRow() );

    //kdDebug(36001) << "KSpreadView::slotPopupInsertRow() : recalc all tables" << endl;
    KSpreadTable *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
	tbl->recalc(true);

    //kdDebug(36001) << "KSpreadView::slotPopupInsertRow() : changeNameCellRef" << endl;
    QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
    for( ; it.current(); ++it )
	it.current()->changeNameCellRef( QPoint(0, m_pVBorderWidget->markerRow()),true,KSpreadTable::RowInsert,m_pTable->name());
    //kdDebug(36001) << "KSpreadView::slotPopupInsertRow() : changeNameCellRef done" << endl;

    updateEditWidget();
}

void KSpreadView::slotPopupRemoveRow()
{
    m_pTable->deleteRow( m_pVBorderWidget->markerRow() );

    KSpreadTable *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
	tbl->recalc(true);
    QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
    for( ; it.current(); ++it )
	it.current()->changeNameCellRef( QPoint(0,m_pVBorderWidget->markerRow()),true,KSpreadTable::RowRemove,m_pTable->name());

    updateEditWidget();
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
    m_delete->plug( m_pPopupMenu );
    m_clear->plug( m_pPopupMenu );
    m_adjust->plug( m_pPopupMenu );
    m_default->plug( m_pPopupMenu );
    m_areaName->plug( m_pPopupMenu );
    // If there is no selection
    QRect selection( m_pTable->selectionRect() );
    if(selection.left()==0 && koDocument()->isReadWrite() )
    {
    	m_pPopupMenu->insertSeparator();
        m_insertCell->plug( m_pPopupMenu );
        m_removeCell->plug( m_pPopupMenu );
    }
    KSpreadCell *cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
    m_pPopupMenu->insertSeparator();
    m_addModifyComment->plug( m_pPopupMenu );
    if(!cell->getComment().isEmpty())
        {
        //m_showComment->plug( m_pPopupMenu );
        m_removeComment->plug( m_pPopupMenu );
        }

    // Remove informations about the last tools we offered
    m_lstTools.clear();
    m_lstTools.setAutoDelete( true );

    if ( !cell->isFormular() && !cell->isValue() && !cell->valueString().isEmpty()&& koDocument()->isReadWrite() )
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
  if ( tool->run( entry->command, &text, "QString", "text/plain") )
      cell->setCellText( text, true );

}

void KSpreadView::deleteSelection()
{
    ASSERT( m_pTable );

    m_pTable->deleteSelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );

    updateEditWidget();
}

void KSpreadView::adjust()
{
    QRect r( activeTable()-> selectionRect() );
    if( r.right() ==0x7FFF)
    {
	KMessageBox::error( this, i18n("Area too large!"));
    }
    else if(r.bottom()==0x7FFF)
    {
	KMessageBox::error( this, i18n("Area too large!"));
    }
    else
    {
    canvasWidget()->adjustArea();
    }
}

void KSpreadView::clearSelection()
{
    ASSERT( m_pTable );
    m_pTable->clearSelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );

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
    KSpreadinsert* dlg = new KSpreadinsert( this, "Insert",QPoint(m_pCanvas->markerColumn(), m_pCanvas->markerRow()),KSpreadinsert::Insert );
    dlg->show();
}

void KSpreadView::slotRemove()
{
    KSpreadinsert* dlg = new KSpreadinsert( this, "Remove",QPoint(m_pCanvas->markerColumn(), m_pCanvas->markerRow()),KSpreadinsert::Remove );

    dlg->show();
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
    	{	
	canvasWidget()->equalizeRow();    	
    	}
}

void KSpreadView::equalizeColumn()
{
    QRect selection( activeTable()->selectionRect() );
    if(selection.right()==0x7FFF)
    	KMessageBox::error( this, i18n("Area too large!"));
    else
    	{
	canvasWidget()->equalizeColumn();
    	}
}


void KSpreadView::layoutDlg()
{
  QRect selection( m_pTable->selectionRect() );
  if((selection.right()==0x7FFF) ||(selection.bottom()==0x7FFF))
  	{
  	KMessageBox::error( this, i18n("Area too large!"));
	}
  else
  	{
  	//m_pCanvas->hideMarker();

  	if ( selection.contains( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) ) )
    	   CellLayoutDlg dlg( this, m_pTable, selection.left(), selection.top(),
		       selection.right(), selection.bottom() );
  	else
           CellLayoutDlg dlg( this, m_pTable, m_pCanvas->markerColumn(), m_pCanvas->markerRow(), m_pCanvas->markerColumn(), m_pCanvas->markerRow() );

  	m_pDoc->setModified( true );

   	// Update the toolbar (bold/italic/font...)
  	updateEditWidget();

  	//m_pCanvas->showMarker();
  	}
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
    QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( "'KChart' in ServiceTypes" );
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
        QApplication::beep();
        KMessageBox::sorry( this, i18n("You cannot delete the only table of the map."), i18n("Remove table") ); // FIXME bad english? no english!
        return;
    }
    QApplication::beep();
    int ret = KMessageBox::warningYesNo(this,i18n("You are going to remove the active table.\nDo you want to continue?"),i18n("Remove table"));

    if ( ret == 3 )
    {
        KSpreadTable *tbl = activeTable();
        doc()->map()->removeTable( tbl );
		removeTable(tbl);
        delete tbl;
    }
}



void KSpreadView::setText( const QString& _text )
{
  if ( m_pTable == 0L )
    return;

  m_pTable->setText( m_pCanvas->markerRow(), m_pCanvas->markerColumn(), _text );
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

void KSpreadView::slotChangeSelection( KSpreadTable *_table, const QRect &_old, const QRect &_new )
{
    // qDebug("void KSpreadView::slotChangeSelection( KSpreadTable *_table, const QRect &_old %i %i|%i %i, const QRect &_new %i %i|%i %i )\n",_old.left(),_old.top(),_old.right(),_old.bottom(),_new.left(),_new.top(),_new.right(),_new.bottom());

    // Emit a signal for internal use
    emit sig_selectionChanged( _table, _new );

    if ( _new.left() == 0 && _new.right() == 0 )
	m_tableFormat->setEnabled( FALSE );
    else
	m_tableFormat->setEnabled( TRUE );

    // Send some event around
    KSpreadSelectionChanged ev( _new, activeTable()->name() );
    QApplication::sendEvent( this, &ev );

    // Do we display this table ?
    if ( _table != m_pTable )
	return;

    QRect uni( _old );
    if ( uni.left() == 0 && uni.right() == 0 )
	uni = _new;
    else if ( _new.left() != 0 || _new.right() != 0 )
	uni = uni.unite( _new );

    // ########## Torben: Why redraw? Should not we just invert ?
    m_pCanvas->updateCellRect( uni );
    m_pVBorderWidget->update();
    m_pHBorderWidget->update();

    if ( _old.right() == 0x7fff || _new.right() == 0x7fff )
	m_pVBorderWidget->update();
    else if ( _old.bottom() == 0x7fff || _new.bottom() == 0x7fff )
	m_pHBorderWidget->update();
}

void KSpreadView::slotUnselect( KSpreadTable *_table, const QRect& _old )
{
    // Do we display this table ?
    if ( _table != m_pTable )
	return;

    // qDebug("void KSpreadView::slotUnselect( KSpreadTable *_table, const QRect &_old %i %i|%i %i\n",_old.left(),_old.top(),_old.right(),_old.bottom());

    QRect r( _old.x(), _old.y(), _old.width() + 1, _old.height() + 1 );
    m_pCanvas->updateCellRect( r );

    // Are complete columns selected ?
    if ( _old.bottom() == 0x7FFF )
	m_pHBorderWidget->update();	
    // Are complete rows selected ?
    else if ( _old.right() == 0x7FFF )
	m_pVBorderWidget->update();
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

    KoTransformToolBox* box = 0;
    QObject* obj = topLevelWidget()->child( 0, "KoTransformToolBox" );
    if ( !obj )
    {
	box = new KoTransformToolBox( selectedChild(), topLevelWidget() );
	box->show();

	box->setDocumentChild( selectedChild() );
    }
    else
    {
	box = (KoTransformToolBox*)obj;
	box->show();
	box->raise();
    }
}

void KSpreadView::slotChildSelected( KoDocumentChild* ch )
{
    m_transform->setEnabled( TRUE );

    QObject* obj = topLevelWidget()->child( 0, "KoTransformToolBox" );
    if ( obj )
    {
	KoTransformToolBox* box = (KoTransformToolBox*)obj;
	box->setEnabled( TRUE );
	box->setDocumentChild( ch );
    }
}

void KSpreadView::slotChildUnselected( KoDocumentChild* )
{
    m_transform->setEnabled( FALSE );

    QObject* obj = topLevelWidget()->child( 0, "KoTransformToolBox" );
    if ( obj )
    {
	KoTransformToolBox* box = (KoTransformToolBox*)obj;
	box->setEnabled( FALSE );
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

