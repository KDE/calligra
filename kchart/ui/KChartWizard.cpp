/* $Id$ */

#include "KChartWizard.h"
#include "KChartWizardSelectDataPage.h"
#include "KChartWizardSelectChartTypePage.h"
#include "KChartWizardSelectChartSubTypePage.h"
#include "KChartWizardSetupDataPage.h"
#include "KChartWizardLabelsLegendPage.h"
#include "KChartWizardSetupAxesPage.h"

#include <qlineedit.h>
#include <qwidget.h>
#include <qpushbutton.h>

#include <kglobal.h>
#include <kiconloader.h>

KChartWizard::KChartWizard ( KChart* chart, QWidget *parent, const char* name, 
							 bool modal, WFlags f ) :
  KWizard( parent, name, modal, f ),
  _chart( chart )
{
  KGlobal::iconLoader()->setIconType( "icon" );

  // First page: select the range
  _selectdatapage = new KChartWizardSelectDataPage( this );
  addPage( _selectdatapage, i18n( "Select data" ) );

  // Second page: select the major chart type
  _selectcharttypepage =  new KChartWizardSelectChartTypePage( this, _chart );
  addPage( _selectcharttypepage, i18n( "Select chart type" ) );

  // Third page: select the minor chart type
  _selectchartsubtypepage = new KChartWizardSelectChartSubTypePage( this, _chart );
  addPage( _selectchartsubtypepage, i18n( "Select chart subtype" ) );

  // Fourth page: data setup
  _setupdatapage = new KChartWizardSetupDataPage( this, _chart );
  addPage( _setupdatapage, i18n( "Data setup" ) );

  // Fifth page: labels/legends setup
  _labelslegendpage = new KChartWizardLabelsLegendPage( this, _chart );
  addPage( _labelslegendpage, i18n( "Labels and legend" ) );

  // Sixth page: axes setup
  _axespage = new KChartWizardSetupAxesPage( this, _chart );
  addPage( _axespage, i18n( "Setup axes" ) );

  resize( 620, 380 );
}


KChartWizard::~KChartWizard()
{
  delete _selectdatapage;
  delete _selectcharttypepage;
  delete _selectchartsubtypepage;
  delete _setupdatapage;
  delete _labelslegendpage;
  delete _axespage;
}

bool KChartWizard::appropriate( QWidget * w ) const
{
  if ( w == _selectchartsubtypepage )
    // Show the sub-type page only if has anything to show
    return _selectchartsubtypepage->createChildren();
  else
    return true;
}

void KChartWizard::next()
{

  // Some sort of a hack. We want the chart-subtype-page to get 
  // dynamically built when it's going to be shown
  //  if ( currentPage() == _selectcharttypepage )
  //  _selectchartsubtypepage->createChildren();

  QWizard::next();
}

void KChartWizard::accept()
{
  emit finished();
  QWizard::accept();
}

void KChartWizard::reject()
{
  emit cancelled();
  QWizard::reject();
}

void KChartWizard::setDataArea( QString area )
{
  _selectdatapage->rangeED->setText( area );
}


QString KChartWizard::dataArea() const
{
  return _selectdatapage->rangeED->text();
}


void KChartWizard::emitNeedNewData( const char* area, int rowcol, 
									bool firstRowIsLegend, 
									bool firstColIsLabel )
{
  emit needNewData( area, rowcol, firstRowIsLegend, firstColIsLabel );
}



#include "KChartWizard.moc"
