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
/* Module: Background Dialog (header)                             */
/******************************************************************/

#ifndef BACKDIA_H
#define BACKDIA_H

#include <stdlib.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qgrpbox.h>
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
#include <qcombo.h>
#include <qsize.h>

#include <kcolorbtn.h>

#include "qwmf.h"
#include "global.h"

#include "kpgradient.h"

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
  void setClipart(QString);
  QString getClipart() {return fileName;}

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
	  BackType backType=BT_COLOR,QColor backColor1=white,
	  QColor backColor2=white,BCType _bcType=BCT_PLAIN,
	  QString backPic=0,QString backClip=0,
	  BackView backPicView=BV_TILED);
  ~BackDia();                                      
  
  // get values
  QColor getBackColor1() {return color1Choose->color();}     
  QColor getBackColor2() {return color2Choose->color();}     
  BCType getBackColorType() {return bcType;}
  BackType getBackType();
  QString getBackPixFilename() {return chosenPic;}
  QString getBackClipFilename() {return chosenClip;}
  BackView getBackView();

private:

  // dialog objects
  QLabel *lPicName,*picPreview,*lClipName,*colorPreview;        
  QGroupBox *grp1,*grp2,*grp3;
  QRadioButton *radioColor,*radioPic,*vTiled,*vCenter,*vZoom,*radioClip;
  QComboBox *cType;
  QPushButton *okBut,*applyBut,*applyGlobalBut,*cancelBut;
  QPushButton *picChoose,*clipChoose;
  QButtonGroup *buttGrp,*buttGrp2,*buttGrp3;
  KColorButton *color1Choose,*color2Choose;

  // values
  QString chosenPic;
  QString chosenClip;
  ClipPreview *clipPreview;
  BCType bcType;
  
private slots:

  // dialog slots
  void selectCType(int);
  void selectPic(); 
  void selectClip();
  void openPic(const char*);
  void openClip(const char*);
  void colChanged(const QColor&) {selectCType(bcType);}
  void Ok() {emit backOk(false);} 
  void Apply() {emit backOk(false);} 
  void ApplyGlobal() {emit backOk(true);} 

signals:

  // ok
  void backOk(bool);

};
#endif //BACKDIA_H
