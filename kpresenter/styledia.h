/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Pen and Brush style Dialog (header)                    */
/******************************************************************/

#ifndef STYLEDIA_H
#define STYLEDIA_H

#include <stdlib.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qframe.h>
#include <qbttngrp.h>
#include <qcombo.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qsize.h>

#include <kcolordlg.h>

#include "global.h"
#include "kpresenter_utils.h"

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

/******************************************************************/
/* class Pen and Brush preview                                    */
/******************************************************************/

class PBPreview : public QWidget
{
  Q_OBJECT

public:

  // constructor
  PBPreview(QWidget* parent=0,const char* name=0,int _paintType=0);
  
  // set values
  void setPen(QPen _pen) {pen = _pen; repaint(true);}    
  void setBrush(QBrush _brush) {brush = _brush; repaint(true);}
  void setLineBegin(LineEnd lb) {lineBegin = lb; repaint(true);}
  void setLineEnd(LineEnd le) {lineEnd = le; repaint(true);}

protected:

  // paint event
  void paintEvent(QPaintEvent*);

private:

  int paintType;
  QPen pen;
  QBrush brush;
  LineEnd lineBegin,lineEnd;

};

/******************************************************************/
/* class StyleDia                                                 */
/******************************************************************/

class StyleDia : public QDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  StyleDia(QWidget* parent=0,const char* name=0);
  ~StyleDia();                

  // set values
  void setPen(QPen _pen);                        
  void setBrush(QBrush _brush);
  void setLineBegin(LineEnd lb);
  void setLineEnd(LineEnd le);

  // get values
  QPen getPen() {return pen;}
  QBrush getBrush() {return brush;}
  LineEnd getLineBegin() {return lineBegin;}
  LineEnd getLineEnd() {return lineEnd;}

private:

  // dialog objects
  QButtonGroup *penFrame,*brushFrame;  
  QPushButton *choosePCol,*chooseBCol;
  QLabel *penStyle,*brushStyle,*penWidth,*llineBegin,*llineEnd;
  QComboBox *choosePStyle,*chooseBStyle,*choosePWidth,*clineBegin,*clineEnd;
  QPushButton *okBut,*applyBut,*cancelBut;
  PBPreview *penPrev,*brushPrev;

  // pen and brush
  QBrush brush;   
  QPen pen;
  LineEnd lineBegin,lineEnd;

private slots:

  // slots
  void changePCol();   
  void changeBCol(); 
  void changePStyle(int item); 
  void changeBStyle(int item);
  void changePWidth(int item); 
  void changeLineBegin(int item);
  void changeLineEnd(int item);
  void styleDone() {emit styleOk();} 

signals:

  void styleOk();

};
#endif //STYLEDIA_H



