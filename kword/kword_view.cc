/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: View                                                   */
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

#include <opUIUtils.h>
#include <opMenuIf.h>
#include <opToolBarIf.h>

#include <koPartSelectDia.h>
#include <koMainWindow.h>
#include <koDocument.h>
#include <koRuler.h>
#include <koTabChooser.h>
#include <koPartSelectDia.h>

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
/* Class: KWordFrame                                              */
/******************************************************************/
KWordFrame::KWordFrame( KWordView* _view, KWordChild* _child )
    : KoFrame( _view )
{
    m_pKWordView = _view;
    m_pKWordChild = _child;
    obj = 0L;
}

/******************************************************************/
/* Class: KWordView                                               */
/******************************************************************/

/*================================================================*/
KWordView::KWordView( QWidget *_parent, const char *_name, KWordDocument* _doc )
    : QWidget( _parent, _name ), KoViewIf( _doc ), OPViewIf( _doc ), KWord::KWordView_skel(), format( _doc )
{
    shell = 0L;
    setWidget( this );

    KoViewIf::setFocusPolicy( OpenParts::Part::ClickFocus );

    m_pKWordDoc = 0L;
    m_bUnderConstruction = true;
    m_bShowGUI = true;
    m_vMenuTools = 0L;
    m_vToolBarTools = 0L;
    m_vToolBarText = 0L;
    m_lstFrames.setAutoDelete( true );
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
    _viewFormattingChars = false;
    _viewFrameBorders = true;
    _viewTableGrid = true;
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


    setKeyCompression( true );
    setAcceptDrops( true );
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

    setFormat( format, false );

    if ( gui )
        gui->setDocument( m_pKWordDoc );

    format = m_pKWordDoc->getDefaultParagLayout()->getFormat();
    if ( gui )
        gui->getPaperWidget()->formatChanged( format );

    KWFrameSet *frameset;
    for ( unsigned int i = 0; i < m_pKWordDoc->getNumFrameSets(); i++ )
    {
        frameset = m_pKWordDoc->getFrameSet( i );
        if ( frameset->getFrameType() == FT_PART )
            slotInsertObject( dynamic_cast<KWPartFrameSet*>( frameset )->getChild(), dynamic_cast<KWPartFrameSet*>( frameset ) );
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
void KWordView::clipboardDataChanged()
{
    if ( kapp->clipboard()->text().isEmpty() )
    {
        m_vMenuEdit->setItemEnabled( m_idMenuEdit_Paste, false );
        m_vToolBarEdit->setItemEnabled( ID_EDIT_PASTE, false );
    }
    else
    {
        m_vMenuEdit->setItemEnabled( m_idMenuEdit_Paste, true );
        m_vToolBarEdit->setItemEnabled( ID_EDIT_PASTE, true );
    }
}

/*================================================================*/
void KWordView::selectionOnOff()
{
    if ( m_pKWordDoc->has_selection() )
    {
        m_vMenuEdit->setItemEnabled( m_idMenuEdit_Cut, true );
        m_vMenuEdit->setItemEnabled( m_idMenuEdit_Copy, true );
        m_vToolBarEdit->setItemEnabled( ID_EDIT_CUT, true );
        m_vToolBarEdit->setItemEnabled( ID_EDIT_COPY, true );
    }
    else
    {
        m_vMenuEdit->setItemEnabled( m_idMenuEdit_Cut, false );
        m_vMenuEdit->setItemEnabled( m_idMenuEdit_Copy, false );
        m_vToolBarEdit->setItemEnabled( ID_EDIT_CUT, false );
        m_vToolBarEdit->setItemEnabled( ID_EDIT_COPY, false );
    }
}

/*================================================================*/
void KWordView::setFramesToParts()
{
    KWordFrame *frame = 0L;
    for ( unsigned int i = 0; i < m_lstFrames.count(); i++ )
    {
        frame = m_lstFrames.at( i );
        frame->hide();
        frame->view()->setMainWindow( mainWindow() );
        frame->getPartObject()->setView( frame );
        frame->getPartObject()->setMainWindow( mainWindow() );
        frame->getPartObject()->setParentID( id() );
    }
}

/*================================================================*/
void KWordView::hideAllFrames()
{
    KWordFrame *frame = 0L;
    for ( unsigned int i = 0; i < m_lstFrames.count(); i++ )
    {
        frame = m_lstFrames.at( i );
        frame->hide();
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

    m_lstFrames.clear();

    KoViewIf::cleanUp();

    cerr << "... void KWordView::cleanUp()" << endl;
}

/*=========================== file print =======================*/
CORBA::Boolean KWordView::printDlg()
{
    QPrinter prt;
    prt.setMinMax( 1, m_pKWordDoc->getPages() );
    bool makeLandscape = false;

    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    m_pKWordDoc->getPageLayout( pgLayout, cl, hf );

    switch ( pgLayout.format )
    {
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
    case PG_SCREEN:
    {
        warning( i18n( "You use the page layout SCREEN. I print it in DIN A4 LANDSCAPE!" ) );
        prt.setPageSize( QPrinter::A4 );
        makeLandscape = true;
    }   break;
    default:
    {
        warning( i18n( "The used page layout is not supported by QPrinter. I set it to DIN A4." ) );
        prt.setPageSize( QPrinter::A4 );
    } break;
    }

    switch ( pgLayout.orientation )
    {
    case PG_PORTRAIT: prt.setOrientation( QPrinter::Portrait );
        break;
    case PG_LANDSCAPE: prt.setOrientation( QPrinter::Landscape );
        break;
    }

    float left_margin = 0.0;
    float top_margin = 0.0;

    if ( makeLandscape )
    {
        prt.setOrientation( QPrinter::Landscape );
        left_margin = 28.5;
        top_margin = 15.0;
    }

    if ( prt.setup( this ) )
    {
        setCursor( waitCursor );
        gui->getPaperWidget()->viewport()->setCursor( waitCursor );

        QPainter painter;
        painter.begin( &prt );
        m_pKWordDoc->print( &painter, &prt, left_margin, top_margin );
        painter.end();

        setCursor( arrowCursor );
        gui->getPaperWidget()->viewport()->setCursor( ibeamCursor );
    }
    return true;
}

/*================================================================*/
void KWordView::setFormat( const KWFormat &_format, bool _check, bool _update_page, bool _redraw )
{
    if ( _check && _format == format || !m_vToolBarText ) return;

    if ( gui && gui->getPaperWidget() && gui->getPaperWidget()->getCursor() && gui->getPaperWidget()->getCursor()->getParag()
         && gui->getPaperWidget()->getCursor()->getTextPos() > 0
         && gui->getPaperWidget()->getCursor()->getParag()->getKWString()->data()[ gui->getPaperWidget()->getCursor()->getTextPos() - 1 ].attrib
         && gui->getPaperWidget()->getCursor()->getParag()->
         getKWString()->data()[ gui->getPaperWidget()->getCursor()->getTextPos() - 1 ].attrib->getClassId() == ID_KWCharFootNote )
        return;

    format = _format;

    if ( _format.getUserFont()->getFontName() )
    {
        QValueList<QString>::Iterator it = fontList.find( _format.getUserFont()->getFontName().lower() );
        if ( !CORBA::is_nil( m_vToolBarText ) && it != fontList.end() )
        {
            QValueList<QString>::Iterator it2 = fontList.begin();
            int pos = 0;
            for ( ; it != it2; ++it2, ++pos );
            m_vToolBarText->setCurrentComboItem( ID_FONT_LIST, pos );
        }
    }

    if ( _format.getPTFontSize() != -1 )
        if ( !CORBA::is_nil( m_vToolBarText ) )
            m_vToolBarText->setCurrentComboItem( ID_FONT_SIZE, _format.getPTFontSize() - 4 );

    if ( _format.getWeight() != -1 )
    {
        if ( !CORBA::is_nil( m_vToolBarText ) )
            m_vToolBarText->setButton( ID_BOLD, _format.getWeight() == QFont::Bold );
        tbFont.setBold( _format.getWeight() == QFont::Bold );
    }
    if ( _format.getItalic() != -1 )
    {
        if ( !CORBA::is_nil( m_vToolBarText ) )
            m_vToolBarText->setButton( ID_ITALIC, _format.getItalic() == 1 );
        tbFont.setItalic( _format.getItalic() == 1 );
    }
    if ( _format.getUnderline() != -1 )
    {
        if ( !CORBA::is_nil( m_vToolBarText ) )
            m_vToolBarText->setButton( ID_UNDERLINE, _format.getUnderline() == 1 );
        tbFont.setUnderline( _format.getUnderline() == 1 );
    }

    if ( _format.getColor().isValid() )
    {
        if ( !CORBA::is_nil( m_vToolBarText ) )
        {
            OpenPartsUI::Pixmap pix;
            pix.data = CORBA::string_dup( colorToPixString( _format.getColor(), TXT_COLOR ) );

            m_vToolBarText->setButtonPixmap( ID_TEXT_COLOR, pix );
        }
        tbColor = QColor( _format.getColor() );
    }

    if ( !CORBA::is_nil( m_vToolBarText ) )
    {
        m_vToolBarText->setButton( ID_SUPERSCRIPT, false );
        m_vToolBarText->setButton( ID_SUBSCRIPT, false );
    }

    if ( _format.getVertAlign() == KWFormat::VA_NORMAL )
        vertAlign = KWFormat::VA_NORMAL;
    else if ( _format.getVertAlign() == KWFormat::VA_SUB )
    {
        vertAlign = KWFormat::VA_SUB;
        if ( !CORBA::is_nil( m_vToolBarText ) )
            m_vToolBarText->setButton( ID_SUBSCRIPT, true );
    }
    else if ( _format.getVertAlign() == KWFormat::VA_SUPER )
    {
        vertAlign = KWFormat::VA_SUPER;
        if ( !CORBA::is_nil( m_vToolBarText ) )
            m_vToolBarText->setButton( ID_SUPERSCRIPT, true );
    }

    format = _format;

    if ( _update_page )
        gui->getPaperWidget()->formatChanged( format, _redraw );
}

/*================================================================*/
void KWordView::setFlow( KWParagLayout::Flow _flow )
{
    if ( _flow != flow && m_vToolBarText )
    {
        flow = _flow;
        m_vToolBarText->setButton( ID_ALEFT, false );
        m_vToolBarText->setButton( ID_ACENTER, false );
        m_vToolBarText->setButton( ID_ARIGHT, false );
        m_vToolBarText->setButton( ID_ABLOCK, false );

        switch ( flow )
        {
        case KWParagLayout::LEFT:
            m_vToolBarText->setButton( ID_ALEFT, true );
            break;
        case KWParagLayout::CENTER:
            m_vToolBarText->setButton( ID_ACENTER, true );
            break;
        case KWParagLayout::RIGHT:
            m_vToolBarText->setButton( ID_ARIGHT, true );
            break;
        case KWParagLayout::BLOCK:
            m_vToolBarText->setButton( ID_ABLOCK, true );
            break;
        }
    }
}

/*================================================================*/
void KWordView::setLineSpacing( int _spc )
{
    if ( _spc != spc && m_vToolBarText )
    {
        spc = _spc;
        m_vToolBarText->setCurrentComboItem( ID_LINE_SPC, _spc );
    }
}

/*================================================================*/
void KWordView::setParagBorders( KWParagLayout::Border _left, KWParagLayout::Border _right,
                                 KWParagLayout::Border _top, KWParagLayout::Border _bottom )
{
    if ( ( left != _left || right != _right || top != _top || bottom != _bottom ) && m_vToolBarText )
    {
        m_vToolBarText->setButton( ID_BRD_LEFT, false );
        m_vToolBarText->setButton( ID_BRD_RIGHT, false );
        m_vToolBarText->setButton( ID_BRD_TOP, false );
        m_vToolBarText->setButton( ID_BRD_BOTTOM, false );

        left = _left;
        right = _right;
        top = _top;
        bottom = _bottom;

        if ( left.ptWidth > 0 )
        {
            m_vToolBarText->setButton( ID_BRD_LEFT, true );
            tmpBrd = left;
            setParagBorderValues();
        }
        if ( right.ptWidth > 0 )
        {
            m_vToolBarText->setButton( ID_BRD_RIGHT, true );
            tmpBrd = right;
            setParagBorderValues();
        }
        if ( top.ptWidth > 0 )
        {
            m_vToolBarText->setButton( ID_BRD_TOP, true );
            tmpBrd = top;
            setParagBorderValues();
        }
        if ( bottom.ptWidth > 0 )
        {
            m_vToolBarText->setButton( ID_BRD_BOTTOM, true );
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

    return false;
}

/*===============================================================*/
void KWordView::uncheckAllTools()
{
    if ( m_vMenuTools )
    {
        m_vMenuTools->setItemChecked( m_idMenuTools_Edit, false );
        m_vMenuTools->setItemChecked( m_idMenuTools_EditFrame, false );
        m_vMenuTools->setItemChecked( m_idMenuTools_CreateText, false );
        m_vMenuTools->setItemChecked( m_idMenuTools_CreatePix, false );
        m_vMenuTools->setItemChecked( m_idMenuTools_Clipart, false );
        m_vMenuTools->setItemChecked( m_idMenuTools_Table, false );
        m_vMenuTools->setItemChecked( m_idMenuTools_KSpreadTable, false );
        m_vMenuTools->setItemChecked( m_idMenuTools_Formula, false );
        m_vMenuTools->setItemChecked( m_idMenuTools_Part, false );
    }

    if ( m_vToolBarTools )
    {
        m_vToolBarTools->setButton( ID_TOOL_EDIT, false );
        m_vToolBarTools->setButton( ID_TOOL_EDIT_FRAME, false );
        m_vToolBarTools->setButton( ID_TOOL_CREATE_TEXT, false );
        m_vToolBarTools->setButton( ID_TOOL_CREATE_PIX, false );
        m_vToolBarTools->setButton( ID_TOOL_CREATE_CLIPART, false );
        m_vToolBarTools->setButton( ID_TOOL_CREATE_TABLE, false );
        m_vToolBarTools->setButton( ID_TOOL_CREATE_KSPREAD_TABLE, false );
        m_vToolBarTools->setButton( ID_TOOL_CREATE_FORMULA, false );
        m_vToolBarTools->setButton( ID_TOOL_CREATE_PART, false );
    }
}

/*===============================================================*/
void KWordView::setTool( MouseMode _mouseMode )
{
    if ( m_vMenuTools )
    {
        switch ( _mouseMode )
        {
        case MM_EDIT:
            m_vMenuTools->setItemChecked( m_idMenuTools_Edit, true );
            break;
        case MM_EDIT_FRAME:
            m_vMenuTools->setItemChecked( m_idMenuTools_EditFrame, true );
            break;
        case MM_CREATE_TEXT:
            m_vMenuTools->setItemChecked( m_idMenuTools_CreateText, true );
            break;
        case MM_CREATE_PIX:
            m_vMenuTools->setItemChecked( m_idMenuTools_CreatePix, true );
            break;
        case MM_CREATE_CLIPART:
            m_vMenuTools->setItemChecked( m_idMenuTools_Clipart, true );
            break;
        case MM_CREATE_TABLE:
            m_vMenuTools->setItemChecked( m_idMenuTools_Table, true );
            break;
        case MM_CREATE_KSPREAD_TABLE:
            m_vMenuTools->setItemChecked( m_idMenuTools_KSpreadTable, true );
            break;
        case MM_CREATE_FORMULA:
            m_vMenuTools->setItemChecked( m_idMenuTools_Formula, true );
            break;
        case MM_CREATE_PART:
            m_vMenuTools->setItemChecked( m_idMenuTools_Part, true );
            break;
        }
    }

    if ( m_vToolBarTools )
    {
        switch ( _mouseMode )
        {
        case MM_EDIT:
            m_vToolBarTools->setButton( ID_TOOL_EDIT, true );
            break;
        case MM_EDIT_FRAME:
            m_vToolBarTools->setButton( ID_TOOL_EDIT_FRAME, true );
            break;
        case MM_CREATE_TEXT:
            m_vToolBarTools->setButton( ID_TOOL_CREATE_TEXT, true );
            break;
        case MM_CREATE_PIX:
            m_vToolBarTools->setButton( ID_TOOL_CREATE_PIX, true );
            break;
        case MM_CREATE_CLIPART:
            m_vToolBarTools->setButton( ID_TOOL_CREATE_CLIPART, true );
            break;
        case MM_CREATE_TABLE:
            m_vToolBarTools->setButton( ID_TOOL_CREATE_TABLE, true );
            break;
        case MM_CREATE_KSPREAD_TABLE:
            m_vToolBarTools->setButton( ID_TOOL_CREATE_KSPREAD_TABLE, true );
            break;
        case MM_CREATE_FORMULA:
            m_vToolBarTools->setButton( ID_TOOL_CREATE_FORMULA, true );
            break;
        case MM_CREATE_PART:
            m_vToolBarTools->setButton( ID_TOOL_CREATE_PART, true );
            break;
        }
    }

    if ( m_vToolBarText && m_vToolBarFrame )
    {
        if ( _mouseMode == MM_EDIT_FRAME )
        {
            m_vToolBarFrame->setButton( ID_FBRD_LEFT, false );
            m_vToolBarFrame->setButton( ID_FBRD_RIGHT, false );
            m_vToolBarFrame->setButton( ID_FBRD_TOP, false );
            m_vToolBarFrame->setButton( ID_FBRD_BOTTOM, false );

            m_vToolBarFrame->setBarPos( oldFramePos );
            m_vToolBarText->enable( OpenPartsUI::Hide );
            m_vToolBarFrame->enable( OpenPartsUI::Show );
            oldTextPos = m_vToolBarText->barPos();
            m_vToolBarText->setBarPos( OpenPartsUI::Floating );
        }
        else
        {
            m_vToolBarText->setBarPos( oldTextPos );
            m_vToolBarText->enable( OpenPartsUI::Show );
            m_vToolBarFrame->enable( OpenPartsUI::Hide );
            if ( m_vToolBarFrame->barPos() != OpenPartsUI::Floating )
                oldFramePos = m_vToolBarFrame->barPos();
            m_vToolBarFrame->setBarPos( OpenPartsUI::Floating );
        }

        m_vMenuTable->setItemEnabled( m_idMenuTable_InsertRow, false );
        m_vToolBarTable->setItemEnabled( ID_TABLE_INSROW, false );
        m_vMenuTable->setItemEnabled( m_idMenuTable_DeleteRow, false );
        m_vToolBarTable->setItemEnabled( ID_TABLE_DELROW, false );
        m_vMenuTable->setItemEnabled( m_idMenuTable_InsertCol, false );
        m_vToolBarTable->setItemEnabled( ID_TABLE_INSCOL, false );
        m_vMenuTable->setItemEnabled( m_idMenuTable_DeleteCol, false );
        m_vToolBarTable->setItemEnabled( ID_TABLE_DELCOL, false );
        m_vMenuTable->setItemEnabled( m_idMenuTable_JoinCells, false );
        m_vMenuTable->setItemEnabled( m_idMenuTable_SplitCells, false );
        m_vMenuTable->setItemEnabled( m_idMenuTable_UngroupTable, false );

        switch ( _mouseMode )
        {
        case MM_EDIT:
        {
            m_vMenuTable->setItemEnabled( m_idMenuTable_InsertRow, true );
            m_vToolBarTable->setItemEnabled( ID_TABLE_INSROW, true );
            m_vMenuTable->setItemEnabled( m_idMenuTable_DeleteRow, true );
            m_vToolBarTable->setItemEnabled( ID_TABLE_DELROW, true );
            m_vMenuTable->setItemEnabled( m_idMenuTable_InsertCol, true );
            m_vToolBarTable->setItemEnabled( ID_TABLE_INSCOL, true );
            m_vMenuTable->setItemEnabled( m_idMenuTable_DeleteCol, true );
            m_vToolBarTable->setItemEnabled( ID_TABLE_DELCOL, true );
            m_vMenuTable->setItemEnabled( m_idMenuTable_UngroupTable, true );
        } break;
        case MM_EDIT_FRAME:
        {
            m_vMenuTable->setItemEnabled( m_idMenuTable_JoinCells, true );
            m_vMenuTable->setItemEnabled( m_idMenuTable_SplitCells, true );
        } break;
        default: break;
        }
    }
}

/*===============================================================*/
void KWordView::updateStyle( QString _styleName, bool _updateFormat )
{
    styleList.find( _styleName );

    if ( styleList.at() == -1 ) return;

    if ( !CORBA::is_nil( m_vToolBarText ) )
        m_vToolBarText->setCurrentComboItem( ID_STYLE_LIST, styleList.at() );

    m_vToolBarText->setButton( ID_USORT_LIST, false );
    m_vToolBarText->setButton( ID_ENUM_LIST, false );

    if ( _styleName == "Enumerated List" )
        m_vToolBarText->setButton( ID_ENUM_LIST, true );

    if ( _styleName == "Bullet List" )
        m_vToolBarText->setButton( ID_USORT_LIST, true );

    setFormat( m_pKWordDoc->findParagLayout( _styleName )->getFormat(), false, _updateFormat, false );

    gui->getHorzRuler()->setTabList( m_pKWordDoc->findParagLayout( _styleName )->getTabList() );
}

/*===============================================================*/
void KWordView::updateStyleList()
{
    m_vToolBarText->clearCombo( ID_STYLE_LIST );

    OpenPartsUI::StrList stylelist;
    styleList.clear();
    stylelist.length( m_pKWordDoc->paragLayoutList.count() );
    for ( unsigned int i = 0; i < m_pKWordDoc->paragLayoutList.count(); i++ )
    {
        styleList.append( QString( m_pKWordDoc->paragLayoutList.at( i )->getName() ) );
        stylelist[ i ] = CORBA::string_dup( m_pKWordDoc->paragLayoutList.at( i )->getName() );
    }
    m_vToolBarText->insertComboList( ID_STYLE_LIST, stylelist, 0 );
    updateStyle( gui->getPaperWidget()->getParagLayout()->getName() );
}

/*===============================================================*/
void KWordView::editUndo()
{
    m_pKWordDoc->undo();
    gui->getPaperWidget()->recalcWholeText( true );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::editRedo()
{
    m_pKWordDoc->redo();
    gui->getPaperWidget()->recalcWholeText( true );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::editCut()
{
    gui->getPaperWidget()->editCut();

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::editCopy()
{
    gui->getPaperWidget()->editCopy();

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::editPaste()
{
    QClipboard *cb = QApplication::clipboard();

    if ( cb->data()->provides( MIME_TYPE ) )
    {
        if ( cb->data()->encodedData( MIME_TYPE ).size() )
            gui->getPaperWidget()->editPaste( cb->data()->encodedData( MIME_TYPE ), MIME_TYPE );
    }
    else if ( cb->data()->provides( "text/plain" ) )
    {
        if ( cb->data()->encodedData( "text/plain" ).size() )
            gui->getPaperWidget()->editPaste( cb->data()->encodedData( "text/plain" ) );
    }
    else if ( !cb->text().isEmpty() )
        gui->getPaperWidget()->editPaste( cb->text() );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::editSelectAll()
{

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::editFind()
{
    if ( searchDia ) return;

    searchDia = new KWSearchDia( this, "", m_pKWordDoc, gui->getPaperWidget(), this, searchEntry, replaceEntry, fontList );
    searchDia->setCaption( i18n( "KWord - Search & Replace" ) );
    QObject::connect( searchDia, SIGNAL( cancelButtonPressed() ), this, SLOT( searchDiaClosed() ) );
    searchDia->show();

    sendFocusEvent();
}

/*================================================================*/
void KWordView::newView()
{
    assert( ( m_pKWordDoc != 0L ) );

    KWordShell* shell = new KWordShell;
    shell->show();
    shell->setDocument( m_pKWordDoc );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::viewFormattingChars()
{
    m_vMenuView->setItemChecked( m_idMenuView_FormattingChars, !m_vMenuView->isItemChecked( m_idMenuView_FormattingChars ) );
    _viewFormattingChars = m_vMenuView->isItemChecked( m_idMenuView_FormattingChars );
    gui->getPaperWidget()->repaintScreen( !_viewFormattingChars );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::viewFrameBorders()
{
    m_vMenuView->setItemChecked( m_idMenuView_FrameBorders, !m_vMenuView->isItemChecked( m_idMenuView_FrameBorders ) );
    _viewFrameBorders = m_vMenuView->isItemChecked( m_idMenuView_FrameBorders );
    gui->getPaperWidget()->repaintScreen( false );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::viewTableGrid()
{
    m_vMenuView->setItemChecked( m_idMenuView_TableGrid, !m_vMenuView->isItemChecked( m_idMenuView_TableGrid ) );
    _viewTableGrid = m_vMenuView->isItemChecked( m_idMenuView_TableGrid );
    gui->getPaperWidget()->repaintScreen( !_viewTableGrid );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::viewHeader()
{
    m_vMenuView->setItemChecked( m_idMenuView_Header, !m_vMenuView->isItemChecked( m_idMenuView_Header ) );
    m_pKWordDoc->setHeader( m_vMenuView->isItemChecked( m_idMenuView_Header ) );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::viewFooter()
{
    m_vMenuView->setItemChecked( m_idMenuView_Footer, !m_vMenuView->isItemChecked( m_idMenuView_Footer ) );
    m_pKWordDoc->setFooter( m_vMenuView->isItemChecked( m_idMenuView_Footer ) );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::viewDocStruct()
{
    m_vMenuView->setItemChecked( m_idMenuView_DocStruct, !m_vMenuView->isItemChecked( m_idMenuView_DocStruct ) );
    gui->showDocStruct( m_vMenuView->isItemChecked( m_idMenuView_DocStruct ) );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::viewFootNotes()
{
    m_vMenuEdit->setItemChecked( m_idMenuView_FootNotes, true );
    m_vMenuEdit->setItemChecked( m_idMenuView_EndNotes, false );

    m_pKWordDoc->setNoteType( KWFootNoteManager::FootNotes );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::viewEndNotes()
{
    m_vMenuEdit->setItemChecked( m_idMenuView_FootNotes, false );
    m_vMenuEdit->setItemChecked( m_idMenuView_EndNotes, true );

    m_pKWordDoc->setNoteType( KWFootNoteManager::EndNotes );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::insertPicture()
{
    QString file = KFilePreviewDialog::getOpenFileName( QString::null,
							kimgio_patterns(), 0);

    if ( !file.isEmpty() ) m_pKWordDoc->insertPicture( file, gui->getPaperWidget() );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::insertClipart()
{

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::insertSpecialChar()
{

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::insertFrameBreak()
{
    if ( gui->getPaperWidget()->getTable() ) return;

    QKeyEvent e(static_cast<QEvent::Type>(6) /*QEvent::KeyPress*/ ,Key_Return,0,ControlButton);
    gui->getPaperWidget()->keyPressEvent( &e );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::insertVariableDateFix()
{
    gui->getPaperWidget()->insertVariable( VT_DATE_FIX );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::insertVariableDateVar()
{
    gui->getPaperWidget()->insertVariable( VT_DATE_VAR );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::insertVariableTimeFix()
{
    gui->getPaperWidget()->insertVariable( VT_TIME_FIX );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::insertVariableTimeVar()
{
    gui->getPaperWidget()->insertVariable( VT_TIME_VAR );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::insertVariablePageNum()
{
    gui->getPaperWidget()->insertVariable( VT_PGNUM );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::insertVariableOther()
{

    sendFocusEvent();
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
        //debug( "NUMBER: %d", start );

        KWFootNoteDia dia( 0L, "", m_pKWordDoc, gui->getPaperWidget(), start );
        dia.setCaption( i18n( "Insert Footnote/Endnote" ) );
        dia.show();
    }

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::formatFont()
{

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::formatColor()
{

    sendFocusEvent();
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

    sendFocusEvent();
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

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::formatNumbering()
{

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::formatStyle()
{

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::formatFrameSet()
{
    if ( m_pKWordDoc->getFirstSelectedFrame() )
        gui->getPaperWidget()->femProps();
    else
        QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to select at least one frame!" ), i18n( "OK" ) );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::extraSpelling()
{
    currParag = 0L;
    currFrameSetNum = -1;
    kspell = new KSpell( this, i18n( "Spell Checking" ), this, SLOT( spellCheckerReady() ) );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::extraAutoFormat()
{
    KWAutoFormatDia dia( this, "", m_pKWordDoc, gui->getPaperWidget() );
    dia.setCaption( i18n( "Autocorrection" ) );
    dia.show();

    sendFocusEvent();
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

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::extraOptions()
{

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::toolsEdit()
{
    gui->getPaperWidget()->mmEdit();

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::toolsEditFrame()
{
    gui->getPaperWidget()->mmEditFrame();

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::toolsCreateText()
{
    gui->getPaperWidget()->mmCreateText();

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::toolsCreatePix()
{
    gui->getPaperWidget()->mmEdit();
    QString file = KFilePreviewDialog::getOpenFileName( QString::null,
							kimgio_patterns(), 0);

    if ( !file.isEmpty() )
    {
        gui->getPaperWidget()->mmCreatePix();
        gui->getPaperWidget()->setPixmapFilename( file );
    }
    else
        gui->getPaperWidget()->mmEdit();

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::toolsClipart()
{
    gui->getPaperWidget()->mmClipart();

    sendFocusEvent();
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

    sendFocusEvent();
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

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::toolsFormula()
{
    gui->getPaperWidget()->mmFormula();

    QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( "'IDL:KFormula/DocumentFactory:1.0#KFormula' in RepoIds", 1 );
    if ( vec.isEmpty() == 0 )
    {
        cout << "Got no results" << endl;
        QMessageBox::critical( this, i18n( "Error" ), i18n( "Sorry, no formula component registered" ), i18n( "OK" ) );
        return;
    }
    gui->getPaperWidget()->setPartEntry( vec[ 0 ] );

    sendFocusEvent();
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

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::tableInsertRow()
{
    gui->getPaperWidget()->mmEdit();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
        QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to put the cursor into a table to edit it!" ), i18n( "OK" ) );
    else
    {
        KWInsertDia dia( this, "", grpMgr, m_pKWordDoc, KWInsertDia::ROW, gui->getPaperWidget() );
        dia.setCaption( i18n( "Insert Row" ) );
        dia.show();
    }

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::tableInsertCol()
{
    gui->getPaperWidget()->mmEdit();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
        QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to put the cursor into a table to edit it!" ), i18n( "OK" ) );
    else
    {
        if ( grpMgr->getBoundingRect().right() + 62 > static_cast<int>( m_pKWordDoc->getPTPaperWidth() ) )
            QMessageBox::critical( this, i18n( "Error" ), i18n( "There is not enough space at the right of the table\nto insert a new column." ),
                                   i18n( "OK" ) );
        else
        {
            KWInsertDia dia( this, "", grpMgr, m_pKWordDoc, KWInsertDia::COL, gui->getPaperWidget() );
            dia.setCaption( i18n( "Insert Column" ) );
            dia.show();
        }
    }

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::tableDeleteRow()
{
    gui->getPaperWidget()->mmEdit();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
        QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to put the cursor into a table to edit it!" ), i18n( "OK" ) );
    else
    {
        if ( grpMgr->getRows() == 1 )
            QMessageBox::critical( this, i18n( "Error" ), i18n( "The table has only one row. You can't delete the last one!" ), i18n( "OK" ) );
        else
        {
            KWDeleteDia dia( this, "", grpMgr, m_pKWordDoc, KWDeleteDia::ROW, gui->getPaperWidget() );
            dia.setCaption( i18n( "Delete Row" ) );
            dia.show();
        }
    }

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::tableDeleteCol()
{
    gui->getPaperWidget()->mmEdit();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
        QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to put the cursor into a table to edit it!" ), i18n( "OK" ) );
    else
    {
        if ( grpMgr->getCols() == 1 )
            QMessageBox::critical( this, i18n( "Error" ), i18n( "The table has only one column. You can't delete the last one!" ), i18n( "OK" ) );
        else
        {
            KWDeleteDia dia( this, "", grpMgr, m_pKWordDoc, KWDeleteDia::COL, gui->getPaperWidget() );
            dia.setCaption( i18n( "Delete Column" ) );
            dia.show();
        }
    }

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::tableJoinCells()
{
    gui->getPaperWidget()->mmEditFrame();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getCurrentTable();
    if ( !grpMgr )
        QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to select some cells in a table to join them!" ), i18n( "OK" ) );
    else
    {
        QPainter painter;
        painter.begin( gui->getPaperWidget() );
        if ( !grpMgr->joinCells() )
            QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to select some cells which are next to each other\n"
                                                                "and are not already joined." ), i18n( "OK" ) );
        painter.end();
        QRect r = grpMgr->getBoundingRect();
        r = QRect( r.x() - gui->getPaperWidget()->contentsX(),
                   r.y() - gui->getPaperWidget()->contentsY(),
                   r.width(), r.height() );
        gui->getPaperWidget()->repaintScreen( r, true );
    }

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::tableSplitCells()
{
    gui->getPaperWidget()->mmEditFrame();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getCurrentTable();
    if ( !grpMgr )
        QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to select a cell in a table to split it!" ), i18n( "OK" ) );
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
        gui->getPaperWidget()->repaintScreen( r, true );
    }

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::tableUngroupTable()
{
    gui->getPaperWidget()->mmEdit();

    KWGroupManager *grpMgr = gui->getPaperWidget()->getTable();
    if ( !grpMgr )
        QMessageBox::critical( this, i18n( "Error" ), i18n( "You have to put the cursor into a table to edit it!" ), i18n( "OK" ) );
    else
    {
        if ( QMessageBox::warning( this, i18n( "Warning" ), i18n( "Ungrouping a table is an irrevesible action!\n"
                                                                "Do you really want to do that?" ), i18n( "Yes" ), i18n( "No" ) ) == 0 )
        {
            grpMgr->ungroup();
            QRect r = grpMgr->getBoundingRect();
            r = QRect( r.x() - gui->getPaperWidget()->contentsX(),
                       r.y() - gui->getPaperWidget()->contentsY(),
                       r.width(), r.height() );
            gui->getPaperWidget()->repaintScreen( r, true );
        }
    }

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::helpContents()
{

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::helpAbout()
{
    QMessageBox::information( this, "KWord",
                              i18n( "KWord 0.0.1 alpha\n\n"
                                    "( c ) by Torben Weis <weis@kde.org> and \n"
                                    "Reginald Stadlbauer <reggie@kde.org> 1998\n\n"
                                    "KWord is under GNU GPL" ) );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::helpAboutKOffice()
{

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::helpAboutKDE()
{

    sendFocusEvent();
}

/*====================== text style selected  ===================*/
void KWordView::textStyleSelected( const char *style )
{
    gui->getPaperWidget()->applyStyle( style );
    format = m_pKWordDoc->findParagLayout( style )->getFormat();
    gui->getPaperWidget()->formatChanged( format );
    updateStyle( style, false );

    sendFocusEvent();
}

/*======================= text size selected  ===================*/
void KWordView::textSizeSelected( const char *size )
{
    tbFont.setPointSize( atoi( size ) );
    format.setPTFontSize( atoi( size ) );
    gui->getPaperWidget()->formatChanged( format );

    sendFocusEvent();
}

/*======================= text font selected  ===================*/
void KWordView::textFontSelected( const char *font )
{
    tbFont.setFamily( font );
    format.setUserFont( m_pKWordDoc->findUserFont( font ) );
    gui->getPaperWidget()->formatChanged( format );

    sendFocusEvent();
}

/*========================= text bold ===========================*/
void KWordView::textBold()
{
    tbFont.setBold( !tbFont.bold() );
    format.setWeight( tbFont.bold() ? QFont::Bold : QFont::Normal );
    gui->getPaperWidget()->formatChanged( format );

    sendFocusEvent();
}

/*========================== text italic ========================*/
void KWordView::textItalic()
{
    tbFont.setItalic( !tbFont.italic() );
    format.setItalic( tbFont.italic() ? 1 : 0 );
    gui->getPaperWidget()->formatChanged( format );

    sendFocusEvent();
}

/*======================== text underline =======================*/
void KWordView::textUnderline()
{
    tbFont.setUnderline( !tbFont.underline() );
    format.setUnderline( tbFont.underline() ? 1 : 0 );
    gui->getPaperWidget()->formatChanged( format );

    sendFocusEvent();
}

/*=========================== text color ========================*/
void KWordView::textColor()
{
    if ( KColorDialog::getColor( tbColor ) )
    {
        OpenPartsUI::Pixmap pix;
        pix.data = CORBA::string_dup( colorToPixString( tbColor, TXT_COLOR ) );

        m_vToolBarText->setButtonPixmap( ID_TEXT_COLOR, pix );
        format.setColor( tbColor );
        gui->getPaperWidget()->formatChanged( format );
    }

    sendFocusEvent();
}

/*======================= text align left =======================*/
void KWordView::textAlignLeft()
{
    flow = KWParagLayout::LEFT;
    m_vToolBarText->setButton( ID_ALEFT, true );
    m_vToolBarText->setButton( ID_ACENTER, false );
    m_vToolBarText->setButton( ID_ARIGHT, false );
    m_vToolBarText->setButton( ID_ABLOCK, false );
    gui->getPaperWidget()->setFlow( KWParagLayout::LEFT );

    sendFocusEvent();
}

/*======================= text align center =====================*/
void KWordView::textAlignCenter()
{
    flow = KWParagLayout::CENTER;
    m_vToolBarText->setButton( ID_ALEFT, false );
    m_vToolBarText->setButton( ID_ACENTER, true );
    m_vToolBarText->setButton( ID_ARIGHT, false );
    m_vToolBarText->setButton( ID_ABLOCK, false );
    gui->getPaperWidget()->setFlow( KWParagLayout::CENTER );

    sendFocusEvent();
}

/*======================= text align right ======================*/
void KWordView::textAlignRight()
{
    flow = KWParagLayout::RIGHT;
    m_vToolBarText->setButton( ID_ALEFT, false );
    m_vToolBarText->setButton( ID_ACENTER, false );
    m_vToolBarText->setButton( ID_ARIGHT, true );
    m_vToolBarText->setButton( ID_ABLOCK, false );
    gui->getPaperWidget()->setFlow( KWParagLayout::RIGHT );

    sendFocusEvent();
}

/*======================= text align block ======================*/
void KWordView::textAlignBlock()
{
    flow = KWParagLayout::BLOCK;
    m_vToolBarText->setButton( ID_ARIGHT, false );
    m_vToolBarText->setButton( ID_ACENTER, false );
    m_vToolBarText->setButton( ID_ALEFT, false );
    m_vToolBarText->setButton( ID_ABLOCK, true );
    gui->getPaperWidget()->setFlow( KWParagLayout::BLOCK );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::textLineSpacing( const char *spc )
{
    KWUnit u;
    u.setPT( atoi( spc ) );
    gui->getPaperWidget()->setLineSpacing( u );

    sendFocusEvent();
}

/*====================== enumerated list ========================*/
void KWordView::textEnumList()
{
    m_vToolBarText->setButton( ID_USORT_LIST, false );
    if ( m_vToolBarText->isButtonOn( ID_ENUM_LIST ) )
        gui->getPaperWidget()->setEnumList();
    else
        gui->getPaperWidget()->setNormalText();

    sendFocusEvent();
}

/*====================== unsorted list ==========================*/
void KWordView::textUnsortList()
{
    m_vToolBarText->setButton( ID_ENUM_LIST, false );
    if ( m_vToolBarText->isButtonOn( ID_USORT_LIST ) )
        gui->getPaperWidget()->setBulletList();
    else
        gui->getPaperWidget()->setNormalText();

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::textSuperScript()
{
    m_vToolBarText->setButton( ID_SUBSCRIPT, false );
    if ( m_vToolBarText->isButtonOn( ID_SUPERSCRIPT ) )
        vertAlign = KWFormat::VA_SUPER;
    else
        vertAlign = KWFormat::VA_NORMAL;
    format.setVertAlign( vertAlign );
    gui->getPaperWidget()->formatChanged( format );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::textSubScript()
{
    m_vToolBarText->setButton( ID_SUPERSCRIPT, false );
    if ( m_vToolBarText->isButtonOn( ID_SUBSCRIPT ) )
        vertAlign = KWFormat::VA_SUB;
    else
        vertAlign = KWFormat::VA_NORMAL;
    format.setVertAlign( vertAlign );
    gui->getPaperWidget()->formatChanged( format );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::textBorderLeft()
{
    if ( m_vToolBarText->isButtonOn( ID_BRD_LEFT ) )
        left = tmpBrd;
    else
        left.ptWidth = 0;

    gui->getPaperWidget()->setParagLeftBorder( left );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::textBorderRight()
{
    if ( m_vToolBarText->isButtonOn( ID_BRD_RIGHT ) )
        right = tmpBrd;
    else
        right.ptWidth = 0;

    gui->getPaperWidget()->setParagRightBorder( right );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::textBorderTop()
{
    if ( m_vToolBarText->isButtonOn( ID_BRD_TOP ) )
        top = tmpBrd;
    else
        top.ptWidth = 0;

    gui->getPaperWidget()->setParagTopBorder( top );

    sendFocusEvent();
}

/*===============================================================*/
void KWordView::textBorderBottom()
{
    if ( m_vToolBarText->isButtonOn( ID_BRD_BOTTOM ) )
        bottom = tmpBrd;
    else
        bottom.ptWidth = 0;

    gui->getPaperWidget()->setParagBottomBorder( bottom );

    sendFocusEvent();
}

/*================================================================*/
void KWordView::textBorderColor()
{
    if ( KColorDialog::getColor( tmpBrd.color ) )
    {
        OpenPartsUI::Pixmap pix;
        pix.data = CORBA::string_dup( colorToPixString( tmpBrd.color, FRAME_COLOR ) );
        m_vToolBarText->setButtonPixmap( ID_BORDER_COLOR, pix );
    }

    sendFocusEvent();
}

/*================================================================*/
void KWordView::textBorderWidth( const char *width )
{
    tmpBrd.ptWidth = atoi( width );

    sendFocusEvent();
}

/*================================================================*/
void KWordView::textBorderStyle( const char *style )
{
    QString stl( style );

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


    sendFocusEvent();
}

/*================================================================*/
void KWordView::frameBorderLeft()
{
    gui->getPaperWidget()->setLeftFrameBorder( frmBrd, m_vToolBarFrame->isButtonOn( ID_FBRD_LEFT ) );

    sendFocusEvent();
}

/*================================================================*/
void KWordView::frameBorderRight()
{
    gui->getPaperWidget()->setRightFrameBorder( frmBrd, m_vToolBarFrame->isButtonOn( ID_FBRD_RIGHT ) );

    sendFocusEvent();
}

/*================================================================*/
void KWordView::frameBorderTop()
{
    gui->getPaperWidget()->setTopFrameBorder( frmBrd, m_vToolBarFrame->isButtonOn( ID_FBRD_TOP ) );

    sendFocusEvent();
}

/*================================================================*/
void KWordView::frameBorderBottom()
{
    gui->getPaperWidget()->setBottomFrameBorder( frmBrd, m_vToolBarFrame->isButtonOn( ID_FBRD_BOTTOM ) );

    sendFocusEvent();
}

/*================================================================*/
void KWordView::frameBorderColor()
{
    if ( KColorDialog::getColor( frmBrd.color ) )
    {
        OpenPartsUI::Pixmap pix;
        pix.data = CORBA::string_dup( colorToPixString( frmBrd.color, FRAME_COLOR ) );
        m_vToolBarFrame->setButtonPixmap( ID_FBORDER_COLOR, pix );
    }

    sendFocusEvent();
}

/*================================================================*/
void KWordView::frameBorderWidth( const char *width )
{
    frmBrd.ptWidth = atoi( width );

    sendFocusEvent();
}

/*================================================================*/
void KWordView::frameBorderStyle( const char *style )
{
    QString stl( style );

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

    sendFocusEvent();
}

/*================================================================*/
void KWordView::frameBackColor()
{
    QColor c = backColor.color();
    if ( KColorDialog::getColor( c ) )
    {
        backColor.setColor( c );
        OpenPartsUI::Pixmap pix;
        pix.data = CORBA::string_dup( colorToPixString( backColor.color(), BACK_COLOR ) );
        m_vToolBarFrame->setButtonPixmap( ID_FBACK_COLOR, pix );
        gui->getPaperWidget()->setFrameBackgroundColor( backColor );
    }

    sendFocusEvent();
}


/*================================================================*/
void KWordView::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    if ( gui ) gui->resize( width(), height() );

    sendFocusEvent();
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
    if ( CORBA::is_nil( _menubar ) )
    {
        m_vMenuEdit = 0L;
        m_vMenuView = 0L;
        m_vMenuInsert = 0L;
        m_vMenuFormat = 0L;
        m_vMenuExtra = 0L;
        m_vMenuHelp = 0L;
        return true;
    }

    CORBA::WString_var text;

    // edit menu
    text = Q2C( i18n( "&Edit" ) );
    _menubar->insertMenu( text, m_vMenuEdit, -1, -1 );

    OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( ICON( "undo.xpm" ) );
    text = Q2C( i18n( "No Undo possible" ) );
    m_idMenuEdit_Undo = m_vMenuEdit->insertItem6( pix, text, this, "editUndo", CTRL + Key_Z, -1, -1 );
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, false );

    pix = OPUIUtils::convertPixmap( ICON( "redo.xpm" ) );
    text = Q2C( i18n( "No Redo possible" ) );
    m_idMenuEdit_Redo = m_vMenuEdit->insertItem6( pix, text, this, "editRedo", 0, -1, -1 );
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, false );
    m_vMenuEdit->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( ICON( "editcut.xpm" ) );
    text = Q2C( i18n( "&Cut" ) );
    m_idMenuEdit_Cut = m_vMenuEdit->insertItem6( pix, text, this, "editCut", CTRL + Key_X, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "editcopy.xpm" ) );
    text = Q2C( i18n( "&Copy" ) );
    m_idMenuEdit_Copy = m_vMenuEdit->insertItem6( pix, text, this, "editCopy", CTRL + Key_C, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "editpaste.xpm" ) );
    text = Q2C( i18n( "&Paste" ) );
    m_idMenuEdit_Paste = m_vMenuEdit->insertItem6( pix, text, this, "editPaste", CTRL + Key_V, -1, -1 );

    m_vMenuEdit->insertSeparator( -1 );
    pix = OPUIUtils::convertPixmap( ICON( "kwsearch.xpm" ) );
    text = Q2C( i18n( "&Find and Replace..." ) );
    m_idMenuEdit_Find = m_vMenuEdit->insertItem6( pix, text, this, "editFind", CTRL + Key_F, -1, -1 );

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
    text = Q2C( i18n( "&Document Structure" ) );
    m_idMenuView_DocStruct = m_vMenuView->insertItem4( text, this, "viewDocStruct", 0, -1, -1 );
    m_vMenuView->insertSeparator( -1 );
    text = Q2C( i18n( "&Footnotes" ) );
    m_idMenuView_FootNotes = m_vMenuView->insertItem4( text, this, "viewFootNotes", 0, -1, -1 );
    text = Q2C( i18n( "&Endnotes" ) );
    m_idMenuView_EndNotes = m_vMenuView->insertItem4( text, this, "viewEndNotes", 0, -1, -1 );

    m_vMenuView->setCheckable( true );
    m_vMenuView->setItemChecked( m_idMenuView_FrameBorders, true );
    m_vMenuView->setItemChecked( m_idMenuView_TableGrid, true );
    m_vMenuView->setItemChecked( m_idMenuView_Header, m_pKWordDoc->hasHeader() );
    m_vMenuView->setItemChecked( m_idMenuView_Footer, m_pKWordDoc->hasFooter() );
    m_vMenuView->setItemChecked( m_idMenuView_DocStruct, false );
    m_vMenuView->setItemChecked( m_idMenuView_FootNotes, false );
    m_vMenuView->setItemChecked( m_idMenuView_EndNotes, true );

    // insert menu
    text = Q2C( i18n( "&Insert" ) );
    _menubar->insertMenu( text, m_vMenuInsert, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "picture.xpm" ) );
    text = Q2C( i18n( "&Picture..." ) );
    m_idMenuInsert_Picture = m_vMenuInsert->insertItem6( pix, text, this, "insertPicture", Key_F2, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "clipart.xpm" ) );
    text = Q2C( i18n( "&Clipart..." ) );
    m_idMenuInsert_Clipart = m_vMenuInsert->insertItem6( pix, text, this, "insertClipart", Key_F3, -1, -1 );

    m_vMenuInsert->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( ICON( "char.xpm" ) );
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
    text = Q2C( i18n( "Other..." ) );
    m_idMenuInsert_VariableOther = m_vMenuInsert_Variable->insertItem4( text, this, "insertVariableOther", 0, -1, -1 );

    // tools menu
    text = Q2C( i18n( "&Tools" ) );
    _menubar->insertMenu( text, m_vMenuTools, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "edittool.xpm" ) );
    text = Q2C( i18n( "&Edit Text" ) );
    m_idMenuTools_Edit = m_vMenuTools->insertItem6( pix, text, this, "toolsEdit", Key_F4, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "editframetool.xpm" ) );
    text = Q2C( i18n( "&Edit Frames" ) );
    m_idMenuTools_EditFrame = m_vMenuTools->insertItem6( pix, text, this, "toolsEditFrame", Key_F5, -1, -1 );

    m_vMenuTools->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( ICON( "textframetool.xpm" ) );
    text = Q2C( i18n( "&Create Text Frame" ) );
    m_idMenuTools_CreateText = m_vMenuTools->insertItem6( pix, text, this, "toolsCreateText", Key_F6, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "picframetool.xpm" ) );
    text = Q2C( i18n( "&Create Picture Frame" ) );
    m_idMenuTools_CreatePix = m_vMenuTools->insertItem6( pix, text, this, "toolsCreatePix", Key_F7, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "clipart.xpm" ) );
    text = Q2C( i18n( "&Create Clipart Frame" ) );
    m_idMenuTools_Clipart = m_vMenuTools->insertItem6( pix, text, this, "toolsClipart", Key_F8, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "table.xpm" ) );
    text = Q2C( i18n( "&Create Table Frame" ) );
    m_idMenuTools_Table = m_vMenuTools->insertItem6( pix, text, this, "toolsTable", Key_F9, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "table.xpm" ) );
    text = Q2C( i18n( "&Create KSpread Table Frame" ) );
    m_idMenuTools_KSpreadTable = m_vMenuTools->insertItem6( pix, text, this, "toolsKSpreadTable", Key_F10, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "formula.xpm" ) );
    text = Q2C( i18n( "&Create Formula Frame" ) );
    m_idMenuTools_Formula = m_vMenuTools->insertItem6( pix, text, this, "toolsFormula", Key_F11, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "parts.xpm" ) );
    text = Q2C( i18n( "&Create Part Frame" ) );
    m_idMenuTools_Part = m_vMenuTools->insertItem6( pix, text, this, "toolsPart", Key_F12, -1, -1 );

    m_vMenuTools->setCheckable( true );
    m_vMenuTools->setItemChecked( m_idMenuTools_Edit, true );

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

    pix = OPUIUtils::convertPixmap( ICON( "rowin.xpm" ) );
    text = Q2C( i18n( "&Insert Row..." ) );
    m_idMenuTable_InsertRow = m_vMenuTable->insertItem6( pix, text, this, "tableInsertRow", 0, -1, -1 );
    pix = OPUIUtils::convertPixmap( ICON( "colin.xpm" ) );
    text = Q2C( i18n( "I&nsert Column..." ) );
    m_idMenuTable_InsertCol = m_vMenuTable->insertItem6( pix, text, this, "tableInsertCol", 0, -1, -1 );
    pix = OPUIUtils::convertPixmap( ICON( "rowout.xpm" ) );
    text = Q2C( i18n( "&Delete Row..." ) );
    m_idMenuTable_DeleteRow = m_vMenuTable->insertItem6( pix, text, this, "tableDeleteRow", 0, -1, -1 );
    pix = OPUIUtils::convertPixmap( ICON( "colout.xpm" ) );
    text = Q2C( i18n( "&Delete Column..." ) );
    m_idMenuTable_DeleteCol = m_vMenuTable->insertItem6( pix, text, this, "tableDeleteCol", 0, -1, -1 );

    m_vMenuTable->insertSeparator( -1 );

    text = Q2C( i18n( "&Join Cells" ) );
    m_idMenuTable_JoinCells = m_vMenuTable->insertItem4( text, this, "tableJoinCells", 0, -1, -1 );
    text = Q2C( i18n( "&Split Cells" ) );
    m_idMenuTable_SplitCells = m_vMenuTable->insertItem4( text, this, "tableSplitCells", 0, -1, -1 );
    text = Q2C( i18n( "&Ungroup Table" ) );
    m_idMenuTable_UngroupTable = m_vMenuTable->insertItem4( text, this, "tableUngroupTable", 0, -1, -1 );

    // extra menu
    text = Q2C( i18n( "&Extra" ) );
    _menubar->insertMenu( text, m_vMenuExtra, -1, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "spellcheck.xpm" ) );
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
    QObject::connect( m_pKWordDoc, SIGNAL( selectionOnOff() ), this, SLOT( selectionOnOff() ) );

    return true;
}

/*======================= setup edit toolbar ===================*/
bool KWordView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
    if ( CORBA::is_nil( _factory ) )
    {
        m_vToolBarEdit = 0L;
        m_vToolBarText = 0L;
        m_vToolBarInsert = 0L;
        return true;
    }

    m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

    m_vToolBarEdit->setFullWidth( false );

    // undo
    OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( ICON( "undo.xpm" ) );
    CORBA::WString_var toolTip = Q2C( i18n( "Undo" ) );
    m_idButtonEdit_Undo = m_vToolBarEdit->insertButton2( pix, ID_UNDO, SIGNAL( clicked() ), this, "editUndo", true, toolTip, -1 );
    m_vToolBarEdit->setItemEnabled( ID_UNDO, false );

    // redo
    pix = OPUIUtils::convertPixmap( ICON( "redo.xpm" ) );
    toolTip = Q2C( i18n( "Redo" ) );
    m_idButtonEdit_Redo = m_vToolBarEdit->insertButton2( pix, ID_REDO, SIGNAL( clicked() ), this, "editRedo", true, toolTip, -1 );
    m_vToolBarEdit->setItemEnabled( ID_REDO, false );

    m_vToolBarEdit->insertSeparator( -1 );

    // cut
    pix = OPUIUtils::convertPixmap( ICON( "editcut.xpm" ) );
    toolTip = Q2C( i18n( "Cut" ) );
    m_idButtonEdit_Cut = m_vToolBarEdit->insertButton2( pix, ID_EDIT_CUT, SIGNAL( clicked() ), this,
                                                        "editCut", true, toolTip, -1 );

    // copy
    pix = OPUIUtils::convertPixmap( ICON( "editcopy.xpm" ) );
    toolTip = Q2C( i18n( "Copy" ) );
    m_idButtonEdit_Copy = m_vToolBarEdit->insertButton2( pix, ID_EDIT_COPY, SIGNAL( clicked() ), this,
                                                         "editCopy", true, toolTip, -1 );

    // paste
    pix = OPUIUtils::convertPixmap( ICON( "editpaste.xpm" ) );
    toolTip = Q2C( i18n( "Paste" ) );
    m_idButtonEdit_Paste = m_vToolBarEdit->insertButton2( pix, ID_EDIT_PASTE, SIGNAL( clicked() ), this,
                                                          "editPaste", true, toolTip, -1 );

    m_vToolBarEdit->insertSeparator( -1 );

    // spelling
    pix = OPUIUtils::convertPixmap( ICON( "spellcheck.xpm" ) );
    toolTip = Q2C( i18n( "Spell Checking" ) );
    m_idButtonEdit_Spelling = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraSpelling",
                                                             true, toolTip, -1 );

    m_vToolBarEdit->insertSeparator( -1 );

    // find
    pix = OPUIUtils::convertPixmap( ICON( "kwsearch.xpm" ) );
    toolTip = Q2C( i18n( "Find & Replace" ) );
    m_idButtonEdit_Find = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editFind",
                                                         true, toolTip, -1 );

    m_vToolBarEdit->enable( OpenPartsUI::Show );

    // TOOLBAR Insert
    m_vToolBarInsert = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarInsert->setFullWidth( false );

    // picture
    pix = OPUIUtils::convertPixmap( ICON( "picture.xpm" ) );
    toolTip = Q2C( i18n( "Insert Picture" ) );
    m_idButtonInsert_Picture = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertPicture", true, toolTip, -1 );

    // clipart
    pix = OPUIUtils::convertPixmap( ICON( "clipart.xpm" ) );
    toolTip = Q2C( i18n( "Insert Clipart" ) );
    m_idButtonInsert_Clipart = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertClipart", true, toolTip, -1 );

    m_vToolBarInsert->insertSeparator( -1 );

    // special char
    pix = OPUIUtils::convertPixmap( ICON( "char.xpm" ) );
    toolTip = Q2C( i18n( "Insert Special Character" ) );
    m_idButtonInsert_SpecialChar = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertSpecialChar",
                                                                    true, toolTip, -1 );

    m_vToolBarInsert->enable( OpenPartsUI::Show );

    // TOOLBAR table
    m_vToolBarTable = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarTable->setFullWidth( false );

    pix = OPUIUtils::convertPixmap( ICON( "rowin.xpm" ) );
    toolTip = Q2C( i18n( "Insert Row" ) );
    m_idButtonTable_InsertRow = m_vToolBarTable->insertButton2( pix, ID_TABLE_INSROW, SIGNAL( clicked() ), this, "tableInsertRow",
                                                                true, toolTip, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "colin.xpm" ) );
    toolTip = Q2C( i18n( "Insert Column" ) );
    m_idButtonTable_InsertCol = m_vToolBarTable->insertButton2( pix, ID_TABLE_INSCOL, SIGNAL( clicked() ), this, "tableInsertCol",
                                                                true, toolTip, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "rowout.xpm" ) );
    toolTip = Q2C( i18n( "Delete Row" ) );
    m_idButtonTable_DeleteRow = m_vToolBarTable->insertButton2( pix, ID_TABLE_DELROW, SIGNAL( clicked() ), this, "tableDeleteRow",
                                                                true, toolTip, -1 );

    pix = OPUIUtils::convertPixmap( ICON( "colout.xpm" ) );
    toolTip = Q2C( i18n( "Delete Column" ) );
    m_idButtonTable_DeleteCol = m_vToolBarTable->insertButton2( pix, ID_TABLE_DELCOL, SIGNAL( clicked() ), this, "tableDeleteCol",
                                                                true, toolTip, -1 );

    m_vToolBarTable->enable( OpenPartsUI::Show );

    // TOOLBAR Tools
    m_vToolBarTools = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarTools->setFullWidth( false );

    // edit
    pix = OPUIUtils::convertPixmap( ICON( "edittool.xpm" ) );
    toolTip = Q2C( i18n( "Edit Text Tool" ) );
    m_idButtonTools_Edit = m_vToolBarTools->insertButton2( pix, ID_TOOL_EDIT, SIGNAL( clicked() ), this, "toolsEdit",
                                                           true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_EDIT, true );
    m_vToolBarTools->setButton( ID_TOOL_EDIT, true );

    // edit frame
    pix = OPUIUtils::convertPixmap( ICON( "editframetool.xpm" ) );
    toolTip = Q2C( i18n( "Edit Frames Tool" ) );
    m_idButtonTools_EditFrame = m_vToolBarTools->insertButton2( pix, ID_TOOL_EDIT_FRAME, SIGNAL( clicked() ), this, "toolsEditFrame",
                                                                true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_EDIT_FRAME, true );

    // create text frame
    pix = OPUIUtils::convertPixmap( ICON( "textframetool.xpm" ) );
    toolTip = Q2C( i18n( "Create Text Frame" ) );
    m_idButtonTools_CreateText = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_TEXT, SIGNAL( clicked() ), this, "toolsCreateText",
                                                                 true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_TEXT, true );

    // create pix frame
    pix = OPUIUtils::convertPixmap( ICON( "picframetool.xpm" ) );
    toolTip = Q2C( i18n( "Create Picture Frame" ) );
    m_idButtonTools_CreatePix = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_PIX, SIGNAL( clicked() ), this, "toolsCreatePix",
                                                                true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_PIX, true );

    // create clip frame
    pix = OPUIUtils::convertPixmap( ICON( "clipart.xpm" ) );
    toolTip = Q2C( i18n( "Create Clipart Frame" ) );
    m_idButtonTools_Clipart = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_CLIPART, SIGNAL( clicked() ), this, "toolsClipart",
                                                              true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_CLIPART, true );

    // create table frame
    pix = OPUIUtils::convertPixmap( ICON( "table.xpm" ) );
    toolTip = Q2C( i18n( "Create Table Frame" ) );
    m_idButtonTools_Table = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_TABLE, SIGNAL( clicked() ), this, "toolsTable",
                                                            true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_TABLE, true );

    // create table frame
    pix = OPUIUtils::convertPixmap( ICON( "table.xpm" ) );
    toolTip = Q2C( i18n( "Create KSPread Table Frame" ) );
    m_idButtonTools_KSpreadTable = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_KSPREAD_TABLE, SIGNAL( clicked() ), this,
                                                                   "toolsKSpreadTable", true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_KSPREAD_TABLE, true );

    // create formula frame
    pix = OPUIUtils::convertPixmap( ICON( "formula.xpm" ) );
    toolTip = Q2C( i18n( "Create Formula Frame" ) );
    m_idButtonTools_Formula = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_FORMULA, SIGNAL( clicked() ), this, "toolsFormula",
                                                              true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_FORMULA, true );

    // create part frame
    pix = OPUIUtils::convertPixmap( ICON( "parts.xpm" ) );
    toolTip = Q2C( i18n( "Create Part Frame" ) );
    m_idButtonTools_Part = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_PART, SIGNAL( clicked() ), this, "toolsPart",
                                                           true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_CREATE_PART, true );

    m_vToolBarTools->enable( OpenPartsUI::Show );

    // TOOLBAR Text
    m_vToolBarText = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarText->setFullWidth( false );

    // style combobox
    OpenPartsUI::StrList stylelist;
    stylelist.length( m_pKWordDoc->paragLayoutList.count() );
    for ( unsigned int i = 0; i < m_pKWordDoc->paragLayoutList.count(); i++ )
    {
        styleList.append( QString( m_pKWordDoc->paragLayoutList.at( i )->getName() ) );
        stylelist[ i ] = CORBA::string_dup( m_pKWordDoc->paragLayoutList.at( i )->getName() );
    }
    toolTip = Q2C( i18n( "Style" ) );
    m_idComboText_Style = m_vToolBarText->insertCombo( stylelist, ID_STYLE_LIST, false, SIGNAL( activated( const QString & ) ),
                                                       this, "textStyleSelected", true, toolTip,
                                                       200, -1, OpenPartsUI::AtBottom );

    // size combobox
    OpenPartsUI::StrList sizelist;
    sizelist.length( 97 );
    for( int i = 4; i <= 100; i++ )
    {
        char buffer[ 10 ];
        sprintf( buffer, "%i", i );
        sizelist[ i - 4 ] = CORBA::string_dup( buffer );
    }
    toolTip = Q2C( i18n( "Font Size" ) );
    m_idComboText_FontSize = m_vToolBarText->insertCombo( sizelist, ID_FONT_SIZE, true, SIGNAL( activated( const QString & ) ),
                                                          this, "textSizeSelected", true,
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
    m_idComboText_FontList = m_vToolBarText->insertCombo( fonts, ID_FONT_LIST, true, SIGNAL( activated( const QString & ) ), this,
                                                          "textFontSelected", true, toolTip,
                                                          200, -1, OpenPartsUI::AtBottom );
    tbFont.setFamily( fontList[ 0 ] );
    m_vToolBarText->setCurrentComboItem( ID_FONT_LIST, 0 );

    m_vToolBarText->insertSeparator( -1 );

    // bold
    pix = OPUIUtils::convertPixmap( ICON( "bold.xpm" ) );
    toolTip = Q2C( i18n( "Bold" ) );
    m_idButtonText_Bold = m_vToolBarText->insertButton2( pix, ID_BOLD, SIGNAL( clicked() ), this, "textBold", true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_BOLD, true );
    m_vToolBarText->setButton( ID_BOLD, false );
    tbFont.setBold( false );

    // italic
    pix = OPUIUtils::convertPixmap( ICON( "italic.xpm" ) );
    toolTip = Q2C( i18n( "Italic" ) );
    m_idButtonText_Italic = m_vToolBarText->insertButton2( pix, ID_ITALIC, SIGNAL( clicked() ), this, "textItalic", true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ITALIC, true );
    m_vToolBarText->setButton( ID_ITALIC, false );
    tbFont.setItalic( false );

    // underline
    pix = OPUIUtils::convertPixmap( ICON( "underl.xpm" ) );
    toolTip = Q2C( i18n( "Underline" ) );
    m_idButtonText_Underline = m_vToolBarText->insertButton2( pix, ID_UNDERLINE, SIGNAL( clicked() ), this, "textUnderline", true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_UNDERLINE, true );
    m_vToolBarText->setButton( ID_UNDERLINE, false );
    tbFont.setUnderline( false );

    // color
    tbColor = black;
    OpenPartsUI::Pixmap* colpix = new OpenPartsUI::Pixmap;
    colpix->data = CORBA::string_dup( colorToPixString( tbColor, TXT_COLOR ) );
    pix = colpix;
    toolTip = Q2C( i18n( "Text Color" ) );
    m_idButtonText_Color = m_vToolBarText->insertButton2( pix, ID_TEXT_COLOR, SIGNAL( clicked() ), this, "textColor",
                                                          true, toolTip, -1 );

    m_vToolBarText->insertSeparator( -1 );

    // align left
    pix = OPUIUtils::convertPixmap( ICON( "alignLeft.xpm" ) );
    toolTip = Q2C( i18n( "Align Left" ) );
    m_idButtonText_ALeft = m_vToolBarText->insertButton2( pix, ID_ALEFT, SIGNAL( clicked() ), this, "textAlignLeft",
                                                          true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ALEFT, true );
    m_vToolBarText->setButton( ID_ALEFT, true );

    // align center
    pix = OPUIUtils::convertPixmap( ICON( "alignCenter.xpm" ) );
    toolTip = Q2C( i18n( "Align Center" ) );
    m_idButtonText_ACenter = m_vToolBarText->insertButton2( pix, ID_ACENTER, SIGNAL( clicked() ), this, "textAlignCenter",
                                                            true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ACENTER, true );
    m_vToolBarText->setButton( ID_ACENTER, false );

    // align right
    pix = OPUIUtils::convertPixmap( ICON( "alignRight.xpm" ) );
    toolTip = Q2C( i18n( "Align Right" ) );
    m_idButtonText_ARight = m_vToolBarText->insertButton2( pix, ID_ARIGHT, SIGNAL( clicked() ), this, "textAlignRight",
                                                           true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ARIGHT, true );
    m_vToolBarText->setButton( ID_ARIGHT, false );

    QString tmp;
    // align block
    pix = OPUIUtils::convertPixmap( ICON( "alignBlock.xpm" ) );
    toolTip = Q2C( i18n( "Align Block" ) );
    m_idButtonText_ABlock = m_vToolBarText->insertButton2( pix, ID_ABLOCK, SIGNAL( clicked() ), this, "textAlignBlock",
                                                           true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ABLOCK, true );
    m_vToolBarText->setButton( ID_ABLOCK, false );

    // line spacing
    OpenPartsUI::StrList spclist;
    spclist.length( 11 );
    for( unsigned int i = 0; i <= 10; i++ )
    {
        char buffer[ 10 ];
        sprintf( buffer, "%i", i );
        spclist[ i ] = CORBA::string_dup( buffer );
    }
    toolTip = Q2C( i18n( "Line Spacing ( in pt )" ) );
    m_idComboText_LineSpacing = m_vToolBarText->insertCombo( spclist, ID_LINE_SPC, false, SIGNAL( activated( const QString & ) ),
                                                             this, "textLineSpacing", true, toolTip,
                                                             60, -1, OpenPartsUI::AtBottom );
    spc = 0;
    m_vToolBarText->insertSeparator( -1 );

    // enum list
    pix = OPUIUtils::convertPixmap( ICON( "enumList.xpm" ) );
    toolTip = Q2C( i18n( "Enumerated List" ) );
    m_idButtonText_EnumList = m_vToolBarText->insertButton2( pix, ID_ENUM_LIST, SIGNAL( clicked() ), this, "textEnumList",
                                                             true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ENUM_LIST, true );
    m_vToolBarText->setButton( ID_ENUM_LIST, false );

    // unsorted list
    pix = OPUIUtils::convertPixmap( ICON( "unsortedList.xpm" ) );
    toolTip = Q2C( i18n( "Unsorted List" ) );
    m_idButtonText_EnumList = m_vToolBarText->insertButton2( pix, ID_USORT_LIST, SIGNAL( clicked() ), this, "textUnsortList",
                                                             true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_USORT_LIST, true );
    m_vToolBarText->setButton( ID_USORT_LIST, false );

    m_vToolBarText->insertSeparator( -1 );

    // superscript
    pix = OPUIUtils::convertPixmap( ICON( "super.xpm" ) );
    toolTip = Q2C( i18n( "Superscript" ) );
    m_idButtonText_SuperScript = m_vToolBarText->insertButton2( pix, ID_SUPERSCRIPT, SIGNAL( clicked() ), this, "textSuperScript",
                                                                true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_SUPERSCRIPT, true );
    m_vToolBarText->setButton( ID_SUPERSCRIPT, false );

    // subscript
    pix = OPUIUtils::convertPixmap( ICON( "sub.xpm" ) );
    toolTip = Q2C( i18n( "Subscript" ) );
    m_idButtonText_SubScript = m_vToolBarText->insertButton2( pix, ID_SUBSCRIPT, SIGNAL( clicked() ), this, "textSubScript",
                                                              true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_SUBSCRIPT, true );
    m_vToolBarText->setButton( ID_SUBSCRIPT, false );

    m_vToolBarText->insertSeparator( -1 );

    // border left
    pix = OPUIUtils::convertPixmap( ICON( "borderleft.xpm" ) );
    toolTip = Q2C( i18n( "Paragraph Border Left" ) );
    m_idButtonText_BorderLeft = m_vToolBarText->insertButton2( pix, ID_BRD_LEFT, SIGNAL( clicked() ), this, "textBorderLeft",
                                                               true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_BRD_LEFT, true );
    m_vToolBarText->setButton( ID_BRD_LEFT, false );

    // border right
    pix = OPUIUtils::convertPixmap( ICON( "borderright.xpm" ) );
    toolTip = Q2C( i18n( "Paragraph Border Right" ) );
    m_idButtonText_BorderRight = m_vToolBarText->insertButton2( pix, ID_BRD_RIGHT, SIGNAL( clicked() ), this, "textBorderRight",
                                                                true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_BRD_RIGHT, true );
    m_vToolBarText->setButton( ID_BRD_RIGHT, false );

    // border top
    pix = OPUIUtils::convertPixmap( ICON( "bordertop.xpm" ) );
    toolTip = Q2C( i18n( "Paragraph Border Top" ) );
    m_idButtonText_BorderTop = m_vToolBarText->insertButton2( pix, ID_BRD_TOP, SIGNAL( clicked() ), this, "textBorderTop",
                                                              true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_BRD_TOP, true );
    m_vToolBarText->setButton( ID_BRD_TOP, false );

    // border bottom
    pix = OPUIUtils::convertPixmap( ICON( "borderbottom.xpm" ) );
    toolTip = Q2C( i18n( "Paragraph Border Bottom" ) );
    m_idButtonText_BorderBottom = m_vToolBarText->insertButton2( pix, ID_BRD_BOTTOM, SIGNAL( clicked() ), this, "textBorderBottom",
                                                                 true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_BRD_BOTTOM, true );
    m_vToolBarText->setButton( ID_BRD_BOTTOM, false );

    // border color
    tmpBrd.color = black;
    colpix = new OpenPartsUI::Pixmap;
    colpix->data = CORBA::string_dup( colorToPixString( tmpBrd.color, FRAME_COLOR ) );
    pix = colpix;
    toolTip = Q2C( i18n( "Paragraph Border Color" ) );
    m_idButtonText_BorderColor = m_vToolBarText->insertButton2( pix, ID_BORDER_COLOR, SIGNAL( clicked() ), this, "textBorderColor",
                                                                true, toolTip, -1 );

    // border width combobox
    OpenPartsUI::StrList widthlist;
    widthlist.length( 10 );
    for( unsigned int i = 1; i <= 10; i++ )
    {
        char buffer[ 10 ];
        sprintf( buffer, "%i", i );
        widthlist[ i-1 ] = CORBA::string_dup( buffer );
    }
    tmpBrd.ptWidth = 1;
    toolTip = Q2C( i18n( "Paragraph Border Width" ) );
    m_idComboText_BorderWidth = m_vToolBarText->insertCombo( widthlist, ID_BRD_WIDTH, false, SIGNAL( activated( const QString & ) ),
                                                             this, "textBorderWidth", true, toolTip,
                                                             60, -1, OpenPartsUI::AtBottom );

    // border style combobox
    stylelist.length( 5 );
    stylelist[ 0 ] = CORBA::string_dup( i18n( "solid line" ) );
    stylelist[ 1 ] = CORBA::string_dup( i18n( "dash line ( ---- )" ) );
    stylelist[ 2 ] = CORBA::string_dup( i18n( "dot line ( **** )" ) );
    stylelist[ 3 ] = CORBA::string_dup( i18n( "dash dot line ( -*-* )" ) );
    stylelist[ 4 ] = CORBA::string_dup( i18n( "dash dot dot line ( -**- )" ) );
    toolTip = Q2C( i18n( "Paragraph Border Style" ) );
    m_idComboText_BorderStyle = m_vToolBarText->insertCombo( stylelist, ID_BRD_STYLE, false, SIGNAL( activated( const QString & ) ),
                                                             this, "textBorderStyle", true, toolTip,
                                                             150, -1, OpenPartsUI::AtBottom );
    tmpBrd.style = KWParagLayout::SOLID;

    m_vToolBarText->enable( OpenPartsUI::Show );

    // TOOLBAR Frame
    m_vToolBarFrame = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarFrame->setFullWidth( false );

    // border left
    pix = OPUIUtils::convertPixmap( ICON( "borderleft.xpm" ) );
    toolTip = Q2C( i18n( "Frame Border Left" ) );
    m_idButtonFrame_BorderLeft = m_vToolBarFrame->insertButton2( pix, ID_FBRD_LEFT, SIGNAL( clicked() ), this, "frameBorderLeft",
                                                                 true, toolTip, -1 );
    m_vToolBarFrame->setToggle( ID_FBRD_LEFT, true );
    m_vToolBarFrame->setButton( ID_FBRD_LEFT, false );

    // border right
    pix = OPUIUtils::convertPixmap( ICON( "borderright.xpm" ) );
    toolTip = Q2C( i18n( "Frame Border Right" ) );
    m_idButtonFrame_BorderRight = m_vToolBarFrame->insertButton2( pix, ID_FBRD_RIGHT, SIGNAL( clicked() ), this, "frameBorderRight",
                                                                  true, toolTip, -1 );
    m_vToolBarFrame->setToggle( ID_FBRD_RIGHT, true );
    m_vToolBarFrame->setButton( ID_FBRD_RIGHT, false );

    // border top
    pix = OPUIUtils::convertPixmap( ICON( "bordertop.xpm" ) );
    toolTip = Q2C( i18n( "Frame Border Top" ) );
    m_idButtonFrame_BorderTop = m_vToolBarFrame->insertButton2( pix, ID_FBRD_TOP, SIGNAL( clicked() ), this, "frameBorderTop",
                                                                true, toolTip, -1 );
    m_vToolBarFrame->setToggle( ID_FBRD_TOP, true );
    m_vToolBarFrame->setButton( ID_FBRD_TOP, false );

    // border bottom
    pix = OPUIUtils::convertPixmap( ICON( "borderbottom.xpm" ) );
    toolTip = Q2C( i18n( "Frame Border Bottom" ) );
    m_idButtonFrame_BorderBottom = m_vToolBarFrame->insertButton2( pix, ID_FBRD_BOTTOM, SIGNAL( clicked() ), this, "frameBorderBottom",
                                                                   true, toolTip, -1 );
    m_vToolBarFrame->setToggle( ID_FBRD_BOTTOM, true );
    m_vToolBarFrame->setButton( ID_FBRD_BOTTOM, false );

    // border color
    tmpBrd.color = black;
    colpix = new OpenPartsUI::Pixmap;
    colpix->data = CORBA::string_dup( colorToPixString( frmBrd.color, FRAME_COLOR ) );
    pix = colpix;
    toolTip = Q2C( i18n( "Frame Border Color" ) );
    m_idButtonFrame_BorderColor = m_vToolBarFrame->insertButton2( pix, ID_FBORDER_COLOR, SIGNAL( clicked() ), this, "frameBorderColor",
                                                                  true, toolTip, -1 );

    toolTip = Q2C( i18n( "Frame Border Width" ) );
    m_idComboFrame_BorderWidth = m_vToolBarFrame->insertCombo( widthlist, ID_FBRD_WIDTH, false, SIGNAL( activated( const QString & ) ),
                                                               this, "frameBorderWidth", true, toolTip,
                                                               60, -1, OpenPartsUI::AtBottom );

    toolTip = Q2C( i18n( "Frame Border Style" ) );
    m_idComboFrame_BorderStyle = m_vToolBarFrame->insertCombo( stylelist, ID_FBRD_STYLE, false, SIGNAL( activated( const QString & ) ),
                                                               this, "frameBorderStyle", true, toolTip,
                                                               150, -1, OpenPartsUI::AtBottom );

    // frame back color
    backColor.setColor( white );
    colpix = new OpenPartsUI::Pixmap;
    colpix->data = CORBA::string_dup( colorToPixString( backColor.color(), BACK_COLOR ) );
    pix = colpix;
    toolTip = Q2C( i18n( "Frame Background Color" ) );
    m_idButtonFrame_BackColor = m_vToolBarFrame->insertButton2( pix, ID_FBACK_COLOR, SIGNAL( clicked() ), this, "frameBackColor",
                                                                true, toolTip, -1 );

    m_vToolBarFrame->enable( OpenPartsUI::Hide );

    updateStyle( "Standard" );
    setFormat( format, false );
    gui->getPaperWidget()->forceFullUpdate();
    gui->getPaperWidget()->init();

    clipboardDataChanged();
    selectionOnOff();

    gui->getPaperWidget()->repaintScreen( true );
    
    return true;
}

/*============== create a pixmapstring from a color ============*/
QString KWordView::colorToPixString( QColor c, PType _type )
{
    int r, g, b;
    QString pix;
    QString line;

    c.rgb( &r, &g, &b );

    pix = "/* XPM */\n";

    pix += "static char * text_xpm[] = {\n";

    switch ( _type )
    {
    case TXT_COLOR:
    {
        pix += "\"20 20 11 1\",\n";
        pix += "\"h c #c0c000\",\n";
        pix += "\"g c #808000\",\n";
        pix += "\"f c #c0c0ff\",\n";
        pix += "\"a c #000000\",\n";
        pix += "\"d c #ff8000\",\n";
        pix += "\". c none\",\n";
        pix += "\"e c #0000c0\",\n";
        pix += "\"i c #ffff00\",\n";
        line.sprintf( "\"# c #%02X%02X%02X \",\n", r, g, b );
        pix += line.copy();
        pix += "\"b c #c00000\",\n";
        pix += "\"c c #ff0000\",\n";
        pix += "\"....................\",\n";
        pix += "\"....................\",\n";
        pix += "\"....................\",\n";
        pix += "\"........#...........\",\n";
        pix += "\"........#a..........\",\n";
        pix += "\".......###..........\",\n";
        pix += "\".......###a.........\",\n";
        pix += "\"......##aa#.........\",\n";
        pix += "\"......##a.#a........\",\n";
        pix += "\".....##a...#........\",\n";
        pix += "\".....#######a.......\",\n";
        pix += "\"....##aaaaaa#.......\",\n";
        pix += "\"....##a.....aaaaaaaa\",\n";
        pix += "\"...####....#abbccdda\",\n";
        pix += "\"....aaaa....abbccdda\",\n";
        pix += "\"............aee##ffa\",\n";
        pix += "\"............aee##ffa\",\n";
        pix += "\"............agghhiia\",\n";
        pix += "\"............agghhiia\",\n";
        pix += "\"............aaaaaaaa\"};\n";

//        pix += "\" 20 20 3 1 \", \n";

//      pix += "\"  c none \", \n";
//      pix += "\". c black \", \n";
//      line.sprintf( "\"+ c #%02X%02X%02X \", \n", r, g, b );
//      pix += line.copy();

//      pix += "\"                    \", \n";
//      pix += "\"                    \", \n";
//      pix += "\"  ..............    \", \n";
//      pix += "\"   ............     \", \n";
//      pix += "\"        ..          \", \n";
//      pix += "\"        ..          \", \n";
//      pix += "\"        ..          \", \n";
//      pix += "\"        ..          \", \n";
//      pix += "\"        ..          \", \n";
//      pix += "\"        ..          \", \n";
//      pix += "\"        ..          \", \n";
//      pix += "\"        .++++++++   \", \n";
//      pix += "\"        .++++++++   \", \n";
//      pix += "\"        +++++++++   \", \n";
//      pix += "\"        +++++++++   \", \n";
//      pix += "\"        +++++++++   \", \n";
//      pix += "\"        +++++++++   \", \n";
//      pix += "\"        +++++++++   \", \n";
//      pix += "\"                    \", \n";
//      pix += "\"                    \"}; \n";
    } break;
    case FRAME_COLOR:
    {
        pix += "\" 20 20 3 1 \",\n";

        pix += "\"  c none \",\n";
        pix += "\"+ c white \",\n";
        line.sprintf( "\". c #%02X%02X%02X \",\n", r, g, b );
        pix += line.copy();

        pix += "\"                    \",\n";
        pix += "\"                    \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ...++++++++++...  \",\n";
        pix += "\"  ...++++++++++...  \",\n";
        pix += "\"  ...++++++++++...  \",\n";
        pix += "\"  ...++++++++++...  \",\n";
        pix += "\"  ...++++++++++...  \",\n";
        pix += "\"  ...++++++++++...  \",\n";
        pix += "\"  ...++++++++++...  \",\n";
        pix += "\"  ...++++++++++...  \",\n";
        pix += "\"  ...++++++++++...  \",\n";
        pix += "\"  ...++++++++++...  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"                    \",\n";
        pix += "\"                    \"};\n";
    } break;
    case BACK_COLOR:
    {
        pix += "\" 20 20 3 1 \",\n";

        pix += "\"  c none \",\n";
        pix += "\". c red \",\n";
        line.sprintf( "\"+ c #%02X%02X%02X \",\n", r, g, b );
        pix += line.copy();

        pix += "\"                    \",\n";
        pix += "\"                    \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"                    \",\n";
        pix += "\"                    \"};\n";
    } break;
    }

    return QString( pix );
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

    for( int i = 0; i < numFonts; i++ ){

        if ( **fontNames != '-' )
        {
            fontNames ++;
            continue;
        };

        qfontname = "";
        qfontname = *fontNames;
        int dash = qfontname.find ( '-', 1, TRUE );

        if ( dash == -1 )
        {
            fontNames ++;
            continue;
        }

        int dash_two = qfontname.find ( '-', dash + 1 , TRUE );

        if ( dash == -1 )
        {
            fontNames ++;
            continue;
        }


        qfontname = qfontname.mid( dash +1, dash_two - dash -1 );

        if( !qfontname.contains( "open look", TRUE ) )
        {
            if( qfontname != "nil" ){
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

    OpenPartsUI::Pixmap colpix;
    colpix.data = CORBA::string_dup( colorToPixString( tmpBrd.color, FRAME_COLOR ) );
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

    KWordFrame *p = new KWordFrame( this, _child );
    p->setGeometry( _child->geometry() );
    m_lstFrames.append( p );

    KOffice::View_var kv = KOffice::View::_narrow( v );
    kv->setMode( KOffice::View::ChildMode );
    assert( !CORBA::is_nil( kv ) );
    p->attachView( kv );

    p->hide();
    _kwpf->setView( p );
    p->setPartObject( _kwpf );
}

/*================================================================*/
void KWordView::slotUpdateChildGeometry( KWordChild *_child )
{
    // Find frame for child
    KWordFrame *f = 0L;
    QListIterator<KWordFrame> it( m_lstFrames );
    for ( ; it.current() && !f; ++it )
        if ( it.current()->child() == _child ) f = it.current();

    assert( f != 0L );

    // Are we already up to date ?
    if ( _child->geometry() == f->partGeometry() )
        return;

  // TODO scaling
    f->setPartGeometry( _child->geometry() );
}

/*================================================================*/
void KWordView::slotGeometryEnd( KoFrame* _frame )
{
    KWordFrame *f = ( KWordFrame* )_frame;
    // TODO scaling
    m_pKWordDoc->changeChildGeometry( f->child(), _frame->partGeometry() );
}

/*================================================================*/
void KWordView::slotMoveEnd( KoFrame* _frame )
{
    KWordFrame *f = ( KWordFrame* )_frame;
    // TODO scaling
    m_pKWordDoc->changeChildGeometry( f->child(), _frame->partGeometry() );
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
    gui->getPaperWidget()->repaintScreen( true );
}

/*================================================================*/
void KWordView::spellCheckerReady()
{
    if ( !currParag && currFrameSetNum == -1 )
    {
        QObject::connect( kspell, SIGNAL( misspelling( char*, QStrList*, unsigned ) ), this, SLOT( spellCheckerMisspelling( char*, QStrList*, unsigned ) ) );
        QObject::connect( kspell, SIGNAL( corrected( char*, char*, unsigned ) ), this, SLOT( spellCheckerCorrected( char*, char*, unsigned ) ) );
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
            QObject::disconnect( kspell, SIGNAL( corrected( char*, char*, unsigned ) ), this, SLOT( spellCheckerCorrected( char*, char*, unsigned ) ) );
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
            QObject::disconnect( kspell, SIGNAL( corrected( char*, char*, unsigned ) ), this, SLOT( spellCheckerCorrected( char*, char*, unsigned ) ) );
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
            QObject::disconnect( kspell, SIGNAL( corrected( char*, char*, unsigned ) ), this, SLOT( spellCheckerCorrected( char*, char*, unsigned ) ) );
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
        m_bShowGUI = false;
    else
        m_bShowGUI = true;

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
        m_bShowGUI = false;
    else
        m_bShowGUI = true;

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
        m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, true );
        QString str;
        str.sprintf( i18n( "Undo: %s" ), _text.data() );
        text = Q2C( str );
        m_vMenuEdit->changeItemText( text, m_idMenuEdit_Undo );
        m_vToolBarEdit->setItemEnabled( ID_UNDO, true );
    }
    else
    {
        text = Q2C( i18n( "No Undo possible" ) );
        m_vMenuEdit->changeItemText( text, m_idMenuEdit_Undo );
        m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, false );
        m_vToolBarEdit->setItemEnabled( ID_UNDO, false );
    }
}

/*================================================================*/
void KWordView::changeRedo( QString _text, bool _enable )
{
    if ( !m_vMenuEdit || !m_vToolBarEdit ) return;

    CORBA::WString_var text;

    if ( _enable )
    {
        m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, true );
        QString str;
        str.sprintf( i18n( "Redo: %s" ), _text.data() );
        text = Q2C( str );
        m_vMenuEdit->changeItemText( text, m_idMenuEdit_Redo );
        m_vToolBarEdit->setItemEnabled( ID_REDO, true );
    }
    else
    {
        text = Q2C( i18n( "No Redo possible" ) );
        m_vMenuEdit->changeItemText( text, m_idMenuEdit_Redo );
        m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, false );
        m_vToolBarEdit->setItemEnabled( ID_REDO, false );
    }
}

/*================================================================*/
void KWordView::sendFocusEvent()
{
    QFocusEvent e( ( QEvent::Type ) 9 );
    QApplication::sendEvent( shell->getFrame(), &e );
}

/******************************************************************/
/* Class: KWordGUI                                                */
/******************************************************************/

/*================================================================*/
KWordGUI::KWordGUI( QWidget *parent, bool, KWordDocument *_doc, KWordView *_view )
    : QWidget( parent, "" )
{
    doc = _doc;
    view = _view;
    _showStruct = false;

    r_horz = r_vert = 0;

    panner = new KExtPanner( this );
    left = new QWidget( panner );
    left->show();
    paperWidget = new KWPage( left, doc, this );
    docStruct = new KWDocStruct( panner, doc, this );

    panner->activate( docStruct, left );
    panner->setSeparatorPos( 0 );

    KoPageLayout layout;
    KoColumns cols;
    KoKWHeaderFooter hf;
    doc->getPageLayout( layout, cols, hf );

    tabChooser = new KoTabChooser( left, KoTabChooser::TAB_ALL );

    r_horz = new KoRuler( left, paperWidget->viewport(), KoRuler::HORIZONTAL, layout, KoRuler::F_INDENTS | KoRuler::F_TABS, tabChooser );
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

    switch ( KWUnit::unitType( doc->getUnit() ) )
    {
    case U_MM:
    {
        r_horz->setLeftIndent( paperWidget->getLeftIndent().mm() );
        r_horz->setFirstIndent( paperWidget->getFirstLineIndent().mm() );
    } break;
    case U_INCH:
    {
        r_horz->setLeftIndent( paperWidget->getLeftIndent().inch() );
        r_horz->setFirstIndent( paperWidget->getFirstLineIndent().inch() );
    } break;
    case U_PT:
    {
        r_horz->setLeftIndent( paperWidget->getLeftIndent().pt() );
        r_horz->setFirstIndent( paperWidget->getFirstLineIndent().pt() );
    } break;
    }

    r_horz->hide();
    r_vert->hide();

    //setFocusProxy( paperWidget );
    paperWidget->show();
    //paperWidget->setFocusPolicy( QWidget::StrongFocus );
    docStruct->show();

    reorganize();

    if ( doc->getProcessingType() == KWordDocument::DTP )
        paperWidget->setRuler2Frame( 0, 0 );

    // HACK
    if ( doc->viewCount() == 1 && !doc->loaded() )
    {
        QKeyEvent e( static_cast<QEvent::Type>( 6 ) /*QEvent::KeyPress*/ , Key_Delete, 0 ,0 );
        paperWidget->keyPressEvent( &e );
    }

    connect( r_horz, SIGNAL( tabListChanged( QList<KoTabulator>* ) ), paperWidget, SLOT( tabListChanged( QList<KoTabulator>* ) ) );

    paperWidget->forceFullUpdate();
    panner->setAbsSeparatorPos( 0, true );
    connect( panner, SIGNAL( pannerResized() ), this, SLOT( reorganize() ) );

    setKeyCompression( true );
    setAcceptDrops( true );
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
    disconnect( panner, SIGNAL( pannerResized() ), this, SLOT( reorganize() ) );
    if ( _show )
    {
        r_vert->show();
        r_horz->show();
        tabChooser->show();

        tabChooser->setGeometry( 0, 0, 20, 20 );

        if ( !_showStruct || _showStruct && panner->absSeparatorPos() == 0 )
            panner->setAbsSeparatorPos( _showStruct ? ( width() / 100 ) * 20 : 0, true );
        panner->setGeometry( 0, 0, width(), height() );
        paperWidget->setGeometry( 20, 20, left->width() - 20, left->height() - 20 );
        r_horz->setGeometry( 20, 0, left->width() - 20, 20 );
        r_vert->setGeometry( 0, 20, 20, left->height() - 20 );
    }
    else
    {
        r_vert->hide();
        r_horz->hide();
        tabChooser->hide();

        panner->setAbsSeparatorPos( 0, true );
        panner->setGeometry( 0, 0, width(), height() );
        paperWidget->setGeometry( 20, 20, left->width() - 20, left->height() - 20 );

    }
    connect( panner, SIGNAL( pannerResized() ), this, SLOT( reorganize() ) );
}

/*================================================================*/
void KWordGUI::unitChanged( QString u )
{
    doc->setUnit( u );
    doc->setUnitToAll();
}

/*================================================================*/
void KWordGUI::showDocStruct( bool __show )
{
    if ( !__show )
        panner->setAbsSeparatorPos( 0, true );
    else
        panner->setAbsSeparatorPos( ( width() / 100 ) * 20, true );

    _showStruct = __show;

    reorganize();
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
