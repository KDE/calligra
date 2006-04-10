/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
#include <q3buttongroup.h>
#include <qradiobutton.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <kfontdialog.h>


#include "kdchart/KDChartAxisParams.h"
#include "kchart_params.h"

namespace KChart
{

KChartParameterConfigPage::KChartParameterConfigPage( KChartParams* params,
                                                      QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    Q3VBoxLayout* toplevel = new Q3VBoxLayout( this, 10 );

    Q3GridLayout* layout = new Q3GridLayout( 1, 3 );
    toplevel->addLayout( layout );

    Q3ButtonGroup* gb1 = new Q3ButtonGroup( 0, Qt::Vertical, i18n("Parameters"), this );
    gb1->layout()->setSpacing(KDialog::spacingHint());
    gb1->layout()->setMargin(KDialog::marginHint());
    Q3GridLayout *grid1 = new Q3GridLayout(gb1->layout(),9,1);

    grid = new QCheckBox( i18n( "Grid" ), gb1 );
    grid->setWhatsThis( i18n("If this is checked, the grid is shown. If you uncheck this option, the grid will not be displayed anymore."));
    grid1->addWidget(grid, 0, 0);

    yaxis = new QCheckBox( i18n( "Y-axis" ), gb1);
    yaxis->setWhatsThis( i18n("If this is checked, the Y-axis is shown. If you uncheck this option, the Y-axis and the Y grid lines will not be displayed anymore."));
    connect( yaxis, SIGNAL( clicked() ), this, SLOT( axisChanged() ) );
    grid1->addWidget(yaxis, 1, 0);

    xaxis = new QCheckBox( i18n( "X-axis" ), gb1 );
    xaxis->setWhatsThis( i18n("If this is checked, the X-axis is shown. If you uncheck this option, the X-axis and the X grid lines will not be displayed anymore."));
    connect( xaxis, SIGNAL( clicked() ), this, SLOT( axisChanged() ) );
    grid1->addWidget(xaxis, 2, 0);

#if 0
    xlabel = new QCheckBox( i18n( "Has X-label" ), gb1 );
    grid1->addWidget(xlabel,3,0);

    yaxis2 = new QCheckBox( i18n( "Y-axis 2" ), gb1 );
    grid1->addWidget(yaxis2,4,0);
#endif

#if 0 // Moved to the Line page
    lineMarker = new QCheckBox( i18n( "Line marker" ), gb1 );
    grid1->addWidget(lineMarker, 4, 0);
#endif
#if 0
    llabel = new QCheckBox( i18n( "Legend" ), gb1 );
    grid1->addWidget(llabel,6,0);
#endif

    Q3ButtonGroup* gb2 = new Q3ButtonGroup( 0, Qt::Vertical, 
                      i18n("Settings"), this );
    gb2->layout()->setSpacing(KDialog::spacingHint());
    gb2->layout()->setMargin(KDialog::marginHint());
    Q3GridLayout *grid2 = new Q3GridLayout(gb2->layout(),8,1);

    Q3HBoxLayout * top = new Q3HBoxLayout( this );
    // The X axis title
    QLabel *tmpLabel = new QLabel( i18n( "X-title:" ), gb2 );
    top->addWidget(tmpLabel);
    // The Y axis title
    tmpLabel = new QLabel( i18n( "Y-title:" ), gb2 );
    top->addWidget(tmpLabel);

    Q3HBoxLayout * bottom = new Q3HBoxLayout( this );
    xtitle= new QLineEdit( gb2 );
    xtitle->setWhatsThis( i18n("Write the title for the X-axis here, if you want a title. The color for this title is set in the Colors tab, in the same dialog, and the font is set in the Font tab."));
    bottom->addWidget(xtitle);
    Q3BoxLayout * l = new Q3VBoxLayout( this );
    ytitle= new QLineEdit( gb2 );
    ytitle->setWhatsThis( i18n("Write the title for the Y-axis here, if you want a title. The color for this title is set in the Colors tab, in the same dialog, and the font is set in the Font tab."));
    bottom->addWidget(ytitle);

    l->addLayout(top);
    l->addLayout(bottom);
    grid2->addLayout(l, 0, 0);

    // Linear or logarithmic scale
    Q3VButtonGroup *scaletype = new Q3VButtonGroup(i18n("Scale Types"), gb2);
    lin = new QRadioButton( i18n("Linear scale"), scaletype);
    lin->setWhatsThis( i18n("This sets the Y-axis to be linear. This is default."));
    log = new QRadioButton( i18n("Logarithmic scale"), scaletype);
    log->setWhatsThis( i18n("This sets the Y-axis to be logarithmic."));
    grid2->addWidget(scaletype, 1, 0);

    // Decimal precision
    Q3VButtonGroup *precision = new Q3VButtonGroup(i18n("Precision for Numerical Left Axis"), gb2);
    grid2->addWidget(precision, 2, 0);
    QRadioButton * automatic_precision = new QRadioButton( i18n("Automatic precision"), precision);
    automatic_precision->setWhatsThis( i18n("This sets the precision as automatic which means that KChart decides what precision to apply."));
    automatic_precision->setChecked(true);
    max = new QRadioButton( i18n("Decimal precision:"), precision);
    max->setWhatsThis( i18n("This sets the Y-axis precision. For example, if you choose a precision of 2, the value 5 will be displayed as 5.00 alongside the Y-axis."));
    connect(automatic_precision, SIGNAL(toggled(bool)), this,
            SLOT(automatic_precision_toggled(bool)) );
    maximum_length = new QSpinBox(0, 15, 1, precision );
    maximum_length->setWhatsThis( i18n("Set the precision you want to display for the Y-axis, if you choose Decimal precision. The range is 0 to 15; 2 being the default."));
    maximum_length->setValue(2);

#if 0
    tmpLabel = new QLabel( i18n( "Y-label format:" ), gb2 );
    grid2->addWidget(tmpLabel,0,1);

    ylabel_fmt= new QLineEdit( gb2 );
    ylabel_fmt->setMaximumWidth(130);
    grid2->addWidget(ylabel_fmt,1,1);
    ylabel_fmt->setEnabled(false);

    tmpLabel = new QLabel( i18n( "Y-title 2:" ), gb2 );
    grid2->addWidget(tmpLabel,2,1);

    ytitle2= new QLineEdit( gb2 );
    ytitle2->setMaximumWidth(130);
    grid2->addWidget(ytitle2,3,1);

    tmpLabel = new QLabel( i18n( "Y-label format 2:" ), gb2 );
    grid2->addWidget(tmpLabel,4,1);

    ylabel2_fmt= new QLineEdit( gb2 );
    ylabel2_fmt->setMaximumWidth(130);
    grid2->addWidget(ylabel2_fmt,5,1);
#endif
    layout->addWidget(gb1,0,0);
    layout->addWidget(gb2,0,1);

#if 0
    grid1->activate();
    grid2->activate();
#endif
    /*connect( grid, SIGNAL( toggled( bool ) ),
      this, SLOT( changeState( bool ) ) );*/
    /*connect( xaxis, SIGNAL( toggled( bool ) ),
      this, SLOT( changeXaxisState( bool ) ) );*/
}


    void KChartParameterConfigPage::changeXaxisState(bool /*state*/)
{
#if 0
    if(state)
        xlabel->setEnabled(true);
    else
        xlabel->setEnabled(false);
#endif
}

void KChartParameterConfigPage::init()
{
    // PENDING(kalle) adapt these to KDChart
    //     grid->setChecked(_params->grid);
    //     xlabel->setChecked(_params->hasxlabel);

    grid->setChecked(_params->showGrid());

    xaxis->setChecked(_params->axisVisible(KDChartAxisParams::AxisPosBottom));
    yaxis->setChecked(_params->axisVisible(KDChartAxisParams::AxisPosLeft));

#if 0
    llabel->setChecked(_params->legendPosition()!=KDChartParams::NoLegend);
#endif

#if 0
    if ( _params->chartType() == KDChartParams::Line ) {
        lineMarker->setEnabled(true);
        lineMarker->setChecked(_params->lineMarker());
    }
    else {
        lineMarker->setEnabled(false);
        lineMarker->setChecked(false);
    }
#endif

    // PENDING(kalle) Adapt this
    //     if(_params->has_yaxis2())
    //     	{
    //     	yaxis2->setChecked(_params->yaxis2);
    //     	int len=_params->ylabel2_fmt.length();
    //         ylabel2_fmt->setText(_params->ylabel2_fmt.right(len-3));

    //     	ytitle2->setText(_params->ytitle2);
    //     	}
    //     else
#if 0
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
#endif

    xtitle->setText( _params->axisTitle( KDChartAxisParams::AxisPosBottom) );
    ytitle->setText( _params->axisTitle( KDChartAxisParams::AxisPosLeft) );
    axisChanged();

    // Linear / logarithmic Y axis
    if ( _params->axisParams( KDChartAxisParams::AxisPosLeft ).axisCalcMode() ==
                              KDChartAxisParams::AxisCalcLinear )
	lin->setChecked(true);
    else
	log->setChecked(true);
    
     if ( _params->axisParams( KDChartAxisParams::AxisPosLeft ).axisDigitsBehindComma() ==
            KDCHART_AXIS_LABELS_AUTO_DIGITS )
        maximum_length->setEnabled(false);
    else
    {
        max->setChecked(true);
        maximum_length->setValue( _params->axisParams( KDChartAxisParams::AxisPosLeft ).axisDigitsBehindComma() );
    }
}


void KChartParameterConfigPage::apply()
{
#if 0
    _params->setLegendPosition(llabel->isChecked() ? KDChartParams::LegendRight : KDChartParams::NoLegend);
#endif

    _params->setAxisVisible(KDChartAxisParams::AxisPosBottom,xaxis->isChecked());
    _params->setAxisVisible(KDChartAxisParams::AxisPosLeft,yaxis->isChecked());

    _params->setAxisShowGrid(KDChartAxisParams::AxisPosLeft,
			     grid->isChecked() );
    _params->setAxisShowGrid(KDChartAxisParams::AxisPosBottom,
			     grid->isChecked() );

    _params->setAxisTitle( KDChartAxisParams::AxisPosBottom, xtitle->text() );
    _params->setAxisTitle( KDChartAxisParams::AxisPosLeft, ytitle->text() );

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

#if 0
    if( _params->chartType() == KDChartParams::Line )
        _params->setLineMarker(lineMarker->isChecked());
#endif

    // Set the scale for the Y axis (linear / logarithmic)
    {
        KDChartAxisParams  params;
        params = _params->axisParams( KDChartAxisParams::AxisPosLeft );

        if (lin->isChecked())
            params.setAxisCalcMode(KDChartAxisParams::AxisCalcLinear);
        else
        {
            if(params.axisCalcMode() != KDChartAxisParams::AxisCalcLogarithmic)
            {
                // Since in this version of kdChart logarithmic scales only work for Line charts,
                // lets switch now
                _params->setChartType( KChartParams::Line );
            }
            params.setAxisCalcMode(KDChartAxisParams::AxisCalcLogarithmic);
        }

        if ( maximum_length->isEnabled() )
            params.setAxisDigitsBehindComma( maximum_length->value() );
        else
            params.setAxisDigitsBehindComma( KDCHART_AXIS_LABELS_AUTO_DIGITS );

        _params->setAxisParams( KDChartAxisParams::AxisPosLeft, params );
    }
}

void KChartParameterConfigPage::automatic_precision_toggled(bool toggled)
{
    if (toggled)
        maximum_length->setEnabled(false);
    else
        maximum_length->setEnabled(true);
}

void KChartParameterConfigPage::axisChanged()
{
    if ( !xaxis->isChecked() || !yaxis->isChecked() )
    {
        xtitle->setEnabled( false );
        ytitle->setEnabled( false );
    }
    else
    {
        xtitle->setEnabled( true );
        ytitle->setEnabled( true );
    }
}

}  //KChart namespace
