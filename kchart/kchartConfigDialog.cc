/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "kchartConfigDialog.h"
#include "kchartColorConfigPage.h"
#include "kchartGeometryConfigPage.h"
#include "kchartparams.h"

#include "kchartConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KChartConfigDialog::KChartConfigDialog( KChartParameters* params,
					QWidget* parent ) :
    QTabDialog( parent, "Chart config dialog", true ),
    _params( params )
{
    // Geometry page
    // PENDING(kalle) _xstep only for axes charts
    _geompage = new KChartGeometryConfigPage( this );
    addTab( _geompage, i18n( "&Geometry" ) );

	// Color page
    _colorpage = new KChartColorConfigPage( this );
    addTab( _colorpage, i18n( "&Colors" ) );

    //init
    defaults();
	// setup buttons
    setDefaultButton( i18n( "Defaults" ) );
    setCancelButton( i18n( "Cancel" ) );
    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( apply() ) );
    connect( this, SIGNAL( defaultButtonPressed() ), this, SLOT( defaults() ) );
}


void KChartConfigDialog::apply()
{
    // Copy application data from dialog into parameter structure that is also
    // being used by the application.

    // color page
    _params->BGColor = _colorpage->backgroundColor();
    _params->GridColor = _colorpage->gridColor();
    _params->LineColor = _colorpage->lineColor();
    _params->PlotColor = _colorpage->plotColor();
    _params->VolColor = _colorpage->volColor();
    _params->TitleColor = _colorpage->titleColor();
    _params->XTitleColor = _colorpage->xTitleColor();
    _params->YTitleColor = _colorpage->yTitleColor();
    _params->YTitle2Color = _colorpage->yTitle2Color();
    _params->XLabelColor = _colorpage->xLabelColor();
    _params->YLabelColor = _colorpage->yLabelColor();
    _params->YLabel2Color = _colorpage->yLabel2Color();
    _params->EdgeColor = _colorpage->edgeColor();
//     for( uint i = 0; i < NUMDATACOLORS; i++ )
// 	_params->_datacolors.setColor( i, _colorpage->dataColor( i ) );
}


void KChartConfigDialog::defaults()
{
    // Fill pages with values

    // color page
    _colorpage->setBackgroundColor( _params->BGColor );
    _colorpage->setGridColor( _params->GridColor );
    _colorpage->setLineColor( _params->LineColor );
    _colorpage->setPlotColor( _params->PlotColor );
    _colorpage->setVolColor( _params->VolColor );
    _colorpage->setTitleColor( _params->TitleColor );
    _colorpage->setXTitleColor( _params->XTitleColor );
    _colorpage->setYTitleColor( _params->YTitleColor );
    _colorpage->setYTitle2Color( _params->YTitle2Color );
    _colorpage->setXLabelColor( _params->XLabelColor );
    _colorpage->setYLabelColor( _params->YLabelColor );
    _colorpage->setYLabel2Color( _params->YLabel2Color );
    _colorpage->setEdgeColor( _params->EdgeColor );
//     for( uint i = 0; i < NUMDATACOLORS; i++ )
// 	_colorpage->setDataColor( i, _params->_datacolors.color( i ) );
}

