
#include "engine.h"
#include "engine2.h"
#include "enginehelper.h"
#include "enginedraw.h"

#include "kchartparams.h"

#include <qfont.h>
#include <qcolor.h>
#include <qpainter.h>

#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h> //PENDING(kalle) Remove?

float pie_3D_rad = 0.;
float dist_foo1 = 0.;
float dist_foo2 = 0.;

/* expect slice number:     i (index into slice_angle array) *\
 *   and position on slice: f (0: slice middle,              *
 *                             1: leading (clockwise),       *
 *                             2: trailing edge)             *
 *   and 3D depth:          d (0: do depth,                  *
 *                             1: no depth adjustment)       *
\* adjusts for explosion                                     */

/* ------------------------------------------------------- *\
 * oof!  cleaner way???
 * order by angle opposite (180) of depth angle
 * comparing across 0-360 line
\* ------------------------------------------------------- */
extern "C" {
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
pie_gif( short imagewidth,
         short imageheight,
         QPainter*      p, // paint here
         KChartParameters* params, // all the parameters
         int  num_points,
         float val[],   /* data */
         QColor color[],QColor colorShd[] )
{
    params->offsetCol=num_points*params->colPie;
    int offsetCol=params->offsetCol;

    int i;
    QColor BGColor, LineColor, PlotColor;
    QColor *SliceColor = new QColor[num_points];
    QColor *SliceColorShd = new QColor[num_points];
    //QColor  EdgeColor, EdgeColorShd;
    float rad = 0.0;// radius
    float tot_val = 0.0;
    float pscl;
    int  cx;// affects func_px()
    int  cy;// affects func_py()
    /* ~ 1% for a size of 100 pixs */
    /* label sizes will more dictate this */
    float min_grphable = ( params->other_threshold < 0?
                           100.0/(float)QMIN(imagewidth,imageheight):
                           (float)params->other_threshold )/100.0;
    // short num_slices1 = 0;
    // short num_slices2 = 0;
    // char any_too_small = FALSE;
    char *others = new char[num_points];

    // must be used with others[]
    float (*slice_angle)[3] = new float[num_points][3];

    int xdepth_3D = 0;// affects func_px()
    int ydepth_3D = 0;// affects func_py()
    // int      do3Dx = 0;  // reserved for macro use
    // int do3Dy = 0;


    //  GDCPIE_3d_angle = MOD_360(90-params->_3d_angle+360);

    pie_3D_rad = TO_RAD( params->_3d_angle );

    xdepth_3D      = params->threeD() ? (int)( cos((double)MOD_2PI(M_PI_2-pie_3D_rad+2.0*M_PI)) * params->_3d_depth ): 0;
    ydepth_3D      = params->threeD() ? (int)( sin((double)MOD_2PI(M_PI_2-pie_3D_rad+2.0*M_PI)) * params->_3d_depth ): 0;
    //  xdepth_3D      = threeD? (int)( cos(pie_3D_rad) * params->_3d_depth ): 0;
    //  ydepth_3D      = threeD? (int)( sin(pie_3D_rad) * params->_3d_depth ): 0;

        /* ----- get total value ----- */
    for( i=0; i<num_points; ++i )
        tot_val += val[i];



        /* ----- pie sizing ----- */
        /* ----- make width room for labels, depth, etc.: ----- */
        /* ----- determine pie's radius ----- */
    {
        int title_hgt  = !params->title.isEmpty() ? 1                   /*  title? horizontal text line */
            + params->titleFontHeight()
            * (int)cnt_nl( params->title.latin1(), (int*)NULL )
            + 2:
            0;
        float   last = 0.0;
        //        float label_explode_limit = 0.0;
        int cheight;
        int cwidth;

        // maximum: no labels, explosions
        // gotta start somewhere
        rad = (float)QMIN( imagewidth/2-(1+ABS(xdepth_3D)), imageheight/2-(1+ABS(ydepth_3D))-title_hgt );
	
        /* ok fix center, i.e., no floating re labels, explosion, etc. */
        cx = imagewidth/2 /* - xdepth_3D */ ;
        cy = (imageheight-title_hgt)/2 + title_hgt /* + ydepth_3D */ ;

        cheight = (imageheight- title_hgt)/2 /* - ydepth_3D */ ;
        cwidth  = cx;

        /* walk around pie. determine spacing to edge */
        for( i=0; i<num_points; ++i ) 
	  {
            float this_pct = val[i]/tot_val;    /* should never be > 100% */
            float that = this_pct*(2.0*M_PI);   /* pie-portion */
	    
	    if( (this_pct > min_grphable) ||     /* too small */
                (params->missing.isNull() || !params->missing[offsetCol+i]) )
	      {     /* still want angles */
                int this_explode = !params->explode.isNull() ? params->explode[offsetCol+i]: 0;

                double  this_sin;
                double  this_cos;
                slice_angle[i][0] = that/2.0+last; /* mid-point on full pie */
                slice_angle[i][1] = last;          /* 1st on full pie */
                slice_angle[i][2] = that+last;     /* 2nd on full pie */

                this_sin = sin( (double)slice_angle[i][0] );
                this_cos = cos( (double)slice_angle[i][0] );

                if( params->missing.isNull() || !params->missing[offsetCol+i] )
                  {
                    short lbl_wdth,lbl_hgt;
                    float this_y_explode_limit,this_x_explode_limit;

                    /* start slice label height, width     */
                    /*  accounting for PCT placement, font */
                    /*if( lbl && lbl[i] )*/
                    if( !params->legend.isEmpty() && !params->legend[i].isEmpty() )
                      {
                        char foo[1+4+1+1]; /* XPG2 compatibility */
                        int pct_len;
                        int lbl_len = 0;

                        lbl_hgt = ( cnt_nl(params->legend[i].latin1(), &lbl_len) + (params->percent_labels == KCHARTPCTTYPE_ABOVE ||
										    params->percent_labels == KCHARTPCTTYPE_BELOW? 1: 0) )
                          * (params->labelFontHeight()+1);
			
                        sprintf( foo,
                                 (params->percent_labels==KCHARTPCTTYPE_LEFT ||
                                  params->percent_labels==KCHARTPCTTYPE_RIGHT) &&
                                 !params->legend[i].isEmpty()? "(%.0f%%)":
                                 "%.0f%%",
                                 this_pct * 100.0 );
                        pct_len = params->percent_labels == KCHARTPCTTYPE_NONE? 0: strlen(foo);
                        lbl_wdth = ( params->percent_labels == KCHARTPCTTYPE_RIGHT ||
                                     params->percent_labels == KCHARTPCTTYPE_LEFT? lbl_len+1+pct_len:
                                     QMAX(lbl_len,pct_len) )
                          * params->labelFontWidth();
                    } 
		    else
                        lbl_wdth = lbl_hgt = 0;
                    /* end label height, width */

                    /* diamiter limited by this piont's: explosion, label                 */
                    /* (radius to box @ slice_angle) - (explode) - (projected label size) */
                    /* radius constraint due to labels */
                    /*this_y_explode_limit = (float)this_cos==0.0? MAXFLOAT:
                        (       (float)( (double)cheight/ABS(this_cos) ) -
                                                        (float)( this_explode + (lbl[i]? params->label_dist: 0) ) -
                                                        (float)( lbl_hgt/2 ) / (float)ABS(this_cos)     );*/

                    this_y_explode_limit = (float)this_cos==0.0? MAXFLOAT:
                        (       (float)( (double)cheight/ABS(this_cos) ) -
                                                        (float)( this_explode + (!params->legend[i].isEmpty()? params->label_dist: 0) ) -
                                                        (float)( lbl_hgt/2 ) / (float)ABS(this_cos)     );

                    /*this_x_explode_limit = (float)this_sin==0.0? MAXFLOAT:
                        (       (float)( (double)cwidth/ABS(this_sin) ) -
                                                        (float)( this_explode + (lbl[i]? params->label_dist: 0) ) -
                                                        (float)( lbl_wdth ) / (float)ABS(this_sin)      );*/

                    this_x_explode_limit = (float)this_sin==0.0? MAXFLOAT:
                        (       (float)( (double)cwidth/ABS(this_sin) ) -
                                                        (float)( this_explode + (!params->legend[i].isEmpty()? params->label_dist: 0) ) -
                                                        (float)( lbl_wdth ) / (float)ABS(this_sin)      );

                    rad = QMIN( rad, this_y_explode_limit );
                    rad = QMIN( rad, this_x_explode_limit );

                    // ok at this radius (which is most likely larger than final)
                    // adjust for inter-label spacing
                    //                          if( lbl[i] && *lbl[i] )
                    //                                  {
                    //                                  char which_edge = slice_angle[i][0] > M_PI? +1: -1;             // which semi
                    //                                  last_label_yedge = cheight - (int)( (rad +                              // top or bottom of label
                    //                                                                                                          (float)(this_explode +
                    //                                                                                                          (float)GDCPIE_label_dist)) * (float)this_cos ) +
                    //                                                                                       ( (params->labelFontHeight()+1)/2 +
                    //                                                                                                  GDC_label_spacing )*which_edge;
                    //                                  }

                    /* radius constriant due to exploded depth */
                    /* at each edge of the slice, and the middle */
                    /* this is really stupid */
                    /*  this section uses a different algorithm then above, but does the same thing */
                    /*  could be combined, but each is ugly enough! */
                    // PROTECT /0
                    if( params->threeD() )
                        {
                        short   j;
                        int             this_y_explode_pos;
                        int             this_x_explode_pos;

                        // first N E S W (actually no need for N)
                        if( (slice_angle[i][1] < M_PI_2 && M_PI_2 < slice_angle[i][2]) &&                               // E
                            (this_x_explode_pos=OX(i,M_PI_2,1)) > cx+cwidth )
                            rad -= (float)ABS( (double)(1+this_x_explode_pos-(cx+cwidth))/sin(M_PI_2) );
                        if( (slice_angle[i][1] < 3.0*M_PI_2 && 3.0*M_PI_2 < slice_angle[i][2]) &&               // W
                            (this_x_explode_pos=OX(i,3.0*M_PI_2,1)) < cx-cwidth )
                            rad -= (float)ABS( (double)(this_x_explode_pos-(cx+cwidth))/sin(3.0*M_PI_2) );
                        if( (slice_angle[i][1] < M_PI && M_PI < slice_angle[i][2]) &&                                   // S
                            (this_y_explode_pos=OY(i,M_PI,1)) > cy+cheight )
                            rad -= (float)ABS( (double)(1+this_y_explode_pos-(cy+cheight))/cos(M_PI) );

                        for( j=0; j<3; ++j ) {
                            this_y_explode_pos = IY(i,j,1);
                            if( this_y_explode_pos < cy-cheight )
                                rad -= (float)ABS( (double)((cy-cheight)-this_y_explode_pos)/cos((double)slice_angle[i][j]) );
                            if( this_y_explode_pos > cy+cheight )
                                rad -= (float)ABS( (double)(1+this_y_explode_pos-(cy+cheight))/cos((double)slice_angle[i][j]) );

                            this_x_explode_pos = IX(i,j,1);
                            if( this_x_explode_pos < cx-cwidth )
                                rad -= (float)ABS( (double)((cx-cwidth)-this_x_explode_pos)/sin((double)slice_angle[i][j]) );
                            if( this_x_explode_pos > cx+cwidth )
                                rad -= (float)ABS( (double)(1+this_x_explode_pos-(cx+cwidth))/sin((double)slice_angle[i][j]) );
                        }
                    }
                }
                others[i] = FALSE;
            } 
	    else 
	      {
                others[i] = TRUE;
                slice_angle[i][0] = -MAXFLOAT;
	      }
            last += that;
        }
    }

    /* --- allocate the requested colors --- */
    BGColor   = params->BGColor;
    LineColor = params->LineColor;
    PlotColor = params->PlotColor;
    /*if( params->EdgeColor != QColor() )
        {
        EdgeColor = params->EdgeColor;
        if( params->threeD() )
            // PENDING(kalle) This can probably be done more easily
            // with the hsv color model
            EdgeColorShd = QColor( params->EdgeColor.red() / 2,
                                   params->EdgeColor.green() / 2,
                                   params->EdgeColor.blue() / 2 );
         }*/

    /* --- set color for each slice --- */
    for( i=0; i<num_points; ++i )
        if( params->SetColor.count() )
        {
            QColor slc_clr = params->SetColor.color( i );
            SliceColor[i] = slc_clr;
            if( params->threeD() )
                SliceColorShd[i] = QColor( slc_clr.red() / 2,
                                           slc_clr.green() / 2,
                                           slc_clr.blue() / 2 );
        }
        else
        {
          /*SliceColor[i] = PlotColor;
            if( params->threeD() )
                SliceColorShd[i] = QColor( params->PlotColor.red() / 2,
                                           params->PlotColor.green() / 2,
                                           params->PlotColor.blue() / 2 );
          */
          SliceColor[i] = color[i];
          if( params->threeD() )
            SliceColorShd[i]=colorShd[i];
        }

    pscl = (2.0*M_PI)/tot_val;

    /* ----- calc: smallest a slice can be ----- */
    /* 1/2 circum / num slices per side. */
    /*              determined by number of labels that'll fit (height) */
    /* scale to user values */
    /* ( M_PI / (imageheight / (SFONTHGT+1)) ) */
    //  min_grphable = tot_val /
    //                             ( 2.0 * (float)imageheight / (float)(SFONTHGT+1+TFONTHGT+2) );


    if( params->threeD() )
    {
        /* draw background shaded pie */
        {
            float rad1 = rad;
            for( i=0; i<num_points; ++i )
              if( !(others[i]) &&
                (params->missing.isNull() || !params->missing[offsetCol+i]) )
                {

                    float rad = rad1;
                    p->setPen( SliceColorShd[i] );
                    p->drawLine( CX(i,1),CY(i,1),
                                 IX(i,1,1), IY(i,1,1) );
                    p->drawLine( CX(i,1),CY(i,1),
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
                    //                                  gdImageArc( im, CX(i,1), CY(i,1),
                    //                                                          rad*2, rad*2,
                    //                                                          TO_INT_DEG_FLOOR(slice_angle[1][i])+270,
                    //                                                          TO_INT_DEG_CEIL(slice_angle[2][i])+270,
                    //                                                          SliceColorShd[i] );

                    // New - Qt:

                    p->setPen( SliceColor[i] );
                    p->setBrush(SliceColor[i]);

                    //you must draw clockwise direction
                    // => angle (-)

                    p->drawPie( CX(i,1)-(rad*2/2), // x
                                CY(i,1)-(rad*2/2), // y
                                rad*2, rad*2,           // w, h
                                -(TO_INT_DEG_FLOOR(slice_angle[i][1])+270)*16,
                                -((TO_INT_DEG_CEIL(slice_angle[i][2])+270)-
                                 (TO_INT_DEG_FLOOR(slice_angle[i][1])+270))*16 );

                    rad1 = rad;
                    rad *= 3.0/4.0;
                    // PENDING(kalle) Can it really be that Qt has no
                    // flood fill??? If necessary, implement own
                    // algorithm here.
                    //                                  gdImageFillToBorder( im, IX(i,0,1), IY(i,0,1), SliceColorShd[i], SliceColorShd[i] );
                    rad = rad1;
                    /*if( params->EdgeColor != QColor() )
                    {
                        p->setPen( EdgeColorShd );
                        p->drawLine( CX(i,1),CY(i,1),
                                    IX(i,1,1), IY(i,1,1) );
                        p->drawLine( CX(i,1),CY(i,1),
                                    IX(i,2,1), IY(i,2,1) );
                        // For differences between gd arc handling and
                        // Qt arc handling, please see first
                        // QPainter::drawArc() call in this file.

                        // New: Qt
                        p->setBrush(EdgeColorShd);

                        p->drawPie( CX(i,1)-rad, // x
                                    CY(i,1)-rad, // y
                                    rad*2, rad*2,     // w, h
                                    -(TO_INT_DEG(slice_angle[1][i])+270)*16,
                                    -((TO_INT_DEG(slice_angle[2][i])+270)-
                                     (TO_INT_DEG(slice_angle[1][i])+270))*16 );
                        // Original: gd
                        //                                              gdImageArc( im, CX(i,1), CY(i,1),
                        //                                                                      rad*2, rad*2,
                        //                                                                      TO_INT_DEG(slice_angle[1][i])+270, TO_INT_DEG(slice_angle[2][i])+270,
                        //                                                                      EdgeColorShd);
                        }*/
                }
        }
        /* fill in connection to foreground pie */
        /* this is where we earn our keep */
        {
            tmp_slice_t *tmp_slice = new tmp_slice_t[2*num_points+2];
            int t,num_slice_angles = 0;

            for( i=0; i<num_points; ++i )
                 if( params->missing.isNull() || !params->missing[offsetCol+i] )
                   {

		     if( RAD_DIST1(slice_angle[i][1]) < RAD_DIST2(slice_angle[i][0]) )
		       tmp_slice[num_slice_angles].hidden = FALSE;
		     else
		       tmp_slice[num_slice_angles].hidden = TRUE;

                    tmp_slice[num_slice_angles].i       = i;
                    tmp_slice[num_slice_angles].slice   = slice_angle[i][0];
                    tmp_slice[num_slice_angles++].angle = slice_angle[i][1];
                    if( RAD_DIST1(slice_angle[i][2]) < RAD_DIST2(slice_angle[i][0]) )
		      tmp_slice[num_slice_angles].hidden = FALSE;
                    else
		      tmp_slice[num_slice_angles].hidden = TRUE;
                    tmp_slice[num_slice_angles].i       = i;
                    tmp_slice[num_slice_angles].slice   = slice_angle[i][0];
                    tmp_slice[num_slice_angles++].angle = slice_angle[i][2];
                    // identify which 2 slices (i) have a tangent parallel to depth angle
                    if( slice_angle[i][1]<MOD_2PI(pie_3D_rad+M_PI_2) && slice_angle[i][2]>MOD_2PI(pie_3D_rad+M_PI_2) )  
		      {
			tmp_slice[num_slice_angles].i       = i;
                        tmp_slice[num_slice_angles].hidden  = FALSE;
                        tmp_slice[num_slice_angles].slice   = slice_angle[i][0];
                        tmp_slice[num_slice_angles++].angle = MOD_2PI( pie_3D_rad+M_PI_2 );
		      }
                    if( slice_angle[i][1]<MOD_2PI(pie_3D_rad+3.0*M_PI_2) && slice_angle[i][2]>MOD_2PI(pie_3D_rad+3.0*M_PI_2) )  {
		      tmp_slice[num_slice_angles].i       = i;
		      tmp_slice[num_slice_angles].hidden  = FALSE;
		      tmp_slice[num_slice_angles].slice   = slice_angle[i][0];
		      tmp_slice[num_slice_angles++].angle = MOD_2PI( pie_3D_rad+3.0*M_PI_2 );
                    }
                }

            qsort( tmp_slice, num_slice_angles, sizeof(struct tmp_slice_t), ocmpr );
            for( t=0; t<num_slice_angles; ++t ) 
	      {
                QPointArray     gdp( 4 );

                i = tmp_slice[t].i;

                gdp.setPoint( 0, CX(i,0), CY(i,0) );
                gdp.setPoint( 1, CX(i,1), CY(i,1) );
                gdp.setPoint( 2, OX(i,tmp_slice[t].angle,1),
                              OY(i,tmp_slice[t].angle,1) );
		gdp.setPoint( 3, OX(i,tmp_slice[t].angle,0),
			      OY(i,tmp_slice[t].angle,0) );

		if( !(tmp_slice[t].hidden) ) 
		  {
		    p->setBrush( SliceColorShd[i] );
		    p->setPen( SliceColorShd[i] );
		    p->drawPolygon( gdp );
		  } 
		else 
		  {
		    rad -= 2.0;                                                                             /* no peeking */
		    gdp.setPoint( 0, OX(i,slice_angle[i][0],0),
				  OY(i,slice_angle[i][0],0) );
		    gdp.setPoint( 1, OX(i,slice_angle[i][0],1),
				  OY(i,slice_angle[i][0],1) );
		    rad += 2.0;
		    gdp.setPoint( 2, OX(i,slice_angle[i][1],1),
				  OY(i,slice_angle[i][1],1) );
		    gdp.setPoint( 3, OX(i,slice_angle[i][1],0),
				  OY(i,slice_angle[i][1],0) );
		    p->setBrush( SliceColorShd[i] );
		    p->setPen( SliceColorShd[i] );
		    p->drawPolygon( gdp );
		    gdp.setPoint( 2, OX(i,slice_angle[i][2],1),
				  OY(i,slice_angle[i][2],1) );
		    gdp.setPoint( 3, OX(i,slice_angle[i][2],0),
				  OY(i,slice_angle[i][2],0) );
		    p->setBrush( SliceColorShd[i] );
		    p->setPen( SliceColorShd[i] );
		    p->drawPolygon( gdp );
		  }


                                /*if( params->EdgeColor != QColor() )
                                  {
				  p->setPen( EdgeColorShd );
				  p->drawLine( CX(i,0),CY(i,0),
                                                 CX(i,1),CY(i,1) );
						 p->drawLine( OX(i,tmp_slice[t].angle,0),
                                                 OY(i,tmp_slice[t].angle,0),
                                                 OX(i,tmp_slice[t].angle,1),
                                                 OY(i,tmp_slice[t].angle,1) );
                                                 }*/
	      }
            delete [] tmp_slice;
        }
	
    }


        /* ----- pie face ----- */
        {
                // float        last = 0.0;
                float   rad1 = rad;
                for( i=0; i<num_points; ++i )

                  if( !others[i] &&
		      (params->missing.isNull() || !params->missing[offsetCol+i]) )
		    {
		      float   rad = rad1;
		      
                                // last += val[i];
                                // EXPLODE_CX_CY( slice_angle[0][i], i );
		      p->setPen( SliceColor[i] );
		      
		      p->drawLine( CX(i,0),CY(i,0),
				   IX(i,1,0), IY(i,1,0) );
		      p->drawLine( CX(i,0),CY(i,0),
				   IX(i,2,0), IY(i,2,0) );
		      
                                // For the differences between Qt arc handling and gd
                                // arc handling, please see first call to
                                // QPainter::drawArc() in this file.

                                // New - Qt:
                                //put slice color
		      p->setBrush(SliceColor[i]);
		      
		      
		      p->drawPie( CX(i,0)-rad, // x
				  CY(i,0)-rad, // y
				  rad*2, rad*2,// w, h
				  -(TO_INT_DEG_FLOOR(slice_angle[i][1])+270)*16,
				  -((TO_INT_DEG_CEIL(slice_angle[i][2])+270)-
				    (TO_INT_DEG_FLOOR(slice_angle[i][1])+270))*16 );


                                // Original - gd:
                                //                              gdImageArc( im, CX(i,0), CY(i,0),
                                //                                                      (int)rad*2, (int)rad*2,
                                //                                                      TO_INT_DEG_FLOOR(slice_angle[1][i])+270,
                                //                                                      TO_INT_DEG_CEIL(slice_angle[2][i])+270,
                                //                                                      SliceColor[i] );
		      rad1 = rad;
		      rad *= 3.0/4.0;
                                // PENDING(kalle) Can it really be that Qt has no
                                // flood fill?
                                //                              gdImageFillToBorder( im, IX(i,0,0), IY(i,0,0), SliceColor[i], SliceColor[i] );
                                /* catch missed pixels on narrow slices */
		      p->setPen( SliceColor[i] );
		      p->drawLine( CX(i,0),CY(i,0),
				   IX(i,0,0), IY(i,0,0) );
		      
		      rad = rad1;
                                /*if( params->EdgeColor != QColor() )
                                  {
                                        p->setPen( EdgeColor );
                                        p->drawLine( CX(i,0),CY(i,0),
                                                IX(i,1,0), IY(i,1,0) );
                                        p->drawLine( CX(i,0),CY(i,0),
                                                IX(i,2,0), IY(i,2,0) );

                                        // For the differences between Qt arc handling and
                                        // gd arc handling, please see the first call to
                                        // QPainter::drawArc() in this file.

                                        // New - Qt:

                                        p->setBrush(EdgeColor);
                                        p->drawPie( CX(i,0)-rad,//  x
                                                    CY(i,0)-rad ,//   y
                                                   rad*2, rad*2,// w, h
                                      -(TO_INT_DEG(slice_angle[1][i])+270)*16,
                                      -((TO_INT_DEG(slice_angle[2][i])+270)-
                                     (TO_INT_DEG(slice_angle[1][i])+270))*16 );

                                        // Original - gd:
                                        //                                      gdImageArc( im, CX(i,0), CY(i,0),
                                        //                                                              rad*2, rad*2,
                                        //                                                              TO_INT_DEG(slice_angle[1][i])+270, TO_INT_DEG(slice_angle[2][i])+270,
                                        //                                                              EdgeColor );
                                        }*/
		    }
        }


        /* labels */
    if( !params->legend.isEmpty() )
      {
	float   liner = rad;
	
	rad += params->label_dist;
	for( i=0; i<num_points; ++i ) 
	  {
	    if( !others[i] &&
		(params->missing.isNull() || !params->missing[offsetCol+i]) )
	      {
		char pct_str[1+4+1+1];
		int pct_wdth;
		int lbl_wdth;
		short num_nl = cnt_nl( params->legend[i].latin1(), &lbl_wdth );
		int lblx,  pctx;
		int lbly,  pcty = 0;
		int linex, liney;

		lbl_wdth *= params->labelFontWidth();
		sprintf( pct_str,(params->percent_labels==KCHARTPCTTYPE_LEFT ||params->percent_labels==KCHARTPCTTYPE_RIGHT) &&
			 !params->legend[i].isEmpty()? "(%.0f%%)":
			 "%.0f%%",
			 (val[i]/tot_val) * 100.0 );
		
		pct_wdth = params->percent_labels == KCHARTPCTTYPE_NONE?0:strlen(pct_str) * params->labelFontWidth();
		
		lbly = (liney = IY(i,0,0))-( num_nl * (1+params->labelFontHeight()) ) / 2;
		lblx = pctx = linex = IX(i,0,0);
		
		if( slice_angle[i][0] > M_PI )
		  {  /* which semicircle */
		    lblx -= lbl_wdth;
		    pctx = lblx;
		    ++linex;
		  }
		else
		  {
		    --linex;
		  }
		
		switch( params->percent_labels )
		  {
		  case KCHARTPCTTYPE_LEFT:
		    if( slice_angle[i][0] > M_PI )
		      pctx -= lbl_wdth-1;
		    else
		      lblx += pct_wdth+1;
		    pcty = IY(i,0,0) - ( 1+params->labelFontHeight() ) / 2;
		    break;
		  case KCHARTPCTTYPE_RIGHT:
		    if( slice_angle[i][0] > M_PI )
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
		    break;
		    
		  }
		
		
		if( params->percent_labels != KCHARTPCTTYPE_NONE )
		  {
		    p->setPen( LineColor );
		    p->setFont( params->labelFont() );
		    p->drawText( slice_angle[i][0] <= M_PI? pctx:
				 pctx+lbl_wdth-pct_wdth,
				 pcty,
				 pct_str );
		  }
                               
		if( !params->legend[i].isNull() )
		  {
		    QRect br = QFontMetrics( params->labelFont() ).boundingRect( 0, 0, MAXINT, MAXINT, slice_angle[i][0] <= M_PI ?
										 Qt::AlignLeft : Qt::AlignRight, params->legend[i] );
		    p->setPen( LineColor);
		    p->setFont( params->labelFont() );
		    p->drawText( lblx, lbly,
				 br.width(), br.height(),
				 slice_angle[i][0] <= M_PI ?
				 Qt::AlignLeft : Qt::AlignRight,
				 params->legend[i] );
		  }
		if( params->label_line )
		  {
		    float rad = liner;
		    p->setPen( LineColor );
		    p->drawLine( linex, liney,
				 IX(i,0,0), IY(i,0,0) );
		  }
	      }
	  }
	rad -= params->label_dist;
      }
    delete [] SliceColor;
    delete [] SliceColorShd;
    delete [] slice_angle;
    delete others;
}
