/* $Id$ */

#include "KChartWizardLabelsLegendPage.h"
//#include "KChartTypes.h"
//#include "KChartWidget.h"
#include "koChart.h"

#include <qbttngrp.h>
#include <qlined.h>
#include <qradiobt.h>
#include <qlabel.h>

#include <klocale.h>

KChartWizardLabelsLegendPage::KChartWizardLabelsLegendPage( QWidget* parent, KChart* chart ) :
  QWidget( parent ),
  _chart( chart )
{
  /*
  QLabel* xlabelLA = new QLabel( i18n( "X-Label:" ), this );
  xlabelLA->setGeometry( 270, 10, 100, 30 );

  _xlabelED = new QLineEdit( this );
  _xlabelED->setGeometry( 270, 40, 100, 30 );
  connect( _xlabelED, SIGNAL( textChanged( const QString & ) ),
		   _chart, SLOT( setXLabel( const QString & ) ) );
  
  QLabel* ylabelLA = new QLabel( i18n( "Y-Label:" ), this );
  ylabelLA->setGeometry( 270, 90, 100, 30 );

  _ylabelED = new QLineEdit( this );
  _ylabelED->setGeometry( 270, 120, 100, 30 );
  connect( _ylabelED, SIGNAL( textChanged( const QString & ) ),
		   _chart, SLOT( setYLabel( const QString & ) ) );
							 
  QLabel* titleLA = new QLabel( i18n( "Title:" ), this );
  titleLA->setGeometry( 270, 170, 100, 30 );
  
  _titleED = new QLineEdit( this );
  _titleED->setGeometry( 270, 200, 100, 30 );
  connect( _titleED, SIGNAL( textChanged( const QString & ) ),
		   _chart, SLOT( setTitle( const QString & ) ) );
  
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

  connect( placementBG, SIGNAL( clicked( int ) ),
		   _chart, SLOT( setLegendPlacement( int ) ) );

  QFrame* tmpQFrame;
  tmpQFrame = new QFrame( this, "Frame_1" );
  tmpQFrame->setGeometry( 10, 10, 240, 220 );
  tmpQFrame->setFrameStyle( QFrame::Sunken | QFrame::Panel );
  tmpQFrame->setLineWidth( 2 );

  preview = new KChartWidget( _chart, tmpQFrame );
  preview->show();
  _chart->addAutoUpdate( preview );
  preview->resize( tmpQFrame->contentsRect().width(),
				   tmpQFrame->contentsRect().height() );
   
  resize( 600, 300 );
  */
}


KChartWizardLabelsLegendPage::~KChartWizardLabelsLegendPage()
{
  //  _chart->removeAutoUpdate( preview );
}

#include "KChartWizardLabelsLegendPage.moc"
