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
  if (init()== -1) {
    return -1;
  }
  cerr << "Initialization successfull, proceed\n";
  doLabels();
    //  i.e., not up against Y axes
  do_ylbl_fractions =   // %f format not given, or
		( params->ylabel_fmt.isEmpty() ||					//  format doesn't have a %,g,e,E,f or F
		  params->ylabel_fmt.length() == strcspn(params->ylabel_fmt,"%geEfF") );
    if( params->thumbnail ) {
		params->grid = FALSE;
		params->xaxis = FALSE;
		params->yaxis = FALSE;
    }
    debug( "done thumbnails" );
    /* ----- highest & lowest values ----- */
    computeMinMaxValues();
    /* ----- graph height and width within the gif height width ----- */
    /* grapheight/height is the actual size of the scalable graph */
    // scaled, sized, ready
    computeSize();
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
    if(!params->isPie())
    	{
    	drawGridAndLabels(do_ylbl_fractions);
    	qDebug("more advanced grids");
    	}
    /* interviening set grids */
    /*  0 < setno < num_sets   non-inclusive, they've already been covered */
    if( params->grid && params->threeD() && !params->isPie()) 
    {
      qDebug("drawing 3d grids");
      draw3DGrids();
    }
    if( ( params->grid || params->shelf ) && /* line color grid at 0 */
		( (lowest < 0.0 && highest > 0.0) ||
		  (lowest < 0.0 && highest > 0.0) ) && !params->isPie()) {
      qDebug("drawing shelf grids");
      drawShelfGrids();
    }
    /* x ticks and xlables */
    if( (params->grid || params->xaxis) &&!params->isPie() )	
    {
    
      qDebug("drawing  x ticks and xlabels");
       //Commented out because it is prone to math error
      drawXTicks();
    }
    /* ----- solid poly region (volume) ----- */
    /*  so that grid lines appear under solid */
    if( params->do_vol() && !params->isPie() ) {
      qDebug("Doing volume grids");
      drawVolumeGrids();
    }		// volume polys done
    if( params->annotation && params->threeD() &&!params->isPie()) 
    {		/* back half of annotation line */
      qDebug("drawing 3d annotation");
      draw3DAnnotation();
    }

    qDebug("drawing the data!!!");
    drawData();
    setno = 0;
    if( params->scatter && !params->isPie()) 
    {
      qDebug("scatter");
      drawScatter();
    }
    // overlay with a value and an arrow (e.g., total daily change)    
    if( params->thumbnail ) 
    {
      qDebug("scatter");
      drawThumbnails();
    }
    /* box it off */
    /*  after plotting so the outline covers any plot lines */
    if (params->border && !params->isPie()) 
    {
      qDebug("scatter");
      drawBorder();
    }

    if( params->shelf && params->threeD() &&	/* front of 0 shelf */
	( (lowest < 0.0 && highest > 0.0) ||
	  (lowest < 0.0 && highest > 0.0) ) &&!params->isPie())
	   {
      	   draw3DShelf();
    	   }
    
    if (params->annotation && !params->isPie()) 
    {
      qDebug("Draw annotation");
      drawAnnotation();
    }    
}

/* rem circle:  x = rcos(@), y = rsin(@)	*/
