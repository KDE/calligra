/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#include "kchartParameterConfigPage.h"
#include "kchartParameterConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qbuttongroup.h>
#include <kfontdialog.h>

#include "kchart_params.h"

KChartParameterConfigPage::KChartParameterConfigPage( KChartParams* params,
                                                      QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    QVBoxLayout* toplevel = new QVBoxLayout( this, 10 );

    QGridLayout* layout = new QGridLayout( 1, 3 );
    toplevel->addLayout( layout );

    QButtonGroup* gb1 = new QButtonGroup( i18n("Parameters"), this );
    QGridLayout *grid1 = new QGridLayout(gb1,9,1,15,8);

    grid = new QCheckBox( i18n( "Grid" ), gb1 );
    grid1->addWidget(grid,0,0);


    xaxis = new QCheckBox( i18n( "X-Axis" ), gb1 );
    grid1->addWidget(xaxis,1,0);

    yaxis = new QCheckBox( i18n( "Y-Axis" ), gb1);
    grid1->addWidget(yaxis,2,0);

    xlabel = new QCheckBox( i18n( "Has X-Label" ), gb1 );
    grid1->addWidget(xlabel,3,0);

    yaxis2 = new QCheckBox( i18n( "Y-Axis2" ), gb1 );
    grid1->addWidget(yaxis2,4,0);

    lineMarker = new QCheckBox( i18n( "Line Marker" ), gb1 );
    grid1->addWidget(lineMarker,5,0);

    llabel = new QCheckBox( i18n( "Legend" ), gb1 );
    grid1->addWidget(llabel,6,0);

    QButtonGroup* gb2 = new QButtonGroup( i18n("Title"), this );
    QGridLayout *grid2 = new QGridLayout(gb2,8,2,15,8);

    QLabel *tmpLabel = new QLabel( i18n( "Y-Title" ), gb2 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid2->addWidget(tmpLabel,2,0);

    ytitle= new QLineEdit( gb2 );
    ytitle->setMaximumWidth(130);
    grid2->addWidget(ytitle,3,0);
    ytitle->setEnabled(false);

    tmpLabel = new QLabel( i18n( "X-Title" ), gb2 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid2->addWidget(tmpLabel,4,0);

    xtitle= new QLineEdit( gb2 );
    xtitle->setMaximumWidth(130);
    grid2->addWidget(xtitle,5,0);
    xtitle->setEnabled(false);

    tmpLabel = new QLabel( i18n( "Y-Label format" ), gb2 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid2->addWidget(tmpLabel,0,1);

    ylabel_fmt= new QLineEdit( gb2 );
    ylabel_fmt->setMaximumWidth(130);
    grid2->addWidget(ylabel_fmt,1,1);
    ylabel_fmt->setEnabled(false);

    tmpLabel = new QLabel( i18n( "Y-Title 2" ), gb2 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid2->addWidget(tmpLabel,2,1);

    ytitle2= new QLineEdit( gb2 );
    ytitle2->setMaximumWidth(130);
    grid2->addWidget(ytitle2,3,1);

    tmpLabel = new QLabel( i18n( "Y-Label format 2" ), gb2 );
    tmpLabel->setAlignment(Qt::AlignCenter);
    grid2->addWidget(tmpLabel,4,1);

    ylabel2_fmt= new QLineEdit( gb2 );
    ylabel2_fmt->setMaximumWidth(130);
    grid2->addWidget(ylabel2_fmt,5,1);

    layout->addWidget(gb1,0,0);
    layout->addWidget(gb2,0,1);
    grid1->activate();
    grid2->activate();

    /*connect( grid, SIGNAL( toggled( bool ) ),
      this, SLOT( changeState( bool ) ) );*/
    /*connect( xaxis, SIGNAL( toggled( bool ) ),
      this, SLOT( changeXaxisState( bool ) ) );*/
}

void KChartParameterConfigPage::changeXaxisState(bool state)
{
    if(state)
        xlabel->setEnabled(true);
    else
        xlabel->setEnabled(false);

}

void KChartParameterConfigPage::init()
{
    // PENDING(kalle) adapt these to KDChart
    //     grid->setChecked(_params->grid);
    //     xlabel->setChecked(_params->hasxlabel);


    xaxis->setChecked(_params->axisVisible(KDChartAxisParams::AxisPosBottom));
    yaxis->setChecked(_params->axisVisible(KDChartAxisParams::AxisPosLeft));

    llabel->setChecked(_params->legendPosition()!=KDChartParams::NoLegend);
    grid->setChecked(_params->showGrid());

    if( _params->chartType() == KDChartParams::Line )
    {
        lineMarker->setEnabled(true);
        lineMarker->setChecked(_params->lineMarker());
    }
    else
    {
        lineMarker->setEnabled(false);
        lineMarker->setChecked(false);
    }

    // PENDING(kalle) Adapt this
    //     if(_params->has_yaxis2())
    //     	{
    //     	yaxis2->setChecked(_params->yaxis2);
    //     	int len=_params->ylabel2_fmt.length();
    //         ylabel2_fmt->setText(_params->ylabel2_fmt.right(len-3));

    //     	ytitle2->setText(_params->ytitle2);
    //     	}
    //     else
    {
    	yaxis2->setEnabled(false);
    	ylabel2_fmt->setEnabled(false);
    	ytitle2->setEnabled(false);
    }


    // PENDING(kalle) Adapt this
    //     xtitle->setText(_params->xtitle);
    //     ytitle->setText(_params->ytitle);
    //     int len=_params->ylabel_fmt.length();
    //     ylabel_fmt->setText(_params->ylabel_fmt.right(len-3));

    //     if(_params->xaxis)
    //     	xlabel->setEnabled(true);
    //     else
    xlabel->setEnabled(false);
}


void KChartParameterConfigPage::apply()
{

    _params->setLegendPosition(llabel->isChecked() ? KDChartParams::LegendRight : KDChartParams::NoLegend);

    _params->setAxisVisible(KDChartAxisParams::AxisPosBottom,xaxis->isChecked());
    _params->setAxisVisible(KDChartAxisParams::AxisPosLeft,yaxis->isChecked());

    _params->setAxisShowGrid(KDChartAxisParams::AxisPosLeft,grid->isChecked() );
    _params->setAxisShowGrid(KDChartAxisParams::AxisPosBottom,grid->isChecked() );

// PENDING(kalle) Adapt this
    //     _params->border=border->isChecked();
    //     _params->llabel=llabel->isChecked();
    //     if(xaxis->isChecked())
    //     	_params->hasxlabel=xlabel->isChecked();
    //     if(_params->has_yaxis2())
    //     	{
    //     	_params->yaxis2=yaxis2->isChecked();
    //     	if(!ylabel2_fmt->text().isEmpty())
    //     		_params->ylabel2_fmt="%g "+ylabel2_fmt->text();
    //     	_params->ytitle2=ytitle2->text();
    //     	}


    // PENDING(kalle) Adapt this
    //     if(grid->isChecked())
    //     	_params->shelf=shelf->isChecked();
    //     _params->xtitle=xtitle->text();
    //     _params->ytitle=ytitle->text();
    //     if(!ylabel_fmt->text().isEmpty())
    //     	_params->ylabel_fmt="%g "+ylabel_fmt->text();

    // PENDING(kalle) Adapt this
    //     if(annotation->text().isEmpty()) {
    //         if(_params->annotation) {
    //                 delete _params->annotation;
    //                 _params->annotation=0;
    //                 }
    //         }
    //     else
    //         {
    //         if(!_params->annotation)
    //                 _params->annotation=new KChartAnnotationType;
    //         _params->annotation->note=annotation->text();
    //         _params->annotation->color=color->color();
    //         _params->annotation->point=element->value()-1;
    //         _params->setAnnotationFont( annotationFont );
    //         }

    if( _params->chartType() == KDChartParams::Line )
        _params->setLineMarker(lineMarker->isChecked());
}
