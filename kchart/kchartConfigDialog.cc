/*
 * $Id$
 *
 * Copyright 1999-2000 by Kalle Dalheimer, released under Artistic License.
 */


#include "kchartConfigDialog.h"
#include "kchartConfigDialog.moc"

#include "kchartAreaSubTypeChartPage.h"
#include "kchartBackgroundPixmapConfigPage.h"
#include "kchartBarSubTypeChartPage.h"
#include "kchartColorConfigPage.h"
#include "kchartParameterPieConfigPage.h"
#include "kchartFontConfigPage.h"
#include "kchartComboConfigPage.h"
#include "kchartParameterConfigPage.h"
#include "kchartPieConfigPage.h"
#include "kchartParameter3dConfigPage.h"

#include <kapp.h>
#include <klocale.h>
#include <kdebug.h>

#include <qradiobutton.h>

#include "kdchart/KDChartParams.h"

KChartConfigDialog::KChartConfigDialog( KDChartParams* params,
					QWidget* parent ) :
    QTabDialog( parent, "Chart config dialog", true ),
    _params( params ),
    _subTypePage( 0 )
{
    // Geometry page
    // PENDING(kalle) _xstep only for axes charts
    //_geompage = new KChartGeometryConfigPage( this );
    //addTab( _geompage, i18n( "&Geometry" ) );
    setCaption( i18n( "Chart config dialog" ) );
    // Color page
    _colorpage = new KChartColorConfigPage( this );
    addTab( _colorpage, i18n( "&Colors" ) );

    if( _params->chartType() != KDChartParams::Pie )	{
        _parameterpage = new KChartParameterConfigPage(_params,this );
        addTab( _parameterpage, i18n( "&Parameter" ) );

    } else {
        _parameterpiepage = new KChartParameterPieConfigPage(_params,this );
        addTab( _parameterpiepage, i18n( "&Parameter" ) );

        _piepage = new KChartPieConfigPage(_params, this );
        addTab( _piepage, i18n( "&Pie" ) );
    }

    _parameterfontpage = new KChartFontConfigPage(_params,this );
    addTab( _parameterfontpage, i18n( "&Font" ) );

    _backgroundpixpage = new KChartBackgroundPixmapConfigPage( _params, this );
    addTab( _backgroundpixpage, i18n( "&Background pixmap" ) );

    if( ( _params->chartType() == KDChartParams::Pie &&
          _params->threeDPies() ) ||
        ( _params->chartType() == KDChartParams::Bar &&
          _params->threeDBars() ) ) {
        _parameter3dpage = new KChartParameter3dConfigPage(_params,this );
        addTab( _parameter3dpage,i18n("3D Parameters"));
    }
    // Lines and pies might need config pages as well, but not yet
    switch( _params->chartType() ) {
    case KDChartParams::Bar:
        _subTypePage = new KChartBarSubTypeChartPage( _params, this );
        addTab( _subTypePage, i18n( "Chart &Subtype" ) );
        break;
    case KDChartParams::Area:
        _subTypePage = new KChartAreaSubTypeChartPage( _params, this );
        addTab( _subTypePage, i18n( "Chart &Subtype" ) );
        break;
    default:
        ; // do nothing
    };

    if( _params->chartType() == KDChartParams::HiLo &&
        ( _params->hiLoChartSubType() == KDChartParams::HiLoClose ||
          _params->hiLoChartSubType() == KDChartParams::HiLoOpenClose ) ) {
        // PENDING(kalle) Combo page needs to be redone completely.
        //         _hlcChart=new KChartComboPage(_params,this);
//         addTab( _hlcChart, i18n( "HLC Chart" ) );
    }

    //init
    defaults();

    // setup buttons
    setOKButton( i18n( "OK" ) );
    setApplyButton( i18n( "Apply" ) );
    setDefaultButton( i18n( "Defaults" ) );
    setCancelButton( i18n( "Cancel" ) );

    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( apply() ) );
    connect( this, SIGNAL( defaultButtonPressed() ), this, SLOT( defaults() ) );
}


void KChartConfigDialog::apply()
{
    kdDebug(35001) << "***KChartConfig::apply()";
    // Copy application data from dialog into parameter structure that is also
    // being used by the application.

    // color page

    // PENDING(kalle) Replace with equivalents
    //     _params->BGColor = _colorpage->backgroundColor();
    //     _params->GridColor = _colorpage->gridColor();
    _params->setOutlineDataColor( _colorpage->lineColor() );
    // PENDING(kalle) Replace with equivalents
    //     _params->PlotColor = _colorpage->plotColor();
    //     _params->VolColor = _colorpage->volColor();
    //     _params->TitleColor = _colorpage->titleColor();
    //     _params->XTitleColor = _colorpage->xTitleColor();
    //     _params->YTitleColor = _colorpage->yTitleColor();
    //     _params->YTitle2Color = _colorpage->yTitle2Color();
    //     _params->XLabelColor = _colorpage->xLabelColor();
    //     _params->YLabelColor = _colorpage->yLabelColor();
    //     _params->YLabel2Color = _colorpage->yLabel2Color();
    //_params->EdgeColor = _colorpage->edgeColor();

    if( _params->chartType() != KDChartParams::Pie ) {
        _parameterpage->apply();
    } else {
        _parameterpiepage->apply();
        _piepage->apply();
    }

    if( ( _params->chartType() == KDChartParams::Pie &&
          _params->threeDPies() ) ||
        ( _params->chartType() == KDChartParams::Bar &&
          _params->threeDBars() ) )
        _parameter3dpage->apply();

    _parameterfontpage->apply();

    if( _subTypePage )
        _subTypePage->apply();

    if( _params->chartType() == KDChartParams::HiLo &&
        ( _params->hiLoChartSubType() == KDChartParams::HiLoClose ||
          _params->hiLoChartSubType() == KDChartParams::HiLoOpenClose ) ) {
        _hlcChart->apply();

        _backgroundpixpage->apply();

        // data in the params struct has changed; notify application
        emit dataChanged();

        //     for( uint i = 0; i < NUMDATACOLORS; i++ )
        // 	_params->_datacolors.setColor( i, _colorpage->dataColor( i ) );
    }
}

void KChartConfigDialog::defaults()
{
    // color page
    _colorpage->setLineColor( _params->outlineDataColor() );
    // PENDING(kalle) Replace with KDChart equivalents
    //     _colorpage->setBackgroundColor( _params->BGColor );
    //     _colorpage->setGridColor( _params->GridColor );
    //     _colorpage->setPlotColor( _params->PlotColor );
    //     _colorpage->setVolColor( _params->VolColor );
    //     _colorpage->setTitleColor( _params->TitleColor );
    //     _colorpage->setXTitleColor( _params->XTitleColor );
    //     _colorpage->setYTitleColor( _params->YTitleColor );
    //     _colorpage->setYTitle2Color( _params->YTitle2Color );
    //     _colorpage->setXLabelColor( _params->XLabelColor );
    //     _colorpage->setYLabelColor( _params->YLabelColor );
    //     _colorpage->setYLabel2Color( _params->YLabel2Color );
    //_colorpage->setEdgeColor( _params->EdgeColor );

    if( _params->chartType() != KDChartParams::Pie ) {
        _parameterpage->init();
    } else {
        _parameterpiepage->init();
        _piepage->init();
    }

    if( ( _params->chartType() == KDChartParams::Pie &&
          _params->threeDPies() ) ||
        ( _params->chartType() == KDChartParams::Bar &&
          _params->threeDBars() ) )
        _parameter3dpage->init();

    _parameterfontpage->init();

    if( _subTypePage )
        _subTypePage->init();

    if( _params->chartType() == KDChartParams::HiLo &&
        ( _params->hiLoChartSubType() == KDChartParams::HiLoClose ||
          _params->hiLoChartSubType() == KDChartParams::HiLoOpenClose ) ) {
        _hlcChart->init();
    }

    _backgroundpixpage->init();
    //     for( uint i = 0; i < NUMDATACOLORS; i++ )
    // 	_colorpage->setDataColor( i, _params->_datacolors.color( i ) );
}



