/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Autoform Editor                                                */
/* Version: 0.1.0alpha                                            */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: draw widget (header)                                   */
/******************************************************************/

#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <qwidget.h>
#include <qcolor.h>
#include <qevent.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qwmatrix.h>

#include "atfinterpreter.h"

/******************************************************************/
/* Class: DrawWidget                                              */
/******************************************************************/
class DrawWidget : public QWidget
{
  Q_OBJECT

public:

  // constructor - destructor
  DrawWidget(QWidget *parent=0);
  ~DrawWidget();

  // get - set relation
  void setRelation(bool);
  bool getRelation() {return relation;}

  // set pointarray
  void setPointArray(QPointArray pa,QList<ATFInterpreter::AttribList> al) {pntArry = pa; atrLs = al; repaint(true);}

  // get geometry
  int aW() {return aWidth;}
  int aH() {return aHeight;}

  // get - set pen width
  void setPenWidth(unsigned int w) {wid = w; repaint(true);}
  unsigned int getPenWidth() {return wid;}

  // create pixmap
  void createPixmap(const char*);

protected:

  // resize and paint event
  void resizeEvent(QResizeEvent*);
  void paintEvent(QPaintEvent*);

  // ********** variables **********

  // vars for raster
  bool drawRaster;
  int dRastW,dRastH;

  // points
  int xPoints,yPoints;

  // relation
  bool relation;

  // geometry
  int aWidth,aHeight;

  // pointarray - attribute list
  QPointArray pntArry;
  QList<ATFInterpreter::AttribList> atrLs;

  // pen width
  unsigned int wid;

signals:

  // get point array
  void getPntArry(int,int);

};

#endif //DRAWWIDGET_H
    

















