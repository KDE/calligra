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
#include <kdebug.h>


int KChartEngine::doLabels() {
  // Finally, the x labels are taken from the first row
  QArray<QString> xlbl( data->usedCols() );
#ifdef NOXLABELSFORNOW
  kdDebug(35001) <<  "Creating xlbl with " << data->usedCols() << " entries" << endl;
  for( int labels = 0; labels < data->usedCols(); labels++ ) {
    kdDebug(35001) <<  "Retrieving value at position "  << labels << endl;
    const KChartValue& cellval = data->cell( 0, labels );
    kdDebug(35001) <<  "type of field " << labels << " in row 0 is "
                                   << QVariant::typeToName( cellval.value.type() ) << endl;
    if( !cellval.exists ) {
      kdDebug(35001) <<  "No value for x label in col " << labels << endl;
      continue;
    }
    if( cellval.value.type() != QVariant::String ) {
          //if( cellval.value.type() != QVariant::CString ) {
      kdDebug(35001) <<  "Value for x label in col " labels
                                         << " is not a string" << endl;
      continue;
    }

    kdDebug(35001) <<  "Setting label " << labels << " to "
                                   << cellval.value.toString() << endl;
    //          QString l = cellval.value.stringValue();
    //xlbl.at( labels ) = cellval.value.stringValue();
    xlbl.at( labels ) = cellval.value.toString();
    kdDebug(35001) << "Done setting label" << endl;
    //hasxlabels = true;
  }
#endif
  kdDebug(35001) << "labels read" << endl;
  return 0;
};




void KChartEngine::drawBorder()
{
  if( params->border )
    {
      p->setPen( LineColor );
      p->drawLine( PX(0), PY(lowest), PX(num_points-1+(params->do_bar()?2:0)), PY(lowest) );

      setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1;
      p->drawLine( PX(0), PY(highest), PX(num_points-1+(params->do_bar()?2:0)),  PY(highest) );
      setno = 0;
    }
  if( params->border )
        {
          int   x1, y1, x2, y2;

          x1 = PX(0);
          y1 = PY(highest);
          x2 = PX(num_points-1+(params->do_bar()?2:0));
          y2 = PY(lowest);
          p->setPen( LineColor );
          p->drawLine( x1, PY(lowest), x1, y1 );

          setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1;
          p->setPen( LineColor );
          p->drawLine( x1, y1, PX(0), PY(highest) );
          // if( !params->grid || do_vol || params->thumbnail )                                 // grid leaves right side Y open
          {
                p->setPen( LineColor );
                p->drawLine( x2, y2, PX(num_points-1+(params->do_bar()?2:0)), PY(lowest) );
                p->drawLine( PX(num_points-1+(params->do_bar()?2:0)), PY(lowest),
                                         PX(num_points-1+(params->do_bar()?2:0)), PY(highest) );
          }
          setno = 0;
        }

}

void KChartEngine::drawGridAndLabels(bool do_ylbl_fractions)
{
  // to get rid of this

  /* ----- backmost first - grid & labels ----- */
  if (!( params->grid || params->yaxis ))
        {
          // there is nothing to draw in this case
          return;
        }

  /* draw grid lines & y label(s) */
  float tmp_y = lowest;

  QColor labelcolor = params->YLabelColor;
  QColor label2color = params->YLabel2Color;
  /* step from lowest to highest puting in labels and grid at interval points */
  /* since now "odd" intervals may be requested, try to step starting at 0,   */
  /* if lowest < 0 < highest                                                  */
  if ( ylbl_interval ) // avoid endless loop
    for(int i=-1; i<=1; i+=2 ) { // -1, 1
      if( i == -1 )
                if( lowest >= 0.0 ) //  all pos plotting
                  continue;
                else
                  tmp_y = QMIN( 0, highest ); // step down to lowest

      if( i == 1 )
                if( highest <= 0.0 ) // all neg plotting
                  continue;
                else
                  tmp_y = QMAX( 0, lowest ); // step up to highest


      //                        if( !(highest > 0 && lowest < 0) )
      // doesn't straddle 0
      //                                {
      //                                if( i == -1 )
      // only do once: normal
      //                                        continue;
      //                                }
      //                        else
      //                                tmp_y = 0;

      do {      // while( (tmp_y (+-)= ylbl_interval) < [highest,lowest] )
                int             n, d, w;
                char    *price_to_str( float, int*, int*, int*, const char* );
                char    nmrtr[3+1], dmntr[3+1], whole[8];
                char    all_whole = ylbl_interval<1.0? FALSE: TRUE;
                char    *ylbl_str;

                if(params->stack_type==KCHARTSTACKTYPE_PERCENT)
                  {
                        ylbl_str= price_to_str( tmp_y,&n,&d,&w,"%.0f%% ");
                        if(!params->ylabel_fmt.isEmpty())
                          {
                                int len=params->ylabel_fmt.length();
                        //remove %g
                        QString tmp=params->ylabel_fmt.right(len-3);

                                strcat(ylbl_str,tmp.latin1());
                          }
                  }
                else
                  {
                        ylbl_str= price_to_str( tmp_y,&n,&d,&w,
                                                                        do_ylbl_fractions? 0L: params->ylabel_fmt.latin1() );
                  }

                if( do_ylbl_fractions && params->stack_type!=KCHARTSTACKTYPE_PERCENT)
                  {
                        sprintf( nmrtr, "%d", n );
                        sprintf( dmntr, "%d", d );
                        sprintf( whole, "%d", w );
                  }

                //kdDebug(35001) << "drawing 1" << endl;

                if( params->grid )
                  {
                        int     x1, x2, y1, y2;
                        // int  gridline_clr = tmp_y == 0.0? LineColor: GridColor;
                        // tics
                        x1 = PX(0);
                        y1 = PY(tmp_y);
                        p->setPen( GridColor );
                        p->drawLine( x1-2, y1, x1, y1 );
                        setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
                  num_sets:1;// backmost
                        x2 = PX(0);
                        y2 = PY(tmp_y);
                        // w/ new setno
                        p->setPen( GridColor );
                        p->drawLine( x1, y1, x2, y2 );          // depth for 3Ds
                        p->setPen( GridColor );
                        p->drawLine( x2, y2, PX(num_points-1+(params->do_bar()?2:0)), y2 );
                        setno = 0;
                        // set back to foremost
                  }

                //kdDebug(35001) << "drawing 2" << endl;

                // PENDING(kalle) Originally, here was always used one
                // font smaller than params->yAxisFont. Do that again?
                if( params->yaxis )
                  if( do_ylbl_fractions && params->stack_type!=KCHARTSTACKTYPE_PERCENT)
                        {
                          if( w || (!w && !n && !d) )
                                {
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


                          //kdDebug(35001) << "drawing 3" << endl;

                          // PENDING( original uses a 1 step smaller
                          // font here. Do that, too?
                          if( n )
                                {
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
                        }
                  else
                        {

                          p->setPen( labelcolor );
                          p->setFont( params->yAxisFont() );
                          p->drawText( PX(0)-2-strlen(ylbl_str)*params->yAxisFontWidth(),
                                                   PY(tmp_y)-params->yAxisFontHeight()/2,
                                                   ylbl_str );
                        }

                //kdDebug(35001) << "drawing 4" << endl;

                if( params->do_vol() && params->yaxis2 ) {
                  char  vylbl[16];
                  /* opposite of PV(y) */
                  sprintf( vylbl,
                                   !params->ylabel2_fmt.isEmpty()? params->ylabel2_fmt.latin1(): "%.0f",
                                   ((float)(PY(tmp_y)+(setno*ydepth_3D)-vyorig))/vyscl );

                  setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
                num_sets:
                  1; // backmost
                  p->setPen( GridColor );
                  p->drawLine( PX(num_points-1+(params->do_bar()?2:0)), PY(tmp_y),
                                           PX(num_points-1+(params->do_bar()?2:0))+3, PY(tmp_y) );
                  if( atof(vylbl) == 0.0 )                      /* rounding can cause -0 */
                        strcpy( vylbl, "0" );
                  p->setPen( label2color );
                  p->setFont( params->yAxisFont() );
                  p->drawText( PX(num_points-1+(params->do_bar()?2:0))+6,
                                           PY(tmp_y)-params->yAxisFontHeight()/2,
                                           vylbl );
                  setno = 0;
                }
        //kdDebug(35001) << "i=" << i << " tmp_y=" << tmp_y << " ylbl_interval=" << ylbl_interval << " highest=" << highest << " lowest=" << lowest << endl;
      }
      while( ((i>0) && ((tmp_y += ylbl_interval) <= highest)) ||
                         ((i<0) && ((tmp_y -= ylbl_interval) > lowest)) );
    }

  //kdDebug(35001) << "drawing 5" << endl;

    /* catch last (bottom) grid line - specific to an "off" requested interval */
  if( params->grid && params->threeD() ) {
        setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
  num_sets:1;                   // backmost
        p->setPen( GridColor );
        p->drawLine( PX(0), PY(lowest), PX(num_points-1+(params->do_bar()?2:0)), PY(lowest) );
        setno = 0;                                                                                      // set back to foremost
  }

  /* vy axis title */
  drawVYAxisTitle();
  /* y axis title */
  drawYAxisTitle();

}

void KChartEngine::drawVYAxisTitle() {
  /* vy axis title */
  if( params->do_vol() && !params->ytitle2.isEmpty() ) {
        //you can choose YTitle2Color and VolColor
        /*QColor        titlecolor = params->YTitle2Color== Qt::black ?
        VolColor: params->YTitle2Color;*/
        QColor titlecolor= params->YTitle2Color;
        // PENDING(kalle) Check whether this really prints correctly
        p->setFont( params->yTitleFont() );
        p->setPen( titlecolor );
        p->rotate( -90 );
        //when you make a rotate axes are rotating
        // in drawText x=>-y and y=>x

        /*p->drawText( imagewidth-(1+params->yTitleFontHeight()),
                   params->ytitle2.length()*params->yTitleFontWidth()/2 +
                   graphheight/2, params->ytitle2 );
      */
        p->drawText(-(params->ytitle2.length()*params->yTitleFontWidth()/2 +
                                  graphheight/2) ,(imagewidth-(1+params->yTitleFontHeight())),
                                params->ytitle2 );
        p->rotate( 90 );
  }
}


void KChartEngine::drawYAxisTitle() {
  if( params->yaxis && !params->ytitle.isEmpty() )
    {
      int ytit_len = params->ytitle.length()*params->yTitleFontWidth();
      /*QColor  titlecolor = params->YTitleColor==Qt::black?
                PlotColor: params->YTitleColor;*/
      QColor titlecolor = params->YTitleColor;
      p->setPen( titlecolor );
      p->setFont( params->yTitleFont() );
      p->rotate(-90);
      //p->drawText( 0, imageheight/2 + ytit_len/2, params->ytitle );
      p->drawText(- imageheight/2 -ytit_len/2,
                                  params->yTitleFontHeight(),params->ytitle);

      p->rotate(90);
    }
}

void KChartEngine::draw3DGrids()
{
  for( setno=(params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets: 1) - 1;setno > 0; --setno )
        {
          p->setPen( GridColor );
          p->drawLine( PX(0), PY(lowest), PX(0), PY(highest) );
          p->drawLine( PX(0), PY(lowest), PX(num_points-1+(params->do_bar()?2:0)), PY(lowest) );
        }
  setno = 0;
}


void KChartEngine::drawShelfGrids()
{
  int   x1, x2, y1, y2;
  // tics
  x1 = PX(0);
  y1 = PY(0);
  p->setPen( LineColor );
  p->drawLine( x1-2, y1, x1, y1 );
  setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
 num_sets:1;// backmost
  x2 = PX(0);
  y2 = PY(0);// w/ new setno
  p->setPen( LineColor );
  p->drawLine( x1, y1, x2, y2 );// depth for 3Ds
  p->drawLine( x2, y2, PX(num_points-1+(params->do_bar()?2:0)), y2 );
  setno = 0;                                                                                    // set back to foremost
}


void KChartEngine::drawXTicks()
{
  if( num_points == 0 ) // if there are no data points or x labels, there is no need in drawing ticks
        return;

  /* each xlbl + avg due to num_lf_xlbls */
  int neededspace = ( (params->xlabel_spacing==MAXSHORT?0:params->xlabel_spacing)+params->xAxisFontHeight() +
                                          (num_lf_xlbls*(params->xAxisFontHeight()-1))/num_points );
  ASSERT( neededspace );
  int num_xlbls = /* maximum x lables that'll fit */
    neededspace ? ( graphwidth / neededspace ) : 1;

  if( num_xlbls < 1 )
    return; // if we cannot draw any x labels, we go out of here

  QColor labelcolor = params->XLabelColor;

  for(int i=0; i<num_points+(params->do_bar()?2:0); ++i ) {
        if( (i%(1+num_points/num_xlbls) == 0) ||   // labels are regulated
                (num_xlbls >= num_points)         ||
                params->xlabel_spacing == MAXSHORT ) {
          int xi = params->do_bar()? i-1: i;

          if( params->grid ) {
                int x1, x2, y1, y2;
                // tics
                x1 = PX(i);
                y1 = PY(lowest);
                p->setPen( GridColor );
                p->drawLine( x1, y1, x1,  y1+2 );
                setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:num_sets:1; // backmost
                x2 = PX(i);
                y2 = PY(lowest);
                p->setPen( GridColor );
                p->drawLine( x1, y1, x2,  y2 );         // depth perspective
                p->drawLine( x2, y2, x2,  PY(highest) );
                setno = 0;                                                                                      // reset to foremost
          }


          if( !params->do_bar() || (i>0 && xi<num_points) )
                // no label stuff yet
                if( params->xaxis && params->hasxlabel ) {
                  // waiting for GDCImageStringUpNL()
#define LBXH params->xAxisFontHeight()
#define LBXW params->xAxisFontWidth()
                  int xlen = 0;
                  short xstrs_num = cnt_nl( params->xlbl[xi].latin1(), &xlen );
                  //char sub_xlbl[xlen+1];
                  //int xlbl_strt = -1+ PX((float)i+(float)(do_bar?((float)num_points/(float)num_xlbls):0.0)) - (int)((float)(LBXH-2)*((float)xstrs_num/2.0));
                  int xlbl_strt = -1+ PX(i) - (int)((float)(LBXH-2)*((float)xstrs_num/2.0));
                  QString currentfulllabel = params->xlbl[xi];
                  xlen = -1;
                  xstrs_num = -1;
                  QStringList sublabels = QStringList::split( '\n', currentfulllabel );
                  for( QStringList::Iterator sublabelit = sublabels.begin();
                           sublabelit != sublabels.end(); ++sublabelit ) {
                        ++xstrs_num;
                        p->setFont( params->xAxisFont() );
                        p->setPen( labelcolor );
                        p->rotate( 90 );
                        /*p->drawText( xlbl_strt + (LBXH-1)*xstrs_num,
                          PY(lowest) + 2 + 1 + LBXW*xlen,
                          (*sublabelit).latin1() );*/

                        p->drawText((15+PY(lowest) + 2 + 1 + LBXW*xlen) ,
                                                -(xlbl_strt + (LBXH-1)*xstrs_num),
                                                (*sublabelit).latin1() );
                        p->rotate( -90 );

                  }
#undef LBXW
#undef LBXH
                }

        }
  }
}


void KChartEngine::drawVolumeGrids()
{
  int i=0;
  setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
 num_sets: 1; // backmost
  if( params->type == KCHARTTYPE_COMBO_HLC_BAR    ||
      params->type == KCHARTTYPE_COMBO_LINE_BAR   ||
      params->type == KCHARTTYPE_3DCOMBO_LINE_BAR ||
      params->type == KCHARTTYPE_3DCOMBO_HLC_BAR ) {
    //if( uvol[0] != GDC_NOVALUE )
        if(CELLEXISTS( 3, 0 ) )
          draw_3d_bar( p, PX(0), PX(0)+hlf_barwdth,
                                   PV(0), PV(CELLVALUE( 3, 0 )), 0, 0,
                                   ExtVolColor[0],
                                   ExtVolColor[0] );
        for(i=1; i<num_points-1; ++i )
          {
                if( CELLEXISTS( 3, i ) )
                  draw_3d_bar( p, PX(i)-hlf_barwdth, PX(i)+hlf_barwdth,
                                           PV(0), PV(CELLVALUE( 3, i )),
                                           0, 0,
                                           ExtVolColor[i],
                                           ExtVolColor[i] );
          }

        if (CELLEXISTS( 3, i ))
          {
                draw_3d_bar( p, PX(i)-hlf_barwdth, PX(i),
                                         PV(0), PV(CELLVALUE( 3, i )),
                                         0, 0,
                                         ExtVolColor[i],
                                         ExtVolColor[i] );
          }
  }
  else if( params->type == KCHARTTYPE_COMBO_HLC_AREA   ||
                   params->type == KCHARTTYPE_COMBO_LINE_AREA  ||
                   params->type == KCHARTTYPE_3DCOMBO_LINE_AREA||
                   params->type == KCHARTTYPE_3DCOMBO_HLC_AREA )
    for(int i=1; i<num_points; ++i )
          {
        if( CELLEXISTS( 3, i-1 ) && CELLEXISTS( 3, i ) )
                  draw_3d_area( p, PX(i-1), PX(i),
                                                PV(0), PV(CELLVALUE( 3, i-1 )), PV(CELLVALUE(3,i)),
                                                0, 0,
                                                ExtVolColor[i],
                                                ExtVolColor[i] );
          }
  setno = 0;
}


void KChartEngine::draw3DAnnotation()
{
  int x1 = PX(params->annotation->point+(params->do_bar()?1:0));
  int y1 = PY(lowest);
  setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets:
 num_sets:1; // backmost
  p->setPen( AnnoteColor );
  p->drawLine( x1, y1, PX(params->annotation->point+(params->do_bar()?1:0)), PY(lowest) );
  p->drawLine( PX(params->annotation->point+(params->do_bar()?1:0)), PY(lowest),
                           PX(params->annotation->point+(params->do_bar()?1:0)), PY(highest)-2);
  setno = 0;
}

void KChartEngine::draw3DShelf()
{
  int x2 = PX( num_points-1+(params->do_bar()?2:0) ),y2 = PY( 0 );

  p->setPen( LineColor );
  p->drawLine( PX(0), PY(0), x2, y2 );          // front line
  setno = params->stack_type==KCHARTSTACKTYPE_DEPTH? num_hlc_sets? num_hlc_sets: num_sets:1;                            // backmost
  // depth for 3Ds
  p->setPen( LineColor );
  p->drawLine( x2, y2, PX(num_points-1+(params->do_bar()?2:0)), PY(0) );
  setno = 0;// set back to foremost
}
