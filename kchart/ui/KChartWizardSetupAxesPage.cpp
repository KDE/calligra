/* $Id: $ */

#include "KChartWizardSetupAxesPage.h"

#include <qradiobt.h>
#include <qbttngrp.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlined.h>

KChartWizardSetupAxesPage::KChartWizardSetupAxesPage( QWidget* parent ) :
  QWidget( parent )
{
  QButtonGroup* tmpQGroupBox;
  tmpQGroupBox = new QButtonGroup( this, "GroupBox_2" );
  tmpQGroupBox->setGeometry( 260, 10, 140, 80 );
  tmpQGroupBox->setFrameStyle( 49 );
  tmpQGroupBox->setAlignment( 1 );
  
  QFrame* tmpQFrame;
  tmpQFrame = new QFrame( this, "Frame_2" );
  tmpQFrame->setGeometry( 10, 10, 240, 220 );
  {
	QColorGroup normal( QColor( QRgb(0) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( QColor( QRgb(8421504) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( QColor( QRgb(0) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	tmpQFrame->setPalette( palette );
  }
  tmpQFrame->setFrameStyle( 50 );
  tmpQFrame->setLineWidth( 2 );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this, "Label_6" );
  tmpQLabel->setGeometry( 270, 20, 120, 20 );
  tmpQLabel->setText( "Display Description" );
  
  descriptYes = new QRadioButton( this, "RadioButton_6" );
  descriptYes->setGeometry( 270, 40, 100, 20 );
  descriptYes->setText( "Yes" );
  tmpQGroupBox->insert( descriptYes );
  
  descriptNo = new QRadioButton( this, "RadioButton_7" );
  descriptNo->setGeometry( 270, 60, 90, 20 );
  descriptNo->setText( "No" );
  tmpQGroupBox->insert( descriptNo );
  
  tmpQLabel = new QLabel( this, "Label_7" );
  tmpQLabel->setGeometry( 260, 100, 100, 20 );
  tmpQLabel->setText( "Title" );
  
  title = new QLineEdit( this, "LineEdit_2" );
  title->setGeometry( 260, 120, 160, 30 );
  title->setText( "" );

  tmpQLabel = new QLabel( this, "Label_8" );
  tmpQLabel->setGeometry( 260, 160, 40, 30 );
  tmpQLabel->setText( "X-Axis" );

  tmpQLabel = new QLabel( this, "Label_9" );
  tmpQLabel->setGeometry( 260, 200, 40, 30 );
  tmpQLabel->setText( "Y-Axis" );

  xaxis = new QLineEdit( this, "LineEdit_3" );
  xaxis->setGeometry( 310, 160, 110, 30 );
  xaxis->setText( "" );

  yaxis = new QLineEdit( this, "LineEdit_4" );
  yaxis->setGeometry( 310, 200, 110, 30 );
  yaxis->setText( "" );

  parent->resize( 430, 250 );
}


