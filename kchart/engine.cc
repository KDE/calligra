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
//#include <iostream>

#include <kdebug.h>

using namespace std;

KChartEngine keng;


int out_graph( short imagewidth,         // no check for an output device that's too small to fit
           short imageheight,        // needed info (labels, etc), could core dump
           QPainter* p,              // paint into this painter
           KChartParameters* params, // the parameters of the chart
           const KoChart::Data& data
           )
{  // temporary stuff for initializing the engine
  keng.params = params;
  keng.data = &data;
  keng.p = p;
  keng.imagewidth = imagewidth;
  keng.imageheight = imageheight;
  return keng.out_graph();
}

int KChartEngine::compute_yintervals() {
    // TODO
    return 0;
}

/*************************************************************/
//
//  Out graph
//
/*************************************************************/
int KChartEngine::out_graph() {
  if (init()== -1) {
    return -1;
  }
  kdDebug(35001) << "Initialization successfull, proceed" << endl;
  doLabels();
    //  i.e., not up against Y axes
  do_ylbl_fractions =   // %f format not given, or
                ( params->ylabel_fmt.isEmpty() ||                                       //  format doesn't have a %,g,e,E,f or F
                  params->ylabel_fmt.length() == strcspn((const char*)params->ylabel_fmt.local8Bit(),"%geEfF") );
    if( params->thumbnail ) {
                params->grid = FALSE;
                params->xaxis = FALSE;
                params->yaxis = FALSE;
    }
    kdDebug(35001) << "done thumbnails" << endl;
    /* ----- highest & lowest values ----- */
    computeMinMaxValues();
    /* ----- graph height and width within the gif height width ----- */
    /* graphheight/height is the actual size of the scalable graph */
    // scaled, sized, ready
    computeSize();
    kdDebug(35001) << "scaled, sized, ready" << endl;


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

    //  ArrowDColor    = gdImageColorAllocate( im, 0xFF,    0, 0 );
    //  ArrowUColor    = gdImageColorAllocate( im,    0, 0xFF, 0 );
    if( params->annotation )
                AnnoteColor = params->annotation->color;
    /* attempt to import optional background image */
        kdDebug(35001) << "before bgimage" << endl;
        drawBackgroundImage();
    kdDebug(35001) << "Color settings coming!" << endl;
    prepareColors();
    // PENDING(kalle) Do some sophisticated things that involve QPixmap::createHeuristicMask
    // or Matthias' stuff from bwin
    kdDebug(35001) << "before transparent bg" << endl;
    if( params->transparent_bg )
      kdDebug(35001) << "Sorry, transparent backgrounds are not supported yet." << endl;
    //     if( params->transparent_bg )
    //  gdImageColorTransparent( im, BGColor );
    kdDebug(35001) << "Title text is coming" << endl;
    titleText();
    kdDebug(35001) << "start drawing, first the grids" << endl;
    if(!params->isPie()) {
          drawGridAndLabels(do_ylbl_fractions);
          kdDebug(35001) << "more advanced grids" << endl;
        }
    /* intervening set grids */
    /*  0 < setno < num_sets   non-inclusive, they've already been covered */
    if( params->grid && params->threeD() && !params->isPie())
    {
      kdDebug(35001) << "drawing 3d grids" << endl;
      draw3DGrids();
    }
    if( ( params->grid || params->shelf ) && /* line color grid at 0 */
                ( (lowest < 0.0 && highest > 0.0) ||
                  (lowest < 0.0 && highest > 0.0) ) && !params->isPie()) {
      kdDebug(35001) << "drawing shelf grids" << endl;
      drawShelfGrids();
    }
    /* x ticks and xlables */
    if( (params->grid || params->xaxis) &&!params->isPie() )
    {

      kdDebug(35001) << "drawing  x ticks and xlabels" << endl;
      drawXTicks();
    }
    /* ----- solid poly region (volume) ----- */
    /*  so that grid lines appear under solid */
    if( params->do_vol() && !params->isPie() ) {
      kdDebug(35001) << "Doing volume grids" << endl;
      drawVolumeGrids();
    }           // volume polys done
    if( params->annotation && params->threeD() &&!params->isPie())
    {           /* back half of annotation line */
      kdDebug(35001) << "drawing 3d annotation" << endl;
      draw3DAnnotation();
    }

    kdDebug(35001) << "drawing the data!!!" << endl;
    drawData();
    setno = 0;
    if( params->scatter && !params->isPie())
    {
        kdDebug(35001) << "scatter" << endl;
      drawScatter();
    }
    // overlay with a value and an arrow (e.g., total daily change)
    if( params->thumbnail )
    {
      kdDebug(35001) << "scatter" << endl;
      drawThumbnails();
    }
    /* box it off */
    /*  after plotting so the outline covers any plot lines */
    if (params->border && !params->isPie())
    {
      kdDebug(35001) << "scatter" << endl;
      drawBorder();
    }

    if( params->shelf && params->threeD() &&    /* front of 0 shelf */
        ( (lowest < 0.0 && highest > 0.0) ||
          (lowest < 0.0 && highest > 0.0) ) &&!params->isPie())
           {
           draw3DShelf();
           }

    if (params->annotation && !params->isPie())
    {
      kdDebug(35001) << "Draw annotation" << endl;
      drawAnnotation();
    }
  return 0;
}

/* rem circle:  x = rcos(@), y = rsin(@)        */
