/**
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#include <qdom.h>
#include <qtextstream.h>
#include <qbuffer.h>
#include "kchart_part.h"
#include "kchart_view.h"
#include "kchart_factory.h"
#include "kchartWizard.h"
#include <kstandarddirs.h>

#include <kglobal.h>
#include <kdebug.h> // "ported" to kdDebug(35001)

#include "kchart_params.h"
#include "kdchart/KDChart.h"

using namespace std;

// Some hardcoded data for a chart

/* ----- set some data ----- */
// float   a[6]  = { 0.5, 0.09, 0.6, 0.85, 0.0, 0.90 },
// b[6]  = { 1.9, 1.3,  0.6, 0.75, 0.1, -2.0 };
/* ----- X labels ----- */
// char    *t[6] = { "Chicago", "New York", "L.A.", "Atlanta", "Paris, MD\n(USA) ", "London" };
/* ----- data set colors (RGB) ----- */
// QColor   sc[2]    = { QColor( 255, 128, 128 ), QColor( 128, 128, 255 ) };


#include <qpainter.h>

KChartPart::KChartPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoChart::Part( parentWidget, widgetName, parent, name, singleViewMode ),
      _params( 0 ),
      _parentWidget( parentWidget )
{
    m_bLoading = false;
    kdDebug(35001) << "Constructor started!" << endl;

    setInstance( KChartFactory::global(), false );

    (void)new WizardExt( this );
    initDoc();
    m_bCanChangeValue=true;
    // hack
    setModified(true);
}

KChartPart::~KChartPart()
{
    kdDebug(35001) << "Part is going to be destroyed now!!!" << endl;
    delete _params;
}


bool KChartPart::initDoc()
{
    // Initialize the parameter set for this chart document
    kdDebug(35001) << "InitDOC" << endl;
    _params = new KChartParams();
    _params->setThreeDBars( true );

    return TRUE;
}

void KChartPart::initRandomData()
{
    kdDebug()<<"KChartPart::initRandomData()\n";
    // fill cells
    int col,row;
    // initialize some data, if there is none
    if (currentData.rows() == 0) {
        kdDebug(35001) << "Initialize with some data!!!" << endl;
        currentData.expand(4,4);
        currentData.setUsedRows( 4 );
        currentData.setUsedCols( 4 );
        for (row = 0;row < 4;row++)
            for (col = 0;col < 4;col++) {
                KoChart::Value t( (double)row+col );
                // kdDebug(35001) << "Set cell for " << row << "," << col << endl;
                currentData.setCell(row,col,t);
            }
    }

}



KoView* KChartPart::createViewInstance( QWidget* parent, const char* name )
{
    return new KChartView( this, parent, name );
}

void KChartPart::paintContent( QPainter& painter, const QRect& rect, bool transparent, double /*zoomX*/, double /*zoomY*/ )
{
    if (isLoading()) {
        kdDebug(35001) << "Loading... Do not paint!!!..." << endl;
        return;
    }
    // if params is 0, initDoc() has not been called
    Q_ASSERT( _params != 0 );

    // ####### handle transparency
    if( !transparent )
        painter.eraseRect( rect );

    // ## TODO: support zooming

    // kdDebug(35001) << "KChartPart::paintContent called, rows = "
    //                << currentData.usedRows() << ", cols = "
    //                << currentData.usedCols() << endl;

    // Need to draw only the document rectangle described in the parameter rect.
    //  return;
    KDChart::paint( &painter, _params, &currentData, 0, &rect );
}


void KChartPart::setData( const KoChart::Data& data )
{
    // FIXME(khz): replace this when automatic string detection works in KDChart
    //currentData = data;

    uint col, row;

    //Does the top/left cell contain a srting?
    bool isStringTopLeft = data.cell( 0, 0 ).isString();

    //Does the first row (without first cell) contain only strings
    bool isStringFirstRow = TRUE;
    for( uint col = 1; isStringFirstRow && col < data.cols(); col++ )
    {
        isStringFirstRow = data.cell( 0, col ).isString();
    }
    //Just in case, we only have 1 row, we never use it for label text => prevents crash
    if( data.rows() == 1 )
        isStringFirstRow = FALSE;

    //Does the first column (without first cell) contain only strings
    bool isStringFirstCol = TRUE;
    for( uint row = 1; isStringFirstCol && row < data.rows(); row++ )
    {
        isStringFirstCol = data.cell( row, 0 ).isString();
    }
    //Just in case, we only have 1 column, we never use it for axis label text => prevents crash
    if( data.cols() == 1 )
        isStringFirstRow = FALSE;

    uint colStart = 0;
    uint rowStart = 0;
    bool hasColHeader = FALSE;
    bool hasRowHeader = FALSE;

    //Let's check if we have a full axis label text column
    if( isStringFirstCol && isStringTopLeft || isStringFirstCol && isStringFirstRow )
    {
        hasColHeader = TRUE;
        colStart = 1;
    }

    //Let's check if we have a full label text row
    if( isStringFirstRow && isStringTopLeft || isStringFirstCol && isStringFirstRow )
    {
        hasRowHeader = TRUE;
        rowStart = 1;
    }

    if( hasColHeader )
    {
        _params->setLegendSource( KDChartParams::LegendManual );
        for( row = rowStart; row < data.rows(); row++ )
        {
            _params->setLegendText( row - rowStart, data.cell( row, 0 ).stringValue() );
        }
    }
    else
        _params->setLegendSource( KDChartParams::LegendAutomatic );

    if( hasRowHeader )
    {
        KDChartAxisParams bottomparms = _params->axisParams( KDChartAxisParams::AxisPosBottom );
        longLabels.clear();
        shortLabels.clear();
        for( uint col = colStart; col < data.cols(); col++ )
        {
            longLabels << data.cell( 0, col ).stringValue();
            shortLabels << data.cell( 0, col ).stringValue().left( 3 );
        }
        bottomparms.setAxisLabelStringLists( &longLabels, &shortLabels );
        _params->setAxisParams( KDChartAxisParams::AxisPosBottom, bottomparms );
    }
    else
    {
        longLabels.clear();
        shortLabels.clear();
    }

    if( hasColHeader || hasRowHeader )
    {
        KoChart::Data matrix( data.rows() - rowStart, data.cols() - colStart );
        for( col = colStart; col < data.cols(); col++ )
        {
            for( row = rowStart; row < data.rows(); row++ )
            {
                matrix.setCell( row - rowStart, col - colStart, KoChart::Value( data.cell( row, col ).doubleValue() ) );
            }
        }
        currentData = matrix;
    }
    else
        currentData = data;

    //  initLabelAndLegend();
    emit docChanged();
}


bool KChartPart::showWizard()
{
    KChartWizard* wizard = new KChartWizard( this, _parentWidget, "wizard" );
    bool ret = wizard->exec();
    delete wizard;
    return ret;
}

void KChartPart::initLabelAndLegend()
{
    // Labels and legends are automatically initialized to reasonable
    // default values in KDChart
}

void KChartPart::loadConfig( KConfig *conf )
{
    conf->setGroup("ChartParameters");

    // TODO: the fonts
    // PENDING(kalle) Put the applicable ones of these back in
    //   QFont tempfont;
    //   tempfont = conf->readFontEntry("titlefont", &titlefont);
    //   setTitleFont(tempfont);
    //   tempfont = conf->readFontEntry("ytitlefont", &ytitlefont);
    //   setYTitleFont(tempfont);
    //   tempfont = conf->readFontEntry("xtitlefont", &xtitlefont);
    //   setXTitleFont(tempfont);
    //   tempfont = conf->readFontEntry("yaxisfont", &yaxisfont);
    //   setYAxisFont(tempfont);
    //   tempfont = conf->readFontEntry("xaxisfont", &xaxisfont);
    //   setXAxisFont(tempfont);
    //   tempfont = conf->readFontEntry("labelfont", &labelfont);
    //   setLabelFont(tempfont);
    //   tempfont = conf->readFontEntry("annotationfont", &annotationfont);
    //   setAnnotationFont(tempfont);

    //   ylabel_fmt = conf->readEntry("ylabel_fmt", ylabel_fmt );
    //   ylabel2_fmt = conf->readEntry("ylabel2_fmt", ylabel2_fmt);
    //   xlabel_spacing = conf->readNumEntry("xlabel_spacing");
    //   ylabel_density = conf->readNumEntry("ylabel_density", ylabel_density);
    //   requested_ymin = conf->readDoubleNumEntry("requested_ymin", requested_ymin);
    //   requested_ymax = conf->readDoubleNumEntry("requested_ymax", requested_ymax );
    //   requested_yinterval = conf->readDoubleNumEntry("requested_yinterval",
    // 					   requested_yinterval);
    //   shelf = conf->readBoolEntry("shelf", shelf);
    //   grid = conf->readBoolEntry("grid", grid);
    //   xaxis = conf->readBoolEntry("xaxis", xaxis);
    //   yaxis = conf->readBoolEntry("yaxis", yaxis);
    //   yaxis2 = conf->readBoolEntry("yaxis2", yaxis);
    //   llabel = conf->readBoolEntry("llabel", llabel);
    //   yval_style = conf->readNumEntry("yval_style", yval_style);
    //   stack_type = (KChartStackType)conf->readNumEntry("stack_type", stack_type);
    _params->setLineMarker(conf->readBoolEntry("lineMarker",_params->lineMarker()));
    _params->setThreeDBarDepth( conf->readDoubleNumEntry("_3d_depth",
                                                         _params->threeDBarDepth() ) );
    _params->setThreeDBarAngle( conf->readNumEntry( "_3d_angle",
                                                          _params->threeDBarAngle() ) );

    KDChartAxisParams leftparams = _params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams rightparams = _params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams bottomparams = _params->axisParams( KDChartAxisParams::AxisPosBottom );
    bottomparams.setAxisLineColor( conf->readColorEntry( "XTitleColor", 0 ) );
    leftparams.setAxisLineColor( conf->readColorEntry( "YTitleColor", 0 ) );
    rightparams.setAxisLineColor( conf->readColorEntry( "YTitle2Color", 0 ) );
    bottomparams.setAxisLabelsColor( conf->readColorEntry( "XLabelColor", 0 ) );
    leftparams.setAxisLabelsColor( conf->readColorEntry( "YLabelColor", 0 ) );
    rightparams.setAxisLabelsColor( conf->readColorEntry( "YLabel2Color", 0 ) );
    leftparams.setAxisGridColor( conf->readColorEntry( "GridColor", 0 ) );
    _params->setOutlineDataColor( conf->readColorEntry( "LineColor", 0 ) );
    _params->setAxisParams( KDChartAxisParams::AxisPosLeft,
                            leftparams );
    _params->setAxisParams( KDChartAxisParams::AxisPosRight,
                            rightparams );
    _params->setAxisParams( KDChartAxisParams::AxisPosBottom,
                            bottomparams );

    //   hlc_style = (KChartHLCStyle)conf->readNumEntry("hlc_style", hlc_style);
    //   hlc_cap_width = conf->readNumEntry("hlc_cap_width", hlc_cap_width);
    //   // TODO: Annotation font
    //   num_scatter_pts = conf->readNumEntry("num_scatter_pts", num_scatter_pts);
    //   // TODO: Scatter type
    //   thumbnail = conf->readBoolEntry("thumbnail", thumbnail);
    //   thumblabel = conf->readEntry("thumblabel", thumblabel);
    //   border = conf->readBoolEntry("border", border);
    //   BGColor = conf->readColorEntry("BGColor", &BGColor);
    //   PlotColor = conf->readColorEntry("PlotColor", &PlotColor);
    //   VolColor = conf->readColorEntry("VolColor", &VolColor);
    //   EdgeColor = conf->readColorEntry("EdgeColor", &EdgeColor);
    //   loadColorArray(conf, &SetColor, "SetColor");
    //   loadColorArray(conf, &ExtColor, "ExtColor");
    //   loadColorArray(conf, &ExtVolColor, "ExtVolColor");
    //   transparent_bg = conf->readBoolEntry("transparent_bg", transparent_bg);
    //   // TODO: explode, missing
    //   percent_labels = (KChartPercentType)conf->readNumEntry("percent_labels",
    // 							 percent_labels);
    //   label_dist = conf->readNumEntry("label_dist", label_dist);
    //   label_line = conf->readBoolEntry("label_line", label_line);
    _params->setChartType( (KDChartParams::ChartType)conf->readNumEntry( "type", _params->chartType() ) );
    //   other_threshold = conf->readNumEntry("other_threshold", other_threshold);

    //   backgroundPixmapName = conf->readPathEntry( "backgroundPixmapName" );
    //   if( !backgroundPixmapName.isNull() ) {
    //     backgroundPixmap.load( locate( "wallpaper", backgroundPixmapName ));
    //     backgroundPixmapIsDirty = true;
    //   } else
    //     backgroundPixmapIsDirty = false;
    //   backgroundPixmapScaled = conf->readBoolEntry( "backgroundPixmapScaled", true );
    //   backgroundPixmapCentered = conf->readBoolEntry( "backgroundPixmapCentered", true );
    //   backgroundPixmapIntensity = conf->readDoubleNumEntry( "backgroundPixmapIntensity", 0.25 );
}

void KChartPart::defaultConfig(  )
{
    delete _params;
    _params = new KChartParams();
}

void KChartPart::saveConfig( KConfig *conf )
{
    conf->setGroup("ChartParameters");

    // PENDING(kalle) Put some of these back in
    // the fonts
    //   conf->writeEntry("titlefont", titlefont);
    //   conf->writeEntry("ytitlefont", ytitlefont);
    //   conf->writeEntry("xtitlefont", xtitlefont);
    //   conf->writeEntry("yaxisfont", yaxisfont);
    //   conf->writeEntry("xaxisfont", xaxisfont);
    //   conf->writeEntry("labelfont", labelfont);

    //   conf->writeEntry("ylabel_fmt", ylabel_fmt);
    //   conf->writeEntry("ylabel2_fmt", ylabel2_fmt);
    //   conf->writeEntry("xlabel_spacing", xlabel_spacing);
    //   conf->writeEntry("ylabel_density", ylabel_density);
    //   conf->writeEntry("requested_ymin", requested_ymin);
    //   conf->writeEntry("requested_ymax", requested_ymax);
    //   conf->writeEntry("requested_yinterval", requested_yinterval);

    //   conf->writeEntry("shelf", shelf);
    //   conf->writeEntry("grid", grid );
    //   conf->writeEntry("xaxis", xaxis);
    //   conf->writeEntry("yaxis", yaxis);
    //   conf->writeEntry("yaxis2", yaxis2);
    //   conf->writeEntry("llabel", llabel);
    //   conf->writeEntry("yval_style", yval_style );
    //   conf->writeEntry("stack_type", (int)stack_type);

    conf->writeEntry( "_3d_depth", _params->threeDBarDepth() );
    conf->writeEntry( "_3d_angle", _params->threeDBarAngle() );

    KDChartAxisParams leftparams = _params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams rightparams = _params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams bottomparams = _params->axisParams( KDChartAxisParams::AxisPosBottom );
    conf->writeEntry( "LineColor", _params->outlineDataColor() );
    conf->writeEntry( "XTitleColor", bottomparams.axisLineColor() );
    conf->writeEntry( "YTitleColor", leftparams.axisLineColor() );
    conf->writeEntry( "YTitle2Color", rightparams.axisLineColor() );
    conf->writeEntry( "XLabelColor", bottomparams.axisLabelsColor() );
    conf->writeEntry( "YLabelColor", leftparams.axisLabelsColor() );
    conf->writeEntry( "YLabel2Color", rightparams.axisLabelsColor() );
    conf->writeEntry( "GridColor", leftparams.axisGridColor() );

    //   conf->writeEntry("hlc_style", (int)hlc_style);
    //   conf->writeEntry("hlc_cap_width", hlc_cap_width );
    //   // TODO: Annotation type!!!
    //   conf->writeEntry("annotationfont", annotationfont);
    //   conf->writeEntry("num_scatter_pts", num_scatter_pts);
    //   // TODO: Scatter type!!!
    //   conf->writeEntry("thumbnail", thumbnail);
    //   conf->writeEntry("thumblabel", thumblabel);
    //   conf->writeEntry("thumbval", thumbval);
    //   conf->writeEntry("border", border);
    //   conf->writeEntry("BGColor", BGColor);
    //   conf->writeEntry("PlotColor", PlotColor);
    //   conf->writeEntry("VolColor", VolColor);
    //   conf->writeEntry("EdgeColor", EdgeColor);
    //   saveColorArray(conf, &SetColor, "SetColor");
    //   saveColorArray(conf, &ExtColor, "ExtColor");
    //   saveColorArray(conf, &ExtVolColor, "ExtVolColor");


    //   conf->writeEntry("transparent_bg", transparent_bg);
    //   // TODO: explode, missing
    //   conf->writeEntry("percent_labels",(int) percent_labels );
    //   conf->writeEntry("label_dist", label_dist);
    //   conf->writeEntry("label_line", label_line);
    conf->writeEntry( "type", (int)_params->chartType() );
    //   conf->writeEntry("other_threshold", other_threshold);

    // background pixmap stuff
    //   if( !backgroundPixmapName.isNull() )
    // 	conf->writePathEntry( "backgroundPixmapName", backgroundPixmapName );
    //   conf->writeEntry( "backgroundPixmapIsDirty", backgroundPixmapIsDirty );
    //   conf->writeEntry( "backgroundPixmapScaled", backgroundPixmapScaled );
    //   conf->writeEntry( "backgroundPixmapCentered", backgroundPixmapCentered );
    //   conf->writeEntry( "backgroundPixmapIntensity", backgroundPixmapIntensity );
    conf->writeEntry( "lineMarker",(int)_params->lineMarker());
}

QDomDocument KChartPart::saveXML()
{
    kdDebug(35001) << "kchart saveXML called" << endl;
    QDomDocument doc = _params->saveXML( false );

    // save the data values
    QDomElement docRoot = doc.documentElement();
    QDomElement data = doc.createElement( "data" );
    docRoot.appendChild( data );

    int cols = currentData.usedCols()
             ? QMIN(currentData.usedCols(), currentData.cols())
             : currentData.cols();
    int rows = currentData.usedRows()
             ? QMIN(currentData.usedRows(), currentData.rows())
             : currentData.rows();
    data.setAttribute( "cols", cols );
    data.setAttribute( "rows", rows );
    kdDebug(35001) << "      writing  " << cols << "," << rows << "  (cols,rows)." << endl;

    for (int i=0; i!=rows; ++i) {
        for (int j=0; j!=cols; ++j) {
            QDomElement e = doc.createElement( "cell" );
            data.appendChild( e );
            QString valType;
            KoChart::Value & cell( currentData.cell( i,j ) );
            switch ( cell.valueType() ) {
                case KoChart::Value::NoValue: valType = "NoValue";
                              break;
                case KoChart::Value::String:  valType = "String";
                              break;
                case KoChart::Value::Double:  valType = "Double";
                              break;
                case KoChart::Value::DateTime:valType = "DateTime";
                              break;
                default: {
                    valType = "(unknown)";
                    kdDebug(35001) << "ERROR: cell " << i << "," << j << " has unknown type." << endl;
                }
            }
            e.setAttribute( "valType", valType );
            kdDebug(35001) << "      cell " << i << "," << j << " saved with type '" << valType << "'." << endl;
            switch ( cell.valueType() ) {
                case KoChart::Value::String:  e.setAttribute( "value", cell.stringValue() );
                              break;
                case KoChart::Value::Double:  e.setAttribute( "value", QString::number( cell.doubleValue() ) );
                              break;
                case KoChart::Value::DateTime:e.setAttribute( "value", "" );
                              break;
                default: {
                    e.setAttribute( "value", "" );
                    if( KoChart::Value::NoValue != cell.valueType() )
                        kdDebug(35001) << "ERROR: cell " << i << "," << j << " has unknown type." << endl;
                }
            }
        }
    }

    return doc;
}

bool KChartPart::loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles, const QDomDocument& settings, KoStore* )
{
    //todo
    return false;
}

bool KChartPart::loadXML( QIODevice*, const QDomDocument& doc )
{
    kdDebug(35001) << "kchart loadXML called" << endl;
    bool result=_params->loadXML( doc );
    if(!result)
    {
        //try to load old file format
        result=loadOldXML( doc );
    }
    if( result )
    {
        result = loadData( doc, currentData );
#if 0
        bool retData = loadData( doc, currentData );
        if ( !retData )
            initRandomData();
#endif
    }
    return result;
}


bool KChartPart::loadData( const QDomDocument& doc, KoChart::Data& currentData )
{
    kdDebug(35001) << "kchart loadData called" << endl;

    QDomElement chart = doc.documentElement();
    QDomElement data = chart.namedItem("data").toElement();
    bool ok;
    int cols = data.attribute("cols").toInt(&ok);
    kdDebug(35001) << "cols readed as:" << cols << endl;
    if( !ok ){
         return false;
    }
    int rows = data.attribute("rows").toInt(&ok);
    if( !ok ){
         return false;
    }
    kdDebug(35001) << rows << " x " << cols << endl;
    currentData.expand(rows, cols);
    currentData.setUsedCols( cols );
    currentData.setUsedRows( rows );
    kdDebug(35001) << "Expanded!" << endl;
    QDomNode n = data.firstChild();
    //QArray<int> tmpExp(rows*cols);
    //QArray<bool> tmpMissing(rows*cols);
    for (int i=0; i!=rows; i++) {
        for (int j=0; j!=cols; j++) {
            if (n.isNull()) {
                kdDebug(35001) << "Some problems, there is less data than it should be!" << endl;
                break;
            }
            QDomElement e = n.toElement();
            if ( !e.isNull() && e.tagName() == "cell" ) {
                // add the cell to the corresponding place...
                KoChart::Value t;
                if( e.hasAttribute("value") && e.hasAttribute("valType") ) {
                    QString valueType = e.attribute("valType").lower();
                    if( "string" == valueType ) {
                        QString strVal = e.attribute("value");
                        t = KoChart::Value( strVal );
                    }
                    else if( "double" == valueType ) {
                        bool bOk;
                        double val = e.attribute("value").toDouble(&bOk);
                        if( !bOk )
                            val = 0.0;
                        t = KoChart::Value( val );
                    /*
                    } else if( "datetime" == valueType ) {
                        t = . . .
                    */
                    } else {
                        t.clearValue();
                        if( "novalue" != valueType )
                            kdDebug(35001) << "ERROR: cell " << i << "," << j << " has unknown type '" << valueType << "'." << endl;
                    }
                } else
                    t.clearValue();

                currentData.setCell(i,j, t );

		/*
                if ( e.hasAttribute( "hide" ) ) {
                    tmpMissing[cols*j+i] = (bool)e.attribute("hide").toInt( &ok );
                    if( !ok )
                        return false;
                } else {
                    tmpMissing[cols*j+i] = false;
                }
                if( e.hasAttribute( "dist" ) ) {
                    tmpExp[cols*j+i] = e.attribute("dist").toInt( &ok );
                    if ( !ok )
                        return false;
                } else {
                    tmpExp[cols*j+i] = 0;
                }
		*/

                n = n.nextSibling();
            }
        }
    }
    /*
    _params->missing=tmpMissing;
    _params->explode=tmpExp;
    */
    return true;
}



bool KChartPart::loadOldXML( const QDomDocument& doc )
{
    kdDebug(35001) << "kchart loadOldXML called" << endl;
    // <spreadsheet>
    //  m_bLoading = true;
    if ( doc.doctype().name() != "chart" ) {
        //m_bLoading = false;
        return false;
    }

    kdDebug(35001) << "Ok, it is a chart" << endl;

    QDomElement chart = doc.documentElement();
    if ( chart.attribute( "mime" ) != "application/x-kchart" && chart.attribute( "mime" ) != "application/vnd.kde.kchart" )
        return false;

    kdDebug(35001) << "Mimetype ok" << endl;

#if 0
    QDomElement data = chart.namedItem("data").toElement();
    bool ok;
    int cols = data.attribute("cols").toInt(&ok);
    kdDebug(35001) << "cols readed as:" << cols << endl;
    if (!ok)  { return false; }
    int rows = data.attribute("rows").toInt(&ok);
    if (!ok)  { return false; }
    kdDebug(35001) << rows << " x " << cols << endl;
    currentData.expand(rows, cols);
    kdDebug(35001) << "Expanded!" << endl;
    QDomNode n = data.firstChild();
    QArray<int> tmpExp(rows*cols);
    QArray<bool> tmpMissing(rows*cols);

    for (int i=0; i!=rows; i++) {
        for (int j=0; j!=cols; j++) {
            if (n.isNull()) {
                kdDebug(35001) << "Some problems, there is less data than it should be!" << endl;
                break;
            }
            QDomElement e = n.toElement();
            if ( !e.isNull() && e.tagName() == "cell" ) {
                // add the cell to the corresponding place...
                double val = e.attribute("value").toDouble(&ok);
                if (!ok)  {  return false; }
                kdDebug(35001) << i << " " << j << "=" << val << endl;
                KoChart::Value t( val );
                // kdDebug(35001) << "Set cell for " << row << "," << col << endl;
                currentData.setCell(i,j,t);
                if ( e.hasAttribute( "hide" ) ) {
                    tmpMissing[cols*j+i] = (bool)e.attribute("hide").toInt( &ok );
                    if ( !ok )
                        return false;
                } else {
                    tmpMissing[cols*j+i] = false;
                }
                if( e.hasAttribute( "dist" ) ) {
                    tmpExp[cols*j+i] = e.attribute("dist").toInt( &ok );
                    if ( !ok )
                        return false;
                } else {
                    tmpExp[cols*j+i] = 0;
                }

                n = n.nextSibling();
            }
        }
    }
    _params->missing=tmpMissing;
    _params->explode=tmpExp;
#endif


/*
  enum KChartType {
  KCHARTTYPE_LINE,
  KCHARTTYPE_AREA,
  KCHARTTYPE_BAR,
  KCHARTTYPE_HILOCLOSE,
  KCHARTTYPE_COMBO_LINE_BAR, aka, VOL[ume]
  KCHARTTYPE_COMBO_HLC_BAR,
  KCHARTTYPE_COMBO_LINE_AREA,
  KCHARTTYPE_COMBO_HLC_AREA,
  KCHARTTYPE_3DHILOCLOSE,
  KCHARTTYPE_3DCOMBO_LINE_BAR,
  KCHARTTYPE_3DCOMBO_LINE_AREA,
  KCHARTTYPE_3DCOMBO_HLC_BAR,
  KCHARTTYPE_3DCOMBO_HLC_AREA,
  KCHARTTYPE_3DBAR,
  KCHARTTYPE_3DAREA,
  KCHARTTYPE_3DLINE,
  KCHARTTYPE_3DPIE,
  KCHARTTYPE_2DPIE
  };
*/
    bool ok;
    QDomElement params = chart.namedItem( "params" ).toElement();
    if( params.hasAttribute( "type" ) ) {
        int type=params.attribute("type").toInt( &ok );
        if ( !ok )
            return false;
        switch(type)
        {
        case 1:
            _params->setChartType(KDChartParams::Line);
            break;
        case 2:
            _params->setChartType(KDChartParams::Area);
            break;
        case 3:
            _params->setChartType(KDChartParams::Bar);
            break;
        case 4:
            _params->setChartType(KDChartParams::HiLo);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            /*     KCHARTTYPE_COMBO_LINE_BAR, aka, VOL[ume]
                   KCHARTTYPE_COMBO_HLC_BAR,
                   KCHARTTYPE_COMBO_LINE_AREA,
                   KCHARTTYPE_COMBO_HLC_AREA,
            */
            /* line by default*/
            _params->setChartType(KDChartParams::Line);
            break;
        case 9:
            _params->setChartType(KDChartParams::HiLo);
            break;
        case 10:
            _params->setChartType(KDChartParams::Bar);
            break;
        case 11:
            _params->setChartType(KDChartParams::Area);
            break;
        case 12:
            _params->setChartType(KDChartParams::Bar);
            break;
        case 13:
            _params->setChartType(KDChartParams::Area);
            break;
        case 14:
            _params->setChartType(KDChartParams::Bar);
            break;
        case 15:
            _params->setChartType(KDChartParams::Area);
            break;
        case 16:
            _params->setChartType(KDChartParams::Line);
            break;
        case 17:
        case 18:
            _params->setChartType(KDChartParams::Pie);
            break;

        }
        if ( !ok )
            return false;
    }
#if 0
    if( params.hasAttribute( "subtype" ) ) {
        _params->stack_type = (KChartStackType)params.attribute("subtype").toInt( &ok );
        if ( !ok )
            return false;
    }
    if( params.hasAttribute( "hlc_style" ) ) {
        _params->hlc_style = (KChartHLCStyle)params.attribute("hlc_style").toInt( &ok );
        if ( !ok )
            return false;
    }
    if( params.hasAttribute( "hlc_cap_width" ) ) {
        _params->hlc_cap_width = (short)params.attribute( "hlc_cap_width" ).toShort( &ok );
        if( !ok )
            return false;
    }

    QDomElement title = params.namedItem( "title" ).toElement();
    if( !title.isNull()) {
        QString t = title.text();
        _params->title=t;
    }
    QDomElement titlefont = params.namedItem( "titlefont" ).toElement();
    if( !titlefont.isNull()) {
        QDomElement font = titlefont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            _params->setTitleFont(toFont(font));
    }
    QDomElement xtitle = params.namedItem( "xtitle" ).toElement();
    if( !xtitle.isNull()) {
        QString t = xtitle.text();
        _params->xtitle=t;
    }
    QDomElement xtitlefont = params.namedItem( "xtitlefont" ).toElement();
    if( !xtitlefont.isNull()) {
        QDomElement font = xtitlefont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            _params->setXTitleFont(toFont(font));
    }
    QDomElement ytitle = params.namedItem( "ytitle" ).toElement();
    if( !ytitle.isNull()) {
        QString t = ytitle.text();
        _params->ytitle=t;
    }
    QDomElement ytitle2 = params.namedItem( "ytitle2" ).toElement();
    if( !ytitle2.isNull()) {
        QString t = ytitle2.text();
        _params->ytitle2=t;
    }
    QDomElement ytitlefont = params.namedItem( "ytitlefont" ).toElement();
    if( !ytitlefont.isNull()) {
        QDomElement font = ytitlefont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            _params->setYTitleFont(toFont(font));
    }
    QDomElement ylabelfmt = params.namedItem( "ylabelfmt" ).toElement();
    if( !ylabelfmt.isNull()) {
        QString t = ylabelfmt.text();
        _params->ylabel_fmt=t;
    }
    QDomElement ylabel2fmt = params.namedItem( "ylabel2fmt" ).toElement();
    if( !ylabel2fmt.isNull()) {
        QString t = ylabel2fmt.text();
        _params->ylabel2_fmt=t;
    }
    QDomElement labelfont = params.namedItem( "labelfont" ).toElement();
    if( !labelfont.isNull()) {
        QDomElement font = labelfont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            _params->setLabelFont(toFont(font));
    }

    QDomElement yaxisfont = params.namedItem( "yaxisfont" ).toElement();
    if( !yaxisfont.isNull()) {
        QDomElement font = yaxisfont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            _params->setYAxisFont(toFont(font));
    }

    QDomElement xaxisfont = params.namedItem( "xaxisfont" ).toElement();
    if( !xaxisfont.isNull()) {
        QDomElement font = xaxisfont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            _params->setXAxisFont(toFont(font));
    }
    QDomElement annotationFont = params.namedItem("annotationfont").toElement();
    if( !annotationFont.isNull()) {
        QDomElement font = annotationFont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            _params->setAnnotationFont(toFont(font));
    }

    QDomElement yaxis = params.namedItem( "yaxis" ).toElement();
    if( !yaxis.isNull()) {
        if(yaxis.hasAttribute( "yinterval" )) {
            _params->requested_yinterval= yaxis.attribute("yinterval").toDouble( &ok );
            if ( !ok ) return false;
        }
        if(yaxis.hasAttribute( "ymin" )) {
            _params->requested_ymin= yaxis.attribute("ymin").toDouble( &ok );
            if ( !ok ) return false;
        }
        if(yaxis.hasAttribute( "ymax" ) ) {
            _params->requested_ymax= yaxis.attribute("ymax").toDouble( &ok );
            if ( !ok ) return false;
        }
    }
#endif

    QDomElement graph = params.namedItem( "graph" ).toElement();
    if(!graph.isNull()) {
        if(graph.hasAttribute( "grid" )) {
            bool b=(bool) graph.attribute("grid").toInt( &ok );
            _params->setAxisShowGrid(KDChartAxisParams::AxisPosLeft,b );
            _params->setAxisShowGrid(KDChartAxisParams::AxisPosBottom,b );
            if(!ok) return false;
        }
        if(graph.hasAttribute( "xaxis" )) {
            bool b=(bool) graph.attribute("xaxis").toInt( &ok );
            if(!ok) return false;
            _params->setAxisVisible(KDChartAxisParams::AxisPosBottom,b);
        }
        if(graph.hasAttribute( "yaxis" )) {
            bool b=(bool) graph.attribute("yaxis").toInt( &ok );
            if(!ok) return false;
            _params->setAxisVisible(KDChartAxisParams::AxisPosLeft,b);
        }
#if 0
        //no implemented
        if(graph.hasAttribute( "shelf" )) {
            _params->shelf=(bool) graph.attribute("shelf").toInt( &ok );
            if(!ok) return false;
        }
#endif
        if(graph.hasAttribute( "yaxis2" )) {
            bool b=(bool) graph.attribute("yaxis2").toInt( &ok );
            if(!ok) return false;
            _params->setAxisVisible(KDChartAxisParams::AxisPosRight,b);
        }

#if 0
        //no implemented
        if(graph.hasAttribute( "ystyle" )) {
            _params->yval_style=(bool) graph.attribute("ystyle").toInt( &ok );
            if(!ok) return false;
        }
        if(graph.hasAttribute( "border" )) {
            _params->border=(bool) graph.attribute("border").toInt( &ok );
            if(!ok) return false;
        }
        if(graph.hasAttribute( "transbg" )) {
            _params->transparent_bg=(bool) graph.attribute("transbg").toInt( &ok );
            if(!ok) return false;
        }
        if(graph.hasAttribute( "xlabel" )) {
            _params->hasxlabel=(bool) graph.attribute("xlabel").toInt( &ok );
            if(!ok) return false;
        }
        if( graph.hasAttribute( "xlabel_spacing" ) ) {
            _params->xlabel_spacing = (short)graph.attribute( "xlabel_spacing" ).toShort( &ok );
            if( !ok )
                return false;
        }
        if( graph.hasAttribute( "ylabel_density" ) ) {
            _params->ylabel_density = (short)graph.attribute( "ylabel_density" ).toShort( &ok );
            if( !ok )
                return false;
        }
        if(graph.hasAttribute( "line")) {
            _params->label_line=(bool) graph.attribute("line").toInt( &ok );
            if(!ok) return false;
        }
        if(graph.hasAttribute( "percent")) {
            _params->percent_labels=(KChartPercentType) graph.attribute("percent").toInt( &ok );
            if(!ok) return false;
        }
        if(graph.hasAttribute("cross")) {
            _params->cross=(bool) graph.attribute("cross").toInt( &ok );
            if(!ok) return false;
        }
        if(graph.hasAttribute("thumbnail")) {
            _params->thumbnail=(bool) graph.attribute("thumbnail").toInt( &ok );
            if(!ok) return false;
        }
        if(graph.hasAttribute("thumblabel")) {
            _params->thumblabel= graph.attribute("thumblabel");
        }
        if(graph.hasAttribute("thumbval")) {
            _params->thumbval=(bool) graph.attribute("thumbval").toDouble( &ok );
            if(!ok)
                return false;
        }
#endif
    }

#if 0
    QDomElement graphparams = params.namedItem( "graphparams" ).toElement();
    if(!graphparams.isNull()) {
        if(graphparams.hasAttribute( "dept3d" )) {
            _params->_3d_depth=graphparams.attribute("dept3d").toDouble( &ok );
            if(!ok) return false;
        }
        if(graphparams.hasAttribute( "angle3d" )) {
            _params->_3d_angle=graphparams.attribute("angle3d").toShort( &ok );
            if(!ok) return false;
        }
        if(graphparams.hasAttribute( "barwidth" )) {
            _params->bar_width=graphparams.attribute("barwidth").toShort( &ok );
            if(!ok) return false;
        }
        if(graphparams.hasAttribute( "colpie" )) {
            _params->colPie=graphparams.attribute("colpie").toInt( &ok );
            if(!ok) return false;
        }
        if(graphparams.hasAttribute( "other_threshold" )) {
            _params->other_threshold=graphparams.attribute("other_threshold").toShort( &ok );
            if(!ok)
                return false;
        }
        if(graphparams.hasAttribute( "offsetCol" )) {
            _params->offsetCol = graphparams.attribute("offsetCol").toInt( &ok );
            if(!ok)
                return false;
        }
        if(graphparams.hasAttribute( "hard_size" )) {
            _params->hard_size = (bool)graphparams.attribute("hard_size").toInt( &ok );
            if(!ok)
                return false;
        }
        if(graphparams.hasAttribute( "hard_graphheight" )) {
            _params->hard_graphheight = graphparams.attribute("hard_graphheight").toInt( &ok );
            if(!ok)
                return false;
        }
        if(graphparams.hasAttribute( "hard_graphwidth" )) {
            _params->hard_graphwidth = graphparams.attribute("hard_graphwidth").toInt( &ok );
            if(!ok)
                return false;
        }
        if(graphparams.hasAttribute( "hard_xorig" )) {
            _params->hard_xorig = graphparams.attribute("hard_xorig").toInt( &ok );
            if(!ok)
                return false;
        }
        if(graphparams.hasAttribute( "hard_yorig" )) {
            _params->hard_yorig = graphparams.attribute("hard_yorig").toInt( &ok );
            if(!ok)
                return false;
        }
        if(graphparams.hasAttribute( "labeldist" )) {
            _params->label_dist=graphparams.attribute("labeldist").toInt( &ok );
            if(!ok) return false;
        }
    }

    QDomElement graphcolor = params.namedItem( "graphcolor" ).toElement();
    if(!graphcolor.isNull()) {
        if(graphcolor.hasAttribute( "bgcolor" )) {
            _params->BGColor= QColor( graphcolor.attribute( "bgcolor" ) );
        }
        if(graphcolor.hasAttribute( "gridcolor" )) {
            _params->GridColor= QColor( graphcolor.attribute( "gridcolor" ) );
        }
        if(graphcolor.hasAttribute( "linecolor" )) {
            _params->LineColor= QColor( graphcolor.attribute( "linecolor" ) );
        }
        if(graphcolor.hasAttribute( "plotcolor" )) {
            _params->PlotColor= QColor( graphcolor.attribute( "plotcolor" ) );
        }
        if(graphcolor.hasAttribute( "volcolor" )) {
            _params->VolColor= QColor( graphcolor.attribute( "volcolor" ) );
        }
        if(graphcolor.hasAttribute( "titlecolor" )) {
            _params->TitleColor= QColor( graphcolor.attribute( "titlecolor" ) );
        }
        if(graphcolor.hasAttribute( "xtitlecolor" )) {
            _params->XTitleColor= QColor( graphcolor.attribute( "xtitlecolor" ) );
        }
        if(graphcolor.hasAttribute( "ytitlecolor" )) {
            _params->YTitleColor= QColor( graphcolor.attribute( "ytitlecolor" ) );
        }
        if(graphcolor.hasAttribute( "ytitle2color" )) {
            _params->YTitle2Color= QColor( graphcolor.attribute( "ytitle2color" ) );
        }
        if(graphcolor.hasAttribute( "xlabelcolor" )) {
            _params->XLabelColor= QColor( graphcolor.attribute( "xlabelcolor" ) );
        }
        if(graphcolor.hasAttribute( "ylabelcolor" )) {
            _params->YLabelColor= QColor( graphcolor.attribute( "ylabelcolor" ) );
        }
        if(graphcolor.hasAttribute( "ylabel2color" )) {
            _params->YLabel2Color= QColor( graphcolor.attribute( "ylabel2color" ) );
        }
    }

    QDomElement annotation = params.namedItem( "annotation" ).toElement();
    if(!annotation.isNull()) {
        _params->annotation=new KChartAnnotationType;
        if(annotation.hasAttribute( "color" )) {
            _params->annotation->color= QColor( annotation.attribute( "color" ) );
        }
        if(annotation.hasAttribute( "point" )) {
            _params->annotation->point=annotation.attribute("point").toDouble( &ok );
            if(!ok) return false;
        }
    }
    QDomElement note = params.namedItem( "note" ).toElement();
    if ( !note.isNull()) {
        QString t = note.text();
        _params->annotation->note=t;
    }

    QDomElement scatter = params.namedItem( "scatter" ).toElement();
    if( !scatter.isNull() ) {
        _params->scatter = new KChartScatterType;
        if( scatter.hasAttribute( "point" ) ) {
            _params->scatter->point = scatter.attribute( "point" ).toDouble( &ok );
            if( !ok )
                return false;
        }
        if( scatter.hasAttribute( "val" ) ) {
            _params->scatter->val = scatter.attribute( "val" ).toDouble( &ok );
            if( !ok )
                return false;
        }
        if( scatter.hasAttribute( "width" ) ) {
            _params->scatter->width = scatter.attribute( "val" ).toUShort( &ok );
            if( !ok )
                return false;
        }
        if( scatter.hasAttribute( "color" )) {
            _params->scatter->color= QColor( scatter.attribute( "color" ) );
        }
        if( scatter.hasAttribute( "ind" ) ) {
            _params->scatter->ind = (KChartScatterIndType)scatter.attribute( "ind" ).toInt( &ok );
            if( !ok )
                return false;
        }
    }

    QDomElement legend = chart.namedItem("legend").toElement();
    if(!legend.isNull()) {
        int number = legend.attribute("number").toInt(&ok);
        if (!ok)  { return false; }
        QDomNode name = legend.firstChild();
        _params->legend.clear();
        for(int i=0; i<number; i++) {
            if(name.isNull()) {
                kdDebug(35001) << "Some problems, there is less data than it should be!" << endl;
                break;
            }
            QDomElement element = name.toElement();
            if( !element.isNull() && element.tagName() == "name" ) {
                QString t = element.text();
                _params->legend+=t;
                name = name.nextSibling();
            }
        }

    }
    QDomElement xlbl = chart.namedItem("xlbl").toElement();
    if(!xlbl.isNull()) {
        int number = xlbl.attribute("number").toInt(&ok);
        if (!ok)  { return false; }
        QDomNode label = xlbl.firstChild();
        _params->xlbl.clear();
        for (int i=0; i<number; i++) {
            if (label.isNull()) {
                kdDebug(35001) << "Some problems, there is less data than it should be!" << endl;
                break;
            }
            QDomElement element = label.toElement();
            if( !element.isNull() && element.tagName() == "label" ) {
                QString t = element.text();
                _params->xlbl+=t;
                label = label.nextSibling();
            }
        }
    }

    QDomElement backgroundPixmap = chart.namedItem( "backgroundPixmap" ).toElement();
    if( !backgroundPixmap.isNull() ) {
        if( backgroundPixmap.hasAttribute( "name" ) )
            _params->backgroundPixmapName = backgroundPixmap.attribute( "name" );
        if( backgroundPixmap.hasAttribute( "isDirty" ) ) {
            _params->backgroundPixmapIsDirty = (bool)backgroundPixmap.attribute( "isDirty" ).toInt( &ok );
            if( !ok )
                return false;
        }
        if( backgroundPixmap.hasAttribute( "scaled" ) ) {
            _params->backgroundPixmapScaled = (bool)backgroundPixmap.attribute( "scaled" ).toInt( &ok );
            if( !ok )
                return false;
        }
        if( backgroundPixmap.hasAttribute( "centered" ) ) {
            _params->backgroundPixmapCentered = (bool)backgroundPixmap.attribute( "centered" ).toInt( &ok );
            if( !ok )
                return false;
        }
        if( backgroundPixmap.hasAttribute( "intensity" ) ) {
            _params->backgroundPixmapIntensity = backgroundPixmap.attribute( "intensity" ).toFloat( &ok );
            if( !ok )
                return false;
        }
    }

    QDomElement extcolor = chart.namedItem("extcolor").toElement();
    if(!extcolor.isNull()) {
        unsigned int number = extcolor.attribute("number").toInt(&ok);
        if (!ok)  { return false; }
        QDomNode color = extcolor.firstChild();

        for (unsigned int i=0; i<number; i++) {
            if (color.isNull()) {
                kdDebug(35001) << "Some problems, there is less data than it should be!" << endl;
                break;
            }
            QDomElement element = color.toElement();
            if( !element.isNull()) {
                if(element.hasAttribute( "name" )) {
                    _params->ExtColor.setColor(i,QColor( element.attribute( "name" ) ));
                }
                color = color.nextSibling();
            }
        }
    }
    if( !_params->backgroundPixmapName.isNull() ) {
        _params->backgroundPixmap.load( locate( "wallpaper", _params->backgroundPixmapName ));
        _params->backgroundPixmapIsDirty = true;
    }
#endif
    return true;
}

#include "kchart_part.moc"
