/**
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#include <float.h> // For basic data types characteristics.

// For debugging
#include <iostream>
//Added by qt3to4:
#include <Q3ValueList>
using std::cout;
using std::cerr;

#include "kchart_part.h"
#include "kchart_view.h"
#include "kchart_factory.h"
#include "kchartWizard.h"
#include "kchart_params.h"
#include "kdchart/KDChart.h"
#include "kdchart/KDChartTable.h"

#include <KoDom.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoOasisStore.h>
#include <KoOasisLoadingContext.h>

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

KChartPart::KChartPart( QWidget *parentWidget,
			QObject* parent,
			bool singleViewMode )
  : KoChart::Part( parentWidget, parent, singleViewMode ),
    m_params( 0 ),
    m_parentWidget( parentWidget ),
    m_rowLabels(), m_colLabels()
{
    kDebug(35001) << "Constructor started!" << endl;

    setInstance( KChartFactory::global(), false );
    setTemplateType( "kchart_template" );

    // Init some members that need it.
    {
	// Create the chart parameters and let the default be a bar chart
	// with 3D looks.
	m_params = new KChartParams( this );
	m_params->setChartType( KChartParams::Bar );
	m_params->setBarChartSubType( KChartParams::BarNormal );
	m_params->setThreeDBars( true );

        //Changed this to use columns rather than rows by default
        //because I believe that this is the more common format for
        //entering data (you can see this looking at the fact that
        //most spreadsheet packages allow far more rows than columns)
        //-- Robert Knight

	// Handle data in columns by default
	m_params->setDataDirection( KChartParams::DataColumns );
    }

    (void)new WizardExt( this );
    m_bCanChangeValue = true;

    // Display parameters
    m_displayData = m_currentData;

    // Set the size to minimal.
    initEmpty();
}


KChartPart::~KChartPart()
{
    //kDebug(35001) << "Part is going to be destroyed now!!!" << endl;
    delete m_params;
}


void KChartPart::initEmpty()
{
    initNullChart();

    resetURL();
    setEmpty();
}


// This method creates the simplest chart imaginable:
// Data size 1x1, empty, no headers
//
void KChartPart::initNullChart()
{
    // Fill cells with data if there is none.
    //kDebug(35001) << "Initialize null chart." << endl;

    // Empty data.  Note, we don't use (0,0) or (1,1) for the size
    // here, because otherwise KDChart won't draw anything
    m_currentData.expand(2, 2);
    m_params->setFirstRowAsLabel(false);
    m_params->setFirstColAsLabel(false);

    // Fill column and row labels.
    m_colLabels << QString("");
    m_rowLabels << QString("");

    setChartDefaults();

    m_params->setDrawSolidExcessArrows(true);
}


void KChartPart::generateBarChartTemplate()
{
    int  col;
    int  row;

    kDebug()<<"KChartPart::initTestChart()\n";

    // Fill cells with data if there is none.
    if (m_currentData.rows() == 0) {
        //kDebug(35001) << "Initialize with some data!!!" << endl;
        m_currentData.expand( 4, 4 );
        m_currentData.setUsedRows( 4 );
        m_currentData.setUsedCols( 4 );
        for (row = 0; row < 4; row++) {
            for (col = 0; col < 4; col++) {
                m_currentData.setCell(row, col,
				      static_cast <double> (row + col));

		// Fill column label, but only on the first iteration.
		if (row == 0) {
		    m_colLabels << i18n("Column %1",col + 1);
		}
            }

	    // Fill row label.
	    m_rowLabels << i18n("Row %1",row + 1);
	}
    }

    setChartDefaults();
    // FIXME: Should this go into setChartDefaults()?
    m_params->setDrawSolidExcessArrows(true);
}


KoView* KChartPart::createViewInstance( QWidget* parent, const char* name )
{
    return new KChartView( this, parent, name );
}


// ================================================================
//                              Painting


void KChartPart::paintContent( QPainter& painter, const QRect& rect,
			       bool /*transparent*/,
			       double /*zoomX*/, double /*zoomY*/ )
{
    // If params is 0, initDoc() has not been called.
    Q_ASSERT( m_params != 0 );

    KDChartAxisParams  bottomparms;
    bottomparms = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

    // Handle data in rows or columns.
    //
    // This means getting row or column headers from the document and
    // set them as X axis labels or legend according to the current
    // setting.  Also, transpose the data if it should be displayed in
    // columns instead of in rows.


    // Create the displayData table.
    createDisplayData();

    // Create the axis labels and legend.
    QStringList  longLabels;
    QStringList  shortLabels;

    longLabels.clear();
    shortLabels.clear();

    const uint dataColumnCount = m_currentData.cols();
    const uint dataRowCount = m_currentData.rows();
    const uint columnLabelCount = m_colLabels.count();
    const uint rowLabelCount = m_rowLabels.count();

    if (m_params->dataDirection() == KChartParams::DataRows) {
	// Data is handled in rows.  This is the way KDChart works also.

	// Set X axis labels from column headers.
	for ( uint col = 0; col < dataColumnCount; col++ ) {

            QString label = (col < columnLabelCount) ? m_colLabels[col] : QString::null;

            longLabels  << label;
	    shortLabels << label.left( 3 );
	}

	// Set legend from row headers.
        for ( uint row = 0; row < dataRowCount; row++ ) {
            QString label = (row < rowLabelCount) ? m_rowLabels[row] : QString::null;

            m_params->setLegendText( row, label );
        }
    }
    else {
	// Data is handled in columns.

	// Set X axis labels from row headers.
	for ( uint row = 0; row < dataRowCount; row++ ) {

            QString label = (row < rowLabelCount) ? m_rowLabels[row] : QString::null;

            longLabels  << label;
	    shortLabels << label.left( 3 );
	}

	// Set legend from column headers.
        for ( uint col = 0; col < dataColumnCount ; col++ ) {
            QString label = (col < columnLabelCount) ? m_colLabels[col] : QString::null;

            m_params->setLegendText( col, label );
        }
    }

    // FIXME: In a HiLo chart, the Legend should be the same as the
    //        labels on the X Axis.  Should we disable one of them?
    if (m_params->chartType() == KChartParams::HiLo) {

	// Set the correct X axis labels and legend.
	longLabels.clear();
	shortLabels.clear();
	if (m_params->dataDirection() == KChartParams::DataRows) {

	    // If data are in rows, then the X axis labels should be
	    // taken from the row headers.
	    for ( uint row = 0; row < dataRowCount ; row++ ) {

                QString label = (row < rowLabelCount) ? m_rowLabels[row] : QString::null;

                longLabels  << label;
		shortLabels << label.left( 3 );
	    }
	}
	else {
	    // If data are in columns, then the X axis labels should
	    // be taken from the column headers.
	    for ( uint col = 0; col < dataColumnCount; col++ ) {

                QString label = (col < columnLabelCount) ? m_colLabels[col] : QString::null;

                longLabels  << m_colLabels[col];
		shortLabels << m_colLabels[col].left( 3 );
	    }
	}
    }

    // Actually set the axis labels.
    bottomparms.setAxisLabelStringLists( &longLabels, &shortLabels );
    m_params->setAxisParams(KDChartAxisParams::AxisPosBottom, bottomparms);

    // Ok, we have now created a data set for display, and params with
    // suitable legends and axis labels.  Now start the real painting.

    // Handle transparency.
    // Wrong: this flickers; better do this as part of the double-buffering.
    //if ( !transparent )
    //    painter.eraseRect( rect );

    // ## TODO: support zooming

    // Double-buffering
    if ( m_bufferPixmap.width() < rect.width()
	 || m_bufferPixmap.height() < rect.height() )
    {
    	m_bufferPixmap.resize( rect.size() );
    }

    QPainter bufferPainter( &m_bufferPixmap );

    // We only need to draw the document rectangle "rect".
    KDChart::paint( &bufferPainter, m_params, &m_displayData, 0, &rect );

    // This is always the empty rect...
    // Shouldn't creating a QPainter in a paintEvent set up clipping automatically?
    // I thought it did (DF)
    //const QRect clipRect = painter.clipRegion().boundingRect();
    //painter.drawPixmap( clipRect.topLeft(), m_bufferPixmap, clipRect );

    painter.drawPixmap( 0, 0, m_bufferPixmap );
}


// Create the data table m_displayData from m_currentData, taking into
// account if the first row or line contains headers.  The chart type
// HiLo demands special handling.
//
//
// Note: While the current KD Chart 1.1.3 version is still expecting data
//       to be in rows, the upcoming KD Chart 2.0 release will be using
//       data in columns instead, to it will be matching KSpread's way.
//       -khz, 2005-11-15
//
// FIXME: Rewrite so that we only copy data when necessary.
//        On the other hand, the next version of KDChart is able to
//        get data directly without storing it into a KDChartData
//        class first, so we might never need to.
//
void KChartPart::createDisplayData()
{
    int  rowOffset   = 0;
    int  colOffset   = 0;
    int  numDatasets = 0;

    if ( m_params->firstRowAsLabel() )
	rowOffset++;
    if ( m_params->firstColAsLabel() )
	colOffset++;

    // After this sequence, m_DisplayData contains the data in the
    // correct transposition, and the X axis (represented by
    // bottomparms) and the legend contain the correct labels.
    QVariant  value1;
    QVariant  value2;
    int       prop;
    if (m_params->dataDirection() == KChartParams::DataRows) {
	// Data is handled in rows.  This is the way KDChart works also.

	numDatasets = m_currentData.usedRows() - rowOffset;
	m_displayData.expand( numDatasets,
			      m_currentData.usedCols() - colOffset );

	for (uint row = rowOffset; row < m_currentData.usedRows(); row++) {
	    for (uint col = colOffset; col < m_currentData.usedCols(); col++) {
		if ( m_currentData.cellContent( row, col,
						value1, value2, prop ) ) {
		    m_displayData.setCell(row - rowOffset, col - colOffset,
					  value1, value2);
		    m_displayData.setProp(row - rowOffset, col - colOffset,
					  prop);
                }

	    }
	}
    }
    else {
	// Data is handled in columns.  We will have to transpose
	// everything since KDChart wants its data in rows.

	// Resize displayData so that the transposed data has room.
	numDatasets = m_currentData.usedCols() - colOffset;
	m_displayData.expand( numDatasets,
			      m_currentData.usedRows() - rowOffset );

	// Copy data and transpose it.
	for (uint row = colOffset; row < m_currentData.usedCols(); row++) {
	    for (uint col = rowOffset; col < m_currentData.usedRows(); col++) {
                if ( m_currentData.cellContent( col, row,
						value1, value2, prop ) ) {
		    m_displayData.setCell(row - colOffset, col - rowOffset,
					  value1, value2);
		    m_displayData.setProp(row - colOffset, col - rowOffset,
					  prop);
                }
	    }
	}
    }

    if ( m_params->chartType() == KChartParams::Bar
	 && m_params->barNumLines() > 0 ) {

        // Specify that we want to have an additional chart.
        m_params->setAdditionalChartType( KDChartParams::Line );

        const int numBarDatasets = numDatasets - m_params->barNumLines();

        // assign the datasets to the charts: DataEntry, from, to, chart#
        m_params->setChartSourceMode( KDChartParams::DataEntry,
                                      0, numBarDatasets - 1,
                                      0 ); // The bar chart
        m_params->setChartSourceMode( KDChartParams::DataEntry,
                                      numBarDatasets, numDatasets - 1,
                                      1 ); // The line chart
    }
    else {
	// Otherwise we don't want any extra chart.
        m_params->setAdditionalChartType( KDChartParams::NoType );
    }

    // If this is a HiLo chart, we need to manually create the correct
    // values.  This is not done by KDChart.
    //
    // Here we don't need to transpose, since we can start from the
    // newly generated displayData.
    if (m_params->chartType() == KChartParams::HiLo) {
	KDChartTableData  tmpData = m_displayData;

	// Calculate the min, max, open and close values for each row.
	m_displayData.expand(tmpData.usedRows(), 4);
	for (uint row = 0; row < tmpData.usedRows(); row++) {
	    double  minVal   = DBL_MAX;
	    double  maxVal   = -DBL_MAX;

	    // Calculate min and max for this row.
	    //
	    // Note that we have already taken care of different data
	    // directions above.
	    for (uint col = 0; col < tmpData.usedCols(); col++) {
		double  data = tmpData.cellVal(row, col).toDouble();

		if (data < minVal)
		    minVal = data;
		if (data > maxVal)
		    maxVal = data;
	    }
	    m_displayData.setCell(row, 0, minVal); // min
	    m_displayData.setCell(row, 1, maxVal); // max
	    m_displayData.setCell(row, 2, tmpData.cellVal(row, 0).toDouble());   // open
	    m_displayData.setCell(row, 3,                          // close
				  tmpData.cellVal(row, tmpData.usedCols() - 1).toDouble());
	}
    }
}


// ================================================================


void KChartPart::analyzeHeaders()
{
#if 0
    analyzeHeaders( m_currentData );
#else
    doSetData( m_currentData,
	       m_params->firstRowAsLabel(), m_params->firstColAsLabel());
#endif
}


// This function sets the data from an external source.  It is called,
// for instance, when the chart is initialized from a spreadsheet in
// KSpread.
//
void KChartPart::analyzeHeaders( const KDChartTableData& data )
{
    // FIXME(khz): replace this when automatic string detection works in KDChart
    // Does the top/left cell contain a string?
    bool isStringTopLeft = (data.cellVal( 0, 0 ).type() == QVariant::String);

    // Does the first row (without first cell) contain only strings?
    bool isStringFirstRow = true;
    for ( uint col = 1; isStringFirstRow && col < data.cols(); col++ ) {
        isStringFirstRow = (data.cellVal( 0, col ).type() == QVariant::String);
    }

    // Just in case, we only have 1 row, we never use it for label text.
    // This prevents a crash.
    //
    // FIXME: Wonder if this is still true for KDChart 1.1.3 / iw
    //        Disabling...
#if 1
    if ( data.rows() == 1 )
        isStringFirstRow = false;
#endif

    // Does the first column (without first cell) contain only strings?
    bool isStringFirstCol = true;
    for ( uint row = 1; isStringFirstCol && row < data.rows(); row++ ) {
        isStringFirstCol = (data.cellVal( row, 0 ).type() == QVariant::String);
    }

    // Just in case, we only have 1 column, we never use it for axis
    // label text => prevents crash.
#if 1
    if ( data.cols() == 1 )
        isStringFirstRow = FALSE;
#endif

    bool hasColHeader = false;
    bool hasRowHeader = false;

    // Let's check if we have a full axis label text column
    if ( isStringFirstCol && isStringTopLeft
	 || isStringFirstCol && isStringFirstRow )
        hasColHeader = true;

    // Let's check if we have a full label text row.
    if ( isStringFirstRow && isStringTopLeft
	 || isStringFirstCol && isStringFirstRow )
        hasRowHeader = true;

    m_params->setFirstRowAsLabel( hasRowHeader );
    m_params->setFirstColAsLabel( hasColHeader );

    doSetData(data, hasRowHeader, hasColHeader);
}



void KChartPart::doSetData( const KDChartTableData&  data,
			    bool  hasRowHeader,
			    bool  hasColHeader )
{
    uint  rowStart = 0;
    uint  colStart = 0;
    uint  col;
    uint  row;

    // Disable this function.  Maybe we should remove this function
    // altogether, since I was doing some pretty fuzzy thinking when I
    // implemented it.
    return;

    if (hasRowHeader)
      rowStart = 1;
    if (hasColHeader)
      colStart = 1;

    // Generate legend from the column headers if applicable.
    m_rowLabels.clear();
    if ( hasColHeader ) {
        for( row = rowStart; row < data.rows(); row++ )
	    m_rowLabels << data.cellVal( row, 0 ).toString();
    }
    else
        m_params->setLegendSource( KDChartParams::LegendAutomatic );

    // Generate X labels from the row headers if applicable
    m_colLabels.clear();
    if ( hasRowHeader ) {
        for( col = colStart; col < data.cols(); col++ )
	    m_colLabels << data.cellVal( 0, col ).toString();
    }

#if 0
    // If there is a row header and/or a column header, then generate
    // the data that will be used for the chart by translating the
    // original data.
    if ( hasColHeader || hasRowHeader ) {
        KDChartTableData matrix( data.rows() - rowStart, data.cols() - colStart );

        for ( col = colStart; col < data.cols(); col++ ) {
            for ( row = rowStart; row < data.rows(); row++ ) {
                matrix.setCell( row - rowStart, col - colStart,
				data.cellVal( row, col ).toDouble() );
            }
        }
        m_currentData = matrix;
    }
    else
#endif
        m_currentData = data;

    setChartDefaults();

    emit docChanged();
}


void KChartPart::resizeData( int rows, int cols )
{
    m_currentData.expand( rows, cols );
    m_currentData.setUsedRows( rows );
    m_currentData.setUsedCols( cols );
}


void KChartPart::setCellData( int row, int column, const QVariant &val)
{
    m_currentData.setCell( row, column, val );
}


bool KChartPart::showWizard( QString &area )
{
    KChartWizard  *wizard = new KChartWizard( this, m_parentWidget, "wizard" );

    connect( wizard, SIGNAL(finished()), this, SLOT(slotModified()) );

    wizard->setDataArea( area );

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

void KChartPart::setChartDefaults()
{
  //
  // Settings for the Y axis.
  //
  KDChartAxisParams  yAxis;
  yAxis = m_params->axisParams( KDChartAxisParams::AxisPosLeft );

  // decimal symbol and thousands separator
  yAxis.setAxisLabelsRadix( KGlobal::locale()->decimalSymbol(),
			    KGlobal::locale()->thousandsSeparator() );

  m_params->setAxisParams( KDChartAxisParams::AxisPosLeft, yAxis );

  //
  // Settings for the X axis.
  //
  KDChartAxisParams  xAxis;
  xAxis = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

  // These two shouldn't be necessary to set.
  xAxis.setAxisFirstLabelText();
  xAxis.setAxisLastLabelText();

  m_params->setAxisParams( KDChartAxisParams::AxisPosBottom, xAxis );

  // Other parameters for various things.
  m_params->setLineColor();

  // Setting the background layer.
  KDFrame frame;
  frame.setBackground( QBrush( QColor( 230, 222, 222 ) ) );
  m_params->setFrame( KDChartEnums::AreaInnermost, frame, 0, 0, 0, 0 );
}


// ================================================================
//                     Loading and Storing


// ----------------------------------------------------------------
//                 Save and Load program configuration



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
    //   shelf = conf->readEntry("shelf", shelf);
    //   grid = conf->readEntry("grid", grid);
    //   xaxis = conf->readEntry("xaxis", xaxis);
    //   yaxis = conf->readEntry("yaxis", yaxis);
    //   yaxis2 = conf->readEntry("yaxis2", yaxis);
    //   llabel = conf->readEntry("llabel", llabel);
    //   yval_style = conf->readNumEntry("yval_style", yval_style);
    //   stack_type = (KChartStackType)conf->readNumEntry("stack_type", stack_type);
    m_params->setLineMarker(conf->readEntry("lineMarker",
						m_params->lineMarker()));
    m_params->setThreeDBarDepth( conf->readDoubleNumEntry("_3d_depth",
							  m_params->threeDBarDepth() ) );
    m_params->setThreeDBarAngle( conf->readNumEntry( "_3d_angle",
						     m_params->threeDBarAngle() ) );

    KDChartAxisParams  leftparams;
    leftparams = m_params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams  rightparams;
    rightparams = m_params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams  bottomparams;
    bottomparams = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

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
    //   thumbnail = conf->readEntry("thumbnail", thumbnail);
    //   thumblabel = conf->readEntry("thumblabel", thumblabel);
    //   border = conf->readEntry("border", border);
    //   BGColor = conf->readColorEntry("BGColor", &BGColor);
    //   PlotColor = conf->readColorEntry("PlotColor", &PlotColor);
    //   VolColor = conf->readColorEntry("VolColor", &VolColor);
    //   EdgeColor = conf->readColorEntry("EdgeColor", &EdgeColor);
    //   loadColorArray(conf, &SetColor, "SetColor");
    //   loadColorArray(conf, &ExtColor, "ExtColor");
    //   loadColorArray(conf, &ExtVolColor, "ExtVolColor");
    //   transparent_bg = conf->readEntry("transparent_bg", transparent_bg);
    //   // TODO: explode, missing
    //   percent_labels = (KChartPercentType)conf->readNumEntry("percent_labels",
    // 							 percent_labels);
    //   label_dist = conf->readNumEntry("label_dist", label_dist);
    //   label_line = conf->readEntry("label_line", label_line);
    m_params->setChartType( (KChartParams::ChartType)conf->readNumEntry( "type", m_params->chartType() ) );
    //   other_threshold = conf->readNumEntry("other_threshold", other_threshold);

    //   backgroundPixmapName = conf->readPathEntry( "backgroundPixmapName" );
    //   if( !backgroundPixmapName.isNull() ) {
    //     backgroundPixmap.load( KStandardDirs::locate( "wallpaper", backgroundPixmapName ));
    //     backgroundPixmapIsDirty = true;
    //   } else
    //     backgroundPixmapIsDirty = false;
    //   backgroundPixmapScaled = conf->readEntry( "backgroundPixmapScaled", true );
    //   backgroundPixmapCentered = conf->readEntry( "backgroundPixmapCentered", true );
    //   backgroundPixmapIntensity = conf->readDoubleNumEntry( "backgroundPixmapIntensity", 0.25 );
}


void KChartPart::defaultConfig(  )
{
    delete m_params;
    m_params = new KChartParams( this );
    setChartDefaults();
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

    KDChartAxisParams leftparams;
    leftparams   = m_params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams rightparams;
    rightparams  = m_params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams bottomparams;
    bottomparams = m_params->axisParams( KDChartAxisParams::AxisPosBottom );
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


// ----------------------------------------------------------------
//              Save and Load OpenDocument file format


bool KChartPart::loadOasis( const QDomDocument& doc,
			    KoOasisStyles&      oasisStyles,
			    const QDomDocument& /*settings*/,
			    KoStore            *store )
{
    kDebug(35001) << "kchart loadOasis called" << endl;

    // Set some sensible defaults.
    setChartDefaults();

    QDomElement  content = doc.documentElement();
    QDomElement  bodyElem ( KoDom::namedItemNS( content,
						KoXmlNS::office, "body" ) );
    if ( bodyElem.isNull() ) {
        kError(32001) << "No office:body found!" << endl;
        setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return false;
    }

    // Get the office:chart element.
    QDomElement officeChartElem = KoDom::namedItemNS( bodyElem,
						      KoXmlNS::office, "chart" );
    if ( officeChartElem.isNull() ) {
        kError(32001) << "No office:chart found!" << endl;
        QDomElement  childElem;
        QString      localName;
        forEachElement( childElem, bodyElem ) {
            localName = childElem.localName();
        }

        if ( localName.isEmpty() )
            setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No tag found inside office:body." ) );
        else
            setErrorMessage( i18n( "This document is not a chart, but %1. Please try opening it with the appropriate application." , KoDocument::tagNameToDocumentType( localName ) ) );

        return false;
    }

    QDomElement chartElem = KoDom::namedItemNS( officeChartElem,
						KoXmlNS::chart, "chart" );
    if ( chartElem.isNull() ) {
        setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No chart:chart tag found." ) );
        return false;
    }

    // Get the loading context and stylestack from the styles.
    KoOasisLoadingContext  loadingContext( this, oasisStyles, store );
    //KoStyleStack          &styleStack = loadingContext.styleStack();

#if 0  // Example code!!
    // load chart properties into the stylestack.
    styleStack.save();
    styleStack.setTypeProperties( "chart" ); // load chart properties
    loadingContext.fillStyleStack( chartElem, KoXmlNS::chart, "style-name" );

    const QString fillColor = styleStack.attributeNS( KoXmlNS::draw, "fill-color" );
    kDebug() << "fillColor=" << fillColor << endl;

    styleStack.restore();
#endif

    // Load chart parameters parameters, most of these are stored in
    // the chart:plot-area element within chart:chart.
    QString  errorMessage;
    bool     ok = m_params->loadOasis( chartElem, loadingContext, errorMessage,
				       store);
    if ( !ok ) {
        setErrorMessage( errorMessage );
        return false;
    }

    // TODO Load data direction (see loadAuxiliary)

    // Load the data table.
    QDomElement tableElem = KoDom::namedItemNS( chartElem,
						KoXmlNS::table, "table" );
    if ( !tableElem.isNull() ) {
        ok = loadOasisData( tableElem );
        if ( !ok )
            return false; // TODO setErrorMessage
    }

    return true;
}


bool KChartPart::loadOasisData( const QDomElement& tableElem )
{
    int          numberHeaderColumns = 0;
    QDomElement  tableHeaderColumns = KoDom::namedItemNS( tableElem,
							  KoXmlNS::table,
							  "table-header-columns" );

    QDomElement  elem;
    forEachElement( elem, tableHeaderColumns ) {
        if ( elem.localName() == "table-column" ) {
            int repeated = elem.attributeNS( KoXmlNS::table, "number-columns-repeated", QString::null ).toInt();
            numberHeaderColumns += qMax( 1, repeated );
        }
    }

    // With 0 you get no titles, and with more than 1 we ignore the others.
    Q_ASSERT( numberHeaderColumns == 1 );

    int numberDataColumns = 0;
    QDomElement tableColumns = KoDom::namedItemNS( tableElem, KoXmlNS::table, "table-columns" );
    forEachElement( elem, tableColumns ) {
        if ( elem.localName() == "table-column" ) {
            int repeated = elem.attributeNS( KoXmlNS::table, "number-columns-repeated", QString::null ).toInt();
            numberDataColumns += qMax( 1, repeated );
        }
    }

    // Parse table-header-rows for the column names.
    m_colLabels.clear();
    QDomElement tableHeaderRows = KoDom::namedItemNS( tableElem, KoXmlNS::table, "table-header-rows" );
    if ( tableHeaderRows.isNull() )
        kWarning(35001) << "No table-header-rows element found!" << endl;
    QDomElement tableHeaderRow = KoDom::namedItemNS( tableHeaderRows, KoXmlNS::table, "table-row" );
    if ( tableHeaderRow.isNull() )
        kWarning(35001) << "No table-row inside table-header-rows!" << endl;

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
    numberDataColumns = qMax( numberDataColumns, cellNum - numberHeaderColumns );
    if ( (int)m_colLabels.count() != numberDataColumns )
        kWarning(35001) << "Got " << m_colLabels.count()
			 << " column titles, expected " << numberDataColumns
			 << endl;

    // Get the number of rows, and read row labels
    int          numberDataRows = 0;
    QDomElement  tableRows = KoDom::namedItemNS( tableElem, KoXmlNS::table, "table-rows" );

    m_rowLabels.clear();
    forEachElement( elem, tableRows ) {
        if ( elem.localName() == "table-row" ) {
            int repeated = elem.attributeNS( KoXmlNS::table, "number-rows-repeated", QString::null ).toInt();
            Q_ASSERT( repeated <= 1 ); // we don't handle yet the case where data rows are repeated (can this really happen?)
            numberDataRows += qMax( 1, repeated );
            if ( numberHeaderColumns > 0 ) {
                QDomElement firstCell = KoDom::namedItemNS( elem, KoXmlNS::table, "table-cell" );
                QDomElement pElem = KoDom::namedItemNS( firstCell, KoXmlNS::text, "p" );
                m_rowLabels.append( pElem.text() );
            }
        }
    }

    kDebug(35001) << "numberHeaderColumns=" << numberHeaderColumns
		   << " numberDataColumns=" << numberDataColumns
                   << " numberDataRows=" << numberDataRows << endl;

    if ( (int)m_rowLabels.count() != numberDataRows)
        kWarning(35001) << "Got " << m_rowLabels.count()
			 << " row labels, expected " << numberDataRows << endl;

    m_currentData.expand( numberDataRows, numberDataColumns );
    m_currentData.setUsedCols( numberDataColumns );
    m_currentData.setUsedRows( numberDataRows );

    // Now really load the cells.
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
                            kWarning(35001) << "Don't know how to handle value-type " << valueType << endl;
                        else {
                            QString  value = cellElem.attributeNS( KoXmlNS::office, "value", QString::null );
                            double   val = value.toDouble();

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


bool KChartPart::saveOasis( KoStore* store, KoXmlWriter* manifestWriter )
{
    manifestWriter->addManifestEntry( "content.xml", "text/xml" );
    KoOasisStore oasisStore( store );

    KoXmlWriter* contentWriter = oasisStore.contentWriter();
    if ( !contentWriter )
        return false;

    KoGenStyles mainStyles;

    KoXmlWriter* bodyWriter = oasisStore.bodyWriter();
    bodyWriter->startElement( "office:body" );
    bodyWriter->startElement( "office:chart" );
    bodyWriter->startElement( "chart:chart" );

    // Indent to indicate that this is inside some tags.
    {
        // Saves chart class, title, legend, plot-area
        m_params->saveOasis( bodyWriter, mainStyles );

	// Save the data table.
        saveOasisData( bodyWriter, mainStyles );
    }

    bodyWriter->endElement(); // chart:chart
    bodyWriter->endElement(); // office:chart
    bodyWriter->endElement(); // office:body

    contentWriter->startElement( "office:automatic-styles" );
    writeAutomaticStyles( *contentWriter, mainStyles );
    contentWriter->endElement(); // office:automatic-styles

    oasisStore.closeContentWriter();

    // Done with content.xml

#if 0
    if ( !store->open( "styles.xml" ) )
        return false;
    manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
    saveOasisDocumentStyles( store, mainStyles, savingContext, saveFlag,
			     headerFooterContent );
    if ( !store->close() ) // done with styles.xml
        return false;
#endif

    return true;
}


void KChartPart::saveOasisData( KoXmlWriter* bodyWriter,
				KoGenStyles& mainStyles ) const
{
    Q_UNUSED( mainStyles );

    const int cols = m_currentData.usedCols()
                     ? qMin(m_currentData.usedCols(), m_currentData.cols())
                     : m_currentData.cols();
    const int rows = m_currentData.usedRows()
                     ? qMin(m_currentData.usedRows(), m_currentData.rows())
                     : m_currentData.rows();

    bodyWriter->startElement( "table:table" );
    bodyWriter->addAttribute( "table:name", "local-table" );

    // Exactly one header column, always.
    bodyWriter->startElement( "table:table-header-columns" );
    bodyWriter->startElement( "table:table-column" );
    bodyWriter->endElement(); // table:table-column
    bodyWriter->endElement(); // table:table-header-columns

    // Then "cols" columns
    bodyWriter->startElement( "table:table-columns" );
    bodyWriter->startElement( "table:table-column" );
    bodyWriter->addAttribute( "table:number-columns-repeated", cols );
    bodyWriter->endElement(); // table:table-column
    bodyWriter->endElement(); // table:table-columns

    // Exactly one header row, always.
    bodyWriter->startElement( "table:table-header-rows" );
    bodyWriter->startElement( "table:table-row" );

    // The first column in header row is just the header column - no title needed
    bodyWriter->startElement( "table:table-cell" );
    bodyWriter->addAttribute( "office:value-type", "string" );
    bodyWriter->startElement( "text:p" );
    bodyWriter->endElement(); // text:p
    bodyWriter->endElement(); // table:table-cell

    // Save column labels in the first header row, for instance:
    //          <table:table-cell office:value-type="string">
    //            <text:p>Column 1 </text:p>
    //          </table:table-cell>
    QStringList::const_iterator colLabelIt = m_colLabels.begin();
    for ( int col = 0; col < cols ; ++col ) {
        if ( colLabelIt != m_colLabels.end() ) {
            bodyWriter->startElement( "table:table-cell" );
            bodyWriter->addAttribute( "office:value-type", "string" );
            bodyWriter->startElement( "text:p" );
            bodyWriter->addTextNode( *colLabelIt );
            bodyWriter->endElement(); // text:p
            bodyWriter->endElement(); // table:table-cell
            ++colLabelIt;
        }
    }

    bodyWriter->endElement(); // table:table-row
    bodyWriter->endElement(); // table:table-header-rows
    bodyWriter->startElement( "table:table-rows" );

    QStringList::const_iterator rowLabelIt = m_rowLabels.begin();
    for ( int row = 0; row < rows ; ++row ) {
        bodyWriter->startElement( "table:table-row" );

        if ( rowLabelIt != m_rowLabels.end() ) {
            // Save row labels, similar to column labels
            bodyWriter->startElement( "table:table-cell" );
            bodyWriter->addAttribute( "office:value-type", "string" );

            bodyWriter->startElement( "text:p" );
            bodyWriter->addTextNode( *rowLabelIt );
            bodyWriter->endElement(); // text:p

            bodyWriter->endElement(); // table:table-cell
            ++rowLabelIt;
        }

        for ( int col = 0; col < cols; ++col ) {
            QVariant value( m_currentData.cellVal( row, col ) );
            QString  valType;
            QString  valStr;

            switch ( value.type() ) {
            case QVariant::Invalid:
		break;
            case QVariant::String:
		valType = "string";
		valStr  = value.toString();
		break;
            case QVariant::Double:
		valType = "float";
		valStr  = QString::number( value.toDouble(), 'g', DBL_DIG );
		break;
            case QVariant::DateTime:
		valType = "date";
		valStr  = ""; /* like in saveXML, but why? */
		break;
            default: {
                kDebug(35001) << "ERROR: cell " << row << "," << col
                               << " has unknown type." << endl;
                }
            }

	    // Add the value type and the string to the XML tree.
            bodyWriter->startElement( "table:table-cell" );
            if ( !valType.isEmpty() ) {
                bodyWriter->addAttribute( "office:value-type", valType );
                if ( value.type() == QVariant::Double )
                    bodyWriter->addAttribute( "office:value", valStr );

                bodyWriter->startElement( "text:p" );
                bodyWriter->addTextNode( valStr );
                bodyWriter->endElement(); // text:p
            }
	    bodyWriter->endElement(); // table:table-cell
        }
        bodyWriter->endElement(); // table:table-row
    }

    bodyWriter->endElement(); // table:table-rows
    bodyWriter->endElement(); // table:table
}

void KChartPart::writeAutomaticStyles( KoXmlWriter& contentWriter, KoGenStyles& mainStyles ) const
{
    Q3ValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_AUTO );
    Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name, "style:chart-properties" );
    }

}

// ----------------------------------------------------------------
//             Save and Load old KChart file format


QDomDocument KChartPart::saveXML()
{
    QDomElement  tmpElem;

    //kDebug(35001) << "kchart saveXML called" << endl;

    // The biggest part of the saving is done by KDChart itself, so we
    // don't have to do it.
    QDomDocument doc = m_params->saveXML( false );

    // ----------------------------------------------------------------
    // The rest of the saving has to be done by us.

    QDomElement docRoot = doc.documentElement();

    // Save auxiliary data.
    QDomElement aux = doc.createElement( "KChartAuxiliary" );
    docRoot.appendChild( aux );

    // The data direction (rows/columns).
    tmpElem = doc.createElement( "direction" );
    tmpElem.setAttribute( "value", (int) m_params->dataDirection() );
    aux.appendChild( tmpElem );

    tmpElem = doc.createElement( "dataaslabel" );
    tmpElem.setAttribute( "firstrow",
			  m_params->firstRowAsLabel() ? "true" : "false" );
    tmpElem.setAttribute( "firstcol",
			  m_params->firstColAsLabel() ? "true" : "false" );
    aux.appendChild( tmpElem );

    tmpElem = doc.createElement( "barnumlines" );
    tmpElem.setAttribute( "value", (int) m_params->barNumLines() );
    aux.appendChild( tmpElem );

    // Save the data values.
    QDomElement data = doc.createElement( "data" );
    docRoot.appendChild( data );

    int cols = m_currentData.usedCols()
             ? qMin(m_currentData.usedCols(), m_currentData.cols())
             : m_currentData.cols();
    int rows = m_currentData.usedRows()
             ? qMin(m_currentData.usedRows(), m_currentData.rows())
             : m_currentData.rows();
    data.setAttribute( "cols", cols );
    data.setAttribute( "rows", rows );
    kDebug(35001) << "      writing  " << cols << "," << rows << "  (cols,rows)." << endl;

    for (int i=0; i!=rows; ++i) {
        for (int j=0; j!=cols; ++j) {
            QDomElement e = doc.createElement( "cell" );
            data.appendChild( e );
            QString valType;
            QVariant value( m_currentData.cellVal( i,j ) );
            switch ( value.type() ) {
                case QVariant::Invalid:  valType = "NoValue";   break;
                case QVariant::String:   valType = "String";    break;
                case QVariant::Double:   valType = "Double";    break;
                case QVariant::DateTime: valType = "DateTime";  break;
                default: {
                    valType = "(unknown)";
                    kDebug(35001) << "ERROR: cell " << i << "," << j
				   << " has unknown type." << endl;
                }
            }

            e.setAttribute( "valType", valType );
            //kDebug(35001) << "      cell " << i << "," << j
	    //	   << " saved with type '" << valType << "'." << endl;
            switch ( value.type() ) {
                case QVariant::String:  e.setAttribute( "value", value.toString() );
                              break;
                case QVariant::Double:  e.setAttribute( "value", QString::number( value.toDouble() ) );
                              break;
                case QVariant::DateTime:e.setAttribute( "value", "" );
                              break;
                default: {
                    e.setAttribute( "value", "" );
                    if( QVariant::Invalid != value.type() )
                        kDebug(35001) << "ERROR: cell " << i << "," << j
				       << " has unknown type." << endl;
                }
            }
        }
    }

    return doc;
}


bool KChartPart::loadXML( QIODevice*, const QDomDocument& doc )
{
    kDebug(35001) << "kchart loadXML called" << endl;

    // First try to load the KDChart parameters.
    bool  result = m_params->loadXML( doc );

    // If went well, try to load the auxiliary data and the data...
    if (result) {
        result = loadAuxiliary(doc) && loadData( doc, m_currentData );
    }
    else {
	// ...but if it did, try to load the old XML format.
        result = loadOldXML( doc );
    }

    // If everything is OK, then get the headers from the KDChart parameters.
    if (result) {
        QStringList        legendLabels;
        KDChartAxisParams  params;
        params = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

	// Get the legend.
	QString  str;
	uint     index = 0;
	while ((str = m_params->legendText(index++)) != QString::null)
	    legendLabels << str;

	if (m_params->dataDirection() == KChartParams::DataRows) {
	    m_colLabels = params.axisLabelStringList();
	    m_rowLabels = legendLabels;
	}
	else {
	    m_colLabels = legendLabels;
	    m_rowLabels = params.axisLabelStringList();
	}

	setChartDefaults();
    }

    m_params->setDrawSolidExcessArrows(true);

    return result;
}


// Load the auxiliary data.
//
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
		    dir = (int) KChartParams::DataColumns;

		//kDebug(35001) << "Got aux value \"direction\": " << dir << endl;
		m_params->setDataDirection( (KChartParams::DataDirection) dir );
	    }
	    else {
		kDebug(35001) << "Error in direction tag." << endl;
	    }
	}

	// Check for first row / col as label
	else if ( e.tagName() == "dataaslabel" ) {
	    QString  val;

	    if ( e.hasAttribute("firstrow") ) {
		// Read the direction. On failure, use the default.
		val = e.attribute("firstrow");
		if ( val == "true" )
		    m_params->setFirstRowAsLabel( true );
		else
		    m_params->setFirstRowAsLabel( false );
	    }
	    else {
		kDebug(35001) << "Error in barnumlines tag." << endl;
		m_params->setFirstRowAsLabel( false );
	    }

	    if ( e.hasAttribute("firstcol") ) {
		// Read the direction. On failure, use the default.
		val = e.attribute("firstcol");
		if ( val == "true" )
		    m_params->setFirstColAsLabel( true );
		else
		    m_params->setFirstColAsLabel( false );
	    }
	    else {
		kDebug(35001) << "Error in barnumlines tag." << endl;
		m_params->setFirstColAsLabel( false );
	    }
	}

	// Check for number of lines in a bar chart.
	else if ( e.tagName() == "barnumlines" ) {
	    if ( e.hasAttribute("value") ) {
		bool  ok;

		// Read the number of lines. On failure, use the default.
		int   barNumLines = e.attribute("value").toInt(&ok);
		if ( !ok )
		    barNumLines = 0;

		//kDebug(35001) << "Got aux value \"barnumlines\": "
		//	       << barNumLines << endl;
		m_params->setBarNumLines( barNumLines );
	    }
	    else {
		kDebug(35001) << "Error in barnumlines tag." << endl;
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
			   KDChartTableData& m_currentData )
{
    kDebug(35001) << "kchart loadData called" << endl;

    QDomElement chart = doc.documentElement();
    QDomElement data = chart.namedItem("data").toElement();
    bool ok;
    int cols = data.attribute("cols").toInt(&ok);
    kDebug(35001) << "cols readed as:" << cols << endl;
    if ( !ok ){
         return false;
    }

    int rows = data.attribute("rows").toInt(&ok);
    if ( !ok ){
         return false;
    }

    kDebug(35001) << rows << " x " << cols << endl;
    m_currentData.expand(rows, cols);
    m_currentData.setUsedCols( cols );
    m_currentData.setUsedRows( rows );
    kDebug(35001) << "Expanded!" << endl;
    QDomNode n = data.firstChild();
    //QArray<int> tmpExp(rows*cols);
    //QArray<bool> tmpMissing(rows*cols);
    for (int i=0; i!=rows; i++) {
        for (int j=0; j!=cols; j++) {
            if (n.isNull()) {
                kDebug(35001) << "Some problems, there is less data than it should be!" << endl;
                break;
            }
            QDomElement e = n.toElement();
            if ( !e.isNull() && e.tagName() == "cell" ) {
                // add the cell to the corresponding place...
                QVariant t;
                if ( e.hasAttribute("value") && e.hasAttribute("valType") ) {
                    QString valueType = e.attribute("valType").lower();
                    if ( "string" == valueType ) {
                        t = e.attribute("value");
                    }
                    else if ( "double" == valueType ) {
                        bool bOk;
                        double val = e.attribute("value").toDouble(&bOk);
                        if ( !bOk )
                            val = 0.0;
                        t = val;
                    /*
                    } else if ( "datetime" == valueType ) {
                        t = . . .
                    */
                    } else {
                        t.clear();
                        if ( "novalue" != valueType )
                            kDebug(35001) << "ERROR: cell " << i << "," << j << " has unknown type '" << valueType << "'." << endl;
                    }
                } else
                    t.clear();

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


// ----------------------------------------------------------------
//         Save and Load real old KChart file format


bool KChartPart::loadOldXML( const QDomDocument& doc )
{
    kDebug(35001) << "kchart loadOldXML called" << endl;
    if ( doc.doctype().name() != "chart" )
        return false;

    kDebug(35001) << "Ok, it is a chart" << endl;

    QDomElement chart = doc.documentElement();
    if ( chart.attribute( "mime" ) != "application/x-kchart" && chart.attribute( "mime" ) != "application/vnd.kde.kchart" )
        return false;

    kDebug(35001) << "Mimetype ok" << endl;

#if 0
    QDomElement data = chart.namedItem("data").toElement();
    bool ok;
    int cols = data.attribute("cols").toInt(&ok);
    kDebug(35001) << "cols readed as:" << cols << endl;
    if (!ok)  { return false; }
    int rows = data.attribute("rows").toInt(&ok);
    if (!ok)  { return false; }
    kDebug(35001) << rows << " x " << cols << endl;
    m_currentData.expand(rows, cols);
    kDebug(35001) << "Expanded!" << endl;
    QDomNode n = data.firstChild();
    QArray<int> tmpExp(rows*cols);
    QArray<bool> tmpMissing(rows*cols);

    for (int i=0; i!=rows; i++) {
        for (int j=0; j!=cols; j++) {
            if (n.isNull()) {
                kDebug(35001) << "Some problems, there is less data than it should be!" << endl;
                break;
            }

            QDomElement e = n.toElement();
            if ( !e.isNull() && e.tagName() == "cell" ) {
                // add the cell to the corresponding place...
                double val = e.attribute("value").toDouble(&ok);
                if (!ok)  {  return false; }
                kDebug(35001) << i << " " << j << "=" << val << endl;
                KoChart::Value t( val );
                // kDebug(35001) << "Set cell for " << row << "," << col << endl;
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
            m_params->setChartType(KChartParams::Line);
            break;
        case 2:
            m_params->setChartType(KChartParams::Area);
            break;
        case 3:
            m_params->setChartType(KChartParams::Bar);
            break;
        case 4:
            m_params->setChartType(KChartParams::HiLo);
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
            m_params->setChartType(KChartParams::Line);
            break;
        case 9:
            m_params->setChartType(KChartParams::HiLo);
            break;
        case 10:
            m_params->setChartType(KChartParams::Bar);
            break;
        case 11:
            m_params->setChartType(KChartParams::Area);
            break;
        case 12:
            m_params->setChartType(KChartParams::Bar);
            break;
        case 13:
            m_params->setChartType(KChartParams::Area);
            break;
        case 14:
            m_params->setChartType(KChartParams::Bar);
            break;
        case 15:
            m_params->setChartType(KChartParams::Area);
            break;
        case 16:
            m_params->setChartType(KChartParams::Line);
            break;
        case 17:
        case 18:
            m_params->setChartType(KChartParams::Pie);
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
                kDebug(35001) << "Some problems, there is less data than it should be!" << endl;
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
                kDebug(35001) << "Some problems, there is less data than it should be!" << endl;
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
                kDebug(35001) << "Some problems, there is less data than it should be!" << endl;
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
        m_params->backgroundPixmap.load( KStandardDirs::locate( "wallpaper",
						 m_params->backgroundPixmapName ));
        m_params->backgroundPixmapIsDirty = true;
    }
#endif
    return true;
}



void  KChartPart::slotModified()
{
    kDebug(35001) << "slotModified called!" << endl;

    setModified(true);
}


bool KChartPart::showEmbedInitDialog(QWidget* /*parent*/)
{
  // Don't show an embed dialog
  return true;
}


}  //KChart namespace

#include "kchart_part.moc"
