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
    //     _params->PlotColor = _colorpage->plotColor();
    //_params->EdgeColor = _colorpage->edgeColor();
    //     _params->VolColor = _colorpage->volColor();
    KDChartAxisParams leftparams = _params->axisParams( KDChartAxisParams::AxisPosLeft );
    leftparams.setAxisGridColor( _colorpage->gridColor() );
    _params->setOutlineDataColor( _colorpage->lineColor() );
    KDChartAxisParams rightparams = _params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams bottomparams = _params->axisParams( KDChartAxisParams::AxisPosBottom );
    if( _colorpage->xTitleColor().isValid() )
        bottomparams.setAxisLineColor( _colorpage->xTitleColor() );
    else
        bottomparams.setAxisLineColor( _colorpage->titleColor() );
    if( _colorpage->yTitleColor().isValid() )
        leftparams.setAxisLineColor( _colorpage->yTitleColor() );
    else
        leftparams.setAxisLineColor( _colorpage->titleColor() );
    if( _colorpage->yTitle2Color().isValid() )
        rightparams.setAxisLineColor( _colorpage->yTitle2Color() );
    else
        rightparams.setAxisLineColor( _colorpage->titleColor() );
    bottomparams.setAxisLabelsColor( _colorpage->xLabelColor() );
    leftparams.setAxisLabelsColor( _colorpage->yLabelColor() );
    rightparams.setAxisLabelsColor( _colorpage->yLabel2Color() );
    
    _params->setAxisParams( KDChartAxisParams::AxisPosBottom, bottomparams );
    _params->setAxisParams( KDChartAxisParams::AxisPosLeft, leftparams );
    _params->setAxisParams( KDChartAxisParams::AxisPosRight, rightparams );

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
    KDChartAxisParams leftparams( _params->axisParams( KDChartAxisParams::AxisPosLeft ) );
    KDChartAxisParams rightparams( _params->axisParams( KDChartAxisParams::AxisPosRight ) );
    KDChartAxisParams bottomparams( _params->axisParams( KDChartAxisParams::AxisPosBottom ) );
    _colorpage->setGridColor( leftparams.axisGridColor() );
    _colorpage->setTitleColor( QColor() );
    _colorpage->setXTitleColor( bottomparams.axisLineColor() );
    _colorpage->setYTitleColor( leftparams.axisLineColor() );
    _colorpage->setYTitle2Color( rightparams.axisLineColor() );
    _colorpage->setXLabelColor( bottomparams.axisLabelsColor() );
    _colorpage->setYLabelColor( leftparams.axisLabelsColor() );
    _colorpage->setYLabel2Color( rightparams.axisLabelsColor() );
    // PENDING(kalle) Replace with KDChart equivalents
    //     _colorpage->setBackgroundColor( _params->BGColor );
    //     _colorpage->setPlotColor( _params->PlotColor );
    //     _colorpage->setVolColor( _params->VolColor );
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
//      	_colorpage->setDataColor( i, _params->dataColor( i ) );
}



