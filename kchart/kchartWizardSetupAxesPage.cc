/* $Id$ */

#include "kchartWizardSetupAxesPage.h"
#include "kchart_view.h"

#include <qlabel.h>
#include <qframe.h>
#include <qlined.h>
#include <qchkbox.h>
#include <qgrpbox.h>
#include <klocale.h>

kchartWizardSetupAxesPage::kchartWizardSetupAxesPage( QWidget* parent, 
													  KChartPart* chart ) :
  QWidget( parent ),
  _chart( chart )
{
  QFrame* tmpQFrame;
  tmpQFrame = new QFrame( this, "Frame_2" );
  tmpQFrame->setGeometry( 10, 10, 240, 220 );
  tmpQFrame->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  tmpQFrame->setLineWidth( 2 );

  /*
  preview = new kchartWidget( _chart, tmpQFrame );
  preview->show();
  _chart->addAutoUpdate( preview );
  preview->resize( tmpQFrame->contentsRect().width(),
				   tmpQFrame->contentsRect().height() );
  */

  QCheckBox* usetwoaxesCB = new QCheckBox( i18n( "Use two Y axes" ), this );
  usetwoaxesCB->setGeometry( 70, 250, 110, 30 );

  //  usetwoaxesCB->setChecked( _chart->twoAxes() );

  // two axes mode is only possible if there are exactly two datasets
  /*
  usetwoaxesCB->setEnabled( _chart->chartData()->numDatasets() == 2 );

  connect( usetwoaxesCB, SIGNAL( toggled( bool ) ),
		   _chart, SLOT( setTwoAxes( bool ) ) );

  QGroupBox* ticksettingsGB = new QGroupBox( i18n( "Tick settings" ), this );
  ticksettingsGB->setGeometry( 260, 10, 330, 120 );
  
  QCheckBox* showfullgridCB = new QCheckBox( i18n( "Show full grid" ), this );
  showfullgridCB->setGeometry( 270, 30, 100, 30 );
  showfullgridCB->setChecked( _chart->longTicks() );
  connect( showfullgridCB, SIGNAL( toggled( bool ) ),
		   _chart, SLOT( setLongTicks( bool ) ) );

  QCheckBox* showxticksCB = new QCheckBox( i18n( "Show ticks on X axis" ), 
										   this );
  showxticksCB->setGeometry( 270, 60, 140, 30 );
  showxticksCB->setChecked( _chart->xTicks() );
  connect( showxticksCB, SIGNAL( toggled( bool ) ),
		   _chart, SLOT( setXTicks( bool ) ) );

  QLabel* ticklengthLA = new QLabel( i18n( "Tick length:" ), this );
  ticklengthLA->setGeometry( 270, 90, 70, 30 );
  
  QLineEdit* ticklengthED = new QLineEdit( this ); // todo KFloatValidator
  ticklengthED->setGeometry( 340, 90, 40, 30 );
  ticklengthED->setText( QString().setNum(_chart->tickLength()) );
  connect( ticklengthED, SIGNAL( textChanged( const QString & ) ),
                   this, SLOT( setTickLength( const QString & ) ) );

  QLabel* yticksnumLA = new QLabel( i18n( "Number of ticks on Y axis:" ), 
									this );
  yticksnumLA->setGeometry( 400, 90, 150, 30 );
  
  QLineEdit* yticksnumED = new QLineEdit( this ); // todo KFloatValidator
  yticksnumED->setGeometry( 550, 90, 30, 30 );
  yticksnumED->setText( QString().setNum(_chart->yTicksNum()) );
  connect( yticksnumED, SIGNAL( textChanged( const QString & ) ),
		   this, SLOT( setYTicksNum( const QString & ) ) );

  QGroupBox* axeslabelsGB = new QGroupBox( i18n( "Axes labelling" ), this );
  axeslabelsGB->setGeometry( 260, 140, 330, 150 );

  QCheckBox* showxvaluesCB = new QCheckBox( i18n( "Show values on X axis" ), 
											this );
  showxvaluesCB->setGeometry( 270, 160, 140, 30 );
  showxvaluesCB->setChecked( _chart->xPlotValues() );
  connect( showxvaluesCB, SIGNAL( toggled( bool ) ),
		   _chart, SLOT( setXPlotValues( bool ) ) );

  QLabel* showeveryxLA = new QLabel( i18n( "Show every" ), this );
  showeveryxLA->setGeometry( 270, 190, 70, 30 );

  QLineEdit* showeveryxED = new QLineEdit( this ); // todo KFloatValidator
  showeveryxED->setGeometry( 340, 190, 20, 30 );
  showeveryxED->setText( QString().setNum(_chart->xLabelSkip()) );
  connect( showeveryxED, SIGNAL( textChanged( const QString & ) ),
		   this, SLOT( setXLabelSkip( const QString & ) ) );

  QLabel* showeveryx2KA = new QLabel( i18n( ". value on X axis" ), this );
  showeveryx2KA->setGeometry( 360, 190, 100, 30 );

  QCheckBox* showyvaluesCB = new QCheckBox( i18n( "Show values on Y axis" ),
											this );
  showyvaluesCB->setGeometry( 270, 220, 140, 30 );
  showyvaluesCB->setChecked( _chart->yPlotValues() );
  connect( showyvaluesCB, SIGNAL( toggled( bool ) ),
		   _chart, SLOT( setYPlotValues( bool ) ) );


  QLabel* showeveryyLA = new QLabel( i18n( "Show every" ), this );
  showeveryyLA->setGeometry( 270, 250, 70, 30 );

  QLineEdit* showeveryyED = new QLineEdit( this ); // todo KFloatValidator
  showeveryyED->setGeometry( 340, 250, 20, 30 );
  showeveryyED->setText( QString().setNum(_chart->yLabelSkip()) );
  connect( showeveryyED, SIGNAL( textChanged( const QString & ) ),
		   this, SLOT( setYLabelSkip( const QString & ) ) );

  QLabel* showeveryy2KA = new QLabel( i18n( ". value on Y axis" ), this );
  showeveryy2KA->setGeometry( 360, 250, 100, 30 );
  */
}


kchartWizardSetupAxesPage::~kchartWizardSetupAxesPage()
{
  //  _chart->removeAutoUpdate( preview );
}

void kchartWizardSetupAxesPage::setTickLength( const QString & newValue )
{
  //  _chart->setTickLength( newValue.toInt() );
}

void kchartWizardSetupAxesPage::setYTicksNum( const QString & newValue )
{
  //  _chart->setYTicksNum( newValue.toInt() );
}

void kchartWizardSetupAxesPage::setXLabelSkip( const QString & newValue )
{
  //  _chart->setXLabelSkip( newValue.toInt() );
}

void kchartWizardSetupAxesPage::setYLabelSkip( const QString & newValue ) 
{
  //  _chart->setYLabelSkip( newValue.toInt() );
}
  
#include "kchartWizardSetupAxesPage.moc"
