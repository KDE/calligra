/*
	Written 1998-1999 by Uwe Langenkamp

	This software may be used and distributed according to the terms
	of the GNU Public License, incorporated herein by reference.

	The author may be reached as ul@it-guru.de
*/

/* History:
 *          98/19/03 Last Change
 *          98/19/03 Moving Grid added
 *
 */

#include <qpainter.h>
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

#include "qtai_strip.h"

#define FACTOR 1

AIStrip::AIStrip(QWidget *parent,const char *name,int b,int t)
  : AIPlot(parent,name,b,t)
{
  int i;
  for (i=0;i<traces;i++)
    qn[0]=0;
  xrange=1.0;
  w_of_x=10;
  xmax=1;
  xmax=0;
}

void AIStrip::addData(double xp,double *yp)
{
  int i;

  if (qn[0]>=bufsize) {
    qn[0]=bufsize-1;
    for (i=0;i<traces;i++) {
      calced[i]=FALSE;
      memmove((char*)&xtrace[i][0],
	      (char*)&xtrace[i][1],
	      (bufsize-1)*sizeof(double));
      memmove((char*)&ytrace[i][0],
	      (char*)&ytrace[i][1],
	      (bufsize-1)*sizeof(double));
    }
  }

  // Set all X Traces
  for (i=0;i<traces;i++) {
    xtrace[i][qn[0]]=xp;
    calced[i]=FALSE;
    ytrace[i][qn[0]]=yp[i];
    qn[i]=qn[0];
  }

  // ... BUT Check only on trace 0,
  // CAUSE in Strip all Data is based on one time trace
  if (xtrace[0][qn[0]]>=getXmax()){
    setXRange(xtrace[0][qn[0]]-xrange,xtrace[0][qn[0]]);
  } else {
    setXRange(xtrace[0][0],xtrace[0][0]+xrange);
  }

  qn[0]++;

  // Hide Cursor, maybe we should use a p_curs..->hide()
  for (i=0;i<2;i++)
    p_curs[i].set(p_curs[i].x(),p_curs[i].y());
}

void AIStrip::setViewRange(double xr,double ymn,double ymx)
{
  xrange=xr;
  setRanges(0,xrange,ymn,ymx);
}

void AIStrip::drawScale( QPainter *p,int direction,int w,int h)
{
  int   y;
  int   maxx=8,maxy=8;
  int    spos,pos;
  double shift;
  int zx1,zx2,zy1,zy2;

  //p->fillRect(0,0,w,h,c_bg);
  p->setPen(c_grid);				

  if (direction & HORIZONTAL) {

    zx1=(int)(zoomx1[zoomed_in]*(double)w/100.0f);
    zx2=(int)((zoomx2[zoomed_in]-zoomx1[zoomed_in])*(double)w/100.0f);

    p->setWindow(zx1,0,zx2,h);

    shift=(double)((int)(xmin / w_of_x))*w_of_x;
    while(1) {

      spos=(int)(((double)shift-(double)xmin)/(double)(xmax-xmin)*(double)w);

      if (spos>=zx1 && spos<=zx1+zx2) {
	/*
	  if (!((int)shift % (int) (5*w_of_x))) {
	  p->drawLine(spos,(int)0,
	  spos,(int)h);		
	  } else {
	  */
	p->drawLine(spos,(int)0,
		    spos,(int)h/2);		
      }

      shift+=w_of_x;
      if (shift>=xmax)
	break;
    }
    p->drawLine(zx1,(int)0,zx1,(int)h);		
    p->drawLine(zx1+zx2,(int)0,zx1+zx2,(int)h);  	
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

void AIStrip::drawLabel( QPainter *p,int direction,int w,int rw,int h,int rh)
{
  int   y;
  int   maxx=8,maxy=8;
  int   tpos;
  QString str="";
  QFont font("Helvetica",10);
  QFontMetrics fm(font);
  double rzb,rze;
  double lw,lh;
  static double lzx1,lzy1,lzx2,lzy2;
  time_t tt=0;
  struct tm *ts = NULL;

  lzx1=zoomx1[zoomed_in],lzy1=zoomy1[zoomed_in];
  lzx2=zoomx2[zoomed_in],lzy2=zoomy2[zoomed_in];

  //p->fillRect(0,0,w,h,c_bg);
  p->setPen(c_grid);				

  // Calculate begin and end of zoom area to the real world,
  // means orig xmin,xmax
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

  // set original width,height e.g.:
  // zoom(lzx1,lzx2,lzy1,lzy2) =0,0% to 50,50%,
  // window width (rw)= 100 pixel
  // => local virtual width,height (lw,hw)= 200 pixel
  lw=rw*100.0f/(lzx2-lzx1);
  lh=rh*100.0f/(lzy2-lzy1);

  if (direction & HORIZONTAL) {

    //shift=(double)((int)(xmin / w_of_x))*w_of_x;

#ifdef MOVE_X
    while(1) {
      spos=(int)(((double)shift-(double)xmin)/(double)(xmax-xmin)*(double)lw);
      if (spos>=0) {
	str.sprintf("%.2f\0",
		(double)(xmin+(xmax-xmin)*(double)spos/(double)lw));
	
	tpos=(int)(spos-lzx1*lw/100.0f-fm.width(str)/2+(w-rw)/2);

	if (!(tpos<0 || tpos+fm.width(str)>w)) {
	  p->drawText(tpos,0+fm.height(),str);
	}
      }

      shift+=w_of_x;
      if (shift>=xmax)
	break;
    }
#else
    if ((x_axis_display_type & X_AXIS_NUMBER)==X_AXIS_NUMBER) {
      str.sprintf("%.2f\0",(double)rzb);
      tpos=(w-rw)/2-fm.width(str)/2;
      p->drawText(tpos,0+fm.height(),str);

      str.sprintf("%.2f\0",(double)rze);
      tpos=w-(w-rw)/2-fm.width(str)/2;
      p->drawText(tpos,0+fm.height(),str);
    } else if ((x_axis_display_type & X_AXIS_DATE)==X_AXIS_DATE) {
      if ((x_axis_display_type & X_AXIS_UK)==X_AXIS_UK) {
      } else {
	
	if (rzb<=0.0)
		return;

	tt=(time_t)rzb;
	ts = localtime(&tt);
	if (ts==NULL)
		return;

	str.sprintf("%02d:%02d:%02d\0",
		ts->tm_hour,ts->tm_min,ts->tm_sec);
	tpos=(w-rw)/2-fm.width(str)/2;
	p->drawText(tpos,0+fm.height(),str);
	int year=0;
	if (ts->tm_year<80) year=2000+ts->tm_year;
	else year=1900+ts->tm_year;
	str.sprintf("%02d.%02d.%04d\0",
		ts->tm_mday,ts->tm_mon+1,year);
	p->drawText(tpos,0+2*fm.height(),str);
	
	tt=(time_t)rze;
	ts = localtime(&tt);
	str.sprintf("%02d:%02d:%02d\0",
		ts->tm_hour,ts->tm_min,ts->tm_sec);
	tpos=w-(w-rw)/2-fm.width(str)/2;
	p->drawText(tpos,0+fm.height(),str);
      }
    }
#endif
  }
 else if (direction & VERTICAL) {

    for (y=0;y<=maxy;y++) {

      str.sprintf("%.2f\0",(double)(ymax+(ymin-ymax)*(double)y/(double)maxy));
      tpos=(int)(y*lh/maxy-lzy1*lh/100.0f+fm.height()/2+(h-rh)/2);
      if (!(tpos-fm.height()<0 || tpos+fm.height()>h))
	p->drawText(w-fm.width(str),tpos,str);
    }
 }
}

void AIStrip::drawGrid( QPainter *p )
{
  int y;
  int maxx=8,maxy=8;
  int w=width()*20,h=height()*20;
  int    spos;
  double shift;

    int    n;
  static int     bw=-1,bh=-1; // BG Buffer
  static QPixmap *bg_buf=NULL;
  QPainter bg_p;
  QPoint   qp(0,0);
  QColor   c;

  w=width();
  h=height();

#define SHADE_BG
#ifdef SHADE_BG
#else
  // 1999-05-23 UL, disabled, enabled
  //p->fillRect(0,0,w,h,c_bg);
#endif

  QPen pen(c_grid,1,DotLine);
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

  shift=(double)((int)(xmin / w_of_x))*w_of_x;
  while(1) {

    spos=(int)(((double)shift-(double)xmin)/(double)(xmax-xmin)*(double)w);

    p->drawLine((int)spos,(int)0,
		(int)spos,(int)h);

    shift+=w_of_x;
    if (shift>=xmax)
      break;
  }

  for (y=1;y<maxy;y++) {
    p->drawLine((int)0,(int)(y*h/maxy),
		(int)w,(int)(y*h/maxy));		
  }

  p->drawRect(0,0,w,h);
}

void AIStrip::setGridUnit(double wox)
{
  w_of_x=wox; // Set the width between 2 X Axis Scales
}

#include "qtai_strip.moc"
