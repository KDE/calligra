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
#include "kchartLineSubTypeChartPage.h"
#include "kchartColorConfigPage.h"
#include "kchartParameterPieConfigPage.h"
#include "kchartFontConfigPage.h"
#include "kchartComboConfigPage.h"
#include "kchartParameterConfigPage.h"
#include "kchartPieConfigPage.h"
#include "kchartParameter3dConfigPage.h"
#include "kchartPolarSubTypeChartPage.h"

#include <kapp.h>
#include <klocale.h>
#include <kdebug.h>

#include <qradiobutton.h>
#include <qbrush.h>

#include "kdchart/KDChartParams.h"
#include "kdchart/KDFrame.h"

KChartConfigDialog::KChartConfigDialog( KDChartParams* params,
					QWidget* parent,KoChart::Data *dat ) :
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

    _parameterfontpage = new KChartFontConfigPage(_params,this, dat );
    addTab( _parameterfontpage, i18n( "&Font" ) );

    //_backgroundpixpage = new KChartBackgroundPixmapConfigPage( _params, this );
    //addTab( _backgroundpixpage, i18n( "&Background pixmap" ) );

    if( ( _params->chartType() == KDChartParams::Bar &&
          _params->threeDBars() ) )
    {
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
    case KDChartParams::Line:
        _subTypePage = new KChartLineSubTypeChartPage( _params, this );
        addTab( _subTypePage, i18n( "Chart &Subtype" ) );
        break;
    case KDChartParams::Polar:
        _subTypePage = new KChartPolarSubTypeChartPage( _params, this );
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
    kdDebug(35001) << "***KChartConfig::apply()\n";
    // Copy application data from dialog into parameter structure that is also
    // being used by the application.

    // color page

    // background color
    const QColor backColor( _colorpage->backgroundColor() );
    //
    // temp. hack: the background is removed if set to 230,222,222.
    //
    //             For KOffice 1.2 this is to be removed by a checkbox.
    //                                                (khz, 10.12.2001)
    if( 230 == backColor.red() && 222 == backColor.green() && 222 == backColor.blue() ){
        bool bFound;
        const KDChartParams::KDChartFrameSettings * innerFrame =
            _params->frameSettings( KDChartEnums::AreaInnermost, bFound );
        if( bFound ) {
            KDFrame& frame( (KDFrame&)innerFrame->frame() );
            frame.setBackground();
        }
    }
    else
        _params->setSimpleFrame( KDChartEnums::AreaInnermost,
                                 0,0,  0,0,
                             true,
                             true,
                             KDFrame::FrameFlat,
                             1,
                             0,
                             QPen( Qt::NoPen ),
                             QBrush( _colorpage->backgroundColor() ) );
/*
                             &pixUSD,
                             KDFrame::PixStretched ); // enum: PixCentered, PixScaled, PixStretched
*/

    // PENDING(kalle) Replace with equivalents
    //     _params->PlotColor = _colorpage->plotColor();
    //_params->EdgeColor = _colorpage->edgeColor();
    //     _params->VolColor = _colorpage->volColor();

    // set the color for Header, Header2 and Footer
    // (this will be rerplaced for KOffice 1.2 to support
    //  the enhanced header/footer handling of KDChart 1.1
    _params->setHeaderFooterColor( KDChartParams::HdFtPosHeader,
            _colorpage->hdFtColor( KDChartParams::HdFtPosHeader  ) );
    _params->setHeaderFooterColor( KDChartParams::HdFtPosHeader2,
            _colorpage->hdFtColor( KDChartParams::HdFtPosHeader2 ) );
    _params->setHeaderFooterColor( KDChartParams::HdFtPosFooter,
            _colorpage->hdFtColor( KDChartParams::HdFtPosFooter  ) );

    KDChartAxisParams leftparams = _params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams rightparams = _params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams bottomparams = _params->axisParams( KDChartAxisParams::AxisPosBottom );
    leftparams.setAxisGridColor(   _colorpage->gridColor() );

    // temporary solution: make vertical grid lines have the same
    // color as horizontal grid lines           (khz, 10.12.2001)
    bottomparams.setAxisGridColor( _colorpage->gridColor() );

    _params->setOutlineDataColor( _colorpage->lineColor() );
    if( _colorpage->xTitleColor().isValid() )
        bottomparams.setAxisLineColor( _colorpage->xTitleColor() );
    if( _colorpage->yTitleColor().isValid() )
        leftparams.setAxisLineColor( _colorpage->yTitleColor() );
    if( _colorpage->yTitle2Color().isValid() )
        rightparams.setAxisLineColor( _colorpage->yTitle2Color() );
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

    if( ( _params->chartType() == KDChartParams::Bar &&
          _params->threeDBars() ) )
        _parameter3dpage->apply();

    _parameterfontpage->apply();

    if( _subTypePage )
        _subTypePage->apply();

    if( _params->chartType() == KDChartParams::HiLo &&
        ( _params->hiLoChartSubType() == KDChartParams::HiLoClose ||
          _params->hiLoChartSubType() == KDChartParams::HiLoOpenClose ) ) {
        _hlcChart->apply();

        //_backgroundpixpage->apply();

        // data in the params struct has changed; notify application
        //emit dataChanged();

        //     for( uint i = 0; i < NUMDATACOLORS; i++ )
        // 	_params->_datacolors.setColor( i, _colorpage->dataColor( i ) );
    }
    // data in the params struct has changed; notify application
    emit dataChanged();
}

void KChartConfigDialog::defaults()
{
    // color page
    _colorpage->setHdFtColor(     KDChartParams::HdFtPosHeader,
      _params->headerFooterColor( KDChartParams::HdFtPosHeader ) );
    _colorpage->setHdFtColor(     KDChartParams::HdFtPosHeader2,
      _params->headerFooterColor( KDChartParams::HdFtPosHeader2 ) );
    _colorpage->setHdFtColor(     KDChartParams::HdFtPosFooter,
      _params->headerFooterColor( KDChartParams::HdFtPosFooter ) );

    bool bFound;
    const KDChartParams::KDChartFrameSettings * innerFrame =
      _params->frameSettings( KDChartEnums::AreaInnermost, bFound );
    if( bFound )
    {
         const QPixmap* backPixmap;
         KDFrame::BackPixmapMode backPixmapMode;
         const QBrush& background = innerFrame->frame().background( backPixmap, backPixmapMode );
         if( ! backPixmap || backPixmap->isNull() ) {
            _colorpage->setBackgroundColor( background.color() );
         }
         // pending KHZ
         // else
         //     ..  // set the background pixmap
    }
    else
        _colorpage->setBackgroundColor( QColor(230, 222, 222) );

    _colorpage->setLineColor( _params->outlineDataColor() );
    KDChartAxisParams leftparams( _params->axisParams( KDChartAxisParams::AxisPosLeft ) );
    KDChartAxisParams rightparams( _params->axisParams( KDChartAxisParams::AxisPosRight ) );
    KDChartAxisParams bottomparams( _params->axisParams( KDChartAxisParams::AxisPosBottom ) );
    _colorpage->setGridColor( leftparams.axisGridColor() );
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

    if( ( _params->chartType() == KDChartParams::Bar &&
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

    //_backgroundpixpage->init();
//     for( uint i = 0; i < NUMDATACOLORS; i++ )
//      	_colorpage->setDataColor( i, _params->dataColor( i ) );
}



