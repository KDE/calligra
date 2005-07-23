/**
 *
 * Kalle Dalheimer <kalle@kde.org>
 */


#include "kchart_part.h"
#include "kchart_view.h"
#include "kchart_factory.h"
#include "kchartWizard.h"
#include "kchart_params.h"
#include "kdchart/KDChart.h"

#include <koTemplateChooseDia.h>
#include <kodom.h>
#include <koxmlns.h>

#include <kstandarddirs.h>
#include <kglobal.h>
#include <kdebug.h>

#include <qdom.h>
#include <qtextstream.h>
#include <qbuffer.h>
#include <qpainter.h>

using namespace std;


// Some hardcoded data for a chart

/* ----- set some data ----- */
// float   a[6]  = { 0.5, 0.09, 0.6, 0.85, 0.0, 0.90 },
// b[6]  = { 1.9, 1.3,  0.6, 0.75, 0.1, -2.0 };
/* ----- X labels ----- */
// char    *t[6] = { "Chicago", "New York", "L.A.", "Atlanta", "Paris, MD\n(USA) ", "London" };
/* ----- data set colors (RGB) ----- */
// QColor   sc[2]    = { QColor( 255, 128, 128 ), QColor( 128, 128, 255 ) };


namespace KChart
{

KChartPart::KChartPart( QWidget *parentWidget, const char *widgetName,
			QObject* parent, const char* name,
			bool singleViewMode )
  : KoChart::Part( parentWidget, widgetName, parent, name, singleViewMode ),
    m_rowLabels(), m_colLabels(),
    m_params( 0 ),
    m_parentWidget( parentWidget )
{
    kdDebug(35001) << "Constructor started!" << endl;

    setInstance( KChartFactory::global(), false );

    // Init some members that need it.
    {
	// Create the chart parameters and let the default be a bar chart
	// with 3D looks.
	m_params = new KChartParams();
	m_params->setChartType( KDChartParams::Bar );
	m_params->setBarChartSubType( KDChartParams::BarNormal );
	//m_params->setThreeDBars( true );

	// Handle data in rows per default.
	m_auxiliary.m_dataDirection = KChartAuxiliary::DataRows;
    }

    (void)new WizardExt( this );
    m_bCanChangeValue = true;

    // Display parameters
    m_displayData = m_currentData;
}


KChartPart::~KChartPart()
{
    kdDebug(35001) << "Part is going to be destroyed now!!!" << endl;
    delete m_params;
}


bool KChartPart::initDoc(InitDocFlags flags, QWidget* parentWidget)
{
    // Initialize the parameter set for this chart document
    kdDebug(35001) << "================================================================" << endl;
    kdDebug(35001) << "InitDOC: flags = " << flags << endl;
    kdDebug(35001) << "================================================================" << endl;

    QString f;

    // If this is an embedded document, e.g. in KSpread, then we can't
    // change the data.
    if (flags == KoDocument::InitDocEmbedded) {
	return true;
    }

    // If we are supposed to create a new, empty document, then do so.
    if (flags == KoDocument::InitDocEmpty) {
	initNullChart();

	resetURL();
	setEmpty();
	return true;
    }

    KoTemplateChooseDia::ReturnType  ret;
    KoTemplateChooseDia::DialogType  dlgtype;

    // If we must create a new document, then only present templates
    // to the user, otherwise also present existing documents and
    // recent documents.
    if (flags == KoDocument::InitDocFileNew )
	dlgtype = KoTemplateChooseDia::OnlyTemplates;
    else
	dlgtype = KoTemplateChooseDia::Everything;
    ret = KoTemplateChooseDia::choose( KChartFactory::global(), f,
                                       dlgtype, "kchart_template",
				       parentWidget );

    if ( ret == KoTemplateChooseDia::File ) {
	KURL url( f );
	return openURL( url );
    }
    else if ( ret == KoTemplateChooseDia::Empty ) {
	initNullChart();

	resetURL();
	setEmpty();
	return true;
    }
    else if ( ret == KoTemplateChooseDia::Template ) {
		//TODO: Activate this for KOffice 1.5/2.0
// 		if ( f.endsWith("/templates/chart/.source/BarChart.chrt") ) {
// 			generateBarChartTemplate();
// 			return true;
// 		}
        QFileInfo fileInfo( f );
        QString fileName( fileInfo.dirPath( true ) + "/" +
            fileInfo.baseName() + ".chrt" );

        resetURL();
        bool ok = loadNativeFormat( fileName );
        if ( !ok )
            showLoadingErrorDialog();
        setEmpty();
        //initConfig();
        return ok;
    }

    return false;
}


// This method creates the simplest chart imaginable:
// Data size 1x1, empty, no headers
//
void KChartPart::initNullChart()
{
    // Fill cells with data if there is none.
    kdDebug(35001) << "Initialize null chart." << endl;

    // Empty data.
    m_currentData.expand(0, 0);

    // Fill column and row labels.
    m_colLabels << QString("");
    m_rowLabels << QString("");

    setAxisDefaults();

    m_params->setDrawSolidExcessArrows(true);
}


void KChartPart::generateBarChartTemplate()
{
    int  col;
    int  row;

    kdDebug()<<"KChartPart::initTestChart()\n";

    // Fill cells with data if there is none.
    if (m_currentData.rows() == 0) {
        kdDebug(35001) << "Initialize with some data!!!" << endl;
        m_currentData.expand(4,4);
        m_currentData.setUsedRows( 4 );
        m_currentData.setUsedCols( 4 );
        for (row = 0; row < 4; row++) {
            for (col = 0; col < 4; col++) {
                KoChart::Value t( (double) row + col );
                // kdDebug(35001) << "Set cell for " << row << "," << col << endl;
                m_currentData.setCell(row, col, t);

		// Fill column label, but only on the first iteration.
		if (row == 0) {
		    m_colLabels << i18n("Column %1").arg(col + 1);
		}
            }

	    // Fill row label.
	    m_rowLabels << i18n("Row %1").arg(row + 1);
	}
    }

    setAxisDefaults();
    m_params->setDrawSolidExcessArrows(true);
}


KoView* KChartPart::createViewInstance( QWidget* parent, const char* name )
{
    return new KChartView( this, parent, name );
}


void KChartPart::paintContent( QPainter& painter, const QRect& rect,
			       bool transparent,
			       double /*zoomX*/, double /*zoomY*/ )
{
    // If params is 0, initDoc() has not been called.
    Q_ASSERT( m_params != 0 );

    // Handle data in rows or columns.
    //
    // This means getting row or column headers from the document and
    // set them as X axis labels or legend according to the current
    // setting.
    KDChartAxisParams  bottomparms = m_params->axisParams( KDChartAxisParams::AxisPosBottom );
    QStringList  longLabels;
    QStringList  shortLabels;

    longLabels.clear();
    shortLabels.clear();
    if (m_auxiliary.m_dataDirection == KChartAuxiliary::DataRows) {
	// Data is handled in rows.  This is the default.

	// This is efficient so it doesn't matter if we always copy.
	m_displayData = m_currentData;

	// Set X axis labels from column headers.
	for ( uint col = 0; col < m_currentData.cols(); col++ ) {
	    longLabels  << m_colLabels[col];
	    shortLabels << m_colLabels[col].left( 3 );
	}

	// Set legend from row headers.
	for ( uint row = 0; row < m_currentData.rows(); row++ )
	    m_params->setLegendText( row, m_rowLabels[row] );
    }
    else {
	// Data is handled in columns.  We will have to transpose
	// everything since KDChart wants its data in rows.

	// FIXME: Rewrite so that we only copy data when necessary.
	// On the other hand, the next version of KDChart is able to
	// get data directly without storing it into a KDChartData
	// class first, so we might never need to.

	// Resize displayData so that the transposed data has room.
	m_displayData.expand(m_currentData.usedCols(),
			     m_currentData.usedRows());

	// Copy data and transpose it.
	for (uint row = 0; row < m_currentData.usedCols(); row++) {
	    for (uint col = 0; col < m_currentData.usedRows(); col++) {
		m_displayData.setCell(row, col, m_currentData.cell(col, row));
	    }
	}

	// Set X axis labels from row headers.
	for ( uint row = 0; row < m_currentData.rows(); row++ ) {
	    longLabels  << m_rowLabels[row];
	    shortLabels << m_rowLabels[row].left( 3 );
	}

	// Set legend from column headers.
	for ( uint col = 0; col < m_currentData.cols(); col++ )
	    m_params->setLegendText( col, m_colLabels[col] );
    }
    bottomparms.setAxisLabelStringLists( &longLabels, &shortLabels );
    m_params->setAxisParams(KDChartAxisParams::AxisPosBottom, bottomparms);

    // Ok, we have now created a data set for display, and params with
    // suitable legends and axis labels.  Now start the real painting.

    // Handle transparency.
    if( !transparent )
        painter.eraseRect( rect );

    // ## TODO: support zooming

    // We only need to draw the document rectangle "rect".
    KDChart::paint( &painter, m_params, &m_displayData, 0, &rect );
}


// This function sets the data from an external source.  It is called,
// for instance, when the chart is initialized from a spreadsheet in
// KSpread.
//
void KChartPart::setData( const KoChart::Data& data )
{
    // FIXME(khz): replace this when automatic string detection works in KDChart
    //m_currentData = data;

    // Does the top/left cell contain a string?
    bool isStringTopLeft = data.cell( 0, 0 ).isString();

    // Does the first row (without first cell) contain only strings?
    bool isStringFirstRow = TRUE;
    for ( uint col = 1; isStringFirstRow && col < data.cols(); col++ ) {
        isStringFirstRow = data.cell( 0, col ).isString();
    }

    // Just in case, we only have 1 row, we never use it for label text.
    // This prevents a crash.
    if ( data.rows() == 1 )
        isStringFirstRow = FALSE;

    // Does the first column (without first cell) contain only strings?
    bool isStringFirstCol = TRUE;
    for ( uint row = 1; isStringFirstCol && row < data.rows(); row++ ) {
        isStringFirstCol = data.cell( row, 0 ).isString();
    }

    // Just in case, we only have 1 column, we never use it for axis
    // label text => prevents crash.
    if ( data.cols() == 1 )
        isStringFirstRow = FALSE;

    bool hasColHeader = FALSE;
    bool hasRowHeader = FALSE;

    // Let's check if we have a full axis label text column
    if ( isStringFirstCol && isStringTopLeft
	 || isStringFirstCol && isStringFirstRow )
        hasColHeader = TRUE;

    // Let's check if we have a full label text row.
    if ( isStringFirstRow && isStringTopLeft
	 || isStringFirstCol && isStringFirstRow )
        hasRowHeader = TRUE;

    doSetData(data, hasRowHeader, hasColHeader);
}


void KChartPart::doSetData( const KoChart::Data&  data,
			    bool  hasRowHeader,
			    bool  hasColHeader )
{
    uint  rowStart = 0;
    uint  colStart = 0;
    uint  col;
    uint  row;

    if (hasRowHeader)
      rowStart = 1;
    if (hasColHeader)
      colStart = 1;

    // Generate legend from the column headers if applicable.
    m_rowLabels.clear();
    if ( hasColHeader ) {
        for( row = rowStart; row < data.rows(); row++ )
	    m_rowLabels << data.cell( row, 0 ).stringValue();
    }
    else
        m_params->setLegendSource( KDChartParams::LegendAutomatic );

    // Generate X labels from the row headers if applicable
    m_colLabels.clear();
    if ( hasRowHeader ) {
        for( col = colStart; col < data.cols(); col++ )
	    m_colLabels << data.cell( 0, col ).stringValue();
    }

    // If there is a row header and/or a column header, then generate
    // the data that will be used for the chart by translating the
    // original data.
    if ( hasColHeader || hasRowHeader ) {
        KoChart::Data matrix( data.rows() - rowStart, data.cols() - colStart );

        for ( col = colStart; col < data.cols(); col++ ) {
            for ( row = rowStart; row < data.rows(); row++ ) {
                matrix.setCell( row - rowStart, col - colStart,
				KoChart::Value( data.cell( row, col ).doubleValue() ) );
            }
        }
        m_currentData = matrix;
    }
    else
        m_currentData = data;

    setAxisDefaults();

    emit docChanged();
}


bool KChartPart::showWizard()
{
    KChartWizard  *wizard = new KChartWizard( this, m_parentWidget, "wizard" );

    connect( wizard, SIGNAL(finished()), this, SLOT(slotModified()) );

    bool  ret = wizard->exec();

    delete wizard;
    return ret;
}


void KChartPart::initLabelAndLegend()
{
    // Labels and legends are automatically initialized to reasonable
    // default values in KDChart
}


// Set up some values for the chart Axis, that are not well chosen by
// default by KDChart.
//
// FIXME: This function should maybe change name since it does things
//        that are not only for axis.
//
void KChartPart::setAxisDefaults()
{
  //
  // Settings for the Y axis.
  //
  KDChartAxisParams  yAxis
    = m_params->axisParams( KDChartAxisParams::AxisPosLeft );

  // decimal symbol and thousands separator
  yAxis.setAxisLabelsRadix( KGlobal::locale()->decimalSymbol(),
			    KGlobal::locale()->thousandsSeparator() );

  m_params->setAxisParams( KDChartAxisParams::AxisPosLeft, yAxis );

  //
  // Settings for the X axis.
  //
  KDChartAxisParams  xAxis
    = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

  // These two shouldn't be necessary to set.
  xAxis.setAxisFirstLabelText();
  xAxis.setAxisLastLabelText();

  m_params->setAxisParams( KDChartAxisParams::AxisPosBottom, xAxis );

  //FIXME: The following has nothing to do with the axis.  Maybe this
  //       function should change name.
  m_params->setLineColor();
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
    m_params->setLineMarker(conf->readBoolEntry("lineMarker",
						m_params->lineMarker()));
    m_params->setThreeDBarDepth( conf->readDoubleNumEntry("_3d_depth",
							  m_params->threeDBarDepth() ) );
    m_params->setThreeDBarAngle( conf->readNumEntry( "_3d_angle",
						     m_params->threeDBarAngle() ) );

    KDChartAxisParams leftparams = m_params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams rightparams = m_params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams bottomparams = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

    bottomparams.setAxisLineColor( conf->readColorEntry( "XTitleColor", 0 ) );
    leftparams.setAxisLineColor( conf->readColorEntry( "YTitleColor", 0 ) );
    rightparams.setAxisLineColor( conf->readColorEntry( "YTitle2Color", 0 ) );
    bottomparams.setAxisLabelsColor( conf->readColorEntry( "XLabelColor", 0 ) );
    leftparams.setAxisLabelsColor( conf->readColorEntry( "YLabelColor", 0 ) );
    rightparams.setAxisLabelsColor( conf->readColorEntry( "YLabel2Color", 0 ) );
    leftparams.setAxisGridColor( conf->readColorEntry( "GridColor", 0 ) );
    m_params->setOutlineDataColor( conf->readColorEntry( "LineColor", 0 ) );
    m_params->setAxisParams( KDChartAxisParams::AxisPosLeft,
                            leftparams );
    m_params->setAxisParams( KDChartAxisParams::AxisPosRight,
                            rightparams );
    m_params->setAxisParams( KDChartAxisParams::AxisPosBottom,
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
    m_params->setChartType( (KDChartParams::ChartType)conf->readNumEntry( "type", m_params->chartType() ) );
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
    delete m_params;
    m_params = new KChartParams();
    setAxisDefaults();
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

    conf->writeEntry( "_3d_depth", m_params->threeDBarDepth() );
    conf->writeEntry( "_3d_angle", m_params->threeDBarAngle() );

    KDChartAxisParams leftparams   = m_params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams rightparams  = m_params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams bottomparams = m_params->axisParams( KDChartAxisParams::AxisPosBottom );
    conf->writeEntry( "LineColor",    m_params->outlineDataColor() );
    conf->writeEntry( "XTitleColor",  bottomparams.axisLineColor() );
    conf->writeEntry( "YTitleColor",  leftparams.axisLineColor() );
    conf->writeEntry( "YTitle2Color", rightparams.axisLineColor() );
    conf->writeEntry( "XLabelColor",  bottomparams.axisLabelsColor() );
    conf->writeEntry( "YLabelColor",  leftparams.axisLabelsColor() );
    conf->writeEntry( "YLabel2Color", rightparams.axisLabelsColor() );
    conf->writeEntry( "GridColor",    leftparams.axisGridColor() );

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
    conf->writeEntry( "type", (int) m_params->chartType() );
    //   conf->writeEntry("other_threshold", other_threshold);

    // background pixmap stuff
    //   if( !backgroundPixmapName.isNull() )
    // 	conf->writePathEntry( "backgroundPixmapName", backgroundPixmapName );
    //   conf->writeEntry( "backgroundPixmapIsDirty", backgroundPixmapIsDirty );
    //   conf->writeEntry( "backgroundPixmapScaled", backgroundPixmapScaled );
    //   conf->writeEntry( "backgroundPixmapCentered", backgroundPixmapCentered );
    //   conf->writeEntry( "backgroundPixmapIntensity", backgroundPixmapIntensity );
    conf->writeEntry( "lineMarker", (int) m_params->lineMarker());
}


QDomDocument KChartPart::saveXML()
{
    kdDebug(35001) << "kchart saveXML called" << endl;

    // The biggest part of the saving is done by KDChart itself, so we
    // don't have to do it.
    QDomDocument doc = m_params->saveXML( false );

    // ----------------------------------------------------------------
    // The rest of the saving has to be done by us.

    QDomElement docRoot = doc.documentElement();

    // Save auxiliary data.
    QDomElement aux = doc.createElement( "KChartAuxiliary" );
    docRoot.appendChild( aux );

    // Only one auxiliary element so far: the data direction (rows/columns).
    QDomElement e = doc.createElement( "direction" );
    e.setAttribute( "value", (int) m_auxiliary.m_dataDirection );
    aux.appendChild( e );

    // Save the data values.
    QDomElement data = doc.createElement( "data" );
    docRoot.appendChild( data );

    int cols = m_currentData.usedCols()
             ? QMIN(m_currentData.usedCols(), m_currentData.cols())
             : m_currentData.cols();
    int rows = m_currentData.usedRows()
             ? QMIN(m_currentData.usedRows(), m_currentData.rows())
             : m_currentData.rows();
    data.setAttribute( "cols", cols );
    data.setAttribute( "rows", rows );
    kdDebug(35001) << "      writing  " << cols << "," << rows << "  (cols,rows)." << endl;

    for (int i=0; i!=rows; ++i) {
        for (int j=0; j!=cols; ++j) {
            QDomElement e = doc.createElement( "cell" );
            data.appendChild( e );
            QString valType;
            KoChart::Value & cell( m_currentData.cell( i,j ) );
            switch ( cell.valueType() ) {
                case KoChart::Value::NoValue:  valType = "NoValue";   break;
                case KoChart::Value::String:   valType = "String";    break;
                case KoChart::Value::Double:   valType = "Double";    break;
                case KoChart::Value::DateTime: valType = "DateTime";  break;
                default: {
                    valType = "(unknown)";
                    kdDebug(35001) << "ERROR: cell " << i << "," << j
				   << " has unknown type." << endl;
                }
            }

            e.setAttribute( "valType", valType );
            //kdDebug(35001) << "      cell " << i << "," << j
	    //	   << " saved with type '" << valType << "'." << endl;
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
                        kdDebug(35001) << "ERROR: cell " << i << "," << j
				       << " has unknown type." << endl;
                }
            }
        }
    }

    return doc;
}


bool KChartPart::loadOasis( const QDomDocument& doc,
			    KoOasisStyles&      oasisStyles,
			    const QDomDocument& /*settings*/,
			    KoStore* )
{
    kdDebug(35001) << "kchart loadOasis called" << endl;
    QDomElement  content = doc.documentElement();
    QDomElement  bodyElem ( KoDom::namedItemNS( content, KoXmlNS::office, 
						"body" ) );
    if ( bodyElem.isNull() ) {
        kdError(32001) << "No office:body found!" << endl;
        setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return false;
    }

    QDomElement officeChartElem = KoDom::namedItemNS( bodyElem, 
						      KoXmlNS::office, "chart" );
    if ( officeChartElem.isNull() ) {
        kdError(32001) << "No office:chart found!" << endl;
        QDomElement childElem;
        QString localName;
        forEachElement( childElem, bodyElem ) {
            localName = childElem.localName();
        }
        if ( localName.isEmpty() )
            setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No tag found inside office:body." ) );
        else
            setErrorMessage( i18n( "This document is not a chart, but %1. Please try opening it with the appropriate application." ).arg( KoDocument::tagNameToDocumentType( localName ) ) );
        return false;
    }

    QDomElement chartElem = KoDom::namedItemNS( officeChartElem, KoXmlNS::chart, "chart" );
    if ( chartElem.isNull() )
    {
        setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No chart:chart tag found." ) );
        return false;
    }

    // Load parameters
    QString errorMessage;
    bool ok = m_params->loadOasis( chartElem, oasisStyles, errorMessage );
    if ( !ok ) {
        setErrorMessage( errorMessage );
        return false;
    }

    // TODO Load data direction (see loadAuxiliary)

    // Load data
    QDomElement tableElem = KoDom::namedItemNS( chartElem, KoXmlNS::table, "table" );
    if ( !tableElem.isNull() ) {
        ok = loadOasisData( tableElem );
        if ( !ok )
            return false; // TODO setErrorMessage
    }

    // FIXME: Shouldn't this be done first of all, so it could be
    //        overrided from the file?  /ingwa
    setAxisDefaults();

    return true;
}


bool KChartPart::loadOasisData( const QDomElement& tableElem )
{
    int numberHeaderColumns = 0;
    QDomElement tableHeaderColumns = KoDom::namedItemNS( tableElem, KoXmlNS::table, "table-header-columns" );
    QDomElement elem;
    forEachElement( elem, tableHeaderColumns ) {
        if ( elem.localName() == "table-column" ) {
            int repeated = elem.attributeNS( KoXmlNS::table, "number-columns-repeated", QString::null ).toInt();
            numberHeaderColumns += QMAX( 1, repeated );
        }
    }
    Q_ASSERT( numberHeaderColumns == 1 ); // with 0 you get no titles, and with more than 1 we ignore the others.

    int numberDataColumns = 0;
    QDomElement tableColumns = KoDom::namedItemNS( tableElem, KoXmlNS::table, "table-columns" );
    forEachElement( elem, tableColumns ) {
        if ( elem.localName() == "table-column" ) {
            int repeated = elem.attributeNS( KoXmlNS::table, "number-columns-repeated", QString::null ).toInt();
            numberDataColumns += QMAX( 1, repeated );
        }
    }

    // Parse table-header-rows for the column names.
    m_colLabels.clear();
    QDomElement tableHeaderRows = KoDom::namedItemNS( tableElem, KoXmlNS::table, "table-header-rows" );
    if ( tableHeaderRows.isNull() )
        kdWarning(35001) << "No table-header-rows element found!" << endl;
    QDomElement tableHeaderRow = KoDom::namedItemNS( tableHeaderRows, KoXmlNS::table, "table-row" );
    if ( tableHeaderRow.isNull() )
        kdWarning(35001) << "No table-row inside table-header-rows!" << endl;
    int cellNum = 0;
    forEachElement( elem, tableHeaderRow ) {
        if ( elem.localName() == "table-cell" ) {
            ++cellNum;
            if ( cellNum > numberHeaderColumns ) {
                QDomElement pElem = KoDom::namedItemNS( elem, KoXmlNS::text, "p" );
                m_colLabels.append( pElem.text() );
            }
        }
    }
    if ( (int)m_colLabels.count() != numberDataColumns )
        kdWarning(35001) << "Got " << m_colLabels.count() << " column titles, expected " << numberDataColumns << endl;

    // Get the number of rows, and read row labels
    m_rowLabels.clear();
    int numberDataRows = 0;
    QDomElement tableRows = KoDom::namedItemNS( tableElem, KoXmlNS::table, "table-rows" );
    forEachElement( elem, tableRows ) {
        if ( elem.localName() == "table-row" ) {
            int repeated = elem.attributeNS( KoXmlNS::table, "number-rows-repeated", QString::null ).toInt();
            Q_ASSERT( repeated <= 1 ); // we don't handle yet the case where data rows are repeated (can this really happen?)
            numberDataRows += QMAX( 1, repeated );
            if ( numberHeaderColumns > 0 ) {
                QDomElement firstCell = KoDom::namedItemNS( elem, KoXmlNS::table, "table-cell" );
                QDomElement pElem = KoDom::namedItemNS( firstCell, KoXmlNS::text, "p" );
                m_rowLabels.append( pElem.text() );
            }
        }
    }

    kdDebug(35001) << "numberHeaderColumns=" << numberHeaderColumns << " numberDataColumns=" << numberDataColumns
                   << " numberDataRows=" << numberDataRows << endl;

    if ( (int)m_rowLabels.count() != numberDataRows)
        kdWarning(35001) << "Got " << m_rowLabels.count() << " row labels, expected " << numberDataRows << endl;

    m_currentData.expand( numberDataRows, numberDataColumns );
    m_currentData.setUsedCols( numberDataColumns );
    m_currentData.setUsedRows( numberDataRows );

    // Now really load the cells
    int row = 0;
    QDomElement rowElem;
    forEachElement( rowElem, tableRows ) {
        if ( rowElem.localName() == "table-row" ) {
            int col = 0;
            int cellNum = 0;
            QDomElement cellElem;
            forEachElement( cellElem, rowElem ) {
                if ( cellElem.localName() == "table-cell" ) {
                    ++cellNum;
                    if ( cellNum > numberHeaderColumns ) {
                        QString valueType = cellElem.attributeNS( KoXmlNS::office, "value-type", QString::null );
                        if ( valueType != "float" )
                            kdWarning(35001) << "Don't know how to handle value-type " << valueType << endl;
                        else {
                            QString value = cellElem.attributeNS( KoXmlNS::office, "value", QString::null );
                            double val = value.toDouble();
                            m_currentData.setCell( row, col, val );
                        }
                        ++col;
                    }
                }
            }
            ++row;
        }
    }

    return true;
}


bool KChartPart::saveOasis(KoStore*, KoXmlWriter*)
{
    // TODO
    return false;
}


bool KChartPart::loadXML( QIODevice*, const QDomDocument& doc )
{
    kdDebug(35001) << "kchart loadXML called" << endl;

    // First try to load the KDChart parameters.
    bool  result = m_params->loadXML( doc );

    // If that didn't go well, try to load old file format...
    if (!result)
        result = loadOldXML( doc );
    else {
	// ...but if it did, try to load the auxiliary data and the data.
        result = loadAuxiliary(doc)
	    && loadData( doc, m_currentData );
    }

    // If everything is OK, then get the headers from the KDChart parameters.
    if (result) {
	QStringList        legendLabels;
	KDChartAxisParams  params
	    = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

	// Get the legend.
	QString  str;
	uint     index = 0;
	while ((str = m_params->legendText(index++)) != QString::null)
	    legendLabels << str;

	if (m_auxiliary.m_dataDirection == KChartAuxiliary::DataRows) {
	    m_colLabels = params.axisLabelStringList();
	    m_rowLabels = legendLabels;
	}
	else {
	    m_colLabels = legendLabels;
	    m_rowLabels = params.axisLabelStringList();
	}

	setAxisDefaults();
    }

    m_params->setDrawSolidExcessArrows(true);

    return result;
}


// Load the auxiliary data.
// Currently, that means the data direction.
//
bool KChartPart::loadAuxiliary( const QDomDocument& doc )
{
    QDomElement  chart = doc.documentElement();
    QDomElement  aux   = chart.namedItem("KChartAuxiliary").toElement();

    // Older XML files might be missing this section.  That is OK; the
    // defaults will be used.
    if (aux.isNull())
	return true;

    QDomNode node = aux.firstChild();

    // If the aux section exists, it should contain data.
    while (!node.isNull()) {

	QDomElement e = node.toElement();
	if (e.isNull()) {
	    // FIXME: Should this be regarded as an error?
	    node = node.nextSibling();
	    continue;
	}

	// Check for direction
	if ( e.tagName() == "direction" ) {
	    if ( e.hasAttribute("value") ) {
		bool  ok;

		// Read the direction. On failure, use the default.
		int   dir = e.attribute("value").toInt(&ok);
		if ( !ok )
		    dir = (int) KChartAuxiliary::DataRows;

		kdDebug(35001) << "Got aux value \"direction\": " << dir << endl;
		m_auxiliary.m_dataDirection = (KChartAuxiliary::DataDirection) dir;
	    }
	    else {
		kdDebug(35001) << "Error in direction tag." << endl;
	    }
	}
#if 0
	// Expand with more auxiliary types when needed.
	else if ( e.tagName() == "..." ) {
	}
	and so on...
#endif

	node = node.nextSibling();
    }

    return true;
}


bool KChartPart::loadData( const QDomDocument& doc,
			   KoChart::Data& m_currentData )
{
    kdDebug(35001) << "kchart loadData called" << endl;

    QDomElement chart = doc.documentElement();
    QDomElement data = chart.namedItem("data").toElement();
    bool ok;
    int cols = data.attribute("cols").toInt(&ok);
    kdDebug(35001) << "cols readed as:" << cols << endl;
    if ( !ok ){
         return false;
    }

    int rows = data.attribute("rows").toInt(&ok);
    if ( !ok ){
         return false;
    }

    kdDebug(35001) << rows << " x " << cols << endl;
    m_currentData.expand(rows, cols);
    m_currentData.setUsedCols( cols );
    m_currentData.setUsedRows( rows );
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
                if ( e.hasAttribute("value") && e.hasAttribute("valType") ) {
                    QString valueType = e.attribute("valType").lower();
                    if ( "string" == valueType ) {
                        QString strVal = e.attribute("value");
                        t = KoChart::Value( strVal );
                    }
                    else if ( "double" == valueType ) {
                        bool bOk;
                        double val = e.attribute("value").toDouble(&bOk);
                        if ( !bOk )
                            val = 0.0;
                        t = KoChart::Value( val );
                    /*
                    } else if ( "datetime" == valueType ) {
                        t = . . .
                    */
                    } else {
                        t.clearValue();
                        if ( "novalue" != valueType )
                            kdDebug(35001) << "ERROR: cell " << i << "," << j << " has unknown type '" << valueType << "'." << endl;
                    }
                } else
                    t.clearValue();

                m_currentData.setCell(i,j, t );

		/*
                if ( e.hasAttribute( "hide" ) ) {
                    tmpMissing[cols*j+i] = (bool)e.attribute("hide").toInt( &ok );
                    if ( !ok )
                        return false;
                } else {
                    tmpMissing[cols*j+i] = false;
                }
                if ( e.hasAttribute( "dist" ) ) {
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
    m_params->missing=tmpMissing;
    m_params->explode=tmpExp;
    */
    return true;
}


bool KChartPart::loadOldXML( const QDomDocument& doc )
{
    kdDebug(35001) << "kchart loadOldXML called" << endl;
    if ( doc.doctype().name() != "chart" )
        return false;

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
    m_currentData.expand(rows, cols);
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
                m_currentData.setCell(i,j,t);
                if ( e.hasAttribute( "hide" ) ) {
                    tmpMissing[cols*j+i] = (bool)e.attribute("hide").toInt( &ok );
                    if ( !ok )
                        return false;
                } else {
                    tmpMissing[cols*j+i] = false;
                }
                if ( e.hasAttribute( "dist" ) ) {
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
    m_params->missing=tmpMissing;
    m_params->explode=tmpExp;
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
    if ( params.hasAttribute( "type" ) ) {
        int type=params.attribute("type").toInt( &ok );
        if ( !ok )
            return false;
        switch(type)
        {
        case 1:
            m_params->setChartType(KDChartParams::Line);
            break;
        case 2:
            m_params->setChartType(KDChartParams::Area);
            break;
        case 3:
            m_params->setChartType(KDChartParams::Bar);
            break;
        case 4:
            m_params->setChartType(KDChartParams::HiLo);
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
            m_params->setChartType(KDChartParams::Line);
            break;
        case 9:
            m_params->setChartType(KDChartParams::HiLo);
            break;
        case 10:
            m_params->setChartType(KDChartParams::Bar);
            break;
        case 11:
            m_params->setChartType(KDChartParams::Area);
            break;
        case 12:
            m_params->setChartType(KDChartParams::Bar);
            break;
        case 13:
            m_params->setChartType(KDChartParams::Area);
            break;
        case 14:
            m_params->setChartType(KDChartParams::Bar);
            break;
        case 15:
            m_params->setChartType(KDChartParams::Area);
            break;
        case 16:
            m_params->setChartType(KDChartParams::Line);
            break;
        case 17:
        case 18:
            m_params->setChartType(KDChartParams::Pie);
            break;

        }
        if ( !ok )
            return false;
    }
#if 0
    if ( params.hasAttribute( "subtype" ) ) {
        m_params->stack_type = (KChartStackType)params.attribute("subtype").toInt( &ok );
        if ( !ok )
            return false;
    }
    if ( params.hasAttribute( "hlc_style" ) ) {
        m_params->hlc_style = (KChartHLCStyle)params.attribute("hlc_style").toInt( &ok );
        if ( !ok )
            return false;
    }
    if ( params.hasAttribute( "hlc_cap_width" ) ) {
        m_params->hlc_cap_width = (short)params.attribute( "hlc_cap_width" ).toShort( &ok );
        if ( !ok )
            return false;
    }

    QDomElement title = params.namedItem( "title" ).toElement();
    if ( !title.isNull()) {
        QString t = title.text();
        m_params->title=t;
    }
    QDomElement titlefont = params.namedItem( "titlefont" ).toElement();
    if ( !titlefont.isNull()) {
        QDomElement font = titlefont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            m_params->setTitleFont(toFont(font));
    }
    QDomElement xtitle = params.namedItem( "xtitle" ).toElement();
    if ( !xtitle.isNull()) {
        QString t = xtitle.text();
        m_params->xtitle=t;
    }
    QDomElement xtitlefont = params.namedItem( "xtitlefont" ).toElement();
    if ( !xtitlefont.isNull()) {
        QDomElement font = xtitlefont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            m_params->setXTitleFont(toFont(font));
    }
    QDomElement ytitle = params.namedItem( "ytitle" ).toElement();
    if ( !ytitle.isNull()) {
        QString t = ytitle.text();
        m_params->ytitle=t;
    }
    QDomElement ytitle2 = params.namedItem( "ytitle2" ).toElement();
    if ( !ytitle2.isNull()) {
        QString t = ytitle2.text();
        m_params->ytitle2=t;
    }
    QDomElement ytitlefont = params.namedItem( "ytitlefont" ).toElement();
    if ( !ytitlefont.isNull()) {
        QDomElement font = ytitlefont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            m_params->setYTitleFont(toFont(font));
    }
    QDomElement ylabelfmt = params.namedItem( "ylabelfmt" ).toElement();
    if ( !ylabelfmt.isNull()) {
        QString t = ylabelfmt.text();
        m_params->ylabel_fmt=t;
    }
    QDomElement ylabel2fmt = params.namedItem( "ylabel2fmt" ).toElement();
    if ( !ylabel2fmt.isNull()) {
        QString t = ylabel2fmt.text();
        m_params->ylabel2_fmt=t;
    }
    QDomElement labelfont = params.namedItem( "labelfont" ).toElement();
    if ( !labelfont.isNull()) {
        QDomElement font = labelfont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            m_params->setLabelFont(toFont(font));
    }

    QDomElement yaxisfont = params.namedItem( "yaxisfont" ).toElement();
    if ( !yaxisfont.isNull()) {
        QDomElement font = yaxisfont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            m_params->setYAxisFont(toFont(font));
    }

    QDomElement xaxisfont = params.namedItem( "xaxisfont" ).toElement();
    if ( !xaxisfont.isNull()) {
        QDomElement font = xaxisfont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            m_params->setXAxisFont(toFont(font));
    }
    QDomElement annotationFont = params.namedItem("annotationfont").toElement();
    if ( !annotationFont.isNull()) {
        QDomElement font = annotationFont.namedItem( "font" ).toElement();
        if ( !font.isNull() )
            m_params->setAnnotationFont(toFont(font));
    }

    QDomElement yaxis = params.namedItem( "yaxis" ).toElement();
    if ( !yaxis.isNull()) {
        if (yaxis.hasAttribute( "yinterval" )) {
            m_params->requested_yinterval= yaxis.attribute("yinterval").toDouble( &ok );
            if ( !ok ) return false;
        }
        if (yaxis.hasAttribute( "ymin" )) {
            m_params->requested_ymin= yaxis.attribute("ymin").toDouble( &ok );
            if ( !ok ) return false;
        }
        if (yaxis.hasAttribute( "ymax" ) ) {
            m_params->requested_ymax= yaxis.attribute("ymax").toDouble( &ok );
            if ( !ok ) return false;
        }
    }
#endif

    QDomElement graph = params.namedItem( "graph" ).toElement();
    if (!graph.isNull()) {
        if (graph.hasAttribute( "grid" )) {
            bool b=(bool) graph.attribute("grid").toInt( &ok );
            m_params->setAxisShowGrid(KDChartAxisParams::AxisPosLeft,b );
            m_params->setAxisShowGrid(KDChartAxisParams::AxisPosBottom,b );
            if (!ok) return false;
        }
        if (graph.hasAttribute( "xaxis" )) {
            bool b=(bool) graph.attribute("xaxis").toInt( &ok );
            if (!ok) return false;
            m_params->setAxisVisible(KDChartAxisParams::AxisPosBottom,b);
        }
        if (graph.hasAttribute( "yaxis" )) {
            bool b=(bool) graph.attribute("yaxis").toInt( &ok );
            if (!ok) return false;
            m_params->setAxisVisible(KDChartAxisParams::AxisPosLeft,b);
        }
#if 0
        //no implemented
        if (graph.hasAttribute( "shelf" )) {
            m_params->shelf=(bool) graph.attribute("shelf").toInt( &ok );
            if (!ok) return false;
        }
#endif
        if (graph.hasAttribute( "yaxis2" )) {
            bool b=(bool) graph.attribute("yaxis2").toInt( &ok );
            if (!ok) return false;
            m_params->setAxisVisible(KDChartAxisParams::AxisPosRight,b);
        }

#if 0
        //no implemented
        if (graph.hasAttribute( "ystyle" )) {
            m_params->yval_style=(bool) graph.attribute("ystyle").toInt( &ok );
            if (!ok) return false;
        }
        if (graph.hasAttribute( "border" )) {
            m_params->border=(bool) graph.attribute("border").toInt( &ok );
            if (!ok) return false;
        }
        if (graph.hasAttribute( "transbg" )) {
            m_params->transparent_bg=(bool) graph.attribute("transbg").toInt( &ok );
            if (!ok) return false;
        }
        if (graph.hasAttribute( "xlabel" )) {
            m_params->hasxlabel=(bool) graph.attribute("xlabel").toInt( &ok );
            if (!ok) return false;
        }
        if ( graph.hasAttribute( "xlabel_spacing" ) ) {
            m_params->xlabel_spacing = (short)graph.attribute( "xlabel_spacing" ).toShort( &ok );
            if ( !ok )
                return false;
        }
        if ( graph.hasAttribute( "ylabel_density" ) ) {
            m_params->ylabel_density = (short)graph.attribute( "ylabel_density" ).toShort( &ok );
            if ( !ok )
                return false;
        }
        if (graph.hasAttribute( "line")) {
            m_params->label_line=(bool) graph.attribute("line").toInt( &ok );
            if (!ok) return false;
        }
        if (graph.hasAttribute( "percent")) {
            m_params->percent_labels=(KChartPercentType) graph.attribute("percent").toInt( &ok );
            if (!ok) return false;
        }
        if (graph.hasAttribute("cross")) {
            m_params->cross=(bool) graph.attribute("cross").toInt( &ok );
            if (!ok) return false;
        }
        if (graph.hasAttribute("thumbnail")) {
            m_params->thumbnail=(bool) graph.attribute("thumbnail").toInt( &ok );
            if (!ok) return false;
        }
        if (graph.hasAttribute("thumblabel")) {
            m_params->thumblabel= graph.attribute("thumblabel");
        }
        if (graph.hasAttribute("thumbval")) {
            m_params->thumbval=(bool) graph.attribute("thumbval").toDouble( &ok );
            if (!ok)
                return false;
        }
#endif
    }

#if 0
    QDomElement graphparams = params.namedItem( "graphparams" ).toElement();
    if (!graphparams.isNull()) {
        if (graphparams.hasAttribute( "dept3d" )) {
            m_params->_3d_depth=graphparams.attribute("dept3d").toDouble( &ok );
            if (!ok) return false;
        }
        if (graphparams.hasAttribute( "angle3d" )) {
            m_params->_3d_angle=graphparams.attribute("angle3d").toShort( &ok );
            if (!ok) return false;
        }
        if (graphparams.hasAttribute( "barwidth" )) {
            m_params->bar_width=graphparams.attribute("barwidth").toShort( &ok );
            if (!ok) return false;
        }
        if (graphparams.hasAttribute( "colpie" )) {
            m_params->colPie=graphparams.attribute("colpie").toInt( &ok );
            if (!ok) return false;
        }
        if (graphparams.hasAttribute( "other_threshold" )) {
            m_params->other_threshold=graphparams.attribute("other_threshold").toShort( &ok );
            if (!ok)
                return false;
        }
        if (graphparams.hasAttribute( "offsetCol" )) {
            m_params->offsetCol = graphparams.attribute("offsetCol").toInt( &ok );
            if (!ok)
                return false;
        }
        if (graphparams.hasAttribute( "hard_size" )) {
            m_params->hard_size = (bool)graphparams.attribute("hard_size").toInt( &ok );
            if (!ok)
                return false;
        }
        if (graphparams.hasAttribute( "hard_graphheight" )) {
            m_params->hard_graphheight = graphparams.attribute("hard_graphheight").toInt( &ok );
            if (!ok)
                return false;
        }
        if (graphparams.hasAttribute( "hard_graphwidth" )) {
            m_params->hard_graphwidth = graphparams.attribute("hard_graphwidth").toInt( &ok );
            if (!ok)
                return false;
        }
        if (graphparams.hasAttribute( "hard_xorig" )) {
            m_params->hard_xorig = graphparams.attribute("hard_xorig").toInt( &ok );
            if (!ok)
                return false;
        }
        if (graphparams.hasAttribute( "hard_yorig" )) {
            m_params->hard_yorig = graphparams.attribute("hard_yorig").toInt( &ok );
            if (!ok)
                return false;
        }
        if (graphparams.hasAttribute( "labeldist" )) {
            m_params->label_dist=graphparams.attribute("labeldist").toInt( &ok );
            if (!ok) return false;
        }
    }

    QDomElement graphcolor = params.namedItem( "graphcolor" ).toElement();
    if (!graphcolor.isNull()) {
        if (graphcolor.hasAttribute( "bgcolor" )) {
            m_params->BGColor= QColor( graphcolor.attribute( "bgcolor" ) );
        }
        if (graphcolor.hasAttribute( "gridcolor" )) {
            m_params->GridColor= QColor( graphcolor.attribute( "gridcolor" ) );
        }
        if (graphcolor.hasAttribute( "linecolor" )) {
            m_params->LineColor= QColor( graphcolor.attribute( "linecolor" ) );
        }
        if (graphcolor.hasAttribute( "plotcolor" )) {
            m_params->PlotColor= QColor( graphcolor.attribute( "plotcolor" ) );
        }
        if (graphcolor.hasAttribute( "volcolor" )) {
            m_params->VolColor= QColor( graphcolor.attribute( "volcolor" ) );
        }
        if (graphcolor.hasAttribute( "titlecolor" )) {
            m_params->TitleColor= QColor( graphcolor.attribute( "titlecolor" ) );
        }
        if (graphcolor.hasAttribute( "xtitlecolor" )) {
            m_params->XTitleColor= QColor( graphcolor.attribute( "xtitlecolor" ) );
        }
        if (graphcolor.hasAttribute( "ytitlecolor" )) {
            m_params->YTitleColor= QColor( graphcolor.attribute( "ytitlecolor" ) );
        }
        if (graphcolor.hasAttribute( "ytitle2color" )) {
            m_params->YTitle2Color= QColor( graphcolor.attribute( "ytitle2color" ) );
        }
        if (graphcolor.hasAttribute( "xlabelcolor" )) {
            m_params->XLabelColor= QColor( graphcolor.attribute( "xlabelcolor" ) );
        }
        if (graphcolor.hasAttribute( "ylabelcolor" )) {
            m_params->YLabelColor= QColor( graphcolor.attribute( "ylabelcolor" ) );
        }
        if (graphcolor.hasAttribute( "ylabel2color" )) {
            m_params->YLabel2Color= QColor( graphcolor.attribute( "ylabel2color" ) );
        }
    }

    QDomElement annotation = params.namedItem( "annotation" ).toElement();
    if (!annotation.isNull()) {
        m_params->annotation=new KChartAnnotationType;
        if (annotation.hasAttribute( "color" )) {
            m_params->annotation->color= QColor( annotation.attribute( "color" ) );
        }
        if (annotation.hasAttribute( "point" )) {
            m_params->annotation->point=annotation.attribute("point").toDouble( &ok );
            if (!ok) return false;
        }
    }
    QDomElement note = params.namedItem( "note" ).toElement();
    if ( !note.isNull()) {
        QString t = note.text();
        m_params->annotation->note=t;
    }

    QDomElement scatter = params.namedItem( "scatter" ).toElement();
    if ( !scatter.isNull() ) {
        m_params->scatter = new KChartScatterType;
        if ( scatter.hasAttribute( "point" ) ) {
            m_params->scatter->point = scatter.attribute( "point" ).toDouble( &ok );
            if ( !ok )
                return false;
        }
        if ( scatter.hasAttribute( "val" ) ) {
            m_params->scatter->val = scatter.attribute( "val" ).toDouble( &ok );
            if ( !ok )
                return false;
        }
        if ( scatter.hasAttribute( "width" ) ) {
            m_params->scatter->width = scatter.attribute( "val" ).toUShort( &ok );
            if ( !ok )
                return false;
        }
        if ( scatter.hasAttribute( "color" )) {
            m_params->scatter->color= QColor( scatter.attribute( "color" ) );
        }
        if ( scatter.hasAttribute( "ind" ) ) {
            m_params->scatter->ind = (KChartScatterIndType)scatter.attribute( "ind" ).toInt( &ok );
            if ( !ok )
                return false;
        }
    }

    QDomElement legend = chart.namedItem("legend").toElement();
    if (!legend.isNull()) {
        int number = legend.attribute("number").toInt(&ok);
        if (!ok)  { return false; }
        QDomNode name = legend.firstChild();
        m_params->legend.clear();
        for(int i=0; i<number; i++) {
            if (name.isNull()) {
                kdDebug(35001) << "Some problems, there is less data than it should be!" << endl;
                break;
            }
            QDomElement element = name.toElement();
            if ( !element.isNull() && element.tagName() == "name" ) {
                QString t = element.text();
                m_params->legend+=t;
                name = name.nextSibling();
            }
        }

    }
    QDomElement xlbl = chart.namedItem("xlbl").toElement();
    if (!xlbl.isNull()) {
        int number = xlbl.attribute("number").toInt(&ok);
        if (!ok)  { return false; }
        QDomNode label = xlbl.firstChild();
        m_params->xlbl.clear();
        for (int i=0; i<number; i++) {
            if (label.isNull()) {
                kdDebug(35001) << "Some problems, there is less data than it should be!" << endl;
                break;
            }
            QDomElement element = label.toElement();
            if ( !element.isNull() && element.tagName() == "label" ) {
                QString t = element.text();
                m_params->xlbl+=t;
                label = label.nextSibling();
            }
        }
    }

    QDomElement backgroundPixmap = chart.namedItem( "backgroundPixmap" ).toElement();
    if ( !backgroundPixmap.isNull() ) {
        if ( backgroundPixmap.hasAttribute( "name" ) )
            m_params->backgroundPixmapName = backgroundPixmap.attribute( "name" );
        if ( backgroundPixmap.hasAttribute( "isDirty" ) ) {
            m_params->backgroundPixmapIsDirty = (bool)backgroundPixmap.attribute( "isDirty" ).toInt( &ok );
            if ( !ok )
                return false;
        }
        if ( backgroundPixmap.hasAttribute( "scaled" ) ) {
            m_params->backgroundPixmapScaled = (bool)backgroundPixmap.attribute( "scaled" ).toInt( &ok );
            if ( !ok )
                return false;
        }
        if ( backgroundPixmap.hasAttribute( "centered" ) ) {
            m_params->backgroundPixmapCentered = (bool)backgroundPixmap.attribute( "centered" ).toInt( &ok );
            if ( !ok )
                return false;
        }
        if ( backgroundPixmap.hasAttribute( "intensity" ) ) {
            m_params->backgroundPixmapIntensity = backgroundPixmap.attribute( "intensity" ).toFloat( &ok );
            if ( !ok )
                return false;
        }
    }

    QDomElement extcolor = chart.namedItem("extcolor").toElement();
    if (!extcolor.isNull()) {
        unsigned int number = extcolor.attribute("number").toInt(&ok);
        if (!ok)  { return false; }
        QDomNode color = extcolor.firstChild();

        for (unsigned int i=0; i<number; i++) {
            if (color.isNull()) {
                kdDebug(35001) << "Some problems, there is less data than it should be!" << endl;
                break;
            }
            QDomElement element = color.toElement();
            if ( !element.isNull()) {
                if (element.hasAttribute( "name" )) {
                    m_params->ExtColor.setColor(i,QColor( element.attribute( "name" ) ));
                }
                color = color.nextSibling();
            }
        }
    }
    if ( !m_params->backgroundPixmapName.isNull() ) {
        m_params->backgroundPixmap.load( locate( "wallpaper", m_params->backgroundPixmapName ));
        m_params->backgroundPixmapIsDirty = true;
    }
#endif
    return true;
}



void  KChartPart::slotModified()
{
    kdDebug(35001) << "slotModified called!" << endl;

    setModified(true);
}


}  //KChart namespace

#include "kchart_part.moc"
