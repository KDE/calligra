/* $Id$ */

#include "KChartWizardLabelsLegendPage.h"
#include "KChartTypes.h"
#include "KChart.h"

#include <qbttngrp.h>
#include <qlined.h>
#include <qradiobt.h>
#include <qlabel.h>

#include <kapp.h>

KChartWizardLabelsLegendPage::KChartWizardLabelsLegendPage( QWidget* parent, KChart* chart ) :
  QWidget( parent ),
  _chart( chart )
{
  QLabel* xlabelLA = new QLabel( i18n( "X-Label:" ), this );
  xlabelLA->setGeometry( 270, 10, 100, 30 );

  _xlabelED = new QLineEdit( this );
  _xlabelED->setGeometry( 270, 40, 100, 30 );
  connect( _xlabelED, SIGNAL( textChanged( const char* ) ),
		   _chart, SLOT( setXLabel( const char* ) ) );
  
  QLabel* ylabelLA = new QLabel( i18n( "Y-Label:" ), this );
  ylabelLA->setGeometry( 270, 90, 100, 30 );

  _ylabelED = new QLineEdit( this );
  _ylabelED->setGeometry( 270, 120, 100, 30 );
  connect( _ylabelED, SIGNAL( textChanged( const char* ) ),
		   _chart, SLOT( setYLabel( const char* ) ) );
							 
  QLabel* titleLA = new QLabel( i18n( "Title:" ), this );
  titleLA->setGeometry( 270, 170, 100, 30 );
  
  _titleED = new QLineEdit( this );
  _titleED->setGeometry( 270, 200, 100, 30 );
  connect( _titleED, SIGNAL( textChanged( const char* ) ),
		   _chart, SLOT( setTitle( const char* ) ) );
  
  QButtonGroup* placementBG = new QButtonGroup( i18n( "Legend placement" ), 
												this );
  placementBG->setGeometry( 390, 10, 140, 210 );
  
  _righttopRB = new QRadioButton( i18n( "Right/Top" ), this );
  _righttopRB->setGeometry( 410, 30, 100, 30 );
  placementBG->insert( _righttopRB, RightTop );

  _rightcenterRB = new QRadioButton( i18n( "Right/Center" ), this );
  _rightcenterRB->setGeometry( 410, 60, 100, 30 );
  placementBG->insert( _rightcenterRB, RightCenter );

  _rightbottomRB = new QRadioButton( i18n( "Right/Bottom" ), this );
  _rightbottomRB->setGeometry( 410, 90, 100, 30 );
  placementBG->insert( _rightbottomRB, RightBottom );

  _bottomleftRB = new QRadioButton( i18n( "Bottom/Left" ), this );
  _bottomleftRB->setGeometry( 410, 120, 100, 30 );
  placementBG->insert( _bottomleftRB, BottomLeft );

  _bottomcenterRB = new QRadioButton( i18n( "Bottom/Center" ), this );
  _bottomcenterRB->setGeometry( 410, 150, 100, 30 );
  placementBG->insert( _bottomcenterRB, BottomCenter );

  _bottomrightRB = new QRadioButton( i18n( "Bottom/Right" ), this );
  _bottomrightRB->setGeometry( 410, 180, 100, 30 );
  placementBG->insert( _bottomrightRB, BottomRight );

  // initialize the correct button
  ((QRadioButton*)placementBG->find( _chart->legendPlacement() ))->setChecked( true );

  QFrame* tmpQFrame;
  tmpQFrame = new QFrame( this, "Frame_1" );
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
   
  connect( placementBG, SIGNAL( clicked( int ) ),
		   _chart, SLOT( setLegendPlacement( int ) ) );

  resize( 600, 300 );
}


#include "KChartWizardLabelsLegendPage.moc"


