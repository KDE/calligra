/*
	Written 1998-1999 by Uwe Langenkamp

	This software may be used and distributed according to the terms
	of the GNU Public License, incorporated herein by reference.

	The author may be reached as ul@it-guru.de
*/

/* History:
 *          98/13/07 Last Change
 *          98/29/03
 *          98/28/03 Multiple Cursors Added
 *          98/21/03 Add Custom Cursor Bitmap
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

#include "qtai_base.h"

AIBase::AIBase(QWidget *parent,const char *name)
  : QWidget( parent, name)
{
  // Standard Color Set, MMS Style
  setBackgroundColor(0,0,128);
  //c_bg.setRgb(0,0,0);
  setCommonColor(255,255,0);
  //c_grid.setRgb(127,127,127);
  setGridColor(255,255,255);
  autoscale=TRUE;
  invert=FALSE;
}

AIBase::~AIBase()
{
}

void AIBase::invertColor()
{
  if (invert) {
    invert=FALSE;
  } else {
    invert=TRUE;
  }
  c_bg.setRgb(255-c_bg.red(),255-c_bg.green(),255-c_bg.blue());
  c_grid.setRgb(255-c_grid.red(),255-c_grid.green(),255-c_grid.blue());
  c_comm.setRgb(255-c_comm.red(),255-c_comm.green(),255-c_comm.blue());
}

void AIBase::setBackgroundColor(QColor c)
{
  setBackgroundColor(c.red(), c.green(), c.blue());
}

void AIBase::setBackgroundColor(int r, int g, int b)
{
  c_bg.setRgb(r,g,b);
}

QColor AIBase::getBackgroundColor()
{
  return c_bg;
}

void AIBase::setGridColor(QColor c)
{
  setGridColor(c.red(), c.green(), c.blue());
}

void AIBase::setGridColor(int r, int g, int b)
{
  c_grid.setRgb(r,g,b);
}

void AIBase::setCommonColor(QColor c)
{
  setCommonColor(c.red(), c.green(), c.blue());
}

void AIBase::setCommonColor(int r, int g, int b)
{
  c_comm.setRgb(r,g,b);
}

void AIBase::setAutoScale(bool s)
{
	autoscale=s;
}

void   AIBase::drawIt( QPainter *p)
{
}

void   AIBase::printOut(QPainter *p)
{
  fprintf(stderr,"AIBASE.CPP: Print Out!");
  invertColor();
  drawIt(p);
  invertColor();
}
	

