/* $Id$ */

#include "kchartWizardSetupAxesPage.h"
#include "kchart_view.h"
#include "kchartparams.h"

#include <qlabel.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
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
  QLabel *tmpLabel;
  /*
  preview = new kchartWidget( _chart, tmpQFrame );
  preview->show();
  _chart->addAutoUpdate( preview );
  preview->resize( tmpQFrame->contentsRect().width(),
				   tmpQFrame->contentsRect().height() );
  */

  grid = new QCheckBox( i18n( "Has grid" ), this );
  grid->setGeometry( 70, 250, 110, 30 );
  grid->setChecked( _chart->params()->grid );

  tmpLabel=new QLabel(this);
  tmpLabel->setText(i18n("Y interval : "));
  tmpLabel->setGeometry(10,290,50,30);
  QString tmp;
  y_interval=new QLineEdit(this);
  y_interval->setGeometry(70,290,110,30);
  if( _chart->params()->requested_yinterval != -MAXDOUBLE)
        y_interval->setText( tmp.setNum(_chart->params()->requested_yinterval));

  tmpLabel=new QLabel(this);
  tmpLabel->setText(i18n("Y min : "));
  tmpLabel->setGeometry(10,330,50,30);
  y_min=new QLineEdit(this);
  y_min->setGeometry(70,330,110,30);
  if( _chart->params()->requested_ymin != MAXDOUBLE)
        y_min->setText( tmp.setNum(_chart->params()->requested_ymin));

  tmpLabel=new QLabel(this);
  tmpLabel->setText(i18n("Y max : "));
  tmpLabel->setGeometry(10,370,50,30);
  y_max=new QLineEdit(this);
  y_max->setGeometry(70,370,110,30);
  if( _chart->params()->requested_ymax != -MAXDOUBLE)
        y_max->setText( tmp.setNum(_chart->params()->requested_ymax));

  border = new QCheckBox( i18n( "Border" ), this );
  border->setGeometry( 70, 410, 110, 30 );
  border->setChecked( _chart->params()->border );

  tmpLabel=new QLabel(this);
  tmpLabel->setText(i18n("Angle 3D : "));
  tmpLabel->setGeometry(10,440,50,30);
  angle = new QSpinBox(1, 90, 1, this);
  angle->setValue(_chart->params()->_3d_angle);
  angle->setGeometry( 70, 440, 110, 30 );

  /*connect( grid, SIGNAL( toggled( bool ) ),
	   this, SLOT( setGrid( bool ) ) );
    */
  /*
  QGroupBox* ticksettingsGB = new QGroupBox( i18n( "Tick settings" ), this );
  ticksettingsGB->setGeometry( 260, 10, 330, 120 );

  QCheckBox* showfullgridCB = new QCheckBox( i18n( "Show full grid" ), this );
  showfullgridCB->setGeometry( 270, 30, 100, 30 );
  //  showfullgridCB->setChecked( _chart->longTicks() );
  //  connect( showfullgridCB, SIGNAL( toggled( bool ) ),
  //		   _chart, SLOT( setLongTicks( bool ) ) );

  QCheckBox* showxticksCB = new QCheckBox( i18n( "Show ticks on X axis" ),
										   this );
  showxticksCB->setGeometry( 270, 60, 140, 30 );
  //showxticksCB->setChecked( _chart->params()->xTicks() );
  // connect( showxticksCB, SIGNAL( toggled( bool ) ),
  //		   _chart, SLOT( setXTicks( bool ) ) );

  QLabel* ticklengthLA = new QLabel( i18n( "Tick length:" ), this );
  ticklengthLA->setGeometry( 270, 90, 70, 30 );

  QLineEdit* ticklengthED = new QLineEdit( this ); // todo KFloatValidator
  ticklengthED->setGeometry( 340, 90, 40, 30 );
  //ticklengthED->setText( QString().setNum(_chart->tickLength()) );
  */
  /*
  connect( ticklengthED, SIGNAL( textChanged( const QString & ) ),
                   this, SLOT( setTickLength( const QString & ) ) );
    */
    /*
  QLabel* yticksnumLA = new QLabel( i18n( "Number of ticks on Y axis:" ),
									this );
  yticksnumLA->setGeometry( 400, 90, 150, 30 );

  QLineEdit* yticksnumED = new QLineEdit( this ); // todo KFloatValidator
  yticksnumED->setGeometry( 550, 90, 30, 30 );
  */
  //yticksnumED->setText( QString().setNum(_chart->yTicksNum()) );
  /*
  connect( yticksnumED, SIGNAL( textChanged( const QString & ) ),
		   this, SLOT( setYTicksNum( const QString & ) ) );
    */
    /*
  QGroupBox* axeslabelsGB = new QGroupBox( i18n( "Axes labelling" ), this );
  axeslabelsGB->setGeometry( 260, 140, 330, 150 );

  QCheckBox* showxvaluesCB = new QCheckBox( i18n( "Show values on X axis" ),
											this );
  showxvaluesCB->setGeometry( 270, 160, 140, 30 );
  */
  //showxvaluesCB->setChecked( _chart->xPlotValues() );
  /*connect( showxvaluesCB, SIGNAL( toggled( bool ) ),
		   _chart, SLOT( setXPlotValues( bool ) ) );
    */
  /*
  QLabel* showeveryxLA = new QLabel( i18n( "Show every" ), this );
  showeveryxLA->setGeometry( 270, 190, 70, 30 );

  QLineEdit* showeveryxED = new QLineEdit( this ); // todo KFloatValidator
  showeveryxED->setGeometry( 340, 190, 20, 30 );
  */
  //showeveryxED->setText( QString().setNum(_chart->xLabelSkip()) );
  /*
  connect( showeveryxED, SIGNAL( textChanged( const QString & ) ),
		   this, SLOT( setXLabelSkip( const QString & ) ) );
    */
  /*
  QLabel* showeveryx2KA = new QLabel( i18n( ". value on X axis" ), this );
  showeveryx2KA->setGeometry( 360, 190, 100, 30 );

  QCheckBox* showyvaluesCB = new QCheckBox( i18n( "Show values on Y axis" ),
											this );
  showyvaluesCB->setGeometry( 270, 220, 140, 30 );
  */
  //showyvaluesCB->setChecked( _chart->yPlotValues() );
  /*
  connect( showyvaluesCB, SIGNAL( toggled( bool ) ),
		   _chart, SLOT( setYPlotValues( bool ) ) );
    */
    /*
  QLabel* showeveryyLA = new QLabel( i18n( "Show every" ), this );
  showeveryyLA->setGeometry( 270, 250, 70, 30 );

  QLineEdit* showeveryyED = new QLineEdit( this ); // todo KFloatValidator
  showeveryyED->setGeometry( 340, 250, 20, 30 );
  */
  //  showeveryyED->setText( QString().setNum(_chart->yLabelSkip()) );
  /*
  connect( showeveryyED, SIGNAL( textChanged( const QString & ) ),
		   this, SLOT( setYLabelSkip( const QString & ) ) );
   */
  /*
  QLabel* showeveryy2KA = new QLabel( i18n( ". value on Y axis" ), this );
  showeveryy2KA->setGeometry( 360, 250, 100, 30 );
  */
}


kchartWizardSetupAxesPage::~kchartWizardSetupAxesPage()
{
  // _chart->removeAutoUpdate( preview );
}

/*void kchartWizardSetupAxesPage::setGrid( bool value )
{
  _chart->params()->grid = value;
}*/

void kchartWizardSetupAxesPage::apply()
{
 _chart->params()->grid =grid->isChecked() ;
 if( !y_interval->text().isEmpty())
        _chart->params()->requested_yinterval=y_interval->text().toDouble();
 if( !y_max->text().isEmpty())
        _chart->params()->requested_ymax=y_max->text().toDouble();
 if( !y_min->text().isEmpty())
        _chart->params()->requested_ymin=y_min->text().toDouble();
 _chart->params()->border =border->isChecked() ;
 _chart->params()->_3d_angle=angle->value();

}
/*
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

*/
#include "kchartWizardSetupAxesPage.moc"
