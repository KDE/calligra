/*
	Written 1998-1999 by Uwe Langenkamp

	This software may be used and distributed according to the terms
	of the GNU Public License, incorporated herein by reference.

	The author may be reached as ul@it-guru.de
*/

#ifndef AIMeter_H
#define AIMeter_H

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

#include "qtai_bar.h"

#define AI_METER_LEVEL 0
#define AI_METER_TACHO 1

/*******************************************************************/
/******* Begin Draw Window Definition  *****************************/
/*******************************************************************/

class AIMeter : public AIBar
{
  Q_OBJECT
public:
  AIMeter(QWidget *parent,const char *name,int n=1);
  void setType(int type);

protected:
  void   drawIt( QPainter * );
  void   drawGrid( QPainter *p );

private:
  int      zx,zy,r,rs;
  double   fzx,fzy,fr;
  double   amin,amax;
};

/*******************************************************************/
/********* End Draw Window Definition  *****************************/
/*******************************************************************/

#endif // #ifdef PBar_H





