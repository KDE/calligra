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

  m_drawMode = 0;
  m_centerMode = 1;

  slotUpdateView();
}

void KImageView::init()
{
  /******************************************************
   * Menu
   ******************************************************/

  debug( "Registering menu as %li", id() );

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->registerClient( id(), this );
  else
    debug( "Did not get a menu bar manager" );

  /******************************************************
   * Toolbar
   ******************************************************/

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->registerClient( id(), this );
  else
    debug( "Did not get a tool bar manager" );
}

KImageView::~KImageView()
{
  debug( "KImageView::~KImageView() %li", _refcnt() );

  cleanUp();
}

void KImageView::cleanUp()
{
  debug( "void KImageView::cleanUp() " );

  if ( m_bIsClean )
    return;

  debug( "1b) Unregistering menu and toolbar" );

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
  debug( "bool KImageView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" );

  if ( CORBA::is_nil( _factory ) )
  {
    debug( "Setting to nil" );
    m_vToolBarEdit = 0L;
    debug( "niled" );
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
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 4, SIGNAL( clicked() ),
							this, "editImage", true,
							i18n( "Edit image" ), -1 );

  m_vToolBarEdit->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kimage/pics/undo.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "undo", true,
							i18n( "Undo" ), -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kimage/pics/redo.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "redo", true,
							i18n( "Redo" ), -1 );

  m_vToolBarEdit->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kimage/pics/editpaste.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "editImage", true,
							i18n( "Edit image" ), -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kimage/pics/areaselect.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "editImage", true,
							i18n( "Edit image" ), -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kimage/pics/airbrush.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "editImage", true,
							i18n( "Edit image" ), -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kimage/pics/circle.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "editImage", true,
							i18n( "Edit image" ), -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kimage/pics/eraser.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "editImage", true,
							i18n( "Edit image" ), -1 );

  m_vToolBarEdit->enable( OpenPartsUI::Show );

  // Folgendes muss mit der zuletzt eingefuegten ToolBar gemacht werden.
  // Wahrscheinlich ein Bug in den OpenPart

  m_vToolBarEdit->enable(OpenPartsUI::Hide);
  m_vToolBarEdit->setBarPos(OpenPartsUI::Floating);
  m_vToolBarEdit->setBarPos(OpenPartsUI::Top);
  m_vToolBarEdit->enable(OpenPartsUI::Show);                                         

  return true;
}

bool KImageView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
  if ( CORBA::is_nil( _menubar ) )
  {
    m_vMenuEdit = 0L;
    m_vMenuZoom = 0L;
    m_vMenuTransform = 0L;
    return true;
  }

  // Edit
  _menubar->insertMenu( i18n( "&Edit" ), m_vMenuEdit, -1, -1 );

  QString path = kapp->kde_icondir().copy();
  path += "/mini/unknown.xpm";
  OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap( path );
  m_idMenuEdit_Edit = m_vMenuEdit->insertItem6( pix, i18n("&Edit image"), this, "editImage", CTRL + Key_E, -1, -1 );
  m_idMenuEdit_Import = m_vMenuEdit->insertItem( i18n("&Import image"), this, "importImage", CTRL + Key_I );
  m_idMenuEdit_Export = m_vMenuEdit->insertItem( i18n("E&xport image"), this, "exportImage", CTRL + Key_X );

  m_vMenuEdit->insertSeparator( -1 );

  m_idMenuEdit_Page = m_vMenuEdit->insertItem( i18n("&Page Layout"), this, "pageLayout", CTRL + Key_L );
  m_idMenuEdit_Page = m_vMenuEdit->insertItem( i18n("I&nfomations"), this, "infoImage", CTRL + Key_N );

  // View
  _menubar->insertMenu( i18n( "&View" ), m_vMenuView, -1, -1 );

  m_idMenuEdit_Center = m_vMenuView->insertItem( i18n("&Centered"), this, "centered", CTRL + Key_C );

  // Zoom
  _menubar->insertMenu( i18n( "&Zoom" ), m_vMenuZoom, -1, -1 );

  path = kapp->kde_icondir().copy();
  path += "/mini/unknown.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuZoom_FitToView = m_vMenuZoom->insertItem6( pix, i18n("Fit to &view"), this,
						     "fitToView", CTRL + Key_V, -1, -1 );

  path = kapp->kde_icondir().copy();
  path += "/mini/unknown.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuZoom_FitWithProps = m_vMenuZoom->insertItem6( pix, i18n("Fit and keep &proportions"),
							this, "fitWithProportions",
							CTRL + Key_P, -1, -1 );

  path = kapp->kde_icondir().copy();
  path += "/mini/unknown.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuZoom_Original = m_vMenuZoom->insertItem6( pix, i18n("&Original size"), this,
						    "originalSize", CTRL + Key_O, -1, -1 );

  // Transform
  _menubar->insertMenu( i18n( "&Transform" ), m_vMenuTransform, -1, -1 );

  // Filter
  _menubar->insertMenu( i18n( "F&ilter" ), m_vMenuFilter, -1, -1 );

  // PlugIns
  _menubar->insertMenu( i18n( "&Plug-Ins" ), m_vMenuPlugIns, -1, -1 );

  return true;
}

void KImageView::helpUsing()
{
  kapp->invokeHTMLHelp( "kimage/kimage.html", QString::null );
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

/**
 **  Gets the image from the document and resizes it if necessary
 */
void KImageView::slotUpdateView()
{
  if ( m_pDoc->image().isNull() )
    return;

  m_pixmap.convertFromImage( m_pDoc->image() );

  QWidget::update();
}

void KImageView::fitToView()
{
  if ( m_pDoc->image().isNull() )
    return;

  debug( "  Mach was !!!" );

  QWidget::update();
}

void KImageView::fitWithProportions()
{
  if ( m_pDoc->image().isNull() )
    return;

  debug( "  Mach was !!!" );

  QWidget::update();
}

void KImageView::originalSize()
{
  if ( m_pDoc->image().isNull() )
    return;

  debug( "  Mach was !!!" );

  QWidget::update();
}

void KImageView::editImage()
{
  if ( m_pDoc->image().isNull() )
    return;

  debug( "  Mach was !!!" );

  QWidget::update();
}

void KImageView::importImage()
{
  debug( "import this=%i", (int)this );

  QString file = KFileDialog::getOpenFileName( getenv( "HOME" ) );

  if ( file.isNull() )
  {
  	debug( "nix" );
    return;
  }
  
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

void KImageView::infoImage()
{
  if ( m_pDoc->isEmpty() )
  {
    QString tmp;
    QMessageBox::critical( this, i18n( "KImage Error" ), i18n("The document is empty\nNo information available."), i18n( "OK" ) );
    return;
  }
}

void KImageView::resizeEvent( QResizeEvent *_ev )
{
  if ( m_pDoc->isEmpty() )
    return;
}

/**
 ** Paints the image in the shell window
 */
void KImageView::paintEvent( QPaintEvent *_ev )
{
  if ( m_pixmap.isNull() )
    return;

  QPainter painter;
  painter.begin( this );

  if ( m_centerMode )
  {
    painter.drawPixmap( ( width() - m_pixmap.width() ) / 2, ( height() - m_pixmap.height() ) / 2, m_pixmap );
  }
  else
  {
    painter.drawPixmap( 0, 0, m_pixmap );
  }
  
  painter.end();
}

#include "kimage_view.moc"
