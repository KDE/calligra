#include <stdlib.h>
//#include <stdarg.h>
//#include <math.h>
//#include <stdio.h> //PENDING(kalle) Remove?
#include <qcolor.h>
#include <qpainter.h>
#include "engine.h"
#include "engine2.h"
#include "enginehelper.h"
#include "enginedraw.h"
#include <kdebug.h>


/* ------------------------------------------------------------------------- */
/* vals in pixels */
/* ref is front plane */
void draw_3d_bar( QPainter* p,
                         int x1,
                         int x2,
                         int y0,
                         int yhigh,
                         int xdepth,
                         int ydepth,
                         QColor clr,
                         QColor clrshd )
{
//kdDebug(35001) << "draw 3d bar with color 0 = " << clr.red() << ", "
//               << clr.green() << ", " << clr.blue() << "\n"
//               << shadow color 0 = "  << clrshd.red() << ", "
//               << clrshd.green() << ", " << clrshd.blue() << endl;

    QPointArray poly( 4 );
    int usd = QMIN( y0, yhigh ); // up-side-down bars

    if( xdepth || ydepth ) {
        if( y0 != yhigh ) { // 0 height?
            set3dbar( poly, x2, x2, y0, yhigh, xdepth, ydepth ); // side
            p->setBrush( QBrush( clrshd ) );
                        p->setPen( clrshd );
                        p->drawPolygon( poly );
        }
        set3dbar( poly, x1, x2, usd, usd, xdepth, ydepth );             // top
        p->setBrush( QBrush( clr ) );
        p->setPen( clr );
        p->drawPolygon( poly );
    }

    setrect( poly, x1, x2, y0, yhigh ); // front
    p->setBrush( QBrush( clr ) );
    p->setPen( clr );
    p->drawPolygon( poly );

    if( xdepth || ydepth )
        {
        p->setPen( clrshd );
        p->drawLine( x1, usd, x2, usd );
        }
}

void draw_3d_line( QPainter*    p,
                   int /*y0*/,
                   int x1,
                   int x2,
                   int y1[],
                   int y2[],
                   int xdepth,
                   int ydepth,
                   int num_sets,
                   QColor clr[],
                   QColor clrshd[] )
{
// kdDebug(35001) << "draw 3d line with color 0 = " << ... please port this :)
// %d, %d, %d,\nshadow color 0 = %d, %d, %d,\ncolor 1 = %d, %d, %d,\nshadow color 1 = %d, %d, %d", clr[0].red(), clr[0].green(), clr[0].blue(), clrshd[0].red(), clrshd[0].green(), clrshd[0].blue(), clr[1].red(), clr[1].green(), clr[1].blue(), clrshd[1].red(), clrshd[1].green(), clrshd[1].blue() );

#define F(x,i)  (int)( (float)((x)-x1)*slope[i]+(float)y1[i] )
    float depth_slope  = xdepth==0? MAXFLOAT: (float)ydepth/(float)xdepth;
    float *slope = new float[num_sets];
    int *lnclr = new int[num_sets];
    int *shclr = new int[num_sets];
    int i;
    int x;
    QPointArray poly( 4 );
    struct YS   *ypts = new struct YS[num_sets];

    for( i=0; i<num_sets; ++i ) {
        // lnclr[i] = clr[i];
        // shclr[i] = clrshd[i];
        slope[i] = x2==x1? MAXFLOAT: (float)(y2[i]-y1[i])/(float)(x2-x1);
    }

    for( x=x1+1; x<=x2; ++x ) {
        for( i=0; i<num_sets; ++i ) { // load set of points
            ypts[i].y1    = F(x-1,i);
            ypts[i].y2    = F(x,i);
            ypts[i].lnclr = clr[i];
            ypts[i].shclr = clrshd[i];
            ypts[i].slope = slope[i];
        } // sorted "lowest" first
        qsort( ypts, num_sets, sizeof(struct YS), qcmpr );
        // put out in that order
        for( i=0; i<num_sets; ++i ) { // top
            set3dpoly( poly, x-1, x, ypts[i].y1, ypts[i].y2, xdepth, ydepth );
            p->setBrush( QBrush( -ypts[i].slope > depth_slope ?
                                 ypts[i].shclr :
                                 ypts[i].lnclr ) );
            p->setPen( -ypts[i].slope > depth_slope ?
                       ypts[i].shclr :
                       ypts[i].lnclr );
            p->drawPolygon( poly );
            if( x == x1+1 ) { // edging
                p->setPen( -ypts[i].slope <= depth_slope ?
                           ypts[i].shclr :
                           ypts[i].lnclr );
                p->drawLine( x-1, ypts[i].y2,
                             x-1+xdepth, ypts[i].y2-ydepth );
            }
        }
    }
    delete [] slope;
    delete [] lnclr;
    delete [] shclr;
    delete [] ypts;
}

/* ------------------------------------------------------------------------- */
/* vals in pixels */
/* ref is front plane */
void draw_3d_area( QPainter* p,
                          int x1,
                          int x2,
                          int y0,                       // drawn from 0
                          int y1,
                          int y2,
                          int xdepth,
                          int ydepth,
                          QColor clr,
                          QColor clrshd )
{
    kdDebug(35001) << "draw 3d area with color 0 = " << clr.red() << ", "
                   << clr.green() << ", " << clr.blue() << "\n"
                   << "shadow color 0 = " << clrshd.red() << ", "
                   << clrshd.green() << ", " << clrshd.blue() << endl;

    QPointArray poly( 4 );
    int y_intercept = 0;

    if( xdepth || ydepth )
        {
        float line_slope = x2==x1?    MAXFLOAT: (float)-(y2-y1) / (float)(x2-x1);
        float depth_slope  = xdepth==0? MAXFLOAT: (float)ydepth/(float)xdepth;
        y_intercept = (y1 > y0 && y2 < y0) ||                                           // line crosses y0
                (y1 < y0 && y2 > y0)?
                        (int)((1.0/ABS(line_slope))*(float)(ABS(y1-y0)))+x1:0;                                                                          // never

        // edging along y0 depth
        p->setPen( clrshd );
        p->drawLine( x1+xdepth, y0-ydepth, x2+xdepth, y0-ydepth );

        set3dpoly( poly, x1, x2, y1, y2, xdepth, ydepth );// top
        p->setBrush( QBrush(  line_slope > depth_slope ? clrshd : clr ) );
        p->setPen( line_slope > depth_slope ? clrshd : clr );
        p->drawPolygon( poly );
        set3dpoly( poly, x1, x2, y0, y0, xdepth, ydepth+1 );    // along y axis
        p->setBrush( QBrush( clr ) );
        p->setPen( clr );
        p->drawPolygon( poly );
        set3dpoly( poly, x2, x2, y0, y2, xdepth, ydepth );// side
        p->setBrush( QBrush( clrshd ) );
        p->setPen( clrshd );
        p->drawPolygon( poly );

        if( y_intercept )
                {
                p->setPen( clrshd );
                p->drawLine( y_intercept,y0,
                        y_intercept+xdepth, y0-ydepth );        // edging
                }
        p->drawLine( x1, y0, x1+xdepth, y0-ydepth );    // edging
        p->drawLine( x2, y0, x2+xdepth, y0-ydepth );    // edging

        // set3dpoly( poly, x2, x2, y0, y2, xdepth, ydepth );   // side
        // gdImageFilledPolygon( im, poly, 4, clrshd );

        p->drawLine( x1, y1, x1+xdepth, y1-ydepth );// edging
        p->drawLine( x2, y2, x2+xdepth, y2-ydepth );// edging
    }

    if( y1 == y2 )                                                                                              // bar rect
        setrect( poly, x1, x2, y0, y1 );                                                        // front
    else
        {
        poly.setPoint( 0, x1, y0 );
        poly.setPoint( 1, x2, y0 );
        poly.setPoint( 2, x2, y2 );
        poly.setPoint( 3, x1, y1 );
        }
    p->setBrush( QBrush( clr ) );
    p->setPen( clr );
    p->drawPolygon( poly );

    p->setPen( clrshd );
    p->drawLine( x1, y0, x2, y0 );// edging along y0

    if( (xdepth || ydepth) &&   // front edging only on 3D
                (y1<y0 || y2<y0) )                                                                      // and only above y0
                {
                if( y1 > y0 && y2 < y0 )
                        {// line crosses from below y0
                        p->setPen( clrshd );
                        p->drawLine( y_intercept, y0, x2, y2 );
                        }
                else
                        if( y1 < y0 && y2 > y0 )
                                { // line crosses from above y0
                                p->setPen( clrshd );
                                p->drawLine( x1, y1, y_intercept, y0 );
                                }
                        else
                                { // completely above
                                p->setPen( clrshd );
                                p->drawLine( x1, y1, x2, y2 );
                                }
                }
}


