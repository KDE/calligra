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
#include <stdio.h> //PENDING(kalle) Remove?


void kchartEngine::drawData() {



}

void kchartEngine::drawBars() {
  int i, j;
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
}
