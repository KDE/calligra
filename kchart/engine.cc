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
#include "engine2.h"
#include "enginehelper.h"
#include "enginedraw.h"
#include "kchartEngine.h"
#include "kchartparams.h"

#include <qfont.h>
#include <qcolor.h>
#include <qpainter.h>

#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h> //PENDING(kalle) Remove?


kchartEngine keng;


int out_graph( short imagewidth,         // no check for an output device that's too small to fit
	   short imageheight,        // needed info (labels, etc), could core dump
	   QPainter* p,	             // paint into this painter
	   KChartParameters* params, // the parameters of the chart
	   const KChartData& data
	   )
{  // temporary stuff for initializing the engine
  keng.params = params;
  keng.data = &data;
  keng.p = p;
  keng.imagewidth = imagewidth;
  keng.imageheight = imageheight;
  return keng.out_graph();
}









int kchartEngine::compute_yintervals() {
}

/*************************************************************/
//
//  Out graph
//
/*************************************************************/
int kchartEngine::out_graph() {

  int i,j,k;


  if (init()== -1) {
    return -1;
  }
  cerr << "Initialization successfull, proceed\n";
  doLabels();

    //  i.e., not up against Y axes
    char		do_ylbl_fractions =   // %f format not given, or
		( params->ylabel_fmt.isEmpty() ||					//  format doesn't have a %,g,e,E,f or F
		  params->ylabel_fmt.length() == strcspn(params->ylabel_fmt,"%geEfF") );
    if( params->thumbnail ) {
		params->grid = FALSE;
		params->xaxis = FALSE;
		params->yaxis = FALSE;
    }
    debug( "done thumbnails" );


    /* ----- highest & lowest values ----- */
    minmaxValues(num_points, num_sets, uvol,
		 highest, lowest, vhighest, vlowest);

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
		int	vtitle_hgt = params->do_vol() && !params->ytitle2.isEmpty()? 1+params->yTitleFontHeight()+1: 0;
		int	ylabel_wth = 0;
		int	vlabel_wth = 0;
	
		int	xtics       = params->grid||params->xaxis? 1+2: 0;
		int	ytics       = params->grid||params->yaxis? 1+3: 0;
		int	vtics       = params->yaxis&& params->do_vol() ? 3+1: 0;

		qDebug( "done width, height, tics computation" );
	
	
		xdepth_3D      = params->threeD() ? (int)( cos(RAD_DEPTH) * HYP_DEPTH ): 0;
		ydepth_3D      = params->threeD() ? (int)( sin(RAD_DEPTH) * HYP_DEPTH ): 0;
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

		// for the time being: no x labels
		hasxlabels = 0;
		/*	
		if( params->xaxis && hasxlabels ) {
			int biggest     = -MAXINT;
	
			int		len = 0;
			for( i=0; i<num_points; ++i ) {
				biggest = MAX( len, xlbl[i].length() );
			}

			xlabel_hgt = 1+ biggest*params->xAxisFontWidth() +1;
		}
		*/

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
			int jumpout_value = NUM_YPOINTS-1;
			for(int i=1; i<NUM_YPOINTS; ++i ) {
				// if( ypoints[i] > ylbl_interval )
				//	break;
				if( (highest-lowest)/ypoints[i] < ((float)max_num_ylbls-(1.0+1.0))
				    * (float)params->ylabel_density/100.0 ) {
				  jumpout_value = i;
				  break;
				}
			}
			/* gotta go through the above loop to catch the 'tweeners :-| */
			ylbl_interval = params->requested_yinterval != -MAXDOUBLE &&
			  params->requested_yinterval > ypoints[jumpout_value-1] ? 
			  params->requested_yinterval:
			  ypoints[jumpout_value-1];
			// FIXME: This seems to be a total mess: != ???
	
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
	
			if( params->do_vol() ) {
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
		xscl = (float)(graphwidth-xdepth_3Dtotal) / (float)(num_points + (params->do_bar()?2:0));
		yscl = -((float)grapheight) / (float)(highest-lowest);
		if( params->do_vol() ) {
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
    if( params->do_vol() ) {
      VolColor = params->VolColor;
      for(int i=0; i<num_points; ++i )
	if( params->ExtVolColor.count() )
	  ExtVolColor[i] = params->ExtVolColor.color( i );
	else
	  ExtVolColor[i] = VolColor;
    }
    //	ArrowDColor    = gdImageColorAllocate( im, 0xFF,    0, 0 );
    //	ArrowUColor    = gdImageColorAllocate( im,    0, 0xFF, 0 );
    if( params->annotation )
		AnnoteColor = params->annotation->color;


    /* attempt to import optional background image */
    // PENDING(kalle) Put back in
    if( GDC_BGImage ) {
      qDebug("before bgimage");
      drawBackgroundImage();
    }

    qDebug("Color settings coming!");
    prepareColors();
    // PENDING(kalle) Do some sophisticated things that involve QPixmap::createHeuristicMask
    // or Matthias' stuff from bwin
    qDebug("before transparent bg");
    if( params->transparent_bg )
      debug( "Sorry, transparent backgrounds are not supported yet." );
    //     if( params->transparent_bg )
    // 	gdImageColorTransparent( im, BGColor );
    qDebug( "Title text is coming" );   
    titleText();
    qDebug( "start drawing, first the grids" );
    //    drawGridAndLabels(do_ylbl_fractions);
    qDebug("more advanced grids");
    /* interviening set grids */
    /*  0 < setno < num_sets   non-inclusive, they've already been covered */
    if( params->grid && params->threeD() ) {
      qDebug("drawing 3d grids");
      draw3DGrids();
    }
    if( ( params->grid || params->shelf ) &&							/* line color grid at 0 */
		( (lowest < 0.0 && highest > 0.0) ||
		  (lowest < 0.0 && highest > 0.0) ) ) {
      qDebug("drawing shelf grids");
      drawShelfGrids();
    }


    /* x ticks and xlables */
    if( params->grid || params->xaxis )	{
      qDebug("drawing  x ticks and xlabels");
      drawXTicks();
    }

    /* ----- solid poly region (volume) ----- */
    /*  so that grid lines appear under solid */
    if( params->do_vol() ) {
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
			for(i=1; i<num_points-1; ++i ) {
				if( uvol[i] != GDC_NOVALUE )
				  draw_3d_bar( p, PX(i)-hlf_barwdth, PX(i)+hlf_barwdth,
					       PV(0), PV(uvol[i]),
					       0, 0,
					       ExtVolColor[i],
					       ExtVolColor[i] );
			}
			if (uvol[i] != GDC_NOVALUE) {
			  draw_3d_bar( p, PX(i)-hlf_barwdth, PX(i),
				       PV(0), PV(uvol[i]),
				       0, 0,
				       ExtVolColor[i],
				       ExtVolColor[i] );
			}
		} else if( params->type == KCHARTTYPE_COMBO_HLC_AREA   ||
				   params->type == KCHARTTYPE_COMBO_LINE_AREA  ||
				   params->type == KCHARTTYPE_3DCOMBO_LINE_AREA||
				   params->type == KCHARTTYPE_3DCOMBO_HLC_AREA )
		  for(int i=1; i<num_points; ++i ) {
		    if( uvol[i-1] != GDC_NOVALUE && uvol[i] != GDC_NOVALUE )
		      draw_3d_area( p, PX(i-1), PX(i),
				    PV(0), PV(uvol[i-1]), PV(uvol[i]),
				    0, 0,
				    ExtVolColor[i],
				    ExtVolColor[i] );
		  }
		setno = 0;
    }		// volume polys done


    if( params->annotation && params->threeD() ) {		/* back half of annotation line */
      int	x1 = PX(params->annotation->point+(params->do_bar()?1:0)),
	y1 = PY(lowest);
      setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
    num_sets:
      1; // backmost
      p->setPen( AnnoteColor );
      p->drawLine( x1, y1, PX(params->annotation->point+(params->do_bar()?1:0)), PY(lowest) );
      p->drawLine( PX(params->annotation->point+(params->do_bar()?1:0)), PY(lowest),
		   PX(params->annotation->point+(params->do_bar()?1:0)), PY(highest)-2 );
      setno = 0;
    }

    cerr << "Plotting the data comes\n";
    
    /* ---------- start plotting the data ---------- */
    switch( params->type ) {
    case KCHARTTYPE_3DBAR:					/* depth, width, y interval need to allow for whitespace between bars */
    case KCHARTTYPE_BAR:
		/* --------- */
      switch( params->stack_type ) {
      case KCHARTSTACKTYPE_DEPTH:
	for( setno=num_sets-1; setno>=0; --setno )		// back sets first   PX, PY depth
	  for(int i=0; i<num_points; ++i ) {
	    if( CELLEXISTS( setno, i ) )
	      draw_3d_bar( p, PX(i+(params->do_bar()?1:0))-hlf_barwdth, PX(i+(params->do_bar()?1:0))+hlf_barwdth,
			   PY(0), PY( CELLVALUE( setno, i )),
			   xdepth_3D, ydepth_3D,
			   ExtColor[setno][i],
			   params->threeD()? ExtColorShd[setno][i]: ExtColor[setno][i] );
	  }
	setno = 0;
	break;
	
      case KCHARTSTACKTYPE_LAYER:
	{
	  float	lasty[num_points];
	  j = 0;
				//				for( i=0; i<num_points; ++i )
				//					if( CELLEXISTS( j, i ) ) {
				//						lasty[i] = CELLVALUE( j, i );
				//						draw_3d_bar( im, PX(i+(params->do_bar()?1:0))-hlf_barwdth, PX(i+(params->do_bar()?1:0))+hlf_barwdth,
				//										 PY(0), PY(CELLVALUE( j, i )),
				//										 xdepth_3D, ydepth_3D,
				//										 ExtColor[j][i],
				//										 params->threeD()? ExtColorShd[j][i]: ExtColor[j][i] );
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
		barset[k].shclr = params->threeD()? ExtColorShd[j][i]: ExtColor[j][i];
		++k;
	      }
	    }
	    qsort( barset, k, sizeof(struct BS), barcmpr );
	    
	    for( j=0; j<k; ++j ) {
	      draw_3d_bar( p,
			   PX(i+(params->do_bar()?1:0))-hlf_barwdth, PX(i+(params->do_bar()?1:0))+hlf_barwdth,
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
		draw_3d_bar( p, PX(i+(params->do_bar()?1:0))-hlf_barwdth+new_barwdth*j+1,
			     PX(i+(params->do_bar()?1:0))-hlf_barwdth+new_barwdth*(j+1),
			     PY(0), PY(CELLVALUE( j, i )),
			     xdepth_3D, ydepth_3D,
			     ExtColor[j][i],
			     params->threeD()? ExtColorShd[j][i]: ExtColor[j][i] );
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
												  params->threeD()? ExtColorShd[j][i]: ExtColor[j][i] );
							}
					for( j=1; j<num_sets; ++j )
						for( i=1; i<num_points; ++i )
							if( CELLEXISTS(j,i) &&
								CELLEXISTS(j,i-1) ) {
								draw_3d_area( p, PX(i-1), PX(i),
											  PY(lasty[i]), PY(lasty[i-1]+CELLVALUE(j,i-1)), PY(lasty[i]+CELLVALUE(j,i)),
											  xdepth_3D, ydepth_3D,
											  ExtColor[j][i],
											  params->threeD()? ExtColorShd[j][i]: ExtColor[j][i] );
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
										  params->threeD()? ExtColorShd[j][i]: ExtColor[j][i] );
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
										  params->threeD()? ExtColorShd[setno][i]: ExtColor[setno][i] );
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

    if( params->scatter ) {
      drawScatter();
    }




    // overlay with a value and an arrow (e.g., total daily change)
    
    if( params->thumbnail ) {
      drawThumbnails();
    }
    

    /* box it off */
    /*  after plotting so the outline covers any plot lines */
    if( params->border ) {
		p->setPen( LineColor );
		p->drawLine( PX(0), PY(lowest), PX(num_points-1+(params->do_bar()?2:0)), PY(lowest) );

		setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1;
		p->drawLine( PX(0), PY(highest), PX(num_points-1+(params->do_bar()?2:0)),  PY(highest) );
		setno = 0;
    }
    drawBorder();


    if( params->shelf && params->threeD() &&								/* front of 0 shelf */
		( (lowest < 0.0 && highest > 0.0) ||
		  (lowest < 0.0 && highest > 0.0) ) ) {
		int	x2 = PX( num_points-1+(params->do_bar()?2:0) ),
			y2 = PY( 0 );
	
		p->setPen( LineColor );
		p->drawLine( PX(0), PY(0), x2, y2 );		// front line
		setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets:
			1;				// backmost
		// depth for 3Ds
		p->setPen( LineColor );
		p->drawLine( x2, y2, PX(num_points-1+(params->do_bar()?2:0)), PY(0) );
		setno = 0;												// set back to foremost
    }

    if (params->annotation) {
      drawAnnotation();
    }
    
}



/* rem circle:  x = rcos(@), y = rsin(@)	*/






