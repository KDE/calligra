/*
	Written 1998-1999 by Uwe Langenkamp

	This software may be used and distributed according to the terms
	of the GNU Public License, incorporated herein by reference.

	The author may be reached as ul@it-guru.de
*/

/* History:
 *          1998-03-17 Last Change
 *
 */

#include <qpainter.h>
#include <qpixmap.h>
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
#include <kdebug.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "qtai_bar.h"

#define FACTOR 1
#define _BAR_MARGIN 25

AIBar::AIBar(QWidget *parent,const char *name,int n)
  : AIBase(parent,name)
{
  int i;

  printer = new KPrinter;
  brush = new QBrush(SolidPattern);

  nbars=n;

  if ((actual=(double*)malloc(nbars*sizeof(double)))==NULL) {
    actual=NULL;
    nbars=0;
    kdDebug(36001) << "Can't alloc memory for actual!" << endl;
    exit(-1);
  }

  for (i=0;i<n;i++) {
    actual[i] = 0.0;
  }

  for (i=0;i<8;i++) {
    color[i].setRgb( 0, 164, 0 );		
  }

  range_s="";

  c_limits[D_LL].setRgb(255,0,0);
  c_limits[D_HH].setRgb(255,0,0);
  c_limits[D_L].setRgb(255,255,0);
  c_limits[D_H].setRgb(255,255,0);

  scale_tile=8;
  autorepaint=FALSE;
  showscale=TRUE;
  alarm_bg=FALSE;
  calc_limits=FALSE;
}

void AIBar::drawGrid( QPainter *p )
{
  int i;
  int w=width()*FACTOR,h=height()*FACTOR;
  int      bl,br;
  int      bzero=0,bact=0,bmax=1,bmin=0;
  int      bstart,bwidth;
  QString  str="";
  QFont font("Helvetica",9);
  QFontMetrics fm(font);

  p->setFont(font);

  //bl=(int)((double)w/2.0+1);
  //br=(int)((double)2*w/3.0+1);

  QString sm1="";
  QString sm2="";

  sm1.sprintf("%.2f\0",(double)min);
  sm2.sprintf("%.2f\0",(double)max);

  //bl=(int)((double)w/2.0+1);
  //br=(int)((double)2*w/3.0+1);

  if (fm.width(sm1)>fm.width(sm2)) {
    bl=(fm.width(sm1))*FACTOR;
  } else {
    bl=(fm.width(sm2))*FACTOR;
  }
  br=w-5;
  if (br-bl>50) br=bl+50;

  bmax=(int)_BAR_MARGIN;
  bmin=(int)h-_BAR_MARGIN;
  bzero=(int)(bmin-min*(bmin-bmax)/(min-max));
  bstart=bzero,bwidth=bact-bzero;

  p->setWindow(0,0,w,h);	// defines coordinate system

  p->setPen(c_grid);				

  p->drawLine(bl-1,bmax,bl-1,bmin);
  p->drawLine(bl-5,bzero,br+10,bzero);

  for (i=0;i<=scale_tile;i++) {
    p->drawLine(bl-5,bmin-i*(bmin-bmax)/scale_tile,bl,
		bmin-i*(bmin-bmax)/scale_tile);
    str.sprintf("%.0f\0",(double)min-i*(min-max)/scale_tile);		
    p->drawText(bl-8-fm.width(str),bmin-i*(bmin-bmax)/scale_tile+fm.height()/2,str);
  }
}

void AIBar::drawIt( QPainter *p )
{
     QPoint   qp(0,0);
     QColor   c;
     QPointArray qa(4);
     int      w=width()*FACTOR;
     int      h=height()*FACTOR;
     QFont    font("Helvetica",9);
     QFontMetrics fm(font);
     QString str="";
     int      i,j;
     int      bl,br;
     int      bzero,bact,bmax,bmin;
     int      bstart,bwidth;

     QString sm1="";
     QString sm2="";

     if (showscale) {
       sm1.sprintf("%.2f\0",(double)min);
       sm2.sprintf("%.2f\0",(double)max);

       //bl=(int)((double)w/2.0+1);
       //br=(int)((double)2*w/3.0+1);
       if (fm.width(sm1)>fm.width(sm2)) {
	 bl=(fm.width(sm1))*FACTOR;
       } else {
	 bl=(fm.width(sm2))*FACTOR;
       }
       br=w-5;
       if (br-bl>50) br=bl+50;
     } else {
       bl=0;br=w;
     }

     bmax=_BAR_MARGIN; //(int)20;
     bmin=(int)h-_BAR_MARGIN; //-20;
     bstart=bzero=(int)(bmin-min*(bmin-bmax)/(min-max));
	
     p->setViewport(0,0,width(),height());	// defines coordinate system
     p->setWindow(0,0,w,h);	// defines coordinate system


	 int    n;

  static int     bw=-1,bh=-1; // BG Buffer
  static QPixmap *bg_buf=NULL;
  QPainter bg_p;


#define SHADE_BG
#ifdef SHADE_BG
  if (bh!=h || bw!=w || bg_buf==NULL) {
    delete bg_buf;
    bg_buf=new QPixmap(w,h);

    bg_p.begin( bg_buf );			// begin painting

    float step=100.0/(double)h;
    float colval=150.0;

    // Must be buffered !!!
    for (n=0;n<h;n++) {
      c.setRgb(64,64,(int)colval);
      colval+=step;
      bg_p.setPen(c);
      bg_p.drawLine(0,n,w,n);
    }

    bg_p.end();				// painting done

  }

  p->drawPixmap(qp,*bg_buf);
#else
  // 1999-05-23 UL, disabled, enabled
  p->fillRect(0,0,w,h,c_bg);
#endif
  //p->fillRect(0,0,w,h,c_bg);

     p->setFont(font);
     p->setPen( c_comm );
     p->drawText(w-(fm.width(range_s)+10),fm.height(),range_s);

     if (!alarm_bg) {
       for (i=0;i<nbars;i++) {

			c=color[i];
			p->setPen(c);
			str.sprintf("%.1f",actual[i]);
			p->drawText(bl-8-fm.width(str),height()-5,str);
			if (calc_limits) {
				for (j=0;j<2;j++) {
					if ((b_limits[j]==TRUE) && (actual[i]<d_limits[j]))
						c=c_limits[j];
				}
	
				for (j=2;j<4;j++) {
					if ((b_limits[j]==TRUE) && (actual[i]>d_limits[j]))
						c=c_limits[j];	
				}
			}

		bact=(int)((actual[i]-min)*(bmin-bmax)/(min-max)+bmin);
		bwidth=(bact-bzero);
	
		QBrush brush;
	
		brush.setStyle(DiagCrossPattern);
		brush.setColor(color[i]);
	
		p->setPen(c);
		p->fillRect(bl+i*(br-bl)/nbars+1,
		     bstart,
		     (br-bl)/nbars-1,
		     bwidth,c);
		/*
		p->fillRect(bl+i*(br-bl)/nbars+1,
		     bstart,
		     (br-bl)/nbars-1,
		     bwidth,
		     brush);

		p->setPen(color[i]);
		p->drawRect(bl+i*(br-bl)/nbars+1,
		     bstart,
		     (br-bl)/nbars-1,
		     bwidth);
	   */
	   }

		//Limit Paint Routine
		for (i=0;i<4;i++) {
			if (b_limits[i]) {
				bact=(int)((d_limits[i]-min)*(bmin-bmax)/(min-max)+bmin);
				qa.setPoint(0,bl-3,bact);
				qa.setPoint(1,bl-3-5*FACTOR,bact-5*FACTOR);
				qa.setPoint(2,bl-3-5*FACTOR,bact+5*FACTOR);
				qa.setPoint(3,bl-3,bact);
	
				p->setPen(c_limits[i]);	
				p->setBrush(c_limits[i]);	
				//p->drawPolyline(qa,0,4);
				p->drawPolygon(qa,TRUE,0,4);
			}
		}
    } else {
       for (i=0;i<nbars;i++) {

		// L area
		if (b_limits[1]) {
			bstart=(int)((d_limits[1]-min)*(bmin-bmax)/(min-max)+bmin);
			bact=(int)((min-min)*(bmin-bmax)/(min-max)+bmin);
			p->fillRect(bl+i*(br-bl)/nbars+1,
		       bstart,
		       bl+(i+1)*(br-bl)/nbars,
		       bact-bstart,
		       c_limits[1]);	
		}

		// LL area
		if (b_limits[0]) {
			bstart=(int)((d_limits[0]-min)*(bmin-bmax)/(min-max)+bmin);
			bact=(int)((min-min)*(bmin-bmax)/(min-max)+bmin);
			p->fillRect(bl+i*(br-bl)/nbars+1,
		       bstart,
		       bl+(i+1)*(br-bl)/nbars,
		       bact-bstart,
		       c_limits[0]);	
		}

		// H area
		if (b_limits[2]) {
			bstart=(int)((d_limits[2]-min)*(bmin-bmax)/(min-max)+bmin);
			bact=(int)((max-min)*(bmin-bmax)/(min-max)+bmin);
			p->fillRect(bl+i*(br-bl)/nbars+1,
		       bstart,
		       bl+(i+1)*(br-bl)/nbars,
		       bact-bstart,
		       c_limits[2]);	
		}

		// LL area
		if (b_limits[3]) {
			bstart=(int)((d_limits[3]-min)*(bmin-bmax)/(min-max)+bmin);
			bact=(int)((max-min)*(bmin-bmax)/(min-max)+bmin);
			p->fillRect(bl+i*(br-bl)/nbars+1,
			   bstart,
		       bl+(i+1)*(br-bl)/nbars,
		       bact-bstart,
		       c_limits[3]);	
		}

		bact=(int)((actual[i]-min)*(bmin-bmax)/(min-max)+bmin);
		bwidth=(bact-bzero);
	
		p->setPen(c_grid);
		p->drawLine(bl+i*(br-bl)/nbars+1,
		     bzero+bwidth,
		     bl+(i+1)*(br-bl)/nbars,
		     bzero+bwidth);
		
       }
     }

     if (showscale) {
       drawGrid(p);
     } else {
       p->setPen(c_grid);
       p->drawLine(bl,bzero,br,bzero);
     }
}

//
// Called when the print button is clicked.
//

void AIBar::printIt()
{
    if ( printer->setup(this) ) {
	QPainter paint;
	paint.begin( printer );	
	drawIt(&paint);
	paint.end();
    }
}

//
// Called when the widget needs to be updated.
//

void AIBar::paintEvent( QPaintEvent * )
{
  QPoint   qp(0,0);
  QPainter paint;
  QPainter tmp; // for double buffering
  QPixmap  pix(width(),height());

  //QPixmap  pix(width(),height());

  //QPixmap  *pix;

  paint.begin(this);

  // drawIt( &paint );		        // without double buffer
  // pix=new QPixmap(width(),height());

  tmp.begin( &pix );			// begin painting
  tmp.setWindow(0,0,width(),height());
  drawIt( &tmp );				
  tmp.end();				// painting done

  paint.drawPixmap(qp,pix);

  paint.end();

  // delete pix;
}

void AIBar::drawContents(QPainter *p)
{
  drawIt(p);
}

void AIBar::setData(int bar,double value)
{
  actual[bar]=value;
  if (autorepaint)
    repaint(FALSE);
}

void AIBar::setAutoRepaint(bool ar)
{
  autorepaint=ar;
}

void AIBar::showScale(bool sscale)
{
  showscale=sscale;
}

void AIBar::setRanges(double mn,double mx)
{
  min=mn;
  max=mx;
}

void AIBar::setPlotColor(int bar,int r,int g,int b)
{
  color[bar].setRgb(r,g,b);
}

void AIBar::setPlotColor(int bar,QColor c)
{
  setPlotColor(bar,c.red(),c.green(),c.blue());
}

void AIBar::setRangeType(QString rs)
{
  range_s=rs;
}

void AIBar::addLimits(double ll,double l,double h,double hh)
{
  d_limits[D_LL]=ll;
  d_limits[D_L]=l;
  d_limits[D_H]=h;
  d_limits[D_HH]=hh;
}

void AIBar::enableLimits(bool ll,bool l,bool h,bool hh)
{
  b_limits[D_LL]=ll;
  b_limits[D_L]=l;
  b_limits[D_H]=h;
  b_limits[D_HH]=hh;
}

void AIBar::addLimitLL(double ll,bool on)
{
  d_limits[D_LL]=ll;
  b_limits[D_LL]=on;
}

void AIBar::addLimitL(double l,bool on)
{
  d_limits[D_L]=l;
  b_limits[D_L]=on;
}

void AIBar::addLimitH(double h,bool on)
{
  d_limits[D_H]=h;
  b_limits[D_H]=on;
}

void AIBar::addLimitHH(double hh,bool on)
{
  d_limits[D_HH]=hh;
  b_limits[D_HH]=on;
}

void AIBar::setScaleTile(int scale_tile)
{
  this->scale_tile=scale_tile;
}
void AIBar::setAlarmBG(bool alarm_bg)
{
  this->alarm_bg=alarm_bg;
}


void AIBar::setCalcLimits(bool calc_limits)
{
  this->calc_limits=calc_limits;
}
#include "qtai_bar.moc"
