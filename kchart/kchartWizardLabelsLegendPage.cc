/* $Id$ */

#include "kchartWizardLabelsLegendPage.h"
#include "kchart_view.h"

#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include "kchartparams.h"
#include <klocale.h>
#include <kfontdialog.h>

kchartWizardLabelsLegendPage::kchartWizardLabelsLegendPage( QWidget* parent, KChartPart* chart ) :
  QWidget( parent ),
  _chart( chart )
{
  ytitle2=true;
  QLabel* titleLA = new QLabel( i18n( "Title:" ), this );
  titleLA->setGeometry( 270, 10, 100, 30 );

  _titleED = new QLineEdit( this );
  _titleED->setGeometry( 270, 40, 100, 30 );
  _titleED->setText(_chart->params()->title);

  titlefont = new QPushButton( this);
  titlefont->setGeometry( 380,40,80,30 );
  titlefont->setText(i18n("Font"));

  titlecolor=new KColorButton(this);
  titlecolor->setGeometry( 470,40,80,30 );
  title_color=_chart->params()->TitleColor;
  titlecolor->setColor( title_color );


  QLabel* xlabelLA = new QLabel( i18n( "X-Title:" ), this );
  xlabelLA->setGeometry( 270, 90, 100, 30 );

  _xlabelED = new QLineEdit( this );
  _xlabelED->setGeometry( 270, 120, 100, 30 );
  _xlabelED->setText(_chart->params()->xtitle);

  xtitlefont = new QPushButton( this);
  xtitlefont->setGeometry( 380,120,80,30 );
  xtitlefont->setText(i18n("Font"));

  xtitlecolor=new KColorButton(this);
  xtitlecolor->setGeometry( 470,120,80,30 );
  x_color=_chart->params()->XTitleColor;
  xtitlecolor->setColor( x_color );



  QLabel* ylabelLA = new QLabel( i18n( "Y-Title:" ), this );
  ylabelLA->setGeometry( 270, 170, 100, 30 );

  _ylabelED = new QLineEdit( this );
  _ylabelED->setGeometry( 270, 200, 100, 30 );
  _ylabelED->setText(_chart->params()->ytitle);

  ytitlefont = new QPushButton( this);
  ytitlefont->setGeometry( 380,200,80,30 );
  ytitlefont->setText(i18n("Font"));

  ytitlecolor=new KColorButton(this);
  ytitlecolor->setGeometry( 470,200,80,30 );
  y_color=_chart->params()->YTitleColor;
  ytitlecolor->setColor( y_color );

  //ytitle2 doesn't work
  QLabel* ylabelLA2 = new QLabel( i18n( "Y-Title2:" ), this );
  ylabelLA2->setGeometry( 270, 240, 100, 30 );

  _ylabel2ED = new QLineEdit( this );
  _ylabel2ED->setGeometry( 270, 280, 100, 30 );
  _ylabel2ED->setText(_chart->params()->ytitle2);

  ytitle2color=new KColorButton(this);
  ytitle2color->setGeometry( 380,280,80,30 );
  y_color2=_chart->params()->YTitle2Color;
  ytitle2color->setColor( y_color2 );

  xlabel=_chart->params()->xTitleFont();
  ylabel=_chart->params()->yTitleFont();
  title=_chart->params()->titleFont();


  /*
  QButtonGroup* placementBG = new QButtonGroup( i18n( "Legend placement" ),
												this );
  placementBG->setGeometry( 390, 10, 140, 210 );

  _righttopRB = new QRadioButton( i18n( "Right/Top" ), this );
  _righttopRB->setGeometry( 410, 30, 100, 30 );
  placementBG->insert( _righttopRB, RightTop );

  _rightcenterRB = new QRadioButton( i18n( "Right/Center" ), this );
  _rightcenterRB->setGeometry( 410, 60, 100, 30 );
  placementBG->insert( _rightcenterRB, RightCenter );

  _rightbottomRB = new QRadioButton( i18n( "Right/Bottom" ), this );
  _rightbottomRB->setGeometry( 410, 90, 100, 30 );
  placementBG->insert( _rightbottomRB, RightBottom );

  _bottomleftRB = new QRadioButton( i18n( "Bottom/Left" ), this );
  _bottomleftRB->setGeometry( 410, 120, 100, 30 );
  placementBG->insert( _bottomleftRB, BottomLeft );

  _bottomcenterRB = new QRadioButton( i18n( "Bottom/Center" ), this );
  _bottomcenterRB->setGeometry( 410, 150, 100, 30 );
  placementBG->insert( _bottomcenterRB, BottomCenter );

  _bottomrightRB = new QRadioButton( i18n( "Bottom/Right" ), this );
  _bottomrightRB->setGeometry( 410, 180, 100, 30 );
  placementBG->insert( _bottomrightRB, BottomRight );
  // initialize the correct button
  ((QRadioButton*)placementBG->find( _chart->legendPlacement() ))->setChecked( true );

  connect( placementBG, SIGNAL( clicked( int ) ),
		   _chart, SLOT( setLegendPlacement( int ) ) );
  */

  QFrame* tmpQFrame;
  tmpQFrame = new QFrame( this, "Frame_1" );
  tmpQFrame->setGeometry( 10, 10, 240, 220 );
  tmpQFrame->setFrameStyle( QFrame::Sunken | QFrame::Panel );
  tmpQFrame->setLineWidth( 2 );

  /*
  preview = new kchartWidget( _chart, tmpQFrame );
  preview->show();
  _chart->addAutoUpdate( preview );
  preview->resize( tmpQFrame->contentsRect().width(),
				   tmpQFrame->contentsRect().height() );
  */
  connect(xtitlefont,SIGNAL(clicked()),this,SLOT(changeXLabelFont()));
  connect(ytitlefont,SIGNAL(clicked()),this,SLOT(changeYLabelFont()));
  connect(titlefont,SIGNAL(clicked()),this,SLOT(changeTitleFont()));


  connect(xtitlecolor,SIGNAL(changed( const QColor & )),
                this,SLOT(changeXLabelColor(const QColor &)));
  connect(ytitlecolor,SIGNAL(changed( const QColor & )),
                this,SLOT(changeYLabelColor(const QColor &)));
  connect(titlecolor,SIGNAL(changed( const QColor & )),
                this,SLOT(changeTitleColor(const QColor &)));
  connect(ytitle2color,SIGNAL(changed( const QColor & )),
                this,SLOT(changeYTitle2Color(const QColor &)));

  resize( 600, 300 );
}


kchartWizardLabelsLegendPage::~kchartWizardLabelsLegendPage()
{
  //  _chart->removeAutoUpdate( preview );
}

void kchartWizardLabelsLegendPage::changeXLabelFont()
{
    if (KFontDialog::getFont( xlabel,true,this ) == QDialog::Rejected )
      return;

}
void kchartWizardLabelsLegendPage::changeYLabelFont()
{
    if (KFontDialog::getFont( ylabel ,true,this ) == QDialog::Rejected )
      return;

}
void kchartWizardLabelsLegendPage::changeTitleFont()
{
    if (KFontDialog::getFont( title ,true,this ) == QDialog::Rejected )
      return;

}

void kchartWizardLabelsLegendPage::changeXLabelColor(const QColor &_color)
{
 x_color=_color;
}

void kchartWizardLabelsLegendPage::changeYLabelColor(const QColor &_color)
{
y_color=_color;
}

void kchartWizardLabelsLegendPage::changeTitleColor(const QColor &_color)
{
title_color=_color;
}

void kchartWizardLabelsLegendPage::changeYTitle2Color(const QColor &_color)
{
y_color2=_color;
}

void kchartWizardLabelsLegendPage::paintEvent( QPaintEvent * )
{
if(ytitle2)
        {
        _ylabel2ED->setEnabled(true);
        ytitle2color->setEnabled(true);
        }
else
        {
        _ylabel2ED->setEnabled(false);
        ytitle2color->setEnabled(false);
        }
}

void kchartWizardLabelsLegendPage::apply(  )
{
   _chart->params()->setXTitleFont(xlabel);
   _chart->params()->setYTitleFont(ylabel);
   _chart->params()->setTitleFont(title);
   _chart->params()->title= _titleED->text();
   _chart->params()->xtitle= _xlabelED->text();
   _chart->params()->ytitle= _ylabelED->text();
   _chart->params()->XTitleColor=x_color;
   _chart->params()->YTitleColor=y_color;
   _chart->params()->TitleColor=title_color;
   _chart->params()->YTitle2Color=y_color2;
   _chart->params()->ytitle2=_ylabel2ED->text();

}
#include "kchartWizardLabelsLegendPage.moc"
