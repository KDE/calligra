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

  m_undo  = new KAction( i18n( "&Undo" ), KImageBarIcon( "undo" ), 0, this, SLOT( undo() ), actionCollection(), "undo" );
  m_redo  = new KAction( i18n( "&Redo" ), KImageBarIcon( "redo" ), 0, this, SLOT( redo() ), actionCollection(), "redo" );
  m_cut   = new KAction( i18n( "&Cut" ), KImageBarIcon( "editcut" ), 0, this, SLOT( cut() ), actionCollection(), "cut" );
  m_copy  = new KAction( i18n( "&Copy" ), KImageBarIcon( "editcopy" ), 0, this, SLOT( copy() ), actionCollection(), "copy" );
  m_paste = new KAction( i18n( "&Paste" ), KImageBarIcon( "editpaste" ), 0, this, SLOT( paste() ), actionCollection(), "paste" );

  // view actions

  m_viewFactor      = new KAction( i18n( "&Zoom view..." ), 0, this, SLOT( viewZoomFactor() ), actionCollection(), "viewZoomFactor" );
  m_fitToView       = new KAction( i18n( "&Fit image to view" ), KImageBarIcon( "fittoview" ), 0, this, SLOT( viewFitToView() ), actionCollection(), "viewFitToView" );
  m_fitWithProps    = new KAction( i18n( "&Fit image to view props" ), KImageBarIcon( "fitwithprops" ), 0, this, SLOT( viewFitWithProportions() ), actionCollection(), "viewFitWithProportions" );
  m_original        = new KAction( i18n( "&Original size" ), KImageBarIcon( "originalsize" ), 0, this, SLOT( viewOriginalSize() ), actionCollection(), "viewOriginalSize" );
  m_center          = new KAction( i18n( "&Center image" ), 0, this, SLOT( viewCentered() ), actionCollection(), "viewCentered" );
  m_scrollbars      = new KAction( i18n( "&Scrollbars" ), 0, this, SLOT( viewScrollbars() ), actionCollection(), "viewScrollbars" );
  m_info            = new KAction( i18n( "&Informations" ), 0, this, SLOT( viewInformations() ), actionCollection(), "viewInformations" );
  m_backgroundColor = new KAction( i18n( "&Background color" ), 0, this, SLOT( viewBackgroundColor() ), actionCollection(), "viewBackgroundColor" );

  // transform actions

  m_rotateRight    = new KAction( i18n( "Rotate &right" ), 0, this, SLOT( transformRotateRight() ), actionCollection(), "transformRotateRight" );
  m_rotateLeft     = new KAction( i18n( "Rotate &left" ), 0, this, SLOT( transformRotateLeft() ), actionCollection(), "transformRotateLeft" );
  m_rotateAngle    = new KAction( i18n( "Rotate &angle" ), 0, this, SLOT( transformRotateAngle() ), actionCollection(), "transformRotateAngle" );
  m_flipVertical   = new KAction( i18n( "Flip &vertical" ), 0, this, SLOT( transformFlipVertical() ), actionCollection(), "transformFlipVertical" );
  m_flipHorizontal = new KAction( i18n( "Flip &horizontal" ), 0, this, SLOT( transformFlipHorizontal() ), actionCollection(), "transformFlipHorizontal" );
  m_zoomFactor     = new KAction( i18n( "&Zoom..." ), 0, this, SLOT( transformZoomFactor() ), actionCollection(), "transformZoomFactor" );
  m_zoomIn10       = new KAction( i18n( "Zoom &in 10 %" ), 0, this, SLOT( transformZoomIn10() ), actionCollection(), "transformZoomIn10" );
  m_zoomOut10      = new KAction( i18n( "Zoom &out 10%" ), 0, this, SLOT( transformZoomOut10() ), actionCollection(), "transformZoomOut10" );
  m_zoomDouble     = new KAction( i18n( "Zoom &double" ), 0, this, SLOT( transformZoomDouble() ), actionCollection(), "transformZoomDouble" );
  m_zoomHalf       = new KAction( i18n( "Zoom &half" ), 0, this, SLOT( transformZoomHalf() ), actionCollection(), "transformZoomHalf" );
  m_zoomMax        = new KAction( i18n( "Zoom &max" ), 0, this, SLOT( transformZoomMax() ), actionCollection(), "transformZoomMax" );
  m_zoomMaxAspect  = new KAction( i18n( "Zoom max &aspect" ), 0, this, SLOT( transformZoomMaxAspect() ), actionCollection(), "transformZoomMaxAspect" );

  // help actions

  m_helpAbout = new KAction( i18n( "About K&Image..." ), 0, this, SLOT( helpAboutKImage() ), actionCollection(), "helpAboutKImage" );
  m_helpUsing = new KAction( i18n( "Using..." ), 0, this, SLOT( helpUsingHelp() ), actionCollection(), "helpUsingHelp" );

  m_undo->setEnabled( false );
  m_redo->setEnabled( false );
  m_cut->setEnabled( false );
  m_copy->setEnabled( false );
  m_paste->setEnabled( false );

  setBackgroundColor( darkBlue );

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
KImageDocument* KImageView::doc()
{
  return m_pDoc;
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
*/

void KImageView::editPreferences()
{
  KImagePreferencesDialog dlg;

  int result = dlg.exec();
  if( result != QDialog::Accepted )
    return;
}

void KImageView::editPageLayout()
{
  //m_pDoc->paperLayoutDlg();
}

void KImageView::viewFitToView()
{
  if( m_pDoc->image().isNull() )
    return;

  m_drawMode = FitToView;
  slotUpdateView();
}

void KImageView::viewFitWithProportions()
{
  if( m_pDoc->image().isNull() )
    return;

  m_drawMode = FitWithProps;
  slotUpdateView();
}

void KImageView::viewOriginalSize()
{
  if( m_pDoc->image().isNull() )
    return;

  m_drawMode = OriginalSize;
  slotUpdateView();
}

void KImageView::viewInformations()
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
/*
  KZoomFactorDialog dlg( NULL, "KImage" );
  if( dlg.getValue( m_zoomFactor ) != QDialog::Accepted )
  {
    return;
  }
  kdebug( KDEBUG_INFO, 0, "zoom factor: X: %i, Y: %i", m_zoomFactor.x(), m_zoomFactor.y() );
  m_drawMode = ZoomFactor;
*/
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

void KImageView::viewScrollbars()
{
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
  //m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformRotateLeft()
{
  kdebug( KDEBUG_INFO, 0, "Rotate Left" );

  QWMatrix matrix;
  matrix.rotate( -90 );
  //m_pDoc->transformImage( matrix );
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
  //m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformFlipVertical()
{
  kdebug( KDEBUG_INFO, 0, "flipVertical" );

  QWMatrix matrix;
  QWMatrix matrix2( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  matrix *= matrix2;
  //m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformFlipHorizontal()
{
  kdebug( KDEBUG_INFO, 0, "flipHorizontal" );

  QWMatrix matrix;
  QWMatrix matrix2( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  matrix2.rotate( 180 );
  matrix *= matrix2;
  //m_pDoc->transformImage( matrix );
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
  //m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomIn10()
{
  kdebug( KDEBUG_INFO, 0, "Zoom In 10" );

  QWMatrix matrix;
  matrix.scale( 1.1, 1.1 );
  //m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomOut10()
{
  kdebug( KDEBUG_INFO, 0, "Zoom Out 10" );

  QWMatrix matrix;
  matrix.scale( 0.9, 0.9 );
  //m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomDouble()
{
  kdebug( KDEBUG_INFO, 0, "Zoom Double" );

  QWMatrix matrix;
  matrix.scale( 2.0, 2.0 );
  //m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomHalf()
{
  kdebug( KDEBUG_INFO, 0, "Zoom Half" );

  QWMatrix matrix;
  matrix.scale( 0.5, 0.5 );
  //m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomMax()
{
  kdebug( KDEBUG_INFO, 0, "Zoom Max" );

  //QWMatrix matrix( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  //m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::transformZoomMaxAspect()
{
  kdebug( KDEBUG_INFO, 0, "Zoom Max Aspect" );

  //QWMatrix matrix( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
  //m_pDoc->transformImage( matrix );
  slotUpdateView();
}

void KImageView::helpUsingHelp()
{
  kapp->invokeHTMLHelp( "kimage/kimage.html", QString::null );
}

void KImageView::helpAboutKImage()
{
}

/*
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

