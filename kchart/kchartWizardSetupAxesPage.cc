/* $Id$ */

#include "kchartWizardSetupAxesPage.h"
#include "kchart_view.h"

#include <qlabel.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <klocale.h>
#include <kfontdialog.h>
#include <math.h>
#include <float.h>

#include <qspinbox.h>
#include <qpushbutton.h>
#include <qcolor.h>

#include "kdchart/KDChartParams.h"

KChartWizardSetupAxesPage::KChartWizardSetupAxesPage( QWidget* parent,
                                                      KChartPart* chart ) :
  QWidget( parent ),
  _chart( chart )
{
    qDebug( "Sorry, not implemented: KChartWizardSetupAxesPage::KChartWizardSetupAxesPage()" );
#warning Put back in
#ifdef K
  chart3d=true;
  /*QFrame* tmpQFrame;
    tmpQFrame = new QFrame( this, "Frame_2" );
    tmpQFrame->setGeometry( 10, 10, 240, 220 );
    tmpQFrame->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    tmpQFrame->setLineWidth( 2 );*/

  /*
    preview = new kchartWidget( _chart, tmpQFrame );
    preview->show();
    _chart->addAutoUpdate( preview );
    preview->resize( tmpQFrame->contentsRect().width(),
    tmpQFrame->contentsRect().height() );
  */

  QGridLayout *grid1 = new QGridLayout(this,2,2,15,15);

  QGroupBox* tmpQGroupBox;
  tmpQGroupBox = new QGroupBox( this, "GroupBox_1" );
  tmpQGroupBox->setFrameStyle( 49 );

  QGridLayout *grid2 = new QGridLayout(tmpQGroupBox,2,2,15,7);

  grid = new QCheckBox( i18n( "Has grid" ),tmpQGroupBox  );
  grid->setChecked( _chart->params()->grid );
  grid2->addWidget(grid,0,0);

  gridColor=new KColorButton(tmpQGroupBox);
  colorGrid=_chart->params()->GridColor;
  gridColor->setColor( colorGrid );
  grid2->addWidget(gridColor,0,1);

  border = new QCheckBox( i18n( "Border" ), tmpQGroupBox );
  border->setChecked( _chart->params()->border );
  grid2->addWidget(border,1,0);

  borderColor=new KColorButton(tmpQGroupBox);
  colorBorder=_chart->params()->LineColor;
  borderColor->setColor( colorBorder );
  grid2->addWidget(borderColor,1,1);

  grid1->addWidget(tmpQGroupBox,0,0);


  tmpQGroupBox = new QGroupBox( this, "GroupBox_2" );
  tmpQGroupBox->setFrameStyle( 49 );

  grid2 = new QGridLayout(tmpQGroupBox,3,2,15,7);

  QLabel *tmpLabel;
  tmpLabel=new QLabel(tmpQGroupBox);
  tmpLabel->setText(i18n("Angle 3D : "));
  grid2->addWidget(tmpLabel,0,0);
  angle = new QSpinBox(1, 90, 1,tmpQGroupBox );
  angle->setValue(_chart->params()->_3d_angle);
  grid2->addWidget(angle,0,1);

  tmpLabel=new QLabel(tmpQGroupBox);
  tmpLabel->setText(i18n("Depth 3D : "));
  grid2->addWidget(tmpLabel,1,0);
  depth = new QSpinBox(1, 20, 1, tmpQGroupBox);
  depth->setValue(static_cast<int>(_chart->params()->_3d_depth));  // #### FIXME
  grid2->addWidget(depth,1,1);

  tmpLabel=new QLabel(tmpQGroupBox);
  tmpLabel->setText(i18n("bar width : "));
  grid2->addWidget(tmpLabel,2,0);
  barWidth = new QSpinBox(1, 200, 1, tmpQGroupBox);
  barWidth->setValue(_chart->params()->bar_width);
  grid2->addWidget(barWidth,2,1);

  grid1->addWidget(tmpQGroupBox,0,1);


  tmpQGroupBox = new QGroupBox( this, "GroupBox_3" );
  tmpQGroupBox->setFrameStyle( 49 );

  grid2 = new QGridLayout(tmpQGroupBox,3,2,15,7);

  tmpLabel=new QLabel(tmpQGroupBox);
  tmpLabel->setText(i18n("Y interval : "));
  grid2->addWidget(tmpLabel,0,0);

  QString tmp;
  y_interval=new QLineEdit(tmpQGroupBox);
  grid2->addWidget(y_interval,0,1);
  if( _chart->params()->requested_yinterval != -DBL_MAX)
    y_interval->setText( tmp.setNum(_chart->params()->requested_yinterval));

  tmpLabel=new QLabel(tmpQGroupBox);
  tmpLabel->setText(i18n("Y min : "));
  grid2->addWidget(tmpLabel,1,0);
  y_min=new QLineEdit(tmpQGroupBox);
  grid2->addWidget(y_min,1,1);
  if( _chart->params()->requested_ymin != DBL_MAX)
    y_min->setText( tmp.setNum(_chart->params()->requested_ymin));

  tmpLabel=new QLabel(tmpQGroupBox);
  tmpLabel->setText(i18n("Y max : "));
  grid2->addWidget(tmpLabel,2,0);
  y_max=new QLineEdit(tmpQGroupBox);
  grid2->addWidget(y_max,2,1);
  if( _chart->params()->requested_ymax != -DBL_MAX)
    y_max->setText( tmp.setNum(_chart->params()->requested_ymax));

  grid1->addWidget(tmpQGroupBox,1,0);

  tmpQGroupBox = new QGroupBox( this, "GroupBox_3" );
  tmpQGroupBox->setFrameStyle( 49 );

  grid2 = new QGridLayout(tmpQGroupBox,3,2,15,7);

  tmpLabel=new QLabel(tmpQGroupBox);
  tmpLabel->setText(i18n("YLabel format : "));
  grid2->addWidget(tmpLabel,0,0);
  ylabel_fmt=new QLineEdit(tmpQGroupBox);
  grid2->addWidget(ylabel_fmt,0,1);

  if( !_chart->params()->ylabel_fmt.isEmpty()) {
    int len=_chart->params()->ylabel_fmt.length();
    ylabel_fmt->setText(_chart->params()->ylabel_fmt.right(len-3));
  }
  ylabelFont = new QPushButton( tmpQGroupBox);
  grid2->addWidget(ylabelFont,1,0);
  ylabelFont->setText(i18n("Font"));
  ylabel=_chart->params()->yAxisFont();

  ylabelColor=new KColorButton(tmpQGroupBox);
  grid2->addWidget(ylabelColor,1,1);
  ycolor=_chart->params()->YLabelColor;
  ylabelColor->setColor( ycolor );


  tmpLabel=new QLabel(tmpQGroupBox);
  tmpLabel->setText(i18n("YLabel2 format : "));
  grid2->addWidget(tmpLabel,2,0);

  ylabel2_fmt=new QLineEdit(tmpQGroupBox);
  grid2->addWidget(ylabel2_fmt,2,1);
  if( !_chart->params()->ylabel2_fmt.isEmpty()) {
    int len=_chart->params()->ylabel2_fmt.length();
    ylabel2_fmt->setText(_chart->params()->ylabel2_fmt.right(len-3));
  }

  grid1->addWidget(tmpQGroupBox,1,1);



  connect(ylabelFont,SIGNAL(clicked()),this,SLOT(changeLabelFont()));


  connect(ylabelColor,SIGNAL(changed( const QColor & )),
	  this,SLOT(changeLabelColor(const QColor &)));
  connect(borderColor,SIGNAL(changed( const QColor & )),
	  this,SLOT(changeBorderColor(const QColor &)));
  connect(gridColor,SIGNAL(changed( const QColor & )),
	  this,SLOT(changeGridColor(const QColor &)));
#endif
}


KChartWizardSetupAxesPage::~KChartWizardSetupAxesPage()
{
  // _chart->removeAutoUpdate( preview );
}

void KChartWizardSetupAxesPage::changeLabelColor(const QColor &_color)
{
  ycolor=_color;
}

void KChartWizardSetupAxesPage::changeBorderColor(const QColor &_color)
{
  colorBorder=_color;
}

void KChartWizardSetupAxesPage::changeGridColor(const QColor &_color)
{
  colorGrid=_color;
}

void KChartWizardSetupAxesPage::changeLabelFont()
{
  if( KFontDialog::getFont( ylabel,false,this ) == QDialog::Rejected )
    return;
}

void KChartWizardSetupAxesPage::paintEvent( QPaintEvent *)
{
  if(chart3d) {
    angle->setEnabled(true);
    depth->setEnabled(true);
    barWidth->setEnabled(true);
  } else {
    angle->setEnabled(false);
    depth->setEnabled(false);
    barWidth->setEnabled(false);
  }
}

void KChartWizardSetupAxesPage::apply()
{
    qDebug( "Sorry, not implemented: KChartWizardSetupAxesPage::apply()" );
#warning Put back in
#ifdef K
  _chart->params()->grid =grid->isChecked() ;
  if( !y_interval->text().isEmpty())
    _chart->params()->requested_yinterval=y_interval->text().toDouble();
  else
    _chart->params()->requested_yinterval=0;
  if( !y_max->text().isEmpty())
    _chart->params()->requested_ymax=y_max->text().toDouble();
  else
    _chart->params()->requested_ymax=0;
  if( !y_min->text().isEmpty())
    _chart->params()->requested_ymin=y_min->text().toDouble();
  else
    _chart->params()->requested_ymin=0;

  _chart->params()->border =border->isChecked() ;
  _chart->params()->_3d_angle=angle->value();
  if(! ylabel_fmt->text().isEmpty()) {
    QString tmp="%g "+ylabel_fmt->text();
    _chart->params()->ylabel_fmt=tmp;
  } else {
    _chart->params()->ylabel_fmt="";
  }
  _chart->params()->setYAxisFont(ylabel);
  _chart->params()->YLabelColor=ycolor;
  _chart->params()->GridColor=colorGrid;
  _chart->params()->LineColor=colorBorder;
  _chart->params()->_3d_depth=depth->value();
  _chart->params()->bar_width=barWidth->value();
  if(! ylabel2_fmt->text().isEmpty()) {
    QString tmp="%g "+ylabel2_fmt->text();
    _chart->params()->ylabel2_fmt=tmp;
  } else {
    _chart->params()->ylabel2_fmt="";
  }
#endif
}



#include "kchartWizardSetupAxesPage.moc"
