#include <qprinter.h>
#include "kformula_view.h"
#include "kformula_doc.h"
#include "kformula_main.h"
#include "TextElement.h"
#include "matrixwidget.h"

#include <qpainter.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qkeycode.h>

#include "BracketElement.h"
#include "RootElement.h"
#include "TextElement.h"
#include "FractionElement.h"
#include "MatrixElement.h"
#include "PrefixedElement.h"
#include <kiconloader.h>

#include <opUIUtils.h>
#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <kglobal.h>

/**********************************************************
 *
 * KFormulaView
 *
 **********************************************************/

KFormulaView::KFormulaView( QWidget *_parent, const char *_name, KFormulaDoc* _doc ) :
    QWidget( _parent, _name ), KoViewIf( _doc ), OPViewIf( _doc ), KFormula::View_skel()
{
    setWidget( this );
    widget()->setFocusPolicy( QWidget::StrongFocus );
    
    OPPartIf::setFocusPolicy( OpenParts::Part::ClickFocus );

    setBackgroundColor( white );

    m_pDoc = _doc;

    mn_indexList = new QPopupMenu();
    mn_indexList->insertItem(ICON("index0.xpm"),0);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(ICON("index1.xpm"),1);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(ICON("index2.xpm"),2);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(ICON("index3.xpm"),3);
    mn_indexList->setMouseTracking(true);
    mn_indexList->setCheckable(false);

    QObject::connect(mn_indexList,SIGNAL(activated(int)),this,SLOT(insertIndex(int)));

    QObject::connect( m_pDoc, SIGNAL( sig_modified() ),
		      this, SLOT( slotModified() ) );

    QObject::connect( m_pDoc,
		      SIGNAL(sig_changeType(const BasicElement *)    ),
		      this, SLOT( slotTypeChanged(const BasicElement *)    ) );
}

void KFormulaView::init()
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
}

KFormulaView::~KFormulaView()
{
    cleanUp();
}

void KFormulaView::cleanUp()
{
    if ( m_bIsClean )
	return;

    OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
    if ( !CORBA::is_nil( menu_bar_manager ) )
	menu_bar_manager->unregisterClient( id() );

    OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
    if ( !CORBA::is_nil( tool_bar_manager ) )
	tool_bar_manager->unregisterClient( id() );

    m_pDoc->removeView( this );

    KoViewIf::cleanUp();
}

void KFormulaView::paintEvent( QPaintEvent *_ev )
{
    m_pDoc->paintEvent(_ev, this);
}
void KFormulaView::mousePressEvent( QMouseEvent *_ev )
{
    m_pDoc->mousePressEvent(_ev,this);
}

void KFormulaView::resizeEvent( QResizeEvent * )
{
}

void KFormulaView::keyPressEvent( QKeyEvent *k )
{
    cerr << "Key pressed, ascii "<< k->ascii() << endl;
    m_pDoc->keyPressEvent(k);
}



CORBA::Long KFormulaView::addToolButton( OpenPartsUI::ToolBar_ptr toolbar,
					 const char* pictname,
					 const QString &tooltip,
					 const char* func,
					 CORBA::Long _id )
{
    OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( ICON(pictname) );
    CORBA::WString_var toolTip = Q2C( tooltip );

    CORBA::Long id =
	toolbar->insertButton2( pix, _id,
				SIGNAL( clicked() ), this, func, true,
				toolTip, -1 );

    return id;
}

bool KFormulaView::event( const char* _event, const CORBA::Any& _value )
{
    cerr << "CALLED" << endl;

    EVENT_MAPPER( _event, _value );

    MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_ptr, mappingCreateMenubar );
    MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_ptr, mappingCreateToolbar );

    END_EVENT_MAPPER;

    cerr << "CALLE2D" << endl;

    return false;
}

bool KFormulaView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
    if ( CORBA::is_nil( _menubar ) ) {
	m_vMenuView = 0L;
	m_vMenuElement = 0L;
	m_vMenuFormula = 0L;
	m_vMenuHelp = 0L;
	return true;
    }

    cerr << "START --------------- MENU -------------------" << endl;

    CORBA::WString_var text;

    // View
    text = Q2C( i18n( "&View" ) );
    _menubar->insertMenu( text, m_vMenuView, -1, -1 );

    text = Q2C( i18n( "&New View" ) );
    m_idMenuView_NewView = m_vMenuView->insertItem( text, this, "newView", 0 );
    text = Q2C( i18n( "Font Toolbar" ) );
    m_idMenuView_FontToolbar = m_vMenuView->insertItem( text, this, "newView", 0 );
    text = Q2C( i18n( "Text Toolbar" ) );
    m_idMenuView_TextToolbar = m_vMenuView->insertItem( text, this, "newView", 0 );
    text = Q2C( i18n( "Type Toolbar" ) );
    m_idMenuView_TypeToolbar = m_vMenuView->insertItem( text, this, "newView", 0 );

    // Element
    text = Q2C( i18n( "&Element" ) );
    _menubar->insertMenu( text, m_vMenuElement, -1, -1 );

    text = Q2C( i18n( "&Add Index" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_AddIndex, -1, -1 );

    OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( ICON("index0.xpm"));
    text = Q2C( i18n( "Top left" ) );
    m_idMenuElement_AddIndex_TL = m_vMenuElement_AddIndex->insertItem6( pix, text, this, "addTopLeftIndex", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(ICON("index1.xpm") );
    text = Q2C( i18n( "Bottom left" ) );
    m_idMenuElement_AddIndex_BL = m_vMenuElement_AddIndex->insertItem6( pix, text, this, "addBottomLeftIndex", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(ICON("index2.xpm") );
    text = Q2C( i18n( "Top right" ) );
    m_idMenuElement_AddIndex_TR = m_vMenuElement_AddIndex->insertItem6( pix, text, this, "addTopRightIndex", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(ICON("index3.xpm") );
    text = Q2C( i18n( "Bottom right" ) );
    m_idMenuElement_AddIndex_BL = m_vMenuElement_AddIndex->insertItem6( pix, text, this, "addBottomRightIndex", 0, -1, -1 );

    text = Q2C( i18n( "&Add Element" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_AddElement, -1, -1 );	

    pix = OPUIUtils::convertPixmap(ICON("mini-xy.xpm") );
    text = Q2C( i18n( "Plain text" ) );
    m_idMenuElement_AddElement_T = m_vMenuElement_AddElement->insertItem6( pix, text, this, "addText", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(ICON("mini-root.xpm") );
    text = Q2C( i18n( "Root" ) );
    m_idMenuElement_AddElement_R = m_vMenuElement_AddElement->insertItem6( pix, text, this, "addRoot", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(ICON("mini-bra.xpm") );
    text = Q2C( i18n( "Bracket" ) );
    m_idMenuElement_AddElement_B = m_vMenuElement_AddElement->insertItem6( pix, text, this, "addBracket", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(ICON("mini-frac.xpm") );
    text = Q2C( i18n( "Fraction" ) );
    m_idMenuElement_AddElement_F = m_vMenuElement_AddElement->insertItem6( pix, text, this, "addFraction", 0, -1, -1 );

    text = Q2C( i18n( "Set color" ) );
    m_idMenuElement_Color = m_vMenuElement->insertItem( text, this, "elementColor", 0 );

    text = Q2C( i18n( "Remove element" ) );
    m_idMenuElement_Remove = m_vMenuElement->insertItem( text, this, "remove", 0 );

    m_vMenuElement->insertSeparator( -1 );

    text = Q2C( i18n( "&Text" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_Text, -1, -1 );	

    text = Q2C( i18n( "Set font" ) );
    m_idMenuElement_Text_Font = m_vMenuElement_Text->insertItem( text, this, "textFont", 0 );

    pix = OPUIUtils::convertPixmap(ICON("split.xpm") );
    text = Q2C( i18n( "Split at current position" ) );
    m_idMenuElement_Text_Split = m_vMenuElement_Text->insertItem6( pix, text, this, "textSplit", 0, -1, -1 );

    text = Q2C( i18n( "&Root" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_Root, -1, -1 );	

    text = Q2C( i18n( "Use pixmap" ) );
    m_idMenuElement_Root_Pixmap = m_vMenuElement_Root->insertItem( text, this, "togglePixmap", 0 );

    pix = OPUIUtils::convertPixmap(ICON("rootindex.xpm") );
    text = Q2C( i18n( "Add root index (Top left)" ) );
    m_idMenuElement_Root_Index = m_vMenuElement_Root->insertItem6( pix, text, this, "addTopLeftIndex", 0, -1, -1 );

    text = Q2C( i18n( "&Bracket" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_Bracket, -1, -1 );	

    pix = OPUIUtils::convertPixmap(ICON("delimiter.xpm") );
    text = Q2C( i18n( "Set delimiter ..." ) );
    m_idMenuElement_Bracket_Type = m_vMenuElement_Bracket->insertItem6( pix, text, this, "bracketType", 0, -1, -1 );

    text = Q2C( i18n( "&Fraction" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_Fraction, -1, -1 );
	
    text = Q2C( i18n( "Vertical align ..." ) );
    m_idMenuElement_Fraction_VA = m_vMenuElement_Fraction->insertItem( text, this, "addFraction", 0 );

    text = Q2C( i18n( "Horizontal align ..." ) );
    m_idMenuElement_Fraction_HA = m_vMenuElement_Fraction->insertItem( text, this, "fractionHAlign", 0 );

    text = Q2C( i18n( "Set midline" ) );
    m_idMenuElement_Fraction_MidLine = m_vMenuElement_Fraction->insertItem( text, this, "toggleMidline", 0 );

    text = Q2C( i18n( "&Integral" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_Integral, -1, -1 );

    text = Q2C( i18n( "Use pixmap" ) );
    m_idMenuElement_Integral_Pixmap = m_vMenuElement_Integral->insertItem( text, this, "togglePixmap", 0 );

    pix = OPUIUtils::convertPixmap(ICON("Ihigher.xpm") );
    text = Q2C( i18n( "Add higher limit" ) );
    m_idMenuElement_Integral_Higher = m_vMenuElement_Integral->insertItem6( pix, text, this, "addHigher", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(ICON("Ilower.xpm") );
    text = Q2C( i18n( "Add lower limit" ) );
    m_idMenuElement_Integral_Lower = m_vMenuElement_Integral->insertItem6( pix, text, this, "addLower", 0, -1, -1 );

    text = Q2C( i18n( "&Matrix" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_Matrix, -1, -1 );

    // pix = OPUIUtils::convertPixmap(ICON("setmatrix.xpm" );
    // m_idMenuElement_Matrix_Set = m_vMenuElement_Matrix->insertItem6( pix, i18n( "Set dimension" ), this, "matrixSet", 0, -1, -1 );
    text = Q2C( i18n( "Set dimension" ) );
    m_idMenuElement_Matrix_Set = m_vMenuElement_Matrix->insertItem( text, this, "matrixSet", 0 );

    pix = OPUIUtils::convertPixmap(ICON("insrow.xpm") );
    text = Q2C( i18n( "Insert a row" ) );
    m_idMenuElement_Matrix_InsRow = m_vMenuElement_Matrix->insertItem6( pix, text, this, "matrixInsRow", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(ICON("inscol.xpm") );
    text = Q2C( i18n( "Insert a column" ) );
    m_idMenuElement_Matrix_InsCol = m_vMenuElement_Matrix->insertItem6( pix, text, this, "matrixInsCol", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(ICON("remrow.xpm") );
    text = Q2C( i18n( "Remove a row" ) );
    m_idMenuElement_Matrix_RemRow = m_vMenuElement_Matrix->insertItem6( pix, text, this, "matrixRemRow", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(ICON("remcol.xpm") );
    text = Q2C( i18n( "Remove a column" ) );
    m_idMenuElement_Matrix_RemCol = m_vMenuElement_Matrix->insertItem6( pix, text, this, "matrixRemCol", 0, -1, -1 );

    text = Q2C( i18n( "&Decoration" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_Decoration, -1, -1 );

    text = Q2C( i18n( "&Symbol" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_Symbol, -1, -1 );
	
    // Formula
    text = Q2C( i18n( "&Formula" ) );
    _menubar->insertMenu( text, m_vMenuFormula, -1, -1 );

    text = Q2C( i18n( "Set general color" ) );
    m_idMenuFormula_Color = m_vMenuFormula->insertItem( text, this, "addFraction", 0 );

    text = Q2C( i18n( "Set general font" ) );
    m_idMenuFormula_Font = m_vMenuFormula->insertItem( text, this, "generalFont", 0 );

    // Help
    m_vMenuHelp = _menubar->helpMenu();
    if ( CORBA::is_nil( m_vMenuHelp ) )
	{
	    _menubar->insertSeparator( -1 );
	    text = Q2C( i18n( "&Help" ) );
	    _menubar->setHelpMenu( _menubar->insertMenu( text, m_vMenuHelp, -1, -1 ) );
	}

    // m_idMenuHelp_About = m_vMenuHelp->insertItem( i18n( "&About" ), this, "helpAbout", 0 );
    text = Q2C( i18n( "&Using KFormula" ) );
    m_idMenuHelp_Using = m_vMenuHelp->insertItem( text, this, "helpUsing", 0 );

    cerr << "END --------------- MENU -------------------" << endl;

    return true;
}

bool KFormulaView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
    cerr << "bool KFormulaView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" << endl;

    if ( CORBA::is_nil( _factory ) )
	{
	    cerr << "Setting to nil" << endl;
	    m_vToolBarFormula = 0L;
	    m_vToolBarFont = 0L;
	    m_vToolBarType = 0L;
	    cerr << "niled" << endl;
	    return true;
	}

    // Formula
    m_vToolBarFormula = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

    m_idButtonFormula_0 = addToolButton(m_vToolBarFormula, "mini-xy.xpm",
					i18n( "Add/change to simple text" ),
					"addText", 0 );


    m_idButtonFormula_1 = addToolButton(m_vToolBarFormula, "mini-root.xpm",
					i18n( "Add/change to root" ), "addRoot", 1 );

    m_idButtonFormula_2 = addToolButton(m_vToolBarFormula, "mini-frac.xpm",
					i18n( "Add/change fract line" ), "addFraction", 2 );

    m_idButtonFormula_3 = addToolButton(m_vToolBarFormula, "mini-vspace.xpm",
					i18n( "Add/change vertical space" ), "addVertSpace", 3 );
	
    m_idButtonFormula_4 = addToolButton(m_vToolBarFormula, "mini-bra.xpm",
					i18n( "Add/change a bracket block" ), "addBracket", 4 );

    m_idButtonFormula_5 = addToolButton(m_vToolBarFormula, "mini-integral.xpm",
					i18n( "Add/change an integral" ), "addIntegral", 5 );

    m_idButtonFormula_6 = addToolButton(m_vToolBarFormula, "mini-symbols.xpm",
					i18n( "Add/change a block with symbols" ), "addSymbol", 6 );

    m_idButtonFormula_6 = addToolButton(m_vToolBarFormula, "matrix.xpm",
					i18n( "Add/change matrix" ), "addMatrix", 7  );
    	
    m_idButtonFormula_7 = addToolButton(m_vToolBarFormula, "index.xpm",
					i18n( "Add an index at position..." ), "indexList", 8 );

    m_vToolBarFormula->enable( OpenPartsUI::Show );
    // Font
    m_vToolBarFont = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

    OpenPartsUI::StrList fonts;
    fonts.length( 4 );
    fonts[0] = CORBA::string_dup( "utopia" );
    fonts[1] = CORBA::string_dup( "symbol" );
    fonts[2] = CORBA::string_dup( "times" );
    fonts[3] = CORBA::string_dup( "(default)" );

    CORBA::WString_var toolTip;

    toolTip = Q2C( i18n("Font Family") );
    m_idComboFont_FontFamily = m_vToolBarFont->insertCombo( fonts,   0, false, SIGNAL( activated( const QString & ) ), this,
							    "fontSelected", true, toolTip,
							    120, -1, OpenPartsUI::AtBottom );
    m_vToolBarFont->setCurrentComboItem( m_idComboFont_FontFamily, 0 );
	
    OpenPartsUI::StrList sizelist;
    //  int sizes[24] = { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 22, 24, 26, 28, 32, 48, 64 };
    // Why  only these size ?

    sizelist.length( 100 );
    for( unsigned int i = 0; i < 100; i++ )
	{
	    char buffer[ 10 ];
	    sprintf( buffer, "%i", i+1 );
	    sizelist[i] = CORBA::string_dup( buffer );
	}
    toolTip = Q2C( i18n( "Font Size"  ) );
    m_idComboFont_FontSize = m_vToolBarFont->insertCombo( sizelist,  1, true, SIGNAL( activated( const QString & ) ),
							  this, "sizeSelected", true,
							  toolTip, 50, -1, OpenPartsUI::AtBottom );
    m_vToolBarFont->setCurrentComboItem( m_idComboFont_FontSize, 2  );

    m_idButtonFont_Bold = addToolButton(m_vToolBarFont, "bold.xpm",
					i18n( "Bold" ), "fontSwitch", 2 );

    m_idButtonFont_Italic = addToolButton(m_vToolBarFont, "italic.xpm",
					  i18n( "Italic" ),"fontSwitch", 3 );

    m_idButtonFont_Underl = addToolButton(m_vToolBarFont, "underl.xpm",
					  i18n( "Underlined" ), "fontSwitch", 4 );

    warning("provo1");
    //     m_vToolBarFont->insertSeparator(   6  );
    //  warning("provo");
    //

    sizelist.length( 16 );
    for( unsigned int i = 1; i <= 10; i++ )
	{
	    char buffer[20];
	    sprintf(buffer,"+(-) %i",i);
	    sizelist[ i - 1 ] = CORBA::string_dup( buffer );
	}
    for( unsigned int i = 1; i <= 6; i++ )
	{
	    char buffer[20];
	    sprintf(buffer,"*(/) %i/%i",i,i+1);
	    sizelist[ 10 + i - 1 ] = CORBA::string_dup( buffer );
	}

    toolTip = Q2C( i18n( "Font scale mode" ) );
    m_idComboFont_ScaleMode = m_vToolBarFont->insertCombo( sizelist,  5    ,true, SIGNAL( activated( const QString & ) ),
							   this, "sizeSelected", true,
							   toolTip, 80, -1, OpenPartsUI::AtBottom );


    //    m_vToolBarFont->setCurrentComboItem( m_idComboFont_ScaleMode,  2 );
    warning("provo1");
    m_idButtonFont_0 = addToolButton(m_vToolBarFont, "reduce.xpm",
				     i18n( "Reduce the fontSize" ), "reduce", 6 );

    m_idButtonFont_1 = addToolButton(m_vToolBarFont, "enlarge.xpm",
				     i18n( "Increase the fontSize" ), "enlarge", 7 );

    //m_vToolBarFont->insertSeparator( -1 );

    m_idButtonFont_2 = addToolButton(m_vToolBarFont, "elementsw.xpm",
				     i18n( "Reduce/Increase active element fontSize" ),"fontSwitch", 8 );

    m_idButtonFont_3 = addToolButton(m_vToolBarFont, "indexsw.xpm",
				     i18n( "Reduce/Increase indexes fontSize" ),"fontSwitch",  9 );

    m_idButtonFont_4 = addToolButton(m_vToolBarFont, "childrensw.xpm",
				     i18n( "Reduce/Increase children (content) fontSize" ), "fontSwitch", 10 );

    m_idButtonFont_5 = addToolButton(m_vToolBarFont, "nextsw.xpm",
				     i18n( "Reduce/Increase next element fontSize" ), "fontSwitch", 11 );

    m_vToolBarFont->setToggle( m_idButtonFont_Bold, true );
    m_vToolBarFont->setToggle( m_idButtonFont_Underl, true );
    m_vToolBarFont->setToggle( m_idButtonFont_Italic, true );
    m_vToolBarFont->setToggle( m_idButtonFont_2, true );
    m_vToolBarFont->setToggle( m_idButtonFont_3, true );
    m_vToolBarFont->setToggle( m_idButtonFont_4, true );
    m_vToolBarFont->setToggle( m_idButtonFont_5, true );
	
    m_vToolBarFont->setButton( m_idButtonFont_2, true );
    m_vToolBarFont->setButton( m_idButtonFont_3, true );
    m_vToolBarFont->setButton( m_idButtonFont_4, true );
    m_vToolBarFont->setButton( m_idButtonFont_5, false );

    m_vToolBarFont->enable( OpenPartsUI::Show );	
    warning("Fine ");
    // Type
    m_vToolBarType = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

    // ************************* Text *********************
    m_idButtonType_Spl = addToolButton(m_vToolBarType, "split.xpm",
				       i18n( "Split text into 2 elment" ), "textSplit", 0 );
    //m_vToolBarType->insertSeparator(  -1);
    // ************************** Root ********************
    m_idButtonType_RIn = addToolButton(m_vToolBarType, "rootindex.xpm",
				       i18n( "Add root index (i.e. Top Left index)" ), "addTopLeftIndex",  1 );

    // ************************** Brackets ********************	
    //m_vToolBarType->insertSeparator(  -1);

    //    m_idButtonType_Del = addToolButton(m_vToolBarType, "delimiter.xpm",
    //				     i18n( "Set delimiter type" ), "bracketType" );

    fonts.length( 9 );
    fonts[0] = CORBA::string_dup("(");
    fonts[1] = CORBA::string_dup("[");
    fonts[2] = CORBA::string_dup("<");
    fonts[3] = CORBA::string_dup("/");
    fonts[4] = CORBA::string_dup("\\");
    fonts[5] = CORBA::string_dup(")");
    fonts[6] = CORBA::string_dup("]");
    fonts[7] = CORBA::string_dup(">");
    fonts[8] = CORBA::string_dup("|");
    toolTip = Q2C( i18n("Left delimiter") );
    m_idComboType_DelLeft = m_vToolBarType->insertCombo( fonts, 2 ,false, SIGNAL( activated( const QString & ) ), this,
							 "delimiterLeft", true, toolTip,
							 40, -1, OpenPartsUI::AtBottom );

    fonts[0] = CORBA::string_dup(")");
    fonts[1] = CORBA::string_dup("]");
    fonts[2] = CORBA::string_dup(">");
    fonts[3] = CORBA::string_dup("/");
    fonts[4] = CORBA::string_dup("\\");
    fonts[5] = CORBA::string_dup("(");
    fonts[6] = CORBA::string_dup("[");
    fonts[7] = CORBA::string_dup("<");
    fonts[8] = CORBA::string_dup("|");
    toolTip = Q2C( i18n("Right delimiter") );
    m_idComboType_DelRight = m_vToolBarType->insertCombo( fonts, 3, false, SIGNAL( activated( const QString & ) ), this,
							  "delimiterRight", true, toolTip,
							  40, -1, OpenPartsUI::AtBottom );
    //m_vToolBarType->insertSeparator(  7 );

    // ************************** Fraction ********************
    m_idButtonType_MAl = addToolButton(m_vToolBarType, "midalign.xpm",
				       i18n( "Align fraction to midline" ), "fractionAlignM", 4 );

    m_idButtonType_UAl = addToolButton(m_vToolBarType, "upalign.xpm",
				       i18n( "Align fraction to numerator" ), "fractionAlignU", 5 );

    m_idButtonType_DAl = addToolButton(m_vToolBarType, "downalign.xpm",
				       i18n( "Align fraction to denominator" ), "fractionAlignD",6  );

    m_idButtonType_CAl = addToolButton(m_vToolBarType, "centralign.xpm",
				       i18n( "Align center" ), "fractionAlignC", 7 );

    m_idButtonType_LAl = addToolButton(m_vToolBarType, "leftalign.xpm",
				       i18n( "Align left" ), "fractionAlignL", 8 );

    m_idButtonType_RAl = addToolButton(m_vToolBarType, "rightalign.xpm",
				       i18n( "Align Right" ), "fractionAlignR",  9 );

    m_idButtonType_Les = addToolButton(m_vToolBarType, "near.xpm",
				       i18n( "Reduce element vertical distance" ), "fractionDistLess", 10 );

    m_idButtonType_Mor = addToolButton(m_vToolBarType, "far.xpm",
				       i18n( "Increase element vertical distance" ), "fractionDistMore", 11 );

    m_idButtonType_Mid = addToolButton(m_vToolBarType, "midline.xpm",
				       i18n( "Toggle fraction line" ), "toggleMidline", 12 );

    //m_vToolBarType->insertSeparator( 17 );

    // *******************  Integral **************************+
    m_idButtonType_AddH = addToolButton(m_vToolBarType, "Ihigher.xpm",
					i18n( "Add higher limit" ), "integralHigher", 13 );

    m_idButtonType_AddL = addToolButton(m_vToolBarType, "Ilower.xpm",
					i18n( "Add lower limit" ), "integralLower", 14 );

    //m_vToolBarType->insertSeparator( 20 );

    // *********************** Matrix *************************
    m_idButtonType_SetM = addToolButton(m_vToolBarType, "matrix.xpm",
					i18n( "Set matrix dimension" ), "matrixSet", 15 );

    m_idButtonType_InR = addToolButton(m_vToolBarType, "insrow.xpm",
				       i18n( "Insert a row" ), "matrixInsRow", 16 );

    m_idButtonType_InC = addToolButton(m_vToolBarType, "inscol.xpm",
				       i18n( "Insert a column" ), "matrixInsCol",17  );

    m_idButtonType_ReR = addToolButton(m_vToolBarType, "remrow.xpm",
				       i18n( "Remove a row" ), "matrixRemRow", 18 );

    m_idButtonType_ReC = addToolButton(m_vToolBarType, "remcol.xpm",
				       i18n( "Remove a column" ), "matrixRemCol", 19 );

    //m_vToolBarType->insertSeparator( 26 );
    m_vToolBarType->enable( OpenPartsUI::Show );

    // *********************** General *************************
    m_idButtonType_Pix = addToolButton(m_vToolBarType, "remcol.xpm",
				       i18n( "Toggle pixmap use" ), "togglePixmap", 20 );
    warning("vai");
    m_vToolBarType->setToggle( m_idButtonType_UAl, true );
    m_vToolBarType->setToggle( m_idButtonType_MAl, true );
    m_vToolBarType->setToggle( m_idButtonType_DAl, true );
    m_vToolBarType->setToggle( m_idButtonType_CAl, true );
    m_vToolBarType->setToggle( m_idButtonType_LAl, true );
    m_vToolBarType->setToggle( m_idButtonType_RAl, true );
    m_vToolBarType->setToggle( m_idButtonType_Mid, true );
	
    slotTypeChanged(0);





    cerr << "-------------------------------- TOOL --------------" << endl;

    return true;
}


void KFormulaView::slotTypeChanged( const BasicElement *elm)
{
    bool isText, isBracket, isFraction, isPrefixed, isMatrix, isRoot;
    if (elm) {
	const type_info& type = typeid(*elm);
	isText = type == typeid(TextElement);
	isBracket = type == typeid(BracketElement);
	isFraction = type == typeid(FractionElement);
	isPrefixed = type == typeid(PrefixedElement);
	isMatrix = type == typeid(MatrixElement);
	isRoot = type == typeid(RootElement);
    } else {
	isRoot = isMatrix = isPrefixed =
	    isFraction = isBracket = isText = false;
    }

    m_vToolBarType->setItemEnabled(m_idButtonType_Spl,isText);

    // It remains deactivated !!!
    //    m_rMenuBar->setItemEnabled(m_idMenuElement_Fraction,isFraction);

    m_vToolBarType->setItemEnabled(m_idComboType_DelLeft,isBracket);
    m_vToolBarType->setItemEnabled(m_idComboType_DelRight,isBracket);
    m_vToolBarType->setItemEnabled(m_idButtonType_RIn,isRoot);
    m_vToolBarType->setItemEnabled(m_idButtonType_UAl,isFraction);
    m_vToolBarType->setItemEnabled(m_idButtonType_DAl,isFraction);
    m_vToolBarType->setItemEnabled(m_idButtonType_MAl,isFraction);
    m_vToolBarType->setItemEnabled(m_idButtonType_Mid,isFraction);

    m_vToolBarType->setItemEnabled(m_idButtonType_CAl,isFraction||isMatrix);
    m_vToolBarType->setItemEnabled(m_idButtonType_LAl,isFraction||isMatrix);
    m_vToolBarType->setItemEnabled(m_idButtonType_RAl,isFraction||isMatrix);

    m_vToolBarType->setItemEnabled(m_idButtonType_Les,isFraction);
    m_vToolBarType->setItemEnabled(m_idButtonType_Mor,isFraction);
    m_vToolBarType->setItemEnabled(m_idButtonType_AddH,isPrefixed);
    m_vToolBarType->setItemEnabled(m_idButtonType_AddL,isPrefixed);
    m_vToolBarType->setItemEnabled(m_idButtonType_SetM,isMatrix);
    m_vToolBarType->setItemEnabled(m_idButtonType_InC,isMatrix);
    m_vToolBarType->setItemEnabled(m_idButtonType_InR,isMatrix);
    m_vToolBarType->setItemEnabled(m_idButtonType_ReC,isMatrix);
    m_vToolBarType->setItemEnabled(m_idButtonType_ReR,isMatrix);

    if (elm)
	m_vToolBarFont->setCurrentComboItem(m_idComboFont_FontSize, elm->getNumericFont()-1);

    if (isFraction) {
	QString content=elm->getContent();
	m_vToolBarType->setButton(m_idButtonType_UAl,content[1]=='U');
	m_vToolBarType->setButton(m_idButtonType_DAl,content[1]=='D');
	m_vToolBarType->setButton(m_idButtonType_MAl,content[1]=='M');
	m_vToolBarType->setButton(m_idButtonType_CAl,content[2]=='C');
	m_vToolBarType->setButton(m_idButtonType_LAl,content[2]=='L');
	m_vToolBarType->setButton(m_idButtonType_RAl,content[2]=='R');
	m_vToolBarType->setButton(m_idButtonType_Mid,content[0]=='F');
    }
    if(isMatrix)
	{
	    QString content=elm->getContent();
	    m_vToolBarType->setButton(m_idButtonType_CAl,content[2]=='C');
	    m_vToolBarType->setButton(m_idButtonType_LAl,content[2]=='L');
	    m_vToolBarType->setButton(m_idButtonType_RAl,content[2]=='R');

	}
    if (isPrefixed)
	{
	    QString content=elm->getContent();

	    OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( ICON(content.left(1) + "higher.xpm" ));
	    m_vToolBarType->setButtonPixmap( m_idButtonType_AddH, pix );

	    pix = OPUIUtils::convertPixmap( ICON(content.left(1) + "lower.xpm" ));
	    m_vToolBarType->setButtonPixmap(m_idButtonType_AddL, pix );
	}

    debug("Type Changed");
    update();
}


void KFormulaView::slotModified()
{
    update();
}


void KFormulaView::newView()
{
    assert( (m_pDoc != 0L) );

    KFormulaShell* shell = new KFormulaShell;
    shell->show();
    shell->setDocument( m_pDoc );
}

void KFormulaView::addText()
{
    debug("adding Text");
    m_pDoc->addTextElement();
    //    m_vToolBarType->show();
}

void KFormulaView::addRoot()
{
    debug("adding Root");
    m_pDoc->addRootElement();
}

void KFormulaView::addFraction()
{
    debug("adding Fraction");
    m_pDoc->addFractionElement(DEFAULT_FRACTION);
}

void KFormulaView::addVertSpace()
{
    debug("adding VerticalSpace");
    m_pDoc->addFractionElement(DEFAULT_VSPACE);
}

void KFormulaView::addBracket()
{
    debug("adding parentheses");
    m_pDoc->addBracketElement(DEFAULT_DELIMITER);
    //    m_vToolBarType->hide();
}

void KFormulaView::addMatrix()
{
    debug("adding Matrix");
    MatrixSetupWidget *ms=new MatrixSetupWidget();
    ms->setString("MCC002002001006NNNNNN");
    QObject::connect(ms,SIGNAL(returnString(QString)),this,SLOT(createMatrix(QString)));
    ms->show();

}

void KFormulaView::createMatrix(QString str)
{
    if(str!="")
	m_pDoc->addMatrixElement(str);
    update();
}

void KFormulaView::modifyMatrix(QString str)
{
    int x,y,old;
    BasicElement *el=m_pDoc->currentElement();
    if (el==0)
	return;
    MatrixElement *elm = dynamic_cast<MatrixElement*>(el);
    if (elm==0)
	return;

    QString oldc=elm->getContent();
    x=atoi(oldc.mid(3,3));
    y=atoi(oldc.mid(6,3));
    old=x*y;
    x=atoi(str.mid(3,3));
    y=atoi(str.mid(6,3));
    elm->setChildrenNumber(x*y);
    for(int i=old;i<x*y;i++)
	elm->setChild(new BasicElement(m_pDoc,elm,i+4),i);	
    for(int i=x*y;i<old;i++)	
	delete    elm->getChild(i); //code to remove unused children...


    elm->setContent(str);
    update();
}

void KFormulaView::addIntegral()
{
    debug("adding Integral");
    m_pDoc->addPrefixedElement("IS000");
}

void KFormulaView::addSymbol()
{
    debug("##adding Symbol");
    m_pDoc->addPrefixedElement("IF06" );
}

void KFormulaView::reduce()
{
    if(m_pDoc->currentElement()==0) return;
    int level;
    level= FN_REDUCE | FN_P43;
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_2)) level=level | FN_ELEMENT;
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_3)) level=level | FN_INDEXES;
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_4)) level=level | FN_CHILDREN;
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_5)) level=level | FN_NEXT;

    m_pDoc->currentElement()->scaleNumericFont(level);

    update();
}

void KFormulaView::enlarge()
{
    if(m_pDoc->currentElement()==0) return;
    int level;
    level= FN_ENLARGE | FN_P43;

    //    warning(m_vToolBarFont->comboItem(m_idComboFont_ScaleMode));
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_2)) level=level | FN_ELEMENT;
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_3)) level=level | FN_INDEXES;
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_4)) level=level | FN_CHILDREN;
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_5)) level=level | FN_NEXT;

    m_pDoc->currentElement()->scaleNumericFont(level);
    update();
}


void KFormulaView::fractionAlignM()
{
    QString content=m_pDoc->currentElement()->getContent();
    content[1]='M';
    m_pDoc->currentElement()->setContent(content);
    m_vToolBarType->setButton(m_idButtonType_MAl,true);
    m_vToolBarType->setButton(m_idButtonType_DAl,false);
    m_vToolBarType->setButton(m_idButtonType_UAl,false);
    update();
}
void KFormulaView::fractionAlignU()
{

    QString content=m_pDoc->currentElement()->getContent();
    content[1]='U';
    m_pDoc->currentElement()->setContent(content);
    m_vToolBarType->setButton(m_idButtonType_MAl,false);
    m_vToolBarType->setButton(m_idButtonType_DAl,false);
    m_vToolBarType->setButton(m_idButtonType_UAl,true);
    update();
}

void KFormulaView::fractionAlignD()
{

    QString content=m_pDoc->currentElement()->getContent();
    content[1]='D';
    m_pDoc->currentElement()->setContent(content);
    m_vToolBarType->setButton(m_idButtonType_MAl,false);
    m_vToolBarType->setButton(m_idButtonType_DAl,true);
    m_vToolBarType->setButton(m_idButtonType_UAl,false);
    update();
}
void KFormulaView::fractionAlignL()
{

    QString content=m_pDoc->currentElement()->getContent();
    content[2]='L';
    m_pDoc->currentElement()->setContent(content);
    m_vToolBarType->setButton(m_idButtonType_CAl,false);
    m_vToolBarType->setButton(m_idButtonType_RAl,false);
    m_vToolBarType->setButton(m_idButtonType_LAl,true);
    update();
}
void KFormulaView::fractionAlignR()
{

    QString content=m_pDoc->currentElement()->getContent();
    content[2]='R';
    m_pDoc->currentElement()->setContent(content);
    m_vToolBarType->setButton(m_idButtonType_CAl,false);
    m_vToolBarType->setButton(m_idButtonType_RAl,true);
    m_vToolBarType->setButton(m_idButtonType_LAl,false);
    update();
}
void KFormulaView::fractionAlignC()
{

    QString content=m_pDoc->currentElement()->getContent();
    content[2]='C';
    m_pDoc->currentElement()->setContent(content);
    m_vToolBarType->setButton(m_idButtonType_CAl,true);
    m_vToolBarType->setButton(m_idButtonType_RAl,false);
    m_vToolBarType->setButton(m_idButtonType_LAl,false);
    update();
}

void KFormulaView::fractionDist()
{

}

void KFormulaView::fractionDistMore()
{
    QString content=m_pDoc->currentElement()->getContent();
    content.sprintf("%s%i",(const char*)content.left(3),(atoi(content.right(content.length()-3))+1));
    warning(content);
    m_pDoc->currentElement()->setContent(content);
    update();
}
void KFormulaView::fractionDistLess()
{
    QString content=m_pDoc->currentElement()->getContent();
    int space=(atoi(content.right(content.length()-3))-1);
    if (space<1) space =1;
    content.sprintf("%s%i",(const char*)content.left(3),space);
    warning(content);
    m_pDoc->currentElement()->setContent(content);
    update();
}

void KFormulaView::fontSwitch()
{
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_2)) warning("element");
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_3)) warning("index");
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_4)) warning("children");
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_5)) warning("next");
}

void KFormulaView::addTopLeftIndex()
{
    debug("add TopLeft index[0]");
    m_pDoc->addIndex(IN_TOPLEFT);

}

void KFormulaView::addBottomLeftIndex()
{
    debug("add BottomLeft index[1]");
    m_pDoc->addIndex(IN_BOTTOMLEFT);
}

void KFormulaView::addTopRightIndex()
{
    debug("add Top Right index[2]");
    m_pDoc->addIndex(IN_TOPRIGHT);
}

void KFormulaView::addBottomRightIndex()
{
    debug("add Top Right index[3]");
    m_pDoc->addIndex(IN_BOTTOMRIGHT);
}

void KFormulaView::indexList()
{
    debug("index");
    QPoint pnt(QCursor::pos());
    mn_indexList->popup(pnt);
}
void KFormulaView::insertIndex(int i)
{
    debug("index %i",i);
    m_pDoc->addIndex(i);
}

void KFormulaView::sizeSelected(const char *size)
{

    BasicElement *el=m_pDoc->currentElement();
    if (el==0) return;
    el->setNumericFont(atoi(size));
    warning(size);
    update();
}
void KFormulaView::fontSelected(const char *font)
{
    BasicElement *el=m_pDoc->currentElement();

    if (el==0)
	return;

    TextElement *te = dynamic_cast<TextElement*>(el);

    if (te==0)
	return;

    if (font == "(default)")
	font="";

    te->changeFontFamily(font);
    warning(font);
    update();
}

void KFormulaView::modeSelected(const char *)
{
    warning("mode");
}
void KFormulaView::textFont()
{
    warning("Slot textFont");
}
void KFormulaView::textSplit()
{
    warning("Slot textSplit");

    BasicElement *el=m_pDoc->currentElement();

    if (el==0)
	return;

    TextElement *te = dynamic_cast<TextElement*>(el);

    if (te==0)
	return;

    te->split(-1);
    update();

}
void KFormulaView::togglePixmap()
{
    warning("Slot togglePixmap");
}
void KFormulaView::integralLower()
{
    warning("Slot integralLower");
    m_pDoc->addChild(2);
}
void KFormulaView::integralHigher()
{
    warning("Slot integralHigher");
    m_pDoc->addChild(1);
}
void KFormulaView::bracketType()
{
    warning("Slot bracketType");
}
void KFormulaView::matrixSet()
{
    MatrixSetupWidget *ms=new MatrixSetupWidget();
    ms->setString(m_pDoc->currentElement()->getContent());
    QObject::connect(ms,SIGNAL(returnString(QString)),this,SLOT(modifyMatrix(QString)));
    ms->show();

}
void KFormulaView::matrixRemRow()
{
    warning("Slot matrixRemRow");
}
void KFormulaView::matrixRemCol()
{
    warning("Slot matrixRemCol");
}
void KFormulaView::matrixInsRow()
{
    warning("Slot matrixInsRow");
}
void KFormulaView::matrixInsCol()
{
    warning("Slot matrixInsCol");
}
void KFormulaView::fractionHAlign()
{
    warning("Slot fractionHAlign");
}
void KFormulaView::toggleMidline()
{

    warning("Slot toggleMidline");
    QString content=m_pDoc->currentElement()->getContent();
    if (content[0]=='F') content[0]='V'; else content[0]='F';
    m_pDoc->currentElement()->setContent(content);
    update();
}
void KFormulaView::symbolType()
{
    warning("Slot sybolType");
}
void KFormulaView::DecorationType()
{
    warning("Slot decorationType");
}
void KFormulaView::remove()
{
    warning("Slot remove");
}
void KFormulaView::elementColor()
{
    warning("Slot elementColor");
}
void KFormulaView::generalColor()
{
    warning("Slot generalColor");
}
void KFormulaView::generalFont()
{
    warning("Slot generalFont");
}
void KFormulaView::delimiterLeft(const char *left)
{
    QString content=m_pDoc->currentElement()->getContent();
    warning(content);
    content[0]=left[0];
    warning(content);
    m_pDoc->currentElement()->setContent(content);
    update();
}

void KFormulaView::delimiterRight(const char *right)
{
    QString content=m_pDoc->currentElement()->getContent();
    warning(content);
    content[1]=right[0];
    warning(content);
    m_pDoc->currentElement()->setContent(content);
    update();
}

CORBA::Boolean KFormulaView::printDlg()
{

    QPrinter thePrt;
    //  thePrt.setMinMax(1,1);
    if (thePrt.setup(this))
	{
	    if(m_pDoc!=0L)
		m_pDoc->print(&thePrt);
	}
    return true;
}

#include "kformula_view.moc"






