/* GDCHART 0.94b  PIE CHART SAMPLE  12 Nov 1998 */

/* creates a file "pie.gif".  Can be stdout for CGI use. */
/*  vi: :set tabstop=4 */

#include <stdio.h>
#include <values.h>
#include <math.h>

#include "gdc.h"
#include "gdcpie.h"
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

/* labels */
char		*lbl[] = { "CPQ\n(DEC)",
					   "HP",
					   "SCO",
					   "IBM",
					   "SGI",
					   "SUN\nSPARC",
					   "other" }; 

/* values to chart */
float ps[] = { 12.5,
			  20.1,
			  2.0,
			  22.0,
			  5.0,
			  18.0,
			  13.0 };

/* set which slices to explode, and by how much */
int	explode[] = { 0, 0, 0, 0, 0, 20, 0 };

/* set missing slices */
unsigned char	missing[] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE };

main( int argc, char *argv[] )
{
	/* set options  */
	/* a lot of options are set here for illustration */
	/* none need be - see gdcpie.h for defaults */
	GDCPIE_title = "Sample\nPIE";
	GDCPIE_label_line = TRUE;
	GDCPIE_label_dist = 15;				/* dist. labels to slice edge */
										/* can be negative */
	GDCPIE_LineColor = &Qt::black;
	GDCPIE_label_size = GDC_SMALL;
/*	GDCPIE_3d_depth  = 25;	*/
/*	GDCPIE_3d_angle  = 45;				   0 - 359 */
	GDCPIE_explode   = explode;			/* default: NULL - no explosion */
	GDCPIE_Color = new QColor[7];
	GDCPIE_Color[0] = QColor( 255, 64, 64);
	GDCPIE_Color[1] = QColor( 128, 255, 128 );
	GDCPIE_Color[2] = QColor( 128, 128, 255 );
	GDCPIE_Color[3] = QColor( 255, 128, 255 );
	GDCPIE_Color[4] = QColor( 255, 255, 128 );
	GDCPIE_Color[5] = QColor( 128, 255, 255 );
	GDCPIE_Color[6] = QColor( 0, 128, 255);
	GDCPIE_BGColor   = &Qt::white;
	GDCPIE_EdgeColor = &Qt::black;		/* default is GDCPIE_NOCOLOR */
										/* for no edging */
	GDCPIE_missing   = missing;			/* default: NULL - none missing */

										/* add percentage to slice label */
										/* below the slice label */
	GDCPIE_percent_labels = GDCPIE_PCT_RIGHT;

    QApplication gdcsamp1( argc, argv );
    GraphLabel* label = new GraphLabel();
    label->resize( 250, 200 );
    label->setBackgroundMode( QWidget::NoBackground );
    gdcsamp1.setMainWidget( label );
    label->show();

    init_charting_engine();

    return gdcsamp1.exec();
}


void GraphLabel::paintEvent( QPaintEvent* e )
{
    QPixmap buffer( size() );
    buffer.fill( white );
    QPainter p( &buffer );
    /* ----- call the lib ----- */
	pie_gif( width(),			/* width */
			 height(),			/* height */
			 &p,			/* paint here */
			 GDC_2DPIE,		/* or GDC_2DPIE */
			 7,				/* number of slices */
			 lbl,			/* slice labels (unlike out_gif(), can be NULL */
			 ps );			/* data array */

	bitBlt( this, e->rect().topLeft(), &buffer, e->rect() );
}
