/* GDCHART 0.94b  2nd CHART SAMPLE  12 Nov 1998 */

/*
** vi note  :set tabstop=4 **

 a more complicated example
 High Low Close Combo (Volume)  with annotation

 produces a file: g2.gif

 Until a README is ready, see gdchart.h for options
	All options are defaulted, no need to set any
*/

#include <stdio.h>
#include <values.h>

#include "gdc.h"
#include "gdchart.h"
#include "init.h"

#include <qlabel.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qpixmap.h>

class GraphLabel : public QLabel
{
public:
    GraphLabel( QWidget* parent = 0, const char* name = 0 ) : QLabel( parent, name ) {};

protected:
    void paintEvent( QPaintEvent* );
};

/* set some sample data points */
float	h[12]  = {	17.8,  17.1,  17.3,  GDC_NOVALUE,  17.2,  17.1,
					17.3,  17.3,  17.3,  17.1,         17.5,  17.4 };

float	c[12]  =  { 17.0,  16.8,  16.9,  GDC_NOVALUE,  16.9,  16.8,
					17.2,  16.8,  17.0,  16.9,         16.4,  16.1 };

float	l[12]  = {  16.8,  16.8,  16.7,  GDC_NOVALUE,  16.5,  16.0,
					16.1,  16.8,  16.5,  16.9,         16.2,  16.0 };

float	v[12]  = {  150.0, 100.0, 340.0,  GDC_NOVALUE, 999.0, 390.0,
					420.0, 150.0, 100.0,  340.0,       1590.0, 700.0 };
char	*t[12] = {	"May", "Jun", "Jul",  "Aug",       "Sep",  "Oct",
					"Nov", "Dec", "Jan", "Feb",        "Mar",  "Apr" };


main( int argc, char* argv[] )
{

//	/* set color RGB as ulong array */
//	unsigned long	setcolor[3]    = { 0xC0C0FF, 0xFF4040, 0xFFFFFF };

	GDC_ANNOTATION_T	anno;

	anno.color = 0x00FF00;
	strncpy( anno.note, "Did Not\nTrade", MAX_NOTE_LEN );	/* don't exceed MAX_NOTE_LEN */
	anno.point = 3;											/* first is 0 */
	GDC_annotation_font = GDC_TINY;
	GDC_annotation = &anno;									/* set annote option */

	GDC_HLC_style = GDC_HLC_I_CAP | GDC_HLC_CLOSE_CONNECTED;
	GDC_HLC_cap_width = 45;

	GDC_bar_width     = 75;									/* % */

//	GDC_BGImage = "W.gif";

	GDC_title = "Widget Corp.";
	GDC_ytitle = "Price ($)";
	GDC_ytitle2 = "Volume (K)";
	GDC_ytitle_size = GDC_SMALL;
	GDC_VolColor = 0x4040FFL;								/* aka combo */
	GDC_3d_depth  = 4.0;									/* % entire gif */

//	GDC_SetColor  = setcolor;								/* see README */
	GDC_PlotColor = 0xFFFFFF;
	GDC_grid = FALSE;

    QApplication gdcsamp2( argc, argv );
    GraphLabel* label = new GraphLabel();
    label->resize( 250, 200 );
    label->setBackgroundMode( QWidget::NoBackground );
    gdcsamp2.setMainWidget( label );
    label->show();

    init_charting_engine();

    return gdcsamp2.exec();
}


void GraphLabel::paintEvent( QPaintEvent* e )
{
    QPixmap buffer( size() );
    buffer.fill( white );
    QPainter p( &buffer );
    /* ----- call the lib ----- */
	out_graph( width(), height(),									// overall width, height
			   &p,										// open FILE pointer
			   GDC_COMBO_HLC_AREA,							// chart type
			   12,											// number of points
			   t,											// X axis label array
			   1,											// number of sets (see README)
			   h,											// set 1 (high)
			   l,											// low
			   c,											// close
			   v );											// combo/volume

     bitBlt( this, e->rect().topLeft(), &buffer, e->rect() );
}
