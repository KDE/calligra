/*
	Written 1998-1999 by Uwe Langenkamp

	This software may be used and distributed according to the terms
	of the GNU Public License, incorporated herein by reference.

	The author may be reached as ul@it-guru.de
*/

/* History:
 *          1999-10-29 Last Change
 *          1999-10-28 Background shading effect added
 *          1998-16-07 Auto Resize to Polar added
 *          1998-29-03
 *          1998-28-03 Multiple Cursors Added
 *          1998-21-03 Add Custom Cursor Bitmap
 *
 */

#ifndef AIPlot_H
#define AIPlot_H

#include <qwidget.h>
#include <qcolor.h>
#include <qpopmenu.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <koprinter.h>
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


// Convert these to enums

#define AI_PLOT_POLAR 0
#define AI_PLOT_TREND 1

#define AI_PLOT_LINE  0
#define AI_PLOT_BAR   1
#define AI_PLOT_CROSS 2

#define VERTICAL   0x01
#define HORIZONTAL 0x02

#define AI_CTRL_ZOOM  0
#define AI_CTRL_PAN   1
#define AI_CTRL_CURS  2

#define X_AXIS_NUMBER 0x0000001
#define X_AXIS_DATE   0x0000002
#define X_AXIS_GERMAN 0x0000004
#define X_AXIS_UK     0x0000008

class AICursor
{
public:
  AICursor(double x=0.0,double y=0.0);
  double xpos;   // x - Position in %
  double ypos;   // y - Position in %
  int    t;      // Cursor Trace
  int    t_index;// Position on Trace

  void   set(double x,double y,int trace=-1,int index=-1);
  double x(void);
  double y(void);
  int    trace(void);
  int    index(void);
};

class AIPlot : public AIBase
{
    Q_OBJECT
public:
    // Main Methods
    AIPlot(QWidget *parent,const char *name=0,int b=512,int t=1);
    ~AIPlot();
    void   init();

    // Set Methods
    void   setData(int trace,int qn,double *xp,double *yp);
    void   setRanges(double xmn,double xmx,double ymn,double ymx);
    void   setXRange(double xmn,double xmx);
    void   setYRange(double ymn,double ymx);
    void   setPlotColor(int bar,int r,int g,int b);
    void   setPlotColor(int bar,QColor c);
    void   setPlotType(int type);
    void   setGraphType(int type);
    void   setEnabled(int graph,bool on);
    void   setDoubleBuffer(bool on);
    void   setXName(QString xName);
    void   setYName(QString yName);
    void   setXUnit(QString xUnit);
    void   setYUnit(QString yUnit);
    void   resetCalced();

    virtual void setXAxisDisplay(int type);

    // Get Settings
    double getXmin();
    double getYmin();
    double getXmax();
    double getYmax();
    int    getPlotType(void);
    int    getGraphType(void);
    bool   isEnabled(int graph);

    double       *xtrace[8];
    double       *ytrace[8];
    int          bufsize;
    int          traces;
    int          qn[8];
    double       xmin,xmax;
    double       ymin,ymax;

    // Zoom Stuff
    double       azx1,azy1,azx2,azy2; // actual zoom factor,  for zoom rect
    double       zoomx1[8],zoomy1[8],zoomx2[8],zoomy2[8]; // Zoom Factor in %
    int          zoomed_in;
    bool         zooming;

    // Cursors
    AICursor     p_curs[8];
    QColor       c_curs[8];

private slots:
    void         setZoomMode();
    void         setZoomOut();
    void         setPanMode();
    void         setCursMode();

protected:
    virtual void   drawIt( QPainter *p);
    virtual void   mousePressEvent( QMouseEvent *e );
    virtual void   mouseReleaseEvent( QMouseEvent *e );
    virtual void   mouseMoveEvent( QMouseEvent *e );
    virtual QSizePolicy sizePolicy() const;
    
    virtual void   drawGrid( QPainter *p );
    virtual void   drawScale( QPainter *p,int direction,int w,int h);
    virtual void   drawLabel( QPainter *p,int direction,int w,int rw,int h,int rh);

    bool           calced[8];
    int            x_axis_display_type;
	
private:
    void         paintEvent( QPaintEvent *p );
	void         resizeEvent(QResizeEvent *e);
	
    QPixmap      *bg_buf;
    QPointArray  *qa[8];        // Main Array
    KPrinter     *printer;      // Printer
    QPopupMenu   *adhoc;        // Main Popup
    QColor       color[8];
    int			 pcnt[8];
    QCursor      *cs_standard;
    QCursor      *cs_pan;
    bool         trace_enabled[8];
    AICursor     calcTraceCoords(double x,double y);

    double       rzoomx1,rzoomy1,rzoomx2,rzoomy2; // Zoom Factor in %
    double       panx1,pany1,panx2,pany2;
    bool         panning;
    bool         dbuffer;

    int          plottype;
    int          graphtype;
    int          ctrlmode;
    int		 used_curs;

    QString		 xName;
    QString		 yName;
    QString		 xUnit;
    QString		 yUnit;

    bool                 reshade;
};

/*******************************************************************/
/********* End Draw Window Definition  *****************************/
/*******************************************************************/

#endif // #ifdef AIPLot_H





