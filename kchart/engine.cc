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

#include "kchartparams.h"

#include <qfont.h>
#include <qcolor.h>
#include <qpainter.h>

#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h> //PENDING(kalle) Remove?

// retrieve the value of a cell
//#define CELLVALUE( row, col ) data.cell( (row), (col) ).value.doubleValue()

// check whether a cell has a value
//#define CELLEXISTS( row, col ) data.cell( (row), (col) ).exists

class kchartEngine {
public:
  KChartParameters* params;
  KChartData* data;
  QPainter *p;
  int imagewidth;
  int imageheight;  
  int doLabels();
  int minmaxValues( int num_points,
		  int num_sets,
		  float *uvol,
		  float &highest, 
		  float &lowest,
		  float &vhighest,
		  float &vlowest);
  int out_graph();

  int compute_yintervals();

  inline bool CELLEXISTS( int row, int col ) {
    return  data->cell(row,col).exists;
  };
  inline double CELLVALUE(int row, int col) {
    return data->cell(row,col).value.doubleValue();
  }
  
private:
  int num_sets;
  int num_points;
  int graphwidth;
  int grapheight;
  float	xorig, yorig, vyorig;
  float	yscl;
  float	vyscl;
  float	xscl;
  float vhighest;
  float	vlowest;
  float	highest;
  float	lowest;
  float	ylbl_interval;
  int num_lf_xlbls;
  int xdepth_3Dtotal;
  int ydepth_3Dtotal;
  int xdepth_3D;		// affects PX()
  int ydepth_3D;		// affects PY() and PV()
  int hlf_barwdth;		// half bar widths
  int hlf_hlccapwdth;		// half cap widths for HLC_I_CAP and DIAMOND
  int annote_len, annote_hgt;

  /* args */
  int			setno;				// affects PX() and PY()
  // PENDING(kalle) Get rid of this
  float		*uvol;
#define MAXNUMPOINTS 100
  QColor BGColor, LineColor, PlotColor, GridColor, VolColor;
  QColor ExtVolColor[100];
  //  QColor ExtVolColor[num_points];  
  QColor ThumbDColor, ThumbLblColor, ThumbUColor;
    // ArrowDColor,ArrowUColor,
  QColor AnnoteColor;
  //  QColor ExtColor[num_sets][num_points];
  QColor ExtColor[100][100];
  // shade colors only with 3D
  //	intExtColorShd[threeD?1:num_sets][threeD?1:num_points]; // compiler limitation
  QColor ExtColorShd[100][100];
  //  QColor ExtColorShd[num_sets][num_points];
  bool hasxlabels;
  //  QPointArray volpoly( 4 );

  int init();
};

kchartEngine keng;


int out_graph( short imagewidth,         // no check for an output device that's too small to fit
	   short imageheight,        // needed info (labels, etc), could core dump
	   QPainter* p,	             // paint into this painter
	   KChartParameters* params, // the parameters of the chart
	   const KChartData& data
	   )
{
  // temporary stuff for initializing the engine
  keng.params = params;
  keng.data = &data;
  keng.p = p;
  keng.imagewidth = imagewidth;
  keng.imageheight = imageheight;
  return keng.out_graph();
}

int kchartEngine::doLabels() {
  // Finally, the x labels are taken from the first row
  QArray<QString> xlbl( data->cols() );
#ifdef NOXLABELSFORNOW
  debug( "Creating xlbl with %d entries", data->cols() );
  for( int labels = 0; labels < data->cols(); labels++ ) {
    debug( "Retrieving value at position %d", labels );
    const KChartValue& cellval = data->cell( 0, labels );
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
};



int kchartEngine::minmaxValues( 
		  int num_points,
		  int num_sets,
		  float *uvol,
		  float &highest, 
		  float &lowest,
		  float &vhighest,
		  float &vlowest) {
    if( params->stack_type == KCHARTSTACKTYPE_SUM )	// need to walk sideways
      for(int j=0; j<num_points; ++j ) {
	float set_sum = 0.0;
	for(int i=0; i<num_sets; ++i ) {
	  debug( "vor dem crash" );
	  if( CELLEXISTS( i, j ) ) {
	    debug( "nach dem crash" );
	    set_sum += CELLVALUE( i, j );
	    highest = MAX( highest, set_sum );
	    lowest  = MIN( lowest,  set_sum );
	  }
	}
      } else if( params->stack_type == KCHARTSTACKTYPE_LAYER ) // need to walk sideways
	for(int j=0; j<num_points; ++j ) {
	  float neg_set_sum = 0.0, pos_set_sum = 0.0;
	  for(int i=0; i<num_sets; ++i )
	    if( CELLEXISTS( i, j ) )
	      if( CELLVALUE( i, j ) < 0.0 )
		neg_set_sum += CELLVALUE( i, j );
	      else
		pos_set_sum += CELLVALUE( i, j );
	  lowest  = MIN( lowest,  MIN(neg_set_sum,pos_set_sum) );
	  highest = MAX( highest, MAX(neg_set_sum,pos_set_sum) );
} else
	  for(int i=0; i<num_sets; ++i )
	    for(int j=0; j<num_points; ++j ) {
	      debug( "Vor dem crash" );
	      if( CELLEXISTS( i, j ) ) {
		debug( "nach dem crash" );
		highest = MAX( CELLVALUE( i, j ), highest );
		lowest  = MIN( CELLVALUE( i, j ), lowest );
	      }
	    }
    
    debug( "done computation highest and lowest value" );

    if( params->scatter )
      for(int i=0; i<params->num_scatter_pts; ++i ) {
	highest = MAX( ((params->scatter)+i)->val, highest );
	lowest  = MIN( ((params->scatter)+i)->val, lowest  );
      }
    if( params->do_vol() ) { // for now only one combo set allowed
      // vhighest = 1.0;
      // vlowest  = 0.0;
      for(int j=0; j<num_points; ++j )
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
}

int kchartEngine::init() {
  // initializations
  yscl = 0.0;
  vyscl = 0.0;
  xscl = 0.0;
  vhighest = -MAXFLOAT;
  vlowest  = MAXFLOAT;
  highest  = -MAXFLOAT;
  lowest   = MAXFLOAT;
  ylbl_interval  = 0.0;
  num_lf_xlbls   = 0;
  xdepth_3Dtotal = 0;
  ydepth_3Dtotal = 0;
  xdepth_3D      = 0;	       
  ydepth_3D      = 0;	 
  hlf_barwdth	   = 0;		
  hlf_hlccapwdth = 0;	
  annote_len     = 0;
  annote_hgt     = 0;
  setno = 0;
  hasxlabels = false;
  


  // For now, we are assuming that the data is in columns with no labels at all
  // Ergo, num_sets is the number of rows
  num_sets = data->rows();

  // No data sets left -> bail out
  if( num_sets < 1 ) {
    debug( "No data" );
    return -1;
  }

  // And num_points is the number of columns
  num_points = data->cols();
 
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
  doLabels();

    char		num_hlc_sets = params->has_hlc_sets() ? num_sets : 0;
    //  i.e., not up against Y axes
    char		do_ylbl_fractions =   // %f format not given, or
		( params->ylabel_fmt.isEmpty() ||					//  format doesn't have a %,g,e,E,f or F
		  params->ylabel_fmt.length() == strcspn(params->ylabel_fmt,"%geEfF") );


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


    /* ----- highest & lowest values ----- */
    keng.minmaxValues(num_points, num_sets, uvol,
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
    for(int j=0; j<num_sets; ++j )
      for(int i=0; i<num_points; ++i )
	if( params->ExtColor.count() ) {			
	  cerr << "Ext color\n";
	  // changed by me, BL
	  //QColor ext_clr = params->ExtColor.color( num_points*j+i );			
	  QColor ext_clr = params->ExtColor.color( (num_points*j+i) % params->ExtColor.count());
	  ExtColor[j][i]            = ext_clr;
	  if( params->threeD() )
	    ExtColorShd[j][i]     = QColor( ext_clr.red() / 2, ext_clr.green() / 2, ext_clr.blue() / 2 );
	}
	else if( params->SetColor.count() ) {
	  QColor set_clr = params->SetColor.color( j );
	  ExtColor[j][i]     = QColor( set_clr );
	  if( params->threeD() )
	    ExtColorShd[j][i] = QColor( set_clr.red() / 2, set_clr.green() / 2, set_clr.blue() / 2 );
	}
	else {
	  ExtColor[j][i]     = PlotColor;
	  if( params->threeD() )
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
		for(int i=-1; i<=1; i+=2 ) { // -1, 1
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
					p->drawLine( x2, y2, PX(num_points-1+(params->do_bar()?2:0)), y2 );
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
			
				if( params->do_vol() && params->yaxis2 ) {
					char	vylbl[16];
					/* opposite of PV(y) */
					sprintf( vylbl,
							 !params->ylabel2_fmt.isEmpty()? params->ylabel2_fmt: QString( "%.0f" ),
							 ((float)(PY(tmp_y)+(setno*ydepth_3D)-vyorig))/vyscl );
			
					setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
				num_sets:
					1; // backmost
					p->setPen( GridColor );
					p->drawLine( PX(num_points-1+(params->do_bar()?2:0)), PY(tmp_y),
								 PX(num_points-1+(params->do_bar()?2:0))+3, PY(tmp_y) );
					if( atof(vylbl) == 0.0 )									/* rounding can cause -0 */
						strcpy( vylbl, "0" );
					p->setPen( label2color );
					p->setFont( params->yAxisFont() );
					p->drawText( PX(num_points-1+(params->do_bar()?2:0))+6,
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
		if( params->grid && params->threeD() ) {
			setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
		num_sets:
			1;			// backmost
			p->setPen( GridColor );
			p->drawLine( PX(0), PY(lowest), PX(num_points-1+(params->do_bar()?2:0)), PY(lowest) );
			setno = 0;											// set back to foremost
		}
	
		/* vy axis title */
		if( params->do_vol() && !params->ytitle2.isEmpty() ) {
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
    if( params->grid && params->threeD() ) {
		for( setno=(params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1) - 1;
			 setno > 0;
			 --setno ) {
			p->setPen( GridColor );
			p->drawLine( PX(0), PY(lowest), PX(0), PY(highest) );
			p->drawLine( PX(0), PY(lowest), PX(num_points-1+(params->do_bar()?2:0)), PY(lowest) );
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
		p->drawLine( x2, y2, PX(num_points-1+(params->do_bar()?2:0)), y2 );
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
	
		for(int i=0; i<num_points+(params->do_bar()?2:0); ++i ) {
		  if( (i%(1+num_points/num_xlbls) == 0) ||   // labels are regulated
				(num_xlbls >= num_points)         ||
				GDC_xlabel_spacing == MAXSHORT ) {
				int xi = params->do_bar()? i-1: i;
		
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
		
				  /*		       	  
				if( !do_bar || (i>0 && xi<num_points) )
				  // no label stuff yet
					if( params->xaxis && hasxlabels ) {
						// waiting for GDCImageStringUpNL()
#define	LBXH		params->xAxisFontHeight()
#define LBXW        params->xAxisFontWidth()						
						int		xlen = 0;
						short	xstrs_num = cnt_nl( xlbl[xi], &xlen );
						//   char	sub_xlbl[xlen+1];
						//  int		xlbl_strt = -1+ PX((float)i+(float)(do_bar?((float)num_points/(float)num_xlbls):0.0)) - (int)((float)(LBXH-2)*((float)xstrs_num/2.0));
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
				  */
			}
		}
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
			int	scat_x = PX( ((params->scatter)+i)->point + (params->do_bar()?1:0) ),
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
				if( !params->do_bar() )
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
				if( !params->do_bar() )
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
		p->drawLine( PX(0), PY(lowest), PX(num_points-1+(params->do_bar()?2:0)), PY(lowest) );

		setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1;
		p->drawLine( PX(0), PY(highest), PX(num_points-1+(params->do_bar()?2:0)),  PY(highest) );
		setno = 0;
    }
    if( params->border ) {
		int	x1, y1, x2, y2;
	
		x1 = PX(0);
		y1 = PY(highest);
		x2 = PX(num_points-1+(params->do_bar()?2:0));
		y2 = PY(lowest);
		p->setPen( LineColor );
		p->drawLine( x1, PY(lowest), x1, y1 );
	
		setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1;
		p->setPen( LineColor );
		p->drawLine( x1, y1, PX(0), PY(highest) );
		// if( !params->grid || do_vol || params->thumbnail )					// grid leaves right side Y open
		{
			p->setPen( LineColor );
			p->drawLine( x2, y2, PX(num_points-1+(params->do_bar()?2:0)), PY(lowest) );
			p->drawLine( PX(num_points-1+(params->do_bar()?2:0)), PY(lowest),
						 PX(num_points-1+(params->do_bar()?2:0)), PY(highest) );
		}
		setno = 0;
    }

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

    if( params->annotation ) {			/* front half of annotation line */
		int		x1 = PX(params->annotation->point+(params->do_bar()?1:0)),
			y1 = PY(highest);
		int		x2;
		// front line
		p->setPen( AnnoteColor );
		p->drawLine( x1, PY(lowest)+1, x1, y1 );
		if( params->threeD() ) { // on back plane
			setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1;
			x2 = PX(params->annotation->point+(params->do_bar()?1:0));
			// prspective line
			p->setPen( AnnoteColor );
			p->drawLine( x1, y1, x2, PY(highest) );
		} else { // for 3D done with back line
			x2 = PX(params->annotation->point+(params->do_bar()?1:0));
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






