/*
	Written 1998-1999 by Uwe Langenkamp

	This software may be used and distributed according to the terms
	of the GNU Public License, incorporated herein by reference.

	The author may be reached as ul@it-guru.de
*/

#ifndef AIStrip_H
#define AIStrip_H

#include <qwidget.h>
#include <qcolor.h>
#include <qpopmenu.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qbttngrp.h>
#include <qtooltip.h>

#include <stdio.h>
#include <stdlib.h>

#include "qtai_plot.h"

#define LEVEL 0
#define TACHO 1

/*******************************************************************/
/******* Begin Draw Window Definition  *****************************/
/*******************************************************************/

class AIStrip : public AIPlot
{
  Q_OBJECT
public:
  AIStrip(QWidget *parent,const char *name,int b=1,int t=1);
  void   addData(double xp,double *yp);
  void   setViewRange(double xr,double ymn,double ymx);
  void   setGridUnit(double wox); // Width between of X Axis Scales
  double xrange;

protected:
  void drawGrid( QPainter *p );
  void drawScale( QPainter *p,int direction,int w,int h);
  void drawLabel( QPainter *p,int direction,int w,int rw,int h,int rh);

private:
  double w_of_x;
};

/*******************************************************************/
/********* End Draw Window Definition  *****************************/
/*******************************************************************/

#endif // #ifdef AIStrip_H





