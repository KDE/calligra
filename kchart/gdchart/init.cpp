/**
 * $Id:$
 *
 * Init the charting engine
 */

#include <gdc.h>
#include <gdchart.h>
#include <gdcpie.h>
#include <qcolor.h>
#include <qfontmetrics.h>

void init_charting_engine()
{
	GDC_BGColor = new QColor( 0, 0, 0 );	 /* black */
	GDC_GridColor = new QColor( 160, 160, 160 );	 /* gray */
	GDC_LineColor = new QColor( *GDC_DFLTCOLOR );
	GDC_PlotColor = new QColor( *GDC_DFLTCOLOR );
	GDC_VolColor = new QColor( QColor( 160, 160, 255 ) );	 /* lgtblue1 */
	GDC_TitleColor = new QColor( *GDC_DFLTCOLOR ); /* "opposite" of BG */
	GDC_XTitleColor = new QColor( *GDC_DFLTCOLOR );
	GDC_YTitleColor = new QColor( *GDC_DFLTCOLOR );
	GDC_YTitle2Color = new QColor( *GDC_DFLTCOLOR );
	GDC_XLabelColor = new QColor( *GDC_DFLTCOLOR );
	GDC_YLabelColor = new QColor( *GDC_DFLTCOLOR );
	GDC_YLabel2Color = new QColor( *GDC_DFLTCOLOR );
	/* supercedes VolColor	ulong_color[num_points] */
	GDC_ExtVolColor = 0;
	/* supercedes LineColor	ulong_color[num_sets] */
	GDC_SetColor = 0;
	/* supercedes SetColor	ulong_color[num_sets][num_points] */
	GDC_ExtColor = new QColor[12];
 	GDC_ExtColor[0] = Qt::red;
 	GDC_ExtColor[1] = Qt::green;
 	GDC_ExtColor[2] = Qt::blue;
 	GDC_ExtColor[3] = Qt::cyan;
 	GDC_ExtColor[4] = Qt::magenta;
 	GDC_ExtColor[5] = Qt::yellow;
 	GDC_ExtColor[6] = Qt::darkRed;
 	GDC_ExtColor[7] = Qt::darkGreen;
 	GDC_ExtColor[8] = Qt::darkBlue;
 	GDC_ExtColor[9] = Qt::darkCyan;
 	GDC_ExtColor[10] = Qt::darkMagenta;
 	GDC_ExtColor[11] = Qt::darkYellow;
	GDCPIE_BGColor = new QColor( 0, 0, 0 );	/* black */
	GDCPIE_PlotColor = new QColor( 192, 192, 192 );	/* gray */
	GDCPIE_LineColor = new QColor( *GDC_DFLTCOLOR );
	GDCPIE_EdgeColor = new QColor(); /* edging on/off */
	/* [num_points] supercedes GDCPIE_PlotColor */
	GDCPIE_Color = new QColor[12];
 	GDCPIE_Color[0] = Qt::red;
 	GDCPIE_Color[1] = Qt::green;
 	GDCPIE_Color[2] = Qt::blue;
 	GDCPIE_Color[3] = Qt::cyan;
 	GDCPIE_Color[4] = Qt::magenta;
 	GDCPIE_Color[5] = Qt::yellow;
 	GDCPIE_Color[6] = Qt::darkRed;
 	GDCPIE_Color[7] = Qt::darkGreen;
 	GDCPIE_Color[8] = Qt::darkBlue;
 	GDCPIE_Color[9] = Qt::darkCyan;
 	GDCPIE_Color[10] = Qt::darkMagenta;
 	GDCPIE_Color[11] = Qt::darkYellow;


	// PENDING(kalle) Put this somewhere else and make it configurable
	QFont* theKChartTinyFont = new QFont( "Helvetica", 8 );
	QFont* theKChartSmallFont = new QFont( "Helvetica", 12 );
	QFont* theKChartMediumFont = new QFont( "Helvetica", 13 );
	QFont* theKChartLargeFont = new QFont( "Helvetica", 15 );
	QFont* theKChartGiantFont = new QFont( "Helvetica", 20 );

	GDC_fontc = new GDC_FONT_T[5];
	GDC_fontc[0].f = theKChartTinyFont;
	GDC_fontc[0].w = QFontMetrics( *theKChartTinyFont ).maxWidth();
	GDC_fontc[0].h = QFontMetrics( *theKChartTinyFont ).height();
	GDC_fontc[1].f = theKChartSmallFont;
	GDC_fontc[1].w = QFontMetrics( *theKChartSmallFont ).maxWidth();
	GDC_fontc[1].h = QFontMetrics( *theKChartSmallFont ).height();
	GDC_fontc[2].f = theKChartMediumFont;
	GDC_fontc[2].w = QFontMetrics( *theKChartMediumFont ).maxWidth();
	GDC_fontc[2].h = QFontMetrics( *theKChartMediumFont ).height();
	GDC_fontc[3].f = theKChartLargeFont;
	GDC_fontc[3].w = QFontMetrics( *theKChartLargeFont ).maxWidth();
	GDC_fontc[3].h = QFontMetrics( *theKChartLargeFont ).height();
	GDC_fontc[4].f = theKChartGiantFont;
	GDC_fontc[4].w = QFontMetrics( *theKChartGiantFont ).maxWidth();
	GDC_fontc[4].h = QFontMetrics( *theKChartGiantFont ).height();
}

void destroy_charting_engine()
{
	delete GDC_DFLTCOLOR;
	delete GDC_BGColor;
	delete GDC_GridColor;
	delete GDC_LineColor;
	delete GDC_PlotColor;
	delete GDC_VolColor;
	delete GDC_TitleColor;
	delete GDC_XTitleColor;
	delete GDC_YTitleColor;
	delete GDC_YTitle2Color;
	delete GDC_XLabelColor;
	delete GDC_YLabelColor;
	delete GDC_YLabel2Color;
	delete[] GDC_ExtVolColor;
	delete[] GDC_SetColor;
	delete[] GDC_ExtColor;
	delete GDCPIE_BGColor;
	delete GDCPIE_PlotColor;
	delete GDCPIE_LineColor;
	delete GDCPIE_EdgeColor;
	delete[] GDCPIE_Color;
	delete theKChartTinyFont;
	delete theKChartSmallFont;
	delete theKChartMediumFont;
	delete theKChartLargeFont;
	delete theKChartGiantFont;
	delete[] GDC_fontc;
}

