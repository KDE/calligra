#include <qprinter.h>
#include "kchart_view.h"

#include <kapp.h>
#include <qmsgbox.h>
#include <iostream.h>
#include <stdlib.h>
#include <qkeycode.h>
#include <qprndlg.h>

#include <opUIUtils.h>
#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <koPartSelectDia.h>
#include <koAboutDia.h>

#include "kchart_doc.h"
#include "kchart_shell.h"

#include <klocale.h>
#include <kiconloader.h>

/*****************************************************************************
 *
 * KChartView
 *
 *****************************************************************************/

KChartView::KChartView( QWidget *_parent,
			      const char */*_name*/, KChartDoc* _doc ) :
  KoChartView( _parent ), KoViewIf( _doc ), OPViewIf( _doc ), KChart::View_skel()
{
  setWidget( this );

  OPPartIf::setFocusPolicy( OpenParts::Part::ClickFocus );

  m_pDoc = _doc;

  QObject::connect( m_pDoc, SIGNAL( sig_updateView() ), this, SLOT( slotUpdateView() ) );

  widget()->setBackgroundColor(Qt::white);
}

void KChartView::init()
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

  // Paint something
  slotUpdateView();
}

KChartView::~KChartView()
{
  cerr << "KChartView::~KChartView() " << _refcnt() << endl;

  cleanUp();
}

void KChartView::cleanUp()
{
  cerr << "void KChartView::cleanUp() " << endl;

  if ( m_bIsClean )
    return;

  cerr << "1b) Unregistering menu and toolbar" << endl;

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->unregisterClient( id() );

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->unregisterClient( id() );

  m_pDoc->removeView( this );

  KoViewIf::cleanUp();
}

bool KChartView::event( const char* _event, const CORBA::Any& _value )
{
    cerr << "Got Event: " << _event << "\n";
  EVENT_MAPPER( _event, _value );

  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_ptr, mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_ptr, mappingCreateToolbar );

  END_EVENT_MAPPER;

  return false;
}

bool KChartView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  cerr << "bool KChartView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" << endl;

  if ( CORBA::is_nil( _factory ) )
  {
    cerr << "Setting to nil" << endl;
    m_vToolBarEdit = 0L;
    cerr << "niled" << endl;
    return true;
  }

  QString toolTip;

  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( BarIcon( "lines" ) );
  toolTip = i18n( "Lines" );
  m_idButtonEdit_Lines = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "modeLines", true, toolTip, -1 );

  pix = OPUIUtils::convertPixmap( BarIcon( "areas" ) );
  toolTip = i18n( "Areas" );
  m_idButtonEdit_Areas = m_vToolBarEdit->insertButton2( pix, 2, SIGNAL( clicked() ), this, "modeAreas", true, toolTip, -1 );

  pix = OPUIUtils::convertPixmap( BarIcon( "bars" ) );
  toolTip = i18n( "3D-Bars" );
  m_idButtonEdit_Bars = m_vToolBarEdit->insertButton2( pix , 3, SIGNAL( clicked() ), this, "modeBars", true, toolTip, -1 );

  pix = OPUIUtils::convertPixmap( BarIcon( "cakes" ) );
  toolTip = i18n( "Cakes" );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 3, SIGNAL( clicked() ), this, "modeCakes", true, toolTip, -1 );

  m_vToolBarEdit->enable( OpenPartsUI::Show );

  m_vToolBarEdit->enable(OpenPartsUI::Hide);
  m_vToolBarEdit->setBarPos(OpenPartsUI::Floating);
  m_vToolBarEdit->setBarPos(OpenPartsUI::Top);
  m_vToolBarEdit->enable(OpenPartsUI::Show);

  return true;
}

bool KChartView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
  if ( CORBA::is_nil( _menubar ) )
  {
    m_vMenuEdit = 0L;
    return true;
  }

  QString text;

  // Edit
  text = i18n( "&Edit" );
  _menubar->insertMenu( text, m_vMenuEdit, -1, -1 );

  OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( BarIcon( "lines" ) );
  text = i18n("&Lines");
  m_idMenuEdit_Lines = m_vMenuEdit->insertItem6( pix, text, this, "modeLines", CTRL + Key_L, -1, -1 );

  pix = OPUIUtils::convertPixmap( BarIcon( "areas" ) );
  text = i18n("&Areas");
  m_idMenuEdit_Areas = m_vMenuEdit->insertItem6( pix, text, this, "modeAreas", CTRL + Key_A, -1, -1 );

  pix = OPUIUtils::convertPixmap( BarIcon( "bars" ) );
  text = i18n("&Bars");
  m_idMenuEdit_Bars = m_vMenuEdit->insertItem6( pix, text, this, "modeBars", CTRL + Key_B, -1, -1 );

  pix = OPUIUtils::convertPixmap( BarIcon( "cakes" ) );
  text = i18n("&Cakes");
  m_idMenuEdit_Cakes = m_vMenuEdit->insertItem6( pix, text, this, "modeCakes", CTRL + Key_C, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  text = i18n("&Edit Data...");
  m_idMenuEdit_Data = m_vMenuEdit->insertItem( text, this, "editData", CTRL + Key_E );

  text = i18n(" C&onfigure Chart..." );
  m_idMenuConfig_Chart = m_vMenuEdit->insertItem( text, this, "configChart", CTRL + Key_O );

  m_vMenuEdit->insertSeparator( -1 );

  text = i18n("&Page Layout") ;
  m_idMenuEdit_Page = m_vMenuEdit->insertItem( text, this, "pageLayout", CTRL + Key_L );

  return true;
}

void KChartView::editData()
{
  m_pDoc->editData();
}


void KChartView::configChart()
{
	m_pDoc->configChart();
}


void KChartView::helpUsing()
{
  kapp->invokeHTMLHelp( "kdiagramm/kdiagramm.html", QString::null );
}

bool KChartView::printDlg()
{
  QPrinter prt;
  if ( QPrintDialog::getPrinterSetup( &prt ) )
  {
    m_pDoc->print( &prt );
  }

  return true;
}

void KChartView::pageLayout()
{
  m_pDoc->paperLayoutDlg();
}

void KChartView::newView()
{
  assert( (m_pDoc != 0L) );

  KChartShell* shell = new KChartShell;
  shell->show();
  shell->setDocument( m_pDoc );
}

void KChartView::modeLines()
{
  m_pDoc->setDiaType( KoChart::DT_LINIEN );
}

void KChartView::modeAreas()
{
  m_pDoc->setDiaType( KoChart::DT_AREA );
}

void KChartView::modeBars()
{
  m_pDoc->setDiaType( KoChart::DT_SAEULEN );
}

void KChartView::modeCakes()
{
  m_pDoc->setDiaType( KoChart::DT_KREIS );
}

void KChartView::slotUpdateView()
{
  m_diagramm.setData( m_pDoc->data(), "", KoChart::DAT_NUMMER, m_pDoc->diaType() );

  update();
}

#include "kchart_view.moc"
