/* $Id$ */

#include "kchartWizard.h"
#include "kchartWizardSelectDataPage.h"
#include "kchartWizardSelectChartTypePage.h"
#include "kchartWizardSelectChartSubTypePage.h"
#include "kchartWizardSetupDataPage.h"
#include "kchartWizardLabelsLegendPage.h"
#include "kchartWizardSetupAxesPage.h"

#include <qlineedit.h>
#include <qwidget.h>
#include <qpushbutton.h>

#include <kglobal.h>
#include <kiconloader.h>

kchartWizard::kchartWizard ( KChartPart* chart, QWidget *parent, const char* name,
							 bool modal, WFlags f ) :
  KWizard( parent, name, modal, f ),
  _chart( chart )
{
  // hack

  cerr << "Creating kchartWizard\n";
  KGlobal::iconLoader()->setIconType( "icon" );

  // First page: select the range
    //_selectdatapage = new kchartWizardSelectDataPage( this );
   //addPage( _selectdatapage, i18n( "Select data" ) );

  // Second page: select the major chart type
  _selectcharttypepage =  new kchartWizardSelectChartTypePage( this, _chart );
  addPage( _selectcharttypepage, i18n( "Select chart type" ) );
  //finishButton()->setEnabled( TRUE );
  //setFinishEnabled(_selectcharttypepage, true);

  // Third page: select the minor chart type
  //  _selectchartsubtypepage = new kchartWizardSelectChartSubTypePage( this, _chart );
  // addPage( _selectchartsubtypepage, i18n( "Select chart subtype" ) );

  // Fourth page: data setup
  //  _setupdatapage = new kchartWizardSetupDataPage( this, _chart );
  //addPage( _setupdatapage, i18n( "Data setup" ) );

  // Fifth page: labels/legends setup
  _labelslegendpage = new kchartWizardLabelsLegendPage( this, _chart );
  addPage( _labelslegendpage, i18n( "Labels and legend" ) );

  // Sixth page: axes setup
  _axespage = new kchartWizardSetupAxesPage( this, _chart );
  addPage( _axespage, i18n( "Setup axes" ) );
  setFinishEnabled(_axespage, true);
  connect( this ,SIGNAL( finished()),_labelslegendpage,SLOT(apply()));
  connect( this ,SIGNAL( finished()),_selectcharttypepage,SLOT(apply()));
  connect( this ,SIGNAL( finished()),_axespage,SLOT(apply()));
  //resize( 620, 380 );
  cerr << "kchartwizard created\n";
}


kchartWizard::~kchartWizard()
{
    //delete _selectdatapage;
  delete _selectcharttypepage;
  // delete _selectchartsubtypepage;
  //delete _setupdatapage;
  delete _labelslegendpage;
  delete _axespage;
}

bool kchartWizard::appropriate( QWidget * w ) const
{
  if ( w == _selectchartsubtypepage )
    // Show the sub-type page only if has anything to show
    return _selectchartsubtypepage->createChildren();
  else
    return true;
}

void kchartWizard::next()
{

  // Some sort of a hack. We want the chart-subtype-page to get
  // dynamically built when it's going to be shown
  //  if ( currentPage() == _selectcharttypepage )
  //  _selectchartsubtypepage->createChildren();

  QWizard::next();
}

void kchartWizard::accept()
{
  emit finished();
  QWizard::accept();
}

void kchartWizard::reject()
{
  emit cancelled();
  QWizard::reject();
}

void kchartWizard::setDataArea( QString area )
{
  _selectdatapage->rangeED->setText( area );
}


QString kchartWizard::dataArea() const
{
  return _selectdatapage->rangeED->text();
}


void kchartWizard::emitNeedNewData( const char* area, int rowcol,
				    bool firstRowIsLegend,
				    bool firstColIsLabel )
{
  emit needNewData( area, rowcol, firstRowIsLegend, firstColIsLabel );
}



#include "kchartWizard.moc"
