// has to be included first
#include <qprinter.h>

#include <qpainter.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qkeycode.h>

#include <kiconloader.h>

#include "TextElement.h"
#include "matrixwidget.h"
#include "BracketElement.h"
#include "RootElement.h"
#include "TextElement.h"
#include "FractionElement.h"
#include "MatrixElement.h"
#include "PrefixedElement.h"

//#include <kglobal.h>

/**********************************************************
 *
 * KFormulaView
 *
 **********************************************************/
#include "kformula_view.h"
#include "kformula_global.h"
#include "kformula_doc.h"

#include <qpainter.h>
#include <qaction.h>

KFormulaView::KFormulaView( KFormulaDoc* _doc, QWidget* _parent, const char* _name )
    : KoView( _doc, _parent, _name )
{
//    setWidget( this );
//    widget()->setFocusPolicy( QWidget::StrongFocus );

//    OPDocIf::setFocusPolicy( OpenDocs::Doc::ClickFocus );

//    setBackgroundColor( white );

    m_pDoc = _doc;
    setInstance( KFormulaFactory::global());
    setXMLFile( "kformula.rc" );

    mn_indexList = new QPopupMenu();
    mn_indexList->insertItem(BarIcon("index0"),0);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(BarIcon("index1"),1);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(BarIcon("index2"),2);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(BarIcon("index3"),3);
    mn_indexList->setMouseTracking(true);
    mn_indexList->setCheckable(false);



    actionEditUndo = new KAction( i18n( "No Undo possible" ),
				     "undo", 
				     ALT + Key_Z,
				     this, SLOT( addText() ),
				     actionCollection(), "edit_undo" );

    actionElement_AddElement_T = new KAction(i18n( "Add/change to simple text" ),
					"mini-xy",
					 CTRL + Key_1 ,
					this,SLOT(addText()),
					actionCollection(),"addtext");

    actionElement_AddElement_R = new KAction(i18n( "Add/change to root" ),
					"mini-root",
					 CTRL + Key_2 ,
					this,SLOT(addRoot()),
					actionCollection(),"addroot");

    actionElement_AddElement_F = new KAction(i18n( "Add/change to fraction" ),
					"mini-frac",
					 CTRL + Key_3 ,
					this,SLOT(addFraction()),
					actionCollection(),"addfrac");

    actionElement_AddElement_V = new KAction(i18n( "Add/change to vertical space" ),
					"mini-vspace",
					 CTRL + Key_4 ,
					this,SLOT(addVertSpace()),
					actionCollection(),"addvspace");

    actionElement_AddElement_B = new KAction(i18n( "Add/change to bracket" ),
					"mini-bra",
					 CTRL + Key_5 ,
					this,SLOT(addBracket()),
					actionCollection(),"addbra");

    actionElement_AddElement_I = new KAction(i18n( "Add/change to integral" ),
					"mini-integral",
					 CTRL + Key_6 ,
					this,SLOT(addIntegral()),
					actionCollection(),"addintegral");

    actionElement_AddElement_S = new KAction(i18n( "Add/change to symbol" ),
					"mini-symbol",
					 CTRL + Key_7 ,
					this,SLOT(addSymbol()),
					actionCollection(),"addsymbol");

    actionElement_AddElement_M = new KAction(i18n( "Add/change to matrix" ),
					"matrix",
					 CTRL + Key_8 ,
					this,SLOT(addMatrix()),
					actionCollection(),"addmatrix");

    actionElement_AddElement_L = new KAction(i18n( "Add index at pos..." ),
					"index",
					 CTRL + Key_9 ,
					this,SLOT(indexList()),
					actionCollection(),"addindex");

    actionElement_Text_Font = new KFontAction(i18n( "Font family" ),0,
					this,SLOT(fontSelected()),
					actionCollection(),"textfont");
    actionElement_Text_Size = new KFontSizeAction(i18n( "Size" ),0,
					this,SLOT(sizeSelected()),
					actionCollection(),"textsize");

    actionElement_Text_Bold = new KToggleAction(i18n( "Bold" ),
					"bold",
					 CTRL + Key_B ,
					this,SLOT(fontSwitch()),
					actionCollection(),"textbold");

    actionElement_Text_Italic = new KToggleAction(i18n( "Italic" ),
					"italic",
					 CTRL + Key_I ,
					this,SLOT(fontSwitch()),
					actionCollection(),"textitalic");

    actionElement_Text_Under = new KToggleAction(i18n( "Underlined" ),
					"underl",
					 CTRL + Key_U ,
					this,SLOT(fontSwitch()),
					actionCollection(),"textunder");


    actionElement_Font_Element = new KToggleAction(i18n( "Resize element" ),
					"elementsw",
					 CTRL + ALT + Key_E ,
					this,SLOT(fontSwitch()),
					actionCollection(),"fontelement");
    
    //actionElement_Font_Element->setEnabled(false);

    actionElement_Font_Child = new KToggleAction(i18n( "Resize children" ),
					"childrensw",
					 CTRL + ALT + Key_C ,
					this,SLOT(fontSwitch()),
					actionCollection(),"fontchild");

    actionElement_Font_Index = new KToggleAction(i18n( "Resize index" ),
					"indexsw",
					 CTRL + ALT + Key_I ,
					this,SLOT(fontSwitch()),
					actionCollection(),"fontindex");

    actionElement_Font_Next = new KToggleAction(i18n( "Resize next" ),
					"nextsw",
					 CTRL + ALT + Key_N ,
					this,SLOT(fontSwitch()),
					actionCollection(),"fontnext");

    // ************************* Text *********************

    actionElement_Text_Split = new KAction(i18n( "Split text" ),
					"split",
					 CTRL + Key_0 ,
					this,SLOT(textSplit()),
					actionCollection(),"textsplit");

    // ************************** Root ********************

    actionElement_Root_Index = new KAction(i18n( "Root index" ),
					"rootindex",
					 CTRL + Key_0 ,
					this,SLOT(addTopLeftIndex()),
					actionCollection(),"rootindex");

//    m_idButtonType_RIn = addToolButton(m_vToolBarType, "rootindex",
//				       i18n( "Add root index (i.e. Top Left index)" ), "addTopLeftIndex",  1 );

    // ************************** Brackets ********************	
    //m_vToolBarType->insertSeparator(  -1);

    //    m_idButtonType_Del = addToolButton(m_vToolBarType, "delimiter",
    //				     i18n( "Set delimiter type" ), "bracketType" );
    QStringList delimiter;
    delimiter.clear();
    delimiter.append(QString("("));
    delimiter.append(QString("["));
    delimiter.append(QString("<"));
    delimiter.append(QString("/"));
    delimiter.append(QString("\\"));
    delimiter.append(QString(")"));
    delimiter.append(QString("]"));
    delimiter.append(QString(">"));
    delimiter.append(QString("|"));
    //toolTip = Q2C( i18n("Left delimiter") );
    actionElement_Bracket_Type_Left = new KSelectAction(i18n("Left delimiter"),
					   0,this, SLOT(delimiterLeft()),   
					   actionCollection(),"typeleft");
    ((KSelectAction *)actionElement_Bracket_Type_Left)->setItems(delimiter);
/*
    m_idComboType_DelLeft = m_vToolBarType->insertCombo( delimiter, 2 ,false, SIGNAL( activated( const QString & ) ), this,
							 "delimiterLeft", true, toolTip,
							 40, -1, OpenDocsUI::AtBottom );
*/
    delimiter.clear();
    delimiter.append(QString(")"));
    delimiter.append(QString("]"));
    delimiter.append(QString(">"));
    delimiter.append(QString("/"));
    delimiter.append(QString("\\"));
    delimiter.append(QString("("));
    delimiter.append(QString("["));
    delimiter.append(QString("<"));
    delimiter.append(QString("|"));
    actionElement_Bracket_Type_Right = new KSelectAction(i18n("Right delimiter"),
					   0,this, SLOT(delimiterRight()),   
					   actionCollection(),"typeright");
    ((KSelectAction *)actionElement_Bracket_Type_Right)->setItems(delimiter);

//    toolTip = Q2C( i18n("Right delimiter") );
//    m_idComboType_DelRight = m_vToolBarType->insertCombo( delimiter, 3, false, SIGNAL( activated( const QString & ) ), this,
//							  "delimiterRight", true, toolTip,
//							  40, -1, OpenDocsUI::AtBottom );
    //m_vToolBarType->insertSeparator(  7 );

    // ************************** Fraction ********************

    actionElement_Fraction_VA_M = new KToggleAction(i18n( "Middle align" ),
					"midalign",
					 0 ,
					this,SLOT(fractionAlignM()),
					actionCollection(),"fracmiddle");

//   ((KToggleAction *)actionElement_Fraction_VA_M)->setExclusiveGroup("FractionVa");

    actionElement_Fraction_VA_U = new KToggleAction(i18n( "Numerator align" ),
					"upalign",
					 0 ,
					this,SLOT(fractionAlignU()),
					actionCollection(),"fracup");

  // ((KToggleAction *)actionElement_Fraction_VA_U)->setExclusiveGroup("FractionVa");

    actionElement_Fraction_VA_D = new KToggleAction(i18n( "Denominator align" ),
					"downalign",
					 0 ,
					this,SLOT(fractionAlignD()),
					actionCollection(),"fracdown");

  // ((KToggleAction *)actionElement_Fraction_VA_D)->setExclusiveGroup("FractionVa");

    actionElement_Fraction_HA_C = new KToggleAction(i18n( "Center align" ),
					"centralign",
					 0 ,
					this,SLOT(fractionAlignC()),
					actionCollection(),"fraccenter");

  // ((KToggleAction *)actionElement_Fraction_HA_C)->setExclusiveGroup("FractionHa");

    actionElement_Fraction_HA_L = new KToggleAction(i18n( "Left align" ),
					"leftalign",
					 0 ,
					this,SLOT(fractionAlignL()),
					actionCollection(),"fracleft");

  // ((KToggleAction *)actionElement_Fraction_HA_L)->setExclusiveGroup("FractionHa");

    actionElement_Fraction_HA_R = new KToggleAction(i18n( "Right align" ),
					"rightalign",
					 0 ,
					this,SLOT(fractionAlignR()),
					actionCollection(),"fracright");


  // ((KToggleAction *)actionElement_Fraction_HA_R)->setExclusiveGroup("FractionHa");

    actionElement_Fraction_MidLine = new KToggleAction(i18n( "Draw midline" ),
					"midline",
					 0 ,
					this,SLOT(toggleMidline()),
					actionCollection(),"fracmidline");

    actionElement_Fraction_Far = new KAction(i18n( "More distant" ),
					"far",
					 0 ,
					this,SLOT(fracyionDistMore()),
					actionCollection(),"fracfar");

    actionElement_Fraction_Near = new KAction(i18n( "Less distant" ),
					"near",
					 0 ,
					this,SLOT(fracyionDistLess()),
					actionCollection(),"fracnear");


    actionElement_Integral_Lower = new KAction(i18n( "Low limit" ),
					"Ilower",
					 0 ,
					this,SLOT(integralLower()),
					actionCollection(),"ilower");

    actionElement_Integral_Higher = new KAction(i18n( "Hi limit" ),
					"Ihigher",
					 0 ,
					this,SLOT(integralHigher()),
					actionCollection(),"ihigher");

    actionElement_Matrix_Set = new KAction(i18n( "Matrix setup" ),
					"matrix",
					 0 ,
					this,SLOT(matrixSet()),
					actionCollection(),"matrixset");

    actionElement_Matrix_Ins_Row = new KAction(i18n( "Insert row" ),
					"insrow",
					 0 ,
					this,SLOT(matrixInsRow()),
					actionCollection(),"matrixinsrow");
    actionElement_Matrix_Ins_Col = new KAction(i18n( "Insert col" ),
					"inscol",
					 0 ,
					this,SLOT(matrixInsCol()),
					actionCollection(),"matrixinscol");

    actionElement_Matrix_Rem_Row = new KAction(i18n( "Remove row" ),
					"remrow",
					 0 ,
					this,SLOT(matrixRemRow()),
					actionCollection(),"matrixremrow");
    actionElement_Matrix_Rem_Col = new KAction(i18n( "Remove col" ),
					"remcol",
					 0 ,
					this,SLOT(matrixRemCol()),
					actionCollection(),"matrixremcol");

/*
    //m_vToolBarType->insertSeparator( 17 );

    // *******************  Integral **************************+


    m_idButtonType_AddH = addToolButton(m_vToolBarType, "Ihigher",
					i18n( "Add higher limit" ), "integralHigher", 13 );

    m_idButtonType_AddL = addToolButton(m_vToolBarType, "Ilower",
					i18n( "Add lower limit" ), "integralLower", 14 );

    //m_vToolBarType->insertSeparator( 20 );

    // *********************** Matrix *************************
    m_idButtonType_SetM = addToolButton(m_vToolBarType, "matrix",
					i18n( "Set matrix dimension" ), "matrixSet", 15 );

    m_idButtonType_InR = addToolButton(m_vToolBarType, "insrow",
				       i18n( "Insert a row" ), "matrixInsRow", 16 );

    m_idButtonType_InC = addToolButton(m_vToolBarType, "inscol",
				       i18n( "Insert a column" ), "matrixInsCol",17  );

    m_idButtonType_ReR = addToolButton(m_vToolBarType, "remrow",
				       i18n( "Remove a row" ), "matrixRemRow", 18 );

    m_idButtonType_ReC = addToolButton(m_vToolBarType, "remcol",
				       i18n( "Remove a column" ), "matrixRemCol", 19 );

    //m_vToolBarType->insertSeparator( 26 );
    m_vToolBarType->enable( OpenDocsUI::Show );

    // *********************** General *************************
    m_idButtonType_Pix = addToolButton(m_vToolBarType, "remcol",
				       i18n( "Toggle pixmap use" ), "togglePixmap", 20 );
    warning("vai");
    m_vToolBarType->setToggle( m_idButtonType_UAl, true );
    m_vToolBarType->setToggle( m_idButtonType_MAl, true );
    m_vToolBarType->setToggle( m_idButtonType_DAl, true );
    m_vToolBarType->setToggle( m_idButtonType_CAl, true );
    m_vToolBarType->setToggle( m_idButtonType_LAl, true );
    m_vToolBarType->setToggle( m_idButtonType_RAl, true );
    m_vToolBarType->setToggle( m_idButtonType_Mid, true );
	


*/




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
/*
    cerr << "Registering menu as " << id() << endl;

    OpenDocs::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
    if ( !CORBA::is_nil( menu_bar_manager ) )
	menu_bar_manager->registerClient( id(), this );
    else
	cerr << "Did not get a menu bar manager" << endl;
*/
  /******************************************************
   * Toolbar
   ******************************************************/
/*
    OpenDocs::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
    if ( !CORBA::is_nil( tool_bar_manager ) )
	tool_bar_manager->registerClient( id(), this );
    else
	cerr << "Did not get a tool bar manager" << endl;
*/

}

KFormulaView::~KFormulaView()
{
    cleanUp();
}

void KFormulaView::cleanUp()
{
/*    if ( m_bIsClean )
	return;

    OpenDocs::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
    if ( !CORBA::is_nil( menu_bar_manager ) )
	menu_bar_manager->unregisterClient( id() );

    OpenDocs::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
    if ( !CORBA::is_nil( tool_bar_manager ) )
	tool_bar_manager->unregisterClient( id() );

    m_pDoc->removeView( this );

    KoViewIf::cleanUp();
*/
}

void KFormulaView::paintEvent( QPaintEvent *_ev )
{
    m_pDoc->paintEvent(_ev, this);
/* void KFormulaView::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    part()->paintEverything( painter, ev->rect(), FALSE, this );

    painter.end();
}

*/
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


/*
CORBA::Long KFormulaView::addToolButton( OpenDocsUI::ToolBar_ptr toolbar,
					 const char* pictname,
					 const QString &tooltip,
					 const char* func,
					 CORBA::Long _id )
{
    OpenDocsUI::Pixmap_var pix = OPUIUtils::convertPixmap( BarIcon(pictname) );
    CORBA::WString_var toolTip = Q2C( tooltip );

    CORBA::Long id =
	toolbar->insertButton2( pix, _id,
				SIGNAL( clicked() ), this, func, true,
				toolTip, -1 );

    return id;
}*/
/*
bool KFormulaView::event( const QCString & _event, const CORBA::Any& _value )
{
    cerr << "CALLED" << endl;

    EVENT_MAPPER( _event, _value );

    MAPPING( OpenDocsUI::eventCreateMenuBar, OpenDocsUI::typeCreateMenuBar_ptr, mappingCreateMenubar );
    MAPPING( OpenDocsUI::eventCreateToolBar, OpenDocsUI::typeCreateToolBar_ptr, mappingCreateToolbar );

    END_EVENT_MAPPER;

    cerr << "CALLE2D" << endl;

    return false;
}
*/
/*
bool KFormulaView::mappingCreateMenubar( OpenDocsUI::MenuBar_ptr _menubar )
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

    OpenDocsUI::Pixmap_var pix = OPUIUtils::convertPixmap( BarIcon("index0"));
    text = Q2C( i18n( "Top left" ) );
    m_idMenuElement_AddIndex_TL = m_vMenuElement_AddIndex->insertItem6( pix, text, this, "addTopLeftIndex", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(BarIcon("index1") );
    text = Q2C( i18n( "Bottom left" ) );
    m_idMenuElement_AddIndex_BL = m_vMenuElement_AddIndex->insertItem6( pix, text, this, "addBottomLeftIndex", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(BarIcon("index2") );
    text = Q2C( i18n( "Top right" ) );
    m_idMenuElement_AddIndex_TR = m_vMenuElement_AddIndex->insertItem6( pix, text, this, "addTopRightIndex", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(BarIcon("index3") );
    text = Q2C( i18n( "Bottom right" ) );
    m_idMenuElement_AddIndex_BL = m_vMenuElement_AddIndex->insertItem6( pix, text, this, "addBottomRightIndex", 0, -1, -1 );

    text = Q2C( i18n( "&Add Element" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_AddElement, -1, -1 );	

    pix = OPUIUtils::convertPixmap(BarIcon("mini-xy") );
    text = Q2C( i18n( "Plain text" ) );
    m_idMenuElement_AddElement_T = m_vMenuElement_AddElement->insertItem6( pix, text, this, "addText", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(BarIcon("mini-root") );
    text = Q2C( i18n( "Root" ) );
    m_idMenuElement_AddElement_R = m_vMenuElement_AddElement->insertItem6( pix, text, this, "addRoot", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(BarIcon("mini-bra") );
    text = Q2C( i18n( "Bracket" ) );
    m_idMenuElement_AddElement_B = m_vMenuElement_AddElement->insertItem6( pix, text, this, "addBracket", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(BarIcon("mini-frac") );
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

    pix = OPUIUtils::convertPixmap(BarIcon("split") );
    text = Q2C( i18n( "Split at current position" ) );
    m_idMenuElement_Text_Split = m_vMenuElement_Text->insertItem6( pix, text, this, "textSplit", 0, -1, -1 );

    text = Q2C( i18n( "&Root" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_Root, -1, -1 );	

    text = Q2C( i18n( "Use pixmap" ) );
    m_idMenuElement_Root_Pixmap = m_vMenuElement_Root->insertItem( text, this, "togglePixmap", 0 );

    pix = OPUIUtils::convertPixmap(BarIcon("rootindex") );
    text = Q2C( i18n( "Add root index (Top left)" ) );
    m_idMenuElement_Root_Index = m_vMenuElement_Root->insertItem6( pix, text, this, "addTopLeftIndex", 0, -1, -1 );

    text = Q2C( i18n( "&Bracket" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_Bracket, -1, -1 );	

    pix = OPUIUtils::convertPixmap(BarIcon("delimiter") );
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

    pix = OPUIUtils::convertPixmap(BarIcon("Ihigher") );
    text = Q2C( i18n( "Add higher limit" ) );
    m_idMenuElement_Integral_Higher = m_vMenuElement_Integral->insertItem6( pix, text, this, "addHigher", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(BarIcon("Ilower") );
    text = Q2C( i18n( "Add lower limit" ) );
    m_idMenuElement_Integral_Lower = m_vMenuElement_Integral->insertItem6( pix, text, this, "addLower", 0, -1, -1 );

    text = Q2C( i18n( "&Matrix" ) );
    m_vMenuElement->insertItem8( text, m_vMenuElement_Matrix, -1, -1 );

    // pix = OPUIUtils::convertPixmap(BarIcon("setmatrix" );
    // m_idMenuElement_Matrix_Set = m_vMenuElement_Matrix->insertItem6( pix, i18n( "Set dimension" ), this, "matrixSet", 0, -1, -1 );
    text = Q2C( i18n( "Set dimension" ) );
    m_idMenuElement_Matrix_Set = m_vMenuElement_Matrix->insertItem( text, this, "matrixSet", 0 );

    pix = OPUIUtils::convertPixmap(BarIcon("insrow") );
    text = Q2C( i18n( "Insert a row" ) );
    m_idMenuElement_Matrix_InsRow = m_vMenuElement_Matrix->insertItem6( pix, text, this, "matrixInsRow", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(BarIcon("inscol") );
    text = Q2C( i18n( "Insert a column" ) );
    m_idMenuElement_Matrix_InsCol = m_vMenuElement_Matrix->insertItem6( pix, text, this, "matrixInsCol", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(BarIcon("remrow") );
    text = Q2C( i18n( "Remove a row" ) );
    m_idMenuElement_Matrix_RemRow = m_vMenuElement_Matrix->insertItem6( pix, text, this, "matrixRemRow", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap(BarIcon("remcol") );
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

bool KFormulaView::mappingCreateToolbar( OpenDocsUI::ToolBarFactory_ptr _factory )
{
    cerr << "bool KFormulaView::mappingCreateToolbar( OpenDocsUI::ToolBarFactory_ptr _factory )" << endl;

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
    m_vToolBarFormula = _factory->create( OpenDocsUI::ToolBarFactory::Transient );

    m_idButtonFormula_0 = addToolButton(m_vToolBarFormula, "mini-xy",
					i18n( "Add/change to simple text" ),
					"addText", 0 );


    m_idButtonFormula_1 = addToolButton(m_vToolBarFormula, "mini-root",
					i18n( "Add/change to root" ), "addRoot", 1 );

    m_idButtonFormula_2 = addToolButton(m_vToolBarFormula, "mini-frac",
					i18n( "Add/change fract line" ), "addFraction", 2 );

    m_idButtonFormula_3 = addToolButton(m_vToolBarFormula, "mini-vspace",
					i18n( "Add/change vertical space" ), "addVertSpace", 3 );
	
    m_idButtonFormula_4 = addToolButton(m_vToolBarFormula, "mini-bra",
					i18n( "Add/change a bracket block" ), "addBracket", 4 );

    m_idButtonFormula_5 = addToolButton(m_vToolBarFormula, "mini-integral",
					i18n( "Add/change an integral" ), "addIntegral", 5 );

    m_idButtonFormula_6 = addToolButton(m_vToolBarFormula, "mini-symbols",
					i18n( "Add/change a block with symbols" ), "addSymbol", 6 );

    m_idButtonFormula_6 = addToolButton(m_vToolBarFormula, "matrix",
					i18n( "Add/change matrix" ), "addMatrix", 7  );
    	
    m_idButtonFormula_7 = addToolButton(m_vToolBarFormula, "index",
					i18n( "Add an index at position..." ), "indexList", 8 );

    m_vToolBarFormula->enable( OpenDocsUI::Show );
    // Font
    m_vToolBarFont = _factory->create( OpenDocsUI::ToolBarFactory::Transient );

    OpenDocsUI::StrList fonts;
    fonts.length( 4 );
    fonts[0] = CORBA::string_dup( "utopia" );
    fonts[1] = CORBA::string_dup( "symbol" );
    fonts[2] = CORBA::string_dup( "times" );
    fonts[3] = CORBA::string_dup( "(default)" );

    CORBA::WString_var toolTip;

    toolTip = Q2C( i18n("Font Family") );
    m_idComboFont_FontFamily = m_vToolBarFont->insertCombo( fonts,   0, false, SIGNAL( activated( const QString & ) ), this,
							    "fontSelected", true, toolTip,
							    120, -1, OpenDocsUI::AtBottom );
    m_vToolBarFont->setCurrentComboItem( m_idComboFont_FontFamily, 0 );
	
    OpenDocsUI::StrList sizelist;
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
							  toolTip, 50, -1, OpenDocsUI::AtBottom );
    m_vToolBarFont->setCurrentComboItem( m_idComboFont_FontSize, 2  );

    m_idButtonFont_Bold = addToolButton(m_vToolBarFont, "bold",
					i18n( "Bold" ), "fontSwitch", 2 );

    m_idButtonFont_Italic = addToolButton(m_vToolBarFont, "italic",
					  i18n( "Italic" ),"fontSwitch", 3 );

    m_idButtonFont_Underl = addToolButton(m_vToolBarFont, "underl",
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
							   toolTip, 80, -1, OpenDocsUI::AtBottom );


    //    m_vToolBarFont->setCurrentComboItem( m_idComboFont_ScaleMode,  2 );
    warning("provo1");
    m_idButtonFont_0 = addToolButton(m_vToolBarFont, "reduce",
				     i18n( "Reduce the fontSize" ), "reduce", 6 );

    m_idButtonFont_1 = addToolButton(m_vToolBarFont, "enlarge",
				     i18n( "Increase the fontSize" ), "enlarge", 7 );

    //m_vToolBarFont->insertSeparator( -1 );

    m_idButtonFont_2 = addToolButton(m_vToolBarFont, "elementsw",
				     i18n( "Reduce/Increase active element fontSize" ),"fontSwitch", 8 );

    m_idButtonFont_3 = addToolButton(m_vToolBarFont, "indexsw",
				     i18n( "Reduce/Increase indexes fontSize" ),"fontSwitch",  9 );

    m_idButtonFont_4 = addToolButton(m_vToolBarFont, "childrensw",
				     i18n( "Reduce/Increase children (content) fontSize" ), "fontSwitch", 10 );

    m_idButtonFont_5 = addToolButton(m_vToolBarFont, "nextsw",
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

    m_vToolBarFont->enable( OpenDocsUI::Show );	
    warning("Fine ");
    // Type
    m_vToolBarType = _factory->create( OpenDocsUI::ToolBarFactory::Transient );

    // ************************* Text *********************
    m_idButtonType_Spl = addToolButton(m_vToolBarType, "split",
				       i18n( "Split text into 2 elment" ), "textSplit", 0 );
    //m_vToolBarType->insertSeparator(  -1);
    // ************************** Root ********************
    m_idButtonType_RIn = addToolButton(m_vToolBarType, "rootindex",
				       i18n( "Add root index (i.e. Top Left index)" ), "addTopLeftIndex",  1 );

    // ************************** Brackets ********************	
    //m_vToolBarType->insertSeparator(  -1);

    //    m_idButtonType_Del = addToolButton(m_vToolBarType, "delimiter",
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
							 40, -1, OpenDocsUI::AtBottom );

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
							  40, -1, OpenDocsUI::AtBottom );
    //m_vToolBarType->insertSeparator(  7 );

    // ************************** Fraction ********************
    m_idButtonType_MAl = addToolButton(m_vToolBarType, "midalign",
				       i18n( "Align fraction to midline" ), "fractionAlignM", 4 );

    m_idButtonType_UAl = addToolButton(m_vToolBarType, "upalign",
				       i18n( "Align fraction to numerator" ), "fractionAlignU", 5 );

    m_idButtonType_DAl = addToolButton(m_vToolBarType, "downalign",
				       i18n( "Align fraction to denominator" ), "fractionAlignD",6  );

    m_idButtonType_CAl = addToolButton(m_vToolBarType, "centralign",
				       i18n( "Align center" ), "fractionAlignC", 7 );

    m_idButtonType_LAl = addToolButton(m_vToolBarType, "leftalign",
				       i18n( "Align left" ), "fractionAlignL", 8 );

    m_idButtonType_RAl = addToolButton(m_vToolBarType, "rightalign",
				       i18n( "Align Right" ), "fractionAlignR",  9 );

    m_idButtonType_Les = addToolButton(m_vToolBarType, "near",
				       i18n( "Reduce element vertical distance" ), "fractionDistLess", 10 );

    m_idButtonType_Mor = addToolButton(m_vToolBarType, "far",
				       i18n( "Increase element vertical distance" ), "fractionDistMore", 11 );

    m_idButtonType_Mid = addToolButton(m_vToolBarType, "midline",
				       i18n( "Toggle fraction line" ), "toggleMidline", 12 );

    //m_vToolBarType->insertSeparator( 17 );

    // *******************  Integral **************************+
    m_idButtonType_AddH = addToolButton(m_vToolBarType, "Ihigher",
					i18n( "Add higher limit" ), "integralHigher", 13 );

    m_idButtonType_AddL = addToolButton(m_vToolBarType, "Ilower",
					i18n( "Add lower limit" ), "integralLower", 14 );

    //m_vToolBarType->insertSeparator( 20 );

    // *********************** Matrix *************************
    m_idButtonType_SetM = addToolButton(m_vToolBarType, "matrix",
					i18n( "Set matrix dimension" ), "matrixSet", 15 );

    m_idButtonType_InR = addToolButton(m_vToolBarType, "insrow",
				       i18n( "Insert a row" ), "matrixInsRow", 16 );

    m_idButtonType_InC = addToolButton(m_vToolBarType, "inscol",
				       i18n( "Insert a column" ), "matrixInsCol",17  );

    m_idButtonType_ReR = addToolButton(m_vToolBarType, "remrow",
				       i18n( "Remove a row" ), "matrixRemRow", 18 );

    m_idButtonType_ReC = addToolButton(m_vToolBarType, "remcol",
				       i18n( "Remove a column" ), "matrixRemCol", 19 );

    //m_vToolBarType->insertSeparator( 26 );
    m_vToolBarType->enable( OpenDocsUI::Show );

    // *********************** General *************************
    m_idButtonType_Pix = addToolButton(m_vToolBarType, "remcol",
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
*/

void KFormulaView::slotTypeChanged( const BasicElement *elm)
{
    
//    actionElement_Font_Element->setEnabled(false);

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

    actionElement_Text_Split->setEnabled(isText);
    // It remains deactivated !!!
    //    m_rMenuBar->setItemEnabled(m_idMenuElement_Fraction,isFraction);
   (actionElement_Root_Index)->setEnabled(isRoot);
    warning("Toggle");
    (actionElement_Bracket_Type_Left)->setEnabled(isBracket);
    (actionElement_Bracket_Type_Right)->setEnabled(isBracket);
    
    (actionElement_Fraction_VA_U)->setEnabled(isFraction);
    (actionElement_Fraction_VA_D)->setEnabled(isFraction);
    (actionElement_Fraction_VA_M)->setEnabled(isFraction);
    (actionElement_Fraction_HA_C)->setEnabled(isFraction);
    (actionElement_Fraction_HA_L)->setEnabled(isFraction);
    (actionElement_Fraction_HA_R)->setEnabled(isFraction);
    warning("End");
    (actionElement_Fraction_Near)->setEnabled(isFraction);
    (actionElement_Fraction_Far)->setEnabled(isFraction);
    (actionElement_Fraction_MidLine)->setEnabled(isFraction);
    
    (actionElement_Integral_Lower)->setEnabled(isPrefixed);
    (actionElement_Integral_Higher)->setEnabled(isPrefixed);
    (actionElement_Matrix_Set)->setEnabled(isMatrix);
    (actionElement_Matrix_Ins_Row)->setEnabled(isMatrix);
    (actionElement_Matrix_Ins_Col)->setEnabled(isMatrix);
    (actionElement_Matrix_Rem_Row)->setEnabled(isMatrix);
    (actionElement_Matrix_Rem_Col)->setEnabled(isMatrix);

    if (elm)
	((KFontSizeAction *)actionElement_Text_Size)->setFontSize(elm->getNumericFont());

    if (isFraction) {
      QString content=elm->getContent();
      ((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(content[2]=='C');
      ((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(content[2]=='L');
      ((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(content[2]=='R');
      ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(content[1]=='M');
      ((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(content[1]=='U');
      ((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(content[1]=='D');
      ((KToggleAction *)actionElement_Fraction_MidLine)->setChecked(content[0]=='F');
    }
   if(isMatrix)
	{
	    QString content=elm->getContent();
      ((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(content[2]=='C');
      ((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(content[2]=='L');
      ((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(content[2]=='R');

	}
/*
    if (isPrefixed)
	{
	    QString content=elm->getContent();

	    OpenDocsUI::Pixmap_var pix = OPUIUtils::convertPixmap( BarIcon(content.left(1) + "higher" ));
	    m_vToolBarType->setButtonPixmap( m_idButtonType_AddH, pix );

	    pix = OPUIUtils::convertPixmap( BarIcon(content.left(1) + "lower" ));
	    m_vToolBarType->setButtonPixmap(m_idButtonType_AddL, pix );
	}

    debug("Type Changed");
    update();
*/
//    actionElement_Font_Element->setEnabled(false);

}


void KFormulaView::slotModified()
{
    update();
}


/*void KFormulaView::newView()
{
    assert( (m_pDoc != 0L) );

    KFormulaShell* shell = new KFormulaShell;
    shell->show();
    shell->setDocument( m_pDoc );
}
*/
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
    if(((KToggleAction*)actionElement_Font_Element)->isChecked()) level=level | FN_ELEMENT;
    if(((KToggleAction*)actionElement_Font_Index)->isChecked())  level=level | FN_INDEXES;
    if(((KToggleAction*)actionElement_Font_Child)->isChecked()) level=level | FN_CHILDREN;
    if(((KToggleAction*)actionElement_Font_Next)->isChecked())  level=level | FN_NEXT;

    m_pDoc->currentElement()->scaleNumericFont(level);

    update();
}

void KFormulaView::enlarge()
{
    if(m_pDoc->currentElement()==0) return;
    int level;
    level= FN_ENLARGE | FN_P43;

    //    warning(m_vToolBarFont->comboItem(m_idComboFont_ScaleMode));
    if(((KToggleAction*)actionElement_Font_Element)->isChecked()) level=level | FN_ELEMENT;
    if(((KToggleAction*)actionElement_Font_Index)->isChecked())  level=level | FN_INDEXES;
    if(((KToggleAction*)actionElement_Font_Child)->isChecked()) level=level | FN_CHILDREN;
    if(((KToggleAction*)actionElement_Font_Next)->isChecked())  level=level | FN_NEXT;


/*    if(m_vToolBarFont->isButtonOn(m_idButtonFont_2)) level=level | FN_ELEMENT;
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_3)) level=level | FN_INDEXES;
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_4)) level=level | FN_CHILDREN;
    if(m_vToolBarFont->isButtonOn(m_idButtonFont_5)) level=level | FN_NEXT;
*/
    m_pDoc->currentElement()->scaleNumericFont(level);
    update();
}


void KFormulaView::fractionAlignM()
{
    if(!((KToggleAction*)actionElement_Fraction_VA_M)->isChecked())
    if( (((KToggleAction*)actionElement_Fraction_VA_D)->isChecked())
      || (((KToggleAction*)actionElement_Fraction_VA_U)->isChecked())
      ) return;
      
    warning("M");
    QString content=m_pDoc->currentElement()->getContent();
    content[1]='M';
    m_pDoc->currentElement()->setContent(content);
    
   ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(true);
   ((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(false);
   ((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(false);

/*    m_vToolBarType->setButton(m_idButtonType_MAl,true);
    m_vToolBarType->setButton(m_idButtonType_DAl,false);
    m_vToolBarType->setButton(m_idButtonType_UAl,false);
*/    update();
}
void KFormulaView::fractionAlignU()
{
    if(!((KToggleAction*)actionElement_Fraction_VA_U)->isChecked())
    if( (((KToggleAction*)actionElement_Fraction_VA_D)->isChecked())
      || (((KToggleAction*)actionElement_Fraction_VA_M)->isChecked())
      ) return;

    warning("U");
    QString content=m_pDoc->currentElement()->getContent();
    content[1]='U';
    m_pDoc->currentElement()->setContent(content);
 ((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(true);
   ((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(false);
   ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(false);

/*    m_vToolBarType->setButton(m_idButtonType_MAl,false);
    m_vToolBarType->setButton(m_idButtonType_DAl,false);
    m_vToolBarType->setButton(m_idButtonType_UAl,true);
*/    update();
}

void KFormulaView::fractionAlignD()
{
    if(!((KToggleAction*)actionElement_Fraction_VA_D)->isChecked())
    if( (((KToggleAction*)actionElement_Fraction_VA_M)->isChecked())
      || (((KToggleAction*)actionElement_Fraction_VA_U)->isChecked())
      ) return;

    warning("D");
    QString content=m_pDoc->currentElement()->getContent();
    content[1]='D';
    m_pDoc->currentElement()->setContent(content);
 ((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(true);
   ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(false);
   ((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(false);

//
//    m_vToolBarType->setButton(m_idButtonType_MAl,false);
//    m_vToolBarType->setButton(m_idButtonType_DAl,true);
//    m_vToolBarType->setButton(m_idButtonType_UAl,false);
    update();
}
void KFormulaView::fractionAlignL()
{

    QString content=m_pDoc->currentElement()->getContent();
    content[2]='L';
    m_pDoc->currentElement()->setContent(content);
//    m_vToolBarType->setButton(m_idButtonType_CAl,false);
//    m_vToolBarType->setButton(m_idButtonType_RAl,false);
//    m_vToolBarType->setButton(m_idButtonType_LAl,true);
    update();
}
void KFormulaView::fractionAlignR()
{

    QString content=m_pDoc->currentElement()->getContent();
    content[2]='R';
    m_pDoc->currentElement()->setContent(content);
//    m_vToolBarType->setButton(m_idButtonType_CAl,false);
//    m_vToolBarType->setButton(m_idButtonType_RAl,true);
//    m_vToolBarType->setButton(m_idButtonType_LAl,false);
    update();
}
void KFormulaView::fractionAlignC()
{

    QString content=m_pDoc->currentElement()->getContent();
    content[2]='C';
    m_pDoc->currentElement()->setContent(content);
//    m_vToolBarType->setButton(m_idButtonType_CAl,true);
//    m_vToolBarType->setButton(m_idButtonType_RAl,false);
//    m_vToolBarType->setButton(m_idButtonType_LAl,false);
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
//   if(m_vToolBarFont->isButtonOn(m_idButtonFont_2)) warning("element");
//    if(m_vToolBarFont->isButtonOn(m_idButtonFont_3)) warning("index");
//    if(m_vToolBarFont->isButtonOn(m_idButtonFont_4)) warning("children");
//    if(m_vToolBarFont->isButtonOn(m_idButtonFont_5)) warning("next");
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

void KFormulaView::sizeSelected()
{
    int size =  ((KFontSizeAction *)actionElement_Text_Size)->fontSize();
    BasicElement *el=m_pDoc->currentElement();
    if (el==0) return;
    el->setNumericFont(size);
    warning("%d",size);
    update();
}
void KFormulaView::fontSelected()
{

    BasicElement *el=m_pDoc->currentElement();
    QString font =  ((KFontAction *)actionElement_Text_Font)->currentText();
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

void KFormulaView::modeSelected(char *)
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
    if (((KToggleAction *)actionElement_Fraction_MidLine)->isChecked())
     content[0]='F'; else content[0]='V';
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
void KFormulaView::delimiterLeft()
{
    QString left =  ((KSelectAction *)actionElement_Bracket_Type_Left)->currentText();
    QString content=m_pDoc->currentElement()->getContent();
    warning(content);
    content[0]=QString(left)[0];
    warning(content);
    m_pDoc->currentElement()->setContent(content);
    update();
}

void KFormulaView::delimiterRight()
{
    QString right=  ((KSelectAction *)actionElement_Bracket_Type_Right)->currentText();
    QString content=m_pDoc->currentElement()->getContent();
    warning(content);
    content[1]=QString(right)[0];
    warning(content);
    m_pDoc->currentElement()->setContent(content);
    update();
}

bool KFormulaView::printDlg()
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






