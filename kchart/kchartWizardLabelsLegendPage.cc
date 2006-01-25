#include "kchartWizardLabelsLegendPage.h"
#include "kchart_view.h"
#include "kchart_part.h"

#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <klocale.h>
#include <kfontdialog.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qcolor.h>

#include "kchart_params.h"

namespace KChart
{

KChartWizardLabelsLegendPage::KChartWizardLabelsLegendPage( QWidget* parent, KChartPart* chart ) :
    QWidget( parent ),
    _chart( chart )
{
#if 0
    ytitle2=true;
#endif

    KDChartAxisParams leftparams;
    leftparams = _chart->params()->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams bottomparams;
    bottomparams = _chart->params()->axisParams( KDChartAxisParams::AxisPosBottom );
    x_color=bottomparams.axisLineColor();
    y_color=leftparams.axisLineColor();
    xlabel=bottomparams.axisLabelsFont();
    ylabel=leftparams.axisLabelsFont();

    QGridLayout *grid1 = new QGridLayout(this,2,2,KDialog::marginHint(), KDialog::spacingHint());

    QGroupBox* tmpQGroupBox;
    tmpQGroupBox = new QGroupBox( this, "GroupBox_1" );
    tmpQGroupBox->setFrameStyle( 49 );

    QGridLayout *grid2 = new QGridLayout(tmpQGroupBox,4,4,KDialog::marginHint(), KDialog::spacingHint());

    QLabel* titleLA = new QLabel( i18n( "Title:" ),tmpQGroupBox );
    grid2->addWidget(titleLA,0,0);

    _titleED = new QLineEdit( tmpQGroupBox );
    grid2->addWidget(_titleED,0,1);
    _titleED->setText( _chart->params()->header1Text() );

    titlefont = new QPushButton( tmpQGroupBox );
    grid2->addWidget(titlefont,0,2);
    titlefont->setText(i18n("Font..."));

    titlecolor=new KColorButton(tmpQGroupBox);
    grid2->addWidget(titlecolor,0,3);
    title_color=_chart->params()->headerFooterColor( KDChartParams::HdFtPosHeader );
    titlecolor->setColor( title_color );


    QLabel* xlabelLA = new QLabel( i18n( "X-title:" ), tmpQGroupBox );
    //xlabelLA->setEnabled(false); //Not supported by kdChart yet
    grid2->addWidget(xlabelLA,1,0);

    _xlabelED = new QLineEdit( tmpQGroupBox );
    //_xlabelED->setEnabled(false); //Not supported by kdChart yet
    _xlabelED->setText(_chart->params()->axisTitle( KDChartAxisParams::AxisPosBottom));
    grid2->addWidget(_xlabelED,1,1);

    xtitlefont = new QPushButton( tmpQGroupBox );
    xtitlefont->setText(i18n("Font..."));
    //xtitlefont->setEnabled(false); //Not supported by kdChart yet
    grid2->addWidget(xtitlefont,1,2);

    xtitlecolor=new KColorButton(tmpQGroupBox);
    // PENDING(kalle) Put back in
    //  x_color=_chart->params()->XTitleColor;
    xtitlecolor->setColor( x_color );
    //xtitlecolor->setEnabled(false); //Not supported by kdChart yet
    grid2->addWidget(xtitlecolor,1,3);


    QLabel* ylabelLA = new QLabel( i18n( "Y-title:" ), tmpQGroupBox );
    //ylabelLA->setEnabled(false); //Not supported by kdChart yet
    grid2->addWidget(ylabelLA,2,0);

    _ylabelED = new QLineEdit( tmpQGroupBox );
    //_ylabelED->setEnabled(false); //Not supported by kdChart yet
    _ylabelED->setText(_chart->params()->axisTitle( KDChartAxisParams::AxisPosLeft));
    grid2->addWidget(_ylabelED,2,1);


    ytitlefont = new QPushButton( tmpQGroupBox);
    ytitlefont->setText(i18n("Font..."));
    //ytitlefont->setEnabled(false); //Not supported by kdChart yet
    grid2->addWidget(ytitlefont,2,2);

    ytitlecolor=new KColorButton(tmpQGroupBox);
    //ytitlecolor->setEnabled(false); //Not supported by kdChart yet
    // PENDING(kalle) Put back in
    //   y_color=_chart->params()->YTitleColor;
    ytitlecolor->setColor( y_color );
    grid2->addWidget(ytitlecolor,2,3);

#if 0
    //ytitle2 doesn't work
    QLabel* ylabelLA2 = new QLabel( i18n( "Y-title 2:" ), tmpQGroupBox );
    ylabelLA2->setEnabled(false); //Not supported by kdChart yet
    grid2->addWidget(ylabelLA2,3,0);

    _ylabel2ED = new QLineEdit( tmpQGroupBox );
    _ylabel2ED->setEnabled(false); //Not supported by kdChart yet
    // PENDING(kalle) Put back in
    //   _ylabel2ED->setText(_chart->params()->ytitle2);
    grid2->addWidget(_ylabel2ED,3,1);

    ytitle2font = new QPushButton( tmpQGroupBox);
    ytitle2font->setText(i18n("Font..."));
    ytitle2font->setEnabled(false); //Not supported by kdChart yet
    grid2->addWidget(ytitle2font,3,2);

    ytitle2color=new KColorButton(tmpQGroupBox);
    ytitle2color->setEnabled(false); //Not supported by kdChart yet
    // PENDING(kalle) Put back in
    //   y_color2=_chart->params()->YTitle2Color;
    ytitle2color->setColor( y_color2 );
    grid2->addWidget(ytitle2color,3,3);
#endif

    // PENDING(kalle) Put back in
    //   xlabel=_chart->params()->xTitleFont();
    //   ylabel=_chart->params()->yTitleFont();


    QLabel* lab = new QLabel( i18n( "Legend title:" ), tmpQGroupBox );
    grid2->addWidget(lab,3,0);

    _legendTitleText = new QLineEdit( tmpQGroupBox );
    grid2->addWidget(_legendTitleText,3,1);
    _legendTitleText->setText( _chart->params()->legendTitleText() );


    legendTitleFont = new QPushButton( tmpQGroupBox );
    legendTitleFont->setText(i18n("Font..."));
    _legendTitleFont=_chart->params()->legendTitleFont();
    grid2->addWidget(legendTitleFont,3,2);

    legendTitleColor=new KColorButton(tmpQGroupBox);
    _legendTitleColor=_chart->params()->legendTitleTextColor();
    legendTitleColor->setColor( _legendTitleColor );
    grid2->addWidget(legendTitleColor,3,3);


    lab = new QLabel( i18n( "Legend text:" ), tmpQGroupBox );
    grid2->addWidget(lab,4,0);

    legendTextFont = new QPushButton( tmpQGroupBox );
    _legendTextFont=_chart->params()->legendFont();
    legendTextFont->setText(i18n("Font..."));
    grid2->addWidget(legendTextFont,4,2);

    legendTextColor=new KColorButton(tmpQGroupBox);
    _legendTextColor=_chart->params()->legendTextColor();
    legendTextColor->setColor( _legendTextColor );
    grid2->addWidget(legendTextColor,4,3);

    title=_chart->params()->header1Font();
    grid1->addWidget(tmpQGroupBox,0,0);

    connect(xtitlefont,SIGNAL(clicked()),this,SLOT(changeXLabelFont()));
    connect(ytitlefont,SIGNAL(clicked()),this,SLOT(changeYLabelFont()));
#if 0
    connect(ytitle2font,SIGNAL(clicked()),this,SLOT(changeY2LabelFont()));
#endif
    connect(titlefont,SIGNAL(clicked()),this,SLOT(changeTitleFont()));
    connect(legendTitleFont,SIGNAL(clicked()),this,SLOT(changeLegendTitleFont()));

    connect(legendTextFont,SIGNAL(clicked()),this,SLOT(changeLegendTextFont()));

    connect(xtitlecolor,SIGNAL(changed( const QColor & )),
            this,SLOT(changeXLabelColor(const QColor &)));
    connect(ytitlecolor,SIGNAL(changed( const QColor & )),
            this,SLOT(changeYLabelColor(const QColor &)));
    connect(titlecolor,SIGNAL(changed( const QColor & )),
            this,SLOT(changeTitleColor(const QColor &)));
#if 0
    connect(ytitle2color,SIGNAL(changed( const QColor & )),
            this,SLOT(changeYTitle2Color(const QColor &)));
#endif
    connect(legendTitleColor,SIGNAL(changed( const QColor & )),
            this,SLOT(changeLegendTitleColor(const QColor &)));

    connect(legendTextColor,SIGNAL(changed( const QColor & )),
            this,SLOT(changeLegendTextColor(const QColor &)));

}


KChartWizardLabelsLegendPage::~KChartWizardLabelsLegendPage()
{
    //  _chart->removeAutoUpdate( preview );
}

void KChartWizardLabelsLegendPage::changeLegendTextFont()
{
    if (KFontDialog::getFont( _legendTextFont,false,this ) == QDialog::Rejected )
        return;
}

void KChartWizardLabelsLegendPage::changeLegendTitleFont()
{
    if (KFontDialog::getFont( _legendTitleFont,false,this ) == QDialog::Rejected )
        return;
}

void KChartWizardLabelsLegendPage::changeXLabelFont()
{
    if (KFontDialog::getFont( xlabel,false,this ) == QDialog::Rejected )
        return;
}

void KChartWizardLabelsLegendPage::changeYLabelFont()
{
    if (KFontDialog::getFont( ylabel ,false,this ) == QDialog::Rejected )
        return;
}

#if 0
void KChartWizardLabelsLegendPage::changeY2LabelFont()
{
    if (KFontDialog::getFont( ylabel2, false, this ) == QDialog::Rejected )
        return;
}
#endif

void KChartWizardLabelsLegendPage::changeTitleFont()
{
    if (KFontDialog::getFont( title ,false,this ) == QDialog::Rejected )
        return;

}

void KChartWizardLabelsLegendPage::changeLegendTextColor(const QColor &_color)
{
    _legendTextColor=_color;
}

void KChartWizardLabelsLegendPage::changeXLabelColor(const QColor &_color)
{
    x_color=_color;
}

void KChartWizardLabelsLegendPage::changeYLabelColor(const QColor &_color)
{
    y_color=_color;
}

void KChartWizardLabelsLegendPage::changeTitleColor(const QColor &_color)
{
    title_color=_color;
}

#if 0
void KChartWizardLabelsLegendPage::changeYTitle2Color(const QColor &_color)
{
    y_color2=_color;
}
#endif

void KChartWizardLabelsLegendPage::changeLegendTitleColor(const QColor &_color)
{
    _legendTitleColor=_color;
}

void KChartWizardLabelsLegendPage::paintEvent( QPaintEvent * )
{
#if 0
    if( ytitle2 ) {
        _ylabel2ED->setEnabled(true);
        ytitle2color->setEnabled(true);
    }
    else {
        _ylabel2ED->setEnabled(false);
        ytitle2color->setEnabled(false);
    }
#endif
}

void KChartWizardLabelsLegendPage::apply(  )
{
    _chart->params()->setHeader1Text( _titleED->text() );

    // PENDING(kalle) Put back in
    //    _chart->params()->setXTitleFont(xlabel);
    //    _chart->params()->setYTitleFont(ylabel);
    //    _chart->params()->xtitle= _xlabelED->text();
    //    _chart->params()->ytitle= _ylabelED->text();
    //    _chart->params()->XTitleColor=x_color;
    //    _chart->params()->YTitleColor=y_color;
    //    _chart->params()->YTitle2Color=y_color2;
    //    _chart->params()->ytitle2=_ylabel2ED->text();

    _chart->params()->setHeaderFooterColor( KDChartParams::HdFtPosHeader,title_color);
    _chart->params()->setHeader1Font(title);
    _chart->params()->setAxisTitle( KDChartAxisParams::AxisPosBottom, _xlabelED->text() );
    _chart->params()->setAxisTitle( KDChartAxisParams::AxisPosLeft, _ylabelED->text() );

    _chart->params()->setLegendTitleText(_legendTitleText->text());
    _chart->params()->setLegendTitleTextColor(_legendTitleColor);
    _chart->params()->setLegendTitleFont(_legendTitleFont,true);
    _chart->params()->setLegendTextColor(_legendTextColor);
    _chart->params()->setLegendFont( _legendTextFont,true);

    KDChartAxisParams leftparams;
    leftparams   = _chart->params()->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams bottomparams;
    bottomparams = _chart->params()->axisParams( KDChartAxisParams::AxisPosBottom );
    if( x_color.isValid() )
        bottomparams.setAxisLineColor( x_color );
    if( y_color.isValid() )
        leftparams.setAxisLineColor( y_color );
    if(bottomparams.axisLabelsFont()!=xlabel)
        bottomparams.setAxisLabelsFont(xlabel,false);
    if(leftparams.axisLabelsFont()!=ylabel)
        leftparams.setAxisLabelsFont(ylabel,true);

    _chart->params()->setAxisParams( KDChartAxisParams::AxisPosBottom, bottomparams );
    _chart->params()->setAxisParams( KDChartAxisParams::AxisPosLeft, leftparams );
}

}  //KChart namespace

#include "kchartWizardLabelsLegendPage.moc"
