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
#include <kcolordlg.h>
#include <klocale.h>
#include <kprocess.h>
#include <kiconloader.h>

#include <opUIUtils.h>
#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <koPartSelectDia.h>
#include <koPrintDia.h>
#include <koAboutDia.h>

#include "kinputdialog.h"
#include "kintegerinputdialog.h"

#include "kimage_doc.h"
#include "kimage_shell.h"
#include <klocale.h>

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
    return true;
  }

  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( ICON("fittoview.xpm") );
  m_idButtonEdit_Lines = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ),
							this, "viewFitToView", true,
							i18n( "Fit image to view" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("fitwithprops.xpm") );
  m_idButtonEdit_Areas = m_vToolBarEdit->insertButton2( pix, 2, SIGNAL( clicked() ),
							this, "viewFitWithProportions",
							true, i18n( "Fit to view and keep proportions" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("originalsize.xpm") );
  m_idButtonEdit_Bars = m_vToolBarEdit->insertButton2( pix , 3, SIGNAL( clicked() ),
						       this, "viewOriginalSize", true,
						       i18n( "Keep original image size" ), -1 );

  m_vToolBarEdit->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("mini/unknown.xpm") );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 4, SIGNAL( clicked() ),
							this, "editEditImage", true,
							i18n( "Edit image" ), -1 );

  m_vToolBarEdit->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("undo.xpm") );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "editUndo", true,
							i18n( "Undo" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("redo.xpm") );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "editRedo", true,
							i18n( "Redo" ), -1 );

  m_vToolBarEdit->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("editpaste.xpm") );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "editEditImage", true,
							i18n( "Edit image" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("areaselect.xpm") );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "selectArea", true,
							i18n( "Select Area" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("airbrush.xpm") );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "airbrush", true,
							i18n( "Airbrush" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("circle.xpm") );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "circle", true,
							i18n( "Circle" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("eraser.xpm") );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix , 5, SIGNAL( clicked() ),
							this, "eraser", true,
							i18n( "Eraser" ), -1 );

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
  debug( "bool KImageView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )" );

  if ( CORBA::is_nil( _menubar ) )
  {
	debug( "Setting to nil" );
    m_vMenuEdit = 0L;
    m_vMenuView = 0L;
    m_vMenuTransform = 0L;
    m_vMenuFilter = 0L;
    m_vMenuPlugIns = 0L;
    m_vMenuExtras = 0L;
    return true;
  }

  // Edit
  _menubar->insertMenu( i18n( "&Edit" ), m_vMenuEdit, -1, -1 );

  OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( ICON("undo.xpm") );
  m_idMenuEdit_Edit = m_vMenuEdit->insertItem6( pix, i18n("no Undo possible"), this, "editUndo", CTRL + Key_E, -1, -1 );

  pix = OPUIUtils::convertPixmap( ICON("redo.xpm") );
  m_idMenuEdit_Edit = m_vMenuEdit->insertItem6( pix, i18n("no Redo possible"), this, "editRedo", CTRL + Key_E, -1, -1 );

  pix = OPUIUtils::convertPixmap( ICON("mini/unknown.xpm") );
  m_idMenuEdit_Edit = m_vMenuEdit->insertItem6( pix, i18n("&Edit image"), this, "editEditImage", 0, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  m_idMenuEdit_Import = m_vMenuEdit->insertItem( i18n("&Import image"), this, "editImportImage", CTRL + Key_I );
  m_idMenuEdit_Export = m_vMenuEdit->insertItem( i18n("E&xport image"), this, "editExportImage", CTRL + Key_X );
  m_idMenuEdit_Export = m_vMenuEdit->insertItem( i18n("E&mbed Part"), this, "editEmpedPart", 0 );

  m_vMenuEdit->insertSeparator( -1 );

  m_idMenuEdit_Page = m_vMenuEdit->insertItem( i18n("&Page Layout"), this, "editPageLayout", CTRL + Key_L );
  m_idMenuEdit_Preferences = m_vMenuEdit->insertItem( i18n("P&references..."), this, "editPreferences", 0 );

  // View
  _menubar->insertMenu( i18n( "&View" ), m_vMenuView, -1, -1 );

  pix = OPUIUtils::convertPixmap( ICON("fittoview.xpm") );
  m_idMenuView_FitToView = m_vMenuView->insertItem6( pix, i18n("Fit to &view"), this, "viewFitToView", CTRL + Key_V, -1, -1 );

  pix = OPUIUtils::convertPixmap( ICON("fitwithprops.xpm") );
  m_idMenuView_FitWithProps = m_vMenuView->insertItem6( pix, i18n("Fit and keep &proportions"), this, "viewFitWithProportions", CTRL + Key_P, -1, -1 );

  pix = OPUIUtils::convertPixmap( ICON("originalsize.xpm") );
  m_idMenuView_Original = m_vMenuView->insertItem6( pix, i18n("&Original size"), this, "viewOriginalSize", CTRL + Key_O, -1, -1 );

  m_vMenuView->insertSeparator( -1 );

  m_idMenuView_Center = m_vMenuView->insertItem( i18n("&Centered"), this, "viewCentered", 0 );
  m_idMenuView_Info = m_vMenuView->insertItem( i18n("&Scrollbars"), this, "viewScrollbars", 0 );
  m_idMenuView_Info = m_vMenuView->insertItem( i18n("I&nfomations"), this, "viewInfoImage", 0 );
  m_idMenuView_BackgroundColor = m_vMenuView->insertItem( i18n("Background color"), this, "viewBackgroundColor", 0 );

  // Transform
  _menubar->insertMenu( i18n( "&Transform" ), m_vMenuTransform, -1, -1 );

  m_idMenuTransform_RotateRight = m_vMenuTransform->insertItem( i18n("Rotate clockwise"), this, "transformRotateRight", 0 );
  m_idMenuTransform_RotateLeft = m_vMenuTransform->insertItem( i18n("Rotate anti-clockwise"), this, "transformRotateLeft", 0 );
  m_idMenuTransform_RotateAngle = m_vMenuTransform->insertItem( i18n("Rotate with angle..."), this, "transformRotateAngle", 0 );
  m_idMenuTransform_FlipVertical = m_vMenuTransform->insertItem( i18n("Flip vertical"), this, "transformFlipVertical", 0 );
  m_idMenuTransform_FlipHorizontal = m_vMenuTransform->insertItem( i18n("Flip honrizontal"), this, "transformFlipHorizontal", 0 );

  m_vMenuTransform->insertSeparator( -1 );

  m_idMenuTransform_ZoomFactor = m_vMenuTransform->insertItem( i18n("&Zoom..."), this, "transformZoomFactor", 0 );
  m_idMenuTransform_ZoomIn10 = m_vMenuTransform->insertItem( i18n("Zoom &in 10%"), this, "transformZoomIn10", 0 );
  m_idMenuTransform_ZoomOut10 = m_vMenuTransform->insertItem( i18n("Zoom &out 10%"), this, "transformZoomOut10", 0 );
  m_idMenuTransform_ZoomDouble = m_vMenuTransform->insertItem( i18n("&Double size"), this, "transformZoomDouble", 0 );
  m_idMenuTransform_ZoomHalf = m_vMenuTransform->insertItem( i18n("&Half size"), this, "transformZoomHalf", 0 );
  m_idMenuTransform_ZoomMax = m_vMenuTransform->insertItem( i18n("&Max"), this, "transformZoomMax", 0 );
  m_idMenuTransform_ZoomMaxAspect = m_vMenuTransform->insertItem( i18n("Max/&aspect"), this, "transformZoomMaxAspect", 0 );

  // Filter
  _menubar->insertMenu( i18n( "F&ilter" ), m_vMenuFilter, -1, -1 );

  // PlugIns
  _menubar->insertMenu( i18n( "&Plug-Ins" ), m_vMenuPlugIns, -1, -1 );

  // Extras
  _menubar->insertMenu( i18n( "&Extras" ), m_vMenuExtras, -1, -1 );

  m_idMenuExtras_RunGimp = m_vMenuExtras->insertItem( i18n("Run &Gimp"), this, "extrasRunGimp", 0 );
  m_idMenuExtras_RunXV = m_vMenuExtras->insertItem( i18n("Run &xv"), this, "extrasRunXV", 0 );
  m_idMenuExtras_RunCommand = m_vMenuExtras->insertItem( i18n("Run &Command..."), this, "extrasRunCommand", 0 );

  return true;
}

void KImageView::helpUsing()
{
  kapp->invokeHTMLHelp( "kimage/kimage.html", QString::null );
}

CORBA::Boolean KImageView::printDlg()
{
  QPrinter prt;
  if( QPrintDialog::getPrinterSetup( &prt ) )
  {
    m_pDoc->print( &prt );
  }

  return true;
}

void KImageView::editPageLayout()
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
  if( m_pDoc->image().isNull() )
    return;

  double dh = (double) height() / (double) m_pDoc->image().height();
  double dw = (double) width() / (double) m_pDoc->image().width();

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

void KImageView::viewFitToView()
{
  if( m_pDoc->image().isNull() )
    return;

  m_drawMode = 1;
  
  slotUpdateView();
}

void KImageView::viewFitWithProportions()
{
  if( m_pDoc->image().isNull() )
    return;

  m_drawMode = 2;
  
  slotUpdateView();
}

void KImageView::viewOriginalSize()
{
  if( m_pDoc->image().isNull() )
    return;

  m_drawMode = 0;
  
  slotUpdateView();
}

void KImageView::editEditImage()
{
  if( m_pDoc->image().isNull() )
    return;

  QWidget::update();
}

void KImageView::editImportImage()
{
  debug( "import this=%i", (int)this );

  QString file = KFileDialog::getOpenFileName( getenv( "HOME" ) );

  if( file.isNull() )
  {
    return;
  }
  
  if( !m_pDoc->openDocument( file, 0L ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
    return;
  }
}

void KImageView::editExportImage()
{
  if( m_pDoc->isEmpty() )
  {
    QString tmp;
    QMessageBox::critical( this, i18n( "IO Error" ), i18n("The document is empty\nNothing to export."), i18n( "OK" ) );
    return;
  }

  QString file = KFileDialog::getSaveFileName( getenv( "HOME" ) );

  if( file.isNull() )
    return;

  if( !m_pDoc->saveDocument( file, 0L ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
  }
}

void KImageView::viewInfoImage()
{
  if( m_pDoc->isEmpty() )
  {
    QMessageBox::critical( this, i18n( "KImage Error" ), i18n( "The document is empty\nNo information available." ), i18n( "OK" ) );
    return;
  }

  QMessageBox::information( this, i18n( "Image information" ), i18n( "Infos " ), i18n( "OK" ) );
}

void KImageView::viewCentered()
{
  if( m_pDoc->isEmpty() )
  {
    QMessageBox::critical( this, i18n( "KImage Error" ), i18n("The document is empty\nAction not available."), i18n( "OK" ) );
    return;
  }

  m_centerMode = 1 - m_centerMode;

  slotUpdateView();
}

/**
 ** Rotates the image clockwise
 */
void KImageView::transformRotateRight()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "Rotate Right" );

  QWMatrix matrix;
  matrix.rotate( 90 );
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

/**
 ** Rotates the image anti-clockwise
 */
void KImageView::transformRotateLeft()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "Rotate Left" );

  QWMatrix matrix;
  matrix.rotate( -90 );
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

/**
 ** Rotates the image with an angle
 */
void KImageView::transformRotateAngle()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "Rotate Angle" );

  int angle = 0;
  KIntegerInputDialog dlg( NULL, "KImage", i18n( "Enter angle:" ) ); 

  if( dlg.getValue( angle ) != QDialog::Accepted )
    return;

  if( angle == 0 )
    return;
    
  QWMatrix matrix;
  matrix.rotate( angle );
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

/**
 ** Flips the image vertical
 */
void KImageView::transformFlipVertical()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "flipVertical" );

  QWMatrix matrix;
  QWMatrix matrix2( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  matrix *= matrix2;
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

/**
 ** Flips the image horizontal
 */
void KImageView::transformFlipHorizontal()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "flipHorizontal" );

  QWMatrix matrix;
  QWMatrix matrix2( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  matrix2.rotate( 180 );
  matrix *= matrix2;
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

/**
 ** Sets the background color of the viewer
 */
void KImageView::viewBackgroundColor()
{
  KColorDialog dlg;
  QColor color;

  dlg.getColor( color );  
  setBackgroundColor( color );

  QWidget::update();
}

void KImageView::transformZoomFactor()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "Zoom Factor" );

  QWMatrix matrix;
  int factor = (int)(matrix.m11() * 100 );
  KIntegerInputDialog dlg( NULL, "KImage", i18n( "Enter Zoom factor (100 = 1x):" ) ); 

  if( dlg.getValue( factor ) != QDialog::Accepted )
    return;

  if( ( factor <= 0 ) || ( factor == 100 ) )
    return;
    
  double val = (double)factor/100;
  matrix.scale( val, val );
  
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

void KImageView::transformZoomIn10()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "Zoom In 10" );

  QWMatrix matrix;
  matrix.scale( 1.1, 1.1 );
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

void KImageView::transformZoomOut10()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "Zoom Out 10" );

  QWMatrix matrix;
  matrix.scale( 0.9, 0.9 );
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

void KImageView::transformZoomDouble()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "Zoom Double" );

  QWMatrix matrix;
  matrix.scale( 2.0, 2.0 );
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

void KImageView::transformZoomHalf()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "Zoom Half" );

  QWMatrix matrix;
  matrix.scale( 0.5, 0.5 );
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

void KImageView::transformZoomMax()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "Zoom Max" );

  QWMatrix matrix( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

void KImageView::transformZoomMaxAspect()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "Zoom Max Aspect" );

  QWMatrix matrix( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  m_pDoc->transformImage( matrix );

  slotUpdateView();
}

void KImageView::editPreferences()
{
}

QString KImageView::tmpFilename()
{
  QString result;

  result = "/tmp/kimage_pid.image";

  return result;
}

void KImageView::extrasRunGimp()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "starting Process" );

  KProcess proc;

  proc << "gimp";

  proc << "/t";

  proc.start( KProcess::Block );

  debug( "Process ended" );
}

void KImageView::extrasRunXV()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "starting Process" );

  KProcess proc;

  proc << "xv";

  // proc << "/home/devel/.kderc" << "/home/devel/.kde2rc";

  proc.start( KProcess::Block );

  debug( "Process ended" );
}

void KImageView::extrasRunCommand()
{
  if( m_pDoc->isEmpty() )
    return;

  debug( "Run Command" );

  QString command = tmpFilename();
  KInputDialog dlg( this, i18n( "KImage" ), i18n( "Commandline:" ) );

  if( dlg.getStr( command ) != QDialog::Accepted )
    return;

  debug( "starting Process: " + command );

  KProcess proc;

  proc << "display";

  proc << command;

  proc.start( KProcess::Block );

  debug( "Process ended" );
}

void KImageView::resizeEvent( QResizeEvent *_ev )
{
  if( m_pDoc->isEmpty() )
    return;
}

/**
 ** Paints the image in the shell window
 */
void KImageView::paintEvent( QPaintEvent *_ev )
{
  if( m_pixmap.isNull() )
    return;

  QPainter painter;
  painter.begin( this );

  if( m_centerMode )
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
