/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
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
/* Module: Background Dialog (header)                             */
/******************************************************************/

#ifndef BACKDIA_H
#define BACKDIA_H

#include <stdlib.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qgrpbox.h>
#include <qframe.h>
#include <qradiobt.h>
#include <qpushbt.h>
#include <qcolor.h>
#include <qbttngrp.h>
#include <qpixmap.h>
#include <qwmatrix.h>
#include <qstring.h>
#include <qapp.h>
#include <qwidget.h>
#include <qevent.h>
#include <qpicture.h>
#include <qpainter.h>

#include <kcolordlg.h>

#include "qwmf.h"
#include "global.h"

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

/******************************************************************/
/* class ClipPreview                                              */
/******************************************************************/

class ClipPreview : public QWidget
{
  Q_OBJECT

public:

  // constructor - destructor
  ClipPreview(QWidget* parent=0,const char* name=0);
  ~ClipPreview();

  // get - set clipart
  void setClipart(const char*);
  const char* getClipart() {return (const char*)fileName;}

protected:

  // paint
  void paintEvent(QPaintEvent*);

private:

  // internal
  QPicture *pic;
  QString fileName;
  QWinMetaFile wmf;

};

/******************************************************************/
/* class BackDia                                                  */
/******************************************************************/

class BackDia : public QDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  BackDia(QWidget* parent=0,const char* name=0,    
	  BackType backType=BT_COLOR,QColor backColor=white,
	  const char *backPic=0,const char *backClip=0,
	  BackView backPicView=BV_TILED);
  ~BackDia();                                      
  
  // get values
  QColor getBackColor() {return chosenColor;}     
  BackType getBackType();
  const char *getBackPic() {return chosenPic;}
  const char *getBackClip() {return chosenClip;}
  BackView getBPicView();

private:

  // dialog objects
  QLabel *lPicName,*picPreview,*lClipName;        
  QFrame *colorShow;
  QGroupBox *grp1,*grp2,*grp3;
  QRadioButton *radioColor,*radioPic,*vTiled,*vCenter,*vZoom,*radioClip;
  QPushButton *colorChoose,*okBut,*applyBut,*applyGlobalBut,*cancelBut;
  QPushButton *picChoose,*clipChoose;
  QButtonGroup *buttGrp,*buttGrp2,*buttGrp3;

  // values
  QColor chosenColor;           
  char *chosenPic;
  char *chosenClip;
  ClipPreview *clipPreview;

private slots:

  // dialog slots
  void selectColor();
  void selectPic(); 
  void selectClip();
  void openPic(const char*);
  void openClip(const char*);
  void Ok() {emit backOk(false);} 
  void Apply() {emit backOk(false);} 
  void ApplyGlobal() {emit backOk(true);} 

signals:

  // ok
  void backOk(bool);

};
#endif //BACKDIA_H
