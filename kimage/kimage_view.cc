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
#include "kimage_view.h"

#include <kapp.h>
#include <qmsgbox.h>
#include <iostream.h>
#include <stdlib.h>
#include <qkeycode.h>
#include <qprndlg.h>
#include <kfiledialog.h>

#include <opUIUtils.h>
#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <koPartSelectDia.h>
#include <koPrintDia.h>
#include <koAboutDia.h>

#include "kimage_doc.h"
#include "kimage_shell.h"

bool shit = false;

/*****************************************************************************
 *
 * KImageView
 *
 *****************************************************************************/

KImageView::KImageView( QWidget *_parent, const char *_name, KImageDoc* _doc ) :
  QWidget( _parent ), KoViewIf( _doc ), OPViewIf( _doc ), KImage::View_skel()
{
  setWidget( this );

  OPPartIf::setFocusPolicy( OpenParts::Part::ClickFocus ); 

  m_pDoc = _doc;
  
  QObject::connect( m_pDoc, SIGNAL( sig_updateView() ), this, SLOT( slotUpdateView() ) );

  slotUpdateView();
}

void KImageView::init()
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

KImageView::~KImageView()
{
  cerr << "KImageView::~KImageView() " << _refcnt() << endl;

  cleanUp();
}

void KImageView::cleanUp()
{
  cerr << "void KImageView::cleanUp() " << endl;
  
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

bool KImageView::event( const char* _event, const CORBA::Any& _value )
{
  EVENT_MAPPER( _event, _value );

  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_var, mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_var, mappingCreateToolbar );

  END_EVENT_MAPPER;
  
  return false;
}

bool KImageView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  cerr << "bool KImageView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" << endl;
  
  if ( CORBA::is_nil( _factory ) )
  {
    cerr << "Setting to nil" << endl;
    m_vToolBarEdit = 0L;
    cerr << "niled" << endl;
    return true;
  }

  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  QString tmp = kapp->kde_icondir().copy();
  tmp += "/mini/unknown.xpm";
  OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Lines = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ),
							this, "fitToView", true,
							i18n( "Fit image to view" ), -1 );

  tmp = kapp->kde_icondir().copy();
  tmp += "/mini/unknown.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Areas = m_vToolBarEdit->insertButton2( pix, 2, SIGNAL( clicked() ),
							this, "fitWithProportions",
							true, i18n( "Fit to view and keep proportions" ), -1 );

  tmp = kapp->kde_icondir().copy();
  tmp += "/mini/unknown.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Bars = m_vToolBarEdit->insertButton2( pix , 3, SIGNAL( clicked() ),
						       this, "originalSize", true,
						       i18n( "Keep original image size" ), -1 );

  m_vToolBarEdit->insertSeparator( -1 );
  
  tmp = kapp->kde_icondir().copy();
  tmp += "/mini/unknown.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 3, SIGNAL( clicked() ),
							this, "editImage", true, 
							i18n( "Edit image" ), -1 );
  
  m_vToolBarEdit->enable( OpenPartsUI::Show );

  return true;
}

bool KImageView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
  if ( CORBA::is_nil( _menubar ) )
  {
    m_vMenuEdit = 0L;
    return true;
  }

  // Edit  
  _menubar->insertMenu( i18n( "&Edit" ), m_vMenuEdit, -1, -1 );

  QString path = kapp->kde_icondir().copy();
  path += "/mini/unknown.xpm";
  OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap( path );
  m_idMenuEdit_FitToView = m_vMenuEdit->insertItem6( pix, i18n("Fit to &view"), this,
						     "fitToView", CTRL + Key_V, -1, -1 );

  path = kapp->kde_icondir().copy();
  path += "/mini/unknown.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuEdit_FitWithProps = m_vMenuEdit->insertItem6( pix, i18n("Fit and keep &proportions"),
							this, "fitWithProportions",
							CTRL + Key_P, -1, -1 );

  path = kapp->kde_icondir().copy();
  path += "/mini/unknown.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuEdit_Original = m_vMenuEdit->insertItem6( pix, i18n("&Original size"), this,
						    "originalSize", CTRL + Key_O, -1, -1 );
  
  m_vMenuEdit->insertSeparator( -1 );
  
  path = kapp->kde_icondir().copy();
  path += "/mini/unknown.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuEdit_Edit = m_vMenuEdit->insertItem6( pix, i18n("&Edit image"), this, "editImage", CTRL + Key_E, -1, -1 );
  m_idMenuEdit_Import = m_vMenuEdit->insertItem( i18n("&Import image"), this, "importImage", CTRL + Key_I );
  m_idMenuEdit_Export = m_vMenuEdit->insertItem( i18n("E&xport image"), this, "exportImage", CTRL + Key_X );

  m_vMenuEdit->insertSeparator( -1 );
  
  m_idMenuEdit_Page = m_vMenuEdit->insertItem( i18n("&Page Layout"), this, "pageLayout", CTRL + Key_L );

  return true;
}      

void KImageView::helpUsing()
{
  kapp->invokeHTMLHelp( "kimage/kimage.html", 0 );
}

CORBA::Boolean KImageView::printDlg()
{
  QPrinter prt;
  if ( QPrintDialog::getPrinterSetup( &prt ) )
  {    
    m_pDoc->print( &prt );
  }
  
  return true;
}

void KImageView::pageLayout()
{
  m_pDoc->paperLayoutDlg();
}
  
void KImageView::newView()
{
  assert( (m_pDoc != 0L) );

  KImageShell* shell = new KImageShell;
  shell->show();
  shell->setDocument( m_pDoc );
}

void KImageView::slotUpdateView()
{
  if ( m_pDoc->image().isNull() )
    return;
  
  m_pixmap.convertFromImage( m_pDoc->image() );
  
  QWidget::update();
}

void KImageView::fitToView()
{
}

void KImageView::fitWithProportions()
{
}

void KImageView::originalSize()
{
}

void KImageView::editImage()
{
}

void KImageView::importImage()
{
  cerr << "import this=" << (int)this << endl;
  
  QString file = KFileDialog::getOpenFileName( getenv( "HOME" ) );

  if ( file.isNull() )
    return;
  
  if ( !m_pDoc->openDocument( file, 0L ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
    return;
  }
}

void KImageView::exportImage()
{
  if ( m_pDoc->isEmpty() )
  {
    QString tmp;
    QMessageBox::critical( this, i18n( "IO Error" ), i18n("The document is empty\nNothing to export."), i18n( "OK" ) );
    return;
  }
   
  QString file = KFileDialog::getSaveFileName( getenv( "HOME" ) );

  if ( file.isNull() )
    return;
   
  if ( !m_pDoc->saveDocument( file, 0L ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
  }
}

void KImageView::resizeEvent( QResizeEvent *_ev )
{
  if ( m_pDoc->isEmpty() )
    return;
}

void KImageView::paintEvent( QPaintEvent *_ev )
{
  if ( m_pixmap.isNull() )
    return;
  
  QPainter painter;
  painter.begin( this );
  
  painter.drawPixmap( ( width() - m_pixmap.width() ) / 2, ( height() - m_pixmap.height() ) / 2, m_pixmap );
  
  painter.end();
}

#include "kimage_view.moc"
