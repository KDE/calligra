/*
	Written 1998-1999 by Uwe Langenkamp

	This software may be used and distributed according to the terms
	of the GNU Public License, incorporated herein by reference.

	The author may be reached as ul@it-guru.de
*/

/* History:
 *          1999-10-29 Last Change
 *          1999-10-28 Background shading effect added
 *          1999-05-05 Cursor Display Position changed
 *          1999-30-03 Added Units and Names, optimized some functions
 *          1998-16-07 Auto Resize to Polar added
 *          1998-29-03
 *          1998-28-03 Multiple Cursors Added
 *          1998-21-03 Add Custom Cursor Bitmap
 *
 */

#include <qpainter.h>
#include <qbitmap.h>
#include <qpntarry.h>
#include <qapp.h>
#include <qsocknot.h>
#include <qpopmenu.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qkeycode.h>
#include <qtimer.h>
#include <qbttngrp.h>
#include <qfiledlg.h>
#include <qstrlist.h>
#include <qmsgbox.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <sys/types.h>
#include <time.h>

#include "qtai_plot.h"

#define scb_width 16
#define scb_height 16
static unsigned char scb_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define scm_width 16
#define scm_height 16
static unsigned char scm_bits[] = {
   0xff, 0x7f, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40,
   0x01, 0x40, 0x81, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40,
   0x01, 0x40, 0x01, 0x40, 0xff, 0x7f, 0x00, 0x00};

AIPlot::AIPlot(QWidget *parent,const char *name,int b,int t)
  : AIBase( parent, name)
{
  bufsize=b;
  traces=t;
  init();
}

AIPlot::~AIPlot()
{
  int i;

  delete printer;
  delete adhoc;

  for (i=0;i<traces;i++) {
    delete xtrace[i];
    delete ytrace[i];
    delete qa[i];
  }
}

void AIPlot::init()
{
  int i;

  // Get buffers
  // memset(xtrace,0,bufsize*sizeof(double));

  for (i=0;i<8;i++) {
    xtrace[i]=NULL;
    ytrace[i]=NULL;
    qa[i]=NULL;
    qn[i]=0;
  }

  for (i=0;i<traces;i++) {
    trace_enabled[i]=TRUE;
    pcnt[i]=0;
    qa[i] = new QPointArray(bufsize);
    qn[i] = 0;
    xtrace[i]= new double[bufsize];
    ytrace[i]=new double[bufsize];

    // Set data to the new traces, otherwise the program will crash !!!
    memset(xtrace[i],0,bufsize*sizeof(double));
    memset(ytrace[i],0,bufsize*sizeof(double));

	color[i].setRgb( 0, 164, 0 );		
    calced[i]=FALSE;
  }

  // Setup defaults
  printer = new KPrinter;
  bg_buf=NULL;

  plottype=AI_PLOT_TREND;
  graphtype=AI_PLOT_LINE;

  ctrlmode=AI_CTRL_CURS;
  dbuffer=FALSE;

  zoomed_in=0,zooming=FALSE;
  zoomx1[0]=0.0f,zoomy1[0]=0.0f,zoomx2[0]=100.0f,zoomy2[0]=100.0f;

  for (i=0;i<2;i++)
    p_curs[i].set(0.0,0.0);

	used_curs=0;

  c_curs[0].setRgb(127,255,255);
  c_curs[1].setRgb(255,127,255);

#ifdef MYCURS
  QBitmap scb( scb_width, scb_height, scb_bits, TRUE );
  QBitmap scm( scm_width, scm_height, scm_bits, TRUE );

  cs_standard = new QCursor(scb,scm);  // create bitmap cursor
  cs_pan = new QCursor(sizeAllCursor);

  setCursor(*cs_standard);
#endif

  adhoc=new QPopupMenu();
  adhoc->insertItem( tr("Zoom Mode"),this, SLOT(setZoomMode()));
  adhoc->insertItem( tr("Pan Mode"),this, SLOT(setPanMode()));
  adhoc->insertItem( tr("Cursor Mode"),this, SLOT(setCursMode()));
  adhoc->insertSeparator();
  adhoc->insertItem( tr("Zoom Out"),this, SLOT(setZoomOut()));

  xName="";
  yName="";

  xUnit="";
  yUnit="";

  reshade=TRUE;

  x_axis_display_type=X_AXIS_NUMBER;

  xmax=1;
  ymax=1;
  xmin=0;
  ymin=-1;
}

QSizePolicy AIPlot::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

void AIPlot::drawScale( QPainter *p,int direction,int w,int h)
{
  int    x,y;
  int    maxx=4,maxy=4;
  int zx1,zx2,zy1,zy2;
  int    pos;

  //p->fillRect(0,0,w,h,c_bg);

  p->setPen(c_grid);				

  if (direction & HORIZONTAL) {

    zx1=(int)(zoomx1[zoomed_in]*(double)w/100.0f);
    zx2=(int)((zoomx2[zoomed_in]-zoomx1[zoomed_in])*(double)w/100.0f);

    p->setWindow(zx1,0,zx2,h);

    // Draw horizontal scale
    for (x=0;x<=maxx;x++) {
      pos = (int)(x*w/maxx);
      if (pos>=zx1 && pos<=zx1+zx2) {
	p->drawLine(pos,(int)0,
		    pos,(int)h/2);		
      }
    }
  }
 else if (direction & VERTICAL) {

   zy1=(int)(zoomy1[zoomed_in]*(double)h/100.0f);
   zy2=(int)((zoomy2[zoomed_in]-zoomy1[zoomed_in])*(double)h/100.0f);

   p->setWindow(0,zy1,w,zy2);

   // Draw vertical scale
    for (y=0;y<=maxy;y++) {
      pos=(int)(y*h/maxy);
      if (pos>=zy1 && pos<=zy1+zy2) {
	p->drawLine((int)w/2,pos,(int)w,pos);
      }
    }
 }
}

void AIPlot::drawLabel( QPainter *p,int direction,int w,int rw,int h,int rh)
{
  int   x,y;
  // Number of Scale Divisors
  int    maxx=4,maxy=4;

  int   tpos;
  QString str="";
  QFont font("Helvetica",10);
  QFontMetrics fm(font);
  double rzb,rze;
  double lw,lh;
  static double lzx1,lzy1,lzx2,lzy2;
  time_t tt=0;
  struct tm *ts=NULL;
			

  lzx1=zoomx1[zoomed_in],lzy1=zoomy1[zoomed_in];
  lzx2=zoomx2[zoomed_in],lzy2=zoomy2[zoomed_in];

  //p->fillRect(0,0,w,h,c_bg);

  p->setPen(c_grid);				

  // Calculate begin and end of zoom area to the real world
  if (direction & HORIZONTAL) {
      rzb=(double)lzx1*((double)xmax-(double)xmin)/
	(double)100.0f + (double)xmin;
      rze=(double)lzx2*((double)xmax-(double)xmin)/
	(double)100.0f + (double)xmin;
  } else if (direction & VERTICAL) {
      rzb=(double)(100.0f-lzy1)*((double)ymax-(double)ymin)/
	(double)100.0f + (double)ymin;
      rze=(double)(100.0f-lzy2)*((double)ymax-(double)ymin)/
	(double)100.0f + (double)ymin;
  }
  p->setFont(font);

  if (direction & HORIZONTAL) {

    for (x=0;x<=maxx;x++) {
	  double zahl=(double)(xmin+(xmax-xmin)*(double)x/(double)maxx);
		
	  if ((x_axis_display_type & X_AXIS_NUMBER)==X_AXIS_NUMBER) {

		str.sprintf("%.2f",(double)zahl);    		
	  } else if ((x_axis_display_type & X_AXIS_DATE)==X_AXIS_DATE) {

		if ((x_axis_display_type & X_AXIS_UK)==X_AXIS_UK) {
		} else {

			tt=(time_t)zahl;
			if (tt<=0)
				return;
			ts = localtime(&tt);
			if (ts==NULL)
				return;

			str.sprintf("%02d:%02d:%02d\0",
				ts->tm_hour,ts->tm_min,ts->tm_sec);    			
		}
	  }

	  lw=rw*100.0f/(lzx2-lzx1);
      tpos=(int)(x*lw/maxx-lzx1*lw/100.0f-fm.width(str)/2+(w-rw)/2);

      if (!(tpos<0 || tpos+fm.width(str)>w))
		p->drawText(tpos,0+fm.height(),str);

    }
  }
 else if (direction & VERTICAL) {

    for (y=0;y<=maxy;y++) {
	  double zahl=(double)(ymax+(ymin-ymax)*(double)y/(double)maxy);
      str.sprintf("%.2f",zahl);

      lh=rh*100.0f/(lzy2-lzy1);
      tpos=(int)(y*lh/maxy-lzy1*lh/100.0f+fm.height()/2+(h-rh)/2);

      if (!(tpos-fm.height()<0 || tpos+fm.height()>h))
		p->drawText(w-fm.width(str),tpos,str);

    }
 }

}

void AIPlot::drawGrid( QPainter *p )
{
  int x,y,i;

  // Number of Scale Divisors
  int    maxx=8,maxy=8;

  int      w;
  int      h;

  w=width();
  h=height();

#define SHADE_BG
#ifdef SHADE_BG
  //p->drawPixmap(0,0,*bg_buf);
#else
  p->fillRect(0,0,w,h,c_bg);
#endif
  QPen pen(c_grid,1,QPen::DotLine);
  p->setPen(pen);				

  if (zoomed_in>0) {
    p->setWindow(
		 (int)(zoomx1[zoomed_in]*(double)w/100.0f),
		 (int)(zoomy1[zoomed_in]*(double)h/100.0f),
		 (int)((zoomx2[zoomed_in]-zoomx1[zoomed_in])*(double)w/100.0f),
		 (int)((zoomy2[zoomed_in]-zoomy1[zoomed_in])*(double)h/100.0f));
  } else {
    p->setWindow(0,0,w,h);	// defines coordinate system
  }

  switch(plottype) {
  case AI_PLOT_TREND:
    for (x=1;x<maxx;x++) {
      p->drawLine((int)(x*w/maxx),(int)0,
		  (int)(x*w/maxx),(int)h);		
    }

    for (y=1;y<maxy;y++) {
      p->drawLine((int)0,(int)(y*h/maxy),
		  (int)w,(int)(y*h/maxy));		
    }
    break;

  case AI_PLOT_POLAR:
	
    pen=QPen(c_grid,1,SolidLine);
    p->setPen(pen);				
    for (i=0;i<=4;i++)
      p->drawArc((int)((double)i*(double)w/8.0f),
		 (int)((double)i*(double)h/8.0f),
		 (int)((double)w-2.0f*(double)i*(double)w/8.0f),
		 (int)((double)h-2.0f*(double)i*(double)h/8.0f),
		 0,360*16);
	break;
  }

  pen=QPen(c_grid,1,DotLine);
  p->setPen(pen);				
  p->drawRect(0,0,w,h);
}

// New 1999/03/29 Old draw routine, should only used until new
// print and draw has been designed
void AIPlot::drawIt( QPainter *p )
{
     QString name="";
     int fact=10;
     QColor   c;

     int      w;
     int      h;

	 int fh;
	 int fw;

     w=width()*fact;
     h=height()*fact;

     QFont    font("Helvetica",10);
     QFontMetrics fm(font);
     QString  str;
     int      i,j,x,y,cur,zero;
     int      rx=0,ry=0,rw=w,rh=h;
     double   w100=(double)w/100.0f;
     double   h100=(double)h/100.0f;

     double   dx10=w100/10;
     double   rx10=(xmax-xmin)/10;

     static   double ozoomx1=0,ozoomy1=0,ozoomx2=0,ozoomy2=0;
     static double lzx1,lzy1,lzx2,lzy2;

     lzx1=zoomx1[zoomed_in],lzy1=zoomy1[zoomed_in];
     lzx2=zoomx2[zoomed_in],lzy2=zoomy2[zoomed_in];

	 time_t tt=0;
	 struct tm *ts=NULL;
	
	 int max_y=0, min_y=0;
     //p->fillRect(0,0,w,h,c_bg);

     p->setViewport(50,30,width()-80,height()-80);
     p->setClipRect(50,30,width()-80,height()-80);

     drawGrid(p);

     if (zoomed_in>0) {
       p->setWindow((int)(zoomx1[zoomed_in]*w100),
		     (int)(zoomy1[zoomed_in]*h100),
		     (int)((zoomx2[zoomed_in]-zoomx1[zoomed_in])*(double)w100),
		     (int)((zoomy2[zoomed_in]-zoomy1[zoomed_in])*(double)h100));
       max_y=(int)(zoomy1[zoomed_in]*h100);
       min_y=(int)((zoomy2[zoomed_in]-zoomy1[zoomed_in])*(double)h100);
     } else {
       p->setWindow(0,0,w,h);	// defines coordinate system
       max_y=0;
       min_y=h;
     }

     switch (graphtype) {
     case AI_PLOT_LINE:

       // Set the initial min_y and max_y values
       min_y=(int)(h-(ytrace[0][0]-ymin)/(ymax-ymin)*h);
       max_y=(int)(h-(ytrace[0][0]-ymin)/(ymax-ymin)*h);

       for (i=0;i<traces;i++) {
	
	 if (isEnabled(i)) {
	   // Set Pen with Line width, not usefull
	   // consumes too much CPU time
	   // QPen pen=QPen(color[i],2);
	   // p->setPen( pen );

	   // Set to line width 1
	   p->setPen( color[i] );

	   if (qa[i]!=NULL && qn[i]>0) {
	     if (xtrace[i]!=NULL && ytrace[i]!=NULL) {

			// Check if this is already calculated :
			if (!calced[i]) {
				pcnt[i]=0;
	
				// Parts of New Formula 1999/03/12
				// Formula factors and summands to reduce calc. in loop
				double fx; // factor x
				double sx; // summand x
				double fy; // factor y
				double sy; // summand y

				double *sytrace;

				fx=1/(xmax-xmin)*w;
				sx=-xmin/(xmax-xmin)*w;

				fy=-1/(ymax-ymin)*h;
				sy=h+ymin/(ymax-ymin)*h;

				// Single y trace to reduce indication in loop
				sytrace=(double*)(ytrace[i]);

				for (j=0;j<qn[i];j++) {
		
					// Original Formula disabled 1999/03/12
					// x=(int)((xtrace[i][j]-xmin)/(xmax-xmin)*w);
					// y=(int)(h-(ytrace[i][j]-ymin)/(ymax-ymin)*h);
			
					// New Formula 1999/03/12
					x=(int)(sx+fx*xtrace[i][j]);
					y=(int)(sy+fy*sytrace[j]);

					if (y>max_y) max_y=y;
					if (y<min_y) min_y=y;

					if (!(xtrace[i][j]<(xmin-rx10) || xtrace[i][j]>(xmax+rx10))) {
					  qa[i]->setPoint(pcnt[i]++,x,y);
					}
				}
				for (cur=0;cur<2;cur++) {
				  // BAD PERFORMANCE due to large pow and
				  // sqrt calcs in calcTraceCoords
				  // DISABLED 1999-10-27
				  // p_curs[cur]=calcTraceCoords(p_curs[cur].x(),p_curs[cur].y());
				  // New Function SetTraceCoords
				  // by former trace setting
				  // p_curs[cur]=
				}

				calced[i]=TRUE;

			}

			p->drawPolyline(*qa[i],0,pcnt[i]);
	     }
	   }
	 }
       }

     break;

     case AI_PLOT_BAR:

       for (i=0;i<traces;i++) {

	 if (isEnabled(i)) {
	   p->setPen( color[i] );
	
	   pcnt[i]=0;
	   if (qa[i]!=NULL && qn[i]>0) {
	     if (xtrace[i]!=NULL && ytrace[i]!=NULL) {
	       for (j=0;j<qn[i];j++) {
		 x=(int)((xtrace[i][j]-xmin)/(xmax-xmin)*w);
		 y=(int)(h-(ytrace[i][j]-ymin)/(ymax-ymin)*h);
		 zero=(int)(h-(-ymin)/(ymax-ymin)*h);
		 p->drawLine(x,zero,x,y);
	       }
	     }
	   }
	 }
       }
       break;
     }

     p->setClipRect(0,0,width(),height());

     if (zooming || zoomed_in>0) {
		p->setPen(c_comm);
		switch(plottype) {
		case AI_PLOT_POLAR:
			
			#ifdef RADAR_PANEL // Ha Ha
			p->drawEllipse((int)(azx1*w100),
					(int)(azy1*h100),
					(int)((azx2-azx1)*w100),
					(int)((azy2-azy1)*h100));
			break;
			#endif
			
		default:
			p->drawRect((int)(azx1*w100),
					(int)(azy1*h100),
					(int)((azx2-azx1)*w100),
					(int)((azy2-azy1)*h100));
			break;
       }
     }

     p->setViewport(0,0,width(),height());
     p->setWindow(0,0,width(),height());

     p->setWindow(0,0,20*fact,(height()-80)*fact);
     p->setViewport(30,30,20,height()-80);
     drawScale(p,VERTICAL,20*fact,(height()-80)*fact);

     p->setWindow(0,0,30,height()-30);
     p->setViewport(0,0,30,height()-30);

     drawLabel(p,VERTICAL,30,30,height()-30,height()-80);

     // add the y-label
     p->setPen(c_grid);			

     if (yName.length()>0 && yUnit.length()>0) {
       name.sprintf("%s [%s]",yName.latin1(),yUnit.latin1());
     } else if (yName.length()>0) {
       name = yName;
     } else if (yUnit.length()>0) {
       name.sprintf("[%s]",yUnit.latin1());
     }
	
	 p->drawText(5,0+fm.height(),name);

     p->setWindow(0,0,(width()-80)*fact,(20*fact));
     p->setViewport(50,height()-50,width()-80,20);

     drawScale(p,HORIZONTAL,(width()-80)*fact,20*fact);

     p->setWindow(0,0,(width()-30),30);
     p->setViewport(30,height()-30,width()-30,30);

     drawLabel(p,HORIZONTAL,width()-30,width()-80,30,30);

     // add the x-label
     p->setPen(c_grid);		

     if (xName.length()>0 && xUnit.length()>0) {
       name.sprintf("%s [%s]",xName.latin1(),xUnit.latin1());
     } else if (xName.length()>0) {
       name = xName;
     } else if (xUnit.length()>0) {
       name.sprintf("[%s]",xUnit.latin1());
     }
	
	 p->drawText(width()-50-fm.width(name)-15,25,name);

     // Paint the global Information State
     p->setViewport(50,30,width()-80,height()-80);

     p->setWindow(0,0,width()-80,height()-80);
     p->setPen(c_comm);
     p->setFont(font);

     switch(ctrlmode){
     case AI_CTRL_ZOOM:
		str=tr("Zoom Mode");
        break;
     case AI_CTRL_PAN:
		str=tr("Pan Mode");
        break;
     case AI_CTRL_CURS:
        str=tr("Cursor Mode");
        break;
     }
     p->drawText(5,-fm.height()/2,str);
	
	 for (i=0;i<2;i++) {
		if (p_curs[i].trace()>=0 && p_curs[i].index()>=0){
			if ((x_axis_display_type & X_AXIS_NUMBER)==X_AXIS_NUMBER) {
				str.sprintf("%.2f %s, %.2f %s",
					(double)xtrace[p_curs[i].trace()][p_curs[i].index()],
					xUnit.latin1(),
					(double)ytrace[p_curs[i].trace()][p_curs[i].index()],
					yUnit.latin1());				
			} else if ((x_axis_display_type & X_AXIS_DATE)==X_AXIS_DATE) {
				if ((x_axis_display_type & X_AXIS_UK)==X_AXIS_UK) {
				} else {
					tt=(time_t)xtrace[p_curs[i].trace()][p_curs[i].index()];
		 			if (tt<=0)
						return;

					ts = localtime(&tt);
					if (ts==NULL)
						return;
					
					str.sprintf("%02d:%02d:%02d %s, %.2f %s",
							ts->tm_hour,ts->tm_min,ts->tm_sec,
							xUnit.latin1(),
							(double)ytrace[p_curs[i].trace()][p_curs[i].index()],
							yUnit.latin1());
			
				}
			}
	
			p->setPen(c_curs[i]); // Original Cursor Color
			fh=fm.height();
			fw=fm.width(str);
			//p->drawRect(100,-i*(fh/2+5),fw,fh-1);

			p->drawRect(90,10-fh-(fh+5)/2-i*((fh+10)/2),fw+20,fh-2);
			p->setPen( color[p_curs[i].trace()]);
			// HORIZONTAL
			// p->drawText((int)100+i*100,-fm.height()/2,str);
			// VERTICAL
			p->drawText((int)100,5-(fh+3)/2-i*((fh+3)/2+5),str);
		}
	}

	 /* SHOWS THE ZOOM WINDOW SIZE IN GRAPH
	    if(zoomed_in>0) {
	    sprintf(str,"Zoom Area %d (%.1f%%, %.1f%%, %.1f%%, %.1f%%)",
	    zoomed_in,
	    zoomx1[zoomed_in],zoomy1[zoomed_in],
	    zoomx2[zoomed_in],zoomy2[zoomed_in]);
	
	    p->drawText(width()-80-(fm.width(str)-5),20,str);
	    }
	 */

     // ############### Draw Cursor #################
     w=width();
     h=height();

     double fx=w/(lzx2-lzx1);
     double fy=h/(lzy2-lzy1);
	
     p->setViewport(50,30,width()-80,height()-80);
     p->setWindow(0,0,w,h);
     for (i=0;i<2;i++) {

       double px=p_curs[i].x()-lzx1;
       double py=p_curs[i].y()-lzy1;
       p->setPen(c_curs[i]);
       p->drawLine((int)(px*fx-5),(int)(py*fy),
		   (int)(px*fx+5),(int)(py*fy));
       p->drawLine((int)(px*fx),(int)(py*fy-5),
		   (int)(px*fx),(int)(py*fy+5));
     }
}

//
// Called when the widget needs to be updated.
//

void AIPlot::paintEvent( QPaintEvent * )
{

  //return;

  QPoint   qp(0,0);
  QPainter paint;
  QPainter tmp; // for double buffering

  // Seems to be O.K., but dangerous in case of stack overflows
  //QPixmap  pix(width(),height());

  int    n;
  static int     bw=-1,bh=-1; // BG Buffer
  QPainter bg_p;
  QColor   shade_c;

  int w=width();
  int h=height();

#define SHADE_BG
#ifdef SHADE_BG
  //if (bh!=h || bw!=w || bg_buf==NULL) {
  if (reshade) {
    double step=100.0/(double)h;
    double colval=150.0;

    delete bg_buf;
    bg_buf=new QPixmap(w,h);

    tmp.begin( bg_buf );			// begin painting

    for (n=0;n<h;n++) {
      shade_c.setRgb(64,64,(int)colval);
      colval+=step;
      tmp.setPen(shade_c);
      tmp.drawLine(0,n,w,n);
    }

    tmp.end();				// painting done

    bh=h;bw=w;
    reshade=FALSE;
  }
#endif
  if (autoscale) {
    if (width()>height()) {
		resize(height(),height());
		return;
    } else if (width()<height()){
		resize(width(),width());
		return;
    }	
  }

  QPixmap  *pix=new QPixmap(width(),height());

  paint.begin(this);

  if (dbuffer) {
    //QRect area;

    tmp.begin( pix );			// begin painting
    tmp.drawPixmap(qp,*bg_buf);
    //tmp.setWindow(0,0,width(),height());
    drawIt( &tmp );				
    tmp.end();				// painting done

    // Draw Pix
    //area=QRect(50,0,width()-50,height()-50);
    //qp=QPoint(50,0);

    //bitBlt(this,0,0,&pix);

    paint.drawPixmap(qp,*pix);

  } else {

    drawIt( &paint );		        // without double buffer
  }

  paint.end();

  delete pix;
}

void AIPlot::setDoubleBuffer(bool on)
{
  dbuffer=on;
}

void AIPlot::resetCalced()
{
  int i;

  for (i=0;i<traces;i++) {
    calced[i]=FALSE;
  }
}

void AIPlot::setData(int trace,int qn,double *xp,double *yp)
{
  int i;

  if (trace>traces-1)   // Not enough traces prepared
    return;

  calced[trace]=FALSE;

  if (!zooming) {
    this->qn[trace]=qn;
    memcpy((char*)this->xtrace[trace],(char*)xp,qn*sizeof(double));
    memcpy((char*)this->ytrace[trace],(char*)yp,qn*sizeof(double));
  }

  // Hide Cursor, maybe we should use a p_curs..->hide()
  for (i=0;i<2;i++)
    p_curs[i].set(p_curs[i].x(),p_curs[i].y());
}

void   AIPlot::setXName(QString xName)
{
  this->xName=xName;
}

void   AIPlot::setYName(QString yName)
{
    this->yName=yName;
}

void   AIPlot::setXUnit(QString xUnit)
{
  this->xUnit=xUnit;
}

void   AIPlot::setYUnit(QString yUnit)
{
  this->yUnit=yUnit;
}

void AIPlot::setRanges(double xmn,double xmx,double ymn,double ymx)
{
  setXRange(xmn,xmx);
  setYRange(ymn,ymx);
}

void AIPlot::setXRange(double xmn,double xmx)
{
  xmin=xmn;
  xmax=xmx;
}

void AIPlot::setYRange(double ymn,double ymx)
{
  ymin=ymn;
  ymax=ymx;
}

double AIPlot::getXmax()
{
  return xmax;
}

double AIPlot::getYmax()
{
  return ymax;
}

double AIPlot::getXmin()
{
  return xmin;
}

double AIPlot::getYmin()
{
  return ymin;
}

AICursor AIPlot::calcTraceCoords(double x,double y)
{
  AICursor curs(x,y);

  double realx,realy;
  double dist,adist;
  double cursx,cursy;

  int i,t,min_dy=-1,min_dx=-1;

  // Transform x,y to realx,realy
  realx=(x*(xmax-xmin)/100)+xmin;
  realy=((100-y)*(ymax-ymin)/100)+ymin;

  // Compare Traces
  dist=-1;
  for (t=0;t<traces;t++) {
    for (i=0;i<qn[0];i++) {
      adist=sqrt(  pow((xtrace[t][i]-realx)/(xmax-xmin),2)
		 + pow((ytrace[t][i]-realy)/(ymax-ymin),2));
      if (dist==-1) {
	dist=adist;
      } else if (adist<dist) {
	dist=adist;
	min_dx=i;
	min_dy=t;
      }
    }
  }

  if (min_dx!=-1 && min_dy!=-1) {

    // Get the cursx,cursy values and retransform
    cursx=(xtrace[min_dy][min_dx]-xmin)*100/(xmax-xmin);
    cursy=100-(ytrace[min_dy][min_dx]-ymin)*100/(ymax-ymin);

    // Set the cursx,cursy values
    curs.set(cursx,cursy,min_dy,min_dx);
  }

  return curs;
}

// AIPlot Mouse Event Methods

void AIPlot::mousePressEvent( QMouseEvent *e )
{
  double rcursx,rcursy;

  if (ctrlmode==AI_CTRL_ZOOM &&
      zoomed_in<7 && e->button()==LeftButton) {
    rzoomx1=zoomx1[zoomed_in]+(zoomx2[zoomed_in]-zoomx1[zoomed_in])
      *(double)(e->x()-50)/(width()-80);
    rzoomy1=zoomy1[zoomed_in]+(zoomy2[zoomed_in]-zoomy1[zoomed_in])
      *(double)(e->y()-30)/(height()-80);

    zooming=TRUE;
  }

  if (ctrlmode==AI_CTRL_PAN &&
      zoomed_in>0 && e->button()==LeftButton) {

    panx1=100.0f*(double)(e->x()-50)/(width()-80);
    pany1=100.0f*(double)(e->y()-30)/(height()-80);
    panning=TRUE;
    //setCursor(*cs_pan);
  }

  if (ctrlmode==AI_CTRL_CURS &&
      e->button()==LeftButton) {
    rcursx=zoomx1[zoomed_in]+(zoomx2[zoomed_in]-zoomx1[zoomed_in])
      *(double)(e->x()-50)/(width()-80);
    rcursy=zoomy1[zoomed_in]+(zoomy2[zoomed_in]-zoomy1[zoomed_in])
      *(double)(e->y()-30)/(height()-80);

    // Compare Cursors
    {
      double dist,adist;

      int i,min_dy=-1,min_dx=-1;

      // Transform x,y to realx,realy
      //realx=(rcursx*(xmax-xmin)/100)+xmin;
      //realy=((100-rcursy)*(ymax-ymin)/100)+ymin;

      dist=-1;
      used_curs=0;
      for (i=0;i<2;i++) {
	adist=sqrt(  pow(p_curs[i].x()-rcursx,2)
		     + pow(p_curs[i].y()-rcursy,2));
	if (dist==-1) {
	  dist=adist;
	} else if (adist<dist) {
	  dist=adist;
	  used_curs=i;
	}
      }
    }

    p_curs[used_curs].set(rcursx,rcursy);
    //p_curs[0]=calcTraceCoords(rcursx,rcursy);

    repaint(FALSE);
  }

  if (e->button()==RightButton) {
    QPoint p(e->x(),e->y());
    adhoc->popup(mapToGlobal(p));
  }
}

void AIPlot::mouseMoveEvent( QMouseEvent *e )
{
  QPainter paint;
  double panx,pany;
  double rcursx,rcursy;

  if (ctrlmode==AI_CTRL_ZOOM && zooming) {
    rzoomx2=zoomx1[zoomed_in]+(zoomx2[zoomed_in]-zoomx1[zoomed_in])
      *(double)(e->x()-50)/(width()-80);
    rzoomy2=zoomy1[zoomed_in]+(zoomy2[zoomed_in]-zoomy1[zoomed_in])
      *(double)(e->y()-30)/(height()-80);

    if (e->state() & ControlButton) {
      if ((rzoomx2-rzoomx1)>(rzoomy2-rzoomy1)) {
	rzoomy2=rzoomy1+(rzoomx2-rzoomx1);
      } else {
	rzoomx2=rzoomx1+(rzoomy2-rzoomy1);
      }
    }

    if (rzoomx1>rzoomx2)
      {azx1=rzoomx2;azx2=rzoomx1;}
    else
      {azx1=rzoomx1;azx2=rzoomx2;}
    if (rzoomy1>rzoomy2)
      {azy1=rzoomy2;azy2=rzoomy1;}
    else
      {azy1=rzoomy1;azy2=rzoomy2;}


    if (azx1<zoomx1[zoomed_in])azx1=zoomx1[zoomed_in];
    if (azx2>zoomx2[zoomed_in])azx2=zoomx2[zoomed_in];
    if (azy1<zoomy1[zoomed_in])azy1=zoomy1[zoomed_in];
    if (azy2>zoomy2[zoomed_in])azy2=zoomy2[zoomed_in];

    repaint(FALSE);
  }

  if (ctrlmode==AI_CTRL_CURS) {
    rcursx=zoomx1[zoomed_in]+(zoomx2[zoomed_in]-zoomx1[zoomed_in])
      *(double)(e->x()-50)/(width()-80);
    rcursy=zoomy1[zoomed_in]+(zoomy2[zoomed_in]-zoomy1[zoomed_in])
      *(double)(e->y()-30)/(height()-80);

    p_curs[used_curs].set(rcursx,rcursy);
    // OLD DISABLED // RE-ENABLED 1999-10-27
    p_curs[used_curs]=calcTraceCoords(rcursx,rcursy);

    repaint(FALSE);
  }

  if (ctrlmode==AI_CTRL_PAN && zoomed_in>0 && panning) {

    double ozx1=zoomx1[zoomed_in],ozy1=zoomy1[zoomed_in];
    double ozx2=zoomx2[zoomed_in],ozy2=zoomy2[zoomed_in];

    panx2=100.0f*(double)(e->x()-50)/(width()-80);
    pany2=100.0f*(double)(e->y()-30)/(height()-80);

    panx=((double)((double)panx2-(double)panx1)
	  *((double)zoomx2[zoomed_in]-(double)zoomx1[zoomed_in])/100.0f);
    pany=((double)((double)pany2-(double)pany1)
	  *((double)zoomy2[zoomed_in]-(double)zoomy1[zoomed_in])/100.0f);

    zoomx1[zoomed_in]-=panx;
    zoomx2[zoomed_in]-=panx;
    zoomy1[zoomed_in]-=pany;
    zoomy2[zoomed_in]-=pany;

    if (zoomx1[zoomed_in]<0) {
      zoomx1[zoomed_in]=0;
      zoomx2[zoomed_in]=ozx2-ozx1;
    }

    if (zoomx2[zoomed_in]>100.0f) {
      zoomx2[zoomed_in]=100.0f;
      zoomx1[zoomed_in]=100.0f-(ozx2-ozx1);
    }

    if (zoomy1[zoomed_in]<0) {
      zoomy1[zoomed_in]=0;
      zoomy2[zoomed_in]=ozy2-ozy1;
    }

    if (zoomy2[zoomed_in]>100.0f) {
      zoomy2[zoomed_in]=100.0f;
      zoomy1[zoomed_in]=100.0f-(ozy2-ozy1);
    }

    panx1=panx2;
    pany1=pany2;

    azx1=zoomx1[zoomed_in];
    azx2=zoomx2[zoomed_in];
    azy1=zoomy1[zoomed_in];
    azy2=zoomy2[zoomed_in];

    repaint(FALSE);
  }
}

void AIPlot::mouseReleaseEvent( QMouseEvent *e )
{
  double rcursx,rcursy;

  if (ctrlmode==AI_CTRL_ZOOM &&
      zoomed_in<7 && e->button()==LeftButton) {
    rzoomx2=zoomx1[zoomed_in]+(zoomx2[zoomed_in]-zoomx1[zoomed_in])
      *(double)(e->x()-50)/(width()-80);
    rzoomy2=zoomy1[zoomed_in]+(zoomy2[zoomed_in]-zoomy1[zoomed_in])
      *(double)(e->y()-30)/(height()-80);

    if (e->state() & ControlButton) {
      if ((rzoomx2-rzoomx1)>(rzoomy2-rzoomy1)) {
	rzoomy2=rzoomy1+(rzoomx2-rzoomx1);
      } else {
	rzoomx2=rzoomx1+(rzoomy2-rzoomy1);
      }
    }

    zoomed_in++;
    if (rzoomx1>rzoomx2)
      {zoomx1[zoomed_in]=rzoomx2;zoomx2[zoomed_in]=rzoomx1;}
    else
      {zoomx1[zoomed_in]=rzoomx1;zoomx2[zoomed_in]=rzoomx2;}
    if (rzoomy1>rzoomy2)
      {zoomy1[zoomed_in]=rzoomy2;zoomy2[zoomed_in]=rzoomy1;}
    else
      {zoomy1[zoomed_in]=rzoomy1;zoomy2[zoomed_in]=rzoomy2;}

    if (zoomx1[zoomed_in]<zoomx1[zoomed_in-1])
      zoomx1[zoomed_in]=zoomx1[zoomed_in-1];
    if (zoomx2[zoomed_in]>zoomx2[zoomed_in-1])
      zoomx2[zoomed_in]=zoomx2[zoomed_in-1];
    if (zoomy1[zoomed_in]<zoomy1[zoomed_in-1])
      zoomy1[zoomed_in]=zoomy1[zoomed_in-1];
    if (zoomy2[zoomed_in]>zoomy2[zoomed_in-1])
      zoomy2[zoomed_in]=zoomy2[zoomed_in-1];

    if (fabs(zoomx1[zoomed_in]-zoomx2[zoomed_in])<1.0
	|| fabs(zoomy1[zoomed_in]-zoomy2[zoomed_in])<1.0)
      zoomed_in--;

    if (zoomed_in<=0)
      zoomed_in=0;

    // Zoom in
    zooming=FALSE;
    repaint(FALSE);
  }

  if (ctrlmode==AI_CTRL_CURS &&
      e->button()==LeftButton) {
    rcursx=zoomx1[zoomed_in]+(zoomx2[zoomed_in]-zoomx1[zoomed_in])
      *(double)(e->x()-50)/(width()-80);
    rcursy=zoomy1[zoomed_in]+(zoomy2[zoomed_in]-zoomy1[zoomed_in])
      *(double)(e->y()-30)/(height()-80);

    //p_curs[0].set(rcursx,rcursy);
    p_curs[used_curs]=calcTraceCoords(rcursx,rcursy);

    repaint(FALSE);
  }

  if (ctrlmode==AI_CTRL_PAN &&
      e->button()==LeftButton) {
    panning=FALSE;
    //setCursor(*cs_standard);
    repaint(FALSE);
  }
}

void AIPlot::setPlotColor(int bar,int r,int g,int b)
{
  color[bar].setRgb(r,g,b);
}

void AIPlot::setPlotColor(int bar,QColor c)
{
  color[bar].setRgb(c.red(),c.green(),c.blue());
}

void AIPlot::setPlotType(int type)
{
  plottype=type;
}

int AIPlot::getPlotType(void)
{
  return plottype;
}

void AIPlot::setGraphType(int type)
{
  graphtype=type;
}

int AIPlot::getGraphType(void)
{
  return graphtype;
}

void AIPlot::setZoomMode()
{
  ctrlmode=AI_CTRL_ZOOM;
  repaint(FALSE);
}

void AIPlot::setZoomOut()
{
  zoomed_in--;
  if (zoomed_in<=0)
    zoomed_in=0;

  azx1=zoomx1[zoomed_in];
  azy1=zoomy1[zoomed_in];
  azx2=zoomx2[zoomed_in];
  azy2=zoomy2[zoomed_in];
  zooming=FALSE;
  repaint(FALSE);
}

void AIPlot::setPanMode()
{
  ctrlmode=AI_CTRL_PAN;
  repaint(FALSE);
}

void AIPlot::setCursMode()
{
  ctrlmode=AI_CTRL_CURS;
  repaint(FALSE);
}

void AIPlot::setEnabled(int graph,bool on)
{
  trace_enabled[graph]=on;
}

bool AIPlot::isEnabled(int graph)
{
  return trace_enabled[graph];
}

void AIPlot::resizeEvent(QResizeEvent *e)
{
	int i;
	reshade=TRUE;
	for (i=0;i<8;i++) {
		calced[i]=FALSE;
	}
	resize(e->size().width(),e->size().height());
}

void AIPlot::setXAxisDisplay(int type)
{
  x_axis_display_type=type;
}

// AICursor Class and Functions

AICursor::AICursor(double x, double y)
{
  set(x,y);
  xpos=0;
  ypos=0;
  t=0;
  t_index=0;
}

void AICursor::set(double x, double y,int trace,int index)
{
  xpos=x;
  ypos=y;
  t=trace;
  t_index=index;
}

int AICursor::trace(void)
{
  return t;
}

int AICursor::index(void)
{
  return t_index;
}

double AICursor::x(void)
{
  return xpos;
}

double AICursor::y(void)
{
  return ypos;
}

#include "qtai_plot.moc"
