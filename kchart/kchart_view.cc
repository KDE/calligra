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

  /*
  m_rToolBarFile = 0L;
  m_rToolBarEdit = 0L;
  m_vToolBarFactory = 0L;
  */

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
    m_idMenuView = m_rMenuBar->insertMenu( CORBA::string_dup( "&View" ) );

    m_idMenuView_NewView = m_rMenuBar->insertItem( CORBA::string_dup( "&New View" ), m_idMenuView,
						       this, CORBA::string_dup( "newView" ) );
  }

  /*
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if ( !CORBA::is_nil( m_vToolBarFactory ) )
  {
    m_rToolBarFile = m_vToolBarFactory->createToolBar( this, CORBA::string_dup( "File" ) );
    m_rToolBarFile->setFileToolBar( true );
    QString tmp = opapp->kde_icondir().copy();
    tmp += "/mini/mini-doc.xpm";
    QString pix = loadPixmap( tmp );
    m_idButtonFile_Open = m_rToolBarFile->insertButton( CORBA::string_dup( pix ), "Open", 0L, 0L );

    m_rToolBarEdit = m_vToolBarFactory->createToolBar( this, "Edit" );
    tmp = opapp->kde_icondir().copy();
    tmp += "/mini/mini-eyes.xpm";
    pix = loadPixmap( tmp );
    m_idButtonEdit_Darker = m_rToolBarEdit->insertButton( CORBA::string_dup( pix ), "Darker", 0L, 0L );
  }
  */
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

#include "kchart_view.moc"

