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
#include <qwmatrix.h>
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
  m_centerMode = 0;

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

  QString tmp = kapp->kde_datadir().copy();
  tmp += "/kimage/pics/fittoview.xpm";
  OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Lines = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ),
							this, "fitToView", true,
							i18n( "Fit image to view" ), -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kimage/pics/fitwithprops.xpm";
  pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Areas = m_vToolBarEdit->insertButton2( pix, 2, SIGNAL( clicked() ),
							this, "fitWithProportions",
							true, i18n( "Fit to view and keep proportions" ), -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kimage/pics/originalsize.xpm";
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
    m_vMenuView = 0L;
    m_vMenuTransform = 0L;
    m_vMenuFilter = 0L;
    m_vMenuPlugIns = 0L;
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

  // View
  _menubar->insertMenu( i18n( "&View" ), m_vMenuView, -1, -1 );

  path = kapp->kde_datadir().copy();
  path += "/kimage/pics/fittoview.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuView_FitToView = m_vMenuView->insertItem6( pix, i18n("Fit to &view"), this,
						     "fitToView", CTRL + Key_V, -1, -1 );

  path = kapp->kde_datadir().copy();
  path += "/kimage/pics/fitwithprops.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuView_FitWithProps = m_vMenuView->insertItem6( pix, i18n("Fit and keep &proportions"),
							this, "fitWithProportions",
							CTRL + Key_P, -1, -1 );

  path = kapp->kde_datadir().copy();
  path += "/kimage/pics/originalsize.xpm";
  pix = OPUIUtils::loadPixmap( path );
  m_idMenuView_Original = m_vMenuView->insertItem6( pix, i18n("&Original size"), this,
						    "originalSize", CTRL + Key_O, -1, -1 );

  m_vMenuView->insertSeparator( -1 );

  m_idMenuView_Center = m_vMenuView->insertItem( i18n("&Centered"), this, "centered", CTRL + Key_C );
  m_idMenuView_Info = m_vMenuView->insertItem( i18n("I&nfomations"), this, "infoImage", CTRL + Key_N );
  m_idMenuView_BackgroundColor = m_vMenuView->insertItem( i18n("Background color"), this, "backgroundColor", CTRL + Key_N );

  // Transform
  _menubar->insertMenu( i18n( "&Transform" ), m_vMenuTransform, -1, -1 );

  m_idMenuTransform_RotateRight = m_vMenuTransform->insertItem( i18n("Rotate clockwise"), this, "rotateRight", CTRL + Key_C );
  m_idMenuTransform_RotateLeft = m_vMenuTransform->insertItem( i18n("Rotate anti-clockwise"), this, "rotateLeft", CTRL + Key_N );
  m_idMenuTransform_FlipVertical = m_vMenuTransform->insertItem( i18n("Flip vertical"), this, "flipVertical", CTRL + Key_C );
  m_idMenuTransform_FlipHorizontal = m_vMenuTransform->insertItem( i18n("Flip honrizontal"), this, "flipHorizontal", CTRL + Key_N );

  m_vMenuTransform->insertSeparator( -1 );

  m_idMenuTransform_ZoomFactor = m_vMenuTransform->insertItem( i18n("&Zoom..."), this, "zoomFactor", CTRL + Key_N );
  m_idMenuTransform_ZoomIn10 = m_vMenuTransform->insertItem( i18n("Zoom &in 10%"), this, "zoomIn10", CTRL + Key_N );
  m_idMenuTransform_ZoomOut10 = m_vMenuTransform->insertItem( i18n("Zoom &out 10%"), this, "zoomOut10", CTRL + Key_N );
  m_idMenuTransform_ZoomDouble = m_vMenuTransform->insertItem( i18n("&Double size"), this, "zoomDouble", CTRL + Key_N );
  m_idMenuTransform_ZoomHalf = m_vMenuTransform->insertItem( i18n("&Half size"), this, "zoomHalf", CTRL + Key_N );
  m_idMenuTransform_ZoomMax = m_vMenuTransform->insertItem( i18n("&Max"), this, "zoomMax", CTRL + Key_N );
  m_idMenuTransform_ZoomMaxAspect = m_vMenuTransform->insertItem( i18n("Max/&aspect"), this, "zoomMaxAspect", CTRL + Key_N );

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

  double dh = (double)height()/(double)m_pDoc->image().height();
  double dw = (double)width()/(double)m_pDoc->image().width();

  double d = ( dh < dw ? dh : dw );
	
  switch ( m_drawMode )
  {
    default:
  	case 0:
	  m_pixmap.convertFromImage( m_pDoc->image() );
  	  break;
  	case 1:
	  m_pixmap.convertFromImage( m_pDoc->image().smoothScale( width(), height() ) );
  	  break;
  	case 2:
	  m_pixmap.convertFromImage( m_pDoc->image().smoothScale( int( d * m_pDoc->image().width() ), int ( d * m_pDoc->image().height() ) ) );
  	  break;
  }

  QWidget::update();
}

void KImageView::fitToView()
{
  if ( m_pDoc->image().isNull() )
    return;

  m_drawMode = 1;
  
  slotUpdateView();
}

void KImageView::fitWithProportions()
{
  if ( m_pDoc->image().isNull() )
    return;

  m_drawMode = 2;
  
  slotUpdateView();
}

void KImageView::originalSize()
{
  if ( m_pDoc->image().isNull() )
    return;

  m_drawMode = 0;
  
  slotUpdateView();
}

void KImageView::editImage()
{
  if ( m_pDoc->image().isNull() )
    return;

  QWidget::update();
}

void KImageView::importImage()
{
  debug( "import this=%i", (int)this );

  QString file = KFileDialog::getOpenFileName( getenv( "HOME" ) );

  if ( file.isNull() )
  {
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

void KImageView::centered()
{
  if ( m_pDoc->isEmpty() )
  {
    QString tmp;
    QMessageBox::critical( this, i18n( "KImage Error" ), i18n("The document is empty\nNo information available."), i18n( "OK" ) );
    return;
  }

  m_centerMode = 1 - m_centerMode;

  slotUpdateView();
}

void KImageView::rotateRight()
{
  if ( m_pDoc->isEmpty() )
    return;

  debug( "Rotate Right" );

  QWMatrix matrix;
  matrix.rotate( 90 );
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

void KImageView::rotateLeft()
{
  if ( m_pDoc->isEmpty() )
    return;

  debug( "Rotate Left" );

  QWMatrix matrix;
  matrix.rotate( -90 );
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

void KImageView::flipVertical()
{
  if ( m_pDoc->isEmpty() )
    return;

  debug( "flipVertical" );

  QWMatrix matrix;
  QWMatrix matrix2( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  matrix *= matrix2;
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

void KImageView::flipHorizontal()
{
  if ( m_pDoc->isEmpty() )
    return;

  debug( "flipHorizontal" );

  QWMatrix matrix;
  QWMatrix matrix2( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  matrix2.rotate( 180 );
  matrix *= matrix2;
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

void KImageView::backgroundColor()
{
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

  if ( m_centerMode == 1 )
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
