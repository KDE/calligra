#include "kformula_view.h"
#include "kformula_doc.h"
#include "kformula_main.h"

#include <op_app.h>
#include <utils.h>
#include <part_frame_impl.h>

#include <qpainter.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qkeycode.h>

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
}

void KFormulaView::paintEvent( QPaintEvent *_ev )
{
    m_pDoc->paintEvent(_ev, this);
}
void KFormulaView::mousePressEvent( QMouseEvent *_ev )
{
    m_pDoc->mousePressEvent(_ev);
}

void KFormulaView::resizeEvent( QResizeEvent *_ev )
{
}

void KFormulaView::keyPressEvent( QKeyEvent *k )
{
    m_pDoc->keyPressEvent(k);
 /*int c,prev,next,c1,c2,c3,len,type;
 c=m_pDoc->getCurrent();
 warning(c);
 prev=m_pDoc->Blocks[c]->getprev();
 next=m_pDoc->Blocks[c]->getnext();
 c1=m_pDoc->Blocks[c]->getchild1();
 c2=m_pDoc->Blocks[c]->getchild2();
 c3=m_pDoc->Blocks[c]->getchild3();
 type=m_pDoc->Blocks[c]->gettype();
 len=m_pDoc->Blocks[c]->getcont().length();

 if(k->key()==Key_Left) {
			warning("LEFT ;-)");
m_pDoc->Blocks[0]->setcont("ca**o");
  */
  }



void KFormulaView::slotModified()
{
    update();
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
    m_vMenuBarFactory = m_vPartShell->menuBarFactory();
    if ( !CORBA::is_nil( m_vMenuBarFactory ) ) {
	// Menubar
	m_rMenuBar = m_vMenuBarFactory->createMenuBar( this );
	
	// View
	m_idMenuView = m_rMenuBar->insertMenu( i18n( "&View" ) );
	
	m_idMenuView_NewView = m_rMenuBar->insertItem( i18n( "&New View" ),
						       m_idMenuView,
						       this, "newView" );
    }
    
    m_vToolBarFactory = m_vPartShell->toolBarFactory();
    if ( !CORBA::is_nil( m_vToolBarFactory ) ) {
	
	m_rToolBarFormula = m_vToolBarFactory->createToolBar( this, i18n( "Formula" ) );
	
	m_idButtonFormula_0 = addToolButton(m_rToolBarFormula, "mini-xy.xpm", 
					    i18n( "Add/change to simple text" ),
					    "addB0" );

	m_idButtonFormula_1 = addToolButton(m_rToolBarFormula, "mini-root.xpm", 
					    i18n( "Add/change to root" ), "addB1" );

	m_idButtonFormula_2 = addToolButton(m_rToolBarFormula, "mini-frac.xpm",
					    i18n( "Add/change fract line" ), "addB4");

	m_idButtonFormula_3 = addToolButton(m_rToolBarFormula, "mini-vspace.xpm",
					    i18n( "Add/change vertical space" ), "addB4bis");
	
	
	m_idButtonFormula_4 = addToolButton(m_rToolBarFormula, "mini-bra.xpm",
					    i18n( "Add/change a bracket block" ), "addB3" );

	m_idButtonFormula_5 = addToolButton(m_rToolBarFormula, "mini-integral.xpm",
					    i18n( "Add/change an integral" ), "addB2" );

	m_idButtonFormula_6 = addToolButton(m_rToolBarFormula, "mini-symbols.xpm",
					    i18n( "Add/change a block with symbols" ), "addB5" );
    }

    if ( !CORBA::is_nil( m_vToolBarFactory ) )
	{
	    m_rToolBarFont = m_vToolBarFactory->createToolBar( this, i18n( "Font" ) );

	    m_idButtonFont_0 = addToolButton(m_rToolBarFont, "reduce.xpm",
					     i18n( "Reduce the font of active block" ), "reduce");

	    m_idButtonFont_1 = addToolButton(m_rToolBarFont, "enlarge.xpm",
					     i18n( "Enlarge the font of active block" ), "enlarge");

	    m_idButtonFont_2 = addToolButton(m_rToolBarFont, "reduceall.xpm",
					     i18n( "Reduce the font of active block & children" ), "reduceRecur");
      
	    m_idButtonFont_3 = addToolButton(m_rToolBarFont, "enlargeall.xpm",
					     i18n( "Enlarge the font of active block & children" ), "enlargeRecur" );

	    m_idButtonFont_4 = addToolButton(m_rToolBarFont, "enlargenext.xpm",
					     i18n( "Reduce the font of active block, children & next blocks" ), "enlargeAll");

	    m_idButtonFont_5 = addToolButton(m_rToolBarFont, "greek.xpm",
					     i18n( "Set greek font" ), "setGreek" );
	    m_rToolBarFont->setToggle( m_idButtonFont_5, true );
	}
  
    if ( !CORBA::is_nil( m_vToolBarFactory ) )
	{
	    m_rToolBarType = m_vToolBarFactory->createToolBar( this, i18n( "Type" ) );
	  
	    m_idButtonType_0 = addToolButton(m_rToolBarType, "kformula1-0.xpm", 
					     i18n( "Add exponent" ), "addCh1" );

	    m_idButtonType_1 = addToolButton(m_rToolBarType, "kformula2-0.xpm", 
					     i18n( "Add index" ), "addCh2" );

	    m_idButtonType_2 = addToolButton(m_rToolBarType, "kformula2-1.xpm", 
					     i18n( "Add root index" ), "addCh2" );

	    m_idButtonType_3 = addToolButton(m_rToolBarType, "kformula2-2.xpm", 
					     i18n( "Add high limit" ), "addCh2" );

	    m_idButtonType_4 = addToolButton(m_rToolBarType, "kformula3-2.xpm", 
					     i18n( "Add low limit" ), "addCh3" );

	    // TODO check, why two exponents button exist!
	    m_idButtonType_5 = addToolButton(m_rToolBarType, "kformula2-3.xpm", 
					     i18n( "Add exponent" ), "addCh2" );

	    // TODO insert Line Edit control
	}  
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

void KFormulaView::addB0()
{
    debug("addB0");
    m_pDoc->addB0();
}

void KFormulaView::addB1()
{ 
    debug("addB1");
    m_pDoc->addB1();
}

void KFormulaView::addB4()
{
    debug("addB4");
    m_pDoc->addB4();
}

void KFormulaView::addB4bis()
{
    debug("addB4bis");
    m_pDoc->addB4bis();
}

void KFormulaView::addB3()
{
    debug("addB3");
    m_pDoc->addB3();

}

void KFormulaView::addB2()
{
    debug("addB2");
    m_pDoc->addB4();

}

void KFormulaView::addB5()
{
    debug("addB5");
    m_pDoc->addB5();

}

void KFormulaView::reduce()
{
    debug("reduce");
    m_pDoc->reduce();

}

void KFormulaView::enlarge()
{
    debug("enlarge");
    m_pDoc->enlarge();

}

void KFormulaView::reduceRecur()
{
}

void KFormulaView::enlargeRecur()
{
}

void KFormulaView::enlargeAll()
{
}

void KFormulaView::setGreek()
{
}

void KFormulaView::addCh1()
{
    debug("addCh1");
    m_pDoc->addCh1();

}

void KFormulaView::addCh2()
{
    debug("addCh2");
    m_pDoc->addCh2();

}

void KFormulaView::addCh3()
{
    debug("addCh3");
    m_pDoc->addCh3();

}

#include "kformula_view.moc"






