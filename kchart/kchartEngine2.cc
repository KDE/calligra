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


void kchartEngine::drawAnnotation() {
  int x1 = PX(params->annotation->point+(params->do_bar()?1:0)),
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
  if( *(params->annotation->note) ) {  // any note?
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

void kchartEngine::titleText() {
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
}



void kchartEngine::drawThumbnails() {
#ifdef THUMB_VALS
    /* put thmbl and thumbval over vol and plot lines */
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
    // thumblabel, thumbval
#endif
}

