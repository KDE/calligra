#include "engine.h"
#include "engine2.h"
#include "enginehelper.h"
#include "enginedraw.h"

#include "kchartparams.h"
#include "kchartEngine.h"

#include <qfont.h>
#include <qcolor.h>
#include <qpainter.h>

#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <kdebug.h>


int KChartEngine::init() {
  // initializations
  yscl = 0.0;
  vyscl = 0.0;
  xscl = 0.0;
  vhighest = -FLT_MAX;
  vlowest  = FLT_MAX;
  highest  = -FLT_MAX;
  lowest   = FLT_MAX;
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
  //hasxlabels = false;


  // For now, we are assuming that the data is in columns with no labels at all
  // Ergo, num_sets is the number of rows
  if ( data->usedRows() == 0 )   // Data from KSpread
    num_sets = data->rows();
  else
    num_sets = data->usedRows();

  // No data sets left -> bail out
  if( num_sets < 1 ) {
	kdDebug(35001) << "No data" << endl;
	return -1;
  }

  //num_hlc_sets = params->has_hlc_sets() ? num_sets : 0;
  //you are 2 curves
  num_hlc_sets = params->has_hlc_sets() ? 1 : 0;

  // And num_points is the number of columns
  if ( data->usedCols() == 0 ) // Data from KSpread
    num_points = data->cols();
  else
    num_points = data->usedCols();

  /* idiot checks */
  if( imagewidth <= 0 || imageheight <=0 || !p  )
    return -1;
  if( num_points <= 0 ) {
    kdDebug(35001) << "No Data Available" << endl;
    return -1;
  }
  return 1;
}





/* ---------- scatter points  over all other plots ---------- */
/* scatters, by their very nature, don't lend themselves to standard array of points */
/* also, this affords the opportunity to include scatter points onto any type of chart */
/* drawing of the scatter point should be an exposed function, so the user can */
/*  use it to draw a legend, and/or add their own */
void KChartEngine::drawScatter() {
  QColor *scatter_clr = new QColor[params->num_scatter_pts];
  QPointArray ct( 3 );

  for(int i=0; i<params->num_scatter_pts; ++i )
        {
        int hlf_scatterwdth = (int)( (float)(PX(2)-PX(1))
                * (((float)(((params->scatter)+i)->width)/100.0)/2.0) );
        int scat_x = PX( ((params->scatter)+i)->point + (params->do_bar()?1:0) ),
                scat_y = PY( ((params->scatter)+i)->val );

        if( ((params->scatter)+i)->point >= num_points ||// invalid point
                ((params->scatter)+i)->point <  0 )
                continue;
        scatter_clr[i] = ((params->scatter)+i)->color;

        switch( ((params->scatter)+i)->ind )
                {
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
  delete [] scatter_clr;
}
