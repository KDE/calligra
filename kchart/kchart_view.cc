#include "kchart_view.h"
#include "kchart_doc.h"
#include "kchart_main.h"

#include <op_app.h>
#include <utils.h>
#include <part_frame_impl.h>

#include <qpainter.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qkeycode.h>

/**********************************************************
 *
 * KChartView
 *
 **********************************************************/

KChartView::KChartView( QWidget *_parent = 0L, const char *_name = 0L ) :
  QWidget( _parent, _name ), View_impl(), KCharter::View_skel()
{
  setWidget( this );
  
  setBackgroundColor( white );
  
  Control_impl::setFocusPolicy( OPControls::Control::ClickFocus );
 
  m_pDoc = 0L;

  setGeometry( 5000, 5000, 100, 100 );
}

KChartView::~KChartView()
{
  cleanUp();
}

void KChartView::cleanUp()
{
  if ( m_bIsClean )
    return;
  
  m_pDoc->removeView( this );
  
  m_rMenuBar = 0L;
  m_vMenuBarFactory = 0L;

  m_rToolBarLayout = 0L;
  m_vToolBarFactory = 0L;

  View_impl::cleanUp();
}
  
void KChartView::setDocument( KChartDocument *_doc )
{
  if ( m_pDoc )
    m_pDoc->removeView( this );

  View_impl::setDocument( _doc );
  
  m_pDoc = _doc;

  m_pDoc->addView( this );

  QObject::connect( m_pDoc, SIGNAL( sig_modified() ), this, SLOT( slotChartModified() ) );
}

void KChartView::paintEvent( QPaintEvent *_ev )
{
  m_pDoc->chart().repaintChart( this );
}

void KChartView::resizeEvent( QResizeEvent *_ev )
{
  m_pDoc->chart().repaintChart( this );
}

void KChartView::slotChartModified()
{
  update();
}

void KChartView::createGUI()
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
    m_rToolBarLayout = m_vToolBarFactory->createToolBar( this, i18n( "Layout" ) );

    QString tmp = kapp->kde_datadir().copy();
    tmp += "/kchart/pics/bars.xpm";
    QString pix = loadPixmap( tmp );
    m_idButtonLayout_Bars = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), i18n( "Bars" ), this, "setTypeBars" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kchart/pics/area.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Area = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), "Area", this, "setTypeArea" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kchart/pics/lines.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Lines = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), "Lines", this, "setTypeLines" );
  }
}

void KChartView::newView()
{
  assert( (m_pDoc != 0L) );

  KChartShell* shell = new KChartShell;
  shell->enableMenuBar();
  shell->PartShell_impl::enableStatusBar();
  shell->enableToolBars();
  shell->show();
  shell->setDocument( m_pDoc );
  
  CORBA::release( shell );
}

void KChartView::setTypeBars()
{
  m_pDoc->chart().setChartType( Bars );
  m_pDoc->emitModified();
}

void KChartView::setTypeLines()
{
  m_pDoc->chart().setChartType( Lines );
  m_pDoc->emitModified();
}

void KChartView::setTypeArea()
{
  m_pDoc->chart().setChartType( Area );
  m_pDoc->emitModified();
}

#include "kchart_view.moc"






