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

#include "kword_view.h"
#include "kword_doc.h"
#include "kword_main.h"
#include "kword_view.moc"
#include "kword_shell.h"
#include "frame.h"
#include "clipbrd_dnd.h"
#include "defs.h"
#include "kword_page.h"
#include "paragdia.h"
#include "parag.h"
#include "frame.h"
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

#include <opUIUtils.h>
#include <opMenuIf.h>
#include <opToolBarIf.h>

#include <koPartSelectDia.h>
#include <koMainWindow.h>
#include <koDocument.h>
#include <koRuler.h>
#include <koTabChooser.h>
#include <koPartSelectDia.h>
#include <koUIUtils.h>
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
#include <stdlib.h>
#include <X11/Xlib.h>
#define DEBUG

/******************************************************************/
/* Class: KWordFrame						  */
/******************************************************************/
KWordFrame::KWordFrame( KWordView* _view, KWordChild* _child )
    : KoFrame( _view )
{
    m_pKWordView = _view;
    m_pKWordChild = _child;
}

/******************************************************************/
/* Class: KWordView						  */
/******************************************************************/

/*================================================================*/
KWordView::KWordView( QWidget *_parent, const char *_name, KWordDocument* _doc )
    : QWidget( _parent, _name ), KoViewIf( _doc ), OPViewIf( _doc ), KWord::KWordView_skel(), format( _doc )
{
    shell = 0L;
    setWidget( this );

    KoViewIf::setFocusPolicy( OpenParts::Part::ClickFocus );

    m_pKWordDoc = 0L;
    m_bUnderConstruction = TRUE;
    m_bShowGUI = TRUE;
    m_vMenuTools = 0L;
    m_vToolBarTools = 0L;
    m_vToolBarText = 0L;
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
    tmpBrd.color = white;
    tmpBrd.style = KWParagLayout::SOLID;
    tmpBrd.ptWidth = 0;
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
    m_vToolBarText = 0L;
    m_vToolBarFrame = 0L;
    m_pKWordDoc = _doc;
    oldFramePos = OpenPartsUI::Top;
    oldTextPos = OpenPartsUI::Top;
    backColor = QBrush( white );

    QObject::connect( m_pKWordDoc, SIGNAL( sig_insertObject( KWordChild*, KWPartFrameSet* ) ),
		      this, SLOT( slotInsertObject( KWordChild*, KWPartFrameSet* ) ) );
    QObject::connect( m_pKWordDoc, SIGNAL( sig_updateChildGeometry( KWordChild* ) ),
		      this, SLOT( slotUpdateChildGeometry( KWordChild* ) ) );


    setKeyCompression( TRUE );
    setAcceptDrops( TRUE );
}

/*================================================================*/
void KWordView::init()
{
    /******************************************************
     * Menu
     ******************************************************/

    cerr << "Registering menu as " << id() << endl;

    OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
    if ( !CORBA::is_nil( menu_bar_manager ) )
	menu_bar_manager->registerClient( id(), this );
    else
	cerr << "Did not get a menu bar manager" << endl;

  /******************************************************
   * Toolbar
   ******************************************************/

    OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
    if ( !CORBA::is_nil( tool_bar_manager ) )
	tool_bar_manager->registerClient( id(), this );
    else
	cerr << "Did not get a tool bar manager" << endl;

    // Create GUI
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
KWordView::~KWordView()
{
    cerr << "KWordView::~KWordView()" << endl;
    cleanUp();
    cerr << "...KWordView::~KWordView()" << endl;
}

/*================================================================*/
void KWordView::showFormulaToolbar( bool show )
{
    if ( show )
	m_vToolBarFormula->enable( OpenPartsUI::Show );
    else
	m_vToolBarFormula->enable( OpenPartsUI::Hide );
}

/*================================================================*/
void KWordView::clipboardDataChanged()
{
    if ( kapp->clipboard()->text().isEmpty() ) {
	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Paste, FALSE );
	m_vToolBarEdit->setItemEnabled( ID_EDIT_PASTE, FALSE );
    } else {
	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Paste, TRUE );
	m_vToolBarEdit->setItemEnabled( ID_EDIT_PASTE, TRUE );
    }
}

/*================================================================*/
void KWordView::cleanUp()
{
    cerr << "void KWordView::cleanUp()" << endl;

    if ( m_bIsClean )
	return;

    OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
    if ( !CORBA::is_nil( menu_bar_manager ) )
	menu_bar_manager->unregisterClient( id() );

    OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
    if ( !CORBA::is_nil( tool_bar_manager ) )
	tool_bar_manager->unregisterClient( id() );

    m_pKWordDoc->removeView( this );

    KoViewIf::cleanUp();

    cerr << "... void KWordView::cleanUp()" << endl;
}

/*=========================== file print =======================*/
CORBA::Boolean KWordView::printDlg()
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
    }	break;
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
    if ( _check && _format == format || !m_vToolBarText ) return;

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
	QValueList<QString>::Iterator it = fontList.find( _format.getUserFont()->getFontName().lower() );
	if ( !CORBA::is_nil( m_vToolBarText ) && it != fontList.end() ) {
	    QValueList<QString>::Iterator it2 = fontList.begin();
	    int pos = 0;
	    for ( ; it != it2; ++it2, ++pos );
	    m_vToolBarText->setCurrentComboItem( ID_FONT_LIST, pos );
	}
    }

    if ( _format.getPTFontSize() != -1 )
	if ( !CORBA::is_nil( m_vToolBarText ) )
	    m_vToolBarText->setCurrentComboItem( ID_FONT_SIZE, _format.getPTFontSize() - 4 );

    if ( _format.getWeight() != -1 ) {
	if ( !CORBA::is_nil( m_vToolBarText ) )
	    m_vToolBarText->setButton( ID_BOLD, _format.getWeight() == QFont::Bold );
	tbFont.setBold( _format.getWeight() == QFont::Bold );
    }
    if ( _format.getItalic() != -1 ) {
	if ( !CORBA::is_nil( m_vToolBarText ) )
	    m_vToolBarText->setButton( ID_ITALIC, _format.getItalic() == 1 );
	tbFont.setItalic( _format.getItalic() == 1 );
    }
    if ( _format.getUnderline() != -1 ) {
	if ( !CORBA::is_nil( m_vToolBarText ) )
	    m_vToolBarText->setButton( ID_UNDERLINE, _format.getUnderline() == 1 );
	tbFont.setUnderline( _format.getUnderline() == 1 );
    }

    if ( _format.getColor().isValid() ) {
	if ( !CORBA::is_nil( m_vToolBarText ) ) {
	    OpenPartsUI::Pixmap_var pix =
              KOUIUtils::colorPixmap( _format.getColor(), KOUIUtils::TXT_COLOR );
	    m_vToolBarText->setButtonPixmap( ID_TEXT_COLOR, pix );
	}
	tbColor = QColor( _format.getColor() );
    }

    if ( !CORBA::is_nil( m_vToolBarText ) ) {
	m_vToolBarText->setButton( ID_SUPERSCRIPT, FALSE );
	m_vToolBarText->setButton( ID_SUBSCRIPT, FALSE );
    }

    if ( _format.getVertAlign() == KWFormat::VA_NORMAL )
	vertAlign = KWFormat::VA_NORMAL;
    else if ( _format.getVertAlign() == KWFormat::VA_SUB ) {
	vertAlign = KWFormat::VA_SUB;
	if ( !CORBA::is_nil( m_vToolBarText ) )
	    m_vToolBarText->setButton( ID_SUBSCRIPT, TRUE );
    } else if ( _format.getVertAlign() == KWFormat::VA_SUPER ) {
	vertAlign = KWFormat::VA_SUPER;
	if ( !CORBA::is_nil( m_vToolBarText ) )
	    m_vToolBarText->setButton( ID_SUPERSCRIPT, TRUE );
    }

    format = _format;

    if ( _update_page )
	gui->getPaperWidget()->formatChanged( format, _redraw );
}

/*================================================================*/
void KWordView::setFlow( KWParagLayout::Flow _flow )
{
    if ( _flow != flow && m_vToolBarText ) {
	flow = _flow;
	m_vToolBarText->setButton( ID_ALEFT, FALSE );
	m_vToolBarText->setButton( ID_ACENTER, FALSE );
	m_vToolBarText->setButton( ID_ARIGHT, FALSE );
	m_vToolBarText->setButton( ID_ABLOCK, FALSE );

	switch ( flow ) {
	case KWParagLayout::LEFT:
	    m_vToolBarText->setButton( ID_ALEFT, TRUE );
	    break;
	case KWParagLayout::CENTER:
	    m_vToolBarText->setButton( ID_ACENTER, TRUE );
	    break;
	case KWParagLayout::RIGHT:
	    m_vToolBarText->setButton( ID_ARIGHT, TRUE );
	    break;
	case KWParagLayout::BLOCK:
	    m_vToolBarText->setButton( ID_ABLOCK, TRUE );
	    break;
	}
    }
}

/*================================================================*/
void KWordView::setLineSpacing( int _spc )
{
    if ( _spc != spc && m_vToolBarText ) {
	spc = _spc;
	m_vToolBarText->setCurrentComboItem( ID_LINE_SPC, _spc );
    }
}

/*================================================================*/
void KWordView::setParagBorders( KWParagLayout::Border _left, KWParagLayout::Border _right,
				 KWParagLayout::Border _top, KWParagLayout::Border _bottom )
{
    if ( ( left != _left || right != _right || top != _top || bottom != _bottom ) && m_vToolBarText ) {
	m_vToolBarText->setButton( ID_BRD_LEFT, FALSE );
	m_vToolBarText->setButton( ID_BRD_RIGHT, FALSE );
	m_vToolBarText->setButton( ID_BRD_TOP, FALSE );
	m_vToolBarText->setButton( ID_BRD_BOTTOM, FALSE );

	left = _left;
	right = _right;
	top = _top;
	bottom = _bottom;

	if ( left.ptWidth > 0 ) {
	    m_vToolBarText->setButton( ID_BRD_LEFT, TRUE );
	    tmpBrd = left;
	    setParagBorderValues();
	}
	if ( right.ptWidth > 0 ) {
	    m_vToolBarText->setButton( ID_BRD_RIGHT, TRUE );
	    tmpBrd = right;
	    setParagBorderValues();
	}
	if ( top.ptWidth > 0 ) {
	    m_vToolBarText->setButton( ID_BRD_TOP, TRUE );
	    tmpBrd = top;
	    setParagBorderValues();
	}
	if ( bottom.ptWidth > 0 ) {
	    m_vToolBarText->setButton( ID_BRD_BOTTOM, TRUE );
	    tmpBrd = bottom;
	    setParagBorderValues();
	}
    }
}

/*===============================================================*/
bool KWordView::event( const char* _event, const CORBA::Any& _value )
{
    EVENT_MAPPER( _event, _value );

    MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_ptr, mappingCreateMenubar );
    MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_ptr, mappingCreateToolbar );

    END_EVENT_MAPPER;

    return FALSE;
}

/*===============================================================*/
void KWordView::uncheckAllTools()
{
    if ( m_vMenuTools ) {
	m_vMenuTools->setItemChecked( m_idMenuTools_Edit, FALSE );
	m_vMenuTools->setItemChecked( m_idMenuTools_EditFrame, FALSE );
	m_vMenuTools->setItemChecked( m_idMenuTools_CreateText, FALSE );
	m_vMenuTools->setItemChecked( m_idMenuTools_CreatePix, FALSE );
	m_vMenuTools->setItemChecked( m_idMenuTools_Clipart, FALSE );
	m_vMenuTools->setItemChecked( m_idMenuTools_Table, FALSE );
	m_vMenuTools->setItemChecked( m_idMenuTools_KSpreadTable, FALSE );
	m_vMenuTools->setItemChecked( m_idMenuTools_Formula, FALSE );
	m_vMenuTools->setItemChecked( m_idMenuTools_Part, FALSE );
    }

    if ( m_vToolBarTools ) {
	m_vToolBarTools->setButton( ID_TOOL_EDIT, FALSE );
	m_vToolBarTools->setButton( ID_TOOL_EDIT_FRAME, FALSE );
	m_vToolBarTools->setButton( ID_TOOL_CREATE_TEXT, FALSE );
	m_vToolBarTools->setButton( ID_TOOL_CREATE_PIX, FALSE );
	m_vToolBarTools->setButton( ID_TOOL_CREATE_CLIPART, FALSE );
	m_vToolBarTools->setButton( ID_TOOL_CREATE_TABLE, FALSE );
	m_vToolBarTools->setButton( ID_TOOL_CREATE_KSPREAD_TABLE, FALSE );
	m_vToolBarTools->setButton( ID_TOOL_CREATE_FORMULA, FALSE );
	m_vToolBarTools->setButton( ID_TOOL_CREATE_PART, FALSE );
    }
}

/*===============================================================*/
void KWordView::setTool( MouseMode _mouseMode )
{
    if ( m_vMenuTools ) {
	switch ( _mouseMode ) {
	case MM_EDIT:
	    m_vMenuTools->setItemChecked( m_idMenuTools_Edit, TRUE );
	    break;
	case MM_EDIT_FRAME:
	    m_vMenuTools->setItemChecked( m_idMenuTools_EditFrame, TRUE );
	    break;
	case MM_CREATE_TEXT:
	    m_vMenuTools->setItemChecked( m_idMenuTools_CreateText, TRUE );
	    break;
	case MM_CREATE_PIX:
	    m_vMenuTools->setItemChecked( m_idMenuTools_CreatePix, TRUE );
	    break;
	case MM_CREATE_CLIPART:
	    m_vMenuTools->setItemChecked( m_idMenuTools_Clipart, TRUE );
	    break;
	case MM_CREATE_TABLE:
	    m_vMenuTools->setItemChecked( m_idMenuTools_Table, TRUE );
	    break;
	case MM_CREATE_KSPREAD_TABLE:
	    m_vMenuTools->setItemChecked( m_idMenuTools_KSpreadTable, TRUE );
	    break;
	case MM_CREATE_FORMULA:
	    m_vMenuTools->setItemChecked( m_idMenuTools_Formula, TRUE );
	    break;
	case MM_CREATE_PART:
	    m_vMenuTools->setItemChecked( m_idMenuTools_Part, TRUE );
	    break;
	}
    }

    if ( m_vToolBarTools ) {
	switch ( _mouseMode ) {
	case MM_EDIT:
	    m_vToolBarTools->setButton( ID_TOOL_EDIT, TRUE );
	    break;
	case MM_EDIT_FRAME:
	    m_vToolBarTools->setButton( ID_TOOL_EDIT_FRAME, TRUE );
	    break;
	case MM_CREATE_TEXT:
	    m_vToolBarTools->setButton( ID_TOOL_CREATE_TEXT, TRUE );
	    break;
	case MM_CREATE_PIX:
	    m_vToolBarTools->setButton( ID_TOOL_CREATE_PIX, TRUE );
	    break;
	case MM_CREATE_CLIPART:
	    m_vToolBarTools->setButton( ID_TOOL_CREATE_CLIPART, TRUE );
	    break;
	case MM_CREATE_TABLE:
	    m_vToolBarTools->setButton( ID_TOOL_CREATE_TABLE, TRUE );
	    break;
	case MM_CREATE_KSPREAD_TABLE:
	    m_vToolBarTools->setButton( ID_TOOL_CREATE_KSPREAD_TABLE, TRUE );
	    break;
	case MM_CREATE_FORMULA:
	    m_vToolBarTools->setButton( ID_TOOL_CREATE_FORMULA, TRUE );
	    break;
	case MM_CREATE_PART:
	    m_vToolBarTools->setButton( ID_TOOL_CREATE_PART, TRUE );
	    break;
	}
    }

    if ( m_vToolBarText && m_vToolBarFrame ) {
	if ( _mouseMode == MM_EDIT_FRAME ) {
	    m_vToolBarFrame->setButton( ID_FBRD_LEFT, FALSE );
	    m_vToolBarFrame->setButton( ID_FBRD_RIGHT, FALSE );
	    m_vToolBarFrame->setButton( ID_FBRD_TOP, FALSE );
	    m_vToolBarFrame->setButton( ID_FBRD_BOTTOM, FALSE );

	    m_vToolBarFrame->setBarPos( oldFramePos );
	    m_vToolBarText->enable( OpenPartsUI::Hide );
	    m_vToolBarFrame->enable( OpenPartsUI::Show );
	    oldTextPos = m_vToolBarText->barPos();
	    m_vToolBarText->setBarPos( OpenPartsUI::Floating );
	} else {
	    m_vToolBarText->setBarPos( oldTextPos );
	    m_vToolBarText->enable( OpenPartsUI::Show );
	    m_vToolBarFrame->enable( OpenPartsUI::Hide );
	    if ( m_vToolBarFrame->barPos() != OpenPartsUI::Floating )
		oldFramePos = m_vToolBarFrame->barPos();
	    m_vToolBarFrame->setBarPos( OpenPartsUI::Floating );
	}

	m_vMenuTable->setItemEnabled( m_idMenuTable_InsertRow, FALSE );
	m_vToolBarTable->setItemEnabled( ID_TABLE_INSROW, FALSE );
	m_vMenuTable->setItemEnabled( m_idMenuTable_DeleteRow, FALSE );
	m_vToolBarTable->setItemEnabled( ID_TABLE_DELROW, FALSE );
	m_vMenuTable->setItemEnabled( m_idMenuTable_InsertCol, FALSE );
	m_vToolBarTable->setItemEnabled( ID_TABLE_INSCOL, FALSE );
	m_vMenuTable->setItemEnabled( m_idMenuTable_DeleteCol, FALSE );
	m_vToolBarTable->setItemEnabled( ID_TABLE_DELCOL, FALSE );
	m_vMenuTable->setItemEnabled( m_idMenuTable_JoinCells, FALSE );
	m_vMenuTable->setItemEnabled( m_idMenuTable_SplitCells, FALSE );
	m_vMenuTable->setItemEnabled( m_idMenuTable_UngroupTable, FALSE );

	switch ( _mouseMode ) {
	case MM_EDIT: {
	    m_vMenuTable->setItemEnabled( m_idMenuTable_InsertRow, TRUE );
	    m_vToolBarTable->setItemEnabled( ID_TABLE_INSROW, TRUE );
	    m_vMenuTable->setItemEnabled( m_idMenuTable_DeleteRow, TRUE );
	    m_vToolBarTable->setItemEnabled( ID_TABLE_DELROW, TRUE );
	    m_vMenuTable->setItemEnabled( m_idMenuTable_InsertCol, TRUE );
	    m_vToolBarTable->setItemEnabled( ID_TABLE_INSCOL, TRUE );
	    m_vMenuTable->setItemEnabled( m_idMenuTable_DeleteCol, TRUE );
	    m_vToolBarTable->setItemEnabled( ID_TABLE_DELCOL, TRUE );
	    m_vMenuTable->setItemEnabled( m_idMenuTable_UngroupTable, TRUE );
	} break;
	case MM_EDIT_FRAME: {
	    m_vMenuTable->setItemEnabled( m_idMenuTable_JoinCells, TRUE );
	    m_vMenuTable->setItemEnabled( m_idMenuTable_SplitCells, TRUE );
	} break;
	default: break;
	}
    }
}

/*===============================================================*/
void KWordView::updateStyle( QString _styleName, bool _updateFormat )
{
    styleList.find( _styleName );

    if ( styleList.at() == -1 ) 
	return;

    if ( !CORBA::is_nil( m_vToolBarText ) )
	m_vToolBarText->setCurrentComboItem( ID_STYLE_LIST, styleList.at() );

    m_vToolBarText->setButton( ID_USORT_LIST, FALSE );
    m_vToolBarText->setButton( ID_ENUM_LIST, FALSE );

    if ( _styleName == "Enumerated List" )
	m_vToolBarText->setButton( ID_ENUM_LIST, TRUE );

    if ( _styleName == "Bullet List" )
	m_vToolBarText->setButton( ID_USORT_LIST, TRUE );

    setFormat( m_pKWordDoc->findParagLayout( _styleName )->getFormat(), FALSE, _updateFormat, FALSE );

    gui->getHorzRuler()->setTabList( m_pKWordDoc->findParagLayout( _styleName )->getTabList() );
}

/*===============================================================*/
void KWordView::updateStyleList()
{
    m_vToolBarText->clearCombo( ID_STYLE_LIST );

    OpenPartsUI::StrList stylelist;
    styleList.clear();
    stylelist.length( m_pKWordDoc->paragLayoutList.count() );
    for ( unsigned int i = 0; i < m_pKWordDoc->paragLayoutList.count(); i++ ) {
	styleList.append( QString( m_pKWordDoc->paragLayoutList.at( i )->getName() ) );
	stylelist[ i ] = CORBA::string_dup( m_pKWordDoc->paragLayoutList.at( i )->getName() );
    }
    m_vToolBarText->insertComboList( ID_STYLE_LIST, stylelist, 0 );
    updateStyle( gui->getPaperWidget()->getParagLayout()->getName() );
}

/*===============================================================*/
void KWordView::editUndo()
{
//     m_pKWordDoc->undo();
//     gui->getPaperWidget()->recalcWholeText( TRUE );
    if ( gui->getPaperWidget()->formulaIsActive() )
	gui->getPaperWidget()->insertFormulaChar( UNDO_CHAR );
}

/*===============================================================*/
void KWordView::editRedo()
{
//     m_pKWordDoc->redo();
//     gui->getPaperWidget()->recalcWholeText( TRUE );
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
    shell->setDocument( m_pKWordDoc );
}

/*===============================================================*/
void KWordView::viewFormattingChars()
{
    m_vMenuView->setItemChecked( m_idMenuView_FormattingChars,
				 !m_vMenuView->isItemChecked( m_idMenuView_FormattingChars ) );
    _viewFormattingChars = m_vMenuView->isItemChecked( m_idMenuView_FormattingChars );
    gui->getPaperWidget()->repaintScreen( !_viewFormattingChars );
}

/*===============================================================*/
void KWordView::viewFrameBorders()
{
    m_vMenuView->setItemChecked( m_idMenuView_FrameBorders, !m_vMenuView->isItemChecked( m_idMenuView_FrameBorders ) );
    _viewFrameBorders = m_vMenuView->isItemChecked( m_idMenuView_FrameBorders );
    gui->getPaperWidget()->repaintScreen( FALSE );
}

/*===============================================================*/
void KWordView::viewTableGrid()
{
    m_vMenuView->setItemChecked( m_idMenuView_TableGrid, !m_vMenuView->isItemChecked( m_idMenuView_TableGrid ) );
    _viewTableGrid = m_vMenuView->isItemChecked( m_idMenuView_TableGrid );
    gui->getPaperWidget()->repaintScreen( !_viewTableGrid );
}

/*===============================================================*/
void KWordView::viewHeader()
{
    m_vMenuView->setItemChecked( m_idMenuView_Header, !m_vMenuView->isItemChecked( m_idMenuView_Header ) );
    m_pKWordDoc->setHeader( m_vMenuView->isItemChecked( m_idMenuView_Header ) );
}

/*===============================================================*/
void KWordView::viewFooter()
{
    m_vMenuView->setItemChecked( m_idMenuView_Footer, !m_vMenuView->isItemChecked( m_idMenuView_Footer ) );
    m_pKWordDoc->setFooter( m_vMenuView->isItemChecked( m_idMenuView_Footer ) );
}

/*===============================================================*/
void KWordView::viewFootNotes()
{
    m_vMenuEdit->setItemChecked( m_idMenuView_FootNotes, TRUE );
    m_vMenuEdit->setItemChecked( m_idMenuView_EndNotes, FALSE );

    m_pKWordDoc->setNoteType( KWFootNoteManager::FootNotes );
}

/*===============================================================*/
void KWordView::viewEndNotes()
{
    m_vMenuEdit->setItemChecked( m_idMenuView_FootNotes, FALSE );
    m_vMenuEdit->setItemChecked( m_idMenuView_EndNotes, TRUE );

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
    else
    {
	KWFootNoteDia dia( 0L, "", m_pKWordDoc, gui->getPaperWidget(), start );
	dia.setCaption( i18n( "Insert Footnote/Endnote" ) );
	dia.show();
    }
}

/*===============================================================*/
void KWordView::formatFont()
{
}

/*===============================================================*/
void KWordView::formatColor()
{
}

/*===============================================================*/
void KWordView::formatParagraph()
{
    if ( paragDia )
    {
	QObject::disconnect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
	paragDia->close();
	delete paragDia;
	paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_SPACING | KWParagDia::PD_FLOW | KWParagDia::PD_BORDERS |
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
void KWordView::formatNumbering()
{
}

/*===============================================================*/
void KWordView::formatStyle()
{
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
    if ( styleManager )
    {
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
    gui->getPaperWidget()->mmEdit();
}

/*===============================================================*/
void KWordView::toolsEditFrame()
{
    gui->getPaperWidget()->mmEditFrame();
}

/*===============================================================*/
void KWordView::toolsCreateText()
{
    gui->getPaperWidget()->mmCreateText();
}

/*===============================================================*/
void KWordView::toolsCreatePix()
{
    gui->getPaperWidget()->mmEdit();
    QString file = KFilePreviewDialog::getOpenFileName( QString::null,
							KImageIO::pattern(KImageIO::Reading), 0);

    if ( !file.isEmpty() )
    {
	gui->getPaperWidget()->mmCreatePix();
	gui->getPaperWidget()->setPixmapFilename( file );
    }
    else
	gui->getPaperWidget()->mmEdit();
}

/*===============================================================*/
void KWordView::toolsClipart()
{
    gui->getPaperWidget()->mmClipart();
}

/*===============================================================*/
void KWordView::toolsTable()
{
    if ( tableDia )
    {
	tableDia->close();
	delete tableDia;
	tableDia = 0L;
    }

    tableDia = new KWTableDia( this, "", gui->getPaperWidget(), m_pKWordDoc, 7, 5 );
    tableDia->setCaption( i18n( "KWord - Insert Table" ) );
    tableDia->show();
}

/*===============================================================*/
void KWordView::toolsKSpreadTable()
{
    gui->getPaperWidget()->mmKSpreadTable();

    QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( "'IDL:KSpread/DocumentFactory:1.0#KSpread' in RepoIds", 1 );
    if ( vec.isEmpty() )
    {
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
    else
    {
	if ( grpMgr->getRows() == 1 )
	    QMessageBox::critical( this, i18n( "Error" ),
				   i18n( "The table has only one row. You can't delete the last one!" ), i18n( "OK" ) );
	else
	{
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
    else
    {
	if ( grpMgr->getCols() == 1 )
	    QMessageBox::critical( this, i18n( "Error" ),
				   i18n( "The table has only one column. You can't delete the last one!" ), i18n( "OK" ) );
	else
	{
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
    else
    {
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
    else
    {
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
    else
    {
	if ( QMessageBox::warning( this, i18n( "Warning" ), i18n( "Ungrouping a table is an irrevesible action!\n"
								  "Do you really want to do that?" ), i18n( "Yes" ),
				   i18n( "No" ) ) == 0 )
	{
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
void KWordView::textStyleSelected( const CORBA::WChar *_style )
{
    QString style = C2Q( _style );
    gui->getPaperWidget()->applyStyle( style );
    format = m_pKWordDoc->findParagLayout( style )->getFormat();
    gui->getPaperWidget()->formatChanged( format, FALSE );
    updateStyle( style, FALSE );
}

/*======================= text size selected  ===================*/
void KWordView::textSizeSelected( const CORBA::WChar *_size )
{
    QString size = C2Q( _size );
    tbFont.setPointSize( size.toInt() );
    format.setPTFontSize( size.toInt() );
    gui->getPaperWidget()->formatChanged( format );
}

/*======================= text font selected  ===================*/
void KWordView::textFontSelected( const CORBA::WChar *_font )
{
    QString font = C2Q( _font );
    tbFont.setFamily( font );
    format.setUserFont( m_pKWordDoc->findUserFont( font ) );
    gui->getPaperWidget()->formatChanged( format );
}

/*========================= text bold ===========================*/
void KWordView::textBold()
{
    tbFont.setBold( !tbFont.bold() );
    format.setWeight( tbFont.bold() ? QFont::Bold : QFont::Normal );
    gui->getPaperWidget()->formatChanged( format );
}

/*========================== text italic ========================*/
void KWordView::textItalic()
{
    tbFont.setItalic( !tbFont.italic() );
    format.setItalic( tbFont.italic() ? 1 : 0 );
    gui->getPaperWidget()->formatChanged( format );
}

/*======================== text underline =======================*/
void KWordView::textUnderline()
{
    tbFont.setUnderline( !tbFont.underline() );
    format.setUnderline( tbFont.underline() ? 1 : 0 );
    gui->getPaperWidget()->formatChanged( format );
}

/*=========================== text color ========================*/
void KWordView::textColor()
{
    if ( KColorDialog::getColor( tbColor ) )
    {
	OpenPartsUI::Pixmap_var pix = KOUIUtils::colorPixmap( tbColor, KOUIUtils::TXT_COLOR );
	m_vToolBarText->setButtonPixmap( ID_TEXT_COLOR, pix );
	format.setColor( tbColor );
	gui->getPaperWidget()->formatChanged( format );
    }
}

/*======================= text align left =======================*/
void KWordView::textAlignLeft()
{
    flow = KWParagLayout::LEFT;
    m_vToolBarText->setButton( ID_ALEFT, TRUE );
    m_vToolBarText->setButton( ID_ACENTER, FALSE );
    m_vToolBarText->setButton( ID_ARIGHT, FALSE );
    m_vToolBarText->setButton( ID_ABLOCK, FALSE );
    gui->getPaperWidget()->setFlow( KWParagLayout::LEFT );
}

/*======================= text align center =====================*/
void KWordView::textAlignCenter()
{
    flow = KWParagLayout::CENTER;
    m_vToolBarText->setButton( ID_ALEFT, FALSE );
    m_vToolBarText->setButton( ID_ACENTER, TRUE );
    m_vToolBarText->setButton( ID_ARIGHT, FALSE );
    m_vToolBarText->setButton( ID_ABLOCK, FALSE );
    gui->getPaperWidget()->setFlow( KWParagLayout::CENTER );
}

/*======================= text align right ======================*/
void KWordView::textAlignRight()
{
    flow = KWParagLayout::RIGHT;
    m_vToolBarText->setButton( ID_ALEFT, FALSE );
    m_vToolBarText->setButton( ID_ACENTER, FALSE );
    m_vToolBarText->setButton( ID_ARIGHT, TRUE );
    m_vToolBarText->setButton( ID_ABLOCK, FALSE );
    gui->getPaperWidget()->setFlow( KWParagLayout::RIGHT );
}

/*======================= text align block ======================*/
void KWordView::textAlignBlock()
{
    flow = KWParagLayout::BLOCK;
    m_vToolBarText->setButton( ID_ARIGHT, FALSE );
    m_vToolBarText->setButton( ID_ACENTER, FALSE );
    m_vToolBarText->setButton( ID_ALEFT, FALSE );
    m_vToolBarText->setButton( ID_ABLOCK, TRUE );
    gui->getPaperWidget()->setFlow( KWParagLayout::BLOCK );
}

/*===============================================================*/
void KWordView::textLineSpacing( const CORBA::WChar *spc )
{
    KWUnit u;
    u.setPT( C2Q( spc ).toInt() );
    gui->getPaperWidget()->setLineSpacing( u );
}

/*====================== enumerated list ========================*/
void KWordView::textEnumList()
{
    m_vToolBarText->setButton( ID_USORT_LIST, FALSE );
    if ( m_vToolBarText->isButtonOn( ID_ENUM_LIST ) )
	gui->getPaperWidget()->setEnumList();
    else
	gui->getPaperWidget()->setNormalText();
}

/*====================== unsorted list ==========================*/
void KWordView::textUnsortList()
{
    m_vToolBarText->setButton( ID_ENUM_LIST, FALSE );
    if ( m_vToolBarText->isButtonOn( ID_USORT_LIST ) )
	gui->getPaperWidget()->setBulletList();
    else
	gui->getPaperWidget()->setNormalText();
}

/*===============================================================*/
void KWordView::textSuperScript()
{
    m_vToolBarText->setButton( ID_SUBSCRIPT, FALSE );
    if ( m_vToolBarText->isButtonOn( ID_SUPERSCRIPT ) )
	vertAlign = KWFormat::VA_SUPER;
    else
	vertAlign = KWFormat::VA_NORMAL;
    format.setVertAlign( vertAlign );
    gui->getPaperWidget()->formatChanged( format );
}

/*===============================================================*/
void KWordView::textSubScript()
{
    m_vToolBarText->setButton( ID_SUPERSCRIPT, FALSE );
    if ( m_vToolBarText->isButtonOn( ID_SUBSCRIPT ) )
	vertAlign = KWFormat::VA_SUB;
    else
	vertAlign = KWFormat::VA_NORMAL;
    format.setVertAlign( vertAlign );
    gui->getPaperWidget()->formatChanged( format );
}

/*===============================================================*/
void KWordView::textBorderLeft()
{
    if ( m_vToolBarText->isButtonOn( ID_BRD_LEFT ) )
	left = tmpBrd;
    else
	left.ptWidth = 0;

    gui->getPaperWidget()->setParagLeftBorder( left );
}

/*===============================================================*/
void KWordView::textBorderRight()
{
    if ( m_vToolBarText->isButtonOn( ID_BRD_RIGHT ) )
	right = tmpBrd;
    else
	right.ptWidth = 0;

    gui->getPaperWidget()->setParagRightBorder( right );
}

/*===============================================================*/
void KWordView::textBorderTop()
{
    if ( m_vToolBarText->isButtonOn( ID_BRD_TOP ) )
	top = tmpBrd;
    else
	top.ptWidth = 0;

    gui->getPaperWidget()->setParagTopBorder( top );
}

/*===============================================================*/
void KWordView::textBorderBottom()
{
    if ( m_vToolBarText->isButtonOn( ID_BRD_BOTTOM ) )
	bottom = tmpBrd;
    else
	bottom.ptWidth = 0;

    gui->getPaperWidget()->setParagBottomBorder( bottom );
}

/*================================================================*/
void KWordView::textBorderColor()
{
    if ( KColorDialog::getColor( tmpBrd.color ) )
    {
	OpenPartsUI::Pixmap_var pix =
          KOUIUtils::colorPixmap( tmpBrd.color, KOUIUtils::FRAME_COLOR );
	m_vToolBarText->setButtonPixmap( ID_BORDER_COLOR, pix );
    }
}

/*================================================================*/
void KWordView::textBorderWidth( const CORBA::WChar *width )
{
    tmpBrd.ptWidth = C2Q( width ).toInt();
}

/*================================================================*/
void KWordView::textBorderStyle( const CORBA::WChar *style )
{
    QString stl = C2Q( style );

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
    gui->getPaperWidget()->setLeftFrameBorder( frmBrd, m_vToolBarFrame->isButtonOn( ID_FBRD_LEFT ) );
}

/*================================================================*/
void KWordView::frameBorderRight()
{
    gui->getPaperWidget()->setRightFrameBorder( frmBrd, m_vToolBarFrame->isButtonOn( ID_FBRD_RIGHT ) );
}

/*================================================================*/
void KWordView::frameBorderTop()
{
    gui->getPaperWidget()->setTopFrameBorder( frmBrd, m_vToolBarFrame->isButtonOn( ID_FBRD_TOP ) );
}

/*================================================================*/
void KWordView::frameBorderBottom()
{
    gui->getPaperWidget()->setBottomFrameBorder( frmBrd, m_vToolBarFrame->isButtonOn( ID_FBRD_BOTTOM ) );
}

/*================================================================*/
void KWordView::frameBorderColor()
{
    if ( KColorDialog::getColor( frmBrd.color ) )
    {
	OpenPartsUI::Pixmap_var pix =
          KOUIUtils::colorPixmap( frmBrd.color, KOUIUtils::FRAME_COLOR );
	m_vToolBarFrame->setButtonPixmap( ID_FBORDER_COLOR, pix );
    }
}

/*================================================================*/
void KWordView::frameBorderWidth( const CORBA::WChar *width )
{
    frmBrd.ptWidth = C2Q( width ).toInt();
}

/*================================================================*/
void KWordView::frameBorderStyle( const CORBA::WChar *style )
{
    QString stl = C2Q( style );

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
	OpenPartsUI::Pixmap_var pix =
          KOUIUtils::colorPixmap( backColor.color(), KOUIUtils::BACK_COLOR );
	m_vToolBarFrame->setButtonPixmap( ID_FBACK_COLOR, pix );
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
    QApplication::sendEvent( gui->getPaperWidget(), e );
}

/*================================================================*/
void KWordView::mouseMoveEvent( QMouseEvent *e )
{
    QApplication::sendEvent( gui->getPaperWidget(), e );
}

/*================================================================*/
void KWordView::mouseReleaseEvent( QMouseEvent *e )
{
    QApplication::sendEvent( gui->getPaperWidget(), e );
}

/*================================================================*/
void KWordView::focusInEvent( QFocusEvent *e )
{
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
bool KWordView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
    if ( CORBA::is_nil( _menubar ) ) {
	m_vMenuEdit = 0L;
	m_vMenuView = 0L;
	m_vMenuInsert = 0L;
	m_vMenuFormat = 0L;
	m_vMenuExtra = 0L;
	m_vMenuHelp = 0L;
	return TRUE;
    }

    CORBA::WString_var text;

    // edit menu
    text = Q2C( i18n( "&Edit" ) );
    _menubar->insertMenu( text, m_vMenuEdit, -1, -1 );

    OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( BarIcon( "undo" ) );
    text = Q2C( i18n( "No Undo possible" ) );
    m_idMenuEdit_Undo = m_vMenuEdit->insertItem6( pix, text, this, "editUndo", CTRL + Key_Z, -1, -1 );
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, FALSE );

    pix = OPUIUtils::convertPixmap( BarIcon( "redo" ) );
    text = Q2C( i18n( "No Redo possible" ) );
    m_idMenuEdit_Redo = m_vMenuEdit->insertItem6( pix, text, this, "editRedo", 0, -1, -1 );
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, FALSE );
    m_vMenuEdit->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "editcut" ) );
    text = Q2C( i18n( "&Cut" ) );
    m_idMenuEdit_Cut = m_vMenuEdit->insertItem6( pix, text, this, "editCut", CTRL + Key_X, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "editcopy" ) );
    text = Q2C( i18n( "&Copy" ) );
    m_idMenuEdit_Copy = m_vMenuEdit->insertItem6( pix, text, this, "editCopy", CTRL + Key_C, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "editpaste" ) );
    text = Q2C( i18n( "&Paste" ) );
    m_idMenuEdit_Paste = m_vMenuEdit->insertItem6( pix, text, this, "editPaste", CTRL + Key_V, -1, -1 );

    m_vMenuEdit->insertSeparator( -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "kwsearch" ) );
    text = Q2C( i18n( "&Find and Replace..." ) );
    m_idMenuEdit_Find = m_vMenuEdit->insertItem6( pix, text, this, "editFind", CTRL + Key_F, -1, -1 );

    m_vMenuEdit->insertSeparator( -1 );
    text = Q2C( i18n( "&Select all" ) );
    m_idMenuEdit_SelectAll = m_vMenuEdit->insertItem4( text, this, "editSelectAll", 0, -1, -1 );

    m_vMenuEdit->insertSeparator( -1 );
    text = Q2C( i18n( "&Delete Frame" ) );
    m_idMenuEdit_DeleteFrame = m_vMenuEdit->insertItem4( text, this, "editDeleteFrame", 0, -1, -1 );

    text = Q2C( i18n( "&Reconnect Frame..." ) );
    m_idMenuEdit_ReconnectFrame = m_vMenuEdit->insertItem4( text, this, "editReconnectFrame", 0, -1, -1 );

    m_vMenuEdit->insertSeparator( -1 );

    text = Q2C( i18n( "&Custom Variables..." ) );
    m_idMenuEdit_CustomVars = m_vMenuEdit->insertItem4( text, this, "editCustomVars", 0, -1, -1 );

    m_vMenuEdit->insertSeparator( -1 );

    text = Q2C( i18n( "Serial &Letter Database..." ) );
    m_idMenuEdit_SerialLetterDataBase = m_vMenuEdit->insertItem4( text, this,
								  "editSerialLetterDataBase", 0, -1, -1 );

    // View
    text = Q2C( i18n( "&View" ) );
    _menubar->insertMenu( text, m_vMenuView, -1, -1 );

    text = Q2C( i18n( "&New View" ) );
    m_idMenuView_NewView = m_vMenuView->insertItem4( text, this, "newView", 0, -1, -1 );
    m_vMenuView->insertSeparator( -1 );
    text = Q2C( i18n( "&Formatting Chars" ) );
    m_idMenuView_FormattingChars = m_vMenuView->insertItem4( text, this, "viewFormattingChars", 0, -1, -1 );
    text = Q2C( i18n( "Frame &Borders" ) );
    m_idMenuView_FrameBorders = m_vMenuView->insertItem4( text, this, "viewFrameBorders", 0, -1, -1 );
    text = Q2C( i18n( "Table &Grid" ) );
    m_idMenuView_TableGrid = m_vMenuView->insertItem4( text, this, "viewTableGrid", 0, -1, -1 );
    m_vMenuView->insertSeparator( -1 );
    text = Q2C( i18n( "&Header" ) );
    m_idMenuView_Header = m_vMenuView->insertItem4( text, this, "viewHeader", 0, -1, -1 );
    text = Q2C( i18n( "F&ooter" ) );
    m_idMenuView_Footer = m_vMenuView->insertItem4( text, this, "viewFooter", 0, -1, -1 );
    m_vMenuView->insertSeparator( -1 );
    text = Q2C( i18n( "&Footnotes" ) );
    m_idMenuView_FootNotes = m_vMenuView->insertItem4( text, this, "viewFootNotes", 0, -1, -1 );
    text = Q2C( i18n( "&Endnotes" ) );
    m_idMenuView_EndNotes = m_vMenuView->insertItem4( text, this, "viewEndNotes", 0, -1, -1 );

    m_vMenuView->setCheckable( TRUE );
    m_vMenuView->setItemChecked( m_idMenuView_FrameBorders, TRUE );
    m_vMenuView->setItemChecked( m_idMenuView_TableGrid, TRUE );
    m_vMenuView->setItemChecked( m_idMenuView_Header, m_pKWordDoc->hasHeader() );
    m_vMenuView->setItemChecked( m_idMenuView_Footer, m_pKWordDoc->hasFooter() );
    m_vMenuView->setItemChecked( m_idMenuView_FootNotes, FALSE );
    m_vMenuView->setItemChecked( m_idMenuView_EndNotes, TRUE );

    // insert menu
    text = Q2C( i18n( "&Insert" ) );
    _menubar->insertMenu( text, m_vMenuInsert, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "picture" ) );
    text = Q2C( i18n( "&Picture..." ) );
    m_idMenuInsert_Picture = m_vMenuInsert->insertItem6( pix, text, this, "insertPicture", Key_F2, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "clipart" ) );
    text = Q2C( i18n( "&Clipart..." ) );
    m_idMenuInsert_Clipart = m_vMenuInsert->insertItem6( pix, text, this, "insertClipart", Key_F3, -1, -1 );

    m_vMenuInsert->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "char" ) );
    text = Q2C( i18n( "&Special Character..." ) );
    m_idMenuInsert_SpecialChar = m_vMenuInsert->insertItem6( pix, text, this,
							     "insertSpecialChar", ALT + Key_C, -1, -1 );
    m_vMenuInsert->insertSeparator( -1 );
    text = Q2C( i18n( "&Hard frame break" ) );
    m_idMenuInsert_FrameBreak = m_vMenuInsert->insertItem4( text, this, "insertFrameBreak", 0, -1, -1 );

    m_vMenuInsert->insertSeparator( -1 );

    text = Q2C( i18n( "&Variable" ) );
    m_vMenuInsert->insertItem8( text, m_vMenuInsert_Variable, -1, -1 );

    m_vMenuInsert->insertSeparator( -1 );

    text = Q2C( i18n( "&Footnote or Endnote..." ) );
    m_idMenuInsert_FootNoteEndNote = m_vMenuInsert->insertItem4( text, this, "insertFootNoteEndNote", 0, -1, -1 );

    text = Q2C( i18n( "Date ( fix )" ) );
    m_idMenuInsert_VariableDateFix = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableDateFix", 0, -1, -1 );
    text = Q2C( i18n( "Date ( variable )" ) );
    m_idMenuInsert_VariableDateVar = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableDateVar", 0, -1, -1 );
    text = Q2C( i18n( "Time ( fix )" ) );
    m_idMenuInsert_VariableTimeFix = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableTimeFix", 0, -1, -1 );
    text = Q2C( i18n( "Time ( variable )" ) );
    m_idMenuInsert_VariableTimeVar = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableTimeVar", 0, -1, -1 );
    text = Q2C( i18n( "Page Number" ) );
    m_idMenuInsert_VariablePageNum = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariablePageNum", 0, -1, -1 );
    m_vMenuInsert_Variable->insertSeparator( -1 );
    text = Q2C( i18n( "Custom..." ) );
    m_idMenuInsert_VariableCustom = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableCustom", 0, -1, -1 );
    m_vMenuInsert_Variable->insertSeparator( -1 );
    text = Q2C( i18n( "Serial Letter..." ) );
    m_idMenuInsert_VariableSerialLetter = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableSerialLetter",
									       0, -1, -1 );

    // tools menu
    text = Q2C( i18n( "&Tools" ) );
    _menubar->insertMenu( text, m_vMenuTools, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "edittool" ) );
    text = Q2C( i18n( "&Edit Text" ) );
    m_idMenuTools_Edit = m_vMenuTools->insertItem6( pix, text, this, "toolsEdit", Key_F4, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "editframetool" ) );
    text = Q2C( i18n( "&Edit Frames" ) );
    m_idMenuTools_EditFrame = m_vMenuTools->insertItem6( pix, text, this, "toolsEditFrame", Key_F5, -1, -1 );

    m_vMenuTools->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "textframetool" ) );
    text = Q2C( i18n( "&Create Text Frame" ) );
    m_idMenuTools_CreateText = m_vMenuTools->insertItem6( pix, text, this, "toolsCreateText", Key_F6, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "picframetool" ) );
    text = Q2C( i18n( "&Create Picture Frame" ) );
    m_idMenuTools_CreatePix = m_vMenuTools->insertItem6( pix, text, this, "toolsCreatePix", Key_F7, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "clipart" ) );
    text = Q2C( i18n( "&Create Clipart Frame" ) );
    m_idMenuTools_Clipart = m_vMenuTools->insertItem6( pix, text, this, "toolsClipart", Key_F8, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "table" ) );
    text = Q2C( i18n( "&Create Table Frame" ) );
    m_idMenuTools_Table = m_vMenuTools->insertItem6( pix, text, this, "toolsTable", Key_F9, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "table" ) );
    text = Q2C( i18n( "&Create KSpread Table Frame" ) );
    m_idMenuTools_KSpreadTable = m_vMenuTools->insertItem6( pix, text, this, "toolsKSpreadTable", Key_F10, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "formula" ) );
    text = Q2C( i18n( "&Create Formula Frame" ) );
    m_idMenuTools_Formula = m_vMenuTools->insertItem6( pix, text, this, "toolsFormula", Key_F11, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "parts" ) );
    text = Q2C( i18n( "&Create Part Frame" ) );
    m_idMenuTools_Part = m_vMenuTools->insertItem6( pix, text, this, "toolsPart", Key_F12, -1, -1 );

    m_vMenuTools->setCheckable( TRUE );
    m_vMenuTools->setItemChecked( m_idMenuTools_Edit, TRUE );

    // format menu
    text = Q2C( i18n( "&Format" ) );
    _menubar->insertMenu( text, m_vMenuFormat, -1, -1 );

    text = Q2C( i18n( "&Font..." ) );
    m_idMenuFormat_Font = m_vMenuFormat->insertItem4( text, this, "formatFont", ALT + Key_F, -1, -1 );
    text = Q2C( i18n( "&Color..." ) );
    m_idMenuFormat_Color = m_vMenuFormat->insertItem4( text, this, "formatColor", ALT + Key_C, -1, -1 );
    text = Q2C( i18n( "Paragraph..." ) );
    m_idMenuFormat_Paragraph = m_vMenuFormat->insertItem4( text, this, "formatParagraph", ALT + Key_Q, -1, -1 );
    text = Q2C( i18n( "Frame/Frameset..." ) );
    m_idMenuFormat_FrameSet = m_vMenuFormat->insertItem4( text, this, "formatFrameSet", 0, -1, -1 );
    text = Q2C( i18n( "Page..." ) );
    m_idMenuFormat_Page = m_vMenuFormat->insertItem4( text, this, "formatPage", ALT + Key_P, -1, -1 );

    m_vMenuFormat->insertSeparator( -1 );

    text = Q2C( i18n( "&Style..." ) );
    m_idMenuFormat_Style = m_vMenuFormat->insertItem4( text, this, "formatStyle", ALT + Key_A, -1, -1 );

    // table menu
    text = Q2C( i18n( "&Table" ) );
    _menubar->insertMenu( text, m_vMenuTable, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "rowin" ) );
    text = Q2C( i18n( "&Insert Row..." ) );
    m_idMenuTable_InsertRow = m_vMenuTable->insertItem6( pix, text, this, "tableInsertRow", 0, -1, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "colin" ) );
    text = Q2C( i18n( "I&nsert Column..." ) );
    m_idMenuTable_InsertCol = m_vMenuTable->insertItem6( pix, text, this, "tableInsertCol", 0, -1, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "rowout" ) );
    text = Q2C( i18n( "&Delete Row..." ) );
    m_idMenuTable_DeleteRow = m_vMenuTable->insertItem6( pix, text, this, "tableDeleteRow", 0, -1, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "colout" ) );
    text = Q2C( i18n( "&Delete Column..." ) );
    m_idMenuTable_DeleteCol = m_vMenuTable->insertItem6( pix, text, this, "tableDeleteCol", 0, -1, -1 );

    m_vMenuTable->insertSeparator( -1 );

    text = Q2C( i18n( "&Join Cells" ) );
    m_idMenuTable_JoinCells = m_vMenuTable->insertItem4( text, this, "tableJoinCells", 0, -1, -1 );
    text = Q2C( i18n( "&Split Cells" ) );
    m_idMenuTable_SplitCells = m_vMenuTable->insertItem4( text, this, "tableSplitCells", 0, -1, -1 );
    text = Q2C( i18n( "&Ungroup Table" ) );
    m_idMenuTable_UngroupTable = m_vMenuTable->insertItem4( text, this, "tableUngroupTable", 0, -1, -1 );

    m_vMenuTable->insertSeparator( -1 );

    text = Q2C( i18n( "&Delete Table" ) );
    m_idMenuTable_Delete = m_vMenuTable->insertItem4( text, this, "tableDelete", 0, -1, -1 );

    // extra menu
    text = Q2C( i18n( "&Extra" ) );
    _menubar->insertMenu( text, m_vMenuExtra, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "spellcheck" ) );
    text = Q2C( i18n( "&Spell Cheking..." ) );
    m_idMenuExtra_Spelling = m_vMenuExtra->insertItem6( pix, text, this, "extraSpelling", ALT + Key_C, -1, -1 );
    text = Q2C( i18n( "&Autocorrection..." ) );
    m_idMenuExtra_AutoFormat = m_vMenuExtra->insertItem4( text, this, "extraAutoFormat", 0, -1, -1 );
    text = Q2C( i18n( "&Stylist..." ) );
    m_idMenuExtra_Stylist = m_vMenuExtra->insertItem4( text, this, "extraStylist", ALT + Key_S, -1, -1 );

    m_vMenuExtra->insertSeparator( -1 );

    text = Q2C( i18n( "&Options..." ) );
    m_idMenuExtra_Options = m_vMenuExtra->insertItem4( text, this, "extraOptions", ALT + Key_O, -1, -1 );

    // help menu
    m_vMenuHelp = _menubar->helpMenu();
    if ( CORBA::is_nil( m_vMenuHelp ) )
    {
	_menubar->insertSeparator( -1 );
	text = Q2C( i18n( "&Help" ) );
	_menubar->setHelpMenu( _menubar->insertMenu( text, m_vMenuHelp, -1, -1 ) );
    }
    else
	m_vMenuHelp->insertSeparator( -1 );

    text = Q2C( i18n( "&Contents" ) );
    m_idMenuHelp_Contents = m_vMenuHelp->insertItem4( text, this, "helpContents", 0, -1, -1 );
    /* m_rMenuBar->insertSeparator(m_idMenuHelp);
       m_idMenuHelp_About = m_rMenuBar->insertItem( CORBA::string_dup( i18n( "&About KWord..." ) ), m_idMenuHelp,
       this, CORBA::string_dup( "helpAbout" ) );
       m_idMenuHelp_AboutKOffice = m_rMenuBar->insertItem( CORBA::string_dup( i18n( "About K&Office..." ) ), m_idMenuHelp,
       this, CORBA::string_dup( "helpAboutKOffice" ) );
       m_idMenuHelp_AboutKDE = m_rMenuBar->insertItem( CORBA::string_dup( i18n( "&About KDE..." ) ), m_idMenuHelp,
       this, CORBA::string_dup( "helpAboutKDE" ) ); */

    QObject::connect( kapp->clipboard(), SIGNAL( dataChanged() ), this, SLOT( clipboardDataChanged() ) );

    return TRUE;
}

/*======================= setup edit toolbar ===================*/
bool KWordView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
    if ( CORBA::is_nil( _factory ) ) {
	m_vToolBarEdit = 0L;
	m_vToolBarText = 0L;
	m_vToolBarInsert = 0L;
	return TRUE;
    }

    m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

    m_vToolBarEdit->setFullWidth( FALSE );

    // undo
    OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( BarIcon( "undo" ) );
    CORBA::WString_var toolTip = Q2C( i18n( "Undo" ) );
    m_idButtonEdit_Undo = m_vToolBarEdit->insertButton2( pix, ID_UNDO, SIGNAL( clicked() ), this, "editUndo",
							 TRUE, toolTip, -1 );
    //m_vToolBarEdit->setItemEnabled( ID_UNDO, FALSE );

    // redo
    pix = OPUIUtils::convertPixmap( BarIcon( "redo" ) );
    toolTip = Q2C( i18n( "Redo" ) );
    m_idButtonEdit_Redo = m_vToolBarEdit->insertButton2( pix, ID_REDO, SIGNAL( clicked() ), this, "editRedo",
							 TRUE, toolTip, -1 );
    //m_vToolBarEdit->setItemEnabled( ID_REDO, FALSE );

    m_vToolBarEdit->insertSeparator( -1 );

    // cut
    pix = OPUIUtils::convertPixmap( BarIcon( "editcut" ) );
    toolTip = Q2C( i18n( "Cut" ) );
    m_idButtonEdit_Cut = m_vToolBarEdit->insertButton2( pix, ID_EDIT_CUT, SIGNAL( clicked() ), this,
							"editCut", TRUE, toolTip, -1 );

    // copy
    pix = OPUIUtils::convertPixmap( BarIcon( "editcopy" ) );
    toolTip = Q2C( i18n( "Copy" ) );
    m_idButtonEdit_Copy = m_vToolBarEdit->insertButton2( pix, ID_EDIT_COPY, SIGNAL( clicked() ), this,
							 "editCopy", TRUE, toolTip, -1 );

    // paste
    pix = OPUIUtils::convertPixmap( BarIcon( "editpaste" ) );
    toolTip = Q2C( i18n( "Paste" ) );
    m_idButtonEdit_Paste = m_vToolBarEdit->insertButton2( pix, ID_EDIT_PASTE, SIGNAL( clicked() ), this,
							  "editPaste", TRUE, toolTip, -1 );

    m_vToolBarEdit->insertSeparator( -1 );

    // spelling
    pix = OPUIUtils::convertPixmap( BarIcon( "spellcheck" ) );
    toolTip = Q2C( i18n( "Spell Checking" ) );
    m_idButtonEdit_Spelling = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraSpelling",
							     TRUE, toolTip, -1 );

    m_vToolBarEdit->insertSeparator( -1 );

    // find
    pix = OPUIUtils::convertPixmap( BarIcon( "kwsearch" ) );
    toolTip = Q2C( i18n( "Find & Replace" ) );
    m_idButtonEdit_Find = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editFind",
							 TRUE, toolTip, -1 );

    m_vToolBarEdit->enable( OpenPartsUI::Show );

    // TOOLBAR Insert
    m_vToolBarInsert = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarInsert->setFullWidth( FALSE );

    // picture
    pix = OPUIUtils::convertPixmap( BarIcon( "picture" ) );
    toolTip = Q2C( i18n( "Insert Picture" ) );
    m_idButtonInsert_Picture = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertPicture",
								TRUE, toolTip, -1 );

    // clipart
    pix = OPUIUtils::convertPixmap( BarIcon( "clipart" ) );
    toolTip = Q2C( i18n( "Insert Clipart" ) );
    m_idButtonInsert_Clipart = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertClipart",
								TRUE, toolTip, -1 );

    m_vToolBarInsert->insertSeparator( -1 );

    // special char
    pix = OPUIUtils::convertPixmap( BarIcon( "char" ) );
    toolTip = Q2C( i18n( "Insert Special Character" ) );
    m_idButtonInsert_SpecialChar = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertSpecialChar",
								    TRUE, toolTip, -1 );

    m_vToolBarInsert->enable( OpenPartsUI::Show );

    // TOOLBAR table
    m_vToolBarTable = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarTable->setFullWidth( FALSE );

    pix = OPUIUtils::convertPixmap( BarIcon( "rowin" ) );
    toolTip = Q2C( i18n( "Insert Row" ) );
    m_idButtonTable_InsertRow = m_vToolBarTable->insertButton2( pix, ID_TABLE_INSROW, SIGNAL( clicked() ), this,
								"tableInsertRow",
								TRUE, toolTip, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "colin" ) );
    toolTip = Q2C( i18n( "Insert Column" ) );
    m_idButtonTable_InsertCol = m_vToolBarTable->insertButton2( pix, ID_TABLE_INSCOL, SIGNAL( clicked() ), this,
								"tableInsertCol",
								TRUE, toolTip, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "rowout" ) );
    toolTip = Q2C( i18n( "Delete Row" ) );
    m_idButtonTable_DeleteRow = m_vToolBarTable->insertButton2( pix, ID_TABLE_DELROW, SIGNAL( clicked() ), this,
								"tableDeleteRow",
								TRUE, toolTip, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "colout" ) );
    toolTip = Q2C( i18n( "Delete Column" ) );
    m_idButtonTable_DeleteCol = m_vToolBarTable->insertButton2( pix, ID_TABLE_DELCOL, SIGNAL( clicked() ), this,
								"tableDeleteCol",
								TRUE, toolTip, -1 );

    m_vToolBarTable->enable( OpenPartsUI::Show );

    // TOOLBAR Tools
    m_vToolBarTools = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarTools->setFullWidth( FALSE );

    // edit
    pix = OPUIUtils::convertPixmap( BarIcon( "edittool" ) );
    toolTip = Q2C( i18n( "Edit Text Tool" ) );
    m_idButtonTools_Edit = m_vToolBarTools->insertButton2( pix, ID_TOOL_EDIT, SIGNAL( clicked() ), this,
							   "toolsEdit",
							   TRUE, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_EDIT, TRUE );
    m_vToolBarTools->setButton( ID_TOOL_EDIT, TRUE );

    // edit frame
    pix = OPUIUtils::convertPixmap( BarIcon( "editframetool" ) );
    toolTip = Q2C( i18n( "Edit Frames Tool" ) );
    m_idButtonTools_EditFrame = m_vToolBarTools->insertButton2( pix, ID_TOOL_EDIT_FRAME, SIGNAL( clicked() ),
								this, "toolsEditFrame",
								TRUE, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_EDIT_FRAME, TRUE );

    // create text frame
    pix = OPUIUtils::convertPixmap( BarIcon( "textframetool" ) );
    toolTip = Q2C( i18n( "Create Text Frame" ) );
    m_idButtonTools_CreateText = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_TEXT, SIGNAL( clicked() ),
								 this, "toolsCreateText",
								 TRUE, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_TEXT, TRUE );

    // create pix frame
    pix = OPUIUtils::convertPixmap( BarIcon( "picframetool" ) );
    toolTip = Q2C( i18n( "Create Picture Frame" ) );
    m_idButtonTools_CreatePix = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_PIX, SIGNAL( clicked() ),
								this, "toolsCreatePix",
								TRUE, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_PIX, TRUE );

    // create clip frame
    pix = OPUIUtils::convertPixmap( BarIcon( "clipart" ) );
    toolTip = Q2C( i18n( "Create Clipart Frame" ) );
    m_idButtonTools_Clipart = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_CLIPART, SIGNAL( clicked() ),
							      this, "toolsClipart",
							      TRUE, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_CLIPART, TRUE );

    // create table frame
    pix = OPUIUtils::convertPixmap( BarIcon( "table" ) );
    toolTip = Q2C( i18n( "Create Table Frame" ) );
    m_idButtonTools_Table = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_TABLE, SIGNAL( clicked() ),
							    this, "toolsTable",
							    TRUE, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_TABLE, TRUE );

    // create table frame
    pix = OPUIUtils::convertPixmap( BarIcon( "table" ) );
    toolTip = Q2C( i18n( "Create KSPread Table Frame" ) );
    m_idButtonTools_KSpreadTable = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_KSPREAD_TABLE, SIGNAL( clicked() ),
								   this,
								   "toolsKSpreadTable", TRUE, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_KSPREAD_TABLE, TRUE );

    // create formula frame
    pix = OPUIUtils::convertPixmap( BarIcon( "formula" ) );
    toolTip = Q2C( i18n( "Create Formula Frame" ) );
    m_idButtonTools_Formula = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_FORMULA, SIGNAL( clicked() ),
							      this, "toolsFormula",
							      TRUE, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_FORMULA, TRUE );

    // create part frame
    pix = OPUIUtils::convertPixmap( BarIcon( "parts" ) );
    toolTip = Q2C( i18n( "Create Part Frame" ) );
    m_idButtonTools_Part = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_PART, SIGNAL( clicked() ),
							   this, "toolsPart",
							   TRUE, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_PART, TRUE );

    m_vToolBarTools->enable( OpenPartsUI::Show );

    // TOOLBAR Text
    m_vToolBarText = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarText->setFullWidth( FALSE );

    // style combobox
    OpenPartsUI::StrList stylelist;
    stylelist.length( m_pKWordDoc->paragLayoutList.count() );
    for ( unsigned int i = 0; i < m_pKWordDoc->paragLayoutList.count(); i++ ) {
	styleList.append( QString( m_pKWordDoc->paragLayoutList.at( i )->getName() ) );
	stylelist[ i ] = CORBA::string_dup( m_pKWordDoc->paragLayoutList.at( i )->getName() );
    }
    toolTip = Q2C( i18n( "Style" ) );
    m_idComboText_Style = m_vToolBarText->insertCombo( stylelist, ID_STYLE_LIST,
						       FALSE, SIGNAL( activated( const QString & ) ),
						       this, "textStyleSelected", TRUE, toolTip,
						       200, -1, OpenPartsUI::AtBottom );

    // size combobox
    OpenPartsUI::StrList sizelist;
    sizelist.length( 97 );
    for ( int i = 4; i <= 100; i++ ) {
	char buffer[ 10 ];
	sprintf( buffer, "%i", i );
	sizelist[ i - 4 ] = CORBA::string_dup( buffer );
    }
    toolTip = Q2C( i18n( "Font Size" ) );
    m_idComboText_FontSize = m_vToolBarText->insertCombo( sizelist, ID_FONT_SIZE, TRUE,
							  SIGNAL( activated( const QString & ) ),
							  this, "textSizeSelected", TRUE,
							  toolTip, 50, -1, OpenPartsUI::AtBottom );
    m_vToolBarText->setCurrentComboItem( ID_FONT_SIZE, 8 );
    tbFont.setPointSize( 12 );

    // fonts combobox
    getFonts();
    OpenPartsUI::StrList fonts;
    fonts.length( fontList.count() );
    for( unsigned int i = 0; i < fontList.count(); i++ )
	fonts[ i ] = CORBA::string_dup( fontList[ i ] );
    toolTip = Q2C( i18n( "Font List" ) );
    m_idComboText_FontList = m_vToolBarText->insertCombo( fonts, ID_FONT_LIST, TRUE,
							  SIGNAL( activated( const QString & ) ), this,
							  "textFontSelected", TRUE, toolTip,
							  200, -1, OpenPartsUI::AtBottom );
    tbFont.setFamily( fontList[ 0 ] );
    m_vToolBarText->setCurrentComboItem( ID_FONT_LIST, 0 );

    m_vToolBarText->insertSeparator( -1 );

    // bold
    pix = OPUIUtils::convertPixmap( BarIcon( "bold" ) );
    toolTip = Q2C( i18n( "Bold" ) );
    m_idButtonText_Bold = m_vToolBarText->insertButton2( pix, ID_BOLD, SIGNAL( clicked() ),
							 this, "textBold", TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_BOLD, TRUE );
    m_vToolBarText->setButton( ID_BOLD, FALSE );
    tbFont.setBold( FALSE );

    // italic
    pix = OPUIUtils::convertPixmap( BarIcon( "italic" ) );
    toolTip = Q2C( i18n( "Italic" ) );
    m_idButtonText_Italic = m_vToolBarText->insertButton2( pix, ID_ITALIC, SIGNAL( clicked() ),
							   this, "textItalic", TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ITALIC, TRUE );
    m_vToolBarText->setButton( ID_ITALIC, FALSE );
    tbFont.setItalic( FALSE );

    // underline
    pix = OPUIUtils::convertPixmap( BarIcon( "underl" ) );
    toolTip = Q2C( i18n( "Underline" ) );
    m_idButtonText_Underline = m_vToolBarText->insertButton2( pix, ID_UNDERLINE, SIGNAL( clicked() ),
							      this, "textUnderline", TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_UNDERLINE, TRUE );
    m_vToolBarText->setButton( ID_UNDERLINE, FALSE );
    tbFont.setUnderline( FALSE );

    // color
    tbColor = black;
    pix = KOUIUtils::colorPixmap( tbColor, KOUIUtils::TXT_COLOR );
    toolTip = Q2C( i18n( "Text Color" ) );
    m_idButtonText_Color = m_vToolBarText->insertButton2( pix, ID_TEXT_COLOR, SIGNAL( clicked() ),
							  this, "textColor",
							  TRUE, toolTip, -1 );

    m_vToolBarText->insertSeparator( -1 );

    // align left
    pix = OPUIUtils::convertPixmap( BarIcon( "alignLeft" ) );
    toolTip = Q2C( i18n( "Align Left" ) );
    m_idButtonText_ALeft = m_vToolBarText->insertButton2( pix, ID_ALEFT, SIGNAL( clicked() ),
							  this, "textAlignLeft",
							  TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ALEFT, TRUE );
    m_vToolBarText->setButton( ID_ALEFT, TRUE );

    // align center
    pix = OPUIUtils::convertPixmap( BarIcon( "alignCenter" ) );
    toolTip = Q2C( i18n( "Align Center" ) );
    m_idButtonText_ACenter = m_vToolBarText->insertButton2( pix, ID_ACENTER, SIGNAL( clicked() ),
							    this, "textAlignCenter",
							    TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ACENTER, TRUE );
    m_vToolBarText->setButton( ID_ACENTER, FALSE );

    // align right
    pix = OPUIUtils::convertPixmap( BarIcon( "alignRight" ) );
    toolTip = Q2C( i18n( "Align Right" ) );
    m_idButtonText_ARight = m_vToolBarText->insertButton2( pix, ID_ARIGHT, SIGNAL( clicked() ), this, "textAlignRight",
							   TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ARIGHT, TRUE );
    m_vToolBarText->setButton( ID_ARIGHT, FALSE );

    QString tmp;
    // align block
    pix = OPUIUtils::convertPixmap( BarIcon( "alignBlock" ) );
    toolTip = Q2C( i18n( "Align Block" ) );
    m_idButtonText_ABlock = m_vToolBarText->insertButton2( pix, ID_ABLOCK, SIGNAL( clicked() ), this, "textAlignBlock",
							   TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ABLOCK, TRUE );
    m_vToolBarText->setButton( ID_ABLOCK, FALSE );

    // line spacing
    OpenPartsUI::StrList spclist;
    spclist.length( 11 );
    for( unsigned int i = 0; i <= 10; i++ ) {
	char buffer[ 10 ];
	sprintf( buffer, "%i", i );
	spclist[ i ] = CORBA::string_dup( buffer );
    }
    toolTip = Q2C( i18n( "Line Spacing ( in pt )" ) );
    m_idComboText_LineSpacing = m_vToolBarText->insertCombo( spclist, ID_LINE_SPC, FALSE,
							     SIGNAL( activated( const QString & ) ),
							     this, "textLineSpacing", TRUE, toolTip,
							     60, -1, OpenPartsUI::AtBottom );
    spc = 0;
    m_vToolBarText->insertSeparator( -1 );

    // enum list
    pix = OPUIUtils::convertPixmap( BarIcon( "enumList" ) );
    toolTip = Q2C( i18n( "Enumerated List" ) );
    m_idButtonText_EnumList = m_vToolBarText->insertButton2( pix, ID_ENUM_LIST, SIGNAL( clicked() ),
							     this, "textEnumList",
							     TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ENUM_LIST, TRUE );
    m_vToolBarText->setButton( ID_ENUM_LIST, FALSE );

    // unsorted list
    pix = OPUIUtils::convertPixmap( BarIcon( "unsortedList" ) );
    toolTip = Q2C( i18n( "Unsorted List" ) );
    m_idButtonText_EnumList = m_vToolBarText->insertButton2( pix, ID_USORT_LIST, SIGNAL( clicked() ),
							     this, "textUnsortList",
							     TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_USORT_LIST, TRUE );
    m_vToolBarText->setButton( ID_USORT_LIST, FALSE );

    m_vToolBarText->insertSeparator( -1 );

    // superscript
    pix = OPUIUtils::convertPixmap( BarIcon( "super" ) );
    toolTip = Q2C( i18n( "Superscript" ) );
    m_idButtonText_SuperScript = m_vToolBarText->insertButton2( pix, ID_SUPERSCRIPT, SIGNAL( clicked() ),
								this, "textSuperScript",
								TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_SUPERSCRIPT, TRUE );
    m_vToolBarText->setButton( ID_SUPERSCRIPT, FALSE );

    // subscript
    pix = OPUIUtils::convertPixmap( BarIcon( "sub" ) );
    toolTip = Q2C( i18n( "Subscript" ) );
    m_idButtonText_SubScript = m_vToolBarText->insertButton2( pix, ID_SUBSCRIPT, SIGNAL( clicked() ),
							      this, "textSubScript",
							      TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_SUBSCRIPT, TRUE );
    m_vToolBarText->setButton( ID_SUBSCRIPT, FALSE );

    m_vToolBarText->insertSeparator( -1 );

    // border left
    pix = OPUIUtils::convertPixmap( BarIcon( "borderleft" ) );
    toolTip = Q2C( i18n( "Paragraph Border Left" ) );
    m_idButtonText_BorderLeft = m_vToolBarText->insertButton2( pix, ID_BRD_LEFT, SIGNAL( clicked() ),
							       this, "textBorderLeft",
							       TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_BRD_LEFT, TRUE );
    m_vToolBarText->setButton( ID_BRD_LEFT, FALSE );

    // border right
    pix = OPUIUtils::convertPixmap( BarIcon( "borderright" ) );
    toolTip = Q2C( i18n( "Paragraph Border Right" ) );
    m_idButtonText_BorderRight = m_vToolBarText->insertButton2( pix, ID_BRD_RIGHT, SIGNAL( clicked() ),
								this, "textBorderRight",
								TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_BRD_RIGHT, TRUE );
    m_vToolBarText->setButton( ID_BRD_RIGHT, FALSE );

    // border top
    pix = OPUIUtils::convertPixmap( BarIcon( "bordertop" ) );
    toolTip = Q2C( i18n( "Paragraph Border Top" ) );
    m_idButtonText_BorderTop = m_vToolBarText->insertButton2( pix, ID_BRD_TOP, SIGNAL( clicked() ),
							      this, "textBorderTop",
							      TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_BRD_TOP, TRUE );
    m_vToolBarText->setButton( ID_BRD_TOP, FALSE );

    // border bottom
    pix = OPUIUtils::convertPixmap( BarIcon( "borderbottom" ) );
    toolTip = Q2C( i18n( "Paragraph Border Bottom" ) );
    m_idButtonText_BorderBottom = m_vToolBarText->insertButton2( pix, ID_BRD_BOTTOM, SIGNAL( clicked() ),
								 this, "textBorderBottom",
								 TRUE, toolTip, -1 );
    m_vToolBarText->setToggle( ID_BRD_BOTTOM, TRUE );
    m_vToolBarText->setButton( ID_BRD_BOTTOM, FALSE );

    // border color
    tmpBrd.color = black;
    pix = KOUIUtils::colorPixmap( tmpBrd.color, KOUIUtils::FRAME_COLOR );
    toolTip = Q2C( i18n( "Paragraph Border Color" ) );
    m_idButtonText_BorderColor = m_vToolBarText->insertButton2( pix, ID_BORDER_COLOR, SIGNAL( clicked() ),
								this, "textBorderColor",
								TRUE, toolTip, -1 );

    // border width combobox
    OpenPartsUI::StrList widthlist;
    widthlist.length( 10 );
    for( unsigned int i = 1; i <= 10; i++ ) {
	char buffer[ 10 ];
	sprintf( buffer, "%i", i );
	widthlist[ i-1 ] = CORBA::string_dup( buffer );
    }
    tmpBrd.ptWidth = 1;
    toolTip = Q2C( i18n( "Paragraph Border Width" ) );
    m_idComboText_BorderWidth = m_vToolBarText->insertCombo( widthlist, ID_BRD_WIDTH, FALSE,
							     SIGNAL( activated( const QString & ) ),
							     this, "textBorderWidth", TRUE, toolTip,
							     60, -1, OpenPartsUI::AtBottom );

    // border style combobox
    stylelist.length( 5 );
    stylelist[ 0 ] = CORBA::string_dup( i18n( "solid line" ) );
    stylelist[ 1 ] = CORBA::string_dup( i18n( "dash line ( ---- )" ) );
    stylelist[ 2 ] = CORBA::string_dup( i18n( "dot line ( **** )" ) );
    stylelist[ 3 ] = CORBA::string_dup( i18n( "dash dot line ( -*-* )" ) );
    stylelist[ 4 ] = CORBA::string_dup( i18n( "dash dot dot line ( -**- )" ) );
    toolTip = Q2C( i18n( "Paragraph Border Style" ) );
    m_idComboText_BorderStyle = m_vToolBarText->insertCombo( stylelist, ID_BRD_STYLE, FALSE,
							     SIGNAL( activated( const QString & ) ),
							     this, "textBorderStyle", TRUE, toolTip,
							     150, -1, OpenPartsUI::AtBottom );
    tmpBrd.style = KWParagLayout::SOLID;

    m_vToolBarText->enable( OpenPartsUI::Show );

    // TOOLBAR Frame
    m_vToolBarFrame = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarFrame->setFullWidth( FALSE );

    // border left
    pix = OPUIUtils::convertPixmap( BarIcon( "borderleft" ) );
    toolTip = Q2C( i18n( "Frame Border Left" ) );
    m_idButtonFrame_BorderLeft = m_vToolBarFrame->insertButton2( pix, ID_FBRD_LEFT, SIGNAL( clicked() ),
								 this, "frameBorderLeft",
								 TRUE, toolTip, -1 );
    m_vToolBarFrame->setToggle( ID_FBRD_LEFT, TRUE );
    m_vToolBarFrame->setButton( ID_FBRD_LEFT, FALSE );

    // border right
    pix = OPUIUtils::convertPixmap( BarIcon( "borderright" ) );
    toolTip = Q2C( i18n( "Frame Border Right" ) );
    m_idButtonFrame_BorderRight = m_vToolBarFrame->insertButton2( pix, ID_FBRD_RIGHT, SIGNAL( clicked() ),
								  this, "frameBorderRight",
								  TRUE, toolTip, -1 );
    m_vToolBarFrame->setToggle( ID_FBRD_RIGHT, TRUE );
    m_vToolBarFrame->setButton( ID_FBRD_RIGHT, FALSE );

    // border top
    pix = OPUIUtils::convertPixmap( BarIcon( "bordertop" ) );
    toolTip = Q2C( i18n( "Frame Border Top" ) );
    m_idButtonFrame_BorderTop = m_vToolBarFrame->insertButton2( pix, ID_FBRD_TOP, SIGNAL( clicked() ),
								this, "frameBorderTop",
								TRUE, toolTip, -1 );
    m_vToolBarFrame->setToggle( ID_FBRD_TOP, TRUE );
    m_vToolBarFrame->setButton( ID_FBRD_TOP, FALSE );

    // border bottom
    pix = OPUIUtils::convertPixmap( BarIcon( "borderbottom" ) );
    toolTip = Q2C( i18n( "Frame Border Bottom" ) );
    m_idButtonFrame_BorderBottom = m_vToolBarFrame->insertButton2( pix, ID_FBRD_BOTTOM, SIGNAL( clicked() ),
								   this, "frameBorderBottom",
								   TRUE, toolTip, -1 );
    m_vToolBarFrame->setToggle( ID_FBRD_BOTTOM, TRUE );
    m_vToolBarFrame->setButton( ID_FBRD_BOTTOM, FALSE );

    // border color
    pix = KOUIUtils::colorPixmap( frmBrd.color, KOUIUtils::FRAME_COLOR );
    toolTip = Q2C( i18n( "Frame Border Color" ) );
    m_idButtonFrame_BorderColor = m_vToolBarFrame->insertButton2( pix, ID_FBORDER_COLOR, SIGNAL( clicked() ),
								  this, "frameBorderColor",
								  TRUE, toolTip, -1 );

    toolTip = Q2C( i18n( "Frame Border Width" ) );
    m_idComboFrame_BorderWidth = m_vToolBarFrame->insertCombo( widthlist, ID_FBRD_WIDTH, FALSE,
							       SIGNAL( activated( const QString & ) ),
							       this, "frameBorderWidth", TRUE, toolTip,
							       60, -1, OpenPartsUI::AtBottom );

    toolTip = Q2C( i18n( "Frame Border Style" ) );
    m_idComboFrame_BorderStyle = m_vToolBarFrame->insertCombo( stylelist, ID_FBRD_STYLE, FALSE,
							       SIGNAL( activated( const QString & ) ),
							       this, "frameBorderStyle", TRUE, toolTip,
							       150, -1, OpenPartsUI::AtBottom );

    // frame back color
    backColor.setColor( white );
    pix = KOUIUtils::colorPixmap( backColor.color(), KOUIUtils::BACK_COLOR );
    toolTip = Q2C( i18n( "Frame Background Color" ) );
    m_idButtonFrame_BackColor = m_vToolBarFrame->insertButton2( pix, ID_FBACK_COLOR, SIGNAL( clicked() ),
								this, "frameBackColor",
								TRUE, toolTip, -1 );

    m_vToolBarFrame->enable( OpenPartsUI::Hide );

    
    // TOOLBAR Formula
    m_vToolBarFormula = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarFormula->setFullWidth( FALSE );

    pix = OPUIUtils::convertPixmap( BarIcon( "index2" ) );
    toolTip = Q2C( i18n( "Power" ) );
    m_idButtonFormula_Power = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
								this, "formulaPower",
								TRUE, toolTip, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "index3" ) );
    toolTip = Q2C( i18n( "Subscript" ) );
    m_idButtonFormula_Subscript = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
								    this, "formulaSubscript",
								    TRUE, toolTip, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "bra" ) );
    toolTip = Q2C( i18n( "Parentheses" ) );
    m_idButtonFormula_Parentheses = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
								      this, "formulaParentheses",
								      TRUE, toolTip, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "abs" ) );
    toolTip = Q2C( i18n( "Absolute Value" ) );
    m_idButtonFormula_AbsValue = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
								   this, "formulaAbsValue",
								   TRUE, toolTip, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "brackets" ) );
    toolTip = Q2C( i18n( "Brackets" ) );
    m_idButtonFormula_Brackets = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
								   this, "formulaBrackets",
								   TRUE, toolTip, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "frac" ) );
    toolTip = Q2C( i18n( "Fraction" ) );
    m_idButtonFormula_Fraction = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
								   this, "formulaFraction",
								   TRUE, toolTip, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "root" ) );
    toolTip = Q2C( i18n( "Root" ) );
    m_idButtonFormula_Root = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
							       this, "formulaRoot",
							       TRUE, toolTip, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "integral" ) );
    toolTip = Q2C( i18n( "Integral" ) );
    m_idButtonFormula_Integral = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
								   this, "formulaIntegral",
								   TRUE, toolTip, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "matrix" ) );
    toolTip = Q2C( i18n( "Matrix" ) );
    m_idButtonFormula_Matrix = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
								 this, "formulaMatrix",
								 TRUE, toolTip, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "index0" ) );
    toolTip = Q2C( i18n( "Left Superscript" ) );
    m_idButtonFormula_LeftSuper = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
								    this, "formulaLeftSuper",
								    TRUE, toolTip, -1 );
    pix = OPUIUtils::convertPixmap( BarIcon( "index1" ) );
    toolTip = Q2C( i18n( "Left Subscript" ) );
    m_idButtonFormula_LeftSub = m_vToolBarFormula->insertButton2( pix, 1, SIGNAL( clicked() ),
								  this, "formulaLeftSub",
								  TRUE, toolTip, -1 );

    m_vToolBarFormula->enable( OpenPartsUI::Hide );
//     m_vToolBarFormula->setBarPos( OpenPartsUI::Floating );
    
    updateStyle( "Standard" );
    setFormat( format, FALSE );
    gui->getPaperWidget()->forceFullUpdate();
    gui->getPaperWidget()->init();

    clipboardDataChanged();

    gui->getPaperWidget()->repaintScreen( TRUE );

    return TRUE;
}

/*===================== load not KDE installed fonts =============*/
void KWordView::getFonts()
{
    int numFonts;
    Display *kde_display;
    char** fontNames;
    char** fontNames_copy;
    QString qfontname;

    kde_display = kapp->getDisplay();

    bool have_installed = kapp->getKDEFonts( fontList );

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
    if ( !m_vToolBarText ) return;

    m_vToolBarText->setCurrentComboItem( ID_BRD_WIDTH, tmpBrd.ptWidth - 1 );
    m_vToolBarText->setCurrentComboItem( ID_BRD_STYLE, static_cast<int>( tmpBrd.style ) );

    OpenPartsUI::Pixmap_var colpix =
      KOUIUtils::colorPixmap( tmpBrd.color, KOUIUtils::FRAME_COLOR );
    m_vToolBarText->setButtonPixmap( ID_BORDER_COLOR, colpix );
}

/*================================================================*/
void KWordView::slotInsertObject( KWordChild *_child, KWPartFrameSet *_kwpf )
{
    OpenParts::View_var v;

    try
    {
	v = _child->createView( m_vKoMainWindow );
    }
    catch ( OpenParts::Document::MultipleViewsNotSupported &_ex )
    {
	// HACK
	printf( "void KWordView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n" );
	printf( "Could not create view\n" );
	exit( 1 );
    }

    if ( CORBA::is_nil( v ) )
    {
	printf( "void KWordView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n" );
	printf( "return value is 0L\n" );
	exit( 1 );
    }

    KOffice::View_var kv = KOffice::View::_narrow( v );
    kv->setMode( KOffice::View::ChildMode );
    assert( !CORBA::is_nil( kv ) );
    _kwpf->setView( kv );
}

/*================================================================*/
void KWordView::slotUpdateChildGeometry( KWordChild */*_child*/ )
{
}

/*================================================================*/
void KWordView::slotGeometryEnd( KoFrame* /*_frame*/ )
{
}

/*================================================================*/
void KWordView::slotMoveEnd( KoFrame* /*_frame*/ )
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
    if ( !currParag && currFrameSetNum == -1 )
    {
	QObject::connect( kspell, SIGNAL( misspelling( char*, QStrList*, unsigned ) ),
			  this, SLOT( spellCheckerMisspelling( char*, QStrList*, unsigned ) ) );
	QObject::connect( kspell, SIGNAL( corrected( char*, char*, unsigned ) ),
			  this, SLOT( spellCheckerCorrected( char*, char*, unsigned ) ) );
	QObject::connect( kspell, SIGNAL( done( char* ) ), this, SLOT( spellCheckerDone( char* ) ) );
	currParag = 0L;
	for ( unsigned int i = 0; i < m_pKWordDoc->getNumFrameSets(); i++ )
	{
	    KWFrameSet *frameset = m_pKWordDoc->getFrameSet( i );
	    if ( frameset->getFrameType() != FT_TEXT ) continue;
	    currFrameSetNum = i;
	    currParag = dynamic_cast<KWTextFrameSet*>( frameset )->getFirstParag();
	    break;
	}
	if ( !currParag )
	{
	    kspell->cleanUp();
	    QObject::disconnect( kspell, SIGNAL( misspelling( char*, QStrList*, unsigned ) ), this,
				 SLOT( spellCheckerMisspelling( char*, QStrList*, unsigned ) ) );
	    QObject::disconnect( kspell, SIGNAL( corrected( char*, char*, unsigned ) ),
				 this, SLOT( spellCheckerCorrected( char*, char*, unsigned ) ) );
	    QObject::disconnect( kspell, SIGNAL( done( char* ) ), this, SLOT( spellCheckerDone( char* ) ) );
	    delete kspell;
	    return;
	}
    }
    else currParag = currParag->getNext();

    if ( !currParag )
    {
	currFrameSetNum++;
	if ( currFrameSetNum >= static_cast<int>( m_pKWordDoc->getNumFrameSets() ) )
	{
	    kspell->cleanUp();
	    QObject::disconnect( kspell, SIGNAL( misspelling( char*, QStrList*, unsigned ) ), this,
				 SLOT( spellCheckerMisspelling( char*, QStrList*, unsigned ) ) );
	    QObject::disconnect( kspell, SIGNAL( corrected( char*, char*, unsigned ) ),
				 this, SLOT( spellCheckerCorrected( char*, char*, unsigned ) ) );
	    QObject::disconnect( kspell, SIGNAL( done( char* ) ), this, SLOT( spellCheckerDone( char* ) ) );
	    delete kspell;
	    return;
	}
	currParag = 0L;
	for ( unsigned int i = currFrameSetNum; i < m_pKWordDoc->getNumFrameSets(); i++ )
	{
	    KWFrameSet *frameset = m_pKWordDoc->getFrameSet( i );
	    if ( frameset->getFrameType() != FT_TEXT ) continue;
	    currFrameSetNum = i;
	    currParag = dynamic_cast<KWTextFrameSet*>( frameset )->getFirstParag();
	    break;
	}
	if ( !currParag )
	{
	    kspell->cleanUp();
	    QObject::disconnect( kspell, SIGNAL( misspelling( char*, QStrList*, unsigned ) ), this,
				 SLOT( spellCheckerMisspelling( char*, QStrList*, unsigned ) ) );
	    QObject::disconnect( kspell, SIGNAL( corrected( char*, char*, unsigned ) ),
				 this, SLOT( spellCheckerCorrected( char*, char*, unsigned ) ) );
	    QObject::disconnect( kspell, SIGNAL( done( char* ) ), this, SLOT( spellCheckerDone( char* ) ) );
	    delete kspell;
	    return;
	}
    }

    QString text = currParag->getKWString()->toString( 0, currParag->getTextLen() );
    kspell->check( text.data() );
}

/*================================================================*/
void KWordView::spellCheckerMisspelling( char* , QStrList* , unsigned )
{
}

/*================================================================*/
void KWordView::spellCheckerCorrected( char* , char* , unsigned )
{
}

/*================================================================*/
void KWordView::spellCheckerDone( char* )
{
    spellCheckerReady();
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
void KWordView::setMode( KOffice::View::Mode _mode )
{
    KoViewIf::setMode( _mode );

    if ( mode() == KOffice::View::ChildMode && !m_bFocus )
	m_bShowGUI = FALSE;
    else
	m_bShowGUI = TRUE;

    if ( gui )
    {
	gui->showGUI( m_bShowGUI );
	gui->getPaperWidget()->recalcText();
    }
}

/*================================================================*/
void KWordView::setFocus( CORBA::Boolean _mode )
{
    KoViewIf::setFocus( _mode );

    bool old = m_bShowGUI;

    if ( mode() == KOffice::View::ChildMode && !m_bFocus )
	m_bShowGUI = FALSE;
    else
	m_bShowGUI = TRUE;

    if ( gui ) gui->showGUI( m_bShowGUI );

    if ( old != m_bShowGUI )
	resizeEvent( 0L );
}

/*================================================================*/
void KWordView::changeUndo( QString _text, bool _enable )
{
    if ( !m_vMenuEdit || !m_vToolBarEdit ) return;

    CORBA::WString_var text;

    if ( _enable )
    {
	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, TRUE );
	QString str;
	str.sprintf( i18n( "Undo: %s" ), _text.data() );
	text = Q2C( str );
	m_vMenuEdit->changeItemText( text, m_idMenuEdit_Undo );
	m_vToolBarEdit->setItemEnabled( ID_UNDO, TRUE );
    }
    else
    {
	text = Q2C( i18n( "No Undo possible" ) );
	m_vMenuEdit->changeItemText( text, m_idMenuEdit_Undo );
	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, FALSE );
	m_vToolBarEdit->setItemEnabled( ID_UNDO, FALSE );
    }
}

/*================================================================*/
void KWordView::changeRedo( QString _text, bool _enable )
{
    if ( !m_vMenuEdit || !m_vToolBarEdit ) return;

    CORBA::WString_var text;

    if ( _enable )
    {
	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, TRUE );
	QString str;
	str.sprintf( i18n( "Redo: %s" ), _text.data() );
	text = Q2C( str );
	m_vMenuEdit->changeItemText( text, m_idMenuEdit_Redo );
	m_vToolBarEdit->setItemEnabled( ID_REDO, TRUE );
    }
    else
    {
	text = Q2C( i18n( "No Redo possible" ) );
	m_vMenuEdit->changeItemText( text, m_idMenuEdit_Redo );
	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, FALSE );
	m_vToolBarEdit->setItemEnabled( ID_REDO, FALSE );
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
