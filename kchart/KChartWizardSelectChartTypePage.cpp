/* $Id$ */

#include "KChartWizardSelectChartTypePage.h"
//#include "KChartTypes.h"
#include "koChart.h"

#include <qbttngrp.h>
#include <qframe.h>
#include <qpushbt.h>
#include <qlabel.h>

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>

KChartWizardSelectChartTypePage::KChartWizardSelectChartTypePage( QWidget* parent, KChart* chart ) :
  QWidget( parent ),
  _chart( chart )
{
  _typeBG = new QButtonGroup( this );
  _typeBG->setExclusive( true );
  _typeBG->hide();

  QFrame* barsFR = new QFrame( this );
  barsFR->setGeometry( 10, 10, 94, 94 );
  barsFR->setLineWidth( 2 );
  _barsPB = new QPushButton( barsFR );
  _barsPB->setToggleButton( true );
  _barsPB->resize( barsFR->contentsRect().width(), 
				  barsFR->contentsRect().height() );
  _typeBG->insert( _barsPB, Bars );
  _barsPB->setPixmap( KGlobal::iconLoader()->loadIcon( "chart_bars" ) );
  QLabel* barsLA = new QLabel( i18n( "Bars" ), this );
  barsLA->setGeometry( 10, 104, 94, 20 );
  barsLA->setAlignment( AlignCenter );

  QFrame* linesFR = new QFrame( this );
  linesFR->setGeometry( 104, 10, 94, 94 );
  linesFR->setLineWidth( 2 );
  linesFR->setFrameStyle( QFrame::Sunken | QFrame::Panel );
  _linesPB = new QPushButton( linesFR );
  _linesPB->setToggleButton( true );
  _linesPB->resize( linesFR->contentsRect().width(), 
				  linesFR->contentsRect().height() );
  _typeBG->insert( _linesPB, Lines );
  _linesPB->setPixmap( KGlobal::iconLoader()->loadIcon( "chart_lines") );
  QLabel* linesLA = new QLabel( i18n( "Lines" ), this );
  linesLA->setGeometry( 104, 104, 94, 20 );
  linesLA->setAlignment( AlignCenter );

  QFrame* pointsFR = new QFrame( this );
  pointsFR->setGeometry( 198, 10, 94, 94 );
  pointsFR->setLineWidth( 2 );
  pointsFR->setFrameStyle( QFrame::Sunken | QFrame::Panel );
  _pointsPB = new QPushButton( pointsFR );
  _pointsPB->setToggleButton( true );
  _pointsPB->resize( pointsFR->contentsRect().width(), 
				  pointsFR->contentsRect().height() );
  _typeBG->insert( _pointsPB, Points );
  _pointsPB->setPixmap( KGlobal::iconLoader()->loadIcon( "chart_points") );
  QLabel* pointsLA = new QLabel( i18n( "Points" ), this );
  pointsLA->setGeometry( 198, 104, 94, 20 );
  pointsLA->setAlignment( AlignCenter );

  QFrame* linespointsFR = new QFrame( this );
  linespointsFR->setGeometry( 292, 10, 94, 94 );
  linespointsFR->setLineWidth( 2 );
  linespointsFR->setFrameStyle( QFrame::Sunken | QFrame::Panel );
  _linespointsPB = new QPushButton( linespointsFR );
  _linespointsPB->setToggleButton( true );
  _linespointsPB->resize( linespointsFR->contentsRect().width(), 
				  linespointsFR->contentsRect().height() );
  _typeBG->insert( _linespointsPB, LinesPoints );
  _linespointsPB->setPixmap( KGlobal::iconLoader()->loadIcon( "chart_linespoints") );
  QLabel* linespointsLA = new QLabel( i18n( "Lines and Points" ), this );
  linespointsLA->setGeometry( 292, 104, 94, 20 );
  linespointsLA->setAlignment( AlignCenter );

  QFrame* areaFR = new QFrame( this );
  areaFR->setGeometry( 10, 124, 94, 94 );
  areaFR->setLineWidth( 2 );
  areaFR->setFrameStyle( QFrame::Sunken | QFrame::Panel );
  _areaPB = new QPushButton( areaFR );
  _areaPB->setToggleButton( true );
  _areaPB->resize( areaFR->contentsRect().width(), 
				  areaFR->contentsRect().height() );
  _typeBG->insert( _areaPB , Area);
  _areaPB->setPixmap( KGlobal::iconLoader()->loadIcon( "chart_area") );
  QLabel* areaLA = new QLabel( i18n( "Areas" ), this );
  areaLA->setGeometry( 10, 218, 94, 20 );
  areaLA->setAlignment( AlignCenter );

  QFrame* pieFR = new QFrame( this );
  pieFR->setGeometry( 104, 124, 94, 94 );
  pieFR->setLineWidth( 2 );
  pieFR->setFrameStyle( QFrame::Sunken | QFrame::Panel );
  _piePB = new QPushButton( pieFR );
  _piePB->setToggleButton( true );
  _piePB->resize( pieFR->contentsRect().width(), 
				  pieFR->contentsRect().height() );
  _typeBG->insert( _piePB, Pie );
  _piePB->setPixmap( KGlobal::iconLoader()->loadIcon( "chart_pie") );
  QLabel* pieLA = new QLabel( i18n( "Pie" ), this );
  pieLA->setGeometry( 104, 218, 94, 20 );
  pieLA->setAlignment( AlignCenter );

  QFrame* pie3DFR = new QFrame( this );
  pie3DFR->setGeometry( 198, 124, 94, 94 );
  pie3DFR->setLineWidth( 2 );
  pie3DFR->setFrameStyle( QFrame::Sunken | QFrame::Panel );
  _pie3DPB = new QPushButton( pie3DFR );
  _pie3DPB->setToggleButton( true );
  _pie3DPB->resize( pie3DFR->contentsRect().width(), 
				  pie3DFR->contentsRect().height() );
  _typeBG->insert( _pie3DPB, Pie3D );
  _pie3DPB->setPixmap( KGlobal::iconLoader()->loadIcon( "chart_pie3D") );
  QLabel* pie3DLA = new QLabel( i18n( "3D Pie" ), this );
  pie3DLA->setGeometry( 198, 218, 94, 20 );
  pie3DLA->setAlignment( AlignCenter );

  // switch the correct button on
  //  ((QPushButton*)_typeBG->find( _chart->chartType() ))->setOn( true );

  connect( _typeBG, SIGNAL( clicked( int ) ),
		   this, SLOT( chartTypeSelected( int ) ) );
	
  parent->resize( 425, 256 );
}


void KChartWizardSelectChartTypePage::chartTypeSelected( int type )
{
  //  _chart->setChartType( (KChartType)type );
}


#include "KChartWizardSelectChartTypePage.moc"
