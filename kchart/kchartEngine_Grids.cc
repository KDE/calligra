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




void kchartEngine::drawBorder() {
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
}

void kchartEngine::drawGridAndLabels(bool do_ylbl_fractions) {
  // to get rid of this
  int i,j;

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

}


void kchartEngine::draw3DGrids() {
  for( setno=(params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1) - 1;
       setno > 0;
       --setno ) {
    p->setPen( GridColor );
    p->drawLine( PX(0), PY(lowest), PX(0), PY(highest) );
    p->drawLine( PX(0), PY(lowest), PX(num_points-1+(params->do_bar()?2:0)), PY(lowest) );
  }
  setno = 0;  
}


void kchartEngine::drawShelfGrids() {
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
  setno = 0;										       	// set back to foremost  
}


void kchartEngine::drawXTicks() {
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


void kchartEngine::drawVolumeGrids() {
  int i;
  int setno;
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
}


void kchartEngine::draw3DAnnotation() {
  int setno;
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
