/*
	Written 1998-1999 by Uwe Langenkamp

	This software may be used and distributed according to the terms
	of the GNU Public License, incorporated herein by reference.

	The author may be reached as ul@it-guru.de
*/

#ifndef AIBase_H
#define AIBase_H

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

/*******************************************************************/
/******* Begin Draw Window Definition  *****************************/
/*******************************************************************/

class AIBase : public QWidget
{
    Q_OBJECT
public:
    // Main Methods
    AIBase(QWidget *parent,const char *name=0);
    ~AIBase();

    void setBackgroundColor(QColor c);
    void setBackgroundColor(int r, int g, int b);
    void setGridColor(QColor c);
    void setGridColor(int r, int g, int b);
    void setCommonColor(QColor c);
    void setCommonColor(int r, int g, int b);

    QColor getBackgroundColor();

    void setAutoScale(bool s);

    static void setDesktopPal(QApplication *a);
	
    bool   autoscale;

    virtual void printOut(QPainter *p);


protected:
    virtual void   drawIt( QPainter *p);
    virtual void   invertColor();

    QColor       c_bg,c_grid,c_comm;

private:	
    bool invert;
};

/*******************************************************************/
/********* End Draw Window Definition  *****************************/
/*******************************************************************/

#endif // #ifdef AIBase_H





