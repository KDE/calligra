#include "kformula_view.h"
#include "kformula_doc.h"
#include "kformula_main.h"
#include "TextElement.h"
#include "matrixwidget.h"
#include <op_app.h>
#include <utils.h>
#include <part_frame_impl.h>

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

/**********************************************************
 *
 * KFormulaView
 *
 **********************************************************/

KFormulaView::KFormulaView( QWidget *_parent, const char *_name) :
    QWidget( _parent, _name ), View_impl(), KFormula::View_skel()
{
    setWidget( this );
  
    setBackgroundColor( white );
  
    Control_impl::setFocusPolicy( OPControls::Control::ClickFocus );
 
    m_pDoc = 0L;
    
    setGeometry( 5000, 5000, 100, 100 );
}

KFormulaView::~KFormulaView()
{
    cleanUp();
}

void KFormulaView::cleanUp()
{
    if ( m_bIsClean )
	return;
  
    m_pDoc->removeView( this );
  
    m_rMenuBar = 0L;
    m_vMenuBarFactory = 0L;

    m_rToolBarFormula = 0L;
    m_rToolBarFont = 0L;
    m_rToolBarType = 0L;
    m_vToolBarFactory = 0L;

    View_impl::cleanUp();
}
  
void KFormulaView::setDocument( KFormulaDocument *_doc )
{
    if ( m_pDoc )
	m_pDoc->removeView( this );

    View_impl::setDocument( _doc );
  
    m_pDoc = _doc;

    m_pDoc->addView( this );

    QObject::connect( m_pDoc, SIGNAL( sig_modified() ), 
		      this, SLOT( slotModified() ) );

    QObject::connect( m_pDoc, 
		      SIGNAL(sig_changeType(BasicElement *)    ),
		      this, SLOT( slotTypeChanged(BasicElement *)    ) );

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
    m_pDoc->keyPressEvent(k);
 
}



CORBA::Long KFormulaView::addToolButton (ToolBar_ref toolbar,
					 const char* pictname,
					 const char* tooltip,
					 const char* func) 
{
    QString tmp = KApplication::kde_datadir().copy();
    tmp += "/kformula/pics/";
    tmp += pictname;
    
    QString pix = loadPixmap( tmp );
   
    CORBA::Long id = toolbar->insertButton( CORBA::string_dup( pix ), 
					    CORBA::string_dup(tooltip),
					    this,
					    func);
    return id;
}

void KFormulaView::createGUI()
{ 
    QPixmap pix;
    QString tmp = KApplication::kde_datadir().copy();
    tmp += "/kformula/pics/";
    
    mn_indexList = new QPopupMenu();
    pix.load(tmp+"index0.xpm");
    mn_indexList->insertItem(pix,0);
    mn_indexList->insertSeparator();
    pix.load(tmp+"index1.xpm");
    mn_indexList->insertItem(pix,1);
    mn_indexList->insertSeparator();
    pix.load(tmp+"index2.xpm");
    mn_indexList->insertItem(pix,2);
    mn_indexList->insertSeparator();
    pix.load(tmp+"index3.xpm");
    mn_indexList->insertItem(pix,3);
    mn_indexList->setMouseTracking(true);
    mn_indexList->setCheckable(false);

    m_vMenuBarFactory = m_vPartShell->menuBarFactory();
    if ( !CORBA::is_nil( m_vMenuBarFactory ) ) {
	
	QString tmp = KApplication::kde_datadir().copy();
	tmp += "/kformula/pics/";    
	// Menubar
	m_rMenuBar = m_vMenuBarFactory->createMenuBar( this );
	
	// View
	m_idMenuView = m_rMenuBar->insertMenu( i18n( "&View" ) );
	
	m_idMenuView_NewView = 
	    m_rMenuBar->insertItem( i18n( "&New View" ),
				    m_idMenuView,
				    this, "newView" );
	m_idMenuView_FontToolbar = 
	    m_rMenuBar->insertItem( i18n( "Font Toolbar" ),
				    m_idMenuView,
				    this, "newView" );
	m_idMenuView_TextToolbar =
	    m_rMenuBar->insertItem( i18n( "Formula Toolbar" ),
				    m_idMenuView,
				    this, "newView" );
	m_idMenuView_TypeToolbar = 
	    m_rMenuBar->insertItem( i18n( "Type Toolbar" ),
				    m_idMenuView,
				    this, "newView" );
	
	// Element
	m_idMenuElement = m_rMenuBar->insertMenu( i18n( "&Element" ) );
	
	m_idMenuElement_AddIndex = 
	    m_rMenuBar->insertSubMenu( i18n( "Add Index to..." ),
				       m_idMenuElement);
						       
	m_idMenuElement_AddIndex_TL = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"index0.xpm" )), 
				    i18n( "Top left" ),
				    m_idMenuElement_AddIndex,
				    this, "addTopLeftIndex" );
	
	m_idMenuElement_AddIndex_BL = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"index1.xpm" )),
				    i18n( "Bottom left" ),
				    m_idMenuElement_AddIndex,
				    this, "addBottomLeftIndex" );
	m_idMenuElement_AddIndex_TR =
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"index2.xpm" )), 
				    i18n( "Top right" ),
				    m_idMenuElement_AddIndex,
				    this, "addTopRightIndex" );
	m_idMenuElement_AddIndex_BR = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"index3.xpm" )), 
				    i18n( "Bottom Right" ),
				    m_idMenuElement_AddIndex,
				    this, "addBottomRightIndex" );
	
	m_idMenuElement_AddElement = 
	    m_rMenuBar->insertSubMenu( i18n( "Add Element..." ),
				       m_idMenuElement);
	m_idMenuElement_AddElement_T = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"mini-xy.xpm" )), 
				    i18n( "Plain text" ),
				    m_idMenuElement_AddElement,
				    this, "addText" );
	m_idMenuElement_AddElement_R =
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"mini-root.xpm" )), 
				    i18n( "Root" ),
				    m_idMenuElement_AddElement,
				    this, "addRoot" );
	m_idMenuElement_AddElement_B =
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"mini-bra.xpm" )),
				    i18n( "Bracket" ),
				    m_idMenuElement_AddElement,
				    this, "addBracket" );
	m_idMenuElement_AddElement_F =
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"mini-frac.xpm" )), 
				    i18n( "Fraction" ),
				    m_idMenuElement_AddElement,
				    this, "addFraction" );
	m_idMenuElement_Color = 
	    m_rMenuBar->insertItem( i18n( "Set Color" ),
				    m_idMenuElement,
				    this, "elementColor" );
	m_idMenuElement_Remove = 
	    m_rMenuBar->insertItem( i18n( "Remove element" ),
				    m_idMenuElement,
				    this, "remove" );
	
	m_rMenuBar->insertSeparator(m_idMenuElement);
       
	m_idMenuElement_Text = 
	    m_rMenuBar->insertSubMenu( i18n( "Text..." ),
				       m_idMenuElement);
	m_idMenuElement_Text_Font = 
	    m_rMenuBar->insertItem( i18n( "Set Font" ),
				    m_idMenuElement_Text,
				    this, "textFont" );
	m_idMenuElement_Text_Split =
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"split.xpm" )),
				    i18n( "Split at current Position" ),
				    m_idMenuElement_Text,
				    this, "textSplit" );
	
	m_idMenuElement_Root = 
	    m_rMenuBar->insertSubMenu( i18n( "Root..." ),
				       m_idMenuElement);    
	m_idMenuElement_Root_Pixmap = 
	    m_rMenuBar->insertItem( i18n( "Use pixmap" ),
				    m_idMenuElement_Root,
				    this, "togglePixmap" );
	m_idMenuElement_Root_Index = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"rootindex.xpm" )), i18n( "Add root index(Top Left)" ),
				    m_idMenuElement_Root,
				    this, "addTopLeftIndex" );
	
	m_idMenuElement_Bracket = 
	    m_rMenuBar->insertSubMenu( i18n( "Bracket..." ),
				       m_idMenuElement);    
	m_idMenuElement_Bracket_Type = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"delimiter.xpm" )),
				    i18n( "Set delimiter..." ),
				    m_idMenuElement_Bracket,
				    this, "bracketType" );
	
	m_idMenuElement_Fraction = 
	    m_rMenuBar->insertSubMenu( i18n( "Fraction..." ),
				       m_idMenuElement);
	m_idMenuElement_Fraction_VA = 
	    m_rMenuBar->insertItem( i18n( "Vertical Align..." ),
				    m_idMenuElement_Fraction,
				    this, "addFraction" );       
	m_idMenuElement_Fraction_HA =
	    m_rMenuBar->insertItem( i18n( "Horizontal Align..." ),
				    m_idMenuElement_Fraction,
				    this, "fractionHAlign" );	    
	m_idMenuElement_Fraction_MidLine = 
	    m_rMenuBar->insertItem( i18n( "Set Midline" ),
				    m_idMenuElement_Fraction,
				    this, "toggleMidline" );	    
	
	m_idMenuElement_Integral = 
	    m_rMenuBar->insertSubMenu( i18n( "Integral..." ),
				       m_idMenuElement);
	m_idMenuElement_Integral_Pixmap = 
	    m_rMenuBar->insertItem( i18n( "Use pixmap" ),
				    m_idMenuElement_Integral,
				    this, "togglePixmap" );	    
	m_idMenuElement_Integral_Higher = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"Ihigher.xpm" )),
				    i18n( "Add higher limit" ),
				    m_idMenuElement_Integral,
				    this, "addHigher" );
	m_idMenuElement_Integral_Lower = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"Ilower.xpm" )),
				    i18n( "Add lower limit" ),
				    m_idMenuElement_Integral,
				    this, "addLower" );	
						       					           
	m_idMenuElement_Matrix = 
	    m_rMenuBar->insertSubMenu( i18n( "Matrix..." ),
				       m_idMenuElement);
	m_idMenuElement_Matrix_Set = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"setmatrix.xpm" )), 
				    i18n( "Set dimension" ),
				    m_idMenuElement_Matrix,
				    this, "matrixSet" );	
	m_idMenuElement_Matrix_InsRow = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"insrow.xpm" )),
				    i18n( "Insert a row" ),
				    m_idMenuElement_Matrix,
				    this, "matrixInsRow" );	
	m_idMenuElement_Matrix_InsCol =
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"inscol.xpm" )),
				    i18n( "Insert a column" ),
				    m_idMenuElement_Matrix,
				    this, "matrixInsCol" );	
	m_idMenuElement_Matrix_RemRow = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"remrow.xpm" )),
				    i18n( "Remove a row" ),
				    m_idMenuElement_Matrix,
				    this, "matrixRemRow" );	
	m_idMenuElement_Matrix_RemCol = 
	    m_rMenuBar->insertItemP(CORBA::string_dup(loadPixmap( tmp+"remcol.xpm" )),
				    i18n( "Remove a column" ),
				    m_idMenuElement_Matrix,
				    this, "matrixRemCol" );	

	m_idMenuElement_Decoration = m_rMenuBar->insertSubMenu( i18n( "Decoration..." ),
								m_idMenuElement);    
	m_idMenuElement_Symbol = m_rMenuBar->insertSubMenu( i18n( "Symbol..." ),
							    m_idMenuElement);    
	    
	// Element
	m_idMenuFormula = m_rMenuBar->insertMenu( i18n( "&Formula" ) );
	m_idMenuFormula_Color = 
	    m_rMenuBar->insertItem( i18n( "Set general color" ),
				    m_idMenuFormula,
				    this, "addFraction" );	
	m_idMenuFormula_Font = 
	    m_rMenuBar->insertItem( i18n( "Set general font" ),
				    m_idMenuFormula,
				    this, "generalFont" );
    }

    QObject::connect(mn_indexList,SIGNAL(activated(int)),this,SLOT(insertIndex(int)));

    m_vToolBarFactory = m_vPartShell->toolBarFactory();
    if ( !CORBA::is_nil( m_vToolBarFactory ) ) {
	
	m_rToolBarFormula = m_vToolBarFactory->createToolBar( this, i18n( "Formula" ) );
	
	m_idButtonFormula_0 = addToolButton(m_rToolBarFormula, "mini-xy.xpm", 
					    i18n( "Add/change to simple text" ),
					    "addText" );

	m_idButtonFormula_1 = addToolButton(m_rToolBarFormula, "mini-root.xpm", 
					    i18n( "Add/change to root" ), "addRoot" );

	m_idButtonFormula_2 = addToolButton(m_rToolBarFormula, "mini-frac.xpm",
					    i18n( "Add/change fract line" ), "addFraction");

	m_idButtonFormula_3 = addToolButton(m_rToolBarFormula, "mini-vspace.xpm",
					    i18n( "Add/change vertical space" ), "addVertSpace");
	
	
	m_idButtonFormula_4 = addToolButton(m_rToolBarFormula, "mini-bra.xpm",
					    i18n( "Add/change a bracket block" ), "addBracket" );

	m_idButtonFormula_5 = addToolButton(m_rToolBarFormula, "mini-integral.xpm",
					    i18n( "Add/change an integral" ), "addIntegral" );

	m_idButtonFormula_6 = addToolButton(m_rToolBarFormula, "mini-symbols.xpm",
					    i18n( "Add/change a block with symbols" ), "addSymbol" );

	m_idButtonFormula_6 = addToolButton(m_rToolBarFormula, "matrix.xpm",
					    i18n( "Add/change matrix" ), "addMatrix" );
    	
	m_idButtonFormula_7 = addToolButton(m_rToolBarFormula, "index.xpm", 
					    i18n( "Add an index at position..." ), "indexList" );

    }

    if ( !CORBA::is_nil( m_vToolBarFactory ) )
	{
	    m_rToolBarFont = m_vToolBarFactory->createToolBar( this, i18n( "Font" ) );

	    //	    m_rToolBarFont->setPos(KToolBar::Left);
	    m_idComboFont_FontFamily = m_rToolBarFont->insertCombo(true,CORBA::string_dup(i18n("Font Family")),70,
								   this,CORBA::string_dup("fontSelected"));
	    m_rToolBarFont->insertComboItem(m_idComboFont_FontFamily ,CORBA::string_dup("utopia"),-1);
	    m_rToolBarFont->insertComboItem(m_idComboFont_FontFamily ,CORBA::string_dup("symbol"),-1);
	    m_rToolBarFont->insertComboItem(m_idComboFont_FontFamily ,CORBA::string_dup("(default)"),-1);
	    m_rToolBarFont->setCurrentComboItem(m_idComboFont_FontFamily ,0);
	    
	    m_idComboFont_FontSize = m_rToolBarFont->insertCombo(true,CORBA::string_dup(i18n("Font Size of active Element")),60,
								 this,CORBA::string_dup("sizeSelected"));
	    for(unsigned int i = 1;i <= FN_MAX;i++)
		{
		    char buffer[10];
		    sprintf(buffer,"%i",i);
		    m_rToolBarFont->insertComboItem(m_idComboFont_FontSize,CORBA::string_dup(buffer),-1);
		}
	    m_rToolBarFont->setCurrentComboItem(m_idComboFont_FontSize,31);

	    m_idButtonFont_Bold = addToolButton(m_rToolBarFont, "bold.xpm",
						i18n( "Bold" ),"fontSwitch");
      
	    m_idButtonFont_Italic = addToolButton(m_rToolBarFont, "italic.xpm",
						  i18n( "Italic" ),"fontSwitch");

	    m_idButtonFont_Underl = addToolButton(m_rToolBarFont, "underl.xpm",
						  i18n( "Underlined" ), "fontSwitch");



            m_rToolBarFont->insertSeparator();
            m_idComboFont_ScaleMode = m_rToolBarFont->insertCombo(true,CORBA::string_dup(i18n("Font Scale Mode")),80,
								  this,CORBA::string_dup("sizeSelected"));
	    for(unsigned int i = 1;i <= 10;i++)
		{
		    char buffer[10];
		    sprintf(buffer,"+(-) %i",i);
		    m_rToolBarFont->insertComboItem(m_idComboFont_ScaleMode,CORBA::string_dup(buffer),-1);
		}
	    for(unsigned int i = 1;i <= 6;i++)
		{
		    char buffer[10];
		    sprintf(buffer,"*(/) %i/%i",i,i+1);
		    m_rToolBarFont->insertComboItem(m_idComboFont_ScaleMode,CORBA::string_dup(buffer),-1);
		}
	    m_rToolBarFont->setCurrentComboItem(m_idComboFont_ScaleMode,12);		
	    
	
	    m_idButtonFont_0 = addToolButton(m_rToolBarFont, "reduce.xpm",
					     i18n( "Reduce the fontSize" ), "reduce");

	    m_idButtonFont_1 = addToolButton(m_rToolBarFont, "enlarge.xpm",
					     i18n( "Increase the fontSize" ), "enlarge");
            m_rToolBarFont->insertSeparator();
 
	    m_idButtonFont_2 = addToolButton(m_rToolBarFont, "elementsw.xpm",
					     i18n( "Reduce/Increase active element fontSize" ),"fontSwitch");
      
	    m_idButtonFont_3 = addToolButton(m_rToolBarFont, "indexsw.xpm",
					     i18n( "Reduce/Increase indexes fontSize" ),"fontSwitch");

	    m_idButtonFont_4 = addToolButton(m_rToolBarFont, "childrensw.xpm",
					     i18n( "Reduce/Increase children (content) fontSize" ), "fontSwitch");

	    m_idButtonFont_5 = addToolButton(m_rToolBarFont, "nextsw.xpm",
					     i18n( "Reduce/Increase next element fontSize" ), "fontSwitch" );
	    m_rToolBarFont->setToggle( m_idButtonFont_Bold, true );
	    m_rToolBarFont->setToggle( m_idButtonFont_Underl, true );
	    m_rToolBarFont->setToggle( m_idButtonFont_Italic, true );
	    m_rToolBarFont->setToggle( m_idButtonFont_2, true );
	    m_rToolBarFont->setToggle( m_idButtonFont_3, true );
	    m_rToolBarFont->setToggle( m_idButtonFont_4, true );
	    m_rToolBarFont->setToggle( m_idButtonFont_5, true );
	    
	    m_rToolBarFont->setButton( m_idButtonFont_2, true );
	    m_rToolBarFont->setButton( m_idButtonFont_3, true );
	    m_rToolBarFont->setButton( m_idButtonFont_4, true );
	    m_rToolBarFont->setButton( m_idButtonFont_5, false );
	    

	}
  
    if ( !CORBA::is_nil( m_vToolBarFactory ) )
	{
	    m_rToolBarType = m_vToolBarFactory->createToolBar( this, i18n( "Type" ) );


	    // ************************* Text *********************
	    m_idButtonType_Spl = addToolButton(m_rToolBarType, "split.xpm", 
					       i18n( "Split text into 2 elment" ), "textSplit" );
            m_rToolBarType->insertSeparator();		  
	    // ************************** Root ********************
	    m_idButtonType_RIn = addToolButton(m_rToolBarType, "rootindex.xpm", 
					       i18n( "Add root index (i.e. Top Left index)" ), "addTopLeftIndex" );

	    // ************************** Brackets ********************	  
            m_rToolBarType->insertSeparator();	

	    //    m_idButtonType_Del = addToolButton(m_rToolBarType, "delimiter.xpm", 
	    //				     i18n( "Set delimiter type" ), "bracketType" );

	    m_idComboType_DelLeft = m_rToolBarType->insertCombo(true,CORBA::string_dup(i18n("Left Delimiter")),40,
								this,CORBA::string_dup("delimiterLeft"));
	    m_rToolBarType->insertComboItem(m_idComboType_DelLeft ,CORBA::string_dup("("),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelLeft ,CORBA::string_dup("["),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelLeft ,CORBA::string_dup("<"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelLeft ,CORBA::string_dup("/"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelLeft ,CORBA::string_dup("\\"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelLeft ,CORBA::string_dup(")"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelLeft ,CORBA::string_dup("]"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelLeft ,CORBA::string_dup(">"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelLeft ,CORBA::string_dup("|"),-1);
	    m_idComboType_DelRight = m_rToolBarType->insertCombo(true,CORBA::string_dup(i18n("Right Delimiter")),40,
								 this,CORBA::string_dup("delimiterRight"));
							   
	    m_rToolBarType->insertComboItem(m_idComboType_DelRight ,CORBA::string_dup(")"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelRight,CORBA::string_dup("]"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelRight,CORBA::string_dup(">"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelRight ,CORBA::string_dup("/"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelRight ,CORBA::string_dup("\\"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelRight ,CORBA::string_dup("("),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelRight ,CORBA::string_dup("["),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelRight ,CORBA::string_dup("<"),-1);
	    m_rToolBarType->insertComboItem(m_idComboType_DelRight,CORBA::string_dup("|"),-1);
            m_rToolBarType->insertSeparator();
	    // ************************** Fraction ********************
	    m_idButtonType_MAl = addToolButton(m_rToolBarType, "midalign.xpm", 
					       i18n( "Align fraction to midline" ), "fractionAlignM" );

	    m_idButtonType_UAl = addToolButton(m_rToolBarType, "upalign.xpm", 
					       i18n( "Align fraction to numerator" ), "fractionAlignU" );

	    m_idButtonType_DAl = addToolButton(m_rToolBarType, "downalign.xpm", 
					       i18n( "Align fraction to denominator" ), "fractionAlignD" );

	    m_idButtonType_CAl = addToolButton(m_rToolBarType, "centralign.xpm", 
					       i18n( "Align center" ), "fractionAlignC" );

	    m_idButtonType_LAl = addToolButton(m_rToolBarType, "leftalign.xpm", 
					       i18n( "Align left" ), "fractionAlignL" );

	    m_idButtonType_RAl = addToolButton(m_rToolBarType, "rightalign.xpm", 
					       i18n( "Align Right" ), "fractionAlignR" );

	    m_idButtonType_Les = addToolButton(m_rToolBarType, "near.xpm", 
					       i18n( "Reduce element vertical distance" ), "fractionDistLess" );

	    m_idButtonType_Mor = addToolButton(m_rToolBarType, "far.xpm", 
					       i18n( "Increase element vertical distance" ), "fractionDistMore" );

	    m_idButtonType_Mid = addToolButton(m_rToolBarType, "midline.xpm", 
					       i18n( "Toggle fraction line" ), "toggleMidline" );
            m_rToolBarType->insertSeparator();
	    // *******************  Integral **************************+
	    m_idButtonType_AddH = addToolButton(m_rToolBarType, "Ihigher.xpm", 
						i18n( "Add higher limit" ), "integralHigher" );

	    m_idButtonType_AddL = addToolButton(m_rToolBarType, "Ilower.xpm", 
						i18n( "Add lower limit" ), "integralLower" );
            m_rToolBarType->insertSeparator();
	    // *********************** Matrix *************************
	    m_idButtonType_SetM = addToolButton(m_rToolBarType, "setmatrix.xpm", 
						i18n( "Set matrix dimension" ), "matrixSet" );
	    m_idButtonType_InR = addToolButton(m_rToolBarType, "insrow.xpm", 
					       i18n( "Insert a row" ), "matrixInsRow" );
	    m_idButtonType_InC = addToolButton(m_rToolBarType, "inscol.xpm", 
					       i18n( "Insert a column" ), "matrixInsCol" );
	    m_idButtonType_ReR = addToolButton(m_rToolBarType, "remrow.xpm", 
					       i18n( "Remove a row" ), "matrixRemRow" );
	    m_idButtonType_ReC = addToolButton(m_rToolBarType, "remcol.xpm", 
					       i18n( "Remove a column" ), "matrixRemCol" );

            m_rToolBarType->insertSeparator();
	    // *********************** General *************************
	    m_idButtonType_Pix = addToolButton(m_rToolBarType, "pixmap.xpm", 
					       i18n( "Toggle pixmap use" ), "togglePixmap" );

	    m_rToolBarType->setToggle( m_idButtonType_UAl, true );
	    m_rToolBarType->setToggle( m_idButtonType_MAl, true );
	    m_rToolBarType->setToggle( m_idButtonType_DAl, true );
	    m_rToolBarType->setToggle( m_idButtonType_CAl, true );
	    m_rToolBarType->setToggle( m_idButtonType_LAl, true );
	    m_rToolBarType->setToggle( m_idButtonType_RAl, true );
	    m_rToolBarType->setToggle( m_idButtonType_Mid, true );
	    
	    // TODO insert Line Edit control
	}  
    slotTypeChanged(0);
}


void KFormulaView::slotTypeChanged( BasicElement *elm)
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

    m_rToolBarType->setItemEnabled(m_idButtonType_Spl,isText);

// It remains deactivated !!!    
//    m_rMenuBar->setItemEnabled(m_idMenuElement_Fraction,isFraction);
    
    m_rToolBarType->setItemEnabled(m_idComboType_DelLeft,isBracket);
    m_rToolBarType->setItemEnabled(m_idComboType_DelRight,isBracket);
    m_rToolBarType->setItemEnabled(m_idButtonType_RIn,isRoot);
    m_rToolBarType->setItemEnabled(m_idButtonType_UAl,isFraction);
    m_rToolBarType->setItemEnabled(m_idButtonType_DAl,isFraction); 
    m_rToolBarType->setItemEnabled(m_idButtonType_MAl,isFraction);  
    m_rToolBarType->setItemEnabled(m_idButtonType_Mid,isFraction); 
    m_rToolBarType->setItemEnabled(m_idButtonType_CAl,isFraction); 
    m_rToolBarType->setItemEnabled(m_idButtonType_LAl,isFraction); 
    m_rToolBarType->setItemEnabled(m_idButtonType_RAl,isFraction);  
    m_rToolBarType->setItemEnabled(m_idButtonType_Les,isFraction); 
    m_rToolBarType->setItemEnabled(m_idButtonType_Mor,isFraction); 
    m_rToolBarType->setItemEnabled(m_idButtonType_AddH,isPrefixed); 
    m_rToolBarType->setItemEnabled(m_idButtonType_AddL,isPrefixed); 
    m_rToolBarType->setItemEnabled(m_idButtonType_SetM,isMatrix);  
    m_rToolBarType->setItemEnabled(m_idButtonType_InC,isMatrix);
    m_rToolBarType->setItemEnabled(m_idButtonType_InR,isMatrix);
    m_rToolBarType->setItemEnabled(m_idButtonType_ReC,isMatrix);    
    m_rToolBarType->setItemEnabled(m_idButtonType_ReR,isMatrix);

    if (elm)              
    m_rToolBarFont->setCurrentComboItem(m_idComboFont_FontSize, elm->getNumericFont()-1 );

    if (isFraction) { 
	QString content=elm->getContent();
	m_rToolBarType->setButton(m_idButtonType_UAl,content[1]=='U'); 
	m_rToolBarType->setButton(m_idButtonType_DAl,content[1]=='D'); 
	m_rToolBarType->setButton(m_idButtonType_MAl,content[1]=='M'); 
	m_rToolBarType->setButton(m_idButtonType_CAl,content[2]=='C'); 
	m_rToolBarType->setButton(m_idButtonType_LAl,content[2]=='L'); 
	m_rToolBarType->setButton(m_idButtonType_RAl,content[2]=='R'); 
	m_rToolBarType->setButton(m_idButtonType_Mid,content[0]=='F'); 
    }
  if (isPrefixed) { 
	QString content=elm->getContent();
    QString tmp = KApplication::kde_datadir().copy();
    tmp += "/kformula/pics/";
	m_rToolBarType->setButtonPixmap(m_idButtonType_AddH,CORBA::string_dup(loadPixmap(tmp+content.left(1)+"higher.xpm"  )));                 
	m_rToolBarType->setButtonPixmap(m_idButtonType_AddL ,CORBA::string_dup(loadPixmap(tmp+content.left(1)+"lower.xpm"  )));                 
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
    shell->enableMenuBar();
    shell->PartShell_impl::enableStatusBar();
    shell->enableToolBars();
    shell->show();
    shell->setDocument( m_pDoc );
  
    CORBA::release( shell );
}

void KFormulaView::addText()
{
    debug("adding Text");
    m_pDoc->addTextElement();
    //    m_rToolBarType->show();
}

void KFormulaView::addRoot()
{ 
    debug("adding Root");
    m_pDoc->addRootElement();
}

void KFormulaView::addFraction()
{
    debug("adding Fraction");
    m_pDoc->addFractionElement("FMC10");
}

void KFormulaView::addVertSpace()
{
    debug("adding VerticalSpace");
    m_pDoc->addFractionElement("VUC10");
}

void KFormulaView::addBracket()
{
    debug("adding parenthesis");
    m_pDoc->addBracketElement("()");
    //    m_rToolBarType->hide();
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
 BasicElement *el=m_pDoc->activeElement();
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
    if(m_pDoc->activeElement()==0) return;
    int level;
    level= FN_REDUCE | FN_P43; 
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_2)) level=level | FN_ELEMENT;
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_3)) level=level | FN_INDEXES;
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_4)) level=level | FN_CHILDREN;
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_5)) level=level | FN_NEXT;

    m_pDoc->activeElement()->scaleNumericFont(level);
    update();
}

void KFormulaView::enlarge()
{
    if(m_pDoc->activeElement()==0) return;
    int level;
    level= FN_ENLARGE | FN_P43;

    //    warning(m_rToolBarFont->comboItem(m_idComboFont_ScaleMode));
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_2)) level=level | FN_ELEMENT;
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_3)) level=level | FN_INDEXES;
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_4)) level=level | FN_CHILDREN;
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_5)) level=level | FN_NEXT;

    m_pDoc->activeElement()->scaleNumericFont(level);
    update();
}


void KFormulaView::fractionAlignM()
{ 
    QString content=m_pDoc->activeElement()->getContent();
    content[1]='M';
    m_pDoc->activeElement()->setContent(content);
    m_rToolBarType->setButton(m_idButtonType_MAl,false);
    m_rToolBarType->setButton(m_idButtonType_DAl,false);
    m_rToolBarType->setButton(m_idButtonType_UAl,false);
    update();
}
void KFormulaView::fractionAlignU()
{

    QString content=m_pDoc->activeElement()->getContent();
    content[1]='U';
    m_pDoc->activeElement()->setContent(content);
    m_rToolBarType->setButton(m_idButtonType_MAl,false);
    m_rToolBarType->setButton(m_idButtonType_DAl,false);
    m_rToolBarType->setButton(m_idButtonType_UAl,false);
    update();
}

void KFormulaView::fractionAlignD()
{

    QString content=m_pDoc->activeElement()->getContent();
    content[1]='D';
    m_pDoc->activeElement()->setContent(content);
    m_rToolBarType->setButton(m_idButtonType_MAl,false);
    m_rToolBarType->setButton(m_idButtonType_DAl,false);
    m_rToolBarType->setButton(m_idButtonType_UAl,false);
    update();
}
void KFormulaView::fractionAlignL()
{

    QString content=m_pDoc->activeElement()->getContent();
    content[2]='L';
    m_pDoc->activeElement()->setContent(content);
    m_rToolBarType->setButton(m_idButtonType_CAl,false);
    m_rToolBarType->setButton(m_idButtonType_RAl,false);
    m_rToolBarType->setButton(m_idButtonType_LAl,false);
    update();
}
void KFormulaView::fractionAlignR()
{

    QString content=m_pDoc->activeElement()->getContent();
    content[2]='R';
    m_pDoc->activeElement()->setContent(content);
    m_rToolBarType->setButton(m_idButtonType_CAl,false);
    m_rToolBarType->setButton(m_idButtonType_RAl,false);
    m_rToolBarType->setButton(m_idButtonType_LAl,false);
    update();
}
void KFormulaView::fractionAlignC()
{

    QString content=m_pDoc->activeElement()->getContent();
    content[2]='C';
    m_pDoc->activeElement()->setContent(content);
    m_rToolBarType->setButton(m_idButtonType_CAl,false);
    m_rToolBarType->setButton(m_idButtonType_RAl,false);
    m_rToolBarType->setButton(m_idButtonType_LAl,false);
    update();
}

void KFormulaView::fractionDist()
{

}

void KFormulaView::fractionDistMore()
{
    QString content=m_pDoc->activeElement()->getContent();
    content.sprintf("%s%i",(const char*)content.left(3),(atoi(content.right(content.length()-3))+1));
    warning(content);
    m_pDoc->activeElement()->setContent(content);
    update();
}
void KFormulaView::fractionDistLess()
{
    QString content=m_pDoc->activeElement()->getContent();
    int space=(atoi(content.right(content.length()-3))-1);
    if (space<1) space =1;
    content.sprintf("%s%i",(const char*)content.left(3),space);
    warning(content);
    m_pDoc->activeElement()->setContent(content);
    update();
}

void KFormulaView::fontSwitch()
{
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_2)) warning("element");
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_3)) warning("index");
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_4)) warning("children");
    if(m_rToolBarFont->isButtonOn(m_idButtonFont_5)) warning("next");
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

    BasicElement *el=m_pDoc->activeElement();
    if (el==0) return;
    el->setNumericFont(atoi(size)); 
    warning(size);
    update();
}
void KFormulaView::fontSelected(const char *font)
{
    BasicElement *el=m_pDoc->activeElement();

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
}
void KFormulaView::togglePixmap()
{ 
    warning("Slot togglePixmap");
}
void KFormulaView::integralLower()
{
    warning("Slot integralLower");
}
void KFormulaView::integralHigher()
{ 
    warning("Slot integralHigher");
}
void KFormulaView::bracketType()
{ 
    warning("Slot bracketType");
}
void KFormulaView::matrixSet()
{ 
    MatrixSetupWidget *ms=new MatrixSetupWidget();
    ms->setString(m_pDoc->activeElement()->getContent()); 
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
    QString content=m_pDoc->activeElement()->getContent();
    if (content[0]=='F') content[0]='V'; else content[0]='F';
    m_pDoc->activeElement()->setContent(content);
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
    QString content=m_pDoc->activeElement()->getContent();
    warning(content);
    content[0]=left[0];
    warning(content);
    m_pDoc->activeElement()->setContent(content);
    update();
}

void KFormulaView::delimiterRight(const char *right)
{ 
    QString content=m_pDoc->activeElement()->getContent();
    warning(content);
    content[1]=right[0];
    warning(content);
    m_pDoc->activeElement()->setContent(content);
    update();
}        

#include "kformula_view.moc"






