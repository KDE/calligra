/* GDCHART 0.94b  GDC.C  12 Nov 1998 */

/**
 * Version hacked by Kalle Dalheimer (kalle@kde.org) to use QPainter
 * instead of libgd.
 *
 * Put into KChart
 *
 * $Id$
 */

#include "engine.h"
#include "kchartparams.h"

#include <qfont.h>
#include <qcolor.h>
#include <qpainter.h>

#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h> //PENDING(kalle) Remove?

// retrieve the value of a cell
#define CELLVALUE( row, col ) data.cell( (row), (col) ).value.doubleValue()

// check whether a cell has a value
#define CELLEXISTS( row, col ) data.cell( (row), (col) ).exists


/* ------------------------------------------------------------------ *\
 * count (natural) substrings (new line sep)
\* ------------------------------------------------------------------ */
short
cnt_nl( const char *nstr,
		int		*len )			/* strlen - max seg */
{
	short	c           = 1;
	short	max_seg_len = 0;
	short	tmplen      = 0;

	if( !nstr )
		{
			if( len )
				*len = 0;
			return 0;
		}

	while( *nstr )
		{
			if( *nstr == '\n' )
				{
					++c;
					max_seg_len = MAX( tmplen, max_seg_len );
					tmplen = 0;
				}
			else
				++tmplen;
			++nstr;
		}

	if( len )
		*len = MAX( tmplen, max_seg_len );		/* don't forget last seg */
	return c;
}

#ifdef WE_MIGHT_NEED_THIS_LATER_BETTER_NOT_DELETE_IT_NOW
/* ------------------------------------------------------------------ *\
 * gd out a string with '\n's
\* ------------------------------------------------------------------ */
void
GDCImageStringNL( gdImagePtr		im,
				  QFont f,
				  int				x,
				  int				y,
				  char				*str,
				  int				clr,
				  KChartJustifyType	justify )
{
    int		i;
    int		len;
    int     max_len;
    short   strs_num = cnt_nl( str, &max_len );
    char    sub_str[max_len+1];

    len      = -1;
    strs_num = -1;
    i = -1;
    do {
		++i;
		++len;
		sub_str[len] = *(str+i);
		if( *(str+i) == '\n' ||
			*(str+i) == '\0' ) {
			int	xpos;
	
			sub_str[len] = '\0';
			++strs_num;
			switch( justify ) {
			case KCHARTJUSTIFYLEFT:	
				xpos = x;					
				break;
			case KCHARTJUSTIFYRIGHT:	
				xpos = x+f->w*(max_len-len);
				break;
			case KCHARTJUSTIFYCENTER:
			default:					
				xpos = x+f->w*(max_len-len)/2;
			}
			gdImageString( im,
						   f->f,
						   xpos,
						   y + (f->h-1)*strs_num,
						   sub_str,
						   clr );
			len = -1;
		}
    }
    while( *(str+i) );
}
#endif

/* ------------------------------------------------------------------------ */
void
out_err( QPainter*,
		 int,
		 int,
		 QColor,
		 QColor,
		 char *err_str )
{
	debug( "%s", err_str );
}

#define HIGHSET		0
#define LOWSET		1
#define CLOSESET	2

// scaled translation onto graph
#define PX( x )		(int)( xorig + (setno*xdepth_3D) + (x)*xscl )
#define PY( y )		(int)( yorig - (setno*ydepth_3D) + (y)*yscl )
#define PV( y )		(int)( vyorig - (setno*ydepth_3D) + (y)*vyscl )

inline static void setrect( QPointArray& gdp, int x1, int x2, int y1, int y2 )
{	
    gdp.setPoint( 0, x1, y1 );
    gdp.setPoint( 1, x2, y1 );
    gdp.setPoint( 2, x2, y2 );
    gdp.setPoint( 3, x1, y2 );
}
	
#ifdef THUMB_VALS
// -------------------------------------------------------------------
// draw an arrow at (x,y)-upper left in arrwclr to the size of SmallFont
// could, with just a little difficulty, be made to accept a font size
// -------------------------------------------------------------------
void
smallarrow( QPainter* p,
			int	x,
			int	y,
			char up,
			int	arrwclr )
{
    debug( "+++smallarrow" );
	QFont f( "Helvetica", 10 );
	QFontMetrics fm( f );
	int h = fm.height();
    p->drawLine( x+2, y, x+2, y+h );
    p->drawLine( x+3, y, x+3, y+h );
    if( up ) {	/*   oo   */
		/*  uoou  */
		p->setPen( arrwclr );
		p->drawPoint( x, y+2 );   /* uuoouu */
		p->drawPoint( x+1, y+2 ); /*   oo   */
		p->drawPoint( x+4, y+2 );	/*   oo   */
		p->drawPoint( x+5, y+2 );	/*   oo   */
		p->drawPoint( x+1, y+1 ); /*   oo   */
		p->drawPoint( x+4, y+1 );	/*   oo   */
		/*   oo   */
    } else { /* ddoodd */
		/*  dood  */
		p->setPen( arrwclr );
		p->drawPoint( x,   y+(h-2) );	/*   oo   */
		p->drawPoint( x+1, y+(h-2) );
		p->drawPoint( x+4, y+(h-2) );
		p->drawPoint( x+5, y+(h-2) );
		p->drawPoint( x+1, y+(h-1) );
		p->drawPoint( x+4, y+(h-1) );
    }
}
#endif


inline void set3dpoly( QPointArray& gdp, int x1, int x2, int y1, int y2,
				  int xoff, int yoff )
{
    gdp.setPoint( 0, x1, y1 );
    gdp.setPoint( 1, x1 + xoff, y1 - yoff );
    gdp.setPoint( 2, x2 + xoff, y2 - yoff );
    gdp.setPoint( 3, x2, y2 );
}

/* ------------------------------------------------------------------------- */
/* vals in pixels */
/* ref is front plane */
/* allows for intersecting 3D lines      */
/*  (also used for single 3D lines >:-Q  */
struct YS { int y1; int y2; float slope; QColor lnclr; QColor shclr; };
static int qcmpr( const void *a, const void *b )
{ if( ((struct YS*)a)->y2 < ((struct YS*)b)->y2 ) return 1;
 if( ((struct YS*)a)->y2 > ((struct YS*)b)->y2 ) return -1;
 return 0; }
void
draw_3d_line( QPainter*	p,
			  int y0,
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
	debug( "draw 3d line with color 0 = %d, %d, %d,\nshadow color 0 = %d, %d, %d,\ncolor 1 = %d, %d, %d,\nshadow color 1 = %d, %d, %d", clr[0].red(), clr[0].green(), clr[0].blue(), clrshd[0].red(), clrshd[0].green(), clrshd[0].blue(), clr[1].red(), clr[1].green(), clr[1].blue(), clrshd[1].red(), clrshd[1].green(), clrshd[1].blue() );

#define F(x,i)	(int)( (float)((x)-x1)*slope[i]+(float)y1[i] )
    float depth_slope  = xdepth==0? MAXFLOAT: (float)ydepth/(float)xdepth;
    float slope[num_sets];
    int lnclr[num_sets],
		shclr[num_sets];
    int	i;
    int	x;
    QPointArray	poly( 4 );
    struct YS	ypts[num_sets];

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
}

/* ------------------------------------------------------------------------- */
/* vals in pixels */
/* ref is front plane */
void
draw_3d_area( QPainter* p,
			  int x1,
			  int x2,
			  int y0,			// drawn from 0
			  int y1,
			  int y2,
			  int xdepth,
			  int ydepth,
			  QColor clr,
			  QColor clrshd )
{
	debug( "draw 3d area with color 0 = %d, %d, %d,\nshadow color 0 = %d, %d, %d", clr.red(), clr.green(), clr.blue(), clrshd.red(), clrshd.green(), clrshd.blue() );

    QPointArray poly( 4 );
    int	y_intercept = 0;

    if( xdepth || ydepth ) {
		float		line_slope   = x2==x1?    MAXFLOAT: (float)-(y2-y1) / (float)(x2-x1);
		float		depth_slope  = xdepth==0? MAXFLOAT: (float)ydepth/(float)xdepth;

		y_intercept = (y1 > y0 && y2 < y0) ||						// line crosses y0
			(y1 < y0 && y2 > y0)?
			(int)((1.0/ABS(line_slope))*(float)(ABS(y1-y0)))+x1:
			0;										// never
	
		// edging along y0 depth
		p->setPen( clrshd );
		p->drawLine( x1+xdepth, y0-ydepth, x2+xdepth, y0-ydepth );
	
		set3dpoly( poly, x1, x2, y1, y2, xdepth, ydepth );		// top
		p->setBrush( QBrush(  line_slope > depth_slope ? clrshd : clr ) );
		p->setPen( line_slope > depth_slope ? clrshd : clr );
		p->drawPolygon( poly );
		set3dpoly( poly, x1, x2, y0, y0, xdepth, ydepth+1 );	// along y axis
		p->setBrush( QBrush( clr ) );
		p->setPen( clr );
		p->drawPolygon( poly );
		set3dpoly( poly, x2, x2, y0, y2, xdepth, ydepth );		// side
		p->setBrush( QBrush( clrshd ) );
		p->setPen( clrshd );
		p->drawPolygon( poly );
	
		if( y_intercept ) {
			p->setPen( clrshd );
			p->drawLine( y_intercept,        y0,
						 y_intercept+xdepth, y0-ydepth );	// edging
		}
		p->drawLine( x1, y0, x1+xdepth, y0-ydepth );	// edging
		p->drawLine( x2, y0, x2+xdepth, y0-ydepth );	// edging

		// set3dpoly( poly, x2, x2, y0, y2, xdepth, ydepth );	// side
		// gdImageFilledPolygon( im, poly, 4, clrshd );

		p->drawLine( x1, y1, x1+xdepth, y1-ydepth );	// edging
		p->drawLine( x2, y2, x2+xdepth, y2-ydepth );	// edging
    }

    if( y1 == y2 )												// bar rect
		setrect( poly, x1, x2, y0, y1 );							// front
    else {
		poly.setPoint( 0, x1, y0 );
		poly.setPoint( 1, x2, y0 );
		poly.setPoint( 2, x2, y2 );
		poly.setPoint( 3, x1, y1 );
    }
    p->setBrush( QBrush( clr ) );
    p->setPen( clr );
    p->drawPolygon( poly );

    p->setPen( clrshd );
    p->drawLine( x1, y0, x2, y0 );			// edging along y0

    if( (xdepth || ydepth) &&								// front edging only on 3D
		(y1<y0 || y2<y0) )									// and only above y0
		{
			if( y1 > y0 && y2 < y0 ) {						// line crosses from below y0
				p->setPen( clrshd );
				p->drawLine( y_intercept, y0, x2, y2 );
			} else
				if( y1 < y0 && y2 > y0 ) { // line crosses from above y0
					p->setPen( clrshd );
					p->drawLine( x1, y1, y_intercept, y0 );
				} else { // completely above
					p->setPen( clrshd );
					p->drawLine( x1, y1, x2, y2 );
				}
		}
}

inline void set3dbar( QPointArray& gdp, int x1, int x2, int y1, int y2,
				 int xoff, int yoff )
{
    gdp.setPoint( 0, x1, y1 );
    gdp.setPoint( 1, x1 + xoff, y1 - yoff );
    gdp.setPoint( 2, x2 + xoff, y2 - yoff );
    gdp.setPoint( 3, x2, y2 );
}

/* ------------------------------------------------------------------------- */
/* vals in pixels */
/* ref is front plane */
void
draw_3d_bar( QPainter* p,
			 int x1,
			 int x2,
			 int y0,
			 int yhigh,
			 int xdepth,
			 int ydepth,
			 QColor clr,
			 QColor clrshd )
{
	//	debug( "draw 3d bar with color 0 = %d, %d, %d,\nshadow color 0 = %d, %d, %d", clr.red(), clr.green(), clr.blue(), clrshd.red(), clrshd.green(), clrshd.blue() );

    QPointArray poly( 4 );
    int	usd = MIN( y0, yhigh ); // up-side-down bars

    if( xdepth || ydepth ) {
		if( y0 != yhigh ) { // 0 height?
			set3dbar( poly, x2, x2, y0, yhigh, xdepth, ydepth );	// side
			p->setBrush( QBrush( clrshd ) );
			p->setPen( clrshd );
			p->drawPolygon( poly );
		}

		set3dbar( poly, x1, x2, usd, usd, xdepth, ydepth );		// top
		p->setBrush( QBrush( clr ) );
		p->setPen( clr );
		p->drawPolygon( poly );
    }

    setrect( poly, x1, x2, y0, yhigh ); // front
    p->setBrush( QBrush( clr ) );
    p->setPen( clr );
    p->drawPolygon( poly );

    if( xdepth || ydepth ) {
		p->setPen( clrshd );
		p->drawLine( x1, usd, x2, usd );
    }
}

/* ------------------------------------------------------------------------- */
struct BS { float y1; float y2; QColor clr; QColor shclr; };
static int barcmpr( const void *a, const void *b )
{
  if( ((struct BS*)a)->y2 < ((struct BS*)b)->y2 ) return -1;
  if( ((struct BS*)a)->y2 > ((struct BS*)b)->y2 ) return 1;
  return 0;
}

/* ------------------------------------------------------------------------- */
// little/no error checking  0:    ok,
//							 -ret: error no graph output
//							 ret:  error graph out
// watch out for # params and array sizes==num_points
typedef float* floatptr;

int
out_graph( short imagewidth,         // no check for an output device that's too small to fit
	   short imageheight,        // needed info (labels, etc), could core dump
	   QPainter* p,	             // paint into this painter
	   KChartParameters* params, // the parameters of the chart
	   const KChartData& data
	   )
{
	// For now, we are assuming that the data is in columns with no labels at all
	
	// Ergo, num_sets is the number of rows
        int num_sets = data.rows();

	// No data sets left -> bail out
	if( num_sets < 1 ) {
		debug( "No data" );
		return -1;
	}

	// And num_points is the number of columns
	int num_points = data.cols();

	int i, j, k;
	
	// Does it have xlabels at all?
	bool hasxlabels = false;

	// Finally, the x labels are taken from the first row
	QArray<QString> xlbl( data.cols() );
#ifdef NOXLABELSFORNOW
	debug( "Creating xlbl with %d entries", data.cols() );
	for( int labels = 0; labels < data.cols(); labels++ ) {
		debug( "Retrieving value at position %d", labels );
		const KChartValue& cellval = data.cell( 0, labels );
		debug( "type of field %d in row 0 is %s", labels, QVariant::typeToName( cellval.value.type() ).latin1() );
		if( !cellval.exists ) {
			debug( "No value for x label in col %d", labels );
			continue;
		}
		if( cellval.value.type() != QVariant::String ) {
			debug( "Value for x label in col %d is not a string", labels );
			continue;
		}
		
		debug( "Setting label %d to %s", labels, cellval.value.stringValue().latin1() );
		//		QString l = cellval.value.stringValue();
		xlbl.at( labels ) = cellval.value.stringValue();
		debug( "Done setting label" );
		hasxlabels = true;
	}
#endif

	debug( "labels read" );

    int			graphwidth;
    int			grapheight;
    //  gdImagePtr	im;
    // gdImagePtr	bg_img = NULL;

    float		xorig, yorig, vyorig;
    float		yscl     = 0.0;
    float		vyscl    = 0.0;
    float		xscl     = 0.0;
    float		vhighest = -MAXFLOAT;
    float		vlowest  = MAXFLOAT;
    float		highest  = -MAXFLOAT;
    float		lowest   = MAXFLOAT;
    QPointArray volpoly( 4 );

    char		do_vol = ( params->type == KCHARTTYPE_COMBO_HLC_BAR   ||		// aka: combo
						   params->type == KCHARTTYPE_COMBO_HLC_AREA  ||
						   params->type == KCHARTTYPE_COMBO_LINE_BAR  ||
						   params->type == KCHARTTYPE_COMBO_LINE_AREA ||
						   params->type == KCHARTTYPE_3DCOMBO_HLC_BAR ||
						   params->type == KCHARTTYPE_3DCOMBO_HLC_AREA||
						   params->type == KCHARTTYPE_3DCOMBO_LINE_BAR||
						   params->type == KCHARTTYPE_3DCOMBO_LINE_AREA );
    char		threeD = ( params->type == KCHARTTYPE_3DAREA          ||
						   params->type == KCHARTTYPE_3DLINE          ||
						   params->type == KCHARTTYPE_3DBAR           ||
						   params->type == KCHARTTYPE_3DHILOCLOSE     ||
						   params->type == KCHARTTYPE_3DCOMBO_HLC_BAR ||
						   params->type == KCHARTTYPE_3DCOMBO_HLC_AREA||
						   params->type == KCHARTTYPE_3DCOMBO_LINE_BAR||
						   params->type == KCHARTTYPE_3DCOMBO_LINE_AREA );
    char		num_hlc_sets =
		( params->type == KCHARTTYPE_COMBO_HLC_BAR   ||
		  params->type == KCHARTTYPE_COMBO_HLC_AREA  ||
		  params->type == KCHARTTYPE_3DCOMBO_HLC_BAR ||
		  params->type == KCHARTTYPE_3DCOMBO_HLC_AREA||
		  params->type == KCHARTTYPE_3DHILOCLOSE     ||
		  params->type == KCHARTTYPE_HILOCLOSE )? num_sets: 0;
    char		do_bar = ( params->type == KCHARTTYPE_3DBAR ||					// offset X objects to leave
						   params->type == KCHARTTYPE_BAR );					//  room at X(0) and X(n)
    //  i.e., not up against Y axes
    char		do_ylbl_fractions = 							// %f format not given, or
		( params->ylabel_fmt.isEmpty() ||					//  format doesn't have a %,g,e,E,f or F
		  params->ylabel_fmt.length() == strcspn(params->ylabel_fmt,"%geEfF") );
    float		ylbl_interval  = 0.0;
    int			num_lf_xlbls   = 0;
    int			xdepth_3Dtotal = 0;
    int			ydepth_3Dtotal = 0;
    int			xdepth_3D      = 0;		// affects PX()
    int			ydepth_3D      = 0;		// affects PY() and PV()
    int			hlf_barwdth	   = 0;		// half bar widths
    int			hlf_hlccapwdth = 0;		// half cap widths for HLC_I_CAP and DIAMOND
    int			annote_len     = 0,
		annote_hgt     = 0;

    /* args */
    int			setno = 0;				// affects PX() and PY()

	// PENDING(kalle) Get rid of this
    float		*uvol;

    QColor BGColor, LineColor, PlotColor, GridColor, VolColor,
		ExtVolColor[num_points], ThumbDColor, ThumbLblColor, ThumbUColor,
		//				ArrowDColor,	
		//				ArrowUColor,
		AnnoteColor, ExtColor[num_sets][num_points];
    // shade colors only with 3D
    //	int			ExtColorShd[threeD?1:num_sets][threeD?1:num_points]; // compiler limitation
    QColor ExtColorShd[num_sets][num_points];

    /* idiot checks */
    if( imagewidth <= 0 || imageheight <=0 || !p  )
		return -1;
    if( num_points <= 0 ) {
		debug( "No Data Available" );
		return 1;
    }

	debug( "done idiot checks" );

    if( params->thumbnail ) {
		params->grid = FALSE;
		params->xaxis = FALSE;
		params->yaxis = FALSE;
    }

	debug( "done thumbnails" );

    // ----- get args  va number of float arrays -----
	/* va_list ap;
     va_start( ap, num_sets );
     for( i=0; i<num_sets; ++i )
 		uvals[i] = va_arg(ap, float*);
     if( do_vol )
 		uvol = va_arg(ap, float*);
		va_end(ap); */

    /* ----- highest & lowest values ----- */
    if( params->stack_type == KCHARTSTACKTYPE_SUM )	// need to walk sideways
		for( j=0; j<num_points; ++j ) {
			float set_sum = 0.0;
			for( i=0; i<num_sets; ++i ) {
				debug( "vor dem crash" );
				if( CELLEXISTS( i, j ) ) {
					debug( "nach dem crash" );
					set_sum += CELLVALUE( i, j );
					highest = MAX( highest, set_sum );
					lowest  = MIN( lowest,  set_sum );
				}
			}
		} else if( params->stack_type == KCHARTSTACKTYPE_LAYER ) // need to walk sideways
			for( j=0; j<num_points; ++j ) {
				float neg_set_sum = 0.0, pos_set_sum = 0.0;
				for( i=0; i<num_sets; ++i )
					if( CELLEXISTS( i, j ) )
						if( CELLVALUE( i, j ) < 0.0 )
							neg_set_sum += CELLVALUE( i, j );
						else
							pos_set_sum += CELLVALUE( i, j );
				lowest  = MIN( lowest,  MIN(neg_set_sum,pos_set_sum) );
				highest = MAX( highest, MAX(neg_set_sum,pos_set_sum) );
			} else
				for( i=0; i<num_sets; ++i )
					for( j=0; j<num_points; ++j ) {
						debug( "Vor dem crash" );
						if( CELLEXISTS( i, j ) ) {
							debug( "nach dem crash" );
							highest = MAX( CELLVALUE( i, j ), highest );
							lowest  = MIN( CELLVALUE( i, j ), lowest );
						}
					}

	debug( "done computation highest and lowest value" );

    if( params->scatter )
		for( i=0; i<params->num_scatter_pts; ++i ) {
			highest = MAX( ((params->scatter)+i)->val, highest );
			lowest  = MIN( ((params->scatter)+i)->val, lowest  );
		}
    if( do_vol ) { // for now only one combo set allowed
		// vhighest = 1.0;
		// vlowest  = 0.0;
		for( j=0; j<num_points; ++j )
			if( uvol[j] != GDC_NOVALUE ) {
				vhighest = MAX( uvol[j], vhighest );
				vlowest  = MIN( uvol[j], vlowest );
			}
		if( vhighest == -MAXFLOAT )			// no values
			vhighest = 1.0;						// for scaling, need a range
		else if( vhighest < 0.0 )
			vhighest = 0.0;
		if( vlowest > 0.0 || vlowest == MAXFLOAT )
			vlowest = 0.0;						// vol should always start at 0
    }

	debug( "done vlowest computation" );

    if( lowest == MAXFLOAT )
		lowest = 0.0;
    if( highest == -MAXFLOAT )
		highest = 1.0;							// need a range
    if( params->type == KCHARTTYPE_AREA  ||					// bars and area should always start at 0
		params->type == KCHARTTYPE_BAR   ||
		params->type == KCHARTTYPE_3DBAR ||
		params->type == KCHARTTYPE_3DAREA )
		if( highest < 0.0 )
			highest = 0.0;
		else if( lowest > 0.0 )						// negs should be drawn from 0
			lowest = 0.0;

    if( params->requested_ymin != -MAXDOUBLE && params->requested_ymin < lowest )
		lowest = params->requested_ymin;
    if( params->requested_ymax != -MAXDOUBLE && params->requested_ymax > highest )
		highest = params->requested_ymax;

	qDebug( "done requested_* computation" );


    /* ----- graph height and width within the gif height width ----- */
    /* grapheight/height is the actual size of the scalable graph */
    {
		int	title_hgt  = !params->title.isEmpty()? 2				/* title? horizontal text line(s) */
			+ cnt_nl(params->title.latin1(),(int*)NULL)*params->titleFontHeight()
			+ 2:
			2;
		int	xlabel_hgt = 0;
		int	xtitle_hgt = !params->xtitle.isEmpty()? 1+params->xTitleFontHeight()+1: 0;
		int	ytitle_hgt = !params->ytitle.isEmpty()? 1+params->yTitleFontHeight()+1: 0;
		int	vtitle_hgt = do_vol&& !params->ytitle2.isEmpty()? 1+params->yTitleFontHeight()+1: 0;
		int	ylabel_wth = 0;
		int	vlabel_wth = 0;
	
		int	xtics       = params->grid||params->xaxis? 1+2: 0;
		int	ytics       = params->grid||params->yaxis? 1+3: 0;
		int	vtics       = params->yaxis&&do_vol? 3+1: 0;

		qDebug( "done width, height, tics computation" );
	
	
#define	HYP_DEPTH	( (double)((imagewidth+imageheight)/2) * ((double)params->_3d_depth)/100.0 )
#define RAD_DEPTH	( (double)params->_3d_angle*2*M_PI/360 )
		xdepth_3D      = threeD? (int)( cos(RAD_DEPTH) * HYP_DEPTH ): 0;
		ydepth_3D      = threeD? (int)( sin(RAD_DEPTH) * HYP_DEPTH ): 0;
		xdepth_3Dtotal = xdepth_3D*(params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
									num_sets:
									1 );
		ydepth_3Dtotal = ydepth_3D*(params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
									num_sets:
									1 );
		annote_hgt = params->annotation && *(params->annotation->note)?
			1 +											/* space to note */
			(1+params->annotationFontHeight()) *		/* number of '\n' substrs */
			cnt_nl(params->annotation->note,&annote_len) +
			1 +											/* space under note */
			2: 0;										/* space to chart */
		annote_len *= params->annotationFontWidth();

		qDebug( "done annote_len computation" );
	
		if( params->xaxis && hasxlabels ) {
			int biggest     = -MAXINT;
	
			int		len = 0;
			for( i=0; i<num_points; ++i ) {
				biggest = MAX( len, xlbl[i].length() );
			}

			xlabel_hgt = 1+ biggest*params->xAxisFontWidth() +1;
		}
	
		grapheight = imageheight - ( xtics          +
									 xtitle_hgt     +
									 xlabel_hgt     +
									 title_hgt      +
									 annote_hgt     +
									 ydepth_3Dtotal +
									 2 );
		if( params->hard_size && params->hard_graphheight )				/* user wants to use his */
			grapheight = params->hard_graphheight;
		params->hard_graphheight = grapheight;

		qDebug( "done grapheight computation" );

		// before width can be known...
		/* ----- y labels intervals ----- */
		{
			float	tmp_highest;
			/* possible y gridline points */
			float	ypoints[] = { 1.0/64.0, 1.0/32.0, 1.0/16.0, 1.0/8.0, 1.0/4.0, 1.0/2.0,
								  1.0,      2.0,      3.0,      5.0,     10.0,    25.0,
								  50.0,     100.0,    250.0,    500.0,   1000.0,  2500,    5000.0,
								  10000.0,  25000.0,  50000.0,  100000.0,500000.0,1000000, 5000000,
								  10000000 };
#define	NUM_YPOINTS	(sizeof(ypoints) / sizeof(float))
			int		max_num_ylbls;
			int		longest_ylblen = 0;
			/* maximum y lables that'll fit... */
			max_num_ylbls = grapheight / (3+params->yAxisFontHeight());
			if( max_num_ylbls < 3 ) {
				/* gdImageDestroy(im);		haven't yet created it */
				debug( "Insufficient Height" );
				return 2;
			}
			cerr << "Here it is still ok!\n";
			/* one "space" interval above + below */
			for( i=1; i<NUM_YPOINTS; ++i )
				// if( ypoints[i] > ylbl_interval )
				//	break;
				if( (highest-lowest)/ypoints[i] < ((float)max_num_ylbls-(1.0+1.0))
					* (float)params->ylabel_density/100.0 )
					break;
			/* gotta go through the above loop to catch the 'tweeners :-| */
			
			ylbl_interval = params->requested_yinterval != -MAXDOUBLE &&
				params->requested_yinterval > ypoints[i-1]?	  params->requested_yinterval:
				ypoints[i-1];
	
			/* perform floating point remainders */
			/* gonculate largest interval-point < lowest */
			if( lowest != 0.0 &&
				lowest != params->requested_ymin ) {
				if( lowest < 0.0 )
					lowest -= ylbl_interval;
				// lowest = (lowest-ypoints[0]) -
				// 			( ( ((lowest-ypoints[0])/ylbl_interval)*ylbl_interval ) -
				// 			   ( (float)((int)((lowest-ypoints[0])/ylbl_interval))*ylbl_interval ) );
				lowest = ylbl_interval * (float)(int)((lowest-ypoints[0])/ylbl_interval);
			}
			cerr << "Alive and healthy!\n";

			/* find smallest interval-point > highest */
			tmp_highest = lowest;
#warning "Kalle take a look at this, this is an infinite cycle for some reason"
			int maxcount = 0;
			do {	// while( (tmp_highest += ylbl_interval) <= highest )
				int		nmrtr, dmntr, whole;
				char	*price_to_str( float, int*, int*, int*, const char* );
				int		lbl_len;
				char	foo[32];
		
				if( params->yaxis )	{ /* XPG2 compatibility */
				  cerr << "At least I am doing something\n";
					sprintf( foo, do_ylbl_fractions ? QString( "%.0f" ): params->ylabel_fmt, tmp_highest );
					lbl_len = ylbl_interval<1.0? strlen( price_to_str(tmp_highest,
																	  &nmrtr,
																	  &dmntr,
																	  &whole,
																	  do_ylbl_fractions? QString::null: params->ylabel_fmt) ):
						strlen( foo );
					longest_ylblen = MAX( longest_ylblen, lbl_len );
				}
				cerr << "Too long, way yoo long" << maxcount << "\n";
			} while( (tmp_highest += ylbl_interval) <= highest && (maxcount++ < 100)); // BL.
			cout << "Uff, this was dangerous!\n";
			ylabel_wth = longest_ylblen * params->yAxisFontWidth();
			highest = params->requested_ymax==-MAXDOUBLE? tmp_highest:
				MAX( params->requested_ymax, highest );
	
			if( do_vol ) {
				float	num_yintrvls = (highest-lowest) / ylbl_interval;
				/* no skyscrapers */
				if( vhighest != 0.0 )
					vhighest += (vhighest-vlowest) / (num_yintrvls*2.0);
				if( vlowest != 0.0 )
					vlowest -= (vhighest-vlowest) / (num_yintrvls*2.0);
		
				if( params->yaxis2 ) {
					char	svlongest[32];
					int		lbl_len_low  = sprintf( svlongest, !params->ylabel2_fmt.isEmpty()? params->ylabel2_fmt: QString( "%.0f" ), vlowest );
					int		lbl_len_high = sprintf( svlongest, !params->ylabel2_fmt.isEmpty()? params->ylabel2_fmt: QString( "%.0f" ), vhighest );
					vlabel_wth = 1
						+ MAX( lbl_len_low,lbl_len_high ) * params->yAxisFontWidth();
				}
			}
		}
	
		graphwidth = imagewidth - ( ( (params->hard_size && params->hard_xorig)? params->hard_xorig:
									  ( ytitle_hgt +
										ylabel_wth +
										ytics ) )
									+ vtics
									+ vtitle_hgt
									+ vlabel_wth
									+ xdepth_3Dtotal );
		if( params->hard_size && params->hard_graphwidth )				/* user wants to use his */
			graphwidth = params->hard_graphwidth;
		params->hard_graphwidth = graphwidth;

		qDebug( "done graphwidth computation" );

		/* ----- scale to gif size ----- */
		/* offset to 0 at lower left (where it should be) */
		xscl = (float)(graphwidth-xdepth_3Dtotal) / (float)(num_points + (do_bar?2:0));
		yscl = -((float)grapheight) / (float)(highest-lowest);
		if( do_vol ) {
			float	hilow_diff = vhighest-vlowest==0.0? 1.0: vhighest-vlowest;
	
			vyscl = -((float)grapheight) / hilow_diff;
			vyorig = (float)grapheight
				+ ABS(vyscl) * MIN(vlowest,vhighest)
				+ ydepth_3Dtotal
				+ title_hgt
				+ annote_hgt;
		}
		xorig = (float)( imagewidth - ( graphwidth +
										vtitle_hgt +
										vtics      +
										vlabel_wth ) );
		if( params->hard_size && params->hard_xorig )
			xorig = params->hard_xorig;
		params->hard_xorig = (int)xorig;
		//	yorig = (float)grapheight + ABS(yscl * lowest) + ydepth_3Dtotal + title_hgt;
		yorig = (float)grapheight
			+ ABS(yscl) * MIN(lowest,highest)
			+ ydepth_3Dtotal
			+ title_hgt
			+ annote_hgt;
		//????	if( params->hard_size && params->hard_yorig )					/* vyorig too? */
		//????		yorig = params->hard_yorig;
		params->hard_yorig = (int)yorig;
	
		hlf_barwdth     = (int)( (float)(PX(2)-PX(1)) * (((float)params->bar_width/100.0)/2.0) );	// used only for bars
		hlf_hlccapwdth  = (int)( (float)(PX(2)-PX(1)) * (((float)params->hlc_cap_width/100.0)/2.0) );
    }
    // scaled, sized, ready

	qDebug( "scaled, sized, ready" );

    BGColor = params->BGColor;
    LineColor = params->LineColor;
    PlotColor = params->PlotColor;
    GridColor = params->GridColor;
    if( do_vol ) {
		VolColor = params->VolColor;
		for( i=0; i<num_points; ++i )
			if( params->ExtVolColor.count() )
				ExtVolColor[i] = params->ExtVolColor.color( i );
			else
				ExtVolColor[i] = VolColor;
    }
    //	ArrowDColor    = gdImageColorAllocate( im, 0xFF,    0, 0 );
    //	ArrowUColor    = gdImageColorAllocate( im,    0, 0xFF, 0 );
    if( params->annotation )
		AnnoteColor = params->annotation->color;
    qDebug("before bgimage");

    /* attempt to import optional background image */
    // PENDING(kalle) Put back in
    if( GDC_BGImage ) {
		debug( "Sorry, not implemented: background images" );
#ifdef SUPPORT_BACKGROUND_IMAGES
		FILE	*in = fopen(GDC_BGImage, "rb");
		if( !in ) {
			; // Cant load background image, drop it
		}
		else {
			if( bg_img = gdImageCreateFromGif(in) ) {					// =
				int	bgxpos = gdImageSX(bg_img)<imagewidth?  imageheight/2 - gdImageSX(bg_img)/2:  0,
					bgypos = gdImageSY(bg_img)<imageheight? imageheight/2 - gdImageSY(bg_img)/2: 0;
		
		
				if( gdImageSX(bg_img) > imagewidth ||				// resize only if too big
					gdImageSY(bg_img) > imageheight ) {				//  [and center]
					gdImageCopyResized( im, bg_img,				// dst, src
										bgxpos, bgypos,			// dstX, dstY
										0, 0,					// srcX, srcY
										imagewidth, imageheight,	// dstW, dstH
										imagewidth, imageheight );	// srcW, srcH
				} else											// just center
					gdImageCopy( im, bg_img,					// dst, src
								 bgxpos, bgypos,				// dstX, dstY
								 0, 0,							// srcX, srcY
								 imagewidth, imageheight );			// W, H
			}
			fclose(in);
		}
#endif
    }
    qDebug("Color settings coming!");
    for( j=0; j<num_sets; ++j )
		for( i=0; i<num_points; ++i )
			if( params->ExtColor.count() ) {			
			  cerr << "Ext color\n";
			  // changed by me, BL
			  //QColor ext_clr = params->ExtColor.color( num_points*j+i );			
			  QColor ext_clr = params->ExtColor.color( (num_points*j+i) % params->ExtColor.count());
			  ExtColor[j][i]            = ext_clr;
			  if( threeD )
			    ExtColorShd[j][i]     = QColor( ext_clr.red() / 2, ext_clr.green() / 2, ext_clr.blue() / 2 );
			}
			else if( params->SetColor.count() ) {
				QColor set_clr = params->SetColor.color( j );
				ExtColor[j][i]     = QColor( set_clr );
				if( threeD )
					ExtColorShd[j][i] = QColor( set_clr.red() / 2, set_clr.green() / 2, set_clr.blue() / 2 );
			}
			else {
				ExtColor[j][i]     = PlotColor;
				if( threeD )
					ExtColorShd[j][i] = QColor( params->PlotColor.red() / 2, params->PlotColor.green() / 2, params->PlotColor.blue() / 2 );
			}
			

    // PENDING(kalle) Do some sophisticated things that involve QPixmap::createHeuristicMask
	// or Matthias' stuff from bwin
    qDebug("before transparent bg");
    if( params->transparent_bg )
		debug( "Sorry, transparent backgrounds are not supported yet." );
	//     if( params->transparent_bg )
	// 	gdImageColorTransparent( im, BGColor );

	qDebug( "done handling background images" );

    if( !params->title.isEmpty() ) {
		int	tlen;
		QColor	titlecolor = params->TitleColor;
	
		cnt_nl( params->title.latin1(), &tlen );
		p->setFont( params->titleFont() );
		p->setPen( titlecolor );
		// PENDING(kalle) Check whether this really does line breaks
		QRect br = QFontMetrics( params->titleFont() ).boundingRect( 0, 0,
																	 MAXINT,
																	 MAXINT,
																	 Qt::AlignCenter,
																	 params->title );
		p->drawText( imagewidth/2 - tlen*params->titleFontWidth()/2, // x
					 0, // y
					 br.width(), br.height(),
					 Qt::AlignCenter, params->title );
    }

    qDebug( "done with the title text" );

    if( !params->xtitle.isEmpty() ) {
		QColor	titlecolor = params->XTitleColor == Qt::black ?
			PlotColor: params->XTitleColor;
		p->setPen( titlecolor );
		p->setFont( params->titleFont() );
		p->drawText( imagewidth/2 - params->xtitle.length()*params->xTitleFontWidth()/2,
					 imageheight-params->xTitleFontHeight()-1, params->xtitle );
    }

    qDebug( "start drawing" );

    /* ----- start drawing ----- */
    /* ----- backmost first - grid & labels ----- */
    if( params->grid || params->yaxis ){	/* grid lines & y label(s) */
		float	tmp_y = lowest;
		QColor labelcolor = params->YLabelColor== Qt::black ?
			LineColor: params->YLabelColor;
		QColor label2color = params->YLabel2Color== Qt::black ?
			VolColor: params->YLabel2Color;
	
		/* step from lowest to highest puting in labels and grid at interval points */
		/* since now "odd" intervals may be requested, try to step starting at 0,   */
		/* if lowest < 0 < highest                                                  */
		for( i=-1; i<=1; i+=2 ) { // -1, 1
			if( i == -1 )	
				if( lowest >= 0.0 ) //	all pos plotting
					continue;
				else
					tmp_y = MIN( 0, highest ); // step down to lowest
		
			if( i == 1 )	
				if( highest <= 0.0 ) //	all neg plotting
					continue;
				else
					tmp_y = MAX( 0, lowest ); // step up to highest
		
		
			//			if( !(highest > 0 && lowest < 0) )					// doesn't straddle 0
			//				{
			//				if( i == -1 )									// only do once: normal
			//					continue;
			//				}
			//			else
			//				tmp_y = 0;
		
			do {	// while( (tmp_y (+-)= ylbl_interval) < [highest,lowest] )
				int		n, d, w;
				char	*price_to_str( float, int*, int*, int*, const char* );
				char	nmrtr[3+1], dmntr[3+1], whole[8];
				char	all_whole = ylbl_interval<1.0? FALSE: TRUE;
			
				char	*ylbl_str = price_to_str( tmp_y,&n,&d,&w,
												  do_ylbl_fractions? QString::null: params->ylabel_fmt );
				if( do_ylbl_fractions )	{
					sprintf( nmrtr, "%d", n );
					sprintf( dmntr, "%d", d );
					sprintf( whole, "%d", w );
				}

				// qDebug( "drawing 1" );
		
				if( params->grid ) {
					int	x1, x2, y1, y2;
					// int	gridline_clr = tmp_y == 0.0? LineColor: GridColor;
					// tics
					x1 = PX(0);		y1 = PY(tmp_y);
					p->setPen( GridColor );
					p->drawLine( x1-2, y1, x1, y1 );
					setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
				num_sets:
					1;			// backmost
					x2 = PX(0);		y2 = PY(tmp_y);						// w/ new setno
					p->setPen( GridColor );
					p->drawLine( x1, y1, x2, y2 );		// depth for 3Ds
					p->setPen( GridColor );
					p->drawLine( x2, y2, PX(num_points-1+(do_bar?2:0)), y2 );
					setno = 0;											// set back to foremost
				}
				
				// qDebug( "drawing 2" );

				// PENDING(kalle) Originally, here was always used one
				// font smaller than params->yAxisFont. Do that again?
				if( params->yaxis )
					if( do_ylbl_fractions ) {
						if( w || (!w && !n && !d) ) {
							p->setPen( labelcolor );
							p->setFont( params->yAxisFont() );
							p->drawText( PX(0)-2-strlen(whole)*params->yAxisFontWidth()
										 - ( (!all_whole)?
											 (strlen(nmrtr)*params->yAxisFontWidth() +
											  params->yAxisFontWidth() +
											  strlen(nmrtr)*params->yAxisFontWidth()) :
											 1 ),
										 PY(tmp_y)-params->yAxisFontHeight()/2,
										 whole );
						}

						// qDebug( "drawing 3" );

						// PENDING( original uses a 1 step smaller
						// font here. Do that, too?
						if( n )	{
							p->setPen( labelcolor );
							p->setFont( params->yAxisFont() );
							p->drawText( PX(0)-2-strlen(nmrtr)*params->yAxisFontWidth()
										 -params->yAxisFontWidth()
										 -strlen(nmrtr)*params->yAxisFontWidth() + 1,
										 PY(tmp_y)-params->yAxisFontHeight()/2 + 1,
										 nmrtr );
							p->drawText( PX(0)-2-params->yAxisFontWidth()
										 -strlen(nmrtr)*params->yAxisFontWidth(),
										 PY(tmp_y)-params->yAxisFontHeight()/2,
										 "/" );
							p->drawText( PX(0)-2-strlen(nmrtr)*params->yAxisFontWidth() - 2,
										 PY(tmp_y)-params->yAxisFontHeight()/2 + 3,
										 dmntr );
						}
					} else {
						p->setPen( labelcolor );
						p->setFont( params->yAxisFont() );
						p->drawText( PX(0)-2-strlen(ylbl_str)*params->yAxisFontWidth(),
									 PY(tmp_y)-params->yAxisFontHeight()/2,
									 ylbl_str );
					}

				// qDebug( "drawing 4" );
			
				if( do_vol && params->yaxis2 ) {
					char	vylbl[16];
					/* opposite of PV(y) */
					sprintf( vylbl,
							 !params->ylabel2_fmt.isEmpty()? params->ylabel2_fmt: QString( "%.0f" ),
							 ((float)(PY(tmp_y)+(setno*ydepth_3D)-vyorig))/vyscl );
			
					setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
				num_sets:
					1; // backmost
					p->setPen( GridColor );
					p->drawLine( PX(num_points-1+(do_bar?2:0)), PY(tmp_y),
								 PX(num_points-1+(do_bar?2:0))+3, PY(tmp_y) );
					if( atof(vylbl) == 0.0 )									/* rounding can cause -0 */
						strcpy( vylbl, "0" );
					p->setPen( label2color );
					p->setFont( params->yAxisFont() );
					p->drawText( PX(num_points-1+(do_bar?2:0))+6,
								 PY(tmp_y)-params->yAxisFontHeight()/2,
								 vylbl );
					setno = 0;
				}
			}
			while( ((i>0) && ((tmp_y += ylbl_interval) < highest)) ||
				   ((i<0) && ((tmp_y -= ylbl_interval) > lowest)) );
		}

		// qDebug( "drawing 5" );

		/* catch last (bottom) grid line - specific to an "off" requested interval */
		if( params->grid && threeD ) {
			setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
		num_sets:
			1;			// backmost
			p->setPen( GridColor );
			p->drawLine( PX(0), PY(lowest), PX(num_points-1+(do_bar?2:0)), PY(lowest) );
			setno = 0;											// set back to foremost
		}
	
		/* vy axis title */
		if( do_vol && !params->ytitle2.isEmpty() ) {
			QColor	titlecolor = params->YTitle2Color== Qt::black ?
				VolColor: params->YTitle2Color;
			// PENDING(kalle) Check whether this really prints correctly
			p->setFont( params->yTitleFont() );
			p->setPen( titlecolor );
			p->rotate( 90 );
			p->drawText( imagewidth-(1+params->yTitleFontHeight()),
						 params->ytitle2.length()*params->yTitleFontWidth()/2 +
						 grapheight/2, params->ytitle2 );
			p->rotate( -90 );
		}
	
		/* y axis title */
		if( params->yaxis && !params->ytitle.isEmpty() ) {
			int	ytit_len = params->ytitle.length()*params->yTitleFontWidth();
			QColor	titlecolor = params->YTitleColor==Qt::black?
				PlotColor: params->YTitleColor;
			p->setPen( titlecolor );
			p->setFont( params->yTitleFont() );
			p->drawText( 0, imageheight/2 + ytit_len/2, params->ytitle );
		}
    }

    /* interviening set grids */
    /*  0 < setno < num_sets   non-inclusive, they've already been covered */
    if( params->grid && threeD ) {
		for( setno=(params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1) - 1;
			 setno > 0;
			 --setno ) {
			p->setPen( GridColor );
			p->drawLine( PX(0), PY(lowest), PX(0), PY(highest) );
			p->drawLine( PX(0), PY(lowest), PX(num_points-1+(do_bar?2:0)), PY(lowest) );
		}
		setno = 0;
    }

    if( ( params->grid || params->shelf ) &&							/* line color grid at 0 */
		( (lowest < 0.0 && highest > 0.0) ||
		  (lowest < 0.0 && highest > 0.0) ) ) {
		int	x1, x2, y1, y2;
		// tics
		x1 = PX(0);		y1 = PY(0);
		p->setPen( LineColor );
		p->drawLine( x1-2, y1, x1, y1 );
		setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
		  num_sets:
		1;				// backmost
		x2 = PX(0);		y2 = PY(0);								// w/ new setno
		p->setPen( LineColor );
		p->drawLine( x1, y1, x2, y2 );			// depth for 3Ds
		p->drawLine( x2, y2, PX(num_points-1+(do_bar?2:0)), y2 );
		setno = 0;												// set back to foremost
    }


    /* x ticks and xlables */
    if( params->grid || params->xaxis )	{
		int		num_xlbls =										/* maximum x lables that'll fit */
			/* each xlbl + avg due to num_lf_xlbls */
			graphwidth /
			( (GDC_xlabel_spacing==MAXSHORT?0:GDC_xlabel_spacing)+params->xAxisFontHeight() +
			  (num_lf_xlbls*(params->xAxisFontHeight()-1))/num_points );
		QColor labelcolor = params->XLabelColor== Qt::black ?
			LineColor: params->XLabelColor;
	
		for( i=0; i<num_points+(do_bar?2:0); ++i )
			if( (i%(1+num_points/num_xlbls) == 0) ||					// # x labels are regulated
				(num_xlbls >= num_points)         ||
				GDC_xlabel_spacing == MAXSHORT ) {
				int	xi = do_bar? i-1: i;
		
				if( params->grid ) {
					int	x1, x2, y1, y2;
					// tics
					x1 = PX(i);		y1 = PY(lowest);
					p->setPen( GridColor );
					p->drawLine( x1, y1, x1,  y1+2 );
					setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
				num_sets:
					1; // backmost
					x2 = PX(i);		y2 = PY(lowest);
					p->setPen( GridColor );
					p->drawLine( x1, y1, x2,  y2 );		// depth perspective
					p->drawLine( x2, y2, x2,  PY(highest) );
					setno = 0;											// reset to foremost
				}
		
				if( !do_bar || (i>0 && xi<num_points) )
					if( params->xaxis && hasxlabels ) {
						/* waiting for GDCImageStringUpNL() */
#define	LBXH		params->xAxisFontHeight()
#define LBXW        params->xAxisFontWidth()						
						int		xlen = 0;
						short	xstrs_num = cnt_nl( xlbl[xi], &xlen );
						//						char	sub_xlbl[xlen+1];
						//						int		xlbl_strt = -1+ PX((float)i+(float)(do_bar?((float)num_points/(float)num_xlbls):0.0)) - (int)((float)(LBXH-2)*((float)xstrs_num/2.0));
						int		xlbl_strt = -1+ PX(i) - (int)((float)(LBXH-2)*((float)xstrs_num/2.0));
						QString currentfulllabel = xlbl[xi];
						xlen      = -1;
						xstrs_num = -1;
						j = -1;
						QStringList sublabels = QStringList::split( '\n', currentfulllabel );
						for( QStringList::Iterator sublabelit = sublabels.begin();
							 sublabelit != sublabels.end(); ++sublabelit ) {
							++xstrs_num;
							p->setFont( params->xAxisFont() );
							p->setPen( labelcolor );
							p->rotate( 90 );
							p->drawText( xlbl_strt + (LBXH-1)*xstrs_num,
										 PY(lowest) + 2 + 1 + LBXW*xlen,
										 (*sublabelit).latin1() );
							p->rotate( -90 );
						}
#undef LBXW
#undef LBXH
					}
			}
    }

    /* ----- solid poly region (volume) ----- */
    /*  so that grid lines appear under solid */
    if( do_vol ) {
		setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
    num_sets:
		1; // backmost
		if( params->type == KCHARTTYPE_COMBO_HLC_BAR    ||
			params->type == KCHARTTYPE_COMBO_LINE_BAR   ||
			params->type == KCHARTTYPE_3DCOMBO_LINE_BAR ||
			params->type == KCHARTTYPE_3DCOMBO_HLC_BAR ) {
			if( uvol[0] != GDC_NOVALUE )
				draw_3d_bar( p, PX(0), PX(0)+hlf_barwdth,
							 PV(0), PV(uvol[0]),
							 0, 0,
							 ExtVolColor[0],
							 ExtVolColor[0] );
			for( i=1; i<num_points-1; ++i )
				if( uvol[i] != GDC_NOVALUE )
					draw_3d_bar( p, PX(i)-hlf_barwdth, PX(i)+hlf_barwdth,
								 PV(0), PV(uvol[i]),
								 0, 0,
								 ExtVolColor[i],
								 ExtVolColor[i] );
			if( uvol[i] != GDC_NOVALUE )
				draw_3d_bar( p, PX(i)-hlf_barwdth, PX(i),
							 PV(0), PV(uvol[i]),
							 0, 0,
							 ExtVolColor[i],
							 ExtVolColor[i] );
		} else if( params->type == KCHARTTYPE_COMBO_HLC_AREA   ||
				   params->type == KCHARTTYPE_COMBO_LINE_AREA  ||
				   params->type == KCHARTTYPE_3DCOMBO_LINE_AREA||
				   params->type == KCHARTTYPE_3DCOMBO_HLC_AREA )
			for( i=1; i<num_points; ++i )
				if( uvol[i-1] != GDC_NOVALUE && uvol[i] != GDC_NOVALUE )
					draw_3d_area( p, PX(i-1), PX(i),
								  PV(0), PV(uvol[i-1]), PV(uvol[i]),
								  0, 0,
								  ExtVolColor[i],
								  ExtVolColor[i] );
		setno = 0;
    }		// volume polys done

    if( params->annotation && threeD ) {		/* back half of annotation line */
		int	x1 = PX(params->annotation->point+(do_bar?1:0)),
			y1 = PY(lowest);
		setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
    num_sets:
		1; // backmost
		p->setPen( AnnoteColor );
		p->drawLine( x1, y1, PX(params->annotation->point+(do_bar?1:0)), PY(lowest) );
		p->drawLine( PX(params->annotation->point+(do_bar?1:0)), PY(lowest),
					 PX(params->annotation->point+(do_bar?1:0)), PY(highest)-2 );
		setno = 0;
    }

    /* ---------- start plotting the data ---------- */
    switch( params->type ) {
    case KCHARTTYPE_3DBAR:					/* depth, width, y interval need to allow for whitespace between bars */
    case KCHARTTYPE_BAR:
		/* --------- */
		switch( params->stack_type ) {
		case KCHARTSTACKTYPE_DEPTH:
			for( setno=num_sets-1; setno>=0; --setno )		// back sets first   PX, PY depth
				for( i=0; i<num_points; ++i )
					if( CELLEXISTS( setno, i ) )
						draw_3d_bar( p, PX(i+(do_bar?1:0))-hlf_barwdth, PX(i+(do_bar?1:0))+hlf_barwdth,
									 PY(0), PY( CELLVALUE( setno, i )),
									 xdepth_3D, ydepth_3D,
									 ExtColor[setno][i],
									 threeD? ExtColorShd[setno][i]: ExtColor[setno][i] );
			setno = 0;
			break;
			
		case KCHARTSTACKTYPE_LAYER:
			{
				float	lasty[num_points];
				j = 0;
				//				for( i=0; i<num_points; ++i )
				//					if( CELLEXISTS( j, i ) ) {
				//						lasty[i] = CELLVALUE( j, i );
				//						draw_3d_bar( im, PX(i+(do_bar?1:0))-hlf_barwdth, PX(i+(do_bar?1:0))+hlf_barwdth,
				//										 PY(0), PY(CELLVALUE( j, i )),
				//										 xdepth_3D, ydepth_3D,
				//										 ExtColor[j][i],
				//										 threeD? ExtColorShd[j][i]: ExtColor[j][i] );
				//						}
				for( i=0; i<num_points; ++i ) {
					struct BS	barset[num_sets];
					float		lasty_pos = 0.0;
					float		lasty_neg = 0.0;
					int			k;
					
					for( j=0, k=0; j<num_sets; ++j ) {
						if( CELLEXISTS( j, i ) ) {
							if( CELLVALUE( j, i ) < 0.0 ) {
								barset[k].y1 = lasty_neg;
								barset[k].y2 = CELLVALUE( j, i ) + lasty_neg;
								lasty_neg    = barset[k].y2;
							} else {
								barset[k].y1 = lasty_pos;
								barset[k].y2 = CELLVALUE( j, i ) + lasty_pos;
								lasty_pos    = barset[k].y2;
							}
							barset[k].clr   = ExtColor[j][i];
							barset[k].shclr = threeD? ExtColorShd[j][i]: ExtColor[j][i];
							++k;
						}
					}
					qsort( barset, k, sizeof(struct BS), barcmpr );
					
					for( j=0; j<k; ++j ) {
						draw_3d_bar( p,
									 PX(i+(do_bar?1:0))-hlf_barwdth, PX(i+(do_bar?1:0))+hlf_barwdth,
									 PY(barset[j].y1), PY(barset[j].y2),
									 xdepth_3D, ydepth_3D,
									 barset[j].clr,
									 barset[j].shclr );
					}
				}
			}
			break;
			
		case KCHARTSTACKTYPE_BESIDE:
			{												// h/.5, h/1, h/1.5, h/2, ...
				int	new_barwdth = (int)( (float)hlf_barwdth / ((float)num_sets/2.0) );
				for( i=0; i<num_points; ++i )
					for( j=0; j<num_sets; ++j )
						if( CELLEXISTS( j, i ) )
							draw_3d_bar( p, PX(i+(do_bar?1:0))-hlf_barwdth+new_barwdth*j+1,
										 PX(i+(do_bar?1:0))-hlf_barwdth+new_barwdth*(j+1),
										 PY(0), PY(CELLVALUE( j, i )),
										 xdepth_3D, ydepth_3D,
										 ExtColor[j][i],
										 threeD? ExtColorShd[j][i]: ExtColor[j][i] );
			}
			break;
		}
		break;
	
    case KCHARTTYPE_LINE:
    case KCHARTTYPE_COMBO_LINE_BAR:
    case KCHARTTYPE_COMBO_LINE_AREA:
		for( j=num_sets-1; j>=0; --j )
			for( i=1; i<num_points; ++i )
				if( CELLEXISTS(j,i-1) && CELLEXISTS(j,i-1) ) {
					p->setPen( ExtColor[j][i] );
					p->drawLine( PX(i-1), PY(CELLVALUE( j, i-1 )), PX(i), PY(CELLVALUE( j, i )) );
					p->drawLine( PX(i-1), PY(CELLVALUE(j,i-1))+1, PX(i), PY(CELLVALUE(j,i))+1 );
				} else {
					if( CELLEXISTS(j,i-1) ) {
						p->setPen( ExtColor[j][i] );
						p->drawPoint( PX(i-1), PY(CELLVALUE(j,i-1)) );
					}
					if( CELLEXISTS(j,i) ) {
						p->setPen( ExtColor[j][i] );
						p->drawPoint( PX(i), PY(CELLVALUE(j,i)) );
					}
				}
		break;
	
    case KCHARTTYPE_3DLINE:
    case KCHARTTYPE_3DCOMBO_LINE_BAR:
    case KCHARTTYPE_3DCOMBO_LINE_AREA:
		{
			int	y1[num_sets],
				y2[num_sets];
	
			for( i=1; i<num_points; ++i ) {
				if( params->stack_type == KCHARTSTACKTYPE_DEPTH ) {
					for( j=num_sets-1; j>=0; --j )
						if( CELLEXISTS(j,i-1) &&
							CELLEXISTS(j,i) ) {
							setno = j;
							y1[j] = PY(CELLVALUE(j,i-1));
							y2[j] = PY(CELLVALUE(j,i));
			
							draw_3d_line( p,
										  PY(0),
										  PX(i-1), PX(i),
										  &(y1[j]), &(y2[j]),
										  xdepth_3D, ydepth_3D,
										  1,
										  &(ExtColor[j][i]),
										  &(ExtColorShd[j][i]) );
							setno = 0;
						}
				}
				else if( params->stack_type == KCHARTSTACKTYPE_BESIDE ||
						 params->stack_type == KCHARTSTACKTYPE_SUM ) {			// all same plane
					int		set;
					QColor clr[num_sets], clrshd[num_sets];
					float	usey1 = 0.0,
						usey2 = 0.0;
					for( j=0,set=0; j<num_sets; ++j )
						if( CELLEXISTS(j,i-1) &&
							CELLEXISTS(j,i) ) {
							if( params->stack_type == KCHARTSTACKTYPE_SUM ) {
								usey1 += CELLVALUE(j,i-1);
								usey2 += CELLVALUE(j,i);
							} else {
								usey1 = CELLVALUE(j,i-1);
								usey2 = CELLVALUE(j,i);
							}
							y1[set]     = PY(usey1);
							y2[set]     = PY(usey2);
							clr[set]    = ExtColor[j][i];
							clrshd[set] = ExtColorShd[j][i];	/* fred */
							++set;
						}
					draw_3d_line( p,
								  PY(0),
								  PX(i-1), PX(i),
								  y1, y2,
								  xdepth_3D, ydepth_3D,
								  set,
								  clr,
								  clrshd );
				}
			}
		}
		break;
	
    case KCHARTTYPE_AREA:
    case KCHARTTYPE_3DAREA:
		switch( params->stack_type )
			{
			case KCHARTSTACKTYPE_SUM:
				{
					float	lasty[num_points];
					j = 0;
					for( i=1; i<num_points; ++i )
						if( CELLEXISTS(j,i) )
							{
								lasty[i] = CELLVALUE(j,i);
								if( CELLEXISTS(j,i-1) )
									draw_3d_area( p, PX(i-1), PX(i),
												  PY(0), PY(CELLVALUE(j,i-1)), PY(CELLVALUE(j,i)),
												  xdepth_3D, ydepth_3D,
												  ExtColor[j][i],
												  threeD? ExtColorShd[j][i]: ExtColor[j][i] );
							}
					for( j=1; j<num_sets; ++j )
						for( i=1; i<num_points; ++i )
							if( CELLEXISTS(j,i) &&
								CELLEXISTS(j,i-1) ) {
								draw_3d_area( p, PX(i-1), PX(i),
											  PY(lasty[i]), PY(lasty[i-1]+CELLVALUE(j,i-1)), PY(lasty[i]+CELLVALUE(j,i)),
											  xdepth_3D, ydepth_3D,
											  ExtColor[j][i],
											  threeD? ExtColorShd[j][i]: ExtColor[j][i] );
								lasty[i] += CELLVALUE(j,i);
							}
				}
				break;
		
			case KCHARTSTACKTYPE_BESIDE:								// behind w/o depth
				for( j=num_sets-1; j>=0; --j )					// back sets 1st  (setno = 0)
					for( i=1; i<num_points; ++i )
						if( CELLEXISTS(j,i-1) && CELLEXISTS(j,i) )
							draw_3d_area( p, PX(i-1), PX(i),
										  PY(0), PY(CELLVALUE(j,i-1)), PY(CELLVALUE(j,i)),
										  xdepth_3D, ydepth_3D,
										  ExtColor[j][i],
										  threeD? ExtColorShd[j][i]: ExtColor[j][i] );
				break;
		
			case KCHARTSTACKTYPE_DEPTH:
			default:
				for( setno=num_sets-1; setno>=0; --setno )		// back sets first   PX, PY depth
					for( i=1; i<num_points; ++i )
						if( CELLEXISTS(setno,i-1) &&
							CELLEXISTS(setno,i) )
							draw_3d_area( p, PX(i-1), PX(i),
										  PY(0), PY(CELLVALUE(setno,i-1)), PY(CELLVALUE(setno,i)),
										  xdepth_3D, ydepth_3D,
										  ExtColor[setno][i],
										  threeD? ExtColorShd[setno][i]: ExtColor[setno][i] );
				setno = 0;
			}
		break;
	
    case KCHARTTYPE_3DHILOCLOSE:
    case KCHARTTYPE_3DCOMBO_HLC_BAR:
    case KCHARTTYPE_3DCOMBO_HLC_AREA:
		{
			QPointArray  poly( 4 );
			for( j=num_hlc_sets-1; j>=0; --j ) {
				for( i=1; i<num_points+1; ++i )
					if( CELLEXISTS(CLOSESET+j*3,i-1) ) {
						if( (params->hlc_style & KCHARTHLCSTYLE_ICAP) &&			// bottom half of 'I'
							CELLEXISTS(LOWSET+j*3,i-1) ) {
							set3dpoly( poly, PX(i-1)-hlf_hlccapwdth, PX(i-1)+hlf_hlccapwdth,
									   PY(CELLVALUE(LOWSET+j*3,i-1)), PY(CELLVALUE(LOWSET+j*3,i-1)),
									   xdepth_3D, ydepth_3D );
							p->setBrush( QBrush( ExtColor[LOWSET+j*3][i-1] ) );
							p->setPen( ExtColor[LOWSET+j*3][i-1] );
							p->drawPolygon( poly );
							p->setBrush( QBrush( ExtColorShd[LOWSET+j*3][i-1], Qt::NoBrush ) );
							p->setPen( ExtColorShd[LOWSET+j*3][i-1] );
							p->drawPolygon( poly );
						}
						// all HLC have vert line
						if( CELLEXISTS(LOWSET+j*3,i-1) ) {// bottom 'half'
								set3dpoly( poly, PX(i-1), PX(i-1),
										   PY(CELLVALUE(LOWSET+j*3,i-1)), PY(CELLVALUE(CLOSESET+j*3,i-1)),
										   xdepth_3D, ydepth_3D );
								p->setBrush( QBrush( ExtColor[LOWSET+j*3][i-1] ) );
								p->setPen( ExtColor[LOWSET+j*3][i-1] );
								p->drawPolygon( poly );
								p->setBrush( QBrush( ExtColorShd[LOWSET+j*3][i-1], Qt::NoBrush ) );
								p->setPen( ExtColorShd[LOWSET+j*3][i-1] );
								p->drawPolygon( poly );
							}
						if( CELLEXISTS(HIGHSET+j*3,i-1 ) ) {// top 'half'
								set3dpoly( poly, PX(i-1), PX(i-1),
										   PY(CELLVALUE(CLOSESET+j*3,i-1)), PY(CELLVALUE(HIGHSET+j*3,i-1)),
										   xdepth_3D, ydepth_3D );
								p->setBrush( QBrush( ExtColor[HIGHSET+j*3][i-1] ) );
								p->setPen( ExtColor[HIGHSET+j*3][i-1] );
								p->drawPolygon( poly );
								p->setBrush( QBrush( ExtColorShd[HIGHSET+j*3][i-1], Qt::NoBrush ) );
								p->setPen( ExtColorShd[HIGHSET+j*3][i-1] );
								p->drawPolygon( poly );
							}
						// line at close
						p->setPen( ExtColorShd[CLOSESET+j*3][i-1] );
						p->drawLine( PX(i-1), PY(CELLVALUE(CLOSESET+j*3,i-1)),
									 PX(i-1)+xdepth_3D, PY(CELLVALUE(CLOSESET+j*3,i-1))-ydepth_3D );
						// top half 'I'
						if( !( (params->hlc_style & KCHARTHLCSTYLE_DIAMOND) &&
							   (PY(CELLVALUE(HIGHSET+j*3,i-1)) > PY(CELLVALUE(CLOSESET+j*3,i-1))-hlf_hlccapwdth) ) &&
							CELLEXISTS(HIGHSET+j*3,i-1) )
							if( params->hlc_style & KCHARTHLCSTYLE_ICAP )	{
									set3dpoly( poly, PX(i-1)-hlf_hlccapwdth, PX(i-1)+hlf_hlccapwdth,
											   PY(CELLVALUE(HIGHSET+j*3,i-1)), PY(CELLVALUE(HIGHSET+j*3,i-1)),
											   xdepth_3D, ydepth_3D );
									p->setBrush( QBrush( ExtColor[HIGHSET+j*3][i-1] ) );
									p->setPen( ExtColor[HIGHSET+j*3][i-1] );
									p->drawPolygon( poly );
									p->setBrush( QBrush( ExtColorShd[HIGHSET+j*3][i-1], Qt::NoBrush ) );
									p->setPen( ExtColorShd[HIGHSET+j*3][i-1] );
									p->drawPolygon( poly );
								}
			
						if( i < num_points &&
							CELLEXISTS(CLOSESET+j*3,i) ) {
								if( params->hlc_style & KCHARTHLCSTYLE_CLOSECONNECTED )	/* line from prev close */
									{
										set3dpoly( poly, PX(i-1), PX(i),
												   PY(CELLVALUE(CLOSESET+j*3,i-1)), PY(CELLVALUE(CLOSESET+j*3,i-1)),
												   xdepth_3D, ydepth_3D );
										p->setBrush( QBrush( ExtColor[CLOSESET+j*3][i] ) );
										p->setPen( ExtColor[CLOSESET+j*3][i] );
										p->drawPolygon( poly );
										p->setBrush( QBrush( ExtColorShd[CLOSESET+j*3][i], Qt::NoBrush ) );
										p->setPen( ExtColorShd[CLOSESET+j*3][i] );
										p->drawPolygon( poly );
									}
								else	// CLOSE_CONNECTED and CONNECTING are mutually exclusive
									if( params->hlc_style &
										KCHARTHLCSTYLE_CONNECTING ) {	/* thin connecting line */
										int	y1 = PY(CELLVALUE(CLOSESET+j*3,i-1)),
											y2 = PY(CELLVALUE(CLOSESET+j*3,i));
										draw_3d_line( p,
													  PY(0),
													  PX(i-1), PX(i),
													  &y1, &y2,					// rem only 1 set
													  xdepth_3D, ydepth_3D,
													  1,
													  &(ExtColor[CLOSESET+j*3][i]),
													  &(ExtColorShd[CLOSESET+j*3][i]) );
										// edge font of it
										p->setPen( ExtColorShd[CLOSESET+j*3][i] );
										p->drawLine( PX(i-1), PY(CELLVALUE(CLOSESET+j*3,i-1)),
													 PX(i), PY(CELLVALUE(CLOSESET+j*3,i)) );
									}
								// top half 'I' again
								if( PY(CELLVALUE(CLOSESET+j*3,i-1)) <= PY(CELLVALUE(CLOSESET+j*3,i)) &&
									CELLEXISTS(HIGHSET+j*3,i-1) )
									if( params->hlc_style & KCHARTHLCSTYLE_ICAP ) {
										set3dpoly( poly, PX(i-1)-hlf_hlccapwdth, PX(i-1)+hlf_hlccapwdth,
												   PY(CELLVALUE(HIGHSET+j*3,i-1)), PY(CELLVALUE(HIGHSET+j*3,i-1)),
												   xdepth_3D, ydepth_3D );
										p->setBrush( QBrush( ExtColor[HIGHSET+j*3][i-1] ) );
										p->setPen( ExtColor[HIGHSET+j*3][i-1] );
										p->drawPolygon( poly );
										p->setBrush( QBrush( ExtColorShd[HIGHSET+j*3][i-1], Qt::NoBrush ) );
										p->setPen( ExtColorShd[HIGHSET+j*3][i-1] );
										p->drawPolygon( poly );
									}
							}
						if( params->hlc_style & KCHARTHLCSTYLE_DIAMOND )  { // front
							poly.setPoint( 0, PX(i-1)-hlf_hlccapwdth,
										   PY(CELLVALUE(CLOSESET+j*3,i-1) ) );
							poly.setPoint( 1, PX(i-1), PY(CELLVALUE(CLOSESET+j*3,i-1))+hlf_hlccapwdth );
							poly.setPoint( 2, PX(i-1)+hlf_hlccapwdth, PY(CELLVALUE(CLOSESET+j*3,i-1) ) );
							poly.setPoint( 3, PX(i-1), PY(CELLVALUE(CLOSESET+j*3,i-1))-hlf_hlccapwdth );
							p->setBrush( QBrush( ExtColor[CLOSESET+j*3][i-1] ) );
							p->setPen( ExtColor[CLOSESET+j*3][i-1] );
							p->drawPolygon( poly );
							p->setBrush( QBrush( ExtColorShd[CLOSESET+j*3][i-1], Qt::NoBrush ) );
							p->setPen( ExtColorShd[CLOSESET+j*3][i-1] );
							p->drawPolygon( poly );
							// bottom side
							set3dpoly( poly, PX(i-1), PX(i-1)+hlf_hlccapwdth,
									   PY(CELLVALUE(CLOSESET+j*3,i-1))+hlf_hlccapwdth,
									   PY(CELLVALUE(CLOSESET+j*3,i-1)),
									   xdepth_3D, ydepth_3D );
							p->setBrush( QBrush( ExtColorShd[CLOSESET+j*3][i-1] ) );
							p->setPen( ExtColorShd[CLOSESET+j*3][i-1] );
							p->drawPolygon( poly );

							// gdImagePolygon( im, poly, 4, ExtColor[CLOSESET+j*3][i-1] );
							// top side
							set3dpoly( poly, PX(i-1), PX(i-1)+hlf_hlccapwdth,
									   PY(CELLVALUE(CLOSESET+j*3,i-1))-hlf_hlccapwdth,
									   PY(CELLVALUE(CLOSESET+j*3,i-1)),
									   xdepth_3D, ydepth_3D );
							p->setBrush( QBrush( ExtColor[CLOSESET+j*3][i-1] ) );
							p->setPen( ExtColor[CLOSESET+j*3][i-1] );
							p->drawPolygon( poly );
							p->setBrush( QBrush( ExtColorShd[CLOSESET+j*3][i-1], Qt::NoBrush ) );
							p->setPen( ExtColorShd[CLOSESET+j*3][i-1] );
							p->drawPolygon( poly );
						}
					}
			}
		}
		break;
	
    case KCHARTTYPE_HILOCLOSE:
    case KCHARTTYPE_COMBO_HLC_BAR:
    case KCHARTTYPE_COMBO_HLC_AREA:
		for( j=num_hlc_sets-1; j>=0; --j ) {
			for( i=0; i<num_points; ++i )
				if( CELLEXISTS(CLOSESET+j*3,i) ){ /* all HLC have vert line */
					if( CELLEXISTS(LOWSET+j*3,i) ) {
						p->setPen( ExtColor[LOWSET+(j*3)][i] );
						p->drawLine( PX(i), PY(CELLVALUE(CLOSESET+j*3,i)),
									 PX(i), PY(CELLVALUE(LOWSET+j*3,i)) );
					}
					if( CELLEXISTS(HIGHSET+j*3,i) ) {
						p->setPen( ExtColor[HIGHSET+j*3][i] );
						p->drawLine( PX(i), PY(CELLVALUE(HIGHSET+j*3,i)),
									 PX(i), PY(CELLVALUE(CLOSESET+j*3,i)) );
					}

					if( params->hlc_style & KCHARTHLCSTYLE_ICAP ) {
						if( CELLEXISTS(LOWSET+j*3,i) ) {
							p->setPen( ExtColor[LOWSET+j*3][i] );
							p->drawLine( PX(i)-hlf_hlccapwdth, PY(CELLVALUE(LOWSET+j*3,i)),
										 PX(i)+hlf_hlccapwdth, PY(CELLVALUE(LOWSET+j*3,i)) );
						}
						if( CELLEXISTS(HIGHSET+j*3,i) ) {
							p->setPen( ExtColor[HIGHSET+j*3][i] );
							p->drawLine( PX(i)-hlf_hlccapwdth, PY(CELLVALUE(HIGHSET+j*3,i)),
										 PX(i)+hlf_hlccapwdth, PY(CELLVALUE(HIGHSET+j*3,i)) );
						}
					}
					if( params->hlc_style & KCHARTHLCSTYLE_DIAMOND ) {
						QPointArray cd( 4 );
						cd.setPoint( 0, PX(i)-hlf_hlccapwdth, PY(CELLVALUE(CLOSESET+j*3,i) ) );
						cd.setPoint( 1, PX(i), PY(CELLVALUE(CLOSESET+j*3,i))+hlf_hlccapwdth );
						cd.setPoint( 2, PX(i)+hlf_hlccapwdth, PY(CELLVALUE(CLOSESET+j*3,i) ) );
						cd.setPoint( 3, PX(i), PY(CELLVALUE(CLOSESET+j*3,i))-hlf_hlccapwdth );
						p->setBrush( QBrush( ExtColor[CLOSESET+j*3][i] ) );
						p->setPen( ExtColor[CLOSESET+j*3][i] );
						p->drawPolygon( cd );
					}
				}
			for( i=1; i<num_points; ++i )
				if( CELLEXISTS(CLOSESET+j*3,i-1) &&
					CELLEXISTS(CLOSESET+j*3,i) ) {
					if( params->hlc_style & KCHARTHLCSTYLE_CLOSECONNECTED ) {	// line from prev close
						p->setPen( ExtColor[CLOSESET+j*3][i] );
						p->drawLine( PX(i-1), PY(CELLVALUE(CLOSESET+j*3,i-1)),
									 PX(i), PY(CELLVALUE(CLOSESET+j*3,i-1)) );
					} else	// CLOSE_CONNECTED and CONNECTING are mutually exclusive
						if( params->hlc_style & KCHARTHLCSTYLE_CONNECTING ) {		// thin connecting line
							p->setPen( ExtColor[CLOSESET+j*3][i] );
							p->drawLine( PX(i-1), PY(CELLVALUE(CLOSESET+j*3,i-1)),
										 PX(i), PY(CELLVALUE(CLOSESET+j*3,i)) );
						}
				}
		}
		break;
    }
    setno = 0;

    /* ---------- scatter points  over all other plots ---------- */
    /* scatters, by their very nature, don't lend themselves to standard array of points */
    /* also, this affords the opportunity to include scatter points onto any type of chart */
    /* drawing of the scatter point should be an exposed function, so the user can */
    /*  use it to draw a legend, and/or add their own */
    if( params->scatter ) {
		QColor		scatter_clr[params->num_scatter_pts];
		QPointArray ct( 3 );
	
		for( i=0; i<params->num_scatter_pts; ++i ) {
			int		hlf_scatterwdth = (int)( (float)(PX(2)-PX(1))
											 * (((float)(((params->scatter)+i)->width)/100.0)/2.0) );
			int	scat_x = PX( ((params->scatter)+i)->point + (do_bar?1:0) ),
				scat_y = PY( ((params->scatter)+i)->val );
	
			if( ((params->scatter)+i)->point >= num_points ||				// invalid point
				((params->scatter)+i)->point <  0 )
				continue;
			scatter_clr[i] = ((params->scatter)+i)->color;
	
			switch( ((params->scatter)+i)->ind ) {
			case KCHARTSCATTER_TRIANGLE_UP:
				ct.setPoint( 0, scat_x, scat_y );
				ct.setPoint( 1, scat_x - hlf_scatterwdth, scat_y + hlf_scatterwdth );
				ct.setPoint( 2, scat_x + hlf_scatterwdth, scat_y + hlf_scatterwdth );
				if( !do_bar )
					if( ((params->scatter)+i)->point == 0 )
						ct.setPoint( 1, scat_x, ct.point( 1 ).y() );
					else
						if( ((params->scatter)+i)->point == num_points-1 )
							ct.setPoint( 2, scat_x, ct.point( 2 ).y() );
				p->setBrush( QBrush( scatter_clr[i] ) );
				p->setPen( scatter_clr[i] );
				p->drawPolygon( ct );
				break;
			case KCHARTSCATTER_TRIANGLE_DOWN:
				ct.setPoint( 0, scat_x, scat_y );
				ct.setPoint( 1, scat_x - hlf_scatterwdth, scat_y - hlf_scatterwdth );
				ct.setPoint( 2, scat_x + hlf_scatterwdth, scat_y - hlf_scatterwdth );
				if( !do_bar )
					if( ((params->scatter)+i)->point == 0 )
						ct.setPoint( 1, scat_x, ct.point( 1 ).y() );
					else
						if( ((params->scatter)+i)->point == num_points-1 )
							ct.setPoint( 2, scat_x, ct.point( 2 ).y() );
				p->setBrush( QBrush( scatter_clr[i] ) );
				p->setPen( scatter_clr[i] );
				p->drawPolygon( ct );
				break;
			}
		}
    }


    // overlay with a value and an arrow (e.g., total daily change)
#ifdef THUMB_VALS
    /* put thmbl and thumbval over vol and plot lines */
    if( thumbnail ) {
		int     n, d, w;
		char	thmbl[32];
		char	*price_to_str( float, int*, int*, int* );
		char	nmrtr[3+1], dmntr[3+1], whole[8];
	
		char	*dbg = price_to_str( ABS(thumbval),&n,&d,&w );
		sprintf( nmrtr, "%d", n );
		sprintf( dmntr, "%d", d );
		sprintf( whole, "%d", w );
	
		p->setPen( ThumbLblColor );
		p->setFont( gdFontSmall );
		p->drawText( graphwidth/2-strlen(thumblabel)*SFONTWDTH/2,
					 1,
					 thumblabel );
		if( w || n ) {
			int		chgcolor  = thumbval>0.0? ThumbUColor: ThumbDColor;
			int		thmbvalwidth = SFONTWDTH +	// up/down arrow
				(w?strlen(whole)*SFONTWDTH: 0) +	// whole
				(n?strlen(nmrtr)*TFONTWDTH	  +	// numerator
				 SFONTWDTH					  +	// /
				 strlen(dmntr)*TFONTWDTH:		// denominator
				 0);							// no frac part
	
			smallarrow( p, graphwidth/2-thmbvalwidth/2, SFONTHGT, thumbval>0.0, chgcolor );
			if( w ) {
				p->setFont( gdFontSmall );
				p->setPen( chgcolor );
				p->drawText( (graphwidth/2-thmbvalwidth/2)+SFONTWDTH,
							 SFONTHGT+2,
							 whole );
			}
			if( n ) {
				p->setFont( gdFontTiny );
				p->setPen( chgcolor );
				p->drawText( (graphwidth/2-thmbvalwidth/2)   +	// start
							 SFONTWDTH					   +	// arrow
							 (w? strlen(whole)*SFONTWDTH: 0) +	// whole
							 2,
							 SFONTHGT+2-2,
							 nmrtr );
				p->setFont( gdFontSmall );
				p->drawText( (graphwidth/2-thmbvalwidth/2)  +		// start
							 SFONTWDTH					  +		// arrow
							 (w? strlen(whole)*SFONTWDTH: 0) +	// whole
							 strlen(nmrtr)*TFONTWDTH,				// numerator
							 SFONTHGT+2,
							 '/' );
				p->setFont( gdFontTiny );
				p->drawText( (graphwidth/2-thmbvalwidth/2)  +		// start
							 SFONTWDTH					  +		// arrow
							 (w? strlen(whole)*SFONTWDTH: 0) +		// whole
							 strlen(nmrtr)*TFONTWDTH		  +		// numerator
							 SFONTWDTH - 3,						// /
							 SFONTHGT+2+4,
							 dmntr );
			}
		}
    }		// thumblabel, thumbval
#endif

    /* box it off */
    /*  after plotting so the outline covers any plot lines */
    if( params->border ) {
		p->setPen( LineColor );
		p->drawLine( PX(0), PY(lowest), PX(num_points-1+(do_bar?2:0)), PY(lowest) );

		setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1;
		p->drawLine( PX(0), PY(highest), PX(num_points-1+(do_bar?2:0)),  PY(highest) );
		setno = 0;
    }
    if( params->border ) {
		int	x1, y1, x2, y2;
	
		x1 = PX(0);
		y1 = PY(highest);
		x2 = PX(num_points-1+(do_bar?2:0));
		y2 = PY(lowest);
		p->setPen( LineColor );
		p->drawLine( x1, PY(lowest), x1, y1 );
	
		setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1;
		p->setPen( LineColor );
		p->drawLine( x1, y1, PX(0), PY(highest) );
		// if( !params->grid || do_vol || params->thumbnail )					// grid leaves right side Y open
		{
			p->setPen( LineColor );
			p->drawLine( x2, y2, PX(num_points-1+(do_bar?2:0)), PY(lowest) );
			p->drawLine( PX(num_points-1+(do_bar?2:0)), PY(lowest),
						 PX(num_points-1+(do_bar?2:0)), PY(highest) );
		}
		setno = 0;
    }

    if( params->shelf && threeD &&								/* front of 0 shelf */
		( (lowest < 0.0 && highest > 0.0) ||
		  (lowest < 0.0 && highest > 0.0) ) ) {
		int	x2 = PX( num_points-1+(do_bar?2:0) ),
			y2 = PY( 0 );
	
		p->setPen( LineColor );
		p->drawLine( PX(0), PY(0), x2, y2 );		// front line
		setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets:
			1;				// backmost
		// depth for 3Ds
		p->setPen( LineColor );
		p->drawLine( x2, y2, PX(num_points-1+(do_bar?2:0)), PY(0) );
		setno = 0;												// set back to foremost
    }

    if( params->annotation ) {			/* front half of annotation line */
		int		x1 = PX(params->annotation->point+(do_bar?1:0)),
			y1 = PY(highest);
		int		x2;
		// front line
		p->setPen( AnnoteColor );
		p->drawLine( x1, PY(lowest)+1, x1, y1 );
		if( threeD ) { // on back plane
			setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1;
			x2 = PX(params->annotation->point+(do_bar?1:0));
			// prspective line
			p->setPen( AnnoteColor );
			p->drawLine( x1, y1, x2, PY(highest) );
		} else { // for 3D done with back line
			x2 = PX(params->annotation->point+(do_bar?1:0));
			p->setPen( AnnoteColor );
			p->drawLine( x1, y1, x1, y1-2 );
		}
		/* line-to and note */
		if( *(params->annotation->note) ) {						// any note?
			if( params->annotation->point >= (num_points/2) ) {		/* note to the left */
				p->setPen( AnnoteColor );
				p->drawLine( x2,              PY(highest)-2,
							 x2-annote_hgt/2, PY(highest)-2-annote_hgt/2 );
				// PENDING(kalle) Check whether this really does line breaks
				p->setFont( params->titleFont() );
				QRect br = QFontMetrics( params->titleFont() ).boundingRect( 0, 0, MAXINT,
																					  MAXINT,
																					  Qt::AlignRight,
																					  params->title );
				p->drawText(   x2-annote_hgt/2-1-annote_len - 1,
							   PY(highest)-annote_hgt+1,
							   br.width(), br.height(),
							   Qt::AlignRight, params->annotation->note );
			} else { /* note to right */
				p->setPen( AnnoteColor );
				p->drawLine( x2, PY(highest)-2,
							 x2+annote_hgt/2, PY(highest)-2-annote_hgt/2 );
				// PENDING(kalle) Check whether this really does line breaks
				p->setFont( params->annotationFont() );
				QRect br = QFontMetrics( params->annotationFont() ).boundingRect( 0, 0,
																						   MAXINT,
																						   MAXINT,
																						   Qt::AlignLeft,
																						   params->title );
				p->drawText( x2+annote_hgt/2+1 + 1,
							 PY(highest)-annote_hgt+1,
							 br.width(), br.height(),
							 Qt::AlignLeft, params->annotation->note );
			}
		}
		setno = 0;
    }
}



/* rem circle:  x = rcos(@), y = rsin(@)	*/


inline static double func_px( int cx, int x, double rad, double pscl )
{
    return( cx + (int)( ((float)rad)*sin(pscl*(double)(x)) ) );
}


inline static double func_py( int cy, int x, double rad, double pscl )
{
    return( cy - (int)( ((float)rad)*cos(pscl*(double)(x)) ) );
}


#define CX( i, d ) ( cx + ((d)? xdepth_3D: 0) + \
            (int)( (double)(GDCPIE_explode?GDCPIE_explode[((i))]:0) * \
                   sin((double)(slice_angle[0][(i)])) ) )

#define CY( i, d ) ( cy - ((d)? ydepth_3D: 0) - \
            (int)( (double)(GDCPIE_explode?GDCPIE_explode[(i)]:0) * \
                   cos((double)(slice_angle[0][(i)])) ) )



/* expect slice number:     i (index into slice_angle array) *\
 *   and position on slice: f (0: slice middle,              *
 *                             1: leading (clockwise),       *
 *                             2: trailing edge)             *
 *   and 3D depth:          d (0: do depth,                  *
 *                             1: no depth adjustment)       *
\* adjusts for explosion                                     */
#define IX( i, f, d ) ( CX( (i), (d) ) + (int)( (double)rad * sin((double)(slice_angle[f][(i)])) ) )

#define IY( i, f, d ) ( CY( (i), (d) ) - (int)( (double)rad * cos((double)(slice_angle[f][(i)])) ) )


/* same as above except o is angle */
#define OX( i, o, d ) ( CX( (i),(d)) + (int)( (double)rad * sin((double)((o))) ) )

#define OY( i, o, d ) ( CY( i,d) - (int)( (double)rad * cos((double)(o)) ) )

#define TO_INT_DEG( o ) (int)rint( (double)((o)/(2.0*M_PI)*360.0) )

#define TO_INT_DEG_FLOOR( o ) (int)floor( (double)((o)/(2.0*M_PI)*360.0) )

#define TO_INT_DEG_CEIL( o ) (int)ceil( (double)((o)/(2.0*M_PI)*360.0) )

#define TO_RAD( o ) ( (o)/360.0*(2.0*M_PI) )

/* assume !> 4*PI */
#define MOD_2PI( o ) ( (o)>=(2.0*M_PI)? ((o)-(2.0*M_PI)): (((o)<0)? ((o)+(2.0*M_PI)): (o)) )

#define MOD_360( o ) ( (o)>=360? (o)-360: (o) )								/* assume !> 720 */


struct tmp_slice_t { int	i;					// original index
    char	hidden;				// 'behind' top [3D] pie
    float	angle;				// radian
    float	slice; };			// radian
static float				pie_3D_rad;			// user requested 3D angle in radians

// degrees (radians) between angle a, and depth angle
// 1&2, so comparisons can be done.
#define RAD_DIST1( a ) ( (dist_foo1=ABS(((a>-.00001&&a<.00001)?0.00001:a)-pie_3D_rad)), ((dist_foo1>M_PI)? ABS(dist_foo1-2.0*M_PI): dist_foo1) )

#define RAD_DIST2( a ) ( (dist_foo2=ABS(((a>-.00001&&a<.00001)?0.00001:a)-pie_3D_rad)), ((dist_foo2>M_PI)? ABS(dist_foo2-2.0*M_PI): dist_foo2) )


static float				dist_foo1, dist_foo2;

/* ------------------------------------------------------- *\
 * oof!  cleaner way???
 * order by angle opposite (180) of depth angle
 * comparing across 0-360 line
\* ------------------------------------------------------- */
static int ocmpr( const void *p1, const void *p2 )
{
    const tmp_slice_t *a=(tmp_slice_t *) p1;
    const tmp_slice_t *b=(tmp_slice_t *) p2;
    if( RAD_DIST1(a->angle) < RAD_DIST2(b->angle) )
        return 1;
    if( RAD_DIST1(a->angle) > RAD_DIST2(b->angle) )
        return -1;

    /* a tie (will happen between each slice) */
    /* are we within pie_3D_rad */
    if( (a->angle < pie_3D_rad) && (pie_3D_rad < a->slice) ||
        (a->slice < pie_3D_rad) && (pie_3D_rad < a->angle) )
        return 1;
    if( (b->slice < pie_3D_rad) && (pie_3D_rad < b->angle) ||
        (b->angle < pie_3D_rad) && (pie_3D_rad < b->slice) )
        return -1;

    /* let slice angle decide */
    if( RAD_DIST1(a->slice) < RAD_DIST2(b->slice) )
        return 1;
    if( RAD_DIST1(a->slice) > RAD_DIST2(b->slice) )
        return -1;

    return 0;
}

/* ======================================================= *\
 * PIE
 *
 * Notes:
 *  always drawn from 12:00 position clockwise
 *  'missing' slices don't get labels
 *  sum(val[0], ... val[num_points-1]) is assumed to be 100%
\* ======================================================= */
void
pie_gif( short			imagewidth,
         short			imageheight,
		 QPainter*      p,					// paint here
         KChartParameters*	params,				// all the parameters
         int			num_points,
         char			*lbl[],				/* data labels */
         float			val[] )				/* data */
{
    int			i;

    QColor BGColor, LineColor, PlotColor, EdgeColor, EdgeColorShd,
        SliceColor[num_points], SliceColorShd[num_points];

    float		rad = 0.0;					// radius
    float		tot_val = 0.0;
    float		pscl;
    int			cx,							// affects func_px()
        cy;							// affects func_py()
    /* ~ 1% for a size of 100 pixs */
    /* label sizes will more dictate this */
    float		min_grphable = ( params->other_threshold < 0?
								 100.0/(float)MIN(imagewidth,imageheight):
								 (float)params->other_threshold )/100.0;
    short		num_slices1 = 0,
        num_slices2 = 0;
    char		any_too_small = FALSE;
    char		others[num_points];
    float		slice_angle[3][num_points];	// must be used with others[]
    char		threeD = ( params->type == KCHARTTYPE_3DPIE );

    int			xdepth_3D      = 0,			// affects func_px()
        ydepth_3D      = 0;			// affects func_py()
    int			do3Dx = 0,					// reserved for macro use
        do3Dy = 0;

    //	GDCPIE_3d_angle = MOD_360(90-params->_3d_angle+360);
    pie_3D_rad = TO_RAD( params->_3d_angle );

    xdepth_3D      = threeD? (int)( cos((double)MOD_2PI(M_PI_2-pie_3D_rad+2.0*M_PI)) * params->_3d_depth ): 0;
    ydepth_3D      = threeD? (int)( sin((double)MOD_2PI(M_PI_2-pie_3D_rad+2.0*M_PI)) * params->_3d_depth ): 0;
    //	xdepth_3D      = threeD? (int)( cos(pie_3D_rad) * params->_3d_depth ): 0;
    //	ydepth_3D      = threeD? (int)( sin(pie_3D_rad) * params->_3d_depth ): 0;

	/* ----- get total value ----- */
    for( i=0; i<num_points; ++i )
        tot_val += val[i];

	/* ----- pie sizing ----- */
	/* ----- make width room for labels, depth, etc.: ----- */
	/* ----- determine pie's radius ----- */
    {
        int		title_hgt  = !params->title.isEmpty() ? 1			/*  title? horizontal text line */
            + params->titleFontHeight()
            * (int)cnt_nl( params->title.latin1(), (int*)NULL )
            + 2:
            0;
        float	last = 0.0;
        float	label_explode_limit = 0.0;
        int		cheight,
            cwidth;

        // maximum: no labels, explosions
        // gotta start somewhere
        rad = (float)MIN( imagewidth/2-(1+ABS(xdepth_3D)), imageheight/2-(1+ABS(ydepth_3D))-title_hgt );

        /* ok fix center, i.e., no floating re labels, explosion, etc. */
        cx = imagewidth/2 /* - xdepth_3D */ ;
        cy = (imageheight-title_hgt)/2 + title_hgt /* + ydepth_3D */ ;

        cheight = (imageheight- title_hgt)/2 /* - ydepth_3D */ ;
        cwidth  = cx;

        /* walk around pie. determine spacing to edge */
        for( i=0; i<num_points; ++i ) {
            float	this_pct = val[i]/tot_val;	/* should never be > 100% */
            float	that = this_pct*(2.0*M_PI);	/* pie-portion */
            if( (this_pct > min_grphable) ||	/* too small */
                (!GDCPIE_missing || !GDCPIE_missing[i]) ) {	/* still want angles */
                int this_explode = GDCPIE_explode? GDCPIE_explode[i]: 0;
                double	this_sin;
                double	this_cos;
                slice_angle[0][i] = that/2.0+last; /* mid-point on full pie */
                slice_angle[1][i] = last;		   /* 1st on full pie */
                slice_angle[2][i] = that+last;	   /* 2nd on full pie */
                this_sin        = sin( (double)slice_angle[0][i] );
                this_cos        = cos( (double)slice_angle[0][i] );

                if( !GDCPIE_missing || !(GDCPIE_missing[i]) ) {
                    short	lbl_wdth,
                        lbl_hgt;
                    float	this_y_explode_limit,
                        this_x_explode_limit;

                    /* start slice label height, width     */
                    /*  accounting for PCT placement, font */
                    if( lbl && lbl[i] )	{
                        char	foo[1+4+1+1];					/* XPG2 compatibility */
                        int		pct_len;
                        int		lbl_len = 0;
                        lbl_hgt = ( cnt_nl(lbl[i], &lbl_len) + (params->percent_labels == KCHARTPCTTYPE_ABOVE ||
																params->percent_labels == KCHARTPCTTYPE_BELOW? 1: 0) )
                            * (params->labelFontHeight()+1);
                        sprintf( foo,
                                 (params->percent_labels==KCHARTPCTTYPE_LEFT ||
                                  params->percent_labels==KCHARTPCTTYPE_RIGHT) &&
                                 lbl[i]? "(%.0f%%)":
                                 "%.0f%%",
                                 this_pct * 100.0 );
                        pct_len = params->percent_labels == KCHARTPCTTYPE_NONE? 0: strlen(foo);
                        lbl_wdth = ( params->percent_labels == KCHARTPCTTYPE_RIGHT ||
                                     params->percent_labels == KCHARTPCTTYPE_LEFT? lbl_len+1+pct_len:
                                     MAX(lbl_len,pct_len) )
                            * params->labelFontWidth();
                    } else
                        lbl_wdth = lbl_hgt = 0;
                    /* end label height, width */
				
                    /* diamiter limited by this piont's: explosion, label                 */
                    /* (radius to box @ slice_angle) - (explode) - (projected label size) */
                    /* radius constraint due to labels */
                    this_y_explode_limit = (float)this_cos==0.0? MAXFLOAT:
                        (	(float)( (double)cheight/ABS(this_cos) ) -
							(float)( this_explode + (lbl[i]? GDCPIE_label_dist: 0) ) -
							(float)( lbl_hgt/2 ) / (float)ABS(this_cos)	);
                    this_x_explode_limit = (float)this_sin==0.0? MAXFLOAT:
                        (	(float)( (double)cwidth/ABS(this_sin) ) -
							(float)( this_explode + (lbl[i]? GDCPIE_label_dist: 0) ) -
							(float)( lbl_wdth ) / (float)ABS(this_sin)	);
				
                    rad = MIN( rad, this_y_explode_limit );
                    rad = MIN( rad, this_x_explode_limit );

                    // ok at this radius (which is most likely larger than final)
                    // adjust for inter-label spacing
                    //				if( lbl[i] && *lbl[i] )
                    //					{
                    //					char which_edge = slice_angle[0][i] > M_PI? +1: -1;		// which semi
                    //					last_label_yedge = cheight - (int)( (rad +				// top or bottom of label
                    //														(float)(this_explode +
                    //														(float)GDCPIE_label_dist)) * (float)this_cos ) +
                    //											     ( (params->labelFontHeight()+1)/2 +
                    //													GDC_label_spacing )*which_edge;
                    //					}

                    /* radius constriant due to exploded depth */
                    /* at each edge of the slice, and the middle */
                    /* this is really stupid */
                    /*  this section uses a different algorithm then above, but does the same thing */
                    /*  could be combined, but each is ugly enough! */
                    // PROTECT /0
                    if( threeD ) {
                        short	j;
                        int		this_y_explode_pos;
                        int		this_x_explode_pos;

                        // first N E S W (actually no need for N)
                        if( (slice_angle[1][i] < M_PI_2 && M_PI_2 < slice_angle[2][i]) &&				// E
                            (this_x_explode_pos=OX(i,M_PI_2,1)) > cx+cwidth )
                            rad -= (float)ABS( (double)(1+this_x_explode_pos-(cx+cwidth))/sin(M_PI_2) );
                        if( (slice_angle[1][i] < 3.0*M_PI_2 && 3.0*M_PI_2 < slice_angle[2][i]) &&		// W
                            (this_x_explode_pos=OX(i,3.0*M_PI_2,1)) < cx-cwidth )
                            rad -= (float)ABS( (double)(this_x_explode_pos-(cx+cwidth))/sin(3.0*M_PI_2) );
                        if( (slice_angle[1][i] < M_PI && M_PI < slice_angle[2][i]) &&					// S
                            (this_y_explode_pos=OY(i,M_PI,1)) > cy+cheight )
                            rad -= (float)ABS( (double)(1+this_y_explode_pos-(cy+cheight))/cos(M_PI) );

                        for( j=0; j<3; ++j ) {
                            this_y_explode_pos = IY(i,j,1);
                            if( this_y_explode_pos < cy-cheight )
                                rad -= (float)ABS( (double)((cy-cheight)-this_y_explode_pos)/cos((double)slice_angle[j][i]) );
                            if( this_y_explode_pos > cy+cheight )
                                rad -= (float)ABS( (double)(1+this_y_explode_pos-(cy+cheight))/cos((double)slice_angle[j][i]) );

                            this_x_explode_pos = IX(i,j,1);
                            if( this_x_explode_pos < cx-cwidth )
                                rad -= (float)ABS( (double)((cx-cwidth)-this_x_explode_pos)/sin((double)slice_angle[j][i]) );
                            if( this_x_explode_pos > cx+cwidth )
                                rad -= (float)ABS( (double)(1+this_x_explode_pos-(cx+cwidth))/sin((double)slice_angle[j][i]) );
                        }
                    }
                }
                others[i] = FALSE;
            } else {
                others[i] = TRUE;
                slice_angle[0][i] = -MAXFLOAT;
            }
            last += that;
        }
    }

    /* --- allocate the requested colors --- */
    BGColor   = params->BGColor;
    LineColor = params->LineColor;
    PlotColor = params->PlotColor;
    if( params->EdgeColor != QColor() ) {
        EdgeColor = params->EdgeColor;
        if( threeD )
            // PENDING(kalle) This can probably be done more easily
            // with the hsv color model
            EdgeColorShd = QColor( params->EdgeColor.red() / 2,
                                   params->EdgeColor.green() / 2,
                                   params->EdgeColor.blue() / 2 );
    }

    /* --- set color for each slice --- */
    for( i=0; i<num_points; ++i )
        if( params->SetColor.count() ) {
            QColor slc_clr = params->SetColor.color( i );
            SliceColor[i]     = slc_clr;
            if( threeD )
                SliceColorShd[i] = QColor( slc_clr.red() / 2,
                                           slc_clr.green() / 2,
                                           slc_clr.blue() / 2 );
        } else {
            SliceColor[i]     = PlotColor;
            if( threeD )
                SliceColorShd[i] = QColor( params->PlotColor.red() / 2,
                                           params->PlotColor.green() / 2,
                                           params->PlotColor.blue() / 2 );
        }

    pscl = (2.0*M_PI)/tot_val;
	
    /* ----- calc: smallest a slice can be ----- */
    /* 1/2 circum / num slices per side. */
    /*              determined by number of labels that'll fit (height) */
    /* scale to user values */
    /* ( M_PI / (imageheight / (SFONTHGT+1)) ) */
    //	min_grphable = tot_val /
    //				   ( 2.0 * (float)imageheight / (float)(SFONTHGT+1+TFONTHGT+2) );


    if( threeD ) {
        /* draw background shaded pie */
        {
            float	rad1 = rad;
            for( i=0; i<num_points; ++i )
                if( !(others[i]) &&
                    (!GDCPIE_missing || !GDCPIE_missing[i]) ) {
                    float	rad = rad1;
                    p->setPen( SliceColorShd[i] );
                    p->drawLine( CX(i,1),
								 CY(i,1),
                                 IX(i,1,1), IY(i,1,1) );
                    p->drawLine( CX(i,1),
								 CY(i,1),
                                 IX(i,2,1), IY(i,2,1) );
					
                    // original parameters:
                    // center: CX(i,1), CY(i,1)
                    // width, height: rad*2, rad*2
                    // start: TO_INT_DEG_FLOOR(slice_angle[1][i])+270)
                    // end: TO_INT_DEG_CEIL(slice_angle[2][i])+270
                    // PENDING(kalle) Qt arcs start at 3 o'clock, where do gd arcs start?
                    // Differences in arc handling: Qt uses x,y,w,h,
                    // gd uses cy,cy,w,h; Qt specifies start angle and
                    // arc length, gd specifies start angle and eng
                    // angle; Qt angles are specified in 16/th
                    // degrees, gd angles in full degrees

                    // Original - gd:
                    // 					gdImageArc( im, CX(i,1), CY(i,1),
                    // 								rad*2, rad*2,
                    // 								TO_INT_DEG_FLOOR(slice_angle[1][i])+270,
                    // 								TO_INT_DEG_CEIL(slice_angle[2][i])+270,
                    // 								SliceColorShd[i] );

                    // New - Qt:
                    p->setPen( SliceColorShd[i] );
                    p->drawArc( CX(i,1)-(rad*2/2), // x
                                CY(i,1)-(rad*2/2), // y
                                rad*2, rad*2,           // w, h
                                (TO_INT_DEG_FLOOR(slice_angle[1][i])+270)*16,
                                ((TO_INT_DEG_CEIL(slice_angle[2][i])+270)-
                                 (TO_INT_DEG_FLOOR(slice_angle[1][i])+270))*16 );
								
                    rad1 = rad;
                    rad *= 3.0/4.0;
                    // PENDING(kalle) Can it really be that Qt has no
                    // flood fill??? If necessary, implement own
                    // algorithm here.
                    //					gdImageFillToBorder( im, IX(i,0,1), IY(i,0,1), SliceColorShd[i], SliceColorShd[i] );
                    rad = rad1;
                    if( params->EdgeColor != QColor() ) {
                        p->setPen( EdgeColorShd );
                        p->drawLine( CX(i,1),
									 CY(i,1),
									 IX(i,1,1), IY(i,1,1) );
                        p->drawLine( CX(i,1),
									 CY(i,1),
									 IX(i,2,1), IY(i,2,1) );
                        // For differences between gd arc handling and
                        // Qt arc handling, please see first
                        // QPainter::drawArc() call in this file.

                        // New: Qt
                        p->drawArc( CX(i,1)-rad, // x
                                    CY(i,1)-rad, // y
                                    rad*2, rad*2,     // w, h
                                    (TO_INT_DEG(slice_angle[1][i])+270)*16,
                                    ((TO_INT_DEG(slice_angle[2][i])+270)-
                                     (TO_INT_DEG(slice_angle[1][i])+270))*16 );
                        // Original: gd
                        // 						gdImageArc( im, CX(i,1), CY(i,1),
                        // 									rad*2, rad*2,
                        // 									TO_INT_DEG(slice_angle[1][i])+270, TO_INT_DEG(slice_angle[2][i])+270,
                        // 									EdgeColorShd);
                    }
                }
        }
        /* fill in connection to foreground pie */
        /* this is where we earn our keep */
        {
            struct tmp_slice_t	tmp_slice[2*num_points+2];
            int					t,
                num_slice_angles = 0;

            for( i=0; i<num_points; ++i )
                if( !GDCPIE_missing || !GDCPIE_missing[i] )	{
                    if( RAD_DIST1(slice_angle[1][i]) < RAD_DIST2(slice_angle[0][i]) )
                        tmp_slice[num_slice_angles].hidden = FALSE;
                    else
                        tmp_slice[num_slice_angles].hidden = TRUE;
                    tmp_slice[num_slice_angles].i       = i;
                    tmp_slice[num_slice_angles].slice   = slice_angle[0][i];
                    tmp_slice[num_slice_angles++].angle = slice_angle[1][i];
                    if( RAD_DIST1(slice_angle[2][i]) < RAD_DIST2(slice_angle[0][i]) )
                        tmp_slice[num_slice_angles].hidden = FALSE;
                    else
                        tmp_slice[num_slice_angles].hidden = TRUE;
                    tmp_slice[num_slice_angles].i       = i;
                    tmp_slice[num_slice_angles].slice   = slice_angle[0][i];
                    tmp_slice[num_slice_angles++].angle = slice_angle[2][i];
                    // identify which 2 slices (i) have a tangent parallel to depth angle
                    if( slice_angle[1][i]<MOD_2PI(pie_3D_rad+M_PI_2) && slice_angle[2][i]>MOD_2PI(pie_3D_rad+M_PI_2) )	{
                        tmp_slice[num_slice_angles].i       = i;
                        tmp_slice[num_slice_angles].hidden  = FALSE;
                        tmp_slice[num_slice_angles].slice   = slice_angle[0][i];
                        tmp_slice[num_slice_angles++].angle = MOD_2PI( pie_3D_rad+M_PI_2 );
                    }
                    if( slice_angle[1][i]<MOD_2PI(pie_3D_rad+3.0*M_PI_2) && slice_angle[2][i]>MOD_2PI(pie_3D_rad+3.0*M_PI_2) )	{
                        tmp_slice[num_slice_angles].i       = i;
                        tmp_slice[num_slice_angles].hidden  = FALSE;
                        tmp_slice[num_slice_angles].slice   = slice_angle[0][i];
                        tmp_slice[num_slice_angles++].angle = MOD_2PI( pie_3D_rad+3.0*M_PI_2 );
                    }
                }
			
            qsort( tmp_slice, num_slice_angles, sizeof(struct tmp_slice_t), ocmpr );
            for( t=0; t<num_slice_angles; ++t )	{
                QPointArray	gdp( 4 );
				
                i = tmp_slice[t].i;
				
                gdp.setPoint( 0, CX(i,0), CY(i,0) );
                gdp.setPoint( 1, CX(i,1), CY(i,1) );
                gdp.setPoint( 2, OX(i,tmp_slice[t].angle,1),
                              OY(i,tmp_slice[t].angle,1) );
				gdp.setPoint( 3, OX(i,tmp_slice[t].angle,0),
							  OY(i,tmp_slice[t].angle,0) );
							
				if( !(tmp_slice[t].hidden) ) {
					p->setBrush( SliceColorShd[i] );
					p->setPen( SliceColorShd[i] );
					p->drawPolygon( gdp );
				} else {
					rad -= 2.0;										/* no peeking */
					gdp.setPoint( 0, OX(i,slice_angle[0][i],0),
								  OY(i,slice_angle[0][i],0) );
					gdp.setPoint( 1, OX(i,slice_angle[0][i],1),
								  OY(i,slice_angle[0][i],1) );
					rad += 2.0;
					gdp.setPoint( 2, OX(i,slice_angle[1][i],1),
								  OY(i,slice_angle[1][i],1) );
					gdp.setPoint( 3, OX(i,slice_angle[1][i],0),
								  OY(i,slice_angle[1][i],0) );
					p->setBrush( SliceColorShd[i] );
					p->setPen( SliceColorShd[i] );
					p->drawPolygon( gdp );
					gdp.setPoint( 2, OX(i,slice_angle[2][i],1),
								  OY(i,slice_angle[2][i],1) );
					gdp.setPoint( 3, OX(i,slice_angle[2][i],0),
								  OY(i,slice_angle[2][i],0) );
					p->setBrush( SliceColorShd[i] );
					p->setPen( SliceColorShd[i] );
					p->drawPolygon( gdp );
				}
				
				
				if( params->EdgeColor != QColor() ) {
					p->setPen( EdgeColorShd );
					p->drawLine( CX(i,0),
								 CY(i,0),
								 CX(i,1),
								 CY(i,1) );
					p->drawLine( OX(i,tmp_slice[t].angle,0),
								 OY(i,tmp_slice[t].angle,0),
								 OX(i,tmp_slice[t].angle,1),
								 OY(i,tmp_slice[t].angle,1) );
				}
			}
		}
	}
	
	
	/* ----- pie face ----- */
	{
		// float	last = 0.0;
		float	rad1 = rad;
		for( i=0; i<num_points; ++i )
			if( !others[i] &&
				(!GDCPIE_missing || !GDCPIE_missing[i]) ) {
				float	rad = rad1;

				// last += val[i];
				// EXPLODE_CX_CY( slice_angle[0][i], i );
				p->setPen( SliceColor[i] );
				p->drawLine( CX(i,0),
							 CY(i,0),
							 IX(i,1,0), IY(i,1,0) );
				p->drawLine( CX(i,0),
							 CY(i,0),
							 IX(i,2,0), IY(i,2,0) );
				
				// For the differences between Qt arc handling and gd
				// arc handling, please see first call to
				// QPainter::drawArc() in this file.

				// New - Qt:
				p->drawArc( CX(i,0)-rad, // x
							CY(i,0)-rad, // y
							rad*2, rad*2,     // w, h
							(TO_INT_DEG_FLOOR(slice_angle[1][i])+270)*16,
							((TO_INT_DEG_CEIL(slice_angle[2][i])+270)-
							 (TO_INT_DEG_FLOOR(slice_angle[1][i])+270))*16 );

				// Original - gd:
				// 				gdImageArc( im, CX(i,0), CY(i,0),
				// 							(int)rad*2, (int)rad*2,
				// 							TO_INT_DEG_FLOOR(slice_angle[1][i])+270,
				// 							TO_INT_DEG_CEIL(slice_angle[2][i])+270,
				// 							SliceColor[i] );
				rad1 = rad;
				rad *= 3.0/4.0;
				// PENDING(kalle) Can it really be that Qt has no
				// flood fill?
				//				gdImageFillToBorder( im, IX(i,0,0), IY(i,0,0), SliceColor[i], SliceColor[i] );
				/* catch missed pixels on narrow slices */
				p->setPen( SliceColor[i] );
				p->drawLine( CX(i,0),
							 CY(i,0),
							 IX(i,0,0), IY(i,0,0) );
				rad = rad1;
				if( params->EdgeColor != QColor() ) {
					p->setPen( EdgeColor );
					p->drawLine( CX(i,0),
								 CY(i,0),
								 IX(i,1,0), IY(i,1,0) );
					p->drawLine( CX(i,0),
								 CY(i,0),
								 IX(i,2,0), IY(i,2,0) );

					// For the differences between Qt arc handling and
					// gd arc handling, please see the first call to
					// QPainter::drawArc() in this file.

					// New - Qt:
					p->drawArc( CX(i,0)-rad,   // x
								CY(i,0)-rad,   // y
								rad*2, rad*2,       // w, h
								(TO_INT_DEG(slice_angle[1][i])+270)*16,
								((TO_INT_DEG(slice_angle[2][i])+270)-
								 (TO_INT_DEG(slice_angle[1][i])+270))*16 );

					// Original - gd:
					// 					gdImageArc( im, CX(i,0), CY(i,0),
					// 								rad*2, rad*2,
					// 								TO_INT_DEG(slice_angle[1][i])+270, TO_INT_DEG(slice_angle[2][i])+270,
					// 								EdgeColor );
				}
			}
	}
	
	if( !params->title.isEmpty() ) {
		int	title_len;
		
		cnt_nl( params->title.latin1(), &title_len );
		// PENDING(kalle) Check whether this really does line breaks
		QRect br = QFontMetrics( params->titleFont() ).boundingRect( 0, 0, MAXINT,
																				 MAXINT,
																				 Qt::AlignCenter,
																				 params->title );
		p->drawText( (imagewidth-title_len*params->titleFontWidth())/2,
					 1, // y
					 br.width(), br.height(),
					 Qt::AlignCenter, params->title );
				
		// Original
		//                 GDCImageStringNL( im,
		//                                   params->titleFont(),
		//                                   (imagewidth-title_len*params->titleFontWidth())/2,
		//                                   1,
		//                                   params->title,
		//                                   LineColor,
		//                                   KCHARTJUSTIFYCENTER );
	}
	
	/* labels */
	if( lbl ) {
		float	liner = rad;

		rad += GDCPIE_label_dist;
		for( i=0; i<num_points; ++i ) {
			if( !others[i] &&
				(!GDCPIE_missing || !GDCPIE_missing[i]) ) {
				char	pct_str[1+4+1+1];
				int		pct_wdth;
				int		lbl_wdth;
				short	num_nl = cnt_nl( lbl[i], &lbl_wdth );
				int		lblx,  pctx,
					lbly,  pcty,
					linex, liney;
				
				lbl_wdth *= params->labelFontWidth();
				sprintf( pct_str,
						 (params->percent_labels==KCHARTPCTTYPE_LEFT ||
						  params->percent_labels==KCHARTPCTTYPE_RIGHT) &&
						 lbl[i]? "(%.0f%%)":
						 "%.0f%%",
						 (val[i]/tot_val) * 100.0 );
				pct_wdth = params->percent_labels == KCHARTPCTTYPE_NONE?
					0:
					strlen(pct_str) * params->labelFontWidth();
				
				lbly = (liney = IY(i,0,0))-( num_nl * (1+params->labelFontHeight()) ) / 2;
				lblx = pctx = linex = IX(i,0,0);
				
				if( slice_angle[0][i] > M_PI ) {								/* which semicircle */
					lblx -= lbl_wdth;
					pctx = lblx;
					++linex;
				} else
					--linex;

				switch( params->percent_labels )	{
				case KCHARTPCTTYPE_LEFT:	
					if( slice_angle[0][i] > M_PI )
						pctx -= lbl_wdth-1;
					else
						lblx += pct_wdth+1;
					pcty = IY(i,0,0) - ( 1+params->labelFontHeight() ) / 2;
					break;
				case KCHARTPCTTYPE_RIGHT:	
					if( slice_angle[0][i] > M_PI )
						lblx -= pct_wdth-1;
					else
						pctx += lbl_wdth+1;
					pcty = IY(i,0,0) - ( 1+params->labelFontHeight() ) / 2;
					break;
				case KCHARTPCTTYPE_ABOVE:	
					lbly += (1+params->labelFontHeight()) / 2;
					pcty = lbly - (params->labelFontHeight());
					break;
				case KCHARTPCTTYPE_BELOW:	
					lbly -= (1+params->labelFontHeight()) / 2;
					pcty = lbly + (params->labelFontHeight()) * num_nl;
					break;
				case KCHARTPCTTYPE_NONE:
				default:
					;
				}
				
				if( params->percent_labels != KCHARTPCTTYPE_NONE ) {
					p->setPen( LineColor );
					p->setFont( params->labelFont() );
					p->drawText( slice_angle[0][i] <= M_PI? pctx:
								 pctx+lbl_wdth-pct_wdth,
								 pcty,
								 pct_str );
				}
				if( lbl[i] ) {
					QRect br = QFontMetrics( params->labelFont() ).boundingRect( 0, 0, MAXINT, MAXINT, slice_angle[0][i] <= M_PI ?
																							 Qt::AlignLeft : Qt::AlignRight, lbl[i] );
					p->drawText( lblx, lbly,
								 br.width(), br.height(),
								 slice_angle[0][i] <= M_PI ?
								 Qt::AlignLeft : Qt::AlignRight,
								 lbl[i] );
				}
				if( GDCPIE_label_line )	{
					float	rad = liner;
					p->setPen( LineColor );
					p->drawLine( linex, liney,
								 IX(i,0,0), IY(i,0,0) );
				}
			}
		}
		rad -= GDCPIE_label_dist;
	}
}



/******************************************************
 * Global variables holding parameter data (for now)  *
 *****************************************************/

/*** Common options ***/
GDC_HOLD_IMAGE_T	GDC_hold_img = GDC_DESTROY_IMAGE;
void				*GDC_image = (void*)NULL;	/* in/out */

/*** Stuff ***/
short GDC_xlabel_spacing	= 5;			/* pixels  MAXSHORT means force all */
char GDC_yval_style		= TRUE;
char *GDC_thumblabel;
float GDC_thumbval		= -MAXFLOAT;
char *GDC_BGImage;
char *GDCPIE_title = 0;		/* NLs ok here */
int	GDCPIE_label_dist = 1+8/2;		/* 1+GDC_fontc[GDCPIE_label_size].h/2 */
unsigned char GDCPIE_label_line = false;		/* from label to slice */
int	*GDCPIE_explode = 0;	/* [num_points] */
unsigned char *GDCPIE_missing = 0;	/* TRUE/FALSE */

/*** Colors ***/
QColor* GDC_BGColor;	 /* black */
QColor* GDC_GridColor;	 /* gray */
QColor* GDC_LineColor;
QColor* GDC_PlotColor;
QColor* GDC_VolColor;	 /* lgtblue1 */
QColor* GDC_TitleColor; /* "opposite" of BG */
QColor* GDC_XTitleColor;
QColor* GDC_YTitleColor;
QColor* GDC_YTitle2Color;
QColor* GDC_XLabelColor;
QColor* GDC_YLabelColor;
QColor* GDC_YLabel2Color;
/* supercedes VolColor	ulong_color[num_points] */
QColor *GDC_ExtVolColor( 0 );
/* supercedes LineColor	ulong_color[num_sets] */
QColor *GDC_SetColor( 0 );
/* supercedes SetColor	ulong_color[num_sets][num_points] */
QColor *GDC_ExtColor( 0 );
QColor* GDCPIE_BGColor;	/* black */
QColor* GDCPIE_PlotColor;	/* gray */
QColor* GDCPIE_LineColor;
QColor* GDCPIE_EdgeColor; /* edging on/off */
/* [num_points] supercedes GDCPIE_PlotColor */
QColor *GDCPIE_Color( 0 );



/********************************************************
 * The price conv stuff
 *******************************************************/
/*
*  - price as float to a string (ostensibly for printing)
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

/* ----------------------------------------------------------------- */
/* -- convert a float to a printable string, in form:             -- */
/* --	W N/D                                                     -- */
/* -- where W is whole, N is numerator, D is denominator          -- */
/* -- the frac N/D is one of 2nds, 4,8,16,32,64,128,256ths        -- */
/* -- if cannot convert, return str of the float                  -- */
/* ----------------------------------------------------------------- */

#define EPSILON		((1.0/256.0)/2.0)
#define GET_DEC(x)	( (x) - (float)(int)(x) )

char*
price_to_str( float	price,
			  int	*numorator,
			  int	*demoninator,
			  int	*decimal,
			  const char	*fltfmt )			// printf fmt'ing str
{
    static char rtn[64];
	int			whole = (int)price;
	float		dec   = GET_DEC( price ),
				numr;
	/* float		pow( double, double ); */

	// caller doesn't want fractions
	if( fltfmt )
		{
		sprintf( rtn, fltfmt, price );
		*numorator = *demoninator = *decimal = 0;
		return rtn;
		}

	numr = dec * 256;
	/* check if we have a perfect fration in 256ths */
	{	
		float	rdec = GET_DEC( numr );

		if( rdec < EPSILON )
			;							/* close enough to frac */
		else if( (1-rdec) < EPSILON )	/* just over but close enough */
			++numr;
		else							/* no frac match */
		{
			sprintf( rtn, "%f", price );
			*numorator = *demoninator = *decimal = 0;
			return rtn;
		}
	}

	/* now have numr 256ths */
	/* resolve down */
	if( numr != 0 )
		{
		int	cnt = 8;

		while( (float)(numr)/2.0 == (float)(int)(numr/2) )
			{
			numr /= 2;
			--cnt;
			}

		/* don't want both whole AND numerator to be - */
		if( whole<0 && numr<0.0 )
			numr = -numr;
		*numorator = (int)numr;
		*demoninator = (int)pow(2.0, (float)cnt);
		*decimal = whole;
		sprintf( rtn, "%d %d/%d", whole,
								  (int)numr,
								  *demoninator );
		}
	else
		{
		*numorator = *demoninator = 0;
		*decimal = whole;
		sprintf( rtn, "%d", whole );
		}

    return rtn;
}

