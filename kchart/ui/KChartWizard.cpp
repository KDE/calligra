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
  setCancelButton();
  setOkButton();
  setDefaultButton();
  setHelpButton();
  setEnablePopupMenu(true);
  setEnableArrowButtons(true);
  setDirectionsReflectsPage(true);

  // First page: select the range
  _selectdatapage = new KWizardPage;
  QWidget* selectdatawidget = new KChartWizardSelectDataPage( this );
  _selectdatapage->w = selectdatawidget;
  _selectdatapage->title = i18n( "Select data" );
  _selectdatapage->enabled = true;
  addPage( _selectdatapage );

  // Second page: select the major chart type
  _selectcharttypepage = new KWizardPage;
  QWidget* selectcharttypewidget = new KChartWizardSelectChartTypePage( this,
																		_chart );
  _selectcharttypepage->w = selectcharttypewidget;
  _selectcharttypepage->title = i18n( "Select chart type" );
  _selectcharttypepage->enabled = true;
  addPage( _selectcharttypepage );

  // Third page: select the minor chart type
  _selectchartsubtypepage = new KWizardPage;
  QWidget* selectchartsubtypewidget = new KChartWizardSelectChartSubTypePage( this, _chart );
  _selectchartsubtypepage->w = selectchartsubtypewidget;
  _selectchartsubtypepage->title = i18n( "Select chart subtype" );
  _selectchartsubtypepage->enabled = true;
  addPage( _selectchartsubtypepage );

  // Fourth page: data setup
  _setupdatapage = new KWizardPage;
  QWidget* setupdatawidget = new KChartWizardSetupDataPage( this );
  _setupdatapage->w = setupdatawidget;
  _setupdatapage->title = i18n( "Data setup" );
  _setupdatapage->enabled = true;
  addPage( _setupdatapage );

  // Fifth page: labels/legends setup
  _labelslegendpage = new KWizardPage;
  QWidget* labelslegendwidget = new KChartWizardLabelsLegendPage( this );
  _labelslegendpage->w = labelslegendwidget;
  _labelslegendpage->title = i18n( "Labels and legend" );
  _labelslegendpage->enabled = true;
  addPage( _labelslegendpage );

  // Sixth page: axes setup
  _axespage = new KWizardPage;
  QWidget* axeswidget = new KChartWizardSetupAxesPage( this );
  _axespage->w = axeswidget;
  _axespage->title = i18n( "Setup axes" );
  _axespage->enabled = true;
  addPage( _axespage );

  resize( 400, 300 );
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
  selectDataPage()->rangeED->setText( area );
}


QString KChartWizard::dataArea() const
{
  return selectDataPage()->rangeED->text();
}
