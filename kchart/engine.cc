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
      drawVolumeGrids();

    }		// volume polys done


    if( params->annotation && params->threeD() ) {		/* back half of annotation line */
      qDebug("drawing 3d annotation");
      draw3DAnnotation();
    }

    qDebug("drawing the data!!!");
    drawData();

 

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
      qDebug("Draw annotation");
      drawAnnotation();
    }
    
}



/* rem circle:  x = rcos(@), y = rsin(@)	*/






