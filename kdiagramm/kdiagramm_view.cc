/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qprinter.h>
#include "kdiagramm_view.h"

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
#include <koPrintDia.h>
#include <koAboutDia.h>

#include "kdiagramm_doc.h"
#include "kdiagramm_shell.h"

/*****************************************************************************
 *
 * KDiagrammView
 *
 *****************************************************************************/

KDiagrammView::KDiagrammView( QWidget *_parent, const char *_name, KDiagrammDoc* _doc ) :
  KoDiagrammView( _parent ), KoViewIf( _doc ), OPViewIf( _doc ), KDiagramm::View_skel()
{
  setWidget( this );

  OPPartIf::setFocusPolicy( OpenParts::Part::ClickFocus );

  m_pDoc = _doc;

  QObject::connect( m_pDoc, SIGNAL( sig_updateView() ), this, SLOT( slotUpdateView() ) );

  setBackgroundColor(white);
}

void KDiagrammView::init()
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

KDiagrammView::~KDiagrammView()
{
  cerr << "KDiagrammView::~KDiagrammView() " << _refcnt() << endl;

  cleanUp();
}

void KDiagrammView::cleanUp()
{
  cerr << "void KDiagrammView::cleanUp() " << endl;

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

bool KDiagrammView::event( const char* _event, const CORBA::Any& _value )
{
  EVENT_MAPPER( _event, _value );

  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_var, mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_var, mappingCreateToolbar );
  MAPPING( KDiagramm::View::eventConfigured, KDiagramm::View::EventConfigured,
	   mappingEventConfigured );


  END_EVENT_MAPPER;

  return false;
}

bool KDiagrammView::mappingEventConfigured( KDiagramm::View::EventConfigured _event )
{
  switch ( _event.mode )
  {
  case KDiagramm::LINES:
    m_pDoc->setDiaType( KoDiagramm::DT_LINIEN );
    break;
  case KDiagramm::AREAS:
    m_pDoc->setDiaType( KoDiagramm::DT_AREA );
    break;
  case KDiagramm::BARS:
    m_pDoc->setDiaType( KoDiagramm::DT_SAEULEN );
    break;
  case KDiagramm::CAKES:
    m_pDoc->setDiaType( KoDiagramm::DT_KREIS );
    break;
  }

  return true;
}

bool KDiagrammView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  cerr << "bool KDiagrammView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" << endl;

  if ( CORBA::is_nil( _factory ) )
  {
    cerr << "Setting to nil" << endl;
    m_vToolBarEdit = 0L;
    cerr << "niled" << endl;
    return true;
  }

  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  QString tmp = kapp->kde_datadir().copy();
  tmp += "/kdiagramm/pics/lines.xpm";
  OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Lines = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "modeLines", true, i18n( "Lines" ), -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kdiagramm/pics/areas.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Areas = m_vToolBarEdit->insertButton2( pix, 2, SIGNAL( clicked() ), this, "modeAreas", true, i18n( "Areas" ), -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kdiagramm/pics/bars.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Bars = m_vToolBarEdit->insertButton2( pix , 3, SIGNAL( clicked() ), this, "modeBars", true, i18n( "3D-Bars" ), -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kdiagramm/pics/cakes.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 3, SIGNAL( clicked() ), this, "modeCakes", true, i18n( "Cakes" ), -1 );

  m_vToolBarEdit->enable( OpenPartsUI::Show );

  m_vToolBarEdit->enable( OpenPartsUI::Hide );
  m_vToolBarEdit->setBarPos(OpenPartsUI::Floating);
  m_vToolBarEdit->setBarPos(OpenPartsUI::Top);
  m_vToolBarEdit->enable( OpenPartsUI::Show );

  return true;
}

bool KDiagrammView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
  if ( CORBA::is_nil( _menubar ) )
  {
    m_vMenuEdit = 0L;
    return true;
  }

  // Edit
  _menubar->insertMenu( i18n( "&Edit" ), m_vMenuEdit, -1, -1 );

  QString path = kapp->kde_datadir().copy();
  path += "/kdiagramm/pics/lines.xpm";
  OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap( path );
  m_idMenuEdit_Lines = m_vMenuEdit->insertItem6( pix, i18n("&Lines"), this, "modeLines", CTRL + Key_L, -1, -1 );

  path = kapp->kde_datadir().copy();
  path += "/kdiagramm/pics/areas.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuEdit_Areas = m_vMenuEdit->insertItem6( pix, i18n("&Areas"), this, "modeAreas", CTRL + Key_A, -1, -1 );

  path = kapp->kde_datadir().copy();
  path += "/kdiagramm/pics/bars.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuEdit_Bars = m_vMenuEdit->insertItem6( pix, i18n("&Bars"), this, "modeBars", CTRL + Key_B, -1, -1 );

  path = kapp->kde_datadir().copy();
  path += "/kdiagramm/pics/cakes.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuEdit_Cakes = m_vMenuEdit->insertItem6( pix, i18n("&Cakes"), this, "modeCakes", CTRL + Key_C, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  m_idMenuEdit_Data = m_vMenuEdit->insertItem( i18n("&Edit Data..."), this, "editData", CTRL + Key_E );

  m_vMenuEdit->insertSeparator( -1 );

  m_idMenuEdit_Page = m_vMenuEdit->insertItem( i18n("&Page Layout"), this, "pageLayout", CTRL + Key_L );

  m_idMenuConfig_Chart = m_vMenuEdit->insertItem( i18n( "C&onfigure Chart..." ), this, "configChart", CTRL + Key_O );

  return true;
}

void KDiagrammView::configChart()
{
       m_pDoc->configChart();
}



void KDiagrammView::editData()
{
  m_pDoc->editData();
}

void KDiagrammView::helpUsing()
{
  kapp->invokeHTMLHelp( "kdiagramm/kdiagramm.html", QString::null );
}

CORBA::Boolean KDiagrammView::printDlg()
{
  QPrinter prt;
  if ( QPrintDialog::getPrinterSetup( &prt ) )
  {
    m_pDoc->print( &prt );
  }

  return true;
}

void KDiagrammView::pageLayout()
{
  m_pDoc->paperLayoutDlg();
}

void KDiagrammView::newView()
{
  assert( (m_pDoc != 0L) );

  KDiagrammShell* shell = new KDiagrammShell;
  shell->show();
  shell->setDocument( m_pDoc );
}

void KDiagrammView::modeLines()
{
  KDiagramm::View::EventConfigured event;
  event.mode = KDiagramm::LINES;

  EMIT_EVENT( this, KDiagramm::View::eventConfigured, event );
}

void KDiagrammView::modeAreas()
{
  KDiagramm::View::EventConfigured event;
  event.mode = KDiagramm::AREAS;

  EMIT_EVENT( this, KDiagramm::View::eventConfigured, event );
}

void KDiagrammView::modeBars()
{
  KDiagramm::View::EventConfigured event;
  event.mode = KDiagramm::BARS;

  EMIT_EVENT( this, KDiagramm::View::eventConfigured, event );
}

void KDiagrammView::modeCakes()
{
  KDiagramm::View::EventConfigured event;
  event.mode = KDiagramm::CAKES;

  EMIT_EVENT( this, KDiagramm::View::eventConfigured, event );
}

void KDiagrammView::slotUpdateView()
{
  m_diagramm.setData( m_pDoc->data(), "", KoDiagramm::DAT_NUMMER, m_pDoc->diaType() );

  update();
}

#include "kdiagramm_view.moc"
