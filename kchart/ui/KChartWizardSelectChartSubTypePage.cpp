/* $Id$ */

#include "KChartWizardSelectChartSubTypePage.h"
#include "KChart.h"
#include "KChartTypes.h"

#include <qframe.h>
#include <qpushbt.h>
#include <qbttngrp.h>
#include <qlabel.h>
#include <qobjcoll.h>

#include <kiconloader.h>
#include <klocale.h>

KChartWizardSelectChartSubTypePage::KChartWizardSelectChartSubTypePage( QWidget* parent, KChart* chart ) :
  QWidget( parent ),
  _chart( chart ),
  _no( -1 )
{
  connect( parent, SIGNAL( selected( int ) ),
		   this, SLOT( createChildren( int ) ) );

  _charttype = _chart->chartType();
}


void KChartWizardSelectChartSubTypePage::setNumber( int no )
{
  _no = no;
}


void KChartWizardSelectChartSubTypePage::chartSubTypeSelected( int type )
{
  switch( _chart->chartType() ) {
  case Bars:
	_chart->setOverwrite( (OverwriteMode)type );
	break;
  default: {} // more to come
  };
}


void KChartWizardSelectChartSubTypePage::createChildren( int no )
{
  /*
  if( no != _no )
	return; // that was for another page
  */

  _charttype = _chart->chartType(); // note new chart type

  // PENDING(kalle): The pages should be prepared as QWidgets and just
  // put in as needed to avoid recreating them over and over again. 

  // clear the page
  QObjectList *list = queryList( "QWidget" );
  QObjectListIt it( *list );          // iterate over the child widgets
  QObject * obj;
  while ( (obj=it.current()) != 0 ) { // for each found object...
	++it;
	((QWidget*)obj)->hide();
	removeChild( obj );
  }
  delete list;                        // delete the list, not the objects

  _typeBG = new QButtonGroup( this );
  _typeBG->setExclusive( true );
  _typeBG->hide();


  switch( _chart->chartType() ) {
  case Bars: {
	QFrame* normalbarsFR = new QFrame( this );
	normalbarsFR->show();
	normalbarsFR->setGeometry( 10, 10, 94, 94 );
	normalbarsFR->setLineWidth( 2 );
	_normalbarsPB = new QPushButton( normalbarsFR );
	_normalbarsPB->setToggleButton( true );
	_normalbarsPB->resize( normalbarsFR->contentsRect().width(), 
						   normalbarsFR->contentsRect().height() );
	_normalbarsPB->setOn( true );
	_normalbarsPB->show();
	_typeBG->insert( _normalbarsPB, SideBySide );
	_normalbarsPB->setPixmap( BarIcon( "chart_normalbars") );
	QLabel* normalbarsLA = new QLabel( i18n( "Normal" ), this );
	normalbarsLA->show();
	normalbarsLA->setGeometry( 10, 104, 94, 20 );
	normalbarsLA->setAlignment( AlignCenter );
	
	QFrame* barsontopFR = new QFrame( this );
	barsontopFR->show();
	barsontopFR->setGeometry( 104, 10, 94, 94 );
	barsontopFR->setLineWidth( 2 );
	barsontopFR->setFrameStyle( QFrame::Sunken | QFrame::Panel );
	_barsontopPB = new QPushButton( barsontopFR );
	_barsontopPB->setToggleButton( true );
	_barsontopPB->resize( barsontopFR->contentsRect().width(), 
						  barsontopFR->contentsRect().height() );
	_barsontopPB->show();
	_typeBG->insert( _barsontopPB, OnTop );
	_barsontopPB->setPixmap( BarIcon( "chart_barsontop" ) );
	QLabel* barsontopLA = new QLabel( i18n( "On Top" ), this );
	barsontopLA->setGeometry( 104, 104, 94, 20 );
	barsontopLA->setAlignment( AlignCenter );
	barsontopLA->show();
	
	QFrame* barsinfrontFR = new QFrame( this );
	barsinfrontFR->setGeometry( 198, 10, 94, 94 );
	barsinfrontFR->setLineWidth( 2 );
	barsinfrontFR->setFrameStyle( QFrame::Sunken | QFrame::Panel );
	barsinfrontFR->show();
	_barsinfrontPB = new QPushButton( barsinfrontFR );
	_barsinfrontPB->setToggleButton( true );
	_barsinfrontPB->resize( barsinfrontFR->contentsRect().width(), 
							barsinfrontFR->contentsRect().height() );
	_barsinfrontPB->show();
	_typeBG->insert( _barsinfrontPB, InFront );
	_barsinfrontPB->setPixmap( BarIcon( "chart_barsinfront" ) );
	QLabel* barsinfrontLA = new QLabel( i18n( "In Front" ), this );
	barsinfrontLA->setGeometry( 198, 104, 94, 20 );
	barsinfrontLA->setAlignment( AlignCenter );
	barsinfrontLA->show();

	// switch the correct button on
    ((QPushButton*)_typeBG->find( _chart->overwrite() ))->setOn( true );

	break;
  }  
  default: {
	QLabel* nosubtypesLA = new QLabel( i18n( "There are no subtypes for this chart type" ), this );
	nosubtypesLA->setFont( QFont( "Helvetica", 12, QFont::Bold ) );
	nosubtypesLA->adjustSize();
	nosubtypesLA->move( 10, 10 );
	nosubtypesLA->show();
  }
  }


  connect( _typeBG, SIGNAL( clicked( int ) ),
		   this, SLOT( chartSubTypeSelected( int ) ) );
}



#include "KChartWizardSelectChartSubTypePage.moc"
