/*
	Written 1998-1999 by Uwe Langenkamp

	This software may be used and distributed according to the terms
	of the GNU Public License, incorporated herein by reference.

	The author may be reached as ul@it-guru.de
*/

/* History:
 *          99/10/28 Last Change
 *          99/10/28 Shaded Background color added
 *			99/10/26 Changes will only be made to free qtai!!!
 *          98/17/03
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "qtai_meter.h"

#define FACTOR 1

AIMeter::AIMeter(QWidget *parent,const char *name,int n)
  : AIBar(parent,name,n)
{
  int i;

  for (i=0;i<8;i++) {
    color[i].setRgb( 0, 164, 0 );		
  }

  setType(AI_METER_LEVEL);
}

void AIMeter::drawGrid( QPainter *p )
{
  int w=width()*FACTOR,h=height()*FACTOR;
  QString str="";
  QWMatrix matrix;
  QWMatrix zmatrix;
  QFont font("Helvetica",10);
  QFontMetrics fm(font);
  double   azero;
  int      i;

  p->setFont(font);
  p->setPen(c_grid);
  p->setWindow(0,0,w,h);	        // defines coordinate system

  azero=(0-min)*(amax-amin)/(max-min)+amin;
  zmatrix.translate( zx, zy );	// move to center
  zmatrix.rotate(270+azero);
  p->setWorldMatrix( zmatrix );		// use this world matrix

  str.sprintf("%.2f",0.0);
  p->drawText(-fm.width(str)/2,-rs-10,str);

  matrix.translate( zx, zy );	// move to center
  matrix.rotate(270+amin);

  for (i=0;i<=scale_tile;i++) {

    p->setWorldMatrix( matrix );	// use this world matrix
    p->drawLine(0,-r,0,-rs);

    if (i==0 || i==scale_tile) {
      str.sprintf("%.2f",i*(max-min)/scale_tile+min);
      p->drawText(-fm.width(str)/2,-rs-10,str);
    }

    matrix.rotate((amax-amin)/scale_tile);
  }
}

void AIMeter::drawIt( QPainter *p )
{
  QPoint   qp(0,0);
  QWMatrix matrix,pmatrix;
  int      w=width()*FACTOR;
  int      h=height()*FACTOR;
  QFont    font("Helvetica",10);
  QFontMetrics fm(font);
  int      i;
  double   aact;
  QString str="";

  zx=(int)(fzx*w);
  zy=(int)(fzy*h);
  r=(int)(fr*h);
  rs=(int)(fr*h+10);

  QPainter tmp; // for double buffering
  QPixmap  pix(width(),height());

  tmp.begin( &pix );

  tmp.setWindow(0,0,w,h);	// defines coordinate system
int    n;

  static int     bw=-1,bh=-1; // BG Buffer
  static QPixmap *bg_buf=NULL;
  QPainter bg_p;
  QColor   c;

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

  tmp.drawPixmap(qp,*bg_buf);
#else
  // 1999-05-23 UL, disabled, enabled
  tmp.fillRect(0,0,w,h,c_bg);
#endif
  //tmp.fillRect(0,0,w,h,c_bg);

  tmp.setPen(c_grid);

  drawGrid(&tmp);

  pmatrix.translate(0,0);	                // move to center
  tmp.setWorldMatrix( pmatrix );		// use this world matrix

  tmp.setFont(font);
  tmp.setPen(c_comm);

  tmp.drawText(w-(fm.width(range_s)+10),fm.height(),range_s);

  for (i=0;i<nbars;i++) {
    tmp.setWorldMatrix( pmatrix );		// use this world matrix

    tmp.setPen(color[i]);
    str.sprintf("%.2f",actual[i]);
    tmp.drawText(10,(i+1)*fm.height(),str);

    aact=(actual[i]-min)*(amax-amin)/(max-min)+amin;
    matrix.translate( zx, zy );	                // move to center
    matrix.rotate(270+aact);
    tmp.setWorldMatrix( matrix );		// use this world matrix

    // draw data scale
    tmp.drawLine(-2,0,0,-r);
    //tmp.drawLine(0,0,0,-r);
    tmp.drawLine(2,0,0,-r);
  }

  p->drawPixmap(qp,pix);

  tmp.end();
}

void AIMeter::setType(int type)
{
  switch(type) {
  case AI_METER_LEVEL:
    // Type Level Meter
    amin=60;
    amax=120;
    fzx=0.5f,fzy=1.3f,fr=0.8f;
    break;
  case AI_METER_TACHO:
    // Type Tachometer
    amin=-40;
    amax=220;
    fzx=0.5f,fzy=0.5f,fr=0.3f;
    break;
  }
}
#include "qtai_meter.moc"
