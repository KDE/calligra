/* GDCHART 0.94b  1st CHART SAMPLE  12 Nov 1998 */

/* writes gif file to stdout */

/* sample gdchart usage */
/* this will produce a 3D BAR chart */

/* Ported to Qt by Kalle Dalheimer <kalle@dalheimer.de> */


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


/* ----- set some data ----- */
float   a[6]  = { 0.5, 0.09, 0.6, 0.85, 0.0, 0.90 },
    b[6]  = { 1.9, 1.3,  0.6, 0.75, 0.1, -2.0 };
/* ----- X labels ----- */
char    *t[6] = { "Chicago", "New York", "L.A.", "Atlanta", "Paris, MD\n(USA) ", "London" };
/* ----- data set colors (RGB) ----- */
QColor   sc[2]    = { QColor( 255, 128, 128 ), QColor( 128, 128, 255 ) };

 
int main( int argc, char* argv[] )
{
    GDC_BGColor   = &Qt::white;                  /* backgound color (white) */
    GDC_LineColor = &Qt::black;                  /* line color      (black) */
    GDC_SetColor  = &(sc[0]);                   /* assign set colors */
    
    GDC_stack_type = GDC_STACK_BESIDE;
    
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
     out_graph( width(), height(),      /* short       width, height */
                &p,        // Paint into this painter
                GDC_3DBAR,     /* GDC_CHART_T chart type */
                6,             /* int         number of points per data set */
                t,             /* char*[]     array of X labels */
                2,             /* int         number of data sets */
                a,             /* float[]     data set 1 */
                b );           /*  ...        data set n */
     bitBlt( this, e->rect().topLeft(), &buffer, e->rect() );
}
