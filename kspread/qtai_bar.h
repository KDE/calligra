/*
	Written 1998-1999 by Uwe Langenkamp

	This software may be used and distributed according to the terms
	of the GNU Public License, incorporated herein by reference.

	The author may be reached as ul@it-guru.de
*/

#ifndef AIBar_H
#define AIBar_H

#include <qwidget.h>
#include <qcolor.h>
#include <qpopmenu.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <kprinter.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qbttngrp.h>
#include <qtooltip.h>

#include <stdio.h>
#include <stdlib.h>

#include "qtai_base.h"

/*******************************************************************/
/******* Begin Draw Window Definition  *****************************/
/*******************************************************************/

#define D_LL   0
#define D_L    1
#define D_H    2
#define D_HH   3

class AIBar : public AIBase
{
    Q_OBJECT
public:
    AIBar(QWidget *parent,const char *name,int n=1);
    void   setData(int bar,double value);
    void   setRanges(double mn,double mx);
    void   setRangeType(QString range_type_string);

    void   setScaleTile(int scale_tile);
    void   setAutoRepaint(bool ar);
    void   setPlotColor(int bar,int r,int g,int b);
    void   setPlotColor(int bar,QColor c);
    void   setCalcLimits(bool calc_limits);
    void   setAlarmBG(bool alarm_bg);
      // shows the alarm levels as bar background and uses pointer
      // for min/max and actual value display
    void   showScale(bool sscale);

    void   addLimits(double ll,double l,double h,double hh);
    void   addLimitLL(double ll,bool on);
    void   addLimitL(double l,bool on);
    void   addLimitH(double h,bool on);
    void   addLimitHH(double hh,bool on);

    void   enableLimits(bool ll,bool l,bool h,bool hh);

    QBrush       *brush;
    int          nbars;
    double       min,max;
    double       *actual;
    QColor       color[8];
    QString      range_s;
    double       d_limits[4];
    bool         b_limits[4];
    QColor       c_limits[4];
public slots:
    void   printIt();

signals:

protected:
    virtual void drawIt( QPainter *p);
    virtual void drawGrid( QPainter *p);
    void    drawContents(QPainter *p);
    void    paintEvent( QPaintEvent *p );
    int     scale_tile;
    bool    autorepaint;
    bool    showscale;
    bool    alarm_bg;
    bool    calc_limits;

private:
    KPrinter	 *printer;
};

/*******************************************************************/
/********* End Draw Window Definition  *****************************/
/*******************************************************************/

#endif // #ifdef AIBar_H





