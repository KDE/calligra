/* GDCHART 0.94b  GDC_PIE.C  12 Nov 1998 */

/**
 * $Id$
 *
 * Ported to Qt instead of libgd by Kalle Dalheimer, kalle@kde.org
 */

#include <stdio.h>
#include <values.h>
#include <math.h>

#define GDC_INCL
#define GDC_LIB
#include "gdc.h"
#include "gdcpie.h"

#include <qpainter.h>

/* rem circle:  x = rcos(@), y = rsin(@)	*/

extern struct	GDC_FONT_T	GDC_fontc[];

inline static void setrect( QPointArray& gdp, int x1, int x2, int y1, int y2 )
{
    gdp.setPoint( 0, x1, y1 );
    gdp.setPoint( 1, x2, y1 );
    gdp.setPoint( 2, x2, y2 );
    gdp.setPoint( 3, x1, y2 );
}


inline static double func_px( int cx, int x, double rad, double pscl )
{
    return( cx + (int)( ((float)rad)*sin(pscl*(double)(x)) ) );
}


inline static double func_py( int cy, int x, double rad, double pscl )
{
    return( cy - (int)( ((float)rad)*cos(pscl*(double)(x)) ) );
}


inline static double func_cx( int cx, int i, int d, int xdepth_3D, int** slice_angle )
{
    return( cx + (d? xdepth_3D: 0) + 
            (int)( (double)(GDCPIE_explode?GDCPIE_explode[(i)]:0) * 
                   sin((double)(slice_angle[0][i])) ) );
}


inline static double func_cy( int cy, int i, int d, int ydepth_3D, int** slice_angle )
{
    return( cy - (d? ydepth_3D: 0) - 
            (int)( (double)(GDCPIE_explode?GDCPIE_explode[(i)]:0) * 
                   cos((double)(slice_angle[0][i])) ) );
}


/* expect slice number:     i (index into slice_angle array) *\ 
 *   and position on slice: f (0: slice middle,              *
 *                             1: leading (clockwise),       *
 *                             2: trailing edge)             *
 *   and 3D depth:          d (0: do depth,                  *
 *                             1: no depth adjustment)       *
\* adjusts for explosion                                     */
inline static double func_ix( int i, int f, int d )
{
    return( func_cx(cx, i, d, xdepth_3D, slice_angle) + (int)( (double)rad * sin((double)(slice_angle[f][i])) ) );
}
inline static double func_iy( int i, int f, int d )
{
    return( func_cy(i,d) - (int)( (double)rad * cos((double)(slice_angle[f][i])) ) );
}
/* same as above except o is angle */
inline static double func_ox( int i, int o, int d )
{
    return( func_cx(i,d) + (int)( (double)rad * sin((double)(o)) ) );
}
inline static double func_oy( int i, int o, int d )
{
    return( func_cy(i,d) - (int)( (double)rad * cos((double)(o)) ) );
}
inline static int func_to_int_deg( double o )
{
    return (int)rint( (double)((o)/(2.0*M_PI)*360.0) );
}
inline static int func_to_int_deg_floor( double o )
{
    return (int)floor( (double)((o)/(2.0*M_PI)*360.0) );
}
inline static int func_to_int_deg_ceil( double o )
{
    return (int)ceil( (double)((o)/(2.0*M_PI)*360.0) );
}
inline static double func_to_rad( double o )
{
    return ( (o)/360.0*(2.0*M_PI) );
}
/* assume !> 4*PI */
inline static double func_mod_2pi( double o )
{
    return ( (o)>=(2.0*M_PI)? ((o)-(2.0*M_PI)): (((o)<0)? ((o)+(2.0*M_PI)): (o)) );
}
inline static double func_mod_360( double o )
{
    return ( (o)>=360? (o)-360: (o) );								/* assume !> 720 */ 
}

struct tmp_slice_t { int	i;					// original index
    char	hidden;				// 'behind' top [3D] pie
    float	angle;				// radian 
    float	slice; };			// radian
static float				pie_3D_rad;			// user requested 3D angle in radians

// degrees (radians) between angle a, and depth angle
// 1&2, so comparisons can be done.
inline static double func_rad_dist1( double a )
{
    return ( (dist_foo1=ABS(((a>-.00001&&a<.00001)?0.00001:a)-pie_3D_rad)), ((dist_foo1>M_PI)? ABS(dist_foo1-2.0*M_PI): dist_foo1) );
}
inline static double func_rad_dist2( double a )
{
    return ( (dist_foo2=ABS(((a>-.00001&&a<.00001)?0.00001:a)-pie_3D_rad)), ((dist_foo2>M_PI)? ABS(dist_foo2-2.0*M_PI): dist_foo2) );
}

static float				dist_foo1, dist_foo2;

/* ------------------------------------------------------- *\ 
 * oof!  cleaner way???
 * order by angle opposite (180) of depth angle
 * comparing across 0-360 line
\* ------------------------------------------------------- */
static int ocmpr( struct tmp_slice_t *a, struct tmp_slice_t *b )
{
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
pie_gif( QPainter*      p,					// paint here
         short			imagewidth,
         short			imageheight,
         GDCPIE_TYPE	type,
         int			num_points,
         char			*lbl[],				/* data labels */
         float			val[] )				/* data */
{
    int			i;

    QColor BGColor,
        LineColor,
        PlotColor,
        EdgeColor,
        EdgeColorShd,
        SliceColor[num_points],
        SliceColorShd[num_points];

    float		rad = 0.0;					// radius
    float		tot_val = 0.0;
    float		pscl;
    int			cx,							// affects func_px()
        cy;							// affects func_py()
    /* ~ 1% for a size of 100 pixs */
    /* label sizes will more dictate this */
    float		min_grphable = ( GDCPIE_other_threshold < 0?
		                 100.0/(float)MIN(imagewidth,imageheight):
		                 (float)GDCPIE_other_threshold )/100.0;
    short		num_slices1 = 0,
        num_slices2 = 0;
    char		any_too_small = FALSE;
    char		others[num_points];
    float		slice_angle[3][num_points];	// must be used with others[]
    char		threeD = ( type == GDC_3DPIE );

    int			xdepth_3D      = 0,			// affects func_px()
        ydepth_3D      = 0;			// affects func_py()
    int			do3Dx = 0,					// reserved for macro use
        do3Dy = 0;

    //	GDCPIE_3d_angle = MOD_360(90-GDCPIE_3d_angle+360);
    pie_3D_rad = TO_RAD( GDCPIE_3d_angle );

    xdepth_3D      = threeD? (int)( cos((double)MOD_2PI(M_PI_2-pie_3D_rad+2.0*M_PI)) * GDCPIE_3d_depth ): 0;
    ydepth_3D      = threeD? (int)( sin((double)MOD_2PI(M_PI_2-pie_3D_rad+2.0*M_PI)) * GDCPIE_3d_depth ): 0;
    //	xdepth_3D      = threeD? (int)( cos(pie_3D_rad) * GDCPIE_3d_depth ): 0;
    //	ydepth_3D      = threeD? (int)( sin(pie_3D_rad) * GDCPIE_3d_depth ): 0;

    load_font_conversions();

	/* ----- get total value ----- */
    for( i=0; i<num_points; ++i )
        tot_val += val[i];

	/* ----- pie sizing ----- */
	/* ----- make width room for labels, depth, etc.: ----- */
	/* ----- determine pie's radius ----- */
    {
        int		title_hgt  = GDCPIE_title? 1			/*  title? horizontal text line */
            + GDC_fontc[GDCPIE_title_size].h
            * (int)cnt_nl( GDCPIE_title, (int*)NULL )
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
            float	this = this_pct*(2.0*M_PI);	/* pie-portion */
            if( (this_pct > min_grphable) ||	/* too small */
                (!GDCPIE_missing || !GDCPIE_missing[i]) ) {	/* still want angles */
                int this_explode = GDCPIE_explode? GDCPIE_explode[i]: 0;
                double	this_sin;
                double	this_cos;
                slice_angle[0][i] = this/2.0+last; /* mid-point on full pie */
                slice_angle[1][i] = last;		   /* 1st on full pie */
                slice_angle[2][i] = this+last;	   /* 2nd on full pie */
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
                        lbl_hgt = ( cnt_nl(lbl[i], &lbl_len) + (GDCPIE_percent_labels == GDCPIE_PCT_ABOVE ||
	                GDCPIE_percent_labels == GDCPIE_PCT_BELOW? 1: 0) )
                            * (GDC_fontc[GDCPIE_label_size].h+1);
                        sprintf( foo,
                                 (GDCPIE_percent_labels==GDCPIE_PCT_LEFT ||
                                  GDCPIE_percent_labels==GDCPIE_PCT_RIGHT) &&
                                 lbl[i]? "(%.0f%%)":
                                 "%.0f%%",
                                 this_pct * 100.0 );
                        pct_len = GDCPIE_percent_labels == GDCPIE_PCT_NONE? 0: strlen(foo);
                        lbl_wdth = ( GDCPIE_percent_labels == GDCPIE_PCT_RIGHT ||
                                     GDCPIE_percent_labels == GDCPIE_PCT_LEFT? lbl_len+1+pct_len:
                                     MAX(lbl_len,pct_len) )
                            * GDC_fontc[GDCPIE_label_size].w;
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
                    //											     ( (GDC_fontc[GDCPIE_label_size].h+1)/2 +
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
                            this_y_explode_pos = func_iy(i,j,1);
                            if( this_y_explode_pos < cy-cheight )
                                rad -= (float)ABS( (double)((cy-cheight)-this_y_explode_pos)/cos((double)slice_angle[j][i]) );
                            if( this_y_explode_pos > cy+cheight )
                                rad -= (float)ABS( (double)(1+this_y_explode_pos-(cy+cheight))/cos((double)slice_angle[j][i]) );

                            this_x_explode_pos = func_ix(i,j,1);
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
            last += this;
        }
    }

    /* --- allocate the requested colors --- */
    BGColor   = GDCPIE_BGColor;
    LineColor = GDCPIE_LineColor;
    PlotColor = GDCPIE_PlotColor;
    if( GDCPIE_EdgeColor != GDC_NOCOLOR ) {
        EdgeColor = GDCPIE_EdgeColor;
        if( threeD )
            // PENDING(kalle) This can probably be done more easily
            // with the hsv color model
            EdgeColorShd = QColor( GDCPIE_EdgeColor.red() / 2,
                                   GDCPIE_EdgeColor.green() / 2,
                                   GDCPIE_EdgeColor.blue() / 2 );
    }

    /* --- set color for each slice --- */
    for( i=0; i<num_points; ++i )
        if( GDCPIE_Color ) {
            QColor slc_clr = GDCPIE_Color[i];
            SliceColor[i]     = slc_clr;
            if( threeD )
                SliceColorShd[i] = QColor( slc_clr.red() / 2,
                                           slc_clr.green() / 2,
                                           slc_clr.blue() / 2 );
        } else {
            SliceColor[i]     = PlotColor;
            if( threeD )
                SliceColorShd[i] = QColor( GDCPIE_PlotColor.red() / 2,
                                           GDCPIE_PlotColor.green() / 2,
                                           GDCPIE_PlotColor.blue() / 2 );
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
                    p->drawLine( func_cx(i,1), func_cy(i,1), 
                                 func_ix(i,1,1), func_iy(i,1,1) );
                    p->drawLine( func_cx(i,1), func_cy(i,1), 
                                 func_ix(i,2,1), func_iy(i,2,1) );
					
                    // original parameters:
                    // center: func_cx(i,1), func_cy(i,1)
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
                    // 					gdImageArc( im, func_cx(i,1), func_cy(i,1),
                    // 								rad*2, rad*2,
                    // 								TO_INT_DEG_FLOOR(slice_angle[1][i])+270,
                    // 								TO_INT_DEG_CEIL(slice_angle[2][i])+270,
                    // 								SliceColorShd[i] );

                    // New - Qt:
                    p->setPen( SliceColorShd[i] );
                    p->drawArc( func_cx(i,1)-(rad*2/2), // x
                                func_cy(i,1)-(rad*2/2), // y
                                rad*2, rad*2,           // w, h
                                (TO_INT_DEG_FLOOR(slice_angle[1][i])+270)*16,
                                ((TO_INT_DEG_CEIL(slice_angle[2][i])+270)-
                                 (TO_INT_DEG_FLOOR(slice_angle[1][i])+270))*16 );
								
                    rad1 = rad;
                    rad *= 3.0/4.0;
                    // PENDING(kalle) Can it really be that Qt has no
                    // flood fill??? If necessary, implement own
                    // algorithm here.
                    //					gdImageFillToBorder( im, func_ix(i,0,1), func_iy(i,0,1), SliceColorShd[i], SliceColorShd[i] );
                    rad = rad1;
                    if( GDCPIE_EdgeColor != GDC_NOCOLOR ) {
                        p->setPen( EdgeColorShd );
                        p->drawLine( func_cx(i,1), func_cy(i,1), func_ix(i,1,1), func_iy(i,1,1) );
                        p->drawLine( func_cx(i,1), func_cy(i,1), func_ix(i,2,1), func_iy(i,2,1) );
                        // For differences between gd arc handling and 
                        // Qt arc handling, please see first
                        // QPainter::drawArc() call in this file.

                        // New: Qt
                        p->drawArc( func_cx(i,1)-rad, // x
                                    func_cy(i,1)-rad, // y
                                    rad*2, rad*2,     // w, h
                                    (TO_INT_DEG(slice_angle[1][i])+270)*16,
                                    ((TO_INT_DEG(slice_angle[2][i])+270)-
                                     (TO_INT_DEG(slice_angle[1][i])+270))*16 );
                        // Original: gd
                        // 						gdImageArc( im, func_cx(i,1), func_cy(i,1), 
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
				
                gdp.setPoint( 0, func_cy(i,0), func_cy(i,0) );
                gdp.setPoint( 1, func_cx(i,1), func_cy(i,1) );
                gdp.setPoint( 2, OX(i,tmp_slice[t].angle,1),
                              OY(i,tmp_slice[t].angle,1);
                              gdp.setPoint( 3, OX(i,tmp_slice[t].angle,0),
                                            OY(i,tmp_slice[t].angle,0);
				
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
				

                                            if( GDCPIE_EdgeColor != GDC_NOCOLOR ) {
	p->setPen( EdgeColorShd );
	p->drawLine( func_cx(i,0), func_cy(i,0), 
	             func_cx(i,1), func_cy(i,1) );
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
                        // EXPLODE_func_cx_func_cy( slice_angle[0][i], i );
                        p->setPen( SliceColor[i] );
                        p->drawLine( func_cx(i,0), func_cy(i,0), 
                                     func_ix(i,1,0), func_iy(i,1,0) );
                        p->drawLine( func_cx(i,0), func_cy(i,0), 
                                     func_ix(i,2,0), func_iy(i,2,0) );
				
                        // For the differences between Qt arc handling and gd
                        // arc handling, please see first call to
                        // QPainter::drawArc() in this file.

                        // New - Qt:
                        p->drawArc( func_cx(i,0)-rad, // x
                                    func_cy(i,0)-rad, // y
                                    rad*2, rad*2,     // w, h
                                    (TO_INT_DEG_FLOOR(slice_angle[1][i])+270)*16,
                                    ((TO_INT_DEG_CEIL(slice_angle[2][i])+270)-
                                     (TO_INT_DEG_FLOOR(slice_angle[1][i])+270))*16 );

                        // Original - gd:
                        // 				gdImageArc( im, func_cx(i,0), func_cy(i,0), 
                        // 							(int)rad*2, (int)rad*2,
                        // 							TO_INT_DEG_FLOOR(slice_angle[1][i])+270,
                        // 							TO_INT_DEG_CEIL(slice_angle[2][i])+270,
                        // 							SliceColor[i] );
                        rad1 = rad;
                        rad *= 3.0/4.0;
                        // PENDING(kalle) Can it really be that Qt has no
                        // flood fill?
                        //				gdImageFillToBorder( im, func_ix(i,0,0), func_iy(i,0,0), SliceColor[i], SliceColor[i] );
                        /* catch missed pixels on narrow slices */
                        p->setPen( SliceColor[i] );
                        p->drawLine( func_cx(i,0), func_cy(i,0), 
                                     func_ix(i,0,0), func_iy(i,0,0) );
                        rad = rad1;
                        if( GDCPIE_EdgeColor != GDC_NOCOLOR ) {
                            p->setPen( EdgeColor );
                            p->drawLine( func_cx(i,0), func_cy(i,0), 
                                         func_ix(i,1,0), func_iy(i,1,0) );
                            p->drawLine( func_cx(i,0), func_cy(i,0), 
                                         func_ix(i,2,0), func_iy(i,2,0) );

                            // For the differences between Qt arc handling and 
                            // gd arc handling, please see the first call to
                            // QPainter::drawArc() in this file.

                            // New - Qt:
                            p->drawArc( func_cx(i,0)-rad,   // x
                                        func_cy(i,0)-rad,   // y
                                        rad*2, rad*2,       // w, h
                                        (TO_INT_DEG(slice_angle[1][i])+270)*16,
                                        ((TO_INT_DEG(slice_angle[2][i])+270)-
                                         (TO_INT_DEG(slice_angle[1][i])+270))*16 );

                            // Original - gd:
                            // 					gdImageArc( im, func_cx(i,0), func_cy(i,0), 
                            // 								rad*2, rad*2,
                            // 								TO_INT_DEG(slice_angle[1][i])+270, TO_INT_DEG(slice_angle[2][i])+270,
                            // 								EdgeColor );
                        }
                    }
            }
	
            if( GDCPIE_title ) {
                int	title_len;
		
                cnt_nl( GDCPIE_title, &title_len );
                GDCImageStringNL( im,
                                  &GDC_fontc[GDCPIE_title_size],
                                  (imagewidth-title_len*GDC_fontc[GDCPIE_title_size].w)/2,
                                  1,
                                  GDCPIE_title,
                                  LineColor,
                                  GDC_JUSTIFY_CENTER );
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
				
                        lbl_wdth *= GDC_fontc[GDCPIE_label_size].w;
                        sprintf( pct_str,
                                 (GDCPIE_percent_labels==GDCPIE_PCT_LEFT ||
                                  GDCPIE_percent_labels==GDCPIE_PCT_RIGHT) &&
                                 lbl[i]? "(%.0f%%)":
                                 "%.0f%%",
                                 (val[i]/tot_val) * 100.0 );
                        pct_wdth = GDCPIE_percent_labels == GDCPIE_PCT_NONE?
                            0:
                            strlen(pct_str) * GDC_fontc[GDCPIE_label_size].w;
				
                        lbly = (liney = func_iy(i,0,0))-( num_nl * (1+GDC_fontc[GDCPIE_label_size].h) ) / 2;
                        lblx = pctx = linex = func_ix(i,0,0);
				
                        if( slice_angle[0][i] > M_PI ) {								/* which semicircle */
                            lblx -= lbl_wdth;
                            pctx = lblx;
                            ++linex;
                        } else
                            --linex;

                        switch( GDCPIE_percent_labels )	{
                        case GDCPIE_PCT_LEFT:	
                            if( slice_angle[0][i] > M_PI )
                                pctx -= lbl_wdth-1;
                            else
                                lblx += pct_wdth+1;
                            pcty = func_iy(i,0,0) - ( 1+GDC_fontc[GDCPIE_label_size].h ) / 2;
                            break;
                        case GDCPIE_PCT_RIGHT:	
                            if( slice_angle[0][i] > M_PI )
                                lblx -= pct_wdth-1;
                            else
                                pctx += lbl_wdth+1;
                            pcty = func_iy(i,0,0) - ( 1+GDC_fontc[GDCPIE_label_size].h ) / 2;
                            break;
                        case GDCPIE_PCT_ABOVE:	
                            lbly += (1+GDC_fontc[GDCPIE_label_size].h) / 2;
                            pcty = lbly - (GDC_fontc[GDCPIE_label_size].h);
                            break;
                        case GDCPIE_PCT_BELOW:	
                            lbly -= (1+GDC_fontc[GDCPIE_label_size].h) / 2;
                            pcty = lbly + (GDC_fontc[GDCPIE_label_size].h) * num_nl;
                            break;
                        case GDCPIE_PCT_NONE:
                        default:
                        }
				
                        if( GDCPIE_percent_labels != GDCPIE_PCT_NONE ) {
							p->setPen( LineColor );
							p->setFont( GDC_fontc[GDCPIE_label_size].f );
							p->drawText( slice_angle[0][i] <= M_PI? pctx:
										 pctx+lbl_wdth-pct_wdth,
										 pcty,
										 pct_str );
						}
                        if( lbl[i] ) {
							QRect br = QFontMetrics( GDC_fontc[GDCPIE_label_size] ).boundingRect( 0, 0, MAXINT, MAXINT, lbl[i] );
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
                                         func_ix(i,0,0), func_iy(i,0,0) );
                        }
                    }
                }
                rad -= GDCPIE_label_dist;
            }
        }
