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

KFormulaView::KFormulaView( QWidget *_parent = 0L, const char *_name = 0L ) :
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

  QObject::connect( m_pDoc, SIGNAL( sig_modified() ), this, SLOT( slotModified() ) );
}

void KFormulaView::paintEvent( QPaintEvent *_ev )
{
}

void KFormulaView::resizeEvent( QResizeEvent *_ev )
{
}

void KFormulaView::slotModified()
{
  update();
}

void KFormulaView::createGUI()
{ 
  m_vMenuBarFactory = m_vPartShell->menuBarFactory();
  if ( !CORBA::is_nil( m_vMenuBarFactory ) )
  {
    // Menubar
    m_rMenuBar = m_vMenuBarFactory->createMenuBar( this );

    // View
    m_idMenuView = m_rMenuBar->insertMenu( i18n( "&View" ) );

    m_idMenuView_NewView = m_rMenuBar->insertItem( i18n( "&New View" ), m_idMenuView,
						       this, "newView" );
  }

  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if ( !CORBA::is_nil( m_vToolBarFactory ) )
  {
    m_rToolBarFormula = m_vToolBarFactory->createToolBar( this, i18n( "Formula" ) );

    QString tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/mini-xy.xpm";
    QString pix = loadPixmap( tmp );
    m_idButtonFormula_0 = m_rToolBarFormula->insertButton( CORBA::string_dup( pix ), i18n( "Add/change to simple text" ), this, "addB0" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/mini-root.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFormula_1 = m_rToolBarFormula->insertButton( CORBA::string_dup( pix ), i18n( "Add/change to root" ), this, "addB1" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/mini-frac.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFormula_2 = m_rToolBarFormula->insertButton( CORBA::string_dup( pix ), i18n( "Add/change fract line" ), this, "addB4" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/mini-vspace.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFormula_3 = m_rToolBarFormula->insertButton( CORBA::string_dup( pix ), i18n( "Add/change vertical space" ), this, "addB4bis" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/mini-bra.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFormula_4 = m_rToolBarFormula->insertButton( CORBA::string_dup( pix ), i18n( "Add/change a bracket block" ), this, "addB3" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/mini-integral.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFormula_5 = m_rToolBarFormula->insertButton( CORBA::string_dup( pix ), i18n( "Add/change an integral" ), this, "addB2" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/mini-symbols.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFormula_6 = m_rToolBarFormula->insertButton( CORBA::string_dup( pix ), i18n( "Add/change a block with symbols" ), this, "addB5" );
  }

  if ( !CORBA::is_nil( m_vToolBarFactory ) )
  {
    m_rToolBarFont = m_vToolBarFactory->createToolBar( this, i18n( "Font" ) );

    QString tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/reduce.xpm";
    QString pix = loadPixmap( tmp );
    m_idButtonFont_0 = m_rToolBarFont->insertButton( CORBA::string_dup( pix ), i18n( "Reduce the font of active block" ), this, "reduce" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/enlarge.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFont_1 = m_rToolBarFont->insertButton( CORBA::string_dup( pix ), i18n( "Enlarge the font of active block" ), this, "enlarge" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/reduceall.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFont_2 = m_rToolBarFont->insertButton( CORBA::string_dup( pix ), i18n( "Reduce the font of active block & children" ),
						     this, "reduceRecur" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/enlargeall.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFont_3 = m_rToolBarFont->insertButton( CORBA::string_dup( pix ), i18n( "Enlarge the font of active block & children" ),
						     this, "enlargeRecur" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/enlargenext.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFont_4 = m_rToolBarFont->insertButton( CORBA::string_dup( pix ), i18n( "Reduce the font of active block, children & next blocks" ),
						     this, "enlargeAll" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/greek.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFont_5 = m_rToolBarFont->insertButton( CORBA::string_dup( pix ), i18n( "Set greek font" ), this, "setGreek" );
    m_rToolBarFont->setToggle( m_idButtonFont_5, true );
  }
  
  if ( !CORBA::is_nil( m_vToolBarFactory ) )
  {
    m_rToolBarType = m_vToolBarFactory->createToolBar( this, i18n( "Type" ) );

    QString tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/kformula1-0.xpm";
    QString pix = loadPixmap( tmp );
    m_idButtonType_0 = m_rToolBarType->insertButton( CORBA::string_dup( pix ), i18n( "Add exponent" ), this, "addCh1" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/kformula2-0.xpm";
    pix = loadPixmap( tmp );
    m_idButtonType_1 = m_rToolBarType->insertButton( CORBA::string_dup( pix ), i18n( "Add index" ), this, "addCh2" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/kformula2-1.xpm";
    pix = loadPixmap( tmp );
    m_idButtonType_2 = m_rToolBarType->insertButton( CORBA::string_dup( pix ), i18n( "Add root index" ), this, "addCh2" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/kformula2-2.xpm";
    pix = loadPixmap( tmp );
    m_idButtonType_3 = m_rToolBarType->insertButton( CORBA::string_dup( pix ), i18n( "Add high limit" ), this, "addCh2" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/kformula3-2.xpm";
    pix = loadPixmap( tmp );
    m_idButtonType_1 = m_rToolBarType->insertButton( CORBA::string_dup( pix ), i18n( "Add low limit" ), this, "addCh3" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kformula/pics/kformula2-3.xpm";
    pix = loadPixmap( tmp );
    m_idButtonType_1 = m_rToolBarType->insertButton( CORBA::string_dup( pix ), i18n( "Add exponent" ), this, "addCh2" );

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

#include "kformula_view.moc"






