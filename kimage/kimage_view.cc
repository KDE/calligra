/* This file is part of the KDE project

   Copyright (C) 1999 Michael Koch <koch@kde.org>

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
#include <qmsgbox.h>
#include <qkeycode.h>
#include <qprndlg.h>
#include <qwmatrix.h>

#include <kaction.h>
#include <kfiledialog.h>
#include <kcolordlg.h>
#include <klocale.h>
#include <kiconloader.h>
#include <klineeditdlg.h>

#include <koPartSelectDia.h>
#include <koAboutDia.h>

#include "kintegerinputdialog.h"
#include "zoomfactordlg.h"
#include "preferencesdlg.h"

#include "kimage_doc.h"
#include "kimage_view.h"
#include "kimage_shell.h"
#include "kimage_global.h"

KImageView::KImageView( KImageDocument* doc, QWidget* parent, const char* name )
  : ContainerView( doc, parent, name )
  , m_pDoc( doc )
{
  //setWidget( this );

  QObject::connect( m_pDoc, SIGNAL( sigUpdateView() ), this, SLOT( slotUpdateView() ) );

  // edit actions

  m_undo = new KAction( tr( "&Undo" ), KImageBarIcon( "undo" ), 0, this, SLOT( undo() ), actionCollection(), "undo" );
  m_undo = new KAction( tr( "&Redo" ), KImageBarIcon( "redo" ), 0, this, SLOT( redo() ), actionCollection(), "redo" );
  m_undo = new KAction( tr( "&Cut" ), KImageBarIcon( "editcut" ), 0, this, SLOT( cut() ), actionCollection(), "cut" );
  m_undo = new KAction( tr( "&Copy" ), KImageBarIcon( "editcopy" ), 0, this, SLOT( copy() ), actionCollection(), "copy" );
  m_undo = new KAction( tr( "&Paste" ), KImageBarIcon( "editpaste" ), 0, this, SLOT( paste() ), actionCollection(), "paste" );

  // view actions

  setBackgroundColor( red );

  m_drawMode = OriginalSize;
  m_centerMode = 0;
  m_zoomFactorValue = QPoint( 100, 100 );
  slotUpdateView();
}

void KImageView::paintEvent( QPaintEvent* event )
{
  if( m_pixmap.isNull() )
    return;

  QPainter painter;

  painter.begin( this );

  // TODO : Scaling

  m_pDoc->paintContent( painter, event->rect(), false );
  
  /*
  if( m_centerMode )
  {
    painter.drawPixmap( ( width() - m_pixmap.width() ) / 2, ( height() - m_pixmap.height() ) / 2, m_pixmap );
  }
  else
  {
    painter.drawPixmap( 0, 0, m_pixmap );
  }
  */
  
  painter.end();
}

/*
void KImageView::init()
{
  kdebug( KDEBUG_INFO, 0, "Registering menu as %li", id() );

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->registerClient( id(), this );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a menu bar manager" );

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->registerClient( id(), this );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a tool bar manager" );
}

KImageView::~KImageView()
{
  kdebug( KDEBUG_INFO, 0, "KImageView::~KImageView() %li", _refcnt() );

  cleanUp();
}

void KImageView::cleanUp()
{
  kdebug( KDEBUG_INFO, 0, "void KImageView::cleanUp() " );

  if ( m_bIsClean )
  {
    return;
  }

  kdebug( KDEBUG_INFO, 0, "1b) Unregistering menu and toolbar" );

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->unregisterClient( id() );

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->unregisterClient( id() );

  m_pDoc->removeView( this );

  KoViewIf::cleanUp();
}

KImageDocument* KImageView::doc()
{
  return m_pDoc;
}

bool KImageView::event( const QCString & _event, const CORBA::Any& _value )
{
  EVENT_MAPPER( _event, _value );

  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_ptr, mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_ptr, mappingCreateToolbar );

  END_EVENT_MAPPER;

  return false;
}

bool KImageView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  kdebug( KDEBUG_INFO, 0, "bool KImageView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" );

  if ( CORBA::is_nil( _factory ) )
  {
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    m_vToolBarEdit = 0L;
    return true;
  }

  QString text;
  OpenPartsUI::Pixmap_var pix;

  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  text =  i18n( "Fit image to view" ) ;
  pix = OPICON( "fittoview" );
  m_idButtonEdit_Lines = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "viewFitToView", true, text, -1 );

  text=  i18n( "Fit to view and keep proportions" ) ;
  pix = OPICON( "fitwithprops" );
  m_idButtonEdit_Areas = m_vToolBarEdit->insertButton2( pix, 2, SIGNAL( clicked() ), this, "viewFitWithProportions", true, text, -1 );

  text=  i18n( "Keep original image size" ) ;
  pix = OPICON( "originalsize" );
  m_idButtonEdit_Bars = m_vToolBarEdit->insertButton2( pix, 3, SIGNAL( clicked() ), this, "viewOriginalSize", true, text, -1 );

  m_vToolBarEdit->insertSeparator( -1 );

  text=  i18n( "Edit image" ) ;
  pix = OPICON( "undo" );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix, 4, SIGNAL( clicked() ), this, "editEditImage", true, text, -1 );

<<<<<<< kimage_view.cc
  m_vToolBarEdit->insertSeparator( -1 );

  text=  i18n( "Undo" ) ;
  pix = OPICON( "undo" );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix, 5, SIGNAL( clicked() ), this, "editUndo", true, text, -1 );

  text=  i18n( "Redo" ) ;
  pix = OPICON( "redo" );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix, 6, SIGNAL( clicked() ), this, "editRedo", true, text, -1 );

  m_vToolBarEdit->insertSeparator( -1 );

  text=  i18n( "Edit image" ) ;
  pix = OPICON( "editpaste" );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix, 7, SIGNAL( clicked() ), this, "editEditImage", true, text, -1 );

  text=  i18n( "Select Area" ) ;
  pix = OPICON( "areaselect" );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix, 8, SIGNAL( clicked() ), this, "selectArea", true, text, -1 );

  text=  i18n( "Airbrush" ) ;
  pix = OPICON( "airbrush" );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix, 9, SIGNAL( clicked() ), this, "airbrush", true, text, -1 );

  text=  i18n( "Circle" ) ;
  pix = OPICON( "circle" );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix, 10, SIGNAL( clicked() ), this, "circle", true, text, -1 );

  text=  i18n( "Eraser" ) ;
  pix = OPICON( "eraser" );
  m_idButtonEdit_Cakes = m_vToolBarEdit->insertButton2( pix, 11, SIGNAL( clicked() ), this, "eraser", true, text, -1 );

=======
>>>>>>> 1.33
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
  kdebug( KDEBUG_INFO, 0, "bool KImageView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )" );

  if ( CORBA::is_nil( _menubar ) )
  {
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    m_vMenuEdit = 0L;
    m_vMenuView = 0L;
    m_vMenuTransform = 0L;
    return true;
  }

  QString text;
  OpenPartsUI::Pixmap_var pix;

  // Edit
  text =  i18n( "&Edit" ) ;
  _menubar->insertMenu( text, m_vMenuEdit, -1, -1 );

<<<<<<< kimage_view.cc
  text =  i18n( "no Undo possible" ) ;
  pix = OPICON( "undo" );
  m_idMenuEdit_Undo = m_vMenuEdit->insertItem6( pix, text, this, "editUndo", 0, -1, -1 );

  text =  i18n( "no Redo possible" ) ;
  pix = OPICON( "redo" );
  m_idMenuEdit_Redo = m_vMenuEdit->insertItem6( pix, text, this, "editRedo", 0, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

=======
>>>>>>> 1.33
  text =  i18n( "&Import image" ) ;
  m_vMenuEdit->insertItem4( text, this, "editImportImage", CTRL + Key_I, -1, -1 );

  text =  i18n( "E&xport image" ) ;
  m_vMenuEdit->insertItem4( text, this, "editExportImage", CTRL + Key_X, -1, -1 );

  text =  i18n( "E&mbed Part" ) ;
  m_vMenuEdit->insertItem( text, this, "editEmbedPart", 0 );

  m_vMenuEdit->insertSeparator( -1 );

  text =  i18n( "&Page Layout" ) ;
  m_vMenuEdit->insertItem4( text, this, "editPageLayout", CTRL + Key_L, -1, -1 );

  text =  i18n( "P&references..." ) ;
  m_vMenuEdit->insertItem( text, this, "editPreferences", 0 );

  // View
  text =  i18n( "&View" ) ;
  _menubar->insertMenu( text, m_vMenuView, -1, -1 );

  text =  i18n( "Zoom..." ) ;
  m_idMenuView_ZoomFactor = m_vMenuView->insertItem( text, this, "viewZoomFactor", 0 );

  text =  i18n( "Fit to &view" ) ;
  pix = OPICON( "fittoview" );
  m_idMenuView_FitToView = m_vMenuView->insertItem6( pix, text, this, "viewFitToView", CTRL + Key_V, -1, -1 );

  text =  i18n( "Fit and keep &proportions" ) ;
  pix = OPICON( "fitwithprops" );
  m_idMenuView_FitWithProps = m_vMenuView->insertItem6( pix, text, this, "viewFitWithProportions", CTRL + Key_P, -1, -1 );

  text =  i18n( "&Original size" ) ;
  pix = OPICON( "originalsize" );
  m_idMenuView_Original = m_vMenuView->insertItem6( pix, text, this, "viewOriginalSize", CTRL + Key_O, -1, -1 );

  m_vMenuView->insertSeparator( -1 );

  text =  i18n( "&Centered" ) ;
  m_idMenuView_Center = m_vMenuView->insertItem( text, this, "viewCentered", 0 );

  text =  i18n( "&Scrollbars" ) ;
  m_idMenuView_Info = m_vMenuView->insertItem( text, this, "viewScrollbars", 0 );

  text =  i18n( "I&nformations" ) ;
  m_idMenuView_Info = m_vMenuView->insertItem( text, this, "viewInfoImage", 0 );

  text =  i18n( "Background color" ) ;
  m_idMenuView_BackgroundColor = m_vMenuView->insertItem( text, this, "viewBackgroundColor", 0 );

  // Transform
  text =  i18n( "&Transform" ) ;
  _menubar->insertMenu( text, m_vMenuTransform, -1, -1 );

  text =  i18n( "Rotate clockwise" ) ;
  m_idMenuTransform_RotateRight = m_vMenuTransform->insertItem( text, this, "transformRotateRight", 0 );

  text =  i18n( "Rotate anti-clockwise" ) ;
  m_idMenuTransform_RotateLeft = m_vMenuTransform->insertItem( text, this, "transformRotateLeft", 0 );

  text =  i18n( "Rotate with angle..." ) ;
  m_idMenuTransform_RotateAngle = m_vMenuTransform->insertItem( text, this, "transformRotateAngle", 0 );

  text =  i18n( "Flip vertical" ) ;
  m_idMenuTransform_FlipVertical = m_vMenuTransform->insertItem( text, this, "transformFlipVertical", 0 );

  text =  i18n( "Flip honrizontal" ) ;
  m_idMenuTransform_FlipHorizontal = m_vMenuTransform->insertItem( text, this, "transformFlipHorizontal", 0 );

  m_vMenuTransform->insertSeparator( -1 );

  text =  i18n( "&Zoom..." ) ;
  m_idMenuTransform_ZoomFactor = m_vMenuTransform->insertItem( text, this, "transformZoomFactor", 0 );

  text =  i18n( "zoom &in 10%" ) ;
  m_idMenuTransform_ZoomIn10 = m_vMenuTransform->insertItem( text, this, "transformZoomIn10", 0 );

  text =  i18n( "Zoom &out 10%" ) ;
  m_idMenuTransform_ZoomOut10 = m_vMenuTransform->insertItem( text, this, "transformZoomOut10", 0 );

  text =  i18n( "&Double size" ) ;
  m_idMenuTransform_ZoomDouble = m_vMenuTransform->insertItem( text, this, "transformZoomDouble", 0 );

  text =  i18n( "&Half size" ) ;
  m_idMenuTransform_ZoomHalf = m_vMenuTransform->insertItem( text, this, "transformZoomHalf", 0 );

  text =  i18n( "&Max" ) ;
  m_idMenuTransform_ZoomMax = m_vMenuTransform->insertItem( text, this, "transformZoomMax", 0 );

  text =  i18n( "Max/&aspect") ;
  m_idMenuTransform_ZoomMaxAspect = m_vMenuTransform->insertItem( text, this, "transformZoomMaxAspect", 0 );

  return true;
}

void KImageView::helpUsing()
{
  kapp->invokeHTMLHelp( "kimage/kimage.html", QString::null );
}

bool KImageView::printDlg()
{
  QPrinter prt;

  if( QPrintDialog::getPrinterSetup( &prt ) )
  {
    m_pDoc->print( &prt );
  }
  return true;
}

void KImageView::newView()
{
  ASSERT( m_pDoc != 0L );

  KImageShell* shell = new KImageShell;
  shell->show();
  shell->setDocument( m_pDoc );
}
*/

void KImageView::undo()
{
}

void KImageView::redo()
{
}

void KImageView::cut()
{
}

void KImageView::copy()
{
}

void KImageView::paste()
{
}

/*
void KImageView::editImportImage()
{
  kdebug( KDEBUG_INFO, 0, "import this=%i", (int) this );

  QString filter = i18n( "*.*|All files\n*.bmp|BMP\n*.jpg|JPEG\n*.png|PNG" );
  QString file = KFileDialog::getOpenFileName( getenv( "HOME" ), filter );

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
  {
    return;
  }
  if( !m_pDoc->saveDocument( file, 0L ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
  }
}

void KImageView::editPreferences()
{
  KImagePreferencesDialog dlg;

  int result = dlg.exec();
  if( result != QDialog::Accepted )
    return;
}

void KImageView::editPageLayout()
{
  m_pDoc->paperLayoutDlg();
}

void KImageView::viewFitToView()
{
  if( m_pDoc->image().isNull() )
  {
    return;
  }
  m_drawMode = FitToView;
  slotUpdateView();
}

void KImageView::viewFitWithProportions()
{
  if( m_pDoc->image().isNull() )
  {
    return;
  }
  m_drawMode = FitWithProps;
  slotUpdateView();
}

void KImageView::viewOriginalSize()
{
  if( m_pDoc->image().isNull() )
  {
    return;
  }
  m_drawMode = OriginalSize;
  slotUpdateView();
}

void KImageView::viewInfoImage()
{
  if( m_pDoc->isEmpty() )
  {
    QMessageBox::critical( this, i18n( "KImage Error" ), i18n( "The document is empty\nNo information available." ), i18n( "OK" ) );
    return;
  }

  QString tmp;

  QMessageBox::information( this, i18n( "Image information" ),
    tmp.sprintf( "X-Size : %i\nY-Size : %i\nColor depth : %i\n" + m_pDoc->m_strImageFormat,
    m_pixmap.size().width(),
    m_pixmap.size().height(),
    m_pixmap.depth() ), i18n( "OK" ) );
}

void KImageView::viewZoomFactor()
{
  if( m_pDoc->isEmpty() )
  {
    QMessageBox::critical( this, i18n( "KImage Error" ), i18n("The document is empty\nAction not available."), i18n( "OK" ) );
    return;
  }
  KZoomFactorDialog dlg( NULL, "KImage" );
  if( dlg.getValue( m_zoomFactor ) != QDialog::Accepted )
  {
    return;
  }
  kdebug( KDEBUG_INFO, 0, "zoom factor: X: %i, Y: %i", m_zoomFactor.x(), m_zoomFactor.y() );
  m_drawMode = ZoomFactor;
  slotUpdateView();
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

void KImageView::viewBackgroundColor()
{
  KColorDialog dlg;
  QColor color;

  dlg.getColor( color );
  setBackgroundColor( color );
  QWidget::update();
}

void KImageView::transformRotateRight()
{
  kdebug( KDEBUG_INFO, 0, "Rotate Right" );

  QWMatrix matrix;
  matrix.rotate( 90 );
  m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformRotateLeft()
{
  kdebug( KDEBUG_INFO, 0, "Rotate Left" );

  QWMatrix matrix;
  matrix.rotate( -90 );
  m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformRotateAngle()
{
  kdebug( KDEBUG_INFO, 0, "Rotate Angle" );

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

void KImageView::transformFlipVertical()
{
  kdebug( KDEBUG_INFO, 0, "flipVertical" );

  QWMatrix matrix;
  QWMatrix matrix2( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  matrix *= matrix2;
  m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformFlipHorizontal()
{
  kdebug( KDEBUG_INFO, 0, "flipHorizontal" );

  QWMatrix matrix;
  QWMatrix matrix2( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  matrix2.rotate( 180 );
  matrix *= matrix2;
  m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomFactor()
{
  kdebug( KDEBUG_INFO, 0, "Zoom Factor" );

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
  kdebug( KDEBUG_INFO, 0, "Zoom In 10" );

  QWMatrix matrix;
  matrix.scale( 1.1, 1.1 );
  m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomOut10()
{
  kdebug( KDEBUG_INFO, 0, "Zoom Out 10" );

  QWMatrix matrix;
  matrix.scale( 0.9, 0.9 );
  m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomDouble()
{
  kdebug( KDEBUG_INFO, 0, "Zoom Double" );

  QWMatrix matrix;
  matrix.scale( 2.0, 2.0 );
  m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomHalf()
{
  kdebug( KDEBUG_INFO, 0, "Zoom Half" );

  QWMatrix matrix;
  matrix.scale( 0.5, 0.5 );
  m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomMax()
{
  kdebug( KDEBUG_INFO, 0, "Zoom Max" );

  QWMatrix matrix( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomMaxAspect()
{
  kdebug( KDEBUG_INFO, 0, "Zoom Max Aspect" );

  QWMatrix matrix( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  m_pDoc->transformImage( matrix );
  slotUpdateView();
}

QString KImageView::tmpFilename()
{
  QString file;

  file.sprintf( "/tmp/kimage_%i.image", getpid() );
  kdebug( KDEBUG_INFO, 0, file );
  return file;
}

void KImageView::resizeEvent( QResizeEvent* )
{
  slotUpdateView();
}
*/

void KImageView::slotUpdateView()
{
/*
  if( m_pDoc->image().isNull() )
  {
    return;
  }

  double dh, dw, d;
	
  switch ( m_drawMode )
  {
  	case OriginalSize:
	  m_pixmap.convertFromImage( m_pDoc->image() );
  	  break;
  	case FitToView:
	  m_pixmap.convertFromImage( m_pDoc->image().smoothScale( width(), height() ) );
  	  break;
  	case FitWithProps:
      dh = (double) height() / (double) m_pDoc->image().height();
      dw = (double) width() / (double) m_pDoc->image().width();
      d = ( dh < dw ? dh : dw );
	  m_pixmap.convertFromImage( m_pDoc->image().smoothScale( int( d * m_pDoc->image().width() ), int ( d * m_pDoc->image().height() ) ) );
  	  break;
    case ZoomFactor:
      dw = m_pDoc->image().width() * m_zoomFactor.x() / 100;
      dh = m_pDoc->image().height() * m_zoomFactor.y() / 100;
	  m_pixmap.convertFromImage( m_pDoc->image().smoothScale( int( dw * m_pDoc->image().width() ), int ( dh * m_pDoc->image().height() ) ) );
  	  break;
  }
  QWidget::update();
*/
}

#include "kimage_view.moc"

