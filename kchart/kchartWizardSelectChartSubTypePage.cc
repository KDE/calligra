/* $Id$ */

#include "kchartWizardSelectChartSubTypePage.h"
#include "kchart_view.h"

#include <qframe.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qcollection.h>
#include <qobjectlist.h>

#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>


kchartWizardSelectChartSubTypePage::kchartWizardSelectChartSubTypePage( QWidget* parent,
						    KChartPart* chart ) :
  QWidget( parent ),
  _chart( chart )
{
    //  _charttype = _chart->chartType();
    chartSubType=true;
    QVBoxLayout *lay1 = new QVBoxLayout( this );
    lay1->setMargin( 5 );
    lay1->setSpacing( 10 );

    QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n( "Chart Sub Type" ),this );
    grp->setRadioButtonExclusive( TRUE );
    grp->layout();
    lay1->addWidget(grp);
    depth=new QRadioButton( i18n("Depth"), grp ); ;
    sum=new QRadioButton( i18n("Sum"), grp );
    beside=new QRadioButton( i18n("Beside"), grp );
    layer=new QRadioButton( i18n("Layer"), grp );
    switch((int)_chart->params()->stack_type)
        {
         case (int)KCHARTSTACKTYPE_DEPTH:
                {
                 depth->setChecked(true);
                 break;
                }
         case (int)KCHARTSTACKTYPE_SUM:
                {
                 sum->setChecked(true);
                 break;
                }
         case (int)KCHARTSTACKTYPE_BESIDE:
                {
                 beside->setChecked(true);
                 break;
                }
          case (int)KCHARTSTACKTYPE_LAYER:
                {
                 layer->setChecked(true);
                 break;
                 }
          default:
                {
                 cout <<"Error in stack_type\n";
                 break;
                }
        }
if(!chartSubType)
        grp->setEnabled(false);
}


void kchartWizardSelectChartSubTypePage::chartSubTypeSelected( int /*type*/ )
{
    /*
  switch( _chart->chartType() ) {
  case Bars:
	_chart->setOverwrite( (OverwriteMode)type );
	break;
  default: {} // more to come
  };
    */
}

bool kchartWizardSelectChartSubTypePage::createChildren( )
{
    //  _charttype = _chart->chartType(); // note new chart type

  // PENDING(kalle): The pages should be prepared as QWidgets and just
  // put in as needed to avoid recreating them over and over again.
  //
  // Not sure if we need to do it (eats more memory). (David)

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

  /*
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
	_normalbarsPB->setPixmap( KGlobal::iconLoader()->loadIcon( "chart_normalbars") );
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
	_barsontopPB->setPixmap( KGlobal::iconLoader()->loadIcon( "chart_barsontop" ) );
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
	_barsinfrontPB->setPixmap( KGlobal::iconLoader()->loadIcon( "chart_barsinfront" ) );
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
        return false;
  }
  }
  */


  connect( _typeBG, SIGNAL( clicked( int ) ),
		   this, SLOT( chartSubTypeSelected( int ) ) );
}

void kchartWizardSelectChartSubTypePage::apply()
{
if(chartSubType)
{
 if(depth->isChecked())
        {
        _chart->params()->stack_type = KCHARTSTACKTYPE_DEPTH;
        }
 else if(sum->isChecked())
        {
        _chart->params()->stack_type = KCHARTSTACKTYPE_SUM;
        }
 else if(beside->isChecked())
        {
        _chart->params()->stack_type = KCHARTSTACKTYPE_BESIDE;
        }
 else if(layer->isChecked())
        {
        _chart->params()->stack_type = KCHARTSTACKTYPE_LAYER;
        }
 else
        {
        cout <<"Error in groupbutton\n";
        }
 }
else
{
_chart->params()->stack_type = KCHARTSTACKTYPE_DEPTH;
}
}


#include "kchartWizardSelectChartSubTypePage.moc"
