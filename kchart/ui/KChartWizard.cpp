/* $Id$ */

#include "KChartWizard.h"
#include "KChartWizardSelectDataPage.h"
#include "KChartWizardSelectChartTypePage.h"
#include "KChartWizardSelectChartSubTypePage.h"
#include "KChartWizardSetupDataPage.h"
#include "KChartWizardLabelsLegendPage.h"
#include "KChartWizardSetupAxesPage.h"

#include <qlined.h>
#include <qwidget.h>

KChartWizard::KChartWizard ( KChart* chart, QWidget *parent, const char* name, 
							 bool modal, WFlags f ) :
  KWizard( parent, name, modal, f ),
  _chart( chart )
{
  // General setup
  /*
  setCancelButton();
  setOkButton();
  setDefaultButton();
  setHelpButton();
  setEnablePopupMenu(true);
  setEnableArrowButtons(true);
  setDirectionsReflectsPage(true);
  */

  // First page: select the range
  _selectdatapage = new KChartWizardSelectDataPage( this );
  addPage( _selectdatapage, i18n( "Select data" ) );

  // Second page: select the major chart type
  _selectcharttypepage =  new KChartWizardSelectChartTypePage( this, _chart );
  addPage( _selectcharttypepage, i18n( "Select chart type" ) );

  // Third page: select the minor chart type
  _selectchartsubtypepage = new KChartWizardSelectChartSubTypePage( this, _chart );
  addPage( _selectchartsubtypepage, i18n( "Select chart subtype" ) );
  //((KChartWizardSelectChartSubTypePage*)selectchartsubtypewidget)->setNumber( page );

  // Fourth page: data setup
  _setupdatapage = new KChartWizardSetupDataPage( this, _chart );
  addPage( _setupdatapage, i18n( "Data setup" ) );

  // Fifth page: labels/legends setup
  _labelslegendpage = new KChartWizardLabelsLegendPage( this, _chart );
  addPage( _labelslegendpage, i18n( "Labels and legend" ) );

  // Sixth page: axes setup
  _axespage = new KChartWizardSetupAxesPage( this, _chart );
  addPage( _axespage, i18n( "Setup axes" ) );

  resize( 400, 500 );
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


void KChartWizard::setDataArea( QString area )
{
  ((KChartWizardSelectDataPage*)_selectdatapage)->rangeED->setText( area );
}


QString KChartWizard::dataArea() const
{
  return ((KChartWizardSelectDataPage*)_selectdatapage)->rangeED->text();
}


void KChartWizard::emitNeedNewData( const char* area, int rowcol, 
									bool firstRowIsLegend, 
									bool firstColIsLabel )
{
  emit needNewData( area, rowcol, firstRowIsLegend, firstColIsLabel );
}



#include "KChartWizard.moc"
