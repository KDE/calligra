/* $Id: $ */

#include "KChartWizard.h"
#include "KChartWizardSelectDataPage.h"
#include "KChartWizardSelectChartTypePage.h"
#include "KChartWizardSetupChartPage.h"
#include "KChartWizardSetupAxesPage.h"
#include <qwidget.h>

KChartWizard::KChartWizard ( QWidget *parent, const char* name, 
							 bool modal, WFlags f ) :
  KWizard( parent, name, modal, f )
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
  wizpage1 = new KWizardPage;
  QWidget* wizwidget1 = new KChartWizardSelectDataPage( this );
  wizpage1->w = wizwidget1;
  wizpage1->title = i18n( "Select data" );
  wizpage1->enabled = true;
  addPage( wizpage1 );

  // Second page: select the major chart type
  wizpage2 = new KWizardPage;
  QWidget* wizwidget2 = new KChartWizardSelectChartTypePage( this );
  wizpage2->w = wizwidget2;
  wizpage2->title = i18n( "Select chart type" );
  wizpage2->enabled = true;
  addPage( wizpage2 );

  // Third page: chart setup
  wizpage3 = new KWizardPage;
  QWidget* wizwidget3 = new KChartWizardSetupChartPage( this );
  wizpage3->w = wizwidget3;
  wizpage3->title = i18n( "Chart setup" );
  wizpage3->enabled = true;
  addPage( wizpage3 );

  // Fourth page: axes setup
  wizpage4 = new KWizardPage;
  QWidget* wizwidget4 = new KChartWizardSetupAxesPage( this );
  wizpage4->w = wizwidget4;
  wizpage4->title = i18n( "Axes setup" );
  wizpage4->enabled = true;
  addPage( wizpage4 );

  resize( 400, 300 );
}


KChartWizard::~KChartWizard()
{
  delete wizpage1;
  delete wizpage2;
  delete wizpage3;
}
